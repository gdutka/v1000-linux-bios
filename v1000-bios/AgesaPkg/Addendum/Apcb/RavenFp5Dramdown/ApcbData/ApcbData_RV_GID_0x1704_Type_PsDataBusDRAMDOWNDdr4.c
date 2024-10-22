
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

//UDIMM DATABUS DDR4
APCB_TYPE_DATA_START_SIGNATURE();
APCB_TYPE_HEADER  ApcbTypeHeader = { APCB_GROUP_MEMORY, APCB_MEM_TYPE_PS_SODIMM_DDR4_DATA_BUS, (sizeof(APCB_TYPE_HEADER) + sizeof (PSCFG_DATABUS_ENTRY_D4) * 30), 0, 0, 0 };  // SizeOfType will be fixed up by tool
PSCFG_DATABUS_ENTRY_D4 DataBusSOdimmDdr4[] = {
  {1, DDR1333,                                                   V1_2, DIMM_SR,           NP,                RTTNOM_OFF, RTTWR_OFF, RTTPRK_80,  0x1E, 0x1E, 0xA,  0x5A, 0x18},
  {1, DDR1333,                                                   V1_2, DIMM_DR,           NP,                RTTNOM_OFF, RTTWR_80,  RTTPRK_240, 0x1E, 0x1E, 0xA,  0x5A, 0x18},
  {1, DDR1600 + DDR1866 + DDR2133 + DDR2400 + DDR2667,           V1_2, DIMM_SR,           NP,                RTTNOM_48, RTTWR_OFF, RTTPRK_OFF,  0x3F, 0x3F, 0x18, 0x5A, 0x18},
  {1, DDR1600 + DDR1866 + DDR2133 + DDR2400 + DDR2667,           V1_2, DIMM_DR,           NP,                RTTNOM_48, RTTWR_OFF, RTTPRK_OFF, 0x3F, 0x3F, 0x18, 0x5A, 0x18},
  {1, DDR2933 + DDR3200,                                         V1_2, DIMM_SR,           NP,                RTTNOM_OFF, RTTWR_OFF, RTTPRK_48,  0x3F, 0x3F, 0x18, 0x5A, 0x18},
};

APCB_TYPE_DATA_END_SIGNATURE();

int main (IN int argc, IN char * argv [ ])
{
  return 0;
}





