/** @file
  Implementation of SmmControlPei module.

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

#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <Library/IoLib.h>

#include <Ppi/SmmControl.h>

#include <Fch.h>

/**
  Trigger the software SMI

  @param  Data                  The value to be set on the software SMI data port

  @retval EFI_SUCCESS           Function completes successfully

**/
EFI_STATUS
EFIAPI
SmmTrigger (
  IN UINT8    *Data
  )
{
  UINT16  *OutputData;
  UINT32  OutputPort;
  UINT32  Value32 = 0;

  //
  // Enable command port SMI (Set SmiReg:0xB2[7:6] = 2b'01')
  //
  Value32 = MmioRead32 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REGB2);
  Value32 &= ~(BIT6 + BIT7);
  Value32 |= BIT6;
  MmioWrite32 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REGB2, Value32);

  //
  // Clear SmiEnB (SmiReg:0x98[31]) to enable SMI function
  //
  Value32 = MmioRead32 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REG98);
  Value32 &= ~BIT31;
  MmioWrite32 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REG98, Value32);

  OutputPort = PcdGet16 (PcdSoftwareSmiPort);
  OutputData = (UINT16 *) Data;

  //
  // Generate the APMC SMI
  //
  IoWrite16 (
    (UINTN)  OutputPort,
    (UINT16) *OutputData
    );

  return EFI_SUCCESS;
}

/**
  Clear the SMI status

  @param  None

  @retval EFI_SUCCESS           The function completes successfully
  @retval EFI_DEVICE_ERROR      Something error occurred

**/
EFI_STATUS
EFIAPI
SmmClear (
  VOID
  )
{
  UINT32      Value32 = 0;

  //
  // Clear SMI CMD Port Status Bit (Write 1 to SmiReg:0x88[11])
  //
  Value32 = MmioRead32 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REG88);
  Value32 &= (UINT32)-1;
  Value32 |= BIT11;
  MmioWrite32 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REG88, Value32);

  //
  // Set the EOS Bit (SmiReg:0x98[28] = 1)
  //
  Value32 = MmioRead32 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REG98);
  Value32 &= (UINT32)-1;
  Value32 |= BIT28;
  MmioWrite32 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REG98, Value32);

  return EFI_SUCCESS;
}

/**
  Invokes SMI activation from either the preboot or runtime environment.

  @param  PeiServices           General purpose services available to every PEIM.
  @param  This                  The PEI_SMM_CONTROL_PPI instance.
  @param  ArgumentBuffer        The optional sized data to pass into the protocol activation.
  @param  ArgumentBufferSize    The optional size of the data.
  @param  Periodic              An optional mechanism to periodically repeat activation.
  @param  ActivationInterval    An optional parameter to repeat at this period one
                                time or, if the Periodic Boolean is set, periodically.

  @retval EFI_SUCCESS           The SMI/PMI has been engendered.
  @retval EFI_DEVICE_ERROR      The timing is unsupported.
  @retval EFI_INVALID_PARAMETER The activation period is unsupported.
  @retval EFI_NOT_STARTED       The SMM base service has not been initialized.

**/
EFI_STATUS
EFIAPI
PeiActivate (
  IN      EFI_PEI_SERVICES           **PeiServices,
  IN      PEI_SMM_CONTROL_PPI        *This,
  IN OUT  INT8                       *ArgumentBuffer OPTIONAL,
  IN OUT  UINTN                      *ArgumentBufferSize OPTIONAL,
  IN      BOOLEAN                    Periodic OPTIONAL,
  IN      UINTN                      ActivationInterval OPTIONAL
  )
{
  EFI_STATUS  Status;
  UINT8       Data[2] = {0xFF, 0xFF};

  if (Periodic) {
    DEBUG ((EFI_D_WARN, "Invalid parameter\n"));
    return EFI_INVALID_PARAMETER;
  }

  if (ArgumentBuffer == NULL) {
    Data[0] = 0xFF;
    Data[1] = 0xFF;
  } else {
    if (ArgumentBufferSize == NULL || *ArgumentBufferSize > 2) {
      return EFI_INVALID_PARAMETER;
    }
    CopyMem (Data, ArgumentBuffer, *ArgumentBufferSize );
  }

  //
  // Clear any pending the APM SMI
  //
  Status = SmmClear ();
  if (EFI_ERROR (Status)) {
    return Status;
  }

  return SmmTrigger (Data);
}

/**
  Clears any system state that was created in response to the Active call.

  @param  PeiServices           General purpose services available to every PEIM.
  @param  This                  The PEI_SMM_CONTROL_PPI instance.
  @param  Periodic              Optional parameter to repeat at this period one 
                                time or, if the Periodic Boolean is set, periodically.

  @retval EFI_SUCCESS           The SMI/PMI has been engendered.
  @retval EFI_DEVICE_ERROR      The source could not be cleared.
  @retval EFI_INVALID_PARAMETER The service did not support the Periodic input argument.

**/
EFI_STATUS
EFIAPI
PeiDeactivate (
  IN EFI_PEI_SERVICES             **PeiServices,
  IN PEI_SMM_CONTROL_PPI          *This,
  IN BOOLEAN                      Periodic OPTIONAL
  )
{
  if (Periodic) {
    return EFI_INVALID_PARAMETER;
  }

  return SmmClear ();
}

//
// Globals
//
STATIC PEI_SMM_CONTROL_PPI mSmmControlPpi = {
  PeiActivate,
  PeiDeactivate
};

STATIC EFI_PEI_PPI_DESCRIPTOR mPpiList = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gPeiSmmControlPpiGuid,
  &mSmmControlPpi
};

/**
  Main entry for the SMM Control PEIM.

  This routine is to install PEI_SMM_CONTROL_PPI.

  @param  FileHandle            Handle of the file being invoked.
  @param  PeiServices           Pointer to PEI Services table.

  @retval EFI_SUCCESS           Returns EFI_SUCCESS by default.

**/
EFI_STATUS
EFIAPI
SmmControlPeiInitialize (
  IN EFI_PEI_FILE_HANDLE        FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
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
    //
    // Install EFI SMM Control PPI
    //
    Status = (**PeiServices).InstallPpi (PeiServices, &mPpiList);
    ASSERT_EFI_ERROR (Status);
  }

  return EFI_SUCCESS;
}
