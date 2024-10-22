/** @file
  PEI H2O check point library. It provides functionalities to register,
  unregister and trigger check point and also has function to get check point
  information from handle.

;******************************************************************************
;* Copyright (c) 2017 - 2019, Insyde Software Corporation. All Rights Reserved.
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
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/HobLib.h>

#define MAX_HANDLES_IN_HOB             8
#define MAX_HOB_INFO_COUNT             2
#define H2O_CP_INFO_SIGNATURE          SIGNATURE_32 ('H', 'C', 'P', 'I')
#define H2O_HOB_INFO_SIGNATURE         SIGNATURE_32 ('H', 'H', 'O', 'I')

typedef struct _INTERNAL_H2O_CP_HANDLE {
  UINTN                HandleIndex;
  H2O_CP_PRIORITY      Tpl;
  H2O_CP_HANDLER       Handler;
  UINTN                HandlerData;
  CONST CHAR8          *HandlerName;
  CONST CHAR8          *FunctionName;
  CONST CHAR8          *FileName;
} INTERNAL_H2O_CP_HANDLE;


typedef struct _INTERNAL_H2O_CP_INFO {
  UINT32                     Signature;
  UINTN                      NumberOfHandles;
  UINT32                     TriggerTimes;
  UINT32                     NumberOfHobs;
  INTERNAL_H2O_CP_HANDLE     Handles[MAX_HANDLES_IN_HOB];
  //
  // May have checkpoint data after this struct
  //
  // Void     *CheckPointData;
} INTERNAL_H2O_CP_INFO;

typedef struct _INTERNAL_H2O_CP_HOB_ADDRESS {
  UINTN        HobListStart;
  UINTN        EfiMemoryTop;
} INTERNAL_H2O_CP_HOB_ADDRESS;

typedef struct _INTERNAL_H2O_HOB_INFO {
  UINT32                       Signature;
  UINTN                        ValidHobInfo;
  INTERNAL_H2O_CP_HOB_ADDRESS  HobAddress[MAX_HOB_INFO_COUNT];
} INTERNAL_H2O_HOB_INFO;


STATIC EFI_GUID                 mInternalH2OPeiHobInfoGuid = {0x56586ced, 0xa2b1, 0x4f96, 0xb2, 0xce, 0x5a, 0x82, 0xa7, 0x69, 0x99, 0x7e};
STATIC H2O_CP_PRIORITY          mTpls[] = {H2O_CP_HIGH,
                                           H2O_CP_MEDIUM_HIGH,
                                           H2O_CP_MEDIUM,
                                           H2O_CP_MEDIUM_LOW,
                                           H2O_CP_LOW
                                          };
#define NUMBER_OF_TPL  (sizeof(mTpls)/sizeof(H2O_CP_PRIORITY))
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

  @return Pointer to ASCII string of H2O checkpoint Status.
**/
STATIC
CHAR8 *
CpStatusToStr (
  IN UINT32     Status
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
  Internal function to get GUID HOB from input H2O CP handle

  @param[in]  Handle            The handle that is associated with the registered checkpoint handler.

  @return  The pointer to start address of GUID HOB or NULL if not found.
**/
STATIC
EFI_HOB_GUID_TYPE *
GetGuidHobFromCpHandle (
  IN  INTERNAL_H2O_CP_HANDLE      *Handle
  )
{
  INTERNAL_H2O_CP_HANDLE        *CurrentHandle;
  INTERNAL_H2O_CP_INFO          *CpInfo;

  if (Handle->Handler == NULL) {
    return NULL;
  }

  CurrentHandle = Handle - Handle->HandleIndex;
  CpInfo = BASE_CR (CurrentHandle, INTERNAL_H2O_CP_INFO, Handles);
  return (EFI_HOB_GUID_TYPE *)(((UINT8 *)CpInfo) - sizeof (EFI_HOB_GUID_TYPE));
}

/**
  Internal function to add HOB address information to HOB

  @retval  EFI_SUCCESS                 Add new HOB address information successfully
  @retval  EFI_OUT_OF_RESOURCES        Unable to add new new HOB address information.
**/
STATIC
EFI_STATUS
AddHobAddressInfo (
  VOID
  )
{
  INTERNAL_H2O_HOB_INFO         *HobInfo;
  EFI_HOB_GUID_TYPE             *GuidHob;
  EFI_HOB_HANDOFF_INFO_TABLE    *HandOffHob;

  GuidHob = GetFirstGuidHob (&mInternalH2OPeiHobInfoGuid);
  if (GuidHob == NULL) {
    HobInfo = BuildGuidHob (&mInternalH2OPeiHobInfoGuid, sizeof (INTERNAL_H2O_HOB_INFO));
    if (HobInfo == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
    ZeroMem (HobInfo, sizeof (INTERNAL_H2O_HOB_INFO));
    HobInfo->Signature = H2O_HOB_INFO_SIGNATURE;
  } else {
    HobInfo = (INTERNAL_H2O_HOB_INFO *) GET_GUID_HOB_DATA (GuidHob);
  }
  if (HobInfo->ValidHobInfo >= MAX_HOB_INFO_COUNT) {
    return EFI_OUT_OF_RESOURCES;
  }
  HandOffHob = GetHobList ();
  if (HandOffHob == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  if (HobInfo->ValidHobInfo != 0 && (UINTN) HandOffHob == HobInfo->HobAddress[HobInfo->ValidHobInfo - 1].HobListStart) {
    return EFI_SUCCESS;
  }
  HobInfo->HobAddress[HobInfo->ValidHobInfo].HobListStart = (UINTN) HandOffHob;
  HobInfo->HobAddress[HobInfo->ValidHobInfo].EfiMemoryTop = (UINTN) HandOffHob->EfiMemoryTop;
  HobInfo->ValidHobInfo++;
  return EFI_SUCCESS;
}
/**
  Internal function to add registered CP GUID to internal database.

  @param[in]  Handle                   Input H2O_CP_HANDLE.

  @return     Pointer to INTERNAL_H2O_CP_HANDLE instance or NULL if not found.
**/
STATIC
INTERNAL_H2O_CP_HANDLE *
FindCpHandle (
  IN  H2O_CP_HANDLE     Handle
  )
{
  UINTN                          HandleAddress;
  UINTN                          Offset;
  EFI_HOB_GUID_TYPE             *GuidHob;
  INTERNAL_H2O_HOB_INFO         *HobInfo;
  UINTN                         Index;

  GuidHob = GetFirstGuidHob (&mInternalH2OPeiHobInfoGuid);
  if (GuidHob == NULL) {
    return NULL;
  }
  HobInfo = (INTERNAL_H2O_HOB_INFO *) GET_GUID_HOB_DATA (GuidHob);
  if (HobInfo->ValidHobInfo == 0) {
    return NULL;
  }
  if ((UINTN) GuidHob < HobInfo->HobAddress[HobInfo->ValidHobInfo - 1].HobListStart ||
      (UINTN) GuidHob > HobInfo->HobAddress[HobInfo->ValidHobInfo - 1].EfiMemoryTop) {
    AddHobAddressInfo ();
  }
  HandleAddress = (UINTN) Handle;
  if (HandleAddress >= HobInfo->HobAddress[HobInfo->ValidHobInfo - 1].HobListStart &&
      HandleAddress <= HobInfo->HobAddress[HobInfo->ValidHobInfo - 1].EfiMemoryTop) {
    return (INTERNAL_H2O_CP_HANDLE *) HandleAddress;
  }
  for (Index = 0; Index < HobInfo->ValidHobInfo - 1; Index++) {
    if (HandleAddress >= HobInfo->HobAddress[Index].HobListStart &&
        HandleAddress <= HobInfo->HobAddress[Index].EfiMemoryTop) {
      Offset = HandleAddress - HobInfo->HobAddress[Index].HobListStart;
      HandleAddress = Offset + HobInfo->HobAddress[HobInfo->ValidHobInfo - 1].HobListStart;
      return (INTERNAL_H2O_CP_HANDLE *) HandleAddress;
    }
  }
  return NULL;
}

/**
  Returns the first instance of the matched GUID HOB among the whole HOB list and the
  the singature in the HOB data is matched with the input signature.


  @param[in]  Guid          The GUID to match with in the HOB list.
  @param[in]  Signature     The signature to match with the HOB signature.

  @return The first instance of the matched GUID HOB among the whole HOB list.

**/
VOID *
EFIAPI
GetFirstValidGuidHob (
  IN CONST EFI_GUID         *Guid,
  IN CONST UINT32           Signature
  )
{
  EFI_HOB_GUID_TYPE             *GuidHob;

  for (GuidHob = GetFirstGuidHob (Guid); GuidHob != NULL; GuidHob = GetNextGuidHob (Guid, GET_NEXT_HOB (GuidHob))) {
    if (Signature == *((UINT32 *) GET_GUID_HOB_DATA (GuidHob))) {
      return  GuidHob;
    }
  }
  return NULL;
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
  EFI_HOB_GUID_TYPE             *GuidHob;
  INTERNAL_H2O_CP_INFO          *CpInfo;
  INTERNAL_H2O_CP_INFO          *FirstCpInfo;
  UINTN                         HobSize;

  if (H2OCheckpoint == NULL || Handler == NULL || Handle == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  if (Priority != H2O_CP_LOW && Priority != H2O_CP_MEDIUM_LOW && Priority != H2O_CP_MEDIUM &&
      Priority != H2O_CP_MEDIUM_HIGH && Priority != H2O_CP_HIGH) {
    return EFI_INVALID_PARAMETER;
  }

  CpInfo       = NULL;
  FirstCpInfo  = NULL;
  GuidHob = GetFirstValidGuidHob (H2OCheckpoint, H2O_CP_INFO_SIGNATURE);
  if (GuidHob != NULL) {
    //
    // Find GUID HOB which have free space to store input handler
    //
    CpInfo       = (INTERNAL_H2O_CP_INFO *) GET_GUID_HOB_DATA (GuidHob);
    FirstCpInfo  = CpInfo;
    while (CpInfo->Signature != H2O_CP_INFO_SIGNATURE || CpInfo->NumberOfHandles == MAX_HANDLES_IN_HOB) {
      GuidHob = GetNextGuidHob (H2OCheckpoint, GET_NEXT_HOB (GuidHob));
      if (GuidHob == NULL) {
        break;
      }
      CpInfo = (INTERNAL_H2O_CP_INFO *) GET_GUID_HOB_DATA (GuidHob);
    }
  }
  if (GuidHob == NULL) {
    HobSize = FirstCpInfo == NULL ? sizeof (INTERNAL_H2O_CP_INFO) + sizeof (VOID *) : sizeof (INTERNAL_H2O_CP_INFO);
    CpInfo = BuildGuidHob (H2OCheckpoint, HobSize);
    if (CpInfo == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
    ZeroMem (CpInfo, HobSize);
    if (FirstCpInfo == NULL) {
      FirstCpInfo = CpInfo;
    }
    FirstCpInfo->NumberOfHobs++;
    CpInfo->Signature = H2O_CP_INFO_SIGNATURE;
  }

  AddHobAddressInfo ();

  CpInfo->Handles[CpInfo->NumberOfHandles].HandleIndex   = CpInfo->NumberOfHandles;
  CpInfo->Handles[CpInfo->NumberOfHandles].Tpl           = Priority;
  CpInfo->Handles[CpInfo->NumberOfHandles].Handler       = Handler;
  CopyMem (&CpInfo->Handles[CpInfo->NumberOfHandles].HandlerData, (VOID *) ((UINTN) Handler), sizeof (UINTN));
  DEBUG_CODE (
    CpInfo->Handles[CpInfo->NumberOfHandles].HandlerName     = HandlerName;
    CpInfo->Handles[CpInfo->NumberOfHandles].FunctionName    = FunctionName;
    CpInfo->Handles[CpInfo->NumberOfHandles].FileName        = FileName;
    if (FirstCpInfo->TriggerTimes != 0) {
      DEBUG((DEBUG_INFO, "CP==>WARNING: Registration of Checkpoint handler %g happened after 1st Trigger.\n", H2OCheckpoint));
    }
  );
  CpInfo->NumberOfHandles++;

  *Handle =(H2O_CP_HANDLE) &CpInfo->Handles[CpInfo->NumberOfHandles - 1];
  return FirstCpInfo->TriggerTimes == 0 ? EFI_SUCCESS : EFI_WARN_STALE_DATA;
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
  EFI_HOB_GUID_TYPE             *GuidHob;
  EFI_HOB_GUID_TYPE             *FirstGuidHob;
  INTERNAL_H2O_CP_INFO          *CpInfo;
  INTERNAL_H2O_CP_HANDLE        *CpHandle;

  if (Handle == NULL || H2OCheckpointData == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  CpHandle = FindCpHandle (Handle);
  if (CpHandle == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  GuidHob = GetGuidHobFromCpHandle (CpHandle);
  if (GuidHob == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  FirstGuidHob = GetFirstValidGuidHob (&GuidHob->Name, H2O_CP_INFO_SIGNATURE);
  if (FirstGuidHob == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  CpInfo = (INTERNAL_H2O_CP_INFO *) GET_GUID_HOB_DATA (FirstGuidHob);
  CopyMem (H2OCheckpointData, (VOID *) (CpInfo + 1), sizeof (VOID *));
  if (H2OCheckpoint != NULL) {
    CopyGuid (H2OCheckpoint, &GuidHob->Name);
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
  EFI_HOB_GUID_TYPE        *GuidHob;
  EFI_HOB_GUID_TYPE        *FirstGuidHob;
  UINTN                    TplIndex;
  INTERNAL_H2O_CP_INFO     *FirstCpInfo;
  INTERNAL_H2O_CP_INFO     *CpInfo;
  UINTN                    HandleIndex;
  UINTN                    HobIndex;
  H2O_CP_DATA              *CpDataHeader;
  UINTN                    HandlerData;

  if (H2OCheckpoint == NULL || H2OCheckpointData == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  //
  // Check have any checkpoint has been registered.
  //
  FirstGuidHob = GetFirstValidGuidHob (H2OCheckpoint, H2O_CP_INFO_SIGNATURE);
  if (FirstGuidHob == NULL) {
    //
    // Create a new HOB to record trigger times if this checkpoint hasn't been registerred
    // before trigger.
    //
    CpInfo = BuildGuidHob (H2OCheckpoint, sizeof (INTERNAL_H2O_CP_INFO) + sizeof (VOID *));
    if (CpInfo == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
    ZeroMem (CpInfo, sizeof (INTERNAL_H2O_CP_INFO));
    CpInfo->Signature = H2O_CP_INFO_SIGNATURE;
    CpInfo->NumberOfHobs++;
    CpInfo->TriggerTimes++;
    return EFI_SUCCESS;
  }

  //
  // Save the checkpoint data at the end of first GUID HOB.
  //
  FirstCpInfo = (INTERNAL_H2O_CP_INFO *) GET_GUID_HOB_DATA (FirstGuidHob);
  CopyMem (FirstCpInfo + 1, &H2OCheckpointData, sizeof (VOID *));
  CpDataHeader = (H2O_CP_DATA *) H2OCheckpointData;
  //
  // Trigger all of callback functions according to different priorities.
  //
  for (TplIndex = 0; TplIndex < NUMBER_OF_TPL; TplIndex++){
    CpInfo = FirstCpInfo;
    GuidHob = FirstGuidHob;
    for (HobIndex = 0; ; HobIndex++) {
      ASSERT (CpInfo->NumberOfHandles <= MAX_HANDLES_IN_HOB);
      for (HandleIndex = 0; HandleIndex < CpInfo->NumberOfHandles; HandleIndex++) {
        if (mTpls[TplIndex] == CpInfo->Handles[HandleIndex].Tpl) {
          ASSERT (CpInfo->Handles[HandleIndex].Handler != NULL);
          CopyMem (&HandlerData, (VOID *) ((UINTN) CpInfo->Handles[HandleIndex].Handler), sizeof (UINTN));
          if (HandlerData != CpInfo->Handles[HandleIndex].HandlerData) {
            DEBUG((DEBUG_INFO, "CP==>Corrupted Handler: %a, Priority: %a, Function: %a, File Name: %a\n",
                               CpInfo->Handles[HandleIndex].HandlerName  == NULL ? "Unknown" : CpInfo->Handles[HandleIndex].HandlerName,
                               TplToStr (CpInfo->Handles[HandleIndex].Tpl),
                               CpInfo->Handles[HandleIndex].FunctionName == NULL ? "Unknown" : CpInfo->Handles[HandleIndex].FunctionName,
                               CpInfo->Handles[HandleIndex].FileName     == NULL ? "Unknown" : CpInfo->Handles[HandleIndex].FileName
                               ));
            continue;
          }
          DEBUG((DEBUG_INFO, "CP==>Call Handler: %a, Priority: %a, Function: %a, File Name: %a\n",
                             CpInfo->Handles[HandleIndex].HandlerName  == NULL ? "Unknown" : CpInfo->Handles[HandleIndex].HandlerName,
                             TplToStr (CpInfo->Handles[HandleIndex].Tpl),
                             CpInfo->Handles[HandleIndex].FunctionName == NULL ? "Unknown" : CpInfo->Handles[HandleIndex].FunctionName,
                             CpInfo->Handles[HandleIndex].FileName     == NULL ? "Unknown" : CpInfo->Handles[HandleIndex].FileName
                             ));
          CpInfo->Handles[HandleIndex].Handler (NULL, (H2O_CP_HANDLE) &CpInfo->Handles[HandleIndex]);
          DEBUG((DEBUG_INFO, "CP==>Handler: %a returned %a.\n",
                             CpInfo->Handles[HandleIndex].HandlerName  == NULL ? "Unknown" : CpInfo->Handles[HandleIndex].HandlerName,
                             CpStatusToStr(CpDataHeader->Status)
                             ));
          //
          // Clear H2O_CP_TASK_BREAK bit and return directly if callback function enables H2O_CP_TASK_BREAK bit to break this checkpoint.
          //
          if ((CpDataHeader->Status & H2O_CP_TASK_BREAK) == H2O_CP_TASK_BREAK) {
            CpDataHeader->Status &= ~H2O_CP_TASK_BREAK;
            FirstCpInfo->TriggerTimes++;
            DEBUG((DEBUG_INFO, "CP==>Break Handler: %a Function: %a, File Name: %a.\n",
                               CpInfo->Handles[HandleIndex].HandlerName  == NULL ? "Unknown" : CpInfo->Handles[HandleIndex].HandlerName,
                               CpInfo->Handles[HandleIndex].FunctionName == NULL ? "Unknown" : CpInfo->Handles[HandleIndex].FunctionName,
                               CpInfo->Handles[HandleIndex].FileName     == NULL ? "Unknown" : CpInfo->Handles[HandleIndex].FileName
                               ));
            return EFI_SUCCESS;
          }
        }
      }
      //
      // To prevent spend too much time to get HOB, break directly if it is last GUID HOB
      //
      if (HobIndex + 1 == FirstCpInfo->NumberOfHobs) {
        break;
      }

      for (GuidHob = GetNextGuidHob (H2OCheckpoint, GET_NEXT_HOB (GuidHob));
           GuidHob != NULL;
           GuidHob = GetNextGuidHob (H2OCheckpoint, GET_NEXT_HOB (GuidHob))) {
        CpInfo = (INTERNAL_H2O_CP_INFO *) GET_GUID_HOB_DATA (GuidHob);
        if (CpInfo->Signature == H2O_CP_INFO_SIGNATURE) {
          break;
        }
      }
      ASSERT (GuidHob != NULL);
      if (GuidHob == NULL) {
        break;
      }
    }
  }
  FirstCpInfo->TriggerTimes++;
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
  INTERNAL_H2O_CP_HANDLE     *CpHandle;

  CpHandle = FindCpHandle (Handle);
  if (CpHandle == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  //
  // Set Tpl in CpHandle to 0 to indicate this handle is unregister.
  //
  CpHandle->Tpl = 0;
  return EFI_SUCCESS;
}

