/** @file
  Header file for PostCodeConfigLib
  
  This library provides functions to configure the debug registers
  and IDT table entry to perform PostCode feature.

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

#ifndef _POST_CODE_MISC_LIB_H_
#define _POST_CODE_MISC_LIB_H_

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/PrintLib.h>
#include <Library/DebugLib.h>

#define POST_CODE_MASK      (UINTN)(-1)
#define BUFFER_SIZE         50

//
// Graphic VGA resolution: 
// 1024 x 768
//
#define GRAPHIC_VGA_MAX_COL   128
#define GRAPHIC_VGA_MAX_ROW   48

typedef enum {
  ScreenDisplayStr,
  ScreenClearStr,  
  ComPortStr,
} POST_CODE_STR_TYPE;


/**
  Get the operand size, in bytes, of the out instruction which is the previous instruction of the SystemContextIp.
  It will be the Post Code length.

  @param [in]   SystemContextIp    The current IP when the exception is triggering.
  @param [out]  PostCodeLength     The length, in bytes, of the Post Code which is outputted to the debug port.
  
**/
VOID
EFIAPI 
GetPostCodeLength (
  IN  UINTN SystemContextIp,
  OUT UINT8 *PostCodeLength
  );

/**
  Produces a Null-terminated ASCII string in an output buffer based on a Null-terminated
  ASCII format string.

  There are three type strings will be generated. 
  The output string is specified by StringType.
    1.ScreenDisplayStr: The Post Code string which is printed on monitor 
    2.ScreenClearStr  : The blank string which is used to clear the Post Code on monitor
    3.ComPortStr      : The Post Code string which is sent to COM Port 

  @param [in]  StringType    The string type which will be generated.
  @param [in]  PostCode      The Post Code which will be generated into the output string.
  @param [in]  BufferSize    The size, in bytes, of the output buffer specified by Buffer.
  @param [out] Buffer        A pointer to the output buffer for the produced Null-terminated 
                             ASCII string.

  @return The number of ASCII characters in the produced output buffer not including the
          Null-terminator. If return 0, the bufferSize may be too small or the Buffer is NULL.
**/
UINTN
EFIAPI 
GetPostCodeString (
  IN  POST_CODE_STR_TYPE StringType,
  IN  UINTN              PostCode,     OPTIONAL
  IN  UINTN              BufferSize,
  OUT CHAR8              *Buffer
  );

#endif 
