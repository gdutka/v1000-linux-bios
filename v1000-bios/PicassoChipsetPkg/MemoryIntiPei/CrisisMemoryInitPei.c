/** @file
 Cpu Install Efi Memory PEIM for Crisis recovery phase. 
 
***************************************************************************
* Copyright (c) 2016, Insyde Software Corp. All Rights Reserved.
*
* You may not reproduce, distribute, publish, display, perform, modify, adapt,
* transmit, broadcast, present, recite, release, license or otherwise exploit
* any part of this publication in any form, by any means, without the prior
* written permission of Insyde Software Corporation.
*
******************************************************************************
*/

#include "InstallPeiMemoryInternal.h"

/**
 This is the declaration of Crisis Memory Init PEIM entry point. 
 
 @param[in]         FileHandle          Handle of the file being invoked.
 @param[in]         PeiServices         Describes the list of possible PEI Services.

 @retval            EFI_SUCCESS         The operation completed successfully.
 @retval            Others              An unexpected error occurred.
*/
EFI_STATUS
EFIAPI
CrisisMemoryInitPeiEntryPoint (
  IN       EFI_PEI_FILE_HANDLE     FileHandle,
  IN CONST EFI_PEI_SERVICES        **PeiServices
  )
{
  EFI_STATUS                        Status;
  EFI_BOOT_MODE                     BootMode;

  //
  // Obtain boot mode.
  //
  Status = PeiServicesGetBootMode (&BootMode);
  ASSERT_EFI_ERROR (Status);

  if (BootMode == BOOT_IN_RECOVERY_MODE) {
    POST_CODE (PEI_MEMORY_INIT_FOR_CRISIS);

    // Disable WB cache Setting in Variable MTRR.
    // AmdInitPostPeim has finished the sizing/Resume jobs and Setup WB cache for the DRAM found.
    // Disable it before InstallEfiMemory/InstallS3Memory 
    // This is to prevent the new stack switched by PEI kernel remains on cache.
    // Once the Cache is teared down, these new stack data will be lost. 

    //
    // Check T-Seg
    //
    if (AsmReadMsr64 (TOP_MEM) == 0) {
      ReportMemoryError (PeiServices, 0);
    }

    //
    // Install EFI memory HOBs
    //
    Status = InstallEfiMemory (PeiServices, BootMode);
    ASSERT_EFI_ERROR (Status);

    POST_CODE (PEI_SWITCH_STACK);
    return EFI_SUCCESS;
  }

  return EFI_UNSUPPORTED;
}