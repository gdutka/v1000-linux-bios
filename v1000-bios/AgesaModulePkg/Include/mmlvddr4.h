/* $NoKeywords:$ */
/**
 * @file
 *
 * mmLvDdr4.h
 *
 * Main low voltage DDR4 support common header
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Main)
 * @e \$Revision: 311790 $ @e \$Date: 2015-01-27 13:03:49 +0800 (Tue, 27 Jan 2015) $
 *
 **/
/*****************************************************************************
  *
  * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
  *
  *******************************************************************************
  *
 */

#ifndef _MMLVDDR4_H_
#define _MMLVDDR4_H_

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
BOOLEAN
MemMLvDdr4PerformanceEnhPre (
  IN OUT   MEM_MAIN_DATA_BLOCK *MemMainPtr
  );

BOOLEAN
MemMLvDdr4PerformanceEnhFinalize (
  IN OUT   MEM_MAIN_DATA_BLOCK *MemMainPtr
  );
#endif  /* _MMLVDDR4_H_ */

