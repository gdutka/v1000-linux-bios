/** @file

  Include file for LBR (last branch record) feature.

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

#ifndef _CPU_LBR_H_
#define _CPU_LBR_H_

#include "Cpu.h"

/**
  Detect capability of LBR for specified processor.
  
  This function detects capability of LBR for specified processor.

  @param  ProcessorNumber   The handle number of specified processor.

**/
VOID
LbrDetect (
  UINTN   ProcessorNumber
  );

/**
  Configures Processor Feature Lists for LBR for all processors.
  
  This function configures Processor Feature Lists for LBR for all processors.

**/
VOID
LbrConfigFeatureList (
  VOID
  );

/**
  Produces entry of LBR in Register Table for specified processor.
  
  This function produces entry of LBR in Register Table for specified processor.

  @param  ProcessorNumber   The handle number of specified processor.
  @param  Attribute         Machine check Attribute.

**/
VOID
LbrReg (
  UINTN      ProcessorNumber,
  VOID       *Attribute
  );

#endif
