/** @file
  The instance of SerialPortPostCodeLib Library

;******************************************************************************
;* Copyright (c) 2016 - 2017, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <Library/SerialPortPostCodeLib.h>
#include <Library/PrintLib.h>
#include <Library/PostCodeMiscLib.h>
#include <SerialPortFunc.h>

/**
  This function writes the PostCode string "<<Post Code>>: XX" to the serial port.

  @param [in] PostCode   The PostCode value of the PostCode string "<<Post Code>>: XX"

**/
VOID
SerialPortPostCode (
  IN UINTN PostCode
  )
{
  CHAR8 String[BUFFER_SIZE];
  UINTN CharCount;

  //
  // Transfer PostCode to ASCII string
  //
  CharCount = GetPostCodeString (
                ComPortStr,
                PostCode,
                BUFFER_SIZE,
                String
                );

  //
  // Write the PostCode string to serial port
  //
  SerialPortWrite ((UINT8 *) String, CharCount);
  
}

