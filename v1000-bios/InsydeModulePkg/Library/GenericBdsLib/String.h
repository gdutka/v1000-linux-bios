/** @file
  GenericBdsLib

;******************************************************************************
;* Copyright (c) 2012 - 2017, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

/**
  String support

Copyright (c) 2010, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _STRING_H_
#define _STRING_H_

#include "InternalBdsLib.h"
#include <Library/HiiLib.h>
#include <Library/DebugLib.h>
#include <Library/DevicePathLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BvdtLib.h>

extern EFI_HII_HANDLE gBdsLibStringPackHandle;

//
// This is the VFR compiler generated header file which defines the
// string identifiers.
//

extern UINT8  GenericBdsLibStrings[];

/**
  Get string by string id from HII Interface


  @param Id              String ID.

  @retval  CHAR16 *  String from ID.
  @retval  NULL      If error occurs.

**/
CHAR16 *
BdsLibGetStringById (
  IN  EFI_STRING_ID   Id
  );

INTN
BdsLibStriCmp (
  IN CHAR16                          *Str1,
  IN CHAR16                          *Str2
  );

#endif // _STRING_H_
