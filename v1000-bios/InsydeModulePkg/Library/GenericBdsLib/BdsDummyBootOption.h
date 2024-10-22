/** @file
  Header file of dummy boot option related functions

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
#ifndef _BDS_DUMMY_BOOT_OPTION_H_
#define _BDS_DUMMY_BOOT_OPTION_H_

#include "InternalBdsLib.h"

EFI_STATUS
CreateDummyBootOptions (
  VOID
  );

BOOLEAN
IsNewPositionDeterminedByVirtualBootOrder (
  IN UINT16                     OptionNum
  );

BOOLEAN
IsVirtualBootOrder (
  VOID
  );

EFI_STATUS
GetPositionByVirtualBootOrder (
  IN  UINT16                    OptionNum,
  IN  UINT16                    *BootOrder,
  IN  UINTN                     BootOrderSize,
  OUT UINTN                     *NewPosition
  );

#endif
