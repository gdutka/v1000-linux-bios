/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD CPU prefetch functions.
 *
 * This funtion provides for performance tuning to optimize for specific
 * workloads. For general performance use the recommended settings.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Ccx
 * @e \$Revision: 315062 $   @e \$Date: 2015-03-19 07:54:52 +0800 (Thu, 19 Mar 2015) $
 *
 */
/*
 ****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include <Library/BaseLib.h>
#include "Porting.h"
#include "AMD.h"
#include "Filecode.h"
#include "cpuRegisters.h"
#include "CcxZenRvPrefetch.h"
#include "Library/AmdBaseLib.h"
#include "Library/UefiBootServicesTableLib.h"
#include "Library/CcxRolesLib.h"
#include "Protocol/MpService.h"

#define FILECODE CCX_ZEN_CCXZENRVDXE_CCXZENRVPREFETCH_FILECODE

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
VOID
CcxRvPrefetchModeAps (
  IN       VOID                             *Buffer
  );

/*----------------------------------------------------------------------------------------
 *                           G L O B A L   V A R I A B L E S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/* -----------------------------------------------------------------------------*/
/**
 *
 *  CcxRvInitializePrefetchMode
 *
 *  Description:
 *    This funtion provides for performance tuning to optimize for specific
 *    workloads. For general performance use the recommended settings.
 *
 *  Parameters:
 *    @param[in]  StdHeader                 Config handle for library and services
 *
 *    @retval     AGESA_STATUS
 *
 *
 */
AGESA_STATUS
CcxZenRvInitializePrefetchMode (
  IN       AMD_CONFIG_PARAMS                *StdHeader
  )
{
  EFI_STATUS                Status;
  CCX_PREFETCH_MODE         PrefetchMode;
  EFI_MP_SERVICES_PROTOCOL *MpServices;

  PrefetchMode.HardwarePrefetchMode = PcdGet8 (PcdAmdHardwarePrefetchMode);
  PrefetchMode.SoftwarePrefetchMode = PcdGet8 (PcdAmdSoftwarePrefetchMode);
  if ((PrefetchMode.HardwarePrefetchMode != HARDWARE_PREFETCHER_AUTO) ||
      (PrefetchMode.SoftwarePrefetchMode != SOFTWARE_PREFETCHES_AUTO)) {
    IDS_HDT_CONSOLE (CPU_TRACE, "  CcxRvInitializePrefetchMode:\n");
    IDS_HDT_CONSOLE (CPU_TRACE, "    HardwarePrefetchMode - %d\n", PrefetchMode.HardwarePrefetchMode);
    IDS_HDT_CONSOLE (CPU_TRACE, "    SoftwarePrefetchMode - %d\n", PrefetchMode.SoftwarePrefetchMode);

    Status = gBS->LocateProtocol (&gEfiMpServiceProtocolGuid, NULL, (VOID**)&MpServices);
    ASSERT (!EFI_ERROR (Status));
    if (EFI_ERROR (Status)) {
      return AGESA_ERROR;
    }
    MpServices->StartupAllAPs (
        MpServices,
        CcxRvPrefetchModeAps,
        FALSE,
        NULL,
        0,
        (VOID *) &PrefetchMode,
        NULL
    );
    CcxRvPrefetchModeAps (&PrefetchMode);
  }
  return AGESA_SUCCESS;
}

/*----------------------------------------------------------------------------------------
 *                          L O C A L    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/* -----------------------------------------------------------------------------*/
/**
 *
 *  CcxRvPrefetchModeAps
 *
 *  Description:
 *    This funtion provides for performance tuning to optimize for specific
 *    workloads. For general performance use the recommended settings.
 *
 *  Parameters:
 *    @param[in]  PrefetchMode              Prefetch mode
 *
 *
 */
VOID
CcxRvPrefetchModeAps (
  IN       VOID                             *Buffer
  )
{
  //[start-240426-IB18160212-add]//
  CCX_PREFETCH_MODE                *PrefetchMode;

  PrefetchMode = Buffer;  
  //[end-240426-IB18160212-add]//

  ASSERT (PrefetchMode->HardwarePrefetchMode < MAX_HARDWARE_PREFETCH_MODE);

  switch (PrefetchMode->HardwarePrefetchMode) {
  case DISABLE_HARDWARE_PREFETCH:
    // DC_CFG (MSR_C001_1022)
    //  [13] = 1
    //  [15] = 1
    AsmMsrOr64 (MSR_DC_CFG, (BIT13 | BIT15));
    // CU_CFG3 (MSR_C001_102B)
    //  [3]  = 1
    //  [16] = 1
    //  [17] = 1
    //  [18] = 1
    if (CcxIsComputeUnitPrimary (NULL)) {
      AsmMsrOr64 (MSR_CU_CFG3, (BIT3 | BIT16 | BIT17 | BIT18));
    }
    break;
  case DISABLE_L2_STRIDE_PREFETCHER:
    // For DISABLE_L2_STRIDE_PREFETCHER, also implement the following :
    // DISABLE_L1_PREFETCHER
    // DISABLE_HW_PREFETCHER_TRAINING_ON_SOFTWARE_PREFETCHES
    // Hence, no break to allow code to flow through.
    // CU_CFG3 (MSR_C001_102B)
    //  [1] = 0
    if (CcxIsComputeUnitPrimary (NULL)) {
      AsmMsrAnd64 (MSR_CU_CFG3, ~BIT1);
    }
  case DISABLE_L1_PREFETCHER:
    // For this setting, also implement DISABLE_HW_PREFETCHER_TRAINING_ON_SOFTWARE_PREFETCHES.
    // Hence, no break to allow code to flow through.
    // CU_CFG3 (MSR_C001_102B)
    //  [3] = 1
    if (CcxIsComputeUnitPrimary (NULL)) {
      AsmMsrOr64 (MSR_CU_CFG3, BIT3);
    }
  case DISABLE_HW_PREFETCHER_TRAINING_ON_SOFTWARE_PREFETCHES:
    // DC_CFG (MSR_C001_1022)
    //  [15] = 1
    AsmMsrOr64 (MSR_DC_CFG, BIT15);
    break;
  default:
    break;
  }
  // DISABLE_SOFTWARE_PREFETCHES
  if (PrefetchMode->SoftwarePrefetchMode == DISABLE_SOFTWARE_PREFETCHES) {
    // MSR_DE_CFG (MSR_C001_1029)
    //  [7:2] = 0x3F
    if (CcxIsComputeUnitPrimary (NULL)) {
      AsmMsrOr64 (MSR_DE_CFG, 0xFC);
    }
  }
}


