/** @file

  The CPU specific programming for PiSmmCpuDxeSmm module.

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

#ifndef __SMM_FEATURES_H__
#define __SMM_FEATURES_H__

#define CACHE_WRITE_BACK                       6
#define SMM_DEFAULT_SMBASE                     0x30000

/**
  Disable SMRR register when SmmInit set SMM MTRRs.
**/
VOID
DisableSmrr (
  VOID
  );

/**
  Enable SMRR register when SmmInit restore non SMM MTRRs.
**/
VOID
ReenableSmrr (
  VOID
  );

/**
  Return if it is needed to configure MTRR to set TSEG cacheability.

  @retval  TRUE  - we need configure MTRR
  @retval  FALSE - we do not need configure MTRR
**/
BOOLEAN
NeedConfigureMtrrs (
  VOID
  );

/**
  Processor specific hook point at each SMM entry.

  @param  CpuIndex    The index of the cpu which need to check.
**/
VOID
SmmRendezvousEntry (
  IN UINTN  CpuIndex
  );

/**
  Processor specific hook point at each SMM exit.

  @param  CpuIndex    The index of the cpu which need to check.
**/
VOID
SmmRendezvousExit (
  IN UINTN  CpuIndex
  );

/**
  Initialize SMRR context in SMM Init.
**/
VOID
InitializeSmmMtrrManager (
  VOID
  );

/**
  Initialize SMRR register in SMM Relocate.

  @param  SmrrBase           The base address SMRR.
  @param  SmrrSize           The size of SMRR.
  @param  IsBsp              If this processor treated as BSP.
**/
VOID
SmmInitiFeatures (
  IN UINT32  SmrrBase,
  IN UINT32  SmrrSize,
  IN BOOLEAN IsBsp
  );

/**
  Return if it is needed to relocate SMM base address.

  @retval  TRUE  - we need relocate SMM base address
  @retval  FALSE - we do not need relocate SMM base address
**/
BOOLEAN
NeedRelocateSmbase (
  VOID
  );

#endif
