/** @file

  Code for Analysis phase.

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

#include "Cpu.h"
#include "Feature.h"
#include "ProgramBeforeReset.h"

/**
  Configures Processor Feature List for all processors.

  This function configures Processor Feature List for all processors.

**/
VOID
ConfigProcessorFeatureList (
  VOID
  )
{
  //
  // Configure Feature List for Machine Check
  //
  if (FeaturePcdGet (PcdCpuMachineCheckFlag)) {
    MachineCheckConfigFeatureList ();
  }
  //
  // Config Feature List for LBR feature.
  //
  if (FeaturePcdGet (PcdCpuLbrFlag)) {
    LbrConfigFeatureList ();
  }
}

/**
  Produces Register Tables for all processors.

  This function produces Register Tables for all processors.

**/
VOID
ProduceRegisterTable (
  VOID
  )
{

  UINTN          ProcessorNumber;
  UINT8          Index;
  CPU_FEATURE_ID FeatureID;
  VOID           *Attribute;

  //
  // Parse Processor Feature Lists and translate to Register Tables for all processors.
  //
  for (ProcessorNumber = 0; ProcessorNumber < mCpuConfigConextBuffer.NumberOfProcessors; ProcessorNumber++) {
    
    //
    // Traverse Processor Feature List for this logical processor.
    //
    Index = 1;
    FeatureID = GetProcessorFeatureEntry (ProcessorNumber, Index, &Attribute);
    Index++;
    while (FeatureID != CpuFeatureMaximum) {

      switch (FeatureID) {

      case MachineCheck:
        if (FeaturePcdGet (PcdCpuMachineCheckFlag)) {
          MachineCheckReg (ProcessorNumber, Attribute);
        }
        break;

      case Lbr:
        if (FeaturePcdGet (PcdCpuLbrFlag)) {
          LbrReg (ProcessorNumber, Attribute);
        }
        break;

      default:
        break;
      }

      FeatureID = GetProcessorFeatureEntry (ProcessorNumber, Index, &Attribute);
      Index++;
    }
  }
}

/**
  Produces Pre-SMM-Init Register Tables for all processors.

  This function produces Pre-SMM-Init Register Tables for all processors.

**/
VOID
ProducePreSmmInitRegisterTable (
  VOID
  )
{
  UINTN  ProcessorNumber;
  UINT32 ApicId;

  for (ProcessorNumber = 0; ProcessorNumber < mCpuConfigConextBuffer.NumberOfProcessors; ProcessorNumber++) {
    ApicId = mCpuConfigConextBuffer.CollectedDataBuffer[ProcessorNumber].CpuMiscData.InitialApicID;
    mCpuConfigConextBuffer.PreSmmInitRegisterTable[ProcessorNumber].InitialApicId = ApicId;

    //
    // Microcode update must be first item in the register table.
    //
    if (FeaturePcdGet (PcdCpuMicrocodeUpdateFlag)) {
      MicrocodeReg (ProcessorNumber, TRUE);
    }
  }
}

/**
  Produces register table according to output of Data Collection phase.
  
  This function produces register table according to output of Data Collection phase.

**/
VOID
AnalysisPhase (
  VOID
  )
{
  UINTN                     Index;
  UINTN                     ProcessorNumber;
  CPU_REGISTER_TABLE        *RegisterTable;
  CPU_REGISTER_TABLE_ENTRY  *RegisterTableEntry;
  UINT8                     CallbackSignalValue;

  //
  // Set PcdCpuCallbackSignal to trigger callback function, and reads the value back.
  //
  CallbackSignalValue = SetAndReadCpuCallbackSignal (CPU_PROCESSOR_FEATURE_LIST_CONFIG_SIGNAL);
  //
  // Checks whether the callback function requests to bypass Processor Feature List configuration.
  //
  if (CallbackSignalValue != CPU_BYPASS_SIGNAL) {
    //
    // Configure Processor Feature List for all logical processors.
    //
    ConfigProcessorFeatureList ();
  }

  //
  // Write programming for CPU-only reset at the start of register table
  //
  for (ProcessorNumber = 0; ProcessorNumber < mCpuConfigConextBuffer.NumberOfProcessors; ProcessorNumber++) {
    ProgramBeforeReset (ProcessorNumber);
  }

  //
  // Set PcdCpuCallbackSignal to trigger callback function, and reads the value back.
  //
  CallbackSignalValue = SetAndReadCpuCallbackSignal (CPU_REGISTER_TABLE_TRANSLATION_SIGNAL);
  //
  // Checks whether the callback function requests to bypass Register Table translation.
  //
  if (CallbackSignalValue != CPU_BYPASS_SIGNAL) {
    //
    // Produce register tables for all logical processors.
    //
    ProduceRegisterTable ();
  }

  //
  // Debug information
  //
  for (ProcessorNumber = 0; ProcessorNumber < mCpuConfigConextBuffer.NumberOfProcessors; ProcessorNumber++) {

    RegisterTable = &mCpuConfigConextBuffer.RegisterTable[ProcessorNumber];
    for (Index = 0; Index < RegisterTable->TableLength; Index++) {
      RegisterTableEntry = &RegisterTable->RegisterTableEntry[Index];
      DEBUG ((EFI_D_ERROR, "Processor: %d: MSR: %x, Bit Start: %d, Bit Length: %d, Value: %lx\r\n", ProcessorNumber, RegisterTableEntry->Index, RegisterTableEntry->ValidBitStart, RegisterTableEntry->ValidBitLength, RegisterTableEntry->Value));
    }
  }
  DEBUG ((EFI_D_ERROR, "Capability: %8x\r\n", PcdGet32 (PcdCpuProcessorFeatureCapability)));
  DEBUG ((EFI_D_ERROR, "Configuration: %8x\r\n", PcdGet32 (PcdCpuProcessorFeatureUserConfiguration)));
  DEBUG ((EFI_D_ERROR, "Setting: %8x\r\n", PcdGet32 (PcdCpuProcessorFeatureSetting)));
}
