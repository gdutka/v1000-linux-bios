/** @file

;******************************************************************************
;* Copyright (c) 2012 - 2015, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

/**
  handles console redirection from boot manager

Copyright (c) 2004 - 2010, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "BootMaint.h"

STATIC UART_FLOW_CONTROL_DEVICE_PATH mFlowControlDevicePath =

{
  MESSAGING_DEVICE_PATH,
  MSG_VENDOR_DP,
  (UINT8)(sizeof(UART_FLOW_CONTROL_DEVICE_PATH)),
  (UINT8)((sizeof(UART_FLOW_CONTROL_DEVICE_PATH)) >> 8),
  DEVICE_PATH_MESSAGING_UART_FLOW_CONTROL,
  UART_FLOW_CONTROL_HARDWARE
};

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
  Check whether the device path node is ISA Serial Node.

  @param Acpi           Device path node to be checked

  @retval TRUE          It's ISA Serial Node.
  @retval FALSE         It's NOT ISA Serial Node.

**/
BOOLEAN
IsIsaSerialNode (
  IN ACPI_HID_DEVICE_PATH *Acpi
  )
{
  return (BOOLEAN) (
      (DevicePathType (Acpi) == ACPI_DEVICE_PATH) &&
      (DevicePathSubType (Acpi) == ACPI_DP) &&
      (ReadUnaligned32 (&Acpi->HID) == EISA_PNP_ID (0x0501))
      );
}

/**
  Update Com Ports attributes from DevicePath

  @param DevicePath      DevicePath that contains Com ports

  @retval EFI_SUCCESS   The update is successful.

**/
EFI_STATUS
UpdateComAttributeFromVariable (
  EFI_DEVICE_PATH_PROTOCOL  *DevicePath
  );

/**
  Update the multi-instance device path of Terminal Device based on
  the global TerminalMenu. If ChangeTernimal is TRUE, the terminal
  device path in the Terminal Device in TerminalMenu is also updated.

  @param DevicePath      The multi-instance device path.
  @param ChangeTerminal  TRUE, then device path in the Terminal Device
                         in TerminalMenu is also updated; FALSE, no update.

  @return EFI_SUCCESS    The function completes successfully.

**/
EFI_STATUS
ChangeTerminalDevicePath (
  IN OUT    EFI_DEVICE_PATH_PROTOCOL  **DevicePath,
  IN        BOOLEAN                   ChangeTerminal
  )
{
  EFI_DEVICE_PATH_PROTOCOL  *Node;
  EFI_DEVICE_PATH_PROTOCOL  *Node1;
  ACPI_HID_DEVICE_PATH      *Acpi;
  UART_DEVICE_PATH          *Uart;
  UART_DEVICE_PATH          *Uart1;
  UINTN                     Com;
  BM_TERMINAL_CONTEXT       *NewTerminalContext;
  BM_MENU_ENTRY             *NewMenuEntry;
  UART_FLOW_CONTROL_DEVICE_PATH *FlowControlNode;

  Node  = *DevicePath;
  Node  = NextDevicePathNode (Node);
  Com   = 0;
  while (!IsDevicePathEnd (Node)) {
    Acpi = (ACPI_HID_DEVICE_PATH *) Node;
    if (IsIsaSerialNode (Acpi)) {
      CopyMem (&Com, &Acpi->UID, sizeof (UINT32));
    }

    NewMenuEntry = BOpt_GetMenuEntry (&TerminalMenu, Com);

    NewTerminalContext = (BM_TERMINAL_CONTEXT *) NewMenuEntry->VariableContext;
    if ((DevicePathType (Node) == MESSAGING_DEVICE_PATH) && (DevicePathSubType (Node) == MSG_UART_DP)) {
      Uart = (UART_DEVICE_PATH *) Node;
      CopyMem (
        &Uart->BaudRate,
        &NewTerminalContext->BaudRate,
        sizeof (UINT64)
        );

      CopyMem (
        &Uart->DataBits,
        &NewTerminalContext->DataBits,
        sizeof (UINT8)
        );

      CopyMem (
        &Uart->Parity,
        &NewTerminalContext->Parity,
        sizeof (UINT8)
        );

      CopyMem (
        &Uart->StopBits,
        &NewTerminalContext->StopBits,
        sizeof (UINT8)
        );

      FlowControlNode = (UART_FLOW_CONTROL_DEVICE_PATH *) NextDevicePathNode (Node);
      if (IsUartFlowControlNode (FlowControlNode)) {
        FlowControlNode->FlowControlMap = NewTerminalContext->FlowControl;
      } else {
        //
        // Append the Flow control device node when user enable flow control.
        //
        if (NewTerminalContext->FlowControl != 0) {
          mFlowControlDevicePath.FlowControlMap = NewTerminalContext->FlowControl;
          *DevicePath = AppendDevicePathNode (
                                       *DevicePath,
                                       (EFI_DEVICE_PATH_PROTOCOL *) (&mFlowControlDevicePath)
                                       );
        }
      }

      //
      // Change the device path in the ComPort
      //
      if (ChangeTerminal) {
        Node1 = NewTerminalContext->DevicePath;
        Node1 = NextDevicePathNode (Node1);
        while (!IsDevicePathEnd (Node1)) {
          if ((DevicePathType (Node1) == MESSAGING_DEVICE_PATH) && (DevicePathSubType (Node1) == MSG_UART_DP)) {
            Uart1 = (UART_DEVICE_PATH *) Node1;
            CopyMem (
              &Uart1->BaudRate,
              &NewTerminalContext->BaudRate,
              sizeof (UINT64)
              );

            CopyMem (
              &Uart1->DataBits,
              &NewTerminalContext->DataBits,
              sizeof (UINT8)
              );

            CopyMem (
              &Uart1->Parity,
              &NewTerminalContext->Parity,
              sizeof (UINT8)
              );

            CopyMem (
              &Uart1->StopBits,
              &NewTerminalContext->StopBits,
              sizeof (UINT8)
              );
            break;
          }
          //
          // end if
          //
          Node1 = NextDevicePathNode (Node1);
        }
        //
        // end while
        //
        break;
      }
    }

    Node = NextDevicePathNode (Node);
  }

  return EFI_SUCCESS;
}

/**
  Update the device path that describing a terminal device
  based on the new BaudRate, Data Bits, parity and Stop Bits
  set.

  @param DevicePath terminal device's path

**/
VOID
ChangeVariableDevicePath (
  IN OUT EFI_DEVICE_PATH_PROTOCOL  *DevicePath
  )
{
  EFI_DEVICE_PATH_PROTOCOL  *Node;
  ACPI_HID_DEVICE_PATH      *Acpi;
  UART_DEVICE_PATH          *Uart;
  UINTN                     Com;
  BM_TERMINAL_CONTEXT       *NewTerminalContext;
  BM_MENU_ENTRY             *NewMenuEntry;

  Node  = DevicePath;
  Node  = NextDevicePathNode (Node);
  Com   = 0;
  while (!IsDevicePathEnd (Node)) {
    Acpi = (ACPI_HID_DEVICE_PATH *) Node;
    if (IsIsaSerialNode (Acpi)) {
      CopyMem (&Com, &Acpi->UID, sizeof (UINT32));
    }

    if ((DevicePathType (Node) == MESSAGING_DEVICE_PATH) && (DevicePathSubType (Node) == MSG_UART_DP)) {
      NewMenuEntry = BOpt_GetMenuEntry (
                      &TerminalMenu,
                      Com
                      );
      ASSERT (NewMenuEntry != NULL);
      NewTerminalContext  = (BM_TERMINAL_CONTEXT *) NewMenuEntry->VariableContext;
      Uart                = (UART_DEVICE_PATH *) Node;
      CopyMem (
        &Uart->BaudRate,
        &NewTerminalContext->BaudRate,
        sizeof (UINT64)
        );

      CopyMem (
        &Uart->DataBits,
        &NewTerminalContext->DataBits,
        sizeof (UINT8)
        );

      CopyMem (
        &Uart->Parity,
        &NewTerminalContext->Parity,
        sizeof (UINT8)
        );

      CopyMem (
        &Uart->StopBits,
        &NewTerminalContext->StopBits,
        sizeof (UINT8)
        );
    }

    Node = NextDevicePathNode (Node);
  }
}

/**
  Retrieve ACPI UID of UART from device path

  @param Handle          The handle for the UART device.
  @param AcpiUid         The ACPI UID on output.

  @retval  TRUE   Find valid UID from device path
  @retval  FALSE  Can't find

**/
BOOLEAN
RetrieveUartUid (
  IN EFI_HANDLE   Handle,
  IN OUT UINT32   *AcpiUid
  )
{
  EFI_STATUS                Status;
  ACPI_HID_DEVICE_PATH      *Acpi;
  EFI_DEVICE_PATH_PROTOCOL  *DevicePath;

  Status = gBS->HandleProtocol (
                  Handle,
                  &gEfiDevicePathProtocolGuid,
                  (VOID **) &DevicePath
                  );
  if (EFI_ERROR (Status)) {
    return FALSE;
  }

  Acpi = NULL;
  for (; !IsDevicePathEnd (DevicePath); DevicePath = NextDevicePathNode (DevicePath)) {
    if ((DevicePathType (DevicePath) == MESSAGING_DEVICE_PATH) && (DevicePathSubType (DevicePath) == MSG_UART_DP)) {
      break;
    }
    //
    // Acpi points to the node before the Uart node
    //
    Acpi = (ACPI_HID_DEVICE_PATH *) DevicePath;
  }

  if ((Acpi != NULL) && IsIsaSerialNode (Acpi)) {
    if (AcpiUid != NULL) {
      CopyMem (AcpiUid, &Acpi->UID, sizeof (UINT32));
    }
    return TRUE;
  } else {
    return FALSE;
  }
}

/**
  Sort Uart handles array with Acpi->UID from low to high.

  @param Handles         EFI_SERIAL_IO_PROTOCOL handle buffer
  @param NoHandles       EFI_SERIAL_IO_PROTOCOL handle count
**/
VOID
SortedUartHandle (
  IN  EFI_HANDLE *Handles,
  IN  UINTN      NoHandles
  )
{
  UINTN       Index1;
  UINTN       Index2;
  UINTN       Position;
  UINT32      AcpiUid1;
  UINT32      AcpiUid2;
  UINT32      TempAcpiUid;
  EFI_HANDLE  TempHandle;

  for (Index1 = 0; Index1 < NoHandles-1; Index1++) {
    if (!RetrieveUartUid (Handles[Index1], &AcpiUid1)) {
      continue;
    }
    TempHandle  = Handles[Index1];
    Position    = Index1;
    TempAcpiUid = AcpiUid1;

    for (Index2 = Index1+1; Index2 < NoHandles; Index2++) {
      if (!RetrieveUartUid (Handles[Index2], &AcpiUid2)) {
        continue;
      }
      if (AcpiUid2 < TempAcpiUid) {
        TempAcpiUid = AcpiUid2;
        TempHandle  = Handles[Index2];
        Position    = Index2;
      }
    }
    Handles[Position] = Handles[Index1];
    Handles[Index1]   = TempHandle;
  }
}

/**
  Test whether DevicePath is a valid Terminal


  @param DevicePath      DevicePath to be checked
  @param Termi           If DevicePath is valid Terminal, terminal type is returned.
  @param Com             If DevicePath is valid Terminal, Com Port type is returned.

  @retval  TRUE         If DevicePath point to a Terminal.
  @retval  FALSE        If DevicePath does not point to a Terminal.

**/
BOOLEAN
IsTerminalDevicePath (
  IN  EFI_DEVICE_PATH_PROTOCOL *DevicePath,
  OUT TYPE_OF_TERMINAL         *Termi,
  OUT UINTN                    *Com
  );

/**
  Get a string from the Data Hub record based on
  a device path.

  @param DevPath         The device Path.

  @return A string located from the Data Hub records based on
          the device path.
  @retval NULL  If failed to get the String from Data Hub.

**/
UINT16 *
StrFromDatahub (
  IN EFI_DEVICE_PATH_PROTOCOL                 *DevPath
  )
{
  return NULL;
}

/**
  Build a list containing all serial devices.


  @retval EFI_SUCCESS The function complete successfully.
  @retval EFI_UNSUPPORTED No serial ports present.

**/
EFI_STATUS
LocateSerialIo (
  VOID
  )
{
  UINTN                     Index;
  UINTN                     Index2;
  UINTN                     NoHandles;
  EFI_HANDLE                *Handles;
  EFI_STATUS                Status;
  ACPI_HID_DEVICE_PATH      *Acpi;
  EFI_DEVICE_PATH_PROTOCOL  *DevicePath;
  EFI_SERIAL_IO_PROTOCOL    *SerialIo;
  EFI_DEVICE_PATH_PROTOCOL  *Node;
  EFI_DEVICE_PATH_PROTOCOL  *OutDevicePath;
  EFI_DEVICE_PATH_PROTOCOL  *InpDevicePath;
  EFI_DEVICE_PATH_PROTOCOL  *ErrDevicePath;
  BM_MENU_ENTRY             *NewMenuEntry;
  BM_TERMINAL_CONTEXT       *NewTerminalContext;
  EFI_DEVICE_PATH_PROTOCOL  *NewDevicePath;
  VENDOR_DEVICE_PATH        Vendor;
  UINT32                    FlowControl;
  //
  // Get all handles that have SerialIo protocol installed
  //
  InitializeListHead (&TerminalMenu.Head);
  TerminalMenu.MenuNumber = 0;
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiSerialIoProtocolGuid,
                  NULL,
                  &NoHandles,
                  &Handles
                  );
  if (EFI_ERROR (Status)) {
    //
    // No serial ports present
    //
    return EFI_UNSUPPORTED;
  }

  //
  // Sort Uart handles array with Acpi->UID from low to high
  // then Terminal menu can be built from low Acpi->UID to high Acpi->UID
  //
  SortedUartHandle (Handles, NoHandles);

  for (Index = 0; Index < NoHandles; Index++) {
    //
    // Check to see whether the handle has DevicePath Protocol installed
    //
    gBS->HandleProtocol (
          Handles[Index],
          &gEfiDevicePathProtocolGuid,
          (VOID **) &DevicePath
          );

    Acpi = NULL;
    for (Node = DevicePath; !IsDevicePathEnd (Node); Node = NextDevicePathNode (Node)) {
      if ((DevicePathType (Node) == MESSAGING_DEVICE_PATH) && (DevicePathSubType (Node) == MSG_UART_DP)) {
        break;
      }
      //
      // Acpi points to the node before Uart node
      //
      Acpi = (ACPI_HID_DEVICE_PATH *) Node;
    }

    if ((Acpi != NULL) && IsIsaSerialNode (Acpi)) {
      NewMenuEntry = BOpt_CreateMenuEntry (BM_TERMINAL_CONTEXT_SELECT);
      if (NewMenuEntry == NULL) {
        FreePool (Handles);
        return EFI_OUT_OF_RESOURCES;
      }

      NewTerminalContext = (BM_TERMINAL_CONTEXT *) NewMenuEntry->VariableContext;
      CopyMem (&NewMenuEntry->OptionNumber, &Acpi->UID, sizeof (UINT32));
      NewTerminalContext->DevicePath = DuplicateDevicePath (DevicePath);
      //
      // BugBug: I have no choice, calling EfiLibStrFromDatahub will hang the system!
      // coz' the misc data for each platform is not correct, actually it's the device path stored in
      // datahub which is not completed, so a searching for end of device path will enter a
      // dead-loop.
      //
      NewMenuEntry->DisplayString = StrFromDatahub (DevicePath);
      if (NULL == NewMenuEntry->DisplayString) {
        NewMenuEntry->DisplayString = DevicePathToStr (DevicePath);
      }

      NewMenuEntry->HelpString = NULL;

      gBS->HandleProtocol (
            Handles[Index],
            &gEfiSerialIoProtocolGuid,
            (VOID **) &SerialIo
            );

      CopyMem (
        &NewTerminalContext->BaudRate,
        &SerialIo->Mode->BaudRate,
        sizeof (UINT64)
        );

      CopyMem (
        &NewTerminalContext->DataBits,
        &SerialIo->Mode->DataBits,
        sizeof (UINT8)
        );

      CopyMem (
        &NewTerminalContext->Parity,
        &SerialIo->Mode->Parity,
        sizeof (UINT8)
        );

      CopyMem (
        &NewTerminalContext->StopBits,
        &SerialIo->Mode->StopBits,
        sizeof (UINT8)
        );

      NewTerminalContext->FlowControl = 0;
      SerialIo->GetControl(SerialIo, &FlowControl);
      if ((FlowControl & EFI_SERIAL_HARDWARE_FLOW_CONTROL_ENABLE) != 0) {
        NewTerminalContext->FlowControl = UART_FLOW_CONTROL_HARDWARE;
      }

      InsertTailList (&TerminalMenu.Head, &NewMenuEntry->Link);
      TerminalMenu.MenuNumber++;
    }
  }
  if (Handles != NULL) {
    FreePool (Handles);
  }

  //
  // Get L"ConOut", L"ConIn" and L"ErrOut" from the Var
  //
  OutDevicePath = BootMaintGetVariable (L"ConOut", &gEfiGlobalVariableGuid);
  InpDevicePath = BootMaintGetVariable (L"ConIn", &gEfiGlobalVariableGuid);
  ErrDevicePath = BootMaintGetVariable (L"ErrOut", &gEfiGlobalVariableGuid);
  if (OutDevicePath != NULL) {
    UpdateComAttributeFromVariable (OutDevicePath);
  }

  if (InpDevicePath != NULL) {
    UpdateComAttributeFromVariable (InpDevicePath);
  }

  if (ErrDevicePath != NULL) {
    UpdateComAttributeFromVariable (ErrDevicePath);
  }

  for (Index = 0; Index < TerminalMenu.MenuNumber; Index++) {
    NewMenuEntry = BOpt_GetMenuEntry (&TerminalMenu, Index);
    if (NULL == NewMenuEntry) {
      return EFI_NOT_FOUND;
    }

    NewTerminalContext                = (BM_TERMINAL_CONTEXT *) NewMenuEntry->VariableContext;

    NewTerminalContext->TerminalType  = 0;
    NewTerminalContext->IsConIn       = FALSE;
    NewTerminalContext->IsConOut      = FALSE;
    NewTerminalContext->IsStdErr      = FALSE;

    Vendor.Header.Type                = MESSAGING_DEVICE_PATH;
    Vendor.Header.SubType             = MSG_VENDOR_DP;

    for (Index2 = 0; Index2 < 4; Index2++) {
      CopyMem (&Vendor.Guid, &TerminalTypeGuid[Index2], sizeof (EFI_GUID));
      SetDevicePathNodeLength (&Vendor.Header, sizeof (VENDOR_DEVICE_PATH));
      NewDevicePath = AppendDevicePathNode (
                        NewTerminalContext->DevicePath,
                        (EFI_DEVICE_PATH_PROTOCOL *) &Vendor
                        );
      if (NewMenuEntry->HelpString != NULL) {
        FreePool (NewMenuEntry->HelpString);
      }
      //
      // NewMenuEntry->HelpString = DevicePathToStr (NewDevicePath);
      // NewMenuEntry->DisplayString = NewMenuEntry->HelpString;
      //
      NewMenuEntry->HelpString = NULL;

      if (BdsLibMatchDevicePaths (OutDevicePath, NewDevicePath)) {
        NewTerminalContext->IsConOut      = TRUE;
        NewTerminalContext->TerminalType  = (UINT8) Index2;
      }

      if (BdsLibMatchDevicePaths (InpDevicePath, NewDevicePath)) {
        NewTerminalContext->IsConIn       = TRUE;
        NewTerminalContext->TerminalType  = (UINT8) Index2;
      }

      if (BdsLibMatchDevicePaths (ErrDevicePath, NewDevicePath)) {
        NewTerminalContext->IsStdErr      = TRUE;
        NewTerminalContext->TerminalType  = (UINT8) Index2;
      }
    }
  }

  return EFI_SUCCESS;
}

/**
  Update Com Ports attributes from DevicePath

  @param DevicePath      DevicePath that contains Com ports

  @retval EFI_SUCCESS   The update is successful.
  @retval EFI_NOT_FOUND Can not find specific menu entry
**/
EFI_STATUS
UpdateComAttributeFromVariable (
  EFI_DEVICE_PATH_PROTOCOL  *DevicePath
  )
{
  EFI_DEVICE_PATH_PROTOCOL  *Node;
  EFI_DEVICE_PATH_PROTOCOL  *SerialNode;
  ACPI_HID_DEVICE_PATH      *Acpi;
  UART_DEVICE_PATH          *Uart;
  UART_DEVICE_PATH          *Uart1;
  UINTN                     TerminalNumber;
  BM_MENU_ENTRY             *NewMenuEntry;
  BM_TERMINAL_CONTEXT       *NewTerminalContext;
  UINTN                     Index;
  UART_FLOW_CONTROL_DEVICE_PATH *FlowControlNode;
  BOOLEAN                   HasFlowControlNode;

  HasFlowControlNode = FALSE;
  Node            = DevicePath;
  Node            = NextDevicePathNode (Node);
  TerminalNumber  = 0;
  for (Index = 0; Index < TerminalMenu.MenuNumber; Index++) {
    while (!IsDevicePathEnd (Node)) {
      Acpi = (ACPI_HID_DEVICE_PATH *) Node;
      if (IsIsaSerialNode (Acpi)) {
        CopyMem (&TerminalNumber, &Acpi->UID, sizeof (UINT32));
      }

      if ((DevicePathType (Node) == MESSAGING_DEVICE_PATH) && (DevicePathSubType (Node) == MSG_UART_DP)) {
        Uart          = (UART_DEVICE_PATH *) Node;
        NewMenuEntry  = BOpt_GetMenuEntry (&TerminalMenu, TerminalNumber);
        if (NULL == NewMenuEntry) {
          return EFI_NOT_FOUND;
        }

        NewTerminalContext = (BM_TERMINAL_CONTEXT *) NewMenuEntry->VariableContext;
        CopyMem (
          &NewTerminalContext->BaudRate,
          &Uart->BaudRate,
          sizeof (UINT64)
          );

        CopyMem (
          &NewTerminalContext->DataBits,
          &Uart->DataBits,
          sizeof (UINT8)
          );

        CopyMem (
          &NewTerminalContext->Parity,
          &Uart->Parity,
          sizeof (UINT8)
          );

        CopyMem (
          &NewTerminalContext->StopBits,
          &Uart->StopBits,
          sizeof (UINT8)
          );

        FlowControlNode = (UART_FLOW_CONTROL_DEVICE_PATH *) NextDevicePathNode (Node);
        if (IsUartFlowControlNode (FlowControlNode)) {
          HasFlowControlNode = TRUE;
          NewTerminalContext->FlowControl = (UINT8) ReadUnaligned32 (&FlowControlNode->FlowControlMap);
        } else if (NewTerminalContext->FlowControl != 0) {
          //
          // No Flow Control device path node, assumption no Flow control
          //
          NewTerminalContext->FlowControl = 0;
        }

        SerialNode  = NewTerminalContext->DevicePath;
        SerialNode  = NextDevicePathNode (SerialNode);
        while (!IsDevicePathEnd (SerialNode)) {
          if ((DevicePathType (SerialNode) == MESSAGING_DEVICE_PATH) && (DevicePathSubType (SerialNode) == MSG_UART_DP)) {
            //
            // Update following device paths according to
            // previous acquired uart attributes
            //
            Uart1 = (UART_DEVICE_PATH *) SerialNode;
            CopyMem (
              &Uart1->BaudRate,
              &NewTerminalContext->BaudRate,
              sizeof (UINT64)
              );

            CopyMem (
              &Uart1->DataBits,
              &NewTerminalContext->DataBits,
              sizeof (UINT8)
              );
            CopyMem (
              &Uart1->Parity,
              &NewTerminalContext->Parity,
              sizeof (UINT8)
              );
            CopyMem (
              &Uart1->StopBits,
              &NewTerminalContext->StopBits,
              sizeof (UINT8)
              );

            FlowControlNode = (UART_FLOW_CONTROL_DEVICE_PATH *) NextDevicePathNode (SerialNode);
            if (IsUartFlowControlNode (FlowControlNode)) {
              FlowControlNode->FlowControlMap = NewTerminalContext->FlowControl;
            } else {
              if (HasFlowControlNode) {
                mFlowControlDevicePath.FlowControlMap = NewTerminalContext->FlowControl;
                NewTerminalContext->DevicePath = AppendDevicePathNode (
                                             NewTerminalContext->DevicePath,
                                             (EFI_DEVICE_PATH_PROTOCOL *) (&mFlowControlDevicePath)
                                             );
              }
            }
            break;
          }

          SerialNode = NextDevicePathNode (SerialNode);
        }
        //
        // end while
        //
      }

      Node = NextDevicePathNode (Node);
    }
    //
    // end while
    //
  }

  return EFI_SUCCESS;
}

/**
  Function is used to determine the number of device path instances
  that exist in a device path.


  @param DevicePath      A pointer to a device path data structure.

  @return This function counts and returns the number of device path instances
          in DevicePath.

**/
UINTN
DevicePathInstanceCount (
  IN EFI_DEVICE_PATH_PROTOCOL      *DevicePath
  )
{
  UINTN Count;
  UINTN Size;

  Count = 0;
  while (GetNextDevicePathInstance (&DevicePath, &Size) != NULL) {
    Count += 1;
  }

  return Count;
}

/**
  Build up Console Menu based on types passed in. The type can
  be BM_CONSOLE_IN_CONTEXT_SELECT, BM_CONSOLE_OUT_CONTEXT_SELECT
  and BM_CONSOLE_ERR_CONTEXT_SELECT.

  @param ConsoleMenuType Can be BM_CONSOLE_IN_CONTEXT_SELECT, BM_CONSOLE_OUT_CONTEXT_SELECT
                         and BM_CONSOLE_ERR_CONTEXT_SELECT.

  @retval EFI_UNSUPPORTED The type passed in is not in the 3 types defined.
  @retval EFI_NOT_FOUND   If the EFI Variable defined in UEFI spec with name "ConOutDev",
                          "ConInDev" or "ConErrDev" doesn't exists.
  @retval EFI_OUT_OF_RESOURCES Not enough resource to complete the operations.
  @retval EFI_SUCCESS          Function completes successfully.

**/
EFI_STATUS
GetConsoleMenu (
  IN UINTN              ConsoleMenuType
  )
{
  EFI_DEVICE_PATH_PROTOCOL  *DevicePath;
  EFI_DEVICE_PATH_PROTOCOL  *AllDevicePath;
  EFI_DEVICE_PATH_PROTOCOL  *MultiDevicePath;
  EFI_DEVICE_PATH_PROTOCOL  *DevicePathInst;
  UINTN                     Size;
  UINTN                     AllCount;
  UINTN                     Index;
  UINTN                     Index2;
  BM_MENU_ENTRY             *NewMenuEntry;
  BM_CONSOLE_CONTEXT        *NewConsoleContext;
  TYPE_OF_TERMINAL          Terminal;
  UINTN                     Com;
  BM_MENU_OPTION            *ConsoleMenu;

  DevicePath    = NULL;
  AllDevicePath = NULL;
  AllCount      = 0;
  switch (ConsoleMenuType) {
  case BM_CONSOLE_IN_CONTEXT_SELECT:
    ConsoleMenu = &ConsoleInpMenu;
    DevicePath = BootMaintGetVariable (
                  L"ConIn",
                  &gEfiGlobalVariableGuid
                  );

    AllDevicePath = BootMaintGetVariable (
                      L"ConInDev",
                      &gEfiGlobalVariableGuid
                      );
    break;

  case BM_CONSOLE_OUT_CONTEXT_SELECT:
    ConsoleMenu = &ConsoleOutMenu;
    DevicePath = BootMaintGetVariable (
                  L"ConOut",
                  &gEfiGlobalVariableGuid
                  );

    AllDevicePath = BootMaintGetVariable (
                      L"ConOutDev",
                      &gEfiGlobalVariableGuid
                      );
    break;

  case BM_CONSOLE_ERR_CONTEXT_SELECT:
    ConsoleMenu = &ConsoleErrMenu;
    DevicePath = BootMaintGetVariable (
                  L"ErrOut",
                  &gEfiGlobalVariableGuid
                  );

    AllDevicePath = BootMaintGetVariable (
                      L"ErrOutDev",
                      &gEfiGlobalVariableGuid
                      );
    break;

  default:
    return EFI_UNSUPPORTED;
  }

  if (NULL == AllDevicePath) {
    return EFI_NOT_FOUND;
  }

  InitializeListHead (&ConsoleMenu->Head);

  AllCount                = DevicePathInstanceCount (AllDevicePath);
  ConsoleMenu->MenuNumber = 0;
  //
  // Following is menu building up for Console Devices selected.
  //
  MultiDevicePath = AllDevicePath;
  Index2          = 0;
  for (Index = 0; Index < AllCount; Index++) {
    DevicePathInst  = GetNextDevicePathInstance (&MultiDevicePath, &Size);

    NewMenuEntry    = BOpt_CreateMenuEntry (BM_CONSOLE_CONTEXT_SELECT);
    if (NULL == NewMenuEntry) {
      return EFI_OUT_OF_RESOURCES;
    }

    NewConsoleContext             = (BM_CONSOLE_CONTEXT *) NewMenuEntry->VariableContext;
    NewMenuEntry->OptionNumber    = Index2;

    NewConsoleContext->DevicePath = DuplicateDevicePath (DevicePathInst);
    ASSERT (NewConsoleContext->DevicePath != NULL);
    if (NewConsoleContext->DevicePath == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
    NewMenuEntry->DisplayString   = StrFromDatahub (NewConsoleContext->DevicePath);
    if (NULL == NewMenuEntry->DisplayString) {
      NewMenuEntry->DisplayString = DevicePathToStr (NewConsoleContext->DevicePath);
    }

    NewConsoleContext->IsTerminal = IsTerminalDevicePath (
                                      NewConsoleContext->DevicePath,
                                      &Terminal,
                                      &Com
                                      );

    NewConsoleContext->IsActive = BdsLibMatchDevicePaths (
                                    DevicePath,
                                    NewConsoleContext->DevicePath
                                    );

    if (NewConsoleContext->IsTerminal) {
      BOpt_DestroyMenuEntry (NewMenuEntry);
    } else {
      Index2++;
      ConsoleMenu->MenuNumber++;
      InsertTailList (&ConsoleMenu->Head, &NewMenuEntry->Link);
    }
  }

  return EFI_SUCCESS;
}

/**
  Build up ConsoleOutMenu, ConsoleInpMenu and ConsoleErrMenu

  @retval EFI_SUCCESS    The function always complete successfully.

**/
EFI_STATUS
GetAllConsoles (
  VOID
  )
{
  GetConsoleMenu (BM_CONSOLE_IN_CONTEXT_SELECT);
  GetConsoleMenu (BM_CONSOLE_OUT_CONTEXT_SELECT);
  GetConsoleMenu (BM_CONSOLE_ERR_CONTEXT_SELECT);
  return EFI_SUCCESS;
}

/**
  Free ConsoleOutMenu, ConsoleInpMenu and ConsoleErrMenu

  @retval EFI_SUCCESS    The function always complete successfully.
**/
EFI_STATUS
FreeAllConsoles (
  VOID
  )
{
  BOpt_FreeMenu (&ConsoleOutMenu);
  BOpt_FreeMenu (&ConsoleInpMenu);
  BOpt_FreeMenu (&ConsoleErrMenu);
  BOpt_FreeMenu (&TerminalMenu);
  return EFI_SUCCESS;
}

/**
  Test whether DevicePath is a valid Terminal


  @param DevicePath      DevicePath to be checked
  @param Termi           If DevicePath is valid Terminal, terminal type is returned.
  @param Com             If DevicePath is valid Terminal, Com Port type is returned.

  @retval  TRUE         If DevicePath point to a Terminal.
  @retval  FALSE        If DevicePath does not point to a Terminal.

**/
BOOLEAN
IsTerminalDevicePath (
  IN  EFI_DEVICE_PATH_PROTOCOL *DevicePath,
  OUT TYPE_OF_TERMINAL         *Termi,
  OUT UINTN                    *Com
  )
{
  BOOLEAN                   IsTerminal;
  EFI_DEVICE_PATH_PROTOCOL  *Node;
  VENDOR_DEVICE_PATH        *Vendor;
  UART_DEVICE_PATH          *Uart;
  ACPI_HID_DEVICE_PATH      *Acpi;

  IsTerminal = FALSE;

  Uart   = NULL;
  Vendor = NULL;
  Acpi   = NULL;
  for (Node = DevicePath; !IsDevicePathEnd (Node); Node = NextDevicePathNode (Node)) {
    //
    // Vendor points to the node before the End node
    //
    Vendor = (VENDOR_DEVICE_PATH *) Node;

    if ((DevicePathType (Node) == MESSAGING_DEVICE_PATH) && (DevicePathSubType (Node) == MSG_UART_DP)) {
      Uart = (UART_DEVICE_PATH *) Node;
    }

    if (Uart == NULL) {
      //
      // Acpi points to the node before the UART node
      //
      Acpi = (ACPI_HID_DEVICE_PATH *) Node;
    }
  }

  if (Vendor == NULL ||
      DevicePathType (Vendor) != MESSAGING_DEVICE_PATH ||
      DevicePathSubType (Vendor) != MSG_VENDOR_DP ||
      Uart == NULL) {
    return FALSE;
  }

  //
  // There are four kinds of Terminal types
  // check to see whether this devicepath
  // is one of that type
  //
  if (CompareGuid (&Vendor->Guid, &TerminalTypeGuid[0])) {
    *Termi      = TerminalTypePcAnsi;
    IsTerminal  = TRUE;
  } else {
    if (CompareGuid (&Vendor->Guid, &TerminalTypeGuid[1])) {
      *Termi      = TerminalTypeVt100;
      IsTerminal  = TRUE;
    } else {
      if (CompareGuid (&Vendor->Guid, &TerminalTypeGuid[2])) {
        *Termi      = TerminalTypeVt100Plus;
        IsTerminal  = TRUE;
      } else {
        if (CompareGuid (&Vendor->Guid, &TerminalTypeGuid[3])) {
          *Termi      = TerminalTypeVtUtf8;
          IsTerminal  = TRUE;
        } else {
          IsTerminal = FALSE;
        }
      }
    }
  }

  if (!IsTerminal) {
    return FALSE;
  }

  if ((Acpi != NULL) && IsIsaSerialNode (Acpi)) {
    CopyMem (Com, &Acpi->UID, sizeof (UINT32));
  } else {
    return FALSE;
  }

  return TRUE;
}

/**
  Get mode number according to column and row

  @param CallbackData    The BMM context data.
**/
VOID
GetConsoleOutMode (
  IN  BMM_CALLBACK_DATA    *CallbackData
  )
{
  UINTN                         Col;
  UINTN                         Row;
  UINTN                         CurrentCol;
  UINTN                         CurrentRow;
  UINTN                         Mode;
  UINTN                         MaxMode;
  EFI_STATUS                    Status;
  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL  *ConOut;

  ConOut   = gST->ConOut;
  MaxMode  = (UINTN) (ConOut->Mode->MaxMode);

  CurrentCol = PcdGet32 (PcdSetupConOutColumn);
  CurrentRow = PcdGet32 (PcdSetupConOutRow);
  for (Mode = 0; Mode < MaxMode; Mode++) {
    Status = ConOut->QueryMode (ConOut, Mode, &Col, &Row);
    if (!EFI_ERROR(Status)) {
      if (CurrentCol == Col && CurrentRow == Row) {
        CallbackData->BmmFakeNvData.ConsoleOutMode = (UINT16) Mode;
        break;
      }
    }
  }
}

