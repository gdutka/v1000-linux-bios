/* $NoKeywords:$ */
/**
 * @file
 *
 * AmdNbioPcieRVDxe Driver
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  AmdNbioPcieRVDxe
 * @e \$Revision: 312065 $   @e \$Date: 2015-01-30 04:23:05 -0600 (Fri, 30 Jan 2015) $
 *
 */
/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */
#include <PiDxe.h>
#include <Filecode.h>
#include <Library/AmdBaseLib.h>
#include <Library/IdsLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/AmdNbioBaseServicesProtocol.h>
#include <Protocol/AmdAcpiCratServicesProtocol.h>
#include <Guid/GnbNbioBaseZPInfoHob.h>
#include <Library/UefiLib.h>
#include <Library/HobLib.h>
#include <Guid/HobList.h>

#define FILECODE NBIO_NBIOBASE_AMDNBIOBASERVDXE_AMDNBIOBASERVDXE_FILECODE

/*----------------------------------------------------------------------------------------
 *                         E X T E R N   D E F I N I T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */

EFI_STATUS
EFIAPI
IGpuGetCratHsaProcEntry (
  IN       AMD_IGPU_ACPI_CRAT_SERVICES_PROTOCOL   *This,
  IN OUT   CRAT_HSA_PROCESSING_UNIT               *CratHsaEntry
  );

#define GPU__GC__NUM_SE 1
#define GPU__GC__NUM_SH_PER_SE 1
#define GPU__GC__NUM_CU_PER_SH 11

STATIC AMD_IGPU_ACPI_CRAT_SERVICES_PROTOCOL   mIGpuCratServicesProtocol = {
  0,
  IGpuGetCratHsaProcEntry
};

/**
 * This service retrieves CRAT information about the HSA.
 *
 * @param[in]      This                             A pointer to the
 *                                                  AMD_IGPU_ACPI_CRAT_SERVICES_PROTOCOL instance.
 * @param[in, out] CratHsaEntry                     Contains HSA processor information
 *
 * @retval EFI_SUCCESS                              The HSA processor information was successfully retrieved.
 * @retval EFI_INVALID_PARAMETER                    CratHsaProcInfo is NULL.
 *
 **/
EFI_STATUS
EFIAPI
IGpuGetCratHsaProcEntry (
  IN       AMD_IGPU_ACPI_CRAT_SERVICES_PROTOCOL   *This,
  IN OUT   CRAT_HSA_PROCESSING_UNIT               *CratHsaEntry
  )
{

  if (PcdGetBool (PcdCfgIommuSupport) == TRUE) {
    CratHsaEntry->Flags.HSAmmuPresent = 1;
  }

  CratHsaEntry->Flags.Enabled = 1;
  CratHsaEntry->Flags.GpuPresent = 1;
  CratHsaEntry->NumSIMDPerCU = 4;
  CratHsaEntry->NumSIMDCores = GPU__GC__NUM_SE * GPU__GC__NUM_SH_PER_SE * GPU__GC__NUM_CU_PER_SH * (CratHsaEntry->NumSIMDPerCU);
  CratHsaEntry->MaxWavesSIMD = 40;
  CratHsaEntry->LDSSizeInKB = 64;
  CratHsaEntry->WaveFrontSize = 64;
  CratHsaEntry->NumShaderBanks = GPU__GC__NUM_SE;
  CratHsaEntry->NumArrays = GPU__GC__NUM_SH_PER_SE;
  CratHsaEntry->NumCUPerArray = GPU__GC__NUM_SH_PER_SE * GPU__GC__NUM_CU_PER_SH;
  CratHsaEntry->MaxSlotsScratchCU = 32;

  IDS_HDT_CONSOLE (MAIN_FLOW, "CratHsaEntry->Length: %x\n", CratHsaEntry->Length);
  IDS_HDT_CONSOLE (MAIN_FLOW, "CratHsaEntry->Flags.Enabled: %x\n", CratHsaEntry->Flags.Enabled);
  IDS_HDT_CONSOLE (MAIN_FLOW, "CratHsaEntry->Flags.GpuPresent: %x\n", CratHsaEntry->Flags.GpuPresent);
  IDS_HDT_CONSOLE (MAIN_FLOW, "CratHsaEntry->Flags.CpuPresent: %x\n", CratHsaEntry->Flags.CpuPresent);
  IDS_HDT_CONSOLE (MAIN_FLOW, "CratHsaEntry->Flags.HSAmmuPresent: %x\n", CratHsaEntry->Flags.HSAmmuPresent);
  IDS_HDT_CONSOLE (MAIN_FLOW, "CratHsaEntry->ProcessorIdLow: %x\n", CratHsaEntry->ProcessorIdLow);
  IDS_HDT_CONSOLE (MAIN_FLOW, "CratHsaEntry->NumSIMDPerCU: %x\n", CratHsaEntry->NumSIMDPerCU);
  IDS_HDT_CONSOLE (MAIN_FLOW, "CratHsaEntry->NumSIMDCores: %x\n", CratHsaEntry->NumSIMDCores);
  IDS_HDT_CONSOLE (MAIN_FLOW, "CratHsaEntry->MaxWavesSIMD: %x\n", CratHsaEntry->MaxWavesSIMD);
  IDS_HDT_CONSOLE (MAIN_FLOW, "CratHsaEntry->LDSSizeInKB: %x\n", CratHsaEntry->LDSSizeInKB);
  IDS_HDT_CONSOLE (MAIN_FLOW, "CratHsaEntry->WaveFrontSize: %x\n", CratHsaEntry->WaveFrontSize);
  IDS_HDT_CONSOLE (MAIN_FLOW, "CratHsaEntry->NumShaderBanks: %x\n", CratHsaEntry->NumShaderBanks);
  IDS_HDT_CONSOLE (MAIN_FLOW, "CratHsaEntry->NumArrays: %x\n", CratHsaEntry->NumArrays);
  IDS_HDT_CONSOLE (MAIN_FLOW, "CratHsaEntry->NumCUPerArray: %x\n", CratHsaEntry->NumCUPerArray);
  IDS_HDT_CONSOLE (MAIN_FLOW, "CratHsaEntry->MaxSlotsScratchCU: %x\n", CratHsaEntry->MaxSlotsScratchCU);

  return EFI_SUCCESS;
}

EFI_STATUS
AmdDebugGetConfig (
  IN       DXE_AMD_NBIO_DEBUG_SERVICES_PROTOCOL     *This,
     OUT   UINT32                                   **DebugOptions
  )
{
  EFI_STATUS                           Status;
  GNB_BUILD_OPTIONS_ZP_DATA_HOB        *GnbBuildOptionsRVDataHob;

  //
  // Get information data from HOB.
  //
  Status = EfiGetSystemConfigurationTable (&gEfiHobListGuid, (VOID**)&GnbBuildOptionsRVDataHob);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  GnbBuildOptionsRVDataHob = GetNextGuidHob (&gGnbNbioBaseZPHobInfoGuid, GnbBuildOptionsRVDataHob);
  if (GnbBuildOptionsRVDataHob == NULL) {
    return EFI_NOT_FOUND;
  }

  *DebugOptions = (UINT32*) GnbBuildOptionsRVDataHob;
  return Status;
}

DXE_AMD_NBIO_DEBUG_SERVICES_PROTOCOL mDxeAmdNbioBaseServicesProtocol = {
  AMD_NBIO_DEBUG_OPTION_SERVICES_DXE_REVISION,
  AmdDebugGetConfig
};

EFI_STATUS
EFIAPI
AmdNbioBaseRVDxeEntry (
  IN       EFI_HANDLE         ImageHandle,
  IN       EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS                           Status;
  EFI_HANDLE                           Handle;

  AGESA_TESTPOINT (TpNbioBaseDxeEntry, NULL);
  IDS_HDT_CONSOLE (MAIN_FLOW, "AmdNbioBaseRVDxeEntry\n");
  Handle = NULL;

  if (PcdGet8 (PcdCfgIgpuContorl) != 0) {
    Status = gBS->InstallProtocolInterface (
                    &ImageHandle,
                    &gAmdIGpuAcpiCratServicesProtocolGuid,
                    EFI_NATIVE_INTERFACE,
                    &mIGpuCratServicesProtocol
                    );
  }

  Status = gBS->InstallProtocolInterface (
                  &Handle,
                  &gAmdNbioDebugServicesProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &mDxeAmdNbioBaseServicesProtocol
                  );

  IDS_HDT_CONSOLE (MAIN_FLOW, "AmdNbioBaseRVDxeExit\n");
  AGESA_TESTPOINT (TpNbioBaseDxeExit, NULL);
  return EFI_SUCCESS;
}

