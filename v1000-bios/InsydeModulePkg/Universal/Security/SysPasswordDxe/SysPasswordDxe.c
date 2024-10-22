/** @file
  EFI SysPassword Protocol

;******************************************************************************
;* Copyright (c) 2012 - 2022, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "SysPasswordDxe.h"

VOID                                  *mSecureFirmwarePasswordHob;
VOID                                  *mSupervisorPwHob;
VOID                                  *mUserPwHob;

extern UINT8                            SysPasswordStrings[];

EFI_GUID mSysPasswordStringPackGuid = { 0x5e6571b9, 0xe8d3, 0x41aa, { 0x9a, 0x0, 0xf6, 0x9c, 0x7b, 0x3e, 0xee, 0xd6 } };

EFI_HII_HANDLE                           mStringPackHandle;
/**
  change string type from Unicode to ASCII

  @param  This                 The EFI_SYS_PASSWORD_SERVICE_PROTOCOL instance.
  @param  Password             Unicode password string pointer.
  @param  PasswordLength       Password string length.

  @retval                      ASCII password string pointer

**/
UINT8 *
Unicode2Ascii (
  IN  EFI_SYS_PASSWORD_SERVICE_PROTOCOL *This,
  IN  VOID                              *Password,
  IN  UINTN                             PasswordLength
  )
{
  UINT8                                 CheckSum;
  UINT8                                 *EncodeCode;
  UINT16                                *W16PasswordPtr;
  UINTN                                 W16PaswdLength;
  UINTN                                 Index;
  SYS_PASSWORD_PRIVATE                  *SysPasswordPrivate;

  SysPasswordPrivate = GET_PRIVATE_FROM_SYS_PASSWORD (This);

  CheckSum = 0;
  EncodeCode = NULL;
  W16PasswordPtr = Password;
  //
  //Calculate real Password length
  //
  for (Index = 0; Index < SysPasswordPrivate->SysPasswordSetupInfoData->MaxSysPasswordLength; Index++) {
    if (W16PasswordPtr[Index] == 0) {
      break;
    }
  }
  W16PaswdLength = Index;
  EncodeCode = AllocateZeroPool (W16PaswdLength + 2);
  if (EncodeCode == NULL) {
    return NULL;
  }
  //
  //Put Password length at the first byte
  //
  EncodeCode[0] = (UINT8) W16PaswdLength;
  CheckSum = EncodeCode[0];
  for (Index = 1; Index < W16PaswdLength + 1; Index++) {
    EncodeCode[Index] = (UINT8)(W16PasswordPtr[Index - 1]);
    CheckSum = CheckSum + EncodeCode[Index];
  }
  CheckSum = ~CheckSum + 0x01;
  //
  //put Checksum at the end of password
  //
  EncodeCode[Index] = CheckSum;

  return EncodeCode;
}

/**
  If you want to Encode password string just edit at here.

  @param  PasswordPtr          Password string address.
  @param  PasswordLength       Password string length.

  @retval

**/
UINT8 *
Encode (
  IN  VOID    *PasswordPtr,
  IN  UINTN   PasswordLength
  )
{
  return PasswordPtr;
}

/**
  compare two password

  @param  s1                   password 1
  @param  s2                   password 2

  @retval TRUE                 they are the same
  @retval FALSE                they are different

**/
BOOLEAN
PasswordCmp (
  UINT8       *s1,
  UINT8       *s2
  )
{
  UINT8       Index;

  if (s1[0] != s2[0]) {
    return FALSE;
  }

  for (Index = 1; Index < s1[0] + 2; Index++) {
    if (s1[Index] != s2[Index]) {
      return FALSE;
    }
  }

  return TRUE;
}

/**
  Internal function to check secure password is whether valid.

  @param[in] This            Pointer to EFI_SIMPLE_PASSWORD_PROTOCOL instance.
  @param[in] PasswordPtr     Password address.
  @param[in] PasswordLength  Password length.
  @param[in] PasswordHash    Pointer to password hash value.
  @param[in] HahsLength      Password length in bytes.

  @retval TRUE               The input password is valid.
  @retval FALSE              The input password in invalid.
**/
STATIC
BOOLEAN
IsSecurePasswordValid (
  IN  EFI_SYS_PASSWORD_SERVICE_PROTOCOL    *This,
  IN  VOID                                 *PasswordPtr,
  IN  UINTN                                PasswordLength,
  IN  VOID                                 *PasswordHash,
  IN  UINTN                                HahsLength
  )
{
  CHAR8                     CombinedData[CERT_DATA_SIZE];
  EFI_STATUS                Status;
  EFI_HASH_PROTOCOL         *Hash;
  UINT8                     *AsciiPassword;
  UINTN                     AsciiPasswordLength;
  EFI_HASH_OUTPUT           Digest;

  if (PasswordPtr == NULL || PasswordLength == 0 || PasswordHash == NULL || HahsLength != 32) {
    return FALSE;
  }

  if (!InSmm ()) {
    Status = gBS->LocateProtocol (
                    &gEfiHashProtocolGuid,
                    NULL,
                    (VOID**) &Hash
                    );
  } else {
    Status = gSmst->SmmLocateProtocol (
                      &gEfiHashProtocolGuid,
                      NULL,
                      (VOID**) &Hash
                      );
  }
  if (EFI_ERROR (Status)) {
    return FALSE;
  }
  AsciiPassword = Unicode2Ascii (This, PasswordPtr, PasswordLength);
  if (AsciiPassword == NULL) {
    return FALSE;
  }
  AsciiPasswordLength = AsciiPassword[0];
  Digest.Sha256Hash = AllocateZeroPool (32);
  GetCombinedData (AsciiPassword, AsciiPasswordLength + 1, CombinedData);
  if (AsciiPasswordLength + 1 > CERT_DATA_SIZE) {
    Status = Hash->Hash (
                     Hash,
                     &gEfiHashAlgorithmSha256Guid,
                     FALSE,
                     (UINT8 *) CombinedData,
                     CERT_DATA_SIZE,
                     (EFI_HASH_OUTPUT *) &Digest
                     );
    Status = Hash->Hash (
                     Hash,
                     &gEfiHashAlgorithmSha256Guid,
                     TRUE,
                     &AsciiPassword[CERT_DATA_SIZE],
                     PasswordLength + 1 - CERT_DATA_SIZE,
                     (EFI_HASH_OUTPUT *) &Digest
                     );
  } else {
    Status = Hash->Hash (
                     Hash,
                     &gEfiHashAlgorithmSha256Guid,
                     FALSE,
                     (UINT8 *) CombinedData,
                     CERT_DATA_SIZE,
                     (EFI_HASH_OUTPUT *) &Digest
                     );
  }
  FreePool (AsciiPassword);

  if ((EFI_ERROR(Status)) || (Digest.Sha256Hash == NULL) || (CompareMem (Digest.Sha256Hash, PasswordHash, HahsLength) != 0)) {
    return FALSE;
  }

  return TRUE;
}


/**
  Change password state from unlocked state to locked state.

  @param[in]  This             Pointer to EFI_SYS_PASSWORD_SERVICE_PROTOCOL instance.

  @retval EFI_SUCCESS          Change password state to locked state.
  @retval EFI_UNSUPPORTED      The password doesn't saved in variable storage.
  @retval EFI_ALREADY_STARTED  The password state is already in locked state.
**/
EFI_STATUS
EFIAPI
LockPassword (
  IN  EFI_SYS_PASSWORD_SERVICE_PROTOCOL    *This
  )
{
  UINTN                              VariableSize;
  UINT8                              *VariableBuffer;
  EFI_VARIABLE_AUTHENTICATION        *CertData;
  UINT32                             Attributes;
  EFI_STATUS                         Status;

  if (PcdGetBool (PcdSysPasswordInCmos) || !PcdGetBool (PcdSecureSysPasswordSupported)) {
    return EFI_UNSUPPORTED;
  }

  VariableBuffer = NULL;
  VariableSize = LOCK_PASSWORD_VARIABLE_SIZE;
  VariableBuffer = AllocateZeroPool (VariableSize);
  if (VariableBuffer == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  CertData = (EFI_VARIABLE_AUTHENTICATION *) VariableBuffer;
  InitSecureVariableHeader (VariableSize, CertData);
  CertData->AuthInfo.CertData[CERT_DATA_SIZE] = 0;
  Attributes = EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS |
                 EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS;
  Status = CommonSetVariable (
                  EFI_CHECK_ADMIN_PASSWORD_NAME,
                  &gInsydeSecureFirmwarePasswordGuid,
                  Attributes,
                  VariableSize,
                  VariableBuffer
                  );
  FreePool (VariableBuffer);
  return Status;
}

/**
  Change the Insyde password state to unlocked state.

  @param[in]  This             Pointer to EFI_SYS_PASSWORD_SERVICE_PROTOCOL instance.
  @param[in]  PasswordPtr      The null-terminated string that is the input password.
  @param[in]  PasswordLength   The password length in bytes of PasswordPtr.

  @retval EFI_SUCCESS          Change password state to locked state.
  @retval EFI_UNSUPPORTED      The password doesn't saved in variable storage or password length is too long.
  @retval EFI_ALREADY_STARTED  The password state is already in locked state.
**/
EFI_STATUS
EFIAPI
UnlockPassword (
  IN  EFI_SYS_PASSWORD_SERVICE_PROTOCOL    *This,
  IN  VOID                                 *PasswordPtr,
  IN  UINTN                                PasswordLength
  )
{
  UINTN                              VariableSize;
  UINT8                              *VariableBuffer;
  EFI_VARIABLE_AUTHENTICATION        *CertData;
  UINT32                             Attributes;
  EFI_STATUS                         Status;
  UINT8                              *AsciiPassword;
  UINTN                              AsciiPasswordLength;
  SYS_PASSWORD_PRIVATE               *SysPasswordPrivate;

  SysPasswordPrivate = GET_PRIVATE_FROM_SYS_PASSWORD (This);

  if (PcdGetBool (PcdSysPasswordInCmos) || !PcdGetBool (PcdSecureSysPasswordSupported)) {
    return EFI_UNSUPPORTED;
  }

  AsciiPasswordLength = 0;
  AsciiPassword = NULL;
  Status = This->GetStatus (This, 0);

  if (Status == EFI_SUCCESS) {
    if (PasswordPtr == NULL || *((CHAR16 *) PasswordPtr) == 0) {
      return EFI_INVALID_PARAMETER;
    }
    AsciiPassword = Unicode2Ascii (This, PasswordPtr, PasswordLength);
    if (AsciiPassword == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
    AsciiPasswordLength = AsciiPassword[0];
  }
  if (AsciiPasswordLength > SysPasswordPrivate->SysPasswordSetupInfoData->MaxSysPasswordLength) {
    return EFI_UNSUPPORTED;
  }

  VariableSize = INSYDE_VARIABLE_PASSWORD_HEADER_SIZE + sizeof (UINT8) + AsciiPasswordLength + sizeof (UINT8);
  VariableBuffer = NULL;
  VariableBuffer = AllocateZeroPool (VariableSize);
  if (VariableBuffer == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  CertData = (EFI_VARIABLE_AUTHENTICATION *) VariableBuffer;
  InitSecureVariableHeader (VariableSize, CertData);
  if (AsciiPasswordLength != 0) {
    CopyMem (CertData->AuthInfo.CertData, AsciiPassword , AsciiPasswordLength + sizeof (UINT8));
    FreePool (AsciiPassword);
  }

  CertData->AuthInfo.CertData[AsciiPasswordLength + sizeof (UINT8)] = 1;
  Attributes = EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS |
                 EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS;
  Status = CommonSetVariable (
                  EFI_CHECK_ADMIN_PASSWORD_NAME,
                  &gInsydeSecureFirmwarePasswordGuid,
                  Attributes,
                  VariableSize,
                  VariableBuffer
                  );
  FreePool (VariableBuffer);
  return Status;
}


/**
  According password type to get CMOS offset

  @param  IsUserPassword        Determine to disable supervisor or user password
  @param  Address                Get the address of password storing in CMOS

  @retval EFI_SUCCESS            Get CMOS offset successful
  @retval EFI_INVALID_PARAMETER  Address is NULL.
**/
EFI_STATUS
GetCmosAddressByPasswordKind (
  IN  BOOLEAN                           IsUserPassword,
  OUT UINT8                             *Address
  )
{
  if (Address == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  *Address = IsUserPassword ? UserPassword : SupervisorPassword;
  return EFI_SUCCESS;
}

/**
  According password kind to get password information. These information include
  Password kind and password GUID.

  @param  IsUserPassword         Determine to disable supervisor or user password
  @param  PasswordKind           get the string of password kind
  @param  PwKindLen              string length
  @param  SysPasswordGuid        get the GUID of password kind

  @retval EFI_SUCCESS            Get password type and password GUID successful.
  @retval EFI_INVALID_PARAMETER  PasswordKind or SysPasswordGuid is NULL.

**/
EFI_STATUS
GetPasswordInfoByPasswordKind (
  IN  BOOLEAN                           IsUserPassword,
  OUT CHAR16                            **PasswordKind,
  IN  UINTN                             PwKindStrLen,
  OUT EFI_GUID                          *SysPasswordGuid
  )
{

  if (PasswordKind == NULL || SysPasswordGuid == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (IsUserPassword) {
    StrCpyS ((CHAR16 *)PasswordKind, PwKindStrLen,L"SystemUserPw");
    *SysPasswordGuid = gEfiUserPwGuid;
  } else if (!PcdGetBool (PcdSecureSysPasswordSupported)) {
    StrCpyS ((CHAR16 *)PasswordKind, PwKindStrLen,L"SystemSupervisorPw");
    *SysPasswordGuid = gEfiSupervisorPwGuid;
  } else {
    StrCpyS ((CHAR16 *)PasswordKind, PwKindStrLen,EFI_ADMIN_PASSWORD_NAME);
    *SysPasswordGuid = gInsydeSecureFirmwarePasswordGuid;
  }
  return EFI_SUCCESS;
}

/**
  set supervisor or use password to CMOS

  @param  This                 The EFI_SYS_PASSWORD_SERVICE_PROTOCOL instance.
  @param  UserPassword         Determine to disable supervisor or user password
  @param  Password             buffer pointer to store password

  @retval

**/
EFI_STATUS
WritePasswordInCmos (
  IN  EFI_SYS_PASSWORD_SERVICE_PROTOCOL *This,
  IN  BOOLEAN                           IsUserPassword,
  IN  UINT8                             *Password
  )
{
  EFI_STATUS                Status;
  UINT8                     Address;
  UINT8                     Index;
  UINT8                     PasswordLen;
  SYS_PASSWORD_PRIVATE      *SysPasswordPrivate;

  SysPasswordPrivate = GET_PRIVATE_FROM_SYS_PASSWORD (This);

  if (!PcdGetBool (PcdSysPasswordSupportUserPswd) && IsUserPassword) {
    return EFI_UNSUPPORTED;
  }

  Status = GetCmosAddressByPasswordKind (IsUserPassword, &Address);
  if (Status != EFI_SUCCESS) {
    return Status;
  }

  if (Password == NULL) {
    //
    //  Clean password in CMOS
    //
    for (Index = 0; Index < SysPasswordPrivate->SysPasswordSetupInfoData->MaxSysPasswordLength + 2; Index ++) {
      WriteCmos8 (Address + Index, 0x00);
    }
    return EFI_SUCCESS;
  }

  PasswordLen = Password[0];

  if (PasswordLen == 0) {
    //
    //  Clean password in CMOS
    //
    for (Index = 0; Index < SysPasswordPrivate->SysPasswordSetupInfoData->MaxSysPasswordLength + 2; Index ++) {
      WriteCmos8 (Address + Index, 0x00);
    }
  } else {
    if (PasswordLen > SysPasswordPrivate->SysPasswordSetupInfoData->MaxSysPasswordLength) {
      return EFI_INVALID_PARAMETER;
    }
    for (Index = 0; Index < PasswordLen + 2; Index++) {
      WriteCmos8 (Address, Password[Index]);
      Address++;
    }
  }

  return EFI_SUCCESS;
}

/**
  set supervisor or use password to variable

  @param  This                 The EFI_SYS_PASSWORD_SERVICE_PROTOCOL instance.
  @param  IsUserPassword       Determine to disable supervisor or user password
  @param  Password             buffer pointer to store password

  @retval

**/
EFI_STATUS
WritePasswordToVariable (
  IN  EFI_SYS_PASSWORD_SERVICE_PROTOCOL *This,
  IN  BOOLEAN                           IsUserPassword,
  IN  UINT8                             *Password
  )
{
  UINT8                                 PasswordLen;
  CHAR16                                PasswordKind[PASSWORD_KIND_MAX_LENGTH];
  EFI_GUID                              SysPasswordGuid;

  UINTN                                 VariableSize;
  UINT8                                 *VariableBuffer;
  EFI_VARIABLE_AUTHENTICATION           *CertData;
  UINT32                                Attributes;
  EFI_STATUS                            Status;
  SYS_PASSWORD_PRIVATE                  *SysPasswordPrivate;

  SysPasswordPrivate = GET_PRIVATE_FROM_SYS_PASSWORD (This);

  if (!PcdGetBool (PcdSysPasswordSupportUserPswd) && IsUserPassword) {
    return EFI_UNSUPPORTED;
  }

  PasswordLen    = Password == NULL ? 0 : Password[0];
  if (PasswordLen > SysPasswordPrivate->SysPasswordSetupInfoData->MaxSysPasswordLength) {
    return EFI_INVALID_PARAMETER;
  }
  Attributes     = 0;
  VariableSize   = 0;
  VariableBuffer = NULL;

  if (IsUserPassword) {
    StrCpyS (PasswordKind, sizeof(PasswordKind) / sizeof(CHAR16), L"SystemUserPw");
    SysPasswordGuid = gEfiUserPwGuid;
    VariableSize = PasswordLen == 0 ? 0 : PasswordLen + 2;
    VariableBuffer = AllocateZeroPool (VariableSize);
    if (VariableBuffer == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
    CopyMem (VariableBuffer, Password, VariableSize);
    Attributes = EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE;
  } else if (!PcdGetBool (PcdSecureSysPasswordSupported)) {
    StrCpyS (PasswordKind, sizeof(PasswordKind) / sizeof(CHAR16), L"SystemSupervisorPw");
    SysPasswordGuid = gEfiSupervisorPwGuid;
    VariableSize = PasswordLen == 0 ? 0 : PasswordLen + 2;
    VariableBuffer = NULL;
    VariableBuffer = AllocateZeroPool (VariableSize);
    if (VariableBuffer == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
    CopyMem (VariableBuffer, Password, VariableSize);
    Attributes = EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE;
  } else  {
    StrCpyS (PasswordKind, sizeof(PasswordKind) / sizeof(CHAR16), EFI_ADMIN_PASSWORD_NAME);
    SysPasswordGuid = gInsydeSecureFirmwarePasswordGuid;
    VariableSize = PasswordLen == 0 ?  DELETE_PASSWORD_VARIABLE_SIZE : INSYDE_VARIABLE_PASSWORD_HEADER_SIZE + sizeof (UINT8) + PasswordLen + sizeof (UINT8);
    VariableBuffer = NULL;
    VariableBuffer = AllocateZeroPool (VariableSize);
    if (VariableBuffer == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
    CertData = (EFI_VARIABLE_AUTHENTICATION *) VariableBuffer;
    InitSecureVariableHeader (VariableSize, CertData);
    if (PasswordLen != 0) {
      CopyMem (CertData->AuthInfo.CertData, Password, PasswordLen + sizeof (UINT8));
      CertData->AuthInfo.CertData[PasswordLen + sizeof (UINT8)] = 1;
    }
    Attributes = EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS |
                 EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS;

  }

  if (IsUserPassword || !PcdGetBool (PcdSecureSysPasswordSupported)) {
    Status = SetVariableToSensitiveVariable (
               PasswordKind,
               &SysPasswordGuid,
               Attributes,
               VariableSize,
               VariableBuffer
               );
  } else {
    Status = CommonSetVariable (
               PasswordKind,
               &SysPasswordGuid,
               Attributes,
               VariableSize,
               VariableBuffer
               );
  }
  if (VariableBuffer != NULL) {
    FreePool (VariableBuffer);
  }
  return Status;



}

/**
  set supervisor or use password

  @param  This                 The EFI_SYS_PASSWORD_SERVICE_PROTOCOL instance.
  @param  UserPassword         Determine to disable supervisor or user password
  @param  Password             buffer pointer to store password

  @retval

**/
EFI_STATUS
WritePassword (
  IN  EFI_SYS_PASSWORD_SERVICE_PROTOCOL *This,
  IN  BOOLEAN                           IsUserPassword,
  IN  UINT8                             *Password
  )
{
  EFI_STATUS        Status;

  if (PcdGetBool (PcdSysPasswordInCmos)) {
    Status = WritePasswordInCmos (
               This,
               IsUserPassword,
               Password
               );
  } else {
    Status = WritePasswordToVariable (
               This,
               IsUserPassword,
               Password
               );
  }

  return Status;
}

/**
  get supervisor or use password from CMOS

  @param  This                 The EFI_SYS_PASSWORD_SERVICE_PROTOCOL instance.
  @param  IsUserPassword       Determine to disable supervisor or user password
  @param  Password             buffer pointer to store password

  @retval

**/
EFI_STATUS
ReadPasswordInCmos (
  IN  EFI_SYS_PASSWORD_SERVICE_PROTOCOL *This,
  IN  BOOLEAN                           IsUserPassword,
  OUT UINT8                             **Password
  )
{
  UINT8                                 PasswordLen;
  EFI_STATUS                            Status;
  UINT8                                 CalCheckSum;
  UINT8                                 Address;
  UINT8                                 SaveAddress;
  UINT8                                 Index;
  SYS_PASSWORD_PRIVATE                  *SysPasswordPrivate;

  SysPasswordPrivate = GET_PRIVATE_FROM_SYS_PASSWORD (This);

  if (!PcdGetBool (PcdSysPasswordSupportUserPswd) && IsUserPassword) {
    return EFI_UNSUPPORTED;
  }

  Status =  GetCmosAddressByPasswordKind (IsUserPassword, &Address);
  if (Status != EFI_SUCCESS) {
    return Status;
  }

  PasswordLen = ReadCmos8 (Address);
  if (PasswordLen == 0 || PasswordLen > SysPasswordPrivate->SysPasswordSetupInfoData->MaxSysPasswordLength) {
    for (Index = 0; Index < (SysPasswordPrivate->SysPasswordSetupInfoData->MaxSysPasswordLength + 2); Index ++) {
      WriteCmos8 (Address + Index, 0x00);
    }
    return EFI_NOT_FOUND;
  }
  SaveAddress = Address;
  *Password = NULL;
  *Password = AllocateZeroPool (PasswordLen + 2);
  if (*Password == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  CalCheckSum = 0;
  for (Index = 0; Index < PasswordLen + 2; Index++) {
    (*Password)[Index] = ReadCmos8 (Address);
    CalCheckSum = CalCheckSum + (*Password)[Index];
    Address++;
  }

  if (CalCheckSum != 0) {
    for (Index = 0; Index < (SysPasswordPrivate->SysPasswordSetupInfoData->MaxSysPasswordLength + 2); Index ++) {
      WriteCmos8 (SaveAddress + Index, 0x00);
    }
    FreePool (*Password);
    return EFI_NOT_FOUND;
  }

  return EFI_SUCCESS;
}

/**
  get supervisor or use password from variable

  @param  This                 The EFI_SYS_PASSWORD_SERVICE_PROTOCOL instance.
  @param  IsUserPassword       Determine to disable supervisor or user password
  @param  Password             buffer pointer to store password

  @retval

**/
EFI_STATUS
ReadPasswordFromVariable (
  IN  EFI_SYS_PASSWORD_SERVICE_PROTOCOL *This,
  IN  BOOLEAN                           IsUserPassword,
  OUT UINT8                             **Password
  )
{
  UINT8                                 PasswordLen;
  EFI_STATUS                            Status;
  UINT8                                 CalCheckSum;
  UINT8                                 Index;
  UINTN                                 VariableSize;
  UINT8                                 *PasswordBuf;
  //
  //  The length of PasswordKind depends on L"SystemSupervisorPw" and L"SystemUserPw".
  //
  CHAR16                                PasswordKind[PASSWORD_KIND_MAX_LENGTH];
  EFI_GUID                              SysPasswordGuid;
  VOID                                  *Table;
  UINTN                                 Size;
  SYS_PASSWORD_PRIVATE                  *SysPasswordPrivate;
  BOOLEAN                               SecureVariableInHob;

  SysPasswordPrivate = GET_PRIVATE_FROM_SYS_PASSWORD (This);

  if (!PcdGetBool (PcdSysPasswordSupportUserPswd) && IsUserPassword) {
    return EFI_UNSUPPORTED;
  }

  Table = NULL;
  Size  = 0;;
  VariableSize = (SysPasswordPrivate->SysPasswordSetupInfoData->MaxSysPasswordLength + 2);
  PasswordBuf = NULL;
  PasswordBuf = AllocateZeroPool (VariableSize);
  if (PasswordBuf == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  Status = GetPasswordInfoByPasswordKind (IsUserPassword, (CHAR16 **) &PasswordKind, PASSWORD_KIND_MAX_LENGTH, &SysPasswordGuid);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  SecureVariableInHob = FALSE;
  Status = CommonGetVariable (
             PasswordKind,
             &SysPasswordGuid,
             &VariableSize,
             PasswordBuf
             );
  if (EFI_ERROR (Status)) {
    if (!IsUserPassword) {

      if (CompareGuid (&SysPasswordGuid, &gInsydeSecureFirmwarePasswordGuid)) {
        Table = mSecureFirmwarePasswordHob;
        SecureVariableInHob = TRUE;
      } else {
        Table = mSupervisorPwHob;
      }

      if (Table != NULL) {
        FreePool (PasswordBuf);
        Size = ((EFI_HOB_GENERIC_HEADER *) Table)->HobLength - sizeof (EFI_HOB_GUID_TYPE);
        PasswordBuf = AllocateZeroPool (Size);
        if (PasswordBuf == NULL) {
          return EFI_OUT_OF_RESOURCES;
        }
        CopyMem (PasswordBuf, ((UINT8*) Table) + sizeof (EFI_HOB_GUID_TYPE), Size);
        if (SecureVariableInHob) {
          *Password = AllocateZeroPool (Size + 1);
          if (*Password == NULL) {
            return EFI_OUT_OF_RESOURCES;
          }
          (*Password)[0] = (UINT8) Size;
          CopyMem (*Password + 1, PasswordBuf, Size);
          FreePool (PasswordBuf);
          return EFI_SUCCESS;
        }
      } else {
        FreePool (PasswordBuf);
        return EFI_NOT_FOUND;
      }
    } else {
      if (!IsUserPassword) {
        FreePool (PasswordBuf);
        return EFI_NOT_FOUND;
      } else {
        Table = mUserPwHob;
      }

      if (Table != NULL) {
        Size = ((EFI_HOB_GENERIC_HEADER *) Table)->HobLength - sizeof (EFI_HOB_GUID_TYPE);
        Status = EFI_SUCCESS;
      } else {
        Status = EFI_NOT_FOUND;
      }

      if (Status == EFI_SUCCESS) {
        CopyMem(PasswordBuf, ((UINT8*) Table) + sizeof (EFI_HOB_GUID_TYPE), Size);
      } else {
        FreePool (PasswordBuf);
        return EFI_NOT_FOUND;
      }
    }
  }

  PasswordLen = PasswordBuf[0];

  if (PasswordLen > SysPasswordPrivate->SysPasswordSetupInfoData->MaxSysPasswordLength) {
    FreePool (PasswordBuf);
    return EFI_NOT_FOUND;
  }

  if (IsUserPassword || !PcdGetBool (PcdSecureSysPasswordSupported)) {
    *Password = NULL;
    *Password = AllocateZeroPool (PasswordLen + 2);
    if (*Password == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }

    CalCheckSum = 0;
    for (Index = 0; Index < PasswordLen + 2; Index++) {
      (*Password)[Index] = PasswordBuf[Index];
      CalCheckSum = CalCheckSum + (*Password)[Index];
    }
    if (CalCheckSum != 0) {
      CommonSetVariable (
             PasswordKind,
             &SysPasswordGuid,
             EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
             0,
             NULL
             );
      FreePool (*Password);
      FreePool (PasswordBuf);
      return EFI_NOT_FOUND;
    }
  } else {
    *Password = NULL;
    *Password = AllocateZeroPool (1);
    if (*Password == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
    (*Password)[0] = 1;
  }

  FreePool (PasswordBuf);
  return EFI_SUCCESS;
}

/**
  get supervisor or use password

  @param  This                 The EFI_SYS_PASSWORD_SERVICE_PROTOCOL instance.
  @param  SupervisorOrUser     determine to disable supervisor or use password
  @param  Password             buffer pointer to store password

  @retval

**/
EFI_STATUS
EFIAPI
ReadPassword (
  IN  EFI_SYS_PASSWORD_SERVICE_PROTOCOL *This,
  IN  BOOLEAN                           SupervisorOrUser,
  OUT UINT8                             **Password
  )
{
  EFI_STATUS        Status;

  if (PcdGetBool (PcdSysPasswordInCmos)) {
    Status = ReadPasswordInCmos (
               This,
               SupervisorOrUser,
               Password
               );
  } else {
    Status = ReadPasswordFromVariable (
               This,
               SupervisorOrUser,
               Password
               );
  }

  return Status;
}

/**
  Check supervisor or use password is set or not.

  @param  This                 The EFI_SYS_PASSWORD_SERVICE_PROTOCOL instance.
  @param  SupervisorOrUser     determine to disable supervisor or use password

  @retval EFI_SUCCESS          password is set
  @retval EFI_NOT_FOUND        password isn't set

**/
EFI_STATUS
EFIAPI
GetPasswordStatus (
  IN  EFI_SYS_PASSWORD_SERVICE_PROTOCOL    *This,
  IN  BOOLEAN                              SupervisorOrUser
  )
{
  EFI_STATUS                        Status;
  UINT8                             *Password;

  Password = NULL;

  Status = ReadPassword (This, SupervisorOrUser, &Password);
  if (Status != EFI_SUCCESS || Password == NULL) {
    return Status;
  }

  if (*Password == 0) {
    FreePool (Password);
    return EFI_NOT_FOUND;
  }

  FreePool (Password);

  return EFI_SUCCESS;
}

/**
  Check supervisor or use password is correct or not.

  @param  This                 The EFI_SYS_PASSWORD_SERVICE_PROTOCOL instance.
  @param  PasswordPtr          password address
  @param  PasswordLength       password length
  @param  SupervisorOrUser     determine to disable supervisor or use password

  @retval

**/
EFI_STATUS
EFIAPI
CheckPassword (
    IN  EFI_SYS_PASSWORD_SERVICE_PROTOCOL    *This,
    IN  VOID                                 *PasswordPtr,
    IN  UINTN                                PasswordLength,
    IN  BOOLEAN                              SupervisorOrUser
  )
{
  EFI_STATUS                       Status;
  UINT8                            *SavePassword;
  UINT8                            *InputPassword;
  UINT8                            *InputPasswordTmp;
  VOID                             *Table;

  SavePassword = NULL;
  InputPasswordTmp = NULL;
  Status = ReadPassword (This, SupervisorOrUser, &SavePassword);

  if (Status != EFI_SUCCESS || SavePassword == NULL) {
    return Status;
  }

  if (SavePassword[0] == 0) {
    FreePool (SavePassword);
    return EFI_NOT_FOUND;
  }

  if (!PcdGetBool (PcdSysPasswordInCmos) && PcdGetBool (PcdSecureSysPasswordSupported) && SupervisorOrUser == SystemSupervisor) {
    Table = mSecureFirmwarePasswordHob;
    if (Table != NULL) {
      if (IsSecurePasswordValid (This, PasswordPtr, PasswordLength,((UINT8*) Table) + sizeof (EFI_HOB_GUID_TYPE), sizeof (EFI_SHA256_HASH))) {
        Status = EFI_SUCCESS;
      } else {
        Status = EFI_CRC_ERROR;
      }
    } else {
      Status = This->UnlockPassword (This, PasswordPtr, PasswordLength);
      if (Status == EFI_SUCCESS) {
        //
        // Re-lock password sate if password state is changed by UnlockedPassword ()
        //
        This->LockPassword (This);
      } else if (Status == EFI_ALREADY_STARTED) {
        //
        // Check password successful but password state is in unlocked state, return EFI_SUCCESS
        // and needn't change password state to locked.
        //
        Status = EFI_SUCCESS;
      }
    }
    FreePool (SavePassword);
  } else {
    Status = EFI_SUCCESS;
    InputPasswordTmp = Unicode2Ascii (This, PasswordPtr, PasswordLength);
    if (InputPasswordTmp == NULL) {
      if (SavePassword != NULL) {
        FreePool (SavePassword);
      }
      return EFI_OUT_OF_RESOURCES;
    }
    InputPassword    = Encode (InputPasswordTmp, PasswordLength);

    if (!PasswordCmp (InputPassword, SavePassword)) {
      Status = EFI_CRC_ERROR;
    }

    FreePool (SavePassword);
    FreePool (InputPassword);
  }

  return Status;
}

/**
  This function set system password

  @param  This                 The EFI_SYS_PASSWORD_SERVICE_PROTOCOL instance.
  @param  PasswordPtr          password address
  @param  PasswordLength       password length
  @param  SupervisorOrUser     determine to disable supervisor or use password

  @retval EFI_INVALID_PARAMETER parameter is invalid
  @retval EFI_SUCCESS          set password success

**/
EFI_STATUS
EFIAPI
SetPassword (
    IN  EFI_SYS_PASSWORD_SERVICE_PROTOCOL    *This,
    IN  VOID                            *PasswordPtr,
    IN  UINTN                           PasswordLength,
    IN  BOOLEAN                         SupervisorOrUser
  )
{
  UINT8                             *Password,*PasswordTmp;
  EFI_STATUS                        Status;

  if ((PasswordPtr == NULL) || (PasswordLength == 0)) {
    return EFI_INVALID_PARAMETER;
  }

  PasswordTmp = Unicode2Ascii (This, PasswordPtr, PasswordLength);
  if (PasswordTmp == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  Password = Encode (PasswordTmp, PasswordLength);

  Status = WritePassword (This, SupervisorOrUser, Password);
  //
  //Free Pools that Allocate from Encode()
  //
  FreePool (Password);

  return Status;
}

/**
  This function disable device security.

  @param  This                 The EFI_SYS_PASSWORD_SERVICE_PROTOCOL instance.
  @param  SupervisorOrUser     determine to disable supervisor or use password

  @retval                      return the status of WritePassword()

**/
EFI_STATUS
EFIAPI
DisablePassword (
    IN  EFI_SYS_PASSWORD_SERVICE_PROTOCOL *This,
    IN  BOOLEAN                           SupervisorOrUser
  )
{
  return WritePassword (This, SupervisorOrUser, NULL);
}


/**
  Get the setup info. data

  @param  This                 The EFI_SYS_PASSWORD_SERVICE_PROTOCOL instance.
  @param  SetupInfoBuffer      buffer to store the setup info. data

  @retval EFI_SUCCESS          get finish

**/
EFI_STATUS
EFIAPI
GetSysPswdSetupInfoData (
  IN  EFI_SYS_PASSWORD_SERVICE_PROTOCOL *This,
  OUT SYS_PASSWORD_SETUP_INFO_DATA      *SetupInfoBuffer
  )
{
  SYS_PASSWORD_PRIVATE                *SysPasswordPrivate;

  SysPasswordPrivate = GET_PRIVATE_FROM_SYS_PASSWORD (This);

  CopyMem (SetupInfoBuffer, SysPasswordPrivate->SysPasswordSetupInfoData, sizeof (SYS_PASSWORD_SETUP_INFO_DATA));

  return EFI_SUCCESS;
}

/**
  Set the setup info. data

  @param  This                 The EFI_SYS_PASSWORD_SERVICE_PROTOCOL instance.
  @param  SetupInfoBuffer      buffer to store the setup info. data
  @param  SetupBits            which setup to be modified

  @retval EFI_SUCCESS          set finish

**/
EFI_STATUS
EFIAPI
SetSysPswdSetupInfoData (
  IN  EFI_SYS_PASSWORD_SERVICE_PROTOCOL *This,
  IN  SYS_PASSWORD_SETUP_INFO_DATA      *SetupInfoBuffer,
  IN  UINT32                            SetupBits
  )
{
  UINT32                              BitMask;
  UINTN                               Index;
  SYS_PASSWORD_PRIVATE                *SysPasswordPrivate;
  SYS_PASSWORD_SETUP_INFO_DATA        *SysPasswordSetupInfoData;

  SysPasswordPrivate = GET_PRIVATE_FROM_SYS_PASSWORD (This);

  SysPasswordSetupInfoData = SysPasswordPrivate->SysPasswordSetupInfoData;

  BitMask = 0x1;
  Index   = 0;

  while (SetupBits != 0) {
    while ((BitMask & SetupBits) == 0) {
      Index++;
      BitMask = BitMask << 1;
    }


    switch (Index) {

    case 0:
      SysPasswordSetupInfoData->MaxSysPasswordLength = SetupInfoBuffer->MaxSysPasswordLength;

      SetupBits = SetupBits & (~MAX_SYS_PASSWORD_LENGTH_BIT);
      SysPasswordSetupInfoData->SetupDataValidBits |= MAX_SYS_PASSWORD_LENGTH_BIT;

      break;

    case 1:
      SysPasswordSetupInfoData->CurrentUser = SetupInfoBuffer->CurrentUser;

      SetupBits = SetupBits & (~CURRENT_USER_BIT);
      SysPasswordSetupInfoData->SetupDataValidBits |= CURRENT_USER_BIT;
      break;

    case 2:
      SysPasswordSetupInfoData->HaveSysPasswordCheck = SetupInfoBuffer->HaveSysPasswordCheck;

      SetupBits = SetupBits & (~SYS_PASSWORD_CHECK_BIT);
      SysPasswordSetupInfoData->SetupDataValidBits |= SYS_PASSWORD_CHECK_BIT;
      break;

    default :
      return EFI_UNSUPPORTED;

    }

  }

  return EFI_SUCCESS;
}


/**
  Internal function to allocate memory to store specific GUID data hob information.

  @param[in]  HobGuid      Pointer to specific HOB GUID.

  @return Allocated memory to store HOB data or NULL if not found.
**/
STATIC
VOID *
AllocateHobData (
  IN EFI_GUID        *HobGuid
  )
{
  EFI_STATUS      Status;
  VOID            *HobList;
  VOID            *Table;
  UINTN           HobSize;
  VOID            *HobData;

  Status = EfiGetSystemConfigurationTable (&gEfiHobListGuid, (VOID *) &HobList);
  if (EFI_ERROR (Status)) {
    return NULL;
  }
  Table = GetNextGuidHob (HobGuid, (VOID *) HobList);
  if (Table == NULL) {
    return NULL;
  }
  HobSize = ((EFI_HOB_GENERIC_HEADER *) Table)->HobLength;
  HobData = AllocateZeroPool (HobSize);
  if (HobData == NULL) {
    return NULL;
  }
  CopyMem (HobData, Table, HobSize);
  return HobData;
}



/**
  This function uses to check if supervisor password exist.

  @retval TRUE     Insyde password is in locked state.
  @retval FALSE    Insyde password is in unloced state.
**/
BOOLEAN
EFIAPI
IsSupervisorPwdExist (
  VOID
  )
{
  EFI_SYS_PASSWORD_SERVICE_PROTOCOL         *SysPasswordService;
  EFI_STATUS                                Status;

  Status = gBS->LocateProtocol (
                  &gEfiSysPasswordServiceProtocolGuid,
                  NULL,
                  (VOID **) &SysPasswordService
                  );
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return FALSE;
  }

  Status = SysPasswordService->GetStatus (SysPasswordService, SystemSupervisor);

  return (BOOLEAN) (Status == EFI_SUCCESS ? TRUE : FALSE);
}


/**
  Check SysPassword status

**/

VOID
CheckSysPasswordStatus (
VOID
  )
{
  H2O_DIALOG_PROTOCOL                   *H2oDialogProtocol;
  EFI_STATUS                            Status;
  CHAR16                                *ErrorStatusMsg;
  CHAR16                                *InputPasswordMsg;
  CHAR16                                *ConfirmPasswordMsg;  
  EFI_INPUT_KEY                         Key;
  EFI_SYS_PASSWORD_SERVICE_PROTOCOL     *SysPasswordService;
  SYS_PASSWORD_SETUP_INFO_DATA          SetupInfoBuffer;  
  CHAR16                                *Password;  
  CHAR16                                *ConfirmPassword;    
  CHAR16                                *ConfirmFailMsg;   
  CHAR16                                *ThreeIncorrectMsg;
  CHAR16                                *SysHaltMsg;
  UINT8                                Count; 

  InputPasswordMsg = NULL;
  ErrorStatusMsg = NULL;  
  ConfirmPasswordMsg = NULL;
  ConfirmFailMsg     = NULL;
  ThreeIncorrectMsg  = NULL;
  
  Status = gBS->LocateProtocol (
                  &gH2ODialogProtocolGuid,
                  NULL,
                  (VOID **)&H2oDialogProtocol
                  );
  if (EFI_ERROR(Status)) {
    return;
  }

  Status = gBS->LocateProtocol (
                  &gEfiSysPasswordServiceProtocolGuid,
                  NULL,
                  (VOID **) &SysPasswordService
                  );
  if (EFI_ERROR(Status)) {
    return;
  }
  

  //
  // if Supervisor password exist, leave
  //
  if (IsSupervisorPwdExist()) {
    return;
  }

  ErrorStatusMsg = HiiGetString(mStringPackHandle, STRING_TOKEN (STR_WARNING), NULL);
  H2oDialogProtocol->ConfirmDialog (
                       DlgOk,
                       FALSE,
                       40,
                       NULL,
                       &Key,
                       ErrorStatusMsg
                       );
  
  Status = SysPasswordService->GetSysPswdSetupInfoData(
                             SysPasswordService,
                             &SetupInfoBuffer
                             );
  
  
  Password = AllocatePool ((SetupInfoBuffer.MaxSysPasswordLength + 1) * sizeof (CHAR16));
  if (Password == NULL) {
    return;
  }
  
  ConfirmPassword = AllocatePool ((SetupInfoBuffer.MaxSysPasswordLength + 1) * sizeof (CHAR16));
  if (ConfirmPassword == NULL) {
    FreePool (Password);
    return;
  }
  
  
  InputPasswordMsg = HiiGetString(mStringPackHandle, STRING_TOKEN (STR_INPUT_SYS_PASSWORD), NULL);
  H2oDialogProtocol->PasswordDialog (
                       0,
                       FALSE,
                       (UINT32) (SetupInfoBuffer.MaxSysPasswordLength + 1),
                       Password,
                       &Key,
                       InputPasswordMsg
                       );
  
      
  ConfirmPasswordMsg = HiiGetString(mStringPackHandle, STRING_TOKEN (STR_CONFIRM_SYS_PASSWORD), NULL);
  for (Count = 1;;Count++) {    
    H2oDialogProtocol->PasswordDialog (
                         0,
                         FALSE,
                         (UINT32) (SetupInfoBuffer.MaxSysPasswordLength + 1),
                         ConfirmPassword,
                         &Key,
                         ConfirmPasswordMsg
                         );
    if (StrCmp(Password, ConfirmPassword)) {
      ConfirmFailMsg = HiiGetString(mStringPackHandle, STRING_TOKEN (STR_CONFIRM_INCORRECT_SYS_PASSWORD), NULL);
      H2oDialogProtocol->ConfirmDialog (
                           DlgOk,
                           FALSE,
                           40,
                           NULL,
                           &Key,
                           ConfirmFailMsg
                           );

      if (Count == RETRY_LIMIT) {
        ThreeIncorrectMsg = HiiGetString (mStringPackHandle, STRING_TOKEN (STR_SYS_DIALOG_THREE_ERRORS), NULL);      
        SysHaltMsg = HiiGetString(mStringPackHandle, STRING_TOKEN (STR_SYS_DIALOG_HALT), NULL);
        H2oDialogProtocol->CreateMsgPopUp (
                             40,
                             2,
                             ThreeIncorrectMsg,
                             SysHaltMsg
                             );      
      }
    } else {
      SysPasswordService->SetPassword(SysPasswordService, ConfirmPassword, (SetupInfoBuffer.MaxSysPasswordLength + 1), SystemSupervisor);
      break;
    }
  }

  if (Password != NULL) {
    FreePool(Password);
  }
  
  if (ConfirmPassword != NULL) {
    FreePool(ConfirmPassword);
  }
  gRT->ResetSystem (EfiResetCold, EFI_SUCCESS, 0, NULL);
}


/**

  Check Supervisor password status
  @param[in]        Event               Pointer to this event
  @param[in]        Handle              The handle associated with a previously registered checkpoint handler.

**/
VOID
H2OBdsCpBootDeviceSelectCallback (
  IN EFI_EVENT                              Event,
  IN H2O_CP_HANDLE                          Handle
  )
{
  if (FeaturePcdGet (PcdH2OPasswordSupervisorNotSetWarning)) {
    H2OCpUnregisterHandler (Handle);
    CheckSysPasswordStatus ();
  }
}




/**
  Main entry point to the System Password.

  @param  ImageHandle          Image handle this driver.
  @param  SystemTable          Pointer to SystemTable.

  @retval EFI_STATUS

**/
EFI_STATUS
EFIAPI
InitializeSysPasswordDriver (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS                         Status;
  SYS_PASSWORD_PRIVATE               *PasswordInstance;
  SYS_PASSWORD_PRIVATE               *DxePasswordInstance;
  EFI_HANDLE                         Handle;
  SYS_PASSWORD_SETUP_INFO_DATA       *SysPswdSetupInfoData;
  EFI_SYS_PASSWORD_SERVICE_PROTOCOL  *SysPswdProtocol;
  EDKII_VARIABLE_LOCK_PROTOCOL       *VariableLock;

  mSecureFirmwarePasswordHob = AllocateHobData (&gInsydeSecureFirmwarePasswordHobGuid);
  mSupervisorPwHob           = AllocateHobData (&gEfiSupervisorPwHobGuid);
  mUserPwHob                 = AllocateHobData (&gEfiUserPwHobGuid);

  PasswordInstance = AllocateZeroPool (sizeof (SYS_PASSWORD_PRIVATE));
  if (PasswordInstance == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  Handle = NULL;

  PasswordInstance->Signature         = SYS_PASSWORD_SIGNATURE;

  PasswordInstance->SysPasswordProtocol.GetStatus               = GetPasswordStatus;
  PasswordInstance->SysPasswordProtocol.CheckPassword           = CheckPassword;
  PasswordInstance->SysPasswordProtocol.SetPassword             = SetPassword;
  PasswordInstance->SysPasswordProtocol.DisablePassword         = DisablePassword;
  PasswordInstance->SysPasswordProtocol.LockPassword            = LockPassword;
  PasswordInstance->SysPasswordProtocol.UnlockPassword          = UnlockPassword;
  PasswordInstance->SysPasswordProtocol.GetSysPswdSetupInfoData = GetSysPswdSetupInfoData;
  PasswordInstance->SysPasswordProtocol.SetSysPswdSetupInfoData = SetSysPswdSetupInfoData;

  if (!InSmm ()) {
    Status = gBS->InstallProtocolInterface (
                    &Handle,
                    &gEfiSysPasswordServiceProtocolGuid,
                    EFI_NATIVE_INTERFACE,
                    &(PasswordInstance->SysPasswordProtocol)
                    );

  } else {
    Status = gSmst->SmmInstallProtocolInterface (
                      &Handle,
                      &gEfiSysPasswordServiceProtocolGuid,
                      EFI_NATIVE_INTERFACE,
                      &(PasswordInstance->SysPasswordProtocol)
                      );
  }

  if (Status != EFI_SUCCESS) {
    FreePool (PasswordInstance);
    return Status;
  }

  //
  //  Initialize the setup info data
  //
  if (!InSmm ()) {
    SysPswdSetupInfoData = AllocateReservedZeroPool (sizeof (SYS_PASSWORD_SETUP_INFO_DATA));
    if (SysPswdSetupInfoData == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }

    SysPswdSetupInfoData->MaxSysPasswordLength = SYS_PASSWORD_MAX_NUMBER;
    SysPswdSetupInfoData->CurrentUser          = NoUserType;
    SysPswdSetupInfoData->HaveSysPasswordCheck = TRUE;
    PasswordInstance->SysPasswordSetupInfoData = SysPswdSetupInfoData;

    if (FeaturePcdGet (PcdH2OBdsCpBootDeviceSelectSupported) &&
       FeaturePcdGet (PcdH2OPasswordSupervisorNotSetWarning)) {
      H2O_CP_HANDLE                         H2OBdsCpBootDeviceSelectHandle;
      //
      // Register for callback on system password publication
      //
      H2OBdsCpBootDeviceSelectHandle = NULL;
      Status = H2OCpRegisterHandler (
                 &gH2OBdsCpBootDeviceSelectGuid,
                 H2OBdsCpBootDeviceSelectCallback,
                 SYS_PASSWORD_CHECKPOINT_NOTIFY_TPL,
                 &H2OBdsCpBootDeviceSelectHandle
                 );
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "Checkpoint Register Fail: %g (%r)\n", &gH2OBdsCpBootDeviceSelectGuid, Status));
        return Status;
      }
      DEBUG ((DEBUG_INFO, "Checkpoint Registered: %g (%r)\n", &gH2OBdsCpBootDeviceSelectGuid, Status));

      mStringPackHandle = HiiAddPackages (
                            &mSysPasswordStringPackGuid,
                            ImageHandle,
                            SysPasswordStrings,
                            NULL
                            );

    }

    if (!PcdGetBool (PcdSysPasswordInCmos)) {
      Status = gBS->LocateProtocol (&gEdkiiVariableLockProtocolGuid, NULL, (VOID **) &VariableLock);
      if (!EFI_ERROR (Status)) {
        if (!PcdGetBool (PcdSecureSysPasswordSupported)) {
          VariableLock->RequestToLock (VariableLock, L"SystemSupervisorPw", &gEfiSupervisorPwGuid);
        }
        if (PcdGetBool (PcdSysPasswordSupportUserPswd)) {
          VariableLock->RequestToLock (VariableLock, L"SystemUserPw", &gEfiUserPwGuid);
        }
      }
    }
  } else {
    Status = gBS->LocateProtocol (
                    &gEfiSysPasswordServiceProtocolGuid,
                    NULL,
                    (VOID**) &SysPswdProtocol
                    );
    if (EFI_ERROR (Status)) {
      return Status;
    }
    DxePasswordInstance = GET_PRIVATE_FROM_SYS_PASSWORD (SysPswdProtocol);
    PasswordInstance->SysPasswordSetupInfoData = DxePasswordInstance->SysPasswordSetupInfoData;
  }

  return EFI_SUCCESS;
}

