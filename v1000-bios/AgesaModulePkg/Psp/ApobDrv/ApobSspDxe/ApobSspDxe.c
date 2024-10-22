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
#include <Library/ApobSspLib.h>
#include <Protocol/ApobCommonServiceProtocol.h>
#include <Filecode.h>

#define FILECODE PSP_APOBDRV_APOBSSPDXE_APOBSSPDXE_FILECODE

STATIC APOB_COMMON_SERVICE_PROTOCOL mApobCommonServiceProtocol = {
  ApobGetApcbInstanceSsp,
  ApobGetApcbRecoveryFlagSsp,
  ApobGetDimmCfgUpdatedFlagSsp,
  ApobGetPhysCcdNumberSsp,
  ApobGetPhysComplexNumberSsp,
  ApobGetPhysCoreNumberSsp,
  ApobGetIsThreadEnabledSsp,
  ApobGetEccEnableSsp,
  ApobGetNvdimmPresentInSystemSsp,
  ApobGetNvdimmInfoSsp,
  ApobGetMaxDimmsPerChannelSsp,
  ApobGetMaxChannelsPerDieSsp,
  ApobGetMaxChannelsPerSocketSsp,
  ApobGetDimmSmbusInfoSsp,
  ApobGetMemClkFreqSsp,
  ApobGetDdrMaxRateSsp,
  ApobGetMemGenInfoElementSsp,
  ApobGetNumberOfValidThresholdsSsp,
  ApobGetThresholdsSsp
};


EFI_STATUS
EFIAPI
ApobSspDxeDriverEntry (
  IN       EFI_HANDLE         ImageHandle,
  IN       EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS                  Status;
  EFI_HANDLE                  Handle;


  IDS_HDT_CONSOLE_PSP_TRACE ("ApobSspDxeDriverEntry >>\n");
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

  IDS_HDT_CONSOLE_PSP_TRACE ("ApobSspDxeDriverEntry <<\n");

  return EFI_SUCCESS;
}

