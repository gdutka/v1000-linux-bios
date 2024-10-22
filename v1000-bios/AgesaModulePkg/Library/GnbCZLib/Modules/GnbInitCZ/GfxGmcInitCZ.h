/* $NoKeywords:$ */
/**
 * @file
 *
 * various service procedures
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
#ifndef _GFXGMCINITCZ_H_
#define _GFXGMCINITCZ_H_

#include  "GnbRegistersCZ.h"

AGESA_STATUS
GfxGmcInitCZ (
  IN      GFX_PLATFORM_CONFIG   *Gfx
  );

VOID
GfxGmcInitializeFbLocationCZ (
  IN      GNB_HANDLE            *GnbHandle,
  IN      GFX_PLATFORM_CONFIG   *Gfx
  );

VOID
GfxGmcSecureGarlicAccessCZ (
  IN      GNB_HANDLE            *GnbHandle,
  IN      GFX_PLATFORM_CONFIG   *Gfx
  );

VOID
GfxGmcEnableStutterModePowerGatingCZ (
  IN      GNB_HANDLE            *GnbHandle,
  IN      GFX_PLATFORM_CONFIG   *Gfx
  );



#endif

