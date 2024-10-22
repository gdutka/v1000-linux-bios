/** @file
  This library is BaseCrypto router. It will redirect hash request to each individual
  hash handler registerd, such as SHA1, SHA256.
  Platform can use PcdTpm2HashMask to mask some hash engines.

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
/** @file
  Ihis library is BaseCrypto router. It will redirect hash request to each individual
  hash handler registerd, such as SHA1, SHA256.
  Platform can use PcdTpm2HashMask to mask some hash engines.

Copyright (c) 2013 - 2015, Intel Corporation. All rights reserved. <BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <PiPei.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/Tpm2CommandLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PcdLib.h>
#include <Library/HobLib.h>
#include <Library/HashLib.h>
#include <Library/H2OFvHashLib.h>
#include <Guid/ZeroGuid.h>

#include "HashLibBaseCryptoRouterCommon.h"

#define HASH_LIB_PEI_ROUTER_GUID \
  { 0x84681c08, 0x6873, 0x46f3, { 0x8b, 0xb7, 0xab, 0x66, 0x18, 0x95, 0xa1, 0xb3 } }

EFI_GUID mHashLibPeiRouterGuid = HASH_LIB_PEI_ROUTER_GUID;

typedef struct {
  //
  // If gZeroGuid, SupportedHashMask is 0 for FIRST module which consumes HashLib
  //   or the hash algorithm bitmap of LAST module which consumes HashLib.
  //   HashInterfaceCount and HashInterface are all 0.
  // If gEfiCallerIdGuid, HashInterfaceCount, HashInterface and SupportedHashMask
  //   are the hash interface information of CURRENT module which consumes HashLib.
  //
  EFI_GUID         Identifier;
  UINTN            HashInterfaceCount;
  HASH_INTERFACE   HashInterface[HASH_COUNT];
  UINT32           SupportedHashMask;
  UINT32           BiosSupportedHashMask;
} HASH_INTERFACE_HOB;

/**
  This function gets hash interface hob.

  @param Identifier    Identifier to get hash interface hob.

  @retval hash interface hob.
**/
HASH_INTERFACE_HOB *
InternalGetHashInterfaceHob (
  EFI_GUID      *Identifier
  )
{
  EFI_PEI_HOB_POINTERS  Hob;
  HASH_INTERFACE_HOB    *HashInterfaceHob;

  Hob.Raw = GetFirstGuidHob (&mHashLibPeiRouterGuid);
  while (Hob.Raw != NULL) {
    HashInterfaceHob = GET_GUID_HOB_DATA (Hob);
    if (CompareGuid (&HashInterfaceHob->Identifier, Identifier)) {
      //
      // Found the matched one.
      //
      return HashInterfaceHob;
    }
    Hob.Raw = GET_NEXT_HOB (Hob);
    Hob.Raw = GetNextGuidHob (&mHashLibPeiRouterGuid, Hob.Raw);
  }
  return NULL;
}

/**
  This function creates hash interface hob.

  @param Identifier    Identifier to create hash interface hob.

  @retval hash interface hob.
**/
HASH_INTERFACE_HOB *
InternalCreateHashInterfaceHob (
  EFI_GUID      *Identifier
  )
{
  HASH_INTERFACE_HOB LocalHashInterfaceHob;

  ZeroMem (&LocalHashInterfaceHob, sizeof(LocalHashInterfaceHob));
  CopyGuid (&LocalHashInterfaceHob.Identifier, Identifier);
  return BuildGuidDataHob (&mHashLibPeiRouterGuid, &LocalHashInterfaceHob, sizeof(LocalHashInterfaceHob));
}

/**
  Check mismatch of supported HashMask between modules
  that may link different HashInstanceLib instances.

  @param HashInterfaceHobCurrent    Pointer to hash interface hob for CURRENT module.

**/
VOID
CheckSupportedHashMaskMismatch (
  IN HASH_INTERFACE_HOB *HashInterfaceHobCurrent
  )
{
  HASH_INTERFACE_HOB    *HashInterfaceHobLast;

  HashInterfaceHobLast = InternalGetHashInterfaceHob (&gZeroGuid);
  ASSERT (HashInterfaceHobLast != NULL);

  if ((HashInterfaceHobLast->SupportedHashMask != 0) &&
      (HashInterfaceHobCurrent->SupportedHashMask != HashInterfaceHobLast->SupportedHashMask)) {
    DEBUG ((
      DEBUG_WARN,
      "WARNING: There is mismatch of supported HashMask (0x%x - 0x%x) between modules\n",
      HashInterfaceHobCurrent->SupportedHashMask,
      HashInterfaceHobLast->SupportedHashMask
      ));
    DEBUG ((DEBUG_WARN, "that are linking different HashInstanceLib instances!\n"));
  }

  if ((HashInterfaceHobLast->BiosSupportedHashMask != 0) &&
      (HashInterfaceHobCurrent->BiosSupportedHashMask != HashInterfaceHobLast->BiosSupportedHashMask)) {
    DEBUG ((
      DEBUG_WARN,
      "WARNING: There is mismatch of linked HashInstanceLib (0x%x - 0x%x) between modules\n",
      HashInterfaceHobCurrent->BiosSupportedHashMask,
      HashInterfaceHobLast->BiosSupportedHashMask
      ));
    DEBUG ((DEBUG_WARN, "that are linking different HashInstanceLib instances!\n"));
  }
}

/**
  The function get the hashing from H2OFvHashLib and set to digest list.

  @param[in]  HashType           The hash type.
  @param[in]  DataToHash         The data to hash.
  @param[in]  DataToHashLen      The data size.
  @param[out] Digest             Pointer to digest buffer.

  @retval EFI_SUCCESS            Set digest buffer successfully.
  @retval EFI_NOT_FOUND          Can't find the hashing in database.
  @retval EFI_INVALID_PARAMETER  Input HashType is invalid or Buffer is NULL.
**/
EFI_STATUS
SetH2OFvHashToDigest (
  IN     UINT32             HashType,
  IN     VOID               *DataToHash,
  IN     UINTN              DataToHashLen,
     OUT TPML_DIGEST_VALUES *Digest
  )
{
  UINT8              *HashBuffer;

  if (Digest == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  HashBuffer = NULL;
  HashBuffer = H2OGetFvHash (HashType, (UINTN)DataToHash, DataToHashLen);

  if (HashBuffer == NULL) {
    return EFI_NOT_FOUND;
  }

  DEBUG ((
    DEBUG_INFO,
    "SetH2OFvHashToDigest- FvHash HOB found! (HashType:0x%x, FvBase:0x%x, FvLength:0x%x)\n",
    HashType,
    (UINTN)DataToHash,
    DataToHashLen
    ));

  switch (HashType) {
  case H2O_SHA1_FV_HASH:
    Digest->count = 1;
    Digest->digests[0].hashAlg = TPM_ALG_SHA1;
    CopyMem (Digest->digests[0].digest.sha1, HashBuffer, SHA1_DIGEST_SIZE);
    break;

  case H2O_SHA256_FV_HASH:
    Digest->count = 1;
    Digest->digests[0].hashAlg = TPM_ALG_SHA256;
    CopyMem (Digest->digests[0].digest.sha256, HashBuffer, SHA256_DIGEST_SIZE);
    break;

  case H2O_SHA384_FV_HASH:
    Digest->count = 1;
    Digest->digests[0].hashAlg = TPM_ALG_SHA384;
    CopyMem (Digest->digests[0].digest.sha384, HashBuffer, SHA384_DIGEST_SIZE);
    break;

  case H2O_SHA512_FV_HASH:
    Digest->count = 1;
    Digest->digests[0].hashAlg = TPM_ALG_SHA512;
    CopyMem (Digest->digests[0].digest.sha512, HashBuffer, SHA512_DIGEST_SIZE);
    break;

  case H2O_SM3_256_FV_HASH:
    Digest->count = 1;
    Digest->digests[0].hashAlg = TPM_ALG_SM3_256;
    CopyMem (Digest->digests[0].digest.sm3_256, HashBuffer, SM3_256_DIGEST_SIZE);
    break;

  default:
    return EFI_INVALID_PARAMETER;
  }

  return EFI_SUCCESS;
}

/**
  Hash sequence complete and extend to PCR.

  @param[in]  PcrIndex      PCR to be extended.
  @param[in]  DataToHash    Data to be hashed.
  @param[in]  DataToHashLen Data size.
  @param[out] DigestList    Digest list.

  @retval EFI_SUCCESS     Hash sequence complete and DigestList is returned.
  @retval Others          The PCR extending is failed.
**/
EFI_STATUS
H2OHashAndExtend (
  IN     TPMI_DH_PCR         PcrIndex,
  IN     VOID                *DataToHash,
  IN     UINTN               DataToHashLen,
     OUT TPML_DIGEST_VALUES  *DigestList
  )
{
  EFI_STATUS         Status;
  TPML_DIGEST_VALUES Digest;
  HASH_INTERFACE_HOB *HashInterfaceHob;
  HASH_HANDLE        *HashCtx;
  UINTN              Index;
  UINT32             HashMask;
  UINT32             HashType;

  HashInterfaceHob = InternalGetHashInterfaceHob (&gEfiCallerIdGuid);
  if (HashInterfaceHob == NULL) {
    return EFI_UNSUPPORTED;
  }

  if (HashInterfaceHob->HashInterfaceCount == 0) {
    return EFI_UNSUPPORTED;
  }

  CheckSupportedHashMaskMismatch (HashInterfaceHob);

  HashCtx = AllocateZeroPool (sizeof(*HashCtx) * HashInterfaceHob->HashInterfaceCount);
  ASSERT (HashCtx != NULL);

  ZeroMem (DigestList, sizeof(*DigestList));

  for (Index = 0; Index < HashInterfaceHob->HashInterfaceCount; Index++) {
    HashMask = Tpm2GetHashMaskFromAlgo (&HashInterfaceHob->HashInterface[Index].HashGuid);
    HashType = Tpm2GetH2OHashTypeFromAlgo (&HashInterfaceHob->HashInterface[Index].HashGuid);
    if ((HashMask & PcdGet32 (PcdTpm2HashMask)) != 0) {
      Status = SetH2OFvHashToDigest (HashType, DataToHash, DataToHashLen, &Digest);
      if (EFI_ERROR (Status)) {
        HashInterfaceHob->HashInterface[Index].HashInit (&HashCtx[Index]);
        HashInterfaceHob->HashInterface[Index].HashUpdate (HashCtx[Index], DataToHash, DataToHashLen);
        HashInterfaceHob->HashInterface[Index].HashFinal (HashCtx[Index], &Digest);
        H2OSetFvHash (HashType, (UINTN)DataToHash, DataToHashLen, (VOID *)&Digest.digests[0].digest);
      }
      Tpm2SetHashToDigestList (DigestList, &Digest);
    }
  }

  FreePool (HashCtx);

  Status = Tpm2PcrExtend (
             PcrIndex,
             DigestList
             );
  return Status;
}

/**
  Start hash sequence.

  @param HashHandle Hash handle.

  @retval EFI_SUCCESS          Hash sequence start and HandleHandle returned.
  @retval EFI_OUT_OF_RESOURCES No enough resource to start hash.
**/
EFI_STATUS
EFIAPI
HashStart (
  OUT HASH_HANDLE    *HashHandle
  )
{
  HASH_INTERFACE_HOB *HashInterfaceHob;
  HASH_HANDLE        *HashCtx;
  UINTN              Index;
  UINT32             HashMask;

  HashInterfaceHob = InternalGetHashInterfaceHob (&gEfiCallerIdGuid);
  if (HashInterfaceHob == NULL) {
    return EFI_UNSUPPORTED;
  }

  if (HashInterfaceHob->HashInterfaceCount == 0) {
    return EFI_UNSUPPORTED;
  }

  CheckSupportedHashMaskMismatch (HashInterfaceHob);

  HashCtx = AllocatePool (sizeof(*HashCtx) * HashInterfaceHob->HashInterfaceCount);
  ASSERT (HashCtx != NULL);
  if (HashCtx == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  for (Index = 0; Index < HashInterfaceHob->HashInterfaceCount; Index++) {
    HashMask = Tpm2GetHashMaskFromAlgo (&HashInterfaceHob->HashInterface[Index].HashGuid);
    if ((HashMask & PcdGet32 (PcdTpm2HashMask)) != 0) {
      HashInterfaceHob->HashInterface[Index].HashInit (&HashCtx[Index]);
    }
  }

  *HashHandle = (HASH_HANDLE)HashCtx;

  return EFI_SUCCESS;
}

/**
  Update hash sequence data.

  @param HashHandle    Hash handle.
  @param DataToHash    Data to be hashed.
  @param DataToHashLen Data size.

  @retval EFI_SUCCESS     Hash sequence updated.
**/
EFI_STATUS
EFIAPI
HashUpdate (
  IN HASH_HANDLE    HashHandle,
  IN VOID           *DataToHash,
  IN UINTN          DataToHashLen
  )
{
  HASH_INTERFACE_HOB *HashInterfaceHob;
  HASH_HANDLE        *HashCtx;
  UINTN              Index;
  UINT32             HashMask;

  HashInterfaceHob = InternalGetHashInterfaceHob (&gEfiCallerIdGuid);
  if (HashInterfaceHob == NULL) {
    return EFI_UNSUPPORTED;
  }

  if (HashInterfaceHob->HashInterfaceCount == 0) {
    return EFI_UNSUPPORTED;
  }

  CheckSupportedHashMaskMismatch (HashInterfaceHob);

  HashCtx = (HASH_HANDLE *)HashHandle;

  for (Index = 0; Index < HashInterfaceHob->HashInterfaceCount; Index++) {
    HashMask = Tpm2GetHashMaskFromAlgo (&HashInterfaceHob->HashInterface[Index].HashGuid);
    if ((HashMask & PcdGet32 (PcdTpm2HashMask)) != 0) {
      HashInterfaceHob->HashInterface[Index].HashUpdate (HashCtx[Index], DataToHash, DataToHashLen);
    }
  }

  return EFI_SUCCESS;
}

/**
  Hash sequence complete and extend to PCR.

  @param HashHandle    Hash handle.
  @param PcrIndex      PCR to be extended.
  @param DataToHash    Data to be hashed.
  @param DataToHashLen Data size.
  @param DigestList    Digest list.

  @retval EFI_SUCCESS     Hash sequence complete and DigestList is returned.
**/
EFI_STATUS
EFIAPI
HashCompleteAndExtend (
  IN HASH_HANDLE         HashHandle,
  IN TPMI_DH_PCR         PcrIndex,
  IN VOID                *DataToHash,
  IN UINTN               DataToHashLen,
  OUT TPML_DIGEST_VALUES *DigestList
  )
{
  TPML_DIGEST_VALUES Digest;
  HASH_INTERFACE_HOB *HashInterfaceHob;
  HASH_HANDLE        *HashCtx;
  UINTN              Index;
  EFI_STATUS         Status;
  UINT32             HashMask;

  HashInterfaceHob = InternalGetHashInterfaceHob (&gEfiCallerIdGuid);
  if (HashInterfaceHob == NULL) {
    return EFI_UNSUPPORTED;
  }

  if (HashInterfaceHob->HashInterfaceCount == 0) {
    return EFI_UNSUPPORTED;
  }

  CheckSupportedHashMaskMismatch (HashInterfaceHob);

  HashCtx = (HASH_HANDLE *)HashHandle;
  ZeroMem (DigestList, sizeof(*DigestList));

  for (Index = 0; Index < HashInterfaceHob->HashInterfaceCount; Index++) {
    HashMask = Tpm2GetHashMaskFromAlgo (&HashInterfaceHob->HashInterface[Index].HashGuid);
    if ((HashMask & PcdGet32 (PcdTpm2HashMask)) != 0) {
      HashInterfaceHob->HashInterface[Index].HashUpdate (HashCtx[Index], DataToHash, DataToHashLen);
      HashInterfaceHob->HashInterface[Index].HashFinal (HashCtx[Index], &Digest);
      Tpm2SetHashToDigestList (DigestList, &Digest);
    }
  }

  FreePool (HashCtx);

  Status = Tpm2PcrExtend (
             PcrIndex,
             DigestList
             );
  return Status;
}

/**
  Hash data and extend to PCR.

  @param PcrIndex      PCR to be extended.
  @param DataToHash    Data to be hashed.
  @param DataToHashLen Data size.
  @param DigestList    Digest list.

  @retval EFI_SUCCESS     Hash data and DigestList is returned.
**/
EFI_STATUS
EFIAPI
HashAndExtend (
  IN TPMI_DH_PCR                    PcrIndex,
  IN VOID                           *DataToHash,
  IN UINTN                          DataToHashLen,
  OUT TPML_DIGEST_VALUES            *DigestList
  )
{
  return H2OHashAndExtend (PcrIndex, DataToHash, DataToHashLen, DigestList);
}

/**
  This service register Hash.

  @param HashInterface  Hash interface

  @retval EFI_SUCCESS          This hash interface is registered successfully.
  @retval EFI_UNSUPPORTED      System does not support register this interface.
  @retval EFI_ALREADY_STARTED  System already register this interface.
**/
EFI_STATUS
EFIAPI
RegisterHashInterfaceLib (
  IN HASH_INTERFACE   *HashInterface
  )
{
  EFI_STATUS         Status;
  UINTN              Index;
  HASH_INTERFACE_HOB *HashInterfaceHob;
  UINT32             HashMask;
  UINT32             Tpm2HashMask;

  HashMask = Tpm2GetHashMaskFromAlgo (&HashInterface->HashGuid);

  HashInterfaceHob = InternalGetHashInterfaceHob (&gEfiCallerIdGuid);
  if (HashInterfaceHob == NULL) {
    HashInterfaceHob = InternalCreateHashInterfaceHob (&gEfiCallerIdGuid);
    if (HashInterfaceHob == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
  }

  //
  // Record linked HashInstanceLib bitmap of CURRENT module which consumes HashLib.
  //
  HashInterfaceHob->BiosSupportedHashMask = PcdGet32 (PcdH2OTpm2BiosSupportedHashAlgorithmBitmap) | HashMask;
  Status = PcdSet32S (PcdH2OTpm2BiosSupportedHashAlgorithmBitmap, HashInterfaceHob->BiosSupportedHashMask);
  ASSERT_EFI_ERROR (Status);

  //
  // Check allow
  //
  Tpm2HashMask = PcdGet32 (PcdTpm2HashMask);

  if ((Tpm2HashMask != 0) &&
      ((HashMask & Tpm2HashMask) == 0))
  {
    return EFI_UNSUPPORTED;
  }

  if (HashInterfaceHob->HashInterfaceCount >= HASH_COUNT) {
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // Check duplication
  //
  for (Index = 0; Index < HashInterfaceHob->HashInterfaceCount; Index++) {
    if (CompareGuid (&HashInterfaceHob->HashInterface[Index].HashGuid, &HashInterface->HashGuid)) {
      DEBUG ((DEBUG_ERROR, "Hash Interface (%g) has been registered\n", &HashInterface->HashGuid));
      return EFI_ALREADY_STARTED;
    }
  }

  //
  // Record hash algorithm bitmap of CURRENT module which consumes HashLib.
  //
  HashInterfaceHob->SupportedHashMask = PcdGet32 (PcdTcg2HashAlgorithmBitmap) | HashMask;
  Status = PcdSet32S (PcdTcg2HashAlgorithmBitmap, HashInterfaceHob->SupportedHashMask);
  ASSERT_EFI_ERROR (Status);

  CopyMem (&HashInterfaceHob->HashInterface[HashInterfaceHob->HashInterfaceCount], HashInterface, sizeof(*HashInterface));
  HashInterfaceHob->HashInterfaceCount ++;
  
  return EFI_SUCCESS;
}

/**
  The constructor function of HashLibBaseCryptoRouterPei.

  @param  FileHandle   The handle of FFS header the loaded driver.
  @param  PeiServices  The pointer to the PEI services.

  @retval EFI_SUCCESS           The constructor executes successfully.
  @retval EFI_OUT_OF_RESOURCES  There is no enough resource for the constructor.

**/
EFI_STATUS
EFIAPI
HashLibBaseCryptoRouterPeiConstructor (
  IN EFI_PEI_FILE_HANDLE        FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS            Status;
  HASH_INTERFACE_HOB    *HashInterfaceHob;

  HashInterfaceHob = InternalGetHashInterfaceHob (&gZeroGuid);
  if (HashInterfaceHob == NULL) {
    //
    // No HOB with gZeroGuid Identifier has been created,
    // this is FIRST module which consumes HashLib.
    // Create the HOB with gZeroGuid Identifier.
    //
    HashInterfaceHob = InternalCreateHashInterfaceHob (&gZeroGuid);
    if (HashInterfaceHob == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
  } else {
    //
    // Record hash algorithm bitmap of LAST module which also consumes HashLib.
    //
    HashInterfaceHob->SupportedHashMask = PcdGet32 (PcdTcg2HashAlgorithmBitmap);
    //
    // Record linked HashInstanceLib bitmap of LAST module which also consumes HashLib.
    //
    HashInterfaceHob->BiosSupportedHashMask = PcdGet32 (PcdH2OTpm2BiosSupportedHashAlgorithmBitmap);
  }

  HashInterfaceHob = InternalGetHashInterfaceHob (&gEfiCallerIdGuid);
  if (HashInterfaceHob != NULL) {
    //
    // In PEI phase, some modules may call RegisterForShadow and will be
    // shadowed and executed again after memory is discovered.
    // This is the second execution of this module, clear the hash interface
    // information registered at its first execution.
    //
    ZeroMem (&HashInterfaceHob->HashInterfaceCount, sizeof (*HashInterfaceHob) - sizeof (EFI_GUID));
  }

  //
  // Set PcdTcg2HashAlgorithmBitmap to 0 in CONSTRUCTOR for CURRENT module.
  //
  Status = PcdSet32S (PcdTcg2HashAlgorithmBitmap, 0);
  ASSERT_EFI_ERROR (Status);

  //
  // Set PcdTcg2HashAlgorithmBitmap to 0 in CONSTRUCTOR for CURRENT module.
  //
  Status = PcdSet32S (PcdH2OTpm2BiosSupportedHashAlgorithmBitmap, 0);
  ASSERT_EFI_ERROR (Status);

  return EFI_SUCCESS;
}
