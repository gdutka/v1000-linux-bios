/** @file

 This PEIM is to expose the Disable CAR notification.

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

#include <PiPei.h>
#include <Library/DebugLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/AmdCapsuleLib.h>
#include <Ppi/MemoryDiscovered.h>
#include <Ppi/PspSmmResume.h>

/**
  This function is to dismantle the pre-memory cache-as-RAM mode.

  @param  VOID

  @retval VOID
**/
VOID
AmdDisableCarBx (
  );

/**
  A callback function to disable Cache As Ram. 

  This function is a callback function to disable Cache As Ram by calling AMD_DISABLE_STACK

  @param  PeiServices      Pointer to PEI Services Table
  @param  NotifyDescriptor Address if the notification descriptor data structure 
  @param  Ppi              Address of the PPI that was installed

  @retval EFI_SUCCESS      The function completed successfully

**/
EFI_STATUS
EFIAPI
DisableCacheAsRam (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  )
{
//   AsmDisableCache ();
  AmdDisableCarBx ();
  DEBUG (( EFI_D_INFO, "Dismantle the pre-memory cache-as-RAM\r\n"));
  return EFI_SUCCESS;
}

//
// Notification to disable CAR
//
STATIC EFI_PEI_NOTIFY_DESCRIPTOR        mNotifyList[] = {
  {
    (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gEfiPeiMemoryDiscoveredPpiGuid,
    DisableCacheAsRam
  }
};

/**
  The Entry point of the DisableCacheAsRam PEIM
 
  @param  FileHandle  Handle of the file being invoked.
  @param  PeiServices Describes the list of possible PEI Services. 
                          
  @retval EFI_SUCCESS   CachePpi and disable CAR notification is installed
                        successfully.

**/
EFI_STATUS
EFIAPI
DisableCacheAsRamInit (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS            Status;
  PSP_SMM_RESUME_PPI    *PspSmmResumePpi;

  //
  // Check Secure S3 Resume
  //
  Status = PeiServicesLocatePpi (
                            &gPspSmmResumePpiGuid,
                            0,
                            NULL,
                            (VOID **)&PspSmmResumePpi
                            );
  if (!EFI_ERROR (Status) || AmdCapsuleGetStatus ()) {
    return EFI_SUCCESS;
  }

  //
  // Register for PPI Notifications
  //
  Status = PeiServicesNotifyPpi (&mNotifyList[0]);
  ASSERT_EFI_ERROR (Status);
  
  return Status;
}
