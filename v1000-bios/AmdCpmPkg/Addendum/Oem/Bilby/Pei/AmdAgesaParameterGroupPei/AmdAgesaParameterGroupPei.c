/*****************************************************************************
 *
 * Copyright (C) 2016-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 ******************************************************************************
 */
/*++
Module Name:

  AmdSocAm4Pei.c
  Init Soc interface

Abstract:
--*/
#include <PiPei.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <AmdSoc.h>
#include <Ppi/AmdSocPcdInitPpi.h>
#include "AmdAgesaParameterGroupPei.h"
#include <ChipsetSetupConfig.h>
#include <Library/ChipsetConfigLib.h>
#include <Library/AmdBaseLib.h>
#include <Dxe/AmdPbsSetupDxe/AmdPbsConfig.h>
#include <Library/AmdPbsConfigLib/AmdPbsConfigLib.h>

// AGESA PCDs to be customized, these are samples
AMD_PCD_LIST AmdConfigurationParameterListAm4Br[] = {
  {&gEfiAmdAgesaModulePkgTokenSpaceGuid, TOKEN_ID_PcdBoolSample, (UINT64)TRUE, PCD_BOOL}
};

AMD_PCD_LIST AmdConfigurationParameterListAm4Sm[] = {
  {&gEfiAmdAgesaModulePkgTokenSpaceGuid, TOKEN_ID_PcdBoolSample, (UINT64)TRUE, PCD_BOOL}
};

AMD_PCD_LIST AmdConfigurationParameterListFp4Br[] = {
  {&gEfiAmdAgesaModulePkgTokenSpaceGuid, TOKEN_ID_PcdBoolSample, (UINT64)TRUE, PCD_BOOL}
};

/*++

Routine Description:

  SOC Driver Entry. Initialize SOC device and publish silicon driver installation PPI

Arguments:

Returns:

  EFI_STATUS

--*/
EFI_STATUS
EFIAPI
AmdAgesaParameterGroupPeiInit (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS                           Status = EFI_SUCCESS;
  EFI_PEI_PPI_DESCRIPTOR               *PpiAgesaParameterGroupIdServicePtr;
  AMD_GROUP_LIST_HEADER                *AmdConfigurationParameterGroupListPtr;
  AMD_PCD_PLATFORM_GROUP_LIST          *AmdPcdPlatformGroupListPtr;
  PEI_AMD_AGESA_PARAMETER_GROUP_ID_PPI *AmdAgesaParameterGroupIdPpiPtr;
  AMD_PCD_LIST                         *AmdConfigurationParameterListAm4BrPtr;
  AMD_PCD_LIST                         *AmdConfigurationParameterListAm4SmPtr;
  AMD_PCD_LIST                         *AmdConfigurationParameterListFp4BrPtr;

  DEBUG((DEBUG_INFO, "OEM-PEI-AmdAgesaParameterGroupPeiInit-Start\n"));
  //
  // Allocate memory space for AMD_PCD_LIST
  //
  DEBUG((DEBUG_INFO, "OEM-PEI-AmdAgesaParameterGroupPeiInit-AllocatePool-Size(%X)\n", sizeof (AmdConfigurationParameterListAm4Br)));
  Status = (**PeiServices).AllocatePool (
                             PeiServices,
                             sizeof (AmdConfigurationParameterListAm4Br),
                             &AmdConfigurationParameterListAm4BrPtr
                             );
  if (EFI_ERROR (Status)) {
    DEBUG((DEBUG_INFO, "OEM-PEI-AmdAgesaParameterGroupPeiInit-End-1-Status=%r\n", Status));
    return Status;
  }
  (**PeiServices).SetMem (
                    AmdConfigurationParameterListAm4BrPtr,
                    sizeof (AmdConfigurationParameterListAm4Br),
                    0
                    );
  (**PeiServices).CopyMem ((VOID *) AmdConfigurationParameterListAm4BrPtr, (VOID *) &AmdConfigurationParameterListAm4Br, sizeof (AmdConfigurationParameterListAm4Br));

  DEBUG((DEBUG_INFO, "OEM-PEI-AmdAgesaParameterGroupPeiInit-AllocatePool-Size(%X)\n", sizeof (AmdConfigurationParameterListAm4Sm)));
  Status = (**PeiServices).AllocatePool (
                             PeiServices,
                             sizeof (AmdConfigurationParameterListAm4Sm),
                             &AmdConfigurationParameterListAm4SmPtr
                             );
  if (EFI_ERROR (Status)) {
    DEBUG((DEBUG_INFO, "OEM-PEI-AmdAgesaParameterGroupPeiInit-End-2-Status=%r\n", Status));
    return Status;
  }
  (**PeiServices).SetMem (
                    AmdConfigurationParameterListAm4SmPtr,
                    sizeof (AmdConfigurationParameterListAm4Sm),
                    0
                    );
  (**PeiServices).CopyMem ((VOID *) AmdConfigurationParameterListAm4SmPtr, (VOID *) &AmdConfigurationParameterListAm4Sm, sizeof (AmdConfigurationParameterListAm4Sm));

  DEBUG((DEBUG_INFO, "OEM-PEI-AmdAgesaParameterGroupPeiInit-AllocatePool-Size(%X)\n", sizeof (AmdConfigurationParameterListFp4Br)));
  Status = (**PeiServices).AllocatePool (
                             PeiServices,
                             sizeof (AmdConfigurationParameterListFp4Br),
                             &AmdConfigurationParameterListFp4BrPtr
                             );
  if (EFI_ERROR (Status)) {
    DEBUG((DEBUG_INFO, "OEM-PEI-AmdAgesaParameterGroupPeiInit-End-3-Status=%r\n", Status));
    return Status;
  }
  (**PeiServices).SetMem (
                    AmdConfigurationParameterListFp4BrPtr,
                    sizeof (AmdConfigurationParameterListFp4Br),
                    0
                    );
  (**PeiServices).CopyMem ((VOID *) AmdConfigurationParameterListFp4BrPtr, (VOID *) &AmdConfigurationParameterListFp4Br, sizeof (AmdConfigurationParameterListFp4Br));

  //
  // Customize PCD by reading setup data here if necessary
  //

  //
  // Allocate memory space for AMD_PCD_PLATFORM_GROUP_LIST
  //
  DEBUG((DEBUG_INFO, "OEM-PEI-AmdAgesaParameterGroupPeiInit-AllocatePool-Size(%X)\n", sizeof (AMD_GROUP_LIST_HEADER) * 3));
  Status = (**PeiServices).AllocatePool (
                             PeiServices,
                             sizeof (AMD_GROUP_LIST_HEADER) * 3,  // 3 Groups
                             &AmdConfigurationParameterGroupListPtr
                             );
  if (EFI_ERROR (Status)) {
    DEBUG((DEBUG_INFO, "OEM-PEI-AmdAgesaParameterGroupPeiInit-End-4-Status=%r\n", Status));
    return Status;
  }
  (**PeiServices).SetMem (
                    AmdConfigurationParameterGroupListPtr,
                    sizeof (AMD_GROUP_LIST_HEADER) * 3,
                    0
                    );


  AmdConfigurationParameterGroupListPtr->ListEntry = AmdConfigurationParameterListAm4BrPtr;
  AmdConfigurationParameterGroupListPtr->NumberOfPcdEntries = 1;
  AmdConfigurationParameterGroupListPtr->PlalformGroupGuid = &gAmdConfigurationParameterListAm4BrGuid;
  ((AMD_GROUP_LIST_HEADER *) (AmdConfigurationParameterGroupListPtr + 1))->ListEntry = AmdConfigurationParameterListAm4SmPtr;
  ((AMD_GROUP_LIST_HEADER *) (AmdConfigurationParameterGroupListPtr + 1))->NumberOfPcdEntries = 1;
  ((AMD_GROUP_LIST_HEADER *) (AmdConfigurationParameterGroupListPtr + 1))->PlalformGroupGuid = &gAmdConfigurationParameterListAm4SmGuid;
  ((AMD_GROUP_LIST_HEADER *) (AmdConfigurationParameterGroupListPtr + 2))->ListEntry = AmdConfigurationParameterListFp4BrPtr;
  ((AMD_GROUP_LIST_HEADER *) (AmdConfigurationParameterGroupListPtr + 2))->NumberOfPcdEntries = 1;
  ((AMD_GROUP_LIST_HEADER *) (AmdConfigurationParameterGroupListPtr + 2))->PlalformGroupGuid = &gAmdConfigurationParameterListFp4BrGuid;

  //
  // Allocate memory space for AMD_PCD_PLATFORM_GROUP_LIST
  //
  DEBUG((DEBUG_INFO, "OEM-PEI-AmdAgesaParameterGroupPeiInit-AllocatePool-Size(%X)\n", sizeof (AMD_PCD_PLATFORM_GROUP_LIST)));
  Status = (**PeiServices).AllocatePool (
                             PeiServices,
                             sizeof (AMD_PCD_PLATFORM_GROUP_LIST),
                             &AmdPcdPlatformGroupListPtr
                             );
  if (EFI_ERROR (Status)) {
    DEBUG((DEBUG_INFO, "OEM-PEI-AmdAgesaParameterGroupPeiInit-End-5-Status=%r\n", Status));
    return Status;
  }
  (**PeiServices).SetMem (
                    AmdPcdPlatformGroupListPtr,
                    sizeof (AMD_PCD_PLATFORM_GROUP_LIST),
                    0
                    );
  AmdPcdPlatformGroupListPtr->GroupList = AmdConfigurationParameterGroupListPtr;
  AmdPcdPlatformGroupListPtr->NumberOfGroupsList = 3;
  AmdPcdPlatformGroupListPtr->Version = 0;
  AmdPcdPlatformGroupListPtr->PlalformGroupGuid = &gAmdConfigurationParameterListAm4BrGuid; // Deafult GUID, will be updated in AmdSocCallBackPei

  //
  // Allocate pool for PEI_AMD_AGESA_PARAMETER_GROUP_ID_PPI
  //
  DEBUG((DEBUG_INFO, "OEM-PEI-AmdAgesaParameterGroupPeiInit-AllocatePool-Size(%X)\n", sizeof (PEI_AMD_AGESA_PARAMETER_GROUP_ID_PPI)));
  Status = (**PeiServices).AllocatePool (
                             PeiServices,
                             sizeof (PEI_AMD_AGESA_PARAMETER_GROUP_ID_PPI),
                             &AmdAgesaParameterGroupIdPpiPtr
                             );
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, ">>ERR: Failed to allocate pool!\n"));
    DEBUG((DEBUG_INFO, "OEM-PEI-AmdAgesaParameterGroupPeiInit-End-6-Status=%r\n", Status));
    return Status;
  }
  (**PeiServices).SetMem (
                    AmdAgesaParameterGroupIdPpiPtr,
                    sizeof (PEI_AMD_AGESA_PARAMETER_GROUP_ID_PPI),
                    0
                    );

  AmdAgesaParameterGroupIdPpiPtr->Revision = 0;
  AmdAgesaParameterGroupIdPpiPtr->AmdConfigurationPcdGroupList = AmdPcdPlatformGroupListPtr;
  AmdAgesaParameterGroupIdPpiPtr->AmdSocCallBackPei = (AMD_SOC_CALLBACK_PEI) AmdSocCallBackPei;

  //
  // Allocate pool for PEI_AMD_AGESA_PARAMETER_GROUP_ID_PPI
  //
  DEBUG((DEBUG_INFO, "OEM-PEI-AmdAgesaParameterGroupPeiInit-AllocatePool-Size(%X)\n", sizeof (EFI_PEI_PPI_DESCRIPTOR)));
  Status = (**PeiServices).AllocatePool (
                             PeiServices,
                             sizeof (EFI_PEI_PPI_DESCRIPTOR),
                             &PpiAgesaParameterGroupIdServicePtr
                             );
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, ">>ERR: Failed to allocate pool!\n"));
    DEBUG((DEBUG_INFO, "OEM-PEI-AmdAgesaParameterGroupPeiInit-End-7-Status=%r\n", Status));
    return Status;
  }
  (**PeiServices).SetMem (
                    PpiAgesaParameterGroupIdServicePtr,
                    sizeof (EFI_PEI_PPI_DESCRIPTOR),
                    0
                    );

  PpiAgesaParameterGroupIdServicePtr->Flags = EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST;
  PpiAgesaParameterGroupIdServicePtr->Guid = &gAmdSocAgesaGroupIdentificationPpiGuid;
  PpiAgesaParameterGroupIdServicePtr->Ppi = AmdAgesaParameterGroupIdPpiPtr;

  DEBUG ((EFI_D_ERROR, "***************PEI AGESA Parameter Group Identification Driver Entry***************\n"));

  // Publish Agesa Parameter Group Identification Ppi
  // this ppi need to be installed before SOC driver
  DEBUG((DEBUG_INFO, "OEM-PEI-AmdAgesaParameterGroupPeiInit-InstallPpi-gAmdSocAgesaGroupIdentificationPpiGuid\n"));
  Status = (*PeiServices)->InstallPpi (
                               PeiServices,
                               PpiAgesaParameterGroupIdServicePtr
                               );
  ASSERT_EFI_ERROR (Status);

  DEBUG ((EFI_D_ERROR, "***************PEI AGESA Parameter Group Identification Driver Exit***************\n"));
  DEBUG((DEBUG_INFO, "OEM-PEI-AmdAgesaParameterGroupPeiInit-End-8-Status=%r\n", Status));
  return (Status);
}

/*---------------------------------------------------------------------------------------*/
/**
 *
 * This function use for change AGESA configuration parameter PCD for multiplatform support.
 *
 * It will prpare AGESA configuration group list and return to SOC driver.
 * then the SOC driver will update the PCD value based on the list provide by this function.
 *
 * @param[in]   PeiServices
 *
 */
EFI_STATUS
EFIAPI
AmdSocCallBackPei (
  IN CONST EFI_PEI_SERVICES   **PeiServices,
  IN       SOC_ID_STRUCT *SocId,
  OUT      EFI_GUID **PlalformGroupGuid
  )
{
  EFI_STATUS                  Status;

  DEBUG((DEBUG_INFO, "OEM-PEI-AmdSocCallBackPei-Start\n"));
  Status = EFI_SUCCESS;
  *PlalformGroupGuid = NULL;

  if (SocId->SocFamilyID == F17_ZP_RAW_ID && SocId->PackageType == ZP_PKG_AM4) {
    DEBUG((DEBUG_INFO, "OEM-PEI-AmdSocCallBackPei-SocFamilyID=F17_ZP_RAW_ID\n"));
    DEBUG((DEBUG_INFO, "OEM-PEI-AmdSocCallBackPei-PackageType=ZP_PKG_AM4\n"));
    *PlalformGroupGuid = &gAmdConfigurationParameterListAm4SmGuid;
  } else if (SocId->SocFamilyID == F15_BR_RAW_ID && SocId->PackageType == BR_PKG_AM4) {
    DEBUG((DEBUG_INFO, "OEM-PEI-AmdSocCallBackPei-SocFamilyID=F15_BR_RAW_ID\n"));
    DEBUG((DEBUG_INFO, "OEM-PEI-AmdSocCallBackPei-PackageType=BR_PKG_AM4\n"));
    *PlalformGroupGuid = &gAmdConfigurationParameterListAm4BrGuid;
  } else if (SocId->SocFamilyID == F15_BR_RAW_ID && SocId->PackageType == BR_PKG_FP4) {
    DEBUG((DEBUG_INFO, "OEM-PEI-AmdSocCallBackPei-SocFamilyID=F15_BR_RAW_ID\n"));
    DEBUG((DEBUG_INFO, "OEM-PEI-AmdSocCallBackPei-PackageType=BR_PKG_FP4\n"));
    *PlalformGroupGuid = &gAmdConfigurationParameterListFp4BrGuid;
  }

  DEBUG((DEBUG_INFO, "OEM-PEI-AmdSocCallBackPei-End\n"));
  return Status;
}
