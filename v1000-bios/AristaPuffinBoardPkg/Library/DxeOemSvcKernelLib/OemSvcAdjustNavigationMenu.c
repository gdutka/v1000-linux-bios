/** @file
  Adjust Setup menu which is displayed in Setup Utility.

;******************************************************************************
;* Copyright (c) 2013 - 2016, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

//#include <Protocol/HiiDatabase.h>
//#include <Guid/MdeModuleHii.h>
//#include <Library/BaseLib.h>
//#include <Library/UefiBootServicesTableLib.h>
//#include <Library/BaseMemoryLib.h>
//#include <Library/MemoryAllocationLib.h>
//#include <Library/DxeOemSvcKernelLib.h>
//#include <Library/UefiRuntimeServicesTableLib.h>
//#include <Library/HiiExLib.h>
//#include <ChipsetSetupConfig.h>

//EFI_HII_DATABASE_PROTOCOL      *mHiiDatabase;
//EFI_GUID                       mScuFormSetClassGuid = SETUP_UTILITY_FORMSET_CLASS_GUID;
//EFI_GUID                       mFsGuidSequence[] = {FORMSET_ID_GUID_MAIN,
//                                                    FORMSET_ID_GUID_ADVANCE,
//                                                    FORMSET_ID_GUID_SECURITY,
//                                                    FORMSET_ID_GUID_POWER,
//                                                    FORMSET_ID_GUID_BOOT,
//                                                    FORMSET_ID_GUID_EXIT
//                                                    };

///**
//  Check if input HII handle support specific form set GUID or not.
//
//  @param[in] FormsetGuid         The form set GUID.
//  @param[in] Handle              The HII handle.
//
//  @retval TRUE                   HII handle support specific form set GUID.
//  @retval FALSE                  HII handle does not support specific form set GUID.
//**/
//BOOLEAN
//InternalSupportFormsetGuid (
//  IN EFI_GUID                  *FormsetGuid,
//  IN EFI_HII_HANDLE            HiiHandle
//  )
//{
//  EFI_STATUS                   Status;
//  UINTN                        BufferSize;
//  EFI_HII_PACKAGE_LIST_HEADER  *HiiPackageList;
//  UINT8                        *Package;
//  UINT8                        *OpCodeData;
//  UINT32                       Offset;
//  UINT32                       PackageOffset;
//  UINT32                       PackageListLength;
//  EFI_HII_PACKAGE_HEADER       PackageHeader;
//
//  if (HiiHandle == NULL || FormsetGuid == NULL) {
//    return FALSE;
//  }
//
//  //
//  // Get HII package list
//  //
//  BufferSize = 0;
//  HiiPackageList = NULL;
//  Status = mHiiDatabase->ExportPackageLists (mHiiDatabase, HiiHandle, &BufferSize, HiiPackageList);
//  if (Status != EFI_BUFFER_TOO_SMALL) {
//    return FALSE;
//  }
//  HiiPackageList = AllocatePool (BufferSize);
//  if (HiiPackageList == NULL) {
//    return FALSE;
//  }
//  Status = mHiiDatabase->ExportPackageLists (mHiiDatabase, HiiHandle, &BufferSize, HiiPackageList);
//  if (EFI_ERROR (Status)) {
//    FreePool (HiiPackageList);
//    return FALSE;
//  }
//
//  //
//  // Get Form package and check form set class GUID in this HII package list
//  //
//  Offset = sizeof (EFI_HII_PACKAGE_LIST_HEADER);
//  PackageListLength = ReadUnaligned32 (&HiiPackageList->PackageLength);
//  while (Offset < PackageListLength) {
//    Package = (UINT8 *) HiiPackageList + Offset;
//    CopyMem (&PackageHeader, Package, sizeof (EFI_HII_PACKAGE_HEADER));
//
//    if (PackageHeader.Type == EFI_HII_PACKAGE_FORMS) {
//      PackageOffset = sizeof (EFI_HII_PACKAGE_HEADER);
//      while (PackageOffset < PackageHeader.Length) {
//        OpCodeData = Package + PackageOffset;
//
//        if (((EFI_IFR_OP_HEADER *) OpCodeData)->OpCode == EFI_IFR_FORM_SET_OP &&
//            CompareGuid ((EFI_GUID *) (VOID *) (&((EFI_IFR_FORM_SET *) OpCodeData)->Guid), FormsetGuid)) {
//          FreePool (HiiPackageList);
//          return TRUE;
//        }
//        PackageOffset += ((EFI_IFR_OP_HEADER *) OpCodeData)->Length;
//      }
//    }
//    Offset += PackageHeader.Length;
//  }
//
//  FreePool (HiiPackageList);
//
//  return FALSE;
//}

//#if 0
///**
//  Filter some Hii handle according to Setup user access level
//
//  @param[in]      FormSetClassGuid     Pointer to the formset GUID
//  @param[in, out] HiiHandle            Pointer to HII handle list
//  @param[in, out] HiiHandleCount       Pointer to the number of HII handle list
//
//  @retval EFI_SUCCESS                  Successfully filter Hii handle
//  @retval EFI_NOT_FOUND                Get Setup variable fail
//  @retval EFI_OUT_OF_RESOURCES         Allocate pool fail
//**/
//EFI_STATUS
//FilterByClassAndSubclass (
//  IN     EFI_GUID                      *FormSetClassGuid,
//  IN OUT EFI_HII_HANDLE                *HiiHandle,
//  IN OUT UINTN                         *HiiHandleCount
//  )
//{
//  EFI_STATUS                           Status;
//  KERNEL_CONFIGURATION                 *SetupVar;
//  UINTN                                BufferSize;
//  EFI_HII_HANDLE                       *List;
//  UINTN                                ListCount;
//  UINT32                               Index;
//  EFI_HII_PACKAGE_LIST_HEADER          *Buffer;
//  UINT8                                *TempPtr;
//  EFI_IFR_FORM_SET                     *FormSetPtr;
//  UINT16                               Class;
//  UINT16                               SubClass;
//  EFI_GUID                             SystemConfigurationGuid = SYSTEM_CONFIGURATION_GUID;
//
//  SetupVar   = NULL;
//  BufferSize = 0;
//  Status = gRT->GetVariable (L"Setup", &SystemConfigurationGuid, NULL, &BufferSize, (VOID *) SetupVar);
//  if (Status != EFI_BUFFER_TOO_SMALL) {
//    return EFI_NOT_FOUND;
//  }
//  SetupVar = AllocatePool (BufferSize);
//  if (SetupVar == NULL) {
//    return EFI_OUT_OF_RESOURCES;
//  }
//  gRT->GetVariable (L"Setup", &SystemConfigurationGuid, NULL, &BufferSize, (VOID *) SetupVar);
//
//  ListCount = 0;
//  List      = AllocatePool (*HiiHandleCount * sizeof(EFI_HII_HANDLE));
//  if (List == NULL) {
//    return EFI_UNSUPPORTED;
//  }
//
//  for (Index = 0; Index < *HiiHandleCount; Index++) {
//    //
//    // Am not initializing Buffer since the first thing checked is the size
//    // this way I can get the real buffersize in the smallest code size
//    //
//    Buffer     = NULL;
//    BufferSize = 0;
//    Status = mHiiDatabase->ExportPackageLists (mHiiDatabase, HiiHandle[Index], &BufferSize, Buffer);
//    if (Status == EFI_NOT_FOUND) {
//      break;
//    }
//
//    Buffer = AllocateZeroPool (BufferSize);
//    if (Buffer == NULL) {
//      Status = EFI_OUT_OF_RESOURCES;
//      break;
//    }
//
//    Status = mHiiDatabase->ExportPackageLists (mHiiDatabase, HiiHandle[Index], &BufferSize, Buffer);
//    if (EFI_ERROR (Status)) {
//      break;
//    }
//
//    TempPtr    = (UINT8 *) (Buffer + 1);
//    TempPtr    = (UINT8 *) ((EFI_HII_FORM_PACKAGE_HDR *) TempPtr + 1);
//    FormSetPtr = (EFI_IFR_FORM_SET *) TempPtr;
//
//    //
//    // check Platform form set guid
//    //
//    if (CompareGuid ((EFI_GUID *) (TempPtr + sizeof (EFI_IFR_FORM_SET)), FormSetClassGuid)) {
//      TempPtr  = (UINT8 *) (TempPtr + FormSetPtr->Header.Length);
//      Class    = ((EFI_IFR_GUID_CLASS *) TempPtr)->Class;
//      TempPtr += sizeof (EFI_IFR_GUID_CLASS);
//      SubClass = ((EFI_IFR_GUID_SUBCLASS *) TempPtr)->SubClass;
//
//      if ((Class == EFI_NON_DEVICE_CLASS) ||
//          (SubClass == EFI_USER_ACCESS_THREE && SetupVar->SetUserPass == 1 && SetupVar->UserAccessLevel == 3) ||
//          (SubClass == EFI_USER_ACCESS_TWO   && SetupVar->SetUserPass == 1 && SetupVar->UserAccessLevel == 2)) {
//        gBS->FreePool(Buffer);
//        continue;
//      }
//    }
//
//    List[ListCount] = HiiHandle[Index];
//    ListCount++;
//
//    gBS->FreePool(Buffer);
//  }
//
//  ZeroMem (HiiHandle, *HiiHandleCount * sizeof(EFI_HII_HANDLE));
//  CopyMem (HiiHandle, List, ListCount * sizeof(EFI_HII_HANDLE));
//  *HiiHandleCount = ListCount;
//
//  gBS->FreePool(SetupVar);
//  gBS->FreePool(List);
//
//  return Status;
//}
//#endif

/**
  Adjust setup menu which is displayed in Setup Utility

  @param[in, out] HiiHandle            Pointer to HII handle list
  @param[in, out] HiiHandleCount       Pointer to the number of HII handle list
  @param[in, out] FormSetGuid          Pointer to the formset GUID

  @retval EFI_UNSUPPORTED              Returns unsupported by default.
  @retval EFI_MEDIA_CHANGED            The value of IN OUT parameter is changed.
**/
EFI_STATUS
OemSvcAdjustNavigationMenu (
  IN OUT EFI_HII_HANDLE                *HiiHandle,
  IN OUT UINTN                         *HiiHandleCount,
  IN OUT EFI_GUID                      *FormSetGuid
  )
{
//  EFI_STATUS          Status;
//  EFI_HII_HANDLE      TargetHiiHandle;
//  EFI_HII_HANDLE      *List;
//  UINTN               ListCount;
//  UINTN               ListIndex;
//  UINTN               Index;
//  UINTN               FsGuidIndex;
//  UINTN               FsGuidCount;
//#if 0
//  UINTN               HiiHandleIndex;
//  UINT16              Count;
//  UINT16              OtherVfrStartLabelNum;
//  EFI_HII_HANDLE      OtherVfrHiiHandle;
//  EFI_GUID            OtherVfrFormSetGuid = FORMSET_ID_GUID_ADVANCE;
//#endif
//
//  if (HiiHandle == NULL || HiiHandleCount == NULL || *HiiHandleCount == 0) {
//    return EFI_UNSUPPORTED;
//  }
//
//  Status = gBS->LocateProtocol (&gEfiHiiDatabaseProtocolGuid, NULL, (VOID **) &mHiiDatabase);
//  if (EFI_ERROR (Status)) {
//    return EFI_UNSUPPORTED;
//  }
//
//  ListCount = *HiiHandleCount;
//  List      = AllocateCopyPool (ListCount * sizeof(EFI_HII_HANDLE), HiiHandle);
//  if (List == NULL) {
//    return EFI_UNSUPPORTED;
// }
//
//  //
//  // Sort setup menu order by formset GUID sequence list.
//  //
//  ListIndex = 0;
//  FsGuidCount = sizeof(mFsGuidSequence) / sizeof(EFI_GUID);
//  for (FsGuidIndex = 0; FsGuidIndex < FsGuidCount; FsGuidIndex++) {
//    for (Index = ListIndex; Index < ListCount; Index++) {
//      if (InternalSupportFormsetGuid (&mFsGuidSequence[FsGuidIndex], List[Index])) {
//        TargetHiiHandle = List[Index];
//
//        while (Index > ListIndex) {
//          List[Index] = List[Index - 1];
//          Index--;
//        }
//        List[ListIndex] = TargetHiiHandle;
//        ListIndex++;
//        break;
//      }
//    }
//  }
//#if 0
//  if (ListIndex != ListCount) {
//    //
//    // There is HII handle which formset is not displayed in navigation menu defined in mFsGuidSequence.
//    // Add goto opcode for these formsets in the root form of advance VFR.
//    //
//    OtherVfrHiiHandle = NULL;
//    for (HiiHandleIndex = 0; HiiHandleIndex < ListCount; HiiHandleIndex++) {
//      if (InternalSupportFormsetGuid (&OtherVfrFormSetGuid, HiiHandle[HiiHandleIndex])) {
//        OtherVfrHiiHandle = HiiHandle[HiiHandleIndex];
//        break;
//      }
//    }
//
//    if (OtherVfrHiiHandle != NULL) {
//      OtherVfrStartLabelNum = OTHER_VFR_GOTO_LABEL;
//      HiiCleanLabelOpcode (OtherVfrHiiHandle, &OtherVfrFormSetGuid, ROOT_FORM_ID, OtherVfrStartLabelNum, OTHER_VFR_GOTO_LABEL_END);
//
//      Count = 0;
//      for (HiiHandleIndex = 0; HiiHandleIndex < ListCount; HiiHandleIndex++) {
//        for (Index = 0; Index < ListIndex; Index++) {
//          if (List[Index] == HiiHandle[HiiHandleIndex]) {
//            break;
//          }
//        }
//        if (Index == ListIndex) {
//          HiiInsertRef3Opcode (
//            OtherVfrHiiHandle,
//            &OtherVfrFormSetGuid,
//            ROOT_FORM_ID,
//            OtherVfrStartLabelNum,
//            HiiHandle[HiiHandleIndex],
//            NULL,
//            0,
//            KEY_OTHER_VFR_GOTO_BASE + Count++
//            );
//        }
//      }
//    }
//  }
//
//  FilterByClassAndSubclass (&mScuFormSetClassGuid, List, &ListIndex);
//#endif
//  CopyMem (HiiHandle, List, ListIndex * sizeof(EFI_HII_HANDLE));
//  *HiiHandleCount = ListIndex;
//
//  gBS->FreePool (List);
//
//  return EFI_MEDIA_CHANGED;
  return EFI_UNSUPPORTED;
}