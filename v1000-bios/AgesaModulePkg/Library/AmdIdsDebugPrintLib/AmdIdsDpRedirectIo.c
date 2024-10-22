/**
 * @file
 *
 * AMD Integrated Debug Debug_library Routines
 *
 * Contains all functions related to Redirect IO
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
#include <Pi/PiMultiPhase.h>
#include <Library/AmdBaseLib.h>
#include <Library/AmdIdsDebugPrintLib.h>
#include <AmdIdsDebugPrintLocal.h>
#include <Filecode.h>

#define FILECODE LIBRARY_AMDIDSDEBUGPRINTLIB_AMDIDSDPREDIRECTIO_FILECODE

/**
 *  Determine whether IDS console is enabled.
 *
 *
 *  @retval       TRUE    Alway return true
 *
 **/
BOOLEAN
AmdIdsDpRedirectIoSupport (
  VOID
  )
{
  return PcdGetBool (PcdAmdIdsDebugPrintRedirectIOEnable);
}

VOID
AmdIdsDpRedirectIoConstructor (
  VOID
  )
{
  return;
}

/**
 *  Get customize Filter
 *
 *  @param[in,out] Filter    Filter do be filled
 *
 *  @retval       FALSE    Alway return FALSE
 *
 **/
BOOLEAN
AmdIdsDpRedirectIoGetFilter (
  IN OUT   UINT64 *Filter
  )
{
  return FALSE;
}


#define REDIRECT_IO_DATA_BEGIN 0x5f535452ul
#define REDIRECT_IO_DATA_END   0x5f454e44ul

/**
 *  Print formated string with redirect IO
 *
 *  @param[in] Buffer  - Point to input buffer
 *  @param[in] BufferSize  - Buffer size
 *
**/
VOID
AmdIdsDpRedirectIoPrint (
  IN      CHAR8   *Buffer,
  IN      UINTN BufferSize
  )
{
  UINT32 Value;

  Value = REDIRECT_IO_DATA_BEGIN;
  LibAmdIoWrite (AccessWidth32, PcdGet16 (PcdIdsDebugPort), &Value, NULL);

  while (BufferSize--) {
    LibAmdIoWrite (AccessWidth8, PcdGet16 (PcdIdsDebugPort), Buffer++, NULL);
  }

  Value = REDIRECT_IO_DATA_END;
  LibAmdIoWrite (AccessWidth32, PcdGet16 (PcdIdsDebugPort), &Value, NULL);
}

CONST AMD_IDS_DEBUG_PRINT_INSTANCE  AmdIdsDebugPrintRedirectIoInstance =
{
  AmdIdsDpRedirectIoSupport,
  AmdIdsDpRedirectIoConstructor,
  AmdIdsDpRedirectIoGetFilter,
  AmdIdsDpRedirectIoPrint
};




