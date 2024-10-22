/** @file
 H2O Status Code Definitions

;******************************************************************************
;* Copyright (c) 2018 - 2019, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _H2O_STATUS_CODE_H_
#define _H2O_STATUS_CODE_H_


///
/// General partitioning scheme for Progress and Error Codes are:
///   - 0x0000-0x0FFF    Shared by all sub-classes in a given class.
///   - 0x1000-0x7FFF    Subclass Specific.
///   - 0x8000-0xBFFF    OEM specific.
///   - 0xC000-0xFFFF    H2O specific
///@{

#define H2O_SPECIFIC                              0xC000

//
// H2O Class variable driver subclass error code definition
//
#define H2O_AUTH_VARIABLE_VERIFY_FAILED          (H2O_SPECIFIC | 0x00000101)
#define H2O_VARIABLE_STORE_FULL                  (H2O_SPECIFIC | 0x00000102)
#define H2O_VARIABLE_DUPLICATION                 (H2O_SPECIFIC | 0x00000103)
#define H2O_VARIABLE_RECLAIM_FAILED              (H2O_SPECIFIC | 0x00000104)
#define H2O_VARIABLE_CACHE_CORRUPTION            (H2O_SPECIFIC | 0x00000105)
#define H2O_VARIABLE_WRITE_NON_VOLATILE_FAILED   (H2O_SPECIFIC | 0x00000106)

//
// H2O Class memory type information subclass error code definition
//
#define H2O_TOTAL_RESERVED_MEMORY_TOO_LARGE      (H2O_SPECIFIC | 0x00000201)
#define H2O_SET_MEMORY_INFO_VARIABLE_FAILED      (H2O_SPECIFIC | 0x00000202)

#endif
