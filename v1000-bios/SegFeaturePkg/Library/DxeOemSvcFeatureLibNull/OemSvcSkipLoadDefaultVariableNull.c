/** @file
  Oem Svc Skip LoadDefault Variable

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


#include <Library/DxeOemSvcFeatureLib.h>

EFI_STATUS
OemSvcSkipLoadDefaultVariable (
  IN CHAR16      *VarName,
  IN EFI_GUID    *VarGuid,
  IN OUT VOID    *DefaultData  
  )
{
  /*++
    Todo:
      Add project specific code in here.
  --*/

  return EFI_UNSUPPORTED;
}