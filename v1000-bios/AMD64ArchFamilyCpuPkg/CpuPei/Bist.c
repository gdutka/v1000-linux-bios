/** @file
Implementation of CPU driver for PEI phase.

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "CpuPei.h"
#include "Bist.h"

EFI_SEC_PLATFORM_INFORMATION_PPI mSecPlatformInformationPpi = {
  SecPlatformInformation
};

EFI_PEI_PPI_DESCRIPTOR mPeiSecPlatformInformationPpi = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gEfiSecPlatformInformationPpiGuid,
  &mSecPlatformInformationPpi
};

/**

  Implementation of the PlatformInformation service
  
  Implementation of the PlatformInformation service in EFI_SEC_PLATFORM_INFORMATION_PPI.
  This function will parse SecPlatform Information from BIST Hob.

  @param  PeiServices                Pointer to the PEI Services Table.
  @param  StructureSize              Pointer to the variable describing size of the input buffer.
  @param  PlatformInformationRecord  Pointer to the EFI_SEC_PLATFORM_INFORMATION_RECORD.
  
  @retval EFI_SUCCESS                The data was successfully returned.
  @retval EFI_BUFFER_TOO_SMALL       The buffer was too small.
  
**/
EFI_STATUS
EFIAPI
SecPlatformInformation (
  IN CONST EFI_PEI_SERVICES                  **PeiServices,
  IN OUT UINT64                              *StructureSize,
     OUT EFI_SEC_PLATFORM_INFORMATION_RECORD *PlatformInformationRecord
  )
{
  EFI_HOB_GUID_TYPE       *GuidHob;
  VOID                    *DataInHob;
  UINTN                   DataSize;

  GuidHob = GetFirstGuidHob (&gEfiHtBistHobGuid);
  if (GuidHob == NULL) {
    *StructureSize = 0;
    return EFI_SUCCESS;
  }

  DataInHob = GET_GUID_HOB_DATA (GuidHob);
  DataSize  = GET_GUID_HOB_DATA_SIZE (GuidHob);

  //
  // return the information from BistHob
  //
  if ((*StructureSize) < (UINT64) DataSize) {
    *StructureSize = (UINT64) DataSize;
    return EFI_BUFFER_TOO_SMALL;
  }

  *StructureSize = (UINT64) DataSize;
   
  CopyMem (PlatformInformationRecord, DataInHob, DataSize);

  return EFI_SUCCESS;
}

/**
  A callback function to build CPU(only BSP) BIST. 

  This function is a callback function to build bsp's BIST by calling SecPlatformInformationPpi

  @param  PeiServices      Pointer to PEI Services Table      
  @param  NotifyDescriptor Address if the notification descriptor data structure 
  @param  Ppi              Address of the PPI that was installed     
  @retval EFI_SUCCESS      Retrieve of the BIST data successfully 
  @retval EFI_SUCCESS      No sec platform information ppi export   
  @retval EFI_SUCCESS      The boot mode is S3 path   
**/
EFI_STATUS
EFIAPI
BuildBistAndCpuInfoHob (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  )
{
  EFI_STATUS                            Status;
  EFI_BOOT_MODE                         BootMode;
  EFI_SEC_PLATFORM_INFORMATION_PPI      *SecPlatformInformationPpi;
  UINT64                                InformationSize;
  EFI_SEC_PLATFORM_INFORMATION_RECORD   *SecPlatformInformation;
  EFI_PEI_PPI_DESCRIPTOR                *SecInformationDescriptor;

  Status = PeiServicesGetBootMode(&BootMode);
  ASSERT_EFI_ERROR (Status);

  //
  // locate the SEC platform information PPI
  //
  Status = PeiServicesLocatePpi (
             &gEfiSecPlatformInformationPpiGuid, // GUID
             0,                                  // INSTANCE
             &SecInformationDescriptor,          // EFI_PEI_PPI_DESCRIPTOR
             (VOID **)&SecPlatformInformationPpi // PPI
             );

  if (Status == EFI_NOT_FOUND) {
    return EFI_SUCCESS;
  }

  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // We do not build CPU Hobs on S3, because the data are only needed by DXE drivers.
  // But we still reinstall the Ppi to prevent system hang issue if someone want to locate it.
  //
  if (BootMode != BOOT_ON_S3_RESUME) {
    //
    // Get the size of the sec platform information(BIST data)
    //
    InformationSize         = 0;
    SecPlatformInformation  = NULL;
    Status = SecPlatformInformationPpi->PlatformInformation (
                                          (CONST EFI_PEI_SERVICES **) PeiServices,
                                          &InformationSize,
                                          SecPlatformInformation
                                          );
    if (Status == EFI_BUFFER_TOO_SMALL) {
      Status = PeiServicesAllocatePool (
                                (UINTN) InformationSize,
                                (VOID **) &SecPlatformInformation
                                );

      if (EFI_ERROR (Status)) {
        return Status;
      }
      //
      // Retrieve BIST data
      //
      Status = SecPlatformInformationPpi->PlatformInformation (
                                            (CONST EFI_PEI_SERVICES **) PeiServices,
                                            &InformationSize,
                                            SecPlatformInformation
                                            );
    }

    if (EFI_ERROR (Status)) {
      return Status;
    }

    BuildGuidDataHob (
      &gEfiHtBistHobGuid,
      SecPlatformInformation,
      (UINTN) InformationSize
    );

    BuildGuidDataHob (
      &gEfiSecPlatformInformationPpiGuid,
      SecPlatformInformation,
      (UINTN) InformationSize
    );
  }

  //
  // The old SecPlatformInformation Ppi is on CAR.
  // After memory discovered, we should never get it from CAR, or the data will be crashed.
  // So, we reinstall SecPlatformInformation PPI here.
  //
  Status = PeiServicesReInstallPpi (
                             SecInformationDescriptor,
                             &mPeiSecPlatformInformationPpi
                             );
  return Status;
}
