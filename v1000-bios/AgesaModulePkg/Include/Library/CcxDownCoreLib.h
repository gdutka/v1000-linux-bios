/* $NoKeywords:$ */
/**
 * @file
 *
 *
 * AMD Ccx Down Core Library
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

#ifndef _CCX_DOWN_CORE_LIB_H_
#define _CCX_DOWN_CORE_LIB_H_
#pragma pack (push, 1)

#include "AGESA.h"

/// Selection type for CCD down binning
typedef enum {
  CCD_MODE_AUTO = 0,            ///< Auto
  CCD_MODE_1_CCD,               ///< Enable 1 CCD
  CCD_MODE_2_CCDS,              ///< Enable 2 CCDs
  CCD_MODE_3_CCDS,              ///< Enable 3 CCDs
  CCD_MODE_4_CCDS,              ///< Enable 4 CCDs
  CCD_MODE_5_CCDS,              ///< Enable 5 CCDs
  CCD_MODE_6_CCDS,              ///< Enable 6 CCDs
  CCD_MODE_7_CCDS,              ///< Enable 7 CCDs
  CCD_MODE_MAX                  ///< Used for bounds checking
} CCD_MODE;

/// Selection type for down core
typedef enum {
  CCX_DOWN_CORE_AUTO = 0,       ///< Auto
  CCX_DOWN_CORE_1_0,            ///< 1 + 0 mode
  CCX_DOWN_CORE_1_1,            ///< 1 + 1 mode
  CCX_DOWN_CORE_2_0,            ///< 2 + 0 mode
  CCX_DOWN_CORE_3_0,            ///< 3 + 0 mode
  CCX_DOWN_CORE_2_2,            ///< 2 + 2 mode
  CCX_DOWN_CORE_4_0,            ///< 4 + 0 mode
  CCX_DOWN_CORE_3_3,            ///< 3 + 3 mode
  CCX_DOWN_CORE_MAX             ///< Used for bounds checking
} CCX_DOWN_CORE_MODE;

/// BSC to AP MSR sync up
typedef struct {
  UINT8  SystemDieCount;    ///< System Die count
  UINT32 CoreDisMask[MAX_SOCKETS_SUPPORTED * MAX_DIES_PER_SOCKET];     ///< CoreDisMask for each Die
} CCX_CORE_DIS_MASK;

/* -----------------------------------------------------------------------------*/
/**
 *  Set down core register
 *
 *
 *  @param[in] CcxDownCoreMode - Down core mode
 *  @param[in] StdHeader       - Config handle for library and services.
 *
 */
VOID
CcxDownCore (
  IN       CCX_DOWN_CORE_MODE CcxDownCoreMode,
  IN OUT   AMD_CONFIG_PARAMS  *StdHeader
  );

#pragma pack (pop)
#endif // _CCX_DOWN_CORE_LIB_H_

