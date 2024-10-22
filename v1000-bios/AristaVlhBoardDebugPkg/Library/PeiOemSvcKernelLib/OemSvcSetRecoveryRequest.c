/** @file
  This OemService provides OEM to decide the method of recovery request. 
  When DXE loader found that the DXE-core of firmware volume is corrupt, it will force system to restart. 
  This service will be called to set the recovery requests before system restart. 
  To design the recovery requests according to OEM specification.

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <Library/PeiOemSvcKernelLib.h>
#include <Library/IoLib.h>

#define BIOS_RAM_INDEX            0xCD4
#define BIOS_RAM_DATA             0xCD5
#define RECOVERY_REQUEST_START    0x80
#define RECOVERY_REQUEST_END      0x88
#define RECOVERY_REQUEST_VALUE    0x44

/**
  This OemService provides OEM to decide the method of recovery request. 
  When DXE loader found that the DXE-core of firmware volume is corrupt, it will force system to restart. 
  This service will be called to set the recovery requests before system restart. 
  To design the recovery requests according to OEM specification.

  @param  Based on OEM design.

  @retval EFI_UNSUPPORTED      Returns unsupported by default.
  @retval EFI_SUCCESS          The service is customized in the project.
  @retval EFI_MEDIA_CHANGED    The value of IN OUT parameter is changed. 
  @retval Others               Depends on customization.
**/
EFI_STATUS
OemSvcSetRecoveryRequest (
  VOID
  )
{
  UINT8                        Index;

  //
  // For system into Recovery mode
  // Use BIOS_RAM RECOVERY_REQUEST_START~RECOVERY_REQUEST_END, Fill RECOVERY_REQUEST_VALUE in them
  // This is for ATI SB only
  //
  for (Index = RECOVERY_REQUEST_START; Index <= RECOVERY_REQUEST_END; Index++) {
    IoWrite8 (BIOS_RAM_INDEX, Index);
    IoWrite8 (BIOS_RAM_DATA, RECOVERY_REQUEST_VALUE);
  }

  return EFI_SUCCESS;
}

