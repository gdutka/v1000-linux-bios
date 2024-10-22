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
 * @e \$Revision: 317842 $   @e \$Date: 2015-04-29 15:06:52 +0800 (Wed, 29 Apr 2015) $
 *
 */
/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */

#include <PiPei.h>
#include <Ppi/ApobCommonServicePpi.h>
#include <Library/BaseMemoryLib.h>
#include <Library/BaseLib.h>
#include <Library/AmdBaseLib.h>
#include <Library/IdsLib.h>
#include <Library/ApobSspLib.h>
#include <Filecode.h>

#define FILECODE PSP_APOBDRV_APOBSSPPEI_APOBSSPPEI_FILECODE

STATIC APOB_COMMON_SERVICE_PPI mApobCommonServicePpi = {
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


STATIC EFI_PEI_PPI_DESCRIPTOR mApobCommonServicePpiList =
{
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gApobCommonServicePpiGuid,
  &mApobCommonServicePpi
};


/*++

Routine Description:

  PSP Driver Entry. Initialize PSP device and report DRAM info to PSP once found

Arguments:

Returns:

  EFI_STATUS

--*/
EFI_STATUS
EFIAPI
ApobSspPeiDriverEntry (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS Status;

  IDS_HDT_CONSOLE_PSP_TRACE ("ApobSspPeiDriverEntry >>\n");

  // Public APOB PPI
  IDS_HDT_CONSOLE_PSP_TRACE ("\tInstall PPI\n");
  Status = (**PeiServices).InstallPpi (PeiServices, &mApobCommonServicePpiList);
  if (EFI_ERROR (Status)) {
    IDS_HDT_CONSOLE_PSP_TRACE ("InstallPpi fail\n");
    ASSERT (FALSE);
  }


  IDS_HDT_CONSOLE_PSP_TRACE ("ApobSspPeiDriverEntry <<\n");

  return EFI_SUCCESS;
}


