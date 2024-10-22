/* $NoKeywords:$ */
/**
 * @file
 *
 * Various GfxIntegratedInfoTable definitions
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
#ifndef _GFXINTEGRATEDINFOTABLEV5_H_
#define _GFXINTEGRATEDINFOTABLEV5_H_

AGESA_STATUS
GfxIntInfoTableInitV5 (
  IN      GFX_PLATFORM_CONFIG         *Gfx,
  IN      ATOM_FUSION_SYSTEM_INFO_V5  *SystemInfoTableV5,
  IN      PP_FUSE_ARRAY_V2             *PpFuseArray
  );

VOID
GfxIntInfoTablePostToFbV5 (
  IN       ATOM_FUSION_SYSTEM_INFO_V5     *SystemInfoTableV5Ptr,
  IN       GFX_PLATFORM_CONFIG            *Gfx
  );

VOID
GfxIntInfoTableDebugDumpV5 (
  IN       ATOM_FUSION_SYSTEM_INFO_V5     *SystemInfoTableV5Ptr,
  IN       GFX_PLATFORM_CONFIG            *Gfx
  );

// GMMx00/x04 are required for copying table to frame buffer
#ifndef GMMx00_ADDRESS
  #define GMMx00_ADDRESS                                          0x0
  #define GMMx04_ADDRESS                                          0x4
#endif

#endif

