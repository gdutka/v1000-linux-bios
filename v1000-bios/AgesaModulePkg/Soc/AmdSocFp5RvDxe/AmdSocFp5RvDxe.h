/*****************************************************************************
 *
 * Copyright (C) 2015-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef _AMD_SOC_FP5_RV_DXE_H_
#define _AMD_SOC_FP5_RV_DXE_H_

extern EFI_GUID gAmdErrorLogDepexProtocolGuid;
extern EFI_GUID gAmdFchSandstoneDepexProtocolGuid;
extern EFI_GUID gAmdFchSSSmbusDepexProtocolGuid;
extern EFI_GUID gAmdFchSSSmmDispacherDepexProtocolGuid;
extern EFI_GUID gAmdFchSSSmmControlDepexProtocolGuid;
extern EFI_GUID gAmdPspDxeV2RvDepexProtocolGuid;
extern EFI_GUID gAmdPspP2CmboxV2DepexProtocolGuid;
extern EFI_GUID gAmdPspSmmV2DepexProtocolGuid;
extern EFI_GUID gAmdPspHstiV2DepexProtocolGuid;
extern EFI_GUID gAmdMemAm4DxeZpDepexProtocolGuid;
extern EFI_GUID gAmdMemAmdDxeDepexProtocolGuid;
extern EFI_GUID gAmdNbioBaseRVDxeDepexProtocolGuid;
extern EFI_GUID gAmdNbioSmuV10DxeDepexProtocol;
extern EFI_GUID gAmdNbioPcieRVDxeDepexProtocolGuid;
extern EFI_GUID gAmdNbioIOMMURVDxeDepexProtocolGuid;
extern EFI_GUID gAmdNbioALIBRVDxeDepexProtocolGuid;
extern EFI_GUID gAmdCcxZenRvDepexProtocolGuid;
extern EFI_GUID gAmdCcxZenRvDepexSmmProtocolGuid;
extern EFI_GUID gAmdFabricRvDepexProtocolGuid;
extern EFI_GUID gAmdMemSmbiosV2DepexProtocolGuid;
extern EFI_GUID gAmdApcbServiceRvDepexProtocolGuid;
extern EFI_GUID gAmdRasRvDepexProtocolGuid;
extern EFI_GUID gAmdMemRestoreDepexProtocolGuid;
extern EFI_GUID gApobZpRvDxeDepexProtocolGuid;

//
// Silicon Driver installation list
//

EFI_GUID * SiliconDriverProtocolList[] = {
  &gAmdErrorLogDepexProtocolGuid,
  &gAmdFchSandstoneDepexProtocolGuid,
  &gAmdFchSSSmbusDepexProtocolGuid,
  &gAmdFchSSSmmDispacherDepexProtocolGuid,
  &gAmdFchSSSmmControlDepexProtocolGuid,
  &gAmdPspDxeV2RvDepexProtocolGuid,
  &gAmdPspP2CmboxV2DepexProtocolGuid,
  &gAmdPspSmmV2DepexProtocolGuid,
  &gAmdPspHstiV2DepexProtocolGuid,
  &gAmdMemAm4DxeZpDepexProtocolGuid,
  &gAmdMemAmdDxeDepexProtocolGuid,
  &gAmdNbioBaseRVDxeDepexProtocolGuid,
  &gAmdNbioSmuV10DxeDepexProtocol,
  &gAmdNbioPcieRVDxeDepexProtocolGuid,
  &gAmdNbioIOMMURVDxeDepexProtocolGuid,
  &gAmdNbioALIBRVDxeDepexProtocolGuid,
  &gAmdCcxZenRvDepexProtocolGuid,
  &gAmdCcxZenRvDepexSmmProtocolGuid,
  &gAmdFabricRvDepexProtocolGuid,
  &gAmdMemSmbiosV2DepexProtocolGuid,
  &gAmdApcbServiceRvDepexProtocolGuid,
  &gAmdRasRvDepexProtocolGuid,
  &gAmdMemRestoreDepexProtocolGuid,
  &gApobZpRvDxeDepexProtocolGuid
};

UINT8 SiliconDriverProtocolListNumber = (UINT8) (sizeof (SiliconDriverProtocolList) / sizeof (SiliconDriverProtocolList[0]));

//
// Functions Prototypes
//
EFI_STATUS
EFIAPI
AmdSocFp5RvDxeInit (
  IN       EFI_HANDLE         ImageHandle,
  IN       EFI_SYSTEM_TABLE   *SystemTable
  );


#endif // _AMD_SOC_FP5_RV_DXE_H_

