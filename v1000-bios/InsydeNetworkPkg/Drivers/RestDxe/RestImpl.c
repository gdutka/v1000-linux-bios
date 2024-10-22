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

#include "RestDriver.h"

/**
  Provides a simple HTTP-like interface to send and receive resources from a REST
  service.

  The SendReceive() function sends an HTTP request to this REST service, and returns a
  response when the data is retrieved from the service. RequestMessage contains the HTTP
  request to the REST resource identified by RequestMessage.Request.Url. The
  ResponseMessage is the returned HTTP response for that request, including any HTTP
  status.

  @param[in]  This                Pointer to EFI_REST_PROTOCOL instance for a particular
                                  REST service.
  @param[in]  RequestMessage      Pointer to the HTTP request data for this resource.
  @param[out] ResponseMessage     Pointer to the HTTP response data obtained for this
                                  requested.

  @retval EFI_SUCCESS             Operation succeeded.
  @retval EFI_INVALID_PARAMETER   This, RequestMessage, or ResponseMessage are NULL.
  @retval EFI_DEVICE_ERROR        An unexpected system or network error occurred.
**/
EFI_STATUS
RestSendReceive (
  IN  EFI_REST_PROTOCOL         *This,
  IN  EFI_HTTP_MESSAGE          *RequestMessage,
  OUT EFI_HTTP_MESSAGE          *ResponseMessage
  )
{
  EFI_STATUS                    Status;
  REST_BINDING_INSTANCE         *RestInstance;
  REST_HTTP_TOKEN               *HttpToken;

  //
  // Entry check of RestSendReceive
  //
  if ((This == NULL) || (RequestMessage == NULL) || (ResponseMessage == NULL)) {
    return EFI_INVALID_PARAMETER;
  }
  RestInstance = (REST_BINDING_INSTANCE *) ((UINTN) This);
  if (RestInstance->Signature != REST_BINDING_SIGNATURE) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Create HTTP token
  //
  HttpToken = NULL;
  Status = RestCreateHttpClientToken (RestInstance, &HttpToken);
  if (EFI_ERROR (Status)) {
    goto EXIT;
  }

  //
  // Do HTTP Send Receive to get data from URI.
  //
  HttpToken->HttpRequest = RequestMessage->Data.Request;
  HttpToken->Request.HeaderCount = RequestMessage->HeaderCount;
  HttpToken->Request.Headers     = RequestMessage->Headers;
  HttpToken->Request.BodyLength  = RequestMessage->BodyLength;
  HttpToken->Request.Body        = RequestMessage->Body;
  Status = RestHttpClientParsingURI (HttpToken);
  if (EFI_ERROR (Status)) {
    goto EXIT;
  }

  //
  // Config HTTP
  //
  Status = RestHttpClientConfig (HttpToken);
  if (EFI_ERROR (Status)) {
    goto EXIT;
  }

  //
  // Get response via HTTP
  //
  Status = RestHttpGetResponse (HttpToken);
  if (EFI_ERROR (Status)) {
    goto EXIT;
  } 
  //
  // Feedback content and content length.
  //
  if ((&HttpToken->Response.Data != NULL) && (HttpToken->Response.Data.Response == NULL)) {
    ResponseMessage->Data.Response->StatusCode = HttpToken->ReturnHttpStatusCode;
  } else {
    ResponseMessage->Data.Response = HttpToken->Response.Data.Response;
  }
  ResponseMessage->Body          = HttpToken->Response.Body;
  ResponseMessage->BodyLength    = HttpToken->Response.BodyLength;
  ResponseMessage->Headers       = HttpToken->Response.Headers;
  ResponseMessage->HeaderCount   = HttpToken->Response.HeaderCount;

EXIT:
  RestDestroyHttpClientToken (RestInstance, HttpToken);
  return Status;
}

/**
  The GetServiceTime() function is an optional interface to obtain the current time from
  this REST service instance. If this REST service does not support retrieving the time,
  this function returns EFI_UNSUPPORTED.

  If HTTP processing successed before, It will return the time of final HTTP header.

  @param[in]  This                Pointer to EFI_REST_PROTOCOL instance.
  @param[out] Time                A pointer to storage to receive a snapshot of the
                                  current time of the REST service.

  @retval EFI_SUCCESS             Operation succeeded
  @retval EFI_INVALID_PARAMETER   This or Time are NULL.
  @retval EFI_UNSUPPORTED         The RESTful service does not support returning the
                                  time.
  @retval EFI_DEVICE_ERROR        An unexpected system or network error occurred.
**/
EFI_STATUS
RestGetTime (
  IN  EFI_REST_PROTOCOL         *This,
  OUT EFI_TIME                  *Time
  )
{
  return EFI_UNSUPPORTED;
}

