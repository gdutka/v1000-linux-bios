/** @file
  This driver provides IHISI interface in SMM mode

;*******************************************************************************
;* Copyright (c) 2015, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;*******************************************************************************
*/

#include "Fbts.h"
#include <Library/VariableLib.h>

#include <Protocol/SmmVariable.h>
#include <Protocol/H2OIhisiFbtsToolVersionProtocol.h>

#include <Library/IoLib.h>
#include <Library/SmmOemSvcKernelLib.h>

#include <Guid/DebugMask.h>
#include <Guid/AmdMemoryContextSaveRestoreData.h>
#include <Guid/AmdMemoryContextSaveRestoreVariable.h>
//[-start-221013-IB14860855-add]//
#include "ApcbTokensUpdate.h"
//[-end-221013-IB14860855-add]//

#define EFI_SYSTEM_NV_DATA_HOB_GUID \
  {0xd6e5092d, 0xc7b2, 0x4872, {0xaf, 0x66, 0xfd, 0xc0, 0xe6, 0xf9, 0x5e, 0x78}}

EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL       *mSmmFwBlockService;
#define FBTSAPHOOK_SUPPORT        204

STATIC
IHISI_REGISTER_TABLE
CHIPSET_FBTS_REGISTER_TABLE[] = {
  //
  // AH=17h
  //
  { FBTSGetRomFileAndPlatformTable, "S17Cs_GetPlatformTb", GetRomFileAndPlatformTable}, \

  //
  // AH=1Ch
  //
  { FBTSGetATpInformation, "S1CCs_GetATpInfo000", FbtsGetATpInformation}, \

  //
  // AH=1Fh
  //
  { FBTSApHookPoint, "S1FCs_ApHookForBios", FbtsApHookForBios}, \

//[-start-221013-IB14860855-add]//
  //
  // AH=52h
  //
  { UveVariableConfirm, "S52Cs_ApcbSync", ApcbSyncCallback}
//[-end-221013-IB14860855-add]//

};

/**
  Check the flash region whether is used.

  @param[in] FlashRegion  Flash Region x Register (x = 0 - 3).

  @retval TRUE            This  region is used
  @retval FALSE           This  region is not used
**/
BOOLEAN
CheckFlashRegionIsValid (
  IN       UINT32    FlashRegion
  )
{
  BOOLEAN         Flag = TRUE;

  //
  // the Region Base must be programmed to 0x1FFFh and the Region Limit
  // to 0x0000h to disable the region.
  //
  // Now, the tool programmed 0x0fff to base and 0x000 to limit to indicate
  // this is region is not used.
  //

  //
  //The limit block address must be greater than or equal to the base block address
  //
  if ((FlashRegion & 0x1fff) > (FlashRegion >> 16 & 0x1fff)) {
    Flag = FALSE;
  }
  return Flag;
}

/**
  Get Flash table from Rom file.
  if DescriptorMode is true, the FlashTable will be filled.
  if the descriptor is false,the FlashTable will be filled RegionTypeEos(0xff) directly.

  @param[in]  InputDataBuffer    the pointer to Rom file.
  @param[in]  DataBuffer         IN: the input buffer address.
                                 OUT:the flash region table from rom file.
  @param[in]  DescriptorMode     TRUE is indicate this Rom file is descriptor mode
                                 FALSE is indicate this Rom file is non-descriptor mode

  @retval EFI_SUCCESS            Successfully returns
**/
EFI_STATUS
GetRomFileFlashTable (
  IN       UINT8           *InputDataBuffer,
  IN OUT   FLASH_REGION    *DataBuffer,
  IN       BOOLEAN         DescriptorMode
  )
{
  UINT32          Frba;
  UINT32          Fmba;
  UINT8           Index;
  UINT32          ReadAccess;
  UINT32          WriteAccess;
  UINT32          FlashMap0Reg;
  UINT32          FlashMap1Reg;
  UINT32         *FlashRegionPtr;
  UINT32         *FlashMasterPtr;

  //
  //calulate Flash region base address
  //
  FlashMap0Reg = *((UINT32 *)(InputDataBuffer + FLASH_MAP_0_OFFSET));
  Frba = (FlashMap0Reg & FLASH_REGION_BASE_MASK) >> 12;
  FlashRegionPtr = (UINT32 *)(InputDataBuffer + Frba);
  //
  //calulate Flash master base address
  //
  FlashMap1Reg = *((UINT32 *)(InputDataBuffer + FLASH_MAP_1_OFFSET));
  Fmba = (FlashMap1Reg & FLASH_MASTER_BASE_MASK) << 4;
  FlashMasterPtr = (UINT32 *)(InputDataBuffer + Fmba);

  if (DescriptorMode) {

    for (Index = DESC_REGION; Index < MAX_FLASH_REGION; Index++,  FlashRegionPtr++) {

      if (CheckFlashRegionIsValid (*FlashRegionPtr)){
        DataBuffer->Type = Index;
        DataBuffer->Offset = (*FlashRegionPtr & 0x1fff) << 12;
        DataBuffer->Size = ((*FlashRegionPtr >> 16 & 0x1fff) - (*FlashRegionPtr & 0x1fff) + 1) << 12;
        //
        //Bios primary master always has access permissions to it¡¦s primary region
        //
        if (Index == BIOS_REGION) {
          DataBuffer->Access = ACCESS_AVAILABLE;
        } else {
          ReadAccess = (*FlashMasterPtr >> (FLASH_MASTER_1_READ_ACCESS_BIT + Index)) & ACCESS_AVAILABLE;
          WriteAccess = (*FlashMasterPtr >> (FLASH_MASTER_1_WRITE_ACCESS_BIT + Index)) & ACCESS_AVAILABLE;
          if (ReadAccess == ACCESS_AVAILABLE && WriteAccess == ACCESS_AVAILABLE) {
            DataBuffer->Access = ACCESS_AVAILABLE;
          } else {
            DataBuffer->Access = ACCESS_NOT_AVAILABLE;
          }
        }

        DataBuffer++;
      }
    }
  }
  DataBuffer->Type = REGION_TYPE_OF_EOS;
  return EFI_SUCCESS;
}

/**
  AH = 1Ch
  Passing information to flash program on whether
  if current BIOS (AT-p capable) can be flashed with
  other BIOS ROM file
  AT-p: (Anti-Theft PC Protection).

  @retval EFI_SUCCESS   Success returns.
**/
EFI_STATUS
FbtsGetATpInformation (
  VOID
  )
{
  UINT8                                 Ecx_data = 0;

  //
  // Platform is NOT in stolen mode.
  //
  Ecx_data |= bit(1);
  //
  // Current BIOS is not ATp Capable
  //
  Ecx_data &= ~(bit(7));
  //
  // Return ECX for tool to determin proper error message.
  // Please refer to IHISI spec.
  //
  mH2OIhisi->WriteCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RCX, Ecx_data);

  return EFI_SUCCESS;
}

/**
  AH = 17h , Get platform and Rom file flash descriptor region.

  @retval EFI_SUCCESS   Success returns.
**/
EFI_STATUS
GetRomFileAndPlatformTable (
  VOID
  )
{
//[-start-170705-IB10300174-modify]//
  EFI_STATUS      Status;
  FLASH_REGION   *FlashRegionPtr;
  UINT8          *OutputDataBuffer;

  Status = EFI_UNSUPPORTED;
  OutputDataBuffer = (UINT8 *)(UINTN) mH2OIhisi->ReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RDI);

  //
  // AMD Platform don't report Intel region type
  //
  FlashRegionPtr = (FLASH_REGION *)OutputDataBuffer;
  FlashRegionPtr->Type = REGION_TYPE_OF_EOS;

  OutputDataBuffer += ROM_FILE_REGION_OFFSET;
  FlashRegionPtr = (FLASH_REGION *)OutputDataBuffer;
  FlashRegionPtr->Type = REGION_TYPE_OF_EOS;

  return IHISI_SUCCESS;
//[-end-170705-IB10300174-modify]//
}
/**
  This call back function will be invoked several times during flash process.
  BIOS can know which step is running now.
  BIOS can base on it to do specific hook such as EC idle and weak up.

       CL - denote the start of AP process.
       CL = 0x00, AP terminate. (Before IHISI 0x16)
       CL = 0x01, AP start. (After IHISI 0x10)
       CL = 0x02, Start to read ROM. (Before IHISI 0x14)
       CL = 0x03, Start to write ROM. (Before IHISI 0x15)
       CL = 0x04, Start to write EC. (Before IHISI 0x20)
       CL = 0x05, Before dialog popup.
       CL = 0x06, After dialog close and continue running.

  @param

  @retval EFI_SUCCESS
**/
EFI_STATUS
FbtsApHookForBios (
  VOID
  )
{
  EFI_STATUS                            Status;
  UINT8                                 ApState;
  UINT32                                Eax;
  H2O_IHISI_FBTS_TOOL_VERSION_PROTOCOL  *ToolVersionProtocol = NULL;
  UINT16                                IhisiVersion;

  Status = gSmst->SmmLocateProtocol (
                    &gH2OIhisiFbtsToolVersionProtocolGuid,
                    NULL,
                    (VOID**)&ToolVersionProtocol
                    );

  if (EFI_ERROR (Status)) {
    IhisiVersion = 0;
  } else {
    IhisiVersion = ToolVersionProtocol->IhisiVersion;
  }

//  if (PcdGet16 (PcdIhisiFbtsToolVersion) < FBTSAPHOOK_SUPPORT) {
  if (IhisiVersion < FBTSAPHOOK_SUPPORT) {
    //
    // Return AL for funtion success
    //
    Eax = mH2OIhisi->ReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RAX);
    Eax = (UINT32) ((Eax & 0xffffff00) | (UINT32) IhisiSuccess);
    mH2OIhisi->WriteCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RAX, Eax);
    return EFI_SUCCESS;
  }
  ApState = (UINT8) (mH2OIhisi->ReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RCX));  // CL

  switch (ApState) {
    case 0x00:
      //
      // Here provided a hook before AP terminate.
      //
      if (PcdGetBool (PcdEcSharedFlashSupported) && !PcdGetBool (PcdEcIdlePerWriteBlockSupported)) {
        OemSvcEcIdle (FALSE);
      }
      break;

    case 0x01:
      //
      // Here provided a hook before AP start.
      //

      break;

    case 0x02:
      //
      // Here provided a hook before FbtsRead.
      //
      if (PcdGetBool (PcdEcSharedFlashSupported) && !PcdGetBool (PcdEcIdlePerWriteBlockSupported)) {
        OemSvcEcIdle (TRUE);
      }
      break;

    case 0x03:
      //
      // Here provided a hook before FbtsWrite.
      //
      if (PcdGetBool (PcdEcSharedFlashSupported) && !PcdGetBool (PcdEcIdlePerWriteBlockSupported)) {
        OemSvcEcIdle (TRUE);
      }
      break;

    case 0x04:
      //
      // Here provided a hook before FetsWrite.
      //
      if (PcdGetBool (PcdEcSharedFlashSupported) && !PcdGetBool (PcdEcIdlePerWriteBlockSupported)) {
        OemSvcEcIdle (TRUE);
      }
      break;

    case 0x05:
      //
      // Here provided a hook before dialog popup. (Winflash)
      //
      if (PcdGetBool (PcdEcSharedFlashSupported) && !PcdGetBool (PcdEcIdlePerWriteBlockSupported)) {
        OemSvcEcIdle (FALSE);
      }
      break;

    case 0x06:
      //
      // Here provided a hook after dialog close and continue running. (Winflash)
      //

      break;
  }

  //
  // Return AL for funtion success
  //

  Eax = mH2OIhisi->ReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RAX);
  Eax = (UINT32) ((Eax & 0xffffff00) | (UINT32) IhisiSuccess);
  mH2OIhisi->WriteCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RAX, Eax);

  Status = OemSvcIhisiS1FHookFbtsApHookForBios (ApState);
  DEBUG ((EFI_D_ERROR | EFI_D_INFO, "SmmOemSvcChipsetLib OemSvcIhisiS1FHookFbtsApHookForBios, Status : %r\n", Status));

  return EFI_SUCCESS;

}

/**
  Initialize Fbts relative services

  @retval EFI_SUCCESS        Initialize Fbts services successful.
  @return Others             Any error occurred.
**/
EFI_STATUS
FbtsInit (
  VOID
  )
{
  EFI_STATUS   Status;
  IHISI_REGISTER_TABLE   *SubFuncTable;
  UINT16                  TableCount;

  Status = gSmst->SmmLocateProtocol (
                     &gEfiSmmFwBlockServiceProtocolGuid,
                     NULL,
                     (VOID**)&mSmmFwBlockService
                     );

  ASSERT_EFI_ERROR (Status);

  SubFuncTable = CHIPSET_FBTS_REGISTER_TABLE;
  TableCount = sizeof(CHIPSET_FBTS_REGISTER_TABLE)/sizeof(CHIPSET_FBTS_REGISTER_TABLE[0]);
  Status = RegisterIhisiSubFunction (SubFuncTable, TableCount);
  if (EFI_ERROR(Status)) {
    ASSERT_EFI_ERROR (Status);
  }

  return Status;
}

