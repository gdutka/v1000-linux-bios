/** @file
  Reset Architectural and Reset Notification protocols implementation

;******************************************************************************
;* Copyright (c) 2012 - 2019, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

/** @file
  Reset Architectural and Reset Notification protocols implementation.

  Copyright (c) 2006 - 2017, Intel Corporation. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/


#include "ResetSystem.h"


GLOBAL_REMOVE_IF_UNREFERENCED CHAR16 *mResetTypeStr[] = {
  L"Cold", L"Warm", L"Shutdown", L"PlatformSpecific"
};

//
// The current ResetSystem() notification recursion depth
//
UINTN  mResetNotifyDepth = 0;

EFI_RESET_SYSTEM          mOriginalResetSystem = NULL;
BOOLEAN                   mBootToLegacy = FALSE;


/**
  Register a notification function to be called when ResetSystem() is called.

  The RegisterResetNotify() function registers a notification function that is called when
  ResetSystem()is called and prior to completing the reset of the platform.
  The registered functions must not perform a platform reset themselves. These
  notifications are intended only for the notification of components which may need some
  special-purpose maintenance prior to the platform resetting.
  The list of registered reset notification functions are processed if ResetSystem()is called
  before ExitBootServices(). The list of registered reset notification functions is ignored if
  ResetSystem()is called after ExitBootServices().

  @param[in]  This              A pointer to the EFI_RESET_NOTIFICATION_PROTOCOL instance.
  @param[in]  ResetFunction     Points to the function to be called when a ResetSystem() is executed.

  @retval EFI_SUCCESS           The reset notification function was successfully registered.
  @retval EFI_INVALID_PARAMETER ResetFunction is NULL.
  @retval EFI_OUT_OF_RESOURCES  There are not enough resources available to register the reset notification function.
  @retval EFI_ALREADY_STARTED   The reset notification function specified by ResetFunction has already been registered.

**/
EFI_STATUS
EFIAPI
RegisterResetNotify (
  IN EFI_RESET_NOTIFICATION_PROTOCOL *This,
  IN EFI_RESET_SYSTEM                ResetFunction
  )
{
  RESET_NOTIFICATION_INSTANCE        *Instance;
  LIST_ENTRY                         *Link;
  RESET_NOTIFY_ENTRY                 *Entry;

  if (ResetFunction == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Instance = RESET_NOTIFICATION_INSTANCE_FROM_THIS (This);

  for ( Link = GetFirstNode (&Instance->ResetNotifies)
      ; !IsNull (&Instance->ResetNotifies, Link)
      ; Link = GetNextNode (&Instance->ResetNotifies, Link)
      ) {
    Entry = RESET_NOTIFY_ENTRY_FROM_LINK (Link);
    if (Entry->ResetNotify == ResetFunction) {
      return EFI_ALREADY_STARTED;
    }
  }

  ASSERT (IsNull (&Instance->ResetNotifies, Link));
  Entry = AllocatePool (sizeof (*Entry));
  if (Entry == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  Entry->Signature   = RESET_NOTIFY_ENTRY_SIGNATURE;
  Entry->ResetNotify = ResetFunction;
  InsertTailList (&Instance->ResetNotifies, &Entry->Link);
  return EFI_SUCCESS;
}

/**
  Unregister a notification function.

  The UnregisterResetNotify() function removes the previously registered
  notification using RegisterResetNotify().

  @param[in]  This              A pointer to the EFI_RESET_NOTIFICATION_PROTOCOL instance.
  @param[in]  ResetFunction     The pointer to the ResetFunction being unregistered.

  @retval EFI_SUCCESS           The reset notification function was unregistered.
  @retval EFI_INVALID_PARAMETER ResetFunction is NULL.
  @retval EFI_INVALID_PARAMETER The reset notification function specified by ResetFunction was not previously
                                registered using RegisterResetNotify().

**/
EFI_STATUS
EFIAPI
UnregisterResetNotify (
  IN EFI_RESET_NOTIFICATION_PROTOCOL *This,
  IN EFI_RESET_SYSTEM                ResetFunction
  )
{
  RESET_NOTIFICATION_INSTANCE        *Instance;
  LIST_ENTRY                         *Link;
  RESET_NOTIFY_ENTRY                 *Entry;

  if (ResetFunction == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Instance = RESET_NOTIFICATION_INSTANCE_FROM_THIS (This);

  for ( Link = GetFirstNode (&Instance->ResetNotifies)
      ; !IsNull (&Instance->ResetNotifies, Link)
      ; Link = GetNextNode (&Instance->ResetNotifies, Link)
      ) {
    Entry = RESET_NOTIFY_ENTRY_FROM_LINK (Link);
    if (Entry->ResetNotify == ResetFunction) {
      RemoveEntryList (&Entry->Link);
      FreePool (Entry);
      return EFI_SUCCESS;
    }
  }

  return EFI_INVALID_PARAMETER;
}

RESET_NOTIFICATION_INSTANCE mResetNotification = {
  RESET_NOTIFICATION_INSTANCE_SIGNATURE,
  {
    RegisterResetNotify,
    UnregisterResetNotify
  },
  INITIALIZE_LIST_HEAD_VARIABLE (mResetNotification.ResetNotifies)
};

RESET_NOTIFICATION_INSTANCE mPlatformSpecificResetFilter = {
  RESET_NOTIFICATION_INSTANCE_SIGNATURE,
  {
    RegisterResetNotify,
    UnregisterResetNotify
  },
  INITIALIZE_LIST_HEAD_VARIABLE (mPlatformSpecificResetFilter.ResetNotifies)
};

RESET_NOTIFICATION_INSTANCE mPlatformSpecificResetHandler = {
  RESET_NOTIFICATION_INSTANCE_SIGNATURE,
  {
    RegisterResetNotify,
    UnregisterResetNotify
  },
  INITIALIZE_LIST_HEAD_VARIABLE (mPlatformSpecificResetHandler.ResetNotifies)
};


/**
  Notification function of EVT_SIGNAL_VIRTUAL_ADDRESS_CHANGE.

  This is a notification function registered on EVT_SIGNAL_VIRTUAL_ADDRESS_CHANGE event.
  It convers pointer to new virtual address.

  @param  Event        Event whose notification function is being invoked.
  @param  Context      Pointer to the notification function's context.

**/
VOID
EFIAPI
ResetSystemAddressChangeEventCallBack (
  IN EFI_EVENT                            Event,
  IN VOID                                 *Context
  )
{
  //
  // convert relative base addresses or pointers
  //
  if (mOriginalResetSystem != NULL) {
    gRT->ConvertPointer (0x0, (VOID **) &mOriginalResetSystem);
  }
}

VOID
EFIAPI
ResetSystemHookCallback (
  IN EFI_EVENT   Event,
  IN VOID        *Context
  )
{
  EFI_STATUS     Status;
  VOID           *Instance;
  EFI_EVENT      NewEvent;
  EFI_HANDLE     Handle;

  //
  // Hook reset system.
  //
  Status = gBS->LocateProtocol (
                  &gEfiResetArchProtocolGuid,
                  NULL,
                  &Instance
                  );
  if (EFI_ERROR(Status)) {
    return;
  }

  Status = gBS->CreateEventEx (
                  EVT_NOTIFY_SIGNAL,
                  TPL_NOTIFY,
                  ResetSystemAddressChangeEventCallBack,
                  NULL,
                  &gEfiEventVirtualAddressChangeGuid,
                  &NewEvent
                  );
  if (EFI_ERROR(Status)) {
    return;
  }
  mOriginalResetSystem = gRT->ResetSystem;
  gRT->ResetSystem     = RuntimeServiceResetSystem;

  Handle = NULL;
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &Handle,
                  &gEfiResetNotificationProtocolGuid,             &mResetNotification.ResetNotification,
                  &gEdkiiPlatformSpecificResetFilterProtocolGuid, &mPlatformSpecificResetFilter.ResetNotification,
                  &gEdkiiPlatformSpecificResetHandlerProtocolGuid, &mPlatformSpecificResetHandler.ResetNotification,
                  NULL
                  );

}



/**
  Callback function for legacy boot event.

  @param Event   Event whose notification function is being invoked.
  @param Context Pointer to the notification function's context.

**/
VOID
EFIAPI
LegacyBootCallback (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
{
  mBootToLegacy = TRUE;
  gBS->CloseEvent (Event);
}

/**
  The driver's entry point.

  It creates notification callback for the Reset Architectural Protocol.

  @param[in] ImageHandle  The firmware allocated handle for the EFI image.
  @param[in] SystemTable  A pointer to the EFI System Table.

  @retval EFI_SUCCESS     The entry point is executed successfully.
  @retval other           Cannot install ResetArch protocol.

**/
EFI_STATUS
EFIAPI
InitializeResetSystem (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  VOID                 *ResetArchRegistration;
  EFI_STATUS           Status;
  EFI_EVENT            LegacyBootEvent;
  EFI_HANDLE           Handle;

  if (!PcdGetBool (PcdH2OResetSupported)) {
    EfiCreateProtocolNotifyEvent (
      &gEfiResetArchProtocolGuid,
      TPL_NOTIFY,
      ResetSystemHookCallback,
      NULL,
      &ResetArchRegistration
      );
  } else {
    //
    // Make sure the Reset Architectural Protocol is not already installed in the system
    //
    ASSERT_PROTOCOL_ALREADY_INSTALLED (NULL, &gEfiResetArchProtocolGuid);

    //
    // Hook the runtime service table
    //
    gRT->ResetSystem = RuntimeServiceResetSystem;

    //
    // Now install the Reset RT AP on a new handle
    //
    Handle = NULL;
    Status = gBS->InstallMultipleProtocolInterfaces (
                    &Handle,
                    &gEfiResetArchProtocolGuid,         NULL,
                    &gEfiResetNotificationProtocolGuid, &mResetNotification.ResetNotification,
                    &gEdkiiPlatformSpecificResetFilterProtocolGuid, &mPlatformSpecificResetFilter.ResetNotification,
                    &gEdkiiPlatformSpecificResetHandlerProtocolGuid, &mPlatformSpecificResetHandler.ResetNotification,
                    NULL
                    );
    ASSERT_EFI_ERROR (Status);
  }
  Status = EfiCreateEventLegacyBootEx (
             TPL_CALLBACK,
             LegacyBootCallback,
             NULL,
             &LegacyBootEvent
             );
  ASSERT_EFI_ERROR (Status);

  return EFI_SUCCESS;
}

/**
  Put the system into S3 power state.
**/
VOID
DoS3 (
  VOID
  )
{
  EnterS3WithImmediateWake ();

  //
  // Should not return
  //
  CpuDeadLoop ();
}

/**
  Resets the entire platform.

  @param[in] ResetType          The type of reset to perform.
  @param[in] ResetStatus        The status code for the reset.
  @param[in] DataSize           The size, in bytes, of ResetData.
  @param[in] ResetData          For a ResetType of EfiResetCold, EfiResetWarm, or
                                EfiResetShutdown the data buffer starts with a Null-terminated
                                string, optionally followed by additional binary data.
                                The string is a description that the caller may use to further
                                indicate the reason for the system reset. ResetData is only
                                valid if ResetStatus is something other than EFI_SUCCESS
                                unless the ResetType is EfiResetPlatformSpecific
                                where a minimum amount of ResetData is always required.
                                For a ResetType of EfiResetPlatformSpecific the data buffer
                                also starts with a Null-terminated string that is followed
                                by an EFI_GUID that describes the specific type of reset to perform.
**/
VOID
EFIAPI
RuntimeServiceResetSystem (
  IN EFI_RESET_TYPE   ResetType,
  IN EFI_STATUS       ResetStatus,
  IN UINTN            DataSize,
  IN VOID             *ResetData OPTIONAL
  )
{
  EFI_STATUS          Status;
  UINTN               Size;
  UINTN               CapsuleDataPtr;
  LIST_ENTRY          *Link;
  RESET_NOTIFY_ENTRY  *Entry;

  //
  // Only do REPORT_STATUS_CODE() on first call to RuntimeServiceResetSystem()
  //
  if (mResetNotifyDepth == 0) {
    //
    // Indicate reset system runtime service is called.
    //
    REPORT_STATUS_CODE (EFI_PROGRESS_CODE, (EFI_SOFTWARE_EFI_RUNTIME_SERVICE | EFI_SW_RS_PC_RESET_SYSTEM));
  }

  mResetNotifyDepth++;
  DEBUG ((
    DEBUG_INFO, "DXE ResetSystem2: ResetType %s, Call Depth = %d.\n",
    mResetTypeStr[ResetType], mResetNotifyDepth
    ));

  if (mResetNotifyDepth <= MAX_RESET_NOTIFY_DEPTH) {
    if (!EfiAtRuntime () && !mBootToLegacy) {
      if (FeaturePcdGet (PcdH2ODxeCpResetSystemSupported)) {
        H2O_DXE_CP_RESET_SYSTEM_DATA  ResetSystemData;

        ResetSystemData.Size          = sizeof (H2O_DXE_CP_RESET_SYSTEM_DATA);
        ResetSystemData.Status        = H2O_CP_TASK_NORMAL;
        ResetSystemData.ResetType     = ResetType;
        ResetSystemData.ResetStatus   = ResetStatus;
        ResetSystemData.ResetDataSize = (UINT32) DataSize;
        ResetSystemData.ResetData     = ResetData;

        DEBUG ((DEBUG_INFO, "Checkpoint Trigger: %g\n", &gH2ODxeCpResetSystemGuid));
        H2OCpTrigger (&gH2ODxeCpResetSystemGuid, &ResetSystemData);
        DEBUG ((DEBUG_INFO, "Checkpoint Result: %x\n", ResetSystemData.Status));

        if (ResetSystemData.Status == H2O_CP_TASK_SKIP) {
          return;
        } else if (ResetSystemData.Status == H2O_CP_TASK_UPDATE) {
          ResetType   = ResetSystemData.ResetType;
          ResetStatus = ResetSystemData.ResetStatus;
          DataSize    = ResetSystemData.ResetDataSize;
          ResetData   = ResetSystemData.ResetData;
        }
      }

      //
      // Call reset notification functions registered through the
      // EDKII_PLATFORM_SPECIFIC_RESET_FILTER_PROTOCOL.
      //
      for ( Link = GetFirstNode (&mPlatformSpecificResetFilter.ResetNotifies)
          ; !IsNull (&mPlatformSpecificResetFilter.ResetNotifies, Link)
          ; Link = GetNextNode (&mPlatformSpecificResetFilter.ResetNotifies, Link)
          ) {
        Entry = RESET_NOTIFY_ENTRY_FROM_LINK (Link);
        Entry->ResetNotify (ResetType, ResetStatus, DataSize, ResetData);
      }
      //
      // Call reset notification functions registered through the
      // EFI_RESET_NOTIFICATION_PROTOCOL.
      //
      for ( Link = GetFirstNode (&mResetNotification.ResetNotifies)
          ; !IsNull (&mResetNotification.ResetNotifies, Link)
          ; Link = GetNextNode (&mResetNotification.ResetNotifies, Link)
          ) {
        Entry = RESET_NOTIFY_ENTRY_FROM_LINK (Link);
        Entry->ResetNotify (ResetType, ResetStatus, DataSize, ResetData);
      }
      //
      // call reset notification functions registered through the
      // EDKII_PLATFORM_SPECIFIC_RESET_HANDLER_PROTOCOL.
      //
      for ( Link = GetFirstNode (&mPlatformSpecificResetHandler.ResetNotifies)
          ; !IsNull (&mPlatformSpecificResetHandler.ResetNotifies, Link)
          ; Link = GetNextNode (&mPlatformSpecificResetHandler.ResetNotifies, Link)
          ) {
        Entry = RESET_NOTIFY_ENTRY_FROM_LINK (Link);
        Entry->ResetNotify (ResetType, ResetStatus, DataSize, ResetData);
      }
    }
  } else {
    ASSERT (ResetType < ARRAY_SIZE (mResetTypeStr));
    DEBUG ((DEBUG_ERROR, "DXE ResetSystem2: Maximum reset call depth is met. Use the current reset type: %s!\n", mResetTypeStr[ResetType]));
  }
  if (mOriginalResetSystem != NULL) {
    mOriginalResetSystem (ResetType, ResetStatus, DataSize, ResetData);
  } else {
    switch (ResetType) {
    case EfiResetWarm:

      //
      //Check if there are pending capsules to process
      //
      Size = sizeof (CapsuleDataPtr);
      Status =  EfiGetVariable (
                   EFI_CAPSULE_VARIABLE_NAME,
                   &gEfiCapsuleVendorGuid,
                   NULL,
                   &Size,
                   (VOID *) &CapsuleDataPtr
                   );

      if (Status == EFI_SUCCESS) {
        //
        //Process capsules across a system reset.
        //
        DoS3();
      }

      ResetWarm ();
      break;

    case EfiResetCold:
      ResetCold ();
      break;

    case EfiResetShutdown:
      ResetShutdown ();
      return ;

    case EfiResetPlatformSpecific:
      ResetPlatformSpecific (DataSize, ResetData);
      return;

    default:
      return ;
    }

    //
    // Given we should have reset getting here would be bad
    //
    ASSERT (FALSE);
  }

}
