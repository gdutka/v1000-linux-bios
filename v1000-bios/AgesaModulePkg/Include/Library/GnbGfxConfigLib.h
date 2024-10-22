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
 * @e \$Revision: 313706 $   @e \$Date: 2015-02-25 21:00:43 -0600 (Wed, 25 Feb 2015) $
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
#ifndef _GNBGFXCONFIGLIB_H_
#define _GNBGFXCONFIGLIB_H_

#include <GnbGfx.h>

AGESA_STATUS
GfxConfigPostInterface (
  IN       AMD_CONFIG_PARAMS        *StdHeader
  );

AGESA_STATUS
GfxConfigEnvInterface (
  IN       AMD_CONFIG_PARAMS        *StdHeader
  );

AGESA_STATUS
GfxConfigMidInterface (
  IN       AMD_CONFIG_PARAMS        *StdHeader
  );

VOID
GfxConfigDebugDump (
  IN       GFX_PLATFORM_CONFIG      *Gfx
  );

VOID
GfxGetUmaInfo (
     OUT   UMA_INFO          *UmaInfo,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

AGESA_STATUS
GfxLocateConfigData (
  IN       AMD_CONFIG_PARAMS     *StdHeader,
     OUT   GFX_PLATFORM_CONFIG   **Gfx
  );

AGESA_STATUS
GfxEnableGmmAccess (
  IN OUT  GFX_PLATFORM_CONFIG         *Gfx
  );

#endif

