/* $NoKeywords:$ */
/**
 * @file
 *
 * mfdmi.h
 *
 * Feature DMI functions
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem)
 * @e \$Revision: 309090 $ @e \$Date: 2014-12-10 02:28:05 +0800 (Wed, 10 Dec 2014) $
 *
 **/
/*****************************************************************************
  *
  * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
  *
  *******************************************************************************
  *
 */


#ifndef _MFDMI_H_
#define _MFDMI_H_

#include "MemDmi.h"

/*----------------------------------------------------------------------------
 *   Mixed (DEFINITIONS AND MACROS / TYPEDEFS, STRUCTURES, ENUMS)
 *
 *----------------------------------------------------------------------------
 */

/*-----------------------------------------------------------------------------
 *                         DEFINITIONS AND MACROS
 *
 *-----------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------
 *                         TYPEDEFS, STRUCTURES, ENUMS
 *
 *----------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------
 *                           FUNCTIONS PROTOTYPE
 *
 *----------------------------------------------------------------------------
 */

UINT64
MemFGetNodeMemBase (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 NodeLimit
  );

UINT64
MemFGetDctMemBase (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 DctLimit
  );

UINT64
MemFGetDctInterleavedMemSize (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

UINT64
MemFGetDctInterleavedLimit (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 Dct,
  IN       UINT64 DctMemBase,
  IN       UINT64 DctInterleavedMemSize
  );

#endif

