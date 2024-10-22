/* $NoKeywords:$ */
/**
 * @file
 *
 * mpUczFP4D4
 *
 * Platform specific settings for CZ DDR4 UDIMM system
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Ps/CZ)
 * @e \$Revision: 328656 $ @e \$Date: 2015-10-12 15:49:41 +0800 (Mon, 12 Oct 2015) $
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
#include "ma.h"
#include "mm.h"
#include "mn.h"
#include "mp.h"
#include "PlatformMemoryConfiguration.h"
#include "Filecode.h"
CODE_GROUP (G2_PEI)
RDATA_GROUP (G2_PEI)

#define FILECODE LIBRARY_MEM_PSCZFP4LIB_MPUCZFP4D4_FILECODE
// For A1 or Greater tables
#define AMD_REV_F15_CZ_GTE_A1    (~AMD_REV_F15_CZ_A0 & AMD_REV_F15_ALL)
/*----------------------------------------------------------------------------
 *                          DEFINITIONS AND MACROS
 *
 *----------------------------------------------------------------------------
 */

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

