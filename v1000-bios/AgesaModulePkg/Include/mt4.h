/* $NoKeywords:$ */
/**
 * @file
 *
 * mt4.h
 *
 * Common Technology
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Tech/DDR4)
 * @e \$Revision: 322890 $ @e \$Date: 2015-07-20 14:11:57 +0800 (Mon, 20 Jul 2015) $
 *
 **/
/*****************************************************************************
  *
  * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
  *
  *******************************************************************************
  *
 */

#ifndef _MT4_H_
#define _MT4_H_

typedef struct _DDR4_NCK_MIN {
  UINT8  SpdOffset;
  UINT16 PageSize;
  UINT8  nCKMin; 
} DDR4_NCK_MIN;

/*----------------------------------------------------------------------------
 *   Mixed (DEFINITIONS AND MACROS / TYPEDEFS, STRUCTURES, ENUMS)
 *
 *----------------------------------------------------------------------------
 */

#define PGSZ_512 512
#define PGSZ_1K  1024
#define PGSZ_2K  2048
#define PGSZ_NA  0

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
MemConstructTechBlock4 (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

BOOLEAN
MemTDIMMPresence4 (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  );

BOOLEAN
MemTSPDCalcWidth4 (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  );

BOOLEAN
MemTSPDGetTargetSpeed4 (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  );

BOOLEAN
MemTAutoCycTiming4 (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  );

BOOLEAN
MemTSPDSetBanks4 (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  );

VOID
MemTGetCSIntLvAddr4 (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 BankAddrMap,
  OUT      UINT8 *LowBit,
  OUT      UINT8 *HiBit
  );

BOOLEAN
MemTGetDimmSpdBuffer4 (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN OUT   UINT8 **SpdBuffer,
  IN       UINT8 Dimm
  );
#endif  /* _MT4_H_ */



