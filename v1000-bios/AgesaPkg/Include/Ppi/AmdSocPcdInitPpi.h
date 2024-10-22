/*****************************************************************************
 *
 * Copyright (C) 2015-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 **/
#ifndef _AMD_SOC_PCD_INIT_PPI_H_
#define _AMD_SOC_PCD_INIT_PPI_H_

#include <AmdSoc.h>

//----------------------------------------------------
//
//  Amd Agesa Parameter Group Identification
//
//-------------------------------------------------------
typedef
EFI_STATUS
(EFIAPI *AMD_SOC_CALLBACK_PEI)(
  IN CONST EFI_PEI_SERVICES   **PeiServices,
  IN       SOC_ID_STRUCT *SocId,
  OUT      EFI_GUID **PlalformGroupGuid
  );

/// AMD AGESA Group Identification Ppi
typedef struct _PEI_AMD_AGESA_PARAMETER_GROUP_ID_PPI {
  UINTN    Revision;                                                           ///< Revision Number
  AMD_PCD_PLATFORM_GROUP_LIST *AmdConfigurationPcdGroupList;                   ///< AMD AGESA Configuration Parameter Group List.
  AMD_SOC_CALLBACK_PEI AmdSocCallBackPei;                                      ///< SOC driver callback for pass SOC ID to platform
  } PEI_AMD_AGESA_PARAMETER_GROUP_ID_PPI;

// Current PPI revision
#define AMD_AGESA_PARAMETER_GROUP_ID_PPI_REVISION   0x00

//----------------------------------------------------
//
//  AMD PCD Init Ready PPI
//
//-------------------------------------------------------

typedef struct _PEI_AMD_PCD_INIT_READY_PPI {
  UINTN    Revision;               ///< Revision Number
} PEI_AMD_PCD_INIT_READY_PPI;

// Current PPI revision
#define PEI_AMD_PCD_INIT_READY_PPI_REVISION   0x00


extern EFI_GUID gAmdSocAgesaGroupIdentificationPpiGuid;
extern EFI_GUID gAmdPcdInitReadyPpiGuid;

#endif

