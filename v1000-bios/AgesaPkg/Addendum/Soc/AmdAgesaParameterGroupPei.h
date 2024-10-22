/*****************************************************************************
 *
 * Copyright (C) 2015-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
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
extern EFI_GUID gAmdConfigurationParameterListAGuid;
extern EFI_GUID gAmdConfigurationParameterListBGuid;
extern EFI_GUID gAmdConfigurationParameterListCGuid;


#endif // _AMD_AGESA_PARAMETERGROUP_PEI_H_


