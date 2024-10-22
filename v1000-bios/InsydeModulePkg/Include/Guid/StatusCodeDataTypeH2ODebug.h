/** @file
  This file defines the GUID and data structure used to pass DEBUG() macro
  information to the Status Code Protocol and Status Code PPI.

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

#ifndef _STATUS_CODE_DATA_TYPE_H2O_DEBUG_H_
#define _STATUS_CODE_DATA_TYPE_H2O_DEBUG_H_

///
/// The Global ID used to identify a structure of type EFI_DEBUG_INFO.
///
#define EFI_STATUS_CODE_DATA_TYPE_H2O_DEBUG_GUID \
  { \
    0x4210019b, 0x9f17, 0x468c, { 0xbf, 0xe3, 0x53, 0x4, 0x73, 0xee, 0xe4, 0x3c }  \
  }

typedef struct {
  ///
  /// The debug error level passed into a DEBUG() macro.
  ///
  UINT32        ErrorLevel;
  CHAR8         *Format;
  VA_LIST       VaListMarker;
  BASE_LIST     BaseListMarker;

} H2O_DEBUG_INFO;

extern EFI_GUID gEfiStatusCodeDataTypeH2ODebugGuid;

#endif // _STATUS_CODE_DATA_TYPE_H2O_DEBUG_H_
