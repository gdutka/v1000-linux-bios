/** @file
  Extended API for Opal Core library.

;******************************************************************************
;* Copyright (c) 2017, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _OPAL_CORE_EX_H_
#define _OPAL_CORE_EX_H_

#include <IndustryStandard/TcgStorageOpal.h>

#include <Library/TcgStorageCoreLib.h>
#include <Protocol/StorageSecurityCommand.h>

/**
  The function performs send, recv, check comIDs, check method status action.

  @param[in]      Session         OPAL_SESSION related to this method..
  @param[in]      SendSize        Transfer Length of Buffer (in bytes) - always a multiple of 512
  @param[in]      Buffer          Address of Data to transfer
  @param[in]      BufferSize      Full Size of Buffer, including space that may be used for padding.
  @param[in]      ParseStruct     Structure used to parse received TCG response.
  @param[in]      MethodStatus    Method status of last action performed.  If action succeeded, it should be TCG_METHOD_STATUS_CODE_SUCCESS.

**/
TCG_RESULT
EFIAPI
OpalPerformMethod(
  OPAL_SESSION     *Session,
  UINT32           SendSize,
  VOID             *Buffer,
  UINT32           BufferSize,
  TCG_PARSE_STRUCT *ParseStruct,
  UINT8            *MethodStatus
  );

#endif // _OPAL_CORE_EX_H_
