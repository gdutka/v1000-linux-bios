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
#include <PiPei.h>
#include <Library/AmdBaseLib.h>
#include <Library/IdsLib.h>
#include <Library/PcdLib.h>
#include <Library/PeiServicesLib.h>
#include <Ppi/AmdIdsDebugPrintPpi.h>
#include <Filecode.h>

#define FILECODE LIBRARY_IDSPEILIB_IDSPEILIB_FILECODE

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
  VA_LIST Marker;
  EFI_STATUS                Status;
  AMD_IDS_DEBUG_PRINT_PPI   *AmdIdsDebugPrintPpi;

  VA_START (Marker, Format); //init marker to 1st dynamic parameters.
  //Locate IDS debug print PPI
  Status  = PeiServicesLocatePpi (
            &gAmdIdsDebugPrintPpiGuid,
            0,
            NULL,
            (VOID **)&AmdIdsDebugPrintPpi
            );
  if (!EFI_ERROR (Status)) {
    AmdIdsDebugPrintPpi->print (Flag, Format, Marker);
  }
  VA_END (Marker);
}

