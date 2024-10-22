/* $NoKeywords:$ */
/**
 * @file
 *
 * GFX POST initialization.
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 328050 $   @e \$Date: 2015-09-29 06:53:15 +0800 (Tue, 29 Sep 2015) $
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
#include  "Gnb.h"
#include  "GnbPcie.h"
#include  "Library/AmdHeapLib.h"
#include  "GnbPcieConfig.h"
#include  "GnbGfx.h"
#include  "GfxConfigLib.h"
#include  "GnbFuseTable.h"
#include  "GnbRegisterAccCZ.h"
#include  "GnbRegistersCZ.h"
#include  "GnbSmuInitLibV8.h"
#include  "OptionGnb.h"
#include  "GnbLibHeap.h"
#include  "GnbFamServices.h"

#define FILECODE LIBRARY_GNBCZLIB_MODULES_GNBINITCZ_GFXPOSTINITCZ_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

extern GNB_BUILD_OPTIONS  GnbBuildOptions;

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */
AGESA_STATUS
GfxPostInterfaceCZ (
  IN       AMD_CONFIG_PARAMS               *StdHeader
  );

/*----------------------------------------------------------------------------------------*/
/**
 * Allocate UMA configuration data
 *
 *
 *
 * @param[in]     StdHeader     Standard configuration header
 * @retval        AGESA_STATUS  Always succeeds
 */

AGESA_STATUS
GfxConfigPostInterfaceCZ (
  IN       AMD_CONFIG_PARAMS        *StdHeader
  )
{
  GFX_PLATFORM_CONFIG       *Gfx;
  AMD_POST_PARAMS           *PostParamsPtr;
  AGESA_STATUS              Status;
  GNB_BUILD_OPTIONS_COMMON  *GnbCommonOptions;
  PostParamsPtr = (AMD_POST_PARAMS *)StdHeader;
  Status = AGESA_SUCCESS;
  AGESA_TESTPOINT (TpGfxConfigPostInterfaceCZEntry, NULL);
  IDS_HDT_CONSOLE (GNB_TRACE, "GfxConfigPostInterfaceCZ Enter\n");
  Gfx = GnbAllocateHeapBuffer (AMD_GFX_PLATFORM_CONFIG_HANDLE, sizeof (GFX_PLATFORM_CONFIG), StdHeader);
  ASSERT (Gfx != NULL);
  if (Gfx != NULL) {
    LibAmdMemFill (Gfx, 0x00, sizeof (GFX_PLATFORM_CONFIG), StdHeader);
    GnbCommonOptions = (GNB_BUILD_OPTIONS_COMMON*) GnbFmGnbBuildOptions (StdHeader);
    if (GnbBuildOptions.IgfxModeAsPcieEp) {
      Gfx->GfxControllerMode = GfxControllerPcieEndpointMode;
      Gfx->GfxPciAddress.AddressValue = MAKE_SBDFO (0, 0, 1, 0, 0);
    } else {
      Gfx->GfxControllerMode = GfxControllerLegacyBridgeMode;
      Gfx->GfxPciAddress.AddressValue = MAKE_SBDFO (0, 1, 5, 0, 0);
    }
    Gfx->StdHeader = (PVOID) StdHeader;
    Gfx->GnbHdAudio = TRUE;//CFG_GNB_HD_AUDIO;                    //form UserOptions PostParamsPtr->PlatformConfig.GnbHdAudio;
    Gfx->AbmSupport = FALSE;//CFG_ABM_SUPPORT;                     //form UserOptions PostParamsPtr->PlatformConfig.AbmSupport;
    Gfx->DynamicRefreshRate = 0;//CFG_DYNAMIC_REFRESH_RATE;    //form UserOptions PostParamsPtr->PlatformConfig.DynamicRefreshRate;
    Gfx->LcdBackLightControl = 200;//CFG_LCD_BACK_LIGHT_CONTROL; //form UserOptions PostParamsPtr->PlatformConfig.LcdBackLightControl;
    Gfx->AmdPlatformType = 0;//CFG_AMD_PLATFORM_TYPE;          //form UserOptions UserOptions.CfgAmdPlatformType;
    Gfx->GmcClockGating = GnbCommonOptions->CfgGmcClockGating;
    Gfx->GmcPowerGating = GnbCommonOptions->GmcPowerGating;
    Gfx->UmaSteering = GnbCommonOptions->CfgUmaSteering;
    GNB_DEBUG_CODE (
      GfxConfigDebugDump (Gfx);
      );
  } else {
    Status = AGESA_ERROR;
  }
  IDS_OPTION_HOOK (IDS_GNB_PLATFORMCFG_OVERRIDE, Gfx, StdHeader);
  IDS_HDT_CONSOLE (GNB_TRACE, "GfxConfigPostInterfaceCZ Exit [0x%x]\n", Status);
  AGESA_TESTPOINT (TpGfxConfigPostInterfaceCZExit, NULL);
  return  Status;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Init GFX at Post.
 *
 *
 *
 * @param[in]     StdHeader     Standard configuration header
 * @retval        AGESA_STATUS
 */


AGESA_STATUS
GfxPostInterfaceCZ (
  IN       AMD_CONFIG_PARAMS               *StdHeader
  )
{
  AMD_POST_PARAMS           *PostParamsPtr;
  AGESA_STATUS              Status;
  GFX_PLATFORM_CONFIG       *Gfx;
  D0F0xBC_xC0014000_STRUCT  D0F0xBC_xC0014000;
  D0F0xD4_x013014AC_STRUCT  D0F0xD4_x13014AC;
  D0F0xD4_x013014B6_STRUCT  D0F0xD4_x13014B6;
  GNB_HANDLE                *GnbHandle;

  PostParamsPtr = (AMD_POST_PARAMS *)StdHeader;
  Status = AGESA_SUCCESS;
  AGESA_TESTPOINT (TpGfxPostInterfaceCZEntry, NULL);
  IDS_HDT_CONSOLE (GNB_TRACE, "GfxPostInterfaceCZ Enter\n");
  GnbHandle = GnbGetHandle (StdHeader);
  ASSERT (GnbHandle != NULL);
  GnbRegisterReadCZ (GnbHandle, D0F0xBC_xC0014000_TYPE,
      D0F0xBC_xC0014000_ADDRESS, &D0F0xBC_xC0014000.Value, 0, StdHeader);
  Status = GfxLocateConfigData (StdHeader, &Gfx);
  ASSERT (Status == AGESA_SUCCESS);
  if (Status == AGESA_SUCCESS) {
    if (D0F0xBC_xC0014000.Field.GPU_DIS == 1) {
      PostParamsPtr->MemConfig.UmaMode = UMA_NONE;
      Gfx->GfxFusedOff = TRUE;
    }
  } else {
    PostParamsPtr->MemConfig.UmaMode = UMA_NONE;
  }
  if (PostParamsPtr->MemConfig.UmaMode == UMA_NONE) {
    GnbRegisterReadCZ (GnbHandle, D0F0xD4_x013014AC_TYPE, D0F0xD4_x013014AC_ADDRESS, &D0F0xD4_x13014AC.Value, 0, StdHeader);
    GnbRegisterReadCZ (GnbHandle, D0F0xD4_x013014B6_TYPE, D0F0xD4_x013014B6_ADDRESS, &D0F0xD4_x13014B6.Value, 0, StdHeader);
    D0F0xD4_x13014AC.Field.StrapBifAudioEnPin = FALSE;
    D0F0xD4_x13014B6.Field.StrapBifAudioEn = FALSE;
    GnbRegisterWriteCZ (GnbHandle, D0F0xD4_x013014AC_TYPE, D0F0xD4_x013014AC_ADDRESS, &D0F0xD4_x13014AC.Value, GNB_REG_ACC_FLAG_S3SAVE, StdHeader);
    GnbRegisterWriteCZ (GnbHandle, D0F0xD4_x013014B6_TYPE, D0F0xD4_x013014B6_ADDRESS, &D0F0xD4_x13014B6.Value, GNB_REG_ACC_FLAG_S3SAVE, StdHeader);
  }

  IDS_HDT_CONSOLE (GNB_TRACE, "GfxPostInterfaceCZ Exit [0x%x]\n", Status);
  AGESA_TESTPOINT (TpGfxPostInterfaceCZExit, NULL);
  return  Status;
}

