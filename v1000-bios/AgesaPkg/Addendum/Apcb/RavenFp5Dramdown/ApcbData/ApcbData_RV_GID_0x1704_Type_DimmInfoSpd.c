
/* $NoKeywords:$ */
/**
 * @file
 *
 * @e \$Revision:$   @e \$Date:$
 */
/*****************************************************************************
 *
 * Copyright (C) 2016-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 **/


#include "MyPorting.h"
#include <RV/APCB.h>
#include <ApcbCustomizedDefinitions.h>
#include <ApcbDefaults.h>

CHAR8 mDummyBuf;

APCB_TYPE_DATA_START_SIGNATURE();
APCB_TYPE_HEADER       ApcbTypeHeader = { APCB_GROUP_MEMORY, APCB_MEM_TYPE_DIMM_INFO_SMBUS, (sizeof(APCB_TYPE_HEADER) + sizeof (DIMM_INFO_SMBUS)), 0, 0, 0 };  // SizeOfType will be fixed up by tool

DIMM_INFO_SMBUS  DimmInfoSmbus [] =
{

  TRUE, 0, 0, 0, BLDCFG_SPD_CH_A_DIMM0_ADDRESS, 0xFF, 0xFF, 0xFF,
  TRUE, 0, 0, 1, BLDCFG_SPD_CH_A_DIMM1_ADDRESS, 0xFF, 0xFF, 0xFF,
  TRUE, 0, 1, 0, BLDCFG_SPD_CH_B_DIMM0_ADDRESS, 0xFF, 0xFF, 0xFF,
  TRUE, 0, 1, 1, BLDCFG_SPD_CH_B_DIMM1_ADDRESS, 0xFF, 0xFF, 0xFF,

};

APCB_TYPE_DATA_END_SIGNATURE();


int main (IN int argc, IN char * argv [ ])
{
  return 0;
}





