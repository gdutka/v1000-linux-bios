/** @file
  DXE H2O check point library. It provides functionalities to register,
  unregister and trigger check point and also has function to get check point
  information from handle.

;******************************************************************************
;* Copyright (c) 2017 - 2018, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/
#include <Uefi.h>
#include <Guid/H2OCp.h>

#include <Library/H2OCpLib.h>
#include <Library/BaseLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiLib.h>

#include <Protocol/SmmBase2.h>

STATIC H2O_CP_PRIORITY       mTpls[] = {H2O_CP_HIGH,
                                        H2O_CP_MEDIUM_HIGH,
                                        H2O_CP_MEDIUM,
                                        H2O_CP_MEDIUM_LOW,
                                        H2O_CP_LOW
                                        };
#define NUMBER_OF_TPL       (sizeof(mTpls)/sizeof(H2O_CP_PRIORITY))

typedef struct {
  UINT32               Signature;
  LIST_ENTRY           RegisterLink;
  LIST_ENTRY           TriggerLink;
  EFI_GUID             Guid;
  H2O_CP_PRIORITY      Tpl;
  H2O_CP_HANDLER       Handler;
  CONST CHAR8          *HandlerName;
  CONST CHAR8          *FunctionName;
  CONST CHAR8          *FileName;
} INTERNAL_H2O_CP_HANDLE;

typedef struct {
  UINT32               Signature;
  LIST_ENTRY           HandleLink;
  LIST_ENTRY           RegisterLink[NUMBER_OF_TPL];
  LIST_ENTRY           TriggerLink[NUMBER_OF_TPL];
  UINTN                TriggerTimes;
  H2O_CP_PRIORITY      CurrentTriggerTpl;
  EFI_GUID             Guid;
  VOID                 *CpData;
} INTERNAL_H2O_CP_HANDLE_HEADER;

#define H2O_PRIVATE_CP_HEADER_SIGNATURE             SIGNATURE_32 ('H', 'P', 'C', 'H')
#define H2O_PRIVATE_CP_SIGNATURE                    SIGNATURE_32 ('H', 'P', 'C', 'P')
#define H2O_CP_HEADER_NODE_FROM_LINK(a)             CR (a, INTERNAL_H2O_CP_HANDLE_HEADER, HandleLink, H2O_PRIVATE_CP_HEADER_SIGNATURE)
#define H2O_CP_NODE_FROM_REGISTER_LINK(a)           CR (a, INTERNAL_H2O_CP_HANDLE, RegisterLink, H2O_PRIVATE_CP_SIGNATURE)
#define H2O_CP_NODE_FROM_TRIGGER_LINK(a)            CR (a, INTERNAL_H2O_CP_HANDLE, TriggerLink, H2O_PRIVATE_CP_SIGNATURE)

STATIC LIST_ENTRY                         *mCpList = NULL;
STATIC EFI_GUID                           mInternalH2OCpListHeadGuid = {0xf4540d21, 0x18c6, 0x4e74, 0xbc, 0x2d, 0x3c, 0x6f, 0xa0, 0x8d, 0x8b, 0xc6};
STATIC INTERNAL_H2O_CP_HANDLE             mCurrentCpHandle;

#ifndef MDEPKG_NDEBUG
/**
  Internal function to get H2O checkpoint priority string according to input priority.

  @param[in] H2O_CP_PRIORITY    Input H2O checkpoint priority.

  @return Pointer to ASCII string of H2O checkpoint priority.
**/
STATIC
CHAR8 *
TplToStr (
  H2O_CP_PRIORITY      Tpl
  )
{
   if (Tpl == H2O_CP_HIGH) {
     return "H2O_CP_HIGH";
   } else if (Tpl == H2O_CP_MEDIUM_HIGH) {
     return "H2O_CP_MEDIUM_HIGH";
   } else if (Tpl == H2O_CP_MEDIUM) {
     return "H2O_CP_MEDIUM";
   } else if (Tpl == H2O_CP_MEDIUM_LOW) {
     return "H2O_CP_MEDIUM_LOW";
   } else if (Tpl == H2O_CP_LOW) {
     return "H2O_CP_LOW";
   }
   return "UNKNOWN_CP_PRIORITY";
}

typedef struct {
  UINT32          Status;
  CHAR8           *String;
} CP_STATUS_TO_STRING;

CP_STATUS_TO_STRING      mCpStatusToStr[] = {
                          {H2O_CP_TASK_NORMAL,           "H2O_CP_TASK_NORMAL"},
                          {H2O_CP_TASK_SKIP,             "H2O_CP_TASK_SKIP"},
                          {H2O_CP_TASK_UPDATE,           "H2O_CP_TASK_UPDATE"},
                          {H2O_CP_TASK_BOOT_FAIL,        "H2O_CP_TASK_BOOT_FAIL"},
                          {H2O_CP_TASK_BREAK_NORMAL,     "H2O_CP_TASK_BREAK_NORMAL"},
                          {H2O_CP_TASK_BREAK_SKIP,       "H2O_CP_TASK_BREAK_SKIP"},
                          {H2O_CP_TASK_BREAK_UPDATE,     "H2O_CP_TASK_BREAK_UPDATE"},
                          {H2O_CP_TASK_BREAK_BOOT_FAIL,  "H2O_CP_TASK_BREAK_BOOT_FAIL"}
                         };
/**
  Internal function to get H2O checkpoint status string according to input status.

  @param[in] Status    Input H2O checkpoint Status.

  @return Pointer to ASCII string of H2O checkpoint status.
**/
STATIC
CHAR8 *
CpStatusToStr (
  IN UINT32      Status
  )
{
  UINTN         Count;
  UINTN         Index;

  Count = sizeof (mCpStatusToStr) / sizeof (CP_STATUS_TO_STRING);

  for (Index = 0; Index < Count; Index++) {
    if (Status == mCpStatusToStr[Index].Status) {
      return mCpStatusToStr[Index].String;
    }
  }
  return "UNKNOWN_CP_STATUS";
}
#endif

/**
  Internal function to get INTERNAL_H2O_CP_HANDLE_HEADER instance according to input checkpoint.

  @param[in]  H2OCheckpoint     Pointer to a GUID that specifies the checkpoint for which the
                                handler is being registered.

  @return Pointer to the INTERNAL_H2O_CP_HANDLE_HEADER instance or NULL if not found
**/
STATIC
INTERNAL_H2O_CP_HANDLE_HEADER  *
FindCpHeader (
  IN  CONST EFI_GUID      *H2OCheckpoint
  )
{
  LIST_ENTRY                        *List;
  INTERNAL_H2O_CP_HANDLE_HEADER     *CurrentCp;

  for (List = GetFirstNode (mCpList); List != mCpList; List = GetNextNode (mCpList, List)) {
    CurrentCp = H2O_CP_HEADER_NODE_FROM_LINK(List);
    if (CompareGuid (H2OCheckpoint, &CurrentCp->Guid)){
      return CurrentCp;
    }
  }
  return NULL;
}

/**
  Internal function to get array index according to input TPL.

  @param[in]  Tpl        Checkpoint priority level.

  @return The array index of the input TPL in mTpls array.
**/
STATIC
UINTN
GetIndexFromTpl (
  H2O_CP_PRIORITY        Tpl
  )
{
  UINTN        Index;

  for (Index = 0; Index < NUMBER_OF_TPL; Index++) {
    if (Tpl == mTpls[Index]) {
      return Index;
    }
  }

  ASSERT (FALSE);
  return 0;
}

/**
  This function registers a handler for the specified checkpoint with the specified priority.

  @param[in]  H2OCheckpoint     Pointer to a GUID that specifies the checkpoint for which the
                                handler is being registered.
  @param[in]  Handler           Pointer to the handler function.
  @param[in]  Priority          Enumerated value that specifies the priority with which the function
                                will be associated.
  @param[out] Handle            Pointer to the returned handle that is associated with the newly
                                registered checkpoint handler.
  @param[in]  HandlerName       Ascii string to handler function name.
  @param[in]  FunctionName      Ascii string to function name which we register handler function.
  @param[in]  FileName          Ascii string to file name which we register handler function.

  @retval EFI_SUCCESS           Register check point handle successfully.
  @retval EFI_INVALID_PARAMETER H2OCheckpoint ,Handler or Handle is NULL.
  @retval EFI_OUT_OF_RESOURCES  Allocate memory for Handle failed.
**/
EFI_STATUS
H2OCpRegister (
  IN  CONST EFI_GUID      *H2OCheckpoint,
  IN  H2O_CP_HANDLER      Handler,
  IN  H2O_CP_PRIORITY     Priority,
  OUT H2O_CP_HANDLE       *Handle,
  IN  CONST CHAR8         *HandlerName,
  IN  CONST CHAR8         *FunctionName,
  IN  CONST CHAR8         *FileName
  )
{
  INTERNAL_H2O_CP_HANDLE         *CpHandle;
  INTERNAL_H2O_CP_HANDLE_HEADER  *CpHandleHeader;
  UINTN                          Index;


  if (H2OCheckpoint == NULL || Handler == NULL || Handle == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  if (Priority != H2O_CP_LOW && Priority != H2O_CP_MEDIUM_LOW && Priority != H2O_CP_MEDIUM &&
      Priority != H2O_CP_MEDIUM_HIGH && Priority != H2O_CP_HIGH) {
    return EFI_INVALID_PARAMETER;
  }

  CpHandleHeader = FindCpHeader (H2OCheckpoint);
  if (CpHandleHeader == NULL) {
    CpHandleHeader = AllocateZeroPool (sizeof (INTERNAL_H2O_CP_HANDLE_HEADER));
    if (CpHandleHeader == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
    CpHandleHeader->Signature = H2O_PRIVATE_CP_HEADER_SIGNATURE;
    for (Index = 0; Index < NUMBER_OF_TPL; Index++) {
      InitializeListHead (&CpHandleHeader->RegisterLink[Index]);
    }
    CopyGuid (&CpHandleHeader->Guid, H2OCheckpoint);
    InsertTailList (mCpList, &CpHandleHeader->HandleLink);
  }

  CpHandle = AllocateZeroPool (sizeof (INTERNAL_H2O_CP_HANDLE));
  if (CpHandle == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  CpHandle->Signature = H2O_PRIVATE_CP_SIGNATURE;
  CopyGuid (&CpHandle->Guid, H2OCheckpoint);
  CpHandle->Tpl        = Priority;
  CpHandle->Handler    = Handler;
  DEBUG_CODE (
    CpHandle->HandlerName     = HandlerName;
    CpHandle->FunctionName    = FunctionName;
    CpHandle->FileName        = FileName;
    if (CpHandleHeader->TriggerTimes != 0) {
      DEBUG((DEBUG_INFO, "CP==>WARNING: Registration of Checkpoint handler %g happened after 1st Trigger.\n", H2OCheckpoint));
    }
  );
  InsertTailList (&CpHandleHeader->RegisterLink[GetIndexFromTpl (Priority)], &CpHandle->RegisterLink);
  *Handle = (H2O_CP_HANDLE) CpHandle;
  //
  // To make sure lower or equal priority checkpoint can be triggered, insert registered checkpoint to trigger list if
  // the priority of this checkpoint is lower or equal the current trigger priority.
  //
  if (CpHandleHeader->CurrentTriggerTpl != 0 && CpHandleHeader->CurrentTriggerTpl >= Priority) {
    InsertTailList (&CpHandleHeader->TriggerLink[GetIndexFromTpl (Priority)], &CpHandle->TriggerLink);
  }

  return CpHandleHeader->TriggerTimes == 0 ? EFI_SUCCESS : EFI_WARN_STALE_DATA;
}

/**
  This function returns the checkpoint data structure that was installed when the checkpoint was
  triggered and, optionally, the GUID that was associated with the checkpoint.

  @param[in]  Handle            The handle associated with a previously registered checkpoint
                                handler.
  @param[out] H2OCheckpointData The pointer to the checkpoint structure that was installed.
  @param[out] H2OCheckpoint     Optional pointer to the returned pointer to the checkpoint GUID.

  @retval EFI_SUCCESS           Get check point data successfully.
  @retval EFI_INVALID_PARAMETER Handle or H2OCheckpointData is NULL or Handle is invalid.
  @retval EFI_INVALID_PARAMETER It does not refer to a previously registered checkpoint handler.
  @return Others                Other error occurred while getting check point information.
**/
EFI_STATUS
H2OCpLookup (
  IN  H2O_CP_HANDLE     Handle,
  OUT VOID              **H2OCheckpointData,
  OUT EFI_GUID          *H2OCheckpoint       OPTIONAL
  )
{
  INTERNAL_H2O_CP_HANDLE         *CpHandle;
  INTERNAL_H2O_CP_HANDLE_HEADER  *CpHandleHeader;


  if (Handle == NULL || H2OCheckpointData == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Check handle signature and using guid from handle to find CP header to make
  // sure this is a valid handle.
  //
  CpHandle = (INTERNAL_H2O_CP_HANDLE *) Handle;
  if (CpHandle->Signature != H2O_PRIVATE_CP_SIGNATURE) {
    return EFI_INVALID_PARAMETER;
  }

  CpHandleHeader = FindCpHeader (&CpHandle->Guid);
  if (CpHandleHeader == NULL || CpHandleHeader->Signature != H2O_PRIVATE_CP_HEADER_SIGNATURE) {
    return EFI_INVALID_PARAMETER;
  }

  *H2OCheckpointData = CpHandleHeader->CpData;
  if (H2OCheckpoint != NULL) {
    CopyGuid (H2OCheckpoint, &CpHandle->Guid);
  }
  return EFI_SUCCESS;
}

/**
  This function install checks point data to check point GUID and triggers check point
  according to check point GUID.

  @param[in] H2OCheckpoint      Pointer to the GUID associated with the H2O checkpoint.
  @param[in] H2OCheckpointData  Pointer to the data associated with the H2O checkpoint.


  @retval EFI_SUCCESS           Trigger check point successfully.
  @retval EFI_INVALID_PARAMETER H2OCheckpoint or H2OCheckpointData is NULL.
  @retval Other                 Install H2OCheckpoint protocol failed.
**/
EFI_STATUS
H2OCpTrigger (
  IN CONST EFI_GUID *H2OCheckpoint,
  IN CONST VOID     *H2OCheckpointData
  )
{
  UINTN                          Index;
  LIST_ENTRY                     *List;
  LIST_ENTRY                     *Head;
  INTERNAL_H2O_CP_HANDLE_HEADER  *CpHandleHeader;
  INTERNAL_H2O_CP_HANDLE         *CpHandle;
  H2O_CP_DATA                    *CpDataHeader;

  if (H2OCheckpoint == NULL || H2OCheckpointData == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  CpHandleHeader = FindCpHeader (H2OCheckpoint);
  if (CpHandleHeader == NULL) {
    //
    // Create a new INTERNAL_H2O_CP_HANDLE_HEADER to record trigger times if this checkpoint hasn't
    // been registerred before trigger.
    //
    CpHandleHeader = AllocateZeroPool (sizeof (INTERNAL_H2O_CP_HANDLE_HEADER));
    if (CpHandleHeader == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
    CpHandleHeader->Signature = H2O_PRIVATE_CP_HEADER_SIGNATURE;
    for (Index = 0; Index < NUMBER_OF_TPL; Index++) {
      InitializeListHead (&CpHandleHeader->RegisterLink[Index]);
    }
    CopyGuid (&CpHandleHeader->Guid, H2OCheckpoint);
    InsertTailList (mCpList, &CpHandleHeader->HandleLink);
    CpHandleHeader->TriggerTimes++;
    return EFI_SUCCESS;
  }
  //
  // Assign CP data and trigger all CP hanlders.
  //
  CopyMem (&CpHandleHeader->CpData, &H2OCheckpointData, sizeof (VOID *));
  CpDataHeader = (H2O_CP_DATA *) H2OCheckpointData;
  //
  // Link all of handler to be triggered to TriggerLink
  //
  for (Index = 0; Index < NUMBER_OF_TPL; Index++) {
    InitializeListHead (&CpHandleHeader->TriggerLink[Index]);
    Head = &CpHandleHeader->RegisterLink[Index];
    for (List = GetFirstNode (Head); List != Head; List = GetNextNode (Head, List)) {
      CpHandle = H2O_CP_NODE_FROM_REGISTER_LINK (List);
      InsertTailList (&CpHandleHeader->TriggerLink[Index], &CpHandle->TriggerLink);
    }
  }
  //
  // Trigger all of the checkpoint handlers
  //
  for (Index = 0; Index < NUMBER_OF_TPL; Index++) {
    Head = &CpHandleHeader->TriggerLink[Index];
    CpHandleHeader->CurrentTriggerTpl = mTpls[Index];
    while (!IsListEmpty (Head)) {
      CpHandle = H2O_CP_NODE_FROM_TRIGGER_LINK(Head->ForwardLink);
      RemoveEntryList (&CpHandle->TriggerLink);
      CpHandle->TriggerLink.ForwardLink = NULL;
      DEBUG((DEBUG_INFO, "CP==>Call Handler: %a, Priority: %a, Function: %a, File Name: %a\n",
                         CpHandle->HandlerName  == NULL ? "Unknown" : CpHandle->HandlerName,
                         TplToStr(CpHandle->Tpl),
                         CpHandle->FunctionName == NULL ? "Unknown" : CpHandle->FunctionName,
                         CpHandle->FileName     == NULL ? "Unknown" : CpHandle->FileName
                         ));
      DEBUG_CODE (CopyMem (&mCurrentCpHandle, CpHandle, sizeof (INTERNAL_H2O_CP_HANDLE)););
      CpHandle->Handler (NULL, (H2O_CP_HANDLE) CpHandle);
      DEBUG((DEBUG_INFO, "CP==>Handler: %a returned %a.\n",
                         mCurrentCpHandle.HandlerName == NULL ? "Unknown" : mCurrentCpHandle.HandlerName,
                         CpStatusToStr(CpDataHeader->Status)
                         ));
      //
      // Clear H2O_CP_TASK_BREAK bit and return directly if callback function enables H2O_CP_TASK_BREAK bit to break this checkpoint.
      //
      if ((CpDataHeader->Status & H2O_CP_TASK_BREAK) == H2O_CP_TASK_BREAK) {
        CpDataHeader->Status &= ~H2O_CP_TASK_BREAK;
        CpHandleHeader->CurrentTriggerTpl = 0;
        CpHandleHeader->TriggerTimes++;
        DEBUG((DEBUG_INFO, "CP==>Break Handler: %a Function: %a, File Name: %a.\n",
                           mCurrentCpHandle.HandlerName  == NULL ? "Unknown" : mCurrentCpHandle.HandlerName,
                           mCurrentCpHandle.FunctionName == NULL ? "Unknown" : mCurrentCpHandle.FunctionName,
                           mCurrentCpHandle.FileName     == NULL ? "Unknown" : mCurrentCpHandle.FileName
                           ));
        return EFI_SUCCESS;
      }
    }
  }
  CpHandleHeader->CurrentTriggerTpl = 0;
  CpHandleHeader->TriggerTimes++;
  return EFI_SUCCESS;
}

/**
  This function unregisters the handle and frees any associated resources.

  @param[in] Handle             The handle that is associated with the registered checkpoint handler.

  @retval EFI_SUCCESS           The function completed successfully.
  @return EFI_INVALID_PARAMETER Handle is NULL, Handle is invalid or does not refer to a previously
                                registered checkpoint handler.
**/
EFI_STATUS
H2OCpUnregisterHandler (
  IN H2O_CP_HANDLE     Handle
  )
{
  LIST_ENTRY                     *List;
  LIST_ENTRY                     *Head;
  INTERNAL_H2O_CP_HANDLE         *CpHandle;
  INTERNAL_H2O_CP_HANDLE_HEADER  *CpHandleHeader;

  if (Handle == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  //
  // Check handle signature and using guid from handle to find CP header to make
  // sure this is a valid handle.
  //
  CpHandle = (INTERNAL_H2O_CP_HANDLE *) Handle;
  if (CpHandle->Signature != H2O_PRIVATE_CP_SIGNATURE) {
    return EFI_INVALID_PARAMETER;
  }
  CpHandleHeader = FindCpHeader (&CpHandle->Guid);
  if (CpHandleHeader == NULL || CpHandleHeader->Signature != H2O_PRIVATE_CP_HEADER_SIGNATURE) {
    return EFI_INVALID_PARAMETER;
  }

  Head = &CpHandleHeader->RegisterLink[GetIndexFromTpl (CpHandle->Tpl)];
  for (List = GetFirstNode (Head); List != Head; List = GetNextNode (Head, List)) {
    CpHandle = H2O_CP_NODE_FROM_REGISTER_LINK (List);
    if (CpHandle == Handle) {
      //
      // Remove the handler if queue in trigger list
      //
      if (CpHandle->TriggerLink.ForwardLink != NULL) {
        RemoveEntryList (&CpHandle->TriggerLink);
      }
      RemoveEntryList (List);
      FreePool (CpHandle);
      //
      // To prevent from all of the Handlers in the same checkpoint are unregistered in the H2OCpTrigger()
      // To make sure link list data in the H2OCpTrigger()is correct, Don't free INTERNAL_H2O_CP_HANDLE_HEADER
      // even if all of the Handlers in the same checkpoint are unregistered.
      //
      return EFI_SUCCESS;
    }
  }

  return EFI_INVALID_PARAMETER;
}

/**
  The constructor function saves the address of check point list address in an internal
  protocol. And then all of library instances can access the same database.

  It will ASSERT() if that operation fails and it will always return EFI_SUCCESS.

  @param  ImageHandle   The firmware allocated handle for the EFI image.
  @param  SystemTable   A pointer to the EFI System Table.

  @retval EFI_SUCCESS   This function completes  successfully.
  @return Others        Any error occurred while saving or getting check point address from
                        volatile variable.
**/
EFI_STATUS
EFIAPI
DxeH2OCpLibConstructor (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS                     Status;
  EFI_HANDLE                     Handle;
  EFI_SMM_BASE2_PROTOCOL         *SmmBase;
  EFI_SMM_SYSTEM_TABLE2          *Smst;
  BOOLEAN                        InSmm;

  Status = gBS->LocateProtocol (
                  &gEfiSmmBase2ProtocolGuid,
                  NULL,
                  (VOID **)&SmmBase
                  );
  InSmm = FALSE;
  if (!EFI_ERROR (Status)) {
    SmmBase->InSmm (SmmBase, &InSmm);
  }

  if (InSmm) {
    Status = SmmBase->GetSmstLocation(
                        SmmBase,
                        &Smst
                        );
    if (EFI_ERROR (Status)) {
      return Status;
    }
    Status = Smst->SmmLocateProtocol (
                      &mInternalH2OCpListHeadGuid,
                      NULL,
                      (VOID **) &mCpList
                      );
    if (EFI_ERROR (Status)) {
      mCpList = AllocatePool (sizeof (LIST_ENTRY));
      ASSERT (mCpList != NULL);
      if (mCpList == NULL) {
        return EFI_OUT_OF_RESOURCES;
      }
      InitializeListHead (mCpList);
      Handle = NULL;
      Status = Smst->SmmInstallProtocolInterface (
                        &Handle,
                        &mInternalH2OCpListHeadGuid,
                        EFI_NATIVE_INTERFACE,
                        (VOID *) mCpList
                        );
    }
  } else {
    Status = gBS->LocateProtocol (
                    &mInternalH2OCpListHeadGuid,
                    NULL,
                    (VOID **) &mCpList
                    );
    if (EFI_ERROR (Status)) {
      mCpList = AllocatePool (sizeof (LIST_ENTRY));
      ASSERT (mCpList != NULL);
      if (mCpList == NULL) {
        return EFI_OUT_OF_RESOURCES;
      }
      InitializeListHead (mCpList);
      Handle = NULL;
      Status = gBS->InstallProtocolInterface (
                      &Handle,
                      &mInternalH2OCpListHeadGuid,
                      EFI_NATIVE_INTERFACE,
                      (VOID *) mCpList
                      );
    }
  }
  return Status;
}
