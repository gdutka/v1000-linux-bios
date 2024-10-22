/** @file

;*******************************************************************************
;* Copyright (c) 2013 - 2017, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;*******************************************************************************
*/

#include "OemAcpiPlatform.h"

#include <ChipsetSetupConfig.h>

#include <IndustryStandard/Acpi.h>

#include <Protocol/AcpiSupport.h>

#include <Library/DebugLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>

#include <Library/KernelConfigLib.h>

EFI_ACPI_SUPPORT_PROTOCOL               *mAcpiSupport = NULL;

/**
  CRB ACPI Platform driver installation function.

  @param[in]        ImageHandle         Handle for this drivers loaded image protocol.
  @param[in]        SystemTable         EFI system table.

  @retval           EFI_SUCCESS         The driver installed without error.
                    EFI_ABORTED         The driver encountered an error and could not
                                        complete installation of the ACPI tables.
*/
EFI_STATUS
InstallOemAcpiPlatform (
  IN EFI_HANDLE                         ImageHandle,
  IN EFI_SYSTEM_TABLE                   *SystemTable
  )
{
  EFI_STATUS                            Status;
  EFI_ACPI_TABLE_VERSION                Version;
  KERNEL_CONFIGURATION                  KernelSetup;

  //
  // Get SYSTEM_CONFIGURATION
  //
  Status = GetKernelConfiguration (&KernelSetup);
  if (EFI_ERROR (Status)) {
    return Status;
  }
	
  if (PcdGetBool(PcdCrbOnlySupport)) {
    Status = CrbUpdateOemDsdt ();
    if (EFI_ERROR(Status)) {
      DEBUG ( ( EFI_D_ERROR, "CRB Update DSDT failed, Status : %r\n", Status ) );
      return Status;
    }
  }

  Status = UpdateOemTableID ();
  if (EFI_ERROR(Status)) {
    return Status;
  }
  
  //
  // By default, a table belongs in all ACPI table versions published.
  //
  Version = EFI_ACPI_TABLE_VERSION_1_0B | EFI_ACPI_TABLE_VERSION_2_0 | EFI_ACPI_TABLE_VERSION_3_0;
  if (KernelSetup.AcpiVer == 2) {
    Version |= EFI_ACPI_TABLE_VERSION_4_0;
  }
	
  return mAcpiSupport->PublishTables (mAcpiSupport, Version);
}

