/** @file
  Dxe driver will register a ready to boot event to program SsidSvid.

;******************************************************************************
;* Copyright (c) 2012 - 2018, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <Uefi.h>
#include <IndustryStandard/Pci.h>
#include <Library/IoLib.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/CommonPciLib.h>
#include <Library/PciExpressLib.h>
#include <Library/S3BootScriptLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DxeChipsetSvcLib.h>
#include <Library/PcdLib.h>
#include <Protocol/PciEnumerationComplete.h>
#include <Library/DxeOemSvcKernelLib.h>
#include <Protocol/AmdCpmDisplayFeatureProtocol/AmdCpmDisplayFeatureProtocol.h>
#include <GNbReg.h>

EFI_STATUS
DisableAmdCpmSetGpuSsidSvid (
  );

VOID
EFIAPI
SetSsidSvidCallBack (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  );

VOID
ProgramSsidSvid (
  IN     UINT8     Bus,
  IN     UINT8     Dev,
  IN     UINT8     Func,
  IN     UINT32    SsidSvid
  );

//EFI_STATUS
//ProgramSsidSvidForSpecialDevice(
//  UINT8                                      Bus,
//  UINT8                                      Dev,
//  UINT8                                      Func,
//  UINT16                                     VendorId,
//  UINT16                                     DeviceId,
//  UINT16                                     ClassCode,
//  UINT32                                     SsidSvid
//  );
//
//EFI_STATUS
//ProgramSsidSvidFunction0x10DE (
//  IN     UINT8    Bus,
//  IN     UINT8    Dev,
//  IN     UINT8    Func,
//  IN     UINT32   SsidSvid
//  );
//
//EFI_STATUS
//ProgramSsidSvidFunction0x1002 (
//  IN     UINT8    Bus,
//  IN     UINT8    Dev,
//  IN     UINT8    Func,
//  IN     UINT32   SsidSvid
//  );
//
//OEM_SSID_SVID_TABLE mPlatformSsidTableDefault[] = {
//  0x10DE, DEVICE_ID_DONT_CARE, ProgramSsidSvidFunction0x10DE,
//  0x1002, DEVICE_ID_DONT_CARE, ProgramSsidSvidFunction0x1002,
//  DEVICE_ID_DONT_CARE, DEVICE_ID_DONT_CARE, NULL
//};

/**
  The Entry Point of SetSsidSvidDxe. It register a ready to boot event for programming SsidSvid.

  @param ImageHandle    The firmware allocated handle for the EFI image.
  @param SystemTable    A pointer to the EFI System Table.

  @retval EFI_SUCCESS   The entry point is executed successfully.

**/
EFI_STATUS
SetSsidSvidEntryPoint (
  IN     EFI_HANDLE          ImageHandle,
  IN     EFI_SYSTEM_TABLE    *SystemTable
  )
{
  EFI_EVENT                                  SetSsidSvidEvent;
  VOID                                       *Registration; 

  SetSsidSvidEvent = EfiCreateProtocolNotifyEvent (
                       &gEfiPciEnumerationCompleteProtocolGuid,
                       TPL_NOTIFY,
                       SetSsidSvidCallBack,
                       NULL,
                       &Registration
                       );

  ASSERT (SetSsidSvidEvent != NULL);
    
  return EFI_SUCCESS;
}

VOID
EFIAPI
SetSsidSvidCallBack (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
{
  EFI_STATUS          Status;
  UINT8               Bus;
  UINT8               Dev;
  UINT8               Func;
  UINT32              SsidSvid;
  UINT8               Value8;
  UINT16              VendorId;
  UINT16              DeviceId;
  UINT16              ClassCode;
  UINT8               HeaderType;
  UINT8               BusLimit;
  UINT8               FuncLimit;
  UINT32              DefaultSsidSvid;
  VOID                *ProtocolPointer;

  //
  // Check whether this is real ExitPmAuth notification, or just a SignalEvent
  //
  Status = gBS->LocateProtocol (&gEfiPciEnumerationCompleteProtocolGuid, NULL, (VOID **)&ProtocolPointer);
  if (EFI_ERROR (Status)) {
    return ;
  }

  Status   = EFI_SUCCESS;

  BusLimit = 1;
  SsidSvid = 0xFFFFFFFF;

  for (Bus = 0; Bus <= BusLimit; Bus++) {
    for (Dev = 0; Dev <= 0x1F ; Dev++) {
      ClassCode = PciExpressRead16 (PCI_EXPRESS_LIB_ADDRESS (Bus, Dev, 0, PCI_CLASSCODE_OFFSET + 1));
      if (ClassCode == 0xFFFF) {
        continue;
      }
      
      HeaderType = PciExpressRead8 (PCI_EXPRESS_LIB_ADDRESS (Bus, Dev, 0, PCI_HEADER_TYPE_OFFSET));
      if (HeaderType & HEADER_TYPE_MULTI_FUNCTION) {
        FuncLimit = 7;
      } else {
        FuncLimit = 0;
      }
      
      for (Func = 0; Func <= FuncLimit; Func++) {
        ClassCode = PciExpressRead16 (PCI_EXPRESS_LIB_ADDRESS (Bus, Dev, Func, PCI_CLASSCODE_OFFSET + 1));
        
        if (ClassCode == 0xFFFF) {
          continue;
        } else if (ClassCode == ((PCI_CLASS_BRIDGE << 8) | PCI_CLASS_BRIDGE_P2P)) {
          Value8 = PciExpressRead8 (PCI_EXPRESS_LIB_ADDRESS (Bus, Dev, Func, PCI_BRIDGE_SUBORDINATE_BUS_REGISTER_OFFSET));
          if (Value8 > BusLimit) {
            BusLimit = Value8;
          }
        }
        VendorId = PciExpressRead16 (PCI_EXPRESS_LIB_ADDRESS (Bus, Dev, Func, PCI_VENDOR_ID_OFFSET));
        DeviceId = PciExpressRead16 (PCI_EXPRESS_LIB_ADDRESS (Bus, Dev, Func, PCI_DEVICE_ID_OFFSET));
        DefaultSsidSvid = PciExpressRead32 (PCI_EXPRESS_LIB_ADDRESS (Bus, Dev, Func, PCI_SVID_OFFSET));

        SsidSvid = (UINT32)PcdGet32(PcdDefaultSsidSvid);

        if (SsidSvid == 0) {
          SsidSvid = ((UINT32)DeviceId << 16) + VendorId;
        }

        //
        // OemServices
        //
        Status = OemSvcUpdateSsidSvidInfo (
                   Bus, 
                   Dev, 
                   Func, 
                   VendorId, 
                   DeviceId, 
                   ClassCode, 
                   &SsidSvid
                   );

        if (!EFI_ERROR(Status)) {
          if (VendorId == INTERNAL_VGA_VID && (DeviceId == INTERNAL_VGA_DID13 || DeviceId == INTERNAL_VGA_DID14)) {
            if (DefaultSsidSvid != PciExpressRead32 (PCI_EXPRESS_LIB_ADDRESS (Bus, Dev, Func, PCI_SVID_OFFSET))) {
              DisableAmdCpmSetGpuSsidSvid ();
            }
          }
          continue;
        }
//        //
//        // 2. Program SSID/SVID, this function should only service project specific device which should not be at Bus 0
//        //
//        if (Bus > 0) {
//          Status = ProgramSsidSvidForSpecialDevice (Bus, Dev, Func, VendorId, DeviceId, ClassCode, SsidSvid);
//          if (!EFI_ERROR(Status)) {
//             continue;
//          }
//        }

        Status = DxeCsSvcProgramChipsetSsid (Bus, Dev, Func, VendorId, DeviceId, SsidSvid);
        if (!EFI_ERROR(Status)) {
          if (VendorId == INTERNAL_VGA_VID && (DeviceId == INTERNAL_VGA_DID13 || DeviceId == INTERNAL_VGA_DID14)) {
            if (DefaultSsidSvid != PciExpressRead32 (PCI_EXPRESS_LIB_ADDRESS (Bus, Dev, Func, PCI_SVID_OFFSET))) {
              DisableAmdCpmSetGpuSsidSvid ();
            }
          }
          continue;
        }
        ProgramSsidSvid (Bus, Dev, Func, SsidSvid);
      }
    } 
  }
  return ;
}

VOID
ProgramSsidSvid (
  IN     UINT8     Bus,
  IN     UINT8     Dev,
  IN     UINT8     Func,
  IN     UINT32    SsidSvid
  )
{
  UINT8           PciHeaderType;
  UINT8           SubsystemCapOffset;
  UINT8           PciSsidOffset;
  EFI_STATUS      Status;
  UINT64          BootScriptPciAddress;

  PciHeaderType = PciExpressRead8 (PCI_EXPRESS_LIB_ADDRESS (Bus, Dev, Func, PCI_HEADER_TYPE_OFFSET));

  if (FixedPcdGetBool (PcdNoBridgeDeviceSsid)) {
    UINT16          ClassCode;
    //
    // According to PCI spec Section "Subsystem Vendor ID and Subsystem ID"
    //   Base class 6 with sub class 0,1,2,3,4, or base class 8 with sub class 0,1,2,3
    //   are excluded from the requirement of programming these registers
    //
    ClassCode = PciExpressRead16 (PCI_EXPRESS_LIB_ADDRESS (Bus, Dev, Func, PCI_CLASSCODE_OFFSET + 1));
    if ((ClassCode >= (PCI_CLASS_BRIDGE << 8) + PCI_CLASS_BRIDGE_HOST) 
      && (ClassCode <= (PCI_CLASS_BRIDGE << 8) + PCI_CLASS_BRIDGE_P2P)) {
      return;
    }
    if ((ClassCode >= (PCI_CLASS_SYSTEM_PERIPHERAL << 8) + PCI_SUBCLASS_PIC) && 
      (ClassCode <= (PCI_CLASS_SYSTEM_PERIPHERAL << 8) + PCI_SUBCLASS_RTC)) {
      return;
    }
  }

  if (( PciHeaderType & HEADER_LAYOUT_CODE) != HEADER_TYPE_DEVICE ) {
    Status = PciFindCapId ( Bus, Dev, Func, EFI_PCI_CAPABILITY_ID_SSID, &SubsystemCapOffset );
    if ( !EFI_ERROR ( Status ) ) {
      PciSsidOffset = SubsystemCapOffset + 0x04;
      PciExpressWrite32 (PCI_EXPRESS_LIB_ADDRESS (Bus, Dev, Func, PciSsidOffset), SsidSvid);

      BootScriptPciAddress = S3_BOOT_SCRIPT_LIB_PCI_ADDRESS (Bus, Dev, Func, PciSsidOffset);
      S3BootScriptSavePciCfgWrite (
          S3BootScriptWidthUint32, 
          BootScriptPciAddress,
          1, 
          &SsidSvid);

    }
  } else {
    PciExpressWrite32 (PCI_EXPRESS_LIB_ADDRESS (Bus, Dev, Func, PCI_SUBSYSTEM_VENDOR_ID_OFFSET), SsidSvid);

    BootScriptPciAddress = S3_BOOT_SCRIPT_LIB_PCI_ADDRESS (Bus, Dev, Func, PCI_SUBSYSTEM_VENDOR_ID_OFFSET);
    S3BootScriptSavePciCfgWrite (
        S3BootScriptWidthUint32, 
        BootScriptPciAddress,
        1, 
        &SsidSvid);
    
  }
  return;
}

EFI_STATUS
DisableAmdCpmSetGpuSsidSvid (
  )
{
  EFI_STATUS                       Status;
  AMD_CPM_DISPLAY_FEATURE_PROTOCOL *CpmDisplayFeatureProtocolPtr;
  AMD_CPM_DISPLAY_FEATURE_TABLE    *DisplayFeatureTablePtr;

  Status = gBS->LocateProtocol (
                  &gAmdCpmDisplayFeatureProtocolGuid,
                  NULL,
                  (VOID**)&CpmDisplayFeatureProtocolPtr
                  );
  if (!EFI_ERROR (Status)) {
    DisplayFeatureTablePtr = CpmDisplayFeatureProtocolPtr->DisplayFeatureData.DisplayFeatureTablePtr;
    DisplayFeatureTablePtr->FunctionDisableMask |= BIT0;
  }

  return Status;
}

/**
  Program SSID/SVID on special add-on device with special mechanism.

  @param[in] Bus        PCI Bus number.
  @param[in] Dev        PCI Device number.
  @param[in] Func       PCI Function number.
  @param[in] VendorId   Vendor ID.
  @param[in] DeviceId   Device ID.
  @param[in] ClassCode  Class Code.
  @param[in] SsidSvid   SSID/SVID.

  @retval EFI_SUCCESS     Find out the special add-on device from list, 
                          and program SSID/SVID on it successfully.
  @retval EFI_UNSUPPORTED Can't find the special add-on device, 
                          and program SSID/SVID as default.

**/
//EFI_STATUS
//ProgramSsidSvidForSpecialDevice(
//  IN UINT8             Bus,
//  IN UINT8             Dev,
//  IN UINT8             Func,
//  IN UINT16            VendorId,
//  IN UINT16            DeviceId,
//  IN UINT16            ClassCode,
//  IN UINT32            SsidSvid
//  )
//{
//  UINT8                               Index;
//
////
//// Get special programming add-on device list from default PlatformSsidTable.
////
//  for (Index = 0; mPlatformSsidTableDefault[Index].SpecialSsidSvidFunction != NULL; Index++ ) {
//    if (mPlatformSsidTableDefault[Index].VendorId == VendorId) {
//      if ((mPlatformSsidTableDefault[Index].DeviceId == DEVICE_ID_DONT_CARE)
//        || (mPlatformSsidTableDefault[Index].DeviceId == DeviceId)){
//        mPlatformSsidTableDefault[Index].SpecialSsidSvidFunction (Bus, Dev, Func, SsidSvid);
//        return EFI_SUCCESS;
//      }
//    }
//  }
//
//  return EFI_UNSUPPORTED;
//}  