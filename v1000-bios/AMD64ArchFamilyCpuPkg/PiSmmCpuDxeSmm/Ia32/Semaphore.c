/** @file
  Semaphore mechanism to indicate to the BSP that an AP has exited SMM
  after SMBASE relocation.

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

#include "PiSmmCpuDxeSmm.h"

UINTN   mSmmRelocationOriginalAddress;
BOOLEAN *mRebasedFlag;

/**
  Hook return address of SMM Save State so that semaphore code
  can be executed immediately after AP exits SMM to indicate to
  the BSP that an AP has exited SMM after SMBASE relocation.

  @param RebasedFlag  Pointer to a flag. AP sets it to be TRUE after exit of SMM.
  @param CpuState     Pointer to the SMM Save State Map.
**/
VOID
SemaphoreHook (
  IN BOOLEAN                       *RebasedFlag,
  IN AMD_SMM_CPU_STATE             *CpuState
  )
{
  mRebasedFlag = RebasedFlag;

  //
  // The offset of EIP/RIP is different depending on the SMMRevId
  //
  if (CpuState->x86.SMMRevId < SMM_MIN_REV_ID_x64) {
    mSmmRelocationOriginalAddress = (UINTN) CpuState->x86._EIP;
    CpuState->x86._EIP            = (UINT32) (UINTN) &SmmRelocationSemaphoreComplete;
  } else {
    mSmmRelocationOriginalAddress = (UINTN) CpuState->x64._RIP;
    CpuState->x64._RIP            = (UINT64) (UINTN) &SmmRelocationSemaphoreComplete;
  }

  if (CpuState->x64.AutoHALTRestart & BIT0) {
    //
    // Clear the auto HALT restart flag so the RSM instruction returns 
    //   program control to the instruction following the HLT instruction,
    //   actually returns to SmmRelocationSemaphoreComplete
    //
    CpuState->x64.AutoHALTRestart &= ~BIT0;
  }
}
          
