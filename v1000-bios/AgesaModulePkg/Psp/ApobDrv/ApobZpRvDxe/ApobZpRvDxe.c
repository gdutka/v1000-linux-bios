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
#include <Library/ApobZpRvLib.h>
#include <Protocol/ApobCommonServiceProtocol.h>
#include <Filecode.h>

#define FILECODE PSP_APOBDRV_APOBZPRVDXE_APOBZPRVDXE_FILECODE

STATIC APOB_COMMON_SERVICE_PROTOCOL mApobCommonServiceProtocol = {
  ApobGetApcbInstanceZpRv,
  ApobGetApcbRecoveryFlagZpRv,
  ApobGetDimmCfgUpdatedFlagZpRv,
  NULL,
  ApobGetPhysComplexNumberZpRv,
  ApobGetPhysCoreNumberZpRv,
  ApobGetIsThreadEnabledZpRv,
  ApobGetEccEnableZpRv,
  ApobGetNvdimmPresentInSystemZpRv,
  ApobGetNvdimmInfoZpRv,
  ApobGetMaxDimmsPerChannelZpRv,
  ApobGetMaxChannelsPerDieZpRv,
  ApobGetMaxChannelsPerSocketZpRv,
  ApobGetDimmSmbusInfoZpRv,
  ApobGetMemClkFreqZpRv,
  ApobGetDdrMaxRateZpRv,
  ApobGetMemGenInfoElementZpRv,
  ApobGetNumberOfValidThresholdsZpRv,
  ApobGetThresholdsZpRv
};


EFI_STATUS
EFIAPI
ApobZpRvDxeDriverEntry (
  IN       EFI_HANDLE         ImageHandle,
  IN       EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS                  Status;
  EFI_HANDLE                  Handle;


  IDS_HDT_CONSOLE_PSP_TRACE ("ApobZpRvDxeDriverEntry >>\n");
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

  IDS_HDT_CONSOLE_PSP_TRACE ("ApobZpRvDxeDriverEntry <<\n");

  return EFI_SUCCESS;
}

