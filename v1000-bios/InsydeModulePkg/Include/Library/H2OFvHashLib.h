/** @file
  H2O FV Hash Library Defintion.
;******************************************************************************
;* Copyright (c) 2018, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _H2O_FV_HASH_LIB_H_
#define _H2O_FV_HASH_LIB_H_

#include <Uefi.h>

#define H2O_SHA1_FV_HASH         0x01
#define H2O_SHA224_FV_HASH       0x02
#define H2O_SHA256_FV_HASH       0x03
#define H2O_SHA384_FV_HASH       0x04
#define H2O_SHA512_FV_HASH       0x05
#define H2O_SM3_256_FV_HASH      0x06
#define H2O_FV_HASH_MAX          H2O_SM3_256_FV_HASH

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
  );

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
  );


#endif

