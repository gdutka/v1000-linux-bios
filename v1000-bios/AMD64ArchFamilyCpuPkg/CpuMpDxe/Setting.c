/** @file

  Code for Setting phase.

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
#include "MpService.h"

BOOLEAN    mRestoreSettingAfterInit = FALSE;
BOOLEAN    mSetBeforeCpuOnlyReset;

/**
  Programs registers for the calling processor.

  This function programs registers for the calling processor.

  @param  PreSmmInit         Specify the target register table.
                             If TRUE, the target is the pre-SMM-init register table.
                             If FALSE, the target is the post-SMM-init register table.
  @param  ProcessorNumber    Handle number of specified logical processor.

**/
VOID
SetProcessorRegisterEx (
  IN BOOLEAN  PreSmmInit,
  IN UINTN    ProcessorNumber
  )
{
  CPU_REGISTER_TABLE        *RegisterTable;
  CPU_REGISTER_TABLE_ENTRY  *RegisterTableEntry;
  UINTN                     Index;
  UINTN                     Value;
  UINTN                     StartIndex;
  UINTN                     EndIndex;

  if (PreSmmInit) {
    RegisterTable = &mCpuConfigConextBuffer.PreSmmInitRegisterTable[ProcessorNumber];
  } else {
    RegisterTable = &mCpuConfigConextBuffer.RegisterTable[ProcessorNumber];
  }
  
  //
  // If microcode patch has been applied, then the first register table entry
  // is for microcode upate, so it is skipped.  
  //
  StartIndex = 0;
  if (FeaturePcdGet (PcdCpuMicrocodeUpdateFlag)) {
    if (GET_CPU_MISC_DATA (ProcessorNumber, MicrocodeRevision) != 0) {
      RegisterTableEntry = &RegisterTable->RegisterTableEntry[0];
      if (RegisterTableEntry->RegisterType == Msr && RegisterTableEntry->Index == EFI_MSR_IA32_BIOS_UPDT_TRIG) {
        StartIndex = 1;
      }
    }
  }

  if (mSetBeforeCpuOnlyReset) {
    EndIndex    = StartIndex + RegisterTable->NumberBeforeReset;
  } else {
    StartIndex += RegisterTable->NumberBeforeReset;
    EndIndex    = RegisterTable->TableLength;
  }

  //
  // Traverse Register Table of this logical processor
  //
  for (Index = StartIndex; Index < EndIndex; Index++) {

    RegisterTableEntry = &RegisterTable->RegisterTableEntry[Index];
    
    //
    // Check the type of specified register
    //
    switch (RegisterTableEntry->RegisterType) {
    //
    // The specified register is Control Register
    //
    case ControlRegister:
      switch (RegisterTableEntry->Index) {
      case 0:
        Value = AsmReadCr0 ();
        Value = (UINTN) BitFieldWrite64 (
                          Value,
                          RegisterTableEntry->ValidBitStart,
                          RegisterTableEntry->ValidBitStart + RegisterTableEntry->ValidBitLength - 1,
                          RegisterTableEntry->Value
                          );
        AsmWriteCr0 (Value);
        break;
      case 2:
        Value = AsmReadCr2 ();
        Value = (UINTN) BitFieldWrite64 (
                          Value,
                          RegisterTableEntry->ValidBitStart,
                          RegisterTableEntry->ValidBitStart + RegisterTableEntry->ValidBitLength - 1,
                          RegisterTableEntry->Value
                          );
        AsmWriteCr2 (Value);
        break;
      case 3:
        Value = AsmReadCr3 ();
        Value = (UINTN) BitFieldWrite64 (
                          Value,
                          RegisterTableEntry->ValidBitStart,
                          RegisterTableEntry->ValidBitStart + RegisterTableEntry->ValidBitLength - 1,
                          RegisterTableEntry->Value
                          );
        AsmWriteCr3 (Value);
        break;
      case 4:
        Value = AsmReadCr4 ();
        Value = (UINTN) BitFieldWrite64 (
                          Value,
                          RegisterTableEntry->ValidBitStart,
                          RegisterTableEntry->ValidBitStart + RegisterTableEntry->ValidBitLength - 1,
                          RegisterTableEntry->Value
                          );
        AsmWriteCr4 (Value);
        break;
      case 8:
        //
        //  Do we need to support CR8?
        //
        break;
      default:
        break;
      }
      break;
    //
    // The specified register is Model Specific Register
    //
    case Msr:
      //
      // If this function is called to restore register setting after INIT signal,
      // there is no need to restore MSRs in register table.
      //
      if (!mRestoreSettingAfterInit) {
        if (RegisterTableEntry->ValidBitLength >= 64) {
          //
          // If length is not less than 64 bits, then directly write without reading
          //
          AsmWriteMsr64 (
            RegisterTableEntry->Index,
            RegisterTableEntry->Value
            );
        } else {
          //
          // Set the bit section according to bit start and length
          //
          AsmMsrBitFieldWrite64 (
            RegisterTableEntry->Index,
            RegisterTableEntry->ValidBitStart,
            RegisterTableEntry->ValidBitStart + RegisterTableEntry->ValidBitLength - 1,
            RegisterTableEntry->Value
            );
        }
      }
      break;
    //
    // Enable or disable cache
    //
    case CacheControl:
      //
      // If value of the entry is 0, then disable cache.  Otherwise, enable cache.
      //
      if (RegisterTableEntry->Value == 0) {
        AsmDisableCache ();
      } else {
        AsmEnableCache ();
      }
      break;

    default:
      break;
    }
  }
}

/**
  Programs registers after SMM initialization for the calling processor.

  This function programs registers after SMM initialization for the calling processor.

  @param  ProcessorNumber    Handle number of specified logical processor.

**/
VOID
SetProcessorRegister (
  IN UINTN  ProcessorNumber
  )
{
  SetProcessorRegisterEx (FALSE, ProcessorNumber);
}

/**
  Programs registers before SMM initialization for the calling processor.

  This function programs registers before SMM initialization for the calling processor.

  @param  ProcessorNumber    Handle number of specified logical processor.

**/
VOID
SetPreSmmInitProcessorRegister (
  IN UINTN  ProcessorNumber
  )
{
  SetProcessorRegisterEx (TRUE, ProcessorNumber);
}

/**
  Programs processor registers according to register tables.

  This function programs processor registers according to register tables.

**/
VOID
SettingPhase (
  VOID
  )
{
  UINT8                     CallbackSignalValue;
  UINTN                     Index;
  UINTN                     ProcessorNumber;
  CPU_REGISTER_TABLE        *RegisterTable;

  //
  // Set PcdCpuCallbackSignal to trigger callback function, and reads the value back.
  //
  CallbackSignalValue = SetAndReadCpuCallbackSignal (CPU_PROCESSOR_SETTING_SIGNAL);
  //
  // Checks whether the callback function requests to bypass Setting phase.
  //
  if (CallbackSignalValue == CPU_BYPASS_SIGNAL) {
    return;
  }



  //
  // Program processors' registers in sequential mode.
  //
  mSetBeforeCpuOnlyReset = FALSE;
  for (Index = 0; Index < mCpuConfigConextBuffer.NumberOfProcessors; Index++) {
    
    ProcessorNumber = mCpuConfigConextBuffer.SettingSequence[Index];

    if (ProcessorNumber == mCpuConfigConextBuffer.BspNumber) {
      SetProcessorRegister (ProcessorNumber);
    } else {

      DispatchAPAndWait (
        FALSE,
        GET_CPU_MISC_DATA (ProcessorNumber, ApicID),
        SetProcessorRegister
        );
    }

    RegisterTable = &mCpuConfigConextBuffer.RegisterTable[ProcessorNumber];
    RegisterTable->InitialApicId = GetInitialLocalApicId (ProcessorNumber);
  }
  //
  // Set PcdCpuCallbackSignal to trigger callback function
  //
  PcdSet8 (PcdCpuCallbackSignal, CPU_PROCESSOR_SETTING_END_SIGNAL);

  //
  // From now on, SetProcessorRegister() will be called only by SimpleApProcWrapper()
  // and ApProcWrapper to restore register settings after INIT signal, so switch
  // this flag from FALSE to TRUE.
  //
  mRestoreSettingAfterInit = TRUE;
}
