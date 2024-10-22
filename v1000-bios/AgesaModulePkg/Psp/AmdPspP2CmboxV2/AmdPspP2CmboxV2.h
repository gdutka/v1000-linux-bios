/* $NoKeywords:$ */
/**
 * @file
 *
 * PspP2Cmbox Driver
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AMD PSP UEFI Driver
 * @e sub-project   PspP2Cmbox Driver
 * @e \$Revision$   @e \$Date$
 *
 */
/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef _PSP_P2C_MBOX_H_
#define _PSP_P2C_MBOX_H_

#include <Base.h>
#include <Pi/PiFirmwareVolume.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Protocol/FchSmmSwDispatch2.h>
#include <Protocol/FchSmmMiscDispatch.h>

#include <Library/AmdPspFlashAccLib.h>

#include "AGESA.h"
#include <Library/AmdBaseLib.h>
#include <Library/IdsLib.h>
#include <Library/AmdSocBaseLib.h>
#include <Library/AmdPspBaseLibV2.h>
#include <Library/AmdPspMboxLibV2.h>
#include <Protocol/PspMboxSmmBufferAddressProtocol.h>

#define EFI_LBA_LIST_TERMINATOR   0xFFFFFFFFFFFFFFFFULL
#define MAX_WRITABLE_ENTRY_REGIONS            16

P2C_MBOX_STATUS
EFIAPI
ProcessPspCommand (
  IN UINT64                 Cmd,
  IN MBOX_DEFAULT_BUFFER   *MboxBuffer
);

EFI_STATUS
PspFlashAccSmmCommunicateHandler (
  IN EFI_HANDLE  DispatchHandle,
  IN CONST VOID  *Context,
  IN OUT VOID    *CommBuffer,
  IN OUT UINTN   *CommBufferSize
  );
/// SMI command target NV region
typedef enum {
  SMI_TARGET_NVRAM = 0,           ///< PSP NVRAM
  SMI_TARGET_TOKEN_UNLOCK = 1,            ///< PSP token unlock data
  SMI_TARGET_VM_GUARD = 2,            ///< VM Guard data region
  SMI_TARGET_COREMCE_DATA = 3,   ///< core machine check exception
  SMI_TARGET_TEE_WONE_NVRAM = 4,   ///< TEE_WONE_NVRAM data
  SMI_TARGET_PSP_NVRAM = 5,      ///< PSP NVRAM for RPMC
  SMI_TARGET_NO_USE = 0xFE,           ///< Not used
  SMI_TARGET_END = 0xFF,            ///< End flag
} SMI_TARGET_ID;

/// Directory type
typedef enum {
  DIR_TYPE_PSP = 0,               ///< PSP Directory
  DIR_TYPE_BIOS = 1,                ///< BIOS Directory
  DIR_TYPE_END = 0xFF,                ///< End flag
} DIR_TYPE;

///< Structure describe PSP NV Region
typedef struct _PSP_NV_DESCRIPTOR {
  UINT32           TargetID;            ///< Target NV ID, defined in SMI_TARGET_ID
  DIR_TYPE         Directory;           ///< Directory Type, defined in DIR_TYPE
  UINT8            DirEntryID;            ///< Entry ID
  BOOLEAN          Detected;            ///< Flag to present if entry detected
  UINT64           NvRegionBase;            ///< Region base
  UINT32           NvRegionSize;            ///< Region Size
} PSP_NV_DESCRIPTOR;
#endif  //_PSP_P2C_MBOX_H_

