/** @file
  GenericBdsLib

;******************************************************************************
;* Copyright (c) 2012 - 2016, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

/**
  declares interface functions

Copyright (c) 2004 - 2008, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _EFI_BDS_BBS_SUPPORT_H_
#define _EFI_BDS_BBS_SUPPORT_H_

#include "InternalBdsLib.h"

#define VAR_FLAG  EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE

#pragma pack(1)
///
/// For each legacy boot option in BBS table, a corresponding Boot#### variables is created.
/// The structure saves the mapping relationship between #### and the index in the BBS table.
///
typedef struct {
  UINT16    BootOptionNumber;
  UINT16    BbsIndex;
  UINT16    BbsType;
} BOOT_OPTION_BBS_MAPPING;
#pragma pack()

#endif
