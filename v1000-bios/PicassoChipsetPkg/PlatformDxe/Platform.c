/** @file

;*******************************************************************************
;* Copyright (c) 2013 - 2017, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;*******************************************************************************
*/

#include "Platform.h"
#include "UsbLegacy.h"
#include <Library/DxeServicesTableLib.h>

EFI_PS2_POLICY_PROTOCOL  mPs2PolicyData = {
  EFI_KEYBOARD_NUMLOCK,
  (EFI_PS2_INIT_HARDWARE) Ps2SkipHWInit
};


/**

  Initial some platform specific register.

  @param  Event
  @param  *Context

**/
VOID
EFIAPI
PlatformSpecialRegInit (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
{
  EFI_STATUS                    Status;
  UINT64                        TopMem;
  EFI_GLOBAL_NVS_AREA_PROTOCOL  *GlobalNvsAreaProtocol;
  EFI_GLOBAL_NVS_AREA           *GlobalNvsAreaPtr;


  Status = gBS->LocateProtocol (&gEfiGlobalNvsAreaProtocolGuid, NULL, (VOID**)&GlobalNvsAreaProtocol);
  if (!EFI_ERROR (Status)) {
    GlobalNvsAreaPtr = GlobalNvsAreaProtocol->Area;

    TopMem = AsmReadMsr64 (TOP_MEM);
    GlobalNvsAreaPtr->TopOfMemLo = (UINT8)(TopMem >> 16);
    GlobalNvsAreaPtr->TopOfMemHi = (UINT8)(TopMem >> 24);
    GlobalNvsAreaPtr->RomSize = PcdGet32 (PcdFlashAreaSize);
  }

  return;
}

//struct {
//  UINT8   Bus;
//  UINT8   Dev;
//  UINT8   Func;
//} OnboardEhciControllerList[] = {
//  FCH_EHCI1_BUS,
//  FCH_EHCI1_DEV,
//  FCH_EHCI1_FUNC,
//
//  FCH_EHCI2_BUS,
//  FCH_EHCI2_DEV,
//  FCH_EHCI2_FUNC,
//
//  FCH_EHCI3_BUS,
//  FCH_EHCI3_DEV,
//  FCH_EHCI3_FUNC,
//  //End of List
//  0xff
//};

//struct {
//  UINT8   Bus;
//  UINT8   Dev;
//  UINT8   Func;
//} OnboardUsbControllerList[] = {
//  FCH_OHCI1_BUS,
//  FCH_OHCI1_DEV,
//  FCH_OHCI1_FUNC,
//
//  FCH_OHCI2_BUS,
//  FCH_OHCI2_DEV,
//  FCH_OHCI2_FUNC,
//
//  FCH_OHCI3_BUS,
//  FCH_OHCI3_DEV,
//  FCH_OHCI3_FUNC,
//
//  //End of List
//  0xff
//};

//VOID
//EhciResetUsbController (
//  VOID
//  )
//{
//  UINTN       Bar0Addr;
//  UINTN       Index;
//  UINT16      Value16;
//  UINTN       HcOperationAddr;
//  UINTN       EecpPtr;
//  UINTN       PciAddress;
//
//  Value16 = 0;
//  for (Index = 0; OnboardEhciControllerList[Index].Bus != 0xff; Index++ ) {
//    PciAddress = PCI_EXPRESS_LIB_ADDRESS (OnboardEhciControllerList[Index].Bus, OnboardEhciControllerList[Index].Dev, OnboardEhciControllerList[Index].Func, 0);
//
//    if (PciExpressRead16 (PciAddress + R_VENDOR_ID) == 0xFFFF) {
//      continue;
//    }
//    Bar0Addr = PciExpressRead32 (PciAddress + R_BAR0);
//
//    Bar0Addr &= 0xffffffc0;
//
//    EecpPtr = MmioRead8 (Bar0Addr + R_FCH_EHCI_MEM_EECP_CAPABILITY_PTR);
//
//    PciExpressWrite16 (PciAddress + EecpPtr + R_FCH_EHCI_LEGACY_CONTROL, 0);
//
//    HcOperationAddr = Bar0Addr + MmioRead8 (Bar0Addr + R_FCH_EHCI_MEM_CAPLENGTH);
//    MmioWrite32 (HcOperationAddr + R_FCH_EHCI_MEM_COMMAND, B_FCH_EHCI_MEM_RESET_HC);
//    MicroSecondDelay (9000);
//    MmioOr32 (HcOperationAddr + R_FCH_EHCI_MEM_CONFIGURATION, B_CONFIGURATION);
//    MmioWrite32 (HcOperationAddr + R_FCH_EHCI_MEM_STATUS, MmioRead8 (HcOperationAddr + R_FCH_EHCI_MEM_STATUS));
//
//    Value16 = PciExpressRead16 (PciAddress + EecpPtr + R_FCH_EHCI_LEGACY_STATUS);
//
//    PciExpressWrite16 (PciAddress + EecpPtr + R_FCH_EHCI_LEGACY_STATUS, Value16);
//
//    PciExpressWrite8 (PciAddress + R_PCI_CFG_COMMAND, 0);
//
//    PciExpressWrite32 (PciAddress + R_BAR0, 0);
//  }
//
//}

//VOID
//OhciResetUsbController (
//  VOID
//  )
//{
//  UINTN       Bar0Addr;
//  UINTN       Index;
//  UINT32      Value32;
//  UINT32      Count;
//  UINTN       PciAddress;
//
//  Value32 = 0;
//
//  for (Index = 0; OnboardUsbControllerList[Index].Bus != 0xff; Index++ ) {
//    PciAddress = PCI_EXPRESS_LIB_ADDRESS (OnboardUsbControllerList[Index].Bus, OnboardUsbControllerList[Index].Dev, OnboardUsbControllerList[Index].Func, 0);
//
//    if (PciExpressRead16 (PciAddress + R_VENDOR_ID) == 0xFFFF) {
//      continue;
//    }
//    Bar0Addr = PciExpressRead32 (PciAddress + R_BAR0);
//
//    Bar0Addr &= 0xffffffc0;
//
//    //
//    // 1. Clear All Int Status
//    //
//    MmioWrite32 (Bar0Addr + R_FCH_OHCI_MEM_HC_INT_STATUS, 0xffffffff);
//
//    //
//    // 2. Clear HC Control Register
//    //
//    MmioAnd32 (Bar0Addr + R_FCH_OHCI_MEM_HC_CONTROL, (UINT32)~(BIT2 + BIT4 + BIT5 + BIT8));
//
//    //
//    // 3. Clear HcControlHeadED
//    //
//    MmioWrite32 (Bar0Addr + R_FCH_OHCI_MEM_HC_CONTROL_HEAD_ED, 0);
//    MmioWrite32 (Bar0Addr + R_FCH_OHCI_MEM_HC_BULK_HEAD_ED, 0);
//
//    //
//    // 4. Reset Host Controller
//    //
//    MmioAnd32 (Bar0Addr + R_FCH_OHCI_MEM_HC_CONTROL, (UINT32)~(0x7FF));
//
//    MmioOr32 (Bar0Addr + R_FCH_OHCI_MEM_HC_COMMAND_STATUS, BIT0);
//
//    //
//    // 5. Wait Host Controller Reset Complete
//    //
//    for (Count = 0; Count < 100; Count++) {
//      if (!(MmioRead32 (Bar0Addr + R_FCH_OHCI_MEM_HC_COMMAND_STATUS) & BIT0)) break;
//    }
//
//    //
//    // 6. Disable All INT
//    //
//    MmioWrite32 (Bar0Addr + R_FCH_OHCI_MEM_HC_INT_ENABLE, 0);
//    MmioWrite32 (Bar0Addr + R_FCH_OHCI_MEM_HC_INT_STATUS, 0xffffffff);
//
//    //
//    // 7. Clear Controller Bar0 Resource and Command resgister
//    //
//    PciExpressWrite8 (PciAddress + R_PCI_CFG_COMMAND, 0);
//
//    PciExpressWrite32 (PciAddress + R_BAR0, 0);
//  }
//
//  return ;
//}


/**

  For OEM to implement any required initialize just after PCI resource assignment.

  @param  Event
  @param  *Context

**/
VOID
EFIAPI
PciEnumCompleteCallback (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
{
  EFI_STATUS              Status;
  VOID                    *Interface;

  //
  // Insure the protocol really installed
  //
  Status = gBS->LocateProtocol (
                  &gEfiPciEnumerationCompleteProtocolGuid,
                  NULL,
                  (VOID **)&Interface
                  );
  if (Status != EFI_SUCCESS) {
    return ;
  }

  //
  // Close event for one time shot
  //
  gBS->CloseEvent (Event);

  Status = OemSvcDxePlatformAfterAllPciInitFinished ();

  return ;
}

//VOID
//ResetUsbController (
//  VOID
//  )
//{
//  if (!PcdGetBool (PcdH2ODdtSupported)) {
//    EhciResetUsbController ();
//  }
////  OhciResetUsbController ();
//}

//VOID
//DisableEhciByPlatformHardwareSwitch (
//  VOID
//  )
//{
//  VOID                      *Buffer;
//  PLATFORM_HARDWARE_SWITCH  *PlatformHardwareSwitch;
//  UINTN                     PciAddress;
//  UINT8                     Setting;
//  UINT32                    EhciCtrl0MisControl2;
//  BOOLEAN                   EchiCtrl2Exist;
//
//  Buffer = GetFirstGuidHob (&gPlatformHardwareSwitchGuid);
//
//  Setting = 0;
//  EhciCtrl0MisControl2 = 0;
//  EchiCtrl2Exist = FALSE;
//  PlatformHardwareSwitch = NULL;
//
//   if (Buffer == NULL) {
//    return;
//  }
//
//  //
//  // Back up each ShareRegisterEnable flags of Misc Control 2 of all EHCI controllers then clean it
//  //
//  PciAddress = PCI_EXPRESS_LIB_ADDRESS (
//                 FCH_EHCI1_BUS,
//                 FCH_EHCI1_DEV, 
//                 FCH_EHCI1_FUNC, 
//                 R_FCH_EHCI_MISC_COLTROL2
//                 );
//
//  EhciCtrl0MisControl2 = PciExpressRead32(PciAddress);
//  PciAddress = PCI_EXPRESS_LIB_ADDRESS (
//                 FCH_EHCI1_BUS,
//                 FCH_EHCI1_DEV,
//                 FCH_EHCI1_FUNC,
//                 R_FCH_EHCI_MISC_COLTROL2
//                 );
//  PciExpressWrite32 (
//    PciAddress,
//    (EhciCtrl0MisControl2 & (!B_FCH_EHCI_MISC_COLTROL2_SHARE_REG_ENABLE))
//  );
//
//
//  //
//  // Disable all EHCI ports and XHCI ports by PlatformHardwareSwitch setting
//  //
//  PlatformHardwareSwitch = (PLATFORM_HARDWARE_SWITCH *) GET_GUID_HOB_DATA (Buffer);
//
//  if ((PlatformHardwareSwitch->UsbPortSwitch.CONFIG & 0x0F) != 0x0F) {
//  	Setting = (~PlatformHardwareSwitch->UsbPortSwitch.CONFIG) & 0x0F;
//  	PciAddress = PCI_EXPRESS_LIB_ADDRESS (
//  	               FCH_EHCI1_BUS,
//  	               FCH_EHCI1_DEV,
//  	               FCH_EHCI1_FUNC,
//  	               R_FCH_EHCI_HUB_CONFIG4 + 2
//  	               );
//  	PciExpressAndThenOr8 (
//  	  PciAddress,
//  	  0x0F,
//  	  (Setting << 4)
//  	  );
//  }
//
//  if ((PlatformHardwareSwitch->UsbPortSwitch.CONFIG & 0xF0) != 0xF0) {
//    Setting = ((~PlatformHardwareSwitch->UsbPortSwitch.CONFIG) & 0xF0) >> 4;
//    PciAddress = PCI_EXPRESS_LIB_ADDRESS (
//                   FCH_XHCI_BUS,
//                   FCH_XHCI_DEV,
//                   FCH_XHCI_FUNC,
//                   FCH_XHCI_REG48
//                   );
//    PciExpressWrite32 (
//      PciAddress,
//      R_XHCI_INDIRECT_DISABLE_PORT_REGISTER
//      );
//  
//    PciAddress = PCI_EXPRESS_LIB_ADDRESS (
//                   FCH_XHCI_BUS,
//                   FCH_XHCI_DEV,
//                   FCH_XHCI_FUNC,
//                   FCH_XHCI_REG4C
//                   );
//    PciExpressAndThenOr8 (
//       PciAddress,
//       0xFC,
//       Setting
//       ); 
//  
//  
//  }
//
//  //
//  // Recovery each ShareRegisterEnable flags of Misc Control 2 of all EHCI controllers
//  // 
//  PciAddress = PCI_EXPRESS_LIB_ADDRESS (
//                 FCH_EHCI1_BUS,
//                 FCH_EHCI1_DEV,
//                 FCH_EHCI1_FUNC,
//                 R_FCH_EHCI_MISC_COLTROL2
//                 ); 
//  PciExpressWrite32 (
//    PciAddress,
//    EhciCtrl0MisControl2
//  );
//
//  return;
//}

EFI_STATUS
InstallGlobalNvsAreaProtocol (
  VOID
  )
{
  EFI_STATUS                    Status;
  EFI_HANDLE                    Handle;
  UINTN                         BufferSize;
  EFI_GLOBAL_NVS_AREA_PROTOCOL  *GlobalNvsAreadProtocol;
  EFI_GLOBAL_NVS_AREA           *GlobalNvsAreaPtr;
  EFI_OEM_GLOBAL_NVS_AREA       *OemGlobalNvsAreaPtr;

  BufferSize = sizeof (EFI_GLOBAL_NVS_AREA_PROTOCOL);
  Status = gBS->AllocatePool (
                  EfiACPIMemoryNVS,
                  BufferSize,
                  (VOID**)&GlobalNvsAreadProtocol
                  );
  if (EFI_ERROR(Status)) {
    return Status;
  }
  ZeroMem (GlobalNvsAreadProtocol, sizeof (EFI_GLOBAL_NVS_AREA_PROTOCOL));

  BufferSize = sizeof (EFI_GLOBAL_NVS_AREA);
  Status = gBS->AllocatePool (
                  EfiACPIMemoryNVS,
                  BufferSize,
                  (VOID**)&GlobalNvsAreaPtr
                  );
  if (EFI_ERROR(Status)) {
    return Status;
  }
  ZeroMem (GlobalNvsAreaPtr, BufferSize);
  GlobalNvsAreadProtocol->Area = GlobalNvsAreaPtr;

  BufferSize = sizeof (EFI_OEM_GLOBAL_NVS_AREA);
  Status = gBS->AllocatePool (
                  EfiACPIMemoryNVS,
                  BufferSize,
                  (VOID**)&OemGlobalNvsAreaPtr
                  );
  if (EFI_ERROR(Status)) {
    return Status;
  }
  ZeroMem (OemGlobalNvsAreaPtr, BufferSize);
  GlobalNvsAreadProtocol->OemArea = OemGlobalNvsAreaPtr;

  Handle = NULL;
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &Handle,
                  &gEfiGlobalNvsAreaProtocolGuid,
                  GlobalNvsAreadProtocol,
                  NULL
                  );

  return Status;
}


//VOID
//DisableEhciByPlatformHardwareSwitch (
//  VOID
//  );

/**

  This is the standard EFI driver point for the Platform Driver. This
  driver is responsible for setting up any platform specific policy or
  initialization information.

  @param  ImageHandle      Handle for the image of this driver
  @param  SystemTable      Pointer to the EFI System Table

  @retval EFI_SUCCESS      Policy decisions set

**/
EFI_STATUS
DxePlatformEntryPoint (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS                        Status;
  EFI_HANDLE                        Handle;
  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL   *PciRootBridgeIo;
  EFI_EVENT                         LastInitEvent;
  EFI_EVENT                         PciEnumCompleteEvent;
  VOID                              *Registration;
  EFI_BOOT_MODE                     BootMode;
  BOOLEAN                           Flag;
  KERNEL_CONFIGURATION              KernelSetup;
  CHIPSET_CONFIGURATION             ChipsetSetup;
  

  Status = gBS->LocateProtocol (&gEfiPciRootBridgeIoProtocolGuid, NULL, (VOID**)&PciRootBridgeIo);
  ASSERT_EFI_ERROR (Status);

  Status = GetKernelConfiguration (&KernelSetup);
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = GetChipsetConfiguration (&ChipsetSetup);
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  BootMode = GetBootModeHob();

  //
  // Provide a unified hook point for OEM after Pci enumeration
  //
  Status = gBS->CreateEvent (
                  EVT_NOTIFY_SIGNAL,
                  TPL_CALLBACK,
                  PciEnumCompleteCallback,
                  NULL,
                  &PciEnumCompleteEvent
                  );
  ASSERT_EFI_ERROR (Status);

  if (!EFI_ERROR (Status)) {
    Status = gBS->RegisterProtocolNotify (
                    &gEfiPciEnumerationCompleteProtocolGuid,
                    PciEnumCompleteEvent,
                    &Registration
                    );
    ASSERT_EFI_ERROR (Status);
  }

//  if (BootMode == BOOT_IN_RECOVERY_MODE) {
//    //
//    // In the Recovery Mode,
//    // need to reset the USB to insure no any pending SMIs during POST
//    //
//    ResetUsbController ();
//  }

  Flag = TRUE;
  Status = OemSvcHookPlatformDxe (&ChipsetSetup, PciRootBridgeIo, Flag);
  DEBUG ( ( EFI_D_ERROR | EFI_D_INFO, "DxeOemSvcChipsetLib OemSvcHookPlatformDxe 1, Status : %r\n", Status ) );
  
  //
  // Initialize Platform Specific registers
  //
  ProcessRegTablePci (gPlatformRegs, PciRootBridgeIo, NULL);

  if (KernelSetup.NumLock == 0) {
    mPs2PolicyData.KeyboardLight &= (~EFI_KEYBOARD_NUMLOCK);
  }

  //
  // Install protocol to to allow access to this Policy.
  //
  Handle = NULL;
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &Handle,
                  &gEfiPs2PolicyProtocolGuid, &mPs2PolicyData,
                  NULL
                  );
  ASSERT_EFI_ERROR(Status);

  //
  // Install Legacy USB setup policy protocol depending upon
  // whether or not Legacy USB setup options are enabled or not.
  //
  UsbLegacyPlatformInstall ();

  InstallGlobalNvsAreaProtocol ();

  Flag = FALSE;
  Status = OemSvcHookPlatformDxe (&ChipsetSetup, PciRootBridgeIo, Flag);
  DEBUG ( ( EFI_D_ERROR | EFI_D_INFO, "DxeOemSvcChipsetLib OemSvcHookPlatformDxe 1, Status : %r\n", Status ) );

//  DisableEhciByPlatformHardwareSwitch ();

  Status = EfiCreateEventReadyToBootEx (
             TPL_NOTIFY,
             PlatformSpecialRegInit,
             NULL,
             &LastInitEvent
             );
  ASSERT_EFI_ERROR (Status);

  if (((KernelSetup.SataCnfigure == 2) ||
       (KernelSetup.SataCnfigure == 5)) &&
    (ChipsetSetup.AHCIMODE == 1)) {
    Status = gBS->InstallMultipleProtocolInterfaces (
                    &Handle,
                    &gEfiLoadAmdAhciUefiDriverProtocolGuid,
                    NULL,
                    NULL
                    );
  }

  return EFI_SUCCESS;
}
