/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD AGESA PSP Resume Service Protocol Initialization
 *
 * Contains code to initialize GUID for AMD_PSP_RESUME_SERVICE_PROTOCOL_GUID
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  PSP
 * @e \$Revision$   @e \$Date$
 *
 */
/****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef _AMD_CAPSULE_SMM_HOOK_PROTOCOL_H_
#define _AMD_CAPSULE_SMM_HOOK_PROTOCOL_H_

//Definition for AmdCapsuleHookFlag
#define PSP_CAPSULE_HOOK_FLAG_NOSXINFO_NOCLRSMMLCK 0x00000000ul
#define PSP_CAPSULE_HOOK_FLAG_SXINFO_NOCLRSMMLCK   0x00000001ul
#define PSP_CAPSULE_HOOK_FLAG_SXINFO_CLRSMMLCK     0x00000002ul

typedef EFI_STATUS (*AMD_CAPSULE_SMM_HOOK) (
  IN       UINT32  AmdCapsuleHookFlag
);

typedef struct _AMD_CAPSULE_SMM_HOOK_PROTOCOL
{
  AMD_CAPSULE_SMM_HOOK Hook;
} AMD_CAPSULE_SMM_HOOK_PROTOCOL;

extern EFI_GUID gAmdCapsuleSmmHookProtocolGuid;

#endif //_AMD_CAPSULE_SMM_HOOK_PROTOCOL_H_

