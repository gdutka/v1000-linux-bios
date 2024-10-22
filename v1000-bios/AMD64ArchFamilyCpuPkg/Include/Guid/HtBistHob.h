/** @file

  GUID used for HT BIST Status HOB entries in the HOB list.

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

#ifndef _HT_BIST_HOB_GUID_H_
#define _HT_BIST_HOB_GUID_H_

#define EFI_HT_BIST_HOB_GUID \
  { \
    0xbe644001, 0xe7d4, 0x48b1, {0xb0, 0x96, 0x8b, 0xa0, 0x47, 0xbc, 0x7a, 0xe7 } \
  }

extern EFI_GUID gEfiHtBistHobGuid;

#endif
