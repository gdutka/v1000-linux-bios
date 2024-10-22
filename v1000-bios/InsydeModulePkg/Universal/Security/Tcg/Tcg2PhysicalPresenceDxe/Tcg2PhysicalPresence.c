/** @file
  This driver checks whether there is pending TPM request. If yes,
  it will display TPM request information and ask for user confirmation.
  The TPM request will be cleared after it is processed.

;******************************************************************************
;* Copyright (c) 2013 - 2018, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <PiDxe.h>

#include <Protocol/GraphicsOutput.h>
#include <Protocol/TrEEPhysicalPresence.h>
#include <Protocol/VariableLock.h>
#include <Protocol/H2ODialog.h>
#include <Protocol/EndOfBdsBootSelection.h>

#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PrintLib.h>
#include <Library/HiiLib.h>
#include <Library/Tpm2CommandLib.h>
#include <Library/OemGraphicsLib.h>
#include <Library/DxeOemSvcKernelLib.h>
#include <Library/Tcg2PhysicalPresenceLib.h>
#include <Library/H2OCpLib.h>

#include <Guid/EventGroup.h>
#include <Guid/Tcg2PhysicalPresenceData.h>
#include <Guid/UsbEnumeration.h>
//
// TrEEOnTcg2ThunkBegin
//
#include <Guid/TrEEPhysicalPresenceData.h>
#include <Guid/H2OTpm20DtpmPublishAcpiTableDone.h>
#include <Library/VariableLib.h>
//
// TrEEOnTcg2ThunkEnd
//

#include <IndustryStandard/TcgPhysicalPresence.h>

#include <Base.h>
#include <KernelSetupConfig.h>

#define CONFIRM_BUFFER_SIZE         4096

EFI_HII_HANDLE mTcg2PpStringPackHandle = NULL;
EFI_GUID       mTcg2UpdatedPlatformAuthGuid = {
  0xebab103e, 0x8906, 0x4dc8, 0xbd, 0xd1, 0xa0, 0xbc, 0x26, 0xb1, 0xba, 0x86
};

/**
  Check and execute the physical presence command requested

  @param[in]  Event     Event whose notification function is being invoked
  @param[in]  Handle    Checkpoint handle.

**/
VOID
EFIAPI
Tcg2ExecuteTpmRequestCallback (
  IN      EFI_EVENT                           Event,
  IN      H2O_CP_HANDLE                       Handle
  );

/**
  Lock TrEE PPI flags varaible.

  @param[in]  Event        Event whose notification function is being invoked
  @param[in]  Context      Pointer to the notification function's context

**/
VOID
EFIAPI
LockTrEEPpFlagVariable (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  );

/**
  Lock TCG2 PPI flags varaible.

  @param[in]  Event        Event whose notification function is being invoked
  @param[in]  Context      Pointer to the notification function's context

**/
VOID
EFIAPI
LockTcg2PpFlagVariable (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  );

/**
  BDS Init check point notification handler.
  Sync TrEE PP variables with TCG2 PP variables.

  @param[in]  Event     Event whose notification function is being invoked
  @param[in]  Handle    Checkpoint handle.
**/
VOID
EFIAPI
SyncTrEEPpVariableCallback (
  IN      EFI_EVENT                           Event,
  IN      H2O_CP_HANDLE                       Handle
  );

/**
  Execute physical presence operation.

  @param[in]      This                Pointer to EFI_TREE_PHYSICAL_PRESENCE_PROTOCOL.
  @param[in]      CommandCode         Physical presence operation value.
  @param[in]      LastReturnedCode    Return code.
  @param[in, out] PpiFlags            The physical presence interface flags.

  @retval EFI_SUCCESS                         Operation completed successfully.
  @retval EFI_INVALID_PARAMETER               One or more of the parameters are incorrect.
  @retval Others                              Unexpected error.

**/
EFI_STATUS
EFIAPI
ExeucteTcg2PhysicalPresence (
  IN      EFI_TREE_PHYSICAL_PRESENCE_PROTOCOL *This,
  IN      UINT32                              CommandCode,
  OUT     UINT32                              *LastReturnedCode,
  IN OUT  EFI_TREE_PHYSICAL_PRESENCE_FLAGS    *PpiFlags OPTIONAL
  );

/**
  Execute physical presence operation with TCG2 definition.

  @param[in]      This                        EFI TrEE Protocol instance.
  @param[in]      CommandCode                 Physical presence operation value.
  @param[out]     LastReturnedCode            Execute result
  @param[in, out] PpiFlags                    Physical presence interface flags.
  @param[in]      CommandParameter            Physical presence operation parameter.

  @retval EFI_SUCCESS                         Operation completed successfully.
  @retval EFI_INVALID_PARAMETER               One or more of the parameters are incorrect.
  @retval Others                              Unexpected error.

**/
EFI_STATUS
EFIAPI
ExeucteTcg2PhysicalPresenceEx (
  IN      EFI_TREE_PHYSICAL_PRESENCE_PROTOCOL *This,
  IN      UINT32                              CommandCode,
  OUT     UINT32                              *LastReturnedCode,
  IN OUT  EFI_TCG2_PHYSICAL_PRESENCE_FLAGS    *PpiFlags OPTIONAL,
  IN      UINT32                              CommandParameter
  );

EFI_TREE_PHYSICAL_PRESENCE_PROTOCOL  mTrEEPhysicalPresenceProtocol = {
  ExeucteTcg2PhysicalPresence,
  ExeucteTcg2PhysicalPresenceEx
  };

/**
  Provides lastest platformAuth value.

  @retval PlatformAuth
**/
STATIC
TPM2B_AUTH *
GetAuthKey (
  VOID
  )
{
  EFI_STATUS           Status;
  TPM2B_AUTH           *Auth;

  Status = gBS->LocateProtocol (&mTcg2UpdatedPlatformAuthGuid, NULL, (VOID **) &Auth);
  if (EFI_ERROR (Status)) {
    //
    // Auth value is default before protocol installation
    //
    return NULL;
  }
  return Auth;
}

/**
  Disable the specific hierarchy.

  @param [in]  PlatformAuth    Platform auth value. NULL means no platform auth change.
  @param [in]  PpiFlags        Pointer to EFI_TCG2_PHYSICAL_PRESENCE_FLAGS.

  @retval EFI_SUCCESS          Operation completed successfully.
  @retval other                The operation was unsuccessful.
**/
STATIC
EFI_STATUS
Tpm2HierarchyDisable (
  IN TPM2B_AUTH                          *PlatformAuth,
  IN EFI_TCG2_PHYSICAL_PRESENCE_FLAGS    *PpiFlags
  )
{
  EFI_STATUS                Status;
  TPMS_AUTH_COMMAND         *AuthSession;
  TPMS_AUTH_COMMAND         LocalAuthSession;

  if (PlatformAuth == NULL) {
    AuthSession = NULL;
  } else {
    ZeroMem (&LocalAuthSession, sizeof(LocalAuthSession));
    LocalAuthSession.sessionHandle = TPM_RS_PW;
    LocalAuthSession.hmac.size = PlatformAuth->size;
    CopyMem (LocalAuthSession.hmac.buffer, PlatformAuth->buffer, PlatformAuth->size);
    AuthSession = &LocalAuthSession;
  }

  Status = EFI_SUCCESS;
  if (PpiFlags->PPFlags & TCG2_BIOS_INFORMATION_FLAG_HIERACHY_CONTROL_STORAGE_DISABLE) {
    Status = Tpm2HierarchyControl (TPM_RH_PLATFORM, AuthSession, TPM_RH_OWNER, NO);
    ASSERT_EFI_ERROR (Status);
    DEBUG ((EFI_D_INFO, "[TPM2] Disable Storage Hierarchy %r\n", Status));
  }

  if (PpiFlags->PPFlags & TCG2_BIOS_INFORMATION_FLAG_HIERACHY_CONTROL_ENDORSEMENT_DISABLE) {
    Status = Tpm2HierarchyControl (TPM_RH_PLATFORM, AuthSession, TPM_RH_ENDORSEMENT, NO);
    ASSERT_EFI_ERROR (Status);
    DEBUG ((EFI_D_INFO, "[TPM2] Disable Storage Hierarchy %r\n", Status));
  }

  return Status;
}

/**
  Send ClearControl and Clear command to TPM.

  @retval EFI_SUCCESS           Operation completed successfully.
  @retval EFI_TIMEOUT           The register can't run into the expected status in time.
  @retval EFI_BUFFER_TOO_SMALL  Response data buffer is too small.
  @retval EFI_DEVICE_ERROR      Unexpected device behavior.

**/
STATIC
EFI_STATUS
Tpm2CommandClear (
  IN TPM2B_AUTH             *PlatformAuth OPTIONAL
  )
{
  EFI_STATUS                Status;
  TPMS_AUTH_COMMAND         *AuthSession;
  TPMS_AUTH_COMMAND         LocalAuthSession;

  if (PlatformAuth == NULL) {
    AuthSession = NULL;
  } else {
    AuthSession = &LocalAuthSession;
    ZeroMem (&LocalAuthSession, sizeof(LocalAuthSession));
    LocalAuthSession.sessionHandle = TPM_RS_PW;
    LocalAuthSession.hmac.size = PlatformAuth->size;
    CopyMem (LocalAuthSession.hmac.buffer, PlatformAuth->buffer, PlatformAuth->size);
  }

  DEBUG ((EFI_D_INFO, "Tpm2ClearControl ... \n"));
  Status = Tpm2ClearControl (TPM_RH_PLATFORM, AuthSession, NO);
  DEBUG ((EFI_D_INFO, "Tpm2ClearControl - %r\n", Status));
  if (EFI_ERROR (Status)) {
    goto Done;
  }
  DEBUG ((EFI_D_INFO, "Tpm2Clear ... \n"));
  Status = Tpm2Clear (TPM_RH_PLATFORM, AuthSession);
  DEBUG ((EFI_D_INFO, "Tpm2Clear - %r\n", Status));

Done:
  ZeroMem (&LocalAuthSession.hmac, sizeof(LocalAuthSession.hmac));
  return Status;
}

/**
  Change EPS.

  @param[in]  PlatformAuth      Platform auth value. NULL means no platform auth change.

  @retval EFI_SUCCESS Operation completed successfully.
**/
STATIC
EFI_STATUS
Tpm2CommandChangeEps (
  IN TPM2B_AUTH                *PlatformAuth OPTIONAL
  )
{
  EFI_STATUS                Status;
  TPMS_AUTH_COMMAND         *AuthSession;
  TPMS_AUTH_COMMAND         LocalAuthSession;

  if (PlatformAuth == NULL) {
    AuthSession = NULL;
  } else {
    AuthSession = &LocalAuthSession;
    ZeroMem (&LocalAuthSession, sizeof(LocalAuthSession));
    LocalAuthSession.sessionHandle = TPM_RS_PW;
    LocalAuthSession.hmac.size = PlatformAuth->size;
    CopyMem (LocalAuthSession.hmac.buffer, PlatformAuth->buffer, PlatformAuth->size);
  }

  Status = Tpm2ChangeEPS (TPM_RH_PLATFORM, AuthSession);
  DEBUG ((EFI_D_INFO, "Tpm2ChangeEPS - %r\n", Status));
  return Status;
}


/**
  Execute physical presence operation requested by the OS.

  @param[in]      This                Pointer to EFI_TREE_PHYSICAL_PRESENCE_PROTOCOL.
  @param[in]      CommandCode         Physical presence operation value.
  @param[in]      LastReturnedCode    Return code.
  @param[in, out] PpiFlags            The physical presence interface flags.
  @param[in]      CommandParameter    Physical presence operation parameter.

  @retval EFI_SUCCESS                         Operation completed successfully.
  @retval EFI_INVALID_PARAMETER               One or more of the parameters are incorrect.
  @retval EFI_NOT_READY                       Required service is not ready.
  @retval Others                              Unexpected error.

**/
STATIC
EFI_STATUS
Tcg2ExecutePhysicalPresence (
  IN      EFI_TREE_PHYSICAL_PRESENCE_PROTOCOL *This,
  IN      UINT32                              CommandCode,
  OUT     UINT32                              *LastReturnedCode,
  IN OUT  EFI_TCG2_PHYSICAL_PRESENCE_FLAGS    *PpiFlags,
  IN      UINT32                              CommandParameter
  )
{
  EFI_STATUS                                  Status;
  UINT32                                      TpmResult;
  EFI_TCG2_EVENT_ALGORITHM_BITMAP             TpmHashAlgorithmBitmap;
  UINT32                                      ActivePcrBanks;
  VOID                                        *Interface;

  if (LastReturnedCode == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (CommandCode == TCG2_PHYSICAL_PRESENCE_NO_ACTION) {
    *LastReturnedCode = TCG_PP_OPERATION_RESPONSE_SUCCESS;
    return EFI_SUCCESS;
  }

  if (CommandCode <= TCG2_PHYSICAL_PRESENCE_NO_ACTION_MAX) {
    //
    // Need TCG2 protocol.
    //
    Status = gBS->LocateProtocol (&gEfiTcg2ProtocolGuid, NULL, (VOID **)&Interface);
    if (EFI_ERROR (Status)) {
      DEBUG ((EFI_D_ERROR, "Tcg2ExecutePhysicalPresence - TCG2 Protocol is not ready. CommandCode = 0x%x \n", CommandCode));
      return EFI_NOT_READY;
    }
  }

  TpmResult = TCG_PP_OPERATION_RESPONSE_BIOS_FAILURE;
  switch (CommandCode) {

  case TCG2_PHYSICAL_PRESENCE_ENABLE:
    PpiFlags->PPFlags &= ~TCG2_BIOS_INFORMATION_FLAG_HIERACHY_CONTROL_STORAGE_DISABLE;
    PpiFlags->PPFlags &= ~TCG2_BIOS_INFORMATION_FLAG_HIERACHY_CONTROL_ENDORSEMENT_DISABLE;
    TpmResult = TCG_PP_OPERATION_RESPONSE_SUCCESS;
    break;

  case TCG2_PHYSICAL_PRESENCE_DISABLE:
    PpiFlags->PPFlags |= TCG2_BIOS_INFORMATION_FLAG_HIERACHY_CONTROL_STORAGE_DISABLE;
    PpiFlags->PPFlags |= TCG2_BIOS_INFORMATION_FLAG_HIERACHY_CONTROL_ENDORSEMENT_DISABLE;
    Status = Tpm2HierarchyDisable (GetAuthKey (), PpiFlags);
    if (EFI_ERROR (Status)) {
      TpmResult = TCG_PP_OPERATION_RESPONSE_BIOS_FAILURE;
    } else {
      TpmResult = TCG_PP_OPERATION_RESPONSE_SUCCESS;
    }
    break;

  case TCG2_PHYSICAL_PRESENCE_DISABLE_ENDORSEMENT_ENABLE_STORAGE_HIERARCHY:
    PpiFlags->PPFlags &= ~TCG2_BIOS_INFORMATION_FLAG_HIERACHY_CONTROL_STORAGE_DISABLE;
    PpiFlags->PPFlags |= TCG2_BIOS_INFORMATION_FLAG_HIERACHY_CONTROL_ENDORSEMENT_DISABLE;
    Status = Tpm2HierarchyDisable (GetAuthKey (), PpiFlags);
    if (EFI_ERROR (Status)) {
      TpmResult = TCG_PP_OPERATION_RESPONSE_BIOS_FAILURE;
    } else {
      TpmResult = TCG_PP_OPERATION_RESPONSE_SUCCESS;
    }
    break;

  case TCG2_PHYSICAL_PRESENCE_SET_PP_REQUIRED_FOR_TURN_ON_TRUE:
    PpiFlags->PPFlags |= TCG2_BIOS_TPM_MANAGEMENT_FLAG_PP_REQUIRED_FOR_TURN_ON;
    TpmResult = TCG_PP_OPERATION_RESPONSE_SUCCESS;
    break;

  case TCG2_PHYSICAL_PRESENCE_SET_PP_REQUIRED_FOR_TURN_ON_FALSE:
    PpiFlags->PPFlags &= ~TCG2_BIOS_TPM_MANAGEMENT_FLAG_PP_REQUIRED_FOR_TURN_ON;
    TpmResult = TCG_PP_OPERATION_RESPONSE_SUCCESS;
    break;

  case TCG2_PHYSICAL_PRESENCE_SET_PP_REQUIRED_FOR_TURN_OFF_TRUE:
    PpiFlags->PPFlags |= TCG2_BIOS_TPM_MANAGEMENT_FLAG_PP_REQUIRED_FOR_TURN_OFF;
    TpmResult = TCG_PP_OPERATION_RESPONSE_SUCCESS;
    break;

  case TCG2_PHYSICAL_PRESENCE_SET_PP_REQUIRED_FOR_TURN_OFF_FALSE:
    PpiFlags->PPFlags &= ~TCG2_BIOS_TPM_MANAGEMENT_FLAG_PP_REQUIRED_FOR_TURN_OFF;
    TpmResult = TCG_PP_OPERATION_RESPONSE_SUCCESS;
    break;

  case TCG2_PHYSICAL_PRESENCE_SET_PP_REQUIRED_FOR_CHANGE_EPS_TRUE:
    PpiFlags->PPFlags |= TCG2_BIOS_TPM_MANAGEMENT_FLAG_PP_REQUIRED_FOR_CHANGE_EPS;
    TpmResult = TCG_PP_OPERATION_RESPONSE_SUCCESS;
    break;

  case TCG2_PHYSICAL_PRESENCE_SET_PP_REQUIRED_FOR_CHANGE_EPS_FALSE:
    PpiFlags->PPFlags &= ~TCG2_BIOS_TPM_MANAGEMENT_FLAG_PP_REQUIRED_FOR_CHANGE_EPS;
    TpmResult = TCG_PP_OPERATION_RESPONSE_SUCCESS;
    break;

  case TCG2_PHYSICAL_PRESENCE_SET_PP_REQUIRED_FOR_CHANGE_PCRS_TRUE:
    PpiFlags->PPFlags |= TCG2_BIOS_TPM_MANAGEMENT_FLAG_PP_REQUIRED_FOR_CHANGE_PCRS;
    TpmResult = TCG_PP_OPERATION_RESPONSE_SUCCESS;
    break;

  case TCG2_PHYSICAL_PRESENCE_SET_PP_REQUIRED_FOR_CHANGE_PCRS_FALSE:
    PpiFlags->PPFlags &= ~TCG2_BIOS_TPM_MANAGEMENT_FLAG_PP_REQUIRED_FOR_CHANGE_PCRS;
    TpmResult = TCG_PP_OPERATION_RESPONSE_SUCCESS;
    break;

  case TCG2_PHYSICAL_PRESENCE_CLEAR:
  case TCG2_PHYSICAL_PRESENCE_ENABLE_CLEAR:
  case TCG2_PHYSICAL_PRESENCE_ENABLE_CLEAR_2:
  case TCG2_PHYSICAL_PRESENCE_ENABLE_CLEAR_3:
    Status = Tpm2CommandClear (GetAuthKey ());
    if (EFI_ERROR (Status)) {
      TpmResult = TCG_PP_OPERATION_RESPONSE_BIOS_FAILURE;
    } else {
      TpmResult = TCG_PP_OPERATION_RESPONSE_SUCCESS;
    }
    break;

  case TCG2_PHYSICAL_PRESENCE_SET_PP_REQUIRED_FOR_CLEAR_TRUE:
    PpiFlags->PPFlags |= TCG2_BIOS_TPM_MANAGEMENT_FLAG_PP_REQUIRED_FOR_CLEAR;
    TpmResult = TCG_PP_OPERATION_RESPONSE_SUCCESS;
    break;

  case TCG2_PHYSICAL_PRESENCE_SET_PP_REQUIRED_FOR_CLEAR_FALSE:
    PpiFlags->PPFlags &= ~TCG2_BIOS_TPM_MANAGEMENT_FLAG_PP_REQUIRED_FOR_CLEAR;
    TpmResult = TCG_PP_OPERATION_RESPONSE_SUCCESS;
    break;

  case TCG2_PHYSICAL_PRESENCE_SET_PCR_BANKS:
    Status = Tpm2GetCapabilitySupportedAndActivePcrs (&TpmHashAlgorithmBitmap, &ActivePcrBanks);
    ASSERT_EFI_ERROR (Status);
    Status = Tpm2PcrAllocateBanks (GetAuthKey (), TpmHashAlgorithmBitmap, CommandParameter);
    if (EFI_ERROR (Status)) {
      TpmResult = TCG_PP_OPERATION_RESPONSE_BIOS_FAILURE;
    } else {
      TpmResult = TCG_PP_OPERATION_RESPONSE_SUCCESS;
    }
    break;

  case TCG2_PHYSICAL_PRESENCE_CHANGE_EPS:
    Status = Tpm2CommandChangeEps (GetAuthKey ());
    if (EFI_ERROR (Status)) {
      TpmResult = TCG_PP_OPERATION_RESPONSE_BIOS_FAILURE;
    } else {
      TpmResult = TCG_PP_OPERATION_RESPONSE_SUCCESS;
    }
    break;

  case TCG2_PHYSICAL_PRESENCE_LOG_ALL_DIGESTS:
    Status = Tpm2GetCapabilitySupportedAndActivePcrs (&TpmHashAlgorithmBitmap, &ActivePcrBanks);
    ASSERT_EFI_ERROR (Status);
    Status = Tpm2PcrAllocateBanks (GetAuthKey (), TpmHashAlgorithmBitmap, TpmHashAlgorithmBitmap);
    if (EFI_ERROR (Status)) {
      TpmResult = TCG_PP_OPERATION_RESPONSE_BIOS_FAILURE;
    } else {
      TpmResult = TCG_PP_OPERATION_RESPONSE_SUCCESS;
    }
    break;

  case TCG2_PHYSICAL_PRESENCE_ENABLE_BLOCK_SID:
    PpiFlags->PPFlags |= TCG2_BIOS_STORAGE_MANAGEMENT_FLAG_ENABLE_BLOCK_SID;
    TpmResult = TCG_PP_OPERATION_RESPONSE_SUCCESS;

  case TCG2_PHYSICAL_PRESENCE_DISABLE_BLOCK_SID:
    PpiFlags->PPFlags &= ~TCG2_BIOS_STORAGE_MANAGEMENT_FLAG_ENABLE_BLOCK_SID;
    TpmResult = TCG_PP_OPERATION_RESPONSE_SUCCESS;

  case TCG2_PHYSICAL_PRESENCE_SET_PP_REQUIRED_FOR_ENABLE_BLOCK_SID_FUNC_TRUE:
    PpiFlags->PPFlags |= TCG2_BIOS_STORAGE_MANAGEMENT_FLAG_PP_REQUIRED_FOR_ENABLE_BLOCK_SID;
    TpmResult = TCG_PP_OPERATION_RESPONSE_SUCCESS;

  case TCG2_PHYSICAL_PRESENCE_SET_PP_REQUIRED_FOR_ENABLE_BLOCK_SID_FUNC_FALSE:
    PpiFlags->PPFlags &= ~TCG2_BIOS_STORAGE_MANAGEMENT_FLAG_PP_REQUIRED_FOR_ENABLE_BLOCK_SID;
    TpmResult = TCG_PP_OPERATION_RESPONSE_SUCCESS;

  case TCG2_PHYSICAL_PRESENCE_SET_PP_REQUIRED_FOR_DISABLE_BLOCK_SID_FUNC_TRUE:
    PpiFlags->PPFlags |= TCG2_BIOS_STORAGE_MANAGEMENT_FLAG_PP_REQUIRED_FOR_DISABLE_BLOCK_SID;
    TpmResult = TCG_PP_OPERATION_RESPONSE_SUCCESS;

  case TCG2_PHYSICAL_PRESENCE_SET_PP_REQUIRED_FOR_DISABLE_BLOCK_SID_FUNC_FALSE:
    PpiFlags->PPFlags &= ~TCG2_BIOS_STORAGE_MANAGEMENT_FLAG_PP_REQUIRED_FOR_DISABLE_BLOCK_SID;
    TpmResult = TCG_PP_OPERATION_RESPONSE_SUCCESS;

  default:
    if (CommandCode <= TCG2_PHYSICAL_PRESENCE_NO_ACTION_MAX) {
      TpmResult = TCG_PP_OPERATION_RESPONSE_SUCCESS;
    } else {
      TpmResult = TCG_PP_OPERATION_RESPONSE_BIOS_FAILURE;
    }
    break;
  }

  *LastReturnedCode = TpmResult;
  return EFI_SUCCESS;
}

/**
  The driver's entry point.

  @param[in] ImageHandle  The firmware allocated handle for the EFI image.
  @param[in] SystemTable  A pointer to the EFI System Table.

  @retval EFI_SUCCESS     The entry point is executed successfully.
  @retval other           Some error occurs when executing this entry point.

**/
EFI_STATUS
EFIAPI
DriverEntry (
  IN EFI_HANDLE                               ImageHandle,
  IN EFI_SYSTEM_TABLE                         *SystemTable
  )
{
  EFI_STATUS                                  Status;
  UINTN                                       DataSize;
  EFI_TCG2_PHYSICAL_PRESENCE                  PpData;
  EFI_TCG2_PHYSICAL_PRESENCE_FLAGS            PpiFlags;
  VOID                                        *Registration;
  H2O_CP_HANDLE                               BdsCpDisplayBeforeHandle;
  H2O_CP_HANDLE                               BdsCpInitHandle;
  EFI_TREE_PHYSICAL_PRESENCE                  TrEEPpData;

  //
  // Initialize physical presence flags.
  //
  DataSize = sizeof (EFI_TCG2_PHYSICAL_PRESENCE_FLAGS);
  Status = gRT->GetVariable (
                  TCG2_PHYSICAL_PRESENCE_FLAGS_VARIABLE,
                  &gEfiTcg2PhysicalPresenceGuid,
                  NULL,
                  &DataSize,
                  &PpiFlags
                  );
  if (EFI_ERROR (Status)) {
    if (Status == EFI_NOT_FOUND) {
      PpiFlags.PPFlags = PcdGet32 (PcdH2OTpm2PpFlagsDefault);
      Status   = gRT->SetVariable (
                        TCG2_PHYSICAL_PRESENCE_FLAGS_VARIABLE,
                        &gEfiTcg2PhysicalPresenceGuid,
                        EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                        sizeof (EFI_TCG2_PHYSICAL_PRESENCE_FLAGS),
                        &PpiFlags
                        );
    }
    ASSERT_EFI_ERROR (Status);
  }

  //
  // Initialize physical presence variable.
  //
  DataSize = sizeof (EFI_TCG2_PHYSICAL_PRESENCE);
  Status = gRT->GetVariable (
                  TCG2_PHYSICAL_PRESENCE_VARIABLE,
                  &gEfiTcg2PhysicalPresenceGuid,
                  NULL,
                  &DataSize,
                  &PpData
                  );
  if (EFI_ERROR (Status)) {
    if (Status == EFI_NOT_FOUND) {
      ZeroMem ((VOID*)&PpData, sizeof (PpData));
      DataSize = sizeof (EFI_TCG2_PHYSICAL_PRESENCE);
      Status   = gRT->SetVariable (
                        TCG2_PHYSICAL_PRESENCE_VARIABLE,
                        &gEfiTcg2PhysicalPresenceGuid,
                        EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                        DataSize,
                        &PpData
                        );
    }
    ASSERT_EFI_ERROR (Status);
  }

  //
  // Lock the PpFlag variable.
  //
  EfiCreateProtocolNotifyEvent (
    &gEdkiiVariableLockProtocolGuid,
    TPL_CALLBACK,
    LockTcg2PpFlagVariable,
    NULL,
    (VOID **) &Registration
    );

  //
  // TrEEOnTcg2ThunkBegin
  //
  if (PcdGetBool (PcdCompatibleTrEEPpSupported)) {
    //
    // Syn TCG2 variable if TREE has request within.
    //
    DataSize = sizeof (EFI_TREE_PHYSICAL_PRESENCE);
    Status = gRT->GetVariable (
                    TREE_PHYSICAL_PRESENCE_VARIABLE,
                    &gEfiTrEEPhysicalPresenceGuid,
                    NULL,
                    &DataSize,
                    &TrEEPpData
                    );
    if (!EFI_ERROR (Status) && (TrEEPpData.PPRequest != TREE_PHYSICAL_PRESENCE_NO_ACTION)) {
      PpData.PPRequest          = TrEEPpData.PPRequest;
      PpData.LastPPRequest      = TrEEPpData.LastPPRequest;
      PpData.PPResponse         = TrEEPpData.PPResponse;
      PpData.PPRequestParameter = 0;

      Status   = gRT->SetVariable (
                        TCG2_PHYSICAL_PRESENCE_VARIABLE,
                        &gEfiTcg2PhysicalPresenceGuid,
                        EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                        sizeof(EFI_TCG2_PHYSICAL_PRESENCE),
                        &PpData
                        );
      ASSERT_EFI_ERROR (Status);

      TrEEPpData.PPRequest = TREE_PHYSICAL_PRESENCE_NO_ACTION;
      Status   = gRT->SetVariable (
                        TREE_PHYSICAL_PRESENCE_VARIABLE,
                        &gEfiTrEEPhysicalPresenceGuid,
                        EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                        sizeof(EFI_TREE_PHYSICAL_PRESENCE),
                        &TrEEPpData
                        );
      ASSERT_EFI_ERROR (Status);
    }

    //
    // Lock the PpFlag variable.
    //
    EfiCreateProtocolNotifyEvent (
      &gEdkiiVariableLockProtocolGuid,
      TPL_CALLBACK,
      LockTrEEPpFlagVariable,
      NULL,
      (VOID **) &Registration
      );

    if (FeaturePcdGet (PcdH2OBdsCpDisplayBeforeSupported)) {
      //
      // Sync the result of request execution in TCG2 variable back to TREE variable, and lock TrEEPpiFlags.
      //
      Status = H2OCpRegisterHandler (
                 &gH2OBdsCpDisplayBeforeGuid,
                 SyncTrEEPpVariableCallback,
                 H2O_CP_MEDIUM_LOW,
                 &BdsCpDisplayBeforeHandle
                 );
      if (EFI_ERROR (Status)) {
        DEBUG ((EFI_D_ERROR, "Checkpoint Register Fail: %g (%r)\n", &gH2OBdsCpDisplayBeforeGuid, Status));
        return Status;
      }
      DEBUG ((EFI_D_INFO, "Checkpoint Registered: %g (%r)\n", &gH2OBdsCpDisplayBeforeGuid, Status));
    }
  }
  //
  // TrEEOnTcg2ThunkEnd
  //

  Status = gBS->InstallProtocolInterface (
                  &ImageHandle,
                  &gEfiTrEEPhysicalPresenceProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &mTrEEPhysicalPresenceProtocol
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_INFO, "[TPM2] Install TrEEPhysicalPresenceProtocol failed, Status=%r\n", Status));
    return Status;
  }

  if (FeaturePcdGet (PcdH2OBdsCpInitSupported)) {
    Status = H2OCpRegisterHandler (
               &gH2OBdsCpInitGuid,
               Tcg2ExecuteTpmRequestCallback,
               H2O_CP_MEDIUM,
               &BdsCpInitHandle
               );
    if (EFI_ERROR (Status)) {
      DEBUG ((EFI_D_ERROR, "Checkpoint Register Fail: %g (%r)\n", &gH2OBdsCpInitGuid, Status));
      return Status;
    }
    DEBUG ((EFI_D_INFO, "Checkpoint Registered: %g (%r)\n", &gH2OBdsCpInitGuid, Status));
  }

  return Status;
}

/**
  Execute physical presence operation requested by the OS.

  @param[in]      This                Pointer to EFI_TREE_PHYSICAL_PRESENCE_PROTOCOL.
  @param[in]      CommandCode         Physical presence operation value.
  @param[in]      LastReturnedCode    Return code.
  @param[in, out] PpiFlags            The physical presence interface flags.

  @retval EFI_SUCCESS                         Operation completed successfully.
  @retval EFI_INVALID_PARAMETER               One or more of the parameters are incorrect.
  @retval Others                              Unexpected error.

**/
EFI_STATUS
EFIAPI
ExeucteTcg2PhysicalPresence (
  IN      EFI_TREE_PHYSICAL_PRESENCE_PROTOCOL *This,
  IN      UINT32                              CommandCode,
  OUT     UINT32                              *LastReturnedCode,
  IN OUT  EFI_TREE_PHYSICAL_PRESENCE_FLAGS    *PpiFlags OPTIONAL
  )
{
  EFI_STATUS                                  Status;
  EFI_TCG2_PHYSICAL_PRESENCE_FLAGS            TcgPpiFlags;

  if (PpiFlags == NULL) {
    TcgPpiFlags.PPFlags = 0;
    Status = Tcg2ExecutePhysicalPresence (
               This,
               CommandCode,
               LastReturnedCode,
               &TcgPpiFlags,
               0
               );
  } else {
    TcgPpiFlags.PPFlags = PpiFlags->PPFlags;
    Status = Tcg2ExecutePhysicalPresence (
               This,
               CommandCode,
               LastReturnedCode,
               &TcgPpiFlags,
               0
               );
    PpiFlags->PPFlags = (UINT8)TcgPpiFlags.PPFlags;
  }
  return Status;
}

/**
  Execute physical presence operation with TCG2 definition.

  @param[in]      This                        EFI TrEE Protocol instance.
  @param[in]      CommandCode                 Physical presence operation value.
  @param[out]     LastReturnedCode            Execute result
  @param[in, out] PpiFlags                    Physical presence interface flags.
  @param[in]      CommandParameter            Physical presence operation parameter.

  @retval EFI_SUCCESS                         Operation completed successfully.
  @retval EFI_INVALID_PARAMETER               One or more of the parameters are incorrect.
  @retval Others                              Unexpected error.

**/
EFI_STATUS
EFIAPI
ExeucteTcg2PhysicalPresenceEx (
  IN      EFI_TREE_PHYSICAL_PRESENCE_PROTOCOL *This,
  IN      UINT32                              CommandCode,
  OUT     UINT32                              *LastReturnedCode,
  IN OUT  EFI_TCG2_PHYSICAL_PRESENCE_FLAGS    *PpiFlags OPTIONAL,
  IN      UINT32                              CommandParameter
  )
{
  EFI_STATUS                                  Status;
  EFI_TCG2_PHYSICAL_PRESENCE_FLAGS            TcgPpiFlags;

  if (PpiFlags == NULL) {
    TcgPpiFlags.PPFlags = 0;
    Status = Tcg2ExecutePhysicalPresence (
               This,
               CommandCode,
               LastReturnedCode,
               &TcgPpiFlags,
               CommandParameter
               );
  } else {
    Status = Tcg2ExecutePhysicalPresence (
               This,
               CommandCode,
               LastReturnedCode,
               PpiFlags,
               CommandParameter
               );
  }
  return Status;
}

/**
  Lock TCG2 PPI flags varaible.

  @param[in]  Event        Event whose notification function is being invoked
  @param[in]  Context      Pointer to the notification function's context

**/
VOID
EFIAPI
LockTcg2PpFlagVariable (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  )
{
  EFI_STATUS                        Status;
  EDKII_VARIABLE_LOCK_PROTOCOL      *VariableLockProtocol;

  Status = gBS->LocateProtocol (&gEdkiiVariableLockProtocolGuid, NULL, (VOID **) &VariableLockProtocol);
  if (EFI_ERROR (Status)) {
    return;
  }

  //
  // This flags variable controls whether physical presence is required for TPM command.
  // It should be protected from malicious software. We set it as read-only variable here.
  //
  Status = VariableLockProtocol->RequestToLock (
                                   VariableLockProtocol,
                                   TCG2_PHYSICAL_PRESENCE_FLAGS_VARIABLE,
                                   &gEfiTcg2PhysicalPresenceGuid
                                   );
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "[TPM2] Error when lock variable %s, Status = %r\n", TCG2_PHYSICAL_PRESENCE_FLAGS_VARIABLE, Status));
    ASSERT_EFI_ERROR (Status);
  }

  gBS->CloseEvent (Event);
}

/**
  Lock TrEE PPI flags varaible.

  @param[in]  Event        Event whose notification function is being invoked
  @param[in]  Context      Pointer to the notification function's context

**/
VOID
EFIAPI
LockTrEEPpFlagVariable (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  )
{
  EFI_STATUS                        Status;
  EDKII_VARIABLE_LOCK_PROTOCOL      *VariableLockProtocol;

  Status = gBS->LocateProtocol (&gEdkiiVariableLockProtocolGuid, NULL, (VOID **) &VariableLockProtocol);
  if (EFI_ERROR (Status)) {
    return;
  }

  //
  // This flags variable controls whether physical presence is required for TPM command.
  // It should be protected from malicious software. We set it as read-only variable here.
  //
  Status = VariableLockProtocol->RequestToLock (
                                   VariableLockProtocol,
                                   TREE_PHYSICAL_PRESENCE_FLAGS_VARIABLE,
                                   &gEfiTrEEPhysicalPresenceGuid
                                   );
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "[TPM2] Error when lock variable %s, Status = %r\n", TREE_PHYSICAL_PRESENCE_FLAGS_VARIABLE, Status));
    ASSERT_EFI_ERROR (Status);
  }

  gBS->CloseEvent (Event);
}

/**
  Check and execute the physical presence command requested

  @param[in]  Event     Event whose notification function is being invoked
  @param[in]  Handle    Checkpoint handle.

**/
VOID
EFIAPI
Tcg2NeedConsoleExecuteTpmRequestCallback (
  IN      EFI_EVENT                           Event,
  IN      H2O_CP_HANDLE                       Handle
  )
{
  H2OCpUnregisterHandler (Handle);

  //
  // Execute pending TPM request.
  //
  Tcg2PhysicalPresenceLibProcessRequest (GetAuthKey ());
}

/**
  Check and execute the physical presence command requested

  @param[in]  Event     Event whose notification function is being invoked
  @param[in]  Handle    Checkpoint handle.

**/
VOID
EFIAPI
Tcg2ExecuteTpmRequestCallback (
  IN      EFI_EVENT                           Event,
  IN      H2O_CP_HANDLE                       Handle
  )
{
  EFI_STATUS                              Status;
  H2O_CP_HANDLE                           CpHandle;

  H2OCpUnregisterHandler (Handle);

  if (Tcg2PhysicalPresenceLibNeedUserConfirm ()) {
    //
    // User confirm dialog is needed, executes the request when connect console is ready.
    //
    if (FeaturePcdGet (PcdH2OBdsCpDisplayBeforeSupported)) {
      Status = H2OCpRegisterHandler (
                 &gH2OBdsCpDisplayBeforeGuid,
                 Tcg2NeedConsoleExecuteTpmRequestCallback,
                 H2O_CP_MEDIUM,
                 &CpHandle
                 );
      if (EFI_ERROR (Status)) {
        DEBUG ((EFI_D_ERROR, "Checkpoint Register Fail: %g (%r)\n", &gH2OBdsCpDisplayBeforeGuid, Status));
      }
      DEBUG ((EFI_D_INFO, "Checkpoint Registered: %g (%r)\n", &gH2OBdsCpDisplayBeforeGuid, Status));
    }
    return;
  }
  //
  // Execute pending TPM request.
  //
  Tcg2PhysicalPresenceLibProcessRequest (GetAuthKey ());
}

/**
  BDS Init check point notification handler.
  Replaces the current platformAuth with a value from the RNG.

  @param[in]  Event     Event whose notification function is being invoked
  @param[in]  Context   Pointer to the notification function's context
**/
VOID
EFIAPI
SyncTrEEPpVariableCallback (
  IN      EFI_EVENT                           Event,
  IN      H2O_CP_HANDLE                       Handle
  )
{
  EFI_STATUS                                  Status;
  VOID                                        *Interface;
  UINTN                                       DataSize;
  EFI_TCG2_PHYSICAL_PRESENCE                  TcgPpData;
  EFI_TCG2_PHYSICAL_PRESENCE_FLAGS            TcgPpiFlags;
  EFI_TREE_PHYSICAL_PRESENCE                  TrEEPpData;
  EFI_TREE_PHYSICAL_PRESENCE_FLAGS            TrEEPpiFlags;

  H2OCpUnregisterHandler (Handle);

  Status = gBS->LocateProtocol (&gH2OTpm20DtpmPublishAcpiTableDoneGuid, NULL, (VOID**)&Interface);
  if (!EFI_ERROR (Status)) {
    //
    // TCG PPI spec 1.30 is supported for DTPM2 driver that owned by kernel.
    // Thunk does not need.
    //
    return;
  }

  //
  // TrEEPhysicalPresence(TCG PPI spec 1.20) on Tcg2PhysicalPresence(TCG PPI spec 1.30) thunk.
  //
  DataSize = sizeof (EFI_TCG2_PHYSICAL_PRESENCE);
  Status = gRT->GetVariable (
                  TCG2_PHYSICAL_PRESENCE_VARIABLE,
                  &gEfiTcg2PhysicalPresenceGuid,
                  NULL,
                  &DataSize,
                  &TcgPpData
                  );
  if (EFI_ERROR (Status)) {
    return;
  }

  DataSize = sizeof (EFI_TCG2_PHYSICAL_PRESENCE_FLAGS);
  Status = gRT->GetVariable (
                  TCG2_PHYSICAL_PRESENCE_FLAGS_VARIABLE,
                  &gEfiTcg2PhysicalPresenceGuid,
                  NULL,
                  &DataSize,
                  &TcgPpiFlags
                  );
  if (EFI_ERROR (Status)) {
    return;
  }

  TrEEPpData.PPRequest     = TcgPpData.PPRequest;
  TrEEPpData.LastPPRequest = TcgPpData.LastPPRequest;
  TrEEPpData.PPResponse    = TcgPpData.PPResponse;

  Status   = gRT->SetVariable (
                    TREE_PHYSICAL_PRESENCE_VARIABLE,
                    &gEfiTrEEPhysicalPresenceGuid,
                    EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                    sizeof(EFI_TREE_PHYSICAL_PRESENCE),
                    &TrEEPpData
                    );
  if (EFI_ERROR (Status)) {
    return;
  }

  TrEEPpiFlags.PPFlags = 0;
  if ((TcgPpiFlags.PPFlags & TCG2_BIOS_TPM_MANAGEMENT_FLAG_PP_REQUIRED_FOR_CLEAR) == 0) {
    TrEEPpiFlags.PPFlags |= TREE_FLAG_NO_PPI_CLEAR;
  }

  Status = gRT->SetVariable (
                  TREE_PHYSICAL_PRESENCE_FLAGS_VARIABLE,
                  &gEfiTrEEPhysicalPresenceGuid,
                  EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                  sizeof(EFI_TREE_PHYSICAL_PRESENCE_FLAGS),
                  &TrEEPpiFlags
                  );
  if (Status == EFI_WRITE_PROTECTED) {
    Status = SetVariableToSensitiveVariable (
                    TREE_PHYSICAL_PRESENCE_FLAGS_VARIABLE,
                    &gEfiTrEEPhysicalPresenceGuid,
                    EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                    sizeof (EFI_TREE_PHYSICAL_PRESENCE_FLAGS),
                    &TrEEPpiFlags
                    );
  }

  if (EFI_ERROR (Status)) {
    return;
  }
}

