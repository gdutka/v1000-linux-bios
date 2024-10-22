/* $NoKeywords:$ */
/**
 * @file
 *
 * Implement Custom Core Pstates
 *
 * Contains code that Custom Core Pstates
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Library
 * @e \$Revision$   @e \$Date$
 *
 */
/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */

/*****************************************************************************
 *
 * This software package can be used to enable the Overclocking of certain
 * AMD processors and its use is subject to the terms and conditions of the
 * AMD Overclocking Waiver. Enabling overclocking through use of the low-level
 * routines included in this package and operating an AMD processor outside of
 * the applicable AMD product specifications will void any AMD warranty and can
 * result in damage to the processor or the system into which the processor has
 * been integrated. The user of this software assumes, and AMD disclaims, all
 * risk, liability, costs and damages relating to or arising from the overclocking
 * of AMD processors.
 *
 ******************************************************************************
 */

#ifndef _CCX_IDS_CUSTOM_PSTATES_LIB_H_
#define _CCX_IDS_CUSTOM_PSTATES_LIB_H_
#include <Library/AmdIdsHookLib.h>

#pragma pack (push, 1)

#define CCX_IDS_CORE_PSTATES_DISABLED 0
#define CCX_IDS_CORE_PSTATES_CUSTOM 1
#define CCX_IDS_CORE_PSTATES_AUTO 2

/* P-state Registers 0xC001006[B:4] */
#define MSR_PSTATE_0 0xC0010064ul
#define MSR_PSTATE_1 0xC0010065ul
#define MSR_PSTATE_2 0xC0010066ul
#define MSR_PSTATE_3 0xC0010067ul
#define MSR_PSTATE_4 0xC0010068ul
#define MSR_PSTATE_5 0xC0010069ul
#define MSR_PSTATE_6 0xC001006Aul
#define MSR_PSTATE_7 0xC001006Bul

#define PS_REG_BASE MSR_PSTATE_0     /* P-state Register base */
#define PS_MAX_REG  MSR_PSTATE_7     /* Maximum P-State Register */
#define PS_MIN_REG  MSR_PSTATE_0     /* Minimum P-State Register */

/// Custom Core Pstate
typedef struct {
  UINT8  Custom;            ///< Disable/Custom/Auto
  UINT8  CoreFid;           ///< Core FID
  UINT8  CoreDid;           ///< Core DID
  UINT8  CoreVid;           ///< Core VID
} IDS_CUSTOM_CORE_PSTATE;

/* P-state Current Limit Register 0xC0010061 */
#define MSR_PSTATE_CURRENT_LIMIT 0xC0010061ul

/// Pstate Current Limit MSR Register
typedef union {
  struct {                             ///< Bitfields of Pstate Current Limit MSR Register
    UINT64 CurPstateLimit:3;           ///< Current Pstate Limit
    UINT64 :1;                         ///< Reserved
    UINT64 PstateMaxVal:3;             ///< Pstate Max Value
    UINT64 :57;                        ///< Reserved
  } Field;
  UINT64  Value;
} IDS_PSTATE_CURLIM_MSR;

/// P-state MSR
typedef union {
  struct {                             ///< Bitfields of P-state MSR
    UINT64 CpuFid_7_0:8;               ///< CpuFid[7:0]
    UINT64 CpuDfsId:6;                 ///< CpuDfsId
    UINT64 CpuVid:8;                   ///< CpuVid
    UINT64 IddValue:8;                 ///< IddValue
    UINT64 IddDiv:2;                   ///< IddDiv
    UINT64 :31;                        ///< Reserved
    UINT64 PstateEn:1;                 ///< Pstate Enable
  } Field;
  UINT64  Value;
} IDS_PSTATE_MSR;


/// Update MSR
typedef struct {
  UINT32 MsrAddr;                      ///< MSR Address
  UINT64 MsrData;                      ///< MSR Data
} CCX_UPDATE_MSR;

VOID
CcxIdsCustomPstateSetMsrLib (
  IN       IDS_CUSTOM_CORE_PSTATE  *CustomPstates
  );

#pragma pack (pop)
#endif // _CCX_IDS_CUSTOM_PSTATES_ZEN_LIB_H_
