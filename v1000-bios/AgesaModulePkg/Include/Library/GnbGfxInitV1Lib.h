/* $NoKeywords:$ */
/**
 * @file
 *
 *  Gfx Library
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 313706 $   @e \$Date: 2015-02-25 21:00:43 -0600 (Wed, 25 Feb 2015) $:
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


#ifndef _GNBGFXINITV1LIB_H_
#define _GNBGFXINITV1LIB_H_

#include  <GnbDxio.h>
#include  <GnbGfx.h>
#include  <Library/GfxEnumConnectorsLib.h>
#include  <Library/GfxCardInfoLib.h>

BOOLEAN
GfxLibIsControllerPresent (
  IN      AMD_CONFIG_PARAMS           *StdHeader
  );

AGESA_STATUS
GfxInitSsid (
  IN      GFX_PLATFORM_CONFIG         *Gfx
  );


VOID
GfxLibCopyMemToFb (
  IN     VOID                         *Source,
  IN     UINT32                       FbOffset,
  IN     UINT32                       Length,
  IN     GFX_PLATFORM_CONFIG          *Gfx
  );

VOID
GfxLibSetiGpuVgaMode (
  IN     GFX_PLATFORM_CONFIG          *Gfx
  );

#endif

