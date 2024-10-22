/*****************************************************************************
 *
 * Copyright (C) 2016-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 ******************************************************************************
 */

#ifndef _AMD_AGESA_PARAMETERGROUP_PEI_H_
#define _AMD_AGESA_PARAMETERGROUP_PEI_H_

EFI_STATUS
EFIAPI
AmdSocCallBackPei (
  IN CONST EFI_PEI_SERVICES   **PeiServices,
  IN       SOC_ID_STRUCT *SocId,
  OUT      EFI_GUID **PlalformGroupGuid
);


extern EFI_GUID gAmdSocAgesaGroupIdentificationPpiGuid;
extern EFI_GUID gEfiAmdAgesaPkgTokenSpaceGuid;
extern EFI_GUID gEfiAmdAgesaModulePkgTokenSpaceGuid;
extern EFI_GUID gAmdConfigurationParameterListAm4BrGuid;
extern EFI_GUID gAmdConfigurationParameterListAm4SmGuid;
extern EFI_GUID gAmdConfigurationParameterListFp4BrGuid;

#define         TOKEN_ID_PcdBoolSample  0xFF000   // This should be the same value defined in .dec file, and the PCD type should be PcdDynamicEx

#endif // _AMD_AGESA_PARAMETERGROUP_PEI_H_

