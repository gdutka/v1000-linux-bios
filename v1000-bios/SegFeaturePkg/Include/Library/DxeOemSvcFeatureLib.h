/** @file
  Definition for Oem Services Default Lib.

;******************************************************************************
;* Copyright (c) 2012 - 2016, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _DXE_OEM_SVC_FEATURE_LIB_H_
#define _DXE_OEM_SVC_FEATURE_LIB_H_

#include <Uefi.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/VariableLib.h>
#include <Library/PcdLib.h>

EFI_STATUS
OemSvcSkipLoadDefaultVariable (
  IN CHAR16      *VarName,
  IN EFI_GUID    *VarGuid,
  IN OUT VOID    *DefaultData  
  );

#endif
