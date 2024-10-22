/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Ccx SMBIOS common code
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Lib
 * @e \$Revision$   @e \$Date$
 *
 */
/*
 ******************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */


/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include <Library/BaseLib.h>
#include "Filecode.h"
#include "cpuRegisters.h"
#include "Library/CcxBaseX86Lib.h"

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
#define FILECODE LIBRARY_CCXSMBIOSVHLIB_CCXSMBIOSVHCOMMONLIB_FILECODE

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */


/*---------------------------------------------------------------------------------------
 *                           L O C A L    F U N C T I O N S
 *---------------------------------------------------------------------------------------
 */
/* -----------------------------------------------------------------------------*/
/**
 *
 *  GetNameString
 *
 *  Description:
 *    Get name string from MSR_C001_00[35:30]
 *
 *  Parameters:
 *    @param[in, out]    *String       Pointer to name string
 *    @param[in]         *StdHeader
 *
 */
VOID
GetNameString (
  IN OUT   CHAR8 *String,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT8  i;
  UINT8  StringIndex;
  UINT8  MsrIndex;
  UINT64 MsrData;

  StringIndex = 0;
  for (MsrIndex = 0; MsrIndex <= 5; MsrIndex++) {
    MsrData = AsmReadMsr64 ((MSR_CPUID_NAME_STRING0 + MsrIndex));
    for (i = 0; i < 8; i++) {
      String[StringIndex] = (CHAR8) RShiftU64 (MsrData, (8 * i));
      StringIndex++;
    }
  }
  String[StringIndex] = '\0';
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *  IsSourceStrContainTargetStr
 *
 *  Description:
 *    check if source string contains target string.
 *
 *  Parameters:
 *    @param[in, out]    *SourceStr      Pointer to source CHAR array
 *    @param[in, out]    *TargetStr      Pointer to target CHAR array
 *    @param[in]         *StdHeader
 *
 *    @retval            TRUE             Target string is contained in the source string
 *    @retval            FALSE            Target string is not contained in the source string
 */
BOOLEAN
IsSourceStrContainTargetStr (
  IN OUT   CHAR8 *SourceStr,
  IN OUT   CONST CHAR8 *TargetStr,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  BOOLEAN IsContained;
  UINT32  SourceStrIndex;
  UINT32  TargetStrIndex;
  CHAR8 TargetChar;

  IsContained = FALSE;
  if ((TargetStr != NULL) && (SourceStr != NULL)) {
    for (SourceStrIndex = 0; SourceStr[SourceStrIndex] != '\0'; SourceStrIndex++) {
      // Compare TrgString with SrcString from frist charactor to the '\0'
      for (TargetStrIndex = 0; TargetStr[TargetStrIndex] != '\0'; TargetStrIndex++) {
        if (TargetStr[TargetStrIndex] != SourceStr[SourceStrIndex + TargetStrIndex]) {
          // if it's not match, try to check the upcase/lowcase
          TargetChar = 0;
          if (TargetStr[TargetStrIndex] >= 'a' && TargetStr[TargetStrIndex] <= 'z') {
            TargetChar = TargetStr[TargetStrIndex] - ('a' - 'A');
          } else if (TargetStr[TargetStrIndex] >= 'A' && TargetStr[TargetStrIndex] <= 'Z') {
            TargetChar = TargetStr[TargetStrIndex] + ('a' - 'A');
          }
          // compare again
          if (TargetChar != SourceStr[SourceStrIndex + TargetStrIndex]) {
            break;
          }
        }
      }

      if ((TargetStr[TargetStrIndex] == '\0') && (TargetStrIndex != 0)) {
        IsContained = TRUE;
        break;
      }
    }
  }
  return IsContained;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *  AdjustGranularity
 *
 *  Description:
 *    If cache size is greater than or equal to 32M, then set granularity
 *    to 64K. otherwise, set granularity to 1K
 *
 *  Parameters:
 *    @param[in]    *CacheSizePtr
 *
 *    @retval       CacheSize
 *
 *  Processing:
 *
 */
UINT32
AdjustGranularity (
  IN UINT32 *CacheSizePtr
  )
{
  UINT32 CacheSize;

  if (*CacheSizePtr >= 2047 * 1024) {
    CacheSize = (*CacheSizePtr / 64);
    CacheSize |= 0x80000000;
  } else if (*CacheSizePtr >= 0x8000) {
    CacheSize = (UINT16) (*CacheSizePtr / 64);
    CacheSize |= 0x8000;
  } else {
    CacheSize = (UINT16) *CacheSizePtr;
  }

  return (CacheSize);
}


