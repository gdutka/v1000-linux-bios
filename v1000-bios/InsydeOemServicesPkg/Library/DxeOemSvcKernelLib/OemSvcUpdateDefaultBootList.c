/** @file
  Provide OEM to customize the default boot option list.

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

#include <Library/DxeOemSvcKernelLib.h>

/**
 This function can change the default boot option list prior to it being written out to the BootOrder and Boot#### UEFI
 variables. The LoadOrder, LoadOrderVarName and LoadOrderVarGuid are correct.

 If a load option is removed from the list, then it should be freed using the FreeLoadOption() member of the BDS
 Services protocol.

 @param[in, out] BootList         Pointer to a pointer to the first entry in a linked list that specifies the proposed
                                  default boot option list. Each entry in the list has the format H2O_BDS_LOAD_OPTION.

 @retval EFI_SUCCESS              The function has handled the entire defaulting process and the Boot Manager should not
                                  write the BootOrder and Boot#### variables.
 @retval EFI_UNSUPPORTED          The function is not implemented.
 @retval EFI_MEDIA_CHANGED        The function has updated the default boot option list.
**/
EFI_STATUS
OemSvcUpdateDefaultBootList (
  IN OUT LIST_ENTRY          **BootList
  )
{
  /*++
    Todo:
      Add project specific code in here.
  --*/

  return EFI_UNSUPPORTED;
}

