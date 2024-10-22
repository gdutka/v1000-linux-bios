/** @file
 Library Instance for SmmLib Library Class.

***************************************************************************
* Copyright (c) 2013, Insyde Software Corp. All Rights Reserved.
*
* You may not reproduce, distribute, publish, display, perform, modify, adapt,
* transmit, broadcast, present, recite, release, license or otherwise exploit
* any part of this publication in any form, by any means, without the prior
* written permission of Insyde Software Corporation.
*
******************************************************************************
*/

#include "FchSmmLibInternal.h"


/**
  Triggers a run time or boot time SMI.  

  This function triggers a software SMM interrupt and set the APMC status with an 8-bit Data.

  @param  Data                 The value to set the APMC status.

**/
VOID
InternalTriggerSmi (
  IN UINT8                     Data
  )
{
  UINT32               SmmControlData32;
  UINT8                bIrqMask;

  //
  // Clear any pending CmdPort SMI
  //
  ClearSmi ();

  //
  // Enable CmdPort SMI
  //
  SmmControlData32 = MmioRead32 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REGB2);
  SmmControlData32 &= ~(BIT7 + BIT6);
  SmmControlData32 |= BIT6;
  MmioWrite32 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REGB2, SmmControlData32);

  //
  // Clear SmiEnB to enable SMI function
  //
  SmmControlData32 = MmioRead32 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REG98);
  SmmControlData32 &= ~BIT31;
  MmioWrite32 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REG98, SmmControlData32);

  // Temporal WA-Ensure IRQ0 is masked off when entering software SMI
  bIrqMask = IoRead8 (FCH_8259_MASK_REG_MASTER);
  if (!(bIrqMask & BIT0)) {
    IoWrite8 (FCH_8259_MASK_REG_MASTER, 0xff);
  }
  //-WA
 
  //
  // Issue command port SMI
  //
  IoWrite8 (PcdGet16 (PcdSmmDataPort), Data);
  IoWrite8 (PcdGet16 (PcdSoftwareSmiPort), PcdGet8 (PcdSmmActivationData));

  // WA-Restore IRQ0 mask if needed
  if (!(bIrqMask & BIT0)) {
    IoWrite8 (FCH_8259_MASK_REG_MASTER, bIrqMask);
  }
  //-WA
}


/**
  Triggers an SMI at boot time.  

  This function triggers a software SMM interrupt at boot time.

**/
VOID
EFIAPI
TriggerBootServiceSoftwareSmi (
  VOID
  )
{
  InternalTriggerSmi (BOOT_SERVICE_SOFTWARE_SMI_DATA);
}


/**
  Triggers an SMI at run time.  

  This function triggers a software SMM interrupt at run time.

**/
VOID
EFIAPI
TriggerRuntimeSoftwareSmi (
  VOID
  )
{
  InternalTriggerSmi (RUNTIME_SOFTWARE_SMI_DATA);
}


/**
  Gets the software SMI data.  

  This function tests if a software SMM interrupt happens. If a software SMI happens, 
  it retrieves the SMM data and returns it as a non-negative value; otherwise a negative
  value is returned. 

  @return Data                 The data retrieved from SMM data port in case of a software SMI;
                               otherwise a negative value.

**/
INTN
InternalGetSwSmiData (
  VOID
  )
{
  UINT32              SmmControlData32;
  UINT8               Data;

  SmmControlData32 = MmioRead32 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REG88);
  if (((SmmControlData32 & BIT11) != 0) &&
       (IoRead8 (PcdGet16 (PcdSoftwareSmiPort)) == PcdGet8 (PcdSmmActivationData))) {
    Data = IoRead8 (PcdGet16 (PcdSmmDataPort));
    return (INTN)(UINTN)Data; 
  } 

  return -1;
}  


/**
  Test if a boot time software SMI happened.  

  This function tests if a software SMM interrupt happened. If a software SMM interrupt happened and
  it was triggered at boot time, it returns TRUE. Otherwise, it returns FALSE.

  @retval TRUE   A software SMI triggered at boot time happened.
  @retval FLASE  No software SMI happened or the software SMI was triggered at run time.

**/
BOOLEAN
EFIAPI
IsBootServiceSoftwareSmi (
  VOID
  )
{
  return (BOOLEAN) (InternalGetSwSmiData () == BOOT_SERVICE_SOFTWARE_SMI_DATA);
}


/**
  Test if a run time software SMI happened.  

  This function tests if a software SMM interrupt happened. If a software SMM interrupt happened and
  it was triggered at run time, it returns TRUE. Otherwise, it returns FALSE.

  @retval TRUE   A software SMI triggered at run time happened.
  @retval FLASE  No software SMI happened or the software SMI was triggered at boot time.

**/
BOOLEAN
EFIAPI
IsRuntimeSoftwareSmi (
  VOID
  )
{
  return (BOOLEAN) (InternalGetSwSmiData () == RUNTIME_SOFTWARE_SMI_DATA);
}


/**

  Clear the SMI Status Bit; Set the EOS bit. 
  
**/
VOID
EFIAPI
ClearSmi (
  VOID
  )
{
  UINT32              SmmControlData32;

  //
  // Clear SmiCmdPort Status Bit
  //
  SmmControlData32 = MmioRead32 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REG88);
  SmmControlData32 |= BIT11;
  MmioWrite32 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REG88, SmmControlData32);


  //
  // Set the EOS Bit
  //
  SmmControlData32 = MmioRead32 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REG98);  
  SmmControlData32 |= BIT28;
  MmioWrite32(ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REG98, SmmControlData32);
}