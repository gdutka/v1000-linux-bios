/* $NoKeywords:$ */
/**
 * @file
 *
 * IdsHookId.h
 *
 * Contains HOOKID definition
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project: (Proc/Ids/Internal)
 * @e \$Revision: 328983 $   @e \$Date: 2015-10-14 20:56:31 +0800 (Wed, 14 Oct 2015) $
 */
/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 **/

#ifndef _IDS_HOOKID_H_
#define _IDS_HOOKID_H_

typedef UINT32 HOOK_ID;
// 0x00000001 - 0x10000000 for CCX
#define IDS_HOOK_CCX_BEFORE_AP_LAUNCH   0x00000001ul
#define IDS_HOOK_CCX_AFTER_AP_LAUNCH    0x00000002ul
#define IDS_HOOK_CCX_AFTER_PWR_MNG      0x00000003ul
#define IDS_HOOK_CCX_SKIP_UCODE_PATCH   0x00000004ul
#define IDS_HOOK_CCX_SMM                0x00000005ul
#define IDS_HOOK_CCX_SKIP_SMEE          0x00000006ul
#define IDS_HOOK_CCX_PEI                0x00000007ul
#define IDS_HOOK_CCX_CUSTOM_PSTATES     0x00000011ul
#define IDS_HOOK_CCX_SKIP_SMM_LOCK      0x00000012ul
#define IDS_HOOK_CCX_CORE_DIS_MASK      0x00000013ul
#define IDS_HOOK_CCX_READY_TO_BOOT      0x00000014ul
#define IDS_HOOK_C2_LATENCY             0x00000015ul
#define IDS_HOOK_C3_LATENCY             0x00000016ul
#define IDS_HOOK_C1_LATENCY             0x00000017ul

// 0x20000001 - 0x30000000 for DF
#define IDS_HOOK_DF_AFTER_AP_LAUNCH     0x20000001ul
#define IDS_HOOK_DF_DXE_INIT            0x20000002ul
#define IDS_HOOK_DF_RAS_INIT            0x20000003ul
#define IDS_HOOK_DF_RAS_INIT2           0x20000004ul
#define IDS_HOOK_DF_CLK_GATING          0x20000005ul
#define IDS_HOOK_PIE_PWR_MGMT_INIT      0x20000006ul
#define IDS_HOOK_PIE_PWR_MGMT_INIT2     0x20000007ul
#define IDS_HOOK_PIE_PWR_MGMT_INIT3     0x20000008ul
#define IDS_HOOK_PIE_REDIRECT_SCRUB_CTRL_INIT 0x20000009ul
#define IDS_HOOK_PIE_DRAM_SCRUB_TIME_INIT     0x2000000Aul
#define IDS_HOOK_FABRIC_PEI_INIT_END    0x2000000Bul
#define IDS_HOOK_DF_CS_INIT1            0x2000000Cul
#define IDS_HOOK_DF_CS_INIT2            0x2000000Dul

// 0x30000001 - 0x40000000 for PSP
#define IDS_HOOK_PSP_MEM_RESTORE_ENABLED      0x30000001ul

//Special HOOK ID reserved for get IDS NV table

#define IDS_HOOK_BEFORE_MEM_INIT        0x60000001ul

// 0xB0000000 - 0xCFFFFFFF for NBIO
#define IDS_HOOK_NBIO_INIT_EARLY        0xB0000001ul
#define IDS_HOOK_NBIO_INIT_ENV          0xB0000002ul
#define IDS_HOOK_NBIO_BASE_INIT         0xB0000003ul
#define IDS_HOOK_NBIO_LOAD_BUILD_OPTION 0xB0000004ul
#define IDS_HOOK_NBIO_SMUV9_INIT        0xB0000005ul
#define IDS_HOOK_NBIO_DEVICE_REMAP      0xB0000006ul
#define IDS_HOOK_NBIO_PCIE_TOPOLOGY     0xB0000007ul
#define IDS_HOOK_NBIO_PCIE_USER_CONFIG  0xB0000008ul
#define IDS_HOOK_NBIO_PCIE_AER_INIT     0xB0000009ul
#define IDS_HOOK_NBIO_SMUV10_INIT       0xB000000Aul
#define IDS_HOOK_NBIO_SMUV9_INIT_CMN    0xB000000Bul
#define IDS_HOOK_NBIO_GFX_INIT_CONFIG   0xB000000Cul
#define IDS_HOOK_NBIO_GFX_INIT_FRAME_BUFFER 0xB000000Dul
#define IDS_HOOK_NBIO_GFX_VGA_REGISTER_CONFIG 0xB000000Eul
#define IDS_HOOK_NBIO_PSPP_TUNING       0xB000000Ful
#define IDS_HOOK_NBIO_AFTER_CCX         0xB0000010ul
#define IDS_HOOK_NBIO_SMUV10_INIT_CMN   0xB0000011ul
#define IDS_HOOK_NBIO_SMU_INIT          0xB0000012ul
#define IDS_HOOK_NBIO_PCIE_TUNING       0xB0000013ul

// 0xD0000000 - 0xDFFFFFFF for FCH
#define IDS_HOOK_FCH_INIT_RESET         0xD0000001ul
#define IDS_HOOK_FCH_INIT_ENV           0xD0000002ul

// Common
#define IDS_HOOK_BEGINNING_OF_AGESA     0xE0000000ul

#define IDS_HOOK_GET_NV_TABLE           0xFFFFFFFEul
#endif // _IDS_HOOKID_H_

