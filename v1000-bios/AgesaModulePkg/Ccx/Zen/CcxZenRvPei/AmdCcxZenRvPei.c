/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD CCX Zen - Zeppelin API, and related functions.
 *
 * Contains code that initializes the core complex
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CCX
 * @e \$Revision$   @e \$Date$
 *
 */
/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */
/*++
Module Name:

  AmdCcxZenRvPei.c
  Init CCX interface

Abstract:
--*/

#include "PiPei.h"
#include <AGESA.h>
#include "CcxZenRvCacheInit.h"
#include "CcxZenRvDownCoreInit.h"
#include "CcxZenRvBrandString.h"
#include <Library/CcxBaseX86Lib.h>
#include <Library/CcxBistLib.h>
#include <Library/CcxRolesLib.h>
#include <Library/CcxDownCoreLib.h>
#include <Library/CcxResetTablesLib.h>
#include <Library/AmdIdsHookLib.h>
#include <Library/IdsLib.h>
#include <Library/FchBaseLib.h>
#include <Library/AmdCapsuleLib.h>
#include <Ppi/AmdCcxPpi.h>
#include <Ppi/AmdCoreTopologyServicesPpi.h>
#include <Ppi/FabricTopologyServices2Ppi.h>
#include "Filecode.h"

#define FILECODE CCX_ZEN_CCXZENRVPEI_AMDCCXZENRVPEI_FILECODE

STATIC PEI_AMD_CCX_INIT_COMPLETE_PPI mCcxPeiInitCompletePpi = {
  AMD_CCX_PPI_REVISION
};

STATIC EFI_PEI_PPI_DESCRIPTOR mCcxPeiInitCompletePpiList =
{
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gAmdCcxPeiInitCompletePpiGuid,
  &mCcxPeiInitCompletePpi
};

/*++

Routine Description:

  Zen - Zeppelin Driver Entry.  Initialize the core complex.

Arguments:

Returns:

  EFI_STATUS

--*/
EFI_STATUS
EFIAPI
AmdCcxZenRvPeiInit (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  UINTN                                 i;
  UINTN                                 j;
  UINTN                                 k;
  UINTN                                 m;
  UINTN                                 n;
  UINTN                                 NumberOfSockets;
  UINTN                                 NumberOfDies;
  UINTN                                 NumberOfComplexes;
  UINTN                                 NumberOfCores;
  UINTN                                 NumberOfThreads;
  UINTN                                 LogicalThread;
  BOOLEAN                               IsS3;
  EFI_STATUS                            Status;
  EFI_STATUS                            CalledStatus;
  AMD_CONFIG_PARAMS                     StdHeader;
  AMD_CORE_TOPOLOGY_SERVICES_PPI        *CoreTopologyServices;
  AMD_PEI_FABRIC_TOPOLOGY_SERVICES2_PPI *FabricTopologyServices;

  AGESA_TESTPOINT (TpCcxPeiEntry, NULL);

  Status = EFI_SUCCESS;

  IDS_HDT_CONSOLE (CPU_TRACE, "  AmdCcxZenRvPeiInit Entry\n");

  if (CcxIsBsp (&StdHeader)) {
    if ((FchReadSleepType () == 0x3) ||  (AmdCapsuleGetStatus ()) ) {
      IsS3 = TRUE;
    } else {
      IsS3 = FALSE;
    }

    if (!IsS3) {
      CcxZenRvCacheInit (&StdHeader);
      IDS_HDT_CONSOLE (CPU_TRACE, "  CcxZenRvCacheInit Done\n");
    }

    if (CheckBistStatus (&StdHeader) != AGESA_SUCCESS) {
      Status = EFI_DEVICE_ERROR;
    }

    CcxZenRvDownCoreInit (PeiServices);

    CcxProgramTablesAtReset (FchReadSleepType (), &StdHeader);
    IDS_HDT_CONSOLE (CPU_TRACE, "  CcxProgramTablesAtReset Done\n");

    if (!IsS3) {
      CcxZenRvSetBrandString (&StdHeader);
      IDS_HDT_CONSOLE (CPU_TRACE, "  CcxZenRvSetBrandString Done\n");
    }

    if (IsS3) {
      AGESA_TESTPOINT (TpCcxPeiStartLaunchApsForS3, NULL);
      IDS_HDT_CONSOLE (CPU_TRACE, "    Launching APs in PEI for S3\n");

      CalledStatus = (*PeiServices)->LocatePpi (
                                     PeiServices,
                                     &gAmdCoreTopologyServicesPpiGuid,
                                     0,
                                     NULL,
                                     (VOID**)&CoreTopologyServices
                                     );

      CalledStatus = (*PeiServices)->LocatePpi (
                                     PeiServices,
                                     &gAmdFabricTopologyServices2PpiGuid,
                                     0,
                                     NULL,
                                     (VOID**)&FabricTopologyServices
                                     );

      FabricTopologyServices->GetSystemInfo (&NumberOfSockets, NULL, NULL, NULL, NULL);

      for (i = 0; i < NumberOfSockets; i++) {
        FabricTopologyServices->GetProcessorInfo (i, &NumberOfDies, NULL);
        for (j = 0; j < NumberOfDies; j++) {
          CoreTopologyServices->GetCoreTopologyOnDie (CoreTopologyServices, i, j, &NumberOfComplexes, &NumberOfCores, &NumberOfThreads);
          for (k = 0; k < NumberOfComplexes; k++) {
            LogicalThread = 0;
            for (m = 0; m < NumberOfCores; m++) {
              for (n = 0; n < NumberOfThreads; n++) {
                if (!((i == 0) && (j == 0) && (k == 0) && (LogicalThread == 0))) {

                  IDS_HDT_CONSOLE (CPU_TRACE, "      Launch socket %X die %X complex %X core %X thread %X\n", i, j, k, m, n);

                  CoreTopologyServices->LaunchThread (CoreTopologyServices, i, j, k, LogicalThread);
                }
                LogicalThread++;
              }
            }
          }
        }
      }

      // MSRC001_1023[49, TwCfgCombineCr0Cd] = 1
      AsmMsrOr64 (0xC0011023, BIT49);

      AGESA_TESTPOINT (TpCcxPeiEndLaunchApsForS3, NULL);
    }

    CalledStatus = (**PeiServices).InstallPpi (PeiServices, &mCcxPeiInitCompletePpiList);
    if (CalledStatus > Status) {
      Status = CalledStatus;
    }
  } else {
    Status = EFI_DEVICE_ERROR;
  }

  IDS_HDT_CONSOLE (CPU_TRACE, "  AmdCcxZenRvPeiInit End\n");

  AGESA_TESTPOINT (TpCcxPeiExit, NULL);

  return (Status);
}

