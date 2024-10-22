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

#ifndef _GFXCONFIGLIB_H_
#define _GFXCONFIGLIB_H_

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

#endif

