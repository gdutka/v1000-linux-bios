/*****************************************************************************
 *
 * Copyright (C) 2015-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */
/*++
Module Name:

  AmdSocFp5RvPei.c
  Init Soc interface

Abstract:
--*/

#include <Filecode.h>
#include "AGESA.h"
#include <Library/CcxSetMmioCfgBaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/AmdSocBaseLib.h>
#include <Library/PeiSocLogicalIdServicesLib.h>
#include <Library/AmdIdsHookLib.h>
#include <Library/FabricResourceInitLib.h>
#include <Library/PeiSocBistLoggingLib.h>
#include <Ppi/AmdSocPpi.h>
#include <Ppi/AmdSocPcdInitPpi.h>
#include <Ppi/AmdCcxPpi.h>
#include "AmdSocFp5RvPei.h"

#define FILECODE SOC_AMDSOCFP5RVPEI_AMDSOCFP5RVPEI_FILECODE

EFI_STATUS
EFIAPI
SocAfterPeiInitCompleteCallback (
  IN        EFI_PEI_SERVICES                **PeiServices,
  IN        EFI_PEI_NOTIFY_DESCRIPTOR       *NotifyDesc,
  IN        VOID                            *InvokePpi
  );

STATIC PEI_AMD_SOC_PPI mAmdSocInstallPpiTemplate = {
  AMD_SOC_PPI_REVISION,
};

STATIC PEI_AMD_PCD_INIT_READY_PPI mAmdPcdInitReadyPpi = {
  PEI_AMD_PCD_INIT_READY_PPI_REVISION,
};

STATIC EFI_PEI_PPI_DESCRIPTOR mAmdPcdInitReady = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gAmdPcdInitReadyPpiGuid,
  &mAmdPcdInitReadyPpi
};

EFI_PEI_NOTIFY_DESCRIPTOR   mAfterCcxPeiInitCompletePpiCallback = {
  (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gAmdCcxPeiInitCompletePpiGuid,
  SocAfterPeiInitCompleteCallback
};

EFI_STATUS
SocPublishSiliconPpi (
  IN CONST EFI_PEI_SERVICES    **PeiServices,
  IN CONST EFI_GUID            *SiliconDriverId
  )
{
  EFI_STATUS    Status = EFI_SUCCESS;
  EFI_PEI_PPI_DESCRIPTOR    *SocPpiDescriptorTemplate;

  // Allocate memory for the PPI descriptor
  Status = (*PeiServices)->AllocatePool (
                             PeiServices,
                             sizeof (EFI_PEI_PPI_DESCRIPTOR),
                             (VOID **)&SocPpiDescriptorTemplate
                             );
  ASSERT_EFI_ERROR ( Status);

  SocPpiDescriptorTemplate->Flags = (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST);
  SocPpiDescriptorTemplate->Guid  = (EFI_GUID*)SiliconDriverId;
  SocPpiDescriptorTemplate->Ppi   = &mAmdSocInstallPpiTemplate;

  Status = (*PeiServices)->InstallPpi (
                             PeiServices,
                             SocPpiDescriptorTemplate
                             );
  return (Status);
}

/*++

Routine Description:

  SOC Driver Entry. Initialize SOC device and publish silicon driver installation PPI

Arguments:

Returns:

  EFI_STATUS

--*/
EFI_STATUS
EFIAPI
AmdSocFp5RvPeiInit (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  UINT8               i;
  SOC_ID_STRUCT       SocId;
  SOC_ID_STRUCT       SocId2;
  SOC_ID_STRUCT       SocId3;
  EFI_STATUS          Status = EFI_SUCCESS;
  PEI_AMD_AGESA_PARAMETER_GROUP_ID_PPI *PeiAmdAgesaParameterGroupIdPpi;
  AMD_PCD_PLATFORM_GROUP_LIST *AmdPcdPlatformGroupList;
  EFI_GUID *PlatformGroupGuid = NULL;

  DEBUG ((EFI_D_ERROR, "*****************************PEI SOC FP5 RV Driver Entry*********************\n"));

  // Check Hardware Identification
  SocId.SocFamilyID = F17_RV_RAW_ID;
  SocId.PackageType = RV_FP5;

  SocId2.SocFamilyID = F17_RV2_RAW_ID;
  SocId2.PackageType = RV_FP5;

  SocId3.SocFamilyID = F17_PIC_RAW_ID;
  SocId3.PackageType = RV_FP5;

  if (SocHardwareIdentificationCheck (&SocId) || SocHardwareIdentificationCheck (&SocId2) || SocHardwareIdentificationCheck (&SocId3)) {

    // Init PCIE Configuration Space
    CcxSetMmioCfgBaseLib ();
    // Init DF resource
    FabricResourceInit ();

    //
    // Parameter initailization
    //
    Status = (*PeiServices)->LocatePpi (
                PeiServices,
                &gAmdSocAgesaGroupIdentificationPpiGuid,
                0,
                NULL,
                (VOID**)&PeiAmdAgesaParameterGroupIdPpi
                );

    if (EFI_SUCCESS == Status) {
      AmdPcdPlatformGroupList = PeiAmdAgesaParameterGroupIdPpi->AmdConfigurationPcdGroupList;

      //Pass SOC ID to platform BIOS before PCD init.
      Status = PeiAmdAgesaParameterGroupIdPpi->AmdSocCallBackPei (PeiServices, &SocId, &PlatformGroupGuid);
      if (NULL == PlatformGroupGuid) {
        PlatformGroupGuid = AmdPcdPlatformGroupList->PlatformGroupGuid;
      }

      //find Group Agesa configuration list
      for (i = 0; i < AmdPcdPlatformGroupList->NumberOfGroupsList; i++) {
        if (CompareGuid (PlatformGroupGuid, (EFI_GUID *)AmdPcdPlatformGroupList->GroupList[i].PlatformGroupGuid)) {
          // Update PCD
          AmdPcdInit (AmdPcdPlatformGroupList->GroupList[i].NumberOfPcdEntries, (AMD_PCD_LIST *)AmdPcdPlatformGroupList->GroupList[i].ListEntry);
        }
      }
    }

    //
    // Publish the logical ID protocol
    //
    Status = SocLogicalIdServicePpiInstall (PeiServices);

    //
    //Publish Silicon Driver Installation PPI
    //
    Status = (*PeiServices)->InstallPpi (
                             PeiServices,
                             &mAmdPcdInitReady
                             );
    ASSERT_EFI_ERROR (Status);

    IDS_HOOK (IDS_HOOK_BEGINNING_OF_AGESA, NULL, NULL);

    //
    //Publish Silicon Driver Installation PPI
    //
    for (i = 0 ; i < SiliconDriverPpiListNumber; i++) {
      SocPublishSiliconPpi (PeiServices, SiliconDriverPpiList[i]);
    }

    (**PeiServices).NotifyPpi (PeiServices, &mAfterCcxPeiInitCompletePpiCallback);
  }

  DEBUG ((EFI_D_ERROR, "*****************************PEI SOC FP5 RV Driver Exit*********************\n"));
  return (Status);
}

/*++

Routine Description:

  Callback routine once gAmdCcxPeiInitCompletePpiGuid has been installed to ensure the following services have been published
  - Core Topology Services
  - Fabric Topology Services
  - Nbio SMU Services

Arguments:
  **PeiServices
  *NotifyDesc
  *InvokePpi

Returns:

  EFI_STATUS

--*/
EFI_STATUS
EFIAPI
SocAfterPeiInitCompleteCallback (
  IN        EFI_PEI_SERVICES                **PeiServices,
  IN        EFI_PEI_NOTIFY_DESCRIPTOR       *NotifyDesc,
  IN        VOID                            *InvokePpi
  )
{

  LogBistStatus ((CONST EFI_PEI_SERVICES**)PeiServices);

  return EFI_SUCCESS;
}

