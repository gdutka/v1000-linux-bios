/** @file
  A SMM driver for UsbLegacyControl initialization.

;*******************************************************************************
;* Copyright (c) 2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;*******************************************************************************
*/

#include "UsbLegacyControl.h"

EFI_GUID                        mUsbLegacyControlProtocolGuid = USB_LEGACY_CONTROL_PROTOCOL_GUID;
BOOLEAN                         mIo60InOutSelect = FALSE;
BOOLEAN                         mIo64InOutSelect = FALSE;

//
// The device path used for SMI registration. Use dev:0x12 fun:0x00 to it
//
struct {
  ACPI_HID_DEVICE_PATH          Acpi;
  PCI_DEVICE_PATH               Pci;
  EFI_DEVICE_PATH_PROTOCOL      End;
} mDevicePath = {
  { {ACPI_DEVICE_PATH, ACPI_DP, {(UINT8)(sizeof (ACPI_HID_DEVICE_PATH)), (UINT8)((sizeof (ACPI_HID_DEVICE_PATH)) >> 8)}}, EISA_PNP_ID (0x0A03), 0 },
  { {HARDWARE_DEVICE_PATH, HW_PCI_DP, {(UINT8)(sizeof (PCI_DEVICE_PATH)), (UINT8)((sizeof (PCI_DEVICE_PATH)) >> 8)}}, 0x00, 0x12 },
  { END_DEVICE_PATH_TYPE, END_ENTIRE_DEVICE_PATH_SUBTYPE, {END_DEVICE_PATH_LENGTH, 0} }
};

//
// The body of UsbLegacyControlProtocol used for protocol installation
//
typedef struct {
  USB_LEGACY_CONTROL_PROTOCOL   UsbLegacyControlProtocol;
  BOOLEAN                       SmiHandlerInstalled;
} USB_LEGACY_CONTROL_PRIVATE;

USB_LEGACY_CONTROL_PRIVATE      mPrivate = {
  {UsbLegacyControl, NULL, NULL, FALSE},
  FALSE
};

EFI_SMM_SYSTEM_TABLE2               *mSmst;

/**
  Entry point for EFI drivers.

  @param  ImageHandle - EFI_HANDLE
  @param  SystemTable - EFI_SYSTEM_TABLE

  @retval EFI_SUCCESS

**/
EFI_STATUS
EFIAPI
UsbLegacyControlDriverEntryPoint (
  IN  EFI_HANDLE                                ImageHandle,
  IN  EFI_SYSTEM_TABLE                          *SystemTable
  )
{
  EFI_STATUS                            Status;
  EFI_HANDLE                            Handle;
  BOOLEAN                               InSmm;
  EFI_SMM_USB_DISPATCH2_PROTOCOL        *UsbDispatch;
  EFI_SMM_USB_REGISTER_CONTEXT          UsbContext;
  EFI_SMM_BASE2_PROTOCOL                *SmmBase;
  UINT8                                 Data8;
  EFI_USB_LEGACY_PLATFORM_PROTOCOL      *UsbLegacyPlatform;
  USB_LEGACY_MODIFIERS                  UsbLegacyModifiers;

  Data8 = 0;

  Status = SystemTable->BootServices->LocateProtocol (
                          &gEfiUsbLegacyPlatformProtocolGuid,
                          NULL,
                          (VOID**)&UsbLegacyPlatform
                          );

  if (EFI_ERROR (Status)) {
    return Status;
  }  

  ZeroMem (&UsbLegacyModifiers, sizeof (USB_LEGACY_MODIFIERS));
  Status = UsbLegacyPlatform->GetUsbPlatformOptions (
                                UsbLegacyPlatform,
                                &UsbLegacyModifiers
                                );

  if (EFI_ERROR(Status) || (!UsbLegacyModifiers.UsbLegacyEnable)) {
    return EFI_ABORTED;
  }

  //
  // if port 64 is "0xff", it's legacyFree
  //
  if (IoRead8 (0x64) != 0xFF) {
    return EFI_UNSUPPORTED ;
  }

  Status= gBS->LocateProtocol (
                 &gEfiSmmBase2ProtocolGuid,
                 NULL,
                 (VOID**)&SmmBase
                 );
  if (!EFI_ERROR (Status)) {
    SmmBase->InSmm (SmmBase, &InSmm);
  } else {
    InSmm = FALSE;
  }

  if (InSmm) {
    //
    // To get Smm Syatem Table.
    //
    Status = SmmBase->GetSmstLocation (
                        SmmBase,
                        &mSmst
                        );
    ASSERT_EFI_ERROR (Status);
    if (EFI_ERROR (Status)) {
      return Status;
    }
    //
    // Register USB legacy SMI handler
    //
    Status = mSmst->SmmLocateProtocol (
                      &gEfiSmmUsbDispatch2ProtocolGuid,
                      NULL,
                      (VOID**)&UsbDispatch
                      );
    if (EFI_ERROR(Status)) {
      return EFI_UNSUPPORTED;
    }
    UsbContext.Type = UsbLegacy;
    UsbContext.Device = (EFI_DEVICE_PATH_PROTOCOL*)&mDevicePath;
    Status = UsbDispatch->Register (
                            UsbDispatch,
                            (EFI_SMM_HANDLER_ENTRY_POINT2) UsbLegacyControlSmiHandler,
                            &UsbContext,
                            &Handle
                            );
    if (EFI_ERROR(Status)) {
      return EFI_UNSUPPORTED;
    }
    //
    // This is SMM instance
    //
    mPrivate.UsbLegacyControlProtocol.InSmm = TRUE;
  } else {
    IoWrite8 (FCH_IOMAP_REGCD6, FCH_PMIOA_REGED);
    Data8 = IoRead8 (FCH_IOMAP_REGCD7);
    Data8 &= ~(BIT1);
    Data8 |= BIT1;
    IoWrite8 (FCH_IOMAP_REGCD6, FCH_PMIOA_REGED);
    IoWrite8 (FCH_IOMAP_REGCD7, Data8);
  }

  //
  // Install USB_LEGACY_CONTROL_PROTOCOL
  //
  Handle = NULL;
  Status = gBS->InstallProtocolInterface (
                  &Handle,
                  &mUsbLegacyControlProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &mPrivate.UsbLegacyControlProtocol
                  );

  return Status;
}

/**
  Main routine for the USB legacy control

  @param  Command - USB_LEGACY_CONTROL_SETUP_EMULATION
                    USB_LEGACY_CONTROL_GET_KBC_DATA_POINTER
                    USB_LEGACY_CONTROL_GET_KBC_STATUS_POINTER
                    USB_LEGACY_CONTROL_GENERATE_IRQ
  @param  Param   - The parameter for the command

  @retval EFI_SUCCESS

**/
EFI_STATUS
EFIAPI
UsbLegacyControl (
  IN     UINTN                                          Command,
  IN     VOID                                           *Param
  )
{
  UINT32          IntrReg;
  UINT32          ControlReg;
//  UINT32          StatusReg;

  switch (Command) {
    case USB_LEGACY_CONTROL_SETUP_EMULATION:  //USB3_HC_HID_SUPPORT_SETUP_EMULATION
      //
      // Used for emulation on/off
      //
      ControlReg = MmioRead32 (HCE_BASE_ADDRESS + HCE_CONTROL);
      IntrReg = MmioRead32 (HCE_BASE_ADDRESS + HCE_INTREN);

      if (!Param) {
        //
        // Turn off the trap SMI
        //
        ControlReg &= ~(HCE_CTRL_EMULATION_ENABLE + HCE_CTRL_IRQEN);
        IntrReg &= ~(HCE_INTREN_EMULATION_SMIEN);
        MmioWrite32 (HCE_BASE_ADDRESS + HCE_CONTROL, ControlReg);
        MmioWrite32 (HCE_BASE_ADDRESS + HCE_INTREN, IntrReg);
      } else {
        //
        // Turn on the trap SMI
        //
        ControlReg |= (HCE_CTRL_EMULATION_ENABLE + HCE_CTRL_IRQEN);
        IntrReg |= (HCE_INTREN_EMULATION_SMIEN);
        MmioWrite32 (HCE_BASE_ADDRESS + HCE_INTREN, IntrReg);
        MmioWrite32 (HCE_BASE_ADDRESS + HCE_CONTROL, ControlReg);
      }

      //
      // Inspect the SMI handler data to see is it need to update
      //
      if (Param && !mPrivate.SmiHandlerInstalled && mPrivate.UsbLegacyControlProtocol.SmiHandler) {
        mPrivate.SmiHandlerInstalled = TRUE;
        //
        // Trap SMI if now in BS to pass the SMI handler related data to SMM instance
        //
        if (mPrivate.UsbLegacyControlProtocol.InSmm == FALSE) {
          //
          // Trigger SMI through 64 port write
          //
          IoWrite8 (0x64 , 0);
        }
      }
      break;

    case USB_LEGACY_CONTROL_GET_KBC_DATA_POINTER:  //USB3_HC_HID_SUPPORT_GET_KBC_DATA_POINTER
      //
      // Get the "pointer" of KBC data (port 0x60)
      //
      if (mIo60InOutSelect) {
        *(UINT32**)Param = (UINT32*)(UINTN)(HCE_BASE_ADDRESS + HCE_INPUT);
        mIo60InOutSelect = FALSE;
      } else {
        *(UINT32**)Param = (UINT32*)(UINTN)(HCE_BASE_ADDRESS + HCE_OUTPUT);
      }
      break;

    case USB_LEGACY_CONTROL_GET_KBC_STATUS_POINTER:  //USB3_HC_HID_SUPPORT_GET_KBC_STATUS_POINTER
      //
      // Get the "pointer" of KBC status (port 0x64)
      //
      if (mIo64InOutSelect) {
        *(UINT32**)Param = (UINT32*)(UINTN)(HCE_BASE_ADDRESS + HCE_INPUT);
        mIo64InOutSelect = FALSE;
      } else {
        *(UINT32**)Param = (UINT32*)(UINTN)(HCE_BASE_ADDRESS + HCE_STATUS);
      }
      break;

    case USB_LEGACY_CONTROL_GENERATE_IRQ:  //USB3_HC_HID_SUPPORT_GENERATE_IRQ
      //
      // Set the CHARACTER_PENDING to make sure the read port 60 will generate SMI
      // for queued KBC output if any
      //
      ControlReg = MmioRead32 (HCE_BASE_ADDRESS + HCE_CONTROL);
      ControlReg |= HCE_CTRL_CHARACTER_PENDING;
      MmioWrite32 (HCE_BASE_ADDRESS + HCE_CONTROL, ControlReg);
      break;

    case USB_LEGACY_CONTROL_GET_CAPABILITY:
      //
      // report  ASYNC_CPU_TRAP capability to ehci/xhci.
      //
      *(UINT32**)Param = (UINT32*)(UINTN)CAP_ASYNC_CPU_TRAP; //BUGBUG need to check what is correct Param format.
      break;

  }

  return EFI_SUCCESS;
}

/**
  The UsbLegacyControl SMI main handler

  @param  Handle  - Handle
  @param  Context - Context

  @retval

**/
VOID
UsbLegacyControlSmiHandler (
  IN EFI_HANDLE  Handle,
  IN CONST VOID  *Context,
  IN OUT VOID    *CommBuffer      OPTIONAL,
  IN OUT UINTN   *CommBufferSize  OPTIONAL
  )
{
  UINT32                        StatusReg;
  UINT32                        ControlReg;
  UINT32                        TrapEvent;

  TrapEvent = 0;

  ControlReg = MmioRead32 (HCE_BASE_ADDRESS + HCE_CONTROL);

  if (ControlReg & (HCE_CTRL_EMULATION_INTERRUPT)) {
    StatusReg = MmioRead32 (HCE_BASE_ADDRESS + HCE_STATUS);
    if (ControlReg & HCE_CTRL_GATEA20_SEQUENCE) {
      //
      // A20 gate sequence
      //
      TrapEvent = KBC_TRAP_A20GATE_CHANGED | KBC_TRAP_WRITE_DATA_PORT;

      ControlReg ^= HCE_CTRL_A20_STATE;
      MmioWrite32 (HCE_BASE_ADDRESS + HCE_CONTROL, ControlReg);
    } else if ((ControlReg & HCE_CTRL_CHARACTER_PENDING) && !(StatusReg & HCE_STATUS_OUTPUT_FULL)) {
      if (StatusReg & HCE_STATUS_INPUT_FULL) {
        mIo60InOutSelect = TRUE;
      } else {
        mIo60InOutSelect = FALSE;
      }
      //
      // Port 60 readed
      //
      TrapEvent = KBC_TRAP_READ_DATA_PORT;

      ControlReg &= ~(HCE_CTRL_CHARACTER_PENDING);  
      MmioWrite32 (HCE_BASE_ADDRESS + HCE_CONTROL, ControlReg);
    } else if (StatusReg & HCE_STATUS_INPUT_FULL) {
      //
      // Write operation occurred
      //
      if (StatusReg & HCE_STATUS_CMD_DATA) {
        //
        // Port 64 writed
        //
        mIo64InOutSelect = TRUE;
        TrapEvent = KBC_TRAP_WRITE_COMMAND_PORT;
      } else {
        //
        // Port 60 writed
        //
        mIo64InOutSelect = FALSE;
        TrapEvent = KBC_TRAP_WRITE_DATA_PORT;
      }
    }

    //
    // Call sub handler
    //
    if (mPrivate.UsbLegacyControlProtocol.SmiHandler) {
      mPrivate.UsbLegacyControlProtocol.SmiHandler (TrapEvent, mPrivate.UsbLegacyControlProtocol.SmiContext);
    }
  }
}
