
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
APCB_TYPE_HEADER       ApcbTypeHeader = { APCB_GROUP_MEMORY, APCB_MEM_TYPE_ERROR_OUT_EVENT_CONTROL, (sizeof(APCB_TYPE_HEADER) + sizeof (PSP_ERROR_OUT_CONTROL_STRUCT)), 0, 0, 0 };  // SizeOfType will be fixed up by tool

///< Platform Specific Overrides
PSP_ERROR_OUT_CONTROL_STRUCT PspEventOutControl = {
  FALSE,
  FALSE,
  FALSE,
  FALSE,
  0x84,
  15000,
  0x80,
  TRUE,
  4,
  4,
  6,
  6,
  FALSE,
  {
    85,
    0x01,
    0xC0
  },
  {
    {BEEP_ERROR_TYPE_GENERAL|BEEP_ERROR_OPERATION_MASK,     0x5555,   0x00000063},
    {BEEP_ERROR_TYPE_MEMORY|BEEP_ERROR_OPERATION_MASK,      0x5555,   0x00000065},
    {BEEP_ERROR_TYPE_DF|BEEP_ERROR_OPERATION_MASK,          0x5555,   0x00000067},
    {BEEP_ERROR_TYPE_CCX|BEEP_ERROR_OPERATION_MASK,         0x5555,   0x00000069},
    {BEEP_ERROR_TYPE_GNB|BEEP_ERROR_OPERATION_MASK,         0x5555,   0x0000006B},
    {BEEP_ERROR_TYPE_PSP|BEEP_ERROR_OPERATION_MASK,         0x5555,   0x0000006D},
    {BEEP_ERROR_TYPE_SMU|BEEP_ERROR_OPERATION_MASK,         0x5555,   0x0000006F},
    {BEEP_ERROR_TYPE_UNKNOW|BEEP_ERROR_OPERATION_MASK,      0x5555,   0x00000063}
  },
  FALSE
};  

APCB_TYPE_DATA_END_SIGNATURE();


int main (IN int argc, IN char * argv [ ])
{
  return 0;
}





