/* $NoKeywords:$ */
/**
 * @file
 *
 * Implement Ccx IDS sync up MSRs
 *
 * Contains code that sync up MSRs
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Library
 * @e \$Revision$   @e \$Date$
 *
 */
/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */
#include "PiDxe.h"
#include <Library/BaseLib.h>
#include "CcxVhIdsSyncMsrSmm.h"
#include "Library/SmmServicesTableLib.h"
#include "Filecode.h"

#define FILECODE LIBRARY_CCXVHIDSHOOKLIB_SMM_CCXVHIDSSYNCMSRSMM_FILECODE

/*----------------------------------------------------------------------------------------
 *                           G L O B A L   V A R I A B L E S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */

VOID
CcxIdsVhSmmSyncMsrAps (
  IN       VOID *MsrTable
  );

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*---------------------------------------------------------------------------------------*/
/**
 * CcxIdsVhSmmSyncMsr
 *
 * Sync variable MTRR
 *
 * @param[in]  ApMsrSync         MSRs table
 * @param[in]  StdHeader         Config handle for library and services
 *
 */
VOID
CcxIdsVhSmmSyncMsr (
  IN OUT   IDS_BSC_AP_MSR_SYNC *ApMsrSync,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{
  UINT16      i;

  //
  //Sync all MSR settings with BSP
  //

  for (i = 0; ApMsrSync[i].RegisterAddress != 0; i++) {
    if (ApMsrSync[i].ForceSetting == FALSE) {
      ApMsrSync[i].RegisterValue = AsmReadMsr64 (ApMsrSync[i].RegisterAddress);
    }
  }

  for (i = 1; i < gSmst->NumberOfCpus; i++) {
    gSmst->SmmStartupThisAp (
            CcxIdsVhSmmSyncMsrAps,
            i,
            (VOID *) ApMsrSync
            );
  }
}

/*----------------------------------------------------------------------------------------
 *                          L O C A L    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*---------------------------------------------------------------------------------------*/
/**
 * AP task to sync MSRs with the BSC
 *
 *
 * @param[in]  MsrTable          MSRs table
 *
 */
VOID
CcxIdsVhSmmSyncMsrAps (
  IN       VOID *MsrTable
  )
{
  UINT8   i;

  for (i = 0; ((IDS_BSC_AP_MSR_SYNC *) MsrTable)[i].RegisterAddress != 0; i++) {
    AsmMsrAndThenOr64 (
        ((IDS_BSC_AP_MSR_SYNC *) MsrTable)[i].RegisterAddress,
        ~(((IDS_BSC_AP_MSR_SYNC *) MsrTable)[i].RegisterMask),
        (((IDS_BSC_AP_MSR_SYNC *) MsrTable)[i].RegisterValue &
        ((IDS_BSC_AP_MSR_SYNC *) MsrTable)[i].RegisterMask)
        );
  }
}


