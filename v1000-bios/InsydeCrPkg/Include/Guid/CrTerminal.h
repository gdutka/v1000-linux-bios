/** @file

  GUIDs used for CR Terminal

;******************************************************************************
;* Copyright (c) 2018, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef __CR_TERMINAL_H__
#define __CR_TERMINAL_H__

#define H2O_CR_LOG_TERM_GUID \
  { \
    0xF3895AE6, 0x96BA, 0x4478, 0x83, 0xA9, 0x47, 0x07, 0x8B, 0x6D, 0x81, 0x87 \
  }

#define DEVICE_PATH_MESSAGING_LOG_TERM     H2O_CR_LOG_TERM_GUID

extern EFI_GUID gH2OCrLogTermGuid;
#endif
