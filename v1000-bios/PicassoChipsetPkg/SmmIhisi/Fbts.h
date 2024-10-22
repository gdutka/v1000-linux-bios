/** @file

;*******************************************************************************
;* Copyright (c) 2015, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;*******************************************************************************
*/

#ifndef _IHISI_FBTS_H_
#define _IHISI_FBTS_H_

#include "IhisiSmm.h"
#include <ChipsetSetupConfig.h>
#include <Library/IoLib.h>
#include <Library/FdSupportLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Protocol/SmmFwBlockService.h>

#define EFI_CACHE_VARIABLE_MTRR_BASE  0x200
#define EFI_CACHE_MTRR_VALID          0x800

typedef struct {
  UINT8      IhisiAreaType;
  UINT8      FlashAreaType;
} FLASH_MAP_MAPPING_TABLE;

#define FLASH_VALID_SIGNATURE           0x0FF0A55A      //Flash Descriptor is considered valid
#define FLASH_MAP_0_OFFSET              0x04
#define FLASH_REGION_BASE_MASK          0x00ff0000
#define FLASH_MAP_1_OFFSET              0x08
#define FLASH_MASTER_BASE_MASK          0x000000ff
#define PLATFORM_REGION_OFFSET          0x00
#define ROM_FILE_REGION_OFFSET          0x100
#define FLASH_MASTER_1_READ_ACCESS_BIT  16
#define FLASH_MASTER_1_WRITE_ACCESS_BIT 24
#define TDT_STATE_INACTIVE              0x0
#define TDT_STATE_ACTIVE                0x01
#define TDT_STATE_LOCK_OR_STOLEN        0x02
#define DOS_FLASH_TOOL                  0x01
#define WINDOWS_FLASH_TOOL              0x02
#define LINUX_FLASH_TOOL                0x03
#define SHELL_FLASH_TOOL                0x04

//
// Function Prototype
//

/**
  Passing information to flash program on whether
  if current BIOS (AT-p capable) can be flashed with
  other BIOS ROM file
  AT-p: (Anti-Theft PC Protection).

  @retval EFI_SUCCESS   Success returns.
**/
EFI_STATUS
FbtsGetATpInformation (
  VOID
  );

/**
  Get platform and Rom file flash descriptor region.

  @retval EFI_SUCCESS   Success returns.
**/
EFI_STATUS
GetRomFileAndPlatformTable (
  VOID
  );

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
  );

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
  );

/**
  Initialize Fbts relative services

  @retval EFI_SUCCESS        Initialize Fbts services successful.
  @return Others             Any error occurred.
**/
EFI_STATUS
FbtsInit (
  VOID
  );

#endif
