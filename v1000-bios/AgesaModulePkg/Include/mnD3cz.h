/* $NoKeywords:$ */
/**
 * @file
 *
 * mnD3cz.h
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

#ifndef _MND3CZ_H_
#define _MND3CZ_H_

/*----------------------------------------------------------------------------
 *   Mixed (DEFINITIONS AND MACROS / TYPEDEFS, STRUCTURES, ENUMS)
 *
 *----------------------------------------------------------------------------
 */
#define MAX_D3_DCTS_PER_NODE_CZ       2
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
MemNInitNBDataD3CZ (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNSetDdrModeD3CZ (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNConfigureDctForTrainingD3CZ (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNConfigureDctNormalD3CZ (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNProgramMemPstateRegD3CZ (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 MemPstate
  );

VOID
MemNProgramCycTimingsD3CZ (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNPredriverInitD3CZ (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNProgramCadDataBusD3CZ (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNPhyFifoConfigD3CZ (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

AGESA_STATUS
MemMD3FlowCZ (
  IN OUT   MEM_MAIN_DATA_BLOCK *MemMainPtr
  );

VOID
MemNSetPmuSequenceControlD3CZ (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNPopulatePmuSramConfigD3CZ (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNPopulatePmuSramTimingsD3CZ (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNProgramTurnaroundTimingsD3CZ (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNModeRegisterInitializationD3CZ (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNGetCSIntLvAddrD3CZ (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 BankAddrMap,
  OUT      UINT8 *LowBit,
  OUT      UINT8 *HiBit
  );
#endif  /* _MND3CZ_H_ */

