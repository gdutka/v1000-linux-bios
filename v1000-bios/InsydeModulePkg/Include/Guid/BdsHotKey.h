/** @file
  Definitions for BDS hot key

;******************************************************************************
;* Copyright (c) 2016, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef __BDS_HOT_KEY_GUID_H__
#define __BDS_HOT_KEY_GUID_H__

#define H2O_BDS_HOT_KEY_GUID \
  { \
    0x7949382b, 0x21a9, 0x4bbf, 0xa0, 0x61, 0x58, 0xfc, 0xef, 0xf4, 0x24, 0xc3 \
  }

#define H2O_BDS_HOT_KEY_DESC_GUID \
  { \
    0x42d9df7a, 0xb49d, 0x4fbf, 0x85, 0x0b, 0x0d, 0x35, 0xa5, 0xdd, 0xb2, 0xe3 \
  }

#define H2O_BDS_HOT_KEY_DESC_QUIET_GUID \
  { \
    0x31849028, 0x226b, 0x439c, 0x9f, 0x20, 0xd7, 0xc2, 0x27, 0xcc, 0xc5, 0xdb \
  }

typedef struct _PCD_H2O_HOT_KEY {
  EFI_GUID                        HotKeyProtocolGuid;
  UINT32                          Id;
//CHAR8                           HotKeys[];
//CHAR8                           Options[];
//CHAR8                           DevicePath[];
} PCD_H2O_HOT_KEY;

extern EFI_GUID gH2OBdsHotKeyGuid;
extern EFI_GUID gH2OBdsHotKeyDescGuid;
extern EFI_GUID gH2OBdsHotKeyDescQuietGuid;

#endif
