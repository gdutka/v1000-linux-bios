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

#include <Base.h>

#include <Library/PcdLib.h>
#include <Library/BaseLib.h>
#include <Library/CpuLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/PciLib.h>
#include <Library/LocalApicLib.h>

#include "PiSmmCpuDxeSmm.h"
#include "SmmFeatures.h"

/**
  Return if SMRR is supported

  @retval TRUE  SMRR is supported.
  @retval FALSE SMRR is not supported.

**/
BOOLEAN
IsSmrrSupported (
  VOID
  )
{
  return FALSE;
}

/**
  Initialize SMRR in SMM relocate.

  @param  SmrrBase           The base address SMRR.
  @param  SmrrSize           The size of SMRR.
**/
VOID
InitSmrr (
  IN UINT32                SmrrBase,
  IN UINT32                SmrrSize
  )
{
}

/**
  Disable SMRR register when SmmInit replace non-SMM MTRRs.
**/
VOID
DisableSmrr (
  VOID
  )
{
}

/**
  Enable SMRR register when SmmInit restores non-SMM MTRRs.
**/
VOID
EnableSmrr (
  VOID
  )
{
}

/**
  Configure SMRR register at each SMM entry.
**/
VOID
ConfigSmrr (
  VOID
  )
{
}

/**
  Enable SMRR register when SmmInit restore non SMM MTRRs.
**/
VOID
ReenableSmrr (
  VOID
  )
{
}

/**
  Return if it is needed to configure MTRR to set TSEG cacheability.

  @retval  TRUE  - we need configure MTRR
  @retval  FALSE - we do not need configure MTRR
**/
BOOLEAN
NeedConfigureMtrrs (
  VOID
  )
{
  return FALSE;
}

/**
  Processor specific hook point at each SMM entry.

  @param  CpuIndex    The index of the cpu which need to check.

**/
VOID
SmmRendezvousEntry (
  IN UINTN  CpuIndex
  )
{
//   AsmWriteMsr64 (0xC0010113, AsmReadMsr64 (0xC0010113) | 0x0003);

  //
  // Set up MSRC001_0116 here
  //
}

/**
  Processor specific hook point at each SMM entry.

  @param  CpuIndex    The index of the cpu which need to check.
**/
VOID
SmmRendezvousExit (
  IN UINTN  CpuIndex
  )
{
  //
  // Set up MSRC001_0116 here
  //
}

/**
  Initialize SMRR context in SMM Init.
**/
VOID
InitializeSmmMtrrManager (
  VOID
  )
{
}

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
  )
{
  AsmWriteMsr64 (0xC0010112, SmrrBase);
  AsmWriteMsr64 (0xC0010113, ~((UINT64)SmrrSize - 1) | 0x4400);
}


/**
  Return if it is needed to relocate SMM base address.

  @retval  TRUE  - we need relocate SMM base address
  @retval  FALSE - we do not need relocate SMM base address
**/
BOOLEAN
NeedRelocateSmbase (
  VOID
  )
{
  return (AsmReadMsr64 (0xC0010111) == SMM_DEFAULT_SMBASE);
}

