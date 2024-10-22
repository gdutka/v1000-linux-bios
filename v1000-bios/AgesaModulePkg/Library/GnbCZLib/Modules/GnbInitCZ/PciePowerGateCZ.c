/* $NoKeywords:$ */
/**
 * @file
 *
 * PCIe power gate initialization
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 309090 $   @e \$Date: 2014-12-10 02:28:05 +0800 (Wed, 10 Dec 2014) $
 *
 */
/*
*****************************************************************************
*
* Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
*
*******************************************************************************
*
*/
/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include  <Library/BaseLib.h>
#include  "Gnb.h"
#include  "GnbPcie.h"
#include  "OptionGnb.h"
#include  "GnbPcieConfig.h"
#include  "GnbPcieFamServices.h"
#include  "GnbPcieInitLib.h"
#include  "GnbSmuInitLibV8.h"
#include  "GnbRegistersCZ.h"
#include  "GnbRegisterAccCZ.h"
#include  "PciePowerGateCZ.h"

#define FILECODE LIBRARY_GNBCZLIB_MODULES_GNBINITCZ_PCIEPOWERGATECZ_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

extern GNB_BUILD_OPTIONS      GnbBuildOptions;

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------*/
/**
 * Power down unused lanes
 *
 *
 *
 *
 * @param[in]       Wrapper         Pointer to wrapper configuration
 * @param[in, out]  Buffer          Not used
 * @param[in]       Pcie            Pointer to global PCIe configuration
 * @retval          AGESA_SUCCESS
 *
 */

AGESA_STATUS
STATIC
PciePowerGatePowerDownUnusedLanesCallbackCZ (
  IN       PCIe_WRAPPER_CONFIG   *Wrapper,
  IN OUT   VOID                  *Buffer,
  IN       PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  UINT8                   Index;
  UINTN                   State;
  UINT32                  LaneBitmap;
  UINT32                  LaneBitmapNative;
  UINT16                  StartLane;
  UINT16                  EndLane;
  DEV_OBJECT              DevObject;
  UINT32                  SmuArg[6];
  GNB_HANDLE              *GnbHandle;
  SMU_PCIE_PHY_LN_STRUCT  Phy_Data;
  UINT8                   PowerGatingFlags;


  IDS_HDT_CONSOLE (GNB_TRACE, "PciePowerGatePowerDownUnusedLanesCallbackCZ Enter\n");

  if (Wrapper->Features.PowerOffUnusedLanes != 0) {
    PowerGatingFlags = *(UINT8 *) Buffer;
    Phy_Data.Value = 0;
    GnbHandle = GnbGetHandle (GnbLibGetHeader (Pcie));
    GnbSmuServiceCommonInitArgumentsV8 (GnbHandle, (AMD_CONFIG_PARAMS *)Pcie->StdHeader, &DevObject, SmuArg);

    LaneBitmapNative = PcieUtilGetWrapperLaneBitMap (
                        LANE_TYPE_PHY_NATIVE_ALL,
                        0,
                        Wrapper
                        );
    LaneBitmap = PcieUtilGetWrapperLaneBitMap (
                   LANE_TYPE_PHY_NATIVE_ALL,
                   LANE_TYPE_PCIE_PHY_NATIVE_ALLOC_ACTIVE | LANE_TYPE_DDI_PHY_NATIVE_ACTIVE | LANE_TYPE_PCIE_PHY_NATIVE_HOTPLUG,
                   Wrapper
                   );
    if (LaneBitmap == LaneBitmapNative) {
      State = 0;
      StartLane = 0;
      EndLane = 0;
      for (Index = 0; Index <= (HighBitSet32 (LaneBitmap) + 1); Index++) {
        if ((State == 0) && ((LaneBitmap & (1 << Index)) != 0)) {
          StartLane = Index;
          State = 1;
        } else if ((State == 1) && ((LaneBitmap & (1 << Index)) == 0)) {
          EndLane = Index - 1;
          State = 0;

          Phy_Data.Field.Tx = 1;
          Phy_Data.Field.Rx = 1;
          Phy_Data.Field.Core = 1;
          if ((PowerGatingFlags & PCIE_POWERGATING_SKIP_CORE) != 0) {
            Phy_Data.Field.SkipCore = 1;
          }
          if ((PowerGatingFlags & PCIE_POWERGATING_SKIP_PHY) != 0) {
            Phy_Data.Field.SkipPhy = 1;
          }
          Phy_Data.Field.LowerLaneID = StartLane + Wrapper->StartPhyLane;
          Phy_Data.Field.UpperLaneID = EndLane + Wrapper->StartPhyLane;
          IDS_HDT_CONSOLE (
            GNB_TRACE,
            "  LowerLaneID - %02d UpperLaneID - %02d Tx - %d  Rx - %d Core - %d Exit\n",
            Phy_Data.Field.LowerLaneID,
            Phy_Data.Field.UpperLaneID,
            Phy_Data.Field.Tx,
            Phy_Data.Field.Rx,
            Phy_Data.Field.Core
            );

          SmuArg[0] = Phy_Data.Value;
          GnbSmuServiceRequestV8 (
            &DevObject,
            SMC_MSG_PHY_LN_OFF,
            SmuArg,
            GNB_REG_ACC_FLAG_S3SAVE
            );
        }
      }
    }
  }
  IDS_HDT_CONSOLE (GNB_TRACE, "PciePowerGatePowerDownUnusedLanesCallbackCZ Exit\n");
  return AGESA_SUCCESS;
}




/*----------------------------------------------------------------------------------------*/
/**
 * Pcie Power gate init
 *
 *   Late PCIe initialization
 *
 * @param[in]  Pcie                Pointer to global PCIe configuration
 * @retval     AGESA_SUCCESS       Topology successfully mapped
 */

AGESA_STATUS
PciePowerGateCZ (
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  UINT8                   PowerGatingFlags;
  GNB_HANDLE              *GnbHandle;

  IDS_HDT_CONSOLE (GNB_TRACE, "PciePowerGateCZ Enter\n");

  GnbHandle = GnbGetHandle (GnbLibGetHeader (Pcie));

  PowerGatingFlags = GnbBuildOptions.CfgPciePowerGatingFlags;

  IDS_HDT_CONSOLE (GNB_TRACE, "  PowerGatingFlags = %02x\n", PowerGatingFlags);

  IDS_OPTION_HOOK (IDS_GNB_PCIE_POWER_GATING, &PowerGatingFlags, GnbLibGetHeader (Pcie));
  IDS_HDT_CONSOLE (GNB_TRACE, "  PowerGatingFlags = %02x\n", PowerGatingFlags);

  // Power down unused lanes
  PcieConfigRunProcForAllWrappers (
    DESCRIPTOR_PCIE_WRAPPER | DESCRIPTOR_DDI_WRAPPER,
    PciePowerGatePowerDownUnusedLanesCallbackCZ,
    &PowerGatingFlags,
    Pcie
    );

  IDS_HDT_CONSOLE (GNB_TRACE, "PciePowerGateCZ Exit\n");
  return  AGESA_SUCCESS;
}

