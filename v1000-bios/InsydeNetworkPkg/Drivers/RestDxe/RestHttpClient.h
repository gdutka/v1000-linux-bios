/** @file
  REST protocol implementation.

;******************************************************************************
;* Copyright (c) 2012 - 2019, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _REST_HTTP_CLIENT_H_
#define _REST_HTTP_CLIENT_H_

#include "RestDriver.h"

#define URL_MAX_LENGTH         1024

#define HTTP_DELAY_TIME        (100000 * 10000U) // 100 Seconds

#define  HTTP_TIMEOUT_SECOND        1000
#define  HTTP_RESPONSE_TIME_OUT     (120 * HTTP_TIMEOUT_SECOND)

typedef struct _REST_HTTP_TOKEN {
  //
  // HttpMessage Header and Body from Request
  //
  EFI_HTTP_REQUEST_DATA        *HttpRequest;
  EFI_HTTP_MESSAGE             Request;
  EFI_HTTP_MESSAGE             Response;

  //
  // Instance variables
  //
  CHAR8                        *HostName;
  EFI_IPv4_ADDRESS             RemoteAddr;
  EFI_IPv6_ADDRESS             RemoteAddr6;
  EFI_HANDLE                   HttpHandle;
  EFI_HTTP_PROTOCOL            *Http;
  EFI_HANDLE                   Dns4Handle;
  EFI_DNS4_PROTOCOL            *Dns4;
  EFI_IP4_CONFIG2_PROTOCOL     *Ip4Config2;

  UINTN                        DnsServerCounter;
  EFI_IPv4_ADDRESS             *DnsServerList;

  EFI_HTTP_CONFIG_DATA         HttpConfigData;
  EFI_HTTPv4_ACCESS_POINT      Http4AccessPoint;

  EFI_EVENT                    RequestEvent;
  BOOLEAN                      IsRxDone;

  EFI_EVENT                    ResponseEvent;
  BOOLEAN                      IsTxDone;

  EFI_EVENT                    TimeoutEvent;

  EFI_HTTP_STATUS_CODE         ReturnHttpStatusCode;
  UINTN                        ContentLength;
  BOOLEAN                      IdentityMode;
  UINT8                        *ChunkDataBuffer;
  UINTN                        ChunkDataLength;

  BOOLEAN                      IsIpv6;

  EFI_IP6_CONFIG_PROTOCOL     *Ip6Config;
} REST_HTTP_TOKEN;

EFI_STATUS
RestCreateHttpClientToken (
  IN  REST_BINDING_INSTANCE    *Instance,
  OUT REST_HTTP_TOKEN          **RestToken
  );

EFI_STATUS
RestHttpClientParsingURI (
  IN OUT REST_HTTP_TOKEN       *RestHttpToken
  );

EFI_STATUS
RestDestroyHttpClientToken (
  REST_BINDING_INSTANCE        *Instance,
  REST_HTTP_TOKEN              *HttpToken
  );

EFI_STATUS
RestHttpClientConfig (
  IN REST_HTTP_TOKEN           *HttpToken
  );

EFI_STATUS
RestHttpGetResponse (
  IN REST_HTTP_TOKEN           *HttpToken
  );

#endif
