/* $NoKeywords:$ */
/**
 * @file
 *
 * PSP SMM Driver
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AMD PSP UEFI Driver
 * @e sub-project   PSP SMM Driver
 * @e \$Revision$   @e \$Date$
 *
 */
/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef _AMD_PSP_SMMV2_H_
#define _AMD_PSP_SMMV2_H_

#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Protocol/LoadedImage.h>
#include <Protocol/MpService.h>

#include <Protocol/PspPlatformProtocol.h>
#include <Protocol/FchSmmSxDispatch2.h>
#include <Protocol/FchSmmSwDispatch2.h>
#include <Protocol/AmdCapsuleSmmHookProtocol.h>
#include <Protocol/PspMboxSmmBufferAddressProtocol.h>
#include <Library/AmdBaseLib.h>
#include <Library/IdsLib.h>
#include <Library/AmdPspBaseLibV2.h>
#include <Library/AmdPspMboxLibV2.h>
#include <Library/AmdCapsuleLib.h>
#include "PspResumeMemInfo.h"

#include <Filecode.h>

#define   SMM_SAVE_SIZE   0x200
#define SMM_RESUME_TEMP_REGION_SIZE   (1024 * 1)
#define AP_SYNC_BUFFER_SIZE (sizeof (UINT32))

typedef struct _SMM_SAVE_AREA_CONTEXT SMM_SAVE_AREA_CONTEXT;

/// Used to keep SMM context
typedef struct _SMM_SAVE_AREA_CONTEXT {
  VOID     *SaveAreaOffset;  ///< Point to SMM Context
  UINT8    SaveAreaBuffer[SMM_SAVE_SIZE]; ///< SMM Context buffer
} SMM_SAVE_AREA_CONTEXT;

extern SMM_SAVE_AREA_CONTEXT *mSmmSaveAreaCopy;

extern PSP_PLATFORM_PROTOCOL  mPspPlatformProtocolCopy;
extern PSP_SMM_RSM_MEM_INFO   *mPspSmmRsmMemInfoPtr;
extern UINTN                  mApRsmEntryPoint;
extern UINT8                  *mPspMboxSmmBuffer;
extern BOOLEAN                *mPspMboxSmmFlagAddr;
extern volatile UINT32        *mApSyncFlag;

EFI_STATUS
CopySmmSaveAreaToLocal (
  IN OUT   VOID **SmmSaveStart
  );

#endif  //_AMD_PSP_SMMV2_H_

