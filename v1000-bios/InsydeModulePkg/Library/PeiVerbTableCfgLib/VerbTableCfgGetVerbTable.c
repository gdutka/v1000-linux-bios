/** @file
  Implementation of GetVerbTable() ,which get and parse verbtable binary in flash region.
  This lib is associated with user defined module GenVerbTableRegionData.

;******************************************************************************
;* Copyright (c) 2016 - 2017, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "VerbTableCfgGetVerbTable.h"

/**
  find matched region by board id, and contruct a COMMON_CHIPSET_AZALIA_VERB_TABLE
  If match not found, upate VerbTableHeaderDataAddress to NULL

  @param[out] *VerbTableHeaderDataAddress  A pointer to common verbtable entry.

  @retval EFI_MEDIA_CHANGED                Get common verbtable entry success.
  @retval EFI_NOT_FOUND                    Support board not found.
**/

EFI_STATUS
GetVerbTable (
  OUT COMMON_CHIPSET_AZALIA_VERB_TABLE      **VerbTableHeaderDataAddress
  )
{
  COMMON_CHIPSET_AZALIA_VERB_TABLE *AzaliaVerbTable;
  EFI_STATUS Status;
  UINT8      Instance;
  UINTN      BoardId;
  VOID       *VerbTableRaw;
  UINT16     DwordOffset;
  UINT8      RegionId[FDM_ENTRY_REGION_ID_SIZE];
  UINT64     RegionOffset;
  UINT64     RegionSize;
  UINT32     Attribs;   
  UINT8      Index;
  UINT8      NumberOfCodecs;
  COMMON_VERB_TABLE_BIN_HEADER  *CommonVerbTableBinHeader;
                        
  AzaliaVerbTable = NULL;
  Status = EFI_NOT_FOUND;
  Instance = 1;
  VerbTableRaw = NULL;
  DwordOffset  = 0;
  BoardId      = 0xff;
  Index        = 0;
  NumberOfCodecs = 0;
  CommonVerbTableBinHeader = NULL;

  BoardId = LibPcdGetSku ();

  if (BoardId == 0xff) {
    *VerbTableHeaderDataAddress = NULL;
    return EFI_NOT_FOUND;
  }

  //
  //  Enumerate each verbtable RegionType  in FDM
  //
  while (TRUE) {

    Status = FdmGetNAt (
      &gH2OFlashMapRegionVerbTableGuid,
      Instance,
      RegionId,
      &RegionOffset,
      &RegionSize,
      &Attribs
      );

    if (EFI_ERROR (Status)) {
      *VerbTableHeaderDataAddress = NULL;
      return Status;
    }

    for (Index = 0; Index < SUPPORT_BOARD_COUNT; Index++) {

      if (((H2O_FLASH_MAP_VERB_TABLE_ID *)(RegionId))->Boards[Index] == 0xff) {
        break;
      }

      //
      //  First detected binary with default board id 0 will be returned anyway.
      //
      if ((((H2O_FLASH_MAP_VERB_TABLE_ID *)(RegionId))->Boards[Index] == (UINT8)BoardId) || 
          (((H2O_FLASH_MAP_VERB_TABLE_ID *)(RegionId))->Boards[Index] == DEFAULT_BOARD_VALUE)) {
        //
        //  VerbTableRaw : a pointer to matched verbtable binary entry.
        //
        VerbTableRaw = (VOID *)(UINTN)(FdmGetBaseAddr () + RegionOffset);

        //
        //  First byte of verbtable binary indicate total codec numbers.
        //  Ex. If there are two codecs exist, then two pairs of common table required to store pointers to its header and data,
        //      plus extra one to store NULL pointer.
        //
        //  NOTE: This pool is allocated to store pointers to meet the requirement of chipset init code.
        //        (COMMON_CHIPSET_AZALIA_VERB_TABLE is a common table to store pointers point to verbtable header and data)
        //        If pointer lengh in pei is 32bit long, in most of cases, the pool cost about 32bit * 4 ~ 32bit * 6 (16byte ~ 24byte).
        //        Memorymanagement in pei phase could be different from other phases, user need to careffully control the usage of memory.
        //        Any Violation of this function can resualt in unexpected memory issues.
        //        Caller has responsibility to free this pool when COMMON_CHIPSET_AZALIA_VERB_TABLE is nolonger used
        //        and the verb setting process in chipset init is finished.
        //
        NumberOfCodecs = *((UINT8 *)VerbTableRaw);
        AzaliaVerbTable = AllocateZeroPool (sizeof (COMMON_CHIPSET_AZALIA_VERB_TABLE) * (NumberOfCodecs + 1));

        if (AzaliaVerbTable == NULL) {
          *VerbTableHeaderDataAddress = NULL;
          Status = EFI_OUT_OF_RESOURCES;
          return Status;
        }

        //
        //  Binary format :
        //              ---------------------------------------------
        //       1Byte  |      Number of codecs                     | 
        //              ---------------------------------------------
        //       14Byte |   COMMON_CHIPSET_AZALIA_VERB_TABLE_HEADER |  --
        //              ---------------------------------------------   |
        //        2Byte |      Data size in Dword                   |   | codec1
        //              ---------------------------------------------   |
        //    non-fixed |      Verb table data                      |  --
        //              ---------------------------------------------
        //       14Byte |   COMMON_CHIPSET_AZALIA_VERB_TABLE_HEADER |  --
        //              ---------------------------------------------   |
        //        2Byte |      Data size in Dword                   |   | codec2
        //              ---------------------------------------------   |
        //    non-fixed |      Verb table data                      |  --
        //              ---------------------------------------------
        //              |                  ...                      |  -- codec n
        //

        //
        //  Move pointer to header of first codec.
        //
        CommonVerbTableBinHeader = (VOID *)(((UINT8 *)VerbTableRaw) + 1);
        //
        //  Install address of each header and data into COMMON_CHIPSET_AZALIA_VERB_TABLE.
        //
        for (Index = 0; Index < NumberOfCodecs; Index++) {
          AzaliaVerbTable[Index].VerbTableHeader = (VOID *)CommonVerbTableBinHeader;
          AzaliaVerbTable[Index].VerbTableData   = (VOID *)(CommonVerbTableBinHeader + 1);
          DwordOffset = CommonVerbTableBinHeader->DataSizeDword;
          //
          //  Move pointer to header of next codec.
          //
          CommonVerbTableBinHeader = CommonVerbTableBinHeader + 1;
          CommonVerbTableBinHeader = (VOID *)(((UINT32 *)CommonVerbTableBinHeader) + DwordOffset);
        }
        
        //
        //  The last pair must be NULL
        //
        AzaliaVerbTable[NumberOfCodecs].VerbTableHeader = NULL;
        AzaliaVerbTable[NumberOfCodecs].VerbTableData   = NULL;
        *VerbTableHeaderDataAddress = AzaliaVerbTable;

        return EFI_MEDIA_CHANGED;
      }
    }
    Instance++;
  }

}
