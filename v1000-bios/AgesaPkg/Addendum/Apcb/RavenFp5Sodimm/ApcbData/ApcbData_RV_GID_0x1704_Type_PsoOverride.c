
/* $NoKeywords:$ */
/**
 * @file
 *
 * @e \$Revision:$   @e \$Date:$
 */
/*****************************************************************************
 *
 * Copyright (C) 2015-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 **/

#include "MyPorting.h"
#include <RV/APCB.h>
#include <ApcbCustomizedDefinitions.h>
#include <ApcbDefaults.h>

CHAR8 mDummyBuf;

APCB_TYPE_DATA_START_SIGNATURE();
APCB_TYPE_HEADER       ApcbTypeHeader = { APCB_GROUP_MEMORY, APCB_MEM_TYPE_PSO_DATA, (sizeof(APCB_TYPE_HEADER) + sizeof (PSP_PSO_STRUCT)), 0, 0, 0 };  // SizeOfType will be fixed up by tool

///< Platform Specific Overrides
PSP_PSO_STRUCT PlatformMemoryConfiguration = {
  //-----------------------------------------
  // Platform Memory Configuration Data Block
  //-----------------------------------------
    BLDCFG_MEM_CLK_MAP,
    BLDCFG_CKE_TRI_MAP,
    BLDCFG_ODT_TRI_MAP,
    BLDCFG_CS_TRI_MAP,
    BLDCFG_NUM_DIMMS_SUPPORTED,
    BLDCFG_NUM_CHNL_SUPPORTED,
    BLDCFG_NUM_SOLDERED_DOWN_DIMM_SUPPORTED,
    BLDCFG_NUM_CS_SUPPORTED,

    BLDCFG_BUS_SPEED_OVERRIDE,
    BLDCFG_DRAM_TECH_OVERRIDE,
    BLDCFG_SOLDRD_DOWN_DIMM_TYPE,
    BLDCFG_MEM_POWER_POLICY_OVERRIDE,
    BLDCFG_NUM_MOTHERBOARD_LAYERS
};  

APCB_TYPE_DATA_END_SIGNATURE();


int main (IN int argc, IN char * argv [ ])
{
  return 0;
}





