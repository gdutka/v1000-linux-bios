/** @file

  Vga support protocol definition

;******************************************************************************
;* Copyright (c) 2016 - 2017, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _VGA_SUPPORT_H_
#define _VGA_SUPPORT_H_

#include <Protocol/GraphicsOutput.h>

typedef struct {
  BOOLEAN                      *GopInstalled;
  EFI_GRAPHICS_OUTPUT_PROTOCOL **GraphicsOutput;
} VGA_SUPPORT_PROTOCOL;

extern EFI_GUID gH2OPostCodeVgaProtocolGuid;

#endif
