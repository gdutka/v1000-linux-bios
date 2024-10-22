/** @file
  Device Info Library Definition.
;******************************************************************************
;* Copyright (c) 2018, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _DEVICE_INFO_LIB_H_
#define _DEVICE_INFO_LIB_H_

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
  IN  UINTN                     TokenNumber,
  OUT EFI_DEVICE_PATH_PROTOCOL  **DevicePath,
  OUT UINT32                    *DeviceAttribCount
  );

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
  IN  UINTN                     TokenNumber,
  IN  UINT32                    DeviceAttribIndex,
  OUT CHAR8                     **DeviceAttribName,
  OUT CHAR8                     **DeviceAttribValue
  );

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
  IN  UINTN                     TokenNumber,
  IN  CONST CHAR8               *DeviceAttribName,
  OUT CHAR8                     **DeviceAttribValue
  );

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
  IN UINTN                      TokenNumber
  );

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
  IN UINTN                      TokenNumber
  );

#endif

