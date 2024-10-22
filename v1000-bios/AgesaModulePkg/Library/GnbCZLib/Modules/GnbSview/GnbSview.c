/* $NoKeywords:$ */
/**
 * @file
 *
 * Interface to initialize Graphics Controller at mid POST
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 309352 $   @e \$Date: 2014-12-15 09:11:26 +0800 (Mon, 15 Dec 2014) $
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
#include  "GfxConfigLib.h"
#include  "GnbCommonLib.h"
#include  "GnbGfxFamServices.h"
#define FILECODE LIBRARY_GNBCZLIB_MODULES_GNBSVIEW_GNBSVIEW_FILECODE
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
AGESA_STATUS
GfxInitSview (
  IN      AMD_CONFIG_PARAMS               *StdHeader
  );


/*----------------------------------------------------------------------------------------*/
/**
 * Copy memory content to FB
 *
 *
 * @param[in] Source       Pointer to source
 * @param[in] FbOffset     FB offset
 * @param[in] Length       The length to copy
 * @param[in] Gfx          Pointer to global GFX configuration
 *
 */
VOID
GfxLibCopyMemToFb (
  IN     VOID                   *Source,
  IN     UINT32                  FbOffset,
  IN     UINT32                  Length,
  IN     GFX_PLATFORM_CONFIG    *Gfx
  )
{
  UINT32 GMMx00;
  UINT32 GMMx04;
  UINT32        Index;
  for (Index = 0; Index < Length; Index = Index + 4 ) {
    GMMx00 = 0x80000000 | (FbOffset + Index);
    GMMx04 = *(UINT32*) ((UINT8*)Source + Index);
    GnbLibMemWrite (Gfx->GmmBase, AccessWidth32, &GMMx00, GnbLibGetHeader (Gfx));
    GnbLibMemWrite (Gfx->GmmBase + 4, AccessWidth32, &GMMx04, GnbLibGetHeader (Gfx));
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * Init SVIEW configuration
 *
 *
 *
 * @param[in] StdHeader       Standard configuration header
 * @retval    AGESA_STATUS
 */

AGESA_STATUS
GfxInitSview (
  IN      AMD_CONFIG_PARAMS               *StdHeader
  )
{
  AGESA_STATUS          Status;
  AGESA_STATUS          AgesaStatus;
  GFX_PLATFORM_CONFIG   *Gfx;
  UINT32                Value32;
  IDS_HDT_CONSOLE (GNB_TRACE, "GfxInitSview Enter\n");
  AgesaStatus = AGESA_SUCCESS;
  Status =  GfxLocateConfigData (StdHeader, &Gfx);
  AGESA_STATUS_UPDATE (Status, AgesaStatus);
  if (Status == AGESA_SUCCESS) {
    if (GnbLibPciIsDevicePresent (MAKE_SBDFO (0, 0, 1, 0, 0), StdHeader)) {
      if (!GfxFmIsVbiosPosted (Gfx)) {
        GFX_VBIOS_IMAGE_INFO  VbiosImageInfo;
        LibAmdMemCopy (&VbiosImageInfo.StdHeader, StdHeader, sizeof (AMD_CONFIG_PARAMS), StdHeader);
        VbiosImageInfo.ImagePtr = NULL;
        VbiosImageInfo.GfxPciAddress = Gfx->GfxPciAddress;
        VbiosImageInfo.Flags = GFX_VBIOS_IMAGE_FLAG_SPECIAL_POST;
        GnbLibPciRead (Gfx->GfxPciAddress.AddressValue | 0x4, AccessWidth32, &Value32, StdHeader);
        GnbLibPciRMW (Gfx->GfxPciAddress.AddressValue | 0x4, AccessWidth8, 0xff, BIT1 | BIT2 | BIT0, StdHeader);
        Status = AgesaGetVbiosImage (0, &VbiosImageInfo);
        if (Status == AGESA_SUCCESS && VbiosImageInfo.ImagePtr != NULL) {
          GfxLibCopyMemToFb (VbiosImageInfo.ImagePtr, 0, (*((UINT8*) VbiosImageInfo.ImagePtr + 2)) << 9, Gfx);
        }
        GnbLibPciWrite (Gfx->GfxPciAddress.AddressValue | 0x4, AccessWidth32, &Value32, StdHeader);
      }
    }
  }
  IDS_HDT_CONSOLE (GNB_TRACE, "GfxInitSview Exit [0x%x]\n", AgesaStatus);
  return  AgesaStatus;
}

