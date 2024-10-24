/* $NoKeywords:$ */
/**
 * @file
 *
 * GFX env post initialization.
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
#include  "GnbGfx.h"
#include  "GnbTable.h"
#include  "GnbPcieConfig.h"
#include  "GnbCommonLib.h"
#include  "GfxConfigLib.h"
#include  "GnbGfxFamServices.h"
#include  "GfxLibCZ.h"
#include  "GnbRegistersCZ.h"
#include  "GnbRegisterAccCZ.h"
#include  "GnbHandleLib.h"
#include  "OptionGnb.h"
#include  "Library/AmdHeapLib.h"

#define FILECODE LIBRARY_GNBCZLIB_MODULES_GNBINITCZ_GFXENVINITCZ_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

extern GNB_TABLE ROMDATA      GfxEnvInitTableCZ[];
//extern GNB_BUILD_OPTIONS_CZ   GnbBuildOptionsCZ;

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */
AGESA_STATUS
STATIC
GfxEnvInitCZ (
  IN      GFX_PLATFORM_CONFIG   *Gfx
  );

AGESA_STATUS
GfxEnvInterfaceCZ (
  IN      AMD_CONFIG_PARAMS     *StdHeader
  );

/*----------------------------------------------------------------------------------------*/
/**
 * Initialize GFX straps.
 *
 *
 * @param[in] Gfx             Pointer to global GFX configuration
 * @retval    AGESA_STATUS
 */

AGESA_STATUS
STATIC
GfxEnvInitCZ (
  IN      GFX_PLATFORM_CONFIG   *Gfx
  )
{
  D0F0xD4_x010914E1_STRUCT    D0F0xD4_x10914E1;
  D0F0xD4_x010914E2_STRUCT    D0F0xD4_x10914E2;
  D0F0xD4_x01091507_STRUCT    D0F0xD4_x1091507;
  D0F0x64_x1D_STRUCT          D0F0x64_x1D;
  UINT32                      D0F0xD4_x010914C3;

  GNB_HANDLE                 *GnbHandle;

  IDS_HDT_CONSOLE (GNB_TRACE, "GfxEnvInitCZ Enter\n");

  GnbHandle = GnbGetHandle (GnbLibGetHeader (Gfx));

  GnbRegisterReadCZ (GnbHandle, D0F0xD4_x010914E1_TYPE, D0F0xD4_x010914E1_ADDRESS, &D0F0xD4_x10914E1.Value, 0, GnbLibGetHeader (Gfx));
  GnbRegisterReadCZ (GnbHandle, D0F0xD4_x010914E2_TYPE, D0F0xD4_x010914E2_ADDRESS, &D0F0xD4_x10914E2.Value, 0, GnbLibGetHeader (Gfx));
  GnbRegisterReadCZ (GnbHandle, D0F0xD4_x01091507_TYPE, D0F0xD4_x01091507_ADDRESS, &D0F0xD4_x1091507.Value, 0, GnbLibGetHeader (Gfx));
  GnbRegisterReadCZ (GnbHandle, D0F0xD4_x010914C3_TYPE, D0F0xD4_x010914C3_ADDRESS, &D0F0xD4_x010914C3, 0, GnbLibGetHeader (Gfx));

  GnbLibPciIndirectRead (
    GNB_SBDFO | D0F0x60_ADDRESS,
    D0F0x64_x1D_ADDRESS,
    AccessWidth32,
    &D0F0x64_x1D.Value,
    GnbLibGetHeader (Gfx)
    );

  D0F0x64_x1D.Field.VgaEn = 0x1;
  D0F0x64_x1D.Field.Vga16En = 0x1;

  D0F0xD4_x10914E2.Field.StrapBifAzLegacyDeviceTypeDis = 0x0;
  D0F0xD4_x10914E2.Field.StrapBifF0LegacyDeviceTypeDis = 0x0;
  D0F0xD4_x1091507.Field.StrapBifAudioEnPin = Gfx->GnbHdAudio;
  D0F0xD4_x10914E2.Field.StrapBifAudioEn = Gfx->GnbHdAudio;

  D0F0xD4_x10914E1.Field.StrapBifRegApSize = 0x2;

  if (Gfx->UmaInfo.UmaSize > 128 * 0x100000) {
    D0F0xD4_x10914E1.Field.StrapBifMemApSize = 0x1;
    D0F0xD4_x1091507.Field.StrapBifMemApSizePin = 0x1;
  } else if (Gfx->UmaInfo.UmaSize > 64 * 0x100000) {
    D0F0xD4_x10914E1.Field.StrapBifMemApSize = 0x0;
    D0F0xD4_x1091507.Field.StrapBifMemApSizePin = 0x0;
  } else {
    D0F0xD4_x10914E1.Field.StrapBifMemApSize = 0x2;
    D0F0xD4_x1091507.Field.StrapBifMemApSizePin = 0x2;
  }

  GnbLibPciIndirectWrite (
    GNB_SBDFO | D0F0x60_ADDRESS,
    D0F0x64_x1D_ADDRESS,
    AccessS3SaveWidth32,
    &D0F0x64_x1D.Value,
    GnbLibGetHeader (Gfx)
    );

  D0F0xD4_x010914C3 |= BIT0;
  GnbRegisterWriteCZ (GnbHandle, D0F0xD4_x010914E1_TYPE, D0F0xD4_x010914E1_ADDRESS, &D0F0xD4_x10914E1.Value, GNB_REG_ACC_FLAG_S3SAVE, GnbLibGetHeader (Gfx));
  GnbRegisterWriteCZ (GnbHandle, D0F0xD4_x010914E2_TYPE, D0F0xD4_x010914E2_ADDRESS, &D0F0xD4_x10914E2.Value, GNB_REG_ACC_FLAG_S3SAVE, GnbLibGetHeader (Gfx));
  GnbRegisterWriteCZ (GnbHandle, D0F0xD4_x01091507_TYPE, D0F0xD4_x01091507_ADDRESS, &D0F0xD4_x1091507.Value, GNB_REG_ACC_FLAG_S3SAVE, GnbLibGetHeader (Gfx));
  GnbRegisterWriteCZ (GnbHandle, D0F0xD4_x010914C3_TYPE, D0F0xD4_x010914C3_ADDRESS, &D0F0xD4_x010914C3, GNB_REG_ACC_FLAG_S3SAVE, GnbLibGetHeader (Gfx));

  IDS_HDT_CONSOLE (GNB_TRACE, "GfxEnvInitCZ Exit\n");
  return  AGESA_SUCCESS;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Init GFX at Env Post.
 *
 *
 *
 * @param[in] StdHeader     Standard configuration header
 * @retval    AGESA_STATUS
  */


AGESA_STATUS
GfxEnvInterfaceCZ (
  IN      AMD_CONFIG_PARAMS               *StdHeader
  )
{
  AGESA_STATUS          Status;
  AGESA_STATUS          AgesaStatus;
  GFX_PLATFORM_CONFIG   *Gfx;
//  GNB_HANDLE            *GnbHandle;
  UINT32                Property;
  GNB_BUILD_OPTIONS_CZ  *GnbBuildOptionData;

  AGESA_TESTPOINT (TpGfxEnvInterfaceCZEntry, NULL);
  IDS_HDT_CONSOLE (GNB_TRACE, "GfxEnvInterfaceCZ Enter\n");
  AgesaStatus = AGESA_SUCCESS;
  Status = AGESA_SUCCESS;

  GnbBuildOptionData = GnbLocateHeapBuffer (AMD_GNB_BUILD_OPTIONS_HANDLE, StdHeader);
  ASSERT (GnbBuildOptionData != NULL);

  Property = TABLE_PROPERTY_DEFAULT;
  Property |= GnbBuildOptionData->CfgChubClockGating ? TABLE_PROPERTY_CHUB_CLOCK_GATING : 0;
  Property |= !GnbBuildOptionData->CfgAcpClockGating ? TABLE_PROPERTY_ACP_CLOCK_GATING_DISABLED : 0;

  Status = GfxLocateConfigData (StdHeader, &Gfx);
  AGESA_STATUS_UPDATE (Status, AgesaStatus);
  if (Status == AGESA_SUCCESS) {
    if (Gfx->UmaInfo.UmaMode != UMA_NONE) {
      Status = GfxEnvInitCZ (Gfx);
      AGESA_STATUS_UPDATE (Status, AgesaStatus);
      ASSERT (Status == AGESA_SUCCESS);
    } else {
      GfxFmDisableController (StdHeader);
      Property |= TABLE_PROPERTY_IGFX_DISABLED;
    }
  } else {
    GfxFmDisableController (StdHeader);
    Property |= TABLE_PROPERTY_IGFX_DISABLED;
  }

//  IDS_OPTION_HOOK (IDS_GNB_PROPERTY, &Property, StdHeader);

//  GnbHandle = GnbGetHandle (StdHeader);
//  ASSERT (GnbHandle != NULL);
//  Status = GnbProcessTable (
//             GnbHandle,
//             GfxEnvInitTableCZ,
//             Property,
//             GNB_TABLE_FLAGS_FORCE_S3_SAVE,
//             StdHeader
//             );
  AGESA_STATUS_UPDATE (Status, AgesaStatus);

  IDS_HDT_CONSOLE (GNB_TRACE, "GfxEnvInterfaceCZ Exit [0x%x]\n", AgesaStatus);
  AGESA_TESTPOINT (TpGfxEnvInterfaceCZExit, NULL);
  return  Status;
}

