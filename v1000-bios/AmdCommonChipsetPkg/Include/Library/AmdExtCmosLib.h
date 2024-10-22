/** @file
 AMDEXTCMOS library include file

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _AMD_EXT_CMOS_LIB_H_
#define _AMD_EXT_CMOS_LIB_H_

#include <Uefi.h>


/**
 Read an 8-bit value from the address offset of Ext CMOS.

 @param[in] Address  Address offset of Ext CMOS.

 @return An 8-bit value in the address offset of Ext CMOS.
**/
UINT8
AmdReadExtCmos8 (
  IN UINT8                                 Address
  );

/**
 Write an 8-bit value to the address offset of Ext CMOS.

 @param[in] Address  Address offset of CMOS.
 @param[in] Data     Data written into CMOS.
**/
VOID
AmdWriteExtCmos8 (
  IN UINT8                                 Address,
  IN UINT8                                 Data
  );

/**
 Read a 16-bit value from the address offset of Ext CMOS.

 @param[in] Address  Address offset of Ext CMOS.

 @return A 16-bit value in the address offset of Ext CMOS.
**/
UINT16
AmdReadExtCmos16 (
  IN UINT8                                 Address
  );

/**
 Write a 16-bit value to the address offset of Ext CMOS.

 @param[in] Address  Address offset of Ext CMOS.
 @param[in] Data     Data written into Ext CMOS.
**/
VOID
AmdWriteExtCmos16 (
  IN UINT8                                 Address,
  IN UINT16                                Data
  );

/**
 Read a 32-bit value from the address offset of Ext CMOS.

 @param[in] Address  Address offset of Ext CMOS.

 @return A 32-bit value in the address offset of Ext CMOS.
**/
UINT32
AmdReadExtCmos32 (
  IN UINT8                                 Address
  );

/**
 Write a 32-bit value to the address offset of Ext CMOS.

 @param[in] Address  Address offset of Ext CMOS.
 @param[in] Data     Data written into Ext CMOS.
**/
VOID
AmdWriteExtCmos32 (
  IN UINT8                                 Address,
  IN UINT32                                Data
  );

#endif

