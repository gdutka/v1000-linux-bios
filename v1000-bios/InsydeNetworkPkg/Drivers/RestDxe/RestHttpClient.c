/** @file
  REST protocol implementation.

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

#include "RestDriver.h"

EFI_HTTP_HEADER mCommonHeader[] = {
  {HTTP_HEADER_HOST,       "localhost"},
  {HTTP_HEADER_ACCEPT,     "*/*"},
  {HTTP_HEADER_USER_AGENT, "UefiRestProtocol/1.0"}
  };

EFI_STATUS
RestHttpInput (
  IN REST_HTTP_TOKEN           *HttpToken,
  IN BOOLEAN                   HeaderOnly
  );

EFI_STATUS
RestHttpOutput (
  IN REST_HTTP_TOKEN           *HttpToken
  );

/**
  Check URL validation

  TRUE  : Valid
  FALSE : Invalid
**/
BOOLEAN
IsURLValid (
  UINT16                       *URL
  )
{
  UINTN                        Index;

  Index = 0;
  while (URL[Index] != 0) {
    /// Ascii charectors check, a-z, A-Z
    if ((URL[Index] < 0x23) && (URL[Index] > 0x7E)) {
      return FALSE;
    }
    switch (URL[Index]) {
    case '<':
    case '>':
    case '\\':
    case '^':
    case '`':
    case '{':
    case '}':
      return FALSE;
    }
    Index ++;
  }
  return TRUE;
}

//
// Update HOST name from HTTP request message.
//
EFI_STATUS
RestUpdateHttpHost (
  OUT EFI_HTTP_MESSAGE          *DestHttpMessage,
  IN  CHAR8                     *HostName
  )
{
  EFI_STATUS                    Status;
  UINTN                         Index;

  Status = EFI_NOT_FOUND;
  for (Index = 0; Index < DestHttpMessage->HeaderCount; Index++) {
    if (AsciiStrCmp (DestHttpMessage->Headers[Index].FieldName, HTTP_HEADER_HOST) != 0) {
      continue;
    }
    Status = EFI_SUCCESS;
    DestHttpMessage->Headers[Index].FieldValue = HostName;
    break;
  }
  return Status;
}

//
// This function will merge command HTTP headers and append http headers.
//
EFI_STATUS
RestMegerHttpHeaders (
  IN  EFI_HTTP_MESSAGE          *AppendHeader,
  OUT EFI_HTTP_MESSAGE          *DestHttpMessage
  )
{
  UINTN                         AppendHeaderIndex;
  UINTN                         AppendHeaderCounter;
  UINTN                         CommonHeaderIndex;
  UINTN                         CommonHeaderCounter;
  UINTN                         RealHeaderIndex;
  UINTN                         RealHeaderCounter;
  EFI_HTTP_HEADER               *AppendHttpHeaders;
  EFI_HTTP_HEADER               *HttpHeaders;
  CHAR8                         *FieldName;

  //
  // Calculate number of common header and copy common headers.
  //
  CommonHeaderCounter = sizeof (mCommonHeader) / sizeof (EFI_HTTP_HEADER);
  AppendHeaderCounter = AppendHeader->HeaderCount;
  AppendHttpHeaders   = AppendHeader->Headers;
  RealHeaderCounter   = CommonHeaderCounter + AppendHeaderCounter;
  HttpHeaders = AllocateZeroPool (sizeof (EFI_HTTP_HEADER) * RealHeaderCounter);
  if (HttpHeaders == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // Append headers into HttpHeaders
  // RealHeaderIndex push to end of CommonHeaders, to append the header not exist in common headers.
  //
  RealHeaderIndex = CommonHeaderCounter;
  CopyMem (HttpHeaders, mCommonHeader, CommonHeaderCounter * sizeof (EFI_HTTP_HEADER));
  for (AppendHeaderIndex = 0; AppendHeaderIndex < AppendHeaderCounter; AppendHeaderIndex++) {
    FieldName = AppendHttpHeaders[AppendHeaderIndex].FieldName;
    for (CommonHeaderIndex = 0; CommonHeaderIndex < CommonHeaderCounter; CommonHeaderIndex++) {
      if (AsciiStrCmp (HttpHeaders[CommonHeaderIndex].FieldName, FieldName) != 0) {
        continue;
      }
      HttpHeaders[CommonHeaderIndex].FieldValue = AppendHttpHeaders[AppendHeaderIndex].FieldValue;
      break;
    }
    if (CommonHeaderIndex < CommonHeaderCounter) {
      //
      // Value overrided by AppendHttpHeaders.
      //
      continue;
    }
    //
    // Insert append header to RealHeaders.
    //
    HttpHeaders[RealHeaderIndex].FieldName  = AppendHttpHeaders[AppendHeaderIndex].FieldName;
    HttpHeaders[RealHeaderIndex].FieldValue = AppendHttpHeaders[AppendHeaderIndex].FieldValue;
    RealHeaderIndex++;
  }

  DestHttpMessage->HeaderCount = RealHeaderIndex;
  DestHttpMessage->Headers     = HttpHeaders;
  return EFI_SUCCESS;
}

//
// Callback to parsing received HTTP Package
//
EFI_STATUS
RestHttpParserCallback (
  IN HTTP_BODY_PARSE_EVENT      EventType,
  IN CHAR8                      *Data,
  IN UINTN                      Length,
  IN VOID                       *Context
  )
{

  //
  // Chunk transfer
  // This function must be review and debug.
  //
  REST_HTTP_TOKEN               *HttpToken;
  UINT8                         *BufferPtr;
  UINTN                         BufferOffser;

  HttpToken = Context;
  BufferOffser = HttpToken->ChunkDataLength;
  if (EventType == BodyParseEventOnData) {
    if (HttpToken->ChunkDataBuffer == NULL) {
      BufferPtr = AllocateZeroPool (Length);
    } else {
      BufferPtr = ReallocatePool (HttpToken->ChunkDataLength, HttpToken->ChunkDataLength + Length, HttpToken->ChunkDataBuffer);
    }
    if (BufferPtr == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
    CopyMem (BufferPtr + BufferOffser, Data, Length);
    HttpToken->ChunkDataBuffer = BufferPtr;
    HttpToken->ChunkDataLength += Length;
  }
  return EFI_SUCCESS;
}

//
// For DEBUG
//
EFI_STATUS
RestHttpStatusDump (
  EFI_HTTP_MESSAGE             *HttpMessage
  )
{
  UINTN                        Index;

  if (!DEBUG_MODE) {
    return EFI_NOT_READY;
  }

  AsciiPrint ("HTTP return Status : ", HttpMessage->Data.Response->StatusCode);
  for (Index = 0; Index < HttpMessage->HeaderCount; Index++) {
    AsciiPrint ("%a : ", HttpMessage->Headers[Index].FieldName);
    AsciiPrint ("%a\n", HttpMessage->Headers[Index].FieldValue);
  }
  return EFI_SUCCESS;
}

EFI_STATUS
RestHttpHeaderDump (
  EFI_HTTP_MESSAGE             *HttpMessage
  )
{
  UINTN                        Index;

  if (!DEBUG_MODE) {
    return EFI_NOT_READY;
  }

  AsciiPrint ("HTTP header\n");
  for (Index = 0; Index < HttpMessage->HeaderCount; Index++) {
    AsciiPrint ("%a : ", HttpMessage->Headers[Index].FieldName);
    AsciiPrint ("%a\n", HttpMessage->Headers[Index].FieldValue);
  }
  return EFI_SUCCESS;
}

/**
  Notify the callback function when an event is triggered.

  @param[in]  Event           The triggered event.
  @param[in]  Context         The opaque parameter to the function.

**/
VOID
RestHttpClientCommonNotify (
  IN EFI_EVENT           Event,
  IN VOID                *Context
  )
{
  *((BOOLEAN *) Context) = TRUE;
}

EFI_STATUS
RestHttpGetDnsFromIp4Config2Data (
  REST_HTTP_TOKEN              *HttpToken
  )
{
  EFI_STATUS                   Status;
  UINTN                        DnsServerCnt;
  EFI_IPv4_ADDRESS             *DnsServers;
  EFI_IP4_CONFIG2_PROTOCOL     *Ip4Config2;

  Ip4Config2   = HttpToken->Ip4Config2;
  DnsServers   = NULL;
  DnsServerCnt = 0;

  Status = Ip4Config2->GetData (
                         Ip4Config2,
                         Ip4Config2DataTypeDnsServer,
                         &DnsServerCnt,
                         NULL
                         );
  if (Status == EFI_BUFFER_TOO_SMALL) {
    DnsServers = AllocateZeroPool (DnsServerCnt);
    if (DnsServers == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
    Status = Ip4Config2->GetData (
                           Ip4Config2,
                           Ip4Config2DataTypeDnsServer,
                           &DnsServerCnt,
                           DnsServers
                           );
  }
  if (EFI_ERROR (Status)) {
    FreePool (DnsServers);
    return Status;
  }

  HttpToken->DnsServerCounter = DnsServerCnt / sizeof (EFI_IPv4_ADDRESS);
  HttpToken->DnsServerList    = DnsServers;
  return EFI_SUCCESS;
}

EFI_STATUS
RestHttpDns4Configure (
  REST_HTTP_TOKEN              *HttpToken
  )
{
  EFI_STATUS                   Status;
  EFI_DNS4_PROTOCOL            *Dns4;
  EFI_DNS4_CONFIG_DATA         Dns4ConfigData;

  ZeroMem (&Dns4ConfigData, sizeof (EFI_DNS4_CONFIG_DATA));
  Dns4ConfigData.DnsServerListCount = HttpToken->DnsServerCounter;
  Dns4ConfigData.DnsServerList      = HttpToken->DnsServerList;
  Dns4ConfigData.EnableDnsCache     = TRUE;
  /// this setting should be review.
  Dns4ConfigData.UseDefaultSetting  = TRUE;
  Dns4ConfigData.Protocol           = EFI_IP_PROTO_UDP;

  Dns4 = HttpToken->Dns4;
  if (Dns4 == NULL) {
      AsciiPrint ("DNS protocol is not exist!\n");
      return EFI_NOT_READY;
  }
  Status = Dns4->Configure (Dns4, &Dns4ConfigData);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  return EFI_SUCCESS;
}

EFI_STATUS
RestHttpDns4TranslateAddress (
  REST_HTTP_TOKEN              *HttpToken
  )
{
  EFI_STATUS                   Status;
  EFI_DNS4_PROTOCOL            *Dns4;
  EFI_DNS4_COMPLETION_TOKEN    Dns4Token;
  BOOLEAN                      Dns4IsDone;
  UINT16                       *HostName;
  UINTN                        HostNameLength;

  HostNameLength = AsciiStrLen (HttpToken->HostName);
  HostName       = AllocateZeroPool (HostNameLength * sizeof (UINT16));
  if (HostName == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  ZeroMem (&Dns4Token, sizeof (EFI_DNS4_COMPLETION_TOKEN));
  Status = gBS->CreateEvent (
                  EVT_NOTIFY_SIGNAL,
                  TPL_NOTIFY,
                  RestHttpClientCommonNotify,
                  &Dns4IsDone,
                  &Dns4Token.Event
                  );
  if (EFI_ERROR (Status)) {
    goto EXIT;
  }

  //
  // Start asynchronous name resolution.
  //
  Dns4             = HttpToken->Dns4;
  Dns4Token.Status = EFI_NOT_READY;
  Dns4IsDone       = FALSE;
  AsciiStrToUnicodeStrS (HttpToken->HostName, HostName, HostNameLength * sizeof (UINT16));
  Status = Dns4->HostNameToIp (Dns4, HostName, &Dns4Token);
  if (EFI_ERROR (Status)) {
    goto EXIT;
  }

  while (!Dns4IsDone) {
    Dns4->Poll (Dns4);
  }

  //
  // This section should be review for parsing DNS if it has IPv4 and IPv6 both.
  // Name resolution is done, check result.
  //
  Status = Dns4Token.Status;
  if (!EFI_ERROR (Status)) {
    if (Dns4Token.RspData.H2AData == NULL) {
      Status = EFI_DEVICE_ERROR;
      goto EXIT;
    }
    if (Dns4Token.RspData.H2AData->IpCount == 0 || Dns4Token.RspData.H2AData->IpList == NULL) {
      Status = EFI_DEVICE_ERROR;
      goto EXIT;
    }
    //
    // Just for a test with parsing URI.DomanName, it will not using IPv4 address yet.
    //
    //IP4_COPY_ADDRESS (IpAddress, Dns4Token.RspData.H2AData->IpList);
    Status = EFI_SUCCESS;
  }

EXIT:
  if (EFI_ERROR (Status)) {
    HttpToken->ReturnHttpStatusCode = HTTP_STATUS_503_SERVICE_UNAVAILABLE;
  }
  if (HostName != NULL) {
    FreePool (HostName);
  }
  gBS->CloseEvent (Dns4Token.Event);
  return Status;
}

///
/// This function will refer PCD to choice DNS capability in IPv4 or IPv6.
/// If there is no PCD setting in PCD database, it will be false to use IPv4.
///
VOID
RestResolveDnsPolicy (
  OUT  BOOLEAN                 *IsIPv6
  )
{
  VOID                         *IsIPv6Ptr;
  UINT8                        Policy;

  /// Check PCD exist in PCD database.
  *IsIPv6 = FALSE;
  IsIPv6Ptr = PcdGetExPtr (&gInsydeTokenSpaceGuid, PcdH2ORestIPv6DnsPolicy);
  if (IsIPv6Ptr == NULL) {
    return;
  }

  Policy = PcdGetEx8 (&gInsydeTokenSpaceGuid, PcdH2ORestIPv6DnsPolicy);
  if (Policy != 0) {
    *IsIPv6 = TRUE;
  }
  return;
}

EFI_STATUS
RestCreateHttpClientToken (
  IN  REST_BINDING_INSTANCE    *Instance,
  OUT REST_HTTP_TOKEN          **RestHttpToken
  )
{
  EFI_STATUS                   Status;
  REST_HTTP_TOKEN              *HttpToken;

  HttpToken = (REST_HTTP_TOKEN *) AllocateZeroPool (sizeof (REST_HTTP_TOKEN));
  if (HttpToken == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  HttpToken->HttpHandle = Instance->HttpHandle;
  Status = gBS->HandleProtocol (
                  HttpToken->HttpHandle,
                  &gEfiHttpProtocolGuid,
                  (VOID **) &HttpToken->Http
                  );
  if (EFI_ERROR(Status)) {
    return Status;
  }

  Status = NetLibCreateServiceChild (
             Instance->ControllerHandle,
             gImageHandle,
             &gEfiDns4ServiceBindingProtocolGuid,
             &HttpToken->Dns4Handle
             );
  if (EFI_ERROR(Status)) {
    HttpToken->Dns4 = NULL;
  } else {
    Status = gBS->HandleProtocol (
                    HttpToken->Dns4Handle,
                    &gEfiDns4ProtocolGuid,
                    (VOID **) &HttpToken->Dns4
                    );
    if (EFI_ERROR(Status)) {
      HttpToken->Dns4 = NULL;
    }
  }

  Status = gBS->HandleProtocol (
                  Instance->ControllerHandle,
                  &gEfiIp4Config2ProtocolGuid,
                  (VOID **) &HttpToken->Ip4Config2
                  );
  if (EFI_ERROR (Status)) {
    HttpToken->Ip4Config2 = NULL;
  }

  HttpToken->IsIpv6 = FALSE;
  Status = gBS->HandleProtocol (
                  Instance->ControllerHandle,
                  &gEfiIp6ConfigProtocolGuid,
                  (VOID **) &HttpToken->Ip6Config
                  );
  if (EFI_ERROR (Status)) {
    HttpToken->Ip6Config = NULL;
  } else {
    RestResolveDnsPolicy (&HttpToken->IsIpv6);
  }

  *RestHttpToken = HttpToken;
  return EFI_SUCCESS;
}

EFI_STATUS
RestHttpClientParsingURI (
  IN OUT REST_HTTP_TOKEN       *RestHttpToken
  )
{
  EFI_STATUS                   Status;
  CHAR8                        *URL;
  UINTN                        URLLen;
  VOID                         *UrlParser;

  UrlParser = NULL;

  if (!IsURLValid (RestHttpToken->HttpRequest->Url)) {
    RestHttpToken->ReturnHttpStatusCode = HTTP_STATUS_400_BAD_REQUEST;
    return EFI_INVALID_PARAMETER;
  }

  //
  // Translate UNICODE to ASCII and parsing URL to check status of URL from HttpRequest
  //
  URLLen = StrLen (RestHttpToken->HttpRequest->Url) + 1;
  URL    = (CHAR8 *) AllocateZeroPool (URLLen);
  if (URL == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  Status = UnicodeStrToAsciiStrS (RestHttpToken->HttpRequest->Url, URL, URLLen);
  if (EFI_ERROR (Status)) {
    goto FINISH;
  }
  
  Status = HttpParseUrl (
             URL,
             (UINT32) URLLen,
             FALSE,
             (VOID **) &UrlParser
             );
  if (EFI_ERROR (Status)) {
    RestHttpToken->ReturnHttpStatusCode = HTTP_STATUS_400_BAD_REQUEST;
    Status =  EFI_INVALID_PARAMETER;
    goto FINISH;
  }

  //
  // Get HostName.
  // It might be those cases.
  //   IPv4 address 192.168.111.1
  //   IPv6 address [2048:1096::1]
  //   Domain name  www.domain.com
  //
  RestHttpToken->HostName   = NULL;
  Status     = HttpUrlGetHostName (URL, UrlParser, &RestHttpToken->HostName);
  if (!EFI_ERROR (Status)) {
    goto FINISH;
  }

  //
  // Try to translate HostName to IPv4, if it success, means the HostName is IPv4 address type.
  //
  Status = HttpUrlGetIp4  (URL, UrlParser, &RestHttpToken->RemoteAddr);
  if (!EFI_ERROR (Status)) {
    goto FINISH;
  }

  //
  // Try to translate HostName to IPv6, if it success, means the HostName is IPv6 address type.
  //
  Status = HttpUrlGetIp6  (URL, UrlParser, &RestHttpToken->RemoteAddr6);
  if (!EFI_ERROR (Status)) {
    RestHttpToken->IsIpv6 = TRUE;
    goto FINISH;
  }

  //
  // Domain name need to parsing by DNS.
  // This function will not check status from DNS, so it RestDxe will choose IPv4 or IPv6 HTTP services.
  //

//// DNS detect function disabled, enable this function after review and resolve problem of DNS.
//
//  //
//  // Get DNS addresses from Ip4Config2 protocol.
//  //
//  if ((RestHttpToken->Ip4Config2 == NULL) || (RestHttpToken->Dns4 == NULL)) {
//    RestHttpToken->ReturnHttpStatusCode = HTTP_STATUS_503_SERVICE_UNAVAILABLE;
//    Status = EFI_UNSUPPORTED;
//    goto FINISH;
//  }
//
//
//  Status = RestHttpGetDnsFromIp4Config2Data (RestHttpToken);
//  if (EFI_ERROR (Status)) {
//    goto FINISH;
//  }
//
//  Status = RestHttpDns4Configure (RestHttpToken);
//  if (EFI_ERROR (Status)) {
//    goto FINISH;
//  }
//
//  Status = RestHttpDns4TranslateAddress (RestHttpToken);
//  if (EFI_ERROR (Status)) {
//    goto FINISH;
//  }

FINISH:
  if (UrlParser != NULL) {
    HttpUrlFreeParser (UrlParser);
  }
  FreePool (URL);
  return Status;
}

EFI_STATUS
RestDestroyHttpClientToken (
  REST_BINDING_INSTANCE        *Instance,
  REST_HTTP_TOKEN              *HttpToken
  )
{
  if (HttpToken == NULL) {
    return EFI_SUCCESS;
  }

  gBS->CloseEvent (HttpToken->TimeoutEvent);
  if (HttpToken->HttpHandle != NULL) {
    HttpToken->Http->Configure (HttpToken->Http, NULL);
//    NetLibDestroyServiceChild (
//      NicHandle,
//      gImageHandle,
//      &gEfiHttpServiceBindingProtocolGuid,
//      HttpToken->HttpHandle
//      );
  }

  if (HttpToken->Dns4Handle != NULL) {
    NetLibDestroyServiceChild (
      Instance->ControllerHandle,
      gImageHandle,
      &gEfiDns4ServiceBindingProtocolGuid,
      HttpToken->Dns4Handle
      );
  }
  if (HttpToken->DnsServerList != NULL) {
    FreePool (HttpToken->DnsServerList);
  }
  FreePool (HttpToken);

  return EFI_SUCCESS;
}

EFI_STATUS
RestHttpClientConfigIpv4 (
  IN      REST_HTTP_TOKEN              *HttpToken,
  IN OUT  EFI_HTTP_CONFIG_DATA         *HttpConfigData
  )
{
  EFI_HTTPv4_ACCESS_POINT      *Http4AccessPoint;

  if (HttpToken->Ip4Config2 == NULL) {
    return EFI_NOT_READY;
  }

  Http4AccessPoint    = AllocateZeroPool (sizeof (EFI_HTTPv4_ACCESS_POINT));
  if (Http4AccessPoint == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  Http4AccessPoint->UseDefaultAddress  = TRUE;
  HttpConfigData->AccessPoint.IPv4Node = Http4AccessPoint;

  return EFI_SUCCESS;
}


EFI_STATUS
RestHttpClientConfigIpv6 (
  IN      REST_HTTP_TOKEN              *HttpToken,
  IN OUT  EFI_HTTP_CONFIG_DATA         *HttpConfigData
  )
{
  EFI_STATUS                       Status;
  EFI_HTTPv6_ACCESS_POINT          *Http6AccessPoint;
  EFI_IP6_CONFIG_PROTOCOL          *Ip6Config;
  UINTN                            DataSize;
  EFI_IP6_CONFIG_INTERFACE_INFO    *IfInfo;

  if (HttpToken->Ip6Config == NULL) {
    return EFI_NOT_READY;
  }

  IfInfo            = NULL;
  Http6AccessPoint  = NULL;
  Ip6Config         = HttpToken->Ip6Config;
  DataSize          = 0;
  Status = Ip6Config->GetData (
                        Ip6Config,
                        Ip6ConfigDataTypeInterfaceInfo,
                        &DataSize,
                        NULL
                        );
  if (Status == EFI_BUFFER_TOO_SMALL) {
    IfInfo = AllocateZeroPool (DataSize);
    if (IfInfo == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
    Status = Ip6Config->GetData (
                      Ip6Config,
                      Ip6ConfigDataTypeInterfaceInfo,
                      &DataSize,
                      IfInfo
                      );
  }
  if (EFI_ERROR (Status)) {
    goto EXIT;
  }

  if (IfInfo == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto EXIT;
  }

  Http6AccessPoint    = AllocateZeroPool (sizeof (EFI_HTTPv6_ACCESS_POINT));
  if (Http6AccessPoint == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto EXIT;
  }

  /// Get lastest address, expect that is not link-local address
  CopyMem (&Http6AccessPoint->LocalAddress, &(IfInfo->AddressInfo[IfInfo->AddressInfoCount-1].Address), sizeof (EFI_IPv6_ADDRESS));
  HttpConfigData->AccessPoint.IPv6Node = Http6AccessPoint;

EXIT:
  if (IfInfo != NULL) {
    FreePool (IfInfo);
  }
  return Status;
}

EFI_STATUS
RestHttpClientConfig (
  IN REST_HTTP_TOKEN           *HttpToken
  )
{
  EFI_STATUS                   Status;
  EFI_EVENT                    Event;
  EFI_HTTP_CONFIG_DATA         *HttpConfigData;

  HttpConfigData = &HttpToken->HttpConfigData;
  HttpConfigData->HttpVersion        = HttpVersion11;
  HttpConfigData->TimeOutMillisec    = HTTP_RESPONSE_TIME_OUT;

  HttpConfigData->LocalAddressIsIPv6   = HttpToken->IsIpv6;
  if (!HttpToken->IsIpv6) {
    Status = RestHttpClientConfigIpv4 (HttpToken, HttpConfigData);
  } else {
    Status = RestHttpClientConfigIpv6 (HttpToken, HttpConfigData);
  }
  if (EFI_ERROR (Status)) {
    goto ON_ERROR;
  }

  Status = HttpToken->Http->Configure (HttpToken->Http, HttpConfigData);
  if (EFI_ERROR (Status)) {
    goto ON_ERROR;
  }

  //
  // Create events for variuos asynchronous operations.
  //
  Event = NULL;
  Status = gBS->CreateEvent (
                  EVT_NOTIFY_SIGNAL,
                  TPL_NOTIFY,
                  RestHttpClientCommonNotify,
                  &HttpToken->IsTxDone,
                  &Event
                  );
  if (EFI_ERROR (Status)) {
    goto ON_ERROR;
  }
  HttpToken->RequestEvent   = Event;

  Event = NULL;
  Status = gBS->CreateEvent (
                  EVT_NOTIFY_SIGNAL,
                  TPL_NOTIFY,
                  RestHttpClientCommonNotify,
                  &HttpToken->IsRxDone,
                  &Event
                  );
  if (EFI_ERROR (Status)) {
    goto ON_ERROR;
  }
  HttpToken->ResponseEvent   = Event;

  Event = NULL;
  Status = gBS->CreateEvent (
                  EVT_TIMER,
                  TPL_CALLBACK,
                  NULL,
                  NULL,
                  &Event
                  );
  if (EFI_ERROR (Status)) {
    goto ON_ERROR;
  }
  HttpToken->TimeoutEvent = Event;

  return EFI_SUCCESS;

ON_ERROR:
  if (HttpConfigData->AccessPoint.IPv4Node != NULL) {
    FreePool (HttpConfigData->AccessPoint.IPv4Node);
  }
  if (HttpConfigData->AccessPoint.IPv6Node != NULL) {
    FreePool (HttpConfigData->AccessPoint.IPv6Node);
  }
  return Status;
}

EFI_STATUS
RestHttpGetResponse (
  IN REST_HTTP_TOKEN           *HttpToken
  )
{
  EFI_STATUS                   Status;
  BOOLEAN                      HeaderOnly;
  VOID                         *Parser;
  UINT8                        *ContentBuffer;
  UINT8                        *Block;
  UINTN                        BlockSize;
  UINTN                        ContentLength;
  UINTN                        ReceivedLength;
  EFI_HTTP_MESSAGE             *Response;

  ContentBuffer = NULL;
  HeaderOnly = (HttpToken->HttpRequest->Method == HttpMethodHead) ? TRUE : FALSE;
  Response   = &HttpToken->Response;
  //
  // Transmit HTTP request
  //
  Status = RestHttpOutput (HttpToken);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Receive HTTP header
  //
  Status = RestHttpInput (HttpToken, TRUE);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = HttpInitMsgParser (
             HttpToken->HttpRequest->Method,
             HttpToken->ReturnHttpStatusCode,
             Response->HeaderCount,
             Response->Headers,
             RestHttpParserCallback,
             (VOID *) HttpToken,
             &Parser
             );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // If "Content-Length" not in HTTP header, perhaps is identity transfer-coding.
  //
  Status = HttpGetEntityLength (Parser, &ContentLength);
  if (!EFI_ERROR (Status)) {
    HttpToken->IdentityMode = TRUE;
    HttpToken->ContentLength = ContentLength;
  }

  if (HeaderOnly) {
    goto EXIT;
  }

  //
  // Start to receive HTTP body.
  //
  ReceivedLength = 0;
  if (HttpToken->IdentityMode) {
    ContentBuffer = AllocateZeroPool (ContentLength);
    if (ContentBuffer == NULL) {
      Status = EFI_OUT_OF_RESOURCES;
      goto EXIT;
    }

    while (ReceivedLength < ContentLength) {
      Response->Body       = ContentBuffer + ReceivedLength;
      if (ContentLength - ReceivedLength >= REST_BLOCK_SIZE) {
        Response->BodyLength = REST_BLOCK_SIZE;
      } else {
        Response->BodyLength = ContentLength - ReceivedLength;
      }
      Status = RestHttpInput (HttpToken, FALSE);
      if (EFI_ERROR (Status)) {
        goto EXIT;
      }
      ReceivedLength += Response->BodyLength;
    }

    if (ReceivedLength > ContentLength) {
      goto EXIT;
    }

    if (Response->Data.Response == NULL) {
      Response->Data.Response = AllocateZeroPool (sizeof (EFI_HTTP_RESPONSE_DATA));
      if (Response->Data.Response == NULL) {
        Status = EFI_OUT_OF_RESOURCES;
        goto EXIT;
      }
    }
    Response->Data.Response->StatusCode = HttpToken->ReturnHttpStatusCode;
    Response->Body       = ContentBuffer;
    Response->BodyLength = ReceivedLength;

  } else {
    //
    // Chunk transfer
    // This function must be review and debug.
    //
    Block     = AllocateZeroPool (REST_BLOCK_SIZE);
    BlockSize = REST_BLOCK_SIZE;
    if (Block == NULL) {
      Status = EFI_OUT_OF_RESOURCES;
      goto EXIT;
    }

    HttpToken->ChunkDataBuffer = NULL;
    HttpToken->ChunkDataLength = 0;
    while (!HttpIsMessageComplete (Parser)) {
      Response->Body       = Block;
      Response->BodyLength = BlockSize;
      Status = RestHttpInput (HttpToken, FALSE);
      if (EFI_ERROR (Status)) {
        goto EXIT;
      }

      //
      // Parse the new received block of the message-body, the block will be saved in cache.
      //
      Status = HttpParseMessageBody (
                 Parser,
                 Response->BodyLength,
                 Response->Body
                 );
      if (EFI_ERROR (Status)) {
        goto EXIT;
      }
    }

    //
    // Get correct length from parser of Chunk transfer.
    //
    Status = HttpGetEntityLength (Parser, &ContentLength);
    if (EFI_ERROR (Status)) {
      goto EXIT;
    }

    Response->Data.Response->StatusCode = HttpToken->ReturnHttpStatusCode;
    Response->Body       = HttpToken->ChunkDataBuffer;
    Response->BodyLength = HttpToken->ChunkDataLength;
  }

EXIT:
  if (EFI_ERROR (Status)) {
    if (ContentBuffer != NULL) {
      FreePool (ContentBuffer);
    }
  }

  if (Parser != NULL) {
    HttpFreeMsgParser (Parser);
  }
  if (!HeaderOnly && (HttpToken->IdentityMode)) {
    if (ContentBuffer != NULL) {
      Response->Body = ContentBuffer;
    }
  }
  return Status;
}

EFI_STATUS
RestHttpOutput (
  IN REST_HTTP_TOKEN           *HttpToken
  )
{
  EFI_STATUS                   Status;
  EFI_HTTP_MESSAGE             RequestMessage;
  EFI_HTTP_REQUEST_DATA        RequestData;
  EFI_HTTP_PROTOCOL            *Http;
  EFI_HTTP_TOKEN               RequestToken;
  EFI_EVENT                    TimeoutEvent;

  ZeroMem (&RequestData, sizeof (EFI_HTTP_REQUEST_DATA));
  ZeroMem (&RequestMessage, sizeof (EFI_HTTP_MESSAGE));
  ZeroMem (&RequestToken, sizeof (EFI_HTTP_TOKEN));
  Http = HttpToken->Http;

  //
  // Prepare HTTP Token
  //
  RequestData.Method = HttpToken->HttpRequest->Method;
  RequestData.Url    = HttpToken->HttpRequest->Url;
  RequestMessage.Data.Request = &RequestData;

  //
  // Prepare HTTP header, if header coming from external
  // Compare and merge it
  //
  if (HttpToken->Request.HeaderCount == 0) {
    RequestMessage.HeaderCount = sizeof (mCommonHeader) / sizeof (EFI_HTTP_HEADER);
    RequestMessage.Headers     = mCommonHeader;
  } else {
    Status = RestMegerHttpHeaders (&HttpToken->Request, &RequestMessage);
    if (EFI_ERROR (Status)) {
      goto EXIT;
    }
  }

  //
  // Update HOST name in HTTP header.
  //
  Status = RestUpdateHttpHost (&RequestMessage, HttpToken->HostName);
  if (EFI_ERROR (Status)) {
    goto EXIT;
  }

  //
  // Prepare HTTP body, if command is POST.
  //
  if ( HttpToken->HttpRequest->Method == HttpMethodPost ||
       HttpToken->HttpRequest->Method == HttpMethodPatch ||
       HttpToken->HttpRequest->Method == HttpMethodPut) {
    if ((HttpToken->Request.BodyLength != 0) && (HttpToken->Request.Body != NULL)) {
      RequestMessage.BodyLength = HttpToken->Request.BodyLength;
      RequestMessage.Body       = HttpToken->Request.Body;
    }
  }
  RequestToken.Event   = HttpToken->RequestEvent;
  RequestToken.Message = &RequestMessage;
  RequestToken.Status  = EFI_NOT_READY;
  HttpToken->IsTxDone = FALSE;
  RestHttpHeaderDump (&HttpToken->Request);

  //
  // Start the timer, and wait Timeout seconds to receive the header packet.
  //
  TimeoutEvent = HttpToken->TimeoutEvent;
  Status = gBS->SetTimer (TimeoutEvent, TimerRelative, HTTP_DELAY_TIME);
  if (EFI_ERROR (Status)) {
    goto EXIT;
  }

  //
  // Transfer HTTP request to server.
  //
  Status = Http->Request (
                   Http,
                   &RequestToken
                   );
  if (EFI_ERROR (Status)) {
    goto EXIT;
  }

  //
  // Wait for finish.
  //
  while (!HttpToken->IsTxDone && EFI_ERROR (gBS->CheckEvent (TimeoutEvent))) {
    Http->Poll (Http);
  }

  gBS->SetTimer (TimeoutEvent, TimerCancel, 0);
  if (!HttpToken->IsTxDone) {
    RequestToken.Status = EFI_TIMEOUT;
  }

EXIT:
  if (DEBUG_MODE) {
    AsciiPrint ("%08x\n", RequestToken.Status);
  }
  if ((RequestMessage.Headers != NULL) && (RequestMessage.Headers != mCommonHeader)) {
    FreePool (RequestMessage.Headers);
  }
  return RequestToken.Status;
}

EFI_STATUS
RestHttpInput (
  IN REST_HTTP_TOKEN           *HttpToken,
  IN BOOLEAN                   HeaderOnly
  )
{
  EFI_STATUS                   Status;
  EFI_HTTP_MESSAGE             ResponseMessage;
  EFI_HTTP_RESPONSE_DATA       ResponseData;
  EFI_HTTP_PROTOCOL            *Http;
  EFI_EVENT                    TimeoutEvent;
  UINT8                        *Block;
  UINTN                        BlockSize;
  EFI_HTTP_TOKEN               ResponseToken;

  Block     = NULL;
  BlockSize = 0;
  if (!HeaderOnly) {
    if ((HttpToken->Response.BodyLength != 0) && (HttpToken->Response.Body != NULL)) {
      Block     = HttpToken->Response.Body;
      BlockSize = HttpToken->Response.BodyLength;
    } else {
      Block = AllocateZeroPool (REST_BLOCK_SIZE);
      if (Block == NULL) {
        return EFI_OUT_OF_RESOURCES;
      }
      BlockSize = REST_BLOCK_SIZE;
    }
  }

  //
  // Start the timer, and wait Timeout seconds to receive the header packet.
  //
  TimeoutEvent = HttpToken->TimeoutEvent;
  Status = gBS->SetTimer (TimeoutEvent, TimerRelative, HTTP_DELAY_TIME);
  if (EFI_ERROR (Status)) {
    goto EXIT;
  }

  //
  // Queue the response token to HTTP instances.
  //
  ZeroMem (&ResponseMessage, sizeof (EFI_HTTP_MESSAGE));
  ZeroMem (&ResponseData, sizeof (EFI_HTTP_RESPONSE_DATA));
  ZeroMem (&ResponseToken, sizeof (EFI_HTTP_TOKEN));
  ResponseMessage.Data.Response = (HeaderOnly) ? &ResponseData : NULL;
  ResponseMessage.Body          = Block;
  ResponseMessage.BodyLength    = BlockSize;
  ResponseMessage.HeaderCount   = 0;
  ResponseMessage.Headers       = NULL;
  ResponseToken.Message = &ResponseMessage;
  ResponseToken.Status  = EFI_NOT_READY;
  ResponseToken.Event   = HttpToken->ResponseEvent;

  Http = HttpToken->Http;
  HttpToken->IsRxDone = FALSE;
  Status = Http->Response (
                   Http,
                   &ResponseToken
                   );
  if (EFI_ERROR (Status)) {
    goto EXIT;
  }

  //
  // Poll the network until receive finish.
  //
  while (!HttpToken->IsRxDone && EFI_ERROR (gBS->CheckEvent (TimeoutEvent))) {
    Http->Poll (Http);
  }

  gBS->SetTimer (TimeoutEvent, TimerCancel, 0);
  if (!HttpToken->IsRxDone) {
    //
    // Timeout occurs, cancel the response token.
    //
    Http->Cancel (Http, &ResponseToken);
    Status = EFI_TIMEOUT;
    goto EXIT;
  }

  if (DEBUG_MODE && HeaderOnly) {
    AsciiPrint ("%08x %d %d\n", ResponseToken.Status, ResponseMessage.HeaderCount, ResponseMessage.BodyLength);
    RestHttpHeaderDump (&ResponseMessage);
  }

  HttpToken->Response.Body         = ResponseMessage.Body;
  HttpToken->Response.BodyLength   = ResponseMessage.BodyLength;
  if ((ResponseMessage.HeaderCount != 0) && (ResponseMessage.Headers != NULL)) {
    HttpToken->Response.HeaderCount  = ResponseMessage.HeaderCount;
    HttpToken->Response.Headers      = ResponseMessage.Headers;
  }

  if (HeaderOnly) {
    HttpToken->ReturnHttpStatusCode  = ResponseMessage.Data.Response->StatusCode;
  }
  Status = ResponseToken.Status;

EXIT:
  gBS->SetTimer (TimeoutEvent, TimerCancel, 0);

  return Status;
}
