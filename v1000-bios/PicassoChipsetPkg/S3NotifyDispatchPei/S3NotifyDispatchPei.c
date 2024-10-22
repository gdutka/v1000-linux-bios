/** @file
  This code supports a the private implementation
  of the S3 Notify Dispatch callback

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corp. All Rights Reserved.
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
#include <Library/PcdLib.h>
#include <Ppi/PostBootScriptTable.h>
#include <Ppi/S3RestoreAcpiCallback.h>
#include <Ppi/SmmControl.h>

EFI_STATUS
EFIAPI
S3BeforeRestoreAcpiCallback (
  IN EFI_PEI_SERVICES              **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR     *NotifyDescriptor,
  IN VOID                          *Ppi
  )
;

EFI_STATUS
EFIAPI
S3AfterRestoreAcpiCallback (
  IN EFI_PEI_SERVICES              **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR     *NotifyDescriptor,
  IN VOID                          *Ppi
  )
;

EFI_PEI_NOTIFY_DESCRIPTOR  mNotifyOnPostScriptList = {
  (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gPeiPostScriptTablePpiGuid,
  S3BeforeRestoreAcpiCallback
};

EFI_PEI_NOTIFY_DESCRIPTOR mNotifyOnS3RestoreAcpiList = {
  (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gPeiS3RestoreAcpiCallbackPpiGuid,
  S3AfterRestoreAcpiCallback
};

/**
  S3 Before Restore Acpi callback to be called when gPeiPostScriptTablePpiGuid is installed.

  @param  PeiServices                 Indirect reference to the PEI Services Table.
  @param  NotifyDescriptor            Address of the notification descriptor data structure. Type
          EFI_PEI_NOTIFY_DESCRIPTOR is defined above.
  @param  Ppi                         Address of the PPI that was installed.

  @retval   EFI_STATUS                Returns EFI_SUCCESS by default.

**/
EFI_STATUS
EFIAPI
S3BeforeRestoreAcpiCallback (
  IN EFI_PEI_SERVICES              **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR     *NotifyDescriptor,
  IN VOID                          *Ppi
  )
{
  EFI_STATUS                       Status;
  PEI_SMM_CONTROL_PPI              *SmmControl;
  UINT16                           SmiCommand;
  UINTN                            Size;

  DEBUG (( EFI_D_INFO, "S3BeforeRestoreAcpi Callback\r\n"));

  //
  // Get needed resource
  //
  Status = PeiServicesLocatePpi (
             &gPeiSmmControlPpiGuid,
             0,
             NULL,
             (VOID **)&SmmControl
             );
  if (EFI_ERROR (Status)) {
    //
    // Never run to here
    //
    CpuDeadLoop();
  }

  //
  // Send command
  // SmiCommand = B1port Data + B0port Data;
  //   B1port Data for connected standby or other driver need,
  //   if don't need use please default 0xFF or SmiCommand use UINT8
  //
  SmiCommand = (0xFF << 8) + PcdGet8 (PcdFchOemBeforePciRestoreSwSmi);
  Size = sizeof (SmiCommand);
  Status = SmmControl->Trigger (
                         PeiServices,
                         SmmControl,
                         (INT8 *)&SmiCommand,
                         &Size,
                         FALSE,
                         0
                         );
  ASSERT_EFI_ERROR (Status);

  return EFI_SUCCESS;
}

/**
  S3 After Restore Acpi callback to be called when gPeiS3RestoreAcpiCallbackPpiGuid is installed.

  @param  PeiServices                 Indirect reference to the PEI Services Table.
  @param  NotifyDescriptor            Address of the notification descriptor data structure. Type
          EFI_PEI_NOTIFY_DESCRIPTOR is defined above.
  @param  Ppi                         Address of the PPI that was installed.

  @retval   EFI_STATUS                Returns EFI_SUCCESS by default.

**/
EFI_STATUS
EFIAPI
S3AfterRestoreAcpiCallback (
  IN EFI_PEI_SERVICES              **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR     *NotifyDescriptor,
  IN VOID                          *Ppi
  )
{
  EFI_STATUS                       Status;
  PEI_SMM_CONTROL_PPI              *SmmControl;
  UINT16                           SmiCommand;
  UINTN                            Size;

  DEBUG (( EFI_D_INFO, "S3AfterRestoreAcpi Callback\r\n"));

  //
  // Get needed resource
  //
  Status = PeiServicesLocatePpi (
             &gPeiSmmControlPpiGuid,
             0,
             NULL,
             (VOID **)&SmmControl
             );
  if (EFI_ERROR (Status)) {
    //
    // Never run to here
    //
    CpuDeadLoop();
  }

  //
  // Send command
  // SmiCommand = B1port Data + B0port Data;
  //   B1port Data for connected standby or other driver need,
  //   if don't need use please default 0xFF or SmiCommand use UINT8.
  //

  SmiCommand = (0xFF << 8) + PcdGet8 (PcdFchOemAfterPciRestoreSwSmi);
  Size = sizeof (SmiCommand);
  Status = SmmControl->Trigger (
                         PeiServices,
                         SmmControl,
                         (INT8 *)&SmiCommand,
                         &Size,
                         FALSE,
                         0
                         );
  ASSERT_EFI_ERROR (Status);

  return EFI_SUCCESS;
}

/**
  The Entry point of the S3NotifyDispatch PEIM

  This function is the Entry point of the S3 Notify Dispatch PEIM

  @param[in]  FileHandle   Pointer to image file handle.
  @param[in]  PeiServices  Pointer to PEI Services Table.

  @retval EFI_SUCCESS      Returns EFI_SUCCESS by default.

**/
EFI_STATUS
EFIAPI
S3NotifyDispatchPeiInitialize (
  IN EFI_PEI_FILE_HANDLE     FileHandle,
  IN CONST EFI_PEI_SERVICES  **PeiServices
  )
{
  EFI_STATUS      Status;
  EFI_BOOT_MODE   BootMode;
  
  Status = (*PeiServices)->GetBootMode (PeiServices, &BootMode);

  if (!EFI_ERROR (Status) && (BootMode != BOOT_ON_S3_RESUME)) {
    //
    // Do nothing except S3 Resume
    //
    return EFI_SUCCESS;
  }

  if (!EFI_ERROR (Status) && (BootMode == BOOT_ON_S3_RESUME)) {
    Status = (*PeiServices)->NotifyPpi(PeiServices, &mNotifyOnPostScriptList);
    ASSERT_EFI_ERROR (Status);

    Status = (*PeiServices)->NotifyPpi(PeiServices, &mNotifyOnS3RestoreAcpiList);
    ASSERT_EFI_ERROR (Status);
  }

  return EFI_SUCCESS;
}
