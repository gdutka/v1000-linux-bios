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
#include <Library/AmdBaseLib.h>
#include <Library/IdsLib.h>
#include <Filecode.h>

#define FILECODE LIBRARY_IDSLIBNULL_IDSLIBNULL_FILECODE


/*--------------------------------------------------------------------------------------*/
/**
 *  IDS back-end code for AGESA_TESTPOINT
 *
 *  @param[in] TestPoint  Progress indicator value, see @ref AGESA_POST_CODE
 *  @param[in,out] StdHeader    The Pointer of AGESA Header
 *
 **/
/*--------------------------------------------------------------------------------------*/
VOID
IdsAgesaTestPoint (
  IN       AGESA_POST_CODE      TestPoint,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  )
{
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
  return TRUE;
}

/**
 *  Prints string to debug host like printf in C
 *
 *  @param[in] Flag    - filter flag
 *  @param[in] *Format - format string
 *  @param[in] ... Variable parameter
 *
**/
VOID
IdsLibDebugPrint (
  IN      UINT64      Flag,
  IN      CONST CHAR8 *Format,
  IN      ...
  )
{
}
