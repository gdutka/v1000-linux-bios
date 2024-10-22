/** @file
 Cpu Install EfiMemory PEIM.

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
#include <Ppi/PspSmmResume.h>

/**
 This is the declaration of Cpu Install Efi Memory PEIM entry point.

 @param[in]         FileHandle          Handle of the file being invoked.
 @param[in]         PeiServices         Describes the list of possible PEI Services.

 @retval            EFI_SUCCESS         The operation completed successfully.
 @retval            Others              An unexpected error occurred.
*/
EFI_STATUS
EFIAPI
CpuInstallEfiMemoryPeiEntryPoint (
  IN       EFI_PEI_FILE_HANDLE     FileHandle,
  IN CONST EFI_PEI_SERVICES        **PeiServices
  )
{
  EFI_STATUS                        Status;
  EFI_BOOT_MODE                     BootMode;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI   *VariableServices;
  PSP_SMM_RESUME_PPI                *PspSmmResumePpi;
  BOOLEAN                           SecureS3Enabled;

  DEBUG ((EFI_D_INFO, "CpuInstallEfiMemoryPeiEntryPoint\n"));

  Status = PeiServicesLocatePpi (
                            &gEfiPeiReadOnlyVariable2PpiGuid,
                            0,
                            NULL,
                            (VOID**)&VariableServices
                            );
  ASSERT_EFI_ERROR (Status);

  //
  // Obtain boot mode.
  //
  Status = PeiServicesGetBootMode (&BootMode);
  ASSERT_EFI_ERROR (Status);

  // Disable WB cache Setting in Variable MTRR.
  // AmdInitPostPeim has finished the sizing/Resume jobs and Setup WB cache for the DRAM found.
  // Disable it before InstallEfiMemory/InstallS3Memory
  // This is to prevent the new stack switched by PEI kernel remains on cache.
  // Once the Cache is teared down, these new stack data will be lost.

  //
  // Disable WB variable MTRR created by AGESA
  //
  Status = PeiServicesLocatePpi (
                            &gPspSmmResumePpiGuid,
                            0,
                            NULL,
                            (VOID **)&PspSmmResumePpi
                            );
  SecureS3Enabled = FALSE;
  if (!EFI_ERROR (Status)) {
    SecureS3Enabled = TRUE;
  }

  //
  // Install EFI memory HOBs
  //
  if (BootMode == BOOT_WITH_FULL_CONFIGURATION ||
      BootMode == BOOT_ON_S4_RESUME ||
      BootMode == BOOT_WITH_DEFAULT_SETTINGS ||
      BootMode == BOOT_ASSUMING_NO_CONFIGURATION_CHANGES) {

    POST_CODE (PEI_MEMORY_INSTALL);
    Status = InstallEfiMemory (PeiServices, BootMode);
    ASSERT_EFI_ERROR (Status);
  }

  if (BootMode == BOOT_ON_S3_RESUME) {

    POST_CODE (S3_INSTALL_S3_MEMORY);
    Status = InstallS3Memory (PeiServices, VariableServices);

    if (EFI_ERROR (Status)) {
      //
      // Report S3 resume errors
      //
      REPORT_STATUS_CODE_EX (
        EFI_ERROR_CODE | EFI_ERROR_UNRECOVERED,                     // Type
        EFI_COMPUTING_UNIT_MEMORY | EFI_CU_MEMORY_EC_S3_RESUME_FAIL,// Value
        0,                                                          // Instance
        NULL,                                                       // *CallerId          OPTIONAL
        NULL,                                                       // *ExtendedDataGuid  OPTIONAL
        NULL,                                                       // *ExtendedData      OPTIONAL
        0                                                           // ExtendedDataSize
        );
      //
      // Clean sleep type. Typically maps to S0.
      //
      IoAnd16 (MmioRead16 (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REG62), (UINT16)~(BIT10 + BIT11 + BIT12));
      PeiServicesResetSystem ();
    }
  }

  return EFI_SUCCESS;
}
