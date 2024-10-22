/* $NoKeywords:$ */
/**
 * @file
 *
 * mt3.h
 *
 * Common Technology
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Tech/DDR3)
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

#ifndef _MT3_H_
#define _MT3_H_

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
MemConstructTechBlock3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN OUT   MEM_NB_BLOCK *NBPtr
  );
BOOLEAN
MemTSetDramMode3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  );

BOOLEAN
MemTDIMMPresence3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  );

BOOLEAN
MemTSPDCalcWidth3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  );

BOOLEAN
MemTSPDGetTargetSpeed3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  );

BOOLEAN
MemTAutoCycTiming3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  );

BOOLEAN
MemTSPDSetBanks3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  );

VOID
MemTGetCSIntLvAddr3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 BankAddrMap,
  OUT      UINT8 *LowBit,
  OUT      UINT8 *HiBit
  );

VOID
MemTSendAllMRCmds3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 ChipSel
  );

VOID
FreqChgCtrlWrd3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  );


BOOLEAN
MemTGetDimmSpdBuffer3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN OUT   UINT8 **SpdBuffer,
  IN       UINT8 Dimm
  );
#endif  /* _MT3_H_ */



