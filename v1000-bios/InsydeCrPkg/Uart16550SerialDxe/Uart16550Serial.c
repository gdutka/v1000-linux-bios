/** @file
    Serial driver for standard UARTS base on an UART16550Access Protocol.

;******************************************************************************
;* Copyright (c) 2015-2017, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "Uart16550Serial.h"
//
// Uart 16550 Driver Global Variables
//
EFI_DRIVER_BINDING_PROTOCOL gUart16550Driver = {
  Uart16550DriverSupported,
  Uart16550DriverStart,
  Uart16550DriverStop,
  0xa,
  NULL,
  NULL
};


SERIAL_DEV  gSerialDevTempate = {
  SERIAL_DEV_SIGNATURE,
  NULL,
  { // SerialIo
    SERIAL_IO_INTERFACE_REVISION,
    Uart16550Reset,
    Uart16550SetAttributes,
    Uart16550SetControl,
    Uart16550GetControl,
    Uart16550Write,
    Uart16550Read,
    NULL
  },
  { // SerialMode
    SERIAL_PORT_SUPPORT_CONTROL_MASK,
    SERIAL_PORT_DEFAULT_TIMEOUT,
    FixedPcdGet64 (PcdUartDefaultBaudRate),     // BaudRate
    SERIAL_PORT_DEFAULT_RECEIVE_FIFO_DEPTH,
    FixedPcdGet8 (PcdUartDefaultDataBits),      // DataBits
    FixedPcdGet8 (PcdUartDefaultParity),        // Parity
    FixedPcdGet8 (PcdUartDefaultStopBits)       // StopBits
  },
  NULL,
  NULL,
  { // UartDevicePath
    {
      MESSAGING_DEVICE_PATH,
      MSG_UART_DP,
      {
        (UINT8) (sizeof (UART_DEVICE_PATH)),
        (UINT8) ((sizeof (UART_DEVICE_PATH)) >> 8)
      }
    },
    0,    // Reserver
    FixedPcdGet64 (PcdUartDefaultBaudRate),
    FixedPcdGet8 (PcdUartDefaultDataBits),
    FixedPcdGet8 (PcdUartDefaultParity),
    FixedPcdGet8 (PcdUartDefaultStopBits)
  },
  NULL,   //Uart16550
  0,      //BaseAddress
  {  // ReceiveFIFO
    0,
    0,
    SERIAL_MAX_BUFFER_SIZE,
    { 0 }
  },
  {  //TransmitFIFO
    0,
    0,
    SERIAL_MAX_BUFFER_SIZE,
    { 0 }
  },
  FALSE,       //SoftwareLoopbackEnable
  FALSE,       //HardwareFlowControl
  FALSE,       //SoftwareFlowControl
  0,           //UartUid
  0,           //HostFlowCtrlState
  0,           //RemoteFlowCtrlState
  UART16550A,  //Uart Type
  NULL         //ControllerNameTable
};

EFI_CONSOLE_REDIRECTION_OPROM_INFO_PROTOCOL  *mCrOpROMInfoProtocol = NULL;
BOOLEAN                                      mLineBreakToCtrlC = FALSE;

VOID
SetSwFcState (
  IN SERIAL_DEV *SerialDevice,
  IN UINT8      SwFcState,
  IN BOOLEAN    IsHost
  )
{
  CR_COMMON_AREA_INFO    *CrOpromInfo;

  if(mCrOpROMInfoProtocol == NULL) {
    if (IsHost) {
      SerialDevice->HostFlowCtrlState   = SwFcState;
    } else {
      SerialDevice->RemoteFlowCtrlState = SwFcState;
    }
    return;
  }

  CrOpromInfo = NULL;
  CrOpromInfo = mCrOpROMInfoProtocol->CrGetCommonArea (mCrOpROMInfoProtocol);
  if (CrOpromInfo != NULL) {
    if (IsHost) {
      CrOpromInfo->FlowState[SerialDevice->UartUid].Host = SwFcState;
    } else {
      CrOpromInfo->FlowState[SerialDevice->UartUid].Remote = SwFcState;
    }
  }
  return;
}


UINT8
GetSwFcState (
  IN SERIAL_DEV *SerialDevice,
  IN BOOLEAN    IsHost
  )
{
  CR_COMMON_AREA_INFO    *CrOpromInfo;

  if(mCrOpROMInfoProtocol == NULL) {
    return (IsHost ? SerialDevice->HostFlowCtrlState : SerialDevice->RemoteFlowCtrlState);
  }

  CrOpromInfo = NULL;
  CrOpromInfo = mCrOpROMInfoProtocol->CrGetCommonArea (mCrOpROMInfoProtocol);
  if (CrOpromInfo != NULL) {
    if (IsHost) {
      return CrOpromInfo->FlowState[SerialDevice->UartUid].Host;
    } else {
      return CrOpromInfo->FlowState[SerialDevice->UartUid].Remote;
    }
  }

  return (IsHost ? SerialDevice->HostFlowCtrlState : SerialDevice->RemoteFlowCtrlState);
}

VOID
EFIAPI
CrOpromInstallNotifyFunction (
  IN EFI_EVENT    Event,
  IN VOID         *Context
  )
{
  EFI_STATUS           Status;
  CR_COMMON_AREA_INFO  *CrOpromInfo;
  SERIAL_DEV           *SerialDev;

  Status = gBS->LocateProtocol (&gH2OCrOpRomInfoProtocolGuid, NULL, (VOID **)&mCrOpROMInfoProtocol);
  if (EFI_ERROR (Status)) {
    return;
  }
  SerialDev = (SERIAL_DEV*)Context;
  SerialDev->UartUid = mCrOpROMInfoProtocol->CrGetUartUid (mCrOpROMInfoProtocol);
  SerialDev->Uart16550->DeviceInfo->UartUid = SerialDev->UartUid;

  if (SerialDev->SoftwareFlowControl) {
    CrOpromInfo = NULL;
    CrOpromInfo = mCrOpROMInfoProtocol->CrGetCommonArea (mCrOpROMInfoProtocol);
    if (CrOpromInfo == NULL) {
      return;
    }
    CrOpromInfo->FlowState[SerialDev->UartUid].Host   = SerialDev->HostFlowCtrlState;
    CrOpromInfo->FlowState[SerialDev->UartUid].Remote = SerialDev->RemoteFlowCtrlState;
  }
  return;
}

/**
  Change SW FlowControl statement

  @param SerialDevice  The Pointer that point to SERIAL_DEV structure
  @param FcData        Output to remote terminal to change SW flowcontrol statement

**/
VOID
SetSoftFlowControl (
  IN SERIAL_DEV              *SerialDevice,
  IN UINT8                   FcData
  )
{
  if (SerialDevice->SoftwareFlowControl) {
    SetSwFcState (SerialDevice, FcData, HOST_SEND);
    WRITE_THR (SerialDevice->Uart16550, FcData);
  }
}

BOOLEAN
IsValidUartNode (
  IN UART_DEVICE_PATH   *UartNode
  )
{
  if (UartNode->Header.Type != MESSAGING_DEVICE_PATH ||
      UartNode->Header.SubType != MSG_UART_DP ||
      sizeof (UART_DEVICE_PATH) != DevicePathNodeLength ((EFI_DEVICE_PATH_PROTOCOL *) UartNode)) {
    return FALSE;
  }

  if (UartNode->BaudRate > SERIAL_PORT_MAX_BAUD_RATE) {
    return FALSE;
  }

  if (UartNode->Parity < NoParity || UartNode->Parity > SpaceParity) {
    return FALSE;
  }

  if (UartNode->DataBits < 5 || UartNode->DataBits > 8) {
    return FALSE;
  }

  if (UartNode->StopBits < OneStopBit || UartNode->StopBits > TwoStopBits) {
    return FALSE;
  }

  if ((UartNode->DataBits == 5) && (UartNode->StopBits == TwoStopBits)) {
    return FALSE;
  }

  if ((UartNode->DataBits >= 6) && (UartNode->DataBits <= 8) && (UartNode->StopBits == OneFiveStopBits)) {
    return FALSE;
  }

  return TRUE;
}

/**
  Check the device path node whether it's the Flow Control node or not.

  @param[in] FlowControl    The device path node to be checked.

  @retval TRUE              It's the Flow Control node.
  @retval FALSE             It's not.

**/
BOOLEAN
IsUartFlowControlNode (
  IN UART_FLOW_CONTROL_DEVICE_PATH *FlowControl
  )
{
  return (BOOLEAN) (
           (DevicePathType (FlowControl) == MESSAGING_DEVICE_PATH) &&
           (DevicePathSubType (FlowControl) == MSG_VENDOR_DP) &&
           (CompareGuid (&FlowControl->Guid, &gEfiUartDevicePathGuid))
           );
}

/**
  Check the device path node whether it contains Flow Control node or not.

  @param[in] DevicePath     The device path to be checked.

  @retval TRUE              It contains the Flow Control node.
  @retval FALSE             It doesn't.

**/
BOOLEAN
ContainsFlowControl (
  IN EFI_DEVICE_PATH_PROTOCOL      *DevicePath
  )
{
  while (!IsDevicePathEnd (DevicePath)) {
    if (IsUartFlowControlNode ((UART_FLOW_CONTROL_DEVICE_PATH *) DevicePath)) {
      return TRUE;
    }
    DevicePath = NextDevicePathNode (DevicePath);
  }

  return FALSE;
}

/**
  Use scratchpad register to test if this serial port is present.

  @param SerialDevice   Pointer to 16550 serial device structure

  @return if this serial port is present
**/
BOOLEAN
SerialPortPresent (
  IN SERIAL_DEV *SerialDevice
  )
{

  UINT8           Temp;
  BOOLEAN         Status;

  Status = TRUE;

  //
  // Save SCR reg
  //
  Temp = READ_SCR (SerialDevice->Uart16550);
  WRITE_SCR (SerialDevice->Uart16550, 0xAA);

  if (READ_SCR (SerialDevice->Uart16550) != 0xAA) {
    Status = FALSE;
  }

  WRITE_SCR (SerialDevice->Uart16550, 0x55);

  if (READ_SCR (SerialDevice->Uart16550) != 0x55) {
    Status = FALSE;
  }
  //
  // Restore SCR
  //
  WRITE_SCR (SerialDevice->Uart16550, Temp);

  return Status;
}

/**
  The user Entry Point for module Uart16550Serial. The user code starts with this function.

  @param[in] ImageHandle    The firmware allocated handle for the EFI image.
  @param[in] SystemTable    A pointer to the EFI System Table.

  @retval EFI_SUCCESS       The entry point is executed successfully.
  @retval other             Some error occurs when executing this entry point.

**/
EFI_STATUS
EFIAPI
Uart16550SerialDriverEntry (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  )
{
  EFI_STATUS              Status;

  mLineBreakToCtrlC = PcdGetBool (PcdH2OCrUart16550aSerialLineBreakToCtrlCSupported);

  //
  // Install driver model protocol(s).
  //
  Status = EfiLibInstallDriverBindingComponentName2 (
             ImageHandle,
             SystemTable,
             &gUart16550Driver,
             ImageHandle,
             &gUart16550ComponentName,
             &gUart16550ComponentName2
             );

  ASSERT_EFI_ERROR (Status);

  return Status;
}

/**
  Check to see if this driver supports the given controller

  @param  This                 A pointer to the EFI_DRIVER_BINDING_PROTOCOL instance.
  @param  Controller           The handle of the controller to test.
  @param  RemainingDevicePath  A pointer to the remaining portion of a device path.

  @return EFI_SUCCESS          This driver can support the given controller

**/
EFI_STATUS
EFIAPI
Uart16550DriverSupported (
  IN EFI_DRIVER_BINDING_PROTOCOL    *This,
  IN EFI_HANDLE                     Controller,
  IN EFI_DEVICE_PATH_PROTOCOL       *RemainingDevicePath
  )
{
  EFI_STATUS                                Status;
  EFI_STATUS                                RetStatus;
  EFI_DEVICE_PATH_PROTOCOL                  *ParentDevicePath;
  H2O_UART_16550_ACCESS_PROTOCOL            *Uart16550;
  UART_DEVICE_PATH                          *UartNode;
  EFI_DEVICE_PATH_PROTOCOL                  *DevicePath;
  UART_FLOW_CONTROL_DEVICE_PATH             *FlowControlNode;
  EFI_OPEN_PROTOCOL_INFORMATION_ENTRY       *OpenInfoBuffer;
  UINTN                                     EntryCount;
  UINTN                                     Index;
  BOOLEAN                                   HasFlowControl;

  //
  // Check RemainingDevicePath validation
  //
  if ((RemainingDevicePath != NULL) && (!IsDevicePathEnd (RemainingDevicePath))) {
    //
    // If RemainingDevicePath isn't the End of Device Path Node,
    // check its validation
    //
    UartNode = (UART_DEVICE_PATH *) RemainingDevicePath;

    if (!IsValidUartNode (UartNode)) {
      return EFI_UNSUPPORTED;
    }

    FlowControlNode = (UART_FLOW_CONTROL_DEVICE_PATH *) NextDevicePathNode (UartNode);
    if (IsUartFlowControlNode (FlowControlNode)) {
      //
      // If the second node is Flow Control Node,
      //   return error when it request other than hardware flow control.
      //
      if (((ReadUnaligned32 (&FlowControlNode->FlowControlMap) & ~FLOW_CONTROL_HARDWARE) != 0) &&
          ((ReadUnaligned32 (&FlowControlNode->FlowControlMap) & ~FLOW_CONTROL_XON_XOFF) != 0)) {
        return EFI_UNSUPPORTED;
      }
    }
  }

  //
  // Open the IO Abstraction(s) needed to perform the supported test
  //
  Status = gBS->OpenProtocol (
                  Controller,
                  &gH2OCrUart16550AccessProtocolGuid,
                  (VOID **) &Uart16550,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_BY_DRIVER
                  );
  if (Status == EFI_ALREADY_STARTED) {
    if (RemainingDevicePath == NULL || IsDevicePathEnd (RemainingDevicePath)) {
      //
      // If RemainingDevicePath is NULL or is the End of Device Path Node
      //
      return EFI_ALREADY_STARTED;
    }
    //
    // When the driver has produced device path with flow control node but RemainingDevicePath only contains UART node,
    //   return unsupported, and vice versa.
    //
    Status = gBS->OpenProtocolInformation (
                    Controller,
                    &gH2OCrUart16550AccessProtocolGuid,
                    &OpenInfoBuffer,
                    &EntryCount
                    );
    if (EFI_ERROR (Status)) {
      return Status;
    }

    RetStatus = EFI_UNSUPPORTED;
    for (Index = 0; Index < EntryCount; Index++) {
      if ((OpenInfoBuffer[Index].AgentHandle == This->DriverBindingHandle) &&
          (OpenInfoBuffer[Index].Attributes & EFI_OPEN_PROTOCOL_BY_CHILD_CONTROLLER) != 0) {
        Status = gBS->OpenProtocol (
                        OpenInfoBuffer[Index].ControllerHandle,
                        &gEfiDevicePathProtocolGuid,
                        (VOID **) &DevicePath,
                        This->DriverBindingHandle,
                        Controller,
                        EFI_OPEN_PROTOCOL_GET_PROTOCOL
                        );
        if (!EFI_ERROR (Status)) {
          HasFlowControl = ContainsFlowControl (RemainingDevicePath);
          if (!(HasFlowControl ^ ContainsFlowControl (DevicePath))) {
            RetStatus = EFI_SUCCESS;
            break;
          }
        }
      }
    }
    FreePool (OpenInfoBuffer);
    return RetStatus;
  }

  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Close the I/O Abstraction(s) used to perform the supported test
  //
  gBS->CloseProtocol (
         Controller,
         &gH2OCrUart16550AccessProtocolGuid,
         This->DriverBindingHandle,
         Controller
         );

  //
  // Open the EFI Device Path protocol needed to perform the supported test
  //
  Status = gBS->OpenProtocol (
                  Controller,
                  &gEfiDevicePathProtocolGuid,
                  (VOID **) &ParentDevicePath,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_BY_DRIVER
                  );

  if (Status == EFI_ALREADY_STARTED) {
    return EFI_SUCCESS;
  }

  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Close protocol, don't use device path protocol in the Support() function
  //
  gBS->CloseProtocol (
         Controller,
         &gEfiDevicePathProtocolGuid,
         This->DriverBindingHandle,
         Controller
         );
  return Status;
}

/**
  Start to management the controller passed in

  @param  This                 A pointer to the EFI_DRIVER_BINDING_PROTOCOL instance.
  @param  Controller           The handle of the controller to test.
  @param  RemainingDevicePath  A pointer to the remaining portion of a device path.

  @return EFI_SUCCESS   Driver is started successfully

**/
EFI_STATUS
EFIAPI
Uart16550DriverStart (
  IN EFI_DRIVER_BINDING_PROTOCOL    *This,
  IN EFI_HANDLE                     Controller,
  IN EFI_DEVICE_PATH_PROTOCOL       *RemainingDevicePath
  )
{
  EFI_STATUS                          Status;
  EFI_STATUS                          RetStatus;
  H2O_UART_16550_ACCESS_PROTOCOL      *Uart16550;
  SERIAL_DEV                          *SerialDevice;
  UINTN                               Index;
  EFI_DEVICE_PATH_PROTOCOL            *ParentDevicePath;
  EFI_OPEN_PROTOCOL_INFORMATION_ENTRY *OpenInfoBuffer;
  UINTN                               EntryCount;
  EFI_SERIAL_IO_PROTOCOL              *SerialIo;
  UART_DEVICE_PATH                    *Uart;
  UINT32                              FlowControlMap;
  UART_FLOW_CONTROL_DEVICE_PATH       *FlowControl;
  EFI_DEVICE_PATH_PROTOCOL            *TempDevicePath;
  UINT32                              Control;
  EFI_EVENT                           CrOpromInstallNotifyEvent;
  VOID                                *CrOpromInstallRegistration;

  CrOpromInstallNotifyEvent = NULL;
  SerialDevice = NULL;
  //
  // Get the Parent Device Path
  //
  Status = gBS->OpenProtocol (
                  Controller,
                  &gEfiDevicePathProtocolGuid,
                  (VOID **) &ParentDevicePath,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_BY_DRIVER
                  );
  if (EFI_ERROR (Status) && Status != EFI_ALREADY_STARTED) {
    return Status;
  }
  //
  // Report status code enable the serial
  //
  REPORT_STATUS_CODE_WITH_DEVICE_PATH (
    EFI_PROGRESS_CODE,
    EFI_P_PC_ENABLE | EFI_PERIPHERAL_SERIAL_PORT,
    ParentDevicePath
    );

  //
  // Grab the IO abstraction we need to get any work done
  //
  Status = gBS->OpenProtocol (
                  Controller,
                  &gH2OCrUart16550AccessProtocolGuid,
                  (VOID **) &Uart16550,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_BY_DRIVER
                  );
  if (EFI_ERROR (Status) && Status != EFI_ALREADY_STARTED) {
    goto Error;
  }

  if (Status == EFI_ALREADY_STARTED) {

    if (RemainingDevicePath == NULL || IsDevicePathEnd (RemainingDevicePath)) {
      //
      // If RemainingDevicePath is NULL or is the End of Device Path Node
      //
      return EFI_SUCCESS;
    }

    //
    // Make sure a child handle does not already exist.  This driver can only
    // produce one child per serial port.
    //
    Status = gBS->OpenProtocolInformation (
                    Controller,
                    &gH2OCrUart16550AccessProtocolGuid,
                    &OpenInfoBuffer,
                    &EntryCount
                    );
    if (EFI_ERROR (Status)) {
      return Status;
    }

    RetStatus = EFI_UNSUPPORTED;
    for (Index = 0; Index < EntryCount; Index++) {
      if ((OpenInfoBuffer[Index].AgentHandle == This->DriverBindingHandle) &&
          (OpenInfoBuffer[Index].Attributes & EFI_OPEN_PROTOCOL_BY_CHILD_CONTROLLER) != 0) {
        Status = gBS->OpenProtocol (
                        OpenInfoBuffer[Index].ControllerHandle,
                        &gEfiSerialIoProtocolGuid,
                        (VOID **) &SerialIo,
                        This->DriverBindingHandle,
                        Controller,
                        EFI_OPEN_PROTOCOL_GET_PROTOCOL
                        );
        if (!EFI_ERROR (Status)) {
          Uart   = (UART_DEVICE_PATH *) RemainingDevicePath;
          SerialDevice = SERIAL_DEV_FROM_THIS (SerialIo);

          Status = SerialIo->SetAttributes (
                               SerialIo,
                               Uart->BaudRate,
                               SerialIo->Mode->ReceiveFifoDepth,
                               SerialIo->Mode->Timeout,
                               (EFI_PARITY_TYPE) Uart->Parity,
                               Uart->DataBits,
                               (EFI_STOP_BITS_TYPE) Uart->StopBits
                               );

          FlowControl = (UART_FLOW_CONTROL_DEVICE_PATH *) NextDevicePathNode (Uart);
          if (!EFI_ERROR (Status) && IsUartFlowControlNode (FlowControl)) {
            Status = SerialIo->GetControl (SerialIo, &Control);
            if (!EFI_ERROR (Status)) {
              if (ReadUnaligned32 (&FlowControl->FlowControlMap) == FLOW_CONTROL_HARDWARE) {
                Control |= EFI_SERIAL_HARDWARE_FLOW_CONTROL_ENABLE;
              } else {
                Control &= ~EFI_SERIAL_HARDWARE_FLOW_CONTROL_ENABLE;
              }
              //
              // Clear the bits that are not allowed to pass to SetControl
              //
              Control &= (EFI_SERIAL_REQUEST_TO_SEND | EFI_SERIAL_DATA_TERMINAL_READY |
                          EFI_SERIAL_HARDWARE_LOOPBACK_ENABLE | EFI_SERIAL_SOFTWARE_LOOPBACK_ENABLE |
                          EFI_SERIAL_HARDWARE_FLOW_CONTROL_ENABLE);
              Status = SerialIo->SetControl (SerialIo, Control);
            }
          }
          RetStatus = EFI_SUCCESS;
          break;
        }
      }
    }

    FreePool (OpenInfoBuffer);
    return RetStatus;
  }

  if (RemainingDevicePath != NULL) {
    if (IsDevicePathEnd (RemainingDevicePath)) {
      //
      // If RemainingDevicePath is the End of Device Path Node,
      // skip enumerate any device and return EFI_SUCESSS
      //
      return EFI_SUCCESS;
    }
  }

  //
  // Initialize the serial device instance
  //
  SerialDevice = AllocateCopyPool (sizeof (SERIAL_DEV), &gSerialDevTempate);
  if (SerialDevice == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto Error;
  }

  SerialDevice->SerialIo.Mode       = &(SerialDevice->SerialMode);
  SerialDevice->Uart16550           = Uart16550;
  SerialDevice->ParentDevicePath    = ParentDevicePath;
  SerialDevice->Type                = (Uart16550->DeviceInfo->FifoSize == 1)? UART16550 : UART16550A;
  FlowControl                       = NULL;
  FlowControlMap                    = 0;

  //
  // Check if RemainingDevicePath is NULL,
  // if yes, use the values from the gSerialDevTempate as no remaining device path was
  // passed in.
  //
  if (RemainingDevicePath != NULL) {
    //
    // If RemainingDevicePath isn't NULL,
    // match the configuration of the RemainingDevicePath. IsHandleSupported()
    // already checked to make sure the RemainingDevicePath contains settings
    // that we can support.
    //
    CopyMem (&SerialDevice->UartDevicePath, RemainingDevicePath, sizeof (UART_DEVICE_PATH));
    FlowControl = (UART_FLOW_CONTROL_DEVICE_PATH *) NextDevicePathNode (RemainingDevicePath);
    if (IsUartFlowControlNode (FlowControl)) {
      FlowControlMap = ReadUnaligned32 (&FlowControl->FlowControlMap);
    } else {
      FlowControl    = NULL;
    }
  }

  AddName (SerialDevice, Uart16550);

  SerialDevice->HardwareFlowControl = (BOOLEAN) (FlowControlMap == FLOW_CONTROL_HARDWARE);
  SerialDevice->SoftwareFlowControl = (BOOLEAN) (FlowControlMap == FLOW_CONTROL_XON_XOFF);

  if (SerialDevice->SoftwareFlowControl) {
    SerialDevice->HostFlowCtrlState   = XON;
    SerialDevice->RemoteFlowCtrlState = XON;
  }

  Status = gBS->CreateEvent (
                  EVT_NOTIFY_SIGNAL,
                  TPL_NOTIFY,
                  CrOpromInstallNotifyFunction,
                  (VOID*)SerialDevice,
                  &CrOpromInstallNotifyEvent
                  );
  if( EFI_ERROR(Status)) {
    return Status;
  }

  Status = gBS->RegisterProtocolNotify (
                  &gH2OCrOpRomInfoProtocolGuid,
                  CrOpromInstallNotifyEvent,
                  &CrOpromInstallRegistration
                  );
  //
  // Report status code the serial present
  //
  REPORT_STATUS_CODE_WITH_DEVICE_PATH (
    EFI_PROGRESS_CODE,
    EFI_P_PC_PRESENCE_DETECT | EFI_PERIPHERAL_SERIAL_PORT,
    ParentDevicePath
    );

  if (!SerialPortPresent (SerialDevice)) {
    Status = EFI_DEVICE_ERROR;
    REPORT_STATUS_CODE_WITH_DEVICE_PATH (
      EFI_ERROR_CODE,
      EFI_P_EC_NOT_DETECTED | EFI_PERIPHERAL_SERIAL_PORT,
      ParentDevicePath
      );
    goto Error;
  }

  //
  // Build the device path by appending the UART node to the ParentDevicePath.
  // The Uart setings are zero here, since  SetAttribute() will update them to match
  // the default setings.
  //
  SerialDevice->DevicePath = AppendDevicePathNode (
                               ParentDevicePath,
                               (EFI_DEVICE_PATH_PROTOCOL *) &SerialDevice->UartDevicePath
                               );
  //
  // Only produce the Flow Control node when remaining device path has it
  //
  if (FlowControl != NULL) {
    TempDevicePath = SerialDevice->DevicePath;
    if (TempDevicePath != NULL) {
      SerialDevice->DevicePath = AppendDevicePathNode (
                                   TempDevicePath,
                                   (EFI_DEVICE_PATH_PROTOCOL *) FlowControl
                                   );
      FreePool (TempDevicePath);
    }
  }
  if (SerialDevice->DevicePath == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto Error;
  }

  //
  // Fill in Serial I/O Mode structure based on either the RemainingDevicePath or defaults.
  //
  SerialDevice->SerialMode.BaudRate         = SerialDevice->UartDevicePath.BaudRate;
  SerialDevice->SerialMode.DataBits         = SerialDevice->UartDevicePath.DataBits;
  SerialDevice->SerialMode.Parity           = SerialDevice->UartDevicePath.Parity;
  SerialDevice->SerialMode.StopBits         = SerialDevice->UartDevicePath.StopBits;

  //
  // Issue a reset to initialize the COM port
  //
  Status = SerialDevice->SerialIo.Reset (&SerialDevice->SerialIo);


  if (EFI_ERROR (Status)) {
    REPORT_STATUS_CODE_WITH_DEVICE_PATH (
      EFI_ERROR_CODE,
      EFI_P_EC_CONTROLLER_ERROR | EFI_PERIPHERAL_SERIAL_PORT,
      ParentDevicePath
      );
    goto Error;
  }
  //
  // Install protocol interfaces for the serial device.
  //
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &SerialDevice->Handle,
                  &gEfiDevicePathProtocolGuid,
                  SerialDevice->DevicePath,
                  &gEfiSerialIoProtocolGuid,
                  &SerialDevice->SerialIo,
                  NULL
                  );
  if (EFI_ERROR (Status)) {
    goto Error;
  }
  //
  // Open For Child Device
  //
  Status = gBS->OpenProtocol (
                  Controller,
                  &gH2OCrUart16550AccessProtocolGuid,
                  (VOID **) &Uart16550,
                  This->DriverBindingHandle,
                  SerialDevice->Handle,
                  EFI_OPEN_PROTOCOL_BY_CHILD_CONTROLLER
                  );

Error:
  if (EFI_ERROR (Status)) {
    gBS->CloseProtocol (
           Controller,
           &gEfiDevicePathProtocolGuid,
           This->DriverBindingHandle,
           Controller
           );
    gBS->CloseProtocol (
           Controller,
           &gH2OCrUart16550AccessProtocolGuid,
           This->DriverBindingHandle,
           Controller
           );
    if (SerialDevice != NULL) {
      if (SerialDevice->DevicePath != NULL) {
        gBS->FreePool (SerialDevice->DevicePath);
      }

      FreeUnicodeStringTable (SerialDevice->ControllerNameTable);
      gBS->FreePool (SerialDevice);
      
      if (CrOpromInstallNotifyEvent != NULL) {
        gBS->CloseEvent(CrOpromInstallNotifyEvent);
      }
    }
  }

  return Status;
}

/**
  Disconnect this driver with the controller, uninstall related protocol instance

  @param  This                  A pointer to the EFI_DRIVER_BINDING_PROTOCOL instance.
  @param  Controller            The handle of the controller to test.
  @param  NumberOfChildren      Number of child device.
  @param  ChildHandleBuffer     A pointer to the remaining portion of a device path.

  @retval EFI_SUCCESS           Operation successfully
  @retval EFI_DEVICE_ERROR      Cannot stop the driver successfully

**/
EFI_STATUS
EFIAPI
Uart16550DriverStop (
  IN  EFI_DRIVER_BINDING_PROTOCOL    *This,
  IN  EFI_HANDLE                     Controller,
  IN  UINTN                          NumberOfChildren,
  IN  EFI_HANDLE                     *ChildHandleBuffer
  )
{
  EFI_STATUS                          Status;
  UINTN                               Index;
  BOOLEAN                             AllChildrenStopped;
  EFI_SERIAL_IO_PROTOCOL              *SerialIo;
  SERIAL_DEV                          *SerialDevice;
  H2O_UART_16550_ACCESS_PROTOCOL      *Uart16550;
  EFI_DEVICE_PATH_PROTOCOL            *DevicePath;

  Status = gBS->HandleProtocol (
                  Controller,
                  &gEfiDevicePathProtocolGuid,
                  (VOID **) &DevicePath
                  );

  //
  // Report the status code disable the serial
  //
  REPORT_STATUS_CODE_WITH_DEVICE_PATH (
    EFI_PROGRESS_CODE,
    EFI_P_PC_DISABLE | EFI_PERIPHERAL_SERIAL_PORT,
    DevicePath
    );

  //
  // Complete all outstanding transactions to Controller.
  // Don't allow any new transaction to Controller to be started.
  //
  if (NumberOfChildren == 0) {
    //
    // Close the bus driver
    //
    Status = gBS->CloseProtocol (
                    Controller,
                    &gH2OCrUart16550AccessProtocolGuid,
                    This->DriverBindingHandle,
                    Controller
                    );

    Status = gBS->CloseProtocol (
                    Controller,
                    &gEfiDevicePathProtocolGuid,
                    This->DriverBindingHandle,
                    Controller
                    );
    return Status;
  }

  AllChildrenStopped = TRUE;

  for (Index = 0; Index < NumberOfChildren; Index++) {

    Status = gBS->OpenProtocol (
                    ChildHandleBuffer[Index],
                    &gEfiSerialIoProtocolGuid,
                    (VOID **) &SerialIo,
                    This->DriverBindingHandle,
                    Controller,
                    EFI_OPEN_PROTOCOL_GET_PROTOCOL
                    );
    if (!EFI_ERROR (Status)) {

      SerialDevice = SERIAL_DEV_FROM_THIS (SerialIo);

      Status = gBS->CloseProtocol (
                      Controller,
                      &gH2OCrUart16550AccessProtocolGuid,
                      This->DriverBindingHandle,
                      ChildHandleBuffer[Index]
                      );

      Status = gBS->UninstallMultipleProtocolInterfaces (
                      ChildHandleBuffer[Index],
                      &gEfiDevicePathProtocolGuid,
                      SerialDevice->DevicePath,
                      &gEfiSerialIoProtocolGuid,
                      &SerialDevice->SerialIo,
                      NULL
                      );
      if (EFI_ERROR (Status)) {
        gBS->OpenProtocol (
               Controller,
               &gH2OCrUart16550AccessProtocolGuid,
               (VOID **) &Uart16550,
               This->DriverBindingHandle,
               ChildHandleBuffer[Index],
               EFI_OPEN_PROTOCOL_BY_CHILD_CONTROLLER
               );
      } else {
        if (SerialDevice->DevicePath != NULL) {
          gBS->FreePool (SerialDevice->DevicePath);
        }

        FreeUnicodeStringTable (SerialDevice->ControllerNameTable);
        gBS->FreePool (SerialDevice);
      }
    }

    if (EFI_ERROR (Status)) {
      AllChildrenStopped = FALSE;
    }
  }

  if (!AllChildrenStopped) {
    return EFI_DEVICE_ERROR;
  }

  return EFI_SUCCESS;
}

/**
  Detect whether specific FIFO is full or not.

  @param Fifo    A pointer to the Data Structure SERIAL_DEV_FIFO

  @return whether specific FIFO is full or not

**/
BOOLEAN
Uart16550FifoFull (
  IN SERIAL_DEV_FIFO *Fifo
  )
{
  if (Fifo->Surplus == 0) {
    return TRUE;
  }

  return FALSE;
}

/**
  Detect whether specific FIFO is empty or not.

  @param  Fifo    A pointer to the Data Structure SERIAL_DEV_FIFO

  @return whether specific FIFO is empty or not

**/
BOOLEAN
Uart16550FifoEmpty (
  IN SERIAL_DEV_FIFO *Fifo
  )
{
  if (Fifo->Surplus == SERIAL_MAX_BUFFER_SIZE) {
    return TRUE;
  }

  return FALSE;
}

/**
  Add data to specific FIFO.

  @param Fifo                  A pointer to the Data Structure SERIAL_DEV_FIFO
  @param Data                  the data added to FIFO

  @retval EFI_SUCCESS           Add data to specific FIFO successfully
  @retval EFI_OUT_OF_RESOURCE   Failed to add data because FIFO is already full

**/
EFI_STATUS
Uart16550FifoAdd (
  IN SERIAL_DEV_FIFO *Fifo,
  IN UINT8           Data
  )
{
  //
  // if FIFO full can not add data
  //
  if (Uart16550FifoFull (Fifo)) {
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // FIFO is not full can add data
  //
  Fifo->Data[Fifo->Last] = Data;
  Fifo->Surplus--;
  Fifo->Last++;
  if (Fifo->Last == SERIAL_MAX_BUFFER_SIZE) {
    Fifo->Last = 0;
  }

  if (Fifo->Surplus < 3) {
    return EFI_BUFFER_TOO_SMALL;
  } else {
    return EFI_SUCCESS;
  }
}

/**
  Remove data from specific FIFO.

  @param Fifo                  A pointer to the Data Structure SERIAL_DEV_FIFO
  @param Data                  the data removed from FIFO

  @retval EFI_SUCCESS           Remove data from specific FIFO successfully
  @retval EFI_OUT_OF_RESOURCE   Failed to remove data because FIFO is empty

**/
EFI_STATUS
Uart16550FifoRemove (
  IN  SERIAL_DEV_FIFO *Fifo,
  OUT UINT8           *Data
  )
{
  //
  // if FIFO is empty, no data can remove
  //
  if (Uart16550FifoEmpty (Fifo)) {
    return EFI_OUT_OF_RESOURCES;
  }
  //
  // FIFO is not empty, can remove data
  //
  *Data = Fifo->Data[Fifo->First];
  Fifo->Surplus++;
  Fifo->First++;
  if (Fifo->First == SERIAL_MAX_BUFFER_SIZE) {
    Fifo->First = 0;
  }

  return EFI_SUCCESS;
}

/**
  Reads and writes all avaliable data.

  @param SerialDevice           The device to flush

  @retval EFI_SUCCESS                  Data was read/written successfully.
  @retval EFI_OUT_OF_RESOURCE   Failed because software receive FIFO is full.  Note, when
                                                   this happens, pending writes are not done.

**/
EFI_STATUS
Uart16550ReceiveTransmit (
  IN SERIAL_DEV *SerialDevice
  )
{
  SERIAL_PORT_LSR Lsr;
  UINT8           Data;
  BOOLEAN         ReceiveFifoFull;
  SERIAL_PORT_MSR Msr;
  SERIAL_PORT_MCR Mcr;
  UINTN           TimeOut;
  EFI_STATUS      Status;

  Data = 0;

  //
  // Begin the read or write
  //
  if (SerialDevice->SoftwareLoopbackEnable) {
    do {
      ReceiveFifoFull = Uart16550FifoFull (&SerialDevice->Receive);
      if (!Uart16550FifoEmpty (&SerialDevice->Transmit)) {
        Uart16550FifoRemove (&SerialDevice->Transmit, &Data);
        if (ReceiveFifoFull) {
          return EFI_OUT_OF_RESOURCES;
        }

        Uart16550FifoAdd (&SerialDevice->Receive, Data);
      }
    } while (!Uart16550FifoEmpty (&SerialDevice->Transmit));
  } else {
    ReceiveFifoFull = Uart16550FifoFull (&SerialDevice->Receive);
    //
    // For full handshake flow control, tell the peer to send data
    // if receive buffer is available.
    //
    if (SerialDevice->HardwareFlowControl &&
        !FeaturePcdGet(PcdIsaBusSerialUseHalfHandshake)&&
        !ReceiveFifoFull
        ) {
      Mcr.Data     = READ_MCR (SerialDevice->Uart16550);
      Mcr.Bits.Rts = 1;
      WRITE_MCR (SerialDevice->Uart16550, Mcr.Data);
    }
    do {
      Lsr.Data = READ_LSR (SerialDevice->Uart16550);

      //
      // Flush incomming data to prevent a an overrun during a long write
      //
      if ((Lsr.Bits.Dr == 1) && !ReceiveFifoFull) {
        ReceiveFifoFull = Uart16550FifoFull (&SerialDevice->Receive);
        if (!ReceiveFifoFull) {
          if (Lsr.Bits.FIFOe == 1 || Lsr.Bits.Oe == 1 || Lsr.Bits.Pe == 1 || Lsr.Bits.Fe == 1 || Lsr.Bits.Bi == 1) {
            REPORT_STATUS_CODE_WITH_DEVICE_PATH (
              EFI_ERROR_CODE,
              EFI_P_EC_INPUT_ERROR | EFI_PERIPHERAL_SERIAL_PORT,
              SerialDevice->DevicePath
              );
            if (Lsr.Bits.FIFOe == 1 || Lsr.Bits.Pe == 1|| Lsr.Bits.Fe == 1 || Lsr.Bits.Bi == 1) {
              Data = READ_RBR (SerialDevice->Uart16550);
              if ((Lsr.Bits.Bi == 1) && (mLineBreakToCtrlC == TRUE)) {
                //
                // Detect the Break Interrupt(BI) of Line Status Register(LSR), 
                // then translate to Ctrl-C. 
                //
                Uart16550FifoAdd (&SerialDevice->Receive, 0x03); // Ctrl-C
              }
              continue;
            }
          }

          Data = READ_RBR (SerialDevice->Uart16550);

          if (SerialDevice->SoftwareFlowControl == TRUE && (Data == XON || Data == XOFF)) {
            SetSwFcState (SerialDevice, Data, REMOTE_SEND);
          } else {
            Status = Uart16550FifoAdd (&SerialDevice->Receive, Data);
            if (Status == EFI_BUFFER_TOO_SMALL && GetSwFcState (SerialDevice , HOST_SEND) == XON) {
              SetSoftFlowControl (SerialDevice, XOFF);
            }
          }

          //
          // For full handshake flow control, if receive buffer full
          // tell the peer to stop sending data.
          //
          if (SerialDevice->HardwareFlowControl &&
              !FeaturePcdGet(PcdIsaBusSerialUseHalfHandshake)   &&
              Uart16550FifoFull (&SerialDevice->Receive)
              ) {
            Mcr.Data     = READ_MCR (SerialDevice->Uart16550);
            Mcr.Bits.Rts = 0;
            WRITE_MCR (SerialDevice->Uart16550, Mcr.Data);
          }


          continue;
        } else {
          REPORT_STATUS_CODE_WITH_DEVICE_PATH (
            EFI_PROGRESS_CODE,
            EFI_P_SERIAL_PORT_PC_CLEAR_BUFFER | EFI_PERIPHERAL_SERIAL_PORT,
            SerialDevice->DevicePath
            );
        }
      }
      //
      // Do the write
      //
      if (Lsr.Bits.Thre == 1 && !Uart16550FifoEmpty (&SerialDevice->Transmit)) {
        //
        // Make sure the transmit data will not be missed
        //
        if (SerialDevice->HardwareFlowControl) {
          //
          // For half handshake flow control assert RTS before sending.
          //
          if (FeaturePcdGet(PcdIsaBusSerialUseHalfHandshake)) {
            Mcr.Data     = READ_MCR (SerialDevice->Uart16550);
            Mcr.Bits.Rts= 0;
            WRITE_MCR (SerialDevice->Uart16550, Mcr.Data);
          }
          //
          // Wait for CTS
          //
          TimeOut   = 0;
          Msr.Data  = READ_MSR (SerialDevice->Uart16550);
          while ((Msr.Bits.Dcd == 1) && ((Msr.Bits.Cts == 0) ^ FeaturePcdGet(PcdIsaBusSerialUseHalfHandshake))) {
            gBS->Stall (TIMEOUT_STALL_INTERVAL);
            TimeOut++;
            if (TimeOut > 5) {
              break;
            }

            Msr.Data = READ_MSR (SerialDevice->Uart16550);
          }

          if ((Msr.Bits.Dcd == 0) || ((Msr.Bits.Cts == 1) ^ FeaturePcdGet(PcdIsaBusSerialUseHalfHandshake))) {
            Uart16550FifoRemove (&SerialDevice->Transmit, &Data);
            WRITE_THR (SerialDevice->Uart16550, Data);
          }

          //
          // For half handshake flow control, tell DCE we are done.
          //
          if (FeaturePcdGet(PcdIsaBusSerialUseHalfHandshake)) {
            Mcr.Data = READ_MCR (SerialDevice->Uart16550);
            Mcr.Bits.Rts = 1;
            WRITE_MCR (SerialDevice->Uart16550, Mcr.Data);
          }
        } else {

          Uart16550FifoRemove (&SerialDevice->Transmit, &Data);
          if (SerialDevice->SoftwareFlowControl == TRUE) {
            if (GetSwFcState(SerialDevice, REMOTE_SEND) == XON) {
              WRITE_THR (SerialDevice->Uart16550, Data);
            }
          } else {
            WRITE_THR (SerialDevice->Uart16550, Data);
          }

          //
          // Wait for TEMT
          //
          if (SerialDevice->Type == UART16550) {
            TimeOut  = 0;
            Lsr.Data = READ_LSR (SerialDevice->Uart16550);
            while ( Lsr.Bits.Temt != 1 ) {
              gBS->Stall (TIMEOUT_STALL_INTERVAL);
              TimeOut ++;
              if (TimeOut > SerialDevice->SerialIo.Mode->Timeout ) {
                break;
              }
              Lsr.Data = READ_LSR (SerialDevice->Uart16550);
            }
          }
        }
      }
    } while (Lsr.Bits.Thre == 1 && !Uart16550FifoEmpty (&SerialDevice->Transmit));
  }

  return EFI_SUCCESS;
}

//
// Interface Functions
//
/**
  Reset serial device.

  @param This               Pointer to EFI_SERIAL_IO_PROTOCOL

  @retval EFI_SUCCESS        Reset successfully
  @retval EFI_DEVICE_ERROR   Failed to reset

**/
EFI_STATUS
EFIAPI
Uart16550Reset (
  IN EFI_SERIAL_IO_PROTOCOL  *This
  )
{
  EFI_STATUS      Status;
  SERIAL_DEV      *SerialDevice;
  SERIAL_PORT_LCR Lcr;
  SERIAL_PORT_IER Ier;
  SERIAL_PORT_MCR Mcr;
  SERIAL_PORT_FCR Fcr;
  EFI_TPL         Tpl;
  UINT32          Control;
  SERIAL_PORT_LSR Lsr;
  UINTN           TimesOfReadingThre;

  SerialDevice = SERIAL_DEV_FROM_THIS (This);
  
  //
  // To avoid the printing string being cut while the key buffer is cleared.
  // If the device gets error, after 10s, this system will continue working.
  //
  TimesOfReadingThre = 0;

  do {
    Lsr.Data = READ_LSR (SerialDevice->Uart16550);
    TimesOfReadingThre++;
  } while ((Lsr.Bits.Thre != 1) && (TimesOfReadingThre <= 100000));

  //
  // Report the status code reset the serial
  //
  REPORT_STATUS_CODE_WITH_DEVICE_PATH (
    EFI_PROGRESS_CODE,
    EFI_P_PC_RESET | EFI_PERIPHERAL_SERIAL_PORT,
    SerialDevice->DevicePath
    );

  Tpl = gBS->RaiseTPL (TPL_NOTIFY);

  //
  // Make sure DLAB is 0.
  //
  Lcr.Data      = READ_LCR (SerialDevice->Uart16550);
  Lcr.Bits.DLab = 0;
  WRITE_LCR (SerialDevice->Uart16550, Lcr.Data);

  //
  // Turn off all interrupts
  //
  Ier.Data        = READ_IER (SerialDevice->Uart16550);
  Ier.Bits.Ravie  = 0;
  Ier.Bits.Theie  = 0;
  Ier.Bits.Rie    = 0;
  Ier.Bits.Mie    = 0;
  WRITE_IER (SerialDevice->Uart16550, Ier.Data);

  //
  // Disable the FIFO.
  //
  Fcr.Bits.TrFIFOE = 0;
  WRITE_FCR (SerialDevice->Uart16550, Fcr.Data);

  //
  // Turn off loopback and disable device interrupt.
  //
  Mcr.Data      = READ_MCR (SerialDevice->Uart16550);
  Mcr.Bits.Out1 = 0;
  Mcr.Bits.Out2 = 0;
  Mcr.Bits.Lme  = 0;
  //
  // Set the DTR and RTS = TURE, If user's device need flow control and user didn't select
  // flow control on C.R SCU. Let it can work well. (This method is the same as C.R option ROM)
  Mcr.Bits.DtrC = 1;
  Mcr.Bits.Rts  = 1;
  WRITE_MCR (SerialDevice->Uart16550, Mcr.Data);

  //
  // Clear the scratch pad register
  //
  WRITE_SCR (SerialDevice->Uart16550, 0);

  //
  // Go set the current attributes
  //
  Status = This->SetAttributes (
                   This,
                   This->Mode->BaudRate,
                   This->Mode->ReceiveFifoDepth,
                   This->Mode->Timeout,
                   (EFI_PARITY_TYPE) This->Mode->Parity,
                   (UINT8) This->Mode->DataBits,
                   (EFI_STOP_BITS_TYPE) This->Mode->StopBits
                   );

  if (EFI_ERROR (Status)) {
    gBS->RestoreTPL (Tpl);
    ASSERT(FALSE);
    return EFI_DEVICE_ERROR;
  }
  //
  // Go set the current control bits
  //
//[-start-180418-IB08400583-modify]//
  Control = EFI_SERIAL_REQUEST_TO_SEND | EFI_SERIAL_DATA_TERMINAL_READY;
//[-end-180418-IB08400583-modify]//
  if (SerialDevice->HardwareFlowControl) {
    Control |= EFI_SERIAL_HARDWARE_FLOW_CONTROL_ENABLE;
  }
  if (SerialDevice->SoftwareLoopbackEnable) {
    Control |= EFI_SERIAL_SOFTWARE_LOOPBACK_ENABLE;
  }
  Status = This->SetControl (
                   This,
                   Control
                   );

  if (EFI_ERROR (Status)) {
    gBS->RestoreTPL (Tpl);
    ASSERT(FALSE);
    return EFI_DEVICE_ERROR;
  }
  //
  // for 16550A enable FIFO, 16550 disable FIFO
  //
  if (SerialDevice->Type == UART16550A) {
    Fcr.Bits.TrFIFOE  = 1;
    Fcr.Bits.ResetRF  = 1;
    Fcr.Bits.ResetTF  = 1;
  } else {
    Fcr.Bits.TrFIFOE  = 0;
    Fcr.Bits.ResetRF  = 0;
    Fcr.Bits.ResetTF  = 0;
  }
  WRITE_FCR (SerialDevice->Uart16550, Fcr.Data);

  //
  // Reset the software FIFO
  //
  SerialDevice->Receive.First     = 0;
  SerialDevice->Receive.Last      = 0;
  SerialDevice->Receive.Surplus   = SERIAL_MAX_BUFFER_SIZE;
  SerialDevice->Transmit.First    = 0;
  SerialDevice->Transmit.Last     = 0;
  SerialDevice->Transmit.Surplus  = SERIAL_MAX_BUFFER_SIZE;

  gBS->RestoreTPL (Tpl);

  //
  // Device reset is complete
  //
  return EFI_SUCCESS;
}

/**
  Set new attributes to a serial device.

  @param This                     Pointer to EFI_SERIAL_IO_PROTOCOL
  @param  BaudRate                 The baudrate of the serial device
  @param  ReceiveFifoDepth         The depth of receive FIFO buffer
  @param  Timeout                  The request timeout for a single char
  @param  Parity                   The type of parity used in serial device
  @param  DataBits                 Number of databits used in serial device
  @param  StopBits                 Number of stopbits used in serial device

  @retval  EFI_SUCCESS              The new attributes were set
  @retval  EFI_INVALID_PARAMETERS   One or more attributes have an unsupported value
  @retval  EFI_UNSUPPORTED          Data Bits can not set to 5 or 6
  @retval  EFI_DEVICE_ERROR         The serial device is not functioning correctly (no return)

**/
EFI_STATUS
EFIAPI
Uart16550SetAttributes (
  IN EFI_SERIAL_IO_PROTOCOL  *This,
  IN UINT64                  BaudRate,
  IN UINT32                  ReceiveFifoDepth,
  IN UINT32                  Timeout,
  IN EFI_PARITY_TYPE         Parity,
  IN UINT8                   DataBits,
  IN EFI_STOP_BITS_TYPE      StopBits
  )
{
  EFI_STATUS                Status;
  SERIAL_DEV                *SerialDevice;
  UINT32                    Divisor;
  UINT32                    Remained;
  SERIAL_PORT_LCR           Lcr;
  UART_DEVICE_PATH          *Uart;
  EFI_TPL                   Tpl;
  UINTN                     SerialClockFreq;

  SerialDevice = SERIAL_DEV_FROM_THIS (This);

  //
  // Check for default settings and fill in actual values.
  //
  if (BaudRate == 0) {
    BaudRate = PcdGet64 (PcdUartDefaultBaudRate);
  }

  if (ReceiveFifoDepth == 0) {
    ReceiveFifoDepth = SERIAL_PORT_DEFAULT_RECEIVE_FIFO_DEPTH;
  }

  if (Timeout == 0) {
    Timeout = SERIAL_PORT_DEFAULT_TIMEOUT;
  }

  if (Parity == DefaultParity) {
    Parity = (EFI_PARITY_TYPE)PcdGet8 (PcdUartDefaultParity);
  }

  if (DataBits == 0) {
    DataBits = PcdGet8 (PcdUartDefaultDataBits);
  }

  if (StopBits == DefaultStopBits) {
    StopBits = (EFI_STOP_BITS_TYPE) PcdGet8 (PcdUartDefaultStopBits);
  }
  //
  // 5 and 6 data bits can not be verified on a 16550A UART
  // Return EFI_INVALID_PARAMETER if an attempt is made to use these settings.
  //
  if ((DataBits == 5) || (DataBits == 6)) {
    DEBUG ((DEBUG_ERROR, "Uart16550 Fail[%a.%d] DataBits = %d\n", __FUNCTION__, __LINE__, DataBits));
    return EFI_INVALID_PARAMETER;
  }
  //
  // Make sure all parameters are valid
  //
  if ((BaudRate > SERIAL_PORT_MAX_BAUD_RATE) || (BaudRate < SERIAL_PORT_MIN_BAUD_RATE)) {
    DEBUG ((DEBUG_ERROR, "Uart16550 Fail[%a.%d] BaudRate = %d\n", __FUNCTION__, __LINE__, BaudRate));
    return EFI_INVALID_PARAMETER;
  }
  //
  // 50,75,110,134,150,300,600,1200,1800,2000,2400,3600,4800,7200,9600,19200,
  // 38400,57600,115200
  //
  if (BaudRate < 75) {
    BaudRate = 50;
  } else if (BaudRate < 110) {
    BaudRate = 75;
  } else if (BaudRate < 134) {
    BaudRate = 110;
  } else if (BaudRate < 150) {
    BaudRate = 134;
  } else if (BaudRate < 300) {
    BaudRate = 150;
  } else if (BaudRate < 600) {
    BaudRate = 300;
  } else if (BaudRate < 1200) {
    BaudRate = 600;
  } else if (BaudRate < 1800) {
    BaudRate = 1200;
  } else if (BaudRate < 2000) {
    BaudRate = 1800;
  } else if (BaudRate < 2400) {
    BaudRate = 2000;
  } else if (BaudRate < 3600) {
    BaudRate = 2400;
  } else if (BaudRate < 4800) {
    BaudRate = 3600;
  } else if (BaudRate < 7200) {
    BaudRate = 4800;
  } else if (BaudRate < 9600) {
    BaudRate = 7200;
  } else if (BaudRate < 19200) {
    BaudRate = 9600;
  } else if (BaudRate < 38400) {
    BaudRate = 19200;
  } else if (BaudRate < 57600) {
    BaudRate = 38400;
  } else if (BaudRate < 115200) {
    BaudRate = 57600;
  } else if (BaudRate <= SERIAL_PORT_MAX_BAUD_RATE) {
    BaudRate = 115200;
  }

  if ((ReceiveFifoDepth < 1) || (ReceiveFifoDepth > SERIAL_PORT_MAX_RECEIVE_FIFO_DEPTH)) {
    DEBUG ((DEBUG_ERROR, "Uart16550 Fail[%a.%d] ReceiveFifoDepth = %d\n", __FUNCTION__, __LINE__, ReceiveFifoDepth));
    return EFI_INVALID_PARAMETER;
  }

  if ((Timeout < SERIAL_PORT_MIN_TIMEOUT) || (Timeout > SERIAL_PORT_MAX_TIMEOUT)) {
    DEBUG ((DEBUG_ERROR, "Uart16550 Fail[%a.%d] Timeout = %d\n", __FUNCTION__, __LINE__, Timeout));
    return EFI_INVALID_PARAMETER;
  }

  if ((Parity < NoParity) || (Parity > SpaceParity)) {
    DEBUG ((DEBUG_ERROR, "Uart16550 Fail[%a.%d] Parity = %d\n", __FUNCTION__, __LINE__, Parity));
    return EFI_INVALID_PARAMETER;
  }

  if ((DataBits < 5) || (DataBits > 8)) {
    DEBUG ((DEBUG_ERROR, "Uart16550 Fail[%a.%d] DataBits = %d\n", __FUNCTION__, __LINE__, DataBits));
    return EFI_INVALID_PARAMETER;
  }

  if ((StopBits < OneStopBit) || (StopBits > TwoStopBits)) {
    DEBUG ((DEBUG_ERROR, "Uart16550 Fail[%a.%d] StopBits = %d\n", __FUNCTION__, __LINE__, StopBits));
    return EFI_INVALID_PARAMETER;
  }

  //
  // for DataBits = 6,7,8, StopBits can not set OneFiveStopBits
  //
  if ((DataBits >= 6) && (DataBits <= 8) && (StopBits == OneFiveStopBits)) {
    DEBUG ((DEBUG_ERROR, "Uart16550 Fail[%a.%d] DataBits = %d, StopBits = %d\n", __FUNCTION__, __LINE__, DataBits, StopBits));
    return EFI_INVALID_PARAMETER;
  }

  //
  // Compute divisor use to program the baud rate using a round determination
  //
  SerialClockFreq = SerialDevice->Uart16550->DeviceInfo->SerialClockFreq;

  Divisor = (UINT32) DivU64x32Remainder (
                       SerialClockFreq,
                       ((UINT32) BaudRate * 16),
                       &Remained
                       );
  if (Remained != 0) {
    Divisor += 1;
  }

  if ((Divisor == 0) || ((Divisor & 0xffff0000) != 0)) {
    DEBUG ((DEBUG_ERROR, "Uart16550 Fail[%a.%d] Divisor = %d\n", __FUNCTION__, __LINE__, Divisor));
    return EFI_INVALID_PARAMETER;
  }

  Tpl = gBS->RaiseTPL (TPL_NOTIFY);

  //
  // Compute the actual baud rate that the serial port will be programmed for.
  //

  BaudRate = SerialClockFreq / Divisor / 16;


  //
  // Put serial port on Divisor Latch Mode
  //
  Lcr.Data      = READ_LCR (SerialDevice->Uart16550);
  Lcr.Bits.DLab = 1;
  WRITE_LCR (SerialDevice->Uart16550, Lcr.Data);

  //
  // Write the divisor to the serial port
  //
  WRITE_DLL (SerialDevice->Uart16550, (UINT8) (Divisor & 0xff));
  WRITE_DLM (SerialDevice->Uart16550, (UINT8) ((Divisor >> 8) & 0xff));

  //
  // Put serial port back in normal mode and set remaining attributes.
  //
  Lcr.Bits.DLab = 0;

  switch (Parity) {
  case NoParity:
    Lcr.Bits.ParEn    = 0;
    Lcr.Bits.EvenPar  = 0;
    Lcr.Bits.SticPar  = 0;
    break;

  case EvenParity:
    Lcr.Bits.ParEn    = 1;
    Lcr.Bits.EvenPar  = 1;
    Lcr.Bits.SticPar  = 0;
    break;

  case OddParity:
    Lcr.Bits.ParEn    = 1;
    Lcr.Bits.EvenPar  = 0;
    Lcr.Bits.SticPar  = 0;
    break;

  case SpaceParity:
    Lcr.Bits.ParEn    = 1;
    Lcr.Bits.EvenPar  = 1;
    Lcr.Bits.SticPar  = 1;
    break;

  case MarkParity:
    Lcr.Bits.ParEn    = 1;
    Lcr.Bits.EvenPar  = 0;
    Lcr.Bits.SticPar  = 1;
    break;

  default:
    break;
  }

  switch (StopBits) {
  case OneStopBit:
    Lcr.Bits.StopB = 0;
    break;

  case OneFiveStopBits:
  case TwoStopBits:
    Lcr.Bits.StopB = 1;
    break;

  default:
    break;
  }
  //
  // DataBits
  //
  Lcr.Bits.SerialDB = (UINT8) ((DataBits - 5) & 0x03);
  WRITE_LCR (SerialDevice->Uart16550, Lcr.Data);

  //
  // Set the Serial I/O mode
  //
  This->Mode->BaudRate          = BaudRate;
  This->Mode->ReceiveFifoDepth  = ReceiveFifoDepth;
  This->Mode->Timeout           = Timeout;
  This->Mode->Parity            = Parity;
  This->Mode->DataBits          = DataBits;
  This->Mode->StopBits          = StopBits;

  //
  // See if Device Path Node has actually changed
  //
  if (SerialDevice->UartDevicePath.BaudRate == BaudRate &&
      SerialDevice->UartDevicePath.DataBits == DataBits &&
      SerialDevice->UartDevicePath.Parity == Parity &&
      SerialDevice->UartDevicePath.StopBits == StopBits
      ) {
    gBS->RestoreTPL (Tpl);
    DEBUG ((DEBUG_INFO, "Device Path Node has actually changed.\n"));
    return EFI_SUCCESS;
  }
  //
  // Update the device path
  //
  SerialDevice->UartDevicePath.BaudRate = BaudRate;
  SerialDevice->UartDevicePath.DataBits = DataBits;
  SerialDevice->UartDevicePath.Parity   = (UINT8) Parity;
  SerialDevice->UartDevicePath.StopBits = (UINT8) StopBits;

  Status = EFI_SUCCESS;
  if (SerialDevice->Handle != NULL) {
    Uart = (UART_DEVICE_PATH *) (
             (UINTN) SerialDevice->DevicePath
             + GetDevicePathSize (SerialDevice->ParentDevicePath)
             - END_DEVICE_PATH_LENGTH
             );
    CopyMem (Uart, &SerialDevice->UartDevicePath, sizeof (UART_DEVICE_PATH));
    Status = gBS->ReinstallProtocolInterface (
                    SerialDevice->Handle,
                    &gEfiDevicePathProtocolGuid,
                    SerialDevice->DevicePath,
                    SerialDevice->DevicePath
                    );
  }

  gBS->RestoreTPL (Tpl);
  ASSERT_EFI_ERROR(Status);
  return Status;
}

/**
  Set Control Bits.

  @param This              Pointer to EFI_SERIAL_IO_PROTOCOL
  @param Control           Control bits that can be settable

  @retval EFI_SUCCESS       New Control bits were set successfully
  @retval EFI_UNSUPPORTED   The Control bits wanted to set are not supported

**/
EFI_STATUS
EFIAPI
Uart16550SetControl (
  IN EFI_SERIAL_IO_PROTOCOL  *This,
  IN UINT32                  Control
  )
{
  SERIAL_DEV                    *SerialDevice;
  SERIAL_PORT_MCR               Mcr;
  EFI_TPL                       Tpl;
  UART_FLOW_CONTROL_DEVICE_PATH *FlowControl;
  EFI_STATUS                    Status;

  //
  // The control bits that can be set are :
  //     EFI_SERIAL_DATA_TERMINAL_READY: 0x0001  // WO
  //     EFI_SERIAL_REQUEST_TO_SEND: 0x0002  // WO
  //     EFI_SERIAL_HARDWARE_LOOPBACK_ENABLE: 0x1000  // RW
  //     EFI_SERIAL_SOFTWARE_LOOPBACK_ENABLE: 0x2000  // RW
  //     EFI_SERIAL_HARDWARE_FLOW_CONTROL_ENABLE: 0x4000 // RW
  //
  SerialDevice = SERIAL_DEV_FROM_THIS (This);

  //
  // first determine the parameter is invalid
  //
  if ((Control & (~(EFI_SERIAL_REQUEST_TO_SEND | EFI_SERIAL_DATA_TERMINAL_READY |
                    EFI_SERIAL_HARDWARE_LOOPBACK_ENABLE | EFI_SERIAL_SOFTWARE_LOOPBACK_ENABLE |
                    EFI_SERIAL_HARDWARE_FLOW_CONTROL_ENABLE))) != 0) {
    DEBUG ((DEBUG_ERROR, "Uart16550 Fail[%a.%d] Control = %x\n", __FUNCTION__, __LINE__, Control));
    return EFI_UNSUPPORTED;
  }

  Tpl = gBS->RaiseTPL (TPL_NOTIFY);

  Mcr.Data = READ_MCR (SerialDevice->Uart16550);
  Mcr.Bits.DtrC = 0;
  Mcr.Bits.Rts = 0;
  Mcr.Bits.Lme = 0;
  SerialDevice->SoftwareLoopbackEnable = FALSE;
  SerialDevice->HardwareFlowControl = FALSE;

  if ((Control & EFI_SERIAL_DATA_TERMINAL_READY) == EFI_SERIAL_DATA_TERMINAL_READY) {
    Mcr.Bits.DtrC = 1;
  }

  if ((Control & EFI_SERIAL_REQUEST_TO_SEND) == EFI_SERIAL_REQUEST_TO_SEND) {
    Mcr.Bits.Rts = 1;
  }

  if ((Control & EFI_SERIAL_HARDWARE_LOOPBACK_ENABLE) == EFI_SERIAL_HARDWARE_LOOPBACK_ENABLE) {
    Mcr.Bits.Lme = 1;
  }

  if ((Control & EFI_SERIAL_HARDWARE_FLOW_CONTROL_ENABLE) == EFI_SERIAL_HARDWARE_FLOW_CONTROL_ENABLE) {
    SerialDevice->HardwareFlowControl = TRUE;
  }

  WRITE_MCR (SerialDevice->Uart16550, Mcr.Data);

  if ((Control & EFI_SERIAL_SOFTWARE_LOOPBACK_ENABLE) == EFI_SERIAL_SOFTWARE_LOOPBACK_ENABLE) {
    SerialDevice->SoftwareLoopbackEnable = TRUE;
  }

  Status = EFI_SUCCESS;
  if (SerialDevice->Handle != NULL) {
    FlowControl = (UART_FLOW_CONTROL_DEVICE_PATH *) (
                    (UINTN) SerialDevice->DevicePath
                    + GetDevicePathSize (SerialDevice->ParentDevicePath)
                    - END_DEVICE_PATH_LENGTH
                    + sizeof (UART_DEVICE_PATH)
                    );
    if (IsUartFlowControlNode (FlowControl) &&
        ((ReadUnaligned32 (&FlowControl->FlowControlMap) == UART_FLOW_CONTROL_HARDWARE) ^ SerialDevice->HardwareFlowControl)) {
      //
      // Flow Control setting is changed, need to reinstall device path protocol
      //
      WriteUnaligned32 (&FlowControl->FlowControlMap, SerialDevice->HardwareFlowControl ? UART_FLOW_CONTROL_HARDWARE : 0);
      Status = gBS->ReinstallProtocolInterface (
                      SerialDevice->Handle,
                      &gEfiDevicePathProtocolGuid,
                      SerialDevice->DevicePath,
                      SerialDevice->DevicePath
                      );
    }
  }

  gBS->RestoreTPL (Tpl);
  ASSERT_EFI_ERROR(Status);
  return Status;
}

/**
  Get ControlBits.

  @param This          Pointer to EFI_SERIAL_IO_PROTOCOL
  @param Control       Control signals of the serial device

  @retval EFI_SUCCESS   Get Control signals successfully

**/
EFI_STATUS
EFIAPI
Uart16550GetControl (
  IN EFI_SERIAL_IO_PROTOCOL  *This,
  OUT UINT32                 *Control
  )
{
  SERIAL_DEV      *SerialDevice;
  SERIAL_PORT_MSR Msr;
  SERIAL_PORT_MCR Mcr;
  EFI_TPL         Tpl;

  Tpl           = gBS->RaiseTPL (TPL_NOTIFY);

  SerialDevice  = SERIAL_DEV_FROM_THIS (This);

  *Control      = 0;

  //
  // Read the Modem Status Register
  //
  Msr.Data = READ_MSR (SerialDevice->Uart16550);

  if (Msr.Bits.Cts == 1) {
    *Control |= EFI_SERIAL_CLEAR_TO_SEND;
  }

  if (Msr.Bits.Dsr == 1) {
    *Control |= EFI_SERIAL_DATA_SET_READY;
  }

  if (Msr.Bits.Ri == 1) {
    *Control |= EFI_SERIAL_RING_INDICATE;
  }

  if (Msr.Bits.Dcd == 1) {
    *Control |= EFI_SERIAL_CARRIER_DETECT;
  }
  //
  // Read the Modem Control Register
  //
  Mcr.Data = READ_MCR (SerialDevice->Uart16550);

  if (Mcr.Bits.DtrC == 1) {
    *Control |= EFI_SERIAL_DATA_TERMINAL_READY;
  }

  if (Mcr.Bits.Rts == 1) {
    *Control |= EFI_SERIAL_REQUEST_TO_SEND;
  }

  if (Mcr.Bits.Lme == 1) {
    *Control |= EFI_SERIAL_HARDWARE_LOOPBACK_ENABLE;
  }

  if (SerialDevice->HardwareFlowControl) {
    *Control |= EFI_SERIAL_HARDWARE_FLOW_CONTROL_ENABLE;
  }
  //
  // See if the Transmit FIFO is empty
  //
  Uart16550ReceiveTransmit (SerialDevice);

  if (Uart16550FifoEmpty (&SerialDevice->Transmit)) {
    *Control |= EFI_SERIAL_OUTPUT_BUFFER_EMPTY;
  }

  if (Uart16550FifoEmpty (&SerialDevice->Receive)) {
    *Control |= EFI_SERIAL_INPUT_BUFFER_EMPTY;
  }

  if (SerialDevice->SoftwareLoopbackEnable) {
    *Control |= EFI_SERIAL_SOFTWARE_LOOPBACK_ENABLE;
  }

  gBS->RestoreTPL (Tpl);

  return EFI_SUCCESS;
}

/**
  Write the specified number of bytes to serial device.

  @param This               Pointer to EFI_SERIAL_IO_PROTOCOL
  @param  BufferSize         On input the size of Buffer, on output the amount of
                       data actually written
  @param  Buffer             The buffer of data to write

  @retval EFI_SUCCESS        The data were written successfully
  @retval EFI_DEVICE_ERROR   The device reported an error
  @retval EFI_TIMEOUT        The write operation was stopped due to timeout

**/
EFI_STATUS
EFIAPI
Uart16550Write (
  IN EFI_SERIAL_IO_PROTOCOL  *This,
  IN OUT UINTN               *BufferSize,
  IN VOID                    *Buffer
  )
{
  SERIAL_DEV  *SerialDevice;
  UINT8       *CharBuffer;
  UINT32      Index;
  UINTN       Elapsed;
  UINTN       ActualWrite;
  EFI_TPL     Tpl;
  UINTN       Timeout;
  UINTN       BitsPerCharacter;

  SerialDevice  = SERIAL_DEV_FROM_THIS (This);
  Elapsed       = 0;
  ActualWrite   = 0;

  if (*BufferSize == 0) {
    return EFI_SUCCESS;
  }

  if (Buffer == NULL) {
    REPORT_STATUS_CODE_WITH_DEVICE_PATH (
      EFI_ERROR_CODE,
      EFI_P_EC_OUTPUT_ERROR | EFI_PERIPHERAL_SERIAL_PORT,
      SerialDevice->DevicePath
      );

    return EFI_DEVICE_ERROR;
  }

  Tpl         = gBS->RaiseTPL (TPL_NOTIFY);
  CharBuffer  = (UINT8 *) Buffer;

  //
  // Compute the number of bits in a single character.  This is a start bit,
  // followed by the number of data bits, followed by the number of stop bits.
  // The number of stop bits is specified by an enumeration that includes
  // support for 1.5 stop bits.  Treat 1.5 stop bits as 2 stop bits.
  //
  BitsPerCharacter =
    1 +
    This->Mode->DataBits +
    ((This->Mode->StopBits == TwoStopBits) ? 2 : This->Mode->StopBits);

  //
  // Compute the timeout in microseconds to wait for a single byte to be
  // transmitted.  The Mode structure contans a Timeout field that is the
  // maximum time to transmit or receive a character.  However, many UARTs
  // have a FIFO for transmits, so the time required to add one new character
  // to the transmit FIFO may be the time required to flush a full FIFO.  If
  // the Timeout in the Mode structure is smaller than the time required to
  // flush a full FIFO at the current baud rate, then use a timeout value that
  // is required to flush a full transmit FIFO.
  //
  Timeout = MAX (
              This->Mode->Timeout,
              (UINTN)DivU64x64Remainder (
                BitsPerCharacter * (SERIAL_PORT_MAX_RECEIVE_FIFO_DEPTH + 1) * 1000000,
                This->Mode->BaudRate,
                NULL
                )
              );

  for (Index = 0; Index < *BufferSize; Index++) {
    Uart16550FifoAdd (&SerialDevice->Transmit, CharBuffer[Index]);

    while (Uart16550ReceiveTransmit (SerialDevice) != EFI_SUCCESS || !Uart16550FifoEmpty (&SerialDevice->Transmit)) {
      //
      //  Unsuccessful write so check if timeout has expired, if not,
      //  stall for a bit, increment time elapsed, and try again
      //
      if (Elapsed >= Timeout) {
        *BufferSize = ActualWrite;
        gBS->RestoreTPL (Tpl);
        return EFI_TIMEOUT;
      }

      gBS->Stall (TIMEOUT_STALL_INTERVAL);

      Elapsed += TIMEOUT_STALL_INTERVAL;
    }

    ActualWrite++;
    //
    //  Successful write so reset timeout
    //
    Elapsed = 0;
  }

  gBS->RestoreTPL (Tpl);

  return EFI_SUCCESS;
}

/**
  Read the specified number of bytes from serial device.

  @param This               Pointer to EFI_SERIAL_IO_PROTOCOL
  @param BufferSize         On input the size of Buffer, on output the amount of
                            data returned in buffer
  @param Buffer             The buffer to return the data into

  @retval EFI_SUCCESS        The data were read successfully
  @retval EFI_DEVICE_ERROR   The device reported an error
  @retval EFI_TIMEOUT        The read operation was stopped due to timeout

**/
EFI_STATUS
EFIAPI
Uart16550Read (
  IN EFI_SERIAL_IO_PROTOCOL  *This,
  IN OUT UINTN               *BufferSize,
  OUT VOID                   *Buffer
  )
{
  SERIAL_DEV  *SerialDevice;
  UINT32      Index;
  UINT8       *CharBuffer;
  UINTN       Elapsed;
  EFI_STATUS  Status;
  EFI_TPL     Tpl;

  SerialDevice  = SERIAL_DEV_FROM_THIS (This);
  Elapsed       = 0;

  if (*BufferSize == 0) {
    return EFI_SUCCESS;
  }

  if (Buffer == NULL) {
    return EFI_DEVICE_ERROR;
  }

  Tpl     = gBS->RaiseTPL (TPL_NOTIFY);

  Status  = Uart16550ReceiveTransmit (SerialDevice);

  if (EFI_ERROR (Status)) {
    *BufferSize = 0;

    REPORT_STATUS_CODE_WITH_DEVICE_PATH (
      EFI_ERROR_CODE,
      EFI_P_EC_INPUT_ERROR | EFI_PERIPHERAL_SERIAL_PORT,
      SerialDevice->DevicePath
      );

    gBS->RestoreTPL (Tpl);

    return EFI_DEVICE_ERROR;
  }

  CharBuffer = (UINT8 *) Buffer;
  for (Index = 0; Index < *BufferSize; Index++) {
    while (Uart16550FifoRemove (&SerialDevice->Receive, &(CharBuffer[Index])) != EFI_SUCCESS) {

      if (GetSwFcState (SerialDevice , HOST_SEND) == XOFF) {
        SetSoftFlowControl (SerialDevice, XON);
      }

      //
      //  Unsuccessful read so check if timeout has expired, if not,
      //  stall for a bit, increment time elapsed, and try again
      //  Need this time out to get conspliter to work.
      //
      if (Elapsed >= This->Mode->Timeout) {
        *BufferSize = Index;
        gBS->RestoreTPL (Tpl);
        return EFI_TIMEOUT;
      }

      gBS->Stall (TIMEOUT_STALL_INTERVAL);
      Elapsed += TIMEOUT_STALL_INTERVAL;

      Status = Uart16550ReceiveTransmit (SerialDevice);
      if (Status == EFI_DEVICE_ERROR) {
        *BufferSize = Index;
        gBS->RestoreTPL (Tpl);
        return EFI_DEVICE_ERROR;
      }
    }
    //
    //  Successful read so reset timeout
    //
    Elapsed = 0;
  }

  Uart16550ReceiveTransmit (SerialDevice);

  gBS->RestoreTPL (Tpl);

  return EFI_SUCCESS;
}

/**
  Use Uart16550Access protocol to read serial port.

  @param IsaIo         Pointer to H2O_UART_16550_ACCESS_PROTOCOL instance
  @param Offset        Offset in register group

  @return Data read from serial port

**/
UINT8
Uart16550ReadRegister (
  IN H2O_UART_16550_ACCESS_PROTOCOL   *Uart16550,
  IN UINT16                           Index
  )
{
  UINT8 Data;

  //
  // Use IsaIo to access IO
  //
  Uart16550->RegRead ( Uart16550, Index, &Data);

  return Data;
}

/**
  Use IsaIo protocol to write serial port.

  @param  IsaIo         Pointer to EFI_ISA_IO_PROTOCOL instance
  @param  Offset        Offset in register group
  @param  Data          data which is to be written to some serial port register

**/
VOID
Uart16550WriteRegister (
  IN H2O_UART_16550_ACCESS_PROTOCOL   *Uart16550,
  IN UINT16                           Index,
  IN UINT8                            Data
  )
{
  //
  // Use IsaIo to access IO
  //
  Uart16550->RegWrite ( Uart16550, Index, Data);

}

