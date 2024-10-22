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
#include <Library/ApobDummyLib.h>
#include <Protocol/ApobCommonServiceProtocol.h>


STATIC APOB_COMMON_SERVICE_PROTOCOL mApobCommonServiceProtocol = {
  ApobGetApcbInstanceDummy,
  ApobGetApcbRecoveryFlagDummy,
  ApobGetDimmCfgUpdatedFlagDummy,
  NULL,
  ApobGetPhysComplexNumberDummy,
  ApobGetPhysCoreNumberDummy,
  ApobGetIsThreadEnabledDummy,
  ApobGetEccEnableDummy,
  ApobGetNvdimmPresentInSystemDummy,
  ApobGetNvdimmInfoDummy,
  ApobGetMaxDimmsPerChannelDummy,
  ApobGetMaxChannelsPerDieDummy,
  ApobGetMaxChannelsPerSocketDummy,
  ApobGetDimmSmbusInfoDummy,
  ApobGetMemClkFreqDummy,
  ApobGetDdrMaxRateDummy,
  ApobGetMemGenInfoElementDummy,
  ApobGetNumberOfValidThresholdsDummy,
  ApobGetThresholdsDummy
};


EFI_STATUS
EFIAPI
ApobDummyDxeDriverEntry (
  IN       EFI_HANDLE         ImageHandle,
  IN       EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS                  Status;
  EFI_HANDLE                  Handle;


  Handle = NULL;
  //Install APOB Common service Protocol
  Status = gBS->InstallProtocolInterface (
                  &Handle,
                  &gApobCommonServiceProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &mApobCommonServiceProtocol
                  );
  if (EFI_ERROR (Status)) {
    IDS_HDT_CONSOLE_PSP_TRACE ("InstallProtocol fail @ApobDummyDxe\n");
  }


  return EFI_SUCCESS;
}

