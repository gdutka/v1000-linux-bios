
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
// UDIMM ODTPAT DDR4
APCB_TYPE_DATA_START_SIGNATURE();
APCB_TYPE_HEADER       ApcbTypeHeader = { APCB_GROUP_MEMORY, APCB_MEM_TYPE_PS_SODIMM_DDR4_ODT_PAT, (sizeof(APCB_TYPE_HEADER) + sizeof (PSCFG_2D_ODTPAT_ENTRY) * 5), 0, 0, 0 };  // SizeOfType will be fixed up by tool

PSCFG_2D_ODTPAT_ENTRY OdtPatEntry [] = {
	{ NP,      DIMM_SR, 0x00000000, 0x00000000, 0x00000200, 0x00000000 },
	{ NP,      DIMM_DR, 0x00000000, 0x00000000, 0x00000200, 0x00000800 },
	{ DIMM_SR, NP,      0x00000101, 0x00000000, 0x00000000, 0x00000000 },
	{ DIMM_DR, NP,      0x00000101, 0x00000404, 0x00000000, 0x00000000 },
	{ DIMM_SR, DIMM_SR, 0x00000202, 0x00000000, 0x00000101, 0x00000000 },
	{ DIMM_SR, DIMM_DR, 0x00000202, 0x00000000, 0x00000101, 0x00000101 },
	{ DIMM_DR, DIMM_SR, 0x00000202, 0x00000202, 0x00000101, 0x00000000 },
	{ DIMM_DR, DIMM_DR, 0x00000202, 0x00000202, 0x00000101, 0x00000101 },
};

APCB_TYPE_DATA_END_SIGNATURE();

int main (IN int argc, IN char * argv [ ])
{
  return 0;
}





