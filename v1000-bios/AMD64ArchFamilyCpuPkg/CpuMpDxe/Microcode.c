/** @file

  Code for microcode update

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

#include "Cpu.h"
#include "Microcode.h"

MICROCODE_INFO mMicrocodeInfo[FixedPcdGet32 (PcdCpuMaxLogicalProcessorNumber)];

/**
  Verify the DWORD type checksum.
  
  This function verifies the DWORD type checksum.

  @param  ChecksumAddr   The handle number of specified processor.
  @param  ChecksumLen    The handle number of specified processor.

**/
EFI_STATUS
Checksum32Verify (
  IN UINT32 *ChecksumAddr,
  IN UINT32 ChecksumLen
  )
{
  UINT32   Checksum;
  UINT32   Index;
  
  Checksum = 0;
  
  for (Index = 0; Index < ChecksumLen; Index ++) {
    Checksum += ChecksumAddr[Index];
  }
  
  return (Checksum == 0) ? EFI_SUCCESS : EFI_CRC_ERROR;
}

/**
  Get microcode update signature of currently loaded microcode udpate.
**/
INT32
GetCurrentMicrocodeSignature (
  VOID
  )
{
  UINT64 Signature;

  AsmWriteMsr64 (EFI_MSR_IA32_BIOS_SIGN_ID, 0);
  AsmCpuid (EFI_CPUID_VERSION_INFO, NULL, NULL, NULL, NULL);
  Signature = AsmReadMsr64 (EFI_MSR_IA32_BIOS_SIGN_ID);
  return (INT32) RShiftU64 (Signature, 32);
}

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
  )
{
}

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
  )
{
}

/**
  Measure loaded CPU microcode.
  
**/
VOID
MeasureMicrocode (
  VOID
  )
{
}

