/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Ccx SMBIOS common code
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Library
 * @e \$Revision: 309090 $   @e \$Date: 2014-12-10 02:28:05 +0800 (Wed, 10 Dec 2014) $
 *
 */
/*
 ******************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef _LIBRARY_CCXSMBIOSZENZPLIB_CCXSMBIOSZENZPCOMMONLIB_H_
#define _LIBRARY_CCXSMBIOSZENZPLIB_CCXSMBIOSZENZPCOMMONLIB_H_

#pragma pack (push, 1)

/*---------------------------------------------------------------------------------------
 *          M I X E D   (Definitions And Macros / Typedefs, Structures, Enums)
 *---------------------------------------------------------------------------------------
 */


/*---------------------------------------------------------------------------------------
 *                 D E F I N I T I O N S     A N D     M A C R O S
 *---------------------------------------------------------------------------------------
 */


/*---------------------------------------------------------------------------------------
 *               T Y P E D E F S,   S T R U C T U R E S,    E N U M S
 *---------------------------------------------------------------------------------------
 */

/// A structure containing processor name string and
/// the value that should be provide to DMI type 4 processor family
typedef struct {
  IN       CONST CHAR8 *Stringstart;  ///< The literal string
  IN       UINT16  T4ProcFamilySetting; ///< The value set to DMI type 4 processor family
} CPU_T4_PROC_FAMILY;

/*---------------------------------------------------------------------------------------
 *                        F U N C T I O N    P R O T O T Y P E
 *---------------------------------------------------------------------------------------
 */
VOID
GetNameString (
  IN OUT   CHAR8 *String,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

BOOLEAN
IsSourceStrContainTargetStr (
  IN OUT   CHAR8 *SourceStr,
  IN OUT   CONST CHAR8 *TargetStr,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

UINT32
AdjustGranularity (
  IN UINT32 *CacheSizePtr
  );

#pragma pack (pop)
#endif  // _LIBRARY_CCXSMBIOSZENZPLIB_CCXSMBIOSZENZPCOMMONLIB_H_


