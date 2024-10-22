/* $NoKeywords:$ */
/**
 * @file
 *
 * Graphics controller BIF straps control services.
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 309090 $   @e \$Date: 2014-12-10 02:28:05 +0800 (Wed, 10 Dec 2014) $
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


#ifndef _GNBFUSETABLE_H_
#define _GNBFUSETABLE_H_

#pragma pack (push, 1)

/// Fuse definition structure
typedef struct  {
  UINT8                       PPlayTableRev;              ///< PP table revision
  UINT8                       VclkDid[8];                 ///< VCLK DID - entended to 8
  UINT8                       DclkDid[8];                 ///< DCLK DID - entended to 8
  UINT8                       SclkThermDid;               ///< *Thermal SCLK - removed on CZ
  UINT8                       VclkDclkSel[6];             ///< *Vclk/Dclk selector - removed on CZ
  UINT8                       LclkDid[8];                 ///< Lclk DPM DID - entended to 8
  UINT8                       DispClkDid[8];              ///< DispClk DID - entended to 8
  UINT8                       PcieGen2Vid;                ///< Pcie Gen 2 VID
  UINT8                       MainPllId;                  ///< *Main PLL Id from fuses - removed on CZ
  UINT8                       WrCkDid;                    ///< *WRCK SMU clock Divisor - removed on CZ
  UINT8                       GpuBoostCap;                ///< *GPU boost cap - removed on CZ
  UINT32                      SclkDpmBoostMargin;         ///< *Boost margin - removed on CZ
  UINT32                      SclkDpmThrottleMargin;      ///< *Throttle margin - removed on CZ
  BOOLEAN                     VceStateTableSupport;       ///< Support VCE in PP table
  UINT8                       VceFlags[8];                ///< VCE Flags - entended to 8
  UINT8                       VceMclk;                    ///< MCLK for VCE bit map
  UINT8                       VceMclk4;                   ///< MCLK for VCE bit map -- New added for CZ, entended bit 4
  UINT8                       VceMclk5;                   ///< MCLK for VCE bit map -- New added for CZ, entended bit 5
  UINT8                       VceMclk6;                   ///< MCLK for VCE bit map -- New added for CZ, entended bit 6
  UINT8                       VceMclk7;                   ///< MCLK for VCE bit map -- New added for CZ, entended bit 7
  UINT8                       VceReqSclkSel[8];           ///< SCLK selector for VCE - - entended to 8
  UINT8                       EclkDid[8];                 ///< Eclk DID - entended to 8
  UINT8                       HtcEn;                      ///< HTC Enabled
  UINT8                       HtcTmpLmt;                  ///< HTC Temperature Limit
  UINT8                       HtcHystLmt;                 ///< HTC Hysteresis Limit
  UINT8                       MainPllOpFreqIdStartup;     ///< GCK_PLL_FUSES
  UINT8                       DisDllShutdownSR[4];        ///< DRAM Configuration Low [per DCT]
  UINT8                       MemClkFreq[4];              ///< DRAM Configuration High [per DCT]
  UINT8                       MemPhyPllPdMode[4];         ///< *DRAM Controller Miscellaneous 2 [per DCT] - removed on CZ
  UINT8                       M1MemClkFreq[4];            ///< Memory PState Control and Status [per DCT]
  UINT8                       NbPstateEn[4];              ///< NB PState Enable
  UINT8                       NbVid_6_0[4];               ///< NB VID [6:0]
  UINT8                       NbVid_7[4];                 ///< NB VID [7]
  UINT8                       MemPstate[4];               ///< Memory P State
  UINT8                       NbFid[4];                   ///< NB FID
  UINT8                       NbDid[4];                   ///< NB DID
  UINT8                       VddNbVid[8];                ///< Sclk VID - entended to 8
  UINT8                       SclkDid[8];                 ///< Sclk DID - entended to 8
  UINT8                       SamClkDid[5];               ///< *SamClk DID - removed on CZ
  UINT8                       VddNbVidOffset[8];          ///< VddNbVid offset - entended to 8
  UINT8                       NumBoostStates;             ///< NumBoostStates
  UINT8                       PerfFlag;                   ///< PerfFlag bits
  UINT8                       PerfFlag5;                  ///< PerfFlag bit -- New added for CZ, entended bit 5
  UINT8                       PerfFlag6;                  ///< PerfFlag bit -- New added for CZ, entended bit 6
  UINT8                       PerfFlag7;                  ///< PerfFlag bit -- New added for CZ, entended bit 7
  UINT8                       PowerSavingsFlag;           ///< PS Flag bits
  UINT8                       PowerSavingsFlag5;          ///< PS Flag bits -- New added for CZ, entended bit 5
  UINT8                       PowerSavingsFlag6;          ///< PS Flag bits -- New added for CZ, entended bit 6
  UINT8                       PowerSavingsFlag7;          ///< PS Flag bits -- New added for CZ, entended bit 7
  UINT8                       AcpClkDid[8];               ///< Acp Clk DID - entended to 8
  UINT8                       PcieGen3Vid;                ///< Pcie Gen 3 VID
  UINT8                       GfxVid[8];                  ///< Gfx VID - New added for CZ, entended to 8
  UINT8                       GfxVidOffset[8];            ///< GfxVidOffset - New added for CZ, entended to 8
} PP_FUSE_ARRAY_V2;

#pragma pack (pop)

#endif


