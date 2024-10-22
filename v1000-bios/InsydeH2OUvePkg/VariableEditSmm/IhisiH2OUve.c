/** @file
  This file offers some oem service for H2OUVE utility.
;******************************************************************************
;* Copyright (c) 2016, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/
#include <Uefi.h>
#include <Base.h>
#include <KernelSetupConfig.h>
#include <IndustryStandard/Pci22.h>

#include <Library/PcdLib.h>
#include <Library/SetupUtilityLib.h>
#include <Library/VariableLib.h>
#include <Library/DevicePathLib.h>
#include <Library/DebugLib.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/BaseLib.h>
#include <Library/SmmOemSvcKernelLib.h>

#include <Protocol/SmmVariable.h>

#include <H2OIhisi.h>
#include "IhisiH2OUve.h"

extern H2O_IHISI_PROTOCOL               *mH2OIhisi;
VARIABLE_EDIT_BOOT_TYPE_INFO_PROTOCOL   mData = { VARIABLE_EDIT_BOOT_TYPE_INFO_PROTOCOL_CONTENT };
UINTN                                   mTryCount = 0;

//
//This structure is defined at SetupUtilityLibCommon.h
//
typedef struct {
  UINTN    LangNum;
  UINT8    LangString[1];
  } LANGUAGE_DATA_BASE;

STATIC LANGUAGE_DATA_BASE  *mSupportedLangTable;

VOID
SafeFreePool (
  IN      VOID      **Pointer
)
{
  //
  // check if pointer to pointer is null
  //
  if ( (Pointer != NULL) && (*Pointer != NULL)) {
    FreePool (*Pointer); // actually deallocate memory
    *Pointer = NULL; // null terminate
  }
}
#define SafeFreePoolM(p)        SafeFreePool ( (VOID **) & (p))

/**
  Check if the process is allowed to execute in current security level.

  @retval TRUE           Allowed to execute in current security level.
  @retval FALSE          NOT allowed to execute in current security level.
**/
STATIC
BOOLEAN
CheckSecurityLevelAllowed (
  VOID
  )
{
  EFI_STATUS     Status;
  UINT32         SecurityLevelsSupported;
  UINT32         CurrentSecurityLevel;

  Status = mH2OIhisi->GetAuthStatus (&SecurityLevelsSupported, &CurrentSecurityLevel);
  if (Status != EFI_SUCCESS) {
    return FALSE;
  }

  return ((PcdGet32 (PcdH2oUveSecurityLevelAllowed) & CurrentSecurityLevel) != 0) ? TRUE : FALSE;
}

STATIC 
EFI_STATUS 
CheckBootTypeNumber (
  UINT8                                     *ValidBootTypeAry,
  UINT16                                    *BootTypeNumberAry,
  UINTN                                     BootTypeCountMax
  ) 
{
  UINTN                                     BootTypeCount;
  UINTN                                     BootTypeNumAryCount;
  UINTN                                     ValidBoootTypeIdx;

  //
  //calculate the supported device type count
  //
  for (BootTypeCount = 0; BootTypeCount < BootTypeCountMax; BootTypeCount++) {
    if (ValidBootTypeAry[BootTypeCount] == 0) {
      break;
    }
  }

  //
  // check the count of BootTypeNumberAry
  //
  for (BootTypeNumAryCount = 0; BootTypeNumAryCount < BootTypeCountMax; BootTypeNumAryCount++) {
    if (BootTypeNumberAry[BootTypeNumAryCount] == 0) {
      break;
    }
  }
  if (BootTypeCount != BootTypeNumAryCount) {
    return EFI_NO_MAPPING;
  }

  //
  // Check the remaining data in BootTypeNumberAry
  //
  for (;BootTypeNumAryCount < BootTypeCountMax; BootTypeNumAryCount++) {
    if (BootTypeNumberAry[BootTypeNumAryCount] != 0) {
      return EFI_INVALID_PARAMETER;
    }
  }

  //
  // Check the BootTypeNumberAry
  //
  for (ValidBoootTypeIdx = 0; ValidBoootTypeIdx < BootTypeCount; ValidBoootTypeIdx++) {
    for (BootTypeNumAryCount = 0; BootTypeNumAryCount < BootTypeCount; BootTypeNumAryCount++) {
      if ((UINT8)BootTypeNumberAry[BootTypeNumAryCount] == ValidBootTypeAry[ValidBoootTypeIdx]) {
        break;
      }
    }

    if (BootTypeNumAryCount >= BootTypeCount) {
      return EFI_NO_MAPPING;
    }
  }

  return EFI_SUCCESS;
}

STATIC 
BOOLEAN 
CheckBufData (
  VOID                                      *CheckBuf,
  UINTN                                     CheckSize,
  UINT8                                     CheckValue
  ) 
{
  UINTN                                     Index;

  for (Index = 0; Index < CheckSize; Index++) {
    if (*(UINT8 *)((UINTN)CheckBuf + Index) != CheckValue) {
      return FALSE;
    }
  }

  return TRUE;
}

STATIC 
EFI_STATUS 
CheckBootOptionInfo (
  VATS_BOOT_OPTION_INFORMATION_STRUCTURE    *BootOptionInfo,
  UINT16                                    *BootOrder,
  UINTN                                     Count
  )
{
  EFI_STATUS                                Status = EFI_SUCCESS;
  KERNEL_CONFIGURATION                      *SetupVariable = NULL;
  UINTN                                     SetupBufferSize = 0;
  BOOLEAN                                   BootOrderByType; 
  BOOLEAN                                   EfiFirst; 
  UINTN                                     BootOptionIdx;
  UINTN                                     BootOrderIdx;

  if ((BootOptionInfo == NULL) || (BootOrder == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  Status = CommonGetVariableDataAndSize (SETUP_VARIABLE_NAME,
                                           &gSystemConfigurationGuid,
                                           &SetupBufferSize,
                                           (VOID **) &SetupVariable
                                          );
  if (EFI_ERROR (Status)) {
    // goto memory recycling process.
    return Status;
  }
  if ( (SetupVariable == NULL) || (SetupBufferSize == 0)) {
    Status = EFI_NOT_READY;
    // goto memory recycling process.
    return EFI_NOT_READY;
  }

// SEG 5.1 need to overwrite it
  BootOrderByType        = (SetupVariable->BootType != EFI_BOOT_TYPE ? TRUE : FALSE);
// SEG 5.1 need to overwrite it
  BootOrderByType       &= (SetupVariable->LegacyNormalMenuType == NORMAL_MENU ? TRUE : FALSE);
  EfiFirst               = (SetupVariable->BootNormalPriority == EFI_FIRST ? TRUE : FALSE);

  if ((BootOptionInfo->BootOrderByType != BootOrderByType) ||
      (BootOptionInfo->EfiFirst != EfiFirst) ||
      (BootOptionInfo->Count != Count) ||
      (!CheckBufData (BootOptionInfo->Reserve, sizeof (BootOptionInfo->Reserve), 0x0))
    ) {
    SafeFreePoolM (SetupVariable);
    return IHISI_VATS_WRONG_OB_FORMAT;
  }

  for (BootOrderIdx = 0; BootOrderIdx < Count; BootOrderIdx++) {
    for (BootOptionIdx = 0; BootOptionIdx < Count; BootOptionIdx++) {
      if (BootOptionInfo->BootOption[BootOptionIdx].BootOptionNum == BootOrder[BootOrderIdx]) {
        break;
      }
    }

    if (BootOptionIdx == Count) {
      SafeFreePoolM (SetupVariable);
      return IHISI_VATS_WRONG_OB_FORMAT;
    }
  }

  SafeFreePoolM (SetupVariable);
  return EFI_SUCCESS;
}

/**
  Internal function to check Whole ASCII String(include the null terminate) is located in command buffer

  @retval TRUE        Whole String is located in coomand buffer.
  @return FALSE       Any byte of the String in not located in command buffer.
**/
STATIC
BOOLEAN
AsciiStringInCmdBuffer (
  IN  CHAR8      *String
  )
{
  UINTN           Index;

  for (Index = 0; mH2OIhisi->BufferInCmdBuffer ((VOID *) (String + Index), sizeof (CHAR8)); Index++) {
    if (String[Index] == 0) {
      return TRUE;
    }
  }

  return FALSE;
}

/**
  Internal function to check Whole Unicode String(include the null terminate) is located in command buffer

  @retval TRUE        Whole String is located in coomand buffer.
  @return FALSE       Any byte of the String in not located in command buffer.
**/
STATIC
BOOLEAN
UnicodeStringInCmdBuffer (
  IN  CHAR16      *String
  )
{
  UINTN           Index;

  for (Index = 0; mH2OIhisi->BufferInCmdBuffer ((VOID *) (String + Index), sizeof (CHAR16)); Index++) {
    if (String[Index] == 0) {
      return TRUE;
    }
  }

  return FALSE;
}

/**
  Check for buffer-overlap when two buffers are passed in.

  @param[in] Buffer1       The pointer to the buffer1 to be checked.
  @param[in] Buffer1Size   The size in bytes of the input buffer1
  @param[in] Buffer2       The pointer to the buffer2 to be checked.
  @param[in] Buffer2Size   The size in bytes of the input buffer2

  @retval  TURE        The buffers overlap.
  @retval  FALSE       The buffers doesn't overlap.

**/
BOOLEAN
EFIAPI
BuffersOverlap (
  IN VOID           *Buffer1,
  IN UINT32         Buffer1Size,
  IN VOID           *Buffer2,
  IN UINT32         Buffer2Size
  )
{
  return (Buffer1Size != 0 && Buffer2Size != 0 && ((UINTN)Buffer1 + Buffer1Size) > (UINTN)Buffer2 && (UINTN)Buffer1 < ((UINTN)Buffer2 + Buffer2Size));
}

/**
  Get next language from language code list (with separator ';').

  If LangCode is NULL, then ASSERT.
  If Lang is NULL, then ASSERT.

  @param  LangCode               On input: point to first language in the list.
                                 On output: point to next language in the list, or
                                 NULL if no more language in the list.
  @param  Lang                   The first language in the list.

**/
EFI_STATUS 
EFIAPI
SetupUtilityLibGetNextLanguageS (
  IN OUT CHAR8      **LangCode,
  OUT CHAR8         *Lang
  )
{
  UINTN  Index;
  CHAR8  *StringPtr;

  ASSERT (LangCode != NULL);
  ASSERT (*LangCode != NULL);
  ASSERT (Lang != NULL);

  Index = 0;
  StringPtr = *LangCode;
  while (StringPtr[Index] != 0 && StringPtr[Index] != ';') {
    if (++Index >= RFC_3066_ENTRY_SIZE) {
      ASSERT(Index < RFC_3066_ENTRY_SIZE);
      return EFI_INVALID_PARAMETER;
    }
  }

  CopyMem (Lang, StringPtr, Index);
  Lang[Index] = 0;

  if (StringPtr[Index] == ';') {
    Index++;
  }
  *LangCode = StringPtr + Index;
  return EFI_SUCCESS;
}

/**
  According the priority of langdef in UNI file to add the supported language code
  to supported language database.

  @retval EFI_SUCCESS    Initialize supported language database successful
  @retval Other          Get setup utility browser data fail

**/
STATIC
EFI_STATUS
InitializeSupportLanguage (
  VOID
  )
{
  CHAR8                         *Language;
  CHAR8                         Lang[RFC_3066_ENTRY_SIZE];
  CHAR8                         *SuportedLanguage;
  UINTN                         SupportedLangCnt;
  UINTN                         VarSize;

  SuportedLanguage = NULL;
  CommonGetVariableDataAndSize (L"PlatformLangCodes", &gEfiGlobalVariableGuid, &VarSize, (VOID**)(&SuportedLanguage));
  if (SuportedLanguage == NULL) {
    return EFI_NOT_FOUND;
  }

  SupportedLangCnt = 0;
  Language  = SuportedLanguage;
  while (*Language != 0) {
    if (EFI_SUCCESS != SetupUtilityLibGetNextLanguageS (&Language, Lang)) {
      break;
    }
    SupportedLangCnt++;
  }

  mSupportedLangTable = AllocateZeroPool (SupportedLangCnt * RFC_3066_ENTRY_SIZE + sizeof (UINTN));
  if (mSupportedLangTable == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  FreePool (SuportedLanguage);
  SuportedLanguage = NULL;
  CommonGetVariableDataAndSize (L"PlatformLangCodes", &gEfiGlobalVariableGuid, &VarSize, (VOID**)(&SuportedLanguage));
  if (SuportedLanguage == NULL) {
    SafeFreePoolM (mSupportedLangTable);
    return EFI_NOT_FOUND;
  }

  SupportedLangCnt = 0;
  Language  = SuportedLanguage;
  while (*Language != 0) {
    if (EFI_SUCCESS != SetupUtilityLibGetNextLanguageS (&Language, Lang)) {
      break;
    }
    AsciiStrCpyS ((CHAR8 *) &mSupportedLangTable->LangString[SupportedLangCnt * RFC_3066_ENTRY_SIZE], RFC_3066_ENTRY_SIZE, Lang);
    SupportedLangCnt++;
  }

  mSupportedLangTable->LangNum = SupportedLangCnt;
  SafeFreePoolM (SuportedLanguage);

  return EFI_SUCCESS;

}

/**
  Get supported language database. This funciton will return supported language number
  and language string

  @param  LangNumber             Pointer to supported language number
  @param  LanguageString         A double pointer to save the start of supported language string

  @retval EFI_SUCCESS            Initialize supported language database successful
  @retval EFI_INVALID_PARAMETER  Input parameter is invalid.
  @return Other                  Fail to get support language from database.

**/
EFI_STATUS
GetLangDatabase (
  OUT UINTN            *LangNumber,
  OUT UINT8            **LanguageString
  )
{
  UINTN       TotalSize;
  EFI_STATUS  Status;

  if (LangNumber == NULL || LanguageString == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  if (mSupportedLangTable == NULL) {
    Status = InitializeSupportLanguage ();
    if (EFI_ERROR(Status)) {
      return Status;
    }
    if (mSupportedLangTable == NULL) {
      return EFI_UNSUPPORTED;
    }
  }
  *LangNumber = mSupportedLangTable->LangNum;
  TotalSize = *LangNumber * RFC_3066_ENTRY_SIZE;
  *LanguageString = AllocateZeroPool (TotalSize);
  if (*LanguageString == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  CopyMem (*LanguageString, mSupportedLangTable->LangString, TotalSize);

  return EFI_SUCCESS;
}

/**
  Update the global variable(mData) by PCD and Setup variable.

**/
VOID
EFIAPI
UpdateVariableEditBootTypeInfo  (
  VOID
)
{
  EFI_STATUS                            Status;
  UINT8                                 *DefaultLegacyBootTypeOrder;
  UINTN                                 Index;
  VARIABLE_EDIT_BOOT_TYPE_INFO_PROTOCOL VariableEditBootTypeInfoData = { VARIABLE_EDIT_BOOT_TYPE_INFO_PROTOCOL_CONTENT };
  KERNEL_CONFIGURATION                  *SetupVariable;
  UINTN                                 SetupBufferSize;
  UINT32                                SetupAttributes;
  EFI_SMM_VARIABLE_PROTOCOL             *SmmVariable;

  Status = EFI_SUCCESS;
  DefaultLegacyBootTypeOrder = NULL;
  Index = 0;
  SetupVariable = NULL;
  SetupBufferSize = 0;
  SetupAttributes = 0;
  SmmVariable = NULL;

  Status = gSmst->SmmLocateProtocol (
                    &gEfiSmmVariableProtocolGuid,
                    NULL,
                    (VOID **)&SmmVariable
                    );

  if (!EFI_ERROR (Status)) {
    //
    // Try to get Setup variable.
    //
    SetupVariable      = NULL;
    SetupBufferSize  = 0;
    Status = SmmVariable->SmmGetVariable (
                            SETUP_VARIABLE_NAME,
                            &gSystemConfigurationGuid,
                            &SetupAttributes,
                            &SetupBufferSize,
                            NULL
                            );
    if (Status == EFI_BUFFER_TOO_SMALL) {
      SetupVariable = (KERNEL_CONFIGURATION *) AllocateZeroPool (SetupBufferSize);
      if (SetupVariable == NULL) {
        DEBUG ((DEBUG_INFO, "[%a:%d] Status:%r\n", __FUNCTION__, __LINE__, Status));
      } else {
        Status = SmmVariable->SmmGetVariable (
                                SETUP_VARIABLE_NAME,
                                &gSystemConfigurationGuid,
                                &SetupAttributes,
                                &SetupBufferSize,
                                SetupVariable
                                );
        if (EFI_ERROR (Status)) {
          
          SafeFreePoolM (SetupVariable);
          DEBUG ((DEBUG_INFO, "[%a:%d] Status:%r\n", __FUNCTION__, __LINE__, Status));
        } else if (SetupBufferSize < sizeof (KERNEL_CONFIGURATION)) {
          SafeFreePoolM (SetupVariable);
          DEBUG ((DEBUG_INFO, "Warning!! setup variable size(0x%x) is incorrect.\n", SetupBufferSize));
        }
      }
    } else {
      SetupVariable = NULL;
    }

    //
    // Update BootTypeOrderNewDefaultSequence by Setup variable.
    //
    if (SetupVariable != NULL) {
      for (Index = 0; Index < MAX_BOOT_ORDER_NUMBER; ++Index) {
        VariableEditBootTypeInfoData.BootTypeOrderNewDefaultSequence[Index] = SetupVariable->BootTypeOrder[Index];
      }
      SafeFreePoolM (SetupVariable);
    }
  }

  //
  // Update BootTypeOrderDefaultSequence by PcdLegacyBootTypeOrder.
  //
  DefaultLegacyBootTypeOrder = (UINT8 *) PcdGetPtr (PcdLegacyBootTypeOrder);
  if (DefaultLegacyBootTypeOrder != NULL) {
    for (Index = 0; DefaultLegacyBootTypeOrder[Index] != 0 && Index < MAX_BOOT_ORDER_NUMBER; ++Index) {
      VariableEditBootTypeInfoData.BootTypeOrderDefaultSequence[Index] = DefaultLegacyBootTypeOrder[Index];
    }
  }

  CopyMem (&mData , &VariableEditBootTypeInfoData, sizeof (VARIABLE_EDIT_BOOT_TYPE_INFO_PROTOCOL));

}

/**
  Check that whether it is a USB device.

  @param[in] CurrentBbsTable   A BBS table.

  @retval TRUE                 It is a USB device.
  @retval FALSE                It is not a USB device.

**/
STATIC
BOOLEAN
IsUsbDevice (
  IN BBS_TABLE              *CurrentBbsTable
  )
{
  if ((CurrentBbsTable->Class == PCI_CLASS_SERIAL) &&
      (CurrentBbsTable->SubClass == PCI_CLASS_SERIAL_USB)) {
    return TRUE;
  }
  return FALSE;

}

/**
  Get callback list Size.

  @param[in] pCallbackListBuffer     A pointer to the buffer of callback list.
  @param[OUT] Size                   The size of callbacklist.

  @retval EFI_SUCCESS             
  @retval EFI_INVALID_PARAMETER      pCallbackListBuffer or Size is null.
  @retval EFI_BAD_BUFFER_SIZE        Buffer lies outside the command buffer.

**/
STATIC 
EFI_STATUS 
GetCallBackListSize (
  IN  VATS_CALLBACK_LIST_STRUCTURE  *pCallbackListBuffer,
  OUT UINT32                        *Size
  )
{

	CALLBACK_LIST_CALL_BACK_DATA *CallBackData;
	UINT32 Offset;
	UINT32 Index;
  UINT32 StrLength;

	if ((pCallbackListBuffer == NULL) || (Size == NULL)) {
		return EFI_INVALID_PARAMETER;
	}

  if (!mH2OIhisi->BufferInCmdBuffer ((VOID *) pCallbackListBuffer, sizeof (pCallbackListBuffer->ListCount))) {
    DEBUG ((DEBUG_ERROR, "[%a:%d]\n", __FUNCTION__, __LINE__));
    return EFI_BAD_BUFFER_SIZE;
  }

  CallBackData = NULL;
  Offset = 0;
  Index = 0;
  StrLength = 0;

	for (Index = 0; Index < pCallbackListBuffer->ListCount; ++Index) {
		CallBackData = (CALLBACK_LIST_CALL_BACK_DATA *) (Offset + (UINTN)pCallbackListBuffer->CallbackData);
    if (!mH2OIhisi->BufferInCmdBuffer ((VOID *) CallBackData, sizeof(CALLBACK_LIST_CALL_BACK_DATA)) || !AsciiStringInCmdBuffer (CallBackData->CallbackString)) {
      DEBUG ((DEBUG_ERROR, "[%a:%d]\n", __FUNCTION__, __LINE__));
      return EFI_BAD_BUFFER_SIZE;
    }
    StrLength = (UINT32)AsciiStrnLenS ((CONST UINT8 *) CallBackData->CallbackString, STRING_COUNT_MAX);
    if (StrLength == STRING_COUNT_MAX) {
      DEBUG ((DEBUG_ERROR, "Warning!! The Length of StrData is incorrect in CallBackData.(Callback ID:0x%x , String Length: 0x%x)\n", CallBackData->CallbackID, StrLength));
    }
		Offset += (UINT32)sizeof(CALLBACK_LIST_CALL_BACK_DATA) + StrLength;
	}
	*Size = Offset + sizeof(UINT32);

	return EFI_SUCCESS;
}

/**
  Get device type.

  @param[in] BootTypeOrder   A pointer to boot type order array.
  @param[in] BootTypeCount   arrat count of boot type order.
  @param[in] DevType         Device type, for check with boot type order.

  @retval DevType
  @retval OTHER_DRIVER       It can not found at boot type order array.

**/
STATIC 
UINT8
EFIAPI
UveSmmGetBbsTypeTableFromBootTypeOrder (
  IN UINT8                                  *BootTypeOrder,
  IN UINTN                                  BootTypeCount,
  IN UINT16                                 DevType
)
{
  UINTN                                     Index;
  UINT8                                     FindDevType;

  Index = 0;
  FindDevType = OTHER_DRIVER;

  if (BootTypeOrder == NULL) {
    return FindDevType;
  }

  for (Index = 0; Index < BootTypeCount; ++Index) {
    if (BootTypeOrder[Index] == DevType) {
      FindDevType = BootTypeOrder[Index];
      break;
    }
  }

  return FindDevType;
}

/**
  Get the boot type order infomation.

  @param[in] Rax   Value of CPU register.
  @param[in] Rcx   Value of CPU register.
  @param[in] Rsi   Value of CPU register.
  @param[in] Rdi   Value of CPU register.

  @retval EFI_SUCCESS
  @retval EFI_INVALID_PARAMETER          Rdi is 0, Rcx not equal to 01h.
  @retval EFI_NOT_READY                  BootOrder/Setup has not been set.
  @return Other                          Error occurred in this function.

**/
STATIC 
EFI_STATUS
EFIAPI
GetBootTypeOrderInformation (
  IN CONST UINT64                       Rax,
  IN CONST UINT64                       Rcx,
  IN CONST UINT64                       Rsi,
  IN CONST UINT64                       Rdi
)
{
  VATS_BOOT_OPTION_INFORMATION_STRUCTURE *Data;
  KERNEL_CONFIGURATION                  *SetupVariable;
  UINTN                                 SetupBufferSize;
  UINT16                                *BootOrder;
  UINTN                                 BootOrderSize;
  UINTN                                 Count;
  UINTN                                 Index;
  UINT16                                BootString[10];
  UINT8                                 *BootOption;
  UINTN                                 BootOptionSize;
  UINT8                                 *WorkingPtr;
  EFI_DEVICE_PATH_PROTOCOL              *BootOptionDevicePath;
  UINTN                                 BootOptionDevicePathSize;
  BBS_BBS_DEVICE_PATH                   *BbsDp;
  UINT8                                 BootTypeOrder[MAX_BOOT_ORDER_NUMBER];
  EFI_STATUS                            Status;
  UINT16                                DevPathLen;

  Data    = (VATS_BOOT_OPTION_INFORMATION_STRUCTURE *) (UINTN) Rdi;
  if ((Rcx != _ECX__01H_) || (Data == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Check the BufferSize is actually in the Command Buffer.
  //
  if (!mH2OIhisi->BufferInCmdBuffer ((VOID *) &(Data->BufferSize), sizeof (UINT32))) {
    DEBUG ((DEBUG_ERROR, "[%a:%d]\n", __FUNCTION__, __LINE__));
    return IHISI_BUFFER_RANGE_ERROR;
  }
  if (Data->BufferSize < sizeof (VATS_BOOT_OPTION_INFORMATION_STRUCTURE)) {
    DEBUG ((DEBUG_ERROR, "[%a:%d]\n", __FUNCTION__, __LINE__));
    return IHISI_BUFFER_RANGE_ERROR;
  } 
  if (!mH2OIhisi->BufferInCmdBuffer ((VOID *) Data, Data->BufferSize)) {
    DEBUG ((DEBUG_ERROR, "[%a:%d]\n", __FUNCTION__, __LINE__));
    return IHISI_BUFFER_RANGE_ERROR;
  }

  //
  // Check the Count is actually in the Command Buffer.
  //
  if (!mH2OIhisi->BufferInCmdBuffer ((VOID *) &(Data->Count), sizeof (UINT16))) {
    DEBUG ((DEBUG_ERROR, "[%a:%d]\n", __FUNCTION__, __LINE__));
    return IHISI_BUFFER_RANGE_ERROR;
  }

  //
  // Check Data valid or not
  //
  if (!CheckBufData ((VOID *)&Data->BootOrderByType, Data->BufferSize - sizeof (UINT32), 0x0)) {
    return IHISI_VATS_WRONG_OB_FORMAT;
  }

  SetupVariable = NULL;
  SetupBufferSize = 0;
  BootOrder = NULL;
  BootOrderSize = 0;
  Count = 0;
  Index = 0;
  BootOption = NULL;
  BootOptionSize = 0;
  WorkingPtr = NULL;
  BootOptionDevicePath = NULL;
  BootOptionDevicePathSize = 0;
  BbsDp = NULL;
  Status = EFI_SUCCESS;
  DevPathLen = 0;

  ZeroMem (BootString, sizeof (UINT16) * 10);
  ZeroMem (BootTypeOrder, sizeof (UINT8) * MAX_BOOT_ORDER_NUMBER);

  do {
    Status = CommonGetVariableDataAndSize (
               EFI_BOOT_ORDER_VARIABLE_NAME,
               &gEfiGlobalVariableGuid,
               &BootOrderSize,
               (VOID **) &BootOrder
               );
    if (EFI_ERROR (Status)) {
      //
      // goto memory recycling process.
      //
      break;
    }
    if ( (BootOrder == NULL) || (BootOrderSize == 0)) {
      Status = EFI_NOT_READY;
      //
      // goto memory recycling process.
      //
      break;
    }

    Count = BootOrderSize / sizeof (BootOrder[0]);
    if (Data->BufferSize < (UINT16) (sizeof (VATS_BOOT_OPTION_INFORMATION_STRUCTURE) + (Count - 1) * sizeof (VATS_BOOT_OPTION_DATA))) {
      Data->BufferSize   = (UINT16) (sizeof (VATS_BOOT_OPTION_INFORMATION_STRUCTURE) + (Count - 1) * sizeof (VATS_BOOT_OPTION_DATA));
      Status = EFI_BUFFER_TOO_SMALL;
      //
      // goto memory recycling process.
      //
      break;
    }

    Status = CommonGetVariableDataAndSize (
               SETUP_VARIABLE_NAME,
               &gSystemConfigurationGuid,
               &SetupBufferSize,
               (VOID **) &SetupVariable
               );
    if (EFI_ERROR (Status)) {
      //
      // goto memory recycling process.
      //
      break;
    } else if (SetupBufferSize < sizeof (KERNEL_CONFIGURATION)) {
      SafeFreePoolM (SetupVariable);
      DEBUG ((DEBUG_INFO, "Warning!! setup variable size(0x%x) is incorrect.\n", SetupBufferSize));
      Status = EFI_ABORTED;
      break;
    }
    
    if ( (SetupVariable == NULL) || (SetupBufferSize == 0)) {
      Status = EFI_NOT_READY;
      //
      // goto memory recycling process.
      //
      break;
    }

// SEG 5.1 need to overwrite it
    Data->BootOrderByType        = (SetupVariable->BootType != EFI_BOOT_TYPE ? TRUE : FALSE);
// SEG 5.1 need to overwrite it
    Data->BootOrderByType       &= (SetupVariable->LegacyNormalMenuType == NORMAL_MENU ? TRUE : FALSE);
    Data->EfiFirst               = (SetupVariable->BootNormalPriority == EFI_FIRST ? TRUE : FALSE);
    Data->Count                  = (UINT16) Count;
    for (Index = 0; Index < MAX_BOOT_ORDER_NUMBER; ++Index) {
      BootTypeOrder[Index] = (UINT8) SetupVariable->BootTypeOrder[Index];
    }
    SafeFreePoolM (SetupVariable);

    for (Index = 0; Index < Count; ++Index) {
      UnicodeSPrint (BootString, sizeof (BootString), L"Boot%04x", BootOrder[Index]);
      Status = CommonGetVariableDataAndSize (
                 BootString,
                 &gEfiGlobalVariableGuid,
                 &BootOptionSize,
                 (VOID **) &BootOption
                 );
      if (EFI_ERROR (Status)) {
        break;
      }
      if ( (BootOption == NULL) || (BootOptionSize == 0)) {
        break;
      }

      //
      // Find device path in Bootxxxx variable
      //
      WorkingPtr               = BootOption;
      WorkingPtr              += sizeof (UINT32);
      DevPathLen               = *(UINT16 *)WorkingPtr;
      WorkingPtr              += sizeof (UINT16);
      WorkingPtr              += (UINTN) StrSize ( (UINT16 *) WorkingPtr);
      BootOptionDevicePath     = (EFI_DEVICE_PATH_PROTOCOL *) WorkingPtr;
      BootOptionDevicePathSize = GetDevicePathSize (BootOptionDevicePath);

      if (!mH2OIhisi->BufferInCmdBuffer ((VOID *) &(Data->BootOption[Index]), sizeof (VATS_BOOT_OPTION_DATA))) {
        DEBUG ((DEBUG_ERROR, "[%a:%d]\n", __FUNCTION__, __LINE__));
        SafeFreePoolM (BootOption);
        return IHISI_BUFFER_RANGE_ERROR;
      }

      if ( (BBS_DEVICE_PATH == BootOptionDevicePath->Type) && (BBS_BBS_DP == BootOptionDevicePath->SubType)) {
        BbsDp = (VOID *) BootOptionDevicePath;
        WorkingPtr += DevPathLen;

        Data->BootOption[Index].BootOptionNum = BootOrder[Index];

        if (IsUsbDevice ((BBS_TABLE *) WorkingPtr)) {
          Data->BootOption[Index].BootOptionType = BBS_USB;
        } else {
          Data->BootOption[Index].BootOptionType = UveSmmGetBbsTypeTableFromBootTypeOrder (
                                                     & (BootTypeOrder[0]),
                                                     MAX_BOOT_ORDER_NUMBER,
                                                     BbsDp->DeviceType
                                                     );
        }
      } else {
        Data->BootOption[Index].BootOptionNum  = BootOrder[Index];
        Data->BootOption[Index].BootOptionType = OTHER_DRIVER;
      }
      SafeFreePoolM (BootOption);
    }
  } while (0);

  //
  // memory recycling process.
  //
  SafeFreePoolM (SetupVariable);
  SafeFreePoolM (BootOrder);
  return Status;
}

/**
  Set the boot type order infomation.

  @param[in] Rax   Value of CPU register.
  @param[in] Rcx   Value of CPU register.
  @param[in] Rsi   Value of CPU register.
  @param[in] Rdi   Value of CPU register.

  @retval EFI_SUCCESS
  @retval EFI_INVALID_PARAMETER          Rdi is 0, Rcx not equal to 02h.
  @retval EFI_NOT_READY                  BootOrder has not been set.
  @return Other                          Error occurred in this function.

**/
STATIC 
EFI_STATUS
EFIAPI
SetBootTypeOrderInformation (
  IN CONST UINT64                       Rax,
  IN CONST UINT64                       Rcx,
  IN CONST UINT64                       Rsi,
  IN CONST UINT64                       Rdi
)
{
  VATS_BOOT_OPTION_INFORMATION_STRUCTURE *Data;
  UINTN                                 Index;
  UINT16                                *BootOrder;
  UINTN                                 BootOrderSize;
  EFI_STATUS                            Status;
  UINTN                                 Count;

  Data    = (VATS_BOOT_OPTION_INFORMATION_STRUCTURE *) (UINTN) Rdi;
  if ((Rcx != _ECX__02H_) || (Data == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Check the BufferSize is actually in the Command Buffer.
  //
  if (!mH2OIhisi->BufferInCmdBuffer ((VOID *) &(Data->BufferSize), sizeof (UINT32))) {
    DEBUG ((DEBUG_ERROR, "[%a:%d]\n", __FUNCTION__, __LINE__));
    return IHISI_BUFFER_RANGE_ERROR;
  }
  if (Data->BufferSize < sizeof (VATS_BOOT_OPTION_INFORMATION_STRUCTURE)) {
    DEBUG ((DEBUG_ERROR, "[%a:%d]\n", __FUNCTION__, __LINE__));
    return IHISI_BUFFER_RANGE_ERROR;
  }  
  if (!mH2OIhisi->BufferInCmdBuffer ((VOID *) Data, Data->BufferSize)) {
    DEBUG ((DEBUG_ERROR, "[%a:%d]\n", __FUNCTION__, __LINE__));
    return IHISI_BUFFER_RANGE_ERROR;
  }

  //
  // Check the Count is actually in the Command Buffer.
  //
  if (!mH2OIhisi->BufferInCmdBuffer ((VOID *) &(Data->Count), sizeof (UINT16))) {
    DEBUG ((DEBUG_ERROR, "[%a:%d]\n", __FUNCTION__, __LINE__));
    return IHISI_BUFFER_RANGE_ERROR;
  }
  //
  // Check the BootOption array is actually in the Command Buffer.
  //
  for (Index = 0; Index < Data->Count; Index++) {
    if (!mH2OIhisi->BufferInCmdBuffer ((VOID *) &(Data->BootOption[Index]), sizeof (VATS_BOOT_OPTION_DATA))) {
      DEBUG ((DEBUG_ERROR, "[%a:%d]\n", __FUNCTION__, __LINE__));
      return IHISI_BUFFER_RANGE_ERROR;
    }
  }

  Index = 0;
  BootOrder = NULL;
  BootOrderSize = 0;
  Status = EFI_SUCCESS;
  Count = 0;

  do {
    Status = CommonGetVariableDataAndSize (
               EFI_BOOT_ORDER_VARIABLE_NAME,
               &gEfiGlobalVariableGuid,
               &BootOrderSize,
               (VOID **) &BootOrder
               );
    if (EFI_ERROR (Status)) {
      //
      // goto memory recycling process.
      //
      break;
    }
    if ( (BootOrder == NULL) || (BootOrderSize == 0)) {
      Status = EFI_NOT_READY;
      //
      // goto memory recycling process.
      //
      break;
    }

    Count = BootOrderSize / sizeof (BootOrder[0]);
    if (Data->BufferSize < (UINT16) (sizeof (VATS_BOOT_OPTION_INFORMATION_STRUCTURE) + (Count - 1) * sizeof (VATS_BOOT_OPTION_DATA))) {
      Data->BufferSize   = (UINT16) (sizeof (VATS_BOOT_OPTION_INFORMATION_STRUCTURE) + (Count - 1) * sizeof (VATS_BOOT_OPTION_DATA));
      Status = EFI_BUFFER_TOO_SMALL;
      //
      // goto memory recycling process.
      //
      break;
    }

    //
    // Check Data valid or not
    //
    Status = CheckBootOptionInfo (Data, BootOrder, Count);
    if (Status != EFI_SUCCESS) {
      break;
    }

    for (Index = 0; Index < Count; ++Index) {
      BootOrder[Index] = Data->BootOption[Index].BootOptionNum;
    }
    Status = CommonSetVariable (
               EFI_BOOT_ORDER_VARIABLE_NAME,
               &gEfiGlobalVariableGuid,
               BS_RT_NV_VAR_ATTR,
               BootOrderSize,
               (VOID *) BootOrder
               );
    if (EFI_ERROR (Status)) {
      //
      // goto memory recycling process.
      //
      break;
    }
  } while (0);

  //
  // memory recycling process.
  //
  SafeFreePoolM (BootOrder);
  return Status;
}

/**
  Get current boot type infomation.

  @param[in] Rax   Value of CPU register.
  @param[in] Rcx   Value of CPU register.
  @param[in] Rsi   Value of CPU register.
  @param[in] Rdi   Value of CPU register.

  @retval EFI_SUCCESS
  @retval EFI_INVALID_PARAMETER          Rdi is 0, Rcx not equal to 03h.
  @retval EFI_NOT_READY                  Setup variable has not been set.
  @retval EFI_BUFFER_TOO_SMALL           Buffer size is smaller than VATS_BOOT_TYPE_INFORMATION_STRUCTURE.
  @return Other                          Error occurred in this function.

**/
STATIC 
EFI_STATUS
EFIAPI
GetCurrentBootTypeInformation (
  IN CONST UINT64                       Rax,
  IN CONST UINT64                       Rcx,
  IN CONST UINT64                       Rsi,
  IN CONST UINT64                       Rdi
)
{
  VATS_BOOT_TYPE_INFORMATION_STRUCTURE  *Data;
  UINTN                                 Index;
  KERNEL_CONFIGURATION                  *SetupVariable;
  UINTN                                 SetupBufferSize;
  EFI_STATUS                            Status;

  Data    = (VATS_BOOT_TYPE_INFORMATION_STRUCTURE *) (UINTN) Rdi;
  if ((Rcx != _ECX__03H_) || (Data == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Check the BufferSize is actually in the Command Buffer.
  //
  if (!mH2OIhisi->BufferInCmdBuffer ((VOID *) &(Data->BufferSize), sizeof (UINT32))) {
    DEBUG ((DEBUG_ERROR, "[%a:%d]\n", __FUNCTION__, __LINE__));
    return IHISI_BUFFER_RANGE_ERROR;
  }
  //
  // MAX_BOOT_ORDER_NUMBER is defined by BIOS, tool should not know this value.
  // So, tool would not get the actual size of VATS_BOOT_TYPE_INFORMATION_STRUCTURE.
  //
//	  if (Data->BufferSize < sizeof (VATS_BOOT_TYPE_INFORMATION_STRUCTURE)) {
//	    DEBUG ((DEBUG_ERROR, "[%a:%d]\n", __FUNCTION__, __LINE__));
//	    return IHISI_BUFFER_RANGE_ERROR;
//	  }
  if (!mH2OIhisi->BufferInCmdBuffer ((VOID *) Data, Data->BufferSize)) {
    DEBUG ((DEBUG_ERROR, "[%a:%d]\n", __FUNCTION__, __LINE__));
    return IHISI_BUFFER_RANGE_ERROR;
  }

  Index = 0;
  SetupVariable = NULL;
  SetupBufferSize = 0;
  Status = EFI_SUCCESS;

  if (Data->BufferSize < (UINT16) sizeof (VATS_BOOT_TYPE_INFORMATION_STRUCTURE)) {
    Data->BufferSize   = (UINT16) sizeof (VATS_BOOT_TYPE_INFORMATION_STRUCTURE);
    return EFI_BUFFER_TOO_SMALL;
  }

  //
  // Check Data valid or not
  //
  if (!CheckBufData ((VOID *)&Data->Count, Data->BufferSize - sizeof (UINT32), 0x0)) {
    return IHISI_VATS_WRONG_OB_FORMAT;
  }

  Data->Count   =  MAX_BOOT_ORDER_NUMBER;
  StrCpyS (&(Data->BootTypeString[0]), 32, &(mData.BootTypeString[0]));

  CopyMem (Data->BootTypeDefaultNumber, mData.BootTypeOrderNewDefaultSequence, MAX_BOOT_ORDER_NUMBER * sizeof (UINT16));

  do {
    Status = CommonGetVariableDataAndSize (
               SETUP_VARIABLE_NAME,
               &gSystemConfigurationGuid,
               &SetupBufferSize,
               (VOID **) &SetupVariable
               );
    if (EFI_ERROR (Status)) {
      //
      // goto memory recycling process.
      //
      break;
    } else if (SetupBufferSize < sizeof (KERNEL_CONFIGURATION)) {
      SafeFreePoolM (SetupVariable);
      DEBUG ((DEBUG_INFO, "Warning!! setup variable size(0x%x) is incorrect.\n", SetupBufferSize));
      Status = EFI_ABORTED;
      break;
    }
    
    if ( (SetupVariable == NULL) || (SetupBufferSize == 0)) {
      Status = EFI_NOT_READY;
      //
      // goto memory recycling process.
      //
      break;
    }
    for (Index = 0; Index < MAX_BOOT_ORDER_NUMBER; ++Index) {
      Data->BootTypeNumber[Index] = (UINT16) SetupVariable->BootTypeOrder[Index];
    }

  } while (0);

  //
  // memory recycling process.
  //
  SafeFreePoolM (SetupVariable);
  return Status;
}

/**
  Set current boot type infomation.

  @param[in] Rax   Value of CPU register.
  @param[in] Rcx   Value of CPU register.
  @param[in] Rsi   Value of CPU register.
  @param[in] Rdi   Value of CPU register.

  @retval EFI_SUCCESS
  @retval EFI_INVALID_PARAMETER          Rdi is 0, Rcx not equal to 04h.
  @retval EFI_NOT_READY                  Setup variable has not been set.
  @retval EFI_BUFFER_TOO_SMALL           Buffer size is smaller than VATS_BOOT_TYPE_INFORMATION_STRUCTURE.
  @return Other                          Error occurred in this function.

**/
STATIC 
EFI_STATUS
EFIAPI
SetCurrentBootTypeInformation (
  IN CONST UINT64                       Rax,
  IN CONST UINT64                       Rcx,
  IN CONST UINT64                       Rsi,
  IN CONST UINT64                       Rdi
)
{
  VATS_BOOT_TYPE_INFORMATION_STRUCTURE  *Data;
  UINTN                                 Index;
  KERNEL_CONFIGURATION                  *SetupVariable;
  UINTN                                 SetupBufferSize;
  EFI_STATUS                            Status;

  Data    = (VATS_BOOT_TYPE_INFORMATION_STRUCTURE *) (UINTN) Rdi;
  if ((Rcx != _ECX__04H_) || (Data == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Check the BufferSize is actually in the Command Buffer.
  //
  if (!mH2OIhisi->BufferInCmdBuffer ((VOID *) &(Data->BufferSize), sizeof (UINT32))) {
    DEBUG ((DEBUG_ERROR, "[%a:%d]\n", __FUNCTION__, __LINE__));
    return IHISI_BUFFER_RANGE_ERROR;
  }
  if (Data->BufferSize < sizeof (VATS_BOOT_TYPE_INFORMATION_STRUCTURE)) {
    DEBUG ((DEBUG_ERROR, "[%a:%d]\n", __FUNCTION__, __LINE__));
    return IHISI_BUFFER_RANGE_ERROR;
  }
  if (!mH2OIhisi->BufferInCmdBuffer ((VOID *) Data, Data->BufferSize)) {
    DEBUG ((DEBUG_ERROR, "[%a:%d]\n", __FUNCTION__, __LINE__));
    return IHISI_BUFFER_RANGE_ERROR;
  }

  Index = 0;
  SetupVariable = NULL;
  SetupBufferSize = 0;
  Status = EFI_SUCCESS;

  if (Data->BufferSize < (UINT16) sizeof (VATS_BOOT_TYPE_INFORMATION_STRUCTURE)) {
    Data->BufferSize   = (UINT16) sizeof (VATS_BOOT_TYPE_INFORMATION_STRUCTURE);
    return EFI_BUFFER_TOO_SMALL;
  }

  //
  // Check Data is valid or not
  //
  if ((Data->Count != MAX_BOOT_ORDER_NUMBER) || 
      (!CheckBufData ((VOID *)Data->Reserve, sizeof (Data->Reserve), 0x0)) || 
      (StrCmp (&(Data->BootTypeString[0]), &(mData.BootTypeString[0])) != 0)
      ) {
    return IHISI_VATS_WRONG_OB_FORMAT;
  }

  do {
    Status = CommonGetVariableDataAndSize (
               SETUP_VARIABLE_NAME,
               &gSystemConfigurationGuid,
               &SetupBufferSize,
               (VOID **) &SetupVariable
               );
    if (EFI_ERROR (Status)) {
      //
      // goto memory recycling process.
      //
      break;
    } else if (SetupBufferSize < sizeof (KERNEL_CONFIGURATION)) {
      SafeFreePoolM (SetupVariable);
      DEBUG ((DEBUG_INFO, "Warning!! setup variable size(0x%x) is incorrect.\n", SetupBufferSize));
      Status = EFI_ABORTED;
      break;
    }
    
    if ( (SetupVariable == NULL) || (SetupBufferSize == 0)) {
      Status = EFI_NOT_READY;
      //
      // goto memory recycling process.
      //
      break;
    }

    //
    // Check whether the external input(BootTypeNumber) is correct.
    //
    Status = CheckBootTypeNumber (SetupVariable->BootTypeOrder, Data->BootTypeNumber, MAX_BOOT_ORDER_NUMBER);
    if (EFI_ERROR (Status)) {
      break;
    }
    
    for (Index = 0; Index < MAX_BOOT_ORDER_NUMBER; ++Index) {
      SetupVariable->BootTypeOrder[Index] = (UINT8) Data->BootTypeNumber[Index];
    }
    Status = CommonSetVariable (
               SETUP_VARIABLE_NAME,
               &gSystemConfigurationGuid,
               BS_RT_NV_VAR_ATTR,
               SetupBufferSize,
               (VOID *) SetupVariable
               );
    if (EFI_ERROR (Status)) {
      //
      // goto memory recycling process.
      //
      break;
    }
  } while (0);

  //
  // memory recycling process.
  //
  SafeFreePoolM (SetupVariable);
  return Status;
}

/**
  Get current boot type name.

  @param[in] Rax   Value of CPU register.
  @param[in] Rcx   Value of CPU register.
  @param[in] Rsi   Value of CPU register.
  @param[in] Rdi   Value of CPU register.

  @retval EFI_SUCCESS
  @retval EFI_INVALID_PARAMETER          Rdi is 0, Rcx not equal to 05h, cannot find the match with BootTypeNumber.
  @retval EFI_BUFFER_TOO_SMALL           Buffer size is smaller than needed.
  @return Other                          Error occurred in this function.

**/
STATIC 
EFI_STATUS
EFIAPI
GetCurrentBootTypeName (
  IN CONST UINT64                       Rax,
  IN CONST UINT64                       Rcx,
  IN CONST UINT64                       Rsi,
  IN CONST UINT64                       Rdi
)
{
  VATS_BOOT_TYPE_NAME_STRUCTURE         *Data;
  UINTN                                 BufferSize;
  UINTN                                 Index;

  Data    = (VATS_BOOT_TYPE_NAME_STRUCTURE *) (UINTN) Rdi;
  if ((Rcx != _ECX__05H_) || (Data == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Check the BufferSize is actually in the Command Buffer.
  //
  if (!mH2OIhisi->BufferInCmdBuffer ((VOID *) &(Data->BufferSize), sizeof (UINT32))) {
    DEBUG ((DEBUG_ERROR, "[%a:%d]\n", __FUNCTION__, __LINE__));
    return IHISI_BUFFER_RANGE_ERROR;
  }
  if (Data->BufferSize < sizeof (VATS_BOOT_TYPE_NAME_STRUCTURE)) {
    DEBUG ((DEBUG_ERROR, "[%a:%d]\n", __FUNCTION__, __LINE__));
    return IHISI_BUFFER_RANGE_ERROR;
  }
  if (!mH2OIhisi->BufferInCmdBuffer ((VOID *) Data, Data->BufferSize)) {
    DEBUG ((DEBUG_ERROR, "[%a:%d]\n", __FUNCTION__, __LINE__));
    return IHISI_BUFFER_RANGE_ERROR;
  }

  //
  // Check Data valid or not
  //
  if (!CheckBufData ((VOID *)Data->BootTypeName, Data->BufferSize - sizeof (UINT32) - sizeof (UINT16), 0x0)) {
    return IHISI_VATS_WRONG_OB_FORMAT;
  }

  BufferSize = 0;
  Index = 0;

  for (Index = 0; Index < mData.BootTypeStringCount; ++Index) {
    if (Data->BootType == mData.NameMapping[Index].BootTypeID) {
      BufferSize  = sizeof (VATS_BOOT_TYPE_NAME_STRUCTURE);
      BufferSize += sizeof (CHAR16) * StrnLenS (& (mData.NameMapping[Index].BootTypeName[0]), 31);
      break;
    }
  }
  if (Index == mData.BootTypeStringCount) {
    return EFI_INVALID_PARAMETER;
  }
  if (Data->BufferSize < BufferSize) {
    Data->BufferSize   = (UINT32) BufferSize;
    return EFI_BUFFER_TOO_SMALL;
  }

  if (!mH2OIhisi->BufferInCmdBuffer ((VOID *) Data->BootTypeName, 31 * sizeof(CHAR16))) {
    DEBUG ((DEBUG_ERROR, "[%a:%d]\n", __FUNCTION__, __LINE__));
    return IHISI_BUFFER_RANGE_ERROR;
  }

  StrCpyS (Data->BootTypeName, 31, & (mData.NameMapping[Index].BootTypeName[0]));

  return EFI_SUCCESS;
}

/**
  Callback function for H2OUVE.

  @retval EFI_SUCCESS                    dispatch next IHISI 52h function.
  @retval IHISI_END_FUNCTION_CHAIN       Success.
  @retval EFI_INVALID_PARAMETER          Rsi/Rcx is 0, variable name or GUID is mismatched with variable.

**/
EFI_STATUS 
EFIAPI 
H2OSmiCallBack (
  VOID
  ) 
{
  UINT64                                Rsi;
  UINT64                                Rdi;
  EFI_STATUS                            Status;
  UINTN                                 Index;
  KERNEL_CONFIGURATION                  *SetupNVData;
  VATS_SMI_CALLBACK                     *PwdCallbackData;
  VATS_CALLBACK_COMMON_HDR              *Hdr;
  VATS_CALLBACK_STATUS                  *PwdCallbackStatus;
  VATS_CALLBACK_PASSWORD_CHECK          *PwdCheck;
  VATS_CALLBACK_PASSWORD                *CallbackPwd;
  UINTN                                 Size;
  CHAR8                                 *Password;
  CHAR16                                Buffer[STRING_COUNT_MAX];
  UINTN                                 AsciiStringLen;
  UINTN                                 UnicodeStringLen;
  EFI_SYS_PASSWORD_SERVICE_PROTOCOL     *SysPassword;
  BOOLEAN                               IsAdminPwExist;
  BOOLEAN                               IsUserPwExist;
  UINTN                                 AsciiStrMaxSize;

  if (FeaturePcdGet (PcdH2OIhisiAuthSupported)) {
    if (!CheckSecurityLevelAllowed ()) {
      return IHISI_ACCESS_PROHIBITED;
    }
  }

  Rsi = (UINT64)(UINTN) mH2OIhisi->ReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RSI);
  Rdi = (UINT64)(UINTN) mH2OIhisi->ReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RDI);

  if ((Rsi == 0) || (Rdi == 0)) {
    return EFI_INVALID_PARAMETER;
  }

  ZeroMem (Buffer, sizeof (CHAR16) * STRING_COUNT_MAX);

  Status = 0;
  Index = 0;
  SetupNVData  = NULL;
  PwdCallbackData = NULL;
  Hdr = 0;
  PwdCallbackStatus = NULL;
  PwdCheck = NULL;
  CallbackPwd = NULL;
  Size = 0;
  Password = NULL;
  AsciiStringLen = 0;
  UnicodeStringLen = 0;
  SysPassword = NULL;
  IsAdminPwExist = FALSE;
  IsUserPwExist = FALSE;

  PwdCallbackData = (VATS_SMI_CALLBACK *)(UINTN)Rdi;

  //
  // Check Zero Vector and Signature in PwdCallbackData.
  // If failed, try to dispath next function(SetupCallbackSyncCallbackIhisi()).
  //

  if (!mH2OIhisi->BufferInCmdBuffer ((VOID *)PwdCallbackData, sizeof (VATS_SMI_CALLBACK))) {
    DEBUG ((DEBUG_ERROR, "[%a:%d]\n", __FUNCTION__, __LINE__));
    //
    // Try to dispath next function(SetupCallbackSyncCallbackIhisi()).
    //
    return EFI_SUCCESS;
  }
  
  for (Index = 0; Index < 16; ++Index) {
    if (PwdCallbackData->ZeroVector[Index] != 0) {
      break;
    }
  }

  if ((Index != 16) || (PwdCallbackData->Signature != IHISI_VATS_SMI_CB_SIGNATURE)) {
    return EFI_SUCCESS;
  }

  //
  // Check the buffer that is pointed by Rsi is in command buffer or not
  //
  Hdr = (VATS_CALLBACK_COMMON_HDR *)(UINTN)Rsi;

  //
  // Check for buffer-overlap
  //
  if (BuffersOverlap ((VOID *) PwdCallbackData, sizeof(VATS_SMI_CALLBACK), (VOID *) Hdr, sizeof (VATS_CALLBACK_COMMON_HDR))) {
    DEBUG ((DEBUG_ERROR, "[%a:%d]\n", __FUNCTION__, __LINE__));
    return IHISI_BUFFER_RANGE_ERROR;
  }

  //
  // Check the Size of header is actually in the Command Buffer.
  //
  if (!mH2OIhisi->BufferInCmdBuffer ((VOID *) &(Hdr->Size), sizeof (UINT16))) {
    DEBUG ((DEBUG_ERROR, "[%a:%d]\n", __FUNCTION__, __LINE__));
    return IHISI_BUFFER_RANGE_ERROR;
  }
  if (!mH2OIhisi->BufferInCmdBuffer ((VOID *)Hdr, Hdr->Size)) {
    DEBUG ((DEBUG_ERROR, "[%a:%d]\n", __FUNCTION__, __LINE__));
    return IHISI_BUFFER_RANGE_ERROR;
  }

  switch (Hdr->Type) {
    
  case VATS_SMI_CALLBACK_STATUS:
    //
    // Check if the actual size used exceeds the size written in the common header.
    //
    if (Hdr->Size < sizeof (VATS_CALLBACK_STATUS)) {
      DEBUG ((DEBUG_ERROR, "The size(0x%x) in common header is too small for VATS_SMI_CALLBACK_STATUS\n", Hdr->Size));
      return EFI_INVALID_PARAMETER;
    }

    //
    //Check support status of password
    //
    PwdCallbackStatus = (VATS_CALLBACK_STATUS *) Hdr;
    PwdCallbackStatus->Flags = 0;
    PwdCallbackStatus->Flags |= FLAGS_VATS_SMICB_PSW;
    PwdCallbackStatus->Hdr.Status = STS_VATS_SMICB_SUCCESS;
    break;

  case VATS_SMI_CALLBACK_PASSWORD_CHECK:
    //
    // Check if the actual size used exceeds the size written in the common header.
    //
    if (Hdr->Size < sizeof (VATS_CALLBACK_PASSWORD_CHECK)) {
      DEBUG ((DEBUG_ERROR, "The size(0x%x) in common header is too small for VATS_SMI_CALLBACK_PASSWORD_CHECK\n", Hdr->Size));
      return EFI_INVALID_PARAMETER;
    }

    //
    //Check status of password
    //
    PwdCheck = (VATS_CALLBACK_PASSWORD_CHECK *) Hdr;
    Status = gSmst->SmmLocateProtocol (
                      &gEfiSysPasswordServiceProtocolGuid,
                      NULL,
                      (VOID **)&SysPassword
                      );
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "%a:%d Status:%r\n", __FUNCTION__, __LINE__, Status));
      return EFI_ABORTED;
    }

    Status = SysPassword->GetStatus (SysPassword, SystemSupervisor);
    if (!EFI_ERROR (Status)) {
      IsAdminPwExist = TRUE;
    }
    if (PcdGetBool (PcdSysPasswordSupportUserPswd)) {
      Status = SysPassword->GetStatus (SysPassword, SystemUser);
      if (!EFI_ERROR (Status)) {
        IsUserPwExist = TRUE;
      }
    }

    if (!IsAdminPwExist && !IsUserPwExist) {
      PwdCheck->Hdr.Status = STS_VATS_SMICB_SUCCESS;
    } else if (mTryCount >= 5) {
      PwdCheck->Hdr.Status = STS_VATS_SMICB_ACCESS_DENIED;
    } else {
      PwdCheck->Hdr.Status = STS_VATS_SMICB_PSW_CHECK;
    }
    
    break;

  case VATS_SMI_CALLBACK_PASSWORD:
    //
    // Check if the actual size used exceeds the size written in the common header.
    //
    if (Hdr->Size <= (sizeof (VATS_CALLBACK_PASSWORD) + sizeof(KERNEL_CONFIGURATION))) {
      DEBUG ((DEBUG_ERROR, "The size(0x%x) in common header is too small for VATS_SMI_CALLBACK_PASSWORD\n", Hdr->Size));
      return EFI_INVALID_PARAMETER;
    }

    Password = (CHAR8 *) ((UINTN)Hdr + sizeof (VATS_CALLBACK_PASSWORD));
    if (!AsciiStringInCmdBuffer (Password)) {
      DEBUG ((DEBUG_ERROR, "The data is outside the command buffer.\n"));
      return IHISI_BUFFER_RANGE_ERROR;
    }

    AsciiStrMaxSize = MIN (STRING_COUNT_MAX, (Hdr->Size - sizeof (VATS_CALLBACK_PASSWORD) - sizeof(KERNEL_CONFIGURATION)));
    AsciiStringLen = AsciiStrnLenS (Password, AsciiStrMaxSize);
    if (AsciiStringLen == AsciiStrMaxSize) {
      return EFI_INVALID_PARAMETER; 
    }
    
    //
    //Validate password
    //

    CallbackPwd = (VATS_CALLBACK_PASSWORD *) Hdr;
    Password = (CHAR8 *) ((UINTN)CallbackPwd + sizeof (VATS_CALLBACK_PASSWORD));

    if (mTryCount >= 5) {
      CallbackPwd->Hdr.Status = STS_VATS_SMICB_ACCESS_DENIED;
      ZeroMem (Password, AsciiStringLen);
      return IHISI_END_FUNCTION_CHAIN;
    }

    if (AsciiStringLen == STRING_COUNT_MAX) {
      DEBUG ((DEBUG_ERROR, "[%a:%d] Warning!! The length of string is STRING_COUNT_MAX, and it may loss data\n", __FUNCTION__, __LINE__));
      CallbackPwd->Hdr.Status = STS_VATS_SMICB_PSW_CHK_FAILED;
      mTryCount++;
      ZeroMem (Password, AsciiStringLen);
      return IHISI_END_FUNCTION_CHAIN;
    }

    SetupNVData = (KERNEL_CONFIGURATION *)((UINTN)Password + ((AsciiStringLen + 1) * sizeof(*Password)));
    
    AsciiStrnToUnicodeStrS (Password, AsciiStringLen, Buffer, STRING_COUNT_MAX, &UnicodeStringLen);

    Status = gSmst->SmmLocateProtocol (
                      &gEfiSysPasswordServiceProtocolGuid,
                      NULL,
                      (VOID **)&SysPassword
                      );
    if (!EFI_ERROR (Status)) {
      //
      // Check password when the system password service protocol is exist.
      //
      Status = SysPassword->CheckPassword (SysPassword, Buffer, UnicodeStringLen, SystemSupervisor);
      if (EFI_ERROR (Status)) {
        Status = SysPassword->CheckPassword (SysPassword, Buffer, UnicodeStringLen, SystemUser);
        if (EFI_ERROR (Status)) {
        } else {
          SetupNVData->SetUserPass = 1;
        }
      } else {
        SetupNVData->SetUserPass = 0;
      }
    } else {
      DEBUG ((DEBUG_ERROR, "[%a:%d] Warning!! Fail to get system password service, and the status is%r\n", __FUNCTION__, __LINE__, Status));
    }
    if (EFI_ERROR (Status)) {
      CallbackPwd->Hdr.Status = STS_VATS_SMICB_PSW_CHK_FAILED;
      mTryCount++;
    } else {
      CallbackPwd->Hdr.Status  = STS_VATS_SMICB_SUCCESS;
      mTryCount = 0;
    }
    
    ZeroMem (Password, AsciiStringLen);
    ZeroMem (Buffer, sizeof (CHAR16) * STRING_COUNT_MAX);
    break;

  default:
    DEBUG ((DEBUG_ERROR, "The header type(0x%x) is out of range.\n", Hdr->Type));
    return EFI_INVALID_PARAMETER;
    break;
  }

  return IHISI_END_FUNCTION_CHAIN;
}

/**
  Callback function for the modifying of Setup variable.

  @retval EFI_SUCCESS
  @retval EFI_INVALID_PARAMETER          Rsi/Rcx is 0, variable name or GUID is mismatched with variable.

**/
EFI_STATUS
EFIAPI
SetupCallbackSyncCallbackIhisi (
  VOID
)
{
  UINT64                                Rcx;
  UINT64                                Rsi;
  UINT64                                Rdi;
  UINTN                                 Index;
  KERNEL_CONFIGURATION                  *SetupNVData;
  VATS_CALLBACK_LIST_STRUCTURE          *CallbackList;
  CALLBACK_LIST_CALL_BACK_DATA          *CallbackData;
  VATS_RESULT_LIST_STRUCTURE            *ResultArray;
  UINTN                                 Offset;
  UINT16                                Result;
  EFI_STATUS                            Status;
  CHAR16                                Buffer[STRING_COUNT_MAX];
  UINTN                                 IndexPwd;
  VATS_VARIABLE_BUFFER_STRUCTURE        *VarBuffer;
  BOOLEAN                               IsLoadDefault;
  CHAR8                                 *LanguageString;
  UINTN                                 LangNum;
  UINT32                                Size;
  UINTN                                 StrLength;
  EFI_SYS_PASSWORD_SERVICE_PROTOCOL     *SysPassword;

  if (FeaturePcdGet (PcdH2OIhisiAuthSupported)) {
    if (!CheckSecurityLevelAllowed ()) {
      return IHISI_ACCESS_PROHIBITED;
    }
  }

  Rcx = (UINT64)(UINTN) mH2OIhisi->ReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RCX);
  Rsi = (UINT64)(UINTN) mH2OIhisi->ReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RSI);
  Rdi = (UINT64)(UINTN) mH2OIhisi->ReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RDI);
  
  ZeroMem (Buffer, sizeof (CHAR16) * STRING_COUNT_MAX);

  Index = 0;
  SetupNVData  = NULL;
  CallbackList = NULL;
  CallbackData = NULL;
  ResultArray  = NULL;
  Offset = 0;
  Result = CALLBACK_RET_SUCCESS;
  Status = 0;
  IndexPwd  = 0;
  VarBuffer = NULL;
  IsLoadDefault = FALSE;
  LanguageString = NULL;
  LangNum = 0;
  Size = 0;
  StrLength = 0;
  SysPassword = NULL;

  if ((Rsi == 0) || (Rdi == 0) || (Rcx == 0)) {
    return EFI_INVALID_PARAMETER;
  }

  VarBuffer    = (VATS_VARIABLE_BUFFER_STRUCTURE *) (UINTN) Rsi;
  CallbackList = (VATS_CALLBACK_LIST_STRUCTURE *) (UINTN) Rdi;
  ResultArray  = (VATS_RESULT_LIST_STRUCTURE *) (UINTN) Rcx;

  //
  // Check the DataSize is actually in the Command Buffer.
  //
  if (!mH2OIhisi->BufferInCmdBuffer ((VOID *) &(VarBuffer->DataSize), sizeof (UINT32))) {
    DEBUG ((DEBUG_ERROR, "[%a:%d]\n", __FUNCTION__, __LINE__));
    return IHISI_BUFFER_RANGE_ERROR;
  }

  //
  // Check the VariableNameLength is actually in the Command Buffer.
  //
  if (!mH2OIhisi->BufferInCmdBuffer ((VOID *) &(VarBuffer->VariableNameLength), sizeof (UINT32))) {
    DEBUG ((DEBUG_ERROR, "[%a:%d]\n", __FUNCTION__, __LINE__));
    return IHISI_BUFFER_RANGE_ERROR;
  }

  //
  // Check variable Buffer
  //
  if (!mH2OIhisi->BufferInCmdBuffer ((VOID *) VarBuffer, sizeof (VATS_VARIABLE_BUFFER_STRUCTURE) + VarBuffer->VariableNameLength * sizeof (CHAR16) + VarBuffer->DataSize)) {
    DEBUG ((DEBUG_ERROR, "[%a:%d]\n", __FUNCTION__, __LINE__));
    return IHISI_BUFFER_RANGE_ERROR;
  }

  //
  // Check Callback List
  //
  Status = GetCallBackListSize (CallbackList, &Size);
  if (EFI_ERROR(Status)) {
    if (Status == EFI_BAD_BUFFER_SIZE) {
      Status = IHISI_BUFFER_RANGE_ERROR;
    }
    return Status;
  }
  if (!mH2OIhisi->BufferInCmdBuffer ((VOID *) CallbackList, Size)) {
    DEBUG ((DEBUG_ERROR, "[%a:%d]\n", __FUNCTION__, __LINE__));
    return IHISI_BUFFER_RANGE_ERROR;
  }

  //
  // Check the ListCount is actually in the Command Buffer.
  //
  if (!mH2OIhisi->BufferInCmdBuffer ((VOID *) &(CallbackList->ListCount), sizeof (UINT32))) {
    DEBUG ((DEBUG_ERROR, "[%a:%d]\n", __FUNCTION__, __LINE__));
    return IHISI_BUFFER_RANGE_ERROR;
  }
  //
  // Check Result Array
  //
  if (!mH2OIhisi->BufferInCmdBuffer ((VOID *) ResultArray, RESULT_SIZE(CallbackList->ListCount))) {
    DEBUG ((DEBUG_ERROR, "[%a:%d]\n", __FUNCTION__, __LINE__));
    return IHISI_BUFFER_RANGE_ERROR;
  }

  //
  // Check for buffer-overlap
  //
  if (BuffersOverlap ((VOID *) ResultArray, RESULT_SIZE(CallbackList->ListCount), (VOID *) VarBuffer, sizeof (VATS_VARIABLE_BUFFER_STRUCTURE) + VarBuffer->VariableNameLength * sizeof (CHAR16) + VarBuffer->DataSize)) {
    DEBUG ((DEBUG_ERROR, "[%a:%d]\n", __FUNCTION__, __LINE__));
    return IHISI_BUFFER_RANGE_ERROR;
  }
  if (BuffersOverlap ((VOID *) ResultArray, RESULT_SIZE(CallbackList->ListCount), (VOID *) CallbackList, Size)) {
    DEBUG ((DEBUG_ERROR, "[%a:%d]\n", __FUNCTION__, __LINE__));
    return IHISI_BUFFER_RANGE_ERROR;
  }
  if (BuffersOverlap ((VOID *) VarBuffer, sizeof (VATS_VARIABLE_BUFFER_STRUCTURE) + VarBuffer->VariableNameLength * sizeof (CHAR16) + VarBuffer->DataSize, (VOID *) CallbackList, Size)) {
    DEBUG ((DEBUG_ERROR, "[%a:%d]\n", __FUNCTION__, __LINE__));
    return IHISI_BUFFER_RANGE_ERROR;
  }

  Status = OemSvcIhisiS52HookSetupCallbackSyncCallbackIhisi (VarBuffer, CallbackList, ResultArray);
  if (Status == EFI_SUCCESS) {
    return EFI_SUCCESS;
  }

  //
  // Check the content of variable Buffer
  // 
  if (CompareGuid (&VarBuffer->VariableGuid, &gSystemConfigurationGuid) != TRUE) {
    return EFI_INVALID_PARAMETER;
  }
  if (StrnCmp (&VarBuffer->VariableName[0], SETUP_VARIABLE_NAME, VarBuffer->VariableNameLength) != 0) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Get the system password service.
  //
  Status = gSmst->SmmLocateProtocol (
                    &gEfiSysPasswordServiceProtocolGuid,
                    NULL,
                    (VOID **)&SysPassword
                    );
  if (EFI_ERROR(Status)) {
    DEBUG ((DEBUG_ERROR, "[%a:%d] Warning!! Fail to get system password service, and the status is%r\n", __FUNCTION__, __LINE__, Status));
    SysPassword = NULL;
  }
  
  SetupNVData  = VARIABLE_BUFFER_TO_DATA (VarBuffer);
  Offset = (UINTN) (&CallbackList->CallbackData);
  ResultArray->ListCount = 0;

  for (Index = 0; Index < CallbackList->ListCount; Index++) {
    CallbackData = (CALLBACK_LIST_CALL_BACK_DATA *) Offset;
    StrLength = AsciiStrnLenS ((UINT8 *) CallbackData->CallbackString, STRING_COUNT_MAX);
    if (StrLength == STRING_COUNT_MAX) {
      DEBUG ((DEBUG_ERROR, "[%a:%d] Error!! The length of the CallbackString exceeds the limit.\n", __FUNCTION__, __LINE__));
      return EFI_INVALID_PARAMETER;
    }
    
    Result = CALLBACK_RET_SUCCESS;

    switch (CallbackData->CallbackID) {
    //
    // TODO: Project owner should add callback action for these items which belong chipset.
    //

    case KEY_SAVE_EXIT:
    case KEY_SAVE_WITHOUT_EXIT:
      break;

    case KEY_EXIT_DISCARD:
    case KEY_DISCARD_CHANGE:
      break;

    case KEY_SAVE_CUSTOM:
      break;

    case KEY_LOAD_OPTIMAL:
      Result = CALLBACK_RET_SKIP_WRITE_VAR;
      Status = CommonSetVariable (
                 SETUP_VARIABLE_NAME,
                 &gSystemConfigurationGuid,
                 0,
                 0,
                 Buffer
               );
      Status = CommonSetVariable (
                 EFI_TIME_OUT_VARIABLE_NAME,
                 &gEfiGlobalVariableGuid,
                 0,
                 0,
                 Buffer
               );
      Status = CommonSetVariable (
                 L"PlatformLang",
                 &gEfiGlobalVariableGuid,
                 0,
                 0,
                 Buffer
                 );
      Status = CommonSetVariable (
                 EFI_BOOT_ORDER_VARIABLE_NAME,
                 &gEfiGlobalVariableGuid,
                 0,
                 0,
                 Buffer
               );

      IsLoadDefault = TRUE;
      break;

    case KEY_LOAD_CUSTOM:
      Result = CALLBACK_RET_FAIL;
      break;

      //
      // Main Callback Routine
      //
    case KEY_SUPERVISOR_PASSWORD:
      ZeroMem (Buffer, sizeof (CHAR16) * STRING_COUNT_MAX);
      
      if (SysPassword == NULL) {
        SetMem (CallbackData->CallbackString, StrLength, 0xFF);
        Result = CALLBACK_RET_NOT_SUPPORT;
        break;
      }
      
      AsciiStrToUnicodeStrS (CallbackData->CallbackString, Buffer, STRING_COUNT_MAX);
      for (IndexPwd = 0; IndexPwd < ARRAY_SIZE (Buffer); ++IndexPwd) {
        Buffer[IndexPwd] = (Buffer[IndexPwd] == L'\t' ? 0 : Buffer[IndexPwd]);
      }
      IndexPwd = StrnLenS (Buffer, STRING_COUNT_MAX) + 1; // new password start index!
      if (IndexPwd >= STRING_COUNT_MAX) {
        SetMem (CallbackData->CallbackString, StrLength, 0xFF);
        ZeroMem (Buffer, sizeof (CHAR16) * STRING_COUNT_MAX);
        break;
      }

      Status = SysPassword->CheckPassword (SysPassword, Buffer, StrnLenS (Buffer, STRING_COUNT_MAX), SystemSupervisor);
      if (PcdGetBool (PcdSecureSysPasswordSupported)) {
        SysPassword->UnlockPassword (SysPassword, Buffer, StrnLenS (Buffer, STRING_COUNT_MAX));
      }
      if ( (Status == EFI_NOT_FOUND) && (Buffer[0] == 0)) {
        // No Password!
        if (Buffer[IndexPwd] == 0) {
          Status = SysPassword->DisablePassword (SysPassword, SystemSupervisor);
          SetupNVData->SupervisorFlag = 0;
        } else {
          Status = SysPassword->SetPassword (SysPassword, &Buffer[IndexPwd], StrnLenS (&Buffer[IndexPwd], STRING_COUNT_MAX - IndexPwd), SystemSupervisor);
          SetupNVData->SupervisorFlag = 1;
        }
      } else if (!EFI_ERROR (Status)) {
        // Password check ok!
        if (Buffer[IndexPwd] == 0) {
          Status = SysPassword->DisablePassword (SysPassword, SystemSupervisor);
          SetupNVData->SupervisorFlag = 0;
          Status = SysPassword->DisablePassword (SysPassword, SystemUser);
          SetupNVData->UserFlag = 0;
        } else {
          Status = SysPassword->SetPassword (SysPassword, &Buffer[IndexPwd], StrnLenS (&Buffer[IndexPwd], STRING_COUNT_MAX - IndexPwd), SystemSupervisor);
          SetupNVData->SupervisorFlag = 1;
        }
      }
      if (PcdGetBool (PcdSecureSysPasswordSupported)) {
        SysPassword->LockPassword (SysPassword);
      }
      SetMem (CallbackData->CallbackString, StrLength, 0xFF);
      ZeroMem (Buffer, sizeof (CHAR16) * STRING_COUNT_MAX);
      break;

    case KEY_USER_PASSWORD:
      ZeroMem (Buffer, sizeof (CHAR16) * STRING_COUNT_MAX);
      
      if (SysPassword == NULL) {
        SetMem (CallbackData->CallbackString, StrLength, 0xFF);
        Result = CALLBACK_RET_NOT_SUPPORT;
        break;
      }
      AsciiStrToUnicodeStrS (CallbackData->CallbackString, Buffer, STRING_COUNT_MAX);
      for (IndexPwd = 0; IndexPwd < ARRAY_SIZE (Buffer); ++IndexPwd) {
        Buffer[IndexPwd] = (Buffer[IndexPwd] == L'\t' ? 0 : Buffer[IndexPwd]);
      }
      IndexPwd = StrnLenS (Buffer, STRING_COUNT_MAX) + 1; // new password start index!
      if (IndexPwd >= STRING_COUNT_MAX) {
        SetMem (CallbackData->CallbackString, StrLength, 0xFF);
        ZeroMem (Buffer, sizeof (CHAR16) * STRING_COUNT_MAX);
        break;
      }

      Status = SysPassword->CheckPassword (SysPassword, Buffer, StrnLenS (Buffer, STRING_COUNT_MAX), SystemUser);
      if (PcdGetBool (PcdSecureSysPasswordSupported)) {
        SysPassword->UnlockPassword (SysPassword, Buffer, StrnLenS (Buffer, STRING_COUNT_MAX));
      }
      if ( (Status == EFI_NOT_FOUND) && (Buffer[0] == 0)) {
        // No Password!
        if (Buffer[IndexPwd] == 0) {
          Status = SysPassword->DisablePassword (SysPassword, SystemUser);
          SetupNVData->UserFlag = 0;
        } else {
          Status = SysPassword->SetPassword (SysPassword, &Buffer[IndexPwd], StrnLenS (&Buffer[IndexPwd], STRING_COUNT_MAX - IndexPwd), SystemUser);
          SetupNVData->UserFlag = 1;
        }
      } else if (!EFI_ERROR (Status)) {
        // Password check ok!
        if (Buffer[IndexPwd] == 0) {
          Status = SysPassword->DisablePassword (SysPassword, SystemUser);
          SetupNVData->UserFlag = 0;
        } else {
          Status = SysPassword->SetPassword (SysPassword, &Buffer[IndexPwd], StrnLenS (&Buffer[IndexPwd], STRING_COUNT_MAX - IndexPwd), SystemUser);
          SetupNVData->UserFlag = 1;
        }
      }
      if (PcdGetBool (PcdSecureSysPasswordSupported)) {
        SysPassword->LockPassword (SysPassword);
      }
      SetMem (CallbackData->CallbackString, StrLength, 0xFF);
      ZeroMem (Buffer, sizeof (CHAR16) * STRING_COUNT_MAX);
      break;

      //
      // Main Callback Routine
      //
    case KEY_LANGUAGE_UPDATE:
      Status = GetLangDatabase (&LangNum, &LanguageString);
      if (Status != EFI_SUCCESS) {
        Result = CALLBACK_RET_FAIL;
        break;
      }
      Index = SetupNVData->Language;
      if (Index > LangNum) {
        Index = 0;
      }
      
      Status = CommonSetVariable (
                 L"PlatformLang",
                 &gEfiGlobalVariableGuid,
                 EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                 AsciiStrSize (((CHAR8 *)&LanguageString[Index * RFC_3066_ENTRY_SIZE])),
                 (VOID *) &LanguageString[Index * RFC_3066_ENTRY_SIZE]
                 );
      if (Status != EFI_SUCCESS) {
        Result = CALLBACK_RET_FAIL;
        break;
      }                 
      SafeFreePoolM(LanguageString);
      break;

      //
      // Server Callback Routine
      //
    default:
      break;
    }

    if (ResultArray != NULL) {
      ResultArray->ListCount += 1;
      ResultArray->ResultData[Index].CallbackID = CallbackData->CallbackID;
      ResultArray->ResultData[Index].CallbackResult = Result;
    }

    //
    // Move to next callback data
    //
    Offset = Offset + sizeof (CALLBACK_LIST_CALL_BACK_DATA) + StrLength;

  }
  if (IsLoadDefault == FALSE) {
    Status = CommonSetVariable (
               EFI_TIME_OUT_VARIABLE_NAME,
               &gEfiGlobalVariableGuid,
               BS_RT_NV_VAR_ATTR,
               sizeof (SetupNVData->Timeout),
               (VOID *) &SetupNVData->Timeout
               );
  }

  return EFI_SUCCESS;
}


/**
  Implement IHISI SPEC. AH=53h, H2O UEFI variable edit SMI service - Boot information-related services.

  @retval EFI_SUCCESS        Function succeeded.
  @return Other              Error occurred in this function.
**/
EFI_STATUS
BootInfoService (
  VOID
  )
{
  UINT64                          Rax;
  UINT64                          Rcx;
  UINT64                          Rsi;
  UINT64                          Rdi;
  UINTN                           Index;
  H2OUVE_SERVICE_FUNCTION_POINT   FuncPtr;
  H2OUVE_SERVICE_FUNCTION_MAP_TABLE       Ihisi53hFuncMapTable[] = 
  {
  //    mRax        vRax        mRcx        vRcx        mRsi        vRsi        mRdi        vRdi        pFunc
    { _AH__MASK_, _UVE__53H_, _ECX_MASK_, _ECX__01H_, DONT__CARE, DONT__CARE, DONT__CARE, DONT__CARE, GetBootTypeOrderInformation },
    { _AH__MASK_, _UVE__53H_, _ECX_MASK_, _ECX__02H_, DONT__CARE, DONT__CARE, DONT__CARE, DONT__CARE, SetBootTypeOrderInformation },
    { _AH__MASK_, _UVE__53H_, _ECX_MASK_, _ECX__03H_, DONT__CARE, DONT__CARE, DONT__CARE, DONT__CARE, GetCurrentBootTypeInformation },
    { _AH__MASK_, _UVE__53H_, _ECX_MASK_, _ECX__04H_, DONT__CARE, DONT__CARE, DONT__CARE, DONT__CARE, SetCurrentBootTypeInformation },
    { _AH__MASK_, _UVE__53H_, _ECX_MASK_, _ECX__05H_, DONT__CARE, DONT__CARE, DONT__CARE, DONT__CARE, GetCurrentBootTypeName }
  };
	
  FuncPtr = NULL;  
	
  Rax = (UINT64) (UINTN) mH2OIhisi->ReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RAX);
  Rcx = (UINT64) (UINTN) mH2OIhisi->ReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RCX);
  Rsi = (UINT64) (UINTN) mH2OIhisi->ReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RSI);
  Rdi = (UINT64) (UINTN) mH2OIhisi->ReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RDI);
	
  for (Index = 0; Index < ARRAY_SIZE (Ihisi53hFuncMapTable); ++Index) {
	
    if ( (CHECK_CONDITION (Rax, Ihisi53hFuncMapTable[Index].MaskRax, Ihisi53hFuncMapTable[Index].ValueRax)) &&
         (CHECK_CONDITION (Rcx, Ihisi53hFuncMapTable[Index].MaskRcx, Ihisi53hFuncMapTable[Index].ValueRcx)) &&
         (CHECK_CONDITION (Rsi, Ihisi53hFuncMapTable[Index].MaskRsi, Ihisi53hFuncMapTable[Index].ValueRsi)) &&
         (CHECK_CONDITION (Rdi, Ihisi53hFuncMapTable[Index].MaskRdi, Ihisi53hFuncMapTable[Index].ValueRdi))
       ) {
      FuncPtr = Ihisi53hFuncMapTable[Index].FuncPtr;
      break;
    }
	
  }
	
  if (FuncPtr == NULL) {
    return EFI_UNSUPPORTED;
  }
	
  return FuncPtr (Rax, Rcx, Rsi, Rdi);
}

