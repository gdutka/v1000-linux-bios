/** @file
  
;******************************************************************************
;* Copyright (c)  2022, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <Uefi.h>
#include <PiPei.h>

/**
  Main entry for this module.
  This module is a dummy module and is designed to call constructor of PeiFlashRegionLib.

  @param FileHandle             Handle of the file being invoked.
  @param PeiServices            Pointer to PEI Services table.

  @return Status from PeiServicesNotifyPpi.

**/
EFI_STATUS
EFIAPI
FdmInitPeiEntry (
  IN EFI_PEI_FILE_HANDLE        FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  return EFI_SUCCESS;
}
