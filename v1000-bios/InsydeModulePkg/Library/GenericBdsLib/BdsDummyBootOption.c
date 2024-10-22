/** @file
  Dummy boot option related functions

;******************************************************************************
;* Copyright (c) 2017 - 2020, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "BdsDummyBootOption.h"

typedef
BOOLEAN
(*CHECK_BOOT_OPTION_TYPE) (
  IN  UINT16     OptionNum
  );
//
// OptionNum : Dummy function for specific type (ex: dummy usb number is DummyUsbBootOptionNum).
// CheckOptionType : Function uses option number to check option type.
// Found : Record this option is whether found.
//
typedef struct _DUMMY_OPTION_TYPE_MAP {
  DUMMY_BOOT_OPTION_NUM        OptionNum;
  CHECK_BOOT_OPTION_TYPE       CheckOptionType;
  BOOLEAN                      Found;
  EFI_BOOT_ORDER_PRIORITY      Priority;
} DUMMY_OPTION_TYPE_MAP;

STATIC
BOOLEAN
IsUefiUsbBootOption (
  IN  UINT16     OptionNum
  );

STATIC
BOOLEAN
IsUefiCdBootOption (
  IN  UINT16     OptionNum
  );

STATIC
BOOLEAN
IsUefiNetworkBootOption (
  IN  UINT16     OptionNum
  );

STATIC DUMMY_OPTION_TYPE_MAP    mOptionTypeTable[] = {{DummyUsbBootOptionNum,     IsUefiUsbBootOption,     FALSE, RemovableEfiBootOption},
                                                     {DummyCDBootOptionNum,      IsUefiCdBootOption,      FALSE, RemovableEfiBootOption},
                                                     {DummyNetwokrBootOptionNum, IsUefiNetworkBootOption, FALSE, NetWorkEfiBootOption  }
                                                    };
#define OPTION_TYPE_TABLE_COUNT (sizeof (mOptionTypeTable) / sizeof (DUMMY_OPTION_TYPE_MAP))

STATIC DUMMY_BOOT_OPTION_INFO  mDummyBootOptionTable[] = {
  {DummyUsbBootOptionNum,     L"EFI USB Device"},
  {DummyCDBootOptionNum,      L"EFI DVD/CDROM"},
  {DummyNetwokrBootOptionNum, L"EFI Network"}
  };

#define VIRTUAL_BOOT_ORDER_VARIABLE_NAME            L"VirtualBootOrder"

/**
 Add boot option(s) into the input boot order.

 @param[in]      OptionList     The list of boot options.
 @param[in]      OptionNum      The number of boot options in the option list.
 @param[in]      BootOrderSize  The buffer size of boot order.
 @param[in, out] BootOrder      Pointer to the boot order.
 @param[in, out] BootOrderNum   Pointer to the number of boot options in the boot order.

 @retval EFI_SUCCESS            Success to add boot options into input boot order.
 @retval EFI_INVALID_PARAMETER  Input parameter is NULL.
 @retval EFI_OUT_OF_RESOURCES   The buffer size of boot order is not enough.
**/
STATIC
EFI_STATUS
AddOptionsToBootOrder (
  IN     UINT16                 *OptionList,
  IN     UINTN                  OptionNum,
  IN     UINTN                  BootOrderSize,
  IN OUT UINT16                 *BootOrder,
  IN OUT UINTN                  *BootOrderNum
  )
{
  if (OptionList == NULL || BootOrder == NULL || BootOrderNum == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (BootOrderSize < (*BootOrderNum + OptionNum) * sizeof(UINT16)) {
    return EFI_OUT_OF_RESOURCES;
  }

  CopyMem (&BootOrder[*BootOrderNum], OptionList, OptionNum * sizeof(UINT16));
  (*BootOrderNum) += OptionNum;

  return EFI_SUCCESS;
}

/**
 Remove boot option(s) in the input boot order.

 @param[in]      OptionList     The list of boot options.
 @param[in]      OptionNum      The number of boot options in the option list.
 @param[in, out] BootOrder      Pointer to the boot order.
 @param[in, out] BootOrderNum   Pointer to the number of boot options in the boot order.

 @retval EFI_SUCCESS            Success to remove boot options into input boot order.
 @retval EFI_INVALID_PARAMETER  Input parameter is NULL.
**/
STATIC
EFI_STATUS
RemoveOptionsInBootOrder (
  IN     UINT16                 *OptionList,
  IN     UINTN                  OptionNum,
  IN OUT UINT16                 *BootOrder,
  IN OUT UINTN                  *BootOrderNum
  )
{
  UINTN                         OptionIndex;
  UINTN                         Index;
  EFI_STATUS                    Status;

  if (OptionList == NULL || BootOrder == NULL || BootOrderNum == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  for (OptionIndex = 0; OptionIndex < OptionNum; OptionIndex++) {
    Status = GetIndexInOrderArray (BootOrder, *BootOrderNum, OptionList[OptionIndex], &Index);
    if (!EFI_ERROR (Status)) {
      CopyMem (
        &BootOrder[Index],
        &BootOrder[Index + 1],
        (*BootOrderNum - Index - 1) * sizeof (UINT16)
        );
      (*BootOrderNum)--;
    }
  }

  return EFI_SUCCESS;
}

/**
 Create dummy boot options which use to selected by UEFI OS.

 @retval EFI_SUCCESS   Create dummy boot options successful.
 @retval Other         Any error occured while creating dummy boot options.
**/
EFI_STATUS
CreateDummyBootOptions (
  VOID
  )
{
  UINT16                        BootOption[BOOT_OPTION_MAX_CHAR];
  UINTN                         VariableSize;
  UINT8                         *BootOptionVar;
  EFI_STATUS                    Status;
  EFI_DEVICE_PATH_PROTOCOL      DevicePath;
  UINTN                         TableCnt;
  UINTN                         Index;

  TableCnt = sizeof (mDummyBootOptionTable) / sizeof (DUMMY_BOOT_OPTION_INFO);
  Status = EFI_SUCCESS;
  for (Index = 0; Index < TableCnt; Index++) {
    ASSERT (BdsLibIsDummyBootOption (mDummyBootOptionTable[Index].BootOptionNum));
    UnicodeSPrint (BootOption, sizeof (BootOption), L"Boot%04x", mDummyBootOptionTable[Index].BootOptionNum);
    BootOptionVar = BdsLibGetVariableAndSize (
                      BootOption,
                      &gEfiGlobalVariableGuid,
                      &VariableSize
                      );
    if (BootOptionVar != NULL) {
      FreePool (BootOptionVar);
    } else {
      SetDevicePathEndNode (&DevicePath);
      Status = BdsLibUpdateOptionVar (
                 BootOption,
                 LOAD_OPTION_ACTIVE,
                 &DevicePath,
                 mDummyBootOptionTable[Index].Description,
                 (UINT8 *) "RC",
                 2
                 );
      ASSERT_EFI_ERROR (Status);
    }
  }
  return Status;
}

/**
 Parse the input boot order and return first adjacent boot options which belongs to specific dummy boot option.
 If the first option of boot order does not belong to specific dummy boot option, return not found.

 @param[in]  DummyOptionNum    Dummy boot option number.
 @param[in]  BootOrder         Pointer to the boot order.
 @param[in]  BootOrderNum      The number of boot options in the boot order.
 @param[out] OptionNum         Pointer to the number of boot options returned in the output option list.

 @return The list of boot options which are the first adjacent boot options in boot order and belongs to specific
         dummy boot option or NULL if not found.
**/
STATIC
UINT16 *
GetFirstAdjacentOptionsByDummyOptionNum (
  IN  DUMMY_BOOT_OPTION_NUM     DummyOptionNum,
  IN  UINT16                    *BootOrder,
  IN  UINTN                     BootOrderNum,
  OUT UINTN                     *OptionNum
  )
{
  UINTN                         Index;

  for (Index = 0; Index < BootOrderNum; Index++) {
    if (BdsLibGetDummyBootOptionNum (BootOrder[Index]) != DummyOptionNum) {
      break;
    }
  }

  *OptionNum  = Index;
  return AllocateCopyPool (Index * sizeof(UINT16), BootOrder);
}

/**
 Parse the input boot order and return all boot options which belong to specific dummy boot option.

 @param[in]  DummyOptionNum    Dummy boot option number.
 @param[in]  BootOrder         Pointer to the boot order.
 @param[in]  BootOrderNum      The number of boot options in the boot order.
 @param[out] OptionNum         Pointer to the number of boot options returned in the output option list.

 @return The list of boot options which belong to specific dummy boot option or NULL if not found.
**/
STATIC
UINT16 *
GetAllOptionsByDummyOptionNum (
  IN  DUMMY_BOOT_OPTION_NUM     DummyOptionNum,
  IN  UINT16                    *BootOrder,
  IN  UINTN                     BootOrderNum,
  OUT UINTN                     *OptionNum
  )
{
  UINTN                         Index;
  UINTN                         Count;
  UINT16                        *OptionList;

  Count = 0;
  OptionList = AllocatePool (BootOrderNum * sizeof(UINT16));
  if (OptionList == NULL) {
    *OptionNum = 0;
    return OptionList;
  }

  for (Index = 0; Index < BootOrderNum; Index++) {
    if (BdsLibGetDummyBootOptionNum (BootOrder[Index]) == DummyOptionNum) {
      OptionList[Count] = BootOrder[Index];
      Count++;
    }
  }

  *OptionNum = Count;

  if (Count == 0) {
    if (OptionList != NULL) {
      FreePool (OptionList);
      OptionList = NULL;
    }
  }

  return OptionList;
}

/**
 According to device path to check this boot option is whether an EFI shell boot option.

 @param[in] DevicePath          Pointer to device path instance

 @retval TRUE                   This is an EFI shell boot option.
 @retval FALSE                  This isn't an EFI shell boot option.
**/
STATIC
BOOLEAN
IsUefiShellBootOption (
  IN EFI_DEVICE_PATH_PROTOCOL   *DevicePath
  )
{
  EFI_DEVICE_PATH_PROTOCOL      *DevicePathPtr;
  EFI_DEVICE_PATH_PROTOCOL      *LastDeviceNode;
  EFI_GUID                      *FileGuid;

  DevicePathPtr  = DevicePath;
  LastDeviceNode = DevicePath;
  while (!IsDevicePathEnd (DevicePathPtr)) {
    LastDeviceNode = DevicePathPtr;
    DevicePathPtr = NextDevicePathNode (DevicePathPtr);
  }
  FileGuid = EfiGetNameGuidFromFwVolDevicePathNode ((MEDIA_FW_VOL_FILEPATH_DEVICE_PATH *) LastDeviceNode);
  if (FileGuid != NULL && CompareGuid (FileGuid, PcdGetPtr(PcdShellFile))) {
    return TRUE;
  }

  return FALSE;
}

/**
 According to boot option number to check this boot option is whether a virtual boot option.

 @param[in] OptionNum           The boot option number.

 @retval TRUE                   This is a virtual boot option.
 @retval FALSE                  This isn't a virtual boot option.
**/
BOOLEAN
IsVirtualBootOption (
  IN UINT16                     OptionNum
  )
{
  if (BdsLibIsDummyBootOption (OptionNum) || BdsLibGetDummyBootOptionNum (OptionNum) == DummyBootOptionEndNum) {
    return TRUE;
  }

  return FALSE;
}

/**
 Get the boot order priority value of input boot option number.

 @param[in]  BootOptionNum        Boot option number
 @param[out] Priority             Pointer to the priority value

 @retval EFI_SUCCESS              Get the boot order priority value successfully.
 @retval EFI_NOT_FOUND            Boot option variable is not exist.
 @retval Other                    Fail to get priority value from EFI_BOOT_OPTION_POLICY_PROTOCOL.
**/
EFI_STATUS
GetBootOrderPriority (
  IN  UINTN                        BootOptionNum,
  OUT EFI_BOOT_ORDER_PRIORITY      *Priority
  )
{
  EFI_STATUS                       Status;
  UINTN                            Index;
  EFI_BOOT_OPTION_POLICY_PROTOCOL  *BootOptionPolicy;
  CHAR16                           OptionName[20];
  UINTN                            OptionVarSize;
  UINT8                            *OptionVar;
  UINT8                            *Ptr;
  UINT16                           DevPathSize;
  EFI_DEVICE_PATH_PROTOCOL         *DevicePath;
  UINT8                            *OptionalData;
  UINT32                           OptionalDataSize;
  BOOLEAN                          CreateByOS;

  //
  // If option number is dummy option, return the defined priority.
  //
  for (Index = 0; Index < OPTION_TYPE_TABLE_COUNT; Index++) {
    if ((UINTN) mOptionTypeTable[Index].OptionNum == BootOptionNum) {
      *Priority = mOptionTypeTable[Index].Priority;
      return EFI_SUCCESS;
    }
  }

  //
  // Query priority value by passing option's device path to EFI_BOOT_OPTION_POLICY_PROTOCOL.
  //
  Status = gBS->LocateProtocol (&gEfiBootOptionPolicyProtocolGuid, NULL, (VOID **) &BootOptionPolicy);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  UnicodeSPrint (OptionName, sizeof (OptionName), L"Boot%04x", BootOptionNum);
  OptionVar = BdsLibGetVariableAndSize (OptionName, &gEfiGlobalVariableGuid, &OptionVarSize);
  if (OptionVar == NULL) {
    return EFI_NOT_FOUND;
  }

  Ptr = OptionVar;
  Ptr += sizeof (UINT32);
  DevPathSize = *((UINT16 *) Ptr);
  Ptr += sizeof (UINT16);
  Ptr += StrSize ((CHAR16 *) Ptr);
  DevicePath = (EFI_DEVICE_PATH_PROTOCOL *) Ptr;
  Ptr += DevPathSize;
  OptionalData = Ptr;
  OptionalDataSize = (UINT32) (OptionVarSize - (UINTN) (Ptr - OptionVar));

  CreateByOS = TRUE;
  if (((OptionalDataSize == 2) && (AsciiStrnCmp ((CHAR8 *) OptionalData, "RC", 2) == 0)) ||
      ((OptionalDataSize == SHELL_OPTIONAL_DATA_SIZE) && IsUefiShellBootOption (DevicePath) && (AsciiStrnCmp ((CHAR8 *)OptionalData, "RC", 2) == 0))) {
    CreateByOS = FALSE;
  }

  Status = BootOptionPolicy->GetEfiOptionPriority (
                               BootOptionPolicy,
                               CreateByOS,
                               DevicePath,
                               Priority
                               );
  FreePool (OptionVar);
  return Status;
}

/**
 Get virtual boot order.

 @param[out] VirtualBootOrderNum    Pointer to boot option number in order
 @param[out] VirtualBootOrder       Double pointer to virtual boot order

 @retval EFI_SUCCESS     Get virtual boot order successfully.
 @retval EFI_NOT_FOUND   There is no virtual boot order info.
**/
STATIC
EFI_STATUS
GetVirtualBootOrder (
  OUT UINTN                     *VirtualBootOrderNum,
  OUT UINT16                    **VirtualBootOrder
  )
{
  UINT16                        *BootOrder;
  UINTN                         BootOrderSize;
  EFI_STATUS                    Status;
  BOOLEAN                       IsPhysicalBootOrder;
  BOOLEAN                       IsBootOrderHookEnabled;

  //
  // Virtual boot order is came from
  //   (1) BootOrder variable which is virtual boot order
  //   (2) VirtualBootOrder variable
  //
  IsBootOrderHookEnabled = BdsLibIsBootOrderHookEnabled ();
  if (IsBootOrderHookEnabled) {
    BdsLibDisableBootOrderHook ();
  }
  BootOrder = BdsLibGetVariableAndSize (
                L"BootOrder",
                &gEfiGlobalVariableGuid,
                &BootOrderSize
                );
  if (IsBootOrderHookEnabled) {
    BdsLibEnableBootOrderHook ();
  }
  if (BootOrder != NULL) {
    Status = BdsLibGetBootOrderType (BootOrder, BootOrderSize / sizeof (UINT16), &IsPhysicalBootOrder);
    if (!EFI_ERROR (Status) && !IsPhysicalBootOrder) {
      *VirtualBootOrder    = BootOrder;
      *VirtualBootOrderNum = BootOrderSize / sizeof (UINT16);
      return EFI_SUCCESS;
    }
    FreePool (BootOrder);
  }

  BootOrder = BdsLibGetVariableAndSize (
                VIRTUAL_BOOT_ORDER_VARIABLE_NAME,
                &gEfiGenericVariableGuid,
                &BootOrderSize
                );
  if (BootOrder != NULL) {
    *VirtualBootOrder    = BootOrder;
    *VirtualBootOrderNum = BootOrderSize / sizeof (UINT16);
    return EFI_SUCCESS;
  }

  return EFI_NOT_FOUND;
}

/**
 Gett the new position of current boot option is determined by virtual boot order.

 @param[in]  OptionNum          Current boot option number
 @param[in]  BootOrder          Pointer to current boot order
 @param[in]  BootOrderSize      Current boot order size
 @param[out] NewPosition        Pointer to the new position in Current boot order

 @retval EFI_SUCCESS     Get the new position successfully.
 @retval EFI_NOT_FOUND   Current boot option doesn't belong to dummy boot option or fail to get virtual boot order.
**/
EFI_STATUS
GetPositionByVirtualBootOrder (
  IN  UINT16                    OptionNum,
  IN  UINT16                    *BootOrder,
  IN  UINTN                     BootOrderSize,
  OUT UINTN                     *NewPosition
  )
{
  DUMMY_BOOT_OPTION_NUM         DummyBootOptionNum;
  EFI_STATUS                    Status;
  UINT16                        *VirtualBootOrder;
  UINTN                         VirtualBootOrderNum;
  UINT16                        VirtualBootOptionNum;
  UINTN                         VirtualIndex;
  UINTN                         BootOrderNum;
  UINTN                         Index;

  //
  // Get the index value of corresponding dummy boot option of current boot option in the virtual boot order.
  //
  DummyBootOptionNum = BdsLibGetDummyBootOptionNum (OptionNum);
  if (DummyBootOptionNum == DummyBootOptionEndNum) {
    return EFI_NOT_FOUND;
  }

  Status = GetVirtualBootOrder (&VirtualBootOrderNum, &VirtualBootOrder);
  if (EFI_ERROR (Status)) {
    return EFI_NOT_FOUND;
  }

  Status = GetIndexInOrderArray (VirtualBootOrder, VirtualBootOrderNum, DummyBootOptionNum, &VirtualIndex);
  if (EFI_ERROR (Status)) {
    FreePool (VirtualBootOrder);
    return Status;
  }

  //
  // If boot option in virtual boot order is found in boot order, it means that find the new position.
  //
  BootOrderNum = BootOrderSize / sizeof (UINT16);
  for (; VirtualIndex < VirtualBootOrderNum; VirtualIndex++) {
    for (Index = 0; Index < BootOrderNum; Index++) {
      if (IsVirtualBootOption (BootOrder[Index])) {
        VirtualBootOptionNum = BootOrder[Index];
      } else {
        VirtualBootOptionNum = BdsLibGetDummyBootOptionNum (BootOrder[Index]);
      }

      if (VirtualBootOptionNum == VirtualBootOrder[VirtualIndex]) {
        *NewPosition = Index;
        FreePool (VirtualBootOrder);
        return EFI_SUCCESS;
      }
    }
  }

  *NewPosition = BootOrderNum;
  FreePool (VirtualBootOrder);
  return EFI_SUCCESS;
}

/**
 Check if the new position of current boot option is determined by virtual boot order.

 @param[in] OptionNum               Current boot option number

 @retval TRUE     The new position of current boot option is defined by virtual boot order.
 @retval FALSE    The new position of current boot option is not defined by virtual boot order.
**/
BOOLEAN
IsNewPositionDeterminedByVirtualBootOrder (
  IN UINT16                     OptionNum
  )
{
  EFI_STATUS                    Status;
  KERNEL_CONFIGURATION          SystemConfiguration;
  DUMMY_BOOT_OPTION_NUM         DummyBootOptionNum;
  UINT16                        *VirtualBootOrder;
  UINTN                         VirtualBootOrderNum;
  BOOLEAN                       Found;

  if (!FeaturePcdGet (PcdAutoCreateDummyBootOption)) {
    return FALSE;
  }

  //
  // If following confitions are all satisfied, the new position should be determined by virtual boot order.
  // (1) KERNEL_CONFIGURATION.NewPositionPolicy is AUTO.
  // (2) Current boot option belongs to dummy boot option.
  // (3) Dummy boot option exist in virtual boot order.
  //
  Status = GetKernelConfiguration (&SystemConfiguration);
  if (EFI_ERROR (Status) || SystemConfiguration.NewPositionPolicy != IN_AUTO) {
    return FALSE;
  }

  DummyBootOptionNum = BdsLibGetDummyBootOptionNum (OptionNum);
  if (DummyBootOptionNum == DummyBootOptionEndNum) {
    return FALSE;
  }

  Status = GetVirtualBootOrder (&VirtualBootOrderNum, &VirtualBootOrder);
  if (EFI_ERROR (Status)) {
    return FALSE;
  }

  Found = IsOptionNumInOrderArray (VirtualBootOrder, VirtualBootOrderNum, DummyBootOptionNum);
  FreePool (VirtualBootOrder);

  return Found;
}

/**
 According to boot option number to check this boot option is whether an EFI DVD/CDROM boot option.

 @param[in] OptionNum           The boot option number.

 @retval TRUE                   This is an EFI DVD/CDROM boot option.
 @retval FALSE                  This isn't an EFI DVD/CDROM boot option.
**/
STATIC
BOOLEAN
IsUefiCdBootOption (
  IN UINT16                     OptionNum
  )
{
  EFI_DEVICE_PATH_PROTOCOL      *DevicePath;
  BOOLEAN                       IsCd;
  UINT32                        DevicePathType;

  DevicePath = BdsLibGetDevicePathFromBootOption (OptionNum);
  if (DevicePath == NULL) {
    return FALSE;
  }

  IsCd = FALSE;
  DevicePathType = BdsLibGetBootTypeFromDevicePath (DevicePath);
  if (DevicePathType == BDS_EFI_MEDIA_CDROM_BOOT) {
    IsCd = TRUE;
  }

  FreePool (DevicePath);
  return IsCd;
}

/**
 According to boot option number to check this boot option is whether an EFI network boot option.

 @param[in] OptionNum           The boot option number.

 @retval TRUE                   This is an EFI network boot option.
 @retval FALSE                  This isn't an EFI network boot option.
**/
STATIC
BOOLEAN
IsUefiNetworkBootOption (
  IN UINT16                     OptionNum
  )
{
  EFI_DEVICE_PATH_PROTOCOL      *DevicePath;
  BOOLEAN                       IsNetwork;
  UINT32                        DevicePathType;

  DevicePath = BdsLibGetDevicePathFromBootOption (OptionNum);
  if (DevicePath == NULL) {
    return FALSE;
  }

  IsNetwork = FALSE;
  DevicePathType = BdsLibGetBootTypeFromDevicePath (DevicePath);
  if (DevicePathType == BDS_EFI_MESSAGE_MAC_BOOT) {
    IsNetwork = TRUE;
  }

  FreePool (DevicePath);
  return IsNetwork;
}

/**
 According to boot option number to check this boot option is whether an EFI USB boot option.

 @param[in] OptionNum           The boot option number.

 @retval TRUE                   This is an EFI USB boot option.
 @retval FALSE                  This isn't an EFI USB boot option.
**/
STATIC
BOOLEAN
IsUefiUsbBootOption (
  IN UINT16                     OptionNum
  )
{
  EFI_DEVICE_PATH_PROTOCOL      *DevicePath;
  BOOLEAN                       IsUsb;
  UINT32                        DevPathType;

  DevicePath = BdsLibGetDevicePathFromBootOption (OptionNum);
  if (DevicePath == NULL) {
    return FALSE;
  }

  IsUsb = FALSE;
  if (!((DevicePathType (DevicePath) == MESSAGING_DEVICE_PATH) &&
      (DevicePathSubType (DevicePath) == MSG_USB_CLASS_DP))) {
    DevPathType = BdsLibGetBootTypeFromDevicePath (DevicePath);
    if (DevPathType == BDS_EFI_MESSAGE_USB_DEVICE_BOOT) {
      IsUsb = TRUE;
    }
  }

  FreePool (DevicePath);
  return IsUsb;
}

/**
 Check whether BootOrder variable is virtual boot order or not.

 @retval TRUE                   BootOrder variable is virtual boot order.
 @retval FALSE                  BootOrder variable is not virtual boot order.
**/
BOOLEAN
IsVirtualBootOrder (
  VOID
  )
{
  UINT16                        *BootOrder;
  UINTN                         BootOrderSize;
  EFI_STATUS                    Status;
  BOOLEAN                       IsPhysicalBootOrder;

  BootOrder = BdsLibGetVariableAndSize (L"BootOrder", &gEfiGlobalVariableGuid, &BootOrderSize);
  if (BootOrder != NULL) {
    Status = BdsLibGetBootOrderType (BootOrder, BootOrderSize / sizeof (UINT16), &IsPhysicalBootOrder);
    FreePool (BootOrder);
    if (!EFI_ERROR (Status) && !IsPhysicalBootOrder) {
      return TRUE;
    }
  }

  return FALSE;
}

/**
 Convert input physical boot order to virtual boot order.

 @param[in]  PhysicalBootOrder        Pointer to current physical boot order
 @param[in]  PhysicalBootOrderNum     Boot option number in physical boot order
 @param[in]  OrgVirtualBootOrder      Pointer to original virtual boot order
 @param[in]  OrgVirtualBootOrderNum   Boot option number in original virtual boot order
 @param[out] NewVirtualBootOrder      Double pointer to new virtual boot order
 @param[out] NewVirtualBootOrderSize  Pointer to the total size in bytes of new virtual boot order

 @retval EFI_SUCCESS                  Convert input physical boot order successfully.
 @retval EFI_OUT_OF_RESOURCES         Failed to allocate memory.
 @retval EFI_NOT_FOUND                Failed to get kernel configuration.
**/
EFI_STATUS
ConvertToVirtualBootOrder (
  IN  UINT16                        *PhysicalBootOrder,
  IN  UINTN                         PhysicalBootOrderNum,
  IN  UINT16                        *OrgVirtualBootOrder OPTIONAL,
  IN  UINTN                         OrgVirtualBootOrderNum  OPTIONAL,
  OUT UINT16                        **NewVirtualBootOrder,
  OUT UINTN                         *NewVirtualBootOrderSize
  )
{
  UINT16                        *VirtualBootOrder;
  UINTN                         VirtualBootOrderNum;
  EFI_STATUS                    Status;
  UINTN                         Index;
  UINTN                         TableIndex;
  UINTN                         OrgIndex;
  UINTN                         SearchIndex;
  KERNEL_CONFIGURATION          *KernelConfig;
  UINTN                         Size;
  BOOLEAN                       IsEfiFirst;
  EFI_BOOT_ORDER_PRIORITY       Priority;

  //
  // Intialize option type maping table
  //
  for (TableIndex = 0; TableIndex < OPTION_TYPE_TABLE_COUNT; TableIndex++) {
    mOptionTypeTable[TableIndex].Found = FALSE;
  }

  VirtualBootOrder = AllocateZeroPool (sizeof (UINT16) * (PhysicalBootOrderNum + DummyBootOptionEndNum - DummyBootOptionStartNum));
  if (VirtualBootOrder == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  VirtualBootOrderNum = 0;
  for (Index = 0; Index < PhysicalBootOrderNum; Index++) {
    if (IsVirtualBootOption (PhysicalBootOrder[Index])) {
      VirtualBootOrder[VirtualBootOrderNum] = PhysicalBootOrder[Index];
      VirtualBootOrderNum++;
    } else {
      //
      // According to the order of corresponding option type to put dummy option
      //
      for (TableIndex = 0; TableIndex < OPTION_TYPE_TABLE_COUNT; TableIndex++) {
        if (!mOptionTypeTable[TableIndex].Found &&
            mOptionTypeTable[TableIndex].CheckOptionType (PhysicalBootOrder[Index])) {
          mOptionTypeTable[TableIndex].Found = TRUE;
          VirtualBootOrder[VirtualBootOrderNum] = mOptionTypeTable[TableIndex].OptionNum;
          VirtualBootOrderNum++;
          break;
        }
      }
    }
  }

  if (OrgVirtualBootOrder != NULL) {
    //
    // Based on original virtual boot order, put dummy option in original position.
    //
    for (OrgIndex = 0; OrgIndex < OrgVirtualBootOrderNum; OrgIndex++) {
      for (TableIndex = 0; TableIndex < OPTION_TYPE_TABLE_COUNT; TableIndex++) {
        if (mOptionTypeTable[TableIndex].OptionNum == OrgVirtualBootOrder[OrgIndex]) {
          break;
        }
      }
      if (TableIndex == OPTION_TYPE_TABLE_COUNT || mOptionTypeTable[TableIndex].Found) {
        continue;
      }

      for (SearchIndex = OrgIndex + 1; SearchIndex < OrgVirtualBootOrderNum; SearchIndex++) {
        Status = GetIndexInOrderArray (VirtualBootOrder, VirtualBootOrderNum, OrgVirtualBootOrder[SearchIndex], &Index);
        if (!EFI_ERROR (Status)) {
          CopyMem (
            &VirtualBootOrder[Index + 1],
            &VirtualBootOrder[Index],
            (VirtualBootOrderNum - Index) * sizeof (UINT16)
            );
          VirtualBootOrder[Index] = OrgVirtualBootOrder[OrgIndex];
          VirtualBootOrderNum++;
          mOptionTypeTable[TableIndex].Found = TRUE;
          break;
        }
      }
    }
  }

  //
  // Put all of other dummy boot options in the boot order.
  // If position policy is auto, put dummy option in the correct position by specific priority.
  // If position policy is not auto, put dummy option at the end of EFI boot options.
  //
  KernelConfig = BdsLibGetVariableAndSize (SETUP_VARIABLE_NAME, &gSystemConfigurationGuid, &Size);
  if (KernelConfig == NULL) {
    return EFI_NOT_FOUND;
  }
  IsEfiFirst = (BOOLEAN) (KernelConfig->BootNormalPriority == 0);

  if (KernelConfig->NewPositionPolicy == ADD_POSITION_AUTO) {
    for (Index = 0; Index < VirtualBootOrderNum; Index++) {
      //
      // If all dummy boot options are found, no need to go through boot order to insert dummy boot option.
      //
      for (TableIndex = 0; TableIndex < OPTION_TYPE_TABLE_COUNT; TableIndex++) {
        if (!mOptionTypeTable[TableIndex].Found) {
          break;
        }
      }
      if (TableIndex == OPTION_TYPE_TABLE_COUNT) {
        break;
      }

      if (IsEfiDevice (VirtualBootOrder[Index])) {
        Priority = 0;
        GetBootOrderPriority (VirtualBootOrder[Index], &Priority);
      } else {
        Priority = (EFI_BOOT_ORDER_PRIORITY) (IsEfiFirst ? 0xFF : 0);
      }

      for (TableIndex = 0; TableIndex < OPTION_TYPE_TABLE_COUNT; TableIndex++) {
        if (!mOptionTypeTable[TableIndex].Found && mOptionTypeTable[TableIndex].Priority < Priority) {
          mOptionTypeTable[TableIndex].Found = TRUE;
          CopyMem (
            &VirtualBootOrder[Index + 1],
            &VirtualBootOrder[Index],
            (VirtualBootOrderNum - Index) * sizeof (UINT16)
            );
          VirtualBootOrder[Index] = mOptionTypeTable[TableIndex].OptionNum;
          VirtualBootOrderNum++;
          Index++;
        }
      }
    }
  }
  for (TableIndex = 0; TableIndex < OPTION_TYPE_TABLE_COUNT; TableIndex++) {
    if (!mOptionTypeTable[TableIndex].Found) {
      mOptionTypeTable[TableIndex].Found = TRUE;
      VirtualBootOrder[VirtualBootOrderNum] = mOptionTypeTable[TableIndex].OptionNum;
      VirtualBootOrderNum++;
    }
  }
  AdjustBootOrder (IsEfiFirst, VirtualBootOrderNum, VirtualBootOrder);
  FreePool (KernelConfig);

  *NewVirtualBootOrder     = VirtualBootOrder;
  *NewVirtualBootOrderSize = VirtualBootOrderNum * sizeof (UINT16);
  return EFI_SUCCESS;
}

/**
 Synchronize the contents of "BootOrder" to prevent from the contents is updated by SCU or UEFI OS.

 @retval EFI_SUCCESS   Synchronize data successful.
 @retval Other         Set "BootOrder" varible failed.
**/
EFI_STATUS
SyncBootOrder (
  VOID
  )
{
  UINT16                        *BootOrder;
  UINTN                         BootOrderSize;
  UINT16                        *PhysicalBootOrder;
  UINTN                         PhysicalBootOrderSize;
  UINT16                        *WorkingBootOrder;
  UINTN                         WorkingBootOrderNum;
  EFI_STATUS                    Status;
  BOOLEAN                       IsPhysicalBootOrder;
  UINTN                         Index;
  UINTN                         BootOrderNum;
  UINTN                         PhysicalBootOrderNum;
  UINTN                         WorkingBootOrderSize;
  UINT16                        *OptionList;
  UINTN                         OptionNum;
  UINT16                        *VirtualBootOrder;
  UINTN                         VirtualBootOrderSize;
  UINT16                        *NewVirtualBootOrder;
  UINTN                         NewVirtualBootOrderSize;

  if (!FeaturePcdGet (PcdAutoCreateDummyBootOption)) {
    return EFI_SUCCESS;
  }
  if (BdsLibIsBootOrderHookEnabled ()) {
    return EFI_SUCCESS;
  }

  BootOrder = BdsLibGetVariableAndSize (
                L"BootOrder",
                &gEfiGlobalVariableGuid,
                &BootOrderSize
                );
  if (BootOrder == NULL) {
    BootOrderSize       = 0;
    BootOrderNum        = 0;
    IsPhysicalBootOrder = TRUE;
  } else {
    BootOrderNum = BootOrderSize / sizeof (UINT16);
    Status = BdsLibGetBootOrderType (BootOrder, BootOrderNum, &IsPhysicalBootOrder);
    if (EFI_ERROR (Status)) {
      FreePool (BootOrder);
      return Status;
    }
  }

  if (IsPhysicalBootOrder) {
    Status = gRT->SetVariable (
                    PHYSICAL_BOOT_ORDER_NAME,
                    &gEfiGenericVariableGuid,
                    EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                    BootOrderSize,
                    BootOrder
                    );
    //
    // Sync BootOrder to VirtualBootOrder
    //
    VirtualBootOrder = BdsLibGetVariableAndSize (VIRTUAL_BOOT_ORDER_VARIABLE_NAME, &gEfiGenericVariableGuid, &VirtualBootOrderSize);
    if (VirtualBootOrder != NULL) {
      Status = ConvertToVirtualBootOrder (
                 BootOrder,
                 BootOrderNum,
                 VirtualBootOrder,
                 VirtualBootOrderSize / sizeof (UINT16),
                 &NewVirtualBootOrder,
                 &NewVirtualBootOrderSize
                 );
      if (!EFI_ERROR (Status)) {
        gRT->SetVariable (
               VIRTUAL_BOOT_ORDER_VARIABLE_NAME,
               &gEfiGenericVariableGuid,
               EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
               NewVirtualBootOrderSize,
               NewVirtualBootOrder
               );
        if (NewVirtualBootOrder != NULL) {
          FreePool (NewVirtualBootOrder);
        }
      }
      FreePool (VirtualBootOrder);
    }
  } else {
    PhysicalBootOrder = BdsLibGetVariableAndSize (
                          PHYSICAL_BOOT_ORDER_NAME,
                          &gEfiGenericVariableGuid,
                          &PhysicalBootOrderSize
                          );
    PhysicalBootOrderNum = PhysicalBootOrderSize / sizeof (UINT16);

    WorkingBootOrderSize = PhysicalBootOrderSize + BootOrderSize;
    WorkingBootOrder = WorkingBootOrderSize == 0 ? NULL : AllocateZeroPool (WorkingBootOrderSize);
    WorkingBootOrderNum = 0;

    //
    // Based on the priority of BootOrder variable with dummy boot options,
    // sync the priority of physical boot order into working boot order.
    //
    for (Index = 0; Index < BootOrderNum; Index++) {
      if (BdsLibIsDummyBootOption (BootOrder[Index])) {
        if (PhysicalBootOrder == NULL || PhysicalBootOrderNum == 0) {
          continue;
        }

        if (BdsLibGetDummyBootOptionNum (PhysicalBootOrder[0]) == BootOrder[Index]) {
          OptionList = GetFirstAdjacentOptionsByDummyOptionNum (
                         BootOrder[Index],
                         PhysicalBootOrder,
                         PhysicalBootOrderNum,
                         &OptionNum
                         );
        } else {
          OptionList = GetAllOptionsByDummyOptionNum (
                         BootOrder[Index],
                         PhysicalBootOrder,
                         PhysicalBootOrderNum,
                         &OptionNum
                         );
        }

        if (OptionList != NULL) {
          RemoveOptionsInBootOrder (
            OptionList,
            OptionNum,
            PhysicalBootOrder,
            &PhysicalBootOrderNum
            );
          RemoveOptionsInBootOrder (
            BootOrder,
            BootOrderNum,
            OptionList,
            &OptionNum
            );
          AddOptionsToBootOrder (
            OptionList,
            OptionNum,
            WorkingBootOrderSize,
            WorkingBootOrder,
            &WorkingBootOrderNum
            );
          FreePool (OptionList);
        }
      } else {
        AddOptionsToBootOrder (
          &BootOrder[Index],
          1,
          WorkingBootOrderSize,
          WorkingBootOrder,
          &WorkingBootOrderNum
          );
        RemoveOptionsInBootOrder (
          &BootOrder[Index],
          1,
          PhysicalBootOrder,
          &PhysicalBootOrderNum
          );
      }
    }

    if (WorkingBootOrder != NULL && PhysicalBootOrder != NULL) {
      CopyMem (&WorkingBootOrder[WorkingBootOrderNum], PhysicalBootOrder, PhysicalBootOrderNum * sizeof(UINT16));
    }
    WorkingBootOrderNum += PhysicalBootOrderNum;
    Status = gRT->SetVariable (
                    PHYSICAL_BOOT_ORDER_NAME,
                    &gEfiGenericVariableGuid,
                    EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                    WorkingBootOrderNum * sizeof (UINT16),
                    WorkingBootOrder
                    );
    if (PhysicalBootOrder != NULL) {
      FreePool (PhysicalBootOrder);
    }
    if (WorkingBootOrder != NULL) {
      FreePool (WorkingBootOrder);
    }

    gRT->SetVariable (
           VIRTUAL_BOOT_ORDER_VARIABLE_NAME,
           &gEfiGenericVariableGuid,
           EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
           0,
           NULL
           );
  }

  if (BootOrder != NULL) {
    FreePool (BootOrder);
  }

  return EFI_SUCCESS;
}

/**
 Internal function to uninstall all of specific protocols in all of handles.

 @param[in] Protocol    Provides the protocol to remove.

 @retval EFI_SUCCESS    Uninstall all of protcols successfully.
 @return Other          Any other error occurred while uninstalling protocols.
**/
STATIC
EFI_STATUS
UninstallSpecificProtocols (
  IN EFI_GUID                   *Protocol
  )
{
  EFI_STATUS                    Status;
  UINTN                         NoHandles;
  EFI_HANDLE                    *Buffer;
  UINTN                         Index;

  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  Protocol,
                  NULL,
                  &NoHandles,
                  &Buffer
                  );
  if (!EFI_ERROR (Status)) {
    for (Index = 0; Index < NoHandles; Index++) {
      Status = gBS->UninstallProtocolInterface (
                      Buffer[Index],
                      Protocol,
                      NULL
                      );
      if (EFI_ERROR (Status)) {
        FreePool (Buffer);
        return Status;
      }
    }
    FreePool (Buffer);
  }

  return EFI_SUCCESS;
}

/**
 This function uses to remove all of physical boot options and add virtual boot
 options to "BootOrder' variable

 @retval EFI_SUCCESS            Change the contents of "BootOrder" successful
 @retval EFI_NOT_FOUND          "BootOrder" variable doesn't exist.
 @retval Other                  Set "BootOrder" variable failed.
**/
EFI_STATUS
BdsLibChangeToVirtualBootOrder (
  VOID
  )
{
  UINT16                        *BootOrder;
  UINTN                         BootOrderSize;
  UINTN                         BootOrderNum;
  UINT16                        *VirtualBootOrder;
  UINTN                         VirtualBootOrderSize;
  EFI_STATUS                    Status;
  BOOLEAN                       IsPhysicalBootOrder;
  UINT16                        *OrgVirtualBootOrder;
  UINTN                         OrgVirtualBootOrderNum;

  if (!FeaturePcdGet (PcdAutoCreateDummyBootOption)) {
    return EFI_SUCCESS;
  }
  BootOrder = BdsLibGetVariableAndSize (
                L"BootOrder",
                &gEfiGlobalVariableGuid,
                &BootOrderSize
                );

  BdsLibDisableBootOrderHook ();
  if (BootOrder == NULL) {
    return EFI_NOT_FOUND;
  }

  Status = GetVirtualBootOrder (&OrgVirtualBootOrderNum, &OrgVirtualBootOrder);
  if (EFI_ERROR (Status)) {
    OrgVirtualBootOrder    = NULL;
    OrgVirtualBootOrderNum = 0;
  }

  BootOrderNum = BootOrderSize / sizeof (UINT16);
  Status = BdsLibGetBootOrderType (BootOrder, BootOrderNum, &IsPhysicalBootOrder);
  if (!EFI_ERROR (Status) && IsPhysicalBootOrder) {
    Status = ConvertToVirtualBootOrder (
               BootOrder,
               BootOrderNum,
               OrgVirtualBootOrder,
               OrgVirtualBootOrderNum,
               &VirtualBootOrder,
               &VirtualBootOrderSize
               );
    if (EFI_ERROR (Status)) {
      return Status;
    }

    Status = gRT->SetVariable (
                    L"BootOrder",
                    &gEfiGlobalVariableGuid,
                    EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                    VirtualBootOrderSize,
                    VirtualBootOrder
                    );
    if (VirtualBootOrder != NULL) {
      FreePool (VirtualBootOrder);
    }
  }

  FreePool (BootOrder);
  if (OrgVirtualBootOrder != NULL) {
    FreePool (OrgVirtualBootOrder);
    gRT->SetVariable (
           VIRTUAL_BOOT_ORDER_VARIABLE_NAME,
           &gEfiGenericVariableGuid,
           EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
           0,
           NULL
           );
  }
  return EFI_SUCCESS;
}

/**
 Disable BootOrder variable hook mechanism.

 @retval EFI_SUCCESS    Disable BootOrder variable hook mechanism successfully.
 @return Other          Disable BootOrder variable hook mechanism failed.
**/
EFI_STATUS
BdsLibDisableBootOrderHook (
  VOID
  )
{
  EFI_STATUS          Status;
  EFI_HANDLE          Handle;
  VOID                *Interface;

  if (!FeaturePcdGet (PcdAutoCreateDummyBootOption)) {
    return EFI_SUCCESS;
  }

  Status = UninstallSpecificProtocols (&gBootOrderHookEnableGuid);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = gBS->LocateProtocol (
                  &gBootOrderHookDisableGuid,
                  NULL,
                  (VOID **)&Interface
                  );
  if (!EFI_ERROR (Status)) {
    return EFI_SUCCESS;
  }

  Handle = NULL;
  return gBS->InstallProtocolInterface (
                &Handle,
                &gBootOrderHookDisableGuid,
                EFI_NATIVE_INTERFACE,
                NULL
                );
}

/**
 Enable BootOrder variable hook mechanism.

 @retval EFI_SUCCESS    Enable BootOrder variable hook mechanism successfully.
 @return Other          Enable BootOrder variable hook mechanism failed.
**/
EFI_STATUS
BdsLibEnableBootOrderHook (
  VOID
  )
{
  EFI_STATUS                    Status;
  EFI_HANDLE                    Handle;
  VOID                          *Interface;

  if (!FeaturePcdGet (PcdAutoCreateDummyBootOption)) {
    return EFI_SUCCESS;
  }

  Status = UninstallSpecificProtocols (&gBootOrderHookDisableGuid);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = gBS->LocateProtocol (
                  &gBootOrderHookEnableGuid,
                  NULL,
                  (VOID **)&Interface
                  );
  if (!EFI_ERROR (Status)) {
    return EFI_SUCCESS;
  }

  Handle = NULL;
  return gBS->InstallProtocolInterface (
                &Handle,
                &gBootOrderHookEnableGuid,
                EFI_NATIVE_INTERFACE,
                NULL
                );
}

/**
 Function uses to check BootOrder variable hook mechanism is whether enabled.

 @retval TRUE    BootOrder variable hook mechanism is enabled.
 @retval FALSE   BootOrder variable hook mechanism is disabled
**/
BOOLEAN
BdsLibIsBootOrderHookEnabled (
  VOID
  )
{
  EFI_STATUS                    Status;
  VOID                          *BootOrderHook;

  Status = gBS->LocateProtocol (
                  &gBootOrderHookEnableGuid,
                  NULL,
                  (VOID **)&BootOrderHook
                  );
  if (!EFI_ERROR (Status)) {
    return TRUE;
  }

  return FALSE;
}

/**
 According to input boot order to get the boot order type.

 @param[in]  BootOrder          Pointer to BootOrder array.
 @param[in]  BootOrderNum       The boot order number in boot order array.
 @param[out] PhysicalBootOrder  BOOLEAN value to indicate is whether physical boot order.

 @retval EFI_INVALID_PARAMETER  BootOrder is NULL, BootOrderNum is 0 or PhysicalBootOrder is NULL.
 @retval EFI_SUCCESS            Determine the boot order is physical boot order or virtual boot order successful.
**/
EFI_STATUS
BdsLibGetBootOrderType (
  IN  UINT16                    *BootOrder,
  IN  UINTN                     BootOrderNum,
  OUT BOOLEAN                   *PhysicalBootOrder
  )
{
  UINTN                         Index;

  if (BootOrder == NULL || BootOrderNum == 0 || PhysicalBootOrder == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  *PhysicalBootOrder = TRUE;
  for (Index = 0; Index < BootOrderNum; Index++) {
    if (BdsLibIsDummyBootOption (BootOrder[Index])) {
      *PhysicalBootOrder = FALSE;
      break;
    }
  }

  return EFI_SUCCESS;
}

/**
 Based on the boot option number, return the dummy boot option number.
 If return DummyBootOptionEndNum, it means this boot option does not belong to dummy boot option.

 @param[in] OptionNum           The boot option number.

 @return Dummy boot option number or DummyBootOptionEndNum if input boot option does not belong to dummy boot option.
**/
DUMMY_BOOT_OPTION_NUM
BdsLibGetDummyBootOptionNum (
  IN UINT16                     OptionNum
  )
{
  UINTN                         Index;

  for (Index = 0; Index < OPTION_TYPE_TABLE_COUNT; Index++) {
    if (mOptionTypeTable[Index].CheckOptionType (OptionNum)) {
      return mOptionTypeTable[Index].OptionNum;
    }
  }

  return DummyBootOptionEndNum;
}

/**
  This function uses to get all of corresponding mapping boot option numbers from "BootOrder" variable.

  @param[in]  OptionNum         Input option number
  @param[in]  OptionCount       Boot option count.
  @param[out] OptionOrder       Double pointer to option order to saved all of UEFI USB boot option.

  @retval EFI_SUCCESS           Get UEFI USB boot option from "BootOrder" variable successful
  @retval EFI_INVALID_PARAMETER OptionCount, OptionOrder is NULL or OptionNum is not a dummy boot option.
  @retval EFI_NOT_FOUND         Cannot find corresponding option type check function.
**/
EFI_STATUS
BdsLibGetMappingBootOptions (
  IN  UINT16                    OptionNum,
  OUT UINTN                     *OptionCount,
  OUT UINT16                    **OptionOrder
  )
{
  UINTN                         BootOrderSize;
  UINT16                        *BootOrder;
  UINT16                        *MappingBootOrder;
  UINTN                         MappingCount;
  UINTN                         Index;
  UINTN                         TableIndex;

  if (OptionCount == NULL || OptionOrder == NULL || !BdsLibIsDummyBootOption (OptionNum)) {
    return EFI_INVALID_PARAMETER;
  }

  BootOrder = BdsLibGetVariableAndSize (
                L"BootOrder",
                &gEfiGlobalVariableGuid,
                &BootOrderSize
                );
  if (BootOrder == NULL) {
    *OptionCount = 0;
    *OptionOrder = NULL;
  } else {
    //
    // Find corresponding function to check boot option
    //
    for (TableIndex = 0; TableIndex < OPTION_TYPE_TABLE_COUNT; TableIndex++) {
      if (mOptionTypeTable[TableIndex].OptionNum == OptionNum) {
        break;
      }
    }
    ASSERT (TableIndex < OPTION_TYPE_TABLE_COUNT);
    if (TableIndex == OPTION_TYPE_TABLE_COUNT) {
      FreePool (BootOrder);
      return EFI_NOT_FOUND;
    }
    //
    // According to check function to get boot option count and boot option order.
    //
    MappingCount = 0;
    MappingBootOrder = AllocateZeroPool (BootOrderSize);
    if (MappingBootOrder == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }

    for (Index = 0; Index < BootOrderSize / sizeof (UINT16); Index++) {
      if (mOptionTypeTable[TableIndex].CheckOptionType (BootOrder[Index])) {
        MappingBootOrder[MappingCount] = BootOrder[Index];
        MappingCount++;
      }
    }
    FreePool (BootOrder);
    *OptionCount = MappingCount;
    *OptionOrder = MappingBootOrder;
  }
  return EFI_SUCCESS;
}

/**
 According to option number to check this boot option is whether dummy boot option.

 @param[in] OptionNum           UINT16 to save boot option number.

 @retval TRUE           This boot option is dummy boot option.
 @retval FALSE          This boot option isn't dummy boot option.
**/
BOOLEAN
BdsLibIsDummyBootOption (
  IN UINT16                     OptionNum
  )
{
  if (OptionNum > DummyBootOptionStartNum && OptionNum < DummyBootOptionEndNum) {
    return TRUE;
  }

  return FALSE;
}

/**
 This function restores the contents of PHYSICAL_BOOT_ORDER_NAME variable to "BootOrder" variable.

 @retval EFI_SUCCESS      Restore the contents of "BootOrder" variable successful.
 @retval Other            Any error occurred while restoring data to "BootOrder" variable.
**/
EFI_STATUS
BdsLibRestoreBootOrderFromPhysicalBootOrder (
  VOID
  )
{
  EFI_STATUS                    Status;
  UINT16                        *PhysicalBootOrder;
  UINTN                         PhysicalBootOrderSize;
  UINT16                        *BootOrder;
  UINTN                         BootOrderSize;
  BOOLEAN                       IsPhysicalBootOrder;

  if (!FeaturePcdGet (PcdAutoCreateDummyBootOption)) {
    return EFI_SUCCESS;
  }
  BdsLibDisableBootOrderHook ();

  //
  // If current boot order is virtual boot order, save it to keep boot priority before deletion.
  //
  BootOrder = BdsLibGetVariableAndSize (
                L"BootOrder",
                &gEfiGlobalVariableGuid,
                &BootOrderSize
                );
  if (BootOrder != NULL) {
    Status = BdsLibGetBootOrderType (BootOrder, BootOrderSize / sizeof (UINT16), &IsPhysicalBootOrder);
    if (!EFI_ERROR (Status) && !IsPhysicalBootOrder) {
      gRT->SetVariable (
             VIRTUAL_BOOT_ORDER_VARIABLE_NAME,
             &gEfiGenericVariableGuid,
             EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
             BootOrderSize,
             BootOrder
             );
    }
    FreePool (BootOrder);
  }

  PhysicalBootOrder = BdsLibGetVariableAndSize (
                        PHYSICAL_BOOT_ORDER_NAME,
                        &gEfiGenericVariableGuid,
                        &PhysicalBootOrderSize
                        );

  Status = gRT->SetVariable (
                  L"BootOrder",
                  &gEfiGlobalVariableGuid,
                  EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                  PhysicalBootOrderSize,
                  PhysicalBootOrder
                  );
  if (PhysicalBootOrder != NULL) {
    FreePool (PhysicalBootOrder);
  }

  return Status;
}

/**
 This function uses to synchronize the contents of PHYSICAL_BOOT_ORDER_NAME variable with
 the contents of "BootOrder" variable.

 @retval EFI_SUCCESS      Synchronize PHYSICAL_BOOT_ORDER_NAME variable successful.
 @retval Other            Any error occurred while Synchronizing PHYSICAL_BOOT_ORDER_NAME variable.
**/
EFI_STATUS
BdsLibSyncPhysicalBootOrder (
  VOID
  )
{
  EFI_STATUS                    Status;
  UINT16                        *BootOrder;
  UINTN                         BootOrderSize;
  BOOLEAN                       IsPhysicalBootOrder;

  if (!FeaturePcdGet (PcdAutoCreateDummyBootOption)) {
    return EFI_SUCCESS;
  }
  if (BdsLibIsBootOrderHookEnabled ()) {
    return EFI_SUCCESS;
  }

  BootOrder = BdsLibGetVariableAndSize (L"BootOrder", &gEfiGlobalVariableGuid, &BootOrderSize);
  if (BootOrder == NULL) {
    return gRT->SetVariable (
                  PHYSICAL_BOOT_ORDER_NAME,
                  &gEfiGenericVariableGuid,
                  EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                  0,
                  NULL
                  );
  }

  Status = BdsLibGetBootOrderType (BootOrder, BootOrderSize / sizeof (UINT16), &IsPhysicalBootOrder);
  if (!EFI_ERROR (Status) && IsPhysicalBootOrder) {
    Status = gRT->SetVariable (
                    PHYSICAL_BOOT_ORDER_NAME,
                    &gEfiGenericVariableGuid,
                    EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                    BootOrderSize,
                    BootOrder
                    );
  }

  FreePool (BootOrder);
  return Status;
}

