/* $NoKeywords:$ */
/**
 * @file
 *
 * PSP related functions in SMM
 *
 * Contains PSP SMM Resume services
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  PSP
 * @e \$Revision$   @e \$Date$
 */
/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 **/

#include "AmdPspSmmV2.h"
#include "AmdPspResumeServicesV2.h"
#include <Library/CcxRolesLib.h>

#define FILECODE PSP_AMDPSPSMMV2_AMDPSPRESUMESERVICESV2_FILECODE

LIST_ENTRY   PspHeadList;

PSP_RESUME_SERVICE_PROTOCOL PspResumeProtocol = {
  PspRegisterV2,
  PspUnregisterV2,
};

EFI_STATUS
EFIAPI
InstallPspResumeCallbackProtocolV2 (
  )
{
  EFI_STATUS        Status;
  EFI_HANDLE        Handle;

  InitializeListHead (&PspHeadList);

  Handle = NULL;
  Status = gSmst->SmmInstallProtocolInterface (
                    &Handle,
                    &gAmdPspResumeServiceProtocolGuid,
                    EFI_NATIVE_INTERFACE,
                    &PspResumeProtocol
                    );

  return Status;
}

//
// Add callback note in terms with priority. Highest first
//
VOID
EFIAPI
AddCallbackNode (
  IN  PSP_SMM_CALLBACK_NODE *NewNode
  )
{
  LIST_ENTRY          *Node;

  if (IsListEmpty (&PspHeadList)) {
    InsertTailList (&PspHeadList, &(NewNode->ListEntry));
    return;
  }

 for (Node = GetFirstNode (&PspHeadList);
      Node != & PspHeadList;
      Node = GetNextNode (&PspHeadList, Node)) {

    if (NewNode->CallbackPriority >= ((PSP_SMM_CALLBACK_NODE *)Node)->CallbackPriority) {
      InsertHeadList (Node->BackLink, &(NewNode->ListEntry));
      return;
    }
  }

  InsertTailList (&PspHeadList, &(NewNode->ListEntry));

  return;
}

EFI_STATUS
EFIAPI
PspRegisterV2 (
  IN       PSP_RESUME_SERVICE_PROTOCOL        *This,
  IN       PSP_RESUME_CALLBACK                CallBackFunction,
  IN OUT   VOID                               *Context,
  IN       UINTN                              CallbackPriority,
  IN       PSP_RESUME_CALLBACK_FLAG           Flag,
     OUT   EFI_HANDLE                         *DispatchHandle
  )
{
  EFI_STATUS              Status;
  PSP_SMM_CALLBACK_NODE   *NewPspNode;

  Status = EFI_OUT_OF_RESOURCES;

  if (CallBackFunction == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = gSmst->SmmAllocatePool (
                       EfiRuntimeServicesData,
                       sizeof (PSP_SMM_CALLBACK_NODE),
                       (VOID**)&NewPspNode
                       );

  if (EFI_ERROR (Status)) {
    return (Status);
  }

  NewPspNode->CallBackFunction   = CallBackFunction;
  NewPspNode->Context            = Context;
  NewPspNode->CallbackPriority   = CallbackPriority;
  NewPspNode->Flag               = Flag;
  InitializeListHead (&(NewPspNode->ListEntry));

  AddCallbackNode (NewPspNode);
  if (DispatchHandle != NULL) {
    *DispatchHandle                = (EFI_HANDLE)NewPspNode;
  }
  return  EFI_SUCCESS;
}


/*----------------------------------------------------------------------------------------*/
/**
 * Unregister child handler
 *
 *
 * @param[in]       This                  Pointer to protocol
 * @param[in]       DispatchHandle
 *
 * @retval          EFI_SUCCESS           SMI handled by dispatcher
 * @retval          EFI_UNSUPPORTED       SMI not supported by dispcther
 */
/*----------------------------------------------------------------------------------------*/
EFI_STATUS
EFIAPI
PspUnregisterV2 (
  IN       PSP_RESUME_SERVICE_PROTOCOL  *This,
  IN       EFI_HANDLE                   DispatchHandle
  )
{
  EFI_STATUS  Status;
  Status = EFI_NOT_FOUND;

  RemoveEntryList (DispatchHandle);

  gSmst->SmmFreePool (DispatchHandle);
  return  Status;
}

EFI_STATUS
EFIAPI
PspResumeServiceCallBackV2 (
  IN       UINT8 ResumeType
   )
{
  LIST_ENTRY   *Node;
  CHAR8 *ResumeTypeStr;
  BOOLEAN IsBsp;
  BOOLEAN IsCorePrimary;
  BOOLEAN IsCcxPrimary;
  BOOLEAN IsDiePrimary;
  BOOLEAN IsSocketPrimary;
  BOOLEAN NeedExecute;

  NeedExecute     = FALSE;
  IsBsp           = CcxIsBsp (NULL);
  IsCorePrimary   = CcxIsComputeUnitPrimary (NULL);
  IsCcxPrimary    = CcxIsComplexPrimary (NULL);
  IsDiePrimary    = CcxIsDiePrimary (NULL);
  IsSocketPrimary = CcxIsSocketPrimary (NULL);
  
  ResumeTypeStr = ((ResumeType == ResumeFromConnectedStandby) ? "S0i3" : ((ResumeType == ResumeFromS3) ? "S3" : "Unsupported"));
  if (IsBsp) {
    IDS_HDT_CONSOLE_PSP_TRACE ("Psp.ResumeServiceCallBack %s\n", ResumeTypeStr);
  }

  for (Node = GetFirstNode (&PspHeadList);
       Node != &PspHeadList;
       Node = GetNextNode (&PspHeadList, Node)) {
    switch (((PSP_SMM_CALLBACK_NODE *)Node)->Flag) {
    case PspResumeCallBackFlagBspOnly:
      NeedExecute = IsBsp;
      break;
    case PspResumeCallBackFlagCorePrimaryOnly:
      NeedExecute = IsCorePrimary;
      break;
    case PspResumeCallBackFlagCcxPrimaryOnly:
      NeedExecute = IsCcxPrimary;
      break;
    case PspResumeCallBackFlagDiePrimaryOnly:
      NeedExecute = IsDiePrimary;
      break;
    case PspResumeCallBackFlagSocketPrimaryOnly:
      NeedExecute = IsSocketPrimary;
      break;
    case PspResumeCallBackFlagAllCores:
      NeedExecute = TRUE;
      break;
    default:
      ASSERT (FALSE);
      break;
    }
    if (NeedExecute) {
      if (IsBsp) {
        IDS_HDT_CONSOLE_PSP_TRACE ("Call PspResumeService.Hook at 0x%08x\n", (UINTN) ((PSP_SMM_CALLBACK_NODE *)Node)->CallBackFunction);
      }
      // Call all registered callback function in order of priority and pass the resume type
      ((PSP_SMM_CALLBACK_NODE *)Node)->CallBackFunction (
        ResumeType,
        ((PSP_SMM_CALLBACK_NODE *)Node)->Context
      );
    }
  }

  return EFI_SUCCESS;
}

