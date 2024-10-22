/** @file
  Header file for VgaPostCodeLib

  This library provides functions to show the PostCode on the monitor screen
  It uses GOP frame buffer address to implement functions.

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

#ifndef _VGA_POST_CODE_LIB_H_
#define _VGA_POST_CODE_LIB_H_

#include <Uefi.h>
#include <Protocol/GraphicsOutput.h>

/**
 This function uses GOP frame buffer to show the PostCode string
 on the lower right corner of screen.

 @param [in] GraphicsOutput   The GOP interface
 @param [in] PostCode         The PostCode value of the PostCode string
 
 @retval EFI_SUCCESS   PostCode string was successfully shown
 @return Others        Show string error

**/
EFI_STATUS
EFIAPI
VgaShowPostCode (
  EFI_GRAPHICS_OUTPUT_PROTOCOL *GraphicsOutput,
  UINTN                        PostCode
  );

/**
 This function uses GOP frame buffer to clean the PostCode string 
 which was printed on the lower right corner of screen.

 @param [in] GraphicsOutput   The GOP interface

 @retval EFI_SUCCESS   PostCode string was successfully cleaned
 @return Others        Show string error

**/
EFI_STATUS
EFIAPI
VgaClearPostCode (
  EFI_GRAPHICS_OUTPUT_PROTOCOL *GraphicsOutput
  );

#endif 
