/** @file
  Provide OEM to install the PCI Option ROM table and Non-PCI Option ROM table.

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <PiDxe.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiLib.h>

#include <Library/DxeOemSvcKernelLib.h>
#include <Protocol/PciRootBridgeIo.h>
#include <KernelSetupConfig.h>
#include <Library/KernelConfigLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiRuntimeLib.h>
#include <Protocol/PciIo.h>
#include <Library/UefiBootServicesTableLib.h>

/*++

Todo:
  Define the relateaed data.

//=======================================
//         Pci Option Rom Table
//=======================================
PCI_OPTION_ROM_TABLE      PciOptionRomTable[] = {
  {
    NULL_ROM_FILE_GUID,
    0,
    0,
    0,
    0,
    0xffff,
    0xffff
  }
};


//=======================================
//        Non Pci Option Rom Table
//=======================================
//
// System Rom table
//
SYSTEM_ROM_TABLE    SystemRomTable[] = {
  {
    //
    // CSM16 binary
    //
    SYSTEM_ROM_FILE_GUID,
    TRUE,
    SYSTEM_ROM
  },
  {
    NULL_ROM_FILE_GUID,
    FALSE
    MAX_NUM
  }
};
.
.
.
--*/

/**
  To handle a special case, more than 1 PCI devices which has the same VID/DID 
  exist on the system but not all of them has PCI option ROM to be loaded. 
  So provide an OEM service, let project owner to decide if this device has 
  PCI Option ROM to be loaded or not. if SkipGetPciRom is TRUE, 
  means there is no option ROM for this device, otherwise, 
  option ROM described on PCI Option ROM table will be loaded for this device. 
  This OEM service will only be invoked if the VID/DID of PCI device could be found in PCI Option ROM Table.	

  @param[in]  Segment               Segment number.
  @param[in]  Bus                   Bus number.
  @param[in]  Device                Device number.
  @param[in]  Function              Function number.
  @param[in]  VendorId              Device vendor ID.
  @param[in]  DeviceId              Device ID.
  @param[out] *SkipGetPciRom        If SkipGetPciRom == TRUE means that there is no 
			            ROM in this device.
  
  @retval     EFI_UNSUPPORTED       Returns unsupported by default.
  @retval     EFI_SUCCESS           The service is customized in the project.
  @retval     EFI_MEDIA_CHANGED     The value of IN OUT parameter is changed. 
  @retval     Others                Based on OEM design.
**/
EFI_STATUS
OemSvcSkipLoadPciOptionRom (
  IN  UINTN                                 Segment,
  IN  UINTN                                 Bus,
  IN  UINTN                                 Device,
  IN  UINTN                                 Function,
  IN  UINT16                                VendorId,
  IN  UINT16                                DeviceId,
  OUT BOOLEAN                               *SkipGetPciRom
  )
{
  /*++
    Todo:
      Add project specific code in here.
  --*/
//
//  Here is a sample to skip option ROM and UEFI driver for ASM1061 PCIE to SATA card
//  in current mechanism, nomatter it is in FV or on the card, all will be skipped
//
//  if (VendorId == 0x1b21 && DeviceId == 0x0612) {
//    *SkipGetPciRom = TRUE;
//    return EFI_MEDIA_CHANGED;
//  }
//
  EFI_STATUS                        Status;
  UINT8                             SecBus;
  UINT8                             SubBus;
  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL   *PciIoRootBridge;
  UINT64                            Address;
  UINT16                            TestVendorId;
  UINT16                            TestClassCode;
  KERNEL_CONFIGURATION              KernelConfiguration;

  DEBUG ((EFI_D_INFO, " OemSvcSkipLoadPciOptionRom-Start\n"));
  SecBus = 0;
  SubBus = 0;
  TestVendorId = 0;
  TestClassCode = 0;
  PciIoRootBridge = NULL;
  Address = 0;

  Status = GetKernelConfiguration (&KernelConfiguration);
  if (Status != EFI_SUCCESS) {
    return EFI_UNSUPPORTED;
  }

//[-start-240426-IB18160212-modify-]//
  Status = gBS->LocateProtocol (
                  &gEfiPciRootBridgeIoProtocolGuid,
                  NULL,
                  (VOID**)&PciIoRootBridge
                  );
  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }
//[-end-240426-IB18160212-modify-]//

//  if ((VendorId == 0x144D) && (DeviceId == 0xA802)) {
//    *SkipGetPciRom = TRUE;
//    return EFI_MEDIA_CHANGED;
//  }

  if (KernelConfiguration.BootType == EFI_BOOT_TYPE) {
    Address = EFI_PCI_ADDRESS (0, 1, 1, 0x19);
    PciIoRootBridge->Pci.Read (PciIoRootBridge, EfiPciIoWidthUint8, Address, 1, &SecBus);
    Address = EFI_PCI_ADDRESS (0, 1, 1, 0x1A);
    PciIoRootBridge->Pci.Read (PciIoRootBridge, EfiPciIoWidthUint8, Address, 1, &SubBus);
    if ((SecBus != 0) && (SecBus != 0xFF) && (SubBus != 0) && (SubBus != 0xFF)) {
      Address = EFI_PCI_ADDRESS (SecBus, 0, 0, 0);
      PciIoRootBridge->Pci.Read (PciIoRootBridge, EfiPciIoWidthUint16, Address, 1, &TestVendorId);
      Address = EFI_PCI_ADDRESS (SecBus, 0, 0, 0x0A);
      PciIoRootBridge->Pci.Read (PciIoRootBridge, EfiPciIoWidthUint16, Address, 1, &TestClassCode);
      if ((TestVendorId == 0x8086) && (TestClassCode == 0x0604)) {
        if ((Bus >= SecBus) && (Bus <= SubBus)) {
          *SkipGetPciRom = TRUE;
          if ((VendorId == 0x144D) && (DeviceId == 0xA802)) {
            return EFI_UNSUPPORTED;
          }
          return EFI_MEDIA_CHANGED;
        }
      }
    }

    Address = EFI_PCI_ADDRESS (0, 1, 2, 0x19);
    PciIoRootBridge->Pci.Read (PciIoRootBridge, EfiPciIoWidthUint8, Address, 1, &SecBus);
    Address = EFI_PCI_ADDRESS (0, 1, 2, 0x1A);
    PciIoRootBridge->Pci.Read (PciIoRootBridge, EfiPciIoWidthUint8, Address, 1, &SubBus);
    if ((SecBus != 0) && (SecBus != 0xFF) && (SubBus != 0) && (SubBus != 0xFF)) {
      Address = EFI_PCI_ADDRESS (SecBus, 0, 0, 0);
      PciIoRootBridge->Pci.Read (PciIoRootBridge, EfiPciIoWidthUint16, Address, 1, &TestVendorId);
      Address = EFI_PCI_ADDRESS (SecBus, 0, 0, 0x0A);
      PciIoRootBridge->Pci.Read (PciIoRootBridge, EfiPciIoWidthUint16, Address, 1, &TestClassCode);
      if ((TestVendorId == 0x8086) && (TestClassCode == 0x0604)) {
        if ((Bus >= SecBus) && (Bus <= SubBus)) {
          *SkipGetPciRom = TRUE;
          if ((VendorId == 0x144D) && (DeviceId == 0xA802)) {
            return EFI_UNSUPPORTED;
          }
          return EFI_MEDIA_CHANGED;
        }
      }
    }
  }

  return EFI_UNSUPPORTED;
}
