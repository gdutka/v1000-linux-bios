/** @file

  Include file for microcode update

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

#ifndef _CPU_MICROCODE_H_
#define _CPU_MICROCODE_H_

#include "MpCommon.h"

#define MAX_MICROCODE_DESCRIPTOR_LENGTH  100

typedef struct {
  VOID     *MicrocodeData;
  UINTN    MicrocodeSize;
  UINT32   ProcessorId;
  BOOLEAN  Load;
} MICROCODE_INFO;

/**
  Detect whether specified processor can find matching mirocode patch.
  
  This function detects whether specified processor can find matching mirocode patch.

  @param  ProcessorNumber   The handle number of specified processor.
  @param  FirstLoad         Indicates if it is first time load of the microcode update.

**/
VOID
MicrocodeDetect (
  IN UINTN   ProcessorNumber,
  IN BOOLEAN FirstLoad
  );


/**
  Generate entry for microcode load in S3 register table.
  
  For first time microcode load, if needed, an entry is generated in the pre-SMM-init register table.
  For second time microcode load, if needed, an entry is generated in the post-SMM-init register table.

  @param  ProcessorNumber   The handle number of specified processor.
  @param  FirstLoad         Indicates if it is first time load of the microcode update.

**/
VOID
MicrocodeReg (
  IN UINTN    ProcessorNumber,
  IN BOOLEAN  FirstLoad
  );

/**
  Measure loaded CPU microcode.
  
**/
VOID
MeasureMicrocode (
  VOID
  );

#endif
