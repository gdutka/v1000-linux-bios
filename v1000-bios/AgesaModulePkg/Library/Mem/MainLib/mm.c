/* $NoKeywords:$ */
/**
 * @file
 *
 * mm.c
 *
 * Main Memory Entrypoint file
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Main)
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

/*
 *----------------------------------------------------------------------------
 *                                MODULES USED
 *
 *----------------------------------------------------------------------------
 */



#include "AGESA.h"
#include "Library/AmdBaseLib.h"
#include "Library/AmdMemBaseV5Lib.h"
#include "Library/IdsLib.h"
#include "Ids.h"
#include "OptionMemory.h"
#include "mm.h"
#include "mn.h"
#include "mt.h"
#include "Library/AmdHeapLib.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE LIBRARY_MEM_MAINLIB_MM_FILECODE
/* features */

/*----------------------------------------------------------------------------
 *                          DEFINITIONS AND MACROS
 *
 *----------------------------------------------------------------------------
 */

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

/*----------------------------------------------------------------------------
 *                            EXPORTED FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *      This function deallocates heap buffers that were allocated in AmdMemAuto
 *
 *     @param[in,out]   *MemPtr   - Pointer to the MEM_DATA_STRUCT
 *
 *     @return          AGESA_STATUS
 *                          - AGESA_ALERT
 *                          - AGESA_FATAL
 *                          - AGESA_SUCCESS
 *                          - AGESA_WARNING
 */
AGESA_STATUS
MemAmdFinalize (
  IN OUT   MEM_DATA_STRUCT *MemPtr
  )
{
  UINT8 Die;

  for (Die = 0; Die < MemPtr->DieCount; Die++ ) {
    HeapDeallocateBuffer (GENERATE_MEM_HANDLE (ALLOC_TRN_DATA_HANDLE, Die, 0, 0), &MemPtr->StdHeader);
    HeapDeallocateBuffer (GENERATE_MEM_HANDLE (ALLOC_DCT_STRUCT_HANDLE, Die, 0, 0), &MemPtr->StdHeader);
  }

  HeapDeallocateBuffer (GENERATE_MEM_HANDLE (ALLOC_DIE_STRUCT_HANDLE, 0, 0, 0), &MemPtr->StdHeader);
  HeapDeallocateBuffer (AMD_S3_SAVE_HANDLE, &MemPtr->StdHeader);
  HeapDeallocateBuffer (AMD_MEM_SPD_HANDLE, &MemPtr->StdHeader);
  HeapDeallocateBuffer (AMD_MEM_AUTO_HANDLE, &MemPtr->StdHeader);
  HeapDeallocateBuffer (AMD_MEM_DATA_HANDLE, &MemPtr->StdHeader);

  return AGESA_SUCCESS;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 * MemSocketScan - Scan all nodes, recording the physical Socket number,
 * Die Number (relative to the socket), and PCI Device address of each
 * populated socket.
 *
 * This information is used by the northbridge block to map a dram
 * channel on a particular DCT, on a particular CPU Die, in a particular
 * socket to a the DRAM SPD Data for the DIMMS physically connected to
 * that channel.
 *
 * Also, the customer socket map is populated with pointers to the
 * appropriate channel structures, so that the customer can locate the
 * appropriate channel configuration data.
 *
 * This socket scan will always result in Die 0 as the BSP.
 *
 *     @param[in,out]   *mmPtr   - Pointer to the MEM_MAIN_DATA_BLOCK
 *
 */
AGESA_STATUS
MemSocketScan (
  IN OUT   MEM_MAIN_DATA_BLOCK *mmPtr
  )
{
  MEM_DATA_STRUCT *MemPtr;
  UINT8 DieIndex;
  UINT8 DieCount;
  UINT32 SocketId;
  UINT32 DieId;
  UINT8 Die;
  PCI_ADDR Address;
  AGESA_STATUS AgesaStatus;
  ALLOCATE_HEAP_PARAMS AllocHeapParams;

  ASSERT (mmPtr != NULL);
  ASSERT (mmPtr->MemPtr != NULL);
  MemPtr = mmPtr->MemPtr;

  //
  //  Count the number of dies in the system
  //
  DieCount = 0;
  for (Die = 0; Die < MAX_NODES_SUPPORTED; Die++) {
    if (GetSocketModuleOfNode ((UINT32)Die, &SocketId, &DieId, (VOID *)MemPtr)) {
      DieCount++;
    }
  }
  MemPtr->DieCount = DieCount;
  mmPtr->DieCount = DieCount;

  if (DieCount > 0) {
    //
    //  Allocate buffer for DIE_STRUCTs
    //
    AllocHeapParams.RequestedBufferSize = ((UINT16)DieCount * sizeof (DIE_STRUCT));
    AllocHeapParams.BufferHandle = GENERATE_MEM_HANDLE (ALLOC_DIE_STRUCT_HANDLE, 0, 0, 0);
    AllocHeapParams.Persist = HEAP_LOCAL_CACHE;
    if (HeapAllocateBuffer (&AllocHeapParams, &MemPtr->StdHeader) == AGESA_SUCCESS) {
      MemPtr->DiesPerSystem = (DIE_STRUCT *)AllocHeapParams.BufferPtr;
      //
      //  Find SocketId, DieId, and PCI address of each node
      //
      DieIndex = 0;
      for (Die = 0; Die < MAX_NODES_SUPPORTED; Die++) {
        if (GetSocketModuleOfNode ((UINT32)Die, &SocketId, &DieId, (VOID *)MemPtr)) {
          if (GetPciAddress ((VOID *)MemPtr, (UINT8)SocketId, (UINT8)DieId, &Address, &AgesaStatus)) {
            MemPtr->DiesPerSystem[DieIndex].SocketId = (UINT8)SocketId;
            MemPtr->DiesPerSystem[DieIndex].DieId = (UINT8)DieId;
            MemPtr->DiesPerSystem[DieIndex].PciAddr.AddressValue = Address.AddressValue;

            DieIndex++;
          }
        }
      }
      AgesaStatus = AGESA_SUCCESS;
    } else {
      ASSERT(FALSE); // Heap allocation failed for DIE_STRUCTs
      AgesaStatus = AGESA_FATAL;
    }
  } else {
    ASSERT(FALSE); // No die in the system
    AgesaStatus = AGESA_FATAL;
  }
  return AgesaStatus;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *      This function is default function for the fultion list
 *
 *     @param[in,out]   *pMemData   - Pointer to the MEM_DATA_STRUCT
 */
VOID
AmdMemFunctionListDef (
  IN OUT   VOID *pMemData
  )
{
}

