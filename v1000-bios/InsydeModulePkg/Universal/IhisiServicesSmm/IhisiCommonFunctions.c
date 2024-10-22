/** @file
  IHISI common functions implementation

;******************************************************************************
;* Copyright (c) 2023, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/
#include "IhisiCommonFunctions.h"

STATIC
IHISI_REGISTER_TABLE
COMMON_REGISTER_TABLE[] = {
  //
  // AH=80h
  //
  { IhisiAuthStatus,   "S80Kn_AuthStatus000", KernelIhisiAuthStatus     },  \
  //
  // AH=81h
  //
  { IhisiAuthLock,     "S81Kn_AuthLock00000", KernelIhisiAuthLock       },  \
  //
  // AH=82h
  //
  { IhisiAuthUnlock,   "S82Kn_AuthUnlock000", KernelIhisiAuthUnlock     },  \
  //
  // AH=83h
  //
  { IhisiGetCmdBuffer, "S83Kn_GetCmdBuf0000", KernelIhisiGetCmdBuffer   },  \
  { IhisiGetCmdBuffer, "S83Kn_GetImageBuf00", KernelGetImageBuffer      },  \
  //
  // AH=84h
  //
  { IhisiAuth ,        "S84Kn_Auth000000000", KernelIhisiAuth           }
};

BOOLEAN              mIhisiLock;
UINT32               mIhisiSessionToken;
STATIC BOOLEAN       mSmmEndOfDxe;
BOOLEAN              mSmmReadyToBoot;
STATIC UINT32        mSecurityLevel;



/**
  This function used  to check input password is whether valid.

  @param[in] CredentialData   Points to IHITI authentication credential data.

  @retval  TRUE      Password is valid.
  @retval  FALSE     Password is invalid.
**/
STATIC
BOOLEAN
IsValidPassword (
  IN IHISI_AUTH_LOCK                   *CredentialData
  )
{
  EFI_STATUS                            Status;
  EFI_SYS_PASSWORD_SERVICE_PROTOCOL     *SysPasswordProtocol;

  if (CredentialData == NULL) {
    return FALSE;
  }
  Status = gSmst->SmmLocateProtocol (
                    &gEfiSysPasswordServiceProtocolGuid,
                    NULL,
                    (VOID **) &SysPasswordProtocol
                    );
  if (EFI_ERROR (Status)) {
    return FALSE;
  }
  //
  // Check either supervisor or user password is enrolled
  //
  if (FeaturePcdGet (PcdH2OIhisiAuthPasswordUser)){
    Status = SysPasswordProtocol->CheckPassword (
                                    SysPasswordProtocol,
                                    (VOID *)(CredentialData + 1),
                                    CredentialData->Size,
                                    TRUE
                                    );
    if (!EFI_ERROR (Status)) {
      return TRUE;
    }
  }
  if (FeaturePcdGet (PcdH2OIhisiAuthPasswordSupervisor)) {
    Status = SysPasswordProtocol->CheckPassword (
                                    SysPasswordProtocol,
                                    (VOID *)(CredentialData + 1),
                                    CredentialData->Size,
                                    FALSE
                                    );
    if (!EFI_ERROR (Status)) {
      return TRUE;
    }
  }
  return FALSE;
}

/**
  Helper function to check if password is already provided.

  @retval  TRUE      Password is already provided.
  @retval  FALSE     No password.
**/
STATIC
BOOLEAN
IsPasswordEnrolled (
  VOID
  )
{
  EFI_STATUS                            Status;
  EFI_SYS_PASSWORD_SERVICE_PROTOCOL     *SysPasswordProtocol;


  Status = gSmst->SmmLocateProtocol (
                    &gEfiSysPasswordServiceProtocolGuid,
                    NULL,
                    (VOID **) &SysPasswordProtocol
                    );
  if (EFI_ERROR (Status)) {
    return FALSE;
  }
  //
  // Check either supervisor or user password is enrolled
  //
  Status = EFI_NOT_FOUND;
  if (FeaturePcdGet (PcdH2OIhisiAuthPasswordUser)){
    Status = SysPasswordProtocol->GetStatus (SysPasswordProtocol, TRUE);
  }
  if (EFI_ERROR (Status) && FeaturePcdGet (PcdH2OIhisiAuthPasswordSupervisor)) {
    Status = SysPasswordProtocol->GetStatus (SysPasswordProtocol, FALSE);
  }
  return EFI_ERROR (Status) ? FALSE : TRUE;
}

/**
  Helper function to check if secure boot is enabled or not

  @retval  TRUE      SecureBoot is enabled.
  @retval  FALSE     SecureBoot is disabled.
**/
STATIC
BOOLEAN
IsSecureBootEnabled (
  VOID
  )
{
  UINT8      SecureBootEnabled;
  UINTN      DataSize;
  EFI_STATUS Status;

  DataSize = sizeof (SecureBootEnabled);
  Status = CommonGetVariable (
             EFI_SECURE_BOOT_MODE_NAME,
             &gEfiGlobalVariableGuid,
             &DataSize,
             &SecureBootEnabled
             );
  if (!EFI_ERROR (Status) && SecureBootEnabled == 1) {
    return TRUE;
  }
  return FALSE;

}


/**
  Helper function to check if system executes under secure environment.

  @retval  TRUE      System executes under secure environment.
  @retval  FALSE     System executes under insecure environment.
**/
STATIC
BOOLEAN
IsSecureEnvironment (
  VOID
  )
{
  //
  // kernel default security environment is always disabled. Project can use gH2OSmmCpIhisiAuthStatusGuid and
  // gH2OSmmCpIhisiAuthCheckGuid checkpoint to determine secure environment is enabled or not.
  //
  return FALSE;
}

/**
  Get current IHISI security levels

  return Bitmask that indicates which security level was used when the IHISI session was opened.
**/
UINT32
GetCurrentIhisiSecurityLevel (
  VOID
  )
{
  return mIhisiLock ? mSecurityLevel : 0;
}

/**
  Get Supported IHISI security levels

  return Bitmask that indicates the security levels supported by IHISI on this platform
**/
UINT32
GetSupportedIhisiSecurityLevels (
  VOID
  )
{
  UINT32         SupportedSecurityLevels;

  if (!FeaturePcdGet (PcdH2OIhisiAuthEnabled)) {
    SupportedSecurityLevels = IHISI_AUTH_NONE               | IHISI_AUTH_POST_ONLY | IHISI_AUTH_DXE_ONLY |
                              IHISI_AUTH_SECURE_ENVIRONMENT | IHISI_AUTH_PASSWORD  | IHISI_AUTH_SECURE_BOOT_DISABLED;
  } else {
    SupportedSecurityLevels = 0;
    SupportedSecurityLevels |= FeaturePcdGet (PcdH2OIhisiAuthNoneSupported) ? IHISI_AUTH_NONE : 0;
    if (FeaturePcdGet (PcdH2OIhisiAuthPostSupported) && !mSmmReadyToBoot) {
      SupportedSecurityLevels |= IHISI_AUTH_POST_ONLY;
    }
    if (FeaturePcdGet (PcdH2OIhisiAuthDxeSupported) && !mSmmEndOfDxe) {
      SupportedSecurityLevels |= IHISI_AUTH_DXE_ONLY;
    }
    if (FeaturePcdGet (PcdH2OIhisiAuthSecureEnvSupported) && IsSecureEnvironment ()) {
      SupportedSecurityLevels |= IHISI_AUTH_SECURE_ENVIRONMENT;
    }
    if (FeaturePcdGet (PcdH2OIhisiAuthPasswordSupported) && IsPasswordEnrolled ()) {
      SupportedSecurityLevels |= IHISI_AUTH_PASSWORD;
    }
    if (FeaturePcdGet (PcdH2OIhisiAuthSecureBootDisabledSupported) && !IsSecureBootEnabled ()) {
      SupportedSecurityLevels |= IHISI_AUTH_SECURE_BOOT_DISABLED;
    }

    if (FeaturePcdGet (PcdH2OSmmCpIhisiAuthStatusSupported)) {
      H2O_SMM_CP_IHISI_AUTH_STATUS_DATA        IhisiAuthStatusData;

      IhisiAuthStatusData.Size                    = sizeof (H2O_SMM_CP_IHISI_AUTH_STATUS_DATA);
      IhisiAuthStatusData.Status                  = H2O_CP_TASK_NORMAL;
      IhisiAuthStatusData.SupportedSecurityLevels  = SupportedSecurityLevels;
      DEBUG ((DEBUG_INFO, "Checkpoint Trigger: %g\n", &gH2OSmmCpIhisiAuthStatusGuid));
      H2OCpTrigger (&gH2OSmmCpIhisiAuthStatusGuid, &IhisiAuthStatusData);
      DEBUG ((DEBUG_INFO, "Checkpoint Result: %x\n", IhisiAuthStatusData.Status));
      if (IhisiAuthStatusData.Status == H2O_CP_TASK_UPDATE) {
        SupportedSecurityLevels = IhisiAuthStatusData.SupportedSecurityLevels;
      }
    }
  }
  return SupportedSecurityLevels;
}

/**
  Helper function to check if the input security level is whether valid.

  @param[in] SecurityLevel   Bitmask that indicates which security level

  @retval  TRUE      Input security level is valid.
  @retval  FALSE     Input security level is invalid.
**/
BOOLEAN
IsValidSecurityLevel (
  IN  UINT32         SecurityLevel
  )
{
  if (SecurityLevel != IHISI_AUTH_NONE     && SecurityLevel != IHISI_AUTH_POST_ONLY &&
      SecurityLevel != IHISI_AUTH_DXE_ONLY && SecurityLevel != IHISI_AUTH_SECURE_ENVIRONMENT &&
      SecurityLevel != IHISI_AUTH_PASSWORD && SecurityLevel != IHISI_AUTH_SECURE_BOOT_DISABLED) {
    return FALSE;
  }

  if ((SecurityLevel & GetSupportedIhisiSecurityLevels ()) !=  SecurityLevel) {
    return FALSE;
  }
  return TRUE;
}

/**
  Set curreny security level

  @param[in] SecurityLevel        Bitmask that indicates which security level

  @retval  EFI_SUCCESS            Set current security successfully.
  @retval  EFI_INVALID_PARAMETER  Input security level is invalid or unsupported.
**/
STATIC
EFI_STATUS
SetCurrentSecurityLevel (
  IN  UINT32         SecurityLevel
  )
{
  if (SecurityLevel != 0 && !IsValidSecurityLevel (SecurityLevel)) {
    return EFI_INVALID_PARAMETER;
  }
  mSecurityLevel = SecurityLevel;
  return EFI_SUCCESS;
}

/**
  AH=80h, Get IHISI current authentication  status

  @retval EFI_SUCCESS        Command successful returned.
**/
EFI_STATUS
EFIAPI
KernelIhisiAuthStatus (
  VOID
  )
{
  UINT32                 Ebx;

  if (!mIhisiLock) {
    IhisiProtWriteCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RBX, IHISI_EBX_SIGNATURE);
  } else {
    if (!GetRandomNumber32 (&Ebx)) {
      IhisiProtWriteCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RBX, IHISI_EBX_SIGNATURE);
      return IHISI_INVALID_PARAMETER;
    }
    IhisiProtWriteCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RBX, Ebx);
    mIhisiSessionToken = Ebx;
  }
  IhisiProtWriteCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RCX, GetSupportedIhisiSecurityLevels ());
  IhisiProtWriteCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RSI, GetCurrentIhisiSecurityLevel ());
  return IHISI_SUCCESS;
}

/**
  AH=81h, Change IHISI authentication  status to locked.

  @retval EFI_SUCCESS        Command successful returned.
  @retval Other              IHISI authentication  status is already locked or unable to get session token.
**/
EFI_STATUS
EFIAPI
KernelIhisiAuthLock (
  VOID
  )
{
  UINT32                 Ebx;

  if (mIhisiLock) {
    IhisiProtWriteCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RBX, IHISI_EBX_SIGNATURE);
    return IHISI_INVALID_PARAMETER;
  }
  Ebx = 0;
  if (!GetRandomNumber32 (&Ebx)) {
    IhisiProtWriteCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RBX, IHISI_EBX_SIGNATURE);
    return IHISI_INVALID_PARAMETER;
  }
  IhisiProtWriteCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RBX, Ebx);
  mIhisiLock = TRUE;
  mIhisiSessionToken = Ebx;
  if (FeaturePcdGet (PcdH2OIhisiAuthNoneSupported)) {
    SetCurrentSecurityLevel (IHISI_AUTH_NONE);
  } else {
    SetCurrentSecurityLevel (0);
  }
  return IHISI_SUCCESS;
}

/**
  AH=82h, Change IHISI authentication  status to locked.

  @retval EFI_SUCCESS        Command successful returned.
  @retval Other              IHISI authentication  status is already unlocked or session token number is incorrect.
**/
EFI_STATUS
EFIAPI
KernelIhisiAuthUnlock (
  VOID
  )
{
  IhisiProtWriteCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RBX, IHISI_EBX_SIGNATURE);
  mIhisiLock = FALSE;
  SetCurrentSecurityLevel (0);
  return IHISI_SUCCESS;
}


/**
  AH=83h, Get IHISI command buffer.

  @retval IHISI_SUCCESS                     Command successful returned.
  @retval IHISI_UNSUPPORTED_FUNCTION        Not support this command buffer.
**/
EFI_STATUS
EFIAPI
KernelIhisiGetCmdBuffer (
  VOID
  )
{
  UINT32                            Ecx;
  UINT32                            Edx;

  if (!FeaturePcdGet(PcdH2OIhisiCmdBufferSupported) || !FeaturePcdGet (PcdH2OIhisiCmdBufferBelow4Gb)) {
    return IHISI_UNSUPPORTED_FUNCTION;
  }

  Ecx = (UINT32) mIhisiCmdBuffer;
  Edx = (UINT32) mIhisiCmdBufferSize;
  IhisiProtWriteCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RCX, Ecx);
  IhisiProtWriteCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RDX, Edx);
  return IHISI_SUCCESS;
}


/**
  AH=83h, Get IHISI image buffer.

  @retval EFI_SUCCESS        Command successful returned.
**/
EFI_STATUS
EFIAPI
KernelGetImageBuffer (
  VOID
  )
{
  UINT32                            Esi;
  UINT32                            Edi;

  //
  // Not all of system must support image buffer so return IHISI_SUCCESS directly if not support.
  //
  if (!FeaturePcdGet(PcdH2OIhisiImageBufferSupported)) {
    return IHISI_SUCCESS;
  }
  //
  // Still need return IHISI_UNSUPPORTED_FUNCTION if support image buffer but the the buffer isn't
  // below 4G
  //
  if  (!FeaturePcdGet (PcdH2OIhisiImageBufferBelow4Gb)) {
    return IHISI_UNSUPPORTED_FUNCTION;
  }

  Esi = (UINT32) mIhisiImageBuffer;
  Edi = (UINT32) mIhisiImageBufferSize;
  IhisiProtWriteCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RSI, Esi);
  IhisiProtWriteCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RDI, Edi);
  return IHISI_SUCCESS;
}


/**
  AH=84h, locks the IHISI interface and returns the first session token.

  @retval EFI_SUCCESS        Command successful returned.
**/
EFI_STATUS
EFIAPI
KernelIhisiAuth (
  VOID
  )
{
  IHISI_AUTH_LOCK                   *CredentialData ;
  UINT32                            RequestedSecurityLevel;
  BOOLEAN                           Authorized;

  CredentialData = (IHISI_AUTH_LOCK *)(UINTN)IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RSI);

  RequestedSecurityLevel = IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RCX);
  if (!IsValidSecurityLevel (RequestedSecurityLevel)) {
    KernelIhisiAuthUnlock ();
    return IHISI_INVALID_PARAMETER;
  }

  if (RequestedSecurityLevel == IHISI_AUTH_POST_ONLY)  {
    Authorized = !mSmmReadyToBoot ? TRUE : FALSE;
  } else if (RequestedSecurityLevel == IHISI_AUTH_DXE_ONLY)  {
    Authorized = !mSmmEndOfDxe ? TRUE : FALSE;
  } else if (RequestedSecurityLevel == IHISI_AUTH_SECURE_ENVIRONMENT)  {
    Authorized = IsSecureEnvironment () ? TRUE : FALSE;
  } else if (RequestedSecurityLevel == IHISI_AUTH_PASSWORD )  {
    Authorized = IsValidPassword (CredentialData) ? TRUE : FALSE;
  } else if (RequestedSecurityLevel == IHISI_AUTH_SECURE_BOOT_DISABLED) {
    Authorized = !IsSecureBootEnabled () ? TRUE : FALSE;
  } else {
    Authorized = TRUE;
  }

  if (FeaturePcdGet (PcdH2OSmmCpIhisiAuthCheckSupported)) {
    H2O_SMM_CP_IHISI_AUTH_CHECK_DATA        IhisiAuthCheckData;

    IhisiAuthCheckData.Size           = sizeof (H2O_SMM_CP_IHISI_AUTH_CHECK_DATA);
    IhisiAuthCheckData.Status         = H2O_CP_TASK_NORMAL;
    IhisiAuthCheckData.SecurityLevel  = RequestedSecurityLevel;
    IhisiAuthCheckData.AuthData       = CredentialData == NULL ? NULL : (VOID *)(CredentialData + 1);
    IhisiAuthCheckData.AuthDataSize   = CredentialData == NULL ? 0 : CredentialData->Size;
    IhisiAuthCheckData.Authorized     = Authorized;
    DEBUG ((DEBUG_INFO, "Checkpoint Trigger: %g\n", &gH2OSmmCpIhisiAuthCheckGuid));
    H2OCpTrigger (&gH2OSmmCpIhisiAuthCheckGuid, &IhisiAuthCheckData);
    DEBUG ((DEBUG_INFO, "Checkpoint Result: %x\n", IhisiAuthCheckData.Status));
    if (IhisiAuthCheckData.Status == H2O_CP_TASK_UPDATE) {
      Authorized = IhisiAuthCheckData.Authorized;
    }
  }

  if (!Authorized) {
    KernelIhisiAuthUnlock ();
    return RequestedSecurityLevel == IHISI_AUTH_PASSWORD ? IHISI_PASSWORD_INVALID : IHISI_INVALID_PARAMETER;
  }
  SetCurrentSecurityLevel (RequestedSecurityLevel);
  return IHISI_SUCCESS;
}


EFI_STATUS
InstallIhisiCommonServices (
  VOID
  )
{
  EFI_STATUS              Status;
  IHISI_REGISTER_TABLE   *SubFuncTable;
  UINT16                  TableCount;


  SubFuncTable = COMMON_REGISTER_TABLE;
  TableCount = sizeof(COMMON_REGISTER_TABLE) / sizeof(COMMON_REGISTER_TABLE[0]);
  Status = RegisterIhisiSubFunction (SubFuncTable, TableCount);
  if (EFI_ERROR(Status)) {
    ASSERT_EFI_ERROR (Status);
  }

  return EFI_SUCCESS;
}



/**
  Notification function of gEfiSmmEndOfDxeProtocolGuid protocol.

  @param[in] Protocol   Points to the protocol's unique identifier
  @param[in] Interface  Points to the interface instance
  @param[in] Handle     The handle on which the interface was installed

  @retval EFI_SUCCESS   OnSmmEndOfDxe runs successfully
**/

STATIC
EFI_STATUS
EFIAPI
OnSmmEndOfDxe (
  IN CONST EFI_GUID                       *Protocol,
  IN VOID                                 *Interface,
  IN EFI_HANDLE                           Handle
  )
{
  mSmmEndOfDxe = TRUE;
  return EFI_SUCCESS;
}

/**
  Notification function of gEdkiiSmmReadyToBootProtocolGuid protocol.

  @param[in] Protocol   Points to the protocol's unique identifier
  @param[in] Interface  Points to the interface instance
  @param[in] Handle     The handle on which the interface was installed

  @retval EFI_SUCCESS   OnSmmReadyToBoot runs successfully
**/

STATIC
EFI_STATUS
EFIAPI
OnSmmReadyToBoot (
  IN CONST EFI_GUID                       *Protocol,
  IN VOID                                 *Interface,
  IN EFI_HANDLE                           Handle
  )
{
  mSmmReadyToBoot = TRUE;
  return EFI_SUCCESS;
}


/**
  Initial IHISI security status.
**/
VOID
InitIhisiSecurityStatus (
  VOID
  )
{
  VOID                *Registration;

  gSmst->SmmRegisterProtocolNotify (
           &gEfiSmmEndOfDxeProtocolGuid,
           OnSmmEndOfDxe,
           &Registration
           );
  gSmst->SmmRegisterProtocolNotify (
           &gEdkiiSmmReadyToBootProtocolGuid,
           OnSmmReadyToBoot,
           &Registration
           );
}


