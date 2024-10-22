/** @file

  Include file for machine check initialization

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

#ifndef _EFI_MCHK_INIT_H_
#define _EFI_MCHK_INIT_H_

#include "Cpu.h"
#include "MpCommon.h"

typedef struct {
  BOOLEAN   Mca;
  BOOLEAN   ControlMsrPresent;
  UINTN     NumberOfErrorReportingBank;
  BOOLEAN   EnableMca;
} MACHINE_CHECK_ATTRIBUTE;

/**
  Detect capability of machine check for specified processor.
  
  This function detects capability of machine check for specified processor.

  @param  ProcessorNumber   The handle number of specified processor.

**/
VOID
MachineCheckDetect (
  UINTN   ProcessorNumber
  );

/**
  Configures Processor Feature Lists for machine check for all processors.
  
  This function configures Processor Feature Lists for machine check for all processors.

**/
VOID
MachineCheckConfigFeatureList (
  VOID
  );

/**
  Produces entry of machine check in Register Table for specified processor.
  
  This function produces entry of machine check in Register Table for specified processor.

  @param  ProcessorNumber   The handle number of specified processor.
  @param  Attribute         Machine check Attribute.

**/
VOID
MachineCheckReg (
  UINTN      ProcessorNumber,
  VOID       *Attribute
  );

#endif
