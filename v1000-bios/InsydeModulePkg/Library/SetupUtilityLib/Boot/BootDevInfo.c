/** @file
  Boot device information relative functions

;******************************************************************************
;* Copyright (c) 2017, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "BootDevInfo.h"

BOOT_DEV_INFO                               *mBootDevInfo       = NULL;
UINTN                                       mBootDevInfoCount   = 0;


/**
 Check the specific BBS Table entry is USB device

 @param[in] BbsTable                        Pointer to BBS table start address

 @retval TRUE                               It is USB device
 @retval FALSE                              It isn't USB device
**/
STATIC
BOOLEAN
IsUsbDevice (
  IN BBS_TABLE                              *BbsTable
  )
{
  if ((BbsTable->Class    == PCI_CLASS_SERIAL) &&
      (BbsTable->SubClass == PCI_CLASS_SERIAL_USB)) {
    return TRUE;
  }

  return FALSE;
}

/**
 Get the information of Boot#### variable

 @param[in]  BootOptionNum                   Boot option number
 @param[out] Attributes                      Pointer to the attribute value of Boot#### variable
 @param[out] Description                     Double pointer to the description string of Boot#### variable
 @param[out] FilePathList                    Double pointer to the file path of Boot#### variable
 @param[out] OptionalData                    Double pointer to the optional data of Boot#### variable
 @param[out] Attributes                      Pointer to the optional data size of Boot#### variable

 @retval EFI_SUCCESS                         Get the information of Boot#### variable successfully
 @retval Other                               Get variable return failed
**/
EFI_STATUS
GetBootOptionVarInfo (
  IN  UINT16                                BootOptionNum,
  OUT UINT32                                *Attributes       OPTIONAL,
  OUT CHAR16                                **Description     OPTIONAL,
  OUT EFI_DEVICE_PATH_PROTOCOL              **FilePathList    OPTIONAL,
  OUT UINT8                                 **OptionalData    OPTIONAL,
  OUT UINTN                                 *OptionalDataSize OPTIONAL
  )
{
  UINT16                                    VarName[10];
  EFI_STATUS                                Status;
  UINT8                                     *Variable;
  UINTN                                     VariableSize;
  UINT8                                     *Ptr;
  UINT32                                    Attr;
  UINT16                                    DevPathSize;
  CHAR16                                    *Desc;
  UINTN                                     DescSize;
  EFI_DEVICE_PATH_PROTOCOL                  *DevPath;
  UINT8                                     *Optional;
  UINTN                                     OptionalSize;

  UnicodeSPrint (VarName, sizeof (VarName), L"Boot%04x", BootOptionNum);
  Status = CommonGetVariableDataAndSize (VarName, &gEfiGlobalVariableGuid, &VariableSize, (VOID **) &Variable);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Ptr = Variable;
  Attr = *(UINT32 *) Ptr;
  Ptr += sizeof (UINT32);
  DevPathSize = *(UINT16 *) Ptr;
  Ptr += sizeof (UINT16);
  Desc = (CHAR16 *) Ptr;
  DescSize = StrSize (Desc);
  Ptr += DescSize;
  DevPath = (EFI_DEVICE_PATH_PROTOCOL *) Ptr;
  Ptr += DevPathSize;
  Optional = Ptr;
  OptionalSize = VariableSize - (UINTN) (Ptr - Variable);

  if (Attributes != NULL) {
    *Attributes = Attr;
  }
  if (Description != NULL) {
    *Description = AllocateCopyPool (DescSize, Desc);
    if (*Description == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
  }
  if (FilePathList != NULL) {
    *FilePathList = AllocateCopyPool (DevPathSize, DevPath);
    if (*FilePathList == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
  }
  if (OptionalData != NULL && OptionalDataSize != NULL) {
    if (OptionalSize == 0) {
      *OptionalData = NULL;
    } else {
      *OptionalData = AllocateCopyPool (OptionalSize, Optional);
      if (*OptionalData == NULL) {
        return EFI_OUT_OF_RESOURCES;
      }
    }
    *OptionalDataSize = OptionalSize;
  }

  FreePool (Variable);
  return EFI_SUCCESS;
}

/**
 Get the number of device type in boot type order

 @param[in] KernelConfig                    Pointer to kernel configuration

 @return The number of device type in boot type order.
**/
UINT16
GetBootTypeOrderCount (
  IN KERNEL_CONFIGURATION                   *KernelConfig
  )
{
  UINT16                                    Index;

  for (Index = 0;
       Index < sizeof (KernelConfig->BootTypeOrder) && KernelConfig->BootTypeOrder[Index] != 0;
       Index++) {
  }

  return Index;
}

/**
 Get the device type of input BBS table.
 If the device type is not in KERNEL_CONFIGURATION.BootTypeOrder, return other type.

 @param[in]  KernelConfig                   Pointer to kernel configuration
 @param[in]  BbsEntry                       Pointer to BBS table

 @return the device type or other if not found in BootTypeOrder
**/
STATIC
UINT16
GetDevType (
  IN KERNEL_CONFIGURATION                   *KernelConfig,
  IN BBS_TABLE                              *BbsEntry
  )
{
  UINT16                                    DevType;
  UINT16                                    Index;

  DevType = IsUsbDevice (BbsEntry) ? BBS_USB : BbsEntry->DeviceType;

  for (Index = 0; Index < sizeof (KernelConfig->BootTypeOrder); Index++) {
    if (KernelConfig->BootTypeOrder[Index] == DevType) {
      return DevType;
    }
  }

  return OTHER_DRIVER;
}

/**
 Sync mBootDevInfo to the BootOrder of setup utility browser data

 @retval EFI_SUCCESS                        Sync the boot order successfully.
 @retval Other                              Fail to get browser data.
**/
STATIC
EFI_STATUS
BootDevInfoSyncToBootOrder (
  VOID
  )
{
  EFI_STATUS                                Status;
  SETUP_UTILITY_BROWSER_DATA                *SuBrowser;
  UINT16                                    *BootOrder;
  UINT16                                    BootDeviceNum;
  UINT16                                    Index;

  Status = GetSetupUtilityBrowserData (&SuBrowser);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  BootOrder     = SuBrowser->SUCInfo->BootOrder;
  BootDeviceNum = SuBrowser->SUCInfo->AdvBootDeviceNum;
  ASSERT (BootDeviceNum == mBootDevInfoCount);

  for (Index = 0; Index < BootDeviceNum; Index++) {
    BootOrder[Index] = mBootDevInfo[Index].BootOptionNum;
  }

  return EFI_SUCCESS;
}

/**
 Sort boot device info in the sequence of boot order.

 @retval EFI_SUCCESS              Sort boot device info successfully.
 @retval Other                    Fail to get browser data.
**/
EFI_STATUS
BootDevInfoSortByBootOrder (
  VOID
  )
{
  EFI_STATUS                                Status;
  SETUP_UTILITY_BROWSER_DATA                *SuBrowser;
  UINT16                                    *BootOrder;
  UINT16                                    BootDeviceNum;
  UINT16                                    OrderIndex;
  UINT16                                    Index;
  BOOT_DEV_INFO                             TempBootDevInfo;

  Status = GetSetupUtilityBrowserData (&SuBrowser);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  BootOrder     = SuBrowser->SUCInfo->BootOrder;
  BootDeviceNum = SuBrowser->SUCInfo->AdvBootDeviceNum;
  ASSERT (BootDeviceNum == mBootDevInfoCount);

  for (OrderIndex = 0; OrderIndex < BootDeviceNum; OrderIndex++) {
    for (Index = 0; Index < mBootDevInfoCount; Index++) {
      if (mBootDevInfo[Index].BootOptionNum == BootOrder[OrderIndex]) {
        if (Index != OrderIndex) {
          CopyMem (&TempBootDevInfo         , &mBootDevInfo[OrderIndex], sizeof (BOOT_DEV_INFO));
          CopyMem (&mBootDevInfo[OrderIndex], &mBootDevInfo[Index]     , sizeof (BOOT_DEV_INFO));
          CopyMem (&mBootDevInfo[Index]     , &TempBootDevInfo         , sizeof (BOOT_DEV_INFO));
        }
        break;
      }
    }
    ASSERT (Index < mBootDevInfoCount);
  }

  return EFI_SUCCESS;
}

/**
 Sort boot device info by kernel config settings.

 @retval EFI_SUCCESS                        Sort boot device info successfully.
**/
EFI_STATUS
BootDevInfoSortByKernelConfig (
  IN KERNEL_CONFIGURATION                   *KernelConfig
  )
{
  BOOLEAN                                   IsEfiFirst;
  UINTN                                     CurrentIndex;
  UINTN                                     Index;
  BOOT_DEV_INFO                             TempDevInfo;
  UINTN                                     LegacyDevStartIndex;
  UINTN                                     LegacyDevEndIndex;
  UINT16                                    LegacyDevTypeCount;

  if (mBootDevInfo == NULL || mBootDevInfoCount == 0) {
    return EFI_SUCCESS;
  }

  if (KernelConfig->BootMenuType != NORMAL_MENU) {
    return EFI_SUCCESS;
  }

  //
  // Move all EFI devices to front or back part in mBootDevInfo based on kernel config setting.
  //
  IsEfiFirst = (BOOLEAN) (KernelConfig->BootNormalPriority == EFI_FIRST);
  for (CurrentIndex = 0; CurrentIndex < mBootDevInfoCount; CurrentIndex++) {
    if (mBootDevInfo[CurrentIndex].IsEfiBootDev == IsEfiFirst) {
      continue;
    }

    for (Index = CurrentIndex + 1; Index < mBootDevInfoCount; Index++) {
      if (mBootDevInfo[Index].IsEfiBootDev == IsEfiFirst) {
        CopyMem (&TempDevInfo                   , &mBootDevInfo[Index]       , sizeof (BOOT_DEV_INFO));
        CopyMem (&mBootDevInfo[CurrentIndex + 1], &mBootDevInfo[CurrentIndex], sizeof (BOOT_DEV_INFO) * (Index - CurrentIndex));
        CopyMem (&mBootDevInfo[CurrentIndex]    , &TempDevInfo               , sizeof (BOOT_DEV_INFO));
        break;
      }
    }
    if (Index == mBootDevInfoCount) {
      break;
    }
  }

  //
  // Re-order legacy device order by kernel config setting.
  //
  if (KernelConfig->LegacyNormalMenuType != LEGACY_NORMAL_MENU) {
    goto Done;
  }

  LegacyDevStartIndex = mBootDevInfoCount;
  LegacyDevEndIndex   = mBootDevInfoCount;

  for (Index = 0; Index < mBootDevInfoCount; Index++) {
    if (LegacyDevStartIndex == mBootDevInfoCount) {
      if (!mBootDevInfo[Index].IsEfiBootDev) {
        LegacyDevStartIndex = Index;
      }
    } else {
      if (mBootDevInfo[Index].IsEfiBootDev) {
        LegacyDevEndIndex = Index;
        break;
      }
    }
  }
  if (LegacyDevStartIndex == mBootDevInfoCount) {
    goto Done;
  }

  LegacyDevTypeCount = GetBootTypeOrderCount (KernelConfig);
  for (CurrentIndex = 0; CurrentIndex < LegacyDevTypeCount; CurrentIndex++) {
    for (Index = LegacyDevStartIndex; Index < LegacyDevEndIndex; Index++) {
      if (mBootDevInfo[Index].DevType != KernelConfig->BootTypeOrder[CurrentIndex]) {
        continue;
      }

      if (Index > LegacyDevStartIndex) {
        CopyMem (&TempDevInfo                          , &mBootDevInfo[Index]              , sizeof (BOOT_DEV_INFO));
        CopyMem (&mBootDevInfo[LegacyDevStartIndex + 1], &mBootDevInfo[LegacyDevStartIndex], sizeof (BOOT_DEV_INFO) * (Index - LegacyDevStartIndex));
        CopyMem (&mBootDevInfo[LegacyDevStartIndex]    , &TempDevInfo                      , sizeof (BOOT_DEV_INFO));
      }

      LegacyDevStartIndex++;
    }
  }

Done:
  BootDevInfoSyncToBootOrder ();
  return EFI_SUCCESS;
}

/**
 Initialize boot device info.

 @retval EFI_SUCCESS                        Initialize boot device info successfully.
 @retval EFI_OUT_OF_RESOURCES               Allocate memory failed.
 @retval Other                              Fail to get browser data.
**/
EFI_STATUS
BootDevInfoInit (
  VOID
  )
{
  EFI_STATUS                                Status;
  SETUP_UTILITY_BROWSER_DATA                *SuBrowser;
  KERNEL_CONFIGURATION                      *KernelConfig;
  UINT16                                    *BootOrder;
  UINT16                                    BootDevNum;
  BOOT_DEV_INFO                             *DevInfo;
  BOOT_DEV_INFO                             *DevInfoList;
  UINT16                                    Index;
  UINT32                                    Attribute;
  CHAR16                                    *Description;
  EFI_DEVICE_PATH_PROTOCOL                  *DevicePath;
  UINT8                                     *OptionalData;
  UINTN                                     OptionalDataSize;

  Status = GetSetupUtilityBrowserData (&SuBrowser);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  BootDevInfoShutdown ();

  KernelConfig = (KERNEL_CONFIGURATION *) SuBrowser->SCBuffer;
  BootOrder    = SuBrowser->SUCInfo->BootOrder;
  BootDevNum   = SuBrowser->SUCInfo->AdvBootDeviceNum;
  if (BootDevNum == 0) {
    return EFI_SUCCESS;
  }

  DevInfoList = AllocateZeroPool (BootDevNum * sizeof(BOOT_DEV_INFO));
  if (DevInfoList == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  for (Index = 0; Index < BootDevNum; Index++) {
    Status = GetBootOptionVarInfo (BootOrder[Index], &Attribute, &Description, &DevicePath, &OptionalData, &OptionalDataSize);
    if (EFI_ERROR (Status)) {
      return Status;
    }

    DevInfo                = &DevInfoList[Index];
    DevInfo->BootOptionNum = BootOrder[Index];
    DevInfo->pString       = Description;
    DevInfo->DevicePath    = DevicePath;
    DevInfo->IsActive      = (BOOLEAN) ((Attribute & LOAD_OPTION_ACTIVE) == LOAD_OPTION_ACTIVE);
    DevInfo->IsEfiBootDev  = (BOOLEAN) ((DevicePath->Type != BBS_DEVICE_PATH) || (DevicePath->SubType != BBS_BBS_DP));
    if (DevInfo->IsEfiBootDev) {
      DevInfo->BbsEntry = NULL;
      DevInfo->DevType  = 0;
      if (OptionalData != NULL) {
        FreePool (OptionalData);
      }
    } else {
      DevInfo->BbsEntry = (BBS_TABLE *) OptionalData;
      DevInfo->DevType  = (OptionalData == NULL) ? BBS_UNKNOWN : GetDevType (KernelConfig, (BBS_TABLE *) OptionalData);
    }
  }

  mBootDevInfo      = DevInfoList;
  mBootDevInfoCount = BootDevNum;

  BootDevInfoSortByKernelConfig (KernelConfig);
  return EFI_SUCCESS;
}

/**
 Shutdown boot device info.
**/
VOID
BootDevInfoShutdown (
  VOID
  )
{
  UINTN                                     Index;
  BOOT_DEV_INFO                             *DevInfo;

  if (mBootDevInfo == NULL) {
    return;
  }

  for (Index = 0; Index < mBootDevInfoCount; Index++) {
    DevInfo = &mBootDevInfo[Index];

    if (DevInfo->pString != NULL) {
      FreePool (DevInfo->pString);
    }
    if (DevInfo->BbsEntry != NULL) {
      FreePool (DevInfo->BbsEntry);
    }
    if (DevInfo->DevicePath != NULL) {
      FreePool (DevInfo->DevicePath);
    }
  }
  FreePool (mBootDevInfo);

  mBootDevInfoCount = 0;
  mBootDevInfo      = NULL;
}

/**
 Get active value by the specific boot option number.

 @param[in] BootOptionNum                   Boot option number

 @return Active value of specific boot option or TRUE if it can not find the corresponding boot device info.
**/
BOOLEAN
BootDevInfoGetActiveValue (
  IN UINT16                                 BootOptionNum
  )
{
  UINT16                                    Index;

  if (mBootDevInfo == NULL) {
    return TRUE;
  }

  for (Index = 0; Index < mBootDevInfoCount; Index++) {
    if (mBootDevInfo[Index].BootOptionNum == BootOptionNum) {
      return mBootDevInfo[Index].IsActive;
    }
  }

  return TRUE;
}

/**
 Set boot device info to variable storage.

 @retval EFI_SUCCESS              Set to variable storage successfully.
 @retval EFI_NOT_FOUND            There is no boot device info.
 @retval Other                    There is one boot device info failed to set to variable storage.
**/
EFI_STATUS
BootDevInfoSetToVariable (
  VOID
  )
{
  EFI_STATUS                                Status;
  EFI_STATUS                                SetVarStatus;
  UINT16                                    Index;
  CHAR16                                    BootOptionName[20];
  UINT8                                     *BootOption;
  UINTN                                     BootOptionSize;
  UINT32                                    *Attribute;
  UINT32                                    OrgAttribute;

  if (mBootDevInfo == NULL || mBootDevInfoCount == 0) {
    return EFI_NOT_FOUND;
  }

  SetVarStatus = EFI_SUCCESS;
  for (Index = 0; Index < mBootDevInfoCount; Index++) {
    UnicodeSPrint (BootOptionName, sizeof(BootOptionName), L"Boot%04x", mBootDevInfo[Index].BootOptionNum);
    Status = CommonGetVariableDataAndSize (
               BootOptionName,
               &gEfiGlobalVariableGuid,
               &BootOptionSize,
               (VOID **) &BootOption
               );
    if (Status != EFI_SUCCESS || BootOption == NULL) {
      continue;
    }

    Attribute = (UINT32 *) BootOption;
    OrgAttribute = *Attribute;
    if (mBootDevInfo[Index].IsActive) {
      *Attribute |= ((UINT32) LOAD_OPTION_ACTIVE);
    } else {
      *Attribute &= (~((UINT32) LOAD_OPTION_ACTIVE));
    }

    if (OrgAttribute != *Attribute) {
      Status = CommonSetVariable (
                 BootOptionName,
                 &gEfiGlobalVariableGuid,
                 EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                 BootOptionSize,
                 BootOption
                 );
      if (EFI_ERROR (Status)) {
        SetVarStatus = Status;
      }
    }

    FreePool (BootOption);
  }

  return SetVarStatus;
}

