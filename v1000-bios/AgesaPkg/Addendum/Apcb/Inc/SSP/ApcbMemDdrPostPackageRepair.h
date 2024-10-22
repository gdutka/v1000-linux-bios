/*****************************************************************************
 *
 * Copyright (C) 2016-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef _APCB_MEM_DDR_POST_PACKAGE_REPAIR_H_
#define _APCB_MEM_DDR_POST_PACKAGE_REPAIR_H_
//
// @APCB_START
//

///
/// DDR Post Package Repair
///
#define MAX_DPPRCL_ENTRY 64
#define MAX_DPPRCL_SOCKETS 2
#define MAX_DPPRCL_CHANNELS 8
#define MAX_DPPRCL_ENTRIES MAX_DPPRCL_SOCKETS*MAX_DPPRCL_CHANNELS*MAX_DPPRCL_ENTRY    ///< @VALUE:1024

#define DPPR_SOFT_REPAIR 0
#define DPPR_HARD_REPAIR 1
#define DPPR_VALID_REPAIR 1
#define DPPR_INVALID_REPAIR 0
#define DPPR_SKIP_REPAIR  1    // if Repair and Valid and Repair skipped, it
                               // may be due to Bank Group Conflict

#define DPPR_DEVICE_X4   4
#define DPPR_DEVICE_X8   8
#define DPPR_DEVICE_X16  16

#define DPPR_NUM_CACHE_LINES 128
#define DPPR_BUBBLE_COUNT 256

#define DPPR_MAX_X16_DEVICES   72
#define DPPR_MAX_X4_DEVICES   18
#define DPPR_MAX_X8_DEVICES   9

#define DPPR_REPAIR_TARGET_DEVICES 0x1F

#pragma pack(1)
typedef struct _APCB_DPPRCL_REPAIR_ENTRY {
  UINT32 Bank:5;                           ///< [4:0] Bank {BG 3:2,BA 1:0}
  UINT32 RankMultiplier:3;                 ///< [7:5] Rank Multiplier
  UINT32 Device:5;                         ///< [12:8] Device width
                                           ///<        Device = 4, Repair DIMMs with device width x4 only
                                           ///<        Device = 8, Repair DIMMs with device width x8 only
                                           ///<        Device = 16, Repair DIMMs with device width x16 only
                                           ///<        Device = 0x1F - Ignore device width and repair specific devices based on TargetDevice
  UINT32 ChipSelect:2;                     ///< [14:13] ChipSelect
  UINT32 Column:10;                        ///< [24:15] Column Address
  UINT32 RepairType:1;                     ///< [25:25] Hard/soft repair (0=hard, 1=soft)
  UINT32 Valid:1;                          ///< [26:26] Valid entry
  UINT32 TargetDevice:5;                   ///< [31:27] Target Device (only valid if Device = 0x1F)
                                           ///<         x4 (0-17 (with ECC)), x8 (0-8 (with ECC))
  UINT32 Row:18;                           ///< [17:0] Row Address
  UINT32 Socket:3;                         ///< [20:18] Socket
  UINT32 Channel:3;                        ///< [23:21] Channel
  UINT32 Reserverd1:8;                     ///< pad to 32 bit
} APCB_DPPRCL_REPAIR_ENTRY;

/** APCB_DPPRCL_REPAIR_ENTRY
*    @GROUPID: 0x1704
*    @TYPEID: APCB_MEM_TYPE_DDR_POST_PACKAGE_REPAIR (0x5E)
*    APCB_DPPRCL_REPAIR_ENTRY
*/
typedef struct {
  APCB_DPPRCL_REPAIR_ENTRY          ApcbDpprclRepairEntry1st;
  APCB_DPPRCL_REPAIR_ENTRY          ApcbDpprclRepairEntry[];
} APCB_DPPRCL_REPAIR_ENTRY_STRUCTURE;
#pragma pack()

#endif // _APCB_MEM_DDR_POST_PACKAGE_REPAIR_H_

