/** @file

  Code for Machine Check initialization

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

#include "MchkInit.h"

MACHINE_CHECK_ATTRIBUTE MachineCheckAttribute[FixedPcdGet32(PcdCpuMaxLogicalProcessorNumber)];

/**
  Detect capability of machine check for specified processor.
  
  This function detects capability of machine check for specified processor.

  @param  ProcessorNumber   The handle number of specified processor.

**/
VOID
MachineCheckDetect (
  UINTN   ProcessorNumber
  )
{
  EFI_CPUID_REGISTER       *CpuidRegisters;
  UINT32                   RegEdx;
  UINT64                   Value;

  CpuidRegisters = GetProcessorCpuid (ProcessorNumber, EFI_CPUID_VERSION_INFO);
  ASSERT (CpuidRegisters != NULL);
  RegEdx         = CpuidRegisters->RegEdx;

  //
  // Read CPUID(1).EDX[7] for MCE capability
  //
  if (BitFieldRead32 (RegEdx, N_CPUID_MCE_SUPPORT, N_CPUID_MCE_SUPPORT) == 1) { 
    //
    // Read CPUID(1).EDX[14] for MCA capability
    //
    if (BitFieldRead32 (RegEdx, N_CPUID_MCA_SUPPORT, N_CPUID_MCA_SUPPORT) == 1) {
      MachineCheckAttribute[ProcessorNumber].Mca = TRUE;
      //
      // Check MSR IA32_MCG_CAP[8] for the presence of IA32_MCG_CTL MSR
      //
      Value = AsmReadMsr64 (EFI_IA32_MCG_CAP);
      MachineCheckAttribute[ProcessorNumber].ControlMsrPresent = FALSE;
      if (BitFieldRead64 (Value, N_MSR_MCG_CTL_P, N_MSR_MCG_CTL_P) == 1) {
        MachineCheckAttribute[ProcessorNumber].ControlMsrPresent = TRUE;
      }
      MachineCheckAttribute[ProcessorNumber].NumberOfErrorReportingBank = (UINTN) BitFieldRead64 (Value, N_MSR_MCG_COUNT_START, N_MSR_MCG_COUNT_STOP);
    } else {
      MachineCheckAttribute[ProcessorNumber].Mca = FALSE;
    }
    SetProcessorFeatureCapability (ProcessorNumber, MachineCheck, &(MachineCheckAttribute[ProcessorNumber]));
  } else {
    ClearProcessorFeatureCapability (ProcessorNumber, MachineCheck);
  }
}

/**
  Configures Processor Feature Lists for machine check for all processors.
  
  This function configures Processor Feature Lists for machine check for all processors.

**/
VOID
MachineCheckConfigFeatureList (
  VOID
  )
{
  VOID                 *Attribute;
  UINTN                ProcessorNumber;
  BOOLEAN              UserConfigurationSet;

  UserConfigurationSet = FALSE;
  if ((PcdGet32 (PcdCpuProcessorFeatureUserConfiguration) & PCD_CPU_MACHINE_CHECK_BIT) != 0) {
    UserConfigurationSet = TRUE;
  }

  for (ProcessorNumber = 0; ProcessorNumber < mCpuConfigConextBuffer.NumberOfProcessors; ProcessorNumber++) {
    //
    // Check whether this logical processor supports MCE
    //
    if (GetProcessorFeatureCapability (ProcessorNumber, MachineCheck, &Attribute)) {
      if (ProcessorNumber == mCpuConfigConextBuffer.BspNumber) {
        //
        // Set the bit of machine check capability according to BSP capability.
        //
        PcdSet32 (PcdCpuProcessorFeatureCapability, PcdGet32 (PcdCpuProcessorFeatureCapability) | PCD_CPU_MACHINE_CHECK_BIT);
        //
        // Set the bit of machine check setting according to BSP setting
        //
        if (UserConfigurationSet) {
          PcdSet32 (PcdCpuProcessorFeatureSetting, PcdGet32 (PcdCpuProcessorFeatureSetting) | PCD_CPU_MACHINE_CHECK_BIT);
        }
      }
      //
      // Add node into feature list.
      //
      MachineCheckAttribute[ProcessorNumber].EnableMca = UserConfigurationSet;
      AppendProcessorFeatureIntoList (ProcessorNumber, MachineCheck, Attribute);
    }
  }
}

/**
  Produces entry of machine check in Register Table for specified processor.
  
  This function produces entry of machine check in Register Table for specified processor.

  @param  ProcessorNumber   The handle number of specified processor.
  @param  Attribute         Machine check Attribute.

**/
VOID
MachineCheckReg (
  UINTN      ProcessorNumber,
  VOID       *Attribute
  )
{
  MACHINE_CHECK_ATTRIBUTE    *McAttribute;
  UINTN                      Index;
  UINT32                     FamilyId;
  UINT32                     ModelId;
  UINT8                      StartIndex;

  McAttribute = (MACHINE_CHECK_ATTRIBUTE *) Attribute;

  //
  // If user does not enable logging of all errors.
  // Clear all errors in status registers only be performed.
  //
  if (McAttribute->Mca && !McAttribute->EnableMca) {
    //
    // If power-on reset, clear all errors in status registers
    //
    if (PcdGetBool (PcdIsPowerOnReset)) {
      for (Index = 0; Index < McAttribute->NumberOfErrorReportingBank; Index++) {
        WriteRegisterTable (
          ProcessorNumber,
          Msr,
          (UINT32) (EFI_IA32_MC0_STATUS + Index * 4),
          0,
          64,
          0x0
          );
      }
    }
    return ;
  }

  if (McAttribute->Mca && McAttribute->EnableMca) {
    //
    // If IA32_MCG_CTL register is present, enable all MCA features
    //
    if (McAttribute->ControlMsrPresent) {
      WriteRegisterTable (ProcessorNumber, Msr, EFI_IA32_MCG_CTL, 0, 64, 0xffffffffffffffffULL);
    }

    GetProcessorVersionInfo (ProcessorNumber, &FamilyId, &ModelId, NULL, NULL);

    //
    // Enable logging of all errors.
    // For 0x16 Family, exclude IA32_MC0_CTL[8]. Else, include IA32_MC0_CTL register.
    //
    if (FamilyId == 0x16) {
      StartIndex = 1;
      WriteRegisterTable (
        ProcessorNumber,
        Msr,
        (UINT32) (EFI_IA32_MC0_CTL),
        0,
        64,
        0xffffffffffffffffULL & ~(BIT8) //exclude MC0_CTL.SRDEL
      );
    } else {
      StartIndex = 0;
    }
    for (Index = StartIndex; Index < McAttribute->NumberOfErrorReportingBank; Index++) {
      WriteRegisterTable (
        ProcessorNumber,
        Msr,
        (UINT32) (EFI_IA32_MC0_CTL + Index * 4),
        0,
        64,
        0xffffffffffffffffULL
      );
    }

    //
    // If power-on reset, clear all errors in status registers
    //
    if (PcdGetBool (PcdIsPowerOnReset)) {
      for (Index = 0; Index < McAttribute->NumberOfErrorReportingBank; Index++) {
        WriteRegisterTable (
          ProcessorNumber,
          Msr,
          (UINT32) (EFI_IA32_MC0_STATUS + Index * 4),
          0,
          64,
          0x0
          );
      }
    }
  }
  //
  // Set bit 6 of CR4 to enable MCE
  //
  WriteRegisterTable (ProcessorNumber, ControlRegister, 4, 6, 1, 1);
}
