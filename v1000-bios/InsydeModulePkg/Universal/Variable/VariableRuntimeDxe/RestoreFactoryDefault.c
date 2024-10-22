/** @file
  Provide restore factory default related functions

;******************************************************************************
;* Copyright (c) 2015 - 2018, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/


#include "RestoreFactoryDefault.h"
#include "VariableCache.h"
#include <Protocol/VariableDefaultUpdate.h>

/**
  Check is whether need retore factory default.

  @return TRUE   System is in restore factory default mode.
  @return FALSE  System isn't in restore factory default mode.
--*/
BOOLEAN
NeedRestoreFactoryDefault (
  VOID
  )
{
  UINT8                          RestoreFactoryDefault;
  EFI_STATUS                     Status;
  UINT8                          *Buffer;
  UINTN                          StrSize;
  VARIABLE_POINTER_TRACK         Variable;
  UINTN                          VariableCount;


  Status = FindVariableByLifetime (
             EFI_RESTORE_FACTORY_DEFAULT_NAME,
             &gEfiGenericVariableGuid,
             &Variable,
             &VariableCount,
             &mVariableModuleGlobal->VariableBase
             );
  if (!EFI_ERROR (Status) && Variable.CurrPtr->DataSize == sizeof (UINT8)) {
    StrSize = Variable.CurrPtr->NameSize + GET_PAD_SIZE (Variable.CurrPtr->NameSize);
    Buffer = (UINT8 *) Variable.CurrPtr;
    Buffer = Buffer + StrSize + sizeof (VARIABLE_HEADER);
    RestoreFactoryDefault = *Buffer;
    if (RestoreFactoryDefault == 1) {
      return TRUE;
    }
  }

  return FALSE;
}

/**
  Restore NV_VARIABLE_STORE to factory default.

  @return EFI_INVALID_PARAMETER Any input parameter is invalid.
  @return EFI_NOT_FOUND         gEfiVariableDefaultUpdateProtocolGuid doesn't exit.
  @return EFI_UNSUPPORTED       Factory default copy isn't initialized.
  @return EFI_SUCCESS           Restore NV_VARIABLE_STORE to default successful.
--*/
EFI_STATUS
RestoreFactoryDefault (
  VOID
  )
{
  EFI_VARIABLE_DEFAULT_UPDATE_PROTOCOL      *VariableDefaultUpdateProtocol;
  EFI_STATUS                                Status;

  Status = gBS->LocateProtocol (
                  &gEfiVariableDefaultUpdateProtocolGuid,
                  NULL,
                  (VOID **)&VariableDefaultUpdateProtocol
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  Status = VariableDefaultUpdateProtocol->UpdateFactorySetting (VariableDefaultUpdateProtocol, RESTORE_WITH_RESERVING_OTHER_SETTINGS);
  //
  // Update factory setting will cause variable data and variable inconsistent, so disable variable cache.
  //
  DisableVariableCache ();
  //
  // Clear "RestoreFactoryDefault" to 0 to prevent from system always restoring factory default during POST.
  //
  UpdateRestoreFactoryDefaultVariable (0);

  if (!EFI_ERROR (Status)) {
    gST->RuntimeServices->ResetSystem (EfiResetCold, EFI_SUCCESS, 0, NULL);
  }

  return Status;
}

EFI_STATUS
EFIAPI
DummyUpdateFactorySetting (
  IN      EFI_VARIABLE_DEFAULT_UPDATE_PROTOCOL     *This,
  IN      UINT32                                   RestoreType
  )
{
  return EFI_UNSUPPORTED;
}

EFI_STATUS
EFIAPI
DummyUpdateBackupSetting (
  IN      EFI_VARIABLE_DEFAULT_UPDATE_PROTOCOL     *This,
  IN      EFI_DEVICE_PATH_PROTOCOL                 *DevicePath,
  IN      UINT32                                   RestoreType
  )
{
  return EFI_UNSUPPORTED;
}


/**
  Replace all of restore default functions which doesn't use to dummy function.
  This action can prevent from malware uses these functions.

  @return EFI_NOT_FOUND   gEfiVariableDefaultUpdateProtocolGuid doesn't exit.
  @return EFI_SUCCESS     Udpate restore default functions successful.
--*/
EFI_STATUS
UpdateRestoreVariableDefaultProtocol (
  VOID
  )
{
  EFI_STATUS                                  Status;
  UINTN                                       NumberOfHandles;
  EFI_HANDLE                                  *Handles;
  EFI_VARIABLE_DEFAULT_UPDATE_PROTOCOL        *OldVariableDefaultUpdateProtocol;
  EFI_VARIABLE_DEFAULT_UPDATE_PROTOCOL        *NewVariableDefaultUpdateProtocol;


  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiVariableDefaultUpdateProtocolGuid,
                  NULL,
                  &NumberOfHandles,
                  &Handles
                  );

  if (EFI_ERROR (Status)) {
    return Status;
  }
  ASSERT (NumberOfHandles == 1);

  Status = gBS->HandleProtocol (
                  Handles[0],
                  &gEfiVariableDefaultUpdateProtocolGuid,
                  (VOID **)&OldVariableDefaultUpdateProtocol
                  );

  ASSERT_EFI_ERROR (Status);

  NewVariableDefaultUpdateProtocol = VariableAllocateZeroBuffer (sizeof (EFI_VARIABLE_DEFAULT_UPDATE_PROTOCOL), FALSE);
  if (NewVariableDefaultUpdateProtocol == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  CopyMem (NewVariableDefaultUpdateProtocol, OldVariableDefaultUpdateProtocol, sizeof (EFI_VARIABLE_DEFAULT_UPDATE_PROTOCOL));
  NewVariableDefaultUpdateProtocol->UpdateFactorySetting = DummyUpdateFactorySetting;
  NewVariableDefaultUpdateProtocol->UpdateBackupSetting  = DummyUpdateBackupSetting;

  Status = gBS->ReinstallProtocolInterface (
                  Handles[0],
                  &gEfiVariableDefaultUpdateProtocolGuid,
                  OldVariableDefaultUpdateProtocol,
                  NewVariableDefaultUpdateProtocol
                  );

  return Status;

}


/**
  Callback function for gEfiVariableDefaultUpdateProtocolGuid protocol

  @param Event    Event whose notification function is being invoked.
  @param Context  Pointer to the notification function's context.

**/
VOID
EFIAPI
DefaultUpdateCallbackFunction (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )

{
  if (Event != NULL) {
    gBS->CloseEvent (Event);
  }

  if (NeedRestoreFactoryDefault ()) {
    RestoreFactoryDefault ();
  }

  UpdateRestoreVariableDefaultProtocol ();

  return;
}

VOID
InitializeRestoreVariableDefaultService (
  VOID
  )
{
  EFI_STATUS                            Status;
  EFI_VARIABLE_DEFAULT_UPDATE_PROTOCOL  *VariableDefaultUpdateProtocol;
  EFI_EVENT                             Event;
  VOID                                  *Registration;

  Status = gBS->LocateProtocol (&gEfiVariableDefaultUpdateProtocolGuid, NULL, (VOID **)&VariableDefaultUpdateProtocol);
  if (!EFI_ERROR (Status)) {
    DefaultUpdateCallbackFunction (NULL, NULL);
  } else {
    Event = VariableCreateProtocolNotifyEvent (
              &gEfiVariableDefaultUpdateProtocolGuid,
              TPL_CALLBACK,
              DefaultUpdateCallbackFunction,
              NULL,
              &Registration
              );
  }
}

