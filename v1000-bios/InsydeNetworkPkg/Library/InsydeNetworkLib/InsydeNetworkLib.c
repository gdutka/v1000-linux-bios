/** @file
  Control NetworkStack behavior callback via NetworkLocker protocol.

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

#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/InsydeNetworkLib.h>

#include <Guid/Performance.h>
#include <Guid/NetworkEventGroup.h>
#include <Protocol/GraphicsOutput.h>
#include <Protocol/H2OFormBrowser.h>
#include <Protocol/H2ODialog.h>

STATIC NETWORK_LOCKER_PROTOCOL              *mNetworkLockerProtocol = NULL;

/**
  To find out the NetworkLocker protocol installed in NetworkLockerDxe driver's ImageHandle.
  

  @param  NONE

  @retval NULL    NetworkLocker protocol is not found.
  @retval other   The protocol instance.

**/
STATIC
NETWORK_LOCKER_PROTOCOL *
FindNetworkLocker (
  )
{
  EFI_STATUS                         Status;
  UINTN                              HandlesCnt;
  EFI_HANDLE                         *Handles;
  UINTN                              Index;
  VOID                               *TestProtocol;
  NETWORK_LOCKER_PROTOCOL            *NetworkLocker;

  if (mNetworkLockerProtocol != NULL) {
    return mNetworkLockerProtocol;
  }

  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gNetworkLockerProtocolGuid,
                  NULL,
                  &HandlesCnt,
                  &Handles
                  );
  if (EFI_ERROR (Status)) {
    return NULL;
  }

  NetworkLocker = NULL;
  for (Index = 0; Index < HandlesCnt; Index++) {
    Status = gBS->HandleProtocol (
                    Handles[Index],
                    &gEfiDevicePathProtocolGuid,
                    (VOID **) &TestProtocol
                    );
    if (!EFI_ERROR (Status)) {
      continue;
    }

    Status = gBS->HandleProtocol (
                    Handles[Index],
                    &gEfiLoadedImageProtocolGuid,
                    (VOID **) &TestProtocol
                    );
    if (EFI_ERROR (Status)) {
      continue;
    }

    Status = gBS->HandleProtocol (
                    Handles[Index],
                    &gNetworkLockerProtocolGuid,
                    (VOID **) &NetworkLocker
                    );
    if (EFI_ERROR (Status)) {
      continue;
    }
    if ((NetworkLocker->Signature != NETWORK_LOCKER_SIGNATURE) ||
        (NetworkLocker->Version != NETWORK_LOCKER_VERSION)) {
      continue;
    }

    //
    // Found the NetworkLocker protocol on NetworkLockerDxe's image handle.
    //
    mNetworkLockerProtocol = NetworkLocker;
    break;
  }
  gBS->FreePool (Handles);

  //
  // For old mechanism, NetworkLocker protocol installed in a new standalone handle.
  //
  if (NetworkLocker == NULL) {
    Status = gBS->LocateProtocol (
                    &gNetworkLockerProtocolGuid,
                    NULL,
                    (VOID **) &NetworkLocker
                    );
    if (!EFI_ERROR (Status) && (NetworkLocker != NULL)) {
      mNetworkLockerProtocol = NetworkLocker;
    }
  }

  return NetworkLocker;
}

/**
  The network stack drivers policy.
  When drivers callback to this function, it will check NetworkLockerProtocol on ControllerHandle.
  If exist, that protocol will override the policy.

  @param  Controller           Handle of network devices.
  @param  Identifier           Indicate NetworkStack driver type.

  @retval TRUE                 The driver will be reject.
  @retval FALSE                The driver will be binding.

**/
BOOLEAN
NetworkLockerCallback (
  EFI_HANDLE                           Controller,
  NETWORK_STACK_CALLBACK_IDENTIFIER    Identifier
  )
{
  EFI_STATUS                           Status;
  NETWORK_LOCKER_PROTOCOL              *NetworkLockerProtocol;

  Status = EFI_NOT_FOUND;
  NetworkLockerProtocol = NULL;
  if (Controller != NULL) {
    //
    // NetworkLockerProtocol will be update if found protocol on ControllerHandle
    //
    Status = gBS->HandleProtocol (Controller, &gNetworkLockerProtocolGuid, (VOID **) &NetworkLockerProtocol);
  }

  if (NetworkLockerProtocol == NULL) {
    NetworkLockerProtocol = FindNetworkLocker ();
    if (NetworkLockerProtocol == NULL) {
      //
      // It means there is no policy, all network driver will be binding.
      //
      return FALSE;
    }
  }

  if (NetworkLockerProtocol->NetworkStackCallback != NULL) {
    Status = NetworkLockerProtocol->NetworkStackCallback (Identifier);
  } else {
    Status = EFI_UNSUPPORTED;
  }
  return (EFI_ERROR (Status)) ? TRUE : FALSE;
}

/*++

Routine Description:

  Start to gauge on a specified handle, token and host, with Ticker as start tick.

Arguments:

  Handle  - Handle to measure
  Token   - Token to measure
  Host    - Host to measure
  Ticker  - Ticker as start tick

Returns:

  Status code.

--*/
EFI_STATUS
PerformanceStartMeasure (
  EFI_HANDLE          Handle,
  IN CHAR8            *Token,
  IN CHAR8            *Host,
  IN UINT64           Ticker
  )

{
  EFI_STATUS                Status;
  PERFORMANCE_PROTOCOL      *Perf;

  Status = gBS->LocateProtocol (&gPerformanceProtocolGuid, NULL, (VOID **) &Perf);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  return Perf->StartGauge (Handle, (CONST CHAR8 *) Token, (CONST CHAR8 *) Host, Ticker);

}

/*++

Routine Description:

  End gauging on a specified handle, token and host, with Ticker as end tick.

Arguments:

  Handle  - Handle to stop
  Token   - Token to stop
  Host    - Host to stop
  Ticker  - Ticker as end tick

Returns:

  Status code.

--*/
EFI_STATUS
PerformanceEndMeasure (
  EFI_HANDLE          Handle,
  IN CHAR8            *Token,
  IN CHAR8            *Host,
  IN UINT64           Ticker
  )

{
  EFI_STATUS                Status;
  PERFORMANCE_PROTOCOL      *Perf;

  Status = gBS->LocateProtocol (&gPerformanceProtocolGuid, NULL, (VOID **) &Perf);
  if (Status != EFI_SUCCESS) {
    return Status;
  }

  return (Perf->EndGauge(Handle, (CONST CHAR8 *) Token, (CONST CHAR8 *) Host, Ticker)) ;
}

/**
  Check if form browser is H2O form browser or not.

  @return TRUE  It is H2O form browser.
  @return FALSE It is not H2O form browser.
--*/
BOOLEAN
IsH2OFormbrowser (
  VOID
  )
{
  EFI_STATUS                                     Status;
  H2O_FORM_BROWSER_PROTOCOL                      *H2oFormBrowser;

  Status = gBS->LocateProtocol (&gH2OFormBrowserProtocolGuid, NULL, (VOID **) &H2oFormBrowser);
  if (EFI_ERROR (Status)) {
    return FALSE;
  }

  return TRUE;
}

/**
  Pop up message dialog for user to confirm

  @return EFI_SUCCESS           Message pup up successfully.
  @return Other                 Locate H2O dialog protocol fail
--*/
EFI_STATUS
NetworkLibCreatePopUp (
  IN CHAR16           *MessageStr
  )
{
  EFI_STATUS                                     Status;
  H2O_DIALOG_PROTOCOL                            *H2ODialog;
  EFI_INPUT_KEY                                  Key;

  Status = gBS->LocateProtocol (&gH2ODialogProtocolGuid, NULL, (VOID **) &H2ODialog);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  H2ODialog->ConfirmDialog (
               DlgOk,
               FALSE,
               0,
               NULL,
               &Key,
               MessageStr
               );

  return EFI_SUCCESS;
}

VOID
InsydeNetworkLibPxeStart (
  EFI_HANDLE              *Handle
  )
{
  gBS->InstallProtocolInterface (Handle, &gH2ONetworkPxeRunningGuid, EFI_NATIVE_INTERFACE, NULL);
}

VOID
InsydeNetworkLibPxeEnd (
  EFI_HANDLE              Handle
  )
{
  gBS->UninstallProtocolInterface (Handle, &gH2ONetworkPxeRunningGuid, NULL);
}

BOOLEAN
IsPxeRunning (
  )
{
  EFI_STATUS           Status;
  VOID                 *Protocol;

  Status = gBS->LocateProtocol (&gH2ONetworkPxeRunningGuid, NULL, &Protocol);
  return (EFI_ERROR (Status)) ? FALSE : TRUE;
}
