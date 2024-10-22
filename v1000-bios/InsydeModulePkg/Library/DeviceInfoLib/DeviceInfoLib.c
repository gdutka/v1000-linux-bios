/** @file
  This library class provides helper functions to access the Device Information.

;******************************************************************************
;* Copyright (c) 2018 - 2021, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/
#include <Uefi.h>
#include <Protocol/DevicePath.h>
#include <Protocol/DevicePathFromText.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/DeviceInfoLib.h>
#include <Library/DevicePathLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PcdLib.h>
#include <Library/UefiBootServicesTableLib.h>

#define IS_PCI_ROOT(DevNodePtr)        (((ACPI_HID_DEVICE_PATH *) DevNodePtr)->HID == EFI_PNP_ID (0x0a03))

typedef struct _H2O_DEVICE_INFO_PCD_DATA {
  UINTN                                PcdTokenNum;
  UINT32                               AttribCount;
  CHAR8                                **AttribAsciiStrPtrArray;
  CHAR8                                **ValueAsciiStrPtrArray;
  EFI_DEVICE_PATH_PROTOCOL             *DevPath;
} H2O_DEVICE_INFO_PCD_DATA;

H2O_DEVICE_INFO_PCD_DATA               *mDevInfoList               = NULL;
EFI_GUID                               mInternalDeviceInfoListGuid = {0x13b319ad, 0xa7d6, 0x4ab1, 0xb9, 0x74, 0xa0, 0x2a, 0x53, 0x88, 0xa8, 0x7d};

/**
 Get last device node, which is not end type, from device path.

 @param[in] DevicePath          Pointer to device p

 @return Pointer to the last device node or NULL if input parameter DevPath is NULL or only contains end type device node.
**/
STATIC
EFI_DEVICE_PATH_PROTOCOL *
GetLastDevNode (
  IN EFI_DEVICE_PATH_PROTOCOL          *DevPath
  )
{
  EFI_DEVICE_PATH_PROTOCOL             *DevNode;
  EFI_DEVICE_PATH_PROTOCOL             *LastDevNode;

  if (DevPath == NULL) {
    return NULL;
  }

  LastDevNode = NULL;
  for (DevNode = DevPath; !IsDevicePathEnd (DevNode); DevNode = NextDevicePathNode (DevNode)) {
    LastDevNode = DevNode;
  }

  return LastDevNode;
}

/**
 Enumerate all device info PCDs in gH2ODeviceInfoTokenSpaceGuid.

 @param[out] DevInfoList        Pointer to a returned pointer to a device info data list

 @retval EFI_SUCCESS            Enumerate all device info PCDs successfully.
 @retval EFI_OUT_OF_RESOURCES   Allocate memory failed.
 @retval Other                  Fail to locate EFI_DEVICE_PATH_FROM_TEXT_PROTOCOL.
**/
STATIC
EFI_STATUS
EnumerateAllDevInfoPcd (
  OUT H2O_DEVICE_INFO_PCD_DATA         **DevInfoList
  )
{
  EFI_STATUS                           Status;
  EFI_DEVICE_PATH_FROM_TEXT_PROTOCOL   *DevicePathFromText;
  UINTN                                TokenNumber;
  UINTN                                PcdDataSize;
  CHAR8                                *PcdData;
  UINT32                               PcdCount;
  CHAR8                                *DevPathAsciiStr;
  UINTN                                DevPathAsciiStrSize;
  CHAR16                               *DevPathStr;
  UINT32                               AttribCount;
  UINTN                                Offset;
  H2O_DEVICE_INFO_PCD_DATA             *DevInfo;
  CHAR8                                *AttribPtr;
  CHAR8                                *ValuePtr;

  Status = gBS->LocateProtocol (&gEfiDevicePathFromTextProtocolGuid, NULL, (VOID **) &DevicePathFromText);
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  for (TokenNumber  = LibPcdGetNextToken (&gH2ODeviceInfoTokenSpaceGuid, 0), PcdCount = 0;
       TokenNumber != 0;
       TokenNumber  = LibPcdGetNextToken (&gH2ODeviceInfoTokenSpaceGuid, TokenNumber)) {
    if (LibPcdGetExSize (&gH2ODeviceInfoTokenSpaceGuid, TokenNumber) <= sizeof ("")) {
      continue;
    }
    PcdCount++;
  }
  if (PcdCount == 0) {
    *DevInfoList = NULL;
    return EFI_SUCCESS;
  }

  *DevInfoList = AllocateZeroPool ((PcdCount + 1) * sizeof (H2O_DEVICE_INFO_PCD_DATA));
  if (*DevInfoList == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  for (TokenNumber  = LibPcdGetNextToken (&gH2ODeviceInfoTokenSpaceGuid, 0), PcdCount = 0;
       TokenNumber != 0;
       TokenNumber  = LibPcdGetNextToken (&gH2ODeviceInfoTokenSpaceGuid, TokenNumber)) {
    PcdDataSize = LibPcdGetExSize (&gH2ODeviceInfoTokenSpaceGuid, TokenNumber);
    PcdData     = LibPcdGetExPtr (&gH2ODeviceInfoTokenSpaceGuid, TokenNumber);
    if (PcdData == NULL || PcdDataSize <= sizeof ("") || PcdData[PcdDataSize - 1] != CHAR_NULL) {
      continue;
    }

    PcdData = AllocateCopyPool (PcdDataSize, PcdData);
    if (PcdData == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }

    //
    // The PCD data consists of one or more null-terminated ASCII strings.
    // The first ASCII string specify the device path of the on-board device.
    // The second and following strings specify an attribute for that device in the format attribute=value.
    //
    DevInfo              = &((*DevInfoList)[PcdCount++]);
    DevInfo->PcdTokenNum = TokenNumber;

    DevPathAsciiStr     = PcdData;
    DevPathAsciiStrSize = AsciiStrSize (DevPathAsciiStr);
    DevPathStr = AllocatePool (DevPathAsciiStrSize * sizeof (CHAR16));
    if (DevPathStr != NULL) {
      AsciiStrToUnicodeStrS (DevPathAsciiStr, DevPathStr, DevPathAsciiStrSize);
      DevInfo->DevPath = DevicePathFromText->ConvertTextToDevicePath (DevPathStr);
      FreePool (DevPathStr);
    }

    AttribCount = 0;
    Offset      = DevPathAsciiStrSize;
    while (Offset < PcdDataSize) {
      Offset += AsciiStrSize (PcdData + Offset);
      AttribCount++;
    }
    if (AttribCount == 0) {
      continue;
    }

    DevInfo->AttribAsciiStrPtrArray = AllocateZeroPool (AttribCount * sizeof (CHAR8 *));
    DevInfo->ValueAsciiStrPtrArray  = AllocateZeroPool (AttribCount * sizeof (CHAR8 *));
    if (DevInfo->AttribAsciiStrPtrArray == NULL ||
        DevInfo->ValueAsciiStrPtrArray  == NULL) {
      continue;
    }

    AttribCount = 0;
    Offset      = DevPathAsciiStrSize;
    while (Offset < PcdDataSize) {
      AttribPtr = PcdData + Offset;
      Offset += AsciiStrSize (PcdData + Offset);

      ValuePtr = AsciiStrStr (AttribPtr, "=");
      ASSERT (ValuePtr != NULL);
      if (ValuePtr == NULL) {
        continue;
      }
      *ValuePtr = CHAR_NULL;
      ValuePtr++;

      DevInfo->AttribAsciiStrPtrArray[AttribCount] = AttribPtr;
      DevInfo->ValueAsciiStrPtrArray[AttribCount]  = ValuePtr;
      AttribCount++;
    }
    DevInfo->AttribCount = AttribCount;
  }

  return EFI_SUCCESS;
}

/**
 Get device info PCD data.

 @param[in] PcdTokenNum         Unsigned integer that specifies the PCD token associated with a device's information.

 @return Pointer to the device info PCD data or NULL if not found.
**/
STATIC
H2O_DEVICE_INFO_PCD_DATA *
GetDevInfoPcdData (
  IN UINTN                             PcdTokenNum
  )
{
  EFI_STATUS                           Status;
  EFI_HANDLE                           Handle;
  UINTN                                Index;

  if (mDevInfoList == NULL) {
    //
    // Device info list pointer is saved in an internal protocol. All of library instances can access the same database.
    //
    Status = gBS->LocateProtocol (&mInternalDeviceInfoListGuid, NULL, (VOID **) &mDevInfoList);
    if (EFI_ERROR (Status)) {
      EnumerateAllDevInfoPcd (&mDevInfoList);

      Handle = NULL;
      gBS->InstallProtocolInterface (&Handle, &mInternalDeviceInfoListGuid, EFI_NATIVE_INTERFACE, (VOID *) mDevInfoList);
    }

    if (mDevInfoList == NULL) {
      return NULL;
    }
  }

  for (Index = 0; mDevInfoList[Index].PcdTokenNum != 0; Index++) {
    if (mDevInfoList[Index].PcdTokenNum == PcdTokenNum) {
      return &mDevInfoList[Index];
    }
  }

  return NULL;
}

/**
 This function returns basic information about a device.

 @param[in]  TokenNumber        Unsigned integer that specifies the PCD token associated with a device's information.
 @param[out] DevicePath         Pointer to a returned pointer to a device path that indicates the device's device path.
                                The format of the returned device path is consistent with the format specified in the
                                UEFI specification. The device path must not be freed by the caller.
 @param[out] DeviceAttribCount  Pointer to a returned unsigned integer that indicates the number of device attributes
                                associated with the device indicated by DevicePath.

 @retval EFI_SUCCESS            Function returned successfully.
 @retval EFI_INVALID_PARAMETER  DevicePath was NULL or DeviceAttribCount was NULL.
 @retval EFI_VOLUME_CORRUPTED   The format of the device path in the PCD was invalid.
 @retval EFI_NOT_FOUND          There is no device information associated with the specified TokenNumber.
**/
EFI_STATUS
GetDeviceInfo (
  IN  UINTN                            TokenNumber,
  OUT EFI_DEVICE_PATH_PROTOCOL         **DevicePath,
  OUT UINT32                           *DeviceAttribCount
  )
{
  H2O_DEVICE_INFO_PCD_DATA             *DevInfoPcdData;

  if (DevicePath == NULL || DeviceAttribCount == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  DevInfoPcdData = GetDevInfoPcdData (TokenNumber);
  if (DevInfoPcdData == NULL) {
    return EFI_NOT_FOUND;
  }

  if (DevInfoPcdData->DevPath == NULL) {
    return EFI_VOLUME_CORRUPTED;
  }

  *DevicePath        = DevInfoPcdData->DevPath;
  *DeviceAttribCount = DevInfoPcdData->AttribCount;

  return EFI_SUCCESS;
}

/**
 This function returns a name and value of attribute associated with the device associated with the PCD token number.

 @param[in]  TokenNumber        Unsigned integer that specifies the PCD token associated with a device's information.
 @param[in]  DeviceAttribIndex  Unsigned integer that specifies the index of the device attribute. This value must be
                                less than the value returned by the parameter DeviceAttribCount from GetDeviceInfo.
                                Indices are numbered starting with 0 and continue until the last attribute. There are no gaps.
 @param[out] DeviceAttribName   Pointer to a returned pointer to a null-terminated ASCII string that indicates the name
                                of the device attribute associated with the device.
                                The string value must not be freed by the caller.
 @param[out] DeviceAttribValue  Pointer to a returned pointer to a null-terminated ASCII string that indicates the value
                                of the device attribute associated with the device.
                                The string value must not be freed by the caller.

 @retval EFI_SUCCESS            Function returned successfully.
 @retval EFI_INVALID_PARAMETER  DeviceAttribName was NULL or DeviceAttribValue was NULL.
 @retval EFI_NOT_FOUND          There is no device information associated with the specified TokenNumber.
**/
EFI_STATUS
GetDeviceAttrib (
  IN  UINTN                            TokenNumber,
  IN  UINT32                           DeviceAttribIndex,
  OUT CHAR8                            **DeviceAttribName,
  OUT CHAR8                            **DeviceAttribValue
  )
{
  H2O_DEVICE_INFO_PCD_DATA             *DevInfoPcdData;

  if (DeviceAttribName == NULL || DeviceAttribValue == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  DevInfoPcdData = GetDevInfoPcdData (TokenNumber);
  if (DevInfoPcdData == NULL) {
    return EFI_NOT_FOUND;
  }

  if (DevInfoPcdData->AttribCount <= DeviceAttribIndex ||
      DevInfoPcdData->AttribAsciiStrPtrArray == NULL   ||
      DevInfoPcdData->ValueAsciiStrPtrArray  == NULL) {
    return EFI_NOT_FOUND;
  }

  *DeviceAttribName  = DevInfoPcdData->AttribAsciiStrPtrArray[DeviceAttribIndex];
  *DeviceAttribValue = DevInfoPcdData->ValueAsciiStrPtrArray[DeviceAttribIndex];
  return EFI_SUCCESS;
}

/**
 This function returns the device attribute value associated with the device associated with the PCD token number
 and the device attribute name.

 @param[in]  TokenNumber        Unsigned integer that specifies the PCD token associated with a device's information.
 @param[in]  DeviceAttribName   Pointer to a null-terminated ASCII string that specifies the name of the device
                                attribute associated with the device.
 @param[out] DeviceAttribValue  Pointer to a returned pointer to a null-terminated ASCII string that indicates the value
                                of the device attribute associated with the device.
                                The string value must not be freed by the caller.

 @retval EFI_SUCCESS            Function returned successfully.
 @retval EFI_INVALID_PARAMETER  DeviceAttribName was NULL or DeviceAttribValue was NULL.
 @retval EFI_NOT_FOUND          There is no device information associated with the specified TokenNumber.
**/
EFI_STATUS
GetDeviceInfoAttribByName (
  IN  UINTN                            TokenNumber,
  IN  CONST CHAR8                      *DeviceAttribName,
  OUT CHAR8                            **DeviceAttribValue
  )
{
  H2O_DEVICE_INFO_PCD_DATA             *DevInfoPcdData;
  UINT32                               Index;

  if (DeviceAttribName == NULL || DeviceAttribValue == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  DevInfoPcdData = GetDevInfoPcdData (TokenNumber);
  if (DevInfoPcdData == NULL) {
    return EFI_NOT_FOUND;
  }

  if (DevInfoPcdData->AttribAsciiStrPtrArray == NULL ||
      DevInfoPcdData->ValueAsciiStrPtrArray  == NULL) {
    return EFI_NOT_FOUND;
  }

  for (Index = 0; Index < DevInfoPcdData->AttribCount; Index++) {
    if (AsciiStrCmp (DevInfoPcdData->AttribAsciiStrPtrArray[Index], DeviceAttribName) == 0) {
      *DeviceAttribValue = DevInfoPcdData->ValueAsciiStrPtrArray[Index];
      return EFI_SUCCESS;
    }
  }

  return EFI_NOT_FOUND;
}

/**
 This function returns whether the specified device is a PCI device.
 This determination is made by checking the final device node within the device's path to see if it is a PCI root or PCI
 device. If it is, the function will return TRUE. If it is not, or if the TokenNumber does not specify a valid device,
 it will return FALSE.

 @param[in] TokenNumber         Unsigned integer that specifies the PCD token associated with a device's information.

 @return TRUE, if the specified device's path ends in a PCI or PCI root device node. Otherwise, FALSE.
**/
BOOLEAN
IsDevicePathPci (
  IN UINTN                             TokenNumber
  )
{
  H2O_DEVICE_INFO_PCD_DATA             *DevInfoPcdData;
  EFI_DEVICE_PATH_PROTOCOL             *LastDevNode;

  DevInfoPcdData = GetDevInfoPcdData (TokenNumber);
  if (DevInfoPcdData == NULL || DevInfoPcdData->DevPath == NULL) {
    return FALSE;
  }

  LastDevNode = GetLastDevNode (DevInfoPcdData->DevPath);
  if (LastDevNode == NULL) {
    return FALSE;
  }

  if ((DevicePathType (LastDevNode) == ACPI_DEVICE_PATH && DevicePathSubType (LastDevNode) == ACPI_DP && IS_PCI_ROOT(LastDevNode)) ||
      (DevicePathType (LastDevNode) == HARDWARE_DEVICE_PATH && DevicePathSubType (LastDevNode) == HW_PCI_DP)) {
    return TRUE;
  }

  return FALSE;
}

/**
 This function returns whether the specified device is a USB device.
 This determination is made by checking the final device node within the device's path to see if it is a USB device node.
 If it is, the function will return TRUE. If it is not, or if the TokenNumber does not specify a valid device, it will
 return FALSE.

 @param[in] TokenNumber         Unsigned integer that specifies the PCD token associated with a device's information.

 @return TRUE, if the specified device's path ends in a USB device node. Otherwise, FALSE.
**/
BOOLEAN
IsDevicePathUsb (
  IN UINTN                             TokenNumber
  )
{
  H2O_DEVICE_INFO_PCD_DATA             *DevInfoPcdData;
  EFI_DEVICE_PATH_PROTOCOL             *LastDevNode;

  DevInfoPcdData = GetDevInfoPcdData (TokenNumber);
  if (DevInfoPcdData == NULL || DevInfoPcdData->DevPath == NULL) {
    return FALSE;
  }

  LastDevNode = GetLastDevNode (DevInfoPcdData->DevPath);
  if (LastDevNode == NULL) {
    return FALSE;
  }

  if (DevicePathType (LastDevNode) == MESSAGING_DEVICE_PATH && DevicePathSubType (LastDevNode) == MSG_USB_DP) {
    return TRUE;
  }

  return FALSE;
}

