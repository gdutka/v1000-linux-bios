/** @file
 SMM Chipset Services Library.

 This file is a constructor for SMM chipset services library.

***************************************************************************
* Copyright (c) 2012 - 2019, Insyde Software Corp. All Rights Reserved.
*
* You may not reproduce, distribute, publish, display, perform, modify, adapt,
* transmit, broadcast, present, recite, release, license or otherwise exploit
* any part of this publication in any form, by any means, without the prior
* written permission of Insyde Software Corporation.
*
******************************************************************************
*/

#include <PiSmm.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Protocol/H2OSmmChipsetServices.h>

H2O_SMM_CHIPSET_SERVICES_PROTOCOL *mSmmChipsetSvc = NULL;
STATIC VOID                        *mH2OSmmChipsetServicesProtocolRegistration;

/**
  SMM Chipset Services Protocol notification event handler.

  @param[in]  Protocol      Points to the protocol's unique identifier
  @param[in]  Interface     Points to the interface instance
  @param[in]  Handle        The handle on which the interface was installed

  @retval EFI_SUCCESS       SmmEventCallback runs successfully

 **/
EFI_STATUS
EFIAPI
ChipsetSvcNotification (
  IN CONST EFI_GUID                       *Protocol,
  IN VOID                                 *Interface,
  IN EFI_HANDLE                           Handle
  )
{
  EFI_STATUS       Status;
  Status = gSmst->SmmLocateProtocol (
             &gH2OSmmChipsetServicesProtocolGuid,
             NULL,
             (VOID **)&mSmmChipsetSvc
             );
  ASSERT_EFI_ERROR (Status);

  return Status;
}

/**
 Constructor which locates the H2O SMM Chipset Services protocol. Since this
 protocol is also listed in the dependency expression for this library,
 this should succeed.

 @param[in]         ImageHandle         The firmware allocated handle for the EFI image.
 @param[in]         SystemTable         A pointer to the EFI System Table.

 @retval            EFI_SUCCESS         The operation completed successfully.
 @retval            Others              An unexpected error occurred.
*/

EFI_STATUS
EFIAPI
SmmChipsetSvcLibConstructor (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS                        Status;

  if (gSmst == NULL) {
    //
    // The system is not in SMM mode, with with success status directly
    //
    return EFI_SUCCESS;
  }

  Status = gSmst->SmmLocateProtocol (
             &gH2OSmmChipsetServicesProtocolGuid,
             NULL,
             (VOID **)&mSmmChipsetSvc
             );
  if (EFI_ERROR (Status)) {
    //
    // If H2O Chipset Services is not installed, register Protocol notification for H2O Chipset Services
    //
    Status = gSmst->SmmRegisterProtocolNotify (
                      &gH2OSmmChipsetServicesProtocolGuid,
                      ChipsetSvcNotification,
                      &mH2OSmmChipsetServicesProtocolRegistration
                      );
    ASSERT_EFI_ERROR (Status);
  }

  return EFI_SUCCESS;
}


/**
  If a runtime driver exits with an error, it must call this routine
  to free the allocated resource before the exiting.

  @param[in] ImageHandle    The firmware allocated handle for the EFI image.
  @param[in] SystemTable    A pointer to the EFI System Table.

  @retval  EFI_SUCCESS      The Runtime Driver Lib shutdown successfully.
**/
EFI_STATUS
EFIAPI
SmmChipsetSvcLibDestructor (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  if (gSmst != NULL && mH2OSmmChipsetServicesProtocolRegistration != NULL) {
    gSmst->SmmRegisterProtocolNotify (
             &gH2OSmmChipsetServicesProtocolGuid,
             NULL,
             &mH2OSmmChipsetServicesProtocolRegistration
             );
  }

  return EFI_SUCCESS;
}