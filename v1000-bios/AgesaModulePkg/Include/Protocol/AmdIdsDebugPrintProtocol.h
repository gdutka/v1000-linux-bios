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
#ifndef _AMD_IDS_DEBUGPRINT_PROTOCOL_H_
#define _AMD_IDS_DEBUGPRINT_PROTOCOL_H_

typedef
VOID
(*AMD_IDS_DEBUG_PRINT) (
  IN      UINT64      Flag,
  IN      CONST CHAR8 *Format,
  IN      VA_LIST     Marker
  );
///
/// Protocol prototype
///
typedef struct _AMD_IDS_DEBUG_PRINT_PROTOCOL {
  AMD_IDS_DEBUG_PRINT print;    ///< Print function
} AMD_IDS_DEBUG_PRINT_PROTOCOL;

extern EFI_GUID gAmdIdsDebugPrintProtocolGuid;

#endif //_AMD_IDS_DEBUGPRINT_PROTOCOL_H_

