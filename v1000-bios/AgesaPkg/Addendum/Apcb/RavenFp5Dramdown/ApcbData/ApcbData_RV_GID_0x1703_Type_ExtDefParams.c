
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
APCB_TYPE_HEADER       ApcbTypeHeader = {
  APCB_GROUP_DF,
  APCB_DF_TYPE_CONFIG_DEFAULT_PARAMETERS,
  (
    sizeof (APCB_TYPE_HEADER)
  ),
  0,
  0,
  0
};  // SizeOfType will be fixed up by tool

APCB_PARAM_ATTRIBUTE  ParameterListDefaultsAttribute[] = {
  {APCB_TIME_POINT_ANY, APCB_TOKEN_CONFIG_DF_GMI_ENCRYPT, sizeof (UINT8) - 1, 0},
  {APCB_TIME_POINT_ANY, APCB_TOKEN_CONFIG_DF_XGMI_ENCRYPT, sizeof (UINT8) - 1, 0},
  {APCB_TIME_POINT_ANY, APCB_TOKEN_CONFIG_DF_SAVE_RESTORE_MEM_ENCRYPT, sizeof (UINT8) - 1, 0},
  {APCB_TIME_POINT_ANY, APCB_TOKEN_CONFIG_DF_SYS_STORAGE_AT_TOP_OF_MEM, sizeof (UINT8) - 1, 0},
  {APCB_TIME_POINT_ANY, APCB_TOKEN_CONFIG_DF_PROBE_FILTER_ENABLE, sizeof (UINT8) - 1, 0},
  {APCB_TIME_POINT_ANY, APCB_TOKEN_CONFIG_DF_BOTTOMIO, sizeof (UINT8) - 1, 0},
  {APCB_TIME_POINT_ANY, APCB_TOKEN_CONFIG_DF_MEM_INTERLEAVING, sizeof (UINT8) - 1, 0},
  {APCB_TIME_POINT_ANY, APCB_TOKEN_CONFIG_DF_DRAM_INTERLEAVE_SIZE, sizeof (UINT8) - 1, 0},
  {APCB_TIME_POINT_ANY, APCB_TOKEN_CONFIG_DF_ENABLE_CHAN_INTLV_HASH, sizeof (UINT8) - 1, 0},
  {APCB_TIME_POINT_ANY, APCB_TOKEN_CONFIG_DF_PCI_MMIO_SIZE, sizeof (UINT32) - 1, 0},
  {APCB_TIME_POINT_ANY, APCB_TOKEN_CONFIG_DF_CAKE_CRC_THRESH_PERF_BOUNDS, sizeof (UINT32) - 1, 0},
  {APCB_TIME_POINT_ANY, APCB_TOKEN_CONFIG_DF_MEM_CLEAR, sizeof (BOOLEAN) - 1, 0},
  {0, APCB_TOKEN_CONFIG_LIMIT, 0, 0},
};

_1BYTE_ALIGN UINT8  ParameterListDefaultsValue[] = {
  BSU08 (BLDCFG_DF_GMI_ENCRYPT),                  //< GMI encryption
  BSU08 (BLDCFG_DF_XGMI_ENCRYPT),                 //< xGMI encryption
  BSU08 (BLDCFG_DF_SAVE_RESTORE_MEM_ENCRYPT),     //< Save/Restore memory encryption
  BSU08 (BLDCFG_DF_SYS_STORAGE_AT_TOM),           //< Global CC6 enable
  BSU08 (BLDCFG_DF_PROBE_FILTER_ENABLE),          //< SPF
  BSU08 (BLDCFG_DF_BOTTOM_IO),                    //< Bottom IO
  BSU08 (BLDCFG_DF_MEM_INTERLEAVING),             //< Channel, Die, Socket interleaving control
  BSU08 (BLDCFG_DF_DRAM_INTLV_SIZE),              //< Address bit to interleave on
  BSU08 (BLDCFG_DF_CHAN_INTLV_HASH_EN),           //< Enable channel interleaving hash
  BSU32 (BLDCFG_DF_PCI_MMIO_SIZE),                //< Size in bytes of space used for PCI MMIO
  BSU32 (BLDCFG_DF_CAKE_CRC_THRESH_PERF_BOUNDS),  //< Percentage of performance to sacrifice for CAKE CRC in 0.00001% units
  BSBLN (BLDCFG_DF_MEM_CLEAR),          //< Request to do DF PIE mem clear
  BSU08 (0xFF)
};

APCB_TYPE_DATA_END_SIGNATURE();


int main(IN int argc, IN char * argv[])
{
  return 0;
}





