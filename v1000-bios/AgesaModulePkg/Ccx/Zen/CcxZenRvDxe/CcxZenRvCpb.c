/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD CPU Power Management functions.
 *
 * Contains code for doing early power management
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
#include "cpuRegisters.h"
#include "Filecode.h"
#include "Library/AmdS3SaveLib.h"
#include "Protocol/MpService.h"
#include "Library/IdsLib.h"
#include "PiDxe.h"

#define FILECODE CCX_ZEN_CCXZENRVDXE_CCXZENRVCPB_FILECODE

extern  EFI_BOOT_SERVICES *gBS;

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
VOID
CcxZenRvDisableCpbAps (
  IN       VOID  *Void
  );

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*---------------------------------------------------------------------------------------*/
/**
 * Enabling Core Performance Boost.
 *
 * Set up D18F4x15C[BoostSrc] and start the PDMs according to the BKDG.
 *
 * @param[in]  StdHeader               Config handle for library and services.
 *
 * @retval     AGESA_SUCCESS           Always succeeds.
 *
 */
AGESA_STATUS
CcxZenRvInitializeCpb (
  IN       AMD_CONFIG_PARAMS         *StdHeader
  )
{
  EFI_STATUS                Status;
  EFI_MP_SERVICES_PROTOCOL  *MpServices;

  if (PcdGet8 (PcdAmdCpbMode) == 0) {
    IDS_HDT_CONSOLE (CPU_TRACE, "  CcxRvInitializeCpb - Disable CPB\n");

    Status = gBS->LocateProtocol (&gEfiMpServiceProtocolGuid, NULL, (VOID**)&MpServices);
    ASSERT (!EFI_ERROR (Status));
    if (EFI_ERROR (Status)) {
      return AGESA_ERROR;
    }
    MpServices->StartupAllAPs (
        MpServices,
        CcxZenRvDisableCpbAps,
        FALSE,
        NULL,
        0,
        NULL,
        NULL
    );
    CcxZenRvDisableCpbAps (NULL);
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
 *  CcxRvDisableCpbAps
 *
 *  Description:
 *    This funtion disables CPB on all threads.
 *
 */
VOID
CcxZenRvDisableCpbAps (
  IN       VOID  *Void
  )
{
  AsmMsrOr64 (MSR_HWCR, BIT25);
}

