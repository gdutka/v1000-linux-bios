/** @file

;******************************************************************************
;* Copyright (c) 2012 - 2019, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

/**
  PS/2 Mouse driver. Routines that interacts with callers,
  conforming to EFI driver model.

Copyright (c) 2006 - 2011, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "Ps2Mouse.h"
#include "CommPs2.h"

///
/// DriverBinding Protocol Instance
///
EFI_DRIVER_BINDING_PROTOCOL gPS2MouseDriver = {
  PS2MouseDriverSupported,
  PS2MouseDriverStart,
  PS2MouseDriverStop,
  0xa,
  NULL,
  NULL
};

#define MAX_AUX_DEV 8
typedef struct {
  EFI_HANDLE Controller;
  BOOLEAN    IsConnect;
} PS2_MOUSE_DEVICE_INIT;

PS2_MOUSE_DEVICE_INIT mPs2Mouse[MAX_AUX_DEV] = {0};
UINTN                 EntryCount = 0;

/**
  Test to see if this driver supports ControllerHandle. Any ControllerHandle
  than contains a IsaIo protocol can be supported.

  @param  This                Protocol instance pointer.
  @param  ControllerHandle    Handle of device to test
  @param  RemainingDevicePath Optional parameter use to pick a specific child
                              device to start.

  @retval EFI_SUCCESS         This driver supports this device
  @retval EFI_ALREADY_STARTED This driver is already running on this device
  @retval other               This driver does not support this device

**/
EFI_STATUS
EFIAPI
PS2MouseDriverSupported (
  IN EFI_DRIVER_BINDING_PROTOCOL    *This,
  IN EFI_HANDLE                     Controller,
  IN EFI_DEVICE_PATH_PROTOCOL       *RemainingDevicePath
  )
{
  EFI_STATUS                          Status;
  EFI_ISA_IO_PROTOCOL                 *IsaIo;
  EFI_TPL                             OldTpl;
  BOOLEAN                             KeyboardEnable;
  UINT8                               Data;
  UINTN                               Index;

  Status = EFI_SUCCESS;
  Data   = 0;
  KeyboardEnable = FALSE;
  //
  // Open the IO Abstraction(s) needed to perform the supported test
  //
  Status = gBS->OpenProtocol (
                  Controller,
                  &gEfiIsaIoProtocolGuid,
                  (VOID **) &IsaIo,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_BY_DRIVER
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  //
  // Check Ps2Mouse has be connected
  //
  for (Index = 0; Index < MAX_AUX_DEV; Index++) {
    if ((mPs2Mouse[Index].Controller == Controller) && (mPs2Mouse[Index].IsConnect)) {
      gBS->CloseProtocol (
             Controller,
             &gEfiIsaIoProtocolGuid,
             This->DriverBindingHandle,
             Controller
             );
      return EFI_UNSUPPORTED;
    }
  }
  //
  // Use the ISA I/O Protocol to see if Controller is the Keyboard controller
  //
  switch (IsaIo->ResourceList->Device.HID) {
  case EISA_PNP_ID (0xF03):
  //
  // Microsoft PS/2 style mouse
  //
  case EISA_PNP_ID (0xF13):
    //
    // PS/2 Port for PS/2-style Mice
    //
    Status = IsaIo->Io.Read (IsaIo, EfiIsaIoWidthUint8, KBC_CMD_STS_PORT, 1, &Data);
    if ((EFI_ERROR (Status)) || (Data == 0xFF)) {
      Status = EFI_UNSUPPORTED;    //if status register is FF then KBC is missing
    }
    break;

  case EISA_PNP_ID (0x303):
    //
    // IBM Enhanced (101/102-key, PS/2 mouse support)
    //
    if (IsaIo->ResourceList->Device.UID != 1) {
      Status = EFI_UNSUPPORTED;
      break ;
    }

    //
    // Check KBC is exist
    //
    Status = IsaIo->Io.Read (IsaIo, EfiIsaIoWidthUint8, KBC_CMD_STS_PORT, 1, &Data);
    if ((EFI_ERROR (Status)) || (Data == 0xFF)) {
      //
      //if status register is FF then KBC is missing
      //
      Status =  EFI_UNSUPPORTED;
      break;
    }

    //
    // Raise TPL to avoid keyboard operation impact
    //
    OldTpl = gBS->RaiseTPL (TPL_NOTIFY);
    CheckKbStatus (IsaIo, &KeyboardEnable);
    KbcDisableKb (IsaIo);
    KbcDisableAux (IsaIo);

    //
    // KBC need enough time to check output buffer queue and then set OBF flag.
    //
    gBS->Stall (5000);

    //
    // Some KBC will
    // output buffer queue tail, so need clean output buffer queue
    //
    Status = CleanupOutputBuffer (IsaIo);
    if (EFI_ERROR (Status)) {
      Status =  EFI_UNSUPPORTED;
      gBS->RestoreTPL (OldTpl);
      break;
    }

    //
    // Some KBC must do SelfTest before sending mouse command.
    //
    if (((Data & KBC_SYSF) != KBC_SYSF) && !KeyboardEnable) {
      Status = KbcSelfTest (IsaIo);
      if (EFI_ERROR (Status)) {
        Status =  EFI_UNSUPPORTED;
        gBS->RestoreTPL (OldTpl);
        break;
      }
    }

    //
    // check whether there is Ps/2 mouse device in system
    //
    Status = PS2MouseEnable (IsaIo);
    if (EFI_ERROR (Status)) {
      Status =  EFI_UNSUPPORTED;
    }
    KbcDisableAux (IsaIo);

    //
    // clean output buffer queue if somebody shark mouse during POST time
    //
    CleanupOutputBuffer (IsaIo);
    mPs2Mouse[EntryCount].Controller  = Controller;
    mPs2Mouse[EntryCount].IsConnect   = TRUE;
    if (EntryCount < (MAX_AUX_DEV - 1)) {
      EntryCount++;
    }

    gBS->RestoreTPL (OldTpl);
    break ;

  default:
    Status = EFI_UNSUPPORTED;
    break;
  }

  if (KeyboardEnable) {
    KbcEnableKb (IsaIo);
  }

  //
  // Close the I/O Abstraction(s) used to perform the supported test
  //
  gBS->CloseProtocol (
         Controller,
         &gEfiIsaIoProtocolGuid,
         This->DriverBindingHandle,
         Controller
         );

  return Status;
}

/**
  Start this driver on ControllerHandle by opening a IsaIo protocol, creating
  PS2_MOUSE_ABSOLUTE_POINTER_DEV device and install gEfiAbsolutePointerProtocolGuid
  finally.

  @param  This                 Protocol instance pointer.
  @param  ControllerHandle     Handle of device to bind driver to
  @param  RemainingDevicePath  Optional parameter use to pick a specific child
                               device to start.

  @retval EFI_SUCCESS          This driver is added to ControllerHandle
  @retval EFI_ALREADY_STARTED  This driver is already running on ControllerHandle
  @retval other                This driver does not support this device

**/
EFI_STATUS
EFIAPI
PS2MouseDriverStart (
  IN EFI_DRIVER_BINDING_PROTOCOL    *This,
  IN EFI_HANDLE                     Controller,
  IN EFI_DEVICE_PATH_PROTOCOL       *RemainingDevicePath
  )
{
  EFI_STATUS                          Status;
  EFI_STATUS                          EmptyStatus;
  EFI_ISA_IO_PROTOCOL                 *IsaIo;
  PS2_MOUSE_DEV                       *MouseDev;
  UINT8                               Data;
  EFI_TPL                             OldTpl;
  EFI_STATUS_CODE_VALUE               StatusCode;
  EFI_DEVICE_PATH_PROTOCOL            *ParentDevicePath;
  BOOLEAN                             KeyboardEnable;

  StatusCode  = 0;
  MouseDev    = NULL;
  IsaIo       = NULL;
  //
  // Open the device path protocol
  //
  Status = gBS->OpenProtocol (
                  Controller,
                  &gEfiDevicePathProtocolGuid,
                  (VOID **) &ParentDevicePath,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_BY_DRIVER
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  //
  // Report that the keyboard is being enabled
  //
  REPORT_STATUS_CODE_WITH_DEVICE_PATH (
    EFI_PROGRESS_CODE,
    EFI_PERIPHERAL_MOUSE | EFI_P_PC_ENABLE,
    ParentDevicePath
    );

  //
  // Get the ISA I/O Protocol on Controller's handle
  //
  Status = gBS->OpenProtocol (
                  Controller,
                  &gEfiIsaIoProtocolGuid,
                  (VOID **) &IsaIo,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_BY_DRIVER
                  );
  if (EFI_ERROR (Status)) {
    gBS->CloseProtocol (
           Controller,
           &gEfiDevicePathProtocolGuid,
           This->DriverBindingHandle,
           Controller
           );
    return EFI_INVALID_PARAMETER;
  }
  //
  // Raise TPL to avoid keyboard operation impact
  //
  OldTpl = gBS->RaiseTPL (TPL_NOTIFY);

  //
  // Allocate private data
  //
  MouseDev = AllocateZeroPool (sizeof (PS2_MOUSE_DEV));
  if (MouseDev == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto ErrorExit;
  }
  //
  // Setup the device instance
  //
  MouseDev->Signature       = PS2_MOUSE_DEV_SIGNATURE;
  MouseDev->Handle          = Controller;
  MouseDev->SampleRate      = SampleRate20;
  MouseDev->Resolution      = MouseResolution4;
  MouseDev->Scaling         = Scaling1;
  MouseDev->DataPackageSize = 3;
  MouseDev->IsaIo           = IsaIo;
  MouseDev->DevicePath      = ParentDevicePath;


  //
  // Resolution = 4 counts/mm
  //
  MouseDev->Mode.ResolutionX                = 4;
  MouseDev->Mode.ResolutionY                = 4;
  MouseDev->Mode.LeftButton                 = TRUE;
  MouseDev->Mode.RightButton                = TRUE;

  MouseDev->SimplePointerProtocol.Reset     = MouseReset;
  MouseDev->SimplePointerProtocol.GetState  = MouseGetState;
  MouseDev->SimplePointerProtocol.Mode      = &(MouseDev->Mode);

  //
  // Initialize keyboard controller if necessary
  //
  IsaIo->Io.Read (IsaIo, EfiIsaIoWidthUint8, KBC_CMD_STS_PORT, 1, &Data);
  //
  // Fix for random hangs in System waiting for the Key if no KBC is present in BIOS.
  //
  if ((Data & (KBC_PARE | KBC_TIM)) == (KBC_PARE | KBC_TIM)) {
    //
    // If nobody decodes KBC I/O port, it will read back as 0xFF.
    // Check the Time-Out and Parity bit to see if it has an active KBC in system
    //
    Status     = EFI_DEVICE_ERROR;
    StatusCode = EFI_PERIPHERAL_MOUSE | EFI_P_EC_NOT_DETECTED;
    goto ErrorExit;
  }

  REPORT_STATUS_CODE_WITH_DEVICE_PATH (
    EFI_PROGRESS_CODE,
    EFI_PERIPHERAL_MOUSE | EFI_P_MOUSE_PC_SELF_TEST,
    ParentDevicePath
    );

  CheckKbStatus (MouseDev->IsaIo, &KeyboardEnable);
  if ((Data & KBC_SYSF) != KBC_SYSF) {
    if (!KeyboardEnable) {
      Status = KbcSelfTest (IsaIo);
      if (EFI_ERROR (Status)) {
        StatusCode = EFI_PERIPHERAL_MOUSE | EFI_P_EC_CONTROLLER_ERROR;
        goto ErrorExit;
      }
    }
  }

  KbcDisableAux (IsaIo);

  REPORT_STATUS_CODE_WITH_DEVICE_PATH (
    EFI_PROGRESS_CODE,
    EFI_PERIPHERAL_MOUSE | EFI_P_PC_PRESENCE_DETECT,
    ParentDevicePath
    );

  //
  // Setup the WaitForKey event
  //
  Status = gBS->CreateEvent (
                  EVT_NOTIFY_WAIT,
                  TPL_NOTIFY,
                  MouseWaitForInput,
                  MouseDev,
                  &((MouseDev->SimplePointerProtocol).WaitForInput)
                  );
  if (EFI_ERROR (Status)) {
    Status = EFI_OUT_OF_RESOURCES;
    goto ErrorExit;
  }

  MouseDev->ControllerNameTable = NULL;
  AddUnicodeString2 (
    "eng",
    gPs2MouseComponentName.SupportedLanguages,
    &MouseDev->ControllerNameTable,
    L"PS/2 Mouse Device",
    TRUE
    );
  AddUnicodeString2 (
    "en",
    gPs2MouseComponentName2.SupportedLanguages,
    &MouseDev->ControllerNameTable,
    L"PS/2 Mouse Device",
    FALSE
    );


  //
  // Install protocol interfaces for the mouse device.
  //
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &Controller,
                  &gEfiSimplePointerProtocolGuid,
                  &MouseDev->SimplePointerProtocol,
                  NULL
                  );
  if (EFI_ERROR (Status)) {
    goto ErrorExit;
  }

  gBS->RestoreTPL (OldTpl);

  return Status;

ErrorExit:

  if (Status != EFI_DEVICE_ERROR) {
    KbcDisableAux (IsaIo);
  }

  if (StatusCode != 0) {
    REPORT_STATUS_CODE_WITH_DEVICE_PATH (
      EFI_ERROR_CODE | EFI_ERROR_MINOR,
      StatusCode,
      ParentDevicePath
      );
  }

  if ((MouseDev != NULL) && (MouseDev->SimplePointerProtocol.WaitForInput != NULL)) {
    gBS->CloseEvent (MouseDev->SimplePointerProtocol.WaitForInput);
  }

  if ((MouseDev != NULL) && (MouseDev->TimerEvent != NULL)) {
    gBS->CloseEvent (MouseDev->TimerEvent);
  }

  if ((MouseDev != NULL) && (MouseDev->ControllerNameTable != NULL)) {
    FreeUnicodeStringTable (MouseDev->ControllerNameTable);
  }

  if (Status != EFI_DEVICE_ERROR) {
    //
    // Since there will be no timer handler for mouse input any more,
    // exhaust input data just in case there is still mouse data left
    //
    EmptyStatus = EFI_SUCCESS;

    //
    // KBC need enough time to check output buffer queue and then set OBF flag.
    //
    gBS->Stall (5000);
    CleanupOutputBuffer (IsaIo);
  }

  if (MouseDev != NULL) {
    FreePool (MouseDev);
  }

  gBS->CloseProtocol (
         Controller,
         &gEfiDevicePathProtocolGuid,
         This->DriverBindingHandle,
         Controller
         );

  gBS->CloseProtocol (
         Controller,
         &gEfiIsaIoProtocolGuid,
         This->DriverBindingHandle,
         Controller
         );

  gBS->RestoreTPL (OldTpl);

  return Status;
}

/**
  Stop this driver on ControllerHandle. Support stoping any child handles
  created by this driver.

  @param  This              Protocol instance pointer.
  @param  ControllerHandle  Handle of device to stop driver on
  @param  NumberOfChildren  Number of Handles in ChildHandleBuffer. If number of
                            children is zero stop the entire bus driver.
  @param  ChildHandleBuffer List of Child Handles to Stop.

  @retval EFI_SUCCESS       This driver is removed ControllerHandle
  @retval other             This driver was not removed from this device

**/
EFI_STATUS
EFIAPI
PS2MouseDriverStop (
  IN EFI_DRIVER_BINDING_PROTOCOL    *This,
  IN EFI_HANDLE                     Controller,
  IN UINTN                          NumberOfChildren,
  IN EFI_HANDLE                     *ChildHandleBuffer
  )
{
  EFI_STATUS                  Status;
  EFI_SIMPLE_POINTER_PROTOCOL *SimplePointerProtocol;
  PS2_MOUSE_DEV               *MouseDev;

  Status = gBS->OpenProtocol (
                  Controller,
                  &gEfiSimplePointerProtocolGuid,
                  (VOID **) &SimplePointerProtocol,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_GET_PROTOCOL
                  );
  if (EFI_ERROR (Status)) {
    return EFI_SUCCESS;
  }

  MouseDev = PS2_MOUSE_DEV_FROM_THIS (SimplePointerProtocol);

  //
  // Report that the keyboard is being disabled
  //
  REPORT_STATUS_CODE_WITH_DEVICE_PATH (
    EFI_PROGRESS_CODE,
    EFI_PERIPHERAL_MOUSE | EFI_P_PC_DISABLE,
    MouseDev->DevicePath
    );

  Status = gBS->UninstallProtocolInterface (
                  Controller,
                  &gEfiSimplePointerProtocolGuid,
                  &MouseDev->SimplePointerProtocol
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  //
  // Disable mouse on keyboard controller
  //
  KbcDisableAux (MouseDev->IsaIo);

  //
  // Cancel mouse data polling timer, close timer event
  //
  gBS->SetTimer (MouseDev->TimerEvent, TimerCancel, 0);
  gBS->CloseEvent (MouseDev->TimerEvent);

  //
  // Since there will be no timer handler for mouse input any more,
  // exhaust input data just in case there is still mouse data left
  //
  gBS->Stall (5000);
  CleanupOutputBuffer (MouseDev->IsaIo);

  gBS->CloseEvent (MouseDev->SimplePointerProtocol.WaitForInput);
  FreeUnicodeStringTable (MouseDev->ControllerNameTable);
  FreePool (MouseDev);

  gBS->CloseProtocol (
         Controller,
         &gEfiDevicePathProtocolGuid,
         This->DriverBindingHandle,
         Controller
         );

  gBS->CloseProtocol (
         Controller,
         &gEfiIsaIoProtocolGuid,
         This->DriverBindingHandle,
         Controller
         );

  return EFI_SUCCESS;
}

/**
  Reset the Mouse and do BAT test for it, if ExtendedVerification is TRUE and
  there is a mouse device connected to system.

  @param This                 - Pointer of simple pointer Protocol.
  @param ExtendedVerification - Whether configure mouse parameters. True: do; FALSE: skip.


  @retval EFI_SUCCESS         - The command byte is written successfully.
  @retval EFI_DEVICE_ERROR    - Errors occurred during reseting keyboard.

**/
EFI_STATUS
EFIAPI
MouseReset (
  IN EFI_SIMPLE_POINTER_PROTOCOL    *This,
  IN BOOLEAN                        ExtendedVerification
  )
{
  EFI_STATUS    Status;
  PS2_MOUSE_DEV *MouseDev;
  EFI_TPL       OldTpl;
  BOOLEAN       KeyboardEnable;
  UINT8         Data;

  MouseDev = PS2_MOUSE_DEV_FROM_THIS (This);

  //
  // Report reset progress code
  //
  REPORT_STATUS_CODE_WITH_DEVICE_PATH (
    EFI_PROGRESS_CODE,
    EFI_PERIPHERAL_MOUSE | EFI_P_PC_RESET,
    MouseDev->DevicePath
    );

  KeyboardEnable = FALSE;

  //
  // Raise TPL to avoid keyboard operation impact
  //
  OldTpl = gBS->RaiseTPL (TPL_NOTIFY);

  ZeroMem (&MouseDev->State, sizeof (EFI_SIMPLE_POINTER_STATE));
  MouseDev->StateChanged = FALSE;

  CheckKbStatus (MouseDev->IsaIo, &KeyboardEnable);
  KbcDisableKb (MouseDev->IsaIo);
  KbcDisableAux (MouseDev->IsaIo);

  //
  // KBC need enough time to check output buffer queue and then set OBF flag.
  //
  gBS->Stall (5000);

  //
  // Exhaust output buffer data
  //
  CleanupOutputBuffer (MouseDev->IsaIo);
  Status = EFI_SUCCESS;

  //
  // The PS2 mouse driver reset behavior is always successfully return no matter wheater or not there is mouse connected to system.
  // This behavior is needed by performance speed. The following mouse command only succeessfully finish when mouse device is
  // connected to system, so if PS2 mouse device not connect to system or user not ask for, we skip the mouse configuration and enabling
  //
  if ((MouseDev->TimerEvent == NULL || ExtendedVerification) && CheckMouseConnect (MouseDev)) {
    //
    // Send mouse reset command and set mouse default configure
    //
    Status = PS2MouseReset (MouseDev->IsaIo);
    if (EFI_ERROR (Status)) {
      Status = EFI_DEVICE_ERROR;
      goto Exit;
    }
    //
    // command sequence to test DataPackageSize
    //
    Status = PS2MouseSetSampleRate (MouseDev->IsaIo, SampleRate200);
    if (EFI_ERROR (Status)) {
      Status = EFI_DEVICE_ERROR;
      goto Exit;
    }

    Status = PS2MouseSetSampleRate (MouseDev->IsaIo, SampleRate100);
    if (EFI_ERROR (Status)) {
      Status = EFI_DEVICE_ERROR;
      goto Exit;
    }

    Status = PS2MouseSetSampleRate (MouseDev->IsaIo, SampleRate80);
    if (EFI_ERROR (Status)) {
      Status = EFI_DEVICE_ERROR;
      goto Exit;
    }

    Status = PS2MouseGetDeviceId (MouseDev->IsaIo, &Data);
    if (EFI_ERROR (Status)) {
      Status = EFI_DEVICE_ERROR;
      goto Exit;
    }
    MouseDev->DataPackageSize = Data;

    Status = PS2MouseSetResolution (MouseDev->IsaIo, MouseDev->Resolution);
    if (EFI_ERROR (Status)) {
      Status = EFI_DEVICE_ERROR;
      goto Exit;
    }

    Status = PS2MouseSetScaling (MouseDev->IsaIo, MouseDev->Scaling);
    if (EFI_ERROR (Status)) {
      Status = EFI_DEVICE_ERROR;
      goto Exit;
    }

    Status = PS2MouseSetSampleRate (MouseDev->IsaIo, MouseDev->SampleRate);
    if (EFI_ERROR (Status)) {
      Status = EFI_DEVICE_ERROR;
      goto Exit;
    }

    Status = PS2MouseEnable (MouseDev->IsaIo);
    if (EFI_ERROR (Status)) {
      Status = EFI_DEVICE_ERROR;
      goto Exit;
    }
  }

  KbcEnableAux (MouseDev->IsaIo);

Exit:

  if ((MouseDev->TimerEvent == NULL) && !EFI_ERROR (Status))  {

    //
    // Setup a periodic timer, used to poll mouse state
    //
    Status = gBS->CreateEvent (
                    EVT_TIMER | EVT_NOTIFY_SIGNAL,
                    TPL_NOTIFY,
                    PollMouse,
                    MouseDev,
                    &MouseDev->TimerEvent
                    );
    if (!EFI_ERROR (Status)) {
      //
      // Start timer to poll mouse (100 samples per second)
      //
      Status = gBS->SetTimer (MouseDev->TimerEvent, TimerPeriodic, 100000);
    }
  }

  if (KeyboardEnable) {
    KbcEnableKb (MouseDev->IsaIo);
  }

  gBS->RestoreTPL (OldTpl);

  return Status;
}

/**
  Check whether there is Ps/2 mouse device in system

  @param MouseDev   - Mouse Private Data Structure

  @retval TRUE      - Keyboard in System.
  @retval FALSE     - Keyboard not in System.

**/
BOOLEAN
CheckMouseConnect (
  IN  PS2_MOUSE_DEV     *MouseDev
  )

{
  EFI_STATUS     Status;

  Status = PS2MouseEnable (MouseDev->IsaIo);
  if (!EFI_ERROR (Status)) {
    return TRUE;
  }

  return FALSE;
}

/**
  Get and Clear mouse status.

  @param This                 - Pointer of simple pointer Protocol.
  @param State                - Output buffer holding status.

  @retval EFI_INVALID_PARAMETER Output buffer is invalid.
  @retval EFI_NOT_READY         Mouse is not changed status yet.
  @retval EFI_SUCCESS           Mouse status is changed and get successful.
**/
EFI_STATUS
EFIAPI
MouseGetState (
  IN EFI_SIMPLE_POINTER_PROTOCOL    *This,
  IN OUT EFI_SIMPLE_POINTER_STATE   *State
  )
{
  PS2_MOUSE_DEV *MouseDev;
  EFI_TPL       OldTpl;
  EFI_STATUS    Status;

  MouseDev = PS2_MOUSE_DEV_FROM_THIS (This);
  //
  // In order to reduce post time, move Ps2Mouse reset code from DriverStart function
  // to first time system want to get mouse state.
  //
  if (MouseDev->TimerEvent == NULL) {
    Status = MouseDev->SimplePointerProtocol.Reset (
                         &MouseDev->SimplePointerProtocol,
                         TRUE
                         );
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }

  if (State == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (!MouseDev->StateChanged) {
    return EFI_NOT_READY;
  }

  OldTpl = gBS->RaiseTPL (TPL_NOTIFY);
  CopyMem (State, &(MouseDev->State), sizeof (EFI_SIMPLE_POINTER_STATE));

  //
  // clear mouse state
  //
  MouseDev->State.RelativeMovementX = 0;
  MouseDev->State.RelativeMovementY = 0;
  MouseDev->State.RelativeMovementZ = 0;
  MouseDev->StateChanged            = FALSE;
  gBS->RestoreTPL (OldTpl);

  return EFI_SUCCESS;
}

/**

  Event notification function for SIMPLE_POINTER.WaitForInput event.
  Signal the event if there is input from mouse.

  @param Event    event object
  @param Context  event context

**/
VOID
EFIAPI
MouseWaitForInput (
  IN  EFI_EVENT               Event,
  IN  VOID                    *Context
  )
{
  PS2_MOUSE_DEV *MouseDev;

  MouseDev = (PS2_MOUSE_DEV *) Context;

  //
  // In order to reduce post time, move Ps2Mouse reset code from DriverStart function
  // to first time system want to get mouse state.
  //
  if (MouseDev->TimerEvent == NULL) {
    MouseDev->SimplePointerProtocol.Reset (
                &MouseDev->SimplePointerProtocol,
                TRUE
                );
  }

  //
  // Someone is waiting on the mouse event, if there's
  // input from mouse, signal the event
  //
  if (MouseDev->StateChanged) {
    gBS->SignalEvent (Event);
  }

}

/**
  Event notification function for TimerEvent event.
  If mouse device is connected to system, try to get the mouse packet data.

  @param Event      -  TimerEvent in PS2_MOUSE_DEV
  @param Context    -  Pointer to PS2_MOUSE_DEV structure

**/
VOID
EFIAPI
PollMouse (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  )

{
  PS2_MOUSE_DEV *MouseDev;

  MouseDev = (PS2_MOUSE_DEV *) Context;

  //
  // Polling mouse packet data
  //
  PS2MouseGetPacket (MouseDev);
}

/**
  The user Entry Point for module Ps2Mouse. The user code starts with this function.

  @param[in] ImageHandle    The firmware allocated handle for the EFI image.
  @param[in] SystemTable    A pointer to the EFI System Table.

  @retval EFI_SUCCESS       The entry point is executed successfully.
  @retval other             Some error occurs when executing this entry point.

**/
EFI_STATUS
EFIAPI
InitializePs2Mouse(
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  )
{
  EFI_STATUS              Status;

  //
  // Install driver model protocol(s).
  //
  Status = EfiLibInstallDriverBindingComponentName2 (
             ImageHandle,
             SystemTable,
             &gPS2MouseDriver,
             ImageHandle,
             &gPs2MouseComponentName,
             &gPs2MouseComponentName2
             );
  ASSERT_EFI_ERROR (Status);


  return Status;
}

