/* $NoKeywords:$ */
/**
 * @file
 *
 * Definitions for memory context save/restore
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: MEM
 * @e \$Revision: 313706 $   @e \$Date: 2015-02-25 21:00:43 -0600 (Wed, 25 Feb 2015) $
 *
 */
/*
*****************************************************************************
*
* Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
*
*******************************************************************************
*
*/
#ifndef _MEM_RESTORE_H_
#define _MEM_RESTORE_H_

#pragma pack (push, 1)

#define CMOS_PORT_72                                0x72
#define CMOS_PORT_73                                0x73
#define CMOS_MEM_RESTORE_OFFSET                     0xD
#define CMOS_BITMAP_MEM_RESTORE_BOOT_FAIL           0x1
#define CMOS_BITMAP_DISCARD_MEM_CONTEXT             0x2
#define CMOS_BITMAP_APOB_SAVED                      0x4

VOID
AmdMemRestoreSignOff (
  VOID
  );

BOOLEAN
AmdMemRestoreGetFailure (
  VOID
  );

VOID
AmdMemRestoreDiscardCurrentMemContext (
  VOID
  );

#pragma pack (pop)

#endif

