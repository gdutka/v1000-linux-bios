/* $NoKeywords:$ */
/**
 * @file
 *
 * mfamp.c
 *
 * Feature function for AMD Memory Profile (overclocking)
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Feat/AMP)
 * @e \$Revision: 309090 $ @e \$Date: 2014-12-10 02:28:05 +0800 (Wed, 10 Dec 2014) $
 *
 **/
/*****************************************************************************
  *
  * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
  *
  *******************************************************************************
  *
 */
#include "AGESA.h"
#include "mport.h"
#include "mm.h"
#include "mn.h"
#include "Library/AmdMemFeatAmpLib.h"
#include "mt.h"
#include "mt3.h"
#include "Library/IdsLib.h"
#include "Ids.h"
#include "mtspd3.h"
#include "merrhdl.h"
#include "mu.h"
#include "Filecode.h"
CODE_GROUP (G2_PEI)
RDATA_GROUP (G2_PEI)

#define FILECODE LIBRARY_MEM_NULL_FEATAMPLIBNULL_MFAMP_FILECODE

/*----------------------------------------------------------------------------
 *                          DEFINITIONS AND MACROS
 *
 *----------------------------------------------------------------------------
 */
#define SPD_OVRCLK_VAL 3

// AMP SPD bytes offset
#define AMP_SPD_PF         80
#define AMP_SPD_Volt       81
#define AMP_SPD_TCK        82
#define AMP_SPD_TCK_FTB    83
#define AMP_SPD_CASLO      84
#define AMP_SPD_CASHI      85
#define AMP_SPD_TAA        86
#define AMP_SPD_TAA_FTB    87
#define AMP_SPD_TRCD       88
#define AMP_SPD_TRCD_FTB   89
#define AMP_SPD_TRP        90
#define AMP_SPD_TRP_FTB    91
#define AMP_SPD_UPPER_TRC  92
#define AMP_SPD_TRAS       93
#define AMP_SPD_TRC        94
#define AMP_SPD_TRC_FTB    95
#define AMP_SPD_TWR        96
#define AMP_SPD_TWTR       97
#define AMP_SPD_TRTP       98
#define AMP_SPD_TRFC_LO    99
#define AMP_SPD_TRFC_HI    100
#define AMP_SPD_UPPER_TFAW 101
#define AMP_SPD_TFAW       102
#define AMP_SPD_TRRD       103

/*----------------------------------------------------------------------------
 *                           TYPEDEFS AND STRUCTURES
 *
 *----------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------
 *                        PROTOTYPES OF LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */

/*
 *-----------------------------------------------------------------------------
 *                                EXPORTED FUNCTIONS
 *
 *-----------------------------------------------------------------------------
 */

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   Enable overclocking functionality on current node if it is requested.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 *     @return          TRUE
 */

BOOLEAN
MemFAMP (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  return TRUE;
}


