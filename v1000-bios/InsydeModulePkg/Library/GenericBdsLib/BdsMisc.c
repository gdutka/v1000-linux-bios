/** @file
  GenericBdsLib

;******************************************************************************
;* Copyright (c) 2012 - 2020, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

/**
  Misc BDS library function

Copyright (c) 2004 - 2011, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "InternalBdsLib.h"
#include "BdsDummyBootOption.h"
#include <Protocol/DynamicHotKey.h>
#include <Guid/AuthenticatedVariableFormat.h>


#define MAX_STRING_LEN        200

/**
  The function will go through the driver option link list, load and start every driver the
  driver option device path point to.
**/
VOID
BdsLibLaunchDrivers (
  VOID
  )
{
  EFI_STATUS            Status;
  LIST_ENTRY            *OptionList;
  LIST_ENTRY            *Link;
  H2O_BDS_LOAD_OPTION   *DriverOption;
  BOOLEAN               ReconnectAll;

  Status = gBdsServices->GetDriverList (gBdsServices, &OptionList);
  if (Status != EFI_SUCCESS) {
    return;
  }

  ReconnectAll = FALSE;
  for (Link = GetFirstNode (OptionList); !IsNull (OptionList, Link); Link = GetNextNode (OptionList, Link)) {
    DriverOption = BDS_OPTION_FROM_LINK (Link);
    gBdsServices->LaunchLoadOption (gBdsServices, DriverOption, NULL, NULL);
    if ((DriverOption->Attributes & (LOAD_OPTION_ACTIVE | LOAD_OPTION_FORCE_RECONNECT)) == (LOAD_OPTION_ACTIVE | LOAD_OPTION_FORCE_RECONNECT)) {
      ReconnectAll = TRUE;
    }
  }

  if (ReconnectAll) {
    BdsLibDisconnectAllEfi ();
    BdsLibConnectAll ();
  }
}

/**
  The function will go through the driver option link list, load and start
  every driver the driver option device path point to.

  Note: This function is deprecated, suggest use BdsLibLaunchDrivers to launch all drivers.

  @param  BdsDriverLists        The header of the current driver option link list

**/
VOID
EFIAPI
BdsLibLoadDrivers (
  IN LIST_ENTRY                   *BdsDriverLists
  )
{
  EFI_STATUS                Status;
  LIST_ENTRY                *Link;
  BDS_COMMON_OPTION         *Option;
  EFI_HANDLE                ImageHandle;
  EFI_LOADED_IMAGE_PROTOCOL *ImageInfo;
  UINTN                     ExitDataSize;
  CHAR16                    *ExitData;
  BOOLEAN                   ReconnectAll;

  ReconnectAll = FALSE;

  //
  // Process the driver option
  //
  for (Link = BdsDriverLists->ForwardLink; Link != BdsDriverLists; Link = Link->ForwardLink) {
    Option = CR (Link, BDS_COMMON_OPTION, Link, BDS_LOAD_OPTION_SIGNATURE);

    //
    // If a load option is not marked as LOAD_OPTION_ACTIVE,
    // the boot manager will not automatically load the option.
    //
    if (!IS_LOAD_OPTION_TYPE (Option->Attribute, LOAD_OPTION_ACTIVE)) {
      continue;
    }

    //
    // If a driver load option is marked as LOAD_OPTION_FORCE_RECONNECT,
    // then all of the EFI drivers in the system will be disconnected and
    // reconnected after the last driver load option is processed.
    //
    if (IS_LOAD_OPTION_TYPE (Option->Attribute, LOAD_OPTION_FORCE_RECONNECT)) {
      ReconnectAll = TRUE;
    }

    //
    // Make sure the driver path is connected.
    //
    BdsLibConnectDevicePath (Option->DevicePath);

    //
    // Load and start the image that Driver#### describes
    //
    Status = gBS->LoadImage (
                    FALSE,
                    gImageHandle,
                    Option->DevicePath,
                    NULL,
                    0,
                    &ImageHandle
                    );

    if (!EFI_ERROR (Status)) {
      gBS->HandleProtocol (ImageHandle, &gEfiLoadedImageProtocolGuid, (VOID **) &ImageInfo);

      //
      // Verify whether this image is a driver, if not,
      // exit it and continue to parse next load option
      //
      if (ImageInfo->ImageCodeType != EfiBootServicesCode && ImageInfo->ImageCodeType != EfiRuntimeServicesCode) {
        gBS->Exit (ImageHandle, EFI_INVALID_PARAMETER, 0, NULL);
        continue;
      }

      if (Option->LoadOptionsSize != 0) {
        ImageInfo->LoadOptionsSize  = Option->LoadOptionsSize;
        ImageInfo->LoadOptions      = Option->LoadOptions;
      }
      //
      // Before calling the image, enable the Watchdog Timer for
      // the 5 Minute period
      //
      gBS->SetWatchdogTimer (5 * 60, 0x0000, 0x00, NULL);

      Status = gBS->StartImage (ImageHandle, &ExitDataSize, &ExitData);
      DEBUG ((DEBUG_INFO | DEBUG_LOAD, "Driver Return Status = %r\n", Status));

      //
      // Clear the Watchdog Timer after the image returns
      //
      gBS->SetWatchdogTimer (0x0000, 0x0000, 0x0000, NULL);
    }
  }

  //
  // Process the LOAD_OPTION_FORCE_RECONNECT driver option
  //
  if (ReconnectAll) {
    BdsLibDisconnectAllEfi ();
    BdsLibConnectAll ();
  }
}

/**
  Get the Option Number that does not used.
  Try to locate the specific option variable one by one utile find a free number.

  @param  VariableName          Indicate if the boot#### or driver#### option

  @return The Minimal Free Option Number

**/
UINT16
BdsLibGetFreeOptionNumber (
  IN  CHAR16    *VariableName
  )
{
  UINTN         Index;
  CHAR16        StrTemp[10];
  UINT16        *OptionBuffer;
  UINTN         OptionSize;

  //
  // Try to find the minimum free number from 0, 1, 2, 3....
  //
  Index = 0;
  do {
    if (*VariableName == 'B') {
      UnicodeSPrint (StrTemp, sizeof (StrTemp), L"Boot%04x", Index);
    } else {
      UnicodeSPrint (StrTemp, sizeof (StrTemp), L"Driver%04x", Index);
    }
    //
    // try if the option number is used
    //
    OptionBuffer = BdsLibGetVariableAndSize (
                     StrTemp,
                     &gEfiGlobalVariableGuid,
                     &OptionSize
                     );
    if (OptionBuffer == NULL) {
      break;
    }
    Index++;
  } while (TRUE);

  return ((UINT16) Index);
}


/**
  Update the boot#### or driver#### variable.

  @param  OptionName         String part of EFI variable name
  @param  Attribute          The attributes of the boot#### or driver####
  @param  DevicePath         The device path of the boot#### or driver####
  @param  Description        The description of the boot#### or driver####
  @param  OptionalData       Optional data of the boot#### or driver####
  @param  OptionalDataSize   Optional data size of the boot#### or driver####

  @retval EFI_SUCCESS   The boot#### or driver#### have been success updated.
  @retval EFI_ABORTED   Allocate memory resource fail.
  @retval Other         Set variable fail.

**/
EFI_STATUS
BdsLibUpdateOptionVar (
  IN  CHAR16                         *OptionName,
  IN  UINT32                         Attribute,
  IN  EFI_DEVICE_PATH_PROTOCOL       *DevicePath,
  IN  CHAR16                         *Description,
  IN  UINT8                          *OptionalData,   OPTIONAL
  IN  UINT32                         OptionalDataSize
  )
{
  EFI_STATUS                Status;
  UINTN                     OptionSize;
  VOID                      *OptionPtr;

  Status  = BdsLibCreateLoadOption (
              DevicePath,
              Description,
              OptionalData,
              OptionalDataSize,
              &OptionPtr,
              &OptionSize
              );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  *((UINT32 *) OptionPtr) = Attribute;

  Status = gRT->SetVariable (
                  OptionName,
                  &gEfiGlobalVariableGuid,
                  EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                  OptionSize,
                  OptionPtr
                  );
  FreePool (OptionPtr);

  return Status;
}

/**
 Internal function to trigger BOOT_DEVICE_REGISTER_EFI_BOOT_OPTION checkpoint.

 @param[in]  BootOptionNum           Boot option number.
 @param[in]  Description             Double pointer to boot option description string.
 @param[in]  DevicePath              Double pointer to boot option device path.
 @param[in]  OptionalData            Double pointer to boot option optional data.
 @param[in]  OptionalDataSize        Pointer to boot option optional data size.
 @param[out] Valid                   Pointer to flag to specify this boot option variable is valid or not.
 @param[out] CustomizedLoadOption    Double pointer to the customized load option.

 @retval EFI_SUCCESS                 Trigger BOOT_DEVICE_REGISTER_EFI_BOOT_OPTION checkpoint successfully.
 @retval Other                       Fail to create load option data.
**/
STATIC
EFI_STATUS
TriggerCpRegisterEfiBootOption (
  IN  UINT16                         BootOptionNum,
  IN  CHAR16                         *Description,
  IN  EFI_DEVICE_PATH_PROTOCOL       *DevicePath,
  IN  UINT8                          *OptionalData,   OPTIONAL
  IN  UINT32                         OptionalDataSize,
  OUT BOOLEAN                        *Valid,
  OUT H2O_BDS_LOAD_OPTION            **CustomizedLoadOption
  )
{
  EFI_STATUS                                             Status;
  CHAR16                                                 OptionName[10];
  H2O_BDS_CP_BOOT_DEVICE_REGISTER_EFI_BOOT_OPTION_DATA   BootDevRegisterEfiBootOptionData;
  H2O_BDS_LOAD_OPTION                                    *BdsLoadOption;


  UnicodeSPrint (OptionName, sizeof (OptionName), L"Boot%04x", BootOptionNum);
  Status = gBdsServices->CreateLoadOption2 (
                           gBdsServices,
                           H2O_BDS_LOAD_OPTION_TYPE_BOOT,
                           OptionName,
                           &gEfiGlobalVariableGuid,
                           LOAD_OPTION_ACTIVE,
                           DevicePath,
                           Description,
                           OptionalData,
                           OptionalDataSize,
                           &BdsLoadOption
                           );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  *Valid = TRUE;
  *CustomizedLoadOption = NULL;

  ZeroMem (&BootDevRegisterEfiBootOptionData, sizeof (BootDevRegisterEfiBootOptionData));
  BootDevRegisterEfiBootOptionData.Size       = sizeof (H2O_BDS_CP_BOOT_DEVICE_REGISTER_EFI_BOOT_OPTION_DATA);
  BootDevRegisterEfiBootOptionData.Status     = H2O_CP_TASK_NORMAL;
  BootDevRegisterEfiBootOptionData.BootOption = BdsLoadOption;

  DEBUG ((DEBUG_INFO, "Checkpoint Trigger: %g\n", &gH2OBdsCpBootDeviceRegisterEfiBootOptionGuid));
  H2OCpTrigger (&gH2OBdsCpBootDeviceRegisterEfiBootOptionGuid, &BootDevRegisterEfiBootOptionData);
  DEBUG ((DEBUG_INFO, "Checkpoint Result: %x\n", BootDevRegisterEfiBootOptionData.Status));

  if (BootDevRegisterEfiBootOptionData.Status == H2O_CP_TASK_SKIP) {
    *Valid = FALSE;
  } else if (BootDevRegisterEfiBootOptionData.Status == H2O_CP_TASK_UPDATE) {
    *CustomizedLoadOption = BootDevRegisterEfiBootOptionData.BootOption;
  }

  if (BootDevRegisterEfiBootOptionData.Status != H2O_CP_TASK_UPDATE &&
      BootDevRegisterEfiBootOptionData.BootOption != NULL) {
    gBdsServices->FreeLoadOption (gBdsServices, BootDevRegisterEfiBootOptionData.BootOption);
  }
  return EFI_SUCCESS;
}

/**
  This function will register the new boot#### or driver#### option base on
  the VariableName. The new registered boot#### or driver#### will be linked
  to BdsOptionList and also update to the VariableName. After the boot#### or
  driver#### updated, the BootOrder or DriverOrder will also be updated.

  @param  BdsOptionList         The header of the boot#### or driver#### link list. Deprecated.
  @param  DevicePath            The device path which the boot#### or driver####
                                option present
  @param  String                The description of the boot#### or driver####
  @param  VariableName          Indicate if the boot#### or driver#### option

  @retval EFI_SUCCESS           The boot#### or driver#### have been success
                                registered
  @retval EFI_STATUS            Return the status of gRT->SetVariable ().

**/
EFI_STATUS
EFIAPI
BdsLibRegisterNewOption (
  IN  LIST_ENTRY                     *BdsOptionList OPTIONAL,
  IN  EFI_DEVICE_PATH_PROTOCOL       *DevicePath,
  IN  CHAR16                         *String,
  IN  CHAR16                         *VariableName,
  IN  UINT8                          *OptionalData,   OPTIONAL
  IN  UINT32                         OptionalDataSize
  )
{
  EFI_STATUS                         Status;
  UINTN                              Index;
  UINT16                             RegisterOptionNumber;
  UINT16                             *OptionOrder;
  UINTN                              OptionOrderSize;
  UINT16                             *OptionOrderPtr;
  VOID                               *OptionPtr;
  UINTN                              OptionSize;
  UINT8                              *TempPtr;
  EFI_DEVICE_PATH_PROTOCOL           *OptionDevicePath;
  CHAR16                             *Description;
  CHAR16                             OptionName[10];
  BOOLEAN                            UpdateDescription;
  UINTN                              OptionNum;
  EFI_BOOT_OPTION_POLICY_PROTOCOL    *BootOptionPolicy;
  UINTN                              NewPosition;
  BOOLEAN                            IsBootVar;
  UINTN                              DescriptionSize;
  UINTN                              StringSize;
  VOID                               *NewOptionPtr;
  UINTN                              NewOptionSize;
  BOOLEAN                            IsValid;
  UINT32                             Attribute;
  H2O_BDS_LOAD_OPTION                *CustomizedLoadOption;

  DEBUG_CODE (
    if (BdsOptionList != NULL) {
      DEBUG ((DEBUG_INFO, "BdsLibRegisterNewOption(): BdsOptionList is deprecated. Please upgrade caller code to input NULL.\n"));
    }
  );

  OptionPtr         = NULL;
  OptionSize        = 0;
  OptionDevicePath  = NULL;
  UpdateDescription = FALSE;
  Attribute         = LOAD_OPTION_ACTIVE;
  Description       = NULL;
  DescriptionSize   = 0;
  IsBootVar         = (BOOLEAN) (*VariableName == 'B');

  OptionOrderSize = 0;
  OptionOrder = BdsLibGetVariableAndSize (
                  VariableName,
                  &gEfiGlobalVariableGuid,
                  &OptionOrderSize
                  );
  //
  // Compare with current option variable to check if there is an existing boot option.
  //
  OptionNum = OptionOrderSize / sizeof (UINT16);
  RegisterOptionNumber = GetNewBootOptionNo (IsBootVar, OptionOrder, OptionNum);
  for (Index = 0; (OptionOrder != NULL) && (Index < OptionNum); Index++) {
    UnicodeSPrint (OptionName, sizeof (OptionName), IsBootVar ? L"Boot%04x" : L"Driver%04x", OptionOrder[Index]);
    OptionPtr = BdsLibGetVariableAndSize (
                  OptionName,
                  &gEfiGlobalVariableGuid,
                  &OptionSize
                  );
    if (OptionPtr == NULL) {
      continue;
    }

    TempPtr = OptionPtr;
    TempPtr += sizeof (UINT32) + sizeof (UINT16);
    Description = (CHAR16 *) TempPtr;
    DescriptionSize = StrSize (Description);
    TempPtr += DescriptionSize;
    OptionDevicePath = (EFI_DEVICE_PATH_PROTOCOL *) TempPtr;

    if ((CompareMem (OptionDevicePath, DevicePath, GetDevicePathSize (OptionDevicePath)) == 0) ||
        (MatchPartitionDevicePathNode (DevicePath, (HARDDRIVE_DEVICE_PATH *) OptionDevicePath) &&
         BdsLibMatchFilePathDevicePathNode (DevicePath, OptionDevicePath))) {
      //
      // Notes: the description may will change base on the GetStringToken
      // Option description changed, need update.
      //
      UpdateDescription = TRUE;
      RegisterOptionNumber = OptionOrder[Index];
      break;
    }

    FreePool (OptionPtr);
  }

  CustomizedLoadOption = NULL;
  if (FeaturePcdGet (PcdH2OBdsCpBootDeviceRegisterEfiBootOptionSupported) && IsBootVar) {
    IsValid = TRUE;
    TriggerCpRegisterEfiBootOption (
      RegisterOptionNumber,
      String,
      DevicePath,
      OptionalData,
      OptionalDataSize,
      &IsValid,
      &CustomizedLoadOption
      );
    if (!IsValid) {
      Status = EFI_ABORTED;
      goto Exit;
    }
    if (CustomizedLoadOption != NULL) {
      RegisterOptionNumber = CustomizedLoadOption->LoadOrder;
      Attribute            = CustomizedLoadOption->Attributes;
      String               = CustomizedLoadOption->Description;
      OptionalData         = CustomizedLoadOption->LoadOptionalData;
      OptionalDataSize     = CustomizedLoadOption->LoadOptionalDataSize;
    }
  }

  if (UpdateDescription) {
    //
    // Skip updating description if the boot option is referenced by BootNext, because it may be a one-time boot option.
    //
    if (CompareMem (Description, String, DescriptionSize) == 0 ||
        (gBdsServices->BootNextLoadOption != NULL && gBdsServices->BootNextLoadOption->LoadOrder == OptionOrder[Index])) {
      //
      // Got the option, so just return
      //
      FreePool (OptionPtr);
      Status = EFI_SUCCESS;
      goto Exit;
    }

    //
    // Only update descrption field and maintain other filed data
    //
    StringSize = StrSize (String);

    NewOptionSize = OptionSize + StringSize - DescriptionSize;
    NewOptionPtr  = AllocateZeroPool (NewOptionSize);
    if (NewOptionPtr == NULL) {
      FreePool (OptionPtr);
      Status = EFI_OUT_OF_RESOURCES;
      goto Exit;
    }

    TempPtr = NewOptionPtr;
    CopyMem (TempPtr, OptionPtr, sizeof (UINT32) + sizeof (UINT16));
    TempPtr += (sizeof (UINT32) + sizeof (UINT16));
    OptionSize -= (sizeof (UINT32) + sizeof (UINT16));
    CopyMem (TempPtr, String, StringSize);
    TempPtr += StringSize;
    OptionSize -= DescriptionSize;
    CopyMem (TempPtr, OptionDevicePath, OptionSize);
    FreePool (OptionPtr);

    OptionPtr  = NewOptionPtr;
    OptionSize = NewOptionSize;

    //
    // The number in option#### to be updated
    //
    RegisterOptionNumber = OptionOrder[Index];
  } else {
    Status = BdsLibCreateLoadOption (
               DevicePath,
               String,
               OptionalData,
               OptionalDataSize,
               &OptionPtr,
               &OptionSize
               );
    if (EFI_ERROR (Status)) {
      goto Exit;
    }
    (* (UINT32 *) OptionPtr) = Attribute;
  }

  UnicodeSPrint (OptionName, sizeof (OptionName), IsBootVar ? L"Boot%04x" : L"Driver%04x", RegisterOptionNumber);
  Status = gRT->SetVariable (
                  OptionName,
                  &gEfiGlobalVariableGuid,
                  EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                  OptionSize,
                  OptionPtr
                  );
  if (EFI_ERROR (Status) || UpdateDescription) {
    FreePool (OptionPtr);
    goto Exit;
  }

  FreePool (OptionPtr);

  //
  // Update the option order variable
  //
  NewPosition = 0;
  if (*VariableName == 'B' && IsNewPositionDeterminedByVirtualBootOrder (RegisterOptionNumber)) {
    Status = GetPositionByVirtualBootOrder (
               RegisterOptionNumber,
               OptionOrder,
               OptionOrderSize,
               &NewPosition
               );
    if (EFI_ERROR (Status)) {
      NewPosition = OptionNum;
    }
  } else {
    Status = gBS->LocateProtocol (
                    &gEfiBootOptionPolicyProtocolGuid,
                    NULL,
                    (VOID **) &BootOptionPolicy
                    );
    if (!EFI_ERROR (Status)) {
      BootOptionPolicy->FindPositionOfNewBootOption (
                          BootOptionPolicy,
                          DevicePath,
                          OptionOrder,
                          (UINTN) EFI_BOOT_DEV,
                          OptionNum,
                          RegisterOptionNumber,
                          &NewPosition
                          );
    }
  }

  Status = BdsLibNewBootOptionPolicy (
             &OptionOrderPtr,
             OptionOrder,
             OptionNum,
             RegisterOptionNumber,
             NewPosition
             );
  if (EFI_ERROR (Status)) {
    goto Exit;
  }

  Status = gRT->SetVariable (
                  VariableName,
                  &gEfiGlobalVariableGuid,
                  EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                  (OptionNum + 1) * sizeof (UINT16),
                  OptionOrderPtr
                  );
  FreePool (OptionOrderPtr);

Exit:
  if (OptionOrder != NULL) {
    FreePool (OptionOrder);
  }
  if (CustomizedLoadOption != NULL) {
    gBdsServices->FreeLoadOption (gBdsServices, CustomizedLoadOption);
  }

  return Status;
}


/**
  Returns the size of a device path in bytes.

  This function returns the size, in bytes, of the device path data structure specified by DevicePath including the end of device path node.
  If DevicePath is NULL, then 0 is returned.
  If the length of the device path is bigger than MaxSize, also return 0 to indicate this is an invalidate device path.

  @param  DevicePath         A pointer to a device path data structure.
  @param  MaxSize            Max valid device path size. If big than this size, return error.

  @retval 0                  An invalid device path.
  @retval Others             The size of a device path in bytes.

**/
UINTN
GetDevicePathSizeEx (
  IN CONST EFI_DEVICE_PATH_PROTOCOL  *DevicePath,
  IN UINTN                           MaxSize
  )
{
  UINTN  Size;
  UINTN  NodeSize;

  if (DevicePath == NULL) {
    return 0;
  }

  //
  // Search for the end of the device path structure
  //
  Size = 0;
  while (!IsDevicePathEnd (DevicePath)) {
    NodeSize = DevicePathNodeLength (DevicePath);
    if (NodeSize < END_DEVICE_PATH_LENGTH) {
      return 0;
    }

    Size += NodeSize;
    if (Size + END_DEVICE_PATH_LENGTH > MaxSize) {
      return 0;
    }

    DevicePath = NextDevicePathNode (DevicePath);
  }

  NodeSize = DevicePathNodeLength (DevicePath);
  Size += NodeSize;
  if (Size > MaxSize || NodeSize != END_DEVICE_PATH_LENGTH) {
    return 0;
  }

  return Size;
}


/**
  Returns the length of a Null-terminated Unicode string. If the length is
  bigger than MaxStringLen, return length 0 to indicate that this is an
  invalidate string.

  This function returns the number of Unicode characters in the Null-terminated
  Unicode string specified by String.

  If String is NULL, then ASSERT().
  If String is not aligned on a 16-bit boundary, then ASSERT().

  @param  String           A pointer to a Null-terminated Unicode string.
  @param  MaxStringLen     Max string len in this string.

  @retval 0                An invalid string.
  @retval Others           The length of String.

**/
UINTN
StrSizeEx (
  IN      CONST CHAR16              *String,
  IN      UINTN                     MaxStringLen
  )
{
  UINTN                             Length;

  ASSERT (String != NULL && MaxStringLen != 0);
  if (String == NULL || MaxStringLen == 0) {
    return 0;
  }
  ASSERT (((UINTN) String & BIT0) == 0);

  for (Length = 0; *String != L'\0' && MaxStringLen != Length; String++, Length++);

  if (*String != L'\0' && MaxStringLen == Length) {
    return 0;
  }

  return (Length + 1) * sizeof (*String);
}

/**
  Validate the EFI Boot#### variable (VendorGuid/Name)

  @param  Variable              Boot#### variable data.
  @param  VariableSize          Returns the size of the EFI variable that was read

  @retval TRUE                  The variable data is correct.
  @retval FALSE                 The variable data is corrupted.

**/
BOOLEAN
ValidateOption (
  UINT8                     *Variable,
  UINTN                     VariableSize
  )
{
  UINT16                    FilePathSize;
  UINT8                     *TempPtr;
  EFI_DEVICE_PATH_PROTOCOL  *DevicePath;
  EFI_DEVICE_PATH_PROTOCOL  *TempPath;
  UINTN                     TempSize;

  //
  // Skip the option attribute
  //
  TempPtr    = Variable;
  TempPtr   += sizeof (UINT32);

  //
  // Get the option's device path size
  //
  FilePathSize  = *(UINT16 *) TempPtr;
  TempPtr      += sizeof (UINT16);

  //
  // Get the option's description string size
  //
  TempSize = StrSizeEx ((CHAR16 *) TempPtr, VariableSize);
  TempPtr += TempSize;

  //
  // Get the option's device path
  //
  DevicePath =  (EFI_DEVICE_PATH_PROTOCOL *) TempPtr;
  TempPtr    += FilePathSize;


  //
  // Validation boot option variable.
  //
  if ((UINTN) (TempPtr - Variable) >  VariableSize || FilePathSize == 0 || FilePathSize > MAX_VARIABLE_SIZE || TempSize == 0) {
    return FALSE;
  }

  TempPath = DevicePath;
  while (FilePathSize > 0) {
    TempSize = GetDevicePathSizeEx (TempPath, FilePathSize);
    if (TempSize == 0) {
      return FALSE;
    }

    FilePathSize = (UINT16) (FilePathSize - TempSize);
    TempPath    += TempSize;
  }

  return TRUE;

}

/**
  Build the boot#### or driver#### option from the VariableName, the
  build boot#### or driver#### will also be linked to BdsCommonOptionList.

  Note: This function is deprecated, use ConvertVarToLoadOption() in H2O_BDS_SERVICES_PROTOCOL.

  @param  BdsCommonOptionList   The header of the boot#### or driver#### option
                                link list
  @param  VariableName          EFI Variable name indicate if it is boot#### or
                                driver####

  @retval BDS_COMMON_OPTION     Get the option just been created
  @retval NULL                  Failed to get the new option

**/
BDS_COMMON_OPTION *
EFIAPI
BdsLibVariableToOption (
  IN OUT LIST_ENTRY                   *BdsCommonOptionList,
  IN  CHAR16                          *VariableName
  )
{
  UINT32                    Attribute;
  UINT16                    FilePathSize;
  UINT8                     *Variable;
  UINT8                     *TempPtr;
  UINTN                     VariableSize;
  EFI_DEVICE_PATH_PROTOCOL  *DevicePath;
  BDS_COMMON_OPTION         *Option;
  VOID                      *LoadOptions;
  UINT32                    LoadOptionsSize;
  CHAR16                    *Description;
  UINT8                     NumOff;

  DEBUG ((DEBUG_INFO, "BdsLibVariableToOption() is deprecated. Use ConvertVarToLoadOption() instead.\n"));

  //
  // Read the variable. We will never free this data.
  //
  Variable = BdsLibGetVariableAndSize (
              VariableName,
              &gEfiGlobalVariableGuid,
              &VariableSize
              );
  if (Variable == NULL) {
    return NULL;
  }
  //
  // Notes: careful defined the variable of Boot#### or
  // Driver####, consider use some macro to abstract the code
  //
  //
  // Get the option attribute
  //
  TempPtr   =  Variable;
  Attribute =  *(UINT32 *) Variable;
  TempPtr   += sizeof (UINT32);

  //
  // Get the option's device path size
  //
  FilePathSize =  *(UINT16 *) TempPtr;
  TempPtr      += sizeof (UINT16);

  //
  // Get the option's description string
  //
  Description = (CHAR16 *) TempPtr;

  //
  // Get the option's description string size
  //
  TempPtr     += StrSize ((CHAR16 *) TempPtr);

  //
  // Get the option's device path
  //
  DevicePath =  (EFI_DEVICE_PATH_PROTOCOL *) TempPtr;
  TempPtr    += FilePathSize;

  LoadOptions     = TempPtr;
  LoadOptionsSize = (UINT32) (VariableSize - (UINTN) (TempPtr - Variable));

  //
  // The Console variables may have multiple device paths, so make
  // an Entry for each one.
  //
  Option = AllocateZeroPool (sizeof (BDS_COMMON_OPTION));
  if (Option == NULL) {
    return NULL;
  }

  Option->Signature   = BDS_LOAD_OPTION_SIGNATURE;
  Option->DevicePath  = AllocateZeroPool (GetDevicePathSize (DevicePath));
  ASSERT(Option->DevicePath != NULL);
  if (Option->DevicePath == NULL) {
    return NULL;
  }
  CopyMem (Option->DevicePath, DevicePath, GetDevicePathSize (DevicePath));

  Option->Attribute   = Attribute;
  Option->Description = AllocateZeroPool (StrSize (Description));
  ASSERT(Option->Description != NULL);
  if (Option->Description == NULL) {
    return NULL;
  }
  CopyMem (Option->Description, Description, StrSize (Description));

  if (LoadOptionsSize == 0 || BdsLibIsBiosCreatedOption (Variable, VariableSize)) {
    Option->LoadOptions     = NULL;
    Option->LoadOptionsSize = 0;
  } else {
    if (DevicePathType (DevicePath) == BBS_DEVICE_PATH &&
        DevicePathSubType (DevicePath) == BBS_BBS_DP &&
        LoadOptionsSize > sizeof (BBS_TABLE) + sizeof (UINT16)) {
      //
      // For legacy option, optional data (device path) is only used for firmware. Just set BBS_TABLE and bbs index in LoadOptions.
      //
      LoadOptionsSize = sizeof (BBS_TABLE) + sizeof (UINT16);
    }
    Option->LoadOptions = AllocateZeroPool (LoadOptionsSize);
    ASSERT(Option->LoadOptions != NULL);
    if (Option->LoadOptions == NULL) {
      return NULL;
    }
    CopyMem (Option->LoadOptions, LoadOptions, LoadOptionsSize);
    Option->LoadOptionsSize = LoadOptionsSize;
  }

  //
  // Get the value from VariableName Unicode string
  // since the ISO standard assumes ASCII equivalent abbreviations, we can be safe in converting this
  // Unicode stream to ASCII without any loss in meaning.
  //
  if (*VariableName == 'B') {
    NumOff = (UINT8) (sizeof (L"Boot") / sizeof(CHAR16) - 1);
    Option->BootCurrent = (UINT16) ((VariableName[NumOff]  -'0') * 0x1000);
    Option->BootCurrent = (UINT16) (Option->BootCurrent + ((VariableName[NumOff+1]-'0') * 0x100));
    Option->BootCurrent = (UINT16) (Option->BootCurrent +  ((VariableName[NumOff+2]-'0') * 0x10));
    Option->BootCurrent = (UINT16) (Option->BootCurrent + ((VariableName[NumOff+3]-'0')));
  }
  //
  // Insert active entry to BdsDeviceList
  //
  if ((Option->Attribute & LOAD_OPTION_ACTIVE) == LOAD_OPTION_ACTIVE) {
    InsertTailList (BdsCommonOptionList, &Option->Link);
    FreePool (Variable);
    return Option;
  }

  FreePool (Variable);
  FreePool (Option);
  return NULL;
}

/**
  Process BootOrder, or DriverOrder variables, by calling
  BdsLibVariableToOption () for each UINT16 in the variables.

  Note: This function is deprecated, use GetBootList() or GetDriverList() in H2O_BDS_SERVICES_PROTOCOL.

  @param  BdsCommonOptionList   The header of the option list base on variable
                                VariableName
  @param  VariableName          EFI Variable name indicate the BootOrder or
                                DriverOrder

  @retval EFI_SUCCESS           Success create the boot option or driver option
                                list
  @retval EFI_OUT_OF_RESOURCES  Failed to get the boot option or driver option list

**/
EFI_STATUS
EFIAPI
BdsLibBuildOptionFromVar (
  IN  LIST_ENTRY                      *BdsCommonOptionList,
  IN  CHAR16                          *VariableName
  )
{
  UINT16            *OptionOrder;
  UINTN             OptionOrderSize;
  UINTN             Index;
  BDS_COMMON_OPTION *Option;
  CHAR16            OptionName[20];

  DEBUG ((DEBUG_INFO, "BdsLibBuildOptionFromVar() is deprecated. Use GetBootList() for BootOrder or GetDriverList() for DriverOrder.\n"));

  //
  // Zero Buffer in order to get all BOOT#### variables
  //
  ZeroMem (OptionName, sizeof (OptionName));

  //
  // Read the BootOrder, or DriverOrder variable.
  //
  OptionOrder = BdsLibGetVariableAndSize (
                  VariableName,
                  &gEfiGlobalVariableGuid,
                  &OptionOrderSize
                  );
  if (OptionOrder == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  for (Index = 0; Index < OptionOrderSize / sizeof (UINT16); Index++) {
    if (*VariableName == 'B') {
      UnicodeSPrint (OptionName, sizeof (OptionName), L"Boot%04x", OptionOrder[Index]);
    } else {
      UnicodeSPrint (OptionName, sizeof (OptionName), L"Driver%04x", OptionOrder[Index]);
    }

    Option              = BdsLibVariableToOption (BdsCommonOptionList, OptionName);
    if (Option != NULL) {
      Option->BootCurrent = OptionOrder[Index];
    }
  }

  FreePool (OptionOrder);

  return EFI_SUCCESS;
}

/**
  Get boot mode by looking up configuration table and parsing HOB list

  @param  BootMode              Boot mode from PEI handoff HOB.

  @retval EFI_SUCCESS           Successfully get boot mode

**/
EFI_STATUS
EFIAPI
BdsLibGetBootMode (
  OUT EFI_BOOT_MODE       *BootMode
  )
{
  return gBdsServices->GetBootMode (gBdsServices, BootMode);
}

/**
  Read the EFI variable (VendorGuid/Name) and return a dynamically allocated
  buffer, and the size of the buffer. If failure return NULL.

  @param  Name                  String part of EFI variable name
  @param  VendorGuid            GUID part of EFI variable name
  @param  VariableSize          Returns the size of the EFI variable that was read

  @return                       Dynamically allocated memory that contains a copy of the EFI variable
                                Caller is responsible freeing the buffer.
  @retval NULL                  Variable was not read

**/
VOID *
EFIAPI
BdsLibGetVariableAndSize (
  IN  CHAR16              *Name,
  IN  EFI_GUID            *VendorGuid,
  OUT UINTN               *VariableSize
  )
{
  EFI_STATUS  Status;
  UINTN       BufferSize;
  VOID        *Buffer;

  Buffer = NULL;

  //
  // Pass in a zero size buffer to find the required buffer size.
  //
  BufferSize  = 0;
  Status      = gRT->GetVariable (Name, VendorGuid, NULL, &BufferSize, Buffer);
  if (Status == EFI_BUFFER_TOO_SMALL) {
    //
    // Allocate the buffer to return
    //
    Buffer = AllocateZeroPool (BufferSize);
    if (Buffer == NULL) {
      *VariableSize = 0;
      return NULL;
    }
    //
    // Read variable into the allocated buffer.
    //
    Status = gRT->GetVariable (Name, VendorGuid, NULL, &BufferSize, Buffer);
    if (EFI_ERROR (Status)) {
      BufferSize = 0;
      FreePool (Buffer);
      Buffer = NULL;
    }
  }

  *VariableSize = BufferSize;
  return Buffer;
}

/**
  Delete the instance in Multi which matches partly with Single instance

  @param  Multi                 A pointer to a multi-instance device path data
                                structure.
  @param  Single                A pointer to a single-instance device path data
                                structure.

  @return This function will remove the device path instances in Multi which partly
          match with the Single, and return the result device path. If there is no
          remaining device path as a result, this function will return NULL.

**/
EFI_DEVICE_PATH_PROTOCOL *
EFIAPI
BdsLibDelPartMatchInstance (
  IN     EFI_DEVICE_PATH_PROTOCOL  *Multi,
  IN     EFI_DEVICE_PATH_PROTOCOL  *Single
  )
{
  EFI_DEVICE_PATH_PROTOCOL  *Instance;
  EFI_DEVICE_PATH_PROTOCOL  *NewDevicePath;
  EFI_DEVICE_PATH_PROTOCOL  *TempNewDevicePath;
  UINTN                     InstanceSize;
  UINTN                     SingleDpSize;
  UINTN                     Size;

  NewDevicePath     = NULL;
  TempNewDevicePath = NULL;

  if (Multi == NULL || Single == NULL) {
    return Multi;
  }

  Instance        =  GetNextDevicePathInstance (&Multi, &InstanceSize);
  SingleDpSize    =  GetDevicePathSize (Single) - END_DEVICE_PATH_LENGTH;
  InstanceSize    -= END_DEVICE_PATH_LENGTH;

  while (Instance != NULL) {

    Size = (SingleDpSize < InstanceSize) ? SingleDpSize : InstanceSize;

    if ((CompareMem (Instance, Single, Size) != 0)) {
      //
      // Append the device path instance which does not match with Single
      //
      TempNewDevicePath = NewDevicePath;
      NewDevicePath = AppendDevicePathInstance (NewDevicePath, Instance);
      if (TempNewDevicePath != NULL) {
        FreePool(TempNewDevicePath);
      }
    }
    FreePool(Instance);
    Instance = GetNextDevicePathInstance (&Multi, &InstanceSize);
    InstanceSize  -= END_DEVICE_PATH_LENGTH;
  }

  return NewDevicePath;
}

/**
  Function compares a device path data structure to that of all the nodes of a
  second device path instance.

  @param  Multi                 A pointer to a multi-instance device path data
                                structure.
  @param  Single                A pointer to a single-instance device path data
                                structure.

  @retval TRUE                  If the Single device path is contained within Multi device path.
  @retval FALSE                 The Single device path is not match within Multi device path.

**/
BOOLEAN
EFIAPI
BdsLibMatchDevicePaths (
  IN  EFI_DEVICE_PATH_PROTOCOL  *Multi,
  IN  EFI_DEVICE_PATH_PROTOCOL  *Single
  )
{
  EFI_DEVICE_PATH_PROTOCOL  *DevicePath;
  EFI_DEVICE_PATH_PROTOCOL  *DevicePathInst;
  UINTN                     Size;

  if (Multi == NULL || Single  == NULL) {
    return FALSE;
  }

  DevicePath      = Multi;
  DevicePathInst  = GetNextDevicePathInstance (&DevicePath, &Size);

  //
  // Search for the match of 'Single' in 'Multi'
  //
  while (DevicePathInst != NULL) {
    //
    // If the single device path is found in multiple device paths,
    // return success
    //
    if (CompareMem (Single, DevicePathInst, Size) == 0) {
      FreePool (DevicePathInst);
      return TRUE;
    }

    FreePool (DevicePathInst);
    DevicePathInst = GetNextDevicePathInstance (&DevicePath, &Size);
  }

  return FALSE;
}

/**
  This function prints a series of strings.

  @param  ConOut                Pointer to EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL
  @param  ...                   A variable argument list containing series of
                                strings, the last string must be NULL.

  @retval EFI_SUCCESS           Success print out the string using ConOut.
  @retval EFI_STATUS            Return the status of the ConOut->OutputString ().

**/
EFI_STATUS
EFIAPI
BdsLibOutputStrings (
  IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL   *ConOut,
  ...
  )
{
  VA_LIST     Args;
  EFI_STATUS  Status;
  CHAR16      *String;

  Status = EFI_SUCCESS;
  VA_START (Args, ConOut);

  while (!EFI_ERROR (Status)) {
    //
    // If String is NULL, then it's the end of the list
    //
    String = VA_ARG (Args, CHAR16 *);
    if (String == NULL) {
      break;
    }

    Status = ConOut->OutputString (ConOut, String);

    if (EFI_ERROR (Status)) {
      break;
    }
  }

  VA_END(Args);
  return Status;
}


/**
  insert new BootOption number to BootOrder by policy

  @param  NewBootOrder         the new BootOrder
  @param  OldBootOrder         current BootOrder
  @param  OldBootOrderCount    the count of elements in current BootOrder
  @param  NewBootOptionNo      the number of BootOption that would be insert in BootOrder
  @param  Policy               the index of BootOrder that the NewBootOptionNo would be insert in .

  @retval EFI_SUCCESS   Success insert new BootOption number in BootOrder

**/
EFI_STATUS
BdsLibNewBootOptionPolicy(
  UINT16                                     **NewBootOrder,
  UINT16                                     *OldBootOrder,
  UINTN                                      OldBootOrderCount,
  UINT16                                     NewBootOptionNo,
  UINTN                                      Policy
  )
{
  UINTN                                      Front = 0;
  UINTN                                      Back  = 0;
  UINT16                                     *TempPtr1 = NULL;
  UINT16                                     *TempPtr2 = NULL;

  *NewBootOrder = AllocateZeroPool ((OldBootOrderCount + 1) * sizeof (UINT16));
  if (NULL == *NewBootOrder) {
    return EFI_OUT_OF_RESOURCES;
  }

  (*NewBootOrder)[Policy] = NewBootOptionNo;

  if (OldBootOrderCount > 0) {
    //
    // in front of the Policy
    //
    if (Policy > 0) {
      Front = Policy;
      Front = Front * sizeof(UINT16);
      CopyMem (*NewBootOrder, OldBootOrder, Front);
    }

    //
    // in back of the Policy
    //
    if (Policy < OldBootOrderCount) {
      TempPtr2 = OldBootOrder + Policy;
      Back = OldBootOrderCount - Policy;
      Back = Back * sizeof(UINT16);
      ++Policy;
      TempPtr1  = (*NewBootOrder) + Policy;
      CopyMem (TempPtr1, TempPtr2, Back);
    }
  }

  return EFI_SUCCESS;
}

/**
  Get the index of option number in the order array.

  @param[in]  OrderArray               The pointer of order array
  @param[in]  OrderCount               The count of order Array.
  @param[in]  OptionNum                The number of option
  @param[out] OrderIndex               The pointer of order index

  @retval EFI_SUCCESS              Get the index of input option number successfully
  @retval EFI_INVALID_PARAMETER    OrderIndex is NULL.
  @retval EFI_NOT_FOUND            OrderArray is NULL or can not find the option number in the order array.
**/
EFI_STATUS
GetIndexInOrderArray (
  IN  UINT16                           *OrderArray,
  IN  UINTN                            OrderCount,
  IN  UINT16                           OptionNum,
  OUT UINTN                            *OrderIndex
  )
{
  UINTN                                Index;

  if (OrderIndex == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (OrderArray == NULL) {
    return EFI_NOT_FOUND;
  }

  for (Index = 0; Index < OrderCount; Index++) {
    if (OrderArray[Index] == OptionNum) {
      *OrderIndex = Index;
      return EFI_SUCCESS;
    }
  }

  return EFI_NOT_FOUND;
}

/**
  Get the number of new BootOption. The number can not be referenced by the BootOrder or BootNext variables.

  @param[in] IsBootVar             The option is boot option (TRUE) or driver option (FLASE)
  @param[in] BootOrder             The pointer of BootOrder
  @param[in] BootOptionNum         The count of BootOption

  @return the number of new BootOption
**/
UINT16
GetNewBootOptionNo (
  IN BOOLEAN                                 IsBootVar,
  IN UINT16                                  *BootOrder,
  IN UINTN                                   BootOptionNum
  )
{
  UINT16                                     NewNo;
  UINT16                                     *BootNext;
  UINTN                                      Size;

  BootNext = IsBootVar ? BdsLibGetVariableAndSize (EFI_BOOT_NEXT_VARIABLE_NAME, &gEfiGlobalVariableGuid, &Size) : NULL;

  for (NewNo = 0;;NewNo++) {
    if (BootNext != NULL && *BootNext == NewNo) {
      continue;
    }

    if (IsOptionNumInOrderArray (BootOrder, BootOptionNum, NewNo)) {
      continue;
    }

    break;
  }

  if (BootNext != NULL) {
    FreePool (BootNext);
  }

  return NewNo;
}

//
//  Following are BDS Lib functions which contain all the code about setup browser reset reminder feature.
//  Setup Browser reset reminder feature is that an reset reminder will be given before user leaves the setup browser  if
//  user change any option setting which needs a reset to be effective, and  the reset will be applied according to  the user selection.
//


/**
  Enable the setup browser reset reminder feature.
  This routine is used in platform tip. If the platform policy need the feature, use the routine to enable it.
**/
VOID
EFIAPI
EnableResetReminderFeature (
  VOID
  )
{
  mGenericBdsLibGlobalData->ResetReminderFeatureSwitch = TRUE;
}

/**
  Disable the setup browser reset reminder feature.
  This routine is used in platform tip. If the platform policy do not want the feature, use the routine to disable it.

**/
VOID
EFIAPI
DisableResetReminderFeature (
  VOID
  )
{
  mGenericBdsLibGlobalData->ResetReminderFeatureSwitch = FALSE;
}

/**
  Record the info that  a reset is required.
  A  module boolean variable is used to record whether a reset is required.

**/
VOID
EFIAPI
EnableResetRequired (
  VOID
  )
{
  mGenericBdsLibGlobalData->ResetRequired = TRUE;
}

/**
  Record the info that  no reset is required.
  A  module boolean variable is used to record whether a reset is required.

**/
VOID
EFIAPI
DisableResetRequired (
  VOID
  )
{
  mGenericBdsLibGlobalData->ResetRequired = FALSE;
}

/**
  Check whether platform policy enable the reset reminder feature. The default is enabled.

**/
BOOLEAN
EFIAPI
IsResetReminderFeatureEnable (
  VOID
  )
{
  return mGenericBdsLibGlobalData->ResetReminderFeatureSwitch;
}

/**
  Check if  user changed any option setting which needs a system reset to be effective.

**/
BOOLEAN
EFIAPI
IsResetRequired (
  VOID
  )
{
  return mGenericBdsLibGlobalData->ResetRequired;
}

/**
  Check whether a reset is needed, and finish the reset reminder feature.
  If a reset is needed, Popup a menu to notice user, and finish the feature
  according to the user selection.

**/
VOID
EFIAPI
SetupResetReminder (
  VOID
  )
{
  EFI_INPUT_KEY                 Key;
  CHAR16                        *StringBuffer1;
  CHAR16                        *StringBuffer2;


  //
  //check any reset required change is applied? if yes, reset system
  //
  if (IsResetReminderFeatureEnable ()) {
    if (IsResetRequired ()) {

      StringBuffer1 = AllocateZeroPool (MAX_STRING_LEN * sizeof (CHAR16));
      ASSERT (StringBuffer1 != NULL);
      if (StringBuffer1 == NULL) {
        return;
      }
      StringBuffer2 = AllocateZeroPool (MAX_STRING_LEN * sizeof (CHAR16));
      ASSERT (StringBuffer2 != NULL);
      if (StringBuffer2 == NULL) {
        return;
      }
      StrCpyS (StringBuffer1, MAX_STRING_LEN, L"Configuration changed. Reset to apply it Now ? ");
      StrCpyS (StringBuffer2, MAX_STRING_LEN, L"Enter (YES)  /   Esc (NO)");
      //
      // Popup a menu to notice user
      //
      do {
        CreatePopUp (EFI_LIGHTGRAY | EFI_BACKGROUND_BLUE, &Key, StringBuffer1, StringBuffer2, NULL);
      } while ((Key.ScanCode != SCAN_ESC) && (Key.UnicodeChar != CHAR_CARRIAGE_RETURN));

      FreePool (StringBuffer1);
      FreePool (StringBuffer2);
      //
      // If the user hits the YES Response key, reset
      //
      if (Key.UnicodeChar == CHAR_CARRIAGE_RETURN) {
        gRT->ResetSystem (EfiResetCold, EFI_SUCCESS, 0, NULL);
      }
      gST->ConOut->ClearScreen (gST->ConOut);
    }
  }
}

/**
  Get the headers (dos, image, optional header) from an image

  @param  Device                SimpleFileSystem device handle
  @param  FileName              File name for the image
  @param  DosHeader             Pointer to dos header
  @param  Hdr                   The buffer in which to return the PE32, PE32+, or TE header.

  @retval EFI_SUCCESS           Successfully get the machine type.
  @retval EFI_NOT_FOUND         The file is not found.
  @retval EFI_LOAD_ERROR        File is not a valid image file.

**/
EFI_STATUS
EFIAPI
BdsLibGetImageHeader (
  IN  EFI_HANDLE                  Device,
  IN  CHAR16                      *FileName,
  OUT EFI_IMAGE_DOS_HEADER        *DosHeader,
  OUT EFI_IMAGE_OPTIONAL_HEADER_PTR_UNION   Hdr
  )
{
  EFI_STATUS                       Status;
  EFI_SIMPLE_FILE_SYSTEM_PROTOCOL  *Volume;
  EFI_FILE_HANDLE                  Root;
  EFI_FILE_HANDLE                  ThisFile;
  UINTN                            BufferSize;
  UINT64                           FileSize;
  EFI_FILE_INFO                    *Info;

  Root     = NULL;
  ThisFile = NULL;
  //
  // Handle the file system interface to the device
  //
  Status = gBS->HandleProtocol (
                  Device,
                  &gEfiSimpleFileSystemProtocolGuid,
                  (VOID *) &Volume
                  );
  if (EFI_ERROR (Status)) {
    goto Done;
  }

  Status = Volume->OpenVolume (
                     Volume,
                     &Root
                     );
  if (EFI_ERROR (Status)) {
    Root = NULL;
    goto Done;
  }
  ASSERT (Root != NULL);
  if (Root == NULL) {
    goto Done;
  }
  Status = Root->Open (Root, &ThisFile, FileName, EFI_FILE_MODE_READ, 0);
  if (EFI_ERROR (Status)) {
    goto Done;
  }
  ASSERT (ThisFile != NULL);
  if (ThisFile == NULL) {
    goto Done;
  }

  //
  // Get file size
  //
  BufferSize  = SIZE_OF_EFI_FILE_INFO + 200;
  do {
    Info = AllocatePool (BufferSize);
    if (Info == NULL) {
      Status = EFI_OUT_OF_RESOURCES;
      goto Done;
    }
    Status = ThisFile->GetInfo (
                         ThisFile,
                         &gEfiFileInfoGuid,
                         &BufferSize,
                         Info
                         );
    if (!EFI_ERROR (Status)) {
      break;
    }
    if (Status != EFI_BUFFER_TOO_SMALL) {
      FreePool (Info);
      goto Done;
    }
    FreePool (Info);
  } while (TRUE);

  FileSize = Info->FileSize;
  FreePool (Info);

  //
  // Read dos header
  //
  BufferSize = sizeof (EFI_IMAGE_DOS_HEADER);
  Status = ThisFile->Read (ThisFile, &BufferSize, DosHeader);
  if (EFI_ERROR (Status) ||
      BufferSize < sizeof (EFI_IMAGE_DOS_HEADER) ||
      FileSize <= DosHeader->e_lfanew ||
      DosHeader->e_magic != EFI_IMAGE_DOS_SIGNATURE) {
    Status = EFI_LOAD_ERROR;
    goto Done;
  }

  //
  // Move to PE signature
  //
  Status = ThisFile->SetPosition (ThisFile, DosHeader->e_lfanew);
  if (EFI_ERROR (Status)) {
    Status = EFI_LOAD_ERROR;
    goto Done;
  }

  //
  // Read and check PE signature
  //
  BufferSize = sizeof (EFI_IMAGE_OPTIONAL_HEADER_UNION);
  Status = ThisFile->Read (ThisFile, &BufferSize, Hdr.Pe32);
  if (EFI_ERROR (Status) ||
      BufferSize < sizeof (EFI_IMAGE_OPTIONAL_HEADER_UNION) ||
      Hdr.Pe32->Signature != EFI_IMAGE_NT_SIGNATURE) {
    Status = EFI_LOAD_ERROR;
    goto Done;
  }

  //
  // Check PE32 or PE32+ magic
  //
  if (Hdr.Pe32->OptionalHeader.Magic != EFI_IMAGE_NT_OPTIONAL_HDR32_MAGIC &&
      Hdr.Pe32->OptionalHeader.Magic != EFI_IMAGE_NT_OPTIONAL_HDR64_MAGIC) {
    Status = EFI_LOAD_ERROR;
    goto Done;
  }

 Done:
  if (ThisFile != NULL) {
    ThisFile->Close (ThisFile);
  }
  if (Root != NULL) {
    Root->Close (Root);
  }
  return Status;
}

/**
  This routine adjusts the memory information for different memory type and
  saves them into the variables for next boot. It conditionally resets the
  system when the memory information changes. Platform can reserve memory
  large enough (125% of actual requirement) to avoid the reset in the first boot.
**/
VOID
EFIAPI
BdsSetMemoryTypeInformationVariable (
  IN  EFI_EVENT                Event,
  IN  VOID                     *Context
  )
{
  EFI_STATUS                   Status;
  EFI_MEMORY_TYPE_INFORMATION  *PreviousMemoryTypeInformation;
  EFI_MEMORY_TYPE_INFORMATION  *CurrentMemoryTypeInformation;
  UINTN                        VariableSize;
  UINTN                        Index;
  UINTN                        Index1;
  UINT32                       Previous;
  UINT32                       Current;
  UINT32                       Next;
  EFI_HOB_GUID_TYPE            *GuidHob;
  BOOLEAN                      MemoryTypeInformationModified;
  BOOLEAN                      MemoryTypeInformationVariableExists;
  EFI_BOOT_MODE                BootMode;

  MemoryTypeInformationModified       = FALSE;
  MemoryTypeInformationVariableExists = FALSE;


  BootMode = GetBootModeHob ();
  //
  // In BOOT_IN_RECOVERY_MODE, Variable region is not reliable.
  //
  if (BootMode == BOOT_IN_RECOVERY_MODE) {
    return;
  }

  //
  // Only check the the Memory Type Information variable in the boot mode
  // other than BOOT_WITH_DEFAULT_SETTINGS because the Memory Type
  // Information is not valid in this boot mode.
  //
  if (BootMode != BOOT_WITH_DEFAULT_SETTINGS) {
    VariableSize = 0;
    Status = gRT->GetVariable (
                    EFI_MEMORY_TYPE_INFORMATION_VARIABLE_NAME,
                    &gEfiMemoryTypeInformationGuid,
                    NULL,
                    &VariableSize,
                    NULL
                    );
    if (Status == EFI_BUFFER_TOO_SMALL) {
      MemoryTypeInformationVariableExists = TRUE;
    }
  }

  //
  // Retrieve the current memory usage statistics.  If they are not found, then
  // no adjustments can be made to the Memory Type Information variable.
  //
  Status = EfiGetSystemConfigurationTable (
             &gEfiMemoryTypeInformationGuid,
             (VOID **) &CurrentMemoryTypeInformation
             );
  if (EFI_ERROR (Status) || CurrentMemoryTypeInformation == NULL) {
    return;
  }

  //
  // Get the Memory Type Information settings from Hob if they exist,
  // PEI is responsible for getting them from variable and build a Hob to save them.
  // If the previous Memory Type Information is not available, then set defaults
  //
  GuidHob = GetFirstGuidHob (&gEfiMemoryTypeInformationGuid);
  if (GuidHob == NULL) {
    //
    // If Platform has not built Memory Type Info into the Hob, just return.
    //
    return;
  }
  PreviousMemoryTypeInformation = GET_GUID_HOB_DATA (GuidHob);
  VariableSize = GET_GUID_HOB_DATA_SIZE (GuidHob);

  //
  // Use a heuristic to adjust the Memory Type Information for the next boot
  //
  DEBUG ((EFI_D_INFO, "Memory  Previous  Current    Next   \n"));
  DEBUG ((EFI_D_INFO, " Type    Pages     Pages     Pages  \n"));
  DEBUG ((EFI_D_INFO, "======  ========  ========  ========\n"));

  for (Index = 0; PreviousMemoryTypeInformation[Index].Type != EfiMaxMemoryType; Index++) {

    for (Index1 = 0; CurrentMemoryTypeInformation[Index1].Type != EfiMaxMemoryType; Index1++) {
      if (PreviousMemoryTypeInformation[Index].Type == CurrentMemoryTypeInformation[Index1].Type) {
        break;
      }
    }
    if (CurrentMemoryTypeInformation[Index1].Type == EfiMaxMemoryType) {
      continue;
    }

    //
    // Previous is the number of pages pre-allocated
    // Current is the number of pages actually needed
    //
    Previous = PreviousMemoryTypeInformation[Index].NumberOfPages;
    Current  = CurrentMemoryTypeInformation[Index1].NumberOfPages;
    Next     = Previous;

    //
    // Write next variable to 125% * current and Inconsistent Memory Reserved across bootings may lead to S4 fail
    //
    if (Current < Previous) {
      if (BootMode == BOOT_WITH_DEFAULT_SETTINGS) {
        Next = Current + (Current >> 2);
      } else if (!MemoryTypeInformationVariableExists) {
        Next = MAX (Current + (Current >> 2), Previous);
      }
    } else if (Current > Previous) {
      Next = Current + (Current >> 2);
    }
    if (Next > 0 && Next < 4) {
      Next = 4;
    }

    if (Next != Previous) {
      PreviousMemoryTypeInformation[Index].NumberOfPages = Next;
      MemoryTypeInformationModified = TRUE;
    }

    DEBUG ((EFI_D_INFO, "  %02x    %08x  %08x  %08x\n", PreviousMemoryTypeInformation[Index].Type, Previous, Current, Next));
  }

  //
  // If any changes were made to the Memory Type Information settings, then set the new variable value;
  // Or create the variable in first boot.
  //
  if (MemoryTypeInformationModified || !MemoryTypeInformationVariableExists) {
    Status = SetVariableToSensitiveVariable (
               EFI_MEMORY_TYPE_INFORMATION_VARIABLE_NAME,
               &gEfiMemoryTypeInformationGuid,
               EFI_VARIABLE_NON_VOLATILE  | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
               VariableSize,
               PreviousMemoryTypeInformation
               );

    //
    // If the Memory Type Information settings have been modified, then reset the platform
    // so the new Memory Type Information setting will be used to guarantee that an S4
    // entry/resume cycle will not fail.
    //
    if (MemoryTypeInformationModified && PcdGetBool (PcdResetOnMemoryTypeInformationChange)) {
      DEBUG ((EFI_D_INFO, "Memory Type Information settings change. Warm Reset!!!\n"));
      gRT->ResetSystem (EfiResetWarm, EFI_SUCCESS, 0, NULL);
    }
  }
}

/**
  This routine is kept for backward compatibility.
**/
VOID
EFIAPI
BdsLibSaveMemoryTypeInformation (
  VOID
  )
{
  EFI_STATUS                   Status;
  EFI_EVENT                    ReadyToBootEvent;

  Status = EfiCreateEventReadyToBootEx (
             TPL_CALLBACK,
             BdsSetMemoryTypeInformationVariable,
             NULL,
             &ReadyToBootEvent
             );
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR,"Bds Set Memory Type Information Variable Fails\n"));
  }
}


/**
  Identify a user and, if authenticated, returns the current user profile handle.

  @param[out]  User           Point to user profile handle.

  @retval EFI_SUCCESS         User is successfully identified, or user identification
                              is not supported.
  @retval EFI_ACCESS_DENIED   User is not successfully identified

**/
EFI_STATUS
EFIAPI
BdsLibUserIdentify (
  OUT EFI_USER_PROFILE_HANDLE         *User
  )
{
  EFI_STATUS                          Status;
  EFI_USER_MANAGER_PROTOCOL           *Manager;

  Status = gBS->LocateProtocol (
                  &gEfiUserManagerProtocolGuid,
                  NULL,
                  (VOID **) &Manager
                  );
  if (EFI_ERROR (Status)) {
    return EFI_SUCCESS;
  }

  return Manager->Identify (Manager, User);
}


/**
  The *BootOrderPtr[Index] hasn't Boot option, so delete the *BootOrderPtr[Index] and
  set the new L"BootOrder" variable

  @param  BootOrderPtr    IN  - the double pointer point to the original BootOrder
                          OUT - the double pointer point to the Update BootOrder
  @param  Index           Indicate the Offset of BootOrder is invalid
  @param  BootOrderSize   IN  - The pointer of original Boot Order size
                          OUT - The pointer of udpate Boot Order size

  @retval EFI_SUCCESS               Update the New L"BootOrder" variable successful
  @retval EFI_INVALID_PARAMETER     The input parameter is invalid

**/
EFI_STATUS
BdsLibUpdateInvalidBootOrder (
  IN OUT UINT16        **BootOrderPtr,
  IN     UINTN         Index,
  IN OUT UINTN         *BootOrderSize
  )
{
  UINTN                     Index2;
  UINTN                     Index3;
  UINT16                    *BootOrder;
  EFI_STATUS                Status;

  if (BootOrderPtr == NULL || *BootOrderPtr == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (BootOrderSize == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  BootOrder = *BootOrderPtr;
  BootOrder[Index] = 0xffff;
  Index2 = 0;
  for (Index3 = 0; Index3 < *BootOrderSize / sizeof (UINT16); Index3++) {
    if (BootOrder[Index3] != 0xffff) {
      BootOrder[Index2] = BootOrder[Index3];
      Index2 ++;
    }
  }
  Status = gRT->SetVariable (
                  L"BootOrder",
                  &gEfiGlobalVariableGuid,
                  EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                  Index2 * sizeof (UINT16),
                  BootOrder
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  FreePool (BootOrder);

  BootOrder = BdsLibGetVariableAndSize (
                L"BootOrder",
                &gEfiGlobalVariableGuid,
                BootOrderSize
                );
  *BootOrderPtr = BootOrder;

  return EFI_SUCCESS;
}


/**
  Delete the Boot Option from EFI Variable. The Boot Order Arrray
  is also updated.

  @param OptionNumber    The number of Boot option want to be deleted.
  @param BootOrder       The Boot Order array.
  @param BootOrderSize   The size of the Boot Order Array.

  @retval  EFI_SUCCESS           The Boot Option Variable was found and removed
  @retval  EFI_UNSUPPORTED       The Boot Option Variable store was inaccessible
  @retval  EFI_NOT_FOUND         The Boot Option Variable was not found
**/
EFI_STATUS
EFIAPI
BdsLibDeleteBootOption (
  IN UINTN                       OptionNumber,
  IN OUT UINT16                  *BootOrder,
  IN OUT UINTN                   *BootOrderSize
  )
{
  CHAR16      BootOption[10];
  UINTN       Index;
  UINTN       BootOrderCount;
  EFI_STATUS  Status;

  BootOrderCount = *BootOrderSize / sizeof (UINT16);
  Status = GetIndexInOrderArray (BootOrder, BootOrderCount, (UINT16) OptionNumber, &Index);
  if (!EFI_ERROR (Status)) {
    CopyMem (
      &BootOrder[Index],
      &BootOrder[Index + 1],
      (BootOrderCount - Index - 1) * sizeof (UINT16)
      );
    *BootOrderSize -= sizeof (UINT16);
  }

  UnicodeSPrint (BootOption, sizeof (BootOption), L"Boot%04x", OptionNumber);
  return gRT->SetVariable (
                BootOption,
                &gEfiGlobalVariableGuid,
                EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                0,
                NULL
                );
}

/**
  Call Setup Utility by loading Setup Utility application.

  @retval EFI_SUCCESS    Call Setup Utility application successful
  @retval EFI_NOT_FOUND  Can not find Setup Utility application or load image fail
**/
EFI_STATUS
BdsLibCallScuApp (
  VOID
  )
{
  EFI_STATUS                          Status;
  UINTN                               HandleCount;
  EFI_HANDLE                          *HandleBuffer;
  EFI_GUID                            FileNameGuid = {0x3935B0A1, 0xA182, 0x4887, {0xbc, 0x56, 0x67, 0x55, 0x28, 0xe7, 0x88, 0x77}};
  MEDIA_FW_VOL_FILEPATH_DEVICE_PATH   FvFilePath;
  UINTN                               Index;
  EFI_FIRMWARE_VOLUME2_PROTOCOL       *Fv;
  UINTN                               Size;
  EFI_FV_FILETYPE                     Type;
  EFI_FV_FILE_ATTRIBUTES              Attributes;
  UINT32                              AuthenticationStatus;
  EFI_DEVICE_PATH_PROTOCOL            *DevicePath;
  EFI_DEVICE_PATH_PROTOCOL            *SetupAppDevicePath;
  EFI_HANDLE                          ImageHandle;
  UINTN                               ExitDataSize;
  CHAR16                              *ExitData;

  HandleCount  = 0;
  HandleBuffer = NULL;
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiFirmwareVolume2ProtocolGuid,
                  NULL,
                  &HandleCount,
                  &HandleBuffer
                  );
  if (Status != EFI_SUCCESS || HandleBuffer == NULL) {
    return EFI_NOT_FOUND;
  }

  EfiInitializeFwVolDevicepathNode (&FvFilePath, &FileNameGuid);

  for (Index = 0; Index < HandleCount; Index++) {
    Status = gBS->HandleProtocol (
                    HandleBuffer[Index],
                    &gEfiFirmwareVolume2ProtocolGuid,
                    (VOID **) &Fv
                    );
    if (!EFI_ERROR (Status)) {
      Status = Fv->ReadFile (
                     Fv,
                     &FileNameGuid,
                     NULL,
                     &Size,
                     &Type,
                     &Attributes,
                     &AuthenticationStatus
                     );
    }
    if (EFI_ERROR (Status)) {
      continue;
    }

    //
    // Create device path of setup utility application
    //
    DevicePath = DevicePathFromHandle (HandleBuffer[Index]);
    if (DevicePath == NULL) {
      continue;
    }
    SetupAppDevicePath = AppendDevicePathNode (DevicePath, (EFI_DEVICE_PATH_PROTOCOL *) &FvFilePath);
    if (SetupAppDevicePath == NULL) {
      continue;
    }

    Status = gBS->LoadImage (
                    TRUE,
                    gImageHandle,
                    SetupAppDevicePath,
                    NULL,
                    0,
                    &ImageHandle
                    );
    FreePool (SetupAppDevicePath);
    if (EFI_ERROR (Status)) {
      continue;
    }

    gBS->StartImage (ImageHandle, &ExitDataSize, &ExitData);
    break;
  }

  FreePool (HandleBuffer);

  return Status;
}

/**
  Start Setup Utility

  @retval EFI_SUCCESS
  @retval Other

**/
EFI_STATUS
BdsLibStartSetupUtility (
  BOOLEAN       PasswordCheck
  )
{
  EFI_STATUS                            Status;
  EFI_SETUP_UTILITY_PROTOCOL            *SetupUtility;

  if (PasswordCheck) {
    Status = gBS->LocateProtocol (
                    &gEfiSetupUtilityProtocolGuid,
                    NULL,
                    (VOID **) &SetupUtility
                    );
    if (!EFI_ERROR (Status)) {
      Status = SetupUtility->PowerOnSecurity (SetupUtility);
    }
  } else {
    Status = BdsLibCallScuApp ();
  }

  return Status;
}

/**
  check need update platform display device or not

  @param  SkipOriginalCode      TRUE : skip platform display device update
                                FALSE: need do platform display device update

  @retval EFI_SUCCESS   Get Platform display device policy successful
  @retval Other         Cannot get Platform display device policy successful

**/
EFI_STATUS
BdsLibDisplayDeviceReplace (
  OUT    BOOLEAN          *SkipOriginalCode
  )
{
  EFI_STATUS                    Status;

  if (SkipOriginalCode == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  *SkipOriginalCode = PcdGetBool (PcdBootDisplayDeviceReplace);

  //
  // OemServices
  //
  DEBUG ((DEBUG_INFO, "OemKernelServices Call: OemSvcBootDisplayDeviceReplace \n"));
  Status = OemSvcBootDisplayDeviceReplace (SkipOriginalCode);
  DEBUG ((DEBUG_INFO, "OemKernelServices OemSvcBootDisplayDeviceReplace Status: %r\n", Status));

  return Status;
}

EFI_STATUS
BdsLibOnStartOfBdsDiagnostics (
  VOID
  )
{
  EFI_STATUS                        Status;

  Status = gBS->InstallProtocolInterface (
                  &gImageHandle,
                  &gEfiStartOfBdsDiagnosticsProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  NULL
                  );

  return EFI_SUCCESS;
}

EFI_STATUS
BdsLibOnEndOfBdsBootSelection (
  void
  )
{
  EFI_STATUS                        Status;

  //
  // PostCode = 0x28, End of boot selection
  //
  POST_CODE (BDS_END_OF_BOOT_SELECTION);
  Status = gBS->InstallProtocolInterface (
                  &gImageHandle,
                  &gEndOfBdsBootSelectionProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  NULL
                  );
  gDS->Dispatch();

  return EFI_SUCCESS;
}

EFI_STATUS
BdsLibShowOemStringInTextMode (
  IN BOOLEAN                       AfterSelect,
  IN UINT8                         SelectedStringNum
  )
{
  EFI_STATUS                              Status;
  UINTN                                   LocX;
  UINTN                                   LocY;
  UINTN                                   CursorRow;
  UINTN                                   CursorCol;
  CHAR16                                  *StringData;
  UINTN                                   StringCount;
  UINTN                                   Index;
  EFI_OEM_BADGING_SUPPORT_PROTOCOL        *Badging;
  UINTN                                   MaxX;
  UINTN                                   MaxY;
  EFI_UGA_PIXEL                           Foreground;
  EFI_UGA_PIXEL                           Background;
  H2O_BDS_CP_DISPLAY_STRING_BEFORE_DATA   *BdsDisplayStringBeforeData;

  Status  = gBS->LocateProtocol (&gEfiOEMBadgingSupportProtocolGuid, NULL, (VOID **)&Badging);
  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }

  if (FeaturePcdGet (PcdH2OBdsCpDisplayStringBeforeSupported)) {
    TriggerCpDisplayStringBefore (Badging, AfterSelect, SelectedStringNum);
    Status = gBS->LocateProtocol (
                    &gH2OBdsCpDisplayStringBeforeGuid,
                    NULL,
                    (VOID **) &BdsDisplayStringBeforeData
                    );
    if (!EFI_ERROR (Status) && BdsDisplayStringBeforeData->Status == H2O_BDS_TASK_SKIP) {
      return Status;
    }
  }

  CursorRow = gST->ConOut->Mode->CursorRow;
  CursorCol = gST->ConOut->Mode->CursorColumn;

  LocX = 0;
  LocY = 0;

  Status = gST->ConOut->QueryMode (
                          gST->ConOut,
                          gST->ConOut->Mode->Mode,
                          &MaxX,
                          &MaxY
                          );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  if (FeaturePcdGet (PcdDynamicHotKeySupported)) {
    DYNAMIC_HOTKEY_PROTOCOL         *DynamicHotKey;

    DynamicHotKey = NULL;
    Status = gBS->LocateProtocol (&gDynamicHotKeyProtocolGuid, NULL, (VOID **)&DynamicHotKey);
    if (!EFI_ERROR (Status)) {
      //
      //  Get POST string count
      //
      Status = DynamicHotKey->GetDynamicStringCount (DynamicHotKey, AfterSelect, &StringCount);
      if (!EFI_ERROR (Status)) {
        gST->ConOut->EnableCursor (gST->ConOut, FALSE);

        for (Index = 0 ; Index < StringCount ; Index++) {
          //
          //  Get POST string
          //
          Status = DynamicHotKey->GetDynamicString (DynamicHotKey, Index, AfterSelect, &StringData, &LocX, &LocY);
          if (!EFI_ERROR (Status)) {
            DynamicHotKey->AdjustStringPosition (DynamicHotKey, FALSE, FALSE, (MaxY / 2), StringData, &LocX, &LocY);
            gST->ConOut->SetCursorPosition (gST->ConOut, LocX, LocY);
            BdsLibOutputStrings (gST->ConOut, StringData, NULL);
            FreePool (StringData);
          }
        }
        DynamicHotKey->AdjustStringPosition (DynamicHotKey, TRUE, FALSE, 0, NULL, NULL, NULL);
      }
      gST->ConOut->SetCursorPosition (gST->ConOut, 0, CursorCol);

      return Status;
    }
  }

  Status = Badging->GetStringCount (Badging, &StringCount);
  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }

  MaxY = MaxY - StringCount - 1;
  gST->ConOut->SetCursorPosition (gST->ConOut, 0, MaxY);
  gST->ConOut->EnableCursor (gST->ConOut, FALSE);

  for (Index = 0; Index < StringCount ; Index++) {
    if (Badging->GetOemString(Badging, Index, AfterSelect, SelectedStringNum, &StringData, &LocX, &LocY, &Foreground, &Background) ) {
      BdsLibOutputStrings (gST->ConOut, L"\n\r", StringData, NULL);
      FreePool (StringData);
    }
  }
  gST->ConOut->SetCursorPosition (gST->ConOut, 0, CursorCol);

  return EFI_SUCCESS;
}

EFI_STATUS
BdsLibSkipEbcDispatch (
  VOID
  )
{
  EFI_EBC_PROTOCOL            *EbcProtocol;
  EFI_STATUS                  Status;
  EFI_HANDLE                  *HandleBuffer;
  UINTN                       NumHandles;
  UINTN                       Index;

  HandleBuffer  = NULL;
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiEbcProtocolGuid,
                  NULL,
                  &NumHandles,
                  &HandleBuffer
                  );
  if (Status == EFI_SUCCESS) {
    //
    // Loop through the handles
    //
    for (Index = 0; Index < NumHandles; Index++) {
      Status = gBS->HandleProtocol (
                      HandleBuffer[Index],
                      &gEfiEbcProtocolGuid,
                      (VOID **) &EbcProtocol
                      );
      if (Status == EFI_SUCCESS) {
        gBS->UninstallProtocolInterface (
               HandleBuffer[Index],
               &gEfiEbcProtocolGuid,
               EbcProtocol
               );
      }
    }
  }

  if (HandleBuffer != NULL) {
    FreePool (HandleBuffer);
    HandleBuffer = NULL;
  }

  return Status;
}

UINTN
BdsLibGetBootType (
  VOID
  )
{
  UINT8        BootType;

  BootType = DUAL_BOOT_TYPE;
  gBdsServices->GetBootType (gBdsServices, &BootType);
  return BootType;
}

/**

  Show progress bar with title above it. It only works in Graphics mode.


  @param TitleForeground Foreground color for Title.
  @param TitleBackground Background color for Title.
  @param Title           Title above progress bar.
  @param ProgressColor   Progress bar color.
  @param Progress        Progress (0-100)
  @param PreviousValue   The previous value of the progress.

  @retval  EFI_STATUS       Success update the progress bar

**/
EFI_STATUS
BdsLibShowProgress (
  IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL TitleForeground,
  IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL TitleBackground,
  IN CHAR16                        *Title,
  IN EFI_GRAPHICS_OUTPUT_BLT_PIXEL ProgressColor,
  IN UINTN                         Progress,
  IN UINTN                         PreviousValue
  )
{
  EFI_STATUS                     Status;
  EFI_GRAPHICS_OUTPUT_PROTOCOL   *GraphicsOutput;
  EFI_UGA_DRAW_PROTOCOL          *UgaDraw;
  UINT32                         SizeOfX;
  UINT32                         SizeOfY;
  UINT32                         ColorDepth;
  UINT32                         RefreshRate;
  UINTN                          PosY;
  CHAR16                         *PrintStr;
  CHAR16                         *TmpStr;
  CHAR16                         StrPercent[16];

  if (Progress > 100) {
    return EFI_INVALID_PARAMETER;
  }

  UgaDraw = NULL;
  Status = gBS->HandleProtocol (
                  gST->ConsoleOutHandle,
                  &gEfiGraphicsOutputProtocolGuid,
                  (VOID **) &GraphicsOutput
                  );
  if (EFI_ERROR (Status) && FeaturePcdGet (PcdUgaConsumeSupport)) {
    GraphicsOutput = NULL;

    Status = gBS->HandleProtocol (
                    gST->ConsoleOutHandle,
                    &gEfiUgaDrawProtocolGuid,
                    (VOID **) &UgaDraw
                    );
  }
  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }

  SizeOfX = 0;
  SizeOfY = 0;
  if (GraphicsOutput != NULL) {
    SizeOfX = GraphicsOutput->Mode->Info->HorizontalResolution;
    SizeOfY = GraphicsOutput->Mode->Info->VerticalResolution;
  } else if (UgaDraw != NULL) {
    Status = UgaDraw->GetMode (
                        UgaDraw,
                        &SizeOfX,
                        &SizeOfY,
                        &ColorDepth,
                        &RefreshRate
                        );
    if (EFI_ERROR (Status)) {
      return EFI_UNSUPPORTED;
    }
  } else {
    return EFI_UNSUPPORTED;
  }

  PosY        = SizeOfY * 48 / 50;

  PrintXY (
    (SizeOfX - StrLen (Title) * EFI_GLYPH_WIDTH) / 2,
    PosY - EFI_GLYPH_HEIGHT - 1,
    &TitleForeground,
    &TitleBackground,
    Title
    );

  //
  // Show Memory test progress percentage
  //
  TmpStr = BdsLibGetStringById (STRING_TOKEN (STR_MEMORY_TEST_PERCENT));
  if (TmpStr != NULL) {
    UnicodeValueToStringS (StrPercent, sizeof(StrPercent),  0, Progress, 0);
    StrCatS (StrPercent, sizeof(StrPercent) / sizeof(CHAR16), L"%");
    PrintStr = AllocateZeroPool (StrSize (StrPercent) + StrSize (TmpStr));
    if (PrintStr == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
    CopyMem (PrintStr, StrPercent, StrSize (StrPercent));
    StrCatS (PrintStr, (StrSize (StrPercent) + StrSize (TmpStr)) / sizeof(CHAR16), TmpStr);
    PrintXY (
      (SizeOfX - StrLen (PrintStr) * EFI_GLYPH_WIDTH) / 2,
      PosY - 1,
      &TitleForeground,
      &TitleBackground,
      PrintStr
      );
    FreePool (TmpStr);
    FreePool (PrintStr);
  }

  return EFI_SUCCESS;
}

/**
  Delete all of invalid Boot#### variable in input order variable.

  @param[in] VariableName       A Null-terminated string that is the name of the vendor's variable.
                                 Caller is responsible freeing the buffer.
  @Param[in] VendorGuid         A unique identifier for the vendor.

  @retval EFI_SUCCESS   Delete invalid boot options successful.
  @retval Other         Delete invalid boot options failed.

**/
STATIC
EFI_STATUS
DeleteInvalidBootOpitonsInOrder (
  IN  CHAR16       *VariableName,
  IN  EFI_GUID     *VendorGuid
  )
{
  UINT16                    *BootOrder;
  UINT8                     *BootOptionVar;
  UINTN                     BootOrderSize;
  UINTN                     BootOptionSize;
  EFI_STATUS                Status;
  UINTN                     Index;
  UINT16                    BootOption[BOOT_OPTION_MAX_CHAR];

  BootOrder = BdsLibGetVariableAndSize (
                VariableName,
                VendorGuid,
                &BootOrderSize
                );
  if (BootOrder == NULL) {
    return EFI_SUCCESS;
  }

  Index = 0;
  while (Index < BootOrderSize / sizeof (UINT16)) {
    UnicodeSPrint (BootOption, sizeof (BootOption), L"Boot%04x", BootOrder[Index]);
    BootOptionVar = BdsLibGetVariableAndSize (
                      BootOption,
                      &gEfiGlobalVariableGuid,
                      &BootOptionSize
                      );
    if (NULL == BootOptionVar) {
      Index++;
      continue;
    }

    if (!ValidateOption (BootOptionVar, BootOptionSize)) {
      BdsLibDeleteBootOption (BootOrder[Index], BootOrder, &BootOrderSize);
      FreePool (BootOptionVar);
      continue;
    }

    FreePool (BootOptionVar);
    Index++;
  }

  Status = gRT->SetVariable (
                  VariableName,
                  VendorGuid,
                  EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                  BootOrderSize,
                  BootOrder
                  );

  FreePool (BootOrder);

  return Status;

}

/**
  Delete all of invalid boot options which option number is saved
  in L"BootOrder" variable.

  @retval EFI_SUCCESS   Delete invalid boot options successful.
  @retval Other         Delete invalid boot options failed.

**/
EFI_STATUS
BdsLibDeleteInvalidBootOptions (
  VOID
  )
{
  DeleteInvalidBootOpitonsInOrder (L"BootOrder", &gEfiGlobalVariableGuid);
  DeleteInvalidBootOpitonsInOrder (PHYSICAL_BOOT_ORDER_NAME, &gEfiGenericVariableGuid);
  return EFI_SUCCESS;
}

/**
  Clean space character in the front and back of input string.

  @param  Str           Input string

**/
VOID
CleanSpaceChar (
  IN    CHAR16    *Str
  )
{
  UINTN     StrLength;
  UINTN     Start;
  UINTN     End;
  UINTN     ValidLength;
  CHAR16    SpaceChar = ' ';

  if (Str == NULL) {
    return;
  }

  StrLength = StrLen (Str);
  if (StrLength == 0) {
    return;
  }

  Start = 0;
  End   = StrLength - 1;

  while (Str[Start] == SpaceChar) {
    Start++;
  }
  if (Start == StrLength) {
    //
    // All chars are space char, no need to remove space chars.
    //
    return;
  }

  while (Str[End] == SpaceChar) {
    End--;
  }

  ValidLength = End - Start + 1;
  if (ValidLength < StrLength) {
    gBS->CopyMem (&Str[0], &Str[Start], ValidLength * sizeof(CHAR16));
    gBS->SetMem (&Str[ValidLength], (StrLength - ValidLength) * sizeof(CHAR16), 0);
  }
}

/**
  Check whether the EFI_COMPONENT_NAME_PROTOCOL instance is valid for use.

  @param[in] ComponentName      A pointer to the EFI_COMPONENT_NAME_PROTOCOL instance
  @param[in] Handle             The handle of Block IO device

  @retval TRUE                  The protocol is valid
  @retval FALSE                 The protocol is invalid

**/
BOOLEAN
IsValidComponentNameProtocol (
  IN  EFI_COMPONENT_NAME_PROTOCOL    *ComponentName,
  IN  EFI_HANDLE                     Handle
  )
{
  EFI_STATUS                     Status;
  CHAR16                         *String1;
  CHAR16                         *String2;

  //
  // Some incomplete ComponentName protocols return success regardless of invalid inputs, which brings the wrong device name received.
  // Try the cases that gives illegal inputs to filter these incomplete ComponentName protocol.
  //
  // Give illegal inputs (ControllerHandle = NULL)
  //
  Status = ComponentName->GetControllerName (
                            ComponentName,
                            NULL,
                            Handle,
                            ComponentName->SupportedLanguages,
                            &String1
                            );
  if (EFI_ERROR (Status)) {
    return TRUE;
  }

  //
  // Give illegal inputs (ControllerHandle = NULL, ChildHandle = NULL)
  //
  Status = ComponentName->GetControllerName (
                            ComponentName,
                            NULL,
                            NULL,
                            ComponentName->SupportedLanguages,
                            &String2
                            );
  if (EFI_ERROR (Status)) {
    return TRUE;
  }

  //
  // The protocol that return a fixed data in the above cases is considered invalid
  //
  return (CompareMem (String1, String2, StrLen (String1)) != 0);
}

/**
 Get device name from Component Name (2) protocol.

 @param[in] ControllerHandle             The handle of a controller
 @param[in] BlockIoHandle                The handle of a Block IO device
 @param[in] ComponentNameProtocolGuid    A pointer to an EFI_GUID. It points to Component Name (2) protocol GUID
 @param[in] ComponentNameHandleCount     The number of Component Name (2) protocol handles
 @param[in] ComponentNameHandleList      A pointer to an array of Component Name (2) protocol handles

 @return A pointer to device name or NULL if not found.
**/
CHAR16 *
GetDevNameFromComponentNameProtocol (
  IN EFI_HANDLE                          ControllerHandle,
  IN EFI_HANDLE                          BlockIoHandle,
  IN EFI_GUID                            *ComponentNameProtocolGuid,
  IN UINTN                               ComponentNameHandleCount,
  IN EFI_HANDLE                          *ComponentNameHandleList
  )
{
  UINTN                                  Index;
  EFI_COMPONENT_NAME_PROTOCOL            *ComponentName;
  CHAR16                                 *DeviceName;
  EFI_STATUS                             Status;

  for (Index = 0; Index < ComponentNameHandleCount; Index++) {
    Status = gBS->HandleProtocol (
                    ComponentNameHandleList[Index],
                    ComponentNameProtocolGuid,
                    (VOID **) &ComponentName
                    );
    if (EFI_ERROR (Status)) {
      continue;
    }

    Status = ComponentName->GetControllerName (
                               ComponentName,
                               ControllerHandle,
                               BlockIoHandle,
                               ComponentName->SupportedLanguages,
                               &DeviceName
                               );
    if (EFI_ERROR (Status)) {
      //
      // For some Single-LUN storages, BlockIo protocol may be installed on the ControllerHandle instead of a new handle created.
      // Trying the BlockIo handle as the ControllerHandle for retrieving the device name.
      //
      Status = ComponentName->GetControllerName (
                                 ComponentName,
                                 BlockIoHandle,
                                 BlockIoHandle,
                                 ComponentName->SupportedLanguages,
                                 &DeviceName
                                 );
    }
    if (EFI_ERROR (Status)) {
      continue;
    }
    if (!IsValidComponentNameProtocol (ComponentName, BlockIoHandle)) {
      continue;
    }
    return DeviceName;
  }

  return NULL;
}

/**
  Get all hardware boot device information (Block IO device path and device name).

  @param  HwBootDeviceInfoCount    Number of hardware boot device information
  @param  HwBootDeviceInfo         Array pointer of hardware boot device information

  @retval EFI_SUCCESS   Get information successfully
  @retval Other         Locate protocol fail or get usb device information fail

**/
EFI_STATUS
BdsLibGetAllHwBootDeviceInfo (
  OUT UINTN                          *HwBootDeviceInfoCount,
  OUT HARDWARE_BOOT_DEVICE_INFO      **HwBootDeviceInfo
  )
{
  EFI_STATUS                     Status;
  UINTN                          HandleCount;
  EFI_HANDLE                     *HandleBuffer;
  UINTN                          Index;
  EFI_DEVICE_PATH_PROTOCOL       *BlkIoDevicePath;
  EFI_DEVICE_PATH_PROTOCOL       *TempDevicePath;
  EFI_DEVICE_PATH_PROTOCOL       *AppendedDevicePath;
  EFI_DEVICE_PATH_PROTOCOL       *ControllerDevicePath;
  EFI_HANDLE                     ControllerHandle;
  CHAR16                         *DeviceName;
  UINTN                          ComponentNameHandleCount;
  UINTN                          ComponentName2HandleCount;
  EFI_HANDLE                     *ComponentNameHandleBuffer;
  EFI_HANDLE                     *ComponentName2HandleBuffer;
  UINTN                          HwBootDeviceInfoLength;
  UINTN                          Increment;
  BOOLEAN                        StopSearching;

  if (HwBootDeviceInfoCount == NULL || HwBootDeviceInfo == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  *HwBootDeviceInfoCount = 0;
  *HwBootDeviceInfo = NULL;

  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiBlockIoProtocolGuid,
                  NULL,
                  &HandleCount,
                  &HandleBuffer
                  );
  if (EFI_ERROR (Status)) {
    return EFI_NOT_FOUND;
  }

  Status = gBS->LocateHandleBuffer (ByProtocol, &gEfiComponentNameProtocolGuid , NULL, &ComponentNameHandleCount , &ComponentNameHandleBuffer);
  if (EFI_ERROR (Status)) {
    ComponentNameHandleCount   = 0;
    ComponentNameHandleBuffer  = NULL;
  }
  Status = gBS->LocateHandleBuffer (ByProtocol, &gEfiComponentName2ProtocolGuid, NULL, &ComponentName2HandleCount, &ComponentName2HandleBuffer);
  if (EFI_ERROR (Status)) {
    ComponentName2HandleCount  = 0;
    ComponentName2HandleBuffer = NULL;
  }
  if (ComponentNameHandleCount == 0 && ComponentName2HandleCount == 0) {
    goto Exit;
  }

  Increment = 10;
  HwBootDeviceInfoLength = 0;
  for (Index = 0; Index < HandleCount; Index++) {
    Status = gBS->HandleProtocol (
                    HandleBuffer[Index],
                    &gEfiDevicePathProtocolGuid,
                    (VOID *) &BlkIoDevicePath
                    );
    if (EFI_ERROR (Status)) {
      continue;
    }

    //
    // Skip media level BlkIo instance
    //
    TempDevicePath = BlkIoDevicePath;
    while (!IsDevicePathEnd (TempDevicePath) && DevicePathType (TempDevicePath) != MEDIA_DEVICE_PATH) {
      TempDevicePath = NextDevicePathNode (TempDevicePath);
    }
    if (!IsDevicePathEnd (TempDevicePath)) {
      continue;
    }

    //
    // Find contoller handle and retrieve the device name
    //
    StopSearching    = FALSE;
    ControllerHandle = HandleBuffer[Index];
    while (!StopSearching) {
      Status = gBS->HandleProtocol (
                      ControllerHandle,
                      &gEfiDevicePathProtocolGuid,
                      (VOID *)&TempDevicePath
                      );
      if (EFI_ERROR (Status)) {
        break;
      }

      AppendedDevicePath   = NULL;
      ControllerDevicePath = NULL;
      while (!IsDevicePathEnd (NextDevicePathNode (TempDevicePath))) {
        ControllerDevicePath = AppendDevicePathNode (AppendedDevicePath, TempDevicePath);
        if (AppendedDevicePath != NULL) {
          FreePool (AppendedDevicePath);
        }
        AppendedDevicePath = ControllerDevicePath;
        TempDevicePath = NextDevicePathNode (TempDevicePath);
      }

      //
      // Stop the searching at PCI controller device path
      //
      StopSearching = ((DevicePathType (TempDevicePath) == HARDWARE_DEVICE_PATH) &&
                      (DevicePathSubType (TempDevicePath) == HW_PCI_DP));

      Status = gBS->LocateDevicePath (
                      &gEfiDevicePathProtocolGuid,
                      &ControllerDevicePath,
                      &ControllerHandle
                      );
      if (AppendedDevicePath != NULL) {
        FreePool (AppendedDevicePath);
      }
      if (EFI_ERROR (Status)) {
        break;
      }

      DeviceName = GetDevNameFromComponentNameProtocol (ControllerHandle, HandleBuffer[Index], &gEfiComponentName2ProtocolGuid, ComponentName2HandleCount, ComponentName2HandleBuffer);
      if (DeviceName == NULL) {
        DeviceName = GetDevNameFromComponentNameProtocol (ControllerHandle, HandleBuffer[Index], &gEfiComponentNameProtocolGuid, ComponentNameHandleCount, ComponentNameHandleBuffer);
        if (DeviceName == NULL) {
          continue;
        }
      }

      if ((*HwBootDeviceInfoCount) >= HwBootDeviceInfoLength) {
        *HwBootDeviceInfo = ReallocatePool (
                              HwBootDeviceInfoLength * sizeof(HARDWARE_BOOT_DEVICE_INFO),
                              (HwBootDeviceInfoLength + Increment) * sizeof(HARDWARE_BOOT_DEVICE_INFO),
                              *HwBootDeviceInfo
                              );
        if (*HwBootDeviceInfo == NULL) {
          Status = EFI_OUT_OF_RESOURCES;
          goto Exit;
        }
        HwBootDeviceInfoLength += Increment;
      }
      (*HwBootDeviceInfo)[*HwBootDeviceInfoCount].BlockIoDevicePath = BlkIoDevicePath;
      (*HwBootDeviceInfo)[*HwBootDeviceInfoCount].HwDeviceName      = AllocateCopyPool (StrSize (DeviceName), DeviceName);
      CleanSpaceChar ((*HwBootDeviceInfo)[*HwBootDeviceInfoCount].HwDeviceName);

      (*HwBootDeviceInfoCount)++;
      break;
    }
  }

Exit:
  if (HandleCount != 0) {
    FreePool (HandleBuffer);
  }
  if (ComponentNameHandleCount != 0) {
    FreePool (ComponentNameHandleBuffer);
  }
  if (ComponentName2HandleBuffer != NULL) {
    FreePool (ComponentName2HandleBuffer);
  }
  if (*HwBootDeviceInfoCount == 0) {
    Status = EFI_NOT_FOUND;
  }

  return Status;
}

/**
  If input is a hard drive device path, append it to corresponding BlockIo device path.
  If input is not a hard drive device path, output NULL.

  @param  HardDriveDevicePath         Input device path

  @return Device path which combines BlockIo and hard drive device path

**/
EFI_DEVICE_PATH_PROTOCOL *
AppendHardDrivePathToBlkIoDevicePath (
  IN  HARDDRIVE_DEVICE_PATH      *HardDriveDevicePath
  )
{
  EFI_STATUS                Status;
  UINTN                     BlockIoHandleCount;
  EFI_HANDLE                *BlockIoBuffer;
  EFI_DEVICE_PATH_PROTOCOL  *BlockIoDevicePath;
  EFI_DEVICE_PATH_PROTOCOL  *DevicePath;
  EFI_DEVICE_PATH_PROTOCOL  *NewDevicePath;
  UINTN                     Index;

  if (HardDriveDevicePath == NULL) {
    return NULL;
  }

  if (!((DevicePathType (&HardDriveDevicePath->Header) == MEDIA_DEVICE_PATH) &&
        (DevicePathSubType (&HardDriveDevicePath->Header) == MEDIA_HARDDRIVE_DP))) {
    return NULL;
  }

  Status = gBS->LocateHandleBuffer (ByProtocol, &gEfiBlockIoProtocolGuid, NULL, &BlockIoHandleCount, &BlockIoBuffer);
  if (EFI_ERROR (Status) || BlockIoHandleCount == 0) {
    return NULL;
  }

  for (Index = 0; Index < BlockIoHandleCount; Index++) {
    Status = gBS->HandleProtocol (BlockIoBuffer[Index], &gEfiDevicePathProtocolGuid, (VOID *) &BlockIoDevicePath);
    if (EFI_ERROR (Status) || BlockIoDevicePath == NULL) {
      continue;
    }

    if (MatchPartitionDevicePathNode (BlockIoDevicePath, HardDriveDevicePath)) {
      //
      // Combine the Block IO and Hard Drive Device path together.
      //
      DevicePath = NextDevicePathNode ((EFI_DEVICE_PATH_PROTOCOL *) HardDriveDevicePath);
      NewDevicePath = AppendDevicePath (BlockIoDevicePath, DevicePath);

      FreePool (BlockIoBuffer);
      return NewDevicePath;
    }
  }

  FreePool (BlockIoBuffer);
  return NULL;
}


/**
  Compare with each BlockIo device path.

  @param  HardDriveDevicePath      BlockIo device path
  @param  DevicePath               BlockIo device path or hard drive device path

  @retval TRUE           BlockIo device paths are the same
  @retval FALSE          BlockIo device paths are different

**/
BOOLEAN
BdsLibCompareBlockIoDevicePath (
  IN EFI_DEVICE_PATH_PROTOCOL *BlockIoDevicePath,
  IN EFI_DEVICE_PATH_PROTOCOL *DevicePath
  )
{
  EFI_DEVICE_PATH_PROTOCOL *NewDevicePath;
  EFI_DEVICE_PATH_PROTOCOL *TempDevicePath;
  BOOLEAN                  Match;

  Match = TRUE;

  if (BlockIoDevicePath == NULL || DevicePath == NULL || IsDevicePathEnd (BlockIoDevicePath)) {
    return FALSE;
  }

  NewDevicePath = AppendHardDrivePathToBlkIoDevicePath ((HARDDRIVE_DEVICE_PATH *) DevicePath);
  if (NewDevicePath == NULL) {
    NewDevicePath = DevicePath;
  }

  TempDevicePath = NewDevicePath;
  while (!IsDevicePathEnd (BlockIoDevicePath)) {
    if (CompareMem (BlockIoDevicePath, TempDevicePath, DevicePathNodeLength (BlockIoDevicePath)) != 0) {
      Match = FALSE;
      break;
    }

    BlockIoDevicePath = NextDevicePathNode (BlockIoDevicePath);
    TempDevicePath = NextDevicePathNode (TempDevicePath);
  }

  if (NewDevicePath != DevicePath) {
    FreePool (NewDevicePath);
  }

  if (Match) {
    return TRUE;
  }

  return FALSE;
}


/**
  This function allocates memory to generate load option. It is caller's responsibility
  to free load option if caller no longer requires the content of load option.

  @param  DevicePath         Pointer to a packed array of UEFI device paths.
  @param  Description        The user readable description for the load option.
  @param  OptionalData       Pointer to optional data for load option.
  @param  OptionalDataSize   The size of optional data.
  @param  LoadOption         Double pointer to load option.
  @param  LoadOptionSize     The load option size by byte.

  @retval EFI_INVALID_PARAMETER  Any input parameter is invalid.
  @retval EFI_OUT_OF_RESOURCES   Allocate memory failed.
  @retval EFI_SUCCESS            Generate load option successful.

**/
EFI_STATUS
BdsLibCreateLoadOption (
  IN  EFI_DEVICE_PATH_PROTOCOL       *DevicePath,
  IN  CHAR16                         *Description,
  IN  UINT8                          *OptionalData,   OPTIONAL
  IN  UINT32                         OptionalDataSize,
  OUT VOID                           **LoadOption,
  OUT UINTN                          *LoadOptionSize
  )
{
  UINTN             OptionSize;
  VOID              *OptionPtr;
  UINT8             *WorkingPtr;

  if (DevicePath == NULL || Description == NULL || LoadOption == NULL || LoadOptionSize == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  if (OptionalData == NULL) {
    OptionalDataSize = 0;
  }

  OptionSize = sizeof (UINT32) + sizeof (UINT16) + StrSize (Description) + GetDevicePathSize (DevicePath) + OptionalDataSize;
  OptionPtr  = AllocateZeroPool (OptionSize);
  if (OptionPtr == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  WorkingPtr = (UINT8 *) OptionPtr;
  *(UINT32 *) WorkingPtr = LOAD_OPTION_ACTIVE;
  WorkingPtr += sizeof (UINT32);
  *(UINT16 *) WorkingPtr = (UINT16) GetDevicePathSize (DevicePath);
  WorkingPtr += sizeof (UINT16);
  CopyMem (WorkingPtr, Description, StrSize (Description));
  WorkingPtr += StrSize (Description);
  CopyMem (WorkingPtr, DevicePath, GetDevicePathSize (DevicePath));
  WorkingPtr += GetDevicePathSize (DevicePath);
  CopyMem (WorkingPtr, OptionalData, OptionalDataSize);

  *LoadOption = OptionPtr;
  *LoadOptionSize = OptionSize;

  return EFI_SUCCESS;
}

/**
  According to boot option number to get the description from this boot option.
  It is caller's responsibility to free the description if caller no longer requires
  the content of description.

  @param  OptionNum              The boot option number.

  @return NULL                   Get description from boot option failed.
  @return Other                  Get description from boot option successful.

**/
CHAR16 *
BdsLibGetDescriptionFromBootOption (
  IN UINT16     OptionNum
  )
{
  UINT16                     BootOption[BOOT_OPTION_MAX_CHAR];
  VOID                       *BootOptionVar;
  UINTN                      VariableSize;
  UINT8                      *WorkingPtr;
  CHAR16                     *Description;
  UINTN                      DescriptionSize;

  UnicodeSPrint (BootOption, sizeof (BootOption), L"Boot%04x", OptionNum);
  BootOptionVar = BdsLibGetVariableAndSize (
                    BootOption,
                    &gEfiGlobalVariableGuid,
                    &VariableSize
                    );
  if (BootOptionVar == NULL) {
    return NULL;
  }

  //
  // Get the description start address in Boot option variable
  //
  WorkingPtr = (UINT8 *) BootOptionVar;
  WorkingPtr += (sizeof (UINT32) + sizeof (UINT16));
  DescriptionSize = StrSize ((CHAR16 *) WorkingPtr);

  //
  // Copy the contents of device path to allocated memory
  //
  Description = AllocateCopyPool (DescriptionSize, WorkingPtr);
  FreePool (BootOptionVar);

  return Description;
}


/**
  According to boot option number to get the device path from this boot option.
  It is caller's responsibility to free the device path if caller no longer requires
  the content of device path.

  @param  OptionNum              The boot option number.

  @return NULL                   Get device path from boot option failed.
  @return Other                  Get device path from boot option successful.

**/
EFI_DEVICE_PATH_PROTOCOL *
BdsLibGetDevicePathFromBootOption (
  IN  UINT16     OptionNum
  )
{
  UINT16                     BootOption[BOOT_OPTION_MAX_CHAR];
  VOID                       *BootOptionVar;
  UINTN                      VariableSize;
  UINT8                      *WorkingPtr;
  CHAR16                     *Description;
  UINT16                     DevicePathLength;
  EFI_DEVICE_PATH_PROTOCOL   *DevicePath;

  UnicodeSPrint (BootOption, sizeof (BootOption), L"Boot%04x", OptionNum);
  BootOptionVar = BdsLibGetVariableAndSize (
                    BootOption,
                    &gEfiGlobalVariableGuid,
                    &VariableSize
                    );
  if (BootOptionVar == NULL) {
    return NULL;
  }

  //
  // Get the device path start address in Boot option variable
  //
  WorkingPtr = (UINT8 *) BootOptionVar;
  WorkingPtr += sizeof (UINT32);
  DevicePathLength = *((UINT16 *) WorkingPtr);
  WorkingPtr += sizeof (UINT16);
  Description = (UINT16 *) WorkingPtr;
  WorkingPtr += StrSize (Description);

  //
  // Copy the contents of device path to allocated memory
  //
  DevicePath = AllocateCopyPool (DevicePathLength, WorkingPtr);
  FreePool (BootOptionVar);

  return DevicePath;
}

/**
  After checking system password, the checking flag is set to disable.
  Using this function to set the checking flag enable

  @retval EFI_SUCCESS             set the checking flag successful.

**/
EFI_STATUS
SetSysPasswordCheck (
  VOID
  )
{
  EFI_SYS_PASSWORD_SERVICE_PROTOCOL       *SysPasswordService;
  EFI_STATUS                              Status;
  SYS_PASSWORD_SETUP_INFO_DATA            SetupInfoBuffer;
  UINT32                                  SetupBits;

  SetupBits = 0;

  Status = gBS->LocateProtocol (
                  &gEfiSysPasswordServiceProtocolGuid,
                  NULL,
                  (VOID **) &SysPasswordService
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = SysPasswordService->GetSysPswdSetupInfoData(
                                 SysPasswordService,
                                 &SetupInfoBuffer
                                 );

  SetupInfoBuffer.HaveSysPasswordCheck = TRUE;

  SetupBits = SYS_PASSWORD_CHECK_BIT;

  Status = SysPasswordService->SetSysPswdSetupInfoData(
                                 SysPasswordService,
                                 &SetupInfoBuffer,
                                 SetupBits
                                 );

  return Status;
}

/**
  Set all boot options in boot list as connected.
**/
VOID
SetAllBootOptionsConnected (
  VOID
  )
{
  LIST_ENTRY             *OptionList;
  LIST_ENTRY             *Link;
  H2O_BDS_LOAD_OPTION    *CurrentLoadOption;
  EFI_STATUS             Status;

  Status = gBdsServices->GetBootList (gBdsServices, &OptionList);
  if (Status != EFI_SUCCESS) {
    return;
  }
  for (Link = GetFirstNode (OptionList); !IsNull (OptionList, Link); Link = GetNextNode (OptionList, Link)) {
    CurrentLoadOption = BDS_OPTION_FROM_LINK (Link);
    CurrentLoadOption->Connected = TRUE;
  }
}

/**
  Default behavior for a boot attempt fails. This is also a default implementation
  and can be customized in gH2OBdsCpBootFailedGuid checkpoint.

  @param[in]  Option                  Pointer to Boot Option that succeeded to boot.
  @param[in]  Status                  Status returned from failed boot.
  @param[in]  ExitData                Exit data returned from failed boot.
  @param[in]  ExitDataSize            Exit data size returned from failed boot.
**/
VOID
EFIAPI
BdsLibBootFailed (
  IN  H2O_BDS_LOAD_OPTION  *Option,
  IN  EFI_STATUS           Status,
  IN  CHAR16               *ExitData,
  IN  UINTN                ExitDataSize
  )
{
  CHAR16                           *BootErrorString;
  CHAR16                           *PrintString;
  H2O_DIALOG_PROTOCOL              *H2ODialog;
  EFI_INPUT_KEY                    Key;
  EFI_STATUS                       LocateStatus;
  UINTN                            PrintStringBufSize;

  if (FeaturePcdGet (PcdH2OBdsCpBootFailedSupported)) {
    H2O_BDS_CP_BOOT_FAILED_DATA   BdsBootFailedData;

    BdsBootFailedData.Size         = sizeof (H2O_BDS_CP_BOOT_FAILED_DATA);
    BdsBootFailedData.Status       = H2O_CP_TASK_NORMAL;
    BdsBootFailedData.ReturnStatus = Status;
    BdsBootFailedData.ExitData     = ExitData;
    BdsBootFailedData.ExitDataSize = ExitDataSize;
    DEBUG ((DEBUG_INFO, "Checkpoint Trigger: %g\n", &gH2OBdsCpBootFailedGuid));
    H2OCpTrigger (&gH2OBdsCpBootFailedGuid, &BdsBootFailedData);
    DEBUG ((DEBUG_INFO, "Checkpoint Result: %x\n", BdsBootFailedData.Status));
    if (BdsBootFailedData.Status == H2O_BDS_TASK_SKIP) {
      return;
    }
  }
  LocateStatus = gBS->LocateProtocol (
                        &gH2ODialogProtocolGuid,
                        NULL,
                        (VOID **)&H2ODialog
                        );
  ASSERT_EFI_ERROR (LocateStatus);
  if (EFI_ERROR (LocateStatus)) {
    return;
  }

  PrintString = NULL;
  if (Status == EFI_SECURITY_VIOLATION) {
    //
    // Generate output string for boot failed caused by security violation
    //
    if (Option->Description != NULL) {
      BootErrorString = BdsLibGetStringById (STRING_TOKEN (STR_BOOT_FAILED_BY_SECURITY));
      ASSERT (BootErrorString != NULL);
      PrintStringBufSize = StrSize (BootErrorString) + StrSize (Option->Description) - sizeof (CHAR16);
      PrintString = AllocateZeroPool (PrintStringBufSize);
      ASSERT (PrintString != NULL);
      if (PrintString == NULL) {
        return;
      }
      StrCpyS (PrintString, PrintStringBufSize / sizeof(CHAR16), Option->Description);
      StrCatS (PrintString, PrintStringBufSize / sizeof(CHAR16), BootErrorString);
      FreePool (BootErrorString);
    } else {
      PrintString = BdsLibGetStringById (STRING_TOKEN (STR_BOOT_FILE_FAILED_BY_SECURITY));
      ASSERT (PrintString != NULL);
      if (PrintString == NULL) {
        return;
      }
    }
  } else {
    //
    // Generate common boot failed string for other reason.
    //
    if (Option->Description != NULL) {
      BootErrorString = BdsLibGetStringById (STRING_TOKEN (STR_COMMON_BOOT_FAILED));
      ASSERT (BootErrorString != NULL);
      PrintStringBufSize = StrSize (BootErrorString) + StrSize (Option->Description) - sizeof (CHAR16);
      PrintString = AllocateZeroPool (PrintStringBufSize);
      ASSERT (PrintString != NULL);
      if (PrintString == NULL) {
        return;
      }
      StrCpyS (PrintString, PrintStringBufSize / sizeof(CHAR16), Option->Description);
      StrCatS (PrintString, PrintStringBufSize / sizeof(CHAR16), BootErrorString);
    } else {
      PrintString = BdsLibGetStringById (STRING_TOKEN (STR_COMMON_BOOT_FILE_FAILED));
      ASSERT (PrintString != NULL);
      if (PrintString == NULL) {
        return;
      }
    }
  }

  DisableQuietBoot ();
  gST->ConOut->ClearScreen (gST->ConOut);
  H2ODialog->ConfirmDialog (
                    DlgOk,
                    FALSE,
                    0,
                    NULL,
                    &Key,
                    PrintString
                    );
  gST->ConOut->SetCursorPosition (gST->ConOut, 0, 0);
  FreePool (PrintString);

}

/**
  Default behavior for a boot attempt succeeds. We don't expect a boot option to
  return, so the UEFI 2.0 specification defines that you will default to an
  interactive mode and stop processing the BootOrder list in this case. This
  is also a default implementation and can be customized in
  gH2OBdsCpBootSuccessGuid checkpoint.

  @param  Option                  Pointer to Boot Option that succeeded to boot.

**/
VOID
EFIAPI
BdsLibBootSuccess (
  IN  H2O_BDS_LOAD_OPTION *Option
  )
{
  CHAR16                            *String;

  if (FeaturePcdGet (PcdH2OBdsCpBootSuccessSupported)) {
    H2O_BDS_CP_BOOT_SUCCESS_DATA   BdsBootSuccessData;

    BdsBootSuccessData.Size         = sizeof (H2O_BDS_CP_BOOT_SUCCESS_DATA);
    BdsBootSuccessData.Status       = H2O_CP_TASK_NORMAL;
    DEBUG ((DEBUG_INFO, "Checkpoint Trigger: %g\n", &gH2OBdsCpBootSuccessGuid));
    H2OCpTrigger (&gH2OBdsCpBootSuccessGuid, &BdsBootSuccessData);
    DEBUG ((DEBUG_INFO, "Checkpoint Result: %x\n", BdsBootSuccessData.Status));
    if (BdsBootSuccessData.Status == H2O_BDS_TASK_SKIP) {
      return;
    }
  }
  //
  // If Boot returned with EFI_SUCCESS and there is not in the boot device
  // select loop then we need to pop up a UI and wait for user input.
  //
  String = BdsLibGetStringById (STRING_TOKEN (STR_BOOT_SUCCEEDED));
  if (String != NULL) {
    BdsLibOutputStrings (gST->ConOut, String, Option->Description, L"\n\r", NULL);
    FreePool(String);
  }
}

/**
 This function will trigger DxeSmmReadyToLock related events and checkpoints

 The call flow is this function is below:
 1.Trigger gH2OBdsCpDxeSmmReadyToLockBeforeGuid checkpoint if PcdH2OBdsCpDxeSmmReadyToLockBeforeSupported is TRUE
 2.Install gEfiDxeSmmReadyToLockProtocolGuid protocol.
 3.Trigger gH2OBdsCpDxeSmmReadyToLockAfterGuid checkpoint if PcdH2OBdsCpDxeSmmReadyToLockAfterSupported is TRUE
**/
VOID
EFIAPI
BdsLibTriggerDxeSmmReadyToLockEvent (
  VOID
  )
{
  if (FeaturePcdGet (PcdH2OBdsCpDxeSmmReadyToLockBeforeSupported)) {
    TriggerCpDxeSmmReadyToLockBefore ();
  }
  gBS->InstallProtocolInterface (
       &gImageHandle,
       &gEfiDxeSmmReadyToLockProtocolGuid,
       EFI_NATIVE_INTERFACE,
       NULL
       );
  if (FeaturePcdGet (PcdH2OBdsCpDxeSmmReadyToLockAfterSupported)) {
    TriggerCpDxeSmmReadyToLockAfter ();
  }
}
