/** @file
 Library Instance for PeiChipsetConfigLib Library Class.

;******************************************************************************
;* Copyright (c) 2013 - 2017, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <PiPei.h>
#include <Uefi.h>
#include <Library/PeiServicesLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Ppi/ReadOnlyVariable2.h>
#include <KernelSetupConfig.h>
#include <ChipsetSetupConfig.h>

#define MAX_SETUP_VARIABLE_SIZE 0x400

/**
 Retrieve chipset setup configuration data

 @param[out] ChipsetConfig      Pointer to the structure of CHIPSET_CONFIGURATION,
                                this pointer must be allocated with sizeof(CHIPSET_CONFIGURATION)
                                before being called

 @retval EFI_SUCCESS            The chipset configuration is successfully retrieved
 @retval EFI_INVALID_PARAMETER  NULL pointer for input ChipsetConfig paramater
 @return others                 Failed to retrieve chipset configuration
**/
EFI_STATUS
EFIAPI
GetChipsetConfiguration (
  OUT CHIPSET_CONFIGURATION        *ChipsetConfig
  )
{
  EFI_STATUS                       Status;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI  *VariablePpi;
  UINT8                            SystemConfiguration[MAX_SETUP_VARIABLE_SIZE];
  UINTN                            Size;

  ASSERT (ChipsetConfig != NULL);
  if (ChipsetConfig == NULL) {  
    return EFI_INVALID_PARAMETER;
  }

  Status = PeiServicesLocatePpi (
             &gEfiPeiReadOnlyVariable2PpiGuid,
             0,
             NULL,
             (VOID **) &VariablePpi
             );
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR(Status)) {
    return Status;
  }

  Size = MAX_SETUP_VARIABLE_SIZE;
  Status = VariablePpi->GetVariable (
                          VariablePpi,
                          SETUP_VARIABLE_NAME,
                          &gSystemConfigurationGuid,
                          NULL,
                          &Size,
                          SystemConfiguration
                          );
  if (Status == EFI_SUCCESS) {
    CopyMem (ChipsetConfig, SystemConfiguration, PcdGet32 (PcdSetupConfigSize));
  }

  return Status;
}
