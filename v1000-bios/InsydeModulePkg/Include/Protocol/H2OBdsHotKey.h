/** @file
  This protocol is produced by the H2O BDS hot key registrant. There are may many
  protocol instances installed in system. Each protocol instance must match with one PCD
  of gH2OBdsHotKeyGuid. If someone hot key is pressed, BDS will access corresponding
  protocol instance to perform hot key action.

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

#ifndef __H2O_BDS_HOT_KEY_PROTOCOL_H__
#define __H2O_BDS_HOT_KEY_PROTOCOL_H__

#include <Pi/PiBootMode.h>

typedef struct _H2O_BDS_HOT_KEY_PROTOCOL    H2O_BDS_HOT_KEY_PROTOCOL;

typedef struct _H2O_BDS_CONTEXT {
  UINT32                               Size;
  UINT16                               Reserved;
  UINT16                               Timeout;
  UINT64                               OsIndications;
  UINT64                               OsIndicationsSupported;
  EFI_BOOT_MODE                        BootMode;
  UINT8                                BootType;

  //
  // Members beyond this comment can only be accessed after checking the Size member. For example,
  // if (hdc->Size > OFFSET_OF(H2O_BDS_CONTEXT, field))
  //
} H2O_BDS_CONTEXT;

typedef
EFI_STATUS
(EFIAPI *H2O_BDS_HOT_KEY_LAUNCH) (
  IN H2O_BDS_HOT_KEY_PROTOCOL          *This,
  IN UINT32                            Id,
  IN CONST CHAR8                       *Options OPTIONAL,
  IN H2O_BDS_CONTEXT                   *Context
  );

struct _H2O_BDS_HOT_KEY_PROTOCOL {
  UINT32                               Size;
  H2O_BDS_HOT_KEY_LAUNCH               Launch;
};

#endif
