/**
 * @file
 *
 * AMD Integrated Debug Debug_library Routines
 *
 * Contains all functions related to HDTOUT
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  IDS
 * @e \$Revision: 309090 $   @e \$Date: 2014-12-10 02:28:05 +0800 (Wed, 10 Dec 2014) $
 */
/*****************************************************************************
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */
#ifndef _AMD_IDS_DEBUGPRINT_LIB_H_
#define _AMD_IDS_DEBUGPRINT_LIB_H_
#define AMD_IDS_DP_FLAG_ALL       0xFFFFFFFFFFFFFFFFull
/// HDTOUT Header.
typedef struct _HDTOUT_HEADER {
  UINT32 Signature;               ///< 0 0xDB1099CC
  UINT16 Version;                 ///< 4 HDTOUT version.
  UINT16 BufferSize;              ///< 6 Size in bytes.
  UINT16 DataIndex;               ///< 8 Data Index.
  UINT8  PrintCtrl;               ///< 10 0 off no print  1 on print
  UINT8  NumBreakpointUnit;       ///< 11 default 0 no bp unit others number of bp unit
  UINT32 FuncListAddr;            ///< 12 32 bit address to the list of functions that script can execute
  UINT8  ConsoleType;             ///< 16 Console type - deprecated
  UINT8  Event;                   ///< 17 Event type. - deprecated
  UINT8  OutBufferMode;           ///< 18 Off:stack mode, On: heap mode - deprecated
  UINT32 EnableMask;              ///< 19 Bitmap to select which part should be streamed out
  UINT64 ConsoleFilter;           ///< 23 Filter use to select which part should be streamed out
  UINT8  BspOnlyFlag;             ///< 31 1 Only Enable Bsp output, 0 enable On All cores
  UINT8  Reserved[56 - 32];       ///< 32 Reserved for header expansion

  CHAR8  BreakpointList[300];     ///< 56 Breakpoint list
  CHAR8  StatusStr[156];          ///< 356 Shows current node, DCT, CS,...
  CHAR8  Data[2];                 ///< 512 HDTOUT content. Its size will be determined by BufferSize.
} HDTOUT_HEADER;

BOOLEAN
AmdIdsDpHdtOutSupport (
  VOID
  );

VOID
AmdIdsDpHdtOutPrint (
  IN      CHAR8   *Buffer,
  IN      UINTN BufferSize
  );

BOOLEAN
AmdIdsDpHdtOutGetHeader (
  IN OUT   HDTOUT_HEADER **pHdtoutHeaderPtr
  );

VOID
AmdIdsDpHdtOutRegisterInit (
  VOID
  );

VOID
AmdIdsDpHdtOutBufferModeInit (
  );

VOID
AmdIdsDebugPrintf (
  IN      UINT64      FilterFlag,
  IN      CONST CHAR8 *Format,
  IN      VA_LIST     Marker
  );

#endif //_AMD_IDS_DEBUGPRINT_LIB_H_
