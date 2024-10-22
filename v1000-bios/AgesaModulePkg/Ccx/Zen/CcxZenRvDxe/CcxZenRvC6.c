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
#include "Filecode.h"
#include "CcxRegistersRv.h"
#include "Library/CcxRolesLib.h"
#include "Library/AmdS3SaveLib.h"
#include "Library/IdsLib.h"
#include "Protocol/MpService.h"
#include "PiDxe.h"

#define FILECODE CCX_ZEN_CCXZENRVDXE_CCXZENRVC6_FILECODE

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
CcxZenRvInitializeC6Aps (
  IN       VOID    *Buffer
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
CcxZenRvInitializeC6 (
  IN       AMD_CONFIG_PARAMS         *StdHeader
  )
{
  UINT8                     CStateMode;
  UINT16                    CStateBaseAddr;
  EFI_STATUS                Status;
  EFI_MP_SERVICES_PROTOCOL  *MpServices;

  CStateMode = PcdGet8 (PcdAmdCStateMode);
  ASSERT (CStateMode <= 1);

  if (CStateMode == 1) {
    IDS_HDT_CONSOLE (CPU_TRACE, "  CcxZenRvInitializeC6 - Enable C6\n");

    CStateBaseAddr = PcdGet16 (PcdAmdCStateIoBaseAddress);

    Status = gBS->LocateProtocol (&gEfiMpServiceProtocolGuid, NULL, (VOID**)&MpServices);
    ASSERT (!EFI_ERROR (Status));
    if (EFI_ERROR (Status)) {
      return AGESA_ERROR;
    }
    MpServices->StartupAllAPs (
        MpServices,
        CcxZenRvInitializeC6Aps,
        FALSE,
        NULL,
        0,
        (VOID *) &CStateBaseAddr,
        NULL
    );
    CcxZenRvInitializeC6Aps ((VOID*)&CStateBaseAddr);
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
 *  CcxRvInitializeC6Aps
 *
 *  @param[in]  CStateBaseAddr   Pointer to CState base address
 *
 *  Description:
 *    This funtion enables C6 on all threads.
 *
 */
VOID
CcxZenRvInitializeC6Aps (
  IN       VOID    *Buffer
  )
{
  CSTATE_CFG_MSR      CstateCfg;
  AMD_CONFIG_PARAMS   StdHeader;
  CSTATE_ADDRESS_MSR  CstateAddr;
  //[start-240426-IB18160212-add]//
  UINT16              *CStateBaseAddr;

  CStateBaseAddr = Buffer;
  //[start-240426-IB18160212-add]//

  if (CcxIsComputeUnitPrimary (&StdHeader)) {
    CstateCfg.Value = 0;
    CstateCfg.Field.CCR0_CC1DFSID = 8;
    CstateCfg.Field.CCR0_CC6EN = 1;
    CstateCfg.Field.CCR1_CC1DFSID = 8;
    CstateCfg.Field.CCR1_CC6EN = 1;
    CstateCfg.Field.CCR2_CC1DFSID = 8;
    CstateCfg.Field.CCR2_CC6EN = 1;
    AsmWriteMsr64 (MSR_CSTATE_CFG, CstateCfg.Value);
  }

  CstateAddr.Value = 0;
  CstateAddr.Field.CstateAddr = (UINT32) *CStateBaseAddr;
  AsmWriteMsr64 (MSR_CSTATE_ADDRESS, CstateAddr.Value);
}

