/*****************************************************************************
 *
 * Copyright (C) 2016-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */
/*++
Module Name:

  AmdMemPeim.h

Abstract:
--*/

#ifndef _AMD_MEM_PS_SOCKET_LIB_H_
#define _AMD_MEM_PS_SOCKET_LIB_H_

typedef BOOLEAN MEM_PSC_FLOW (
  IN       UINT8 value
  );
/// Table entry
typedef struct {
  UINT8 timing;              ///< Table size
} PSC_TBL_ENTRY;;
/**
 * PLATFORM SPECIFIC CONFIGURATION FLOW BLOCK - Pointers to the sub-engines of platform
 * specific configuration.
 */
typedef struct _MEM_PSC_FLOW_BLOCK {
  PSC_TBL_ENTRY *CadBusSoFamily;         ///< Table Entry of CAD Bus Per Family
  PSC_TBL_ENTRY *CadBusSoFamilySocket;   ///< Table Entry of CAD Bus Per Family/Socket
  MEM_PSC_FLOW *LookupCadBusCfg;         ///< Sub-engine which performs CAD Bus Config value extraction.
} MEM_PSC_FLOW_BLOCK;




#endif
