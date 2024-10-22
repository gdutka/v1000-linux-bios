/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Integrated Debug Print Routines
 *
 * Contains all functions related to IDS Debug Print
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  IDS
 * @e \$Revision: 309090 $   @e \$Date: 2014-12-10 02:28:05 +0800 (Wed, 10 Dec 2014) $
 */
/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 **/

#ifndef _AMD_IDS_DEBUG_PRINT_LOCAL_H_
#define _AMD_IDS_DEBUG_PRINT_LOCAL_H_

/// return status for debug print
typedef enum {
  IDS_DEBUG_PRINT_SUCCESS = 0,      ///< success
  IDS_DEBUG_PRINT_BUFFER_OVERFLOW,      ///< Bufer overflow
} IDS_DEBUG_PRINT_STATUS;

typedef BOOLEAN (*PF_AMD_IDS_DEBUG_PRINT_SUPPORT) (VOID);
typedef BOOLEAN (*PF_AMD_IDS_DEBUG_PRINT_FILTER) (UINT64 *Filter);
typedef VOID (*PF_AMD_IDS_DEBUG_PRINT_PRINT) (CHAR8 *Buffer, UINTN BufferSize);
typedef VOID (*PF_AMD_IDS_DEBUG_CONSTRUCTOR) (VOID);

/// Debug print Hw layer service class
typedef struct _AMD_IDS_DEBUG_PRINT_INSTANCE {
  PF_AMD_IDS_DEBUG_PRINT_SUPPORT  support;    ///Check if support
  PF_AMD_IDS_DEBUG_CONSTRUCTOR    constructor;  ///constructor, will be called prior to driver execution
  PF_AMD_IDS_DEBUG_PRINT_FILTER  customfilter;  ///Get if any customize filters
  PF_AMD_IDS_DEBUG_PRINT_PRINT  print;  ///Print data to Hw layer
} AMD_IDS_DEBUG_PRINT_INSTANCE;

#endif //_AMD_IDS_DEBUG_PRINT_LOCAL_H_


