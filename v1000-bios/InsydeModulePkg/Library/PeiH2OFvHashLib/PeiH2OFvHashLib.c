/** @file
  PEI H2O FV Hash library. It provides functionalities to Set and Get FV Hash
  value.

;******************************************************************************
;* Copyright (c) 2018, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/
#include <Uefi.h>

#include <Library/H2OFvHashLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/HobLib.h>

#define H2O_FV_HASH_INFO_SIGNATURE          SIGNATURE_32 ('H', 'F', 'H', 'I')

typedef struct _INTERNAL_H2O_FV_HASH_INFO {
  UINT32                    Signature;
  UINT32                    HashType;
  EFI_PHYSICAL_ADDRESS      FvBase;
  UINT64                    FvLength;
  //
  //
  // UINT8                   HashBuffer[];
} INTERNAL_H2O_FV_HASH_INFO;

typedef struct _INTERNAL_H2O_FV_HASH_MAP {
  UINT32                    HashType;
  UINTN                     HashSize;
} INTERNAL_H2O_FV_HASH_MAP;

STATIC EFI_GUID          mInternalH2OPeiFvHashInfoGuid = {0x3e7780a, 0x3436, 0x4705, 0xa7, 0x47, 0xff, 0x7e, 0xbb, 0x72, 0x3f, 0xe0};

STATIC INTERNAL_H2O_FV_HASH_MAP mFvHashInfoMap [H2O_FV_HASH_MAX] = {
                                  { H2O_SHA1_FV_HASH,    0x14},
                                  { H2O_SHA224_FV_HASH,  0x1C},
                                  { H2O_SHA256_FV_HASH,  0x20},
                                  { H2O_SHA384_FV_HASH,  0x30},
                                  { H2O_SHA512_FV_HASH,  0x40},
                                  { H2O_SM3_256_FV_HASH, 0x20}
                                };

STATIC
UINTN
GetHashSize (
  IN   UINT32        HashType
  )
{
  UINTN         Index;

  for (Index = 0; Index < H2O_SHA512_FV_HASH; Index++) {
    if (mFvHashInfoMap[Index].HashType == HashType) {
      return mFvHashInfoMap[Index].HashSize;
    }
  }
  return 0;
}


/**
  This function sets FV hash value the database which is controlled by this library. User can use
  H2OGetFvHash () to get FV hash value.

  @param[in]  HashType           Hash type to be set.
  @param[in]  FvBase             Firmware volume base address.
  @param[in]  FvLength           Firmware volume size.
  @param[in]  Buffer             Pointer to hash buffer.

  @retval EFI_SUCCESS            Set hash buffer successfully.
  @retval EFI_INVALID_PARAMETER  Input HashType is invalid or Buffer is NULL.
  @retval EFI_ALREADY_STARTED    Can find FV hash in the database.
  @retval EFI_OUT_OF_RESOURCES   Unable allocate resource to save information.
**/
EFI_STATUS
H2OSetFvHash (
  IN        UINT32                    HashType,
  IN        EFI_PHYSICAL_ADDRESS      FvBase,
  IN        UINT64                    FvLength,
  IN CONST  VOID                      *Buffer
  )
{
  UINTN                       HashSize;
  INTERNAL_H2O_FV_HASH_INFO   *FvHashInfo;

  if (HashType > H2O_SHA512_FV_HASH || Buffer == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  if (H2OGetFvHash (HashType, FvBase, FvLength)) {
    return EFI_ALREADY_STARTED;
  }

  HashSize = GetHashSize (HashType);
  FvHashInfo = BuildGuidHob (&mInternalH2OPeiFvHashInfoGuid, sizeof (INTERNAL_H2O_FV_HASH_INFO) + HashSize);
  if (FvHashInfo == NULL) {
     return EFI_OUT_OF_RESOURCES;
  }
  FvHashInfo->Signature = H2O_FV_HASH_INFO_SIGNATURE;
  FvHashInfo->HashType  = HashType;
  FvHashInfo->FvBase    = FvBase;
  FvHashInfo->FvLength  = FvLength;
  CopyMem (FvHashInfo + 1, Buffer, HashSize);

  return EFI_SUCCESS;
}

/**
  This function get FV hash value

  @param[in]  HashType           Hash type to be set.
  @param[in]  FvBase             Firmware volume base address.
  @param[in]  FvLength           Firmware volume size.

  @return Pointer to the FV hash value or NULL if not found.
**/
VOID *
H2OGetFvHash (
  IN  UINT32                    HashType,
  IN  EFI_PHYSICAL_ADDRESS      FvBase,
  IN  UINT64                    FvLength
  )
{
  EFI_HOB_GUID_TYPE             *GuidHob;
  INTERNAL_H2O_FV_HASH_INFO     *FvHashInfo;

  for (GuidHob = GetFirstGuidHob (&mInternalH2OPeiFvHashInfoGuid);
       GuidHob != NULL;
       GuidHob = GetNextGuidHob (&mInternalH2OPeiFvHashInfoGuid, GET_NEXT_HOB (GuidHob))) {
     FvHashInfo = GET_GUID_HOB_DATA (GuidHob);
     if (FvHashInfo->HashType == HashType && FvHashInfo->FvBase == FvBase &&
         FvHashInfo->FvLength == FvLength) {
       return (VOID *) (FvHashInfo + 1);
     }
  }
  return NULL;
}
