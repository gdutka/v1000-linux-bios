/** @file

   H2O Setup Vatiable library headfile.

;******************************************************************************
;* Copyright (c) 2018, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _DXE_SETUP_VARIABLE_LIB_H_
#define _DXE_SETUP_VARIABLE_LIB_H_

#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/HiiLib.h>
#include <Library/VariableLib.h>
#include <Library/PcdLib.h>


EFI_STATUS
LoadVarDefault (
  IN CHAR16          *VarName,
  IN EFI_GUID        *VarGuid,
  IN UINTN           *DataSize,
  IN VOID            *Data
  );


EFI_STATUS
InitVarDefault (
  IN CHAR16          *VarName,
  IN EFI_GUID        *VarGuid,
  IN UINTN           *DataSize,
  IN VOID            *Data
  );

#endif
