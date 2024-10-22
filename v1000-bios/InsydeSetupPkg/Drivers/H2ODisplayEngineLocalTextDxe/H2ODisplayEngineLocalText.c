/** @file
  Entry point and initial functions for H2O local text display engine driver

;******************************************************************************
;* Copyright (c) 2013 - 2017, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "H2ODisplayEngineLocalText.h"
#include "LTDEPrint.h"

BOOLEAN                                    mIsInNotifyProcess;
H2O_DISPLAY_ENGINE_PRIVATE_DATA            *mDEPrivate;

/**
  Get the corresponding mode number of input resolution for specific simple text output device.

  @param[in] SimpleTextOut             A pointer to the EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL instance.
  @param[in] TargetColumn              Target column value
  @param[in] TargetRow                 Target row value

  @return The mode number or -1 if the input resolution is not supported by simple text output device.
**/
STATIC
INT32
GetModeNum (
  IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL          *SimpleTextOut,
  IN UINTN                                    TargetColumn,
  IN UINTN                                    TargetRow
  )
{
  EFI_STATUS                                  Status;
  INT32                                       Mode;
  UINTN                                       Columns;
  UINTN                                       Rows;

  for (Mode = 0; Mode < SimpleTextOut->Mode->MaxMode; Mode++) {
    Status = SimpleTextOut->QueryMode (SimpleTextOut, Mode, &Columns, &Rows);
    if (!EFI_ERROR (Status) && Columns == TargetColumn && Rows == TargetRow) {
      return Mode;
    }
  }

  return -1;
}

/**
  Get the supported resolution for all attached console output device.

  @param[in]  Private                  Pointer to display engine private data
  @param[out] SupportedColumn          Pointer to supported column value
  @param[out] SupportedRow             Pointer to supported row value

  @retval EFI_SUCCESS                  Get the supported resolution successfully.
  @retval EFI_NOT_FOUND                There is no attached console device or fail to find the supported resolution.
**/
STATIC
EFI_STATUS
GetSupportedResolution (
  IN  H2O_DISPLAY_ENGINE_PRIVATE_DATA         *Private,
  OUT UINTN                                   *SupportedColumn,
  OUT UINTN                                   *SupportedRow
  )
{
  EFI_STATUS                                  Status;
  LIST_ENTRY                                  *Link;
  H2O_FORM_BROWSER_CONSOLE_DEV_NODE           *FirstConDevNode;
  H2O_FORM_BROWSER_CONSOLE_DEV_NODE           *ConDevNode;
  INT32                                       Mode;
  UINTN                                       Columns;
  UINTN                                       Rows;
  BOOLEAN                                     Support;

  if (IsListEmpty (&Private->ConsoleDevListHead)) {
    return EFI_NOT_FOUND;
  }

  Link            = GetFirstNode (&Private->ConsoleDevListHead);
  FirstConDevNode = H2O_FORM_BROWSER_CONSOLE_DEV_NODE_FROM_LINK (Link);

  //
  // Go through all modes of first console device to find out the resolution which are supported for all console devices.
  //
  for (Mode = 0; Mode < FirstConDevNode->SimpleTextOut->Mode->MaxMode; Mode++) {
    Status = FirstConDevNode->SimpleTextOut->QueryMode (FirstConDevNode->SimpleTextOut, Mode, &Columns, &Rows);
    if (EFI_ERROR (Status)) {
      continue;
    }

    Support = TRUE;
    Link = GetNextNode (&Private->ConsoleDevListHead, Link);
    while (!IsNull (&Private->ConsoleDevListHead, Link)) {
      ConDevNode = H2O_FORM_BROWSER_CONSOLE_DEV_NODE_FROM_LINK (Link);
      Link       = GetNextNode (&Private->ConsoleDevListHead, Link);
      if (GetModeNum (ConDevNode->SimpleTextOut, Columns, Rows) == -1) {
        Support = FALSE;
        break;
      }
    }

    if (Support) {
      *SupportedColumn = Columns;
      *SupportedRow    = Rows;
      return EFI_SUCCESS;
    }
  }

  return EFI_NOT_FOUND;
}

/**
  Set all attached consolut output devices to assigned resolution

  @param[in] Private                   Pointer to display engine private data
  @param[in] TargetColumn              Target column value
  @param[in] TargetRow                 Target row value

  @retval EFI_SUCCESS                  Set resolution successsfully.
  @retval EFI_UNSUPPORTED              Resolution is not supported on one console device.
**/
STATIC
EFI_STATUS
SetSimpleTextOutResolution (
  IN H2O_DISPLAY_ENGINE_PRIVATE_DATA          *Private,
  IN UINTN                                    TargetColumn,
  IN UINTN                                    TargetRow
  )
{
  INT32                                       Mode;
  LIST_ENTRY                                  *Link;
  H2O_FORM_BROWSER_CONSOLE_DEV_NODE           *ConDevNode;

  //
  // First, check if all console devices support this resolution.
  // If not, return fail early to make sure all console devices keep the original resolution.
  //
  Link = GetFirstNode (&Private->ConsoleDevListHead);
  while (!IsNull (&Private->ConsoleDevListHead, Link)) {
    ConDevNode = H2O_FORM_BROWSER_CONSOLE_DEV_NODE_FROM_LINK (Link);
    Link       = GetNextNode (&Private->ConsoleDevListHead, Link);

    Mode = GetModeNum (ConDevNode->SimpleTextOut, TargetColumn, TargetRow);
    if (Mode == -1) {
      return EFI_UNSUPPORTED;
    }
  }

  Link = GetFirstNode (&Private->ConsoleDevListHead);
  while (!IsNull (&Private->ConsoleDevListHead, Link)) {
    ConDevNode = H2O_FORM_BROWSER_CONSOLE_DEV_NODE_FROM_LINK (Link);
    Link       = GetNextNode (&Private->ConsoleDevListHead, Link);

    Mode = GetModeNum (ConDevNode->SimpleTextOut, TargetColumn, TargetRow);
    if (Mode != ConDevNode->SimpleTextOut->Mode->Mode) {
      ConDevNode->SimpleTextOut->SetMode (ConDevNode->SimpleTextOut, Mode);
    }
  }

  return EFI_SUCCESS;
}

/**
  Add new console to the console list.

  @param [in] This               A pointer to the H2O_DISPLAY_ENGINE_PROTOCOL instance.
  @param [in] ConsoleHandle      A pointer to the input console handle instance.

  @retval EFI_SUCCESS            Add new console to console list successful.
  @retval EFI_INVALID_PARAMETER  This is NULL, or ConsoleDev is NULL.

**/
EFI_STATUS
EFIAPI
LocalTextInitConsole (
  IN H2O_DISPLAY_ENGINE_PROTOCOL              *This,
  IN H2O_FORM_BROWSER_CONSOLE_DEV             *ConsoleDev
  )
{
  if (This == NULL || ConsoleDev == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  return EFI_SUCCESS;
}

/**
  Attach a specific console to this display engine.

  @param [in] This               A pointer to the H2O_DISPLAY_ENGINE_PROTOCOL instance.
  @param [in] ConsoleDev         A pointer to input H2O_FORM_BROWSER_CONSOLE_DEV instance.

  @retval EFI_SUCCESS            Attach new console successful.
  @retval EFI_INVALID_PARAMETER  This is NULL, or ConsoleDev is NULL.
  @retval EFI_UNSUPPORTED        This display engine doesn't support input console device.

**/
EFI_STATUS
EFIAPI
LocalTextAttachConsole (
  IN H2O_DISPLAY_ENGINE_PROTOCOL              *This,
  IN H2O_FORM_BROWSER_CONSOLE_DEV             *ConsoleDev
  )
{
  H2O_DISPLAY_ENGINE_PRIVATE_DATA             *Private;
  H2O_FORM_BROWSER_CONSOLE_DEV_NODE           *ConDevNode;
  EFI_STATUS                                  Status;
  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL             *SimpleTextOut;
  EFI_GRAPHICS_OUTPUT_PROTOCOL                *GraphicsOut;
  UINT32                                      HorizontalResolution;
  UINT32                                      VerticalResolution;
  UINTN                                       Column;
  UINTN                                       Row;

  if (This == NULL || ConsoleDev == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = gBS->HandleProtocol (ConsoleDev->Handle, &gEfiSimpleTextOutProtocolGuid, (VOID **) &SimpleTextOut);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  Status = gBS->HandleProtocol (ConsoleDev->Handle, &gEfiGraphicsOutputProtocolGuid, (VOID **) &GraphicsOut);
  if (EFI_ERROR (Status)) {
    GraphicsOut = NULL;
  }

  Private = H2O_DISPLAY_ENGINE_PRIVATE_DATA_FROM_PROTOCOL (This);

  ConDevNode = (H2O_FORM_BROWSER_CONSOLE_DEV_NODE *) AllocatePool (sizeof (H2O_FORM_BROWSER_CONSOLE_DEV_NODE));
  if (ConDevNode == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  ConDevNode->Signature     = H2O_FORM_BROWSER_CONSOLE_DEV_NODE_SIGNATURE;
  ConDevNode->SimpleTextOut = SimpleTextOut;
  ConDevNode->GraphicsOut   = GraphicsOut;
  ConDevNode->Handle        = ConsoleDev->Handle;
  ConDevNode->ConDevStatus  = CONSOLE_DEVICE_STATUS_INIT;
  ConDevNode->ConsoleDev    = AllocateCopyPool (sizeof (H2O_FORM_BROWSER_CONSOLE_DEV), ConsoleDev);
  if (ConDevNode->ConsoleDev == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  InsertTailList (&Private->ConsoleDevListHead, &ConDevNode->Link);

  SimpleTextOut->EnableCursor (SimpleTextOut, FALSE);

  GetDisplayEngineResolutionByPcd (&This->Id, &HorizontalResolution, &VerticalResolution);
  Status = SetSimpleTextOutResolution (Private, HorizontalResolution / EFI_GLYPH_WIDTH, VerticalResolution  / EFI_GLYPH_HEIGHT);
  if (!EFI_ERROR (Status)) {
    goto Done;
  }

  Status = SetSimpleTextOutResolution (
             Private,
             PcdGet32 (PcdDefaultHorizontalResolution) / EFI_GLYPH_WIDTH,
             PcdGet32 (PcdDefaultVerticalResolution) / EFI_GLYPH_HEIGHT
             );
  if (!EFI_ERROR (Status)) {
    goto Done;
  }

  Status = GetSupportedResolution (Private, &Column, &Row);
  if (!EFI_ERROR (Status)) {
    Status = SetSimpleTextOutResolution (Private, Column, Row);
    if (!EFI_ERROR (Status)) {
      goto Done;
    }
  }

  //
  // After attaching this console, it can't find the supported resolution for all consoles. Remove it and return fail.
  //
  LocalTextDetachConsole (This, ConsoleDev);
  return EFI_UNSUPPORTED;

Done:
  Status = DEConOutQueryModeWithoutModeNumer (&mScreenMaxX, &mScreenMaxY);
  if (!EFI_ERROR (Status)) {
    mScreenMaxX--;
    mScreenMaxY--;
  }

  return EFI_SUCCESS;
}

/**
  Detach a specific console from this display engine.

  @param [in] This               A pointer to the H2O_DISPLAY_ENGINE_PROTOCOL instance.
  @param [in] ConsoleDev         A pointer to input H2O_FORM_BROWSER_CONSOLE_DEV instance.

  @retval EFI_SUCCESS            Detach a console device from the device engine successful.
  @retval EFI_INVALID_PARAMETER  This is NULL, or ConsoleDev is NULL.
  @retval EFI_NOT_FOUND          The input device console isn't attached to the display engine.

**/
EFI_STATUS
EFIAPI
LocalTextDetachConsole (
  IN H2O_DISPLAY_ENGINE_PROTOCOL              *This,
  IN H2O_FORM_BROWSER_CONSOLE_DEV             *ConsoleDev
  )
{
  EFI_STATUS                                  Status;
  H2O_DISPLAY_ENGINE_PRIVATE_DATA             *Private;
  H2O_FORM_BROWSER_CONSOLE_DEV_NODE           *ConDevNode;
  LIST_ENTRY                                  *Link;

  if (This == NULL || ConsoleDev == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Private = H2O_DISPLAY_ENGINE_PRIVATE_DATA_FROM_PROTOCOL (This);

  if (IsListEmpty (&Private->ConsoleDevListHead)) {
    return EFI_NOT_FOUND;
  }

  //
  // If call this function during processing notify function, replace fake protocol on this console device.
  // It can make sure that interruped notify function can be executed successfully.
  // After interruped notify function is finished, remove the console device.
  //
  if (mIsInNotifyProcess) {
    Status = DEReplaceFakeConOutDev (Private, ConsoleDev->Handle);
    return Status;
  }

  Link = Private->ConsoleDevListHead.ForwardLink;
  while (TRUE) {
    ConDevNode = H2O_FORM_BROWSER_CONSOLE_DEV_NODE_FROM_LINK (Link);
    if (ConsoleDev->Handle == ConDevNode->ConsoleDev->Handle) {
      //
      // Remove Console Device Node
      //
      RemoveEntryList (&ConDevNode->Link);
      FreePool (ConDevNode->ConsoleDev);
      FreePool (ConDevNode);
      DEBUG ((EFI_D_INFO, "Device is detached\n"));
      if (IsListEmpty (&Private->ConsoleDevListHead)) {
        DEExit ();
      }

      return EFI_SUCCESS;
    }
    //
    // Get Next Console
    //
    if (IsNodeAtEnd (&Private->ConsoleDevListHead, Link)) {
      break;
    }
    Link = Link->ForwardLink;
  }

  return EFI_NOT_FOUND;
}

/**
  Report which console devices are supported by this display engine.

  @param[in] This                A pointer to the H2O_DISPLAY_ENGINE_PROTOCOL instance.
  @param[in] ConsoleDev          A pointer to input H2O_FORM_BROWSER_CONSOLE_DEV instance.

  @retval EFI_SUCCESS            Report supported console device type successful.
  @retval EFI_UNSUPPORTED        This display engine doesn't support input console device.
  @retval EFI_INVALID_PARAMETER  This is NULL or ConsoleDev is NULL.

**/
EFI_STATUS
EFIAPI
LocalTextSupportConsole (
  IN H2O_DISPLAY_ENGINE_PROTOCOL              *This,
  IN H2O_FORM_BROWSER_CONSOLE_DEV             *ConsoleDev
  )
{
  EFI_STATUS                                  Status;
  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL             *SimpleTextOut;

  if (This == NULL || ConsoleDev == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = gBS->HandleProtocol (ConsoleDev->Handle, &gEfiSimpleTextOutProtocolGuid, (VOID **) &SimpleTextOut);
  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }

  return EFI_SUCCESS;
}

/**
  Add the notification to the notification queue and signal the Notification event.

  @param [in] This               A pointer to the H2O_DISPLAY_ENGINE_PROTOCOL instance.
  @param [in] Notify             A pointer to the H2O_DISPLAY_ENGINE_EVT instance.

  @retval EFI_SUCCESS            Register notify successful.
  @retval EFI_INVALID_PARAMETER  This is NULL or Notify is NULL.

**/
EFI_STATUS
EFIAPI
LocalTextNotify (
  IN       H2O_DISPLAY_ENGINE_PROTOCOL        *This,
  IN CONST H2O_DISPLAY_ENGINE_EVT             *Notify
  )
{
  EFI_STATUS                                  Status;

  mIsInNotifyProcess = TRUE;
  Status = DEEventCallback (This, Notify);
  mIsInNotifyProcess = FALSE;

  DERemoveFakeConOutDev (mDEPrivate);

  return Status;
}

/**
  Initizlize private data for local text display engine and install display engine protocol

  @param [in] ImageHandle        The image handle
  @param [in] SystemTable        The system table

  @retval EFI_SUCCESS            Success to initialize private data and install display engine protocol
  @retval EFI_OUT_OF_RESOURCES   Display engine private data allocate fail
  @retval Other                  Fail to initialize display engine private data

**/
EFI_STATUS
EFIAPI
H2ODisplayEngineLocalTextEntryPoint (
  IN EFI_HANDLE                               ImageHandle,
  IN EFI_SYSTEM_TABLE                         *SystemTable
  )
{
  EFI_STATUS                                  Status;
  EFI_GUID                                    Guid = H2O_DISPLAY_ENGINE_LOCAL_TEXT_GUID;

  mDEPrivate = (H2O_DISPLAY_ENGINE_PRIVATE_DATA*) AllocateZeroPool (sizeof (H2O_DISPLAY_ENGINE_PRIVATE_DATA));
  if (mDEPrivate == NULL) {
    DEBUG ((EFI_D_INFO, "H2ODisplayEngineLocalTextEntryPoint() mDEPrivate allocate pool fail.\n"));
    return EFI_OUT_OF_RESOURCES;
  }

  mDEPrivate->Signature   = H2O_DISPLAY_ENGINE_SIGNATURE;
  mDEPrivate->ImageHandle = ImageHandle;
  mDEPrivate->DEStatus    = DISPLAY_ENGINE_STATUS_AT_MENU;
  InitializeListHead (&mDEPrivate->ConsoleDevListHead);
  InitializeListHead (&mDEPrivate->PanelListHead);

  Status = gBS->LocateProtocol (&gH2OFormBrowserProtocolGuid, NULL, (VOID **) &mDEPrivate->FBProtocol);
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "[H2ODisplayEngineLib] locate protocol fail in\n"));
    DEBUG ((EFI_D_ERROR, "H2ODisplayEngineLocalTextEntryPoint function\n"));
    return Status;
  }

  mDEPrivate->DisplayEngine.Size           = (UINT32) sizeof (H2O_DISPLAY_ENGINE_PROTOCOL);
  mDEPrivate->DisplayEngine.InitConsole    = LocalTextInitConsole;
  mDEPrivate->DisplayEngine.AttachConsole  = LocalTextAttachConsole;
  mDEPrivate->DisplayEngine.DetachConsole  = LocalTextDetachConsole;
  mDEPrivate->DisplayEngine.SupportConsole = LocalTextSupportConsole;
  mDEPrivate->DisplayEngine.Notify         = LocalTextNotify;
  CopyGuid (&mDEPrivate->DisplayEngine.Id, &Guid);

  Status = gBS->InstallMultipleProtocolInterfaces (
                  &ImageHandle,
                  &gH2ODisplayEngineProtocolGuid,
                  &mDEPrivate->DisplayEngine,
                  NULL
                  );
  return Status;
}
