/** @file
  Encrypt and Decrypt HDD Password String Process

;******************************************************************************
;* Copyright (c) 2022, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <Guid/HddPasswordVariable.h>
#include <Protocol/Hash.h>
#include <Protocol/CryptoServices.h>
#include <Protocol/Tcg2Protocol.h>
#include <Protocol/SmmSxDispatch2.h>
#include <Protocol/AcpiRestoreCallbackDone.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/SavedPasswordStringProcessLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/Tpm2CommandLib.h>
#include <Library/PcdLib.h>
#include <Library/LockBoxLib.h>
#include <Library/UefiLib.h>
#include <Library/HobLib.h>

#define KEY_SIZE (HDD_PASSWORD_ROOT_KEY_SIZE + HDD_PASSWORD_RANDOM_NUM_SIZE)

EFI_GUID       mTcg2UpdatedPlatformAuthGuid = {
  0xebab103e, 0x8906, 0x4dc8, 0xbd, 0xd1, 0xa0, 0xbc, 0x26, 0xb1, 0xba, 0x86
};

CRYPTO_SERVICES_PROTOCOL             *CryptoService  = NULL;
BOOLEAN                              mBootOnS3Resume = FALSE;

/**
  Returns the next entry of the HDD password table.

  @param[in]        HddPasswordTable         Pointer to HddPasswordTable.

  @retval           The pointer to the next table.

**/
HDD_PASSWORD_TABLE *
GetNextTableEntry (
  IN  HDD_PASSWORD_TABLE                *HddPasswordTablePtr
  )
{
  return (HDD_PASSWORD_TABLE *)((UINTN)(HddPasswordTablePtr + 1) + HddPasswordTablePtr->ExtDataSize);
}

/**
  Calculate the number of Hdd password tables.

  @param[in]        HddPasswordTable         Pointer to HddPasswordTable.
  @param[in]        HddPasswordTableSize     HddPasswordTable size.

  @retval The number of tables.

**/
UINTN
NumOfHddPasswordTable (
  IN  HDD_PASSWORD_TABLE                  *HddPasswordTable,
  IN  UINTN                               HddPasswordTableSize
  )
{
  UINTN                               NumOfTable;
  HDD_PASSWORD_TABLE                  *HddPasswordTablePtr;

  if (HddPasswordTable == NULL || HddPasswordTableSize == 0) {
    return 0;
  }

  NumOfTable = 0;
  HddPasswordTablePtr = (HDD_PASSWORD_TABLE *)HddPasswordTable;
  while (((UINTN)HddPasswordTable + HddPasswordTableSize) > (UINTN)HddPasswordTablePtr) {
    HddPasswordTablePtr = GetNextTableEntry (HddPasswordTablePtr);
    NumOfTable++;
  }

  return NumOfTable;
}

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

  if (!InSmm ()) {
    Status = gBS->LocateProtocol (
                    &mTcg2UpdatedPlatformAuthGuid, 
                    NULL, 
                    (VOID **) &Auth
                    );
  } else {
    Status = gSmst->SmmLocateProtocol (
                    &mTcg2UpdatedPlatformAuthGuid,
                    NULL,
                    (VOID **) &Auth
                    );
  }
  if (EFI_ERROR (Status)) {
    //
    // Auth value is default before protocol installation
    //
    return NULL;
  }
  return (TPM2B_AUTH *)Auth;
}

/**
  Clear TPM NV Index with handle NvIndex.

  @param  NvIndex              The index to be clear.
  @param  DataSize             The size of NvIndex in bytes.

  @retval EFI_SUCCESS          Operation completed successfully.
  @retval Others               Operation failed.

**/
EFI_STATUS
Tpm2NvClear (
  IN  UINT32                         NvIndex,
  IN  UINT16                         DataSize
  )
{
  EFI_STATUS                         Status;
  TPM2B_AUTH                         *PlatformAuth;
  TPMS_AUTH_COMMAND                  *AuthSession;
  TPM2B_MAX_BUFFER                   *OutData;
  TPMS_AUTH_COMMAND                  LocalAuthSession;

  OutData = AllocateZeroPool (sizeof (TPM2B_MAX_BUFFER));
  if (OutData == NULL){
    return EFI_OUT_OF_RESOURCES;
  }

  PlatformAuth = GetAuthKey ();
  if (PlatformAuth == NULL) {
    AuthSession = NULL;
  } else {
    ZeroMem (&LocalAuthSession, sizeof (LocalAuthSession));
    AuthSession = &LocalAuthSession;
    AuthSession->sessionHandle = TPM_RS_PW;
    AuthSession->hmac.size = PlatformAuth->size;
    CopyMem (AuthSession->hmac.buffer, PlatformAuth->buffer, PlatformAuth->size);
  }

  Status = Tpm2NvRead (TPM_RH_PLATFORM, NvIndex, AuthSession, DataSize, 0, OutData);

  if (Status != EFI_NOT_FOUND) {
    Status = Tpm2NvUndefineSpace(TPM_RH_PLATFORM, NvIndex, AuthSession);
  } 
  FreePool (OutData);

  return EFI_SUCCESS;
}

/**
  Create TPM NV Index with handle NvIndex.

  @param  NvIndex              The index to define.
  @param  DataSize             The size of NvIndex in bytes.

  @retval EFI_SUCCESS          Operation completed successfully.
  @retval Others               Operation failed.

**/
EFI_STATUS 
Tpm2NvDef (
  IN  UINT32                         NvIndex,
  IN  UINT16                         DataSize
  )
{
  EFI_STATUS                         Status;
  TPM2B_AUTH                         *PlatformAuth;
  TPMS_AUTH_COMMAND                  *AuthSession;
  TPM2B_MAX_BUFFER                   *OutData;
  TPMS_AUTH_COMMAND                  LocalAuthSession;
  TPM2B_AUTH                         AuthData;
  TPM2B_NV_PUBLIC                    NvPublic;
  
  Tpm2NvClear (NvIndex, DataSize);

  OutData = AllocateZeroPool (sizeof (TPM2B_MAX_BUFFER));
  if (OutData == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  PlatformAuth = GetAuthKey ();
  if (PlatformAuth == NULL) {
    AuthSession = NULL;
  } else {
    ZeroMem (&LocalAuthSession, sizeof (LocalAuthSession));
    AuthSession = &LocalAuthSession;
    AuthSession->sessionHandle = TPM_RS_PW;
    AuthSession->hmac.size = PlatformAuth->size;
    CopyMem (AuthSession->hmac.buffer, PlatformAuth->buffer, PlatformAuth->size);
  }
  Status = Tpm2NvRead (TPM_RH_PLATFORM, NvIndex, AuthSession, DataSize, 0, OutData);
  if (EFI_ERROR (Status) && (Status != EFI_NOT_READY)) {
    ZeroMem (&NvPublic, sizeof (NvPublic));
    NvPublic.nvPublic.nvIndex  = NvIndex;
    NvPublic.nvPublic.dataSize = DataSize;
    NvPublic.nvPublic.nameAlg  = TPM_ALG_SHA256;
    NvPublic.nvPublic.attributes.TPMA_NV_PLATFORMCREATE = 1; //bit 30 :TPMA_NV_PLATFORMCREATE
    NvPublic.nvPublic.attributes.TPMA_NV_NO_DA          = 1; //bit 25 :TPMA_NV_NO_DA
    NvPublic.nvPublic.attributes.TPMA_NV_PPREAD         = 1; //bit 16 :TPMA_NV_PPREAD
    NvPublic.nvPublic.attributes.TPMA_NV_PPWRITE        = 1; //bit 0  :TPMA_NV_PPWRITE
    NvPublic.nvPublic.authPolicy.size = 0;                   //empty policy

    NvPublic.size = sizeof (NvPublic.nvPublic.nvIndex) +
                    sizeof (NvPublic.nvPublic.nameAlg) +
                    sizeof (NvPublic.nvPublic.attributes) +
                    sizeof (NvPublic.nvPublic.authPolicy.size) +
                    sizeof (UINT16);

    ZeroMem (&AuthData, sizeof (AuthData));
    Status = Tpm2NvDefineSpace (TPM_RH_PLATFORM, AuthSession, &AuthData, &NvPublic);
  }
  FreePool (OutData);

  return EFI_SUCCESS;
}

/**
  Get Root Key and Random number from TPM.

  @param  RandomNum                         Pointer to the RandomNum.
  @param  RootKey                           Pointer to the RootKey.
  @param  NvIndex                           The index to be read.

  @retval EFI_SUCCESS                       The operation completed successfully.
  @retval Others                            An unexpected error occurred.

**/
EFI_STATUS 
GetKeyFromTpm (
  IN OUT UINT8                       *RandomNum,
  IN OUT UINT8                       *RootKey,
  IN UINT32                          NvIndex
  )
{
  EFI_STATUS                         Status;
  TPM2B_AUTH                         *PlatformAuth;
  TPMS_AUTH_COMMAND                  *AuthSession;
  TPM2B_MAX_BUFFER                   *OutData;
  TPMS_AUTH_COMMAND                  LocalAuthSession;
  UINT8                              Index;

  if (RandomNum == NULL || RootKey == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  OutData = AllocateZeroPool (sizeof (TPM2B_MAX_BUFFER));
  if (OutData == NULL){
    return EFI_OUT_OF_RESOURCES;
  } 

  PlatformAuth = GetAuthKey ();
  if (PlatformAuth == NULL) {
    AuthSession = NULL;
  } else {
    ZeroMem (&LocalAuthSession, sizeof (LocalAuthSession));
    AuthSession = &LocalAuthSession;
    AuthSession->sessionHandle = TPM_RS_PW;
    AuthSession->hmac.size = PlatformAuth->size;
    CopyMem (AuthSession->hmac.buffer, PlatformAuth->buffer, PlatformAuth->size);
  }
  Status = Tpm2NvRead (TPM_RH_PLATFORM, NvIndex, AuthSession, KEY_SIZE, 0, OutData);
  if (!EFI_ERROR (Status)) {
    
    for (Index = 0; Index < HDD_PASSWORD_ROOT_KEY_SIZE; Index++) {   
      RootKey[Index] = OutData->buffer[Index];
    }
    
    for (Index = HDD_PASSWORD_ROOT_KEY_SIZE; Index < KEY_SIZE; Index++) {   
      RandomNum[Index - HDD_PASSWORD_ROOT_KEY_SIZE] = OutData->buffer[Index];
    }
  } else {
    FreePool (OutData);
    return EFI_UNSUPPORTED;
  }
  FreePool (OutData);
  return EFI_SUCCESS;
}

/**
  Save Root Key and Random number in TPM.

  @param  RandomNum                         Pointer to the RandomNum.
  @param  RootKey                           Pointer to the RootKey.
  @param  NvIndex                           The index to be saved.

  @retval EFI_SUCCESS                       The operation completed successfully.
  @retval Others                            An unexpected error occurred.

**/
EFI_STATUS 
SavedKeyInTpm (
  IN UINT8                           *RandomNum,
  IN UINT8                           *RootKey,
  IN UINT32                          NvIndex
  )
{
  EFI_STATUS                         Status;
  TPM2B_AUTH                         *PlatformAuth;
  TPMS_AUTH_COMMAND                  *AuthSession;
  TPM2B_MAX_BUFFER                   *OutData;
  TPMS_AUTH_COMMAND                  LocalAuthSession;
  UINT8                              WriteBuffer[KEY_SIZE];
  UINT8                              Index;

  Tpm2NvDef (NvIndex, KEY_SIZE);
  
  OutData = AllocateZeroPool (sizeof (TPM2B_MAX_BUFFER));
  if (OutData == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  PlatformAuth = GetAuthKey ();
  if (PlatformAuth == NULL) {
    AuthSession = NULL;
  } else {
    ZeroMem (&LocalAuthSession, sizeof (LocalAuthSession));
    AuthSession = &LocalAuthSession;
    AuthSession->sessionHandle = TPM_RS_PW;
    AuthSession->hmac.size = PlatformAuth->size;
    CopyMem (AuthSession->hmac.buffer, PlatformAuth->buffer, PlatformAuth->size);
  }

  ZeroMem (&WriteBuffer, sizeof (WriteBuffer));

  Status = Tpm2NvRead (TPM_RH_PLATFORM, NvIndex, AuthSession, KEY_SIZE, 0, OutData);
  if (!EFI_ERROR(Status)) {
    if (CompareMem (WriteBuffer, OutData->buffer, KEY_SIZE) == 0) {
      return EFI_SUCCESS;
    }
  }
  
  for (Index = 0; Index < HDD_PASSWORD_ROOT_KEY_SIZE; Index++) {
    WriteBuffer[Index] = RootKey[Index];
  }
  
  for (Index = HDD_PASSWORD_ROOT_KEY_SIZE; Index < KEY_SIZE; Index++) {
    WriteBuffer[Index] = RandomNum[Index - HDD_PASSWORD_ROOT_KEY_SIZE];
  }

  OutData->size = KEY_SIZE;
  CopyMem (OutData->buffer, WriteBuffer, OutData->size);
  Status = Tpm2NvWrite (TPM_RH_PLATFORM, NvIndex, AuthSession, OutData, 0);
  FreePool (OutData);
  
  return Status;
}

/**
  SHA256 Hash function.

  @param  DataPtr                           Pointer to the Data.
  @param  DataSize                          The size in byte of the data

  @retval UINT8 *                           Pointer to the Hash data.

**/
UINT8 *
Sha256Hash (
  IN  UINT8                                 *DataPtr,
  IN  UINTN                                 DataSize
  )
{
  EFI_STATUS                                Status;
  EFI_HASH_PROTOCOL                         *EfiHash;
  EFI_HASH_OUTPUT                           HashOutput;
  UINT8                                     *HashData;

  Status                = EFI_SUCCESS;
  EfiHash               = NULL;
  HashOutput.Sha256Hash = NULL;
  HashData              = NULL;

  if ((DataPtr == NULL) || (DataSize == 0)) {
    return NULL;
  }

  if (!InSmm ()) {
    Status = gBS->LocateProtocol (
                    &gEfiHashProtocolGuid,
                    NULL,
                    (VOID **) &EfiHash
                    );
  } else {
    Status = gSmst->SmmLocateProtocol (
                       &gEfiHashProtocolGuid,
                       NULL,
                       (VOID **) &EfiHash
                       );
  }

  if (EFI_ERROR (Status)) {
    return NULL;
  }

  //
  // Hash Data for SHA256 type
  //
  HashOutput.Sha256Hash = AllocateZeroPool (SHA256_DIGEST_SIZE);

  if (HashOutput.Sha256Hash == NULL) {
    FreePool (DataPtr);
    return NULL;
  }

  HashData = AllocateZeroPool (SHA256_DIGEST_SIZE);

  if (HashData == NULL) {
    FreePool (DataPtr);
    FreePool (HashOutput.Sha256Hash);
    return NULL;
  }

  Status = EfiHash->Hash (
                      EfiHash,
                      &gEfiHashAlgorithmSha256Guid,
                      FALSE,
                      (UINT8 *) DataPtr,
                      DataSize,
                      (EFI_HASH_OUTPUT *) &HashOutput
                      );

  if (!EFI_ERROR (Status)) {
    CopyMem (HashData, HashOutput.Sha256Hash, SHA256_DIGEST_SIZE);
  }

  FreePool (HashOutput.Sha256Hash);
  
  return HashData;
}

/**
  Generate 32 bytes (256 bits) Root Key.


  @retval UINT8 *                           Pointer to the Root Key.

**/
UINT8 *
GenerateRootKey (
  VOID
  )
{
  UINT8               *RootKey;
  BOOLEAN             CryptoSuccess;
  EFI_TIME            Time;

  gRT->GetTime (&Time, NULL);
  CryptoSuccess = CryptoService->RandomSeed ((UINT8 *)&Time, sizeof (Time));

  if (!CryptoSuccess) {
    return NULL;
  }

  RootKey = AllocateZeroPool (HDD_PASSWORD_ROOT_KEY_SIZE);
  if (RootKey == NULL) {
    return NULL;
  }

  CryptoSuccess = CryptoService->RandomBytes (RootKey, HDD_PASSWORD_ROOT_KEY_SIZE);

  if (!CryptoSuccess) {
    FreePool (RootKey);
    return NULL;
  }
  
  return RootKey;
}

/**
  Get Secure Key.

  @param  RandomNum                         Pointer to random number buffer.
  @param  RootKey                           Pointer to rootkey buffer.
  @param  SecureKey                         Pointer to the Secure Key.

  @retval EFI_SUCCESS                       The operation completed successfully.
  @retval Others                            An unexpected error occurred.

**/
EFI_STATUS
GetSecureKey (
  IN  UINT8                                 *RandomNum,
  IN  UINT8                                 *RootKey,
  OUT UINT8                                 **SecureKey
  )
{
  EFI_STATUS                                Status;
  BOOLEAN                                   CryptoSuccess;
  UINT8                                     *Nonce;
  UINT8                                     *DerivedKey;
  
  Status        = EFI_SUCCESS;
  CryptoSuccess = FALSE;
  Nonce         = NULL;
  DerivedKey    = NULL;

  if ((RandomNum == NULL) || (RootKey == NULL) || (SecureKey == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Nonce is 32 bytes (256 bits). SHA256 of random number
  //
  Nonce = Sha256Hash (RandomNum, HDD_PASSWORD_RANDOM_NUM_SIZE);

  if (Nonce == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto GetSecureKeyError;
  }

  DerivedKey = AllocateZeroPool (HDD_PASSWORD_SECURITY_KEY_SIZE);

  if (DerivedKey == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto GetSecureKeyError;
  }

  CryptoSuccess = CryptoService->Pbkdf2CreateKey (
                                   Nonce,                                            // Salt
                                   HDD_PASSWORD_NONCE_SIZE,                          // SaltLen
                                   SHA256_WITH_RSA_ENCRYPTION,                       // SHA256_WITH_RSA_ENCRYPTION (0x05)
                                   PasswordIterationCount,                           // 0x2710
                                   RootKey,                                          // Password
                                   HDD_PASSWORD_ROOT_KEY_SIZE,                       // PasswordLen
                                   HDD_PASSWORD_SECURITY_KEY_SIZE,                   // DerivedKeyLen
                                   DerivedKey
                                   );

  if (!CryptoSuccess) {
    Status = EFI_SECURITY_VIOLATION;
    goto GetSecureKeyError;
  }

  *SecureKey = DerivedKey;
  
  ZeroMem (RootKey, HDD_PASSWORD_ROOT_KEY_SIZE);
  ZeroMem (RandomNum, HDD_PASSWORD_RANDOM_NUM_SIZE);
  ZeroMem (Nonce, HDD_PASSWORD_NONCE_SIZE);
  FreePool (Nonce);

GetSecureKeyError:
  return Status;
}

/**
  Generate random number.

  Random number is 16 bytes (128 bits).
  Every time HDD password is stored in a variable, a new random number is created.

  @retval UINT8 *                           A pointer to random number.

**/
UINT8 *
GenerateRandomNum (
  VOID
  )
{
  EFI_STATUS                                Status;
  BOOLEAN                                   CryptoSuccess;
  EFI_TIME                                  Time;
  UINT8                                     *RandomNum;

  Status        = EFI_SUCCESS;
  CryptoSuccess = FALSE;
  RandomNum     = NULL;

  ZeroMem (&Time, sizeof (Time));

  gRT->GetTime (&Time, NULL);

  CryptoSuccess = CryptoService->RandomSeed ((UINT8 *) &Time, sizeof (Time));

  if (!CryptoSuccess) {
    return NULL;
  }

  RandomNum = AllocateZeroPool (HDD_PASSWORD_RANDOM_NUM_SIZE);
  
  if (RandomNum == NULL) {
    return NULL;
  }

  CryptoSuccess = CryptoService->RandomBytes (RandomNum, HDD_PASSWORD_RANDOM_NUM_SIZE);

  if (!CryptoSuccess) {
    FreePool (RandomNum);
    return NULL;
  }

  return RandomNum;
}

/**
  AES CBC function.

  @param  ProcessType                       To do encrypt or decrypt.
  @param  PasswordStr                       Pointer to password string.
  @param  TpmNvIndex                        Pointer to rootkey and random number TpmNvIndex.
  @param  NvIndex                           Save rootkey and random number in TPM NvIndex.
  @param  SkipGenRandomNum                  If flag is true, this means that we still being processed on the same hard disk,
                                            so there is no need to regenerate random number.

  @retval EFI_SUCCESS                       Function has completed successfully.
  @retval Others                            An unexpected error occurred.

**/
EFI_STATUS
AesCbcFunc (
  IN     UINT8                              ProcessType,
  IN OUT CHAR16                             *PasswordStr,
  IN     UINT32                             *TpmNvIndex,
  IN     UINT32                             NvIndex,
  IN     BOOLEAN                            SkipGenRandomNum
  )
{
  EFI_STATUS                                Status;
  BOOLEAN                                   CryptoSuccess;
  VOID                                      *AesCtx;
  UINT8                                     *SecureKey;
  UINT8                                     *RandomNum;
  UINT8                                     *RootKey;
  UINT8                                     *Iv;
  UINT8                                     *InputData;
  UINT8                                     *EncryptData;
  UINT8                                     *DecryptData;

  Status          = EFI_SUCCESS;
  CryptoSuccess   = FALSE;
  AesCtx          = NULL;
  SecureKey       = NULL;
  RandomNum       = NULL;
  RootKey         = NULL;
  Iv              = NULL;
  InputData       = NULL;
  EncryptData     = NULL;
  DecryptData     = NULL;

  InputData = AllocateZeroPool (HDD_PASSWORD_ENCRYPT_SIZE);

  if (InputData == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  CopyMem (InputData, PasswordStr, HDD_PASSWORD_ENCRYPT_SIZE);

  switch (ProcessType) {

  case EncryptProcess:
    //
    // RandomNum is 16 bytes (128 bits), RootKey is 32 bytes (256 bits)
    // Every time HDD Password is stored in a variable, a new random number and RootKey are created
    //
    if (!SkipGenRandomNum) {
      RandomNum = GenerateRandomNum ();
      if (RandomNum == NULL) {
        goto EncryptError;
      }
      
      RootKey = GenerateRootKey ();
      if (RootKey == NULL) {
        goto EncryptError;
      }

      Status = SavedKeyInTpm (RandomNum, RootKey, NvIndex);
      if (EFI_ERROR (Status)) {
        goto EncryptError;
      }
      *TpmNvIndex = NvIndex;
    } else {
      RandomNum = AllocateZeroPool (HDD_PASSWORD_RANDOM_NUM_SIZE);
      if (RandomNum == NULL) {
        goto EncryptError;
      }

      RootKey = AllocateZeroPool (HDD_PASSWORD_ROOT_KEY_SIZE);
      if (RootKey == NULL) {
        goto EncryptError;
      }

      Status = GetKeyFromTpm (RandomNum, RootKey, *TpmNvIndex);
      if (EFI_ERROR (Status)) {
        goto EncryptError;
      }
    }

    //
    // IV is 32 bytes (256 bits). SHA256 of random number
    //
    Iv = Sha256Hash (RandomNum, HDD_PASSWORD_RANDOM_NUM_SIZE);

    if (Iv == NULL) {
      Status = EFI_OUT_OF_RESOURCES;
      goto EncryptError;
    }

    //
    // A 256 bits AES key is needed for HDD Password encryption
    //
    Status = GetSecureKey (RandomNum, RootKey, &SecureKey);

    if (EFI_ERROR (Status)) {
      goto EncryptError;
    }

    EncryptData = AllocateZeroPool (HDD_PASSWORD_ENCRYPT_SIZE);
    if (EncryptData == NULL) {
      Status = EFI_OUT_OF_RESOURCES;
      goto EncryptError;
    }

    AesCtx = AllocateZeroPool (CryptoService->AesGetContextSize ());
    if (AesCtx == NULL) {
      Status = EFI_OUT_OF_RESOURCES;
      goto EncryptError;
    }

    CryptoSuccess = CryptoService->AesInit (AesCtx, SecureKey, (HDD_PASSWORD_SECURITY_KEY_SIZE * BITS_PER_BYTE));

    if (!CryptoSuccess) {
      Status = EFI_SECURITY_VIOLATION;
      goto EncryptError;
    }

    CryptoSuccess = CryptoService->AesCbcEncrypt (
                                     AesCtx,
                                     InputData,
                                     HDD_PASSWORD_ENCRYPT_SIZE,
                                     Iv,
                                     EncryptData
                                     );

    if (!CryptoSuccess) {
      Status = EFI_SECURITY_VIOLATION;
      goto EncryptError;
    }

    CopyMem (PasswordStr, EncryptData, HDD_PASSWORD_ENCRYPT_SIZE);
    ZeroMem (InputData, HDD_PASSWORD_ENCRYPT_SIZE);
    ZeroMem (Iv, HDD_PASSWORD_IV_SIZE);
    ZeroMem (SecureKey, HDD_PASSWORD_SECURITY_KEY_SIZE);

  EncryptError:
    FreePool (InputData);
    FreePool (Iv);
    FreePool (EncryptData);
    FreePool (AesCtx);
    FreePool (SecureKey);
    FreePool (RandomNum);
    FreePool (RootKey);

    break;

  case DecryptProcess:

    RandomNum = AllocateZeroPool (HDD_PASSWORD_RANDOM_NUM_SIZE);
    if (RandomNum == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }

    RootKey = AllocateZeroPool (HDD_PASSWORD_ROOT_KEY_SIZE);
    if (RootKey == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }

    Status = GetKeyFromTpm (RandomNum, RootKey, *TpmNvIndex);
    if (EFI_ERROR (Status)) {
      goto DecryptError;
    }

    //
    // IV is 32 bytes (256 bits). SHA256 of random number
    //
    Iv = Sha256Hash (RandomNum, HDD_PASSWORD_RANDOM_NUM_SIZE);

    if (Iv == NULL) {
      Status = EFI_OUT_OF_RESOURCES;
      goto DecryptError;
    }

    //
    // A 256 bits AES key is needed for HDD Password decryption
    //
    Status = GetSecureKey (RandomNum, RootKey, &SecureKey);

    if (EFI_ERROR (Status)) {
      goto DecryptError;
    }

    DecryptData = AllocateZeroPool (HDD_PASSWORD_ENCRYPT_SIZE);

    if (DecryptData == NULL) {
      Status = EFI_OUT_OF_RESOURCES;
      goto DecryptError;
    }

    AesCtx = AllocatePool (CryptoService->AesGetContextSize ());

    if (AesCtx == NULL) {
      Status = EFI_OUT_OF_RESOURCES;
      goto DecryptError;
    }

    CryptoSuccess = CryptoService->AesInit (AesCtx, SecureKey, (HDD_PASSWORD_SECURITY_KEY_SIZE * BITS_PER_BYTE));

    if (!CryptoSuccess) {
      Status = EFI_SECURITY_VIOLATION;
      goto DecryptError;
    }

    CryptoSuccess = CryptoService->AesCbcDecrypt (
                                     AesCtx,
                                     InputData,
                                     HDD_PASSWORD_ENCRYPT_SIZE,
                                     Iv,
                                     DecryptData
                                     );

    if (!CryptoSuccess) {
      Status = EFI_SECURITY_VIOLATION;
      goto DecryptError;
    }

    CopyMem (PasswordStr, DecryptData, HDD_PASSWORD_ENCRYPT_SIZE);
    
    ZeroMem (RootKey, HDD_PASSWORD_ROOT_KEY_SIZE);
    ZeroMem (RandomNum, HDD_PASSWORD_RANDOM_NUM_SIZE);
    ZeroMem (InputData, HDD_PASSWORD_ENCRYPT_SIZE);
    ZeroMem (Iv, HDD_PASSWORD_IV_SIZE);
    ZeroMem (SecureKey, HDD_PASSWORD_SECURITY_KEY_SIZE);

  DecryptError:
    FreePool (InputData);
    FreePool (RootKey);
    FreePool (RandomNum);
    FreePool (Iv);
    FreePool (SecureKey);
    FreePool (DecryptData);
    FreePool (AesCtx);

    break;
  }

  return Status;
}

/**
  Main entry point for a SMM handler dispatch or communicate-based callback.

  @param[in]     DispatchHandle  The unique handle assigned to this handler by SmiHandlerRegister().
  @param[in]     Context         Points to an optional handler context which was specified when the
                                 handler was registered.
  @param[in,out] CommBuffer      A pointer to a collection of data in memory that will
                                 be conveyed from a non-SMM environment into an SMM environment.
  @param[in,out] CommBufferSize  The size of the CommBuffer.

  @retval EFI_SUCCESS            The interrupt was handled and quiesced. No other handlers
                                 should still be called.
**/
EFI_STATUS
EFIAPI
S3SleepEntryCallBack (
  IN           EFI_HANDLE           DispatchHandle,
  IN     CONST VOID                 *Context         OPTIONAL,
  IN OUT       VOID                 *CommBuffer      OPTIONAL,
  IN OUT       UINTN                *CommBufferSize  OPTIONAL
  )
{
  mBootOnS3Resume = TRUE;
  return EFI_SUCCESS;
}

/**
  S3 Sleep Exit event notification handler.

  @param[in] Protocol   Points to the protocol's unique identifier.
  @param[in] Interface  Points to the interface instance.
  @param[in] Handle     The handle on which the interface was installed.

  @retval EFI_SUCCESS   Notification handler runs successfully.
 **/
EFI_STATUS
EFIAPI
S3SleepExitEventNotify(
  IN CONST EFI_GUID  *Protocol,
  IN VOID            *Interface,
  IN EFI_HANDLE      Handle
  )
{
  mBootOnS3Resume = FALSE;
  return EFI_SUCCESS;
}

/**
  Notification function for Crypto Service Protocol.

  @param  Event                 Event whose notification function is being invoked.
  @param  Context               The pointer to the notification function's context,
                                which is implementation-dependent.

  @retval VOID

**/
VOID
EFIAPI
CryptoNotify (
  IN EFI_EVENT   Event,
  IN VOID        *Context
  )
{
  EFI_STATUS                        Status;

  Status = gBS->LocateProtocol (
                  &gCryptoServicesProtocolGuid,
                  NULL,
                  (VOID **) &CryptoService
                  );
  if (!EFI_ERROR (Status)) {
    gBS->CloseEvent (Event);
  }
}

/**
  Constructor for SavedPasswordStringProcess library.
  This is used to register S3 callback to check whether in S3 resume or not.

  @param ImageHandle     A handle for the image that is initializing this driver
  @param SystemTable     A pointer to the EFI system table

  @retval EFI_SUCEESS
  @return Others          Some error occurs.

**/
EFI_STATUS
EFIAPI
SavedPasswordStringProcessLibConstructor (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS                     Status;
  VOID                           *mHddPasswordS3ExitRegistration;
  EFI_SMM_SX_DISPATCH2_PROTOCOL  *SxDispatch;
  EFI_SMM_SX_REGISTER_CONTEXT    EntryRegisterContext;
  EFI_HANDLE                     S3EntryHandle;
  VOID                           *Registration;
  
  mHddPasswordS3ExitRegistration = NULL;

  if (InSmm ()) {
    Status = gSmst->SmmLocateProtocol (
                        &gEfiSmmSxDispatch2ProtocolGuid,
                        NULL,
                        (VOID **)&SxDispatch
                        );
    if (!EFI_ERROR (Status) && (SxDispatch != NULL)) {
        //
        // Register a S3 entry callback function to
        // determine if it will be during S3 resume.
        //
        EntryRegisterContext.Type  = SxS3;
        EntryRegisterContext.Phase = SxEntry;
        Status = SxDispatch->Register (
                            SxDispatch,
                            S3SleepEntryCallBack,
                            &EntryRegisterContext,
                            &S3EntryHandle
                            );
    }
    if (mHddPasswordS3ExitRegistration == NULL) {
        Status = gSmst->SmmRegisterProtocolNotify (
                        &gAcpiRestoreCallbackDoneProtocolGuid,
                        (EFI_SMM_NOTIFY_FN)S3SleepExitEventNotify,
                        &mHddPasswordS3ExitRegistration
                        );
    }
  } else {
    EfiCreateProtocolNotifyEvent (
      &gCryptoServicesProtocolGuid,
      TPL_CALLBACK,
      CryptoNotify,
      NULL,
      (VOID **)&Registration
      );
  }
  return EFI_SUCCESS;
}

/**
  Saved Password String Process.

  @param  ProcessType                       To do encrypt or decrypt.
  @param  HddPasswordTable                  Pointer to HddPasswordTable.
  @param  HddPasswordTableSize              Size of HddPasswordTable.

  @retval EFI_SUCCESS                       Function has completed successfully.
  @retval Others                            An unexpected error occurred.

**/
EFI_STATUS
SavedPasswordStringProcess (
  IN      UINT8                                 ProcessType,
  IN OUT  HDD_PASSWORD_TABLE                    *HddPasswordTable,
  IN      UINTN                                 HddPasswordTableSize
  )
{
  EFI_STATUS                                Status;
  UINTN                                     NumOfTable;
  HDD_PASSWORD_TABLE                        *HddPasswordTablePtr;
  UINTN                                     HddPasswordTableIndex;
  TCG_OPAL_EXT_DATA                         *TcgExtDataPtr;
  UINT32                                    NvIndex;
  UINT8                                     DummyData;
  UINTN                                     RestoreTableSize;

  Status                = EFI_SUCCESS;
  NumOfTable            = 0;
  HddPasswordTablePtr   = NULL;
  HddPasswordTableIndex = 0;
  RestoreTableSize      = 0;
  TcgExtDataPtr         = NULL;
  NvIndex               = PcdGet32 (PcdH2OHddPasswordTpmBaseNvIndex);
  
  if (!PcdGetBool (PcdH2OHddPasswordEncryptedUsingTpm)) {
    return EFI_UNSUPPORTED;
  }

  if (ProcessType == EncryptProcess) {
    //
    // Save HddPasswordTable in Lockbox for S3 resume autounlock
    //
    Status = SaveLockBox (&gSaveHddPasswordGuid, (VOID *)HddPasswordTable, HddPasswordTableSize);
  }
  
  if (ProcessType == DecryptProcess && mBootOnS3Resume == TRUE) {
    Status = RestoreLockBox (&gSaveHddPasswordGuid, &DummyData, &RestoreTableSize);
    if (Status != EFI_BUFFER_TOO_SMALL) {
      return EFI_INVALID_PARAMETER;
    } else {
      if (RestoreTableSize != HddPasswordTableSize) {
        return EFI_SECURITY_VIOLATION;
      }
      HddPasswordTablePtr = AllocatePool (RestoreTableSize);
      if (HddPasswordTablePtr == NULL) {
        return EFI_OUT_OF_RESOURCES;
      }

      Status = RestoreLockBox (&gSaveHddPasswordGuid, (VOID *)HddPasswordTablePtr, &RestoreTableSize);
      if (EFI_ERROR (Status)) {
        FreePool (HddPasswordTablePtr);
        return Status;
      }
      *HddPasswordTable = *HddPasswordTablePtr;
    }
    return EFI_SUCCESS;
  }

  if (InSmm ()) {
    Status = gSmst->SmmLocateProtocol (
                       &gCryptoServicesProtocolGuid,
                       NULL,
                       (VOID **) &CryptoService
                       );
  }

  if (CryptoService == NULL) {
    return EFI_UNSUPPORTED;
  }

  NumOfTable = NumOfHddPasswordTable (HddPasswordTable, HddPasswordTableSize);
  HddPasswordTablePtr = (HDD_PASSWORD_TABLE *) HddPasswordTable;

  for (HddPasswordTableIndex = 0; HddPasswordTableIndex < NumOfTable;
        HddPasswordTableIndex++, NvIndex++, HddPasswordTablePtr = GetNextTableEntry (HddPasswordTablePtr)) {
    
    if (HddPasswordTablePtr->PasswordStr[0] == 0) {
      continue;
    }

    Status = AesCbcFunc (ProcessType, HddPasswordTablePtr->PasswordStr, &HddPasswordTablePtr->TpmNvIndex, NvIndex, FALSE);

    //
    // TCG Opal Ext Data
    //
    TcgExtDataPtr = (TCG_OPAL_EXT_DATA *) (HddPasswordTablePtr + 1);

    if (TcgExtDataPtr->Signature == TCG_OPAL_EXT_DATA_SIGNATURE) {

      if (TcgExtDataPtr->SIDPasswordStr[0] != 0) {
        AesCbcFunc (ProcessType, TcgExtDataPtr->SIDPasswordStr, &HddPasswordTablePtr->TpmNvIndex, NvIndex, TRUE);
      }

      if (TcgExtDataPtr->Admin1PasswordStr[0] != 0) {
        AesCbcFunc (ProcessType, TcgExtDataPtr->Admin1PasswordStr, &HddPasswordTablePtr->TpmNvIndex, NvIndex, TRUE);
      }

      if (TcgExtDataPtr->User1PasswordStr[0] != 0) {
        AesCbcFunc (ProcessType, TcgExtDataPtr->User1PasswordStr, &HddPasswordTablePtr->TpmNvIndex, NvIndex, TRUE);
      }

      if (TcgExtDataPtr->User2PasswordStr[0] != 0) {
        AesCbcFunc (ProcessType, TcgExtDataPtr->User2PasswordStr, &HddPasswordTablePtr->TpmNvIndex, NvIndex, TRUE);
      }
    }
  }

  if (GetBootModeHob () == BOOT_ON_S4_RESUME) {
    Status = SaveLockBox (&gSaveHddPasswordGuid, (VOID *)HddPasswordTable, HddPasswordTableSize);
  }

  return Status;
}
