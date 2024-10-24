/**
 * @file
 *
 * AMD Integrated Debug Hook Routines
 *
 * Contains all functions related to IDS Hook
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  IDS
 * @e \$Revision: 309090 $   @e \$Date: 2014-12-10 02:28:05 +0800 (Wed, 10 Dec 2014) $
 */
/*****************************************************************************
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */

 /*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include <Uefi.h>
#include <Pi/PiMultiPhase.h>
#include <Library/AmdBaseLib.h>
#include <Library/IdsLib.h>
#include <Library/AmdIdsHookLib.h>
#include <Library/AmdHeapLib.h>
#include <Filecode.h>

#define FILECODE LIBRARY_AMDIDSHOOKLIB_AMDIDSHOOKLIB_FILECODE

/// Control IDS HOOK supported or not. TRUE: IDS HOOK supported. FALSE: IDS HOOK unsupported
BOOLEAN IsIdsHookEnabled (
  )
{
  return PcdGetBool (PcdAmdIdsHookEnable);
}


/// IDS HOOK dispatcher, it will loop and process the table report by GetIdsHookTable
IDS_HOOK_STATUS
IdsHookDispatcher (
  HOOK_ID HookId,
  VOID *Handle,
  VOID *Data
  )
{
  AGESA_STATUS Status;
  IDS_HOOK_STATUS IdsStatus;
  IDS_HOOK_STATUS ReturnStatus;
  IDS_HOOK_TABLE *IdsHookTable;
  IDS_HOOK_ELEMENT  *IdsHookElement;
  BOOLEAN IdsHookExecuted;

  ReturnStatus = IDS_HOOK_ERROR;
  Status = AGESA_ERROR;
  IdsHookExecuted = FALSE;
  IdsHookTable = NULL;
  Status = GetIdsHookTable (&IdsHookTable);
  if ((Status != AGESA_SUCCESS) || (IdsHookTable == NULL)) {
    return IDS_HOOK_UNSUPPORTED;
  }

  //Do simple check, Signature & Revision
  if ((IdsHookTable->Header.Signature != IDS_HOOK_TABLE_SIGNATURE) ||
      (IdsHookTable->Header.Revision != IDS_HOOK_TABLE_REV_1)) {
    ASSERT (FALSE);
    return IDS_HOOK_UNSUPPORTED;
  }

  IdsHookElement = IdsHookTable->pIdsHook;
  while (IdsHookElement->HookId != IDS_HOOK_ID_END) {
    if (IdsHookElement->HookId == HookId) {
      IDS_HDT_CONSOLE (IDS_TRACE, "IdsHookFunc HookId %x\n", HookId);
      //Call work function of specific HookId
      IdsStatus = IdsHookElement->IdsHookFunc (HookId, Handle, Data);
      IdsHookExecuted = TRUE;
      if (IdsStatus != IDS_HOOK_SUCCESS) {
        ReturnStatus = IdsStatus;
      }
    }
    IdsHookElement++;
  }

  if ((IdsHookElement->HookId == IDS_HOOK_ID_END) && (IdsHookExecuted == FALSE)) {
    ReturnStatus = IDS_HOOK_UNSUPPORTED;
  }
  return ReturnStatus;
}


/// Entry function of IDS HOOK
IDS_HOOK_STATUS
IdsHookEntry (
  HOOK_ID HookId,
  VOID *Handle,
  VOID *Data
  )
{
  IDS_HOOK_STATUS Status;

  //Call IP Driver self implemented IDS Hook functions
  Status = IdsHookDispatcher (HookId, Handle, Data);

  //Call External entry if haven't been implemented in IP driver
  if ((Status == IDS_HOOK_UNSUPPORTED)) {
    Status = IdsHookExtEntry (HookId, Handle, Data);
  }
  return Status;
}

#define IDSNVTABLE_FAULT_TOLERANT_SIZE  100
#define IDSNVTABLE_WATCHER_SIZE  (sizeof (UINT32))

/**
 *
 * Read IDS NV value
 *
 * It searches the IDS NV structure from  Heap until the Nv Id is found and return the NV value
 * in the table. Otherwise, return AGESA_UNSUPPORTED.
 *
 * @param[in] IdsNvId       IDS NV ID
 * @param[in,out] IdsNvValue    Pointer of IDS NV Value
 *
 * @retval AGESA_UNSUPPORTED  NV ID is not found in the structure
 *
 **/
AGESA_STATUS
AmdIdsNvReader (
  IN       IDS_NV_ID IdsNvId,
  IN OUT   UINT64 *IdsNvValue
  )
{
  LOCATE_HEAP_PTR        LocateHeapParams;
  ALLOCATE_HEAP_PARAMS   AllocateHeapParams;
  UINT32                 IdsNvTableSize;
  IDS_HOOK_STATUS        IdsHookStatus;
  IDS_NV_TABLE           *IdsNvTable;
  IDS_NV_RECORD_CMN      *IdsNvRecord;
  UINTN                  IdsNvRecordSize;
  AGESA_STATUS           Status;

  //Locate IDS NV table store in HEAP
  LocateHeapParams.BufferPtr = NULL;
  LocateHeapParams.BufferHandle = IDS_NV_TABLE_DATA_DXE_HANDLE;
  Status = HeapLocateBuffer (&LocateHeapParams, NULL);
  IdsNvTable = (IDS_NV_TABLE *) LocateHeapParams.BufferPtr;
  if (Status != AGESA_SUCCESS) {
    //If fail, Initialize the structure by calling IdsExtLib and
    //Store the data to new allocated HEAP
    //Set Size to 0, to get the real size
    IdsNvTable = NULL;
    IdsNvTableSize = 0;
    GetIdsNvTable (IdsNvTable, &IdsNvTableSize);
    if (IdsNvTableSize == 0) {
      //Can't get the IdsNvTable through IdsExtLib, error exit
      IDS_HDT_CONSOLE (CPU_TRACE, "IdsNvTableSize return 0, exit\n");
      return AGESA_UNSUPPORTED;
    }
   IDS_HDT_CONSOLE (CPU_TRACE, "Get IdsNV table size 0x%x\n", IdsNvTableSize);

    AllocateHeapParams.BufferHandle = IDS_NV_TABLE_DATA_DXE_HANDLE;
    AllocateHeapParams.Persist = HEAP_BOOTTIME_SYSTEM_MEM;
    //Add 100 additional bytes for fault-tolerant
    //Add 4 additional bytes for IdsExtLib memory overlap Check
    AllocateHeapParams.RequestedBufferSize = IdsNvTableSize + IDSNVTABLE_FAULT_TOLERANT_SIZE + IDSNVTABLE_WATCHER_SIZE;
    AllocateHeapParams.BufferPtr = NULL;
    Status = HeapAllocateBuffer (&AllocateHeapParams, NULL);
    ASSERT (AllocateHeapParams.BufferPtr != NULL);
    if (Status != AGESA_SUCCESS) {
      IDS_HDT_CONSOLE (CPU_TRACE, "Allocate Heap fail, exit\n");
      return Status;
    }

    //Fill the watcher dword to the tail of buffer
    *((UINT32 *) (AllocateHeapParams.BufferPtr + IdsNvTableSize + IDSNVTABLE_FAULT_TOLERANT_SIZE)) = 0x55AA;
    //Call IdsExtLib for init the buffer
    IdsHookStatus = GetIdsNvTable (AllocateHeapParams.BufferPtr, &IdsNvTableSize);
    ASSERT (IdsHookStatus == IDS_HOOK_SUCCESS);
    if (IdsHookStatus != IDS_HOOK_SUCCESS) {
      IDS_HDT_CONSOLE (CPU_TRACE, "GetIdsNvTable Status return fail, exit\n");
      return AGESA_ERROR;
    }
    //Verify the watcher
    if (*((UINT32 *) (AllocateHeapParams.BufferPtr + IdsNvTableSize + IDSNVTABLE_FAULT_TOLERANT_SIZE)) != 0x55AA) {
      IDS_HDT_CONSOLE (CPU_TRACE, "CBS data exceed the boundary\n");
      ASSERT (FALSE);
      return AGESA_BOUNDS_CHK;
    }
    IdsNvTable = (IDS_NV_TABLE *) AllocateHeapParams.BufferPtr;
  }

  ASSERT (IdsNvTable->Header.Signature == IDS_NV_TABLE_SIGNATURE);
  IdsNvRecordSize = 0;
  IdsNvRecord = (IDS_NV_RECORD_CMN *) &(IdsNvTable->NvRecords);
  //Search the NV table to find the according record
  while (IdsNvRecord->Id != IDS_NV_ID_END) {
    switch (IdsNvRecord->Attrib.size) {
    case IDS_NV_ATTRIB_SIZE_BYTE:
      *IdsNvValue = (UINT64) ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value;
      IdsNvRecordSize = sizeof (IDS_NV_RECORD_U8);
      break;
    case IDS_NV_ATTRIB_SIZE_WORD:
      *IdsNvValue = (UINT64) ((IDS_NV_RECORD_U16 *) IdsNvRecord)->Value;
      IdsNvRecordSize = sizeof (IDS_NV_RECORD_U16);
      break;
    case IDS_NV_ATTRIB_SIZE_DWORD:
      *IdsNvValue = (UINT64) ((IDS_NV_RECORD_U32 *) IdsNvRecord)->Value;
      IdsNvRecordSize = sizeof (IDS_NV_RECORD_U32);
      break;
    case IDS_NV_ATTRIB_SIZE_QWORD:
      *IdsNvValue = (UINT64) ((IDS_NV_RECORD_U64 *) IdsNvRecord)->Value;
      IdsNvRecordSize = sizeof (IDS_NV_RECORD_U64);
      break;
    default:
      ASSERT (FALSE);
      break;
    }
    if (IdsNvRecord->Id == IdsNvId) {
      break;
    }
    IdsNvRecord = (IDS_NV_RECORD_CMN *) ((UINT8 *) IdsNvRecord + IdsNvRecordSize);
  }

  return (IdsNvRecord->Id == IDS_NV_ID_END) ? AGESA_UNSUPPORTED : AGESA_SUCCESS;
}

