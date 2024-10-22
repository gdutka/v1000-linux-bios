/* $NoKeywords:$ */
/**
 * @file
 *
 * mnpmuD3cz.c
 *
 * Support functions for memory training using PMU for CZ
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/NB/CZ)
 * @e \$Revision: 317056 $ @e \$Date: 2015-04-20 22:06:48 +0800 (Mon, 20 Apr 2015) $
 *
 **/
/*****************************************************************************
*
* Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
*
*******************************************************************************
*
*/

/*
 *----------------------------------------------------------------------------
 *                                MODULES USED
 *
 *----------------------------------------------------------------------------
 */

#include "AGESA.h"
#include "mport.h"
#include "Library/IdsLib.h"
#include "Ids.h"
#include "cpuRegisters.h"
#include "mm.h"
#include "mn.h"
#include "mu.h"
#include "mt.h"
#include "mtsdi3.h"
#include "mnreg.h"
#include "mnpmu.h"
#include "mnPmuFirmwareCZ.h"
#include "mnPmuFirmwareD3cz.h"
#include "Filecode.h"
#include "Library/AmdHeapLib.h"
#include "mnPmuSramMsgBlockCZ.h"
#include "OptionMemory.h"
#include "mncz.h"
CODE_GROUP (G3_DXE)
RDATA_GROUP (G3_DXE)


#define FILECODE LIBRARY_MEM_NULL_NBCZLIBNULL_MNPMUD3CZ_FILECODE



/*----------------------------------------------------------------------------
 *                          DEFINITIONS AND MACROS
 *
 *----------------------------------------------------------------------------
 */
#define PMU_TRAINS_ECC_LANES       0x80    ///< PMU trains ECC lanes
/*----------------------------------------------------------------------------
 *                           TYPEDEFS AND STRUCTURES
 *
 *----------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------
 *                        PROTOTYPES OF LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */


ROMDATA PMU_FIRMWARE PmuFirmwareD3ImageTableCZ[] = {
//
//  PMU Signature  Instance   Package String      NULL Reserved   Size                                           Image Data
//
  {{PMU_SIGNATURE,    '0',    PMU_PACKAGE_STRING,  0,     {0}  }, PMU_SRAM_DMEM_SIZE_CZ + PMU_SRAM_IMEM_SIZE_CZ, PMU_DDR3_CZ_9221   },
  {{PMU_SIGNATURE,    '1',    PMU_PACKAGE_STRING,  0,     {0}  }, PMU_IMAGE_NOT_USEABLE },
};
ROMDATA UINT8 PmuFirmwareD3ImageTableSizeCZ = GET_SIZE_OF (PmuFirmwareD3ImageTableCZ);

/*----------------------------------------------------------------------------
 *                            EXPORTED FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */

/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function sets the PMU sequence control for DRAM training
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */
// This is a placholder for D3 Specific Sequence Control
//
// VOID
// MemNSetPmuSequenceControlD3CZ (
//   IN OUT   MEM_NB_BLOCK *NBPtr
//   )
// {
//   LOCATE_HEAP_PTR LocHeap;
//   PMU_SRAM_MSG_BLOCK_CZ *PmuSramMsgBlockPtr;
//
//   LocHeap.BufferHandle = AMD_MEM_PMU_SRAM_MSG_BLOCK_HANDLE;
//
//   if (HeapLocateBuffer (&LocHeap, &(NBPtr->MemPtr->StdHeader)) != AGESA_SUCCESS) {
//     ASSERT(FALSE); // Could not locate heap for PMU SRAM Message Block buffer.
//   }
//
//   PmuSramMsgBlockPtr = (PMU_SRAM_MSG_BLOCK_CZ *) LocHeap.BufferPtr;
//   //
//   // Call Common Sequence Control
//   //
//   MemNSetPmuSequenceControlCZ(NBPtr);
// }
//

/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function passes to PMU DRAM configuration over PMU SRAM for DDR3
 *     DRAM init and training
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */

VOID
MemNPopulatePmuSramConfigD3CZ (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  return;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function passes to PMU DRAM timings over PMU SRAM for DDR3 DRAM init and training.
 *     Timings are MemPstate dependent.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */

VOID
MemNPopulatePmuSramTimingsD3CZ (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{

}

/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function set the PMU sequence control for DRAM training
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */

VOID
MemNSetPmuSequenceControlD3CZ (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{

}
