/** @file
  Bbs type table definition

;******************************************************************************
;* Copyright (c) 2012 - 2018, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _BBS_TYPE_TABLE_H_
#define _BBS_TYPE_TABLE_H_

#include "SetupUtilityLibCommon.h"
#include "BootDevInfo.h"

BBS_TYPE_TABLE *
GetBbsTypeTableByDevType (
  IN UINT8                                  DevType
  );

BBS_TYPE_TABLE *
GetBbsTypeTableByBootOptionNum (
  IN UINT16                                 BootOptionNum
  );

EFI_STATUS
GetLegacyBootDevType (
  OUT UINT16                                *LegacyBootDevTypeCount,
  OUT UINT16                                **LegacyBootDevType
  );

EFI_STATUS
InitBbsTypeTable (
  VOID
  );

VOID
ShutdownBbsTypeTable (
  VOID
  );

EFI_STATUS
SortBbsTypeTable (
  IN UINT16                                 *BootOrder,
  IN UINT16                                 BootDevNum
  );

extern BBS_TYPE_TABLE          mBbsTypeTable[];
extern UINT32                  mBbsTypeTableCount;
#endif
