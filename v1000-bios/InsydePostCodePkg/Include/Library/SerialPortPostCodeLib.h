/** @file
  Header file for SerialPortPostCodeLib
  
  This library provides functions to show the PostCode on the serial port

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

#ifndef _SERIAL_PORT_POST_CODE_LIB_H_
#define _SERIAL_PORT_POST_CODE_LIB_H_


/**
  Initialize the serial device hardware.
  
  @retval RETURN_SUCCESS        The serial device was successfully initialized.
  @retval RETURN_DEVICE_ERROR   The serial device could not be initialized.

**/
RETURN_STATUS
EFIAPI
SerialPortInitialize (
  VOID
  );

/**
  This function writes the PostCode string "<<Post Code>>: XX" to the serial port.

  @param [in] PostCode   The PostCode value of the PostCode string "<<Post Code>>: XX"

**/
VOID
SerialPortPostCode (
  IN UINTN PostCode
  );

#endif 
