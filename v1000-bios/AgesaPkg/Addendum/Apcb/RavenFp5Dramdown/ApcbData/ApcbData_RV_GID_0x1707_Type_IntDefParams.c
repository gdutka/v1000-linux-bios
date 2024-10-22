
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
  APCB_TYPE_CBS_COMMON_PARAMETERS,
  (
    sizeof (APCB_TYPE_HEADER)
  ),
  0,
  0,
  0
};  // SizeOfType will be fixed up by tool

APCB_PARAM_ATTRIBUTE  InternalParameterListDefaultsAttribute[] = {
  {APCB_TIME_POINT_ANY, APCB_TOKEN_CBS_CMN_GNB_GFX_UMA_MODE, sizeof (UINT8) - 1, 0},
  {APCB_TIME_POINT_ANY, APCB_TOKEN_CBS_CMN_GNB_GFX_UMA_VERSION, sizeof (UINT8) - 1, 0},
  {APCB_TIME_POINT_ANY, APCB_TOKEN_CBS_CMN_GNB_GFX_DISPLAY_RESOLUTION, sizeof (UINT8) - 1, 0},
  {APCB_TIME_POINT_ANY, APCB_TOKEN_CBS_CMN_GNB_GFX_UMA_FRAME_BUFFER_SIZE, sizeof (UINT32) - 1, 0},
  {APCB_TIME_POINT_ANY, APCB_TOKEN_CBS_CMN_MEM_DIS_MEM_ERR_INJ, sizeof (UINT8) - 1, 0},
  APCB_CBS_CMN_DEF_ATRIBS_AUTOGEN
  {0, APCB_TOKEN_CBS_CMN_LIMIT, 0, 0},
};

_1BYTE_ALIGN  UINT8  InternalParameterListDefaultsValue[] = {
  BSU08 (BLDCFG_UMA_MODE),
  BSU08 (BLDCFG_UMA_VERSION),
  BSU08 (BLDCFG_DISPLAY_RESOLUTION),
  BSU32 (BLDCFG_GFX_UMA_FRAME_BUFFER_SIZE),
  BSU08 (0x1),
  APCB_CBS_CMN_DEF_VALUES_AUTOGEN
  BSU08 (0xFF),
};

APCB_TYPE_DATA_END_SIGNATURE();


int main (IN int argc, IN char * argv [ ])
{
  return 0;
}






