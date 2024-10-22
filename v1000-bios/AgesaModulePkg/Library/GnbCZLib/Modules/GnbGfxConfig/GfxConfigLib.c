/* $NoKeywords:$ */
/**
 * @file
 *
 * Initialize GFX configuration data structure.
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
#include  "Gnb.h"
#include  "Library/AmdHeapLib.h"
#include  "GnbGfx.h"
#include  "GfxConfigLib.h"
#include  "GnbCommonLib.h"
#include  "OptionGnb.h"

#define FILECODE LIBRARY_GNBCZLIB_MODULES_GNBGFXCONFIG_GFXCONFIGLIB_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */


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
 * Get UMA info
 *
 * UMA info stored on heap by memory module
 *
 * @param[out] UmaInfo      Pointer to UMA info structure
 * @param[in]  StdHeader    Standard configuration header
 */

VOID
GfxGetUmaInfo (
     OUT   UMA_INFO          *UmaInfo,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UMA_INFO *MemUmaInfo;

  MemUmaInfo = GnbLocateHeapBuffer (AMD_UMA_INFO_HANDLE, StdHeader);
  if (MemUmaInfo == NULL) {
    LibAmdMemFill (UmaInfo, 0x00, sizeof (UMA_INFO), StdHeader);
    UmaInfo->UmaMode = UMA_NONE;
  } else {
    LibAmdMemCopy (UmaInfo, MemUmaInfo, sizeof (UMA_INFO), StdHeader);
    if ((UmaInfo->UmaBase == 0) || (UmaInfo->UmaSize == 0)) {
      UmaInfo->UmaMode = UMA_NONE;
    }
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * Locate UMA configuration data
 *
 *
 *
 * @param[in]     StdHeader     Standard configuration header
 * @param[in,out] Gfx           Pointer to GFX configuration
 * @retval        AGESA_STATUS  Data located
 * @retval        AGESA_FATA    Data not found
 */

AGESA_STATUS
GfxLocateConfigData (
  IN       AMD_CONFIG_PARAMS     *StdHeader,
     OUT   GFX_PLATFORM_CONFIG   **Gfx
  )
{
  *Gfx = GnbLocateHeapBuffer (AMD_GFX_PLATFORM_CONFIG_HANDLE, StdHeader);
  if (*Gfx == NULL) {
    IDS_ERROR_TRAP;
    return  AGESA_FATAL;
  }
  (*Gfx)->StdHeader = (PVOID) StdHeader;
  return AGESA_SUCCESS;
}


/*----------------------------------------------------------------------------------------*/
/**
 * Debug dump
 *
 *
 *
 * @param[in]     Gfx           Pointer to GFX configuration
 */

VOID
GfxConfigDebugDump (
  IN       GFX_PLATFORM_CONFIG      *Gfx
  )
{
  IDS_HDT_CONSOLE (GFX_MISC, "<-------------- GFX Config Start ------------->\n");
  IDS_HDT_CONSOLE (GFX_MISC, "  HD Audio - %s\n", (Gfx->GnbHdAudio == 0) ? "Disabled" : "Enabled");
  IDS_HDT_CONSOLE (GFX_MISC, "  DynamicRefreshRate - 0x%x\n", Gfx->DynamicRefreshRate);
  IDS_HDT_CONSOLE (GFX_MISC, "  LcdBackLightControl - 0x%x\n", Gfx->LcdBackLightControl);
  IDS_HDT_CONSOLE (GFX_MISC, "  AbmSupport - %s\n", (Gfx->AbmSupport == 0) ? "Disabled" : "Enabled");
  IDS_HDT_CONSOLE (GFX_MISC, "  GmcClockGating - %s\n", (Gfx->GmcClockGating == 0) ? "Disabled" : "Enabled");
  IDS_HDT_CONSOLE (GFX_MISC, "  GmcPowerGating - %s\n",
    (Gfx->GmcPowerGating == GmcPowerGatingDisabled) ? "Disabled" : (
    (Gfx->GmcPowerGating == GmcPowerGatingStutterOnly) ? "GmcPowerGatingStutterOnly" : (
    (Gfx->GmcPowerGating == GmcPowerGatingWithStutter) ? "GmcPowerGatingWithStutter" : "Unknown"))
  );
  IDS_HDT_CONSOLE (GFX_MISC, "  UmaSteering - %s\n",
    (Gfx->UmaSteering == DefaultGarlic) ? "DefaultGarlic" : (
    (Gfx->UmaSteering == SystemTrafficOnion) ? "SystemTrafficOnion" : (
    (Gfx->UmaSteering == Onion) ? "Onion" : (
    (Gfx->UmaSteering == Garlic) ? "Garlic" : "Unknown")))
    );
  IDS_HDT_CONSOLE (GFX_MISC, "  iGpuVgaMode - %s\n",
    (Gfx->iGpuVgaMode == iGpuVgaAdapter) ? "VGA" : (
    (Gfx->iGpuVgaMode == iGpuVgaNonAdapter) ? "Non VGA" : "Unknown")
    );
  IDS_HDT_CONSOLE (GFX_MISC, "  UmaMode - %s\n", (Gfx->UmaInfo.UmaMode == UMA_NONE) ? "No UMA" : "UMA");
  if (Gfx->UmaInfo.UmaMode != UMA_NONE) {
    IDS_HDT_CONSOLE (GFX_MISC, "  UmaBase - 0x%x\n", Gfx->UmaInfo.UmaBase);
    IDS_HDT_CONSOLE (GFX_MISC, "  UmaSize - 0x%x\n", Gfx->UmaInfo.UmaSize);
    IDS_HDT_CONSOLE (GFX_MISC, "  UmaAttributes - 0x%x\n", Gfx->UmaInfo.UmaAttributes);
  }
  IDS_HDT_CONSOLE (GFX_MISC, "  BootUpDisplayDevice - 0x%x\n", Gfx->BootUpDisplayDevice);
  IDS_HDT_CONSOLE (GFX_MISC, "<-------------- GFX Config End --------------->\n");

}

