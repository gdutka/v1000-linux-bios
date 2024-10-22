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
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/AmdIdsDebugPrintProtocol.h>
#include <Filecode.h>

#define FILECODE LIBRARY_IDSDXELIB_IDSDXELIB_FILECODE

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
  AMD_IDS_DEBUG_PRINT_PROTOCOL  *AmdIdsDebugPrintProtocol;

  VA_START (Marker, Format); //init marker to 1st dynamic parameters.
  //Locate IDS debug print protocol
  Status = gBS->LocateProtocol (&gAmdIdsDebugPrintProtocolGuid, NULL, (VOID**)&AmdIdsDebugPrintProtocol);

  if (!EFI_ERROR (Status)) {
    AmdIdsDebugPrintProtocol->print (Flag, Format, Marker);
  }
  VA_END (Marker);
}
