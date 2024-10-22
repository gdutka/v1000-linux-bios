/** @file

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
**/

#include "TscTimerLibInternal.h"

VOID
GetDedicateStateFrequency (
  IN      UINT64                    DedicatePstate,
  OUT     UINT64                    *Frequency
  )
{
  UINT64        PstateXInfo;
  UINT32        CpuDfsId, CpuFid;
  UINT64        Frequency1, Frequency2;

  // Core current operating frequency in MHz. CoreCOF =
  // (Core::X86::Msr::PStateDef[CpuFid[7:0]]/Core::X86::Msr::PStateDef[CpuDfsId])*200

  PstateXInfo = AsmReadMsr64 (AMD_MSR_PSTATE_0 + (UINT32) DedicatePstate);

  CpuDfsId = (UINT32) (RShiftU64 ((PstateXInfo & 0x3F00), 8)); // Bit[13:8]
  CpuFid = (UINT32) (PstateXInfo & 0xFF);                       // Bit[7:0]

  Frequency1 = (UINT64)(200 * CpuFid / CpuDfsId);
  Frequency2 = MultU64x32 (DivU64x32 (Frequency1, 100), 100);
  Frequency1 = (Frequency1 - Frequency2);
  if (Frequency1 < 50) {
    *Frequency = Frequency2;
  } else {
    *Frequency = Frequency2 + 100;
  }
}

/**  Core current operating frequency in MHz.

  @return The Core current operating frequency in MHz.

**/
UINT64
GetCoreCOF (
  VOID
  )
{
  UINT64               Frequency;
  UINT64               CurPstate;

  CurPstate = AsmReadMsr64 (AMD_MSR_PSTATE_STS) & CURRENT_PSTATE_MASK;

  GetDedicateStateFrequency (CurPstate, &Frequency);

  return Frequency;
}
