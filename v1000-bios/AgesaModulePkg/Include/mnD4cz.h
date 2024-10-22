/* $NoKeywords:$ */
/**
 * @file
 *
 * mnD4cz.h
 *
 * Northbridge CZ
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/NB/CZ)
 * @e \$Revision: 319517 $ @e \$Date: 2015-05-28 12:14:11 +0800 (Thu, 28 May 2015) $
 *
 **/
/*****************************************************************************
  *
  * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
  *
  *******************************************************************************
  *
 */

#ifndef _MND4CZ_H_
#define _MND4CZ_H_

/*----------------------------------------------------------------------------
 *   Mixed (DEFINITIONS AND MACROS / TYPEDEFS, STRUCTURES, ENUMS)
 *
 *----------------------------------------------------------------------------
 */
#define DRAM_TYPE_DDR4_CZ             2
#define MAX_D4_DCTS_PER_NODE_CZ       2
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
MemNInitNBDataD4CZ (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNSetDdrModeD4CZ (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNConfigureDctForTrainingD4CZ (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNConfigureDctNormalD4CZ (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNProgramMemPstateRegD4CZ (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 MemPstate
  );

VOID
MemNProgramCycTimingsD4CZ (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNPredriverInitD4CZ (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNProgramCadDataBusD4CZ (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNPhyFifoConfigD4CZ (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

AGESA_STATUS
MemMD4FlowCZ (
  IN OUT   MEM_MAIN_DATA_BLOCK *MemMainPtr
  );

VOID
MemNSetPmuSequenceControlD4CZ (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNPopulatePmuSramConfigD4CZ (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNPopulatePmuSramTimingsD4CZ (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNModeRegisterInitializationD4CZ (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNProgramTurnaroundTimingsD4CZ (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNGetCSIntLvAddrD4CZ (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 BankAddrMap,
  OUT      UINT8 *LowBit,
  OUT      UINT8 *HiBit
  );

VOID
MemNProgramDqMappingsD4CZ (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

#endif  /* _MND4CZ_H_ */

