/* $NoKeywords:$ */
/**
 * @file
 *
 * PCIe family specific services.
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
#ifndef _GNBGFXFAMSERVICES_H_
#define _GNBGFXFAMSERVICES_H_

#include  "Gnb.h"
#include  "GnbGfx.h"
#include  "GnbPcie.h"

AGESA_STATUS
GfxFmMapEngineToDisplayPath (
  IN       PCIe_ENGINE_CONFIG          *Engine,
     OUT   EXT_DISPLAY_PATH            *DisplayPathList,
  IN       GFX_PLATFORM_CONFIG         *Gfx
  );

typedef AGESA_STATUS F_GFXMAPENGINETODISPLAYPATH (
  IN       PCIe_ENGINE_CONFIG          *Engine,
     OUT   EXT_DISPLAY_PATH            *DisplayPathList,
  IN       GFX_PLATFORM_CONFIG         *Gfx
  );

UINT32
GfxFmCalculateClock (
  IN       UINT8                       Did,
  IN       AMD_CONFIG_PARAMS           *StdHeader
  );

typedef UINT32 F_GFXCALCULATECLOCK (
  IN       UINT8                       Did,
  IN       AMD_CONFIG_PARAMS           *StdHeader
  );

BOOLEAN
GfxFmIsVbiosPosted (
  IN      GFX_PLATFORM_CONFIG          *Gfx
  );

typedef BOOLEAN F_GFXISVBIOSPOSTED (
  IN      GFX_PLATFORM_CONFIG          *Gfx
  );

VOID
GfxFmDisableController (
  IN      AMD_CONFIG_PARAMS     *StdHeader
  );

typedef VOID F_GFXDISABLECONTROLLER (
  IN      AMD_CONFIG_PARAMS     *StdHeader
  );

/// GFX Family services
typedef struct {
  F_GFXMAPENGINETODISPLAYPATH          *GfxMapEngineToDisplayPath;    ///< GfxMapEngineToDisplayPath
  F_GFXDISABLECONTROLLER               *GfxDisableController;         ///< GfxDisableController
  F_GFXCALCULATECLOCK                  *GfxCalculateClock;            ///< GfxCalculateClock
  F_GFXISVBIOSPOSTED                   *GfxIsVbiosPosted;             ///< GfxIsVbiosPosted;
} GFX_FAM_SERVICES;

#endif

