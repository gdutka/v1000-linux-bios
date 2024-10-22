/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD CCX Halt Library
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Lib
 * @e \$Revision: 312770 $   @e \$Date: 2015-02-11 13:24:42 +0800 (Wed, 11 Feb 2015) $
 *
 */
/*
 ******************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 **/

#ifndef _CCX_HALT_LIB_H_
#define _CCX_HALT_LIB_H_
#include "Uefi.h"
#include "AGESA.h"
#include "Library/IdsLib.h"
#pragma pack (push, 1)

/// Build Configuration structure for BLDCFG_AP_MTRR_SETTINGS
typedef struct {
  IN  UINT32 MsrAddr;     ///< Fixed-Sized MTRR address
  IN  UINT64 MsrData;     ///< MTRR Settings
} AP_MTRR_SETTINGS;

/*---------------------------------------------------------------------------------------*/
/**
 * Update AP MTRR settings list according to PCD
 *
 *
 * @param[in, out]  ApMtrrSettingsList List of MTRR settings for AP
 *
 */
VOID
UpdateApMtrrSettings (
  IN OUT   AP_MTRR_SETTINGS  *ApMtrrSettingsList
  );

/*---------------------------------------------------------------------------------------*/
/**
 * Performs a hlt instruction
 *
 *
 * @param[in]  StdHeader         Config handle for library and services
 *
 */
VOID
ExecuteHltInstruction (
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

#pragma pack (pop)
#endif // _CCX_HALT_LIB_H_

