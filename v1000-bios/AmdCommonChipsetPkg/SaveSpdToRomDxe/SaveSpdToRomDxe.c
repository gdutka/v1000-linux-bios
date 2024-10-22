/** @file
  SaveSpdToRomDxe module save SPD data in ROM and other modules can get SPD data
  from ROM instead of SMBus to reduce POST time

;*******************************************************************************
;* Copyright (c) 2013 - 2017, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;*******************************************************************************
*/

#include <Guid/DimmSmbusAddrHob.h>

#include <Library/HobLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/FdSupportLib.h>
#include <Library/SmbusLib.h>

#include <IndustryStandard/SdramSpd.h>

#include <Ddr3SpdRegs.h>
#include <DimmSmbusAddrHobData.h>

#define MAX_DIMMS               FixedPcdGet8  (PcdMaxDimms)
#define MAX_SPD_DATA_SIZE       FixedPcdGet16 (PcdSizeOfSpd)
#define SPD_DDR4_SDRAM          12
/**
  Check the SPD data in ROM is different from current SPD data.
  If different, write SPD Data to ROM.

  @param [in] ImageHandle     ImageHandle of the loaded driver
  @param [in] SystemTable     Pointer to the System Table

  @retval EFI_SUCCESS         SpdInRomDone Protocol created

**/
EFI_STATUS
SaveSpdToRomEntryPoint (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_HOB_GUID_TYPE                   *GuidHob;
  VOID                                *DataInHob;
  UINT8                               *DimmSmbusAddrArray;
  EFI_STATUS                          Status;
  EFI_STATUS                          DummyStatus;
  UINT8                               *SpdInMemory;
  BOOLEAN                             *DimmExist;
  BOOLEAN                             *DimmSameAsPrevious;
  UINT8                               i;
  UINT8                               *SpdInRom;
  EFI_PHYSICAL_ADDRESS                BaseAddress;
  EFI_PHYSICAL_ADDRESS                Dimm0Address;
  UINTN                               SmbusOffset;
  UINT8                               SpdMemoryType;
  UINT8                               *SpdChkList;
  UINT16                              *SpdDDR4ChkList;  
  UINT8                               SpdChkListSize;
  UINTN                               Index;
  BOOLEAN                             DimmIsDifferent;
  UINTN                               BufferSize;
  UINT8                               *Dest;
  UINT8                               *Src;
  DIMM_INFO_SMBUS                     *DimmInfoSmbusArray;
  UINT8                               SmbusAddress;

  DimmSmbusAddrArray = NULL;
  SpdChkList = NULL;
  SpdChkListSize = 0;
  DimmIsDifferent = FALSE;
  DimmInfoSmbusArray = NULL;

  //
  // 1. Get DimmSmbusAddrArray from HOB
  //
  GuidHob = GetFirstGuidHob (&gDimmSmbusAddrHobGuid);
  if (GuidHob != NULL) {
    DataInHob = GET_GUID_HOB_DATA (GuidHob);
    DimmSmbusAddrArray = (UINT8*) DataInHob;
    DimmInfoSmbusArray = (DIMM_INFO_SMBUS*)(UINTN)(DimmSmbusAddrArray + MAX_DIMMS);
  }

  //
  // 2. Allocate memory to save SPD data, Allocate memory for Dimm Exist flag.
  //
  SpdInMemory = AllocatePool (MAX_SPD_DATA_SIZE * MAX_DIMMS);
  SetMem (SpdInMemory, (MAX_SPD_DATA_SIZE * MAX_DIMMS), 0xFF);

  DimmExist = AllocatePool (sizeof (BOOLEAN) * MAX_DIMMS);
  ZeroMem (DimmExist, (sizeof (BOOLEAN) * MAX_DIMMS));

  DimmSameAsPrevious = AllocatePool (sizeof (BOOLEAN) * MAX_DIMMS);
  ZeroMem (DimmSameAsPrevious, (sizeof (BOOLEAN) * MAX_DIMMS));

  //
  // 3. SPD Data process
  //
  BaseAddress = FixedPcdGet32 (PcdFlashSpdDataStoreBase);
  Dimm0Address = FixedPcdGet8 (PcdSmbusAddrChA1);

  for (i = 0; i < MAX_DIMMS; i++) {
    SpdInRom = (UINT8 *)(BaseAddress + (MAX_SPD_DATA_SIZE * i));
    if (DimmSmbusAddrArray != NULL) {
      SmbusAddress = (UINT8)(DimmSmbusAddrArray[i]);
    } else {
      SmbusAddress = (UINT8)(Dimm0Address + i * 2);
    }

    if (!DimmInfoSmbusArray[i].DimmPresent) {
      DimmExist [i] = FALSE;
      //
      // Check SpdInRom to see is this DIMM has plugged or not before.
      //
      if (SpdInRom [SPD_MEMORY_TYPE] != 0xFF) {
        DimmSameAsPrevious[i] = FALSE;
        SpdMemoryType = SmBusReadDataByte (SMBUS_LIB_ADDRESS (SMBUS_LIB_SLAVE_ADDRESS (SmbusAddress), SPD_MEMORY_TYPE, 0, FALSE), &Status);
        goto DimmContentFlagCheck;
      } else {
        DimmSameAsPrevious[i] = TRUE;
        continue;
      }
    }
    //
    // 3.1 Read SPD offset 2 to identify what kind of RAM is using (DDR2 or DDR3)
    //
    SmbusOffset = SPD_MEMORY_TYPE;

    SpdInMemory[(SmbusOffset + (MAX_SPD_DATA_SIZE * i))] = SmBusReadDataByte (SMBUS_LIB_ADDRESS (SMBUS_LIB_SLAVE_ADDRESS (SmbusAddress), SmbusOffset, 0, FALSE), &Status);
    //
    // 3.1.2.2 Depend on offset 2 information to read Part number of this DIMM
    //
    SpdMemoryType = SpdInMemory[(SPD_MEMORY_TYPE + (MAX_SPD_DATA_SIZE * i))];

    if (EFI_ERROR (Status)) {
      //
      // 3.1.1.1 Read Byte error, treat there is no DIMM on this address, process next DIMM
      //
      DimmExist[i] = FALSE;
      //
      // 3.1.1.2 Check SpdInRom to see is this DIMM has plugged or not before.
      //
      if (SpdInRom [SPD_MEMORY_TYPE] != 0xFF) {
        DimmSameAsPrevious[i] = FALSE;
      } else {
        DimmSameAsPrevious[i] = TRUE;
      }
      gBS->Stall (10000);

    } else {
      //
      // 3.1.2.1 Ready Byte success, there should be a DIMM on this address
      //
      DimmExist[i] = TRUE;


      Index = 0;
      DimmSameAsPrevious[i] = TRUE;

      switch (SpdMemoryType) {

        case SPD_VAL_DDR2_TYPE:
          SpdChkList = PcdGetPtr (PcdSpdDdr2ChkList);
          SpdChkListSize = 18;
          do {
            SmbusOffset = SpdChkList[Index];
            SpdInMemory[(SmbusOffset + (MAX_SPD_DATA_SIZE * i))] = SmBusReadDataByte (SMBUS_LIB_ADDRESS(SMBUS_LIB_SLAVE_ADDRESS (SmbusAddress), SmbusOffset, 0, FALSE), &Status);
            //
            // 3.1.2.2.1.1 Compare SpdInMemory with SpdInRom to see is this same DIMM or not
            //
            if ((SpdInMemory[(SmbusOffset + (MAX_SPD_DATA_SIZE * i))] != SpdInRom[SmbusOffset]) || EFI_ERROR (Status)) {
              DimmSameAsPrevious[i] = FALSE;
            }
            Index++;
          } while (DimmSameAsPrevious[i] && (Index < SpdChkListSize));
          break;

        case SPD_DDR3_SDRAM:
        default:
          SpdChkList = PcdGetPtr (PcdSpdDdr3ChkList);
          SpdChkListSize = 11;
          do {
            SmbusOffset = SpdChkList[Index];
            SpdInMemory[(SmbusOffset + (MAX_SPD_DATA_SIZE * i))] = SmBusReadDataByte (SMBUS_LIB_ADDRESS(SMBUS_LIB_SLAVE_ADDRESS (SmbusAddress), SmbusOffset, 0, FALSE), &Status);
            //
            // 3.1.2.2.1.1 Compare SpdInMemory with SpdInRom to see is this same DIMM or not
            //
            if ((SpdInMemory[(SmbusOffset + (MAX_SPD_DATA_SIZE * i))] != SpdInRom[SmbusOffset]) || EFI_ERROR (Status)) {
              DimmSameAsPrevious[i] = FALSE;
            }
            Index++;
          } while (DimmSameAsPrevious[i] && (Index < SpdChkListSize));
          break;
          
        case SPD_DDR4_SDRAM:
          SpdDDR4ChkList = PcdGetPtr (PcdSpdDdr4ChkList);
          SpdChkListSize = 11;

          do {
            SmbusOffset = SpdDDR4ChkList[Index];
            if(SmbusOffset <=255) {
              SpdInMemory[(SmbusOffset + (MAX_SPD_DATA_SIZE * i))] = SmBusReadDataByte (SMBUS_LIB_ADDRESS(SMBUS_LIB_SLAVE_ADDRESS (SmbusAddress), SmbusOffset, 0, FALSE), &Status);
            } else {
              SmBusWriteDataByte (SMBUS_LIB_ADDRESS (SMBUS_LIB_SLAVE_ADDRESS (0X6E), 0, 0, FALSE), 0, &DummyStatus); //SPA1
              SpdInMemory[(SmbusOffset + (MAX_SPD_DATA_SIZE * i))] = SmBusReadDataByte (SMBUS_LIB_ADDRESS(SMBUS_LIB_SLAVE_ADDRESS (SmbusAddress), SmbusOffset - 256, 0, FALSE), &Status);
              SmBusWriteDataByte (SMBUS_LIB_ADDRESS (SMBUS_LIB_SLAVE_ADDRESS (0X6C), 0, 0, FALSE), 0, &DummyStatus); //SPA0
            }
            // 
            // 3.1.2.2.1.1 Compare SpdInMemory with SpdInRom to see is this same DIMM or not
            //
            if ((SpdInMemory[(SmbusOffset + (MAX_SPD_DATA_SIZE * i))] != SpdInRom[SmbusOffset]) || EFI_ERROR (Status)) {
              DimmSameAsPrevious[i] = FALSE;
            }
            Index++;
          } while (DimmSameAsPrevious[i] && (Index < SpdChkListSize));
          break;             
      }
    }
    //
    // 3.2 Not the same with previous one, read whole SPD data in SpdInMemory
    //
DimmContentFlagCheck:
    if (!DimmSameAsPrevious[i]) {
      //
      // 3.2.1 Mark DimmIsDifferent flag so we will program SPD data to ROM later
      //
      DimmIsDifferent = TRUE;

      if (DimmExist[i]) {
        //
        // 3.2.2.1 DIMM exist, read SPD data from SMBUS
        //
        for (SmbusOffset = 0; SmbusOffset < MAX_SPD_DATA_SIZE; SmbusOffset++) {
          if (SpdMemoryType < SPD_DDR4_SDRAM && SmbusOffset < 256) {
            SpdInMemory[(SmbusOffset + (MAX_SPD_DATA_SIZE * i))] = SmBusReadDataByte (SMBUS_LIB_ADDRESS(SMBUS_LIB_SLAVE_ADDRESS (SmbusAddress), SmbusOffset, 0, FALSE), &Status);
          } else if (SpdMemoryType == SPD_DDR4_SDRAM){
            if (SmbusOffset < 256) {
              SpdInMemory[(SmbusOffset + (MAX_SPD_DATA_SIZE * i))] = SmBusReadDataByte (SMBUS_LIB_ADDRESS(SMBUS_LIB_SLAVE_ADDRESS (SmbusAddress), SmbusOffset, 0, FALSE), &Status);
            } else {
              SmBusWriteDataByte (SMBUS_LIB_ADDRESS (SMBUS_LIB_SLAVE_ADDRESS (0X6E), 0, 0, FALSE), 0, &DummyStatus); //SPA1
              SpdInMemory[(SmbusOffset + (MAX_SPD_DATA_SIZE * i))] = SmBusReadDataByte (SMBUS_LIB_ADDRESS(SMBUS_LIB_SLAVE_ADDRESS (SmbusAddress), SmbusOffset - 256, 0, FALSE), &Status);
              SmBusWriteDataByte (SMBUS_LIB_ADDRESS (SMBUS_LIB_SLAVE_ADDRESS (0X6C), 0, 0, FALSE), 0, &DummyStatus); //SPA0
            }
          }
        }
      } else {
        //
        // 3.2.2.2 DIMM is not exist, set SPD data to all 0xFF
        //
        gBS->SetMem (&SpdInMemory [(MAX_SPD_DATA_SIZE * i)], MAX_SPD_DATA_SIZE, 0xFF);
      }

    } else {
      //
      // 3.2.3 DIMM is the same with previous, copy SpdInRom to SpdInRam in case another DIMM is different.
      //
      gBS->CopyMem (&SpdInMemory [(MAX_SPD_DATA_SIZE * i)], &SpdInRom[0], MAX_SPD_DATA_SIZE);
    }
  }

  //
  // 4. If DIMM is different with previous boot, program new SPD data to ROM.
  //
  if (DimmIsDifferent) {
    //
    // 4.2 Erase SPD_DATA_STORE
    //
    BaseAddress = FixedPcdGet32 (PcdFlashSpdDataStoreBase);
    BufferSize = FixedPcdGet32 (PcdFlashSpdDataStoreSize);
    FlashErase ((UINTN)BaseAddress, (UINTN)BufferSize);

    //
    // 4.3 Program SPD_DATA_STORE
    //
    BufferSize = (MAX_SPD_DATA_SIZE * MAX_DIMMS);
    Dest = (UINT8*)(UINTN)BaseAddress;
    Src = SpdInMemory;
    FlashProgram (Dest, Src, &BufferSize, BaseAddress);

  }


  FreePool (SpdInMemory);
  FreePool (DimmExist);
  FreePool (DimmSameAsPrevious);

  return  EFI_SUCCESS;
}
