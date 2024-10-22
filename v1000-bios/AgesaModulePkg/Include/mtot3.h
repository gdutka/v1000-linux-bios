/* $NoKeywords:$ */
/**
 * @file
 *
 * mtot3.h
 *
 * Technology Non-SPD timings for DDR3
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Tech/DDR3)
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

#ifndef _MTOT3_H_
#define _MTOT3_H_

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
MemTAdjustTwrwr3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  );

VOID
MemTAdjustTwrrd3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  );

INT8
MemTGetLD3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  );

#endif  /* _MTOT3_H_ */



