/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Fabric Coherent Slave initialization.
 *
 * This funtion initializes the CS devices in the fabric.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Fabric
 * @e \$Revision$   @e \$Date$
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
#include "Porting.h"
#include "AMD.h"
#include <Library/BaseFabricTopologyLib.h>
#include <FabricRegistersRV.h>
#include <Library/FabricRegisterAccLib.h>
#include <Library/AmdIdsHookLib.h>
#include "Filecode.h"
#include "FabricDfClkGatingInit.h"

#define FILECODE FABRIC_RV_FABRICRVPEI_FABRICDFCLKGATINGINIT_FILECODE

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
BOOLEAN
IsCakeCompressionEnabledOnInstance (
  IN       UINTN  Socket,
  IN       UINTN  Die,
  IN       UINTN  InstanceID
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
 *  FabricClockGatingInit
 *
 *  Description:
 *    This funtion initializes the DF clock gating feature.
 *
 */
VOID
FabricClockGatingInit (
  IN       CONST EFI_PEI_SERVICES       **PeiServices,
  IN       AMD_PEI_SOC_LOGICAL_ID_PPI   *SocLogicalIdPpi
  )
{
  DF_GLBL_CLK_GATER_REGISTER      DfGlblClkGater;

  DfGlblClkGater.Value = 0;
  DfGlblClkGater.Field.MGCGMode = 1;

  IDS_HOOK (IDS_HOOK_DF_CLK_GATING, NULL, (VOID *) &DfGlblClkGater);
  FabricRegisterAccWrite (0, 0, DF_GLBL_CLK_GATER_FUNC, DF_GLBL_CLK_GATER_REG, FABRIC_REG_ACC_BC, DfGlblClkGater.Value, FALSE);
}


