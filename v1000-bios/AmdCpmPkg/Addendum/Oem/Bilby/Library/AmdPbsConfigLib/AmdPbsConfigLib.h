/*****************************************************************************
 *
 * Copyright (C) 2015-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 ******************************************************************************
 */

/**
 Retrieve AMD PBS setup configuration data

 @param[out] AmdPbsConfig       Pointer to the structure of AMD_PBS_SETUP_OPTION,
                                this pointer must be allocated with sizeof(AMD_PBS_SETUP_OPTION)
                                before being called

 @retval EFI_SUCCESS            The AMD PBS configuration is successfully retrieved
 @retval EFI_INVALID_PARAMETER  NULL pointer for input AmdPbsConfig paramater
 @return others                 Failed to retrieve AMD PBS configuration
**/
EFI_STATUS
EFIAPI
GetAmdPbsConfiguration (
  OUT AMD_PBS_SETUP_OPTION        *AmdPbsConfiguration
  );


  /**
   Set AMD PBS setup configuration data in Variable, only avaliable in DXE
  
   @param[out] AmdPbsConfiguration Pointer to the structure of AMD_PBS_SETUP_OPTION,
                                  this pointer must be allocated with sizeof(AMD_PBS_SETUP_OPTION)
                                  before being called
  
   @retval EFI_SUCCESS            The AMD PBS configuration is successfully retrieved
   @retval EFI_INVALID_PARAMETER  NULL pointer for input KernelConfig paramater
   @return others                 Failed to retrieve kernel configuration
  **/
EFI_STATUS
EFIAPI
SetAmdPbsConfiguration (
  IN AMD_PBS_SETUP_OPTION        *AmdPbsConfiguration
  );

