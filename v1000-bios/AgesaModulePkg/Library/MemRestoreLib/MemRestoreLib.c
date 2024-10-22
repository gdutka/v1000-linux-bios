/* $NoKeywords:$ */
/**
 * @file
 *
 * MemRestoreLib.c
 *
 * Memory Context Save/Restore Support
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Main)
 * @e \$Revision: 329870 $ @e \$Date: 2015-10-29 13:13:29 +0800 (Thu, 29 Oct 2015) $
 *
 **/
/*****************************************************************************
  *
  * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
  *
  *******************************************************************************
  *
 */

/*
 *----------------------------------------------------------------------------
 *                                MODULES USED
 *
 *----------------------------------------------------------------------------
 */

#include <Library/BaseLib.h>
#include "AGESA.h"
#include "Library/AmdBaseLib.h"
#include "Library/MemRestore.h"
#include "Filecode.h"
//[-start-240426-IB18160212-remove-]//
//CODE_GROUP (G2_PEI)
//RDATA_GROUP (G2_PEI)
//[-end-240426-IB18160212-remove-]//

#define FILECODE LIBRARY_MEMRESTORELIB_MEMRESTORELIB_FILECODE
/*----------------------------------------------------------------------------
 *                          DEFINITIONS AND MACROS
 *
 *----------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------
 *                           TYPEDEFS AND STRUCTURES
 *
 *----------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------
 *                        PROTOTYPES OF LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */

/**
 *  Set the flag of memory context save/restore to PASS
 *
 *
 **/
VOID
AmdMemRestoreSignOff (
  VOID
  )
{
  UINT8   Value8;
  UINT8   CmosData;

  IDS_HDT_CONSOLE (MAIN_FLOW, "\t[AMD MemRestore] Sign off Mem restore boot\n");

  //Update the CMOS[0xD] bit 0 with flag 0 to indicate S3 completion by memory context restore
  Value8 = CMOS_MEM_RESTORE_OFFSET;
  LibAmdIoWrite (AccessWidth8, CMOS_PORT_72, &Value8, NULL);
  Value8 = 0;
  LibAmdIoRead (AccessWidth8, CMOS_PORT_73, &Value8, NULL);
  CmosData = Value8;

  Value8 = CMOS_MEM_RESTORE_OFFSET;
  LibAmdIoWrite (AccessWidth8, CMOS_PORT_72, &Value8, NULL);
  Value8 = (CmosData & ~CMOS_BITMAP_MEM_RESTORE_BOOT_FAIL) | CMOS_BITMAP_APOB_SAVED;
  LibAmdIoWrite (AccessWidth8, CMOS_PORT_73, &Value8, NULL);
}

/**
 *  Check if memory context save/restore has been performed
 *
 *
 *  @retval CMOS Flag to indicate the failure
 *
 **/
BOOLEAN
AmdMemRestoreGetFailure (
  VOID
  )
{
  UINT8   Value8;

  Value8 = CMOS_MEM_RESTORE_OFFSET;
  LibAmdIoWrite (AccessWidth8, CMOS_PORT_72, &Value8, NULL);
  Value8 = 0x0;
  LibAmdIoRead (AccessWidth8, CMOS_PORT_73, &Value8, NULL);

  IDS_HDT_CONSOLE (MAIN_FLOW, "\t[AMD MemRestore] Get Mem Restore Failure = %d\n", Value8 & CMOS_BITMAP_MEM_RESTORE_BOOT_FAIL);

  if ((Value8 & CMOS_BITMAP_MEM_RESTORE_BOOT_FAIL) != 0) {
    return TRUE;
  }

  return FALSE;
}

/**
 *  Discard the current memory context saved in SPI ROM
 *
 *
 **/
VOID
AmdMemRestoreDiscardCurrentMemContext (
  VOID
  )
{
  UINT8   Value8;
  UINT8   CmosData;

  IDS_HDT_CONSOLE (MAIN_FLOW, "\t[AMD MemRestore] Discard current memory context\n");

  Value8 = CMOS_MEM_RESTORE_OFFSET;
  LibAmdIoWrite (AccessWidth8, CMOS_PORT_72, &Value8, NULL);
  Value8 = 0;
  LibAmdIoRead (AccessWidth8, CMOS_PORT_73, &Value8, NULL);
  CmosData = Value8;

  Value8 = CMOS_MEM_RESTORE_OFFSET;
  LibAmdIoWrite (AccessWidth8, CMOS_PORT_72, &Value8, NULL);
  Value8 = CmosData | CMOS_BITMAP_DISCARD_MEM_CONTEXT;
  LibAmdIoWrite (AccessWidth8, CMOS_PORT_73, &Value8, NULL);
}

