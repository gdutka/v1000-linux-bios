/* $NoKeywords:$ */
/**
 * @file
 *
 * Various GFX service procedures
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
#ifndef _GFXLIBV5_H_
#define _GFXLIBV5_H_

UINT32
GfxLibExtractDramFrequencyV5 (
  IN       UINT8                  Encoding,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  );

AGESA_STATUS
GfxEnableGmmAccessV5 (
  IN OUT   GFX_PLATFORM_CONFIG   *Gfx
  );

AGESA_STATUS
GfxIntAudioEPEnumV5 (
  IN       GFX_PLATFORM_CONFIG    *Gfx,
  IN OUT   UINT8                  *AudioEPCount
  );
#endif

