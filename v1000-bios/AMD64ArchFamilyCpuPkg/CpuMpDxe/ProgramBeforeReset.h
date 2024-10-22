/** @file

  Include file for core to bus ratio and VID on reset.

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

#ifndef _CPU_FREQUENCY_ON_RESET_H_
#define _CPU_FREQUENCY_ON_RESET_H_

#include "Cpu.h"

#define   IA32_FLEX_BRVID_SEL             0x1A1

/**
  Detect whether Platform Requried Bit is set by the processor.
  
  This function detects whether Platform Requried Bit is set by the processor.

  @param  ProcessorNumber   The handle number of specified processor.

**/
VOID
PlatformRequirementDetect (
  UINTN   ProcessorNumber
  );

/**
  Program processor before reset.
  
  This function programs processor before reset.

  @param  ProcessorNumber   The handle number of specified processor.

**/
VOID
ProgramBeforeReset (
  UINTN     ProcessorNumber
  );

#endif
