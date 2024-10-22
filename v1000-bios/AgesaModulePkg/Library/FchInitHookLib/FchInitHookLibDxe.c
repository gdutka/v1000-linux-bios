/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */
#include <Uefi.h>
#include <FchPT.h>
#include <FchImc.h>

/*********************************************************************************
 * Name: FchPlatformImcDxeInit
 *
 * Description
 *   Platform hook to update Imc parameters
 *
 * Input
 *   FchImcData : a pointer to IMC data structure
 *
 * Output
 *
 *
 *********************************************************************************/
EFI_STATUS
EFIAPI
FchPlatformImcDxeInit (
  IN      VOID                          *FchImcData
  )
{
  FCH_IMC                *pFchImc;

  pFchImc  = (FCH_IMC *)FchImcData;

  return EFI_SUCCESS;
}

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
  FCH_PT                *Promontory;

  Promontory  = (FCH_PT *)FchPTData;

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

