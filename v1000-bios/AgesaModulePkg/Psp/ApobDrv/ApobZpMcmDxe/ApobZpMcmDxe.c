/* $NoKeywords:$ */
/**
 * @file
 *
 * APOB driver for ZP and RV SCM system
 *
 * Contains code that public APOB service
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  PSP
 * @e \$Revision$   @e \$Date$
 *
 */
/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */
#include <PiDxe.h>
#include <Library/BaseLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DxeServicesTableLib.h>
#include <Library/AmdBaseLib.h>
#include <Library/IdsLib.h>
#include <Library/ApobZpMcmLib.h>
#include <Protocol/ApobCommonServiceProtocol.h>
#include <Filecode.h>

#define FILECODE PSP_APOBDRV_APOBZPMCMDXE_APOBMCMDXE_FILECODE

STATIC APOB_COMMON_SERVICE_PROTOCOL mApobCommonServiceProtocol = {
  ApobGetApcbInstanceZpMcm,
  ApobGetApcbRecoveryFlagZpMcm,
  ApobGetDimmCfgUpdatedFlagZpMcm,
  NULL,
  ApobGetPhysComplexNumberZpMcm,
  ApobGetPhysCoreNumberZpMcm,
  ApobGetIsThreadEnabledZpMcm,
  ApobGetEccEnableZpMcm,
  ApobGetNvdimmPresentInSystemZpMcm,
  ApobGetNvdimmInfoZpMcm,
  ApobGetMaxDimmsPerChannelZpMcm,
  ApobGetMaxChannelsPerDieZpMcm,
  ApobGetMaxChannelsPerSocketZpMcm,
  ApobGetDimmSmbusInfoZpMcm,
  ApobGetMemClkFreqZpMcm,
  ApobGetDdrMaxRateZpMcm,
  ApobGetMemGenInfoElementZpMcm,
  ApobGetNumberOfValidThresholdsZpMcm,
  ApobGetThresholdsZpMcm
};


EFI_STATUS
EFIAPI
ApobZpMcmDxeDriverEntry (
  IN       EFI_HANDLE         ImageHandle,
  IN       EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS                  Status;
  EFI_HANDLE                  Handle;


  IDS_HDT_CONSOLE_PSP_TRACE ("ApobZpMcmDxeDriverEntry >>\n");
  Handle = NULL;
  //Install APOB Common service Protocol
  Status = gBS->InstallProtocolInterface (
                  &Handle,
                  &gApobCommonServiceProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &mApobCommonServiceProtocol
                  );
  if (EFI_ERROR (Status)) {
    ASSERT (FALSE);
  }

  IDS_HDT_CONSOLE_PSP_TRACE ("ApobZpMcmDxeDriverEntry <<\n");

  return EFI_SUCCESS;
}

