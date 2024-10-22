/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Fabric PIE RAS initialization.
 *
 * This funtion initializes the RAS features of PIE.
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
#include <Library/AmdIdsHookLib.h>
#include "Filecode.h"
#include "FabricPieRasInit.h"

#define FILECODE FABRIC_RV_FABRICRVPEI_FABRICPIERASINIT_FILECODE

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
 *  FabricPieRasInit
 *
 *  Description:
 *    This funtion initializes the RAS features of PIE.
 *
 */
VOID
FabricPieRasInit (
  IN       CONST EFI_PEI_SERVICES       **PeiServices,
  IN       AMD_PEI_SOC_LOGICAL_ID_PPI   *SocLogicalIdPpi
  )
{
  UINTN                           i;
  UINTN                           j;
  UINTN                           k;
  HWA_STS_HI_REGISTER             HwaStsHi;
  HWA_STS_LOW_REGISTER            HwaStsLow;
  HWA_MASK_HI_REGISTER            HwaMaskHi;
  HWA_MASK_LOW_REGISTER           HwaMaskLow;
  DF_GLBL_CTRL_REGISTER           DfGlblCtrl;
  FABRIC_BLK_INST_COUNT_REGISTER  FabricBlkInstCount;

  IDS_HOOK (IDS_HOOK_DF_RAS_INIT, NULL, NULL);

  for (i = 0; i < FabricTopologyGetNumberOfProcessorsPresent (); i++) {
    for (j = 0; j < FabricTopologyGetNumberOfDiesOnSocket (i); j++) {
      FabricBlkInstCount.Value = FabricRegisterAccRead (i, j, FABBLKINSTCOUNT_FUNC, FABBLKINSTCOUNT_REG, FABRIC_REG_ACC_BC);
      for (k = 0; k < FabricBlkInstCount.Field.BlkInstCount; k++) {

        HwaStsLow.Value = FabricRegisterAccRead (i, j, HWA_STS_LOW_FUNC, HWA_STS_LOW_REG, k);
        if (HwaStsLow.Value != 0) {
          HwaMaskLow.Value = FabricRegisterAccRead (i, j, HWA_MASK_LOW_FUNC, HWA_MASK_LOW_REG, k);
          HwaStsLow.Value &= ~HwaMaskLow.Value;
          if (HwaStsLow.Value != 0) {
            IDS_HDT_CONSOLE (TOPO_TRACE, "  DF Hardware Assert Low: Socket %d, Die %d, InstanceID %d, Value %x\n", i, j, k, HwaStsLow.Value);
            HwaStsLow.Value = 0;
          }
          FabricRegisterAccWrite (i, j, HWA_STS_LOW_FUNC, HWA_STS_LOW_REG, k, HwaStsLow.Value, FALSE);
        }

        HwaStsHi.Value = FabricRegisterAccRead (i, j, HWA_STS_HI_FUNC, HWA_STS_HI_REG, k);
        if (HwaStsHi.Value != 0) {
          HwaMaskHi.Value = FabricRegisterAccRead (i, j, HWA_MASK_HI_FUNC, HWA_MASK_HI_REG, k);
          HwaStsHi.Value &= ~HwaMaskHi.Value;
          if (HwaStsHi.Value != 0) {
            IDS_HDT_CONSOLE (TOPO_TRACE, "  DF Hardware Assert High: Socket %d, Die %d, InstanceID %d, Value %x\n", i, j, k, HwaStsHi.Value);
            HwaStsHi.Value = 0;
          }
          FabricRegisterAccWrite (i, j, HWA_STS_HI_FUNC, HWA_STS_HI_REG, k, HwaStsHi.Value, FALSE);
        }
      }

      DfGlblCtrl.Value = FabricRegisterAccRead (i, j, DF_GLBL_CTRL_FUNC, DF_GLBL_CTRL_REG, FABRIC_REG_ACC_BC);
      DfGlblCtrl.Field.DisImmSyncFloodOnFatalErr = 0;
      IDS_HOOK (IDS_HOOK_DF_RAS_INIT2, NULL, (VOID *) &DfGlblCtrl);
      FabricRegisterAccWrite (i, j, DF_GLBL_CTRL_FUNC, DF_GLBL_CTRL_REG, FABRIC_REG_ACC_BC, DfGlblCtrl.Value, FALSE);
    }
  }
}


