/*****************************************************************************
 *
 * Copyright (C) 2015-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef _AMD_SOC_FP5_RV_PEI_H_
#define _AMD_SOC_FP5_RV_PEI_H_

extern EFI_GUID gAmdErrorLogDepexPpiGuid;
extern EFI_GUID gAmdNbioRVEarlyPhaseDepexPpiGuid;
extern EFI_GUID gAmdFchSandstoneDepexPpiGuid;
extern EFI_GUID gAmdFchSSSmbusDepexPpiGuid;
extern EFI_GUID gAmdPspPeiV2DepexPpiGuid;
extern EFI_GUID gAmdMemAm4PeimZpDepexPpiGuid;
extern EFI_GUID gAmdMemPeimDepexPpiGuid;
extern EFI_GUID gAmdNbioBaseRVPeiDepexPpiGuid;
extern EFI_GUID gAmdNbioSmuV10PeiDepexPpiGuid;
extern EFI_GUID gAmdNbioPcieRVPeiDepexPpiGuid;
extern EFI_GUID gAmdNbioIOMMUZPPeiDepexPpiGuid;
extern EFI_GUID gAmdCcxZenRvDepexPpiGuid;
extern EFI_GUID gAmdFabricRvDepexPpiGuid;
extern EFI_GUID gAmdMemSmbiosV2RvPpiGuid;
extern EFI_GUID gAmdMemChanXLatZpDepexPpiGuid;
extern EFI_GUID gAmdMemoryPeiHobRvDepexPpiGuid;
extern EFI_GUID gAmdMemRestoreDepexPpiGuid;
extern EFI_GUID gApobZpRvPeiDepexPpiGuid;

//
// Silicon Driver installation list
//

EFI_GUID * SiliconDriverPpiList[] = {
  &gAmdErrorLogDepexPpiGuid,
  &gAmdNbioRVEarlyPhaseDepexPpiGuid,
  &gAmdFchSandstoneDepexPpiGuid,
  &gAmdFchSSSmbusDepexPpiGuid,
  &gAmdPspPeiV2DepexPpiGuid,
  &gAmdMemAm4PeimZpDepexPpiGuid,
  &gAmdMemPeimDepexPpiGuid,
  &gAmdNbioBaseRVPeiDepexPpiGuid,
  &gAmdNbioSmuV10PeiDepexPpiGuid,
  &gAmdNbioPcieRVPeiDepexPpiGuid,
  &gAmdNbioIOMMUZPPeiDepexPpiGuid,
  &gAmdCcxZenRvDepexPpiGuid,
  &gAmdFabricRvDepexPpiGuid,
  &gAmdMemSmbiosV2RvPpiGuid,
  &gAmdMemChanXLatZpDepexPpiGuid,
  &gAmdMemoryPeiHobRvDepexPpiGuid,
  &gAmdMemRestoreDepexPpiGuid,
  &gApobZpRvPeiDepexPpiGuid
};

UINT8 SiliconDriverPpiListNumber = (UINT8) (sizeof (SiliconDriverPpiList) / sizeof (SiliconDriverPpiList[0]));


#endif // _AMD_SOC_FP5_RV_PEI_H_


