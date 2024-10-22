/** @file
  Internal header file for PeiVerbTableCfgLib.

;******************************************************************************
;* Copyright (c) 2017, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef __VerbTable_Cfg_GetVerbTable_H__
#define __VerbTable_Cfg_GetVerbTable_H__

#include <Ppi/VerbTable.h>
#include <Library/PeiServicesLib.h>
#include <Library/PeiVerbTableCfgLib.h>
#include <Library/FlashRegionLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PeiOemSvcKernelLib.h>
#include <Library/PcdLib.h>

#define  SUPPORT_BOARD_COUNT 6
#define  DEFAULT_BOARD_VALUE 0

#pragma pack(1)

//
//  Format of this ID follow Build technical reference 112.
//  This ID should match with verbtable RegionID in FDM.
//
typedef struct _H2O_FLASH_MAP_VERB_TABLE_ID {
  UINT8 Reserved[10];
  UINT8 Boards[SUPPORT_BOARD_COUNT];
} H2O_FLASH_MAP_VERB_TABLE_ID;

//
//  A private common header type used to parse each codec in verb table binary.
//
typedef struct _COMMON_VERB_TABLE_BIN_HEADER {
  COMMON_CHIPSET_AZALIA_VERB_TABLE_HEADER Header;
  UINT16                                  DataSizeDword;
} COMMON_VERB_TABLE_BIN_HEADER;

#pragma pack()

EFI_GUID  gH2OFlashMapRegionVerbTableGuid = H2O_FLASH_MAP_REGION_VERB_TABLE_GUID;

#endif

