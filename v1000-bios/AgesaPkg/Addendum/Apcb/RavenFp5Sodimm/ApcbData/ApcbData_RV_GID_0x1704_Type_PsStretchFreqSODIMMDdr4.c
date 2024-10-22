
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
//UDIMM MAX FREQ DDR4
APCB_TYPE_DATA_START_SIGNATURE();
APCB_TYPE_HEADER       ApcbTypeHeader = { APCB_GROUP_MEMORY, APCB_MEM_TYPE_PS_SODIMM_DDR4_STRETCH_FREQ, (sizeof(APCB_TYPE_HEADER) + (sizeof(PSCFG_MAXFREQ_ENTRY) * 7)), 0, 0, 0 };  // SizeOfType will be fixed up by tool
PSCFG_MAXFREQ_ENTRY MaxFeqSODIMMDdr4 [] = {
	// Max Freq.
	// Format :
	// DimmPerCh,   Dimms,   SR,   DR,   QR,   Speed1_2V,   Speed_TBD1,   Speed_TBD2
	//
	{ _1DIMM, 1, 1, 0, 0, DDR3200_FREQUENCY, UNSUPPORTED_DDR_FREQUENCY, UNSUPPORTED_DDR_FREQUENCY },
	{ _1DIMM, 1, 0, 1, 0, DDR2667_FREQUENCY, UNSUPPORTED_DDR_FREQUENCY, UNSUPPORTED_DDR_FREQUENCY },
};

APCB_TYPE_DATA_END_SIGNATURE();

int main (IN int argc, IN char * argv [ ])
{
  return 0;
}





