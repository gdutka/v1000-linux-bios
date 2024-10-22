/** @file
  16550 UART Serial Port library functions

;******************************************************************************
;* Copyright (c) 2019, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/
#include <Uefi.h>
#include <Library/H2OSerialConfigLib.h>

/**
  This function returns serial device information, based on the usage.

  @param  SerialDevice      On entry, this points to the default serial device information
                            about the serial device as discovered by the caller.
                            On exit, this points to the updated serial device information.

  @retval EFI_UNSUPPORTED   Caller can us the current contents of the returned structure to initialize the device.
  @retval EFI_MEDIA_CHANGED Caller can us the current contents of the returned structure to initialize the device.
  @retval EFI_SUCCESS       Caller don't do any serial port initialization because the output is disabled,
                            or the device is not present.

**/
RETURN_STATUS
EFIAPI
H2OGetSerialDeviceConfig (
  IN OUT H2O_SERIAL_DEVICE *SerialDevice
  )
{
  return EFI_UNSUPPORTED;
}



/**
  This function returns serial attribute information, based on the usage.

  @param  SerialDevice      On entry, this points to the default serial attribute information
                            about the serial device as discovered by the caller.
                            On exit, this points to the updated serial attribute information.

  @retval EFI_UNSUPPORTED   Caller can us the current contents of the returned structure to initialize the device.
  @retval EFI_MEDIA_CHANGED Caller can us the current contents of the returned structure to initialize the device.
  @retval EFI_SUCCESS       Caller don't do any serial port initialization because the output is disabled,
                            or the device is not present.

**/
RETURN_STATUS
EFIAPI
H2OGetSerialAttribsConfig (
  IN OUT H2O_SERIAL_ATTRIBS *SerialAttribs
  )
{
  return EFI_UNSUPPORTED;
}

