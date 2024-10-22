/** @file

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

/*++

Module Name:

  BiosProtectDxe.c

Abstract:

  This driver registers event to enable BIOS region protection before boot to OS

--*/

#include <Uefi.h>
#include <Library/PcdLib.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/DxeChipsetSvcLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/H2OCpLib.h>
#include <Library/GenericBdsLib.h>
#include <Protocol/BiosRegionLock.h>
#include <Protocol/DxeSmmReadyToLock.h>
#include <Protocol/ExitPmAuth.h>
#include <Protocol/SmmBase2.h>
#include <Guid/EventGroup.h>
#include <Guid/EventLegacyBios.h>
#include <Guid/H2OBdsCheckPoint.h>
#include <SecureFlash.h>

/**
 Bios protection stage 1 to calling BiosRegionLock protocol to set protected BIOS region and then lock.

 @param [in]   Event            A pointer to the Event that triggered the callback.
 @param [in]   Context          A pointer to private data registered with the callback function.

**/
VOID
EFIAPI
BiosProtectionStage1 (
  IN     EFI_EVENT      Event,
  IN     VOID           *Context
  )
{
  EFI_STATUS                    Status;
  BIOS_REGION_LOCK_PROTOCOL     *BiosRegionLock;
  UINTN                         RegionIndex;
  BIOS_PROTECT_REGION           *BiosRegionTable;
  UINT8                         ProtectRegionNum;
  STATIC BOOLEAN                RegionLocked = FALSE;

  ProtectRegionNum = 0;
  BiosRegionLock  = NULL;
  BiosRegionTable = NULL;
  if (Event != NULL) {
    gBS->CloseEvent (Event);
  } else {
    H2OCpUnregisterHandler (Context);
  }
  Status = gBS->LocateProtocol (&gEfiBiosRegionLockProtocolGuid, NULL, (VOID **)&BiosRegionLock);
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return;
  }
  if (RegionLocked) {
    return;
  }
  RegionLocked = TRUE;
  if (PcdGetBool (PcdSecureFlashSupported)) {
    DxeCsSvcGetBiosProtectTable (&BiosRegionTable, &ProtectRegionNum);
    if (BiosRegionTable != NULL && ProtectRegionNum != 0) {
      for (RegionIndex = 0; RegionIndex < ProtectRegionNum; ++RegionIndex) {
        Status = BiosRegionLock->SetRegionByAddress (BiosRegionLock, BiosRegionTable[RegionIndex].Base, BiosRegionTable[RegionIndex].Size);
        if (EFI_ERROR (Status)) {
          DEBUG ((EFI_D_ERROR, "Secure flash support failed in this system!\n"));
          FreePool (BiosRegionTable);
          return;
        }
      }
      FreePool (BiosRegionTable);
    }
  }
  DEBUG ((EFI_D_ERROR | EFI_D_INFO, "Secure flash support succeed.\n"));
  BiosRegionLock->Lock (BiosRegionLock);
}

/**
 Bios protection stage 2 to installing ExitPmAuth protocol for chipset specific callback upon ready to boot

 @param [in]   Event            A pointer to the Event that triggered the callback.
 @param [in]   Handle           Checkpoint handle.

**/
VOID
EFIAPI
BiosProtectionStage2 (
  IN     EFI_EVENT           Event,
  IN     H2O_CP_HANDLE       Handle
  )
{
  EFI_STATUS                             Status;
  VOID                                   *Interface;

  H2OCpUnregisterHandler (Handle);
  //
  // Install ExitPmAuth protocol for chipset specific callback
  //
  Interface = NULL;
  Status = gBS->LocateProtocol (
                  &gExitPmAuthProtocolGuid,
                  NULL,
                  (VOID**)&Interface
                  );
  if (EFI_ERROR (Status)) {
    gBS->InstallProtocolInterface (
           &gImageHandle,
           &gExitPmAuthProtocolGuid,
           EFI_NATIVE_INTERFACE,
           NULL
           );
  }
}

/**
 Bios protection stage 3 to installing DxeSmmReadyToLock protocol for boot script save lockdown
 The timeframe of thsi callback being triggered is before SmmIpl ReadyToBootCallback
 DxeSmmReadyToLockProtocol doorkeeper and after ChipsetPrepareReadyToBoot(for calling AcpiS3Save->S3Save())

 @param [in]   Event            A pointer to the Event that triggered the callback.
 @param [in]   Context          A pointer to private data registered with the callback function.

**/
VOID
EFIAPI
BiosProtectionStage3 (
  IN     EFI_EVENT      Event,
  IN     VOID           *Context
  )
{
  EFI_STATUS                Status;
  VOID                      *Interface;

  gBS->CloseEvent (Event);
  Status = gBS->LocateProtocol (&gEfiDxeSmmReadyToLockProtocolGuid, NULL, &Interface);
  if (!EFI_ERROR (Status)) {
    return;
  }

  BdsLibTriggerDxeSmmReadyToLockEvent ();
}

/**
 Bios protection notification registration callback, use to register event notifications to ensure the
 callback can be triggered at last position of specific TPL level

 @param [in]   Event            A pointer to the Event that triggered the callback.
 @param [in]   Context          A pointer to private data registered with the callback function.

**/
VOID
EFIAPI
BiosProtectionEventHandler1 (
  IN     EFI_EVENT      Event,
  IN     VOID           *Context
  )
{
  gBS->CloseEvent (Event);
  //
  // Register notification on TPL_CALLBACK of ReadyToBoot event for installing DxeSmmReadyToLockProtocol
  // use for SMM lock to lockdown boot script save. The timeframe of this callback being triggered is
  // before SmmIpl ReadyToBootCallback DxeSmmReadyToLockProtocol doorkeeper and after
  // ChipsetPrepareReadyToBoot(for calling AcpiS3Save->S3Save())
  //
  EfiCreateEventReadyToBootEx (
    TPL_CALLBACK,
    BiosProtectionStage3,
    NULL,
    &Event
    );
}

/**
 Bios protection notification registration callback, use to register event notifications to ensure the
 callback can be triggered at last position of specific TPL level

 @param[in] Event           A pointer to the Event that triggered the callback.
 @param[in] Handle          Checkpoint handle.

**/
VOID
EFIAPI
BiosProtectionEventHandler2 (
  IN EFI_EVENT                              Event,
  IN H2O_CP_HANDLE                          Handle
  )
{
  EFI_STATUS             Status;
  H2O_CP_HANDLE          CpHandle;


  H2OCpUnregisterHandler (Handle);

  if (FeaturePcdGet (PcdH2OBdsCpDxeSmmReadyToLockBeforeSupported)) {
    //
    // Register checkpoint of gH2OBdsCpDxeSmmReadyToLockBeforeGuid to lockdown flash part protection
    //
    Status = H2OCpRegisterHandler (
               &gH2OBdsCpDxeSmmReadyToLockBeforeGuid,
               BiosProtectionStage1,
               H2O_CP_MEDIUM_LOW,
               &CpHandle
               );
    if (EFI_ERROR (Status)) {
      DEBUG ((EFI_D_ERROR, "Checkpoint Register Fail: %g (%r)\n", &gH2OBdsCpDxeSmmReadyToLockBeforeGuid, Status));
      return;
    }
    DEBUG ((EFI_D_INFO, "Checkpoint Registered: %g (%r)\n", &gH2OBdsCpDxeSmmReadyToLockBeforeGuid, Status));
  }

  if (FeaturePcdGet (PcdH2OBdsCpReadyToBootBeforeSupported)) {
    //
    // Register checkpoint of BdsCpReadyToBootBefore to lockdown flash part protection
    //
    Status = H2OCpRegisterHandler (
               &gH2OBdsCpReadyToBootBeforeGuid,
               BiosProtectionStage1,
               H2O_CP_MEDIUM_LOW,
               &CpHandle
               );
    if (EFI_ERROR (Status)) {
      DEBUG ((EFI_D_ERROR, "Checkpoint Register Fail: %g (%r)\n", &gH2OBdsCpReadyToBootBeforeGuid, Status));
      return;
    }
    DEBUG ((EFI_D_INFO, "Checkpoint Registered: %g (%r)\n", &gH2OBdsCpReadyToBootBeforeGuid, Status));
    //
    // Register notification on TPL_CALLBACK - 1 of BdsCpReadyToBootBefore event for installing ExitPmAuth protocol
    // It is use for chipset specific callback upon ready to boot
    //
    Status = H2OCpRegisterHandler (
               &gH2OBdsCpReadyToBootBeforeGuid,
               BiosProtectionStage2,
               H2O_CP_MEDIUM_LOW,
               &CpHandle
               );
    if (EFI_ERROR (Status)) {
      DEBUG ((EFI_D_ERROR, "Checkpoint Register Fail: %g (%r)\n", &gH2OBdsCpReadyToBootBeforeGuid, Status));
      return;
    }
    DEBUG ((EFI_D_INFO, "Checkpoint Registered: %g (%r)\n", &gH2OBdsCpReadyToBootBeforeGuid, Status));
  }
}

/**
 Bios protection notification registration callback, use to register event notifications to ensure the
 callback can be triggered at last position of specific TPL level

 @param [in]   Event            A pointer to the Event that triggered the callback.
 @param [in]   Context          A pointer to private data registered with the callback function.

**/
VOID
EFIAPI
BiosProtectionEventHandler3 (
  IN     EFI_EVENT      Event,
  IN     VOID           *Context
  )
{
  EFI_STATUS          Status;
  VOID                *Interface;
  //
  // Double check the interface installation
  //
  Status = gBS->LocateProtocol (
                  &gEfiSmmBase2ProtocolGuid,
                  NULL,
                  (VOID**)&Interface
                  );
  if (!EFI_ERROR (Status)) {
    gBS->CloseEvent (Event);
    //
    // Register notification on TPL_NOTIFY of gEfiEventDxeDispatchGuid events to ensure the entry point of SmmIpl driver is finish
    // as the registration of the doorkeeper of DxeSmmReadyToLockProtocol upon ReadyToBoot event callback is after installation
    // of SmmBase2 protocol
    //
    gBS->CreateEventEx (
           EVT_NOTIFY_SIGNAL,
           TPL_NOTIFY,
           BiosProtectionEventHandler1,
           NULL,
           &gEfiEventDxeDispatchGuid,
           &Event
           );
  }
}

/**
 This routine registers event to enable BIOS region protection.

 @param [in]   ImageHandle      Handle for the image of this driver
 @param [in]   SystemTable      Pointer to the EFI System Table

 @retval EFI_SUCCESS            The function completed successfully

**/
EFI_STATUS
EFIAPI
BiosProtectEntryPoint (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS          Status;
  VOID                *Interface;
  EFI_EVENT           Event;
  H2O_CP_HANDLE       CpHandle;

  if (!FeaturePcdGet(PcdInstantCapsuleUpdateSupported)) {
    if (FeaturePcdGet (PcdH2OBdsCpInitSupported)) {
      //
      // Register notification on TPL_NOTIFY of gH2OBdsCpInitGuid event to register other notifications
      // to ensure the callbacks can be triggered at last position of specific TPL level
      //
      Status = H2OCpRegisterHandler (
                 &gH2OBdsCpInitGuid,
                 BiosProtectionEventHandler2,
                 H2O_CP_HIGH,
                 &CpHandle
                 );
      if (EFI_ERROR (Status)) {
        DEBUG ((EFI_D_ERROR, "Checkpoint Register Fail: %g (%r)\n", &gH2OBdsCpInitGuid, Status));
        return Status;
      }
      DEBUG ((EFI_D_INFO, "Checkpoint Registered: %g (%r)\n", &gH2OBdsCpInitGuid, Status));
    }
    Status = gBS->LocateProtocol (
                    &gEfiSmmBase2ProtocolGuid,
                    NULL,
                    &Interface
                    );
    if (!EFI_ERROR (Status)) {
      //
      // Go ahead directly to register proper handler
      //
      BiosProtectionEventHandler1 (NULL, NULL);
    } else {
      //
      // Register notification for SmmBase2
      //
      EfiCreateProtocolNotifyEvent (
        &gEfiSmmBase2ProtocolGuid,
        TPL_NOTIFY,
        BiosProtectionEventHandler3,
        NULL,
        &Interface
        );
    }
  } else {
    //
    // Register notification on TPL_NOTIFY of LegacyBoot/ExitBootServices events to lockdown flash part protection
    // Postpone the event after boot loader get called for instant capsule update
    //
    gBS->CreateEventEx (
           EVT_NOTIFY_SIGNAL,
           TPL_NOTIFY,
           BiosProtectionStage1,
           NULL,
           &gEfiEventLegacyBootGuid,
           &Event
           );
    gBS->CreateEventEx (
           EVT_NOTIFY_SIGNAL,
           TPL_NOTIFY,
           BiosProtectionStage1,
           NULL,
           &gEfiEventExitBootServicesGuid,
           &Event
           );
    if (FeaturePcdGet (PcdH2OBdsCpReadyToBootBeforeSupported)) {
      //
      // Register notification on TPL_CALLBACK of LegacyBoot/ExitBootServices for installing ExitPmAuth protocol
      // It is use for chipset specific callback upon ready to boot
      //
      Status = H2OCpRegisterHandler (
                 &gH2OBdsCpReadyToBootBeforeGuid,
                 BiosProtectionStage2,
                 H2O_CP_MEDIUM_LOW,
                 &CpHandle
                 );
      if (EFI_ERROR (Status)) {
        DEBUG ((EFI_D_ERROR, "Checkpoint Register Fail: %g (%r)\n", &gH2OBdsCpReadyToBootBeforeGuid, Status));
        return Status;
      }
      DEBUG ((EFI_D_INFO, "Checkpoint Registered: %g (%r)\n", &gH2OBdsCpReadyToBootBeforeGuid, Status));
    }

  }
  return EFI_SUCCESS;
}
