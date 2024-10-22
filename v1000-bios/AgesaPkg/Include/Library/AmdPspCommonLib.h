/* $NoKeywords:$ */
/**
 * @file
 *
 * PSP Base Library
 *
 * Contains interface to the PSP library
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  PSP
 * @e \$Revision: 317842 $   @e \$Date: 2015-04-29 15:06:52 +0800 (Wed, 29 Apr 2015) $
 *
 */
/*
 ******************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 **/

#ifndef _AMD_PSPCOMMONLIB_H_
#define _AMD_PSPCOMMONLIB_H_

#include <AmdPspDirectory.h>
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
BOOLEAN
GetFtpmControlArea (
  IN OUT   VOID **FtpmControlArea
  );

VOID
SwitchPspMmioDecode (
  IN       BOOLEAN SwitchFlag,
  IN OUT   UINT32 *RegisterCopy
  );

BOOLEAN
CheckPspDevicePresent (
  VOID
  );

// Bitmap defining capabilities
// Bitmap defining capabilities
#ifndef PSP_CAP_TPM_SUPPORTED
  #define PSP_CAP_TPM_SUPPORTED (1 << 0)      ///< TPM supported
#endif

#ifndef PSP_CAP_TPM_REQ_FACTORY_RESET
  #define PSP_CAP_TPM_REQ_FACTORY_RESET (1 << 1)   ///< TPM requires a factory reset
#endif

#ifndef PSP_CAP_FTPM_NEED_RECOVERY
  #define PSP_CAP_FTPM_NEED_RECOVERY           (1 << 2)   ///< TPM binary authentication fail, trigger recovery mode
#endif

EFI_STATUS
CheckFtpmCaps (
  IN OUT   UINT32 *Caps
  );

BOOLEAN
PSPEntryInfo (
  IN      UINT32                       EntryType,
  IN OUT  UINT64                      *EntryAddress,
  IN      UINT32                      *EntrySize
  );

typedef
BOOLEAN
(* FP_CONDITIONER) (
  IN       VOID        *Context
  );

#define PSPLIB_WAIT_INFINITELY 0xFFFFFFFFL

BOOLEAN
PspLibTimeOut (
  IN       UINT64              uSec,
  IN       FP_CONDITIONER      Conditioner,
  IN       VOID                *Context
  );

VOID
AcquirePspSmiRegMutex (
  VOID
  );

VOID
ReleasePspSmiRegMutex (
  VOID
  );

#endif // _AMD_PSPCOMMONLIB_H_

