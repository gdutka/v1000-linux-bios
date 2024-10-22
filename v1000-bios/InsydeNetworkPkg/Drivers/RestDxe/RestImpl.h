/** @file
  REST protocol implementation.

;******************************************************************************
;* Copyright (c) 2012 - 2018, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef __REST_IMPL_H__
#define __REST_IMPL_H__

typedef struct {
  EFI_REST_PROTOCOL                 RestProtocol;
  UINT32                            Signature;
  EFI_HANDLE                        ControllerHandle;
  EFI_HANDLE                        HttpHandle;
  EFI_HTTP_PROTOCOL                 *HttpProtocol;
} REST_BINDING_INSTANCE;

typedef struct _REST_INSTANCE {
  EFI_HANDLE                               NicDeviceHandle;
  EFI_SERVICE_BINDING_PROTOCOL             *HttpServiceBinding;
} REST_INSTANCE;

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
  );

/**
  The GetServiceTime() function is an optional interface to obtain the current time from
  this REST service instance. If this REST service does not support retrieving the time,
  this function returns EFI_UNSUPPORTED.

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
  );

extern EFI_REST_PROTOCOL  mRestTemplate;

#endif
