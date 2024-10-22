/** @file
  GenericBdsLib

;******************************************************************************
;* Copyright (c) 2012 - 2017, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

/**
  String support

Copyright (c) 2010, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/
#include "String.h"

/**
  Get string by string id from HII Interface


  @param Id              String ID.

  @retval  CHAR16 *  String from ID.
  @retval  NULL      If error occurs.

**/
CHAR16 *
BdsLibGetStringById (
  IN  EFI_STRING_ID   Id
  )
{
  return HiiGetString (gBdsLibStringPackHandle, Id, NULL);
}

/**
  Get next language from language code list (with separator ';').

  If LangCode is NULL, then ASSERT.
  If Lang is NULL, then ASSERT.

  @param[in, out]  LangCode    On input: point to first language in the list.
                               On output: point to next language in the list, or NULL if no more language in the list.
  @param[out]      Lang        The first language in the list.
**/
VOID
EFIAPI
BdsLibGetNextLanguage (
  IN OUT CHAR8         **LangCode,
  OUT    CHAR8         *Lang
  )
{
  UINTN                Index;
  CHAR8               *StringPtr;

  ASSERT (LangCode != NULL);
  ASSERT (*LangCode != NULL);
  ASSERT (Lang != NULL);

  Index = 0;
  StringPtr = *LangCode;
  while (StringPtr[Index] != 0 && StringPtr[Index] != ';') {
    Index++;
  }

  CopyMem (Lang, StringPtr, Index);
  Lang[Index] = 0;

  if (StringPtr[Index] == ';') {
    Index++;
  }
  *LangCode = StringPtr + Index;
}

/**
  Performs a case-insensitive comparison of two Null-terminated strings.

  @param[in] Str1  A pointer to a Null-terminated string.
  @param[in] Str2  A pointer to a Null-terminated string.

  @retval 0        Str1 is equivalent to Str2.
  @return > 0      Str1 is lexically greater than Str2..
  @return < 0      Str1 is lexically less than Str2.
**/
INTN
BdsLibStriCmp (
  IN CHAR16                          *Str1,
  IN CHAR16                          *Str2
  )
{
  EFI_STATUS                         Status;
  EFI_UNICODE_COLLATION_PROTOCOL     *UnicodeCollation;

  Status = gBS->LocateProtocol (
                  &gEfiUnicodeCollation2ProtocolGuid,
                  NULL,
                  (VOID**) &UnicodeCollation
                  );
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    //
    // EFI_UNICODE_COLLATION2_PROTOCOL must be existence. If the protocol
    // doesn't exist, EFI DEBUG will be asserted and return -1 to indicate the two strings are different.
    //
    return -1;
  }

  return UnicodeCollation->StriColl (UnicodeCollation, Str1, Str2);
}

/**
  Update all BVDT data (which are from .fd file) into HiiDatabase.

  @param  HiiHandle     target EFI_HII_HANDLE instance

**/
VOID
UpdateBvdtToHii (
  IN EFI_HII_HANDLE     HiiHandle
  )
{
  UINTN                StrSize;
  CHAR16               Str[BVDT_MAX_STR_SIZE];
  EFI_STATUS           Status;

  StrSize = BVDT_MAX_STR_SIZE;
  Status = GetBvdtInfo ((BVDT_TYPE) BvdtBiosVer, &StrSize, Str);
  HiiSetString (HiiHandle, STRING_TOKEN(STR_MISC_BIOS_VERSION), Str, NULL);

  StrSize = BVDT_MAX_STR_SIZE;
  Status = GetBvdtInfo ((BVDT_TYPE) BvdtProductName, &StrSize, Str);
  HiiSetString (HiiHandle, STRING_TOKEN(STR_MISC_SYSTEM_PRODUCT_NAME), Str, NULL);

  StrSize = BVDT_MAX_STR_SIZE;
  Status = GetBvdtInfo ((BVDT_TYPE) BvdtCcbVer, &StrSize, Str);
  HiiSetString (HiiHandle, STRING_TOKEN(STR_CCB_VERSION), Str, NULL);
}


/**
  Acquire the string associated with the ProducerGuid and return it.

  @param  ProducerGuid    The Guid to search the HII database for
  @param  Token           The token value of the string to extract
  @param  String          The string that is extracted

  @retval EFI_SUCCESS   The function returns EFI_SUCCESS always.

**/
EFI_STATUS
GetProducerString (
  IN      EFI_GUID                  *ProducerGuid,
  IN      EFI_STRING_ID             Token,
  OUT     CHAR16                    **String
  )
{
  EFI_STRING      TmpString;

  TmpString = HiiGetPackageString (ProducerGuid, Token, NULL);
  if (TmpString == NULL) {
    *String = BdsLibGetStringById (STRING_TOKEN (STR_MISSING_STRING));
  } else {
    *String = TmpString;
  }

  return EFI_SUCCESS;
}
