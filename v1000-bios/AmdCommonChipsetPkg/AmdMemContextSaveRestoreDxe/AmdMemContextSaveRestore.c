/** @file
  A DXE driver for AMD memory context save and restore feature.  

;*******************************************************************************
;* Copyright (c) 2013 Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;*******************************************************************************
*/

#include <PiDxe.h>

#include <Guid/AmdMemoryContextSaveRestoreData.h>
#include <Guid/HobList.h>

#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/HobLib.h>
#include <Library/AmdExtCmosLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>

#include <Protocol/AmdMemContextSaveRestoreProtocol.h>

extern VOID                             *mHobList;

AMD_MEM_CONTEXT_SAVE_RESTORE_PROTOCOL   mAmdMemContextSaveRestoreProtocol;

EFI_STATUS
UpdateMemoryContextSaveRestoreCmosFlag (
  IN  UINT8  Flag
  );

BOOLEAN 
RestoreVolatileMemoryAddress (
  UINTN    *BufferPtrValue
  );

/**
  A ready to boot event for setting POST_PASS_FLAG to extended CMOS
  when the system signals a ReadyToBoot event.

  @param [in] Event        Event instance (unused).
  @param [in] Context      Event Context (It is not used).

  @retval void.

**/
VOID
SetPostPass (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  );

/**
  Gets Hob data for AMD_MEMORY_CONTEXT_SAVE_RESTORE_DATA to determine if 
  it will do Periodic Retraining or not.

  @param None.

  @retval EFI_SUCCESS    It always returns EFI_SUCCESS.

**/
EFI_STATUS
CheckPeriodicRetraining (
  VOID
  );

/**
  Initializes AmdMemContextSaveRestoreDxe DXE driver.

  @param [in] ImageHandle    The firmware allocated handle for the EFI image.
  @param [in] SystemTable    A pointer to the EFI System Table.

  @retval EFI_SUCCESS    The entry point is executed successfully.
  @retval Others         Some error occurs when executing this entry point.

**/
EFI_STATUS
DxeInitAmdMemContextSaveRestore (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS            Status;
  EFI_EVENT             ReadyToBootEvent;
  EFI_HANDLE            Handle;

  CheckPeriodicRetraining ();

  //
  // Sets PostPast in readytoboot event
  //
  Status = EfiCreateEventReadyToBootEx (
             TPL_NOTIFY,
             SetPostPass,
             NULL,
             &ReadyToBootEvent
             );
  ASSERT_EFI_ERROR (Status);

  ZeroMem (&mAmdMemContextSaveRestoreProtocol, sizeof (mAmdMemContextSaveRestoreProtocol));

  mAmdMemContextSaveRestoreProtocol.UpdateMemContextSaveRestoreCmosFlag = UpdateMemoryContextSaveRestoreCmosFlag;
  mAmdMemContextSaveRestoreProtocol.RestoreVolatileMemAddress           = RestoreVolatileMemoryAddress;

  //
  // Publish the AmdMemContextSaveRestore Protocol
  //
  Handle = ImageHandle;
  Status = gBS->InstallProtocolInterface (
                  &Handle,
                  &gAmdMemContextSaveRestoreProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &mAmdMemContextSaveRestoreProtocol
                  );

  return Status;
}

/**
  A ready to boot event for setting POST_PASS_FLAG to extended CMOS
  when the system signals a ReadyToBoot event.

  @param [in] Event        Event instance (unused).
  @param [in] Context      Event Context (It is not used).

  @retval void.

**/
VOID
SetPostPass (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
{
  UINT8  CmosData8;

  CmosData8 = AmdReadExtCmos8 (
                PcdGet8 (CmosAmdMemoryContextSaveRestore)
                );

  CmosData8 |= PcdGet8 (PostPassFlag);
  AmdWriteExtCmos8 (
    PcdGet8 (CmosAmdMemoryContextSaveRestore),
    CmosData8
    );
}

/**
  Gets Hob data for AMD_MEMORY_CONTEXT_SAVE_RESTORE_DATA to determine if 
  it will do Periodic Retraining or not.

  @param None.

  @retval EFI_SUCCESS    It always returns EFI_SUCCESS.

**/
EFI_STATUS
CheckPeriodicRetraining (
  VOID
  )
{
  EFI_STATUS                            Status;
  UINTN                                 VariableSize;
  UINT32                                VarAttrib;
  EFI_TIME                              CurrentTime;
  EFI_HOB_GUID_TYPE                     *GuidHob;
  AMD_MEMORY_CONTEXT_SAVE_RESTORE_DATA  *MemoryStatusData;

  MemoryStatusData = NULL;
  if (mHobList != NULL) {
    if ((GuidHob = GetNextGuidHob (&gAmdMemoryContextSaveRestoreDataGuid, mHobList)) != NULL) {
      //
      // It points to the memory after the GUID.
      //
      ++ GuidHob;
      MemoryStatusData = (AMD_MEMORY_CONTEXT_SAVE_RESTORE_DATA*) GuidHob;
    }
  }

  if (MemoryStatusData == NULL) {
    return EFI_NOT_FOUND;
  }

  if (MemoryStatusData->DoPeriodicRetraining) {
    Status = gRT->GetTime (&CurrentTime, NULL);
    if (EFI_ERROR (Status)) {
      return Status;
    }

    VariableSize = sizeof (EFI_TIME);
    VarAttrib = EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE;

    Status = gRT->SetVariable (
                    AMD_MEMORY_CONTEXT_LAST_TRAINING_TIME_NAME,
                    &gAmdMemContextSaveRestoreVariableGuid,
                    VarAttrib,
                    VariableSize,
                    &CurrentTime
                    );

    if (EFI_ERROR (Status)) {
      return Status;
    }
  }

  return EFI_SUCCESS;
}

EFI_STATUS
UpdateMemoryContextSaveRestoreCmosFlag (
  IN  UINT8  Flag
  )
{
  UINT8  CmosData8;

  CmosData8 = AmdReadExtCmos8 (PcdGet8 (CmosAmdMemoryContextSaveRestore));
  CmosData8 |= Flag;
  AmdWriteExtCmos8 (PcdGet8 (CmosAmdMemoryContextSaveRestore), CmosData8);

  return EFI_SUCCESS;
}

BOOLEAN
GetMemContextPolicy (
  )
{
  EFI_STATUS                            Status;
  VOID                                  *HobList;
  AMD_MEMORY_CONTEXT_SAVE_RESTORE_DATA  *MemoryStatusData;
  EFI_HOB_GUID_TYPE                     *GuidHob;

  HobList = NULL;
  Status = EfiGetSystemConfigurationTable (&gEfiHobListGuid, (VOID *) &HobList);
  if (EFI_ERROR (Status)) {
    return FALSE;
  }
  
  MemoryStatusData = NULL;
  GuidHob = GetNextGuidHob (&gAmdMemoryContextSaveRestoreDataGuid, HobList);
  MemoryStatusData = GET_GUID_HOB_DATA (GuidHob);

  if (MemoryStatusData == NULL) {
    return FALSE;
  }

  return MemoryStatusData->DoMemContextSaveRestore;
}

BOOLEAN 
RestoreVolatileMemoryAddress (
  UINTN    *BufferPtrValue
  )
{
  UINTN         StorePtrValue;
  UINTN         StorePtrAddress;
  VOID          *StorePtr;
  UINTN         StorePtrSize;
  BOOLEAN       MemoryRestoreFlag;
        
  MemoryRestoreFlag = GetMemContextPolicy ();
  
  if (MemoryRestoreFlag) {
    StorePtrValue = PcdGet32 (PcdFlashMctDataStoreBase) + sizeof (UINT64);
    StorePtr = (VOID*)(UINTN)StorePtrValue;
    StorePtrSize = *((UINTN*)StorePtr);
    StorePtrValue += sizeof (UINT64); 
    StorePtr = (VOID*)(UINTN)StorePtrValue;    
    StorePtrAddress = *((UINTN*)StorePtr);
    if ((StorePtrAddress!=0) && (StorePtrAddress != 0xFFFFFFFFFFFFFFFF)) {
      *BufferPtrValue = StorePtrAddress;
      return TRUE;
    }
  }
  return FALSE;
}
