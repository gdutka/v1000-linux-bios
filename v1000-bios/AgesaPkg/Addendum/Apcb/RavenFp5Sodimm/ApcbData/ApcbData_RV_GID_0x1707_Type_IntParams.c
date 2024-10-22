
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
#include <ApcbAutoGen.h>


//#define APCB_TYPE_DATA_START_SIGNATURE()    _16BYTE_ALIGN CHAR8 SigStartTypeData[16] = { '$', 'A', 'P', 'C', 'B', '_', 'T', 'Y', 'P', 'E', '_', 'S', 'T', 'A', 'R', 'T' }
//#define APCB_TYPE_DATA_END_SIGNATURE()      _4BYTE_ALIGN CHAR8 SigEndTypeData[16]   = { '$', 'A', 'P', 'C', 'B', '_', 'T', 'Y', 'P', 'E', '_', 'E', 'N', 'D', '_', '$' }

CHAR8 mDummyBuf;

APCB_TYPE_DATA_START_SIGNATURE();
APCB_TYPE_HEADER       ApcbTypeHeader = {
  APCB_GROUP_CBS,
  APCB_TYPE_CBS_DEBUG_PARAMETERS,
  (
    sizeof (APCB_TYPE_HEADER)
  ),
  0,
  0,
  0
};  // SizeOfType will be fixed up by tool

APCB_PARAM_ATTRIBUTE  InternalParameterListAttribute[] = {
  {APCB_TIME_POINT_ANY, APCB_TOKEN_CBS_DBG_MEM_TIMING_CTL_DDR4, sizeof (UINT8) - 1, 0},
  {APCB_TIME_POINT_ANY, APCB_TOKEN_CBS_DBG_MEM_SPEED_DDR4, sizeof (UINT8) - 1, 0},
  APCB_CBS_DBG_DEF_ATRIBS_AUTOGEN
  {0, APCB_TOKEN_CBS_DBG_LIMIT, 0, 0},
};

_1BYTE_ALIGN UINT8  InternalParameterListDefaultsValue[] = {
  BSU08 (1),                        //< BottomIo - Bottom of 32-bit IO space (8-bits).
  BSU08 (0xFF),                     //< BottomIo - Bottom of 32-bit IO space (8-bits).
  APCB_CBS_DBG_DEF_VALUE_AUTOGEN
  BSU08 (0xFF),
};

APCB_TYPE_DATA_END_SIGNATURE();


int main (IN int argc, IN char * argv [ ])
{
  return 0;
}





