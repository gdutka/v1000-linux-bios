/** @file
  The OemService creates the firmware volume hob, stores the base address 
  and size of PEI firmware volume The base address corresponds to the parameter bootmode.

;******************************************************************************
;* Copyright (c) 2012 - 2019, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <Library/PeiOemSvcKernelLib.h>

/**
  The OemService creates the firmware volume hob, install FirmwareVolumeInfo PPI,  stores the base address 
  and size of PEI firmware volume The base address corresponds to the parameter bootmode.

  @param  *BootMode             Point to EFI_BOOT_MODE.

  @retval EFI_UNSUPPORTED       Returns unsupported by default.
  @retval EFI_SUCCESS           Store firmware volume info success.
  @retval Others                Create firmware volume hob failed.
**/
EFI_STATUS
OemSvcBootModeCreateFv (
  IN  EFI_BOOT_MODE                     BootMode
  )
{
  /*++
    Todo:
      Add project specific code in here.

      Because PeiCore does not also install FvInfo Ppi for firmware volume hob after edk-stable201905 update.
      You should also install FvInfo Ppi for PeiMain to find these FVs. Especially DxeCore FVs and recovery FVs. 

  --*/

  return EFI_UNSUPPORTED;
}
