/** @file

  Code for Selecting Least-Feature Processor as BSP

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

#include "MpService.h"
#include "Cpu.h"

//
// Common set of processors' feature flags.
//
UINT32    CommonFeatureFlagSet[6];
//
// Flags of whether corresponding processor matches common set.
//
BOOLEAN   MatchingCommonSet[FixedPcdGet32(PcdCpuMaxLogicalProcessorNumber)];

/**
  Get the common set of processors' feature flags.
  
  This function calculates the common set of processors' feature flags.
  Processor's feature flags include 6 segments: CPUID(1).ECX, CPUID(1).EDX,
  CPUID(0x80000001).EAX, CPUID(0x80000001).EBX, CPUID(0x80000001).ECX, 
  and CPUID(0x80000001).EDX. "AND" them together and result is the common set.

**/
VOID
GetCommonFeatureFlagSet (
  VOID
  )
{
  EFI_CPUID_REGISTER  *CpuidRegisters;
  UINTN               ProcessorNumber;

  //
  // Initialize common set of feature flag with all "1"s.
  //
  SetMem32 (CommonFeatureFlagSet, sizeof (CommonFeatureFlagSet), 0xffffffff);

  //
  // Traverse all processors' feature flags
  //
  for (ProcessorNumber = 0; ProcessorNumber < mCpuConfigConextBuffer.NumberOfProcessors; ProcessorNumber++) {

    //
    // AND CPUID(1).ECX and CPUID(1).EDX with common set
    //
    CpuidRegisters = GetProcessorCpuid (ProcessorNumber, EFI_CPUID_VERSION_INFO);
    ASSERT (CpuidRegisters != NULL);
    CommonFeatureFlagSet[0] &= CpuidRegisters->RegEcx;
    CommonFeatureFlagSet[1] &= CpuidRegisters->RegEdx;

    //
    // AND CPUID(0x80000001).EAX, CPUID(0x80000001).EBX, CPUID(0x80000001).ECX,
    // and CPUID(0x80000001).EDX with common set.
    //
    CpuidRegisters = GetProcessorCpuid (ProcessorNumber, EFI_CPUID_EXTENDED_CPU_SIG);
    ASSERT (CpuidRegisters != NULL);
    CommonFeatureFlagSet[2] &= CpuidRegisters->RegEax;
    CommonFeatureFlagSet[3] &= CpuidRegisters->RegEbx;
    CommonFeatureFlagSet[4] &= CpuidRegisters->RegEcx;
    CommonFeatureFlagSet[5] &= CpuidRegisters->RegEdx;
  }
}

/**
  Checks which processors match the common set of feature flag
  
  This function Checks which processors match the common set of feature flag.

  @retval TRUE    At least one processor matches the common set of feature flag.
  @retval FALSE   No processor matches the common set of feature flag.

**/
BOOLEAN
CompareProcessorsWithCommonSet (
  VOID
  )
{
  EFI_CPUID_REGISTER  *CpuidRegisters;
  UINTN               ProcessorNumber;
  BOOLEAN             ReturnValue;

  ReturnValue = FALSE;
  for (ProcessorNumber = 0; ProcessorNumber < mCpuConfigConextBuffer.NumberOfProcessors; ProcessorNumber++) {
    MatchingCommonSet[ProcessorNumber] = FALSE;
    
    //
    // Compare CPUID(1).ECX and CPUID(1).EDX with common set
    // 
    CpuidRegisters = GetProcessorCpuid (0, EFI_CPUID_VERSION_INFO);
    ASSERT (CpuidRegisters != NULL);
    if (CommonFeatureFlagSet[0] != CpuidRegisters->RegEcx || CommonFeatureFlagSet[1] != CpuidRegisters->RegEdx) {
      continue;
    }

    //
    // Compare CPUID(0x80000001).EAX, CPUID(0x80000001).EBX, CPUID(0x80000001).ECX,
    // and CPUID(0x80000001).EDX with common set.
    //
    CpuidRegisters = GetProcessorCpuid (ProcessorNumber, EFI_CPUID_EXTENDED_CPU_SIG);
    ASSERT (CpuidRegisters != NULL);
    if (CommonFeatureFlagSet[2] != CpuidRegisters->RegEax || CommonFeatureFlagSet[3] != CpuidRegisters->RegEbx ||
        CommonFeatureFlagSet[4] != CpuidRegisters->RegEcx || CommonFeatureFlagSet[5] != CpuidRegisters->RegEdx) {
      continue;
    }

    //
    // If the processor matches common set, flag it, and set return value as TRUE.
    //
    MatchingCommonSet[ProcessorNumber] = TRUE;
    ReturnValue = TRUE;
  }

  return ReturnValue;
}

/**
  Select least-feature processor as BSP
  
  This function selects least-feature processor as BSP.

**/
VOID
SelectLfpAsBsp (
  VOID
  )
{
  UINTN               ProcessorNumber;
  UINTN               OldestProcessorNumber;
  UINT32              OldestProcessorFms[3];
  UINT32              FamilyId;
  UINT32              ModelId;
  UINT32              SteppingId;
  BOOLEAN             MatchingProcessorExist;

  //
  // Calculate the common set of processors' feature flags
  //
  GetCommonFeatureFlagSet ();

  //
  // Compare all logical processors with common set.
  //
  MatchingProcessorExist = CompareProcessorsWithCommonSet ();

  OldestProcessorFms[2] = 0xffffffff;
  OldestProcessorFms[1] = 0;
  OldestProcessorFms[0] = 0;
  OldestProcessorNumber = mCpuConfigConextBuffer.BspNumber;
  for (ProcessorNumber = 0; ProcessorNumber < mCpuConfigConextBuffer.NumberOfProcessors; ProcessorNumber++) {

    //
    // If only 1 processor matches common set, it should be BSP
    // If more than 1 processors match common set, the one with lowest CPUID should be BSP.
    // If no processor matches common set, the processor with lowest CPUID should be BSP.
    //
    if (!MatchingProcessorExist || MatchingCommonSet[ProcessorNumber]) {
      GetProcessorVersionInfo (ProcessorNumber, &FamilyId, &ModelId, &SteppingId, NULL);

      if (FamilyId > OldestProcessorFms[2]) {
        continue;
      }

      if (FamilyId == OldestProcessorFms[2]) {
        if (ModelId > OldestProcessorFms[1]) {
          continue;
        }
        if (ModelId == OldestProcessorFms[1]) {
          if (SteppingId >= OldestProcessorFms[0]) {
            continue;
          }
        }
      }

      OldestProcessorFms[2] = FamilyId;
      OldestProcessorFms[1] = ModelId;
      OldestProcessorFms[0] = SteppingId;
      OldestProcessorNumber = ProcessorNumber;
    }
  }

  //
  // If current BSP is not the least-feature processor, then switch BSP.
  //
  if (OldestProcessorNumber != mCpuConfigConextBuffer.BspNumber) {
    SwitchBSP (&mMpService, OldestProcessorNumber, TRUE);
    DEBUG ((EFI_D_ERROR, "BSP switched to processor: %8d\r\n", OldestProcessorNumber));
  }
}
