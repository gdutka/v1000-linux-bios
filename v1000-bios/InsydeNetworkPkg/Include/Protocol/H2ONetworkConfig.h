/** @file
  H2O network configuration protocol.

;******************************************************************************
;* Copyright (c) 2019 - 2020, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _H2O_NETWORK_CONFIG_PROTOCOL_H
#define _H2O_NETWORK_CONFIG_PROTOCOL_H

#define H2O_NETWORK_CONFIG_PROTOCOL_GUID \
{ 0x22b057f9, 0x7ea3, 0x42e7, 0x96, 0x63, 0x25, 0x9a, 0x30, 0x19, 0xa2, 0xf9 }

typedef struct _H2O_NETWORK_CONFIG_PROTOCOL   H2O_NETWORK_CONFIG_PROTOCOL;

/**
  This function returns whether the network stack is currently locked enabled(TRUE) for
  the specified device or disabled (FALSE) for the specified device(s).

  This function will scan and report the status of disabled devices.
  If user wants to know the status of a specific on the specific device, use DeviceHandle to specify it.

  For instance -
  Device Status:
    Device A -	Enabled
    Device B -	Disabled
    Device C -	Enabled
  
  Reported Status:
    DeviceHandle A is specified -  TRUE is returned.
    DeviceHandle B is specified -  FALSE is returned.
    DeviceHandle C is specified -  TRUE is returned.
    If not specified (NULL), then this scenario will report TRUE (because Device A and  Device C are enabled).

  @param[in]  DeviceHandle    Handle that specifies the device to enable or disable.
                              If NULL, then it will report whether any device is enabled.

  @retval TRUE                Device(s) is enabled.
  @retval FALSE               Device(s) is disabled.

**/
typedef
BOOLEAN
(*H2O_NETWORK_CONFIG_IS_ENABLED)(
  IN    EFI_HANDLE                     DeviceHandle OPTIONAL
  );

/**
  This function will enable the device and bind the network protocols to the device.
  If user wants to specify specific devices, use DeviceHandle.

  @param[in]  DeviceHandle    Handle that specifies the device to enable or disable.
                              If NULL, then it will enable all devices.

  @retval EFI_SUCCESS           This function completed successfully.
  @retval EFI_DEVICE_ERROR      One or more network devices reported an error when enabled.
  @retval EFI_ALREADY_STARTED   The network stack was already enabled and cannot be disabled.
  @retval EFI_UNSUPPORTED       The network stack does not support configuration.

**/
typedef
EFI_STATUS
(*H2O_NETWORK_CONFIG_ENABLE) (
  IN    EFI_HANDLE                     DeviceHandle OPTIONAL
  );

struct _H2O_NETWORK_CONFIG_PROTOCOL {
  UINT32                               Size;
  H2O_NETWORK_CONFIG_IS_ENABLED        IsEnabled;
  H2O_NETWORK_CONFIG_ENABLE            Enable;
};

extern EFI_GUID gH2ONetworkConfigProtocolGuid;

#endif //_H2O_NETWORK_CONFIG_PROTOCOL_H
