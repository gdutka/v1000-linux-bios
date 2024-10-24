/**
 * @file
 *
 * AMD Integrated Debug Debug_library Routines
 *
 * Contains AMD AGESA debug macros and library functions
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  IDS
 * @e \$Revision: 309090 $   @e \$Date: 2014-12-10 02:28:05 +0800 (Wed, 10 Dec 2014) $
 */
/*****************************************************************************
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */

 /*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include <Uefi.h>
#include <Library/IoLib.h>
#include <Library/BaseLib.h>
#include <Library/TimerLib.h>
#include <Library/AmdBaseLib.h>
#include <Library/IdsLib.h>
#include <Filecode.h>

#define FILECODE  LIBRARY_IDSMISCLIB_IDSMISCLIB_FILECODE
/*--------------------------------------------------------------------------------------*/
/**
 *  IDS back-end code for AGESA_TESTPOINT
 *
 *  @param[in] TestPoint  Progress indicator value, see @ref AGESA_POST_CODE
 *  @param[in,out] StdHeader    The Pointer of AGESA Header
 *
 **/
/*--------------------------------------------------------------------------------------*/
#define AGESA_POST_CODE_PREFIX 0xB0000000ul
VOID
IdsAgesaTestPoint (
  IN       AGESA_POST_CODE      TestPoint,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT32   SizedTp;
  UINT32   TpPrefix;

  TpPrefix = AGESA_POST_CODE_PREFIX;
  if (PcdGetBool (PcdAgesaTestPointEnable)) {
    SizedTp = (UINT32) TestPoint;
    SizedTp |= TpPrefix;

    LibAmdIoWrite (PcdGet8 (PcdAgesaTestPointWidth), PcdGet16 (PcdIdsDebugPort), &SizedTp, StdHeader);
  }
}

/**
 *  IDS Function to display FileCode and Line number on port 0x80 display
 *
 *  @param[in]    FileCode    FileCode and Line Number to display
 *
 **/
BOOLEAN
IdsErrorStop (
  IN       UINT32 FileCode
  )
{
  UINT64  Port80Message;
  UINT16  SimNowEnterDebuggerStatus;
  UINTN   Index;

  // Write out FileCode one time and fire SimNow breakpoint
  IoWrite16 (0xE0, 0xDEAD);
  IoWrite32 (0x80, FileCode);
  SimNowEnterDebuggerStatus = LibAmdSimNowEnterDebugger ();

  Port80Message = 0xDEAD000000000000 | LShiftU64 (FileCode, 16);
  while (1) {
    if (SimNowEnterDebuggerStatus != 0) {
      break;
    }
    for (Index = 0; Index < 6; Index ++) {
      Port80Message = LRotU64 (Port80Message, 8);
      IoWrite32 (0x80, (UINT32)Port80Message);
      MicroSecondDelay (2500000);
    }
    // Rotate back to original value
    Port80Message = LRotU64 (Port80Message, 16);
  }
  return FALSE;
}

/**
 *  IDS Backend Function for ASSERT
 *
 * Halt execution with stop code display.  Stop Code is displayed on port 80, with rotation so that
 * it is visible on 8, 16, or 32 bit display.  The stop code is alternated with 0xDEAD on the display,
 * to help distinguish the stop code from a post code loop.
 * Additional features may be available if using simulation.
 *
 * @param[in]     FileCode    File code(define in FILECODE.h) mix with assert Line num.
 *
 * @retval         TRUE     No error
 **/
BOOLEAN
IdsAssert (
  IN      UINT32 FileCode
  )
{
  if (PcdGetBool (PcdAgesaAssertEnable)) {
    IdsErrorStop (FileCode);
  }
  return TRUE;
}

