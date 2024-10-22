/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD IDS Routines
 *
 * Contains AMD AGESA Integrated Debug Macros
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  IDS
 * @e \$Revision: 324927 $   @e \$Date: 2015-08-24 15:09:45 +0800 (Mon, 24 Aug 2015) $
 */
/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 **/

  /* Macros to aid debugging */
  /* These definitions expand to zero (0) bytes of code when disabled */

#ifndef _IDS_COMPATIBILITY_H_
#define _IDS_COMPATIBILITY_H_

#define IDS_OPTION_HOOK(IdsOption, DataPtr, StdHeader)
#define IDS_SKIP_HOOK(IdsOption, DataPtr, StdHeader)
#define IDS_PERF_TIMESTAMP(ID, StdHeader)

#define IEM_SKIP_CODE(L)
#define IEM_INSERT_CODE(L, Fn, Parm)
#define IDS_HDT_CONSOLE_DEBUG_CODE(Code)
#define IDS_TIMEOUT_CTL(t)

/// Data Structure for Mem ECC parameter override
typedef struct {
  IN BOOLEAN CfgEccRedirection;                   ///< ECC Redirection
  IN UINT16  CfgScrubDramRate;                    ///< Scrub Dram Rate
  IN UINT16  CfgScrubL2Rate;                      ///< Scrub L2Rate
  IN UINT16  CfgScrubL3Rate;                      ///< Scrub L3Rate
  IN UINT16  CfgScrubIcRate;                      ///< Scrub Ic Rate
  IN UINT16  CfgScrubDcRate;                      ///< Scrub Dc Rate
  IN BOOLEAN CfgEccSyncFlood;                     ///< ECC Sync Flood
} ECC_OVERRIDE_STRUCT;


#endif // _IDS_COMPATIBILITY_H_

