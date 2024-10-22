/** @file
  Provide H2O BDS service protocol interface

;******************************************************************************
;* Copyright (c) 2014 - 2021, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/


#include "Bds.h"
#include "String.h"

#define SHELL_ENVIRONMENT_INTERFACE_PROTOCOL \
  { 0x47c7b221, 0xc42a, 0x11d2, 0x8e, 0x57, 0x00, 0xa0, 0xc9, 0x69, 0x72, 0x3b }

#define OPTION_VAR_NAME_BUFFER_CHAR_COUNT   21
#define OPTION_VAR_NAME_FORMAT_STR          L"%s%04x"

#define VAR_ATTR_BS_RT        (EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS)
#define VAR_ATTR_BS_RT_NV     (VAR_ATTR_BS_RT | EFI_VARIABLE_NON_VOLATILE)
#define VAR_ATTR_BS_RT_NV_AT  (VAR_ATTR_BS_RT_NV | EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS)

typedef struct _H2O_BDS_OPTION_TYPE_INFO {
  UINT8                       OptionType;
  LIST_ENTRY                  OptionList;
  CHAR16                      *OrderVarName;
  CHAR16                      *OptionVarPrefixName;
  UINT32                      VarAttribute;
} H2O_BDS_OPTION_TYPE_INFO;

H2O_BDS_OPTION_TYPE_INFO  mOptionTypeInfo[] = {
  {H2O_BDS_LOAD_OPTION_TYPE_DRIVER           , {NULL, NULL}, EFI_DRIVER_ORDER_VARIABLE_NAME  , L"Driver"          , VAR_ATTR_BS_RT_NV   },
  {H2O_BDS_LOAD_OPTION_TYPE_BOOT             , {NULL, NULL}, EFI_BOOT_ORDER_VARIABLE_NAME    , L"Boot"            , VAR_ATTR_BS_RT_NV   },
  {H2O_BDS_LOAD_OPTION_TYPE_SYSPREP          , {NULL, NULL}, EFI_SYS_PREP_ORDER_VARIABLE_NAME, L"SysPrep"         , VAR_ATTR_BS_RT_NV   },
  {H2O_BDS_LOAD_OPTION_TYPE_OS_RECOVERY      , {NULL, NULL}, L"OsRecoveryOrder"              , L"OsRecovery"      , VAR_ATTR_BS_RT_NV_AT},
  {H2O_BDS_LOAD_OPTION_TYPE_PLATFORM_RECOVERY, {NULL, NULL}, NULL                            , L"PlatformRecovery", VAR_ATTR_BS_RT      },
  };

typedef struct _H2O_BDS_ORDER_VAR_INFO {
  UINT16                      OptionCount;
  UINT16                      *OptionOrder;
  EFI_GUID                    *OptionVarGuid;
} H2O_BDS_ORDER_VAR_INFO;

STATIC   VOID             *mShellEnvProtocolCallbackReg;
STATIC   BOOLEAN          mNeedSyncBootOrder = TRUE;
STATIC   EFI_GUID         mShellEnvProtocol = SHELL_ENVIRONMENT_INTERFACE_PROTOCOL;

H2O_BDS_SERVICES_PROTOCOL  mH2OBdsServices = {
  sizeof (H2O_BDS_SERVICES_PROTOCOL),
  BdsServicesGetOsIndications,
  BdsServicesGetTimeout,
  BdsServicesGetBootMode,
  BdsServicesGetBootType,
  NULL,
  NULL,
  NULL,
  BdsServicesGetBootList,
  BdsServicesGetDriverList,
  BdsServicesCreateLoadOption,
  BdsServicesFreeLoadOption,
  BdsServicesConvertVarToLoadOption,
  BdsServicesConvertLoadOptionToVar,
  BdsServicesInsertLoadOption,
  BdsServicesRemoveLoadOption,
  BdsServicesExpandLoadOption,
  BdsServicesLaunchLoadOption,
  BdsServicesEnableHotKeys,
  BdsServicesRegisterCallbackHotKey,
  BdsServicesRegisterLoadOptionHotKey,
  BdsServicesUnregisterHotKey,
  BdsServicesGetBootDisplayMode,
  BdsServicesSetHotKeyDescText,
  BdsServicesSetHotKeyDescPosn,
  BdsServicesSetHotKeyDescColor,
  BdsServicesSetHotKeyDescFont,
  BdsServicesGetSysPrepList,
  BdsServicesGetOsRecoveryList,
  BdsServicesGetPlatformRecoveryList,
  BdsServicesCreateLoadOption2,
  BdsServicesInsertLoadOption2,
  BdsServicesRemoveLoadOption2
};

/**
  Frees pool.

  @param  Buffer                 The allocated pool entry to free.
**/
STATIC
VOID
InternalFreePool (
  IN VOID        *Buffer
  )
{
  if (Buffer != NULL) {
    FreePool (Buffer);
  }
}

/**
  Internal function to free all of allocated memory in specific H2O_BDS_LOAD_OPTION.

  @param[in]  LoadOption                 The allocated load option to free.
**/
STATIC
VOID
InternalFreeBdsLoadOption (
  IN   H2O_BDS_LOAD_OPTION         *LoadOption
  )
{
  if (LoadOption == NULL) {
    return;
  }
  InternalFreePool (LoadOption->DevicePath);
  InternalFreePool (LoadOption->Description);
  InternalFreePool (LoadOption->LoadOptionalData);
  InternalFreePool (LoadOption->StatusString);
  FreePool (LoadOption);
}

/**
  Internal function to check the input str is whether a correct #### format. Here the #### is replaced
  by a unique option number in printable hexadecimal representation using the digits 0-9, and the upper
  case versions of the characters A-F (0000-FFFF).

  @param[in] Str   Input #### string to be checked.

  @retval TRUE     The input string is correct #### format.
  @retval FALSE    The input string is incorrect #### format.
**/
STATIC
BOOLEAN
IsValidOptionNumber (
  IN CONST   CHAR16       *Str
  )
{
  UINTN         Index;

  if (Str == NULL || StrLen (Str) != 4) {
    return FALSE;
  }

  for (Index = 0; Str[Index] != 0; Index++) {
    if (!((Str[Index] >= L'0' && Str[Index] <= L'9') || (Str[Index] >= L'A' && Str[Index] <= L'F'))) {
      return FALSE;
    }
  }
  return TRUE;
}

/**
  Check if the BDS load option is a valid.

  @param[in] BdsLoadOption   Pointer to BDS load option.

  @retval TRUE           The input BDS load option is valid.
  @retval FALSE          The input BDS load option is not valid.
**/
STATIC
BOOLEAN
IsValidLoadOption (
  IN CONST H2O_BDS_LOAD_OPTION         *BdsLoadOption
  )
{
  if (BdsLoadOption == NULL ||
      BdsLoadOption->Signature  != H2O_BDS_LOAD_OPTION_SIGNATURE ||
      BdsLoadOption->OptionType >= H2O_BDS_LOAD_OPTION_TYPE_MAX) {
    return FALSE;
  }

  return TRUE;
}

/**
  Check if the input BDS load option match with input load order and variable GUID.

  @param[in] LoadOption         Pointer to BDS load option.
  @param[in] LoadOrder          Load order number.
  @param[in] LoadOrderVarGuid   Pointer to variable GUID.

  @retval TRUE   The input BDS load option match with input load order and variable GUID.
  @retval FALSE  The input BDS load option does not match with input load order and variable GUID or input parameter is NULL.
**/
STATIC
BOOLEAN
IsLoadOptionMatch (
  IN CONST H2O_BDS_LOAD_OPTION        *LoadOption,
  IN       UINT16                     LoadOrder,
  IN CONST EFI_GUID                   *LoadOrderVarGuid
  )
{
  if (LoadOption == NULL || LoadOrderVarGuid == NULL) {
    return FALSE;
  }

  if (LoadOption->LoadOrder == LoadOrder &&
      CompareGuid (&LoadOption->LoadOrderVarGuid, LoadOrderVarGuid)) {
    return TRUE;
  }

  return FALSE;
}

/**
  Get BDS load option type info.

  @param[in] OptionType       BDS load option type

  @return A pointer to the BDS load option type info or NULL if not found.
**/
STATIC
H2O_BDS_OPTION_TYPE_INFO *
GetOptionTypeInfo (
  IN UINT8                             OptionType
  )
{
  UINTN                                Index;

  for (Index = 0; Index < sizeof (mOptionTypeInfo) / sizeof (H2O_BDS_OPTION_TYPE_INFO); Index++) {
    if (mOptionTypeInfo[Index].OptionType == OptionType) {
      return &mOptionTypeInfo[Index];
    }
  }

  return NULL;
}

/**
  Internal function to calculate UINT16 value from input #### string.

  @param[in] Str   Input #### string to be calculated.

  @return The UINT16 value which calculate from input string.
**/
STATIC
UINT16
GetOptionNumber (
  IN CONST   CHAR16       *Str
  )
{
  UINTN         Index;
  UINT16        Number;

  if (Str == NULL || StrLen (Str) != 4) {
    return 0;
  }

  Number = 0;
  for (Index = 0; Str[Index] != 0; Index++) {
    if (Str[Index] >= L'0' && Str[Index] <= L'9') {
      Number = Number * 0x10 + Str[Index] - L'0';
    } else if (Str[Index] >= L'A' && Str[Index] <= L'F') {
      Number = Number * 0x10 + Str[Index] - L'A' + 0x0A;
    } else {
      return 0;
    }
  }
  return Number;
}

/**
  Enumerate all option variable which variable name match the input option prefix name.
  The output option number list is organized in order.

  @param[in]  OptionPrefixName   A pointer to option variable prefix name.
  @param[out] OptionCount        A pointer to output option count.
  @param[out] OptionNumList      A pointer to output option number list.
  @param[out] OptionGuidList     A pointer to output option variable GUID list.

  @retval EFI_SUCCESS            Enumerate all option variable successfully.
  @retval EFI_INVALID_PARAMETER  Input parameter is NULL.
  @retval EFI_OUT_OF_RESOURCES   Allocate memory failed.
  @retval EFI_NOT_FOUND          There is no option variable which name match the input prefix name.
**/
STATIC
EFI_STATUS
EnumerateAllOptionVar (
  IN CONST CHAR16             *OptionPrefixName,
  OUT      UINT16             *OptionCount,
  OUT      UINT16             **OptionNumList,
  OUT      EFI_GUID           **OptionGuidList
  )
{
  UINTN                       OptionPrefixNameLen;
  CHAR16                      *VarName;
  UINTN                       VarNameSize;
  UINTN                       VarNameBufSize;
  UINTN                       VarNameLen;
  UINT16                      *NumList;
  EFI_GUID                    *GuidList;
  EFI_GUID                    Guid;
  UINT16                      Count;
  EFI_STATUS                  Status;
  UINT16                      OptionNum;
  UINT16                      Index;


  if (OptionPrefixName == NULL || OptionCount == NULL || OptionNumList == NULL || OptionGuidList == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  VarNameBufSize = 50 * sizeof (CHAR16);
  VarName        = AllocateZeroPool (VarNameBufSize);
  if (VarName == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  OptionPrefixNameLen = StrLen (OptionPrefixName);

  //
  // Enumerate all option variables which variable name matches the input option prefix name.
  //
  Count    = 0;
  NumList  = NULL;
  GuidList = NULL;
  while (TRUE) {
    VarNameSize = VarNameBufSize;
    Status      = gRT->GetNextVariableName (&VarNameSize, VarName, &Guid);
    if (Status == EFI_BUFFER_TOO_SMALL) {
      VarName = ReallocatePool (VarNameBufSize, VarNameSize, VarName);
      if (VarName == NULL) {
        return EFI_OUT_OF_RESOURCES;
      }
      VarNameBufSize = VarNameSize;
      Status = gRT->GetNextVariableName (&VarNameSize, VarName, &Guid);
    }
    if (EFI_ERROR (Status)) {
      break;
    }

    VarNameLen = (VarNameSize <= sizeof (CHAR16)) ? 0 : (VarNameSize / sizeof (CHAR16) - 1);

    if (VarNameLen != OptionPrefixNameLen + 4 ||
        StrnCmp (VarName, OptionPrefixName, OptionPrefixNameLen) != 0 ||
        !IsValidOptionNumber (&VarName[OptionPrefixNameLen])) {
      continue;
    }

    //
    // Add option number into list in numeric order.
    //
    OptionNum = GetOptionNumber (&VarName[OptionPrefixNameLen]);

    for (Index = 0; Index < Count; Index++) {
      if (NumList[Index] > OptionNum) {
        break;
      }
    }

    NumList  = ReallocatePool (Count * sizeof (UINT16)  , (Count + 1) * sizeof (UINT16)  , NumList);
    GuidList = ReallocatePool (Count * sizeof (EFI_GUID), (Count + 1) * sizeof (EFI_GUID), GuidList);
    if (NumList == NULL || GuidList == NULL) {
      FreePool (VarName);
      return EFI_OUT_OF_RESOURCES;
    }

    CopyMem (&NumList[Index + 1] , &NumList[Index] , (Count - Index) * sizeof (UINT16));
    CopyMem (&GuidList[Index + 1], &GuidList[Index], (Count - Index) * sizeof (EFI_GUID));
    NumList[Index] = OptionNum;
    CopyGuid (&GuidList[Index], &Guid);
    Count++;
  }
  FreePool (VarName);

  if (Count == 0) {
    return EFI_NOT_FOUND;
  }

  *OptionCount    = Count;
  *OptionNumList  = NumList;
  *OptionGuidList = GuidList;
  return EFI_SUCCESS;
}

/**
  Set OS recovery order variable.

  @param[in] OrderVarInfo        A pointer to order variable info.

  @retval EFI_SUCCESS            Set OS recovery order variable successfully.
  @retval EFI_INVALID_PARAMETER  OrderVarInfo is NULL.
  @retval EFI_UNSUPPORTED        Failed to find OS recovery info.
  @retval Other                  Failed to set variable.
**/
STATIC
EFI_STATUS
OsRecoverySetOrderVar (
  IN CONST H2O_BDS_ORDER_VAR_INFO      *OrderVarInfo
  )
{
  H2O_BDS_OPTION_TYPE_INFO             *OptionTypeInfo;
  UINTN                                OptionIndex;
  UINTN                                Index;
  EFI_GUID                             *OptionVarGuid;
  EFI_GUID                             *VendorGuidOrder;
  UINTN                                VendorGuidOrderCount;
  EFI_STATUS                           Status;

  if (OrderVarInfo == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  OptionTypeInfo = GetOptionTypeInfo (H2O_BDS_LOAD_OPTION_TYPE_OS_RECOVERY);
  if (OptionTypeInfo == NULL) {
    return EFI_UNSUPPORTED;
  }

  VendorGuidOrder = OrderVarInfo->OptionCount == 0 ? NULL : AllocateZeroPool (OrderVarInfo->OptionCount * sizeof (EFI_GUID));
  if (VendorGuidOrder == NULL && OrderVarInfo->OptionCount != 0) {
    return EFI_OUT_OF_RESOURCES;
  }

  VendorGuidOrderCount = 0;
  for (OptionIndex = 0; OptionIndex < OrderVarInfo->OptionCount; OptionIndex++) {
    OptionVarGuid = &OrderVarInfo->OptionVarGuid[OptionIndex];

    for (Index = 0; Index < VendorGuidOrderCount; Index++) {
      if (CompareGuid (&VendorGuidOrder[Index], OptionVarGuid)) {
        break;
      }
    }
    if (Index != VendorGuidOrderCount) {
      continue;
    }

    CopyGuid (&VendorGuidOrder[VendorGuidOrderCount], OptionVarGuid);
    VendorGuidOrderCount++;
  }

  Status = CommonSetVariable (
             OptionTypeInfo->OrderVarName,
             &gEfiGlobalVariableGuid,
             OptionTypeInfo->VarAttribute,
             sizeof (EFI_GUID) * VendorGuidOrderCount,
             VendorGuidOrder
             );

  InternalFreePool (VendorGuidOrder);
  return Status;
}

/**
  Get the order variable info of OS recovery option.

  @param[out] OrderVarInfo       A pointer to order variable info.

  @retval EFI_SUCCESS            Get the order variable info of OS recovery option successfully.
  @retval EFI_INVALID_PARAMETER  Input parameter is NULL.
  @retval EFI_OUT_OF_RESOURCES   Allocate memory failed.
  @retval EFI_UNSUPPORTED        Fail to find OS recovery info.
**/
STATIC
EFI_STATUS
OsRecoveryGetOrderVarInfo (
  OUT H2O_BDS_ORDER_VAR_INFO  *OrderVarInfo
  )
{
  EFI_STATUS                  Status;
  UINT16                      OptionCount;
  UINT16                      *OptionNumList;
  EFI_GUID                    *OptionGuidList;
  UINT16                      Index;
  UINT16                      Count;
  UINT16                      *Order;
  EFI_GUID                    *GuidList;
  UINT16                      GuidIndex;
  EFI_GUID                    *VendorGuidOrder;
  UINTN                       VendorGuidOrderSize;
  UINTN                       VendorGuidCount;
  H2O_BDS_OPTION_TYPE_INFO    *OptionTypeInfo;

  if (OrderVarInfo == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  OptionTypeInfo = GetOptionTypeInfo (H2O_BDS_LOAD_OPTION_TYPE_OS_RECOVERY);
  if (OptionTypeInfo == NULL) {
    return EFI_UNSUPPORTED;
  }

  Count    = 0;
  Order    = NULL;
  GuidList = NULL;

  VendorGuidOrder = NULL;
  Status = CommonGetVariableDataAndSize (
             OptionTypeInfo->OrderVarName,
             &gEfiGlobalVariableGuid,
             &VendorGuidOrderSize,
             (VOID **) &VendorGuidOrder
             );
  if (EFI_ERROR (Status)) {
    goto Done;
  }

  Status = EnumerateAllOptionVar (
             OptionTypeInfo->OptionVarPrefixName,
             &OptionCount,
             &OptionNumList,
             &OptionGuidList
             );
  if (EFI_ERROR (Status)) {
    goto Done;
  }

  //
  // Sort option number by vendor GUID order
  //
  VendorGuidCount = VendorGuidOrderSize / sizeof (EFI_GUID);

  for (GuidIndex = 0; GuidIndex < VendorGuidCount; GuidIndex++) {
    for (Index = 0; Index < OptionCount; Index++) {
      if (!CompareGuid (&OptionGuidList[Index], &VendorGuidOrder[GuidIndex])) {
        continue;
      }

      Order    = ReallocatePool (Count * sizeof (UINT16)  , (Count + 1) * sizeof (UINT16)  , Order);
      GuidList = ReallocatePool (Count * sizeof (EFI_GUID), (Count + 1) * sizeof (EFI_GUID), GuidList);
      if (Order == NULL || GuidList == NULL) {
        return EFI_OUT_OF_RESOURCES;
      }

      Order[Count] = OptionNumList[Index];
      CopyGuid (&GuidList[Count], &OptionGuidList[Index]);
      Count++;
    }
  }
  InternalFreePool (OptionNumList);
  InternalFreePool (OptionGuidList);

Done:
  InternalFreePool (VendorGuidOrder);

  OrderVarInfo->OptionCount   = Count;
  OrderVarInfo->OptionOrder   = Order;
  OrderVarInfo->OptionVarGuid = GuidList;
  return EFI_SUCCESS;
}

/**
  Get the order variable info of platform recovery option.

  @param[out] OrderVarInfo       A pointer to the order variable info.

  @retval EFI_SUCCESS            Get the order variable info of platform recovery option successfully.
  @retval EFI_INVALID_PARAMETER  Input parameter is NULL.
  @retval EFI_UNSUPPORTED        Fail to find platform recovery info.
  @retval EFI_OUT_OF_RESOURCES   Allocate memory failed.
**/
STATIC
EFI_STATUS
PlatformRecoveryGetOrderVarInfo (
  OUT H2O_BDS_ORDER_VAR_INFO  *OrderVarInfo
  )
{
  EFI_STATUS                  Status;
  UINT16                      OptionCount;
  UINT16                      *OptionNumList;
  EFI_GUID                    *OptionGuidList;
  UINT16                      Index;
  UINT16                      Count;
  UINT16                      *Order;
  EFI_GUID                    *GuidList;
  H2O_BDS_OPTION_TYPE_INFO    *OptionTypeInfo;

  if (OrderVarInfo == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  OptionTypeInfo = GetOptionTypeInfo (H2O_BDS_LOAD_OPTION_TYPE_PLATFORM_RECOVERY);
  if (OptionTypeInfo == NULL) {
    return EFI_UNSUPPORTED;
  }

  Count    = 0;
  Order    = NULL;
  GuidList = NULL;

  Status = EnumerateAllOptionVar (
             OptionTypeInfo->OptionVarPrefixName,
             &OptionCount,
             &OptionNumList,
             &OptionGuidList
             );
  if (EFI_ERROR (Status)) {
    goto Done;
  }

  for (Index = 0; Index < OptionCount; Index++) {
    if (!CompareGuid (&OptionGuidList[Index], &gEfiGlobalVariableGuid)) {
      continue;
    }

    Order    = ReallocatePool (Count * sizeof (UINT16)  , (Count + 1) * sizeof (UINT16)  , Order);
    GuidList = ReallocatePool (Count * sizeof (EFI_GUID), (Count + 1) * sizeof (EFI_GUID), GuidList);
    if (Order == NULL || GuidList == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }

    Order[Count] = OptionNumList[Index];
    CopyGuid (&GuidList[Count], &gEfiGlobalVariableGuid);
    Count++;
  }
  InternalFreePool (OptionNumList);
  InternalFreePool (OptionGuidList);

Done:
  OrderVarInfo->OptionCount   = Count;
  OrderVarInfo->OptionOrder   = Order;
  OrderVarInfo->OptionVarGuid = GuidList;
  return EFI_SUCCESS;
}

/**
  Free the order variable info.

  @param[in, out] OrderVarInfo   A pointer to the order variable info.
**/
STATIC
VOID
FreeOrderVarInfo (
  IN OUT H2O_BDS_ORDER_VAR_INFO  *OrderVarInfo
  )
{
  if (OrderVarInfo == NULL) {
    return;
  }
  InternalFreePool (OrderVarInfo->OptionOrder);
  InternalFreePool (OrderVarInfo->OptionVarGuid);
  ZeroMem (OrderVarInfo, sizeof (H2O_BDS_ORDER_VAR_INFO));
}

/**
  Get the order variable info by specific option type.

  @param[in]  OptionType         Option type value.
  @param[out] OrderVarInfo       A pointer to output order variable info.

  @retval EFI_SUCCESS            Get the order variable info successfully.
  @retval EFI_INVALID_PARAMETER  Input parameter is NULL.
  @retval EFI_UNSUPPORTED        Fail to find option type info.
  @retval Other                  Failed to get order variable info.
**/
STATIC
EFI_STATUS
GetOrderVarInfo (
  IN  UINT8                      OptionType,
  OUT H2O_BDS_ORDER_VAR_INFO     *OrderVarInfo
  )
{
  EFI_STATUS                     Status;
  UINTN                          Index;
  UINTN                          OptionOrderSize;
  UINT16                         *OptionOrder;
  H2O_BDS_OPTION_TYPE_INFO       *OptionTypeInfo;

  if (OrderVarInfo == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  switch (OptionType) {

  case H2O_BDS_LOAD_OPTION_TYPE_OS_RECOVERY:
    Status = OsRecoveryGetOrderVarInfo (OrderVarInfo);
    break;

  case H2O_BDS_LOAD_OPTION_TYPE_PLATFORM_RECOVERY:
    Status = PlatformRecoveryGetOrderVarInfo (OrderVarInfo);
    break;

  case H2O_BDS_LOAD_OPTION_TYPE_DRIVER:
  case H2O_BDS_LOAD_OPTION_TYPE_BOOT:
  case H2O_BDS_LOAD_OPTION_TYPE_SYSPREP:
    OptionTypeInfo = GetOptionTypeInfo (OptionType);
    if (OptionTypeInfo == NULL) {
      return EFI_UNSUPPORTED;
    }

    Status = CommonGetVariableDataAndSize (
               OptionTypeInfo->OrderVarName,
               &gEfiGlobalVariableGuid,
               &OptionOrderSize,
               (VOID **) &OptionOrder
               );
    if (EFI_ERROR (Status)) {
      Status          = EFI_SUCCESS;
      OptionOrderSize = 0;
      OptionOrder     = NULL;
    }

    OrderVarInfo->OptionCount   = (UINT16) (OptionOrderSize / sizeof (UINT16));
    OrderVarInfo->OptionOrder   = OptionOrder;
    OrderVarInfo->OptionVarGuid = OrderVarInfo->OptionCount == 0 ? NULL : AllocatePool (OrderVarInfo->OptionCount * sizeof (EFI_GUID));
    if (OrderVarInfo->OptionVarGuid != NULL) {
      for (Index = 0; Index < OrderVarInfo->OptionCount; Index++) {
        CopyGuid (&OrderVarInfo->OptionVarGuid[Index], &gEfiGlobalVariableGuid);
      }
    }
    break;

  default:
    return EFI_UNSUPPORTED;
  }

  return Status;
}

/**
  Based on input order variable info, update order variable.

  @param[in] OptionType          Option type value.
  @param[in] OrderVarInfo        A pointer to order variable info.

  @retval EFI_SUCCESS            Update order variable successfully.
  @retval EFI_INVALID_PARAMETER  Input parameter is NULL.
  @retval EFI_UNSUPPORTED        Fail to find option type info.
  @retval Other                  Failed to set variable.
**/
EFI_STATUS
SetOrderVar (
  IN       UINT8                       OptionType,
  IN CONST H2O_BDS_ORDER_VAR_INFO      *OrderVarInfo
  )
{
  EFI_STATUS                           Status;
  H2O_BDS_OPTION_TYPE_INFO             *OptionTypeInfo;

  if (OrderVarInfo == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  switch (OptionType) {

  case H2O_BDS_LOAD_OPTION_TYPE_OS_RECOVERY:
    Status = OsRecoverySetOrderVar (OrderVarInfo);
    break;

  case H2O_BDS_LOAD_OPTION_TYPE_DRIVER:
  case H2O_BDS_LOAD_OPTION_TYPE_BOOT:
  case H2O_BDS_LOAD_OPTION_TYPE_SYSPREP:
    OptionTypeInfo = GetOptionTypeInfo (OptionType);
    if (OptionTypeInfo == NULL) {
      return EFI_UNSUPPORTED;
    }

    Status = CommonSetVariable (
               OptionTypeInfo->OrderVarName,
               &gEfiGlobalVariableGuid,
               OptionTypeInfo->VarAttribute,
               OrderVarInfo->OptionCount * sizeof (UINT16),
               OrderVarInfo->OptionOrder
               );
    break;

  case H2O_BDS_LOAD_OPTION_TYPE_PLATFORM_RECOVERY:
    //
    // There is no PlatformRecoveryOrder variable, because order is determined by #### of PlatformRecovery#### variable.
    //
  default:
    Status = EFI_UNSUPPORTED;
    break;
  }

  return Status;
}

/**
  According to variable name and GUID to Determine the variable is BootPerv or not.

  @param[in]  VariableName   Name of Variable to be found.
  @param[in]  VendorGuid     Variable vendor GUID.

  @retval     TRUE           This is BootPerv variable.
  @retval     FALSE          This isn't BootPerv variable.
**/
STATIC
BOOLEAN
IsBootPreviousVariable (
  IN CONST  CHAR16                             *VariableName,
  IN CONST  EFI_GUID                           *VendorGuid
  )
{
  if (VariableName != NULL && VendorGuid != NULL) {
    if (StrCmp (VariableName, H2O_BOOT_PREVIOUS_VARIABLE_NAME) == 0 && CompareGuid (VendorGuid, &gEfiGenericVariableGuid)) {
      return TRUE;
    }
  }
  return FALSE;
}

/**
  According to variable name and GUID to Determine the variable is BootCurrent or not.

  @param[in]  VariableName   Name of Variable to be found.
  @param[in]  VendorGuid     Variable vendor GUID.

  @retval     TRUE           This is BootCurrent variable.
  @retval     FALSE          This isn't BootCurrent variable.
**/
STATIC
BOOLEAN
IsBootCurrentVariable (
  IN CONST  CHAR16                             *VariableName,
  IN CONST  EFI_GUID                           *VendorGuid
  )
{
  if (VariableName != NULL && VendorGuid != NULL) {
    if (StrCmp (VariableName, EFI_BOOT_CURRENT_VARIABLE_NAME) == 0 && CompareGuid (VendorGuid, &gEfiGlobalVariableGuid)) {
      return TRUE;
    }
  }
  return FALSE;
}

/**
  According to variable name and GUID to Determine the variable is BootNext or not.

  @param[in]  VariableName   Name of Variable to be found.
  @param[in]  VendorGuid     Variable vendor GUID.

  @retval     TRUE           This is BootNext variable.
  @retval     FALSE          This isn't BootNext variable.
**/
STATIC
BOOLEAN
IsBootNextVariable (
  IN CONST  CHAR16                             *VariableName,
  IN CONST  EFI_GUID                           *VendorGuid
  )
{
  if (VariableName != NULL && VendorGuid != NULL) {
    if (StrCmp (VariableName, EFI_BOOT_NEXT_VARIABLE_NAME) == 0 && CompareGuid (VendorGuid, &gEfiGlobalVariableGuid)) {
      return TRUE;
    }
  }
  return FALSE;
}

/**
  Internal function to insert load option to load option list.

  @param[in]  BdsLoadOption      Load option want to insert to load option list.
  @param[in]  OrderVarInfo       Pointer to order variable info.
  @param[in]  InsertOptionIndex  Order index of inserted load option.

  @retval EFI_SUCCESS            Insert load option to load option list successfully.
  @retval EFI_INVALID_PARAMETER  BdsLoadOption is NULL, OrderVarInfo is NULL.
  @retval EFI_UNSUPPORTED        Fail to find option type info.
**/
STATIC
EFI_STATUS
InsertLoadOptionToList (
  IN       H2O_BDS_LOAD_OPTION     *BdsLoadOption,
  IN CONST H2O_BDS_ORDER_VAR_INFO  *OrderVarInfo,
  IN       UINTN                   InsertOptionIndex
  )
{
  LIST_ENTRY                       *OptionList;
  LIST_ENTRY                       *CurrentList;
  LIST_ENTRY                       *NextList;
  UINTN                            Index;
  H2O_BDS_LOAD_OPTION              *CurrentLoadOption;
  H2O_BDS_OPTION_TYPE_INFO         *OptionTypeInfo;


  if (BdsLoadOption == NULL || OrderVarInfo == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  OptionTypeInfo = GetOptionTypeInfo (BdsLoadOption->OptionType);
  if (OptionTypeInfo == NULL) {
    return EFI_UNSUPPORTED;
  }
  OptionList = &OptionTypeInfo->OptionList;

  if (IsListEmpty (OptionList)) {
    InsertTailList (OptionList, &BdsLoadOption->Link);
    return EFI_SUCCESS;
  }

  CurrentList = GetFirstNode (OptionList);
  for (Index = 0; Index < InsertOptionIndex && Index < OrderVarInfo->OptionCount; Index++) {
    if (IsNull (OptionList, CurrentList)) {
      break;
    }
    CurrentLoadOption = BDS_OPTION_FROM_LINK (CurrentList);
    if (IsLoadOptionMatch (CurrentLoadOption, OrderVarInfo->OptionOrder[Index], &OrderVarInfo->OptionVarGuid[Index])) {
      CurrentList = GetNextNode (OptionList, CurrentList);
    }
  }

  if (CurrentList != OptionList) {
    CurrentLoadOption = BDS_OPTION_FROM_LINK (CurrentList);
    if (IsLoadOptionMatch (CurrentLoadOption, BdsLoadOption->LoadOrder, &BdsLoadOption->LoadOrderVarGuid)) {
      NextList = GetNextNode (OptionList, CurrentList);
      RemoveEntryList (&CurrentLoadOption->Link);
      BdsServicesFreeLoadOption (&mH2OBdsServices, CurrentLoadOption);
      CurrentList = NextList;
    }
  }

  InsertTailList (CurrentList, &BdsLoadOption->Link);

  return EFI_SUCCESS;
}

/**
  Internal function to check the input device path is whether a legacy boot option device path.

  @param[in]  DevicePath   A pointer to a device path data structure.

  @retval TRUE             This is a legacy boot option.
  @retval FALSE            This isn't a legacy boot option.
**/
STATIC
BOOLEAN
IsLegacyBootOption (
  IN  CONST EFI_DEVICE_PATH_PROTOCOL  *DevicePath
  )
{
  if (DevicePath == NULL) {
    return FALSE;
  }

  if ((BBS_DEVICE_PATH == DevicePath->Type) && (BBS_BBS_DP == DevicePath->SubType)) {
    return TRUE;
  }
  return FALSE;
}

/**
  Internal function to check the input device is whether in expanded option list.

  @param[in]  BdsLoadOption  Load option want to insert to load option list.
  @param[in]  DevicePath     A pointer to a device path data structure.

  @retval TRUE               The device path is already in expanded option list
  @retval FALSE              The device path isn't in expanded option list
**/
STATIC
BOOLEAN
IsInExpandedLoadOptions (
  IN        H2O_BDS_LOAD_OPTION         *BdsLoadOption,
  IN  CONST EFI_DEVICE_PATH_PROTOCOL    *DevicePath
  )
{
  LIST_ENTRY               *ExpandedOptions;
  LIST_ENTRY               *Link;
  H2O_BDS_LOAD_OPTION      *CurrentLoadOption;

  if (BdsLoadOption == NULL || !BdsLoadOption->Expanded || DevicePath == NULL) {
    return FALSE;
  }

  ExpandedOptions = &BdsLoadOption->ExpandedLoadOptions;
  for (Link = GetFirstNode (ExpandedOptions); !IsNull (ExpandedOptions, Link); Link = GetNextNode (ExpandedOptions, Link)) {
    CurrentLoadOption = BDS_OPTION_FROM_LINK (Link);
    if (GetDevicePathSize (DevicePath) == GetDevicePathSize (CurrentLoadOption->DevicePath) &&
        CompareMem (DevicePath, CurrentLoadOption->DevicePath, GetDevicePathSize (DevicePath)) == 0) {
      return TRUE;
    }
  }
  return FALSE;
}

/**
  Internal function to add expanded load option to expand option list.

  @param[in]  BdsLoadOption      Load option want to insert to load option list.

  @retval EFI_SUCCESS            Add expaanded load option to option list successfully.
  @retval EFI_INVALID_PARAMETER  BdsLoadOption is NULL.
**/
STATIC
EFI_STATUS
AddExpandedWindowsToGoOption (
  IN OUT  H2O_BDS_LOAD_OPTION         *BdsLoadOption
  )
{
  UINTN                         Index;
  UINTN                         NumberFileSystemHandles;
  EFI_HANDLE                    *FileSystemHandles;
  H2O_BDS_LOAD_OPTION           *NewLoadOption;
  EFI_STATUS                    Status;
  EFI_DEVICE_PATH_PROTOCOL      *DevicePath;

  if (BdsLoadOption == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiSimpleFileSystemProtocolGuid,
                  NULL,
                  &NumberFileSystemHandles,
                  &FileSystemHandles
                  );
  if (EFI_ERROR (Status)) {
    return EFI_SUCCESS;
  }

  for (Index = 0; Index < NumberFileSystemHandles; Index++) {
    DevicePath = DevicePathFromHandle (FileSystemHandles[Index]);
    if (IsWindowsToGo (FileSystemHandles[Index]) && !IsInExpandedLoadOptions (BdsLoadOption, DevicePath)) {
      Status = BdsServicesCreateLoadOption (
                 &mH2OBdsServices,
                 BOOT_OPTION,
                 BDS_OPTION_HAVE_LOAD_ORDER_VAR_NAME(BdsLoadOption) ? BdsLoadOption->LoadOrderVarName  : NULL,
                 BDS_OPTION_HAVE_LOAD_ORDER_VAR_NAME(BdsLoadOption) ? &BdsLoadOption->LoadOrderVarGuid : NULL,
                 LOAD_OPTION_ACTIVE,
                 DevicePath,
                 BdsLoadOption->Description,
                 BdsLoadOption->LoadOptionalData,
                 BdsLoadOption->LoadOptionalDataSize,
                 &NewLoadOption
                 );
      if (Status == EFI_SUCCESS) {
        InsertTailList (&BdsLoadOption->ExpandedLoadOptions, &NewLoadOption->Link);
      }
    }
  }
  InternalFreePool (FileSystemHandles);
  return EFI_SUCCESS;
}

/**
  Internal function to check the input device is whether in input device path array.

  @param[in]  DevicePath        A Pointer to a UEFI device path.
  @param[in]  DevicePaths       A Pointer to a UEFI device path array.
  @param[in]  DevicePathCount   The device path count in device path array.

  @retval TRUE          Device path is in device path array.
  @retval FALSE         Device path isn't in device path array.
**/
STATIC
BOOLEAN
IsDevicePathInArray (
  IN  EFI_DEVICE_PATH_PROTOCOL      *DevicePath,
  IN  EFI_DEVICE_PATH_PROTOCOL      **DevicePaths,
  IN  UINTN                         DevicePathCount
  )
{
  UINTN                 Index;

  if (DevicePath == NULL || DevicePaths == NULL || DevicePathCount == 0) {
    return FALSE;
  }

  for (Index = 0; Index < DevicePathCount; Index++) {
    if (GetDevicePathSize (DevicePath) == GetDevicePathSize (DevicePaths[Index]) &&
        CompareMem (DevicePath, DevicePaths[Index], GetDevicePathSize (DevicePath)) == 0) {
      return TRUE;
    }
  }
  return FALSE;
}

/**
  Internal function to remove redundant boot option in expanded list.

  @param[in]  BdsLoadOption      Load option want to insert to load option list.

  @retval EFI_SUCCESS            Remove redundant boot option successfully.
  @retval EFI_INVALID_PARAMETER  BdsLoadOption is NULL.
  @retval EFI_OUT_OF_RESOURCES   Unable to allocate memory to store device path.
**/
STATIC
EFI_STATUS
RemoveRedundantExpandedWindowsToGoOption (
  IN OUT  H2O_BDS_LOAD_OPTION         *BdsLoadOption
  )
{
  UINTN                         Index;
  UINTN                         NumberFileSystemHandles;
  EFI_HANDLE                    *FileSystemHandles;
  H2O_BDS_LOAD_OPTION           *CurrentLoadOption;
  EFI_STATUS                    Status;
  EFI_DEVICE_PATH_PROTOCOL      **DevicePaths;
  LIST_ENTRY                    *ExpandedOptions;
  LIST_ENTRY                    *Link;
  LIST_ENTRY                    *CurrentLink;

  if (BdsLoadOption == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiSimpleFileSystemProtocolGuid,
                  NULL,
                  &NumberFileSystemHandles,
                  &FileSystemHandles
                  );
  if (EFI_ERROR (Status)) {
    return EFI_SUCCESS;
  }
  DevicePaths = AllocateZeroPool (NumberFileSystemHandles * sizeof (EFI_DEVICE_PATH_PROTOCOL *));
  if (DevicePaths == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  for (Index = 0; Index < NumberFileSystemHandles; Index++) {
    DevicePaths[Index] = DevicePathFromHandle (FileSystemHandles[Index]);
  }

  ExpandedOptions = &BdsLoadOption->ExpandedLoadOptions;
  for (Link = GetFirstNode (ExpandedOptions); !IsNull (ExpandedOptions, Link); Link = GetNextNode (ExpandedOptions, Link)) {
     CurrentLoadOption = BDS_OPTION_FROM_LINK (Link);
     if (!IsDevicePathInArray (CurrentLoadOption->DevicePath, DevicePaths, NumberFileSystemHandles)) {
       CurrentLink = Link;
       Link = CurrentLink->BackLink;
       RemoveEntryList (CurrentLink);
       BdsServicesFreeLoadOption (&mH2OBdsServices, CurrentLoadOption);
     }
  }
  InternalFreePool (DevicePaths);
  InternalFreePool (FileSystemHandles);

  return EFI_SUCCESS;
}
/**
  Internal function to expand windows to go boot option.

  It will set Expanded member in BdsLoadOption to TRUE and insert all of expanded load options
  to ExpandedLoadOptions list in BdsLoadOption if this function return EFI_SUCESS.

  @param[in,out] BdsLoadOption  A pointer to BDS load option.

  @retval EFI_SUCCESS           Expand windows to go boot option successfully.
  @retval EFI_INVALID_PARAMETER BdsLoadOption is NULL.
**/
STATIC
EFI_STATUS
ExpandWinowsToGoOption (
  IN OUT  H2O_BDS_LOAD_OPTION         *BdsLoadOption
  )
{

  EFI_STATUS                  Status;
  KERNEL_CONFIGURATION        SystemConfiguration;

  if (BdsLoadOption == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = GetKernelConfiguration (&SystemConfiguration);
  if (Status == EFI_SUCCESS && SystemConfiguration.UsbBoot != 0) {
    BdsLoadOption->Expanded = TRUE;
    return EFI_SUCCESS;
  }

  AddExpandedWindowsToGoOption (BdsLoadOption);
  RemoveRedundantExpandedWindowsToGoOption (BdsLoadOption);
  BdsLoadOption->Expanded = TRUE;
  return EFI_SUCCESS;
}

/**
  Internal function to expand hard drive media short-form device path to hard driver media full device
  path.

  It will set Expanded member in BdsLoadOption to TRUE and insert all of expanded load options
  to ExpandedLoadOptions list in BdsLoadOption if this function return EFI_SUCESS.

  @param[in,out] BdsLoadOption  A pointer to BDS load option.

  @retval EFI_SUCCESS           Expand hard driver media device path successfully.
  @retval EFI_INVALID_PARAMETER BdsLoadOption is NULL.
**/
STATIC
EFI_STATUS
ExpandHddOption (
  IN   H2O_BDS_LOAD_OPTION         *BdsLoadOption
  )
{
  EFI_DEVICE_PATH_PROTOCOL       *FullDevicePath;
  EFI_STATUS                     Status;
  H2O_BDS_LOAD_OPTION            *NewLoadOption;

  if (BdsLoadOption == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  FullDevicePath = BdsExpandPartitionPartialDevicePathToFull ((HARDDRIVE_DEVICE_PATH *)BdsLoadOption->DevicePath);
  if (FullDevicePath != NULL && !IsInExpandedLoadOptions (BdsLoadOption, FullDevicePath)) {
    Status = BdsServicesCreateLoadOption (
               &mH2OBdsServices,
               BOOT_OPTION,
               BDS_OPTION_HAVE_LOAD_ORDER_VAR_NAME(BdsLoadOption) ? BdsLoadOption->LoadOrderVarName  : NULL,
               BDS_OPTION_HAVE_LOAD_ORDER_VAR_NAME(BdsLoadOption) ? &BdsLoadOption->LoadOrderVarGuid : NULL,
               LOAD_OPTION_ACTIVE,
               FullDevicePath,
               BdsLoadOption->Description,
               BdsLoadOption->LoadOptionalData,
               BdsLoadOption->LoadOptionalDataSize,
               &NewLoadOption
               );
    if (!EFI_ERROR (Status)) {
      InsertTailList (&BdsLoadOption->ExpandedLoadOptions, &NewLoadOption->Link);
    }
    BdsLoadOption->Expanded = TRUE;
  }
  InternalFreePool (FullDevicePath);
  return EFI_SUCCESS;
}

/**
  Internal function to expand firmware file media short-form device path to full device path.

  It will set Expanded member in BdsLoadOption to TRUE and insert all of expanded load options
  to ExpandedLoadOptions list in BdsLoadOption if this function return EFI_SUCESS.

  @param[in,out] BdsLoadOption         A pointer to BDS load option.

  @retval EFI_SUCCESS                  Expand firmware file media device path successfully.
  @retval EFI_INVALID_PARAMETER        BdsLoadOption is NULL.
  @retval Other                        There is no firmware volume on system.
**/
STATIC
EFI_STATUS
ExpandFvFileOption (
  IN OUT H2O_BDS_LOAD_OPTION           *BdsLoadOption
  )
{
  EFI_STATUS                           Status;
  UINTN                                HandleCount;
  EFI_HANDLE                           *HandleBuffer;
  EFI_GUID                             *FvFileName;
  EFI_DEVICE_PATH_PROTOCOL             *DevicePath;
  UINTN                                Index;
  EFI_FIRMWARE_VOLUME2_PROTOCOL        *Fv;
  EFI_FV_FILETYPE                      Type;
  UINTN                                Size;
  EFI_FV_FILE_ATTRIBUTES               Attributes;
  UINT32                               AuthenticationStatus;
  H2O_BDS_LOAD_OPTION                  *NewLoadOption;

  if (BdsLoadOption == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiFirmwareVolume2ProtocolGuid,
                  NULL,
                  &HandleCount,
                  &HandleBuffer
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  FvFileName = &(((MEDIA_FW_VOL_FILEPATH_DEVICE_PATH *) BdsLoadOption->DevicePath)->FvFileName);
  for (Index = 0; Index < HandleCount; Index++) {
    Status = gBS->HandleProtocol (HandleBuffer[Index], &gEfiFirmwareVolume2ProtocolGuid, (VOID **) &Fv);
    if (EFI_ERROR (Status)) {
      continue;
    }

    Status = Fv->ReadFile (
                   Fv,
                   FvFileName,
                   NULL,
                   &Size,
                   &Type,
                   &Attributes,
                   &AuthenticationStatus
                   );
    if (EFI_ERROR (Status)) {
      continue;
    }

    Status = gBS->HandleProtocol (HandleBuffer[Index], &gEfiDevicePathProtocolGuid, (VOID *) &DevicePath);
    if (EFI_ERROR (Status)) {
      continue;
    }

    DevicePath = AppendDevicePath (DevicePath, BdsLoadOption->DevicePath);
    if (DevicePath == NULL) {
      continue;
    }

    if (!IsInExpandedLoadOptions (BdsLoadOption, DevicePath)) {
      Status = BdsServicesCreateLoadOption (
                 &mH2OBdsServices,
                 BOOT_OPTION,
                 BDS_OPTION_HAVE_LOAD_ORDER_VAR_NAME(BdsLoadOption) ? BdsLoadOption->LoadOrderVarName  : NULL,
                 BDS_OPTION_HAVE_LOAD_ORDER_VAR_NAME(BdsLoadOption) ? &BdsLoadOption->LoadOrderVarGuid : NULL,
                 LOAD_OPTION_ACTIVE,
                 DevicePath,
                 BdsLoadOption->Description,
                 BdsLoadOption->LoadOptionalData,
                 BdsLoadOption->LoadOptionalDataSize,
                 &NewLoadOption
                 );
      if (Status == EFI_SUCCESS) {
        InsertTailList (&BdsLoadOption->ExpandedLoadOptions, &NewLoadOption->Link);
        BdsLoadOption->Expanded = TRUE;
      }
    }
    FreePool (DevicePath);
  }

  FreePool (HandleBuffer);
  return EFI_SUCCESS;
}

/**
  Internal function to expand H2O BDS groupt boot option.

  It will set Expanded member in BdsLoadOption to TRUE and insert all of expanded load options
  to ExpandedLoadOptions list in BdsLoadOption if this function return EFI_SUCESS.

  @param[in,out] BdsLoadOption  A pointer to BDS load option.

  @retval EFI_SUCCESS           Expand H2O BDS groupt boot option successfully.
  @retval EFI_INVALID_PARAMETER BdsLoadOption is NULL.
**/
STATIC
EFI_STATUS
ExpandBootGroupOption (
  IN   H2O_BDS_LOAD_OPTION         *BdsLoadOption
  )
{
  EFI_STATUS                          Status;
  UINTN                               HandleCount;
  EFI_HANDLE                          *HandleBuffer;
  H2O_BDS_BOOT_GROUP_DEVICE_PATH      *BootGroupDevicePath;
  UINTN                               Index;
  H2O_BDS_BOOT_GROUP_PROTOCOL         *BootGroup;
  UINT32                              BootOptionArrayLen;
  H2O_BDS_LOAD_OPTION                 *BootOptionArray;
  UINT32                              ArrayIndex;
  H2O_BDS_LOAD_OPTION                 *NewLoadOption;

  if (BdsLoadOption == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gH2OBdsBootGroupProtocolGuid,
                  NULL,
                  &HandleCount,
                  &HandleBuffer
                  );
  if (Status != EFI_SUCCESS) {
    return Status;
  }

  BootGroupDevicePath = (H2O_BDS_BOOT_GROUP_DEVICE_PATH *) BdsLoadOption->DevicePath;
  for (Index = 0; Index < HandleCount; Index++) {
    Status = gBS->HandleProtocol (
                    HandleBuffer[Index],
                    &gH2OBdsBootGroupProtocolGuid,
                    (VOID **) &BootGroup
                    );
    ASSERT (Status == EFI_SUCCESS);
    if (CompareGuid (&BootGroup->VendorGuid, &BootGroupDevicePath->VendorGuid)) {
      Status = BootGroup->GetGroupDevices (
                            BootGroup,
                            BdsLoadOption,
                            &BootOptionArrayLen,
                            &BootOptionArray
                            );
      if (Status != EFI_SUCCESS) {
        break;
      }
      for (ArrayIndex = 0; ArrayIndex < BootOptionArrayLen; ArrayIndex++) {
        if (!IsInExpandedLoadOptions(BdsLoadOption, BootOptionArray[ArrayIndex].DevicePath)) {
          Status = BdsServicesCreateLoadOption (
                     &mH2OBdsServices,
                     BootOptionArray[ArrayIndex].DriverOrBoot,
                     BDS_OPTION_HAVE_LOAD_ORDER_VAR_NAME(&BootOptionArray[ArrayIndex]) ? BootOptionArray[ArrayIndex].LoadOrderVarName  : NULL,
                     BDS_OPTION_HAVE_LOAD_ORDER_VAR_NAME(&BootOptionArray[ArrayIndex]) ? &BootOptionArray[ArrayIndex].LoadOrderVarGuid : NULL,
                     BootOptionArray[ArrayIndex].Attributes,
                     BootOptionArray[ArrayIndex].DevicePath,
                     BootOptionArray[ArrayIndex].Description,
                     BootOptionArray[ArrayIndex].LoadOptionalData,
                     BootOptionArray[ArrayIndex].LoadOptionalDataSize,
                     &NewLoadOption
                     );
          ASSERT (Status == EFI_SUCCESS);
          if (!EFI_ERROR (Status)) {
            InsertTailList (&BdsLoadOption->ExpandedLoadOptions, &NewLoadOption->Link);
          }
        }
      }
      FreePool (BootOptionArray);
      break;
    }
  }

  FreePool (HandleBuffer);
  return EFI_SUCCESS;
}

/**
  Internal function to expand H2O BDS boot option with USB short-form device path.

  It will set Expanded member in BdsLoadOption to TRUE and insert all of expanded load options
  to ExpandedLoadOptions list in BdsLoadOption if this function return EFI_SUCESS.

  @param[in,out] BdsLoadOption  A pointer to BDS load option.

  @retval EFI_SUCCESS           Expand H2O BDS boot option with USB short-form device path successfully.
  @retval EFI_INVALID_PARAMETER BdsLoadOption is NULL.
  @retval EFI_NOT_FOUND         Failed to expand USB short-form device path.
**/
STATIC
EFI_STATUS
ExpandUsbShortFormOption (
  IN H2O_BDS_LOAD_OPTION         *BdsLoadOption
  )
{
  EFI_STATUS                     Status;
  EFI_DEVICE_PATH_PROTOCOL       *FullDevicePath;
  H2O_BDS_LOAD_OPTION            *NewLoadOption;

  if (BdsLoadOption == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  FullDevicePath = BdsLibExpandUsbShortFormDevPath (BdsLoadOption->DevicePath);
  if (FullDevicePath == NULL) {
    return EFI_NOT_FOUND;
  }

  if (!IsInExpandedLoadOptions (BdsLoadOption, FullDevicePath)) {
    Status = BdsServicesCreateLoadOption (
               &mH2OBdsServices,
               BOOT_OPTION,
               BDS_OPTION_HAVE_LOAD_ORDER_VAR_NAME(BdsLoadOption) ? BdsLoadOption->LoadOrderVarName  : NULL,
               BDS_OPTION_HAVE_LOAD_ORDER_VAR_NAME(BdsLoadOption) ? &BdsLoadOption->LoadOrderVarGuid : NULL,
               LOAD_OPTION_ACTIVE,
               FullDevicePath,
               BdsLoadOption->Description,
               BdsLoadOption->LoadOptionalData,
               BdsLoadOption->LoadOptionalDataSize,
               &NewLoadOption
               );
    if (!EFI_ERROR (Status)) {
      InsertTailList (&BdsLoadOption->ExpandedLoadOptions, &NewLoadOption->Link);
    }
    BdsLoadOption->Expanded = TRUE;
  }

  InternalFreePool (FullDevicePath);
  return EFI_SUCCESS;
}

/**
  Internal function to check the load option is whether in order variable.

  @param[in] LoadOption    A pointer to BDS load option.
  @param[in] OrderVarInfo  A pointer to order variable info.

  @retval TRUE             Load option is in the order variable.
  @retval FALSE            Load option isn't in the order variable.
**/
STATIC
BOOLEAN
IsLoadOptionInOrderVar (
  IN CONST H2O_BDS_LOAD_OPTION        *LoadOption,
  IN CONST H2O_BDS_ORDER_VAR_INFO     *OrderVarInfo
  )
{
  UINTN        Index;

  if (LoadOption == NULL || OrderVarInfo == NULL) {
    return FALSE;
  }

  for (Index = 0; Index < OrderVarInfo->OptionCount; Index++) {
    if (IsLoadOptionMatch (LoadOption, OrderVarInfo->OptionOrder[Index], &OrderVarInfo->OptionVarGuid[Index])) {
      return TRUE;
    }
  }
  return FALSE;
}

/**
  Internal function to check the load order number with vendor GUID is whether in option list.

  @param[in]  LoadOrder          Unsigned integer that specifies the current option being booted.
  @param[in]  LoadOrderVarGuid   A pointer to the vendor variable GUID.
  @param[in]  OptionList         A list head of linked list. Each entry is a H2O_BDS_LOAD_OPTION.

  @retval TRUE            Load order number is in the option list.
  @retval FALSE           Load order number isn't in the option list.
**/
STATIC
BOOLEAN
IsOptionNumInOptionList (
  IN       UINT16         LoadOrder,
  IN CONST EFI_GUID       *LoadOrderVarGuid,
  IN CONST LIST_ENTRY     *OptionList
  )
{
  LIST_ENTRY             *Link;
  H2O_BDS_LOAD_OPTION    *CurrentLoadOption;

  if (LoadOrderVarGuid == NULL || OptionList == NULL) {
    return FALSE;
  }

  for (Link = GetFirstNode (OptionList); !IsNull (OptionList, Link); Link = GetNextNode (OptionList, Link)) {
    CurrentLoadOption = BDS_OPTION_FROM_LINK (Link);
    if (IsLoadOptionMatch (CurrentLoadOption, LoadOrder, LoadOrderVarGuid)) {
      return TRUE;
    }
  }

  return FALSE;
}

/**
  Get the order index in order variable for input load option.

  @param[in]  BdsLoadOption      Pointer to load option
  @param[out] OrderIndex         Order index in the option order variable

  @retval EFI_SUCCESS            Get the order index in order variable successfully.
  @retval EFI_INVALID_PARAMETER  BdsLoadOption is invalid or OrderIndex is NULL.
  @retval EFI_UNSUPPORTED        Fail to find option type info.
  @retval EFI_NOT_FOUND          Order variable info is not found.
**/
STATIC
EFI_STATUS
GetOrderIndexInOrderVar (
  IN CONST H2O_BDS_LOAD_OPTION         *BdsLoadOption,
  OUT      UINTN                       *OrderIndex
  )
{
  H2O_BDS_OPTION_TYPE_INFO             *OptionTypeInfo;
  H2O_BDS_ORDER_VAR_INFO               OrderVarInfo;
  EFI_STATUS                           Status;
  UINTN                                Index;
  UINTN                                OptionCount;
  UINT16                               *OptionOrder;
  CHAR16                               OptionVarName[OPTION_VAR_NAME_BUFFER_CHAR_COUNT];
  UINTN                                OptionVarSize;
  UINT16                               *OptionVar;
  UINT8                                *TempPtr;
  EFI_DEVICE_PATH_PROTOCOL             *OptionDevicePath;
  UINT16                               VarBbsTableIndex;
  UINT16                               OptionBbsTableIndex;

  if (!IsValidLoadOption (BdsLoadOption) || OrderIndex == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  OptionTypeInfo = GetOptionTypeInfo (BdsLoadOption->OptionType);
  if (OptionTypeInfo == NULL) {
    return EFI_UNSUPPORTED;
  }

  Status = GetOrderVarInfo (BdsLoadOption->OptionType, &OrderVarInfo);
  if (EFI_ERROR (Status)) {
    return EFI_NOT_FOUND;
  }

  OptionVar   = NULL;
  OptionCount = OrderVarInfo.OptionCount;
  OptionOrder = OrderVarInfo.OptionOrder;
  for (Index = 0; Index < OptionCount; Index++) {
    if (BdsLoadOption->OptionType == H2O_BDS_LOAD_OPTION_TYPE_BOOT && BdsLibIsDummyBootOption (OptionOrder[Index])) {
      if (BdsLoadOption->LoadOrder == OptionOrder[Index]) {
        break;
      }
      continue;
    }

    UnicodeSPrint (OptionVarName, sizeof (OptionVarName), OPTION_VAR_NAME_FORMAT_STR, OptionTypeInfo->OptionVarPrefixName, OptionOrder[Index]);
    Status = CommonGetVariableDataAndSize (
               OptionVarName,
               &OrderVarInfo.OptionVarGuid[Index],
               &OptionVarSize,
               (VOID **) &OptionVar
               );
    if (Status != EFI_SUCCESS) {
      continue;
    }

    TempPtr = (UINT8 *) OptionVar;
    TempPtr += sizeof (UINT32) + sizeof (UINT16);
    TempPtr += StrSize ((CHAR16 *) TempPtr);
    OptionDevicePath = (EFI_DEVICE_PATH_PROTOCOL *) TempPtr;

    if (IsLegacyBootOption (OptionDevicePath)) {
      if (IsLegacyBootOption (BdsLoadOption->DevicePath)) {
        VarBbsTableIndex    = *((UINT16 *) (TempPtr + GetDevicePathSize (OptionDevicePath) + sizeof (BBS_TABLE)));
        OptionBbsTableIndex = *((UINT16 *) ((UINT8 *) BdsLoadOption->LoadOptionalData + sizeof (BBS_TABLE)));
        if (VarBbsTableIndex == OptionBbsTableIndex) {
          break;
        }
      }
    } else {
      if ((CompareMem (BdsLoadOption->DevicePath, OptionDevicePath, GetDevicePathSize (OptionDevicePath)) == 0) ||
          (MatchPartitionDevicePathNode (BdsLoadOption->DevicePath, (HARDDRIVE_DEVICE_PATH *) OptionDevicePath) &&
           BdsLibMatchFilePathDevicePathNode (BdsLoadOption->DevicePath, OptionDevicePath))) {
        break;
      }
    }
    InternalFreePool (OptionVar);
    OptionVar = NULL;
  }

  InternalFreePool (OptionVar);
  FreeOrderVarInfo (&OrderVarInfo);
  if (Index == OptionCount) {
    return EFI_NOT_FOUND;
  }

  *OrderIndex = Index;
  return EFI_SUCCESS;
}

/**
 Check the device path belongs to PXE boot option or not.

 @param[in] DevicePath          Device path

 @retval TRUE                   The device path is for PXE boot option.
 @retval FALSE                  The device path isn't for PXE boot option.
**/
STATIC
BOOLEAN
IsPxeBoot (
  IN EFI_DEVICE_PATH_PROTOCOL   *DevicePath
  )
{
  BOOLEAN                       IsPxeImage;
  EFI_DEVICE_PATH_PROTOCOL      *NetworkDevicePath;

  IsPxeImage = FALSE;
  NetworkDevicePath = DevicePath;
  while (!IsDevicePathEnd (NetworkDevicePath)) {
    if (NetworkDevicePath->Type == MESSAGING_DEVICE_PATH) {
      if (NetworkDevicePath->SubType == MSG_MAC_ADDR_DP) {
        IsPxeImage = TRUE;
        break;
      }
    }
    NetworkDevicePath = NextDevicePathNode (NetworkDevicePath);
  }

  return IsPxeImage;
}

/**
 Check the device path is belong to HTTP boot option or not.

 @param[in] DevicePath          Device path

 @retval TRUE                   The device path is for PXE boot option.
 @retval FALSE                  The device path isn't for PXE boot option.
**/
STATIC
BOOLEAN
IsHttpBootHandle (
  IN  EFI_DEVICE_PATH_PROTOCOL  *BootDevicePath
  )
{
  BOOLEAN                       IsNetworkDevice;
  EFI_DEVICE_PATH_PROTOCOL      *TmpDevicePath;

  if (BootDevicePath == NULL) {
    return FALSE;
  }
  //
  // Find out DevicePath like .../MAC/IP/URI
  //
  TmpDevicePath        = BootDevicePath;

  IsNetworkDevice      = FALSE;
  while (!IsDevicePathEnd (TmpDevicePath)) {
    if (TmpDevicePath->Type == MESSAGING_DEVICE_PATH) {
      if (TmpDevicePath->SubType == MSG_MAC_ADDR_DP) {
        IsNetworkDevice = TRUE;
      } else if (IsNetworkDevice && (TmpDevicePath->SubType == MSG_URI_DP)) {
        return TRUE;
      }
    }
    TmpDevicePath = NextDevicePathNode (TmpDevicePath);
  }
  return FALSE;
}

/**
  Return the buffer and buffer size occupied by the RAM Disk.

  @param[in]  RamDiskDevicePath        RAM Disk device path.
  @param[out] RamDiskSizeInPages       Return RAM Disk size in pages.

  @retval RAM Disk buffer.
**/
VOID *
GetRamDiskMemoryInfo (
  IN EFI_DEVICE_PATH_PROTOCOL *RamDiskDevicePath,
  OUT UINTN                   *RamDiskSizeInPages
  )
{

  EFI_STATUS                  Status;
  EFI_HANDLE                  Handle;
  UINT64                      StartingAddr;
  UINT64                      EndingAddr;

  if (RamDiskDevicePath == NULL) {
    return NULL;
  }

  *RamDiskSizeInPages = 0;
  Status = gBS->LocateDevicePath (&gEfiLoadFileProtocolGuid, &RamDiskDevicePath, &Handle);
  if (EFI_ERROR (Status)) {
    return NULL;
  }
  //
  // Check DevicePath is MediaDevice and SubType is RamDisk.
  //
  if ((DevicePathType (RamDiskDevicePath) != MEDIA_DEVICE_PATH) && (DevicePathSubType (RamDiskDevicePath) != MEDIA_RAM_DISK_DP)) {
    return NULL;
  }
  StartingAddr = ReadUnaligned64 ((UINT64 *) ((MEDIA_RAM_DISK_DEVICE_PATH *) RamDiskDevicePath)->StartingAddr);
  EndingAddr   = ReadUnaligned64 ((UINT64 *) ((MEDIA_RAM_DISK_DEVICE_PATH *) RamDiskDevicePath)->EndingAddr);
  *RamDiskSizeInPages = EFI_SIZE_TO_PAGES ((UINTN) (EndingAddr - StartingAddr + 1));
  return (VOID *) (UINTN) StartingAddr;
}

/**
  Destroy the RAM Disk.

  The destroy operation includes to call RamDisk.Unregister to
  unregister the RAM DISK from RAM DISK driver, free the memory
  allocated for the RAM Disk.

  @param[in]  RamDiskDevicePath    RAM Disk device path.
**/
VOID
DestroyRamDisk (
  IN EFI_DEVICE_PATH_PROTOCOL *RamDiskDevicePath
  )
{
  EFI_STATUS                  Status;
  VOID                        *RamDiskBuffer;
  UINTN                       RamDiskSizeInPages;
  EFI_RAM_DISK_PROTOCOL       *RamDisk;

  if (RamDiskDevicePath == NULL) {
    return;
  }

  RamDiskBuffer = GetRamDiskMemoryInfo (RamDiskDevicePath, &RamDiskSizeInPages);
  if (RamDiskBuffer == NULL) {
    return;
  }

  //
  // Destroy RAM Disk.
  //
  Status = gBS->LocateProtocol (&gEfiRamDiskProtocolGuid, NULL, (VOID **) &RamDisk);
  if (EFI_ERROR (Status)) {
    return;
  }
  RamDisk->Unregister (RamDiskDevicePath);
  FreePages (RamDiskBuffer, RamDiskSizeInPages);
}

/**
  Expand the media device path which points to a BlockIo or SimpleFileSystem instance
  by appending EFI_REMOVABLE_MEDIA_FILE_NAME.

  @param[in] DevicePath  The media device path pointing to a BlockIo or SimpleFileSystem instance.

  @return The next possible full path pointing to the load option.
          Caller is responsible to free the memory.
**/
EFI_DEVICE_PATH_PROTOCOL *
ExpandMediaDevicePath (
  IN  EFI_DEVICE_PATH_PROTOCOL        *DevicePath
  )
{
  EFI_STATUS                          Status;
  EFI_HANDLE                          Handle;
  EFI_BLOCK_IO_PROTOCOL               *BlockIo;
  VOID                                *Buffer;
  EFI_DEVICE_PATH_PROTOCOL            *TempDevicePath;
  EFI_DEVICE_PATH_PROTOCOL            *NextFullPath;
  UINTN                               Size;
  UINTN                               TempSize;
  EFI_HANDLE                          *SimpleFileSystemHandles;
  UINTN                               NumberSimpleFileSystemHandles;
  UINTN                               Index;

  //
  // Check whether the device is connected
  //
  TempDevicePath = DevicePath;
  Status = gBS->LocateDevicePath (&gEfiSimpleFileSystemProtocolGuid, &TempDevicePath, &Handle);
  if (!EFI_ERROR (Status)) {
    NextFullPath = FileDevicePath (Handle, EFI_REMOVABLE_MEDIA_FILE_NAME);
    return NextFullPath;
  }

  Status = gBS->LocateDevicePath (&gEfiBlockIoProtocolGuid, &TempDevicePath, &Handle);
  if (EFI_ERROR (Status)) {
    return NULL;
  }

  //
  // For device boot option only pointing to the removable device handle,
  // should make sure all its children handles (its child partion or media handles)
  // are created and connected.
  //
  gBS->ConnectController (Handle, NULL, NULL, TRUE);

  //
  // Issue a dummy read to the device to check for media change.
  // When the removable media is changed, any Block IO read/write will
  // cause the BlockIo protocol be reinstalled and EFI_MEDIA_CHANGED is
  // returned. After the Block IO protocol is reinstalled, subsequent
  // Block IO read/write will success.
  //
  Status = gBS->HandleProtocol (Handle, &gEfiBlockIoProtocolGuid, (VOID **) &BlockIo);
  if (EFI_ERROR (Status)) {
    return NULL;
  }
  Buffer = AllocatePool (BlockIo->Media->BlockSize);
  if (Buffer != NULL) {
    BlockIo->ReadBlocks (
      BlockIo,
      BlockIo->Media->MediaId,
      0,
      BlockIo->Media->BlockSize,
      Buffer
      );
    FreePool (Buffer);
  }

  //
  // Detect the the default boot file from removable Media
  //
  NextFullPath = NULL;
  Size = GetDevicePathSize (DevicePath) - END_DEVICE_PATH_LENGTH;
  gBS->LocateHandleBuffer (
         ByProtocol,
         &gEfiSimpleFileSystemProtocolGuid,
         NULL,
         &NumberSimpleFileSystemHandles,
         &SimpleFileSystemHandles
         );
  for (Index = 0; Index < NumberSimpleFileSystemHandles; Index++) {
    //
    // Get the device path size of SimpleFileSystem handle
    //
    TempDevicePath = DevicePathFromHandle (SimpleFileSystemHandles[Index]);
    TempSize = GetDevicePathSize (TempDevicePath) - END_DEVICE_PATH_LENGTH;
    //
    // Check whether the device path of boot option is part of the SimpleFileSystem handle's device path
    //
    if ((Size <= TempSize) && (CompareMem (TempDevicePath, DevicePath, Size) == 0)) {
      NextFullPath = FileDevicePath (SimpleFileSystemHandles[Index], EFI_REMOVABLE_MEDIA_FILE_NAME);
      break;
    }
  }

  if (SimpleFileSystemHandles != NULL) {
    FreePool (SimpleFileSystemHandles);
  }

  return NextFullPath;
}

/**
  Get the file buffer from the file system produced by Load File instance.

  @param[in]  LoadFileHandle The handle of LoadFile instance.
  @param[out] RamDiskHandle  Return the RAM Disk handle.

  @return The next possible full path pointing to the load option.
          Caller is responsible to free the memory.
**/
EFI_DEVICE_PATH_PROTOCOL *
ExpandNetworkFileSystem (
  IN  EFI_HANDLE                      LoadFileHandle,
  OUT EFI_HANDLE                      *RamDiskHandle
  )
{
  EFI_STATUS                      Status;
  EFI_HANDLE                      Handle;
  EFI_HANDLE                      *Handles;
  UINTN                           HandleCount;
  UINTN                           Index;
  EFI_DEVICE_PATH_PROTOCOL        *Node;

  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiBlockIoProtocolGuid,
                  NULL,
                  &HandleCount,
                  &Handles
                  );
  if (EFI_ERROR (Status)) {
    Handles = NULL;
    HandleCount = 0;
  }

  Handle = NULL;
  for (Index = 0; Index < HandleCount; Index++) {
    Node = DevicePathFromHandle (Handles[Index]);
    Status = gBS->LocateDevicePath (&gEfiLoadFileProtocolGuid, &Node, &Handle);
    if (!EFI_ERROR (Status) &&
        (Handle == LoadFileHandle) &&
        (DevicePathType (Node) == MEDIA_DEVICE_PATH) && (DevicePathSubType (Node) == MEDIA_RAM_DISK_DP)) {
      //
      // Find the BlockIo instance populated from the LoadFile.
      //
      Handle = Handles[Index];
      break;
    }
  }

  if (Handles != NULL) {
    FreePool (Handles);
  }

  if (Index == HandleCount) {
    Handle = NULL;
  }

  *RamDiskHandle = Handle;

  if (Handle != NULL) {
    //
    // Re-use ExpandMediaDevicePath() to get the full device path of load option.
    // But assume only one SimpleFileSystem can be found under the BlockIo.
    //
    return ExpandMediaDevicePath (DevicePathFromHandle (Handle));
  } else {
    return NULL;
  }
}

/**
  Process HTTP boot, if HttpBootDxe driver return EFI_WARN_FILE_SYSTEM.
  This function will allocate reserved memory to HttpBootDxe, it will get the ISO file and put in reverved memory.
  Then it will register RamDisk and connect to BlockIo and FileSystem handle.
  This function will find the expanded device handle (usually is FS) and Output in ExpandedUriDevicePath and RamDiskHandle parameters.

  @param[in]  LoadFileDevicePath       DevicePath of HTTP boot.
  @param[out] ExpandedUriDevicePath    Expanded device path, include RamDisk and FileSystem.
  @param[out] RamDiskHandle            RamDisk handle, output this parameter to handle RamDisk if BDS need to destroy it.

  @return EFI_SUCCESS                  Function successfully.
  @return EFI_INVALID_PARAMETER        Invalid parameters.
  @return EFI_NOT_FOUND                LoadFile is not found.
  @return others                       Error status from called functions.
**/
EFI_STATUS
ProcessHttpUriBoot (
  IN  EFI_DEVICE_PATH_PROTOCOL         *LoadFileDevicePath,
  OUT EFI_DEVICE_PATH_PROTOCOL         **ExpandedUriDevicePath,
  OUT EFI_HANDLE                       *RamDiskHandle
  )
{
  EFI_STATUS                           Status;
  EFI_HANDLE                           HttpBootHandle;
  EFI_DEVICE_PATH_PROTOCOL             *TempDevicePath;
  EFI_DEVICE_PATH_PROTOCOL             *FilePath;
  EFI_DEVICE_PATH_PROTOCOL             *FullPath;
  EFI_LOAD_FILE_PROTOCOL               *LoadFile;
  UINTN                                BufferSize;
  VOID                                 *FileBuffer;

  if ((LoadFileDevicePath == NULL) || (ExpandedUriDevicePath == NULL) || (RamDiskHandle == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  HttpBootHandle = NULL;
  TempDevicePath = LoadFileDevicePath;
  Status = gBS->LocateDevicePath (
                  &gEfiDevicePathProtocolGuid,
                  &TempDevicePath,
                  &HttpBootHandle
                  );
  if (EFI_ERROR (Status)) {
    goto ON_EXIT;
  }

  //
  // Find out HTTP boot entry and original DevicePath.
  //
  Status = gBS->HandleProtocol (HttpBootHandle, &gEfiLoadFileProtocolGuid, (VOID **) &LoadFile);
  if (EFI_ERROR (Status)) {
    Status = EFI_NOT_FOUND;
    goto ON_EXIT;
  }
  Status = gBS->HandleProtocol (HttpBootHandle, &gEfiDevicePathProtocolGuid, (VOID **) &FilePath);
  if (EFI_ERROR (Status)) {
    Status = EFI_NOT_FOUND;
    goto ON_EXIT;
  }

  //
  // Calling HTTP boot entry
  //
  FileBuffer     = NULL;
  BufferSize = 0;
  Status = LoadFile->LoadFile (LoadFile, FilePath, TRUE, &BufferSize, NULL);
  if ((Status != EFI_BUFFER_TOO_SMALL) && (Status != EFI_WARN_FILE_SYSTEM)) {
    goto ON_EXIT;
  }
  if (Status == EFI_BUFFER_TOO_SMALL) {
    //
    // The load option buffer is directly returned by LoadFile.
    // **HttpBootDxe maybe updates the Device to append the real URL in DevicePath on HttpBootHandle.
    //
    goto ON_EXIT;
  }

  //
  // The load option resides in a RAM disk.
  //
  FileBuffer = AllocateReservedPages (EFI_SIZE_TO_PAGES (BufferSize));
  if (FileBuffer == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto ON_EXIT;
  }

  Status = LoadFile->LoadFile (LoadFile, FilePath, TRUE, &BufferSize, FileBuffer);
  if (EFI_ERROR (Status)) {
    FreePages (FileBuffer, EFI_SIZE_TO_PAGES (BufferSize));
    goto ON_EXIT;
  }

  FullPath = ExpandNetworkFileSystem (HttpBootHandle, RamDiskHandle);
  if (FullPath == NULL) {
    //
    // Free the memory occupied by the RAM disk if there is no BlockIo or SimpleFileSystem instance.
    //
    DestroyRamDisk (DevicePathFromHandle (*RamDiskHandle));
    Status = EFI_UNSUPPORTED;
    goto ON_EXIT;
  }

  *ExpandedUriDevicePath = FullPath;
  return EFI_SUCCESS;

ON_EXIT:
  /// This processing will resolve system hang if BootOption's DevicePath changed.
  *ExpandedUriDevicePath = LoadFileDevicePath;
  return Status;
}

/**
 Check the Device path is a file path pointer to UEFI OS boot option.

 @param[in] DevicePath        Device path

 @retval TRUE                 The device path is pointer to  UEFI OS boot opiton.
 @retval FALSE                The device path isn't pointer to  UEFI OS boot opiton.
**/
STATIC
BOOLEAN
IsUefiOsFilePath (
  IN EFI_DEVICE_PATH_PROTOCOL   *DevicePath
  )
{
  EFI_DEVICE_PATH_PROTOCOL      *WorkingDevicePath;
  EFI_HANDLE                    Handle;
  EFI_STATUS                    Status;
  FILEPATH_DEVICE_PATH          *FilePath;

  WorkingDevicePath = DevicePath;

  Status = gBS->LocateDevicePath (
                  &gEfiSimpleFileSystemProtocolGuid,
                  &WorkingDevicePath,
                  &Handle
                  );
  if (!EFI_ERROR (Status) &&
      DevicePathType (WorkingDevicePath) == MEDIA_DEVICE_PATH &&
      DevicePathSubType (WorkingDevicePath) == MEDIA_FILEPATH_DP) {
    //
    //  If file name isn't default removable file name, we consider this file path pointer to UEFI OS
    //
    FilePath = (FILEPATH_DEVICE_PATH *) WorkingDevicePath;
    if ((StrLen (FilePath->PathName) != StrLen (EFI_REMOVABLE_MEDIA_FILE_NAME)) ||
        (StrCmp (FilePath->PathName, EFI_REMOVABLE_MEDIA_FILE_NAME) != 0)) {
      return TRUE;
    }
  }

  return FALSE;
}

/**
  Initalize H2O_BDS_CP_BOOT_AFTER_DATA data and trigger gH2OBdsCpBootAfterGuid checkpoint.

  @retval EFI_SUCCESS             Trigger gH2OBdsCpBootAfterGuid checkpoint successfully.
  @return Other                   Other error occurred while triggering gH2OBdsCpBootAfterGuid checkpoint.
**/
STATIC
EFI_STATUS
TriggerCpBootAfter (
  VOID
  )
{
  H2O_BDS_CP_BOOT_AFTER_DATA    BdsCpBootAfterData;
  EFI_STATUS                    Status;

  BdsCpBootAfterData.Size   = sizeof (H2O_BDS_CP_BOOT_AFTER_DATA);
  BdsCpBootAfterData.Status = H2O_BDS_TASK_NORMAL;

  DEBUG ((DEBUG_INFO, "Checkpoint Trigger: %g\n", &gH2OBdsCpBootAfterGuid));
  Status = H2OCpTrigger (&gH2OBdsCpBootAfterGuid, &BdsCpBootAfterData);
  DEBUG ((DEBUG_INFO, "Checkpoint Result: %x\n", BdsCpBootAfterData.Status));
  return Status;
}

/**
  Initalize H2O_BDS_CP_BOOT_BEFORE_DATA data and trigger gH2OBdsCpBootBeforeGuid checkpoint.

  @retval EFI_SUCCESS             Trigger gH2OBdsCpBootBeforeGuid checkpoint successfully.
  @return Other                   Other error occurred while triggering gH2OBdsCpBootBeforeGuid checkpoint.
**/
STATIC
EFI_STATUS
TriggerCpBootBefore (
  VOID
  )
{
  H2O_BDS_CP_BOOT_BEFORE_DATA   BdsCpBootBeforeData;
  EFI_STATUS                    Status;

  BdsCpBootBeforeData.Size   = sizeof (H2O_BDS_CP_BOOT_BEFORE_DATA);
  BdsCpBootBeforeData.Status = H2O_BDS_TASK_NORMAL;

  DEBUG ((DEBUG_INFO, "Checkpoint Trigger: %g\n", &gH2OBdsCpBootBeforeGuid));
  Status = H2OCpTrigger (&gH2OBdsCpBootBeforeGuid, &BdsCpBootBeforeData);
  DEBUG ((DEBUG_INFO, "Checkpoint Result: %x\n", BdsCpBootBeforeData.Status));
  return Status;
}

/**
  Internal function to create new H2O_BDS_LOAD_OPTION by input load order.

  @param[in]  OptionType         BDS load option type.
  @param[in]  LoadOrder          Unsigned integer that specifies the current boot option being booted.
  @param[in]  LoadOrderVarGuid   A pointer to the vendor variable GUID.

  @return pointer to H2O_BDS_LOAD_OPTION instance or return NULL if create new boot option failed.
**/
STATIC
H2O_BDS_LOAD_OPTION *
GetNewBdsLoadOption (
  IN       UINT8             OptionType,
  IN       UINT16            LoadOrder,
  IN CONST EFI_GUID          *LoadOrderVarGuid
  )
{
  H2O_BDS_LOAD_OPTION        *LoadOption;
  UINT16                     LoadOptionName[OPTION_VAR_NAME_BUFFER_CHAR_COUNT];
  H2O_BDS_OPTION_TYPE_INFO   *OptionTypeInfo;

  OptionTypeInfo = GetOptionTypeInfo (OptionType);
  if (OptionTypeInfo == NULL) {
    return NULL;
  }

  LoadOption = NULL;
  UnicodeSPrint (LoadOptionName, sizeof (LoadOptionName), OPTION_VAR_NAME_FORMAT_STR, OptionTypeInfo->OptionVarPrefixName, LoadOrder);
  BdsServicesConvertVarToLoadOption (&mH2OBdsServices, LoadOptionName, LoadOrderVarGuid, &LoadOption);
  return LoadOption;
}

/**
  Internal function to check the load option list and order variable info are matched.

  @param[in] OptionType         BDS load option type.
  @param[in] OrderVarInfo       A pointer to order variable info.

  @retval TRUE            The load option list data is correct.
  @retval FALSE           The load option list data is incorrect.
**/
STATIC
BOOLEAN
IsLoadOptionListCorrect (
  IN       UINT8                   OptionType,
  IN CONST H2O_BDS_ORDER_VAR_INFO  *OrderVarInfo
  )
{
  H2O_BDS_OPTION_TYPE_INFO   *OptionTypeInfo;
  LIST_ENTRY                 *OptionList;
  LIST_ENTRY                 *Link;
  UINTN                      Index;
  H2O_BDS_LOAD_OPTION        *CurrentLoadOption;


  if (OrderVarInfo == NULL) {
    return FALSE;
  }

  OptionTypeInfo = GetOptionTypeInfo (OptionType);
  if (OptionTypeInfo == NULL) {
    return FALSE;
  }

  Index = 0;
  OptionList = &OptionTypeInfo->OptionList;
  for (Link = GetFirstNode (OptionList); !IsNull (OptionList, Link); Link = GetNextNode (OptionList, Link)) {
    if (Index >= OrderVarInfo->OptionCount) {
      return FALSE;
    }
    CurrentLoadOption = BDS_OPTION_FROM_LINK (Link);
    if (!IsLoadOptionMatch (CurrentLoadOption, OrderVarInfo->OptionOrder[Index], &OrderVarInfo->OptionVarGuid[Index])) {
      return FALSE;
    }
    Index++;
  }

  return Index == OrderVarInfo->OptionCount ? TRUE : FALSE;
}


/**
  Internal function to remove all of load options in load option list.

  @param[in] OptionType         BDS load option type.
**/
STATIC
VOID
RemoveAllLoadOptions (
  IN UINT8                   OptionType
  )
{
  H2O_BDS_OPTION_TYPE_INFO   *OptionTypeInfo;
  LIST_ENTRY                 *OptionList;
  LIST_ENTRY                 *Link;
  H2O_BDS_LOAD_OPTION        *CurrentLoadOption;

  OptionTypeInfo = GetOptionTypeInfo (OptionType);
  if (OptionTypeInfo == NULL) {
    return;
  }

  OptionList = &OptionTypeInfo->OptionList;
  while (!IsListEmpty (OptionList)) {
    Link = GetFirstNode (OptionList);
    RemoveEntryList (Link);
    CurrentLoadOption = BDS_OPTION_FROM_LINK (Link);
    BdsServicesFreeLoadOption (&mH2OBdsServices, CurrentLoadOption);
  }
}

/**
  Internal function to check if BDS load option is stale data.

  @param[in]  LoadOption   A pointer to BDS load option.

  @retval TRUE      The BDS load option is stale data.
  @retval FALSE     The BDS load option isn't stale data.
**/
STATIC
BOOLEAN
IsLoadOptionStaleData (
  IN H2O_BDS_LOAD_OPTION    *LoadOption
  )
{
  CHAR16                       OptionName[OPTION_VAR_NAME_BUFFER_CHAR_COUNT];
  EFI_STATUS                   Status;
  UINT8                        *EfiLoadOption;
  UINTN                        EfiLoadOptionSize;
  UINT8                        *WorkingPtr;
  CHAR16                       *Description;
  EFI_DEVICE_PATH_PROTOCOL     *DevicePath;
  H2O_BDS_OPTION_TYPE_INFO     *OptionTypeInfo;
  UINT32                       Attributes;
  UINT16                       DevicePathLength;
  UINTN                        UsedSize;
  UINT8                        *OptionalData;
  UINT32                       OptionalDataSize;

  if (LoadOption == NULL) {
    return TRUE;
  }

  OptionTypeInfo = GetOptionTypeInfo (LoadOption->OptionType);
  if (OptionTypeInfo == NULL) {
    return TRUE;
  }
  UnicodeSPrint (OptionName, sizeof (OptionName), OPTION_VAR_NAME_FORMAT_STR, OptionTypeInfo->OptionVarPrefixName, LoadOption->LoadOrder);

  Status = CommonGetVariableDataAndSize (
             OptionName,
             &LoadOption->LoadOrderVarGuid,
             &EfiLoadOptionSize,
             (VOID **) &EfiLoadOption
             );
  if (Status != EFI_SUCCESS) {
    return TRUE;
  }

  WorkingPtr = (UINT8 *) EfiLoadOption;
  Attributes = *((UINT32 *) WorkingPtr);
  WorkingPtr += sizeof (UINT32);
  DevicePathLength = *((UINT16 *) WorkingPtr);
  WorkingPtr += sizeof (UINT16);
  Description = (CHAR16 *) WorkingPtr;
  WorkingPtr += StrSize (Description);
  DevicePath = (EFI_DEVICE_PATH_PROTOCOL *) WorkingPtr;
  WorkingPtr += DevicePathLength;
  UsedSize = (UINTN) (WorkingPtr - (UINT8 *) EfiLoadOption);
  if (EfiLoadOptionSize > UsedSize) {
    OptionalDataSize = (UINT32) (EfiLoadOptionSize - UsedSize);
    OptionalData = WorkingPtr;
  } else {
    OptionalData = NULL;
    OptionalDataSize = 0;
  }

  if (LoadOption->Attributes != Attributes ||
      StrCmp (LoadOption->Description, Description) != 0 ||
      GetDevicePathSize (LoadOption->DevicePath) != GetDevicePathSize (DevicePath) ||
      CompareMem (LoadOption->DevicePath, DevicePath, GetDevicePathSize (DevicePath)) != 0 ||
      LoadOption->LoadOptionalDataSize != OptionalDataSize ||
      (OptionalDataSize != 0 && CompareMem (LoadOption->LoadOptionalData, OptionalData, OptionalDataSize) != 0)
      ) {
    FreePool (EfiLoadOption);
    return TRUE;
  }

  FreePool (EfiLoadOption);
  return FALSE;
}

/**
  Synchronize the load option list from variable.

  @param[in] OptionType      Option type that specifies which option list to be synchronized.

  @retval EFI_SUCCESS        Synchronize boot list successfully.
  @retval EFI_UNSUPPORTED    Fail to find option type info.
**/
STATIC
EFI_STATUS
SyncBdsLoadList (
  IN UINT8                   OptionType
  )
{
  H2O_BDS_OPTION_TYPE_INFO   *OptionTypeInfo;
  H2O_BDS_ORDER_VAR_INFO     OrderVarInfo;
  LIST_ENTRY                 *OptionList;
  LIST_ENTRY                 *Link;
  LIST_ENTRY                 *CurrentLink;
  H2O_BDS_LOAD_OPTION        *CurrentLoadOption;
  UINTN                      Index;
  EFI_STATUS                 Status;

  OptionTypeInfo = GetOptionTypeInfo (OptionType);
  if (OptionTypeInfo == NULL) {
    return EFI_UNSUPPORTED;
  }

  Status = GetOrderVarInfo (OptionType, &OrderVarInfo);
  if (EFI_ERROR (Status)) {
    ZeroMem (&OrderVarInfo, sizeof (OrderVarInfo));
  }

  //
  // Remove redundant bds load option in option list.
  //
  OptionList = &OptionTypeInfo->OptionList;
  for (Link = GetFirstNode (OptionList); !IsNull (OptionList, Link); Link = GetNextNode (OptionList, Link)) {
    CurrentLoadOption = BDS_OPTION_FROM_LINK (Link);
    if (!IsLoadOptionInOrderVar (CurrentLoadOption, &OrderVarInfo) || IsLoadOptionStaleData (CurrentLoadOption)) {
      CurrentLink = Link;
      Link = CurrentLink->BackLink;
      RemoveEntryList (&CurrentLoadOption->Link);
      BdsServicesFreeLoadOption  (&mH2OBdsServices, CurrentLoadOption);
    }
  }
  //
  // Add BDS load option.
  //
  for (Index = 0; Index < OrderVarInfo.OptionCount; Index++) {
    if (!IsOptionNumInOptionList (OrderVarInfo.OptionOrder[Index], &OrderVarInfo.OptionVarGuid[Index], OptionList)) {
      CurrentLoadOption = GetNewBdsLoadOption (OptionType, OrderVarInfo.OptionOrder[Index], &OrderVarInfo.OptionVarGuid[Index]);
      if (CurrentLoadOption == NULL) {
        continue;
      }

      InsertLoadOptionToList (CurrentLoadOption, &OrderVarInfo, Index);
    }
  }

  if (!IsLoadOptionListCorrect (OptionType, &OrderVarInfo)) {
    if (mNeedSyncBootOrder) {
      mNeedSyncBootOrder = FALSE;
      RemoveAllLoadOptions (OptionType);
      SyncBdsLoadList (OptionType);
      for (Link = GetFirstNode (OptionList); !IsNull (OptionList, Link); Link = GetNextNode (OptionList, Link)) {
        CurrentLoadOption = BDS_OPTION_FROM_LINK (Link);
        CurrentLoadOption->Connected = TRUE;
      }
    }
  }
  mNeedSyncBootOrder = TRUE;

  FreeOrderVarInfo (&OrderVarInfo);
  if (OptionType != H2O_BDS_LOAD_OPTION_TYPE_BOOT) {
    return EFI_SUCCESS;
  }
  //
  // Expand all of BDS boot option
  //
  for (Link = GetFirstNode (OptionList); !IsNull (OptionList, Link); Link = GetNextNode (OptionList, Link)) {
    CurrentLoadOption = BDS_OPTION_FROM_LINK (Link);
    BdsServicesExpandLoadOption  (&mH2OBdsServices, CurrentLoadOption);
  }
  return EFI_SUCCESS;
}

/**
  Return linked list of BDS load options derived from the UEFI order and option variables.

  @param[in] This                Option type value.
  @param[in] OptionList          A pointer to a pointer to the first list entry in a linked list. Each list entry
                                 describes a single BDS load option using a "H2O_BDS_LOAD_OPTION" structure.
                                 This structure can be retrieved from the list entry using the "BDS_OPTION_FROM_LINK()" macro.

  @retval EFI_SUCCESS            The function completed successfully.
  @retval EFI_INVALID_PARAMETER  OptionList is set to NULL.
  @retval EFI_UNSUPPORTED        Failed to find OS recovery info.
**/
STATIC
EFI_STATUS
GetBdsLoadList (
  IN  UINT8                      OptionType,
  OUT LIST_ENTRY                 **OptionList
  )
{
  H2O_BDS_OPTION_TYPE_INFO       *OptionTypeInfo;

  if (OptionList == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  OptionTypeInfo = GetOptionTypeInfo (OptionType);
  if (OptionTypeInfo == NULL) {
    return EFI_UNSUPPORTED;
  }

  SyncBdsLoadList (OptionType);
  *OptionList = &OptionTypeInfo->OptionList;
  return EFI_SUCCESS;
}

/**
  According to order in Boot list to update boot priority in BBS table.

  @param[in] *LegacyBios   A pointer to EFI_LEGACY_BIOS_PROTOCOL instance.

  @retval EFI_SUCCESS            Update boot priority successfully.
  @retval EFI_INVALID_PARAMETER  LegacyBios is NULL.
  @retval EFI_UNSUPPORTED        Fail to find option type info.
**/
STATIC
EFI_STATUS
UpdateBbsPriority (
  IN  EFI_LEGACY_BIOS_PROTOCOL          *LegacyBios
  )
{
  BBS_TABLE                     *LocalBbsTable;
  UINT16                        BbsIndex;
  UINT16                        PriorityIndex;
  LIST_ENTRY                    *OptionList;
  LIST_ENTRY                    *Link;
  H2O_BDS_LOAD_OPTION           *BootOption;
  BOOLEAN                       BootCurrentFound;
  H2O_BDS_OPTION_TYPE_INFO      *OptionTypeInfo;

  if (LegacyBios == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  OptionTypeInfo = GetOptionTypeInfo (H2O_BDS_LOAD_OPTION_TYPE_BOOT);
  if (OptionTypeInfo == NULL) {
    return EFI_UNSUPPORTED;
  }

  BootCurrentFound = FALSE;
  OptionList       = &OptionTypeInfo->OptionList;
  for (Link = GetFirstNode (OptionList), PriorityIndex = 0; !IsNull (OptionList, Link);
       Link = GetNextNode (OptionList, Link)) {
    BootOption = BDS_OPTION_FROM_LINK (Link);
    if (!IsLegacyBootOption (BootOption->DevicePath)) {
      continue;
    }
    BbsIndex = *((UINT16 *) ((UINT8 *) BootOption->LoadOptionalData + sizeof (BBS_TABLE)));
    LegacyBios->GetBbsInfo (LegacyBios, NULL, NULL, NULL, &LocalBbsTable);
    LocalBbsTable[BbsIndex].BootPriority = (UINT16) PriorityIndex;
    if (!BootCurrentFound) {
      if (BootOption->LoadOrder == mH2OBdsServices.BootCurrentLoadOption->LoadOrder) {
        LocalBbsTable[BbsIndex].BootPriority = 0;
        BootCurrentFound = TRUE;
      } else {
        LocalBbsTable[BbsIndex].BootPriority++;
      }
    }
    PriorityIndex++;
  }

  return EFI_SUCCESS;
}

/**
  Boot the legacy system with the boot option

  @param  Option                 The legacy boot option which have BBS device path

  @retval EFI_SUCCESS            It will never return EFI_SUCCESS. If boot success, system will enter
                                 legacy OS directly.
  @retval EFI_UNSUPPORTED        There is no legacybios protocol, do not support legacy boot.
  @retval Other                  Return the status of LegacyBios->LegacyBoot ().
**/
EFI_STATUS
LauchLegacyBootOption (
  IN  H2O_BDS_LOAD_OPTION         *Option
  )
{
  EFI_STATUS                Status;
  EFI_LEGACY_BIOS_PROTOCOL  *LegacyBios;

  Status = gBS->LocateProtocol (&gEfiLegacyBiosProtocolGuid, NULL, (VOID **) &LegacyBios);
  if (EFI_ERROR (Status)) {
    //
    // If no LegacyBios protocol we do not support legacy boot
    //
    return EFI_UNSUPPORTED;
  }
  //
  // Notes: if we separate the int 19, then we don't need to refresh BBS
  //
  // to set BBS Table priority
  //
  UpdateBbsPriority (LegacyBios);
  //
  // Write boot to OS performance data for legacy boot.
  //
  WRITE_BOOT_TO_OS_PERFORMANCE_DATA;

  DEBUG ((DEBUG_INFO | DEBUG_LOAD, "Legacy Boot: %S\n", Option->Description));
  return LegacyBios->LegacyBoot (
                      LegacyBios,
                      (BBS_BBS_DEVICE_PATH *) Option->DevicePath,
                      Option->LoadOptionalDataSize,
                      Option->LoadOptionalData
                      );
}

/**
  Callback function to clear screen to prevent from screen has some garbage after
  booting to shell environment.

  @param[in] Event    Event whose notification function is being invoked.
  @param[in] Context  Pointer to the notification function's context.
**/
VOID
EFIAPI
ShellEnvProtocolCallback (
  IN EFI_EVENT                          Event,
  IN VOID                               *Context
  )
{
  gST->ConOut->ClearScreen(gST->ConOut);
}

/**
  According to device path and image handle to boot.

  @param[in]  ImageHandle       Image handle.
  @param[in]  Option            The boot option need to be processed.
  @param[out] ExitDataSize      Returned directly from gBS->StartImage ().
  @param[out] ExitData          Returned directly from gBS->StartImage ().

  @retval EFI_SUCCESS          Boot from recovery boot option successfully.
  @retval Other                Some errors occured during boot process.
**/
STATIC
EFI_STATUS
LaunchBootImage (
  IN  EFI_HANDLE                    ImageHandle,
  IN  H2O_BDS_LOAD_OPTION           *Option,
  OUT UINTN                         *ExitDataSize,
  OUT CHAR16                        **ExitData OPTIONAL
  )
{
  EFI_STATUS                        Status;
  EFI_LOADED_IMAGE_PROTOCOL         *ImageInfo;
  EFI_EVENT                         ShellImageEvent;


  Status = gBS->HandleProtocol (ImageHandle, &gEfiLoadedImageProtocolGuid, (VOID **) &ImageInfo);
  ASSERT_EFI_ERROR (Status);
  if (Status != EFI_SUCCESS) {
    return Status;
  }

  if (Option->LoadOptionalDataSize != 0 &&
      Option->LoadOptionalData != NULL &&
      !((Option->LoadOptionalDataSize == 2 || Option->LoadOptionalDataSize == 6) &&
        AsciiStrnCmp ((CHAR8 *) Option->LoadOptionalData, "RC", 2) == 0)) {
    ImageInfo->LoadOptionsSize  = Option->LoadOptionalDataSize;
    ImageInfo->LoadOptions      = Option->LoadOptionalData;
  }

  //
  // Register Event for Shell Image
  //
  if (mShellEnvProtocolCallbackReg == NULL) {
    Status = gBS->CreateEvent (
                    EVT_NOTIFY_SIGNAL,
                    TPL_CALLBACK,
                    ShellEnvProtocolCallback,
                    NULL,
                    &ShellImageEvent
                    );
    if (!EFI_ERROR (Status)) {
      Status = gBS->RegisterProtocolNotify (
                      &mShellEnvProtocol,
                      ShellImageEvent,
                      &mShellEnvProtocolCallbackReg
                      );
    }
  }
  if (FeaturePcdGet (PcdH2OBdsCpBootBeforeSupported)) {
    TriggerCpBootBefore ();
  }
  //
  // Before calling the image, enable the Watchdog Timer
  //
  gBS->SetWatchdogTimer (PcdGet32 (PcdH2OBdsBootWatchdogTimeOut), 0x0000, 0x00, NULL);

  //
  // PostCode = 0xFB, UEFI Boot Start Image
  //
  POST_CODE (POST_BDS_START_IMAGE);

  PERF_END (0, "PostBDS", NULL, 0);
  WRITE_BOOT_TO_OS_PERFORMANCE_DATA;
  if (FeaturePcdGet (PcdAutoCreateDummyBootOption)) {
    BdsLibChangeToVirtualBootOrder ();
  }

  DEBUG ((EFI_D_INFO | EFI_D_LOAD, "Image Start...\n"));

  //
  // Report status code for OS Loader StartImage.
  //
  REPORT_STATUS_CODE (EFI_PROGRESS_CODE, PcdGet32 (PcdProgressCodeOsLoaderStart));

  Status = gBS->StartImage (ImageHandle, ExitDataSize, ExitData);
  DEBUG ((EFI_D_INFO | EFI_D_LOAD, "Image Return Status = %r\n", Status));

  //
  // Clear the Watchdog Timer after the image returns
  //
  gBS->SetWatchdogTimer (0x0000, 0x0000, 0x0000, NULL);
  if (FeaturePcdGet (PcdAutoCreateDummyBootOption)) {
    SyncBootOrder ();
    BdsLibRestoreBootOrderFromPhysicalBootOrder ();
  }
  if (FeaturePcdGet (PcdH2OBdsCpBootAfterSupported)) {
    TriggerCpBootAfter ();
  }
  return Status;
}

/**
  According to device path  to boot from reocvery boot option.

  @param[in]  Option           The boot option need to be processed.
  @param[in]  ExitDataSize     Returned directly from gBS->StartImage ().
  @param[in]  ExitData         Returned directly from gBS->StartImage ().

  @retval EFI_SUCCESS          Boot from recovery boot option successfully.
  @retval Other                Some errors occured during boot process.
**/
STATIC
EFI_STATUS
LaunchRecoveryOption (
  IN  H2O_BDS_LOAD_OPTION           *Option,
  OUT UINTN                         *ExitDataSize,
  OUT CHAR16                        **ExitData OPTIONAL
  )
{
  EFI_DEVICE_PATH_PROTOCOL  *DevicePath;
  EFI_DEVICE_PATH_PROTOCOL  *FilePath;
  EFI_HANDLE                Handle;
  EFI_STATUS                Status;
  EFI_HANDLE                ImageHandle;


  DevicePath = Option->DevicePath;
  Status = gBS->LocateDevicePath (
                  &gEfiSimpleFileSystemProtocolGuid,
                  &DevicePath,
                  &Handle
                  );
  if (!EFI_ERROR (Status)) {
    FilePath = FileDevicePath (Handle, EFI_REMOVABLE_MEDIA_FILE_NAME);
    if (FilePath != NULL) {
      Status = gBS->LoadImage (
                      TRUE,
                      gImageHandle,
                      FilePath,
                      NULL,
                      0,
                      &ImageHandle
                      );
      if (!EFI_ERROR (Status)) {
        Status = LaunchBootImage (ImageHandle, Option, ExitDataSize, ExitData);
      }

      FreePool (FilePath);
    }
  }

  return Status;
}

/**
  Launch a BDS boot option.

  @param[in]  BdsLoadOption      A pointer to BDS load option.
  @param[out] ExitDataSize       Pointer to the size, in bytes, of ExitData.
  @param[out] ExitData           Pointer to a pointer to a data buffer that includes a Null-terminated
                                 string, optionally followed by additional binary data.

  @retval EFI_SUCCESS            Boot from the input boot option successfully.
  @retval EFI_INVALID_PARAMETER  BdsLoadOption is NULL.
  @retval EFI_NOT_FOUND          If the Device Path is not found in the system
**/
STATIC
EFI_STATUS
LaunchBootOption (
  IN OUT  H2O_BDS_LOAD_OPTION      *BdsLoadOption,
  OUT     UINTN                    *ExitDataSize,
  OUT     CHAR16                   **ExitData      OPTIONAL
  )
{
  EFI_STATUS                                      Status;
  EFI_STATUS                                      RecoveryBootStatus;
  EFI_HANDLE                                      Handle;
  EFI_HANDLE                                      ImageHandle;
  EFI_DEVICE_PATH_PROTOCOL                        *TempDevicePath;
  EFI_DEVICE_PATH_PROTOCOL                        *RemovableMediaFilePath;
  EFI_BLOCK_IO_PROTOCOL                           *BlkIo;
  VOID                                            *Buffer;
  EFI_STATUS                                      LocateDevicePathStatus;
  H2O_BDS_LOAD_OPTION                             *CurrentBootOption;
  LIST_ENTRY                                      *Link;
  BOOLEAN                                         IsLegacyBoot;

  EFI_DEVICE_PATH                                 *UpdatedHttpDevicePath;
  EFI_HANDLE                                      RamDiskHandle;
  EFI_DEVICE_PATH_PROTOCOL                        *CurrentBootDevicePath;

  //
  // All the driver options should have been processed since now boot will be performed.
  //
  PERF_END (0, BDS_TOK, NULL, 0);

  PERF_START (0, "PostBDS", NULL, 0);

  if (BdsLoadOption == NULL || BdsLoadOption->DevicePath == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Before boot to device, always clean BootNext variable.
  //
  CommonSetVariable (L"BootNext", &gEfiGlobalVariableGuid, 0, 0, NULL);

  IsLegacyBoot = (BOOLEAN) ((DevicePathType (BdsLoadOption->DevicePath) == BBS_DEVICE_PATH) &&
                            (DevicePathSubType (BdsLoadOption->DevicePath) == BBS_BBS_DP));
  if (!IsLegacyBoot) {
    EnableOptimalTextMode ();
  }

  //
  // Follow EDKII policy, Set "BootCurrent" variable before ready to boot event
  //
  if (BdsLoadOption->LoadOrderVarName[0] == L'B') {
    //
    // For a temporary boot (i.e. a boot by selected a EFI Shell using "Boot From File"),
    // Boot Current is actually not valid.
    // In this case, "BootCurrent" is not created.
    // Only create the BootCurrent variable when it points to a valid Boot#### variable.
    //
    CommonSetVariable (
      EFI_BOOT_CURRENT_VARIABLE_NAME,
      &gEfiGlobalVariableGuid,
      EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
      sizeof (UINT16),
      &BdsLoadOption->LoadOrder
      );
    BdsServicesConvertVarToLoadOption (
      &mH2OBdsServices,
      EFI_BOOT_CURRENT_VARIABLE_NAME,
      &gEfiGlobalVariableGuid,
      &mH2OBdsServices.BootCurrentLoadOption
      );
  }

  if ((BdsLoadOption->Attributes & LOAD_OPTION_CATEGORY) == LOAD_OPTION_CATEGORY_BOOT) {
    if (FeaturePcdGet (PcdH2OBdsCpReadyToBootBeforeSupported)) {
      TriggerCpReadyToBootBefore ();
    }
    //
    // PostCode = 0x2E, Last Chipset initial before boot to OS
    //
    POST_CODE (BDS_READY_TO_BOOT_EVENT);
    //
    // Signal the EVT_SIGNAL_READY_TO_BOOT event
    //
    EfiSignalEventReadyToBoot();
    if (FeaturePcdGet (PcdH2OBdsCpReadyToBootAfterSupported)) {
      TriggerCpReadyToBootAfter ();
    }

    if (FeaturePcdGet (PcdMemoryMapConsistencyCheck)) {
      CheckRtAndBsMemUsage ();
    }
  }

  if (IsLegacyBoot) {
    if (!FeaturePcdGet (PcdH2OCsmSupported)) {
      return EFI_UNSUPPORTED;
    }
    //
    // TODO: Many CRB doesn't support CSM, we need remove legacy related code in BDS to reduce code size in the future.
    //
    // Check to see if we should legacy BOOT. If yes then do the legacy boot
    //
    if (FeaturePcdGet (PcdAutoCreateDummyBootOption) && BdsLibIsBootOrderHookEnabled ()) {
      BdsLibRestoreBootOrderFromPhysicalBootOrder ();
    }
    //
    // PostCode = 0x2F, Start to boot Legacy OS
    //
    POST_CODE (BDS_GO_LEGACY_BOOT);
    return LauchLegacyBootOption (BdsLoadOption);
  }
  //
  // PostCode = 0x30, Start to boot UEFI OS
  //
  POST_CODE (BDS_GO_UEFI_BOOT);

#ifndef MDEPKG_NDEBUG
  DumpMemoryMap();
#endif

  DEBUG_CODE_BEGIN();
  if (BdsLoadOption->Description == NULL) {
    DEBUG ((DEBUG_INFO | DEBUG_LOAD, "Booting from unknown device path\n"));
  } else {
    DEBUG ((DEBUG_INFO | DEBUG_LOAD, "Booting %S\n", BdsLoadOption->Description));
  }
  DEBUG_CODE_END();

  //
  // Report status code for OS Loader LoadImage.
  //
  REPORT_STATUS_CODE (EFI_PROGRESS_CODE, PcdGet32 (PcdProgressCodeOsLoaderLoad));

  if (IsPxeBoot (BdsLoadOption->DevicePath)) {
    DisableQuietBoot ();
  }

  CurrentBootOption = BdsLoadOption;
  if (BdsLoadOption->Expanded) {
    if (IsListEmpty (&BdsLoadOption->ExpandedLoadOptions)) {
      return EFI_NOT_FOUND;
    }
    Link = GetFirstNode (&BdsLoadOption->ExpandedLoadOptions);
    CurrentBootOption = BDS_OPTION_FROM_LINK (Link);
  }

  Status = EFI_NOT_FOUND;
  RemovableMediaFilePath = NULL;
  ImageHandle   = NULL;
  RamDiskHandle = NULL;
  while (CurrentBootOption != NULL) {
    RamDiskHandle         = NULL;
    UpdatedHttpDevicePath = NULL;
    CurrentBootDevicePath = CurrentBootOption->DevicePath;
    if (IsHttpBootHandle (CurrentBootOption->DevicePath)) {
      //
      // If HTTP boot load the EFI file, the status will be EFI_BUFFER_TOO_SMALL.
      // The CurrentBootOption->DevicePath need to update if HttpBooDxe update the DevicePath.
      // GetDevicePathSize return 0, it means the DevicePath is invalid, so CurrentBootOption->DevicePath should be update.
      //
      Status = ProcessHttpUriBoot (CurrentBootOption->DevicePath, &UpdatedHttpDevicePath, &RamDiskHandle);
      CurrentBootOption->DevicePath = UpdatedHttpDevicePath;
      //
      // Buffer too small means HttpBootDxe will to load PE application, not ISO file.
      //
      if (EFI_ERROR (Status) && (Status != EFI_BUFFER_TOO_SMALL)) {
        break;
      }
      //
      // Update boot device path to file path of RamDisk\SimpleFileSystem\efi\boot\boot$(ARCH).efi
      //
      CurrentBootDevicePath = UpdatedHttpDevicePath;
    }

    Status = gBS->LoadImage (
                    TRUE,
                    gImageHandle,
                    CurrentBootDevicePath,
                    NULL,
                    0,
                    &ImageHandle
                    );
    //
    // If we didn't find an image, we may need to load the default boot behavior for the device.
    //
    if (EFI_ERROR (Status)) {
      //
      // Find a Simple File System protocol on the device path.
      // If the remaining device path is set to end then no Files are being specified, so try the removable media file name.
      //
      TempDevicePath = CurrentBootOption->DevicePath;
      LocateDevicePathStatus = gBS->LocateDevicePath (
                                      &gEfiSimpleFileSystemProtocolGuid,
                                      &TempDevicePath,
                                      &Handle
                                      );
      if (!EFI_ERROR (LocateDevicePathStatus) && IsDevicePathEnd (TempDevicePath)) {
        RemovableMediaFilePath = FileDevicePath (Handle, EFI_REMOVABLE_MEDIA_FILE_NAME);
        if (RemovableMediaFilePath != NULL) {
          //
          // Issue a dummy read to the device to check for media change.
          // When the removable media is changed, any Block IO read/write will cause the BlockIo protocol be reinstalled and EFI_MEDIA_CHANGED is returned.
          // After the Block IO protocol is reinstalled, subsequent Block IO read/write will success.
          //
          Status = gBS->HandleProtocol (
                          Handle,
                          &gEfiBlockIoProtocolGuid,
                          (VOID **) &BlkIo
                          );
          if (!EFI_ERROR (Status)) {
            Buffer = AllocatePool (BlkIo->Media->BlockSize);
            if (Buffer != NULL) {
              BlkIo->ReadBlocks (
                       BlkIo,
                       BlkIo->Media->MediaId,
                       0,
                       BlkIo->Media->BlockSize,
                       Buffer
                       );
              FreePool (Buffer);
            }
          }

          Status = gBS->LoadImage (
                          TRUE,
                          gImageHandle,
                          RemovableMediaFilePath,
                          NULL,
                          0,
                          &ImageHandle
                          );
          if (EFI_ERROR (Status)) {
            FreePool (RemovableMediaFilePath);
            RemovableMediaFilePath = NULL;
          }
        }
      }
    }
    if (Status == EFI_SUCCESS || !BdsLoadOption->Expanded) {
      break;
    }

    Link = GetNextNode (&BdsLoadOption->ExpandedLoadOptions, &CurrentBootOption->Link);
    if (Link == &BdsLoadOption->ExpandedLoadOptions) {
      CurrentBootOption = BDS_OPTION_FROM_LINK (BdsLoadOption->ExpandedLoadOptions.BackLink);
      break;
    }

    CurrentBootOption = BDS_OPTION_FROM_LINK (Link);
  }

  if (Status == EFI_SUCCESS) {
    //
    // Provide the image with it's load options
    //
    Status = LaunchBootImage (ImageHandle, CurrentBootOption, ExitDataSize, ExitData);

    //
    // Destroy HTTP boot created RamDisk to avoid it was out of manage.
    //
    if (RamDiskHandle != NULL) {
      DestroyRamDisk (DevicePathFromHandle(RamDiskHandle));
    }
  }
  if (EFI_ERROR (Status) && RemovableMediaFilePath == NULL && IsUefiOsFilePath (CurrentBootOption->DevicePath)) {
    //
    // Try to boot from recovery boot option and only update status to EFI_SUCCESS,
    // if boot from this recovery file path is successful.
    //
    RecoveryBootStatus = LaunchRecoveryOption (CurrentBootOption, ExitDataSize, ExitData);
    if (!EFI_ERROR (RecoveryBootStatus)) {
      Status = EFI_SUCCESS;
    }
  }
  if (EFI_ERROR (Status) && IS_HW_VENDOR_DEVICE_PATH (CurrentBootOption->DevicePath)) {
    //
    // Always return success if launch boot option by specific vendor device path.
    //
    Status = EFI_SUCCESS;
  }

  //
  // Clear BootCurrent and set BootPrev variables.
  //
  CommonSetVariable (EFI_BOOT_CURRENT_VARIABLE_NAME, &gEfiGlobalVariableGuid, 0, 0, NULL);
  if (mH2OBdsServices.BootCurrentLoadOption != NULL) {
    BdsServicesFreeLoadOption (&mH2OBdsServices, mH2OBdsServices.BootCurrentLoadOption);
    mH2OBdsServices.BootCurrentLoadOption = NULL;
  }
  CommonSetVariable (
    H2O_BOOT_PREVIOUS_VARIABLE_NAME,
    &gEfiGenericVariableGuid,
    EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
    sizeof (UINT16),
    &BdsLoadOption->LoadOrder
    );
  if (mH2OBdsServices.BootPrevLoadOption != NULL) {
    BdsServicesFreeLoadOption (&mH2OBdsServices, mH2OBdsServices.BootPrevLoadOption);
    mH2OBdsServices.BootPrevLoadOption = NULL;
  }
  BdsServicesConvertVarToLoadOption (
    &mH2OBdsServices,
    H2O_BOOT_PREVIOUS_VARIABLE_NAME,
    &gEfiGenericVariableGuid,
    &mH2OBdsServices.BootPrevLoadOption
    );
  //
  // Signal BIOS after the image returns
  //
  SignalImageReturns ();

  if (RemovableMediaFilePath != NULL) {
    FreePool (RemovableMediaFilePath);
  }

  return Status;
}

/**
  Launch a BDS driver option.

  @param[in]  BdsLoadOption      A pointer to BDS load option.
  @param[out] ExitDataSize       Pointer to the size, in bytes, of ExitData.
  @param[out] ExitData           Pointer to a pointer to a data buffer that includes a Null-terminated
                                 string, optionally followed by additional binary data.

  @retval EFI_SUCCESS            Boot from the input boot option successfully.
  @retval EFI_INVALID_PARAMETER  BdsLoadOption is NULL.
  @retval EFI_NOT_FOUND          If the Device Path is not found in the system
**/
STATIC
EFI_STATUS
LaunchDriverOption (
  IN OUT  H2O_BDS_LOAD_OPTION      *BdsLoadOption,
  OUT     UINTN                    *ExitDataSize,
  OUT     CHAR16                   **ExitData      OPTIONAL
  )
{
  EFI_STATUS                Status;
  EFI_HANDLE                ImageHandle;
  EFI_LOADED_IMAGE_PROTOCOL *ImageInfo;

  if (BdsLoadOption == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if ((BdsLoadOption->Attributes & LOAD_OPTION_ACTIVE) != LOAD_OPTION_ACTIVE) {
    return EFI_SUCCESS;
  }
  //
  // Make sure the driver path is connected.
  //
  BdsLibConnectDevicePath (BdsLoadOption->DevicePath);

  //
  // Load and start the image that Driver#### describes
  //
  Status = gBS->LoadImage (
                  FALSE,
                  gImageHandle,
                  BdsLoadOption->DevicePath,
                  NULL,
                  0,
                  &ImageHandle
                  );
  if (Status != EFI_SUCCESS) {
    return Status;
  }

  gBS->HandleProtocol (ImageHandle, &gEfiLoadedImageProtocolGuid, (VOID **) &ImageInfo);

  if (BdsLoadOption->LoadOptionalDataSize != 0) {
    ImageInfo->LoadOptionsSize  = BdsLoadOption->LoadOptionalDataSize;
    ImageInfo->LoadOptions      = BdsLoadOption->LoadOptionalData;
  }

  Status = gBS->StartImage (ImageHandle, ExitDataSize, ExitData);
  DEBUG ((DEBUG_INFO | DEBUG_LOAD, "Driver Return Status = %r\n", Status));

  return EFI_SUCCESS;
}

/**
  Initialize gH2OBdsServicesProtocolGuid protocol data and install gH2OBdsServicesProtocolGuid protocol.

  @retval EFI_SUCCESS           Install gH2OBdsServicesProtocolGuid protocol successfully.
  @return Others                Any error occurred while installing gH2OBdsServicesProtocolGuid protocol.
**/
EFI_STATUS
InstallH2OBdsServicesProtocol (
  VOID
  )
{
  EFI_HANDLE             Handle;
  EFI_STATUS             Status;
  H2O_BDS_LOAD_OPTION    *LoadOption;
  UINTN                  Index;

  for (Index = 0; Index < sizeof (mOptionTypeInfo) / sizeof (H2O_BDS_OPTION_TYPE_INFO); Index++) {
    InitializeListHead (&mOptionTypeInfo[Index].OptionList);
  }

  Status = BdsServicesConvertVarToLoadOption (&mH2OBdsServices, EFI_BOOT_NEXT_VARIABLE_NAME, &gEfiGlobalVariableGuid, &LoadOption);
  if (Status == EFI_SUCCESS) {
    mH2OBdsServices.BootNextLoadOption = LoadOption;
  }
  Handle = NULL;
  return gBS->InstallMultipleProtocolInterfaces (
                  &Handle,
                  &gH2OBdsServicesProtocolGuid,
                  &mH2OBdsServices,
                  NULL
                  );
}

/**
  Return the current value of the OsIndications and OsIndicationsSupported UEFI variable.

  @param[in]  This                    A Pointer to current instance of this protocol.
  @param[out] OsIndications           A pointer to contain the value of the OsIndications.
  @param[out] OsIndicationsSupported  A pointer to contain the value of the OsIndicationsSupported.

  @retval EFI_SUCCESS            Get OsIndications and OsIndicationsSupported successfully.
  @retval EFI_INVALID_PARAMETER  OsIndications or OsIndicationsSupported is NULL.
  @retval EFI_NOT_FOUND          Cannot find OsIndications or OsIndicationsSupported value.
**/
EFI_STATUS
EFIAPI
BdsServicesGetOsIndications (
  IN   H2O_BDS_SERVICES_PROTOCOL   *This,
  OUT  UINT64                      *OsIndications,
  OUT  UINT64                      *OsIndicationsSupported
  )
{
  EFI_STATUS     Status;
  UINT64         Indications;
  UINT64         IndicationsSupport;
  UINTN          Size;

  if (OsIndications == NULL || OsIndicationsSupported == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Size = sizeof (Indications);
  Status = CommonGetVariable (
             EFI_OS_INDICATIONS_VARIABLE_NAME,
             &gEfiGlobalVariableGuid,
             &Size,
             &Indications
             );
  if (Status != EFI_SUCCESS) {
    return Status;
  }
  Size = sizeof (IndicationsSupport);
  Status = CommonGetVariable (
             EFI_OS_INDICATIONS_SUPPORT_VARIABLE_NAME,
             &gEfiGlobalVariableGuid,
             &Size,
             &IndicationsSupport
             );
  if (Status != EFI_SUCCESS) {
    return Status;
  }

  *OsIndications = Indications;
  *OsIndicationsSupported = IndicationsSupport;

  return EFI_SUCCESS;
}

/**
  Return the current value of the OS loader timeout.

  @param[in]  This               A Pointer to current instance of this protocol.
  @param[out] Timeout            A pointer to contain the value of the timeout.

  @retval EFI_SUCCESS            Get Timeout value successfully.
  @retval EFI_INVALID_PARAMETER  Timeout is NULL.
  @retval EFI_NOT_FOUND          Cannot find Timeout value.
**/
EFI_STATUS
EFIAPI
BdsServicesGetTimeout (
  IN   H2O_BDS_SERVICES_PROTOCOL   *This,
  OUT  UINT16                      *Timeout
  )
{
  UINTN       Size;

  if (Timeout == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  Size = sizeof (UINT16);
  return CommonGetVariable (
           EFI_TIME_OUT_VARIABLE_NAME,
           &gEfiGlobalVariableGuid,
           &Size,
           Timeout
           );
}

/**
  Return the current boot mode, such as S4 resume, diagnostics, full configuration, etc.

  @param[in]  This               A Pointer to current instance of this protocol.
  @param[in]  BootMode           A pointer to contain the value of the boot mode.

  @retval EFI_SUCCESS            Get BootMode successfully.
  @retval EFI_INVALID_PARAMETER  BootMode is NULL.
**/
EFI_STATUS
EFIAPI
BdsServicesGetBootMode (
  IN   H2O_BDS_SERVICES_PROTOCOL   *This,
  OUT  EFI_BOOT_MODE               *BootMode
  )
{
  if (BootMode == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  *BootMode = GetBootModeHob ();

  return EFI_SUCCESS;
}

/**
  Return the current boot type, uch as legacy, UEFI or dual.

  @param[in]  This               A Pointer to current instance of this protocol.
  @param[in]  BootType           A pointer to contain the value of the boot mode.

  @retval EFI_SUCCESS            Get BootType successfully.
  @retval EFI_INVALID_PARAMETER  BootType is NULL.
**/
EFI_STATUS
EFIAPI
BdsServicesGetBootType (
  IN   H2O_BDS_SERVICES_PROTOCOL   *This,
  OUT  UINT8                       *BootType
  )
{

  EFI_STATUS                  Status;
  KERNEL_CONFIGURATION        SystemConfiguration;

  if (BootType == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = GetKernelConfiguration (&SystemConfiguration);
  if (!EFI_ERROR (Status)) {
    *BootType = (UINTN) SystemConfiguration.BootType;
  }

  return Status;


}

/**
  Return linked list of BDS boot options derived from the BootOrder and Boot#### UEFI variables.

  @param[in]  This               A Pointer to current instance of this protocol.
  @param[out] BootList           Pointer to linked list of BDS boot options derived from the BootOrder and Boot#### UEFI variables.

  @retval EFI_SUCCESS            Get boot list successfully.
**/
EFI_STATUS
EFIAPI
BdsServicesGetBootList (
  IN   H2O_BDS_SERVICES_PROTOCOL   *This,
  OUT  LIST_ENTRY                  **BootList
  )
{
  return GetBdsLoadList (H2O_BDS_LOAD_OPTION_TYPE_BOOT, BootList);
}

/**
  Return linked list of BDS boot options derived from the DriverOrder and Driver#### UEFI variables.

  @param[in]  This               A Pointer to current instance of this protocol.
  @param[out] DriverList         Pointer to linked list of BDS boot options derived from the DriverOrder and Driver#### UEFI variables.

  @retval EFI_SUCCESS            Get driver list successfully.
**/
EFI_STATUS
EFIAPI
BdsServicesGetDriverList (
  IN   H2O_BDS_SERVICES_PROTOCOL   *This,
  OUT  LIST_ENTRY                  **DriverList
  )
{
  return GetBdsLoadList (H2O_BDS_LOAD_OPTION_TYPE_DRIVER, DriverList);
}

/**
  Get load order from input load order type.

  @param[in]  Type               Input load order type.

  @return   the load order number get from load order type.
**/
STATIC
UINT16
GetLoadOrderFromType (
  IN  UINT16   Type
  )
{
  UINT16       LoadOrder;
  UINTN        DataSize;


  LoadOrder = BOOT_NEXT_LOAD_ORDER;
  if (Type != BOOT_PREVIOUS_LOAD_ORDER && Type != BOOT_CURRENT_LOAD_ORDER && Type != BOOT_NEXT_LOAD_ORDER) {
    return LoadOrder;
  }

  DataSize = sizeof (UINT16);
  switch (Type) {

  case BOOT_PREVIOUS_LOAD_ORDER:
    gRT->GetVariable (H2O_BOOT_PREVIOUS_VARIABLE_NAME, &gEfiGenericVariableGuid, NULL, &DataSize, &LoadOrder);
    break;

  case BOOT_CURRENT_LOAD_ORDER:
    gRT->GetVariable (EFI_BOOT_CURRENT_VARIABLE_NAME, &gEfiGlobalVariableGuid, NULL, &DataSize, &LoadOrder);
    break;

  case BOOT_NEXT_LOAD_ORDER:
    gRT->GetVariable (EFI_BOOT_NEXT_VARIABLE_NAME, &gEfiGlobalVariableGuid, NULL, &DataSize, &LoadOrder);
    break;
  }

  return LoadOrder;
}


/**
  Create a BDS load option in a buffer allocated from pool.

  @param[in]  This               A Pointer to current instance of this protocol.
  @param[in]  DriverOrBoot       Boolean that specifies whether this load option represents a Driver load option
                                 (FALSE) or Boot load option (TRUE).
  @param[in]  OptionName         A Null-terminated string that is the name of the vendor's variable.
  @param[in]  OptionGuid         A unique identifier for the vendor.
  @param[in]  Attributes         The attributes for this load option entry.
  @param[in]  DevicePath         A Pointer to a packed array of UEFI device paths.
  @param[in]  Description        The user readable description for the load option.
  @param[in]  OptionalData       A Pointer to optional data for load option.
  @param[in]  OptionalDataSize   The size by bytes of optional data.
  @param[out] LoadOption         Dynamically allocated memory that contains a new created H2O_BDS_LOAD_OPTION
                                 instance. Caller is responsible freeing the buffer

  @retval EFI_SUCCESS            Create load option successfully.
  @retval EFI_INVALID_PARAMETER  DevicePath is NULL or LoadOption is NULL.
  @retval EFI_INVALID_PARAMETER  The OptionName is correct boot#### or driver#### variable name.
  @retval EFI_OUT_OF_RESOURCES   Allocate memory to create H2O_BDS_LOAD_OPTION failed.
**/
EFI_STATUS
EFIAPI
BdsServicesCreateLoadOption (
  IN        H2O_BDS_SERVICES_PROTOCOL    *This,
  IN        BOOLEAN                      DriverOrBoot,
  IN CONST  CHAR16                       *OptionName OPTIONAL,
  IN CONST  EFI_GUID                     *OptionGuid OPTIONAL,
  IN        UINT32                       Attributes,
  IN CONST  EFI_DEVICE_PATH_PROTOCOL     *DevicePath,
  IN CONST  CHAR16                       *Description OPTIONAL,
  IN CONST  UINT8                        *OptionalData OPTIONAL,
  IN        UINT32                       OptionalDataSize,
  OUT       H2O_BDS_LOAD_OPTION          **LoadOption
  )
{
  return BdsServicesCreateLoadOption2 (
           This,
           DriverOrBoot == DRIVER_OPTION ? H2O_BDS_LOAD_OPTION_TYPE_DRIVER : H2O_BDS_LOAD_OPTION_TYPE_BOOT,
           OptionName,
           OptionGuid,
           Attributes,
           DevicePath,
           Description,
           OptionalData,
           OptionalDataSize,
           LoadOption
           );
}

/**
  Create a BDS load option in a buffer allocated from pool.

  These functions allocation and initialize a BDS load option structure. They do not create a UEFI variable nor do
  they add the load option to one of the BDS load option lists.

  @param[in]  This               A Pointer to current instance of this protocol.
  @param[in]  OptionType         Enumerated value that specifies the type of load option. Valid values are:
                                 H2O_BDS_LOAD_OPTION_TYPE_DRIVER (0)
                                 H2O_BDS_LOAD_OPTION_TYPE_BOOT (1)
                                 H2O_BDS_LOAD_OPTION_TYPE_SYSPREP (2)
                                 H2O_BDS_LOAD_OPTION_TYPE_OS_RECOVERY (3)
                                 H2O_BDS_LOAD_OPTION_TYPE_PLATFORM_RECOVERY (4)
  @param[in]  OptionName         A Null-terminated string that is the name of the vendor's variable.
  @param[in]  OptionGuid         A unique identifier for the vendor.
  @param[in]  Attributes         The attributes for this load option entry.
  @param[in]  DevicePath         A Pointer to a packed array of UEFI device paths.
  @param[in]  Description        The user readable description for the load option.
  @param[in]  OptionalData       A Pointer to optional data for load option.
  @param[in]  OptionalDataSize   The size by bytes of optional data.

  @retval EFI_SUCCESS            Create load option successfully.
  @retval EFI_INVALID_PARAMETER  DevicePath is NULL or LoadOption is NULL.
  @retval EFI_INVALID_PARAMETER  The OptionName is correct boot#### or driver#### variable name.
  @retval EFI_OUT_OF_RESOURCES   Allocate memory to create H2O_BDS_LOAD_OPTION failed.
**/
EFI_STATUS
EFIAPI
BdsServicesCreateLoadOption2 (
  IN        H2O_BDS_SERVICES_PROTOCOL    *This,
  IN        UINT8                        OptionType,
  IN CONST  CHAR16                       *OptionName OPTIONAL,
  IN CONST  EFI_GUID                     *OptionGuid OPTIONAL,
  IN        UINT32                       Attributes,
  IN CONST  EFI_DEVICE_PATH_PROTOCOL     *DevicePath,
  IN CONST  CHAR16                       *Description OPTIONAL,
  IN CONST  UINT8                        *OptionalData OPTIONAL,
  IN        UINT32                       OptionalDataSize,
  OUT       H2O_BDS_LOAD_OPTION          **LoadOption
  )
{
  H2O_BDS_LOAD_OPTION      *CurrentOption;
  EFI_STATUS               Status;
  UINTN                    Index;
  UINTN                    OptionNameLen;

  if (DevicePath == NULL || LoadOption == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = EFI_SUCCESS;
  CurrentOption = AllocateZeroPool (sizeof (H2O_BDS_LOAD_OPTION));
  if (CurrentOption == NULL) {
   return EFI_OUT_OF_RESOURCES;
  }

  CurrentOption->Signature = H2O_BDS_LOAD_OPTION_SIGNATURE;
  InitializeListHead (&CurrentOption->Link);
  CurrentOption->DevicePath = AllocateCopyPool  (GetDevicePathSize (DevicePath), DevicePath);
  if (CurrentOption->DevicePath == NULL) {
    InternalFreeBdsLoadOption (CurrentOption);
    return EFI_OUT_OF_RESOURCES;
  }
  CurrentOption->Connected = FALSE;

  //
  // Determine option type and load order
  //
  if (OptionName != NULL && OptionGuid != NULL) {
    if (IsBootPreviousVariable (OptionName, OptionGuid)) {
      CurrentOption->OptionType   = H2O_BDS_LOAD_OPTION_TYPE_BOOT;
      CurrentOption->LoadOrder    = GetLoadOrderFromType (BOOT_PREVIOUS_LOAD_ORDER);
    } else if (IsBootCurrentVariable (OptionName, OptionGuid)) {
      CurrentOption->OptionType   = H2O_BDS_LOAD_OPTION_TYPE_BOOT;
      CurrentOption->LoadOrder    = GetLoadOrderFromType (BOOT_CURRENT_LOAD_ORDER);
    } else if (IsBootNextVariable (OptionName, OptionGuid)) {
      CurrentOption->OptionType   = H2O_BDS_LOAD_OPTION_TYPE_BOOT;
      CurrentOption->LoadOrder    = GetLoadOrderFromType (BOOT_NEXT_LOAD_ORDER);
    } else {
      OptionNameLen = StrLen (OptionName);
      for (Index = 0; Index < sizeof (mOptionTypeInfo) / sizeof (H2O_BDS_OPTION_TYPE_INFO); Index++) {
        if (StrLen (mOptionTypeInfo[Index].OptionVarPrefixName) + 4 == OptionNameLen &&
            StrnCmp (mOptionTypeInfo[Index].OptionVarPrefixName, OptionName, OptionNameLen - 4) == 0 &&
            IsValidOptionNumber (&OptionName[OptionNameLen - 4])) {
          break;
        }
      }
      if (Index == sizeof (mOptionTypeInfo) / sizeof (H2O_BDS_OPTION_TYPE_INFO)) {
        InternalFreeBdsLoadOption (CurrentOption);
        return EFI_INVALID_PARAMETER;
      }
      CurrentOption->OptionType = mOptionTypeInfo[Index].OptionType;
      CurrentOption->LoadOrder  = GetOptionNumber (&OptionName[OptionNameLen - 4]);
    }
    StrCpyS (CurrentOption->LoadOrderVarName, sizeof(CurrentOption->LoadOrderVarName) / sizeof(CHAR16), OptionName);
    CopyGuid (&CurrentOption->LoadOrderVarGuid, OptionGuid);
  } else {
    CurrentOption->OptionType = OptionType;
  }
  CurrentOption->DriverOrBoot = (CurrentOption->OptionType == H2O_BDS_LOAD_OPTION_TYPE_BOOT) ? BOOT_OPTION : DRIVER_OPTION;

  CurrentOption->Expanded = FALSE;
  InitializeListHead (&CurrentOption->ExpandedLoadOptions);
  CurrentOption->Attributes = Attributes;

  if (Description != NULL) {
    CurrentOption->Description = AllocateCopyPool  (StrSize (Description), Description);
    if (CurrentOption->Description == NULL) {
      InternalFreeBdsLoadOption (CurrentOption);
      return EFI_OUT_OF_RESOURCES;
    }
  }
  if (OptionalDataSize != 0 && OptionalData != NULL) {
    CurrentOption->LoadOptionalData = AllocateCopyPool  (OptionalDataSize, OptionalData);
    if (CurrentOption->LoadOptionalData == NULL) {
      InternalFreeBdsLoadOption (CurrentOption);
      return EFI_OUT_OF_RESOURCES;
    }
    CurrentOption->LoadOptionalDataSize = OptionalDataSize;
  }

  *LoadOption = CurrentOption;
  return EFI_SUCCESS;
}

/**
  Free the memory associated with a BDS load option.

  @param[in]  This               A Pointer to current instance of this protocol.
  @param[in]  LoadOption         The allocated H2O_BDS_LOAD_OPTION instance to free.

  @retval EFI_SUCCESS            Get BootType successfully.
  @retval EFI_INVALID_PARAMETER  LoadOption is NULL.
**/
EFI_STATUS
EFIAPI
BdsServicesFreeLoadOption (
  IN   H2O_BDS_SERVICES_PROTOCOL   *This,
  IN   H2O_BDS_LOAD_OPTION         *LoadOption
  )
{
  LIST_ENTRY                        *ExpandedOptions;
  LIST_ENTRY                        *Link;
  LIST_ENTRY                        *CurrentLink;

  if (LoadOption == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  if (LoadOption->Expanded) {
    ExpandedOptions = &LoadOption->ExpandedLoadOptions;
    for (Link = GetFirstNode (ExpandedOptions); !IsNull (ExpandedOptions, Link); Link = GetNextNode (ExpandedOptions, Link)) {
       CurrentLink = Link;
       Link = CurrentLink->BackLink;
       RemoveEntryList (CurrentLink);
       InternalFreeBdsLoadOption (BDS_OPTION_FROM_LINK (CurrentLink));
    }
  }
  InternalFreeBdsLoadOption (LoadOption);
  return EFI_SUCCESS;
}

/**
  Converts a UEFI variable formatted as a UEFI load option to a BDS load option.

  @param[in]  This               A Pointer to current instance of this protocol.
  @param[in]  VariableName       A Null-terminated string that is the name of the vendor's variable.
  @param[in]  VariableGuid       A unique identifier for the vendor.
  @param[out] BdsLoadOption      Dynamically allocated memory that contains a new created H2O_BDS_LOAD_OPTION
                                 instance. Caller is responsible freeing the buffer.

  @retval EFI_SUCCESS            Convert BDS load option to UEFI load option successfully.
  @retval EFI_INVALID_PARAMETER  VariableName is NULL VariableGuid is NULL or BdsLoadOption is NULL.
  @retval EFI_NOT_FOUND          Variable doesn't exist.
  @retval EFI_OUT_OF_RESOURCES   Allocate memory to create H2O_BDS_LOAD_OPTION failed.
**/
EFI_STATUS
EFIAPI
BdsServicesConvertVarToLoadOption (
  IN         H2O_BDS_SERVICES_PROTOCOL   *This,
  IN   CONST CHAR16                      *VariableName,
  IN   CONST EFI_GUID                    *VariableGuid,
  OUT        H2O_BDS_LOAD_OPTION         **BdsLoadOption
  )
{
  EFI_STATUS                   Status;
  UINT8                        *EfiLoadOption;
  UINTN                        EfiLoadOptionSize;
  UINT8                        *WorkingPtr;
  UINT16                       DevicePathLength;
  CHAR16                       *Description;
  EFI_DEVICE_PATH_PROTOCOL     *DevicePath;
  UINTN                        UsedSize;
  UINT8                        *OptionalData;
  UINT32                       OptionalDataSize;
  UINT32                       Attributes;
  CHAR16                       OptionName[OPTION_VAR_NAME_BUFFER_CHAR_COUNT];

  if (VariableName == NULL || VariableGuid == NULL || BdsLoadOption == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = CommonGetVariableDataAndSize (
             (CHAR16 *) VariableName,
             (EFI_GUID *) VariableGuid,
             &EfiLoadOptionSize,
             (VOID **) &EfiLoadOption
             );
  if (Status != EFI_SUCCESS) {
    return Status;
  }
  if (IsBootPreviousVariable (VariableName, VariableGuid) || IsBootCurrentVariable (VariableName, VariableGuid) ||
      IsBootNextVariable (VariableName, VariableGuid)) {
    UnicodeSPrint (OptionName, sizeof (OptionName), L"Boot%04x", *((UINT16 *) EfiLoadOption));
    FreePool (EfiLoadOption);
    Status = CommonGetVariableDataAndSize (
               OptionName,
               &gEfiGlobalVariableGuid,
               &EfiLoadOptionSize,
               (VOID **) &EfiLoadOption
               );
    if (Status != EFI_SUCCESS) {
      return Status;
    }
  }

  WorkingPtr = (UINT8 *) EfiLoadOption;
  Attributes = *((UINT32 *) WorkingPtr);
  WorkingPtr += sizeof (UINT32);
  DevicePathLength = *((UINT16 *) WorkingPtr);
  WorkingPtr += sizeof (UINT16);
  Description = (CHAR16 *) WorkingPtr;
  WorkingPtr += StrSize (Description);
  DevicePath = (EFI_DEVICE_PATH_PROTOCOL *) WorkingPtr;
  WorkingPtr += DevicePathLength;
  UsedSize = (UINTN) (WorkingPtr - (UINT8 *) EfiLoadOption);
  if (EfiLoadOptionSize > UsedSize && EfiLoadOption != NULL) {
    OptionalDataSize = (UINT32) (EfiLoadOptionSize - UsedSize);
    OptionalData = WorkingPtr;
  } else {
    OptionalData = NULL;
    OptionalDataSize = 0;
  }
  Status = BdsServicesCreateLoadOption (
             &mH2OBdsServices,
             VariableName[0] == L'B' ? BOOT_OPTION : DRIVER_OPTION,
             VariableName,
             VariableGuid,
             Attributes,
             DevicePath,
             Description,
             OptionalData,
             OptionalDataSize,
             BdsLoadOption
             );
  FreePool (EfiLoadOption);
  return Status;
}

/**
  Converts a BDS load option in a UEFI variable formatted as a UEFI load option.

  @param[in]  This               A Pointer to current instance of this protocol.
  @param[in]  BdsLoadOption      A pointer to BDS load option.
  @param[out] VariableName       A Null-terminated string that is the name of the vendor's variable.
                                 Caller is responsible freeing the buffer.
  @Param[out] VariableGuid       A unique identifier for the vendor.

  @retval EFI_SUCCESS            Convert UEFI load option to BDS load option successfully.
  @retval EFI_INVALID_PARAMETER  BdsLoadOption is NULL, VariableName is NULL or VariableGuid is NULL.
  @retval EFI_INVALID_PARAMETER  BdsLoadOption format is incorrect.
  @retval EFI_OUT_OF_RESOURCES   Allocate memory to contain variable name is failed.
  @retval Others                 Any other error occurred in this function.
**/
EFI_STATUS
EFIAPI
BdsServicesConvertLoadOptionToVar (
  IN   H2O_BDS_SERVICES_PROTOCOL   *This,
  IN   H2O_BDS_LOAD_OPTION         *BdsLoadOption,
  OUT  CHAR16                      **VariableName,
  OUT  EFI_GUID                    *VariableGuid
  )
{
  UINT8               *EfiLoadOption;
  UINTN               EfiLoadOptionSize;
  UINT8               *WorkingPtr;
  EFI_STATUS          Status;
  H2O_BDS_OPTION_TYPE_INFO         *OptionTypeInfo;

  if (BdsLoadOption == NULL || BdsLoadOption->Signature != H2O_BDS_LOAD_OPTION_SIGNATURE ||
      VariableName == NULL || VariableGuid == NULL ||
      !BDS_OPTION_HAVE_LOAD_ORDER_VAR_NAME(BdsLoadOption)) {
    return EFI_INVALID_PARAMETER;
  }

  OptionTypeInfo = GetOptionTypeInfo (BdsLoadOption->OptionType);
  if (OptionTypeInfo == NULL) {
    return EFI_UNSUPPORTED;
  }

  //
  // For BootPrev, BootCurrent, BootNext variable, needn't save related variable and just return variable name
  // variable GUID. other variable will save corresponding Boot#### and Driver#### variable.
  //
  if (!(StrCmp (BdsLoadOption->LoadOrderVarName, H2O_BOOT_PREVIOUS_VARIABLE_NAME) == 0 || StrCmp (BdsLoadOption->LoadOrderVarName, EFI_BOOT_CURRENT_VARIABLE_NAME) == 0 ||
      StrCmp (BdsLoadOption->LoadOrderVarName, EFI_BOOT_NEXT_VARIABLE_NAME) == 0)) {
    EfiLoadOptionSize = sizeof (UINT32) + sizeof (UINT16) + StrSize (BdsLoadOption->Description) +
                        GetDevicePathSize (BdsLoadOption->DevicePath) + BdsLoadOption->LoadOptionalDataSize;
    EfiLoadOption = AllocateZeroPool (EfiLoadOptionSize);
    if (EfiLoadOption == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }

    WorkingPtr = EfiLoadOption;
    *(UINT32 *) WorkingPtr = BdsLoadOption->Attributes;
    WorkingPtr += sizeof (UINT32);
    *(UINT16 *) WorkingPtr = (UINT16) GetDevicePathSize (BdsLoadOption->DevicePath);
    WorkingPtr += sizeof (UINT16);
    CopyMem (WorkingPtr, BdsLoadOption->Description, StrSize (BdsLoadOption->Description));
    WorkingPtr += StrSize (BdsLoadOption->Description);
    CopyMem (WorkingPtr, BdsLoadOption->DevicePath, GetDevicePathSize (BdsLoadOption->DevicePath));
    WorkingPtr += GetDevicePathSize (BdsLoadOption->DevicePath);
    CopyMem (WorkingPtr, BdsLoadOption->LoadOptionalData, BdsLoadOption->LoadOptionalDataSize);

    Status = CommonSetVariable (
               BdsLoadOption->LoadOrderVarName,
               &BdsLoadOption->LoadOrderVarGuid,
               OptionTypeInfo->VarAttribute,
               EfiLoadOptionSize,
               EfiLoadOption
               );
    if (Status != EFI_SUCCESS) {
      FreePool (EfiLoadOption);
      return Status;
    }
    FreePool (EfiLoadOption);
  }

  *VariableName = AllocateCopyPool (StrSize (BdsLoadOption->LoadOrderVarName), BdsLoadOption->LoadOrderVarName);
  if (*VariableName == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  CopyGuid (VariableGuid, &BdsLoadOption->LoadOrderVarGuid);

  return EFI_SUCCESS;
}

/**
  Insert a BDS load option into either the Driver or Boot order.

  @param[in]  This               A Pointer to current instance of this protocol.
  @param[in]  BdsLoadOption      A pointer to BDS load option.

  @retval EFI_SUCCESS            Insert a BDS load option into either the Driver or Boot order successfully.
  @retval EFI_INVALID_PARAMETER  BdsLoadOption is NULL.
  @retval EFI_INVALID_PARAMETER  BdsLoadOption format is incorrect.
**/
EFI_STATUS
EFIAPI
BdsServicesInsertLoadOption (
  IN   H2O_BDS_SERVICES_PROTOCOL   *This,
  IN   H2O_BDS_LOAD_OPTION         *BdsLoadOption
  )
{
  H2O_BDS_OPTION_TYPE_INFO             *OptionTypeInfo;
  H2O_BDS_ORDER_VAR_INFO               OrderVarInfo;
  UINTN                                Index;
  EFI_STATUS                           Status;
  EFI_BOOT_OPTION_POLICY_PROTOCOL      *BootOptionPolicy;
  UINTN                                BootOptionType;
  UINTN                                NewPosition;
  UINT16                               *NewOptionOrder;

  if (!IsValidLoadOption (BdsLoadOption)) {
    return EFI_INVALID_PARAMETER;
  }

  if (BdsLoadOption->OptionType != H2O_BDS_LOAD_OPTION_TYPE_BOOT) {
    return BdsServicesInsertLoadOption2 (This, BdsLoadOption, NULL);
  }

  OptionTypeInfo = GetOptionTypeInfo (BdsLoadOption->OptionType);
  if (OptionTypeInfo == NULL) {
    return EFI_UNSUPPORTED;
  }

  Status = GetOrderVarInfo (BdsLoadOption->OptionType, &OrderVarInfo);
  if (EFI_ERROR (Status)) {
    ZeroMem (&OrderVarInfo, sizeof (OrderVarInfo));
  }

  Status = GetOrderIndexInOrderVar (BdsLoadOption, &Index);
  if (!EFI_ERROR (Status)) {
    InsertLoadOptionToList (BdsLoadOption, &OrderVarInfo, Index);
    FreeOrderVarInfo (&OrderVarInfo);
    return EFI_SUCCESS;
  }

  Status = gBS->LocateProtocol (
                  &gEfiBootOptionPolicyProtocolGuid,
                  NULL,
                  (VOID **) &BootOptionPolicy
                  );

  if (Status == EFI_SUCCESS) {
    BootOptionType = IsLegacyBootOption (BdsLoadOption->DevicePath) ? LEGACY_BOOT_DEV : EFI_BOOT_DEV;
    BootOptionPolicy->FindPositionOfNewBootOption (
                        BootOptionPolicy,
                        BdsLoadOption->DevicePath,
                        OrderVarInfo.OptionOrder,
                        BootOptionType,
                        OrderVarInfo.OptionCount,
                        BdsLoadOption->LoadOrder,
                        &NewPosition
                        );
  } else {
    NewPosition = 0;
  }
  BdsLibNewBootOptionPolicy (
    &NewOptionOrder,
    OrderVarInfo.OptionOrder,
    OrderVarInfo.OptionCount,
    BdsLoadOption->LoadOrder,
    NewPosition
    );
  Status = CommonSetVariable (
             OptionTypeInfo->OrderVarName,
             &gEfiGlobalVariableGuid,
             OptionTypeInfo->VarAttribute,
             (OrderVarInfo.OptionCount + 1) * sizeof (UINT16),
             NewOptionOrder
             );
  FreeOrderVarInfo (&OrderVarInfo);
  GetOrderVarInfo (BdsLoadOption->OptionType, &OrderVarInfo);
  InsertLoadOptionToList (BdsLoadOption, &OrderVarInfo, NewPosition);
  FreeOrderVarInfo (&OrderVarInfo);
  InternalFreePool (NewOptionOrder);


  return EFI_SUCCESS;
}

/**
  Insert a BDS load option into the Driver, Boot, SysPrep, OS Recovery or Platform Recovery orders.

  This function inserts the specified load option into one of the BDS load option lists, depending on the value of
  the OptionType member of the load option.

  @param[in]  This               A Pointer to current instance of this protocol.
  @param[in]  BdsLoadOption      A pointer to BDS load option.
  @param[in]  BdsLoadOptionAfter An optional pointer to the load option which must precede the load option in the load
                                 option list. If NULL, then the load option will be inserted at the beginning of the
                                 load option list.

  @retval EFI_SUCCESS            Insert a BDS load option into either the Driver or Boot order successfully.
  @retval EFI_INVALID_PARAMETER  BdsLoadOption is NULL.
  @retval EFI_INVALID_PARAMETER  BdsLoadOptionAfter is non-NULL, but the OptionType member of the load option does not
                                 match that of BdsLoadOption.
  @retval EFI_INVALID_PARAMETER  BdsLoadOption or BdsLoadOptionAfter format is incorrect.
**/
EFI_STATUS
EFIAPI
BdsServicesInsertLoadOption2 (
  IN H2O_BDS_SERVICES_PROTOCOL         *This,
  IN H2O_BDS_LOAD_OPTION               *BdsLoadOption,
  IN H2O_BDS_LOAD_OPTION               *BdsLoadOptionAfter OPTIONAL
  )
{
  EFI_STATUS                           Status;
  H2O_BDS_ORDER_VAR_INFO               OrderVarInfo;
  UINTN                                OptionAfterOrderIndex;
  UINTN                                OptionOrderIndex;
  UINTN                                TargetIndex;
  UINT16                               Count;
  UINT16                               *Order;
  EFI_GUID                             *VarGuid;

  if (!IsValidLoadOption (BdsLoadOption) ||
      (BdsLoadOptionAfter != NULL && (!IsValidLoadOption (BdsLoadOptionAfter) ||
                                      BdsLoadOptionAfter->OptionType != BdsLoadOption->OptionType))) {
    return EFI_INVALID_PARAMETER;
  }

  Status = GetOrderVarInfo (BdsLoadOption->OptionType, &OrderVarInfo);
  if (EFI_ERROR (Status)) {
    ZeroMem (&OrderVarInfo, sizeof (OrderVarInfo));
  }

  Status = GetOrderIndexInOrderVar (BdsLoadOption, &OptionOrderIndex);
  if (!EFI_ERROR (Status)) {
    //
    // Load option exist in boot order, insert it to list and return,
    //
    InsertLoadOptionToList (BdsLoadOption, &OrderVarInfo, OptionOrderIndex);
    goto Exit;
  }

  Status = EFI_SUCCESS;
  if (BdsLoadOptionAfter == NULL) {
    TargetIndex = 0;
  } else {
    Status = GetOrderIndexInOrderVar (BdsLoadOptionAfter, &OptionAfterOrderIndex);
    if (EFI_ERROR (Status)) {
      goto Exit;
    }
    TargetIndex = OptionAfterOrderIndex + 1;
  }

  //
  // Update order variable and insert load option into option list.
  //
  Count   = OrderVarInfo.OptionCount;
  Order   = OrderVarInfo.OptionOrder;
  VarGuid = OrderVarInfo.OptionVarGuid;

  Order   = ReallocatePool (Count * sizeof (UINT16)  , (Count + 1) * sizeof (UINT16)  , Order);
  VarGuid = ReallocatePool (Count * sizeof (EFI_GUID), (Count + 1) * sizeof (EFI_GUID), VarGuid);
  if (Order == NULL || VarGuid == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto Exit;
  }
  CopyMem (&Order[TargetIndex + 1]  , &Order[TargetIndex]  , (Count - TargetIndex) * sizeof (UINT16));
  CopyMem (&VarGuid[TargetIndex + 1], &VarGuid[TargetIndex], (Count - TargetIndex) * sizeof (EFI_GUID));
  Order[TargetIndex] = BdsLoadOption->LoadOrder;
  CopyGuid (&VarGuid[TargetIndex], &BdsLoadOption->LoadOrderVarGuid);
  Count++;

  OrderVarInfo.OptionCount   = Count;
  OrderVarInfo.OptionOrder   = Order;
  OrderVarInfo.OptionVarGuid = VarGuid;

  SetOrderVar (BdsLoadOption->OptionType, &OrderVarInfo);
  InsertLoadOptionToList (BdsLoadOption, &OrderVarInfo, TargetIndex);

Exit:
  FreeOrderVarInfo (&OrderVarInfo);
  return Status;
}


/**
  Remove a BDS load option from either the Driver or Boot order.

  @param[in]  This               A Pointer to current instance of this protocol.
  @param[in]  DriverOrBoot       Boolean that specifies whether this load option represents a Driver load option
                                 (FALSE) or Boot load option (TRUE).
  @param[in]  LoadOrder          Unsigned integer that specifies the current boot option being booted. Corresponds to the four
                                 hexadecimal digits in the #### portion of the UEFI variable name Boot#### or Driver####.

  @retval EFI_SUCCESS            Remove a BDS load option from either the Driver or Boot order successfully.
**/
EFI_STATUS
EFIAPI
BdsServicesRemoveLoadOption (
  IN   H2O_BDS_SERVICES_PROTOCOL   *This,
  IN   BOOLEAN                     DriverOrBoot,
  IN   UINT16                      LoadOrder
  )
{
  return BdsServicesRemoveLoadOption2 (
           This,
           DriverOrBoot == DRIVER_OPTION ? H2O_BDS_LOAD_OPTION_TYPE_DRIVER : H2O_BDS_LOAD_OPTION_TYPE_BOOT,
           LoadOrder,
           NULL
           );
}

/**
  Remove a BDS load option from either the Driver or Boot order.

  This function removes the load option associated with the value LoadOrder from the load option list specified by OptionType.
  If OptionType is H2O_LOAD_OPTION_TYPE_OS_RECOVERY or H2O_LOAD_OPTION_TYPE_PLATFORM_RECOVERY, the load option list is
  further qualified by the OptionVendorGuid.

  @param[in]  This               A Pointer to current instance of this protocol.
  @param[in]  OptionType         Enumerated value that specifies the type of load option to remove. Valid values are:
                                 H2O_BDS_LOAD_OPTION_TYPE_DRIVER (0)
                                 H2O_BDS_LOAD_OPTION_TYPE_BOOT (1)
                                 H2O_BDS_LOAD_OPTION_TYPE_SYSPREP (2)
                                 H2O_BDS_LOAD_OPTION_TYPE_OS_RECOVERY (3)
                                 H2O_BDS_LOAD_OPTION_TYPE_PLATFORM_RECOVERY (4)
  @param[in]  LoadOrder          Unsigned integer that specifies the current boot option being booted. Corresponds to the four
                                 hexadecimal digits in the #### portion of the UEFI variable name Boot#### or Driver####.
  @param[in]  OptionVendorGuid   Pointer to an optional GUID that specifies the group of OS Recovery or Platform Recovery
                                 load options to search for the specified load option. For Driver, Boot or SysPrep load
                                 options, this must be NULL.

  @retval EFI_SUCCESS            Remove a BDS load option from either the Driver or Boot order successfully.
**/
EFI_STATUS
EFIAPI
BdsServicesRemoveLoadOption2 (
  IN H2O_BDS_SERVICES_PROTOCOL         *This,
  IN UINT8                             OptionType,
  IN UINT16                            LoadOrder,
  IN CONST EFI_GUID                    *OptionVendorGuid OPTIONAL
  )
{
  H2O_BDS_OPTION_TYPE_INFO             *OptionTypeInfo;
  LIST_ENTRY                           *OptionList;
  LIST_ENTRY                           *Link;
  H2O_BDS_LOAD_OPTION                  *CurrentLoadOption;
  H2O_BDS_ORDER_VAR_INFO               OrderVarInfo;
  UINTN                                Index;
  EFI_STATUS                           Status;
  CHAR16                               LoadOptionName[OPTION_VAR_NAME_BUFFER_CHAR_COUNT];

  if (OptionType == H2O_BDS_LOAD_OPTION_TYPE_OS_RECOVERY) {
    if (OptionVendorGuid == NULL) {
      return EFI_INVALID_PARAMETER;
    }
  } else {
    if (OptionVendorGuid != NULL && !CompareGuid (OptionVendorGuid, &gEfiGlobalVariableGuid)) {
      return EFI_INVALID_PARAMETER;
    }
  }

  OptionTypeInfo = GetOptionTypeInfo (OptionType);
  if (OptionTypeInfo == NULL) {
    return EFI_UNSUPPORTED;
  }

  OptionList = &OptionTypeInfo->OptionList;
  for (Link = GetFirstNode (OptionList); !IsNull (OptionList, Link); Link = GetNextNode (OptionList, Link)) {
    CurrentLoadOption = BDS_OPTION_FROM_LINK (Link);
    if (CurrentLoadOption->LoadOrder == LoadOrder) {
      RemoveEntryList (&CurrentLoadOption->Link);
      BdsServicesFreeLoadOption (NULL, CurrentLoadOption);
      break;
    }
  }

  Status = GetOrderVarInfo (OptionType, &OrderVarInfo);
  if (EFI_ERROR (Status)) {
    ZeroMem (&OrderVarInfo, sizeof (OrderVarInfo));
  }

  for (Index = 0; Index < OrderVarInfo.OptionCount; Index++) {
    if (OrderVarInfo.OptionOrder[Index] == LoadOrder &&
        (OptionVendorGuid == NULL ||
         CompareGuid (OptionVendorGuid, &OrderVarInfo.OptionVarGuid[Index]))) {
      CopyMem (
        &OrderVarInfo.OptionOrder[Index],
        &OrderVarInfo.OptionOrder[Index + 1],
        (OrderVarInfo.OptionCount - (Index + 1)) * sizeof (UINT16)
        );
      CopyMem (
        &OrderVarInfo.OptionVarGuid[Index],
        &OrderVarInfo.OptionVarGuid[Index + 1],
        (OrderVarInfo.OptionCount - (Index + 1)) * sizeof (EFI_GUID)
        );
      OrderVarInfo.OptionCount--;
      SetOrderVar (OptionType, &OrderVarInfo);
      break;
    }
  }
  FreeOrderVarInfo (&OrderVarInfo);

  UnicodeSPrint (LoadOptionName, sizeof (LoadOptionName), OPTION_VAR_NAME_FORMAT_STR, OptionTypeInfo->OptionVarPrefixName, LoadOrder);
  CommonSetVariable (
    LoadOptionName,
    OptionVendorGuid == NULL ? &gEfiGlobalVariableGuid : (EFI_GUID *) OptionVendorGuid,
    0,
    0,
    NULL
    );

  return EFI_SUCCESS;
}

/**
  Expand a partial load option to one or more fully qualified load options using the rules specified in the
  UEFI specification (USB WWID, hard disk) and the InsydeH2O boot groups.

  @param[in]  This               A Pointer to current instance of this protocol.
  @param[in]  BdsLoadOption      A pointer to BDS load option.

  @retval EFI_SUCCESS            Expand load option successfully.
  @retval EFI_INVALID_PARAMETER  BdsLoadOption is NULL.
  @retval EFI_INVALID_PARAMETER  BdsLoadOption format is incorrect.
**/
EFI_STATUS
EFIAPI
BdsServicesExpandLoadOption (
  IN   H2O_BDS_SERVICES_PROTOCOL   *This,
  IN   H2O_BDS_LOAD_OPTION         *BdsLoadOption
  )
{
  USB_CLASS_DEVICE_PATH          *DevicePath;

  if (BdsLoadOption == NULL || BdsLoadOption->Signature != H2O_BDS_LOAD_OPTION_SIGNATURE) {
    return EFI_INVALID_PARAMETER;
  }

  DevicePath = (USB_CLASS_DEVICE_PATH *) BdsLoadOption->DevicePath;
  if ((DevicePathType (DevicePath) == MESSAGING_DEVICE_PATH && DevicePathSubType (DevicePath) == MSG_USB_CLASS_DP) &&
      DevicePath->VendorId == 0xFFFF && DevicePath->ProductId == 0xFFFF && DevicePath->DeviceClass == 0xFF &&
      DevicePath->DeviceSubClass == 0xFF && DevicePath->DeviceProtocol == 0xFF) {
    ExpandWinowsToGoOption (BdsLoadOption);
  } else if (DevicePathType (BdsLoadOption->DevicePath) == MEDIA_DEVICE_PATH &&
            (DevicePathSubType (BdsLoadOption->DevicePath) == MEDIA_HARDDRIVE_DP)) {
    ExpandHddOption (BdsLoadOption);
  } else if ((DevicePathType (BdsLoadOption->DevicePath) == MEDIA_DEVICE_PATH) &&
             (DevicePathSubType (BdsLoadOption->DevicePath) == MEDIA_PIWG_FW_FILE_DP)) {
    ExpandFvFileOption (BdsLoadOption);
  } else if ((DevicePathType (BdsLoadOption->DevicePath) == HARDWARE_DEVICE_PATH) &&
             (DevicePathSubType (BdsLoadOption->DevicePath) == HW_VENDOR_DP) &&
             (CompareGuid (&((VENDOR_DEVICE_PATH *) BdsLoadOption->DevicePath)->Guid, &gH2OBdsBootDeviceGroupGuid))) {
    ExpandBootGroupOption (BdsLoadOption);
  } else if (IS_USB_SHORT_FORM_DEVICE_PATH(BdsLoadOption->DevicePath)) {
    ExpandUsbShortFormOption (BdsLoadOption);
  }

  return EFI_SUCCESS;
}

/**
  Launch a BDS load option.

  @param[in]  This               A Pointer to current instance of this protocol.
  @param[in]  BdsLoadOption      A pointer to BDS load option.
  @param[out] ExitDataSize       Pointer to the size, in bytes, of ExitData.
  @param[out] ExitData           Pointer to a pointer to a data buffer that includes a Null-terminated
                                 string, optionally followed by additional binary data.

  @retval EFI_SUCCESS            Boot from the input boot option successfully.
  @retval EFI_INVALID_PARAMETER  BdsLoadOption is NULL.
  @retval EFI_INVALID_PARAMETER  BdsLoadOption format is incorrect.
  @retval EFI_NOT_FOUND          If the Device Path is not found in the system
**/
EFI_STATUS
EFIAPI
BdsServicesLaunchLoadOption (
  IN   H2O_BDS_SERVICES_PROTOCOL   *This,
  IN   H2O_BDS_LOAD_OPTION         *BdsLoadOption,
  OUT  UINTN                       *ExitDataSize,
  OUT  CHAR16                      **ExitData      OPTIONAL
  )
{
  if (BdsLoadOption == NULL || BdsLoadOption->Signature != H2O_BDS_LOAD_OPTION_SIGNATURE) {
    return EFI_INVALID_PARAMETER;
  }

  if (BdsLoadOption->DriverOrBoot == BOOT_OPTION) {
    return LaunchBootOption (BdsLoadOption, ExitDataSize, ExitData);
  } else {
    return LaunchDriverOption (BdsLoadOption, ExitDataSize, ExitData);
  }
}

/**
  Return linked list of BDS system preparation options derived from the SysPrepOrder and SysPrep#### UEFI variables.

  @param[in] This                A pointer to the protocol interface structure for this protocol.
  @param[in] SysPrepList         A pointer to a pointer to the first list entry in a linked list. Each list entry
                                 describes a single system preparation load option using a "H2O_BDS_LOAD_OPTION" structure.
                                 This structure can be retrieved from the list entry using the "BDS_OPTION_FROM_LINK()" macro.

  @retval EFI_SUCCESS            The function completed successfully.
  @retval EFI_INVALID_PARAMETER  This is set to NULL or SysPrepList is set to NULL.
**/
EFI_STATUS
EFIAPI
BdsServicesGetSysPrepList (
  IN  H2O_BDS_SERVICES_PROTOCOL    *This,
  OUT LIST_ENTRY                   **SysPrepList
  )
{
  return GetBdsLoadList (H2O_BDS_LOAD_OPTION_TYPE_SYSPREP, SysPrepList);
}

/**
  Return linked list of BDS OS recovery load options derived from the OsRecoveryOrder and OsRecovery#### UEFI variables.

  @param[in] This                A pointer to the protocol interface structure for this protocol.
  @param[in] OsRecoveryList      A pointer to a pointer to the first list entry in a linked list. Each list entry
                                 describes a single OS recovery load option using a "H2O_BDS_LOAD_OPTION" structure.
                                 This structure can be retrieved from the list entry using the "BDS_OPTION_FROM_LINK()" macro.

  @retval EFI_SUCCESS            The function completed successfully.
  @retval EFI_INVALID_PARAMETER  This is set to NULL or OsRecoveryList is set to NULL.
**/
EFI_STATUS
EFIAPI
BdsServicesGetOsRecoveryList (
  IN  H2O_BDS_SERVICES_PROTOCOL    *This,
  OUT LIST_ENTRY                   **OsRecoveryList
  )
{
  return GetBdsLoadList (H2O_BDS_LOAD_OPTION_TYPE_OS_RECOVERY, OsRecoveryList);
}

/**
  Return linked list of BDS platform recovery load options derived from the PlatformRecovery#### UEFI variables.

  @param[in] This                A pointer to the protocol interface structure for this protocol.
  @param[in] OsRecoveryList      A pointer to a pointer to the first list entry in a linked list. Each list entry
                                 describes a single platform recovery load option using a "H2O_BDS_LOAD_OPTION" structure.
                                 This structure can be retrieved from the list entry using the "BDS_OPTION_FROM_LINK()" macro.

  @retval EFI_SUCCESS            The function completed successfully.
  @retval EFI_INVALID_PARAMETER  This is set to NULL or PlatformRecoveryList is set to NULL.
**/
EFI_STATUS
EFIAPI
BdsServicesGetPlatformRecoveryList (
  IN  H2O_BDS_SERVICES_PROTOCOL    *This,
  OUT LIST_ENTRY                   **PlatformRecoveryList
  )
{
  return GetBdsLoadList (H2O_BDS_LOAD_OPTION_TYPE_PLATFORM_RECOVERY, PlatformRecoveryList);
}

