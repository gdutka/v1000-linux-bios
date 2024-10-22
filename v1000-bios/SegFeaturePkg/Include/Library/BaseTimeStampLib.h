/** @file
  Header of BaseTimeStampLib

;******************************************************************************
;* Copyright (c) 2021, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef __BASE_TIME_STAMP_LIB_H__
#define __BASE_TIME_STAMP_LIB_H__

#include <Uefi.h>

/**
  Get timestamp string (ASCII).

  The timestamp string has format of "yyyy/mm/dd hh:mm:ss [aaa.bbbs]".
  As example, "2021/02/17 08:30:15 [015.394s]".
  The time in square brackets is elapsed time since system boot-up.

  @param[in]  BufferSize  The size of the string buffer. According to the format,
                          the size must be greater than 44 bytes.
  @param[out] Buffer      Pointer to the buffer for getting the timestamp string.

  @retval   The timestamp string.
  @retval   NULL. If an inappropriate buffer size was given.

**/
UINTN
EFIAPI
GetTimeStampString (
  IN  UINTN   BufferSize,
  OUT CHAR8   *Buffer
  );

#endif