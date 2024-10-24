/** @file
  This is BaseCrypto router support function definition.

;******************************************************************************
;* Copyright (c) 2017 - 2018, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/
/** @file
  Ihis is BaseCrypto router support function definition.

Copyright (c) 2013, Intel Corporation. All rights reserved. <BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _HASH_LIB_BASE_CRYPTO_ROUTER_COMMON_H_
#define _HASH_LIB_BASE_CRYPTO_ROUTER_COMMON_H_

/**
  The function get hash mask info from algorithm.

  @param HashGuid Hash Guid

  @return HashMask
**/
UINT32
EFIAPI
Tpm2GetHashMaskFromAlgo (
  IN EFI_GUID  *HashGuid
  );

/**
  The function set digest to digest list.

  @param DigestList digest list
  @param Digest     digest data
**/

/**
  The function get H2O hash type info from algorithm.

  @param[in] HashGuid   The hash Guid

  @return HashType
**/
UINT32
EFIAPI
Tpm2GetH2OHashTypeFromAlgo (
  IN EFI_GUID  *HashGuid
  );

VOID
EFIAPI
Tpm2SetHashToDigestList (
  IN OUT TPML_DIGEST_VALUES *DigestList,
  IN TPML_DIGEST_VALUES     *Digest
  );

#endif
