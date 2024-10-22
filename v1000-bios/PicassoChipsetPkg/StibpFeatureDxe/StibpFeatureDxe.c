/** @file
  According SCU setting to Enable/Disable STIBP setting.

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

#include "StibpFeatureDxe.h"

/**
 Enable STIBP feature.
 
 @retval EFI_SUCCESS              Function complete successfully.
 @retval other                    Some error occurs when executing this entry point.
*/
VOID
EFIAPI
EnableStibp (
  VOID
  )
{
  AsmMsrOr64 (0xC001102E, (BIT28 | BIT26 | BIT15)); 
}

/**
 This function will enable each APs STIBP feature.
 
 @retval EFI_SUCCESS              Function complete successfully.
 @retval other                    Some error occurs when executing this entry point.
*/
EFI_STATUS
EFIAPI
EnableAllApsMsrStibpSet (
  VOID
  )
{
  EFI_STATUS                Status;
  EFI_MP_SERVICES_PROTOCOL  *MpServices;
  
  Status     = EFI_SUCCESS;
  MpServices = NULL;

  Status = gBS->LocateProtocol (
                  &gEfiMpServiceProtocolGuid,
                  NULL,
                  (VOID**)&MpServices
                  );
  if (EFI_ERROR (Status)) {
    DEBUG (((DEBUG_ERROR | DEBUG_INFO), "!!! Locate gEfiMpServiceProtocolGuid ERROR(%r) !!!\n", Status));
    return Status;
  }

  //
  // Set each Aps STIBP bits.
  //
  Status = MpServices->StartupAllAPs (
                         MpServices,
                         (EFI_AP_PROCEDURE) EnableStibp,
                         FALSE,
                         NULL,
                         0,
                         NULL,
                         NULL
                         );
  if (EFI_ERROR (Status)) {
    DEBUG (((DEBUG_ERROR | DEBUG_INFO), "!!! StartupAllAPs ERROR(%r) !!!\n", Status));
    return Status;
  }

  return Status;
}

/**
 The user Entry Point for Application. The user code starts with this function
 as the real entry point for the application
 
 @param[in] ImageHandle           The firmware allocated handle for the EFI image.
 @param[in] SystemTable           A pointer to the EFI System Table.
 
 @retval EFI_SUCCESS              Function complete successfully.
 @retval other                    Some error occurs when executing this entry point.
*/
EFI_STATUS
EFIAPI
StibpFeatureDxeEntryPoint (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS             Status;
  CHIPSET_CONFIGURATION  ChipsetConfig;

  Status = EFI_SUCCESS;
  
  Status = GetChipsetConfiguration (&ChipsetConfig);
  if (EFI_ERROR (Status)) {
    DEBUG (((DEBUG_ERROR | DEBUG_INFO), "!!! GetChipsetConfiguration ERROR(%r) !!!\n", Status));
    return Status;
  }
    
  //
  // According the STIBP option setting in SUC to determine whether enalbe all AP's STIBP feature.
  //
  if (ChipsetConfig.StibpStatus) {
    Status = EnableAllApsMsrStibpSet ();
    if (EFI_ERROR (Status)) {
      DEBUG (((DEBUG_ERROR | DEBUG_INFO), "!!! EnableAllApStibpSet ERROR(%r) !!!\n", Status));
      return Status;
    }
  }

  return Status;
}



