/*****************************************************************************
 *
 * Copyright (C) 2016-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef _APCB_MEM_PSO_OVERRIDE_H_
#define _APCB_MEM_PSO_OVERRIDE_H_
//
// @APCB_START
//

///===============================================================================
/// _PSP_PSC_DRAMDOWN_STRUCT
/// This data structure is used to pass wrapper parameters to the memory configuration code
///
/** PSP_PSO_STRUCT
*    @GROUPID: APCB_GROUP_MEMORY
*    @TYPEID: APCB_MEM_TYPE_PSO_DATA
*    This data structure is used to pass memory PSO override data
*/
typedef struct _PSP_PSO_STRUCT {
  PSO_ENTRY MemClkDisMap[MEMCLK_DIS_MAP_SIZE];
  PSO_ENTRY CkeTriMap[CKE_TRI_MAP_SIZE];
  PSO_ENTRY OdtTriMap[ODT_TRI_MAP_SIZE];
  PSO_ENTRY CsTriMap[CS_TRI_MAP_SIZE];
  PSO_ENTRY NumberDimmSupported[NUMBER_OF_DIMMS_SUPPORTED_SIZE];
  PSO_ENTRY NumberSolderedDownDimmsSupported[NUMBER_OF_SOLDERED_DOWN_DIMMS_SUPPORTED_SIZE];
  PSO_ENTRY NumberChipSelectsSupported[NUMBER_OF_CHIP_SELECTS_SUPPORTED_SIZE];
  PSO_ENTRY NumberChannelsSupported[NUMBER_OF_CHANNELS_SUPPORTED_SIZE];
  PSO_ENTRY OverrideBusSpeed[OVERRIDE_DDR_BUS_SPEED_SIZE];
  PSO_ENTRY DramTechnology[DRAM_TECHNOLOGY_SIZE];
  PSO_ENTRY SolderedDownSoDimmType[SOLDERED_DOWN_SODIMM_TYPE_SIZE];
  PSO_ENTRY MemoryPowerPolicyOverride[MEMORY_POWER_POLICY_OVERRIDE_SIZE];
  PSO_ENTRY MotherboardLayers[MOTHER_BOARD_LAYERS_SIZE];
} PSP_PSO_STRUCT;


#endif // _APCB_MEM_PSO_OVERRIDE_H_


