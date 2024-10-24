/** @file
 Setup Utility application to display Setup Utility.

;******************************************************************************
;* Copyright (c) 2013 - 2020, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

*/

#include "SetupUtilityApp.h"

EFI_HII_DATABASE_PROTOCOL     *mHiiDatabase = NULL;
EFI_GUID                      mSetupVarGuid = SYSTEM_CONFIGURATION_GUID;
EFI_GUID                      mH2OFormBrowserProtocolGuid = {0xe1c17f13, 0xba6b, 0x42e5, 0x98, 0x5a, 0xb4, 0x18, 0x7c, 0xa0, 0x1a, 0x3f};

typedef enum {
  FormsetGuidFlagFormsetGuid      = 0,
  FormsetGuidFlagFormsetClassGuid = 1,
  FormsetGuidFlagAll              = 2
} FORMSET_GUID_FLAG;

/**
 Get specific HII package by type

 @param[in]  HiiPackageList            Pointer to HII package list
 @param[in]  HiiPackageListSize        HII package list size
 @param[in]  PackageType               Specific HII package type
 @param[out] Package                   Pointer to the pointer of specific HII package
 @param[out] PackageSize               Pointer to specific HII package size

 @retval EFI_SUCCESS                   Get specific HII package successfully
 @retval EFI_INVALID_PARAMETER         Invalid input parameter
 @retval EFI_NOT_FOUND                 Fail to get specific HII package from list
**/
STATIC
EFI_STATUS
GetHiiPackageByType (
  IN  EFI_HII_PACKAGE_LIST_HEADER      *HiiPackageList,
  IN  UINTN                            HiiPackageListSize,
  IN  UINT32                           PackageType,
  OUT UINT8                            **Package,
  OUT UINT32                           *PackageSize
  )
{
  EFI_HII_PACKAGE_HEADER               *PackageHeader;
  UINTN                                Offset;

  if (HiiPackageList == NULL || HiiPackageListSize == 0 || Package == NULL || PackageSize == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Offset = sizeof (EFI_HII_PACKAGE_LIST_HEADER);
  while (Offset < HiiPackageListSize) {
    PackageHeader = (EFI_HII_PACKAGE_HEADER *) ((UINT8 *) HiiPackageList + Offset);
    Offset       += PackageHeader->Length;

    if (PackageHeader->Type == PackageType) {
      *Package     = (UINT8 *) PackageHeader;
      *PackageSize = PackageHeader->Length;
      return EFI_SUCCESS;
    }
  }

  return EFI_NOT_FOUND;
}

/**
 Get formset related opcodes

 @param[in]  HiiPackageList            Pointer to HII package list
 @param[in]  HiiPackageListSize        HII package list size
 @param[out] FormsetOpcode             Pointer to the pointer of formset opcode
 @param[out] ClassOpcode               Pointer to the pointer of class opcode
 @param[out] SubClassOpcode            Pointer to the pointer of subclass opcode

 @retval EFI_SUCCESS                   Get formset related opcodes successfully
 @retval EFI_INVALID_PARAMETER         Invalid input parameter
 @retval EFI_NOT_FOUND                 Fail to get formset related opcodes from HII form package
 @retval Other                         Fail to get HII form package
**/
STATIC
EFI_STATUS
GetFormsetOpcodes (
  IN  EFI_HII_PACKAGE_LIST_HEADER      *HiiPackageList,
  IN  UINTN                            HiiPackageListSize,
  OUT EFI_IFR_FORM_SET                 **FormsetOpcode,
  OUT EFI_IFR_GUID_CLASS               **ClassOpcode,
  OUT EFI_IFR_GUID_SUBCLASS            **SubClassOpcode
  )
{
  EFI_STATUS                           Status;
  UINT8                                *HiiPackage;
  UINT32                               HiiPackageSize;
  UINTN                                Offset;
  UINT8                                *OpCodeData;
  EFI_IFR_GUID_CLASS                   *Class;
  EFI_IFR_GUID_SUBCLASS                *SubClass;

  if (HiiPackageList == NULL || HiiPackageListSize == 0 || FormsetOpcode == NULL || ClassOpcode == NULL || SubClassOpcode == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = GetHiiPackageByType (HiiPackageList, HiiPackageListSize, EFI_HII_PACKAGE_FORMS, &HiiPackage, &HiiPackageSize);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Offset = sizeof (EFI_HII_PACKAGE_HEADER);
  while (Offset < HiiPackageSize) {
    OpCodeData = HiiPackage + Offset;
    Offset    += ((EFI_IFR_OP_HEADER *) OpCodeData)->Length;

    if (((EFI_IFR_OP_HEADER *) OpCodeData)->OpCode != EFI_IFR_FORM_SET_OP) {
      continue;
    }

    *FormsetOpcode  = (EFI_IFR_FORM_SET *) OpCodeData;
    *ClassOpcode    = NULL;
    *SubClassOpcode = NULL;

    Class = (EFI_IFR_GUID_CLASS *) (OpCodeData + ((EFI_IFR_OP_HEADER *) OpCodeData)->Length);
    if (Class->Header.OpCode == EFI_IFR_GUID_OP && CompareGuid ((EFI_GUID *)((UINT8 *) Class + sizeof (EFI_IFR_OP_HEADER)), &gEfiIfrTianoGuid)) {
      *ClassOpcode = Class;

      SubClass = (EFI_IFR_GUID_SUBCLASS *) ((UINT8 *) Class + Class->Header.Length);
      if (SubClass->Header.OpCode == EFI_IFR_GUID_OP && CompareGuid ((EFI_GUID *)((UINT8 *) SubClass + sizeof (EFI_IFR_OP_HEADER)), &gEfiIfrTianoGuid)) {
        *SubClassOpcode = SubClass;
      }
    }
    return EFI_SUCCESS;
  }

  return EFI_NOT_FOUND;
}

/**
 Get user access information

 @param[out] CurrUser                  Pointer to current user
 @param[out] UserAccessLevel           Pointer to current user access level

 @retval EFI_SUCCESS                   Get user access information successfully
 @retval EFI_INVALID_PARAMETER         CurrUser or UserAccessLevel is NULL
 @retval EFI_OUT_OF_RESOURCES          Fail to get Setup variable
 @retval Other                         Fail to get current user from SysPasswordServiceProtocol
**/
STATIC
EFI_STATUS
GetUserAccessInfo (
  OUT CURRENT_USER_TYPE                *CurrUser,
  OUT UINT8                            *UserAccessLevel
  )
{
  EFI_STATUS                           Status;
  EFI_SYS_PASSWORD_SERVICE_PROTOCOL    *SysPasswordService;
  SYS_PASSWORD_SETUP_INFO_DATA         SetupInfoData;
  KERNEL_CONFIGURATION                 *KernelConfig;

  if (CurrUser == NULL || UserAccessLevel == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = gBS->LocateProtocol (&gEfiSysPasswordServiceProtocolGuid, NULL, (VOID **) &SysPasswordService);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = SysPasswordService->GetSysPswdSetupInfoData (SysPasswordService, &SetupInfoData);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  KernelConfig = CommonGetVariableData (SETUP_VARIABLE_NAME, &gSystemConfigurationGuid);
  if (KernelConfig == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  *CurrUser        = SetupInfoData.CurrentUser;
  *UserAccessLevel = KernelConfig->UserAccessLevel;

  FreePool (KernelConfig);
  return EFI_SUCCESS;
}

/**
 Filter out the HII handle which formset should be hidden by user access authority (user type and access level).

 The filter condition is the same as SetupUtilityDxe's IsSupportedFormSet() which is deprecated and should be removed
 when old form browser is removed.

 @param[in, out] HiiHandleList         Pointer to HII handle list
 @param[in, out] HiiHandleListCount    Pointer to HII handle list count

 @retval EFI_SUCCESS                   Filter out the HII handle successfully
 @retval EFI_INVALID_PARAMETER         HiiHandleList or HiiHandleListCount is NULL
 @retval EFI_OUT_OF_RESOURCES          Fail to allocate memory
 @retval Other                         Fail to get user access info
**/
EFI_STATUS
FilterByUserAccessAuthority (
  IN OUT EFI_HII_HANDLE             *HiiHandleList,
  IN OUT UINTN                      *HiiHandleListCount
  )
{
  EFI_STATUS                        Status;
  CURRENT_USER_TYPE                 CurrUser;
  UINT8                             UserAccessLevel;
  EFI_HII_HANDLE                    *List;
  UINTN                             ListCount;
  EFI_HII_PACKAGE_LIST_HEADER       *Buffer;
  UINTN                             BufferSize;
  UINTN                             Size;
  UINTN                             HiiHandleIndex;
  EFI_IFR_FORM_SET                  *FormsetOpcode;
  EFI_IFR_GUID_CLASS                *ClassOpcode;
  EFI_IFR_GUID_SUBCLASS             *SubClassOpcode;
  UINTN                             Index;
  EFI_GUID                          *ClassGuidList;
  UINT8                             ClassGuidListCount;
  EFI_GUID                          ScuClassGuid = SETUP_UTILITY_FORMSET_CLASS_GUID;

  if (HiiHandleList == NULL || HiiHandleListCount == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (*HiiHandleListCount == 0) {
    return EFI_SUCCESS;
  }

  Status = GetUserAccessInfo (&CurrUser, &UserAccessLevel);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  List = AllocatePool (*HiiHandleListCount * sizeof (EFI_HII_HANDLE));
  if (List == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  Buffer     = NULL;
  BufferSize = 0;
  ListCount  = 0;

  for (HiiHandleIndex = 0; HiiHandleIndex < *HiiHandleListCount; HiiHandleIndex++) {
    List[ListCount++] = HiiHandleList[HiiHandleIndex];
    Size = BufferSize;

    Status = mHiiDatabase->ExportPackageLists (mHiiDatabase, HiiHandleList[HiiHandleIndex], &Size, Buffer);
    if (Status == EFI_BUFFER_TOO_SMALL) {
      Buffer = ReallocatePool (BufferSize, Size, Buffer);
      if (Buffer == NULL) {
        return EFI_OUT_OF_RESOURCES;
      }
      BufferSize = Size;
      Status = mHiiDatabase->ExportPackageLists (mHiiDatabase, HiiHandleList[HiiHandleIndex], &Size, Buffer);
    }
    if (EFI_ERROR (Status)) {
      continue;
    }

    Status = GetFormsetOpcodes (Buffer, Size, &FormsetOpcode, &ClassOpcode, &SubClassOpcode);
    if (EFI_ERROR (Status)) {
      continue;
    }
    if (ClassOpcode == NULL || SubClassOpcode == NULL) {
      continue;
    }

    ClassGuidList      = (EFI_GUID *) ((UINT8 *) FormsetOpcode + sizeof (EFI_IFR_FORM_SET));
    ClassGuidListCount = (UINT8) (FormsetOpcode->Flags & 0x3);
    for (Index = 0; Index < ClassGuidListCount; Index++) {
      if (!CompareGuid (&ClassGuidList[Index], &ScuClassGuid)) {
        continue;
      }

      if ((ClassOpcode->Class == EFI_NON_DEVICE_CLASS) ||
          (SubClassOpcode->SubClass == EFI_USER_ACCESS_THREE && CurrUser == SystemUserType && UserAccessLevel == 3) ||
          (SubClassOpcode->SubClass == EFI_USER_ACCESS_TWO   && CurrUser == SystemUserType && UserAccessLevel == 2)) {
        ListCount--;
      }
      break;
    }
  }

  CopyMem (HiiHandleList, List, ListCount * sizeof (EFI_HII_HANDLE));
  *HiiHandleListCount = ListCount;

  FreePool(List);
  if (Buffer != NULL) {
    FreePool(Buffer);
  }
  return EFI_SUCCESS;
}

/**
  Check if input HII package support specific form set GUID or class GUID or not

  @param[in] FormsetGuid         The pointer to the list of form set GUID or class GUID
  @param[in] FormsetGuidFlag     The pointer to the list of form set GUID flag
  @param[in] FormSetGuidCount    The number of form set GUID or class GUID
  @param[in] Package             Pointer to HII package

  @retval TRUE                   HII package support specific form set class GUID
  @retval FALSE                  HII package does not support specific form set class GUID
**/
BOOLEAN
ScuAppSupportFormsetGuidByPackage (
  IN EFI_GUID                  *FormsetGuid,
  IN UINT8                     *FormsetGuidFlag,
  IN UINT32                    FormSetGuidCount,
  IN UINT8                     *Package
  )
{
  EFI_HII_PACKAGE_HEADER       *PackageHeader;
  UINT32                       Offset;
  UINT8                        *OpCodeData;
  UINT8                        ClassGuidNum;
  EFI_GUID                     *ClassGuid;
  UINT32                       Index;

  if (FormsetGuid == NULL || FormsetGuidFlag == NULL || Package == NULL || FormSetGuidCount == 0) {
    return FALSE;
  }

  PackageHeader = (EFI_HII_PACKAGE_HEADER *) Package;
  if (PackageHeader->Type != EFI_HII_PACKAGE_FORMS) {
    return FALSE;
  }

  Offset = sizeof (EFI_HII_PACKAGE_HEADER);
  while (Offset < PackageHeader->Length) {
    OpCodeData = Package + Offset;

    if (((EFI_IFR_OP_HEADER *) OpCodeData)->OpCode == EFI_IFR_FORM_SET_OP &&
        ((EFI_IFR_OP_HEADER *) OpCodeData)->Length > OFFSET_OF (EFI_IFR_FORM_SET, Flags)) {
      for (Index = 0; Index < FormSetGuidCount; Index++) {
        if ((FormsetGuidFlag[Index] == FormsetGuidFlagFormsetGuid ||
             FormsetGuidFlag[Index] == FormsetGuidFlagAll) &&
            CompareGuid ((GUID *)(VOID *)&((EFI_IFR_FORM_SET *) OpCodeData)->Guid, &FormsetGuid[Index])) {
          return TRUE;
        }
      }

      ClassGuidNum = (UINT8) (((EFI_IFR_FORM_SET *) OpCodeData)->Flags & 0x3);
      ClassGuid    = (EFI_GUID *) (VOID *) (OpCodeData + sizeof (EFI_IFR_FORM_SET));

      while (ClassGuidNum > 0) {
        for (Index = 0; Index < FormSetGuidCount; Index++) {
          if ((FormsetGuidFlag[Index] == FormsetGuidFlagFormsetClassGuid ||
               FormsetGuidFlag[Index] == FormsetGuidFlagAll) &&
              CompareGuid (ClassGuid, &FormsetGuid[Index])) {
            return TRUE;
          }
        }

        ClassGuidNum--;
        ClassGuid++;
      }
    }

    Offset += ((EFI_IFR_OP_HEADER *) OpCodeData)->Length;
  }

  return FALSE;
}

/**
  Check if input HII handle support specific form set GUID or class GUID or not.

  @param[in] FormsetGuid         The pointer to the list of form set GUID or class GUID
  @param[in] FormsetGuidFlag     The pointer to the list of form set GUID flag
  @param[in] FormSetGuidCount    The number of form set GUID or class GUID
  @param[in] Handle              The HII handle.

  @retval TRUE                   HII handle support specific form set class GUID.
  @retval FALSE                  HII handle does not support specific form set class GUID.
**/
BOOLEAN
ScuAppSupportFormsetGuid (
  IN EFI_GUID                  *FormsetGuid,
  IN UINT8                     *FormsetGuidFlag,
  IN UINT32                    FormSetGuidCount,
  IN EFI_HII_HANDLE            HiiHandle
  )
{
  EFI_STATUS                   Status;
  UINTN                        HiiPackageListSize;
  EFI_HII_PACKAGE_LIST_HEADER  *HiiPackageList;
  UINT8                        *Package;
  EFI_HII_PACKAGE_HEADER       *PackageHeader;
  UINT32                       Offset;
  UINT32                       PackageListLength;


  if (HiiHandle == NULL || FormsetGuid == NULL || FormSetGuidCount == 0) {
    return FALSE;
  }

  //
  // Get HII package list
  //
  HiiPackageList     = NULL;
  HiiPackageListSize = 0;
  Status = mHiiDatabase->ExportPackageLists (mHiiDatabase, HiiHandle, &HiiPackageListSize, HiiPackageList);
  if (Status != EFI_BUFFER_TOO_SMALL) {
    return FALSE;
  }
  HiiPackageList = AllocateZeroPool (HiiPackageListSize);
  if (HiiPackageList == NULL) {
    return FALSE;
  }
  Status = mHiiDatabase->ExportPackageLists (mHiiDatabase, HiiHandle, &HiiPackageListSize, HiiPackageList);
  if (EFI_ERROR (Status)) {
    FreePool (HiiPackageList);
    return FALSE;
  }

  //
  // In HII package list, find the Form package which support specific form set class GUID.
  //
  Offset            = sizeof (EFI_HII_PACKAGE_LIST_HEADER);
  PackageListLength = ReadUnaligned32 (&HiiPackageList->PackageLength);
  while (Offset < PackageListLength) {
    Package       = (UINT8 *) HiiPackageList + Offset;
    PackageHeader = (EFI_HII_PACKAGE_HEADER *) Package;

    if (PackageHeader->Type == EFI_HII_PACKAGE_FORMS &&
        ScuAppSupportFormsetGuidByPackage (FormsetGuid, FormsetGuidFlag, FormSetGuidCount, Package)) {
      FreePool (HiiPackageList);
      return TRUE;
    }

    Offset += PackageHeader->Length;
  }

  FreePool (HiiPackageList);

  return FALSE;
}

/**
  Get all SCU HII handles which defined by PCD

  @param[out] HiiHandleList        Pointer to HII handle list
  @param[out] HiiHandleListCount   Pointer to the number of HII handle list

  @retval EFI_SUCCESS              Success to get all SCU HII handles
  @retval EFI_NOT_FOUND            Not found SCU HII handle
  @retval EFI_OUT_OF_RESOURCES     Allocate memory fail
**/
EFI_STATUS
ScuAppGetScuHiiHandles (
  OUT EFI_HII_HANDLE             **HiiHandleList,
  OUT UINTN                      *HiiHandleListCount
  )
{
  EFI_HII_HANDLE                *HiiHandles;
  EFI_HII_HANDLE                *ListBuffer;
  UINTN                         ListCount;
  UINTN                         Index;
  UINT32                        FormSetGuidIndex;
  UINTN                         Num;
  EFI_GUID                      ZeroGuid;
  EFI_GUID                      *FormSetGuid;
  UINT32                        FormSetGuidCount;
  UINT8                         *FormsetGuidFlag;
  UINTN                         FormsetGuidFlagCount;

  ZeroMem (&ZeroGuid, sizeof (EFI_GUID));
  FormSetGuidCount = 0;
  FormSetGuid      = PcdGetPtr(PcdScuFormsetGuidList);
  while (!CompareGuid (&FormSetGuid[FormSetGuidCount], &ZeroGuid)) {
    FormSetGuidCount++;
  }
  if (FormSetGuidCount == 0) {
    return EFI_NOT_FOUND;
  }

  FormsetGuidFlag      = (UINT8 *) PcdGetPtr (PcdScuFormsetFlagList);
  FormsetGuidFlagCount = 0;
  while (FormsetGuidFlag[FormsetGuidFlagCount] != 0xFF) {
    FormsetGuidFlagCount++;
  }
  if (FormsetGuidFlagCount != FormSetGuidCount) {
    FormsetGuidFlagCount = FormSetGuidCount;
    FormsetGuidFlag      = AllocateZeroPool (FormSetGuidCount);
    if (FormsetGuidFlag == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
    for (Index = 0; Index < FormSetGuidCount; Index++) {
      FormsetGuidFlag[Index] = FormsetGuidFlagAll;
    }
  }

  HiiHandles = HiiGetHiiHandles (NULL);
  if (HiiHandles == NULL) {
    return EFI_NOT_FOUND;
  }
  ListCount = 0;
  while (HiiHandles[ListCount] != NULL) {
    ListCount++;
  }
  ListBuffer = AllocateZeroPool (ListCount * sizeof(EFI_HII_HANDLE));
  if (ListBuffer == NULL) {
    FreePool (HiiHandles);
    return EFI_OUT_OF_RESOURCES;
  }

  Num = 0;
  for (FormSetGuidIndex = 0; FormSetGuidIndex < FormSetGuidCount; FormSetGuidIndex++) {
    for (Index = 0; Index < ListCount; Index++) {
      if (HiiHandles[Index] != NULL && ScuAppSupportFormsetGuid (&FormSetGuid[FormSetGuidIndex], &FormsetGuidFlag[FormSetGuidIndex], 1, HiiHandles[Index])) {
        ListBuffer[Num]   = HiiHandles[Index];
        HiiHandles[Index] = NULL;
        Num++;
      }
    }
  }

  FilterByUserAccessAuthority (ListBuffer, &Num);

  FreePool (HiiHandles);
  if (FormsetGuidFlag != NULL &&
      FormsetGuidFlag != (UINT8 *) PcdGetPtr (PcdScuFormsetFlagList)) {
    FreePool (FormsetGuidFlag);
  }

  if (Num == 0) {
    FreePool (ListBuffer);
    return EFI_NOT_FOUND;
  }

  *HiiHandleList      = ListBuffer;
  *HiiHandleListCount = Num;

  return EFI_SUCCESS;
}


/**
  Get opcodes which is for Load Default hot key of Setup Utility

  @param[out] OpCodes              Pointer to opcodes which is for Load Default hot key
  @param[out] OpCodesSize          Pointer to the size of opcodes

  @retval EFI_SUCCESS              Success to get opcodes
  @retval EFI_OUT_OF_RESOURCES     Allocate memory fail
**/
EFI_STATUS
GetLoadDefaultOpCodes (
  OUT UINT8                    **OpCodes,
  OUT UINTN                    *OpCodesSize
  )
{
  UINT8                        *Buffer;
  UINT8                        *BufferPtr;
  UINTN                        BufferSize;
  EFI_STRING_ID                StringId;
  EFI_IFR_FORM                 *OpCodeForm;
  EFI_IFR_ACTION               *OpCodeAction;
  EFI_IFR_END                  *OpCodeEnd;


  //
  //  Genernate following opcodes for Load Default hot key of Setup Utility
  //
  //  form
  //    formid = 0xFFFF,
  //    title = STRING_TOKEN();
  //
  //    text
  //      help   = STRING_TOKEN(),
  //      text   = STRING_TOKEN(),
  //      flags  = INTERACTIVE,
  //      key    = KEY_SCAN_F9;
  //  endform;
  //
  BufferSize = sizeof(EFI_IFR_FORM) + sizeof(EFI_IFR_ACTION) + sizeof(EFI_IFR_END) + sizeof(EFI_IFR_END);
  Buffer     = AllocateZeroPool (BufferSize);
  if (Buffer == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  BufferPtr = Buffer;

  StringId = 0x1;

  OpCodeForm                = (EFI_IFR_FORM *) BufferPtr;
  OpCodeForm->Header.OpCode = EFI_IFR_FORM_OP;
  OpCodeForm->Header.Length = sizeof(EFI_IFR_FORM);
  OpCodeForm->Header.Scope  = 1;
  OpCodeForm->FormId        = 0xFFFF;
  OpCodeForm->FormTitle     = StringId;
  BufferPtr += sizeof(EFI_IFR_FORM);

  OpCodeAction                         = (EFI_IFR_ACTION *) BufferPtr;
  OpCodeAction->Header.OpCode          = EFI_IFR_ACTION_OP;
  OpCodeAction->Header.Length          = sizeof(EFI_IFR_ACTION);
  OpCodeAction->Header.Scope           = 1;
  OpCodeAction->Question.Header.Prompt = StringId;
  OpCodeAction->Question.Header.Help   = StringId;
  OpCodeAction->Question.QuestionId    = KEY_SCAN_F9;
  OpCodeAction->Question.Flags         = EFI_IFR_FLAG_CALLBACK;
  BufferPtr += sizeof(EFI_IFR_ACTION);

  OpCodeEnd                = (EFI_IFR_END *) BufferPtr;
  OpCodeEnd->Header.OpCode = EFI_IFR_END_OP;
  OpCodeEnd->Header.Length = sizeof(EFI_IFR_END);
  BufferPtr += sizeof(EFI_IFR_END);

  OpCodeEnd                = (EFI_IFR_END *) BufferPtr;
  OpCodeEnd->Header.OpCode = EFI_IFR_END_OP;
  OpCodeEnd->Header.Length = sizeof(EFI_IFR_END);

  *OpCodes     = Buffer;
  *OpCodesSize = BufferSize;

  return EFI_SUCCESS;
}


/**
  For each HII handle, append load default opcodes into the form package which support specific form set class GUID.

  @param[in] FormsetClassGuid      The specific form set class GUID
  @param[in] HiiHandleList         Pointer to HII handle list
  @param[in] HiiHandleListCount    The number of HII handle list

  @retval EFI_SUCCESS              Success to append
  @retval EFI_INVALID_PARAMETER    Input parameter is invalid
  @retval EFI_NOT_FOUND            Can not find HII package data from HII database
  @retval EFI_OUT_OF_RESOURCES     Allocate memory fail
  @retval Other                    Fail to get load default opcodes
**/
EFI_STATUS
AppendLoadDefaultOpCodes (
  IN EFI_GUID                   *FormSetClassGuid,
  IN EFI_HII_HANDLE             *HiiHandleList,
  IN UINTN                      HiiHandleListCount
  )
{
  EFI_STATUS                   Status;
  UINT8                        *AddedOpCodes;
  UINTN                        AddedOpCodesSize;
  UINTN                        Index;
  UINTN                        HiiPackageListSize;
  EFI_HII_PACKAGE_LIST_HEADER  *HiiPackageList;
  EFI_HII_PACKAGE_LIST_HEADER  *NewHiiPackageList;
  UINT8                        *NewHiiPackageListPtr;
  UINT32                       Offset;
  UINT32                       PackageListLength;
  BOOLEAN                      Appended;
  UINT8                        *Package;
  EFI_HII_PACKAGE_HEADER       *PackageHeader;
  EFI_HII_PACKAGE_HEADER       *TempPackageHeader;
  UINT8                        Flag;


  if (FormSetClassGuid == NULL || HiiHandleList == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = GetLoadDefaultOpCodes (&AddedOpCodes, &AddedOpCodesSize);
  if (Status != EFI_SUCCESS) {
    return Status;
  }

  Flag = FormsetGuidFlagFormsetClassGuid;

  for (Index = 0; Index < HiiHandleListCount; Index++) {
    HiiPackageList     = NULL;
    HiiPackageListSize = 0;
    Status = mHiiDatabase->ExportPackageLists (mHiiDatabase, HiiHandleList[Index], &HiiPackageListSize, HiiPackageList);
    if (Status != EFI_BUFFER_TOO_SMALL) {
      return EFI_NOT_FOUND;
    }
    HiiPackageList = AllocateZeroPool (HiiPackageListSize);
    if (HiiPackageList == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
    Status = mHiiDatabase->ExportPackageLists (mHiiDatabase, HiiHandleList[Index], &HiiPackageListSize, HiiPackageList);
    if (EFI_ERROR (Status)) {
      FreePool (HiiPackageList);
      return Status;
    }

    NewHiiPackageList = AllocateZeroPool (HiiPackageListSize + AddedOpCodesSize);
    if (NewHiiPackageList == NULL) {
      FreePool (HiiPackageList);
      return EFI_OUT_OF_RESOURCES;
    }
    NewHiiPackageListPtr = (UINT8 *) NewHiiPackageList;

    Appended          = FALSE;
    Offset            = sizeof (EFI_HII_PACKAGE_LIST_HEADER);
    PackageListLength = ReadUnaligned32 (&HiiPackageList->PackageLength);

    CopyMem (NewHiiPackageListPtr, HiiPackageList, Offset);
    NewHiiPackageListPtr += Offset;

    while (Offset < PackageListLength) {
      Package       = (UINT8 *) HiiPackageList + Offset;
      PackageHeader = (EFI_HII_PACKAGE_HEADER *) Package;

      if (PackageHeader->Type == EFI_HII_PACKAGE_FORMS &&
          !Appended &&
          ScuAppSupportFormsetGuidByPackage (FormSetClassGuid, &Flag, 1, Package)) {
        //
        // Append load default opcodes before endformset opcode
        //
        TempPackageHeader = (EFI_HII_PACKAGE_HEADER *) NewHiiPackageListPtr;

        CopyMem (NewHiiPackageListPtr, Package, PackageHeader->Length - sizeof(EFI_IFR_END));
        NewHiiPackageListPtr += PackageHeader->Length - sizeof(EFI_IFR_END);

        CopyMem (NewHiiPackageListPtr, AddedOpCodes, AddedOpCodesSize);
        NewHiiPackageListPtr += AddedOpCodesSize;

        CopyMem (NewHiiPackageListPtr, Package + PackageHeader->Length - sizeof(EFI_IFR_END), sizeof(EFI_IFR_END));
        NewHiiPackageListPtr += sizeof(EFI_IFR_END);

        NewHiiPackageList->PackageLength += (UINT32) AddedOpCodesSize;
        TempPackageHeader->Length        += (UINT32) AddedOpCodesSize;
        Appended = TRUE;
      } else {
        CopyMem (NewHiiPackageListPtr, Package, PackageHeader->Length);
        NewHiiPackageListPtr += PackageHeader->Length;
      }

      Offset += PackageHeader->Length;
    }

    if (Appended) {
      mHiiDatabase->UpdatePackageList (mHiiDatabase, HiiHandleList[Index], NewHiiPackageList);
    }

    FreePool (HiiPackageList);
    FreePool (NewHiiPackageList);
  }

  FreePool (AddedOpCodes);

  return EFI_SUCCESS;
}

/**
 Call SCU by compitable method.

 @retval EFI_SUCCESS              Success to call Setup Utility
 @retval EFI_UNSUPPORTED          StartEntry function pointer is NULL
 @retval Other                    Fail to locate setup utility protocol
**/
EFI_STATUS
CallCompatibleScu (
  VOID
  )
{
  EFI_STATUS                   Status;
  EFI_SETUP_UTILITY_PROTOCOL   *SetupUtility;

  Status = gBS->LocateProtocol (&gEfiSetupUtilityProtocolGuid, NULL, (VOID **) &SetupUtility);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if (SetupUtility->StartEntry == NULL) {
    return EFI_UNSUPPORTED;
  }

  return SetupUtility->StartEntry (SetupUtility);
}

/**
 Display Setup Utility.

 @param[in] HiiHandle             Pointer to HII handle list
 @param[in] HiiHandleCount        The number of HII handle list

 @retval EFI_SUCCESS              Success to display Setup Utility
 @retval Other                    Fail to locate form browser protocol or send form
**/
EFI_STATUS
DisplaySetupUtility (
  IN EFI_HII_HANDLE                *HiiHandle,
  IN UINTN                         HiiHandleCount
  )
{
  EFI_STATUS                    Status;
  EFI_FORM_BROWSER2_PROTOCOL    *FormBrowser2;
  EFI_GUID                      FormSetClassGuid = SETUP_UTILITY_FORMSET_CLASS_GUID;
  EFI_BROWSER_ACTION_REQUEST    ActionRequest;
  EFI_GUID                      ZeroGuid;

  Status = gBS->LocateProtocol (&gEfiFormBrowser2ProtocolGuid, NULL, (VOID **) &FormBrowser2);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  AppendLoadDefaultOpCodes (&FormSetClassGuid, HiiHandle, HiiHandleCount);

  ZeroMem (&ZeroGuid, sizeof (EFI_GUID));
  ActionRequest = EFI_BROWSER_ACTION_REQUEST_NONE;
  Status = FormBrowser2->SendForm (
                           FormBrowser2,
                           HiiHandle,
                           HiiHandleCount,
                           &ZeroGuid,
                           1,
                           NULL,
                           &ActionRequest
                           );
  if (ActionRequest == EFI_BROWSER_ACTION_REQUEST_RESET) {
    gBS->RaiseTPL (TPL_NOTIFY);
    gRT->ResetSystem(EfiResetCold, EFI_SUCCESS, 0, NULL);
  }

  gST->ConOut->SetAttribute (gST->ConOut, EFI_BLACK | EFI_BACKGROUND_BLACK);
  gST->ConOut->ClearScreen (gST->ConOut);

  return Status;
}

/**
 Display Setup Utility to allow user to configure the platform.

 It will install gEfiSetupUtilityApplicationProtocolGuid protocol to trigger Setup related VFR drivers
 to install HII data. Then it will collect all HII handles and display in Setup Utility.
 When exit Setup Utility, it will re-install gEfiSetupUtilityApplicationProtocolGuid protocol to trigger
 Setup related VFR drivers to uninstall HII data.

 @param[in] ImageHandle    The firmware allocated handle for the EFI image.
 @param[in] SystemTable    A pointer to the EFI System Table.

 @retval EFI_SUCCESS            Display Setup Utility successfully.
 @retval EFI_ACCESS_DENIED      Another Setup Utility is running on system now.
 @retval EFI_OUT_OF_RESOURCES   Fail to allocate pool.
 @retval other                  Fail to locate Hii database protocol or install protocol instance.
**/
EFI_STATUS
EFIAPI
SetupUtilityAppMain (
  IN EFI_HANDLE                  ImageHandle,
  IN EFI_SYSTEM_TABLE            *SystemTable
  )
{
  EFI_STATUS                               Status;
  EFI_HANDLE                               SetupUtilityHandle;
  EFI_SETUP_UTILITY_APPLICATION_PROTOCOL   *SetupUtilityApp;
  VOID                                     *H2OFormBrowser;
  EFI_HII_HANDLE                           *HiiHandle;
  UINTN                                    HiiHandleCount;

  //
  // Check if there is only one setup utility application protocol in system
  //
  Status = gBS->LocateProtocol (&gEfiSetupUtilityApplicationProtocolGuid, NULL, (VOID **) &SetupUtilityApp);
  if (!EFI_ERROR (Status)) {
    return EFI_ACCESS_DENIED;
  }

  Status = gBS->LocateProtocol (&gEfiHiiDatabaseProtocolGuid, NULL, (VOID **) &mHiiDatabase);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  SetupUtilityApp = AllocatePool (sizeof(EFI_SETUP_UTILITY_APPLICATION_PROTOCOL));
  if (SetupUtilityApp == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // Clear the Watchdog Timer to prevent from system reset in case launching from BDS load option function.
  //
  gBS->SetWatchdogTimer (0, 0, 0, NULL);

  //
  // Install setup utility application protocol to trigger VFR driver to install HII package
  //
  SetupUtilityHandle = NULL;
  SetupUtilityApp->VfrDriverState = InitializeSetupUtility;
  Status = gBS->InstallProtocolInterface (
                  &SetupUtilityHandle,
                  &gEfiSetupUtilityApplicationProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  SetupUtilityApp
                  );
  if (!EFI_ERROR (Status)) {
    HiiHandle = NULL;
    Status    = ScuAppGetScuHiiHandles (&HiiHandle, &HiiHandleCount);
    if (EFI_ERROR (Status) || HiiHandle == NULL) {
      //
      // It is old setup utility driver in system. Go to SCU by setup utility protocol.
      //
      CallCompatibleScu ();
    } else {
      //
      // SCU application calls SendForm function to display SCU only when H2O form browser exist, because setup utility
      // driver will call SendForm function by himself when old form browser exist.
      //
      Status = gBS->LocateProtocol (&mH2OFormBrowserProtocolGuid, NULL, &H2OFormBrowser);
      if (!EFI_ERROR (Status)) {
        DisplaySetupUtility (HiiHandle, HiiHandleCount);
      }
      FreePool (HiiHandle);
    }

    //
    // Re-install setup utility application protocol to trigger VFR driver to uninstall HII package
    //
    SetupUtilityApp->VfrDriverState = ShutdownSetupUtility;
    gBS->ReinstallProtocolInterface (
           SetupUtilityHandle,
           &gEfiSetupUtilityApplicationProtocolGuid,
           SetupUtilityApp,
           SetupUtilityApp
           );

    gBS->UninstallProtocolInterface (
           SetupUtilityHandle,
           &gEfiSetupUtilityApplicationProtocolGuid,
           SetupUtilityApp
           );
  }

  FreePool (SetupUtilityApp);

  return Status;
}

