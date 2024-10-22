/** @file

;******************************************************************************
;* Copyright (c) 2015, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

/*++ 
  Module Name:
  
    OEMBadgingString.c
  
  Abstract:
  
    OEM can define badging string in this file. 
    
--*/
#include <OEMBadgingString.h>
#include <Protocol/SetupUtility.h>
#include <Library/HiiLib.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/PrintLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>

//
// OEM can modify the background and foreground color of the OEM dadging string through through the below data
// for example:
//   { 50, 280, { 0x00, 0x00, 0x00, 0x00 }, { 0xFF, 0xFF, 0xFF, 0x00 }, STRING_TOKEN ( STR_OEM_BADGING_STR_CPUID ), GetCpuId },
//     { 0x00, 0x00, 0x00, 0x00 } indicate the foreground color { Blue, Green, Red, Reserved }
//     { 0xFF, 0xFF, 0xFF, 0x00 } indicate the background color { Blue, Green, Red, Reserved }
//


//
// Dear all engineer:
//   The following badging string are re-organized to make sure every string has its own function and removed unnecessary string array.
//
OEM_BADGING_STRING *mOemBadgingString = NULL;
OEM_BADGING_STRING mOemBadgingStringInTextMode[] = {
  {       OEM_STRING_LOCATION_X,       OEM_STRING1_LOCATION_Y, { 0xFF, 0xFF, 0xFF, 0x00 }, { 0x00, 0x00, 0x00, 0x00 }, STRING_TOKEN ( STR_OEM_BADGING_STR_CPUID ), GetCpuId },
  {       OEM_STRING_LOCATION_X,       OEM_STRING2_LOCATION_Y, { 0xFF, 0xFF, 0xFF, 0x00 }, { 0x00, 0x00, 0x00, 0x00 }, STRING_TOKEN ( STR_BLANK_STRING   ), NULL}
};

//
// Note: if mOemBadgingString is [1][b] array, then mOemBadgingStringAfterSelect should be [b][b] array
// For example: if mOemBadgingString is [1][5], mOemBadgingStringAfterSelect should be [5][5]
//
OEM_BADGING_STRING *mOemBadgingStringAfterSelect = NULL;



OEM_BADGING_STRING mOemBadgingStringAfterSelectInTextMode[BADGING_AFTER_SELECT_COUNT][BADGING_STRINGS_COUNT_TEXT_MODE] = {
 {
  {       OEM_STRING_LOCATION_X,       OEM_STRING1_LOCATION_Y, { 0xFF, 0xFF, 0xFF, 0x00 }, { 0x00, 0x00, 0x00, 0x00 }, STRING_TOKEN ( STR_OEM_BADGING_STR_CPUID ), GetCpuId },
  {       OEM_STRING_LOCATION_X,       OEM_STRING2_LOCATION_Y, { 0xFF, 0xFF, 0xFF, 0x00 }, { 0x00, 0x00, 0x00, 0x00 }, STRING_TOKEN ( STR_OEM_BADGING_STR_ESC_SELECT), NULL}
 },
 {
  {       OEM_STRING_LOCATION_X,       OEM_STRING1_LOCATION_Y, { 0xFF, 0xFF, 0xFF, 0x00 }, { 0x00, 0x00, 0x00, 0x00 }, STRING_TOKEN ( STR_OEM_BADGING_STR_CPUID  ), GetCpuId },
  {       OEM_STRING_LOCATION_X,       OEM_STRING2_LOCATION_Y, { 0xFF, 0xFF, 0xFF, 0x00 }, { 0x00, 0x00, 0x00, 0x00 }, STRING_TOKEN ( STR_OEM_BADGING_STR_BOOT_OS), NULL}
 }
};

EFI_STATUS
GetId(
  IN OUT UINTN    *CpuId
)
{
  UINT32 RegEax;
  AsmCpuid (0x01, &RegEax, NULL, NULL, NULL);

  *CpuId = RegEax;

  return EFI_SUCCESS;
}

BOOLEAN
GetCpuId(
  IN OEM_BADGING_STRING  *Structure,
  OUT CHAR16             **StringData
)
{
  CHAR16                                *TokenStr;
  UINTN                                 TokenStrLen;
  UINTN                                 CpuId;
  EFI_STATUS                            Status;

  Status = GetId (&CpuId);
  if (EFI_ERROR (Status)) {
    return FALSE;
  }

  TokenStr = HiiGetPackageString (&gEfiCallerIdGuid, Structure->StringToken, NULL);
  ASSERT (TokenStr != NULL);
  if (TokenStr == NULL){
    return FALSE;
  }

  TokenStrLen = StrLen (TokenStr);
  *StringData = ReallocatePool (
                  (TokenStrLen + 1) * sizeof (CHAR16),
                  (TokenStrLen + 1) * sizeof (CHAR16) + 100,
                  TokenStr
                  );
  ASSERT (*StringData != NULL);
  if (*StringData == NULL) {
    FreePool (TokenStr);
    return FALSE;
  }

  UnicodeValueToStringS (&((*StringData)[TokenStrLen]), 100, RADIX_HEX, CpuId, 0);
  return TRUE;
}

