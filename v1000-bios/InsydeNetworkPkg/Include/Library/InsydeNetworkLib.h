/** @file
  NetworkLocker library include file

;******************************************************************************
;* Copyright (c) 2012-2019, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _NETWORK_LOCKER_LIB_H_
#define _NETWORK_LOCKER_LIB_H_

#include <Guid/NetworkEventGroup.h>
#include <Protocol/NetworkLocker.h>

#define SNP            NetworkStackSnpCallback
#define IP4            NetworkStackIp4Callback
#define IP6            NetworkStackIp6Callback
#define UDP4           NetworkStackUdp4Callback
#define UDP6           NetworkStackUdp6Callback
#define TCP4           NetworkStackTcp4Callback
#define TCP6           NetworkStackTcp6Callback
#define MTFTP4         NetworkStackMtftp4Callback
#define MTFTP6         NetworkStackMtftp6Callback
#define DHCP4          NetworkStackDhcp4Callback
#define DHCP6          NetworkStackDhcp6Callback
#define PXE4           NetworkStackPxe4Callback
#define PXE6           NetworkStackPxe6Callback
#define HTTP4          NetworkStackHttp4Callback
#define HTTP6          NetworkStackHttp6Callback
#define HTTPBOOT4      NetworkStackHttpBoot4Callback
#define HTTPBOOT6      NetworkStackHttpBoot6Callback

BOOLEAN
NetworkLockerCallback (
  EFI_HANDLE                           Controller,
  NETWORK_STACK_CALLBACK_IDENTIFIER    Identifier
  );

EFI_STATUS
PerformanceStartMeasure (
  EFI_HANDLE          Handle,
  IN CHAR8            *Token,
  IN CHAR8            *Host,
  IN UINT64           Ticker
  );

EFI_STATUS
PerformanceEndMeasure (
  EFI_HANDLE          Handle,
  IN CHAR8            *Token,
  IN CHAR8            *Host,
  IN UINT64           Ticker
  );

BOOLEAN
IsH2OFormbrowser (
  VOID
  );

EFI_STATUS
NetworkLibCreatePopUp (
  IN CHAR16           *MessageStr
  );

VOID
InsydeNetworkLibPxeStart (
  EFI_HANDLE              *Handle
  );
  
VOID
InsydeNetworkLibPxeEnd (
  EFI_HANDLE              Handle
  );
  
BOOLEAN
IsPxeRunning (
  );
  
/**
  Get DevicePath protocol from handle, generate the unique SHA-1 HEX string.

  @param[in]  DeviceHandle      The handle of device.
  @param[out] UniqueString      String generate by DevicePath, if DevicePath is not duplicated and Modified.
                                This string should be unique in POST time.

  @return EFI_INVAILD_PARAMETER        DeviceHandle or UniqueString is NULL value.
  @return EFI_INVAILD_PARAMETER        DevicePath protocol not exist in input handle.
  @return EFI_NOT_READY                gEfiDevicePathToTextProtocolGuid protocol is not ready.
  @return EFI_OUT_OF_RESOURCES         Memory allocate pool failed.
  @return others                       Status from gBS->HandleProtocol.
**/
EFI_STATUS
GetUniqueStringByDevicePath (
  EFI_HANDLE                           *DeviceHandle,
  CHAR16                               **UniqueString
  );

VOID
UninitializeHashProtocol (
  );

EFI_STATUS
RemoveUnnessaryConfigureVariable (
  IN   EFI_GUID                        *VariableGuid,
  IN   UINT32                          VariableAttribute
  );

#endif
