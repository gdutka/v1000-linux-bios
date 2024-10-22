/** @file
  StibpFeatureDxe.c header.

;******************************************************************************
;* Copyright (c) 2018, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corp.
;*
;******************************************************************************

*/

#ifndef _STIBP_FEATURE_DXE_H_
#define _STIBP_FEATURE_DXE_H_

#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/ChipsetConfigLib.h>
#include <Library/AMD64ArchLib.h>
#include <Protocol/MpService.h>

#include <Library/DebugLib.h>

/**
 Enable STIBP feature.
 
 @retval EFI_SUCCESS              Function complete successfully.
 @retval other                    Some error occurs when executing this entry point.
*/
VOID
EFIAPI
EnableStibp (
  VOID
  );

/**
 This function will enable each APs STIBP feature.
 
 @retval EFI_SUCCESS              Function complete successfully.
 @retval other                    Some error occurs when executing this entry point.
*/
EFI_STATUS
EFIAPI
EnableAllApsMsrStibpSet (
  VOID
  );

#endif



