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
//#include <AGESA.h>
#include <Library/AmdBaseLib.h>
#include <Library/IdsLib.h>
#include <Library/AmdIdsDebugPrintLib.h>

#define _INT_SIZE_OF(n) ((sizeof (n) + sizeof (UINTN) - 1) &~(sizeof (UINTN) - 1))
typedef CHAR8 *VA_LIST;
#undef VA_START
#undef VA_ARG
#undef VA_END
#define VA_START(ap, v) (ap = (VA_LIST) & (v) + _INT_SIZE_OF (v))
#define VA_ARG(ap, t)   (*(t *) ((ap += _INT_SIZE_OF (t)) - _INT_SIZE_OF (t)))
#define VA_END(ap)      (ap = (VA_LIST) 0)

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

  VA_START (Marker, Format); //init marker to 1st dynamic parameters.
  //Call AGESA work function
  AmdIdsDebugPrintf (Flag, Format, Marker);
  VA_END (Marker);
}
