/*****************************************************************************
 *
 * Copyright (C) 2016-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 ******************************************************************************
 */
#include <Uefi.h>
#include <Library/DebugLib.h>
#include <AMD.h>
//#include <Library/AmdHeapLib.h>
//#include <Library/GnbSmuInitV8Lib.h>
#include <Library/PciExpressLib.h>
//#include <Dxe/AmdPbsSetupDxe/AmdPbsConfig.h>
//#include <Library/AmdPbsConfigLib/AmdPbsConfigLib.h>

#pragma pack (push, 1)

#include <FchPT.h>
//#include <FchImc.h>


/*********************************************************************************
 * Name: FchPlatformPTDxeInit
 *
 * Description
 *   Platform hook to update PT parameters
 *
 * Input
 *   FchPTData : a pointer to PT data structure
 *
 * Output
 *
 *
 *********************************************************************************/
EFI_STATUS
EFIAPI
FchPlatformPTDxeInit (
  IN      VOID                          *FchPTData
  )
{
//  FCH_PT                *Promontory;

//  Promontory  = (FCH_PT *)FchPTData;

  return EFI_SUCCESS;
}

/*********************************************************************************
 * Name: FchPlatformOemDxeInit
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
FchPlatformOemDxeInit (
  IN      VOID                          *FchData
  )
{
  return EFI_SUCCESS;
}

#pragma pack (pop)

