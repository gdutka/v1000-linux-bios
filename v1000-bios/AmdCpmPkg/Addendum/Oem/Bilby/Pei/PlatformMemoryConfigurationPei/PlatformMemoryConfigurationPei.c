/*****************************************************************************
 *
 * Copyright (C) 2016-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 ******************************************************************************
 */
#include <PiPei.h>

//
// Libraries
//
#include <Library/PeiServicesLib.h>
#include <Library/DebugLib.h>

/**
 This is the declaration of PEI Platform Memory Configuration PEIM entry point.

 @param[in]         FileHandle          Handle of the file being invoked.
 @param[in]         PeiServices         Describes the list of possible PEI Services.

 @retval            EFI_SUCCESS         The operation completed successfully.
 @retval            Others              An unexpected error occurred.
*/
EFI_STATUS
EFIAPI
PlatformMemoryConfigurationPeiEntryPoint (
  IN EFI_PEI_FILE_HANDLE             FileHandle,
  IN CONST EFI_PEI_SERVICES        **PeiServices
  )
{
  DEBUG((DEBUG_INFO, "OEM-PEI-PlatformMemoryConfigurationPeiEntryPoint-Start\n"));

  DEBUG((DEBUG_INFO, "OEM-PEI-PlatformMemoryConfigurationPeiEntryPoint-End\n"));
  return EFI_SUCCESS;
}
