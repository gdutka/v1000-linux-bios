/* $NoKeywords:$ */
/**
 * @file
 *
 * MEMORY SMM Library
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AMD Memory Smm Library
 * @e sub-project:  Library
 * @e \$Revision$   @e \$Date$
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
#ifndef DRAM_MAP_KB_H_
#define DRAM_MAP_CZ_H_

#define BUS_0 0
#define FUNCTION_1 1
#define FUNCTION_2 2
#define FUNCTION_3 3
#define DEVICE_24 24

#define NUM_NODES 1
#define NUM_CHANNELS_PER_NODE  2
#define NUM_CS_PER_CHANNEL  4
#define MAX_BANK_ADDR  0x7
#define MAX_ROW_ADDR  0xFFFF
#define MAX_COL_ADDR  0x3FFF

VOID
PlatformTranslateSysAddrToCS (
  IN       UINT64      SysAddr,
     OUT   BOOLEAN     *CSFound,
     OUT   UINT64      *NormalizedChannelAddr,
     OUT   UINT8       *NodeID,
     OUT   UINT8       *ChannelSelect,
     OUT   UINT8       *ChipSelect,
     OUT   UINT8       *Bank,
     OUT   UINT32      *Row,
     OUT   UINT16      *Col
  );

VOID
PlatformTranslateCsAddrToSys (
  IN       UINT64      NormalizedChannelAddr,
  IN       UINT8       NormalizedDieId,
  IN       UINT8       ChannelSelect,
     OUT   BOOLEAN     *CSFound,
     OUT   UINT64      *SystemAddr,
     OUT   UINT8       *ChipSelect,
     OUT   UINT8       *Bank,
     OUT   UINT32      *Row,
     OUT   UINT16      *Col
  );

#endif

