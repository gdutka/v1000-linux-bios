/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD HDTOUT PPI prototype definition
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  HDTOUT
 * @e \$Revision: 309090 $   @e \$Date: 2014-12-10 02:28:05 +0800 (Wed, 10 Dec 2014) $
 */
/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 **/
#ifndef _AMD_HDTOUT_PPI_H_
#define _AMD_HDTOUT_PPI_H_


#define AMD_IDS_DEBUG_PRINT_PPI_GUID \
  {0x8663e771, 0x52f6, 0x49fb, 0xb3, 0x45, 0xdb, 0x93, 0x96, 0x5e, 0x6c, 0xab}

typedef
VOID
(*AMD_IDS_DEBUG_PRINT) (
  IN      UINT64      Flag,
  IN      CONST CHAR8 *Format,
  IN      VA_LIST     Marker
  );
///
/// PPI prototype
///
typedef struct _AMD_IDS_DEBUG_PRINT_PPI {
  AMD_IDS_DEBUG_PRINT print;    ///< Print Function
} AMD_IDS_DEBUG_PRINT_PPI;

extern EFI_GUID gAmdIdsDebugPrintPpiGuid;

#endif //_AMD_HDTOUT_PPI_H_

