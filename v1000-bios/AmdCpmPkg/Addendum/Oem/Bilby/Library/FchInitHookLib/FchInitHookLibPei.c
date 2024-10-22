/*****************************************************************************
 *
 * Copyright (C) 2016-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 ******************************************************************************
 */
#include <Uefi.h>
#pragma pack (push, 1)

#include <FchPT.h>
#include <Library/DebugLib.h>
#include <Library/PciLib.h>

/*---------------------------------------------------------------------------------------*/
/**
 * Check RV OPN number
 *
 * @retval    TRUE    This is RV B4 part
 *            FALSE   This is not RV B4 part
 *
 */
BOOLEAN
CheckRvOpnB4 (
  )
{
  UINT32              TempData32;

  TempData32 =  (PcdGet32 (PcdGetRVFamilyOPN) >> 30) & 0x3;

  if (TempData32 == 2) {
    return TRUE;
  } else if (TempData32 == 3) {
    return TRUE;
  }
  return FALSE;
}

/*********************************************************************************
 * Name: FchPlatformPTPeiInit
 *
 * Description
 *   Platform hook to update PT parameters
 *
 * Input
 *   FchPTData : a pointer to PT data structure
 *
 * Output
 *   none if the reset is successful
 *
 *********************************************************************************/
EFI_STATUS
EFIAPI
FchPlatformPTPeiInit (
  IN      VOID                          *FchPTData
  )
{
  FCH_PT                *Promontory;

  Promontory  = (FCH_PT *)FchPTData;

  return EFI_SUCCESS;
}

/*********************************************************************************
 * Name: FchPlatformOemPeiInit
 *
 * Description
 *   Platform Oem hook
 *
 * Input
 *   FchData
 *
 * Output
 *
 *
 *********************************************************************************/
EFI_STATUS
EFIAPI
FchPlatformOemPeiInit (
  IN      VOID                          *FchData
  )
{
  DEBUG ((DEBUG_INFO, "OEM-PEI-FchPlatformOemPeiInit-Start\n"));
  if (CheckRvOpnB4 ()) {
    DEBUG((DEBUG_INFO, "OEM-PEI-FchPlatformOemPeiInit-Set PcdXhci1Disable31Port to TRUE\n"));
    PcdSetBool (PcdXhci1Disable31Port, TRUE);
  }
  DEBUG ((DEBUG_INFO, "OEM-PEI-FchPlatformOemPeiInit-End\n"));
  return EFI_SUCCESS;
}

#pragma pack (pop)

