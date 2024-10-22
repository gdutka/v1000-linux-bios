
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
APCB_TYPE_HEADER       ApcbTypeHeader = { APCB_GROUP_MEMORY, APCB_MEM_TYPE_EXT_VOLTAGE_CONTROL, (sizeof(APCB_TYPE_HEADER) + sizeof (PSP_EXT_VOLTAGE_CONTROL_STRUCT)), 0, 0, 0 };  // SizeOfType will be fixed up by tool

///< Platform Specific Overrides
PSP_EXT_VOLTAGE_CONTROL_STRUCT PspExternalVoltageControl = {
  TRUE,
  0x84,
  0x80,
  4,
  4,
  6,
  6,
  FALSE
};  

APCB_TYPE_DATA_END_SIGNATURE();


int main (IN int argc, IN char * argv [ ])
{
  return 0;
}





