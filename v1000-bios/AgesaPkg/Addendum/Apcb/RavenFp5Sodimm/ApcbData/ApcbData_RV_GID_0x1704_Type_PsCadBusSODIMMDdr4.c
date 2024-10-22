
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
//UDIMM CADBUS DDR4
APCB_TYPE_DATA_START_SIGNATURE();
APCB_TYPE_HEADER  ApcbTypeHeader = { APCB_GROUP_MEMORY, APCB_MEM_TYPE_PS_SODIMM_DDR4_CAD_BUS, (sizeof(APCB_TYPE_HEADER) + sizeof (PSCFG_CADBUS_ENTRY) * 30), 0, 0, 0 };  // SizeOfType will be fixed up by tool

PSCFG_CADBUS_ENTRY CadBusSODimmDdr4[] = {
  {1, DDR1333 + DDR1600 + DDR1866 + DDR2133 + DDR2400, V1_2, DIMM_SR,           NP,                0, 0, 0x00000000, 0x0F, 0x0F, 0x0F, 0x0F},
  {1, DDR1333,                                         V1_2, DIMM_DR,           NP,                0, 0, 0x00360000, 0x0F, 0x0F, 0x0F, 0x0F},
  {1, DDR1600 + DDR1866 + DDR2133 + DDR2400,           V1_2, DIMM_DR,           NP,                0, 1, 0x00000000, 0x0F, 0x0F, 0x0F, 0x0F},
  {1, DDR2667,                                         V1_2, DIMM_SR + DIMM_DR, NP,                1, 1, 0x00000000, 0x0F, 0x0F, 0x0F, 0x0F},
  {1, DDR2933 + DDR3200,                               V1_2, DIMM_SR,           NP,                1, 1, 0x00000000, 0x0F, 0x0F, 0x0F, 0x0F},
};

APCB_TYPE_DATA_END_SIGNATURE();


int main (IN int argc, IN char * argv [ ])
{
  return 0;
}





