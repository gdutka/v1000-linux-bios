
/* $NoKeywords:$ */
/**
 * @file
 *
 * @e \$Revision:$   @e \$Date:$
 */
/*****************************************************************************
 *
 * Copyright (C) 2013-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 **/

#include "MyPorting.h"
#include <RV/APCB.h>
#include <ApcbCustomizedDefinitions.h>
#include <ApcbDefaults.h>

CHAR8 mDummyBuf;

APCB_TYPE_DATA_START_SIGNATURE();
APCB_TYPE_HEADER       ApcbTypeHeader = { APCB_GROUP_MEMORY, APCB_MEM_TYPE_CONSOLE_OUT_CONTROL, (sizeof(APCB_TYPE_HEADER) + sizeof (PSP_CONSOLE_OUT_STRUCT)), 0, 0, 0 };  // SizeOfType will be fixed up by tool

///< Platform Specific Overrides
PSP_CONSOLE_OUT_STRUCT ConsoleOutControl = {
  {
    FALSE,
    0x80,
    TRUE,
    TRUE,
    FALSE,
    FALSE,
    FALSE,
    FALSE,
    FALSE,
    FALSE
  },
  {
    #ifdef BLDCFG_CONOUTCTRL_BREAKPOINT
      BLDCFG_CONOUTCTRL_BREAKPOINT,
    #else
      FALSE,
    #endif

    #ifdef BLDCFG_CONOUTCTRL_BREAKPOINT_ALLDIE
      BLDCFG_CONOUTCTRL_BREAKPOINT_ALLDIE,
    #else
      FALSE,
    #endif
  }
};  

APCB_TYPE_DATA_END_SIGNATURE();


int main (IN int argc, IN char * argv [ ])
{
  return 0;
}





