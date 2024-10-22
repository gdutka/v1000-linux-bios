/* $NoKeywords:$ */
/**
 * @file
 *
 * mftds.h
 *
 * Memory Controller
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


#ifndef _MFTDS_H_
#define _MFTDS_H_

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

VOID
MemFInitTableDrive (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8     time
  );

BOOLEAN
SetTableValuesLoop (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       MEM_TABLE_ALIAS *MTPtr,
  IN       UINT8     time
  );
#endif  /* _MFTDS_H_ */


