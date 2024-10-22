/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Fabric Core Coherent Master initialization.
 *
 * This funtion initializes the CCM devices in the fabric.
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
#include "FabricCcmInit.h"

#define FILECODE FABRIC_RV_FABRICRVPEI_FABRICCCMINIT_FILECODE

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
CONST UINTN ROMDATA RvCcmInstanceIds[] = {
  RV_CCM0_INSTANCE_ID,
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
 *  FabricCcmInit
 *
 *  Description:
 *    This funtion initializes the CCM devices in the fabric.
 *
 */
VOID
FabricCcmInit (
  IN       CONST EFI_PEI_SERVICES       **PeiServices,
  IN       AMD_PEI_SOC_LOGICAL_ID_PPI   *SocLogicalIdPpi
  )
{
  UINTN                                 i;
  CCM_CFG_REGISTER                      CcmCfg;
  CCM_CFG1_REGISTER                     CcmCfg1;
  MASTER_REQ_PRI_REGISTER               MasterReqPri;
  MASTER_LL_LP_REQ_PRI_THRESH_REGISTER  MasterLlLpReqPriThresh;

  MasterReqPri.Value = 0;
  MasterReqPri.Field.LPRdReqPri = 2;
  MasterReqPri.Field.UseSdpReqPri = 1;
  for (i = 0; i < (sizeof (RvCcmInstanceIds) / sizeof (RvCcmInstanceIds[0])); i++) {
    FabricRegisterAccWrite (0, 0, MASTER_REQ_PRI_FUNC, MASTER_REQ_PRI_REG, RvCcmInstanceIds[i], MasterReqPri.Value, FALSE);
  }

  MasterLlLpReqPriThresh.Value = 0;
  MasterLlLpReqPriThresh.Field.LPCumElvPriReqThr = 0x10;
  MasterLlLpReqPriThresh.Field.LPElvPriReqThr = 2;
  FabricRegisterAccWrite (0, 0, MASTER_LL_LP_REQ_PRI_THRESH_FUNC, MASTER_LL_LP_REQ_PRI_THRESH_REG, FABRIC_REG_ACC_BC, MasterLlLpReqPriThresh.Value, FALSE);

  for (i = 0; i < (sizeof (RvCcmInstanceIds) / sizeof (RvCcmInstanceIds[0])); i++) {
    CcmCfg.Value = FabricRegisterAccRead (0, 0, CCM_CFG_FUNC, CCM_CFG_REG, RvCcmInstanceIds[i]);
    CcmCfg.Field.DisProtErrRsp = 1;
    FabricRegisterAccWrite (0, 0, CCM_CFG_FUNC, CCM_CFG_REG, RvCcmInstanceIds[i], CcmCfg.Value, FALSE);

    CcmCfg1.Value = FabricRegisterAccRead (0, 0, CCM_CFG1_FUNC, CCM_CFG1_REG, RvCcmInstanceIds[i]);
    CcmCfg1.Field.EnExtPciGt8BTgtAbt = 1;
    FabricRegisterAccWrite (0, 0, CCM_CFG1_FUNC, CCM_CFG1_REG, RvCcmInstanceIds[i], CcmCfg1.Value, FALSE);
  }
}

