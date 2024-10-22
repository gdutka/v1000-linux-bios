/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Fabric IO Master/Slave initialization.
 *
 * This funtion initializes the IOMS devices in the fabric.
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
#include <Library/IdsLib.h>
#include <Library/BaseFabricTopologyLib.h>
#include <FabricRegistersRV.h>
#include <Library/FabricRegisterAccLib.h>
#include "Filecode.h"
#include "FabricIomsInit.h"

#define FILECODE FABRIC_RV_FABRICRVPEI_FABRICIOMSINIT_FILECODE

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                           G L O B A L   V A R I A B L E S
 *----------------------------------------------------------------------------------------
 */
CONST UINTN ROMDATA RvIomsInstanceIds[] = {
  RV_IOMS0_INSTANCE_ID
};

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
 *  FabricIomsInit
 *
 *  Description:
 *    This funtion initializes the IOMS devices in the fabric.
 *
 */
VOID
FabricIomsInit (
  IN       CONST EFI_PEI_SERVICES       **PeiServices,
  IN       AMD_PEI_SOC_LOGICAL_ID_PPI   *SocLogicalIdPpi
  )
{
  UINTN                     i;
  IOM_CFG_REGISTER          IomCfg;
  IOM_CTRL_REGISTER         IomCtrl;

  for (i = 0; i < (sizeof (RvIomsInstanceIds) / sizeof (RvIomsInstanceIds[0])); i++) {
    IomCtrl.Value = FabricRegisterAccRead (0, 0, IOM_CTRL_FUNC, IOM_CTRL_REG, RvIomsInstanceIds[i]);
    IomCtrl.Field.FrcIomSdpVc7ToFtiVc4 = 1;
    FabricRegisterAccWrite (0, 0, IOM_CTRL_FUNC, IOM_CTRL_REG, RvIomsInstanceIds[i], IomCtrl.Value, FALSE);

    IomCfg.Value = FabricRegisterAccRead (0, 0, IOM_CFG_FUNC, IOM_CFG_REG, RvIomsInstanceIds[i]);
    IomCfg.Field.DisSrcDnOnVc2ToVc7 = 1;
    FabricRegisterAccWrite (0, 0, IOM_CFG_FUNC, IOM_CFG_REG, RvIomsInstanceIds[i], IomCfg.Value, FALSE);
  }
}


