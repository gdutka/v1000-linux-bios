/** @file

Page table manipulation functions for IA-32 processors

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

SPIN_LOCK                           mPFLock;

/**
  Create PageTable for SMM use.

  @return     PageTable Address    

**/
UINT32
SmmInitPageTable (
  VOID
  )
{
  //
  // Initialize spin lock
  //
  InitializeSpinLock (&mPFLock);

  return Gen4GPageTable (0);
}

/**
  Page Fault handler for SMM use.

**/
VOID
SmiDefaultPFHandler (
  VOID
  )
{
  CpuDeadLoop ();
}

/**
  Page Fault handler for SMM use.

  @param   IntNumber             The number of the interrupt.
  @param   ErrorCode             The Error code.
  @param   InstructionAddress    The instruction address which caused page fault.

**/
VOID
EFIAPI
SmiPFHandler (
  IN      UINTN                     IntNumber,
  IN      UINTN                     ErrorCode,
  IN      UINTN                     InstructionAddress
  )
{
  UINTN             PFAddress;

  ASSERT (IntNumber == 0x0e);

  AcquireSpinLock (&mPFLock);
  
  PFAddress = AsmReadCr2 ();

  if ((FeaturePcdGet (PcdCpuSmmStackGuard)) && 
      (PFAddress >= gSmmCpuPrivate->SmrrBase) && 
      (PFAddress < (gSmmCpuPrivate->SmrrBase + gSmmCpuPrivate->SmrrSize))) {
    DEBUG ((EFI_D_ERROR, "SMM stack overflow!\n"));
    CpuDeadLoop ();
  }
  
//  if (FeaturePcdGet (PcdCpuSmmProfileEnable)) {
//    SmmProfilePFHandler (InstructionAddress, ErrorCode);
//  } else {
//    SmiDefaultPFHandler ();
//  }  
  
  ReleaseSpinLock (&mPFLock);
}