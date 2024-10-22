/* $NoKeywords:$ */
/**
 * @file
 *
 * GNB mid post initialization.
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 313706 $   @e \$Date: 2015-02-25 21:00:43 -0600 (Wed, 25 Feb 2015) $
 *
 */
/*
*****************************************************************************
*
* Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
*
*******************************************************************************
*
*/

#ifndef _GNBAZALIALIB_H_
#define _GNBAZALIALIB_H_

#define HDA_MAX_SDIN_NUM                 0x0F
#define TIMEOUT_LOOP_TIMES               0x10
#define TIMEOUT_STALL                    1000

#define HDAx0008_ADDRESS                 0x8
#define HDAx000C_ADDRESS                 0xc
#define HDAx0060_ADDRESS                 0x60
#define HDAx0064_ADDRESS                 0x64
#define HDAx0068_ADDRESS                 0x68

AGESA_STATUS
ConfigureVerbTable (
  IN      UINT32            HdaBaseAddress,
  IN      UINT64            VerbTable,
  IN      AMD_CONFIG_PARAMS *StdHeader
  );

#endif

