/*****************************************************************************
 *
 * Copyright 2008 - 2019 ADVANCED MICRO DEVICES, INC.  All Rights Reserved.
 *
 * AMD is granting you permission to use this software and documentation (if
 * any) (collectively, the "Materials") pursuant to the terms and conditions of
 * the Software License Agreement included with the Materials.  If you do not
 * have a copy of the Software License Agreement, contact your AMD
 * representative for a copy.
 *
 * You agree that you will not reverse engineer or decompile the Materials, in
 * whole or in part, except as allowed by applicable law.
 *
 * WARRANTY DISCLAIMER:  THE MATERIALS ARE PROVIDED "AS IS" WITHOUT WARRANTY OF
 * ANY KIND.  AMD DISCLAIMS ALL WARRANTIES, EXPRESS, IMPLIED, OR STATUTORY,
 * INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, TITLE, NON-INFRINGEMENT, THAT THE
 * MATERIALS WILL RUN UNINTERRUPTED OR ERROR-FREE OR WARRANTIES ARISING FROM
 * CUSTOM OF TRADE OR COURSE OF USAGE.  THE ENTIRE RISK ASSOCIATED WITH THE USE
 * OF THE MATERIAL IS ASSUMED BY YOU.  Some jurisdictions do not allow the
 * exclusion of implied warranties, so the above exclusion may not apply to
 * You.
 *
 * LIMITATION OF LIABILITY AND INDEMNIFICATION:  AMD AND ITS LICENSORS WILL
 * NOT, UNDER ANY CIRCUMSTANCES BE LIABLE TO YOU FOR ANY PUNITIVE, DIRECT,
 * INCIDENTAL, INDIRECT, SPECIAL OR CONSEQUENTIAL DAMAGES ARISING FROM USE OF
 * THE MATERIALS OR THIS AGREEMENT EVEN IF AMD AND ITS LICENSORS HAVE BEEN
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.  In no event shall AMD's total
 * liability to You for all damages, losses, and causes of action (whether in
 * contract, tort (including negligence) or otherwise) exceed the amount of
 * $100 USD. You agree to defend, indemnify and hold harmless AMD and its
 * licensors, and any of their directors, officers, employees, affiliates or
 * agents from and against any and all loss, damage, liability and other
 * expenses (including reasonable attorneys' fees), resulting from Your use of
 * the Materials or violation of the terms and conditions of this Agreement.
 *
 * U.S. GOVERNMENT RESTRICTED RIGHTS:  The Materials are provided with
 * "RESTRICTED RIGHTS." Use, duplication, or disclosure by the Government is
 * subject to the restrictions as set forth in FAR 52.227-14 and
 * DFAR252.227-7013, et seq., or its successor.  Use of the Materials by the
 * Government constitutes acknowledgment of AMD's proprietary rights in them.
 *
 * EXPORT RESTRICTIONS: The Materials may be subject to export restrictions as
 * stated in the Software License Agreement.
 ***************************************************************************/


//This file is auto generated, don't edit it manually

#include "AmdCbsVariable.h"
#include "IdsNvIdRV.h"
#include "IdsNvTable.h"

UINT32 GetIdsNvRecordsSize ();

#pragma pack(1)
typedef struct {
  IDS_NV_ID     Id;
  UINT16        Offset;
} IDS_NV_RECORD;
#pragma pack()

#ifndef OFFSET_OF
  #define OFFSET_OF(type, member) ((UINT8) &(((type*)0)->member))
#endif

STATIC IDS_NV_RECORD mIdsNv8[] = {
  //RedirectForReturnDis
  {IDSNVID_CMN_CPU_GEN_W_A05, OFFSET_OF(CBS_CONFIG,CbsCmnCpuGenWA05)},
  //L2 TLB Associativity
  {IDSNVID_CMN_CPU_L2_TLB_WAY_ALLOC, OFFSET_OF(CBS_CONFIG,CbsCmnCpuL2TlbWayAlloc)},
  //Platform First Error Handling
  {IDSNVID_CMN_CPU_PFEH, OFFSET_OF(CBS_CONFIG,CbsCmnCpuPfeh)},
  //Core Performance Boost
  {IDSNVID_CMN_CPU_CPB, OFFSET_OF(CBS_CONFIG,CbsCmnCpuCpb)},
  //Global C-state Control
  {IDSNVID_CMN_CPU_GLOBAL_CSTATE_CTRL, OFFSET_OF(CBS_CONFIG,CbsCmnCpuGlobalCstateCtrl)},
  //OC Mode
  {IDSNVID_CMN_CPU_OC_MODE, OFFSET_OF(CBS_CONFIG,CbsCmnCpuOcMode)},
  //Streaming Stores Control
  {IDSNVID_CMN_CPU_STREAMING_STORES_CTRL, OFFSET_OF(CBS_CONFIG,CbsCmnCpuStreamingStoresCtrl)},
  //Enable IBS
  {IDSNVID_CMN_CPU_EN_IBS, OFFSET_OF(CBS_CONFIG,CbsCmnCpuEnIbs)},
  //RPMC Control
  {IDSNVID_CMN_SOC_RPMC_CONTROL, OFFSET_OF(CBS_CONFIG,CbsCmnSocRpmcControl)},
  //Custom Core Pstates Legal Disclaimer
  {IDSNVID_CPU_LEGAL_DISCLAIMER, OFFSET_OF(CBS_CONFIG,CbsCpuLegalDisclaimer)},
  //Custom Core Pstates Legal Disclaimer 1
  {IDSNVID_CPU_LEGAL_DISCLAIMER1, OFFSET_OF(CBS_CONFIG,CbsCpuLegalDisclaimer1)},
  //Custom Pstate0
  {IDSNVID_CPU_PST_CUSTOM_P0, OFFSET_OF(CBS_CONFIG,CbsCpuPstCustomP0)},
  //Pstate0 FID
  {IDSNVID_CPU_PST0_FID, OFFSET_OF(CBS_CONFIG,CbsCpuPst0Fid)},
  //Pstate0 DID
  {IDSNVID_CPU_PST0_DID, OFFSET_OF(CBS_CONFIG,CbsCpuPst0Did)},
  //Pstate0 VID
  {IDSNVID_CPU_PST0_VID, OFFSET_OF(CBS_CONFIG,CbsCpuPst0Vid)},
  //Custom Pstate1
  {IDSNVID_CPU_PST_CUSTOM_P1, OFFSET_OF(CBS_CONFIG,CbsCpuPstCustomP1)},
  //Pstate1 FID
  {IDSNVID_CPU_PST1_FID, OFFSET_OF(CBS_CONFIG,CbsCpuPst1Fid)},
  //Pstate1 DID
  {IDSNVID_CPU_PST1_DID, OFFSET_OF(CBS_CONFIG,CbsCpuPst1Did)},
  //Pstate1 VID
  {IDSNVID_CPU_PST1_VID, OFFSET_OF(CBS_CONFIG,CbsCpuPst1Vid)},
  //Custom Pstate2
  {IDSNVID_CPU_PST_CUSTOM_P2, OFFSET_OF(CBS_CONFIG,CbsCpuPstCustomP2)},
  //Pstate2 FID
  {IDSNVID_CPU_PST2_FID, OFFSET_OF(CBS_CONFIG,CbsCpuPst2Fid)},
  //Pstate2 DID
  {IDSNVID_CPU_PST2_DID, OFFSET_OF(CBS_CONFIG,CbsCpuPst2Did)},
  //Pstate2 VID
  {IDSNVID_CPU_PST2_VID, OFFSET_OF(CBS_CONFIG,CbsCpuPst2Vid)},
  //Custom Pstate3
  {IDSNVID_CPU_PST_CUSTOM_P3, OFFSET_OF(CBS_CONFIG,CbsCpuPstCustomP3)},
  //Pstate3 FID
  {IDSNVID_CPU_PST3_FID, OFFSET_OF(CBS_CONFIG,CbsCpuPst3Fid)},
  //Pstate3 DID
  {IDSNVID_CPU_PST3_DID, OFFSET_OF(CBS_CONFIG,CbsCpuPst3Did)},
  //Pstate3 VID
  {IDSNVID_CPU_PST3_VID, OFFSET_OF(CBS_CONFIG,CbsCpuPst3Vid)},
  //Custom Pstate4
  {IDSNVID_CPU_PST_CUSTOM_P4, OFFSET_OF(CBS_CONFIG,CbsCpuPstCustomP4)},
  //Pstate4 FID
  {IDSNVID_CPU_PST4_FID, OFFSET_OF(CBS_CONFIG,CbsCpuPst4Fid)},
  //Pstate4 DID
  {IDSNVID_CPU_PST4_DID, OFFSET_OF(CBS_CONFIG,CbsCpuPst4Did)},
  //Pstate4 VID
  {IDSNVID_CPU_PST4_VID, OFFSET_OF(CBS_CONFIG,CbsCpuPst4Vid)},
  //Custom Pstate5
  {IDSNVID_CPU_PST_CUSTOM_P5, OFFSET_OF(CBS_CONFIG,CbsCpuPstCustomP5)},
  //Pstate5 FID
  {IDSNVID_CPU_PST5_FID, OFFSET_OF(CBS_CONFIG,CbsCpuPst5Fid)},
  //Pstate5 DID
  {IDSNVID_CPU_PST5_DID, OFFSET_OF(CBS_CONFIG,CbsCpuPst5Did)},
  //Pstate5 VID
  {IDSNVID_CPU_PST5_VID, OFFSET_OF(CBS_CONFIG,CbsCpuPst5Vid)},
  //Custom Pstate6
  {IDSNVID_CPU_PST_CUSTOM_P6, OFFSET_OF(CBS_CONFIG,CbsCpuPstCustomP6)},
  //Pstate6 FID
  {IDSNVID_CPU_PST6_FID, OFFSET_OF(CBS_CONFIG,CbsCpuPst6Fid)},
  //Pstate6 DID
  {IDSNVID_CPU_PST6_DID, OFFSET_OF(CBS_CONFIG,CbsCpuPst6Did)},
  //Pstate6 VID
  {IDSNVID_CPU_PST6_VID, OFFSET_OF(CBS_CONFIG,CbsCpuPst6Vid)},
  //Custom Pstate7
  {IDSNVID_CPU_PST_CUSTOM_P7, OFFSET_OF(CBS_CONFIG,CbsCpuPstCustomP7)},
  //Pstate7 FID
  {IDSNVID_CPU_PST7_FID, OFFSET_OF(CBS_CONFIG,CbsCpuPst7Fid)},
  //Pstate7 DID
  {IDSNVID_CPU_PST7_DID, OFFSET_OF(CBS_CONFIG,CbsCpuPst7Did)},
  //Pstate7 VID
  {IDSNVID_CPU_PST7_VID, OFFSET_OF(CBS_CONFIG,CbsCpuPst7Vid)},
  //Relaxed EDC throttling
  {IDSNVID_CPU_RELAXED_EDC_THROTTLING, OFFSET_OF(CBS_CONFIG,CbsCpuRelaxedEdcThrottling)},
  //Core/Thread Enablement Legal Disclaimer
  {IDSNVID_CPU_CORE_THREAD_LEGAL_DISCLAIMER, OFFSET_OF(CBS_CONFIG,CbsCpuCoreThreadLegalDisclaimer)},
  //Downcore control
  {IDSNVID_CMN_CPU_GEN_DOWNCORE_CTRL, OFFSET_OF(CBS_CONFIG,CbsCmnCpuGenDowncoreCtrl)},
  //SMTEN
  {IDSNVID_CMN_CPU_GEN_SMT_EN, OFFSET_OF(CBS_CONFIG,CbsCmnCpuGenSmtEn)},
  //DRAM scrub time
  {IDSNVID_DF_CMN_DRAM_SCRUB_TIME, OFFSET_OF(CBS_CONFIG,CbsDfCmnDramScrubTime)},
  //Redirect scrubber control
  {IDSNVID_DF_CMN_REDIR_SCRUB_CTRL, OFFSET_OF(CBS_CONFIG,CbsDfCmnRedirScrubCtrl)},
  //Disable DF sync flood propagation
  {IDSNVID_DF_CMN_SYNC_FLOOD_PROP, OFFSET_OF(CBS_CONFIG,CbsDfCmnSyncFloodProp)},
  //GMI encryption control
  {IDSNVID_DF_CMN_GMI_ENCRYPTION, OFFSET_OF(CBS_CONFIG,CbsDfCmnGmiEncryption)},
  //xGMI encryption control
  {IDSNVID_DF_CMN_X_GMI_ENCRYPTION, OFFSET_OF(CBS_CONFIG,CbsDfCmnXGmiEncryption)},
  //CC6 memory region encryption
  {IDSNVID_DF_CMN_CC6_MEM_ENCRYPTION, OFFSET_OF(CBS_CONFIG,CbsDfCmnCc6MemEncryption)},
  //Location of private memory regions
  {IDSNVID_DF_CMN_CC6_ALLOCATION_SCHEME, OFFSET_OF(CBS_CONFIG,CbsDfCmnCc6AllocationScheme)},
  //System probe filter
  {IDSNVID_DF_CMN_SYS_PROBE_FILTER, OFFSET_OF(CBS_CONFIG,CbsDfCmnSysProbeFilter)},
  //Memory interleaving
  {IDSNVID_DF_CMN_MEM_INTLV, OFFSET_OF(CBS_CONFIG,CbsDfCmnMemIntlv)},
  //Memory interleaving size
  {IDSNVID_DF_CMN_MEM_INTLV_SIZE, OFFSET_OF(CBS_CONFIG,CbsDfCmnMemIntlvSize)},
  //Channel interleaving hash
  {IDSNVID_DF_CMN_CHNL_INTLV_HASH, OFFSET_OF(CBS_CONFIG,CbsDfCmnChnlIntlvHash)},
  //DF C-state control
  {IDSNVID_DF_CMN_CSTATE_CTRL, OFFSET_OF(CBS_CONFIG,CbsDfCmnCstateCtrl)},
  //xGMI DLWM control
  {IDSNVID_DF_CMNX_GMI_DLWM_CTRL, OFFSET_OF(CBS_CONFIG,CbsDfCmnxGmiDlwmCtrl)},
  //Freeze DF module queues on error
  {IDSNVID_DF_CMN_FREEZE_QUEUE_ERROR, OFFSET_OF(CBS_CONFIG,CbsDfCmnFreezeQueueError)},
  //SMBUS SPD Voltage
  {IDSNVID_CMN_SPD_SMBUS_VOLTAGE, OFFSET_OF(CBS_CONFIG,CbsCmnSpdSmbusVoltage)},
  //DRAM Timing Configuration Legal Disclaimer
  {IDSNVID_CMN_MEM_TIMING_LEGAL_DISCLAIMER, OFFSET_OF(CBS_CONFIG,CbsCmnMemTimingLegalDisclaimer)},
  //DRAM Timing Configuration Legal Disclaimer 1
  {IDSNVID_CMN_MEM_TIMING_LEGAL_DISCLAIMER1, OFFSET_OF(CBS_CONFIG,CbsCmnMemTimingLegalDisclaimer1)},
  //Overclock
  {IDSNVID_CMN_MEM_OVERCLOCK_DDR4, OFFSET_OF(CBS_CONFIG,CbsCmnMemOverclockDdr4)},
  //Memory Clock Speed
  {IDSNVID_CMN_MEM_SPEED_DDR4, OFFSET_OF(CBS_CONFIG,CbsCmnMemSpeedDdr4)},
  //Tcl
  {IDSNVID_CMN_MEM_TIMING_TCL_DDR4, OFFSET_OF(CBS_CONFIG,CbsCmnMemTimingTclDdr4)},
  //Trcdrd
  {IDSNVID_CMN_MEM_TIMING_TRCDRD_DDR4, OFFSET_OF(CBS_CONFIG,CbsCmnMemTimingTrcdrdDdr4)},
  //Trcdwr
  {IDSNVID_CMN_MEM_TIMING_TRCDWR_DDR4, OFFSET_OF(CBS_CONFIG,CbsCmnMemTimingTrcdwrDdr4)},
  //Trp
  {IDSNVID_CMN_MEM_TIMING_TRP_DDR4, OFFSET_OF(CBS_CONFIG,CbsCmnMemTimingTrpDdr4)},
  //Tras
  {IDSNVID_CMN_MEM_TIMING_TRAS_DDR4, OFFSET_OF(CBS_CONFIG,CbsCmnMemTimingTrasDdr4)},
  //Trc Ctrl
  {IDSNVID_CMN_MEM_TIMING_TRC_CTRL_DDR4, OFFSET_OF(CBS_CONFIG,CbsCmnMemTimingTrcCtrlDdr4)},
  //Trc
  {IDSNVID_CMN_MEM_TIMING_TRC_DDR4, OFFSET_OF(CBS_CONFIG,CbsCmnMemTimingTrcDdr4)},
  //TrrdS
  {IDSNVID_CMN_MEM_TIMING_TRRD_S_DDR4, OFFSET_OF(CBS_CONFIG,CbsCmnMemTimingTrrdSDdr4)},
  //TrrdL
  {IDSNVID_CMN_MEM_TIMING_TRRD_L_DDR4, OFFSET_OF(CBS_CONFIG,CbsCmnMemTimingTrrdLDdr4)},
  //Tfaw Ctrl
  {IDSNVID_CMN_MEM_TIMING_TFAW_CTRL_DDR4, OFFSET_OF(CBS_CONFIG,CbsCmnMemTimingTfawCtrlDdr4)},
  //Tfaw
  {IDSNVID_CMN_MEM_TIMING_TFAW_DDR4, OFFSET_OF(CBS_CONFIG,CbsCmnMemTimingTfawDdr4)},
  //TwtrS
  {IDSNVID_CMN_MEM_TIMING_TWTR_S_DDR4, OFFSET_OF(CBS_CONFIG,CbsCmnMemTimingTwtrSDdr4)},
  //TwtrL
  {IDSNVID_CMN_MEM_TIMING_TWTR_L_DDR4, OFFSET_OF(CBS_CONFIG,CbsCmnMemTimingTwtrLDdr4)},
  //Twr Ctrl
  {IDSNVID_CMN_MEM_TIMING_TWR_CTRL_DDR4, OFFSET_OF(CBS_CONFIG,CbsCmnMemTimingTwrCtrlDdr4)},
  //Twr
  {IDSNVID_CMN_MEM_TIMING_TWR_DDR4, OFFSET_OF(CBS_CONFIG,CbsCmnMemTimingTwrDdr4)},
  //Trcpage Ctrl
  {IDSNVID_CMN_MEM_TIMING_TRCPAGE_CTRL_DDR4, OFFSET_OF(CBS_CONFIG,CbsCmnMemTimingTrcpageCtrlDdr4)},
  //TrdrdScL Ctrl
  {IDSNVID_CMN_MEM_TIMING_TRDRD_SC_L_CTRL_DDR4, OFFSET_OF(CBS_CONFIG,CbsCmnMemTimingTrdrdScLCtrlDdr4)},
  //TrdrdScL
  {IDSNVID_CMN_MEM_TIMING_TRDRD_SC_L_DDR4, OFFSET_OF(CBS_CONFIG,CbsCmnMemTimingTrdrdScLDdr4)},
  //TwrwrScL Ctrl
  {IDSNVID_CMN_MEM_TIMING_TWRWR_SC_L_CTRL_DDR4, OFFSET_OF(CBS_CONFIG,CbsCmnMemTimingTwrwrScLCtrlDdr4)},
  //TwrwrScL
  {IDSNVID_CMN_MEM_TIMING_TWRWR_SC_L_DDR4, OFFSET_OF(CBS_CONFIG,CbsCmnMemTimingTwrwrScLDdr4)},
  //Trfc Ctrl
  {IDSNVID_CMN_MEM_TIMING_TRFC_CTRL_DDR4, OFFSET_OF(CBS_CONFIG,CbsCmnMemTimingTrfcCtrlDdr4)},
  //Trfc2 Ctrl
  {IDSNVID_CMN_MEM_TIMING_TRFC2_CTRL_DDR4, OFFSET_OF(CBS_CONFIG,CbsCmnMemTimingTrfc2CtrlDdr4)},
  //Trfc4 Ctrl
  {IDSNVID_CMN_MEM_TIMING_TRFC4_CTRL_DDR4, OFFSET_OF(CBS_CONFIG,CbsCmnMemTimingTrfc4CtrlDdr4)},
  //Fail_CNT
  {IDSNVID_CMN_MEM_OVERCLOCK_FAIL_CNT, OFFSET_OF(CBS_CONFIG,CbsCmnMemOverclockFailCNT)},
  //ProcODT
  {IDSNVID_CMN_MEM_CTRLLER_PROC_ODT_DDR4, OFFSET_OF(CBS_CONFIG,CbsCmnMemCtrllerProcOdtDdr4)},
  //Tcwl
  {IDSNVID_CMN_MEM_TIMING_TCWL_DDR4, OFFSET_OF(CBS_CONFIG,CbsCmnMemTimingTcwlDdr4)},
  //Trtp
  {IDSNVID_CMN_MEM_TIMING_TRTP_DDR4, OFFSET_OF(CBS_CONFIG,CbsCmnMemTimingTrtpDdr4)},
  //Tcke
  {IDSNVID_CMN_MEM_TIMING_TCKE_DDR4, OFFSET_OF(CBS_CONFIG,CbsCmnMemTimingTckeDdr4)},
  //TrdrdSc
  {IDSNVID_CMN_MEM_TIMING_TRDRD_SC_DDR4, OFFSET_OF(CBS_CONFIG,CbsCmnMemTimingTrdrdScDdr4)},
  //TrdrdSd
  {IDSNVID_CMN_MEM_TIMING_TRDRD_SD_DDR4, OFFSET_OF(CBS_CONFIG,CbsCmnMemTimingTrdrdSdDdr4)},
  //TrdrdDd
  {IDSNVID_CMN_MEM_TIMING_TRDRD_DD_DDR4, OFFSET_OF(CBS_CONFIG,CbsCmnMemTimingTrdrdDdDdr4)},
  //TwrwrSc
  {IDSNVID_CMN_MEM_TIMING_TWRWR_SC_DDR4, OFFSET_OF(CBS_CONFIG,CbsCmnMemTimingTwrwrScDdr4)},
  //TwrwrSd
  {IDSNVID_CMN_MEM_TIMING_TWRWR_SD_DDR4, OFFSET_OF(CBS_CONFIG,CbsCmnMemTimingTwrwrSdDdr4)},
  //TwrwrDd
  {IDSNVID_CMN_MEM_TIMING_TWRWR_DD_DDR4, OFFSET_OF(CBS_CONFIG,CbsCmnMemTimingTwrwrDdDdr4)},
  //Trdwr
  {IDSNVID_CMN_MEM_TIMING_TRDWR_DDR4, OFFSET_OF(CBS_CONFIG,CbsCmnMemTimingTrdwrDdr4)},
  //Twrrd
  {IDSNVID_CMN_MEM_TIMING_TWRRD_DDR4, OFFSET_OF(CBS_CONFIG,CbsCmnMemTimingTwrrdDdr4)},
  //Cmd2T
  {IDSNVID_CMN_MEM_CTRLLER2_T_MODE_DDR4, OFFSET_OF(CBS_CONFIG,CbsCmnMemCtrller2TModeDdr4)},
  //Gear Down Mode
  {IDSNVID_CMN_MEM_GEAR_DOWN_MODE_DDR4, OFFSET_OF(CBS_CONFIG,CbsCmnMemGearDownModeDdr4)},
  //Data Mask
  {IDSNVID_CMN_MEM_DATA_MASK_DDR4, OFFSET_OF(CBS_CONFIG,CbsCmnMemDataMaskDdr4)},
  //Power Down Enable
  {IDSNVID_CMN_MEM_CTRLLER_PWR_DN_EN_DDR4, OFFSET_OF(CBS_CONFIG,CbsCmnMemCtrllerPwrDnEnDdr4)},
  //CAD Bus Timing User Controls
  {IDSNVID_CMN_MEM_CAD_BUS_TIMING_CTL_DDR4, OFFSET_OF(CBS_CONFIG,CbsCmnMemCadBusTimingCtlDdr4)},
  //AddrCmdSetup
  {IDSNVID_CMN_MEM_ADDR_CMD_SETUP_DDR4, OFFSET_OF(CBS_CONFIG,CbsCmnMemAddrCmdSetupDdr4)},
  //CsOdtSetup
  {IDSNVID_CMN_MEM_CS_ODT_SETUP_DDR4, OFFSET_OF(CBS_CONFIG,CbsCmnMemCsOdtSetupDdr4)},
  //CkeSetup
  {IDSNVID_CMN_MEM_CKE_SETUP_DDR4, OFFSET_OF(CBS_CONFIG,CbsCmnMemCkeSetupDdr4)},
  //CAD Bus Drive Strength User Controls
  {IDSNVID_CMN_MEM_CAD_BUS_DRV_STREN_CTL_DDR4, OFFSET_OF(CBS_CONFIG,CbsCmnMemCadBusDrvStrenCtlDdr4)},
  //ClkDrvStren
  {IDSNVID_CMN_MEM_CAD_BUS_CLK_DRV_STREN_DDR4, OFFSET_OF(CBS_CONFIG,CbsCmnMemCadBusClkDrvStrenDdr4)},
  //AddrCmdDrvStren
  {IDSNVID_CMN_MEM_CAD_BUS_ADDR_CMD_DRV_STREN_DDR4, OFFSET_OF(CBS_CONFIG,CbsCmnMemCadBusAddrCmdDrvStrenDdr4)},
  //CsOdtDrvStren
  {IDSNVID_CMN_MEM_CAD_BUS_CS_ODT_DRV_STREN_DDR4, OFFSET_OF(CBS_CONFIG,CbsCmnMemCadBusCsOdtDrvStrenDdr4)},
  //CkeDrvStren
  {IDSNVID_CMN_MEM_CAD_BUS_CKE_DRV_STREN_DDR4, OFFSET_OF(CBS_CONFIG,CbsCmnMemCadBusCkeDrvStrenDdr4)},
  //Data Bus Configuration User Controls
  {IDSNVID_CMN_MEM_DATA_BUS_CONFIG_CTL_DDR4, OFFSET_OF(CBS_CONFIG,CbsCmnMemDataBusConfigCtlDdr4)},
  //RttNom
  {IDSNVID_CMN_MEM_CTRLLER_RTT_NOM_DDR4, OFFSET_OF(CBS_CONFIG,CbsCmnMemCtrllerRttNomDdr4)},
  //RttWr
  {IDSNVID_CMN_MEM_CTRLLER_RTT_WR_DDR4, OFFSET_OF(CBS_CONFIG,CbsCmnMemCtrllerRttWrDdr4)},
  //RttPark
  {IDSNVID_CMN_MEM_CTRLLER_RTT_PARK_DDR4, OFFSET_OF(CBS_CONFIG,CbsCmnMemCtrllerRttParkDdr4)},
  //Data Poisoning
  {IDSNVID_CMN_MEM_DATA_POISONING_DDR4, OFFSET_OF(CBS_CONFIG,CbsCmnMemDataPoisoningDdr4)},
  //Disable Memory Error Injection
  {IDSNVID_CMN_MEM_DIS_MEM_ERR_INJ, OFFSET_OF(CBS_CONFIG,CbsCmnMemDisMemErrInj)},
  //DRAM ECC Symbol Size
  {IDSNVID_CMN_MEM_CTRLLER_DRAM_ECC_SYMBOL_SIZE_DDR4, OFFSET_OF(CBS_CONFIG,CbsCmnMemCtrllerDramEccSymbolSizeDdr4)},
  //DRAM ECC Enable
  {IDSNVID_CMN_MEM_CTRLLER_DRAM_ECC_EN_DDR4, OFFSET_OF(CBS_CONFIG,CbsCmnMemCtrllerDramEccEnDdr4)},
  //TSME
  {IDSNVID_CMN_MEM_TSME_DDR4, OFFSET_OF(CBS_CONFIG,CbsCmnMemTsmeDdr4)},
  //Chipselect Interleaving
  {IDSNVID_CMN_MEM_MAPPING_BANK_INTERLEAVE_DDR4, OFFSET_OF(CBS_CONFIG,CbsCmnMemMappingBankInterleaveDdr4)},
  //BankGroupSwap
  {IDSNVID_CMN_MEM_CTRLLER_BANK_GROUP_SWAP_DDR4, OFFSET_OF(CBS_CONFIG,CbsCmnMemCtrllerBankGroupSwapDdr4)},
  //BankGroupSwapAlt
  {IDSNVID_CMN_MEM_CTRLLER_BANK_GROUP_SWAP_ALT_DDR4, OFFSET_OF(CBS_CONFIG,CbsCmnMemCtrllerBankGroupSwapAltDdr4)},
  //Address Hash Bank
  {IDSNVID_CMN_MEM_ADDRESS_HASH_BANK_DDR4, OFFSET_OF(CBS_CONFIG,CbsCmnMemAddressHashBankDdr4)},
  //Address Hash CS
  {IDSNVID_CMN_MEM_ADDRESS_HASH_CS_DDR4, OFFSET_OF(CBS_CONFIG,CbsCmnMemAddressHashCsDdr4)},
  //MBIST Enable
  {IDSNVID_CMN_MEM_MBIST_EN, OFFSET_OF(CBS_CONFIG,CbsCmnMemMbistEn)},
  //MBIST Test Mode
  {IDSNVID_CMN_MEM_MBIST_SUB_TEST, OFFSET_OF(CBS_CONFIG,CbsCmnMemMbistSubTest)},
  //MBIST Aggressors
  {IDSNVID_CMN_MEM_MBIST_AGGRESSORS, OFFSET_OF(CBS_CONFIG,CbsCmnMemMbistAggressors)},
  //MBIST Per Bit Slave Die Reporting
  {IDSNVID_CMN_MEM_MBIST_PER_BIT_SLAVE_DIE_REPORTING, OFFSET_OF(CBS_CONFIG,CbsCmnMemMbistPerBitSlaveDieReporting)},
  //System Configuration
  {IDSNVID_CMN_GNB_SMU_SYSTEM_CONFIG, OFFSET_OF(CBS_CONFIG,CbsCmnGnbSMUSystemConfig)},
  //Audio IOs
  {IDSNVID_DBG_GNB_AUDIO_I_OS, OFFSET_OF(CBS_CONFIG,CbsDbgGnbAudioIOs)},
  //Integrated Graphics Controller
  {IDSNVID_CMN_GNB_GFXI_GPU_CONTROL, OFFSET_OF(CBS_CONFIG,CbsCmnGnbGfxiGPU_CONTROL)},
  //UMA Mode
  {IDSNVID_CMN_GNB_GFX_UMA_MODE, OFFSET_OF(CBS_CONFIG,CbsCmnGnbGfxUmaMode)},
  //UMA Version
  {IDSNVID_CMN_GNB_GFX_UMA_VERSION, OFFSET_OF(CBS_CONFIG,CbsCmnGnbGfxUmaVersion)},
  //Display Resulotuon
  {IDSNVID_CMN_GNB_GFX_DISPLAY_RESOLUTION, OFFSET_OF(CBS_CONFIG,CbsCmnGnbGfxDisplayResolution)},
  //UMA Above 4G
  {IDSNVID_DF_CMN_UMA_ABOVE4_G, OFFSET_OF(CBS_CONFIG,CbsDfCmnUmaAbove4G)},
  //NB Azalia
  {IDSNVID_CMN_GNB_GFX_NB_AZALIA, OFFSET_OF(CBS_CONFIG,CbsCmnGnbGfxNbAzalia)},
  //IOMMU
  {IDSNVID_CMN_GNB_NB_IOMMU, OFFSET_OF(CBS_CONFIG,CbsCmnGnbNbIOMMU)},
  //PSPP Policy
  {IDSNVID_CMN_GNB_PCIE_PSPP_POLICY, OFFSET_OF(CBS_CONFIG,CbsCmnGnbPciePsppPolicy)},
  //Fan Control
  {IDSNVID_DBG_FAN_CTL, OFFSET_OF(CBS_CONFIG,CbsDbgFanCtl)},
  //Force PWM Control
  {IDSNVID_DBG_FORCE_PWM_CTL, OFFSET_OF(CBS_CONFIG,CbsDbgForcePwmCtl)},
  //Force PWM
  {IDSNVID_DBG_FORCE_PWM, OFFSET_OF(CBS_CONFIG,CbsDbgForcePwm)},
  //Fan Table Control
  {IDSNVID_DBG_FAN_TABLE_CTL, OFFSET_OF(CBS_CONFIG,CbsDbgFanTableCtl)},
  //Low Temperature
  {IDSNVID_DBG_FAN_TABLE_TEMP_LOW, OFFSET_OF(CBS_CONFIG,CbsDbgFanTableTempLow)},
  //Medium Temperature
  {IDSNVID_DBG_FAN_TABLE_TEMP_MED, OFFSET_OF(CBS_CONFIG,CbsDbgFanTableTempMed)},
  //High Temperature
  {IDSNVID_DBG_FAN_TABLE_TEMP_HIGH, OFFSET_OF(CBS_CONFIG,CbsDbgFanTableTempHigh)},
  //Critical Temperature
  {IDSNVID_DBG_FAN_TABLE_TEMP_CRITICAL, OFFSET_OF(CBS_CONFIG,CbsDbgFanTableTempCritical)},
  //Low Pwm
  {IDSNVID_DBG_FAN_TABLE_T_PWM_LOW, OFFSET_OF(CBS_CONFIG,CbsDbgFanTableTPwmLow)},
  //Medium Pwm
  {IDSNVID_DBG_FAN_TABLE_PWM_MED, OFFSET_OF(CBS_CONFIG,CbsDbgFanTablePwmMed)},
  //High Pwm
  {IDSNVID_DBG_FAN_TABLE_PWM_HIGH, OFFSET_OF(CBS_CONFIG,CbsDbgFanTablePwmHigh)},
  //Temperature Hysteresis
  {IDSNVID_DBG_FAN_TABLE_HYST, OFFSET_OF(CBS_CONFIG,CbsDbgFanTableHyst)},
  //Pwm Frequency
  {IDSNVID_DBG_FAN_TABLE_PWM_FREQ, OFFSET_OF(CBS_CONFIG,CbsDbgFanTablePwmFreq)},
  //Fan Polarity
  {IDSNVID_DBG_FAN_POLARITY, OFFSET_OF(CBS_CONFIG,CbsDbgFanPolarity)},
  //SATA Controller
  {IDSNVID_CMN_FCH_SATA_ENABLE, OFFSET_OF(CBS_CONFIG,CbsCmnFchSataEnable)},
  //SATA Mode
  {IDSNVID_CMN_FCH_SATA_CLASS, OFFSET_OF(CBS_CONFIG,CbsCmnFchSataClass)},
  //Sata RAS Support
  {IDSNVID_CMN_FCH_SATA_RAS_SUPPORT, OFFSET_OF(CBS_CONFIG,CbsCmnFchSataRasSupport)},
  //Sata Disabled AHCI Prefetch Function
  {IDSNVID_CMN_FCH_SATA_AHCI_DIS_PREFETCH_FUNCTION, OFFSET_OF(CBS_CONFIG,CbsCmnFchSataAhciDisPrefetchFunction)},
  //Aggresive SATA Device Sleep Port 0
  {IDSNVID_DBG_FCH_SATA_AGGRESIVE_DEV_SLP_P0, OFFSET_OF(CBS_CONFIG,CbsDbgFchSataAggresiveDevSlpP0)},
  //Aggresive SATA Device Sleep Port 1
  {IDSNVID_DBG_FCH_SATA_AGGRESIVE_DEV_SLP_P1, OFFSET_OF(CBS_CONFIG,CbsDbgFchSataAggresiveDevSlpP1)},
  //XHCI0 controller enable
  {IDSNVID_CMN_FCH_USB_XHC_I0_ENABLE, OFFSET_OF(CBS_CONFIG,CbsCmnFchUsbXHCI0Enable)},
  //XHCI1 controller enable
  {IDSNVID_CMN_FCH_USB_XHC_I1_ENABLE, OFFSET_OF(CBS_CONFIG,CbsCmnFchUsbXHCI1Enable)},
  //Ac Loss Control
  {IDSNVID_CMN_FCH_SYSTEM_PWR_FAIL_SHADOW, OFFSET_OF(CBS_CONFIG,CbsCmnFchSystemPwrFailShadow)},
  //I2C 0 Enable
  {IDSNVID_CMN_FCH_I2_C0_CONFIG, OFFSET_OF(CBS_CONFIG,CbsCmnFchI2C0Config)},
  //I2C 1 Enable
  {IDSNVID_CMN_FCH_I2_C1_CONFIG, OFFSET_OF(CBS_CONFIG,CbsCmnFchI2C1Config)},
  //I2C 2 Enable
  {IDSNVID_CMN_FCH_I2_C2_CONFIG, OFFSET_OF(CBS_CONFIG,CbsCmnFchI2C2Config)},
  //I2C 3 Enable
  {IDSNVID_CMN_FCH_I2_C3_CONFIG, OFFSET_OF(CBS_CONFIG,CbsCmnFchI2C3Config)},
  //I2C 4 Enable
  {IDSNVID_CMN_FCH_I2_C4_CONFIG, OFFSET_OF(CBS_CONFIG,CbsCmnFchI2C4Config)},
  //I2C 5 Enable
  {IDSNVID_CMN_FCH_I2_C5_CONFIG, OFFSET_OF(CBS_CONFIG,CbsCmnFchI2C5Config)},
  //Uart 0 Enable
  {IDSNVID_CMN_FCH_UART0_CONFIG, OFFSET_OF(CBS_CONFIG,CbsCmnFchUart0Config)},
  //Uart 0 Legacy Options
  {IDSNVID_CMN_FCH_UART0_LEGACY_CONFIG, OFFSET_OF(CBS_CONFIG,CbsCmnFchUart0LegacyConfig)},
  //Uart 1 Enable
  {IDSNVID_CMN_FCH_UART1_CONFIG, OFFSET_OF(CBS_CONFIG,CbsCmnFchUart1Config)},
  //Uart 1 Legacy Options
  {IDSNVID_CMN_FCH_UART1_LEGACY_CONFIG, OFFSET_OF(CBS_CONFIG,CbsCmnFchUart1LegacyConfig)},
  //Uart 2 Enable (no HW FC)
  {IDSNVID_CMN_FCH_UART2_CONFIG, OFFSET_OF(CBS_CONFIG,CbsCmnFchUart2Config)},
  //Uart 2 Legacy Options
  {IDSNVID_CMN_FCH_UART2_LEGACY_CONFIG, OFFSET_OF(CBS_CONFIG,CbsCmnFchUart2LegacyConfig)},
  //Uart 3 Enable (no HW FC)
  {IDSNVID_CMN_FCH_UART3_CONFIG, OFFSET_OF(CBS_CONFIG,CbsCmnFchUart3Config)},
  //Uart 3 Legacy Options
  {IDSNVID_CMN_FCH_UART3_LEGACY_CONFIG, OFFSET_OF(CBS_CONFIG,CbsCmnFchUart3LegacyConfig)},
  //Uart Driver Type
  {IDSNVID_CMN_FCH_UART_DRIVER_TYPE, OFFSET_OF(CBS_CONFIG,CbsCmnFchUartDriverType)},
  //ESPI Enable
  {IDSNVID_CMN_FCH_ESPI_CONFIG, OFFSET_OF(CBS_CONFIG,CbsCmnFchEspiConfig)},
  //XGBE0 enable
  {IDSNVID_CMN_FCH_XGBE0_ENABLE, OFFSET_OF(CBS_CONFIG,CbsCmnFchXgbe0Enable)},
  //XGBE1 enable
  {IDSNVID_CMN_FCH_XGBE1_ENABLE, OFFSET_OF(CBS_CONFIG,CbsCmnFchXgbe1Enable)},
  //eMMC/SD Configure
  {IDSNVID_DBG_FCH_EMMC_ENABLE, OFFSET_OF(CBS_CONFIG,CbsDbgFchEmmcEnable)},
  //Driver Type
  {IDSNVID_DBG_FCH_EMMC_DRIVER_TYPE, OFFSET_OF(CBS_CONFIG,CbsDbgFchEmmcDriverType)},
  //D3 Cold Support
  {IDSNVID_DBG_FCH_EMMC_D3_SUPPORT, OFFSET_OF(CBS_CONFIG,CbsDbgFchEmmcD3Support)},
  //eMMC Boot
  {IDSNVID_DBG_FCH_EMMC_BOOT, OFFSET_OF(CBS_CONFIG,CbsDbgFchEmmcBoot)},
  //LPC Clock Run control
  {IDSNVID_CMN_FCH_LPC_CLOCK_RUN_EN, OFFSET_OF(CBS_CONFIG,CbsCmnFchLpcClockRunEn)},
  //Toggle All PwrGood On Cf9
  {IDSNVID_CMN_TOGGLE_ALL_PWR_GOOD_ON_CF9, OFFSET_OF(CBS_CONFIG,CbsCmnToggleAllPwrGoodOnCf9)},
  //PT SATA Mode
  {IDSNVID_CMN_PT_SATA_CLASS, OFFSET_OF(CBS_CONFIG,CbsCmnPTSataClass)},
  //PT Aggresive SATA Device Sleep Port 0
  {IDSNVID_DBG_PT_SATA_AGGRESIVE_DEV_SLP_P0, OFFSET_OF(CBS_CONFIG,CbsDbgPTSataAggresiveDevSlpP0)},
  //PT Aggresive SATA Device Sleep Port 1
  {IDSNVID_DBG_PT_SATA_AGGRESIVE_DEV_SLP_P1, OFFSET_OF(CBS_CONFIG,CbsDbgPTSataAggresiveDevSlpP1)},
  //PT SATA Port 0 Enable
  {IDSNVID_CMN_PT_SATA_PORT0_ENABLE, OFFSET_OF(CBS_CONFIG,CbsCmnPTSataPort0Enable)},
  //PT SATA Port 1 Enable
  {IDSNVID_CMN_PT_SATA_PORT1_ENABLE, OFFSET_OF(CBS_CONFIG,CbsCmnPTSataPort1Enable)},
  //PT SATA Port 2 Enable
  {IDSNVID_CMN_PT_SATA_PORT2_ENABLE, OFFSET_OF(CBS_CONFIG,CbsCmnPTSataPort2Enable)},
  //PT SATA Port 3 Enable
  {IDSNVID_CMN_PT_SATA_PORT3_ENABLE, OFFSET_OF(CBS_CONFIG,CbsCmnPTSataPort3Enable)},
  //PT SATA Port 4 Enable
  {IDSNVID_CMN_PT_SATA_PORT4_ENABLE, OFFSET_OF(CBS_CONFIG,CbsCmnPTSataPort4Enable)},
  //PT SATA Port 5 Enable
  {IDSNVID_CMN_PT_SATA_PORT5_ENABLE, OFFSET_OF(CBS_CONFIG,CbsCmnPTSataPort5Enable)},
  //PT SATA Port 6 Enable
  {IDSNVID_CMN_PT_SATA_PORT6_ENABLE, OFFSET_OF(CBS_CONFIG,CbsCmnPTSataPort6Enable)},
  //PT SATA Port 7 Enable
  {IDSNVID_CMN_PT_SATA_PORT7_ENABLE, OFFSET_OF(CBS_CONFIG,CbsCmnPTSataPort7Enable)},
  //PT XHCI GEN1
  {IDSNVID_DBG_PT_XHCI_GEN1, OFFSET_OF(CBS_CONFIG,CbsDbgPTXhciGen1)},
  //PT XHCI GEN2
  {IDSNVID_DBG_PT_XHCI_GEN2, OFFSET_OF(CBS_CONFIG,CbsDbgPTXhciGen2)},
  //PT USB Equalization4
  {IDSNVID_DBG_PT_USB_EQUALIZATION4, OFFSET_OF(CBS_CONFIG,CbsDbgPTUsbEqualization4)},
  //PT USB Redriver
  {IDSNVID_DBG_PT_USB_REDRIVER, OFFSET_OF(CBS_CONFIG,CbsDbgPTUsbRedriver)},
  //PT USB31 PORT0
  {IDSNVID_DBG_PT_USB31_P0, OFFSET_OF(CBS_CONFIG,CbsDbgPTUsb31P0)},
  //PT USB31 PORT1
  {IDSNVID_DBG_PT_USB31_P1, OFFSET_OF(CBS_CONFIG,CbsDbgPTUsb31P1)},
  //PT USB30 PORT0
  {IDSNVID_DBG_PT_USB30_P0, OFFSET_OF(CBS_CONFIG,CbsDbgPTUsb30P0)},
  //PT USB30 PORT1
  {IDSNVID_DBG_PT_USB30_P1, OFFSET_OF(CBS_CONFIG,CbsDbgPTUsb30P1)},
  //PT USB30 PORT2
  {IDSNVID_DBG_PT_USB30_P2, OFFSET_OF(CBS_CONFIG,CbsDbgPTUsb30P2)},
  //PT USB30 PORT3
  {IDSNVID_DBG_PT_USB30_P3, OFFSET_OF(CBS_CONFIG,CbsDbgPTUsb30P3)},
  //PT USB30 PORT4
  {IDSNVID_DBG_PT_USB30_P4, OFFSET_OF(CBS_CONFIG,CbsDbgPTUsb30P4)},
  //PT USB30 PORT5
  {IDSNVID_DBG_PT_USB30_P5, OFFSET_OF(CBS_CONFIG,CbsDbgPTUsb30P5)},
  //PT USB20 PORT0
  {IDSNVID_DBG_PT_USB20_P0, OFFSET_OF(CBS_CONFIG,CbsDbgPTUsb20P0)},
  //PT USB20 PORT1
  {IDSNVID_DBG_PT_USB20_P1, OFFSET_OF(CBS_CONFIG,CbsDbgPTUsb20P1)},
  //PT USB20 PORT2
  {IDSNVID_DBG_PT_USB20_P2, OFFSET_OF(CBS_CONFIG,CbsDbgPTUsb20P2)},
  //PT USB20 PORT3
  {IDSNVID_DBG_PT_USB20_P3, OFFSET_OF(CBS_CONFIG,CbsDbgPTUsb20P3)},
  //PT USB20 PORT4
  {IDSNVID_DBG_PT_USB20_P4, OFFSET_OF(CBS_CONFIG,CbsDbgPTUsb20P4)},
  //PT USB20 PORT5
  {IDSNVID_DBG_PT_USB20_P5, OFFSET_OF(CBS_CONFIG,CbsDbgPTUsb20P5)},
  //PT USB31 PORT0
  {IDSNVID_DBG_PT_PROM2_USB31_P0, OFFSET_OF(CBS_CONFIG,CbsDbgPTProm2Usb31P0)},
  //PT USB31 PORT1
  {IDSNVID_DBG_PT_PROM2_USB31_P1, OFFSET_OF(CBS_CONFIG,CbsDbgPTProm2Usb31P1)},
  //PT USB30 PORT0
  {IDSNVID_DBG_PT_PROM2_USB30_P0, OFFSET_OF(CBS_CONFIG,CbsDbgPTProm2Usb30P0)},
  //PT USB30 PORT1
  {IDSNVID_DBG_PT_PROM2_USB30_P1, OFFSET_OF(CBS_CONFIG,CbsDbgPTProm2Usb30P1)},
  //PT USB20 PORT0
  {IDSNVID_DBG_PT_PROM2_USB20_P0, OFFSET_OF(CBS_CONFIG,CbsDbgPTProm2Usb20P0)},
  //PT USB20 PORT1
  {IDSNVID_DBG_PT_PROM2_USB20_P1, OFFSET_OF(CBS_CONFIG,CbsDbgPTProm2Usb20P1)},
  //PT USB20 PORT2
  {IDSNVID_DBG_PT_PROM2_USB20_P2, OFFSET_OF(CBS_CONFIG,CbsDbgPTProm2Usb20P2)},
  //PT USB20 PORT3
  {IDSNVID_DBG_PT_PROM2_USB20_P3, OFFSET_OF(CBS_CONFIG,CbsDbgPTProm2Usb20P3)},
  //PT USB20 PORT4
  {IDSNVID_DBG_PT_PROM2_USB20_P4, OFFSET_OF(CBS_CONFIG,CbsDbgPTProm2Usb20P4)},
  //PT USB20 PORT5
  {IDSNVID_DBG_PT_PROM2_USB20_P5, OFFSET_OF(CBS_CONFIG,CbsDbgPTProm2Usb20P5)},
  //PT USB31 PORT0
  {IDSNVID_DBG_PT_PROM1_USB31_P0, OFFSET_OF(CBS_CONFIG,CbsDbgPTProm1Usb31P0)},
  //PT USB31 PORT1
  {IDSNVID_DBG_PT_PROM1_USB31_P1, OFFSET_OF(CBS_CONFIG,CbsDbgPTProm1Usb31P1)},
  //PT USB30 PORT0
  {IDSNVID_DBG_PT_PROM1_USB30_P0, OFFSET_OF(CBS_CONFIG,CbsDbgPTProm1Usb30P0)},
  //PT USB20 PORT0
  {IDSNVID_DBG_PT_PROM1_USB20_P0, OFFSET_OF(CBS_CONFIG,CbsDbgPTProm1Usb20P0)},
  //PT USB20 PORT1
  {IDSNVID_DBG_PT_PROM1_USB20_P1, OFFSET_OF(CBS_CONFIG,CbsDbgPTProm1Usb20P1)},
  //PT USB20 PORT2
  {IDSNVID_DBG_PT_PROM1_USB20_P2, OFFSET_OF(CBS_CONFIG,CbsDbgPTProm1Usb20P2)},
  //PT USB20 PORT3
  {IDSNVID_DBG_PT_PROM1_USB20_P3, OFFSET_OF(CBS_CONFIG,CbsDbgPTProm1Usb20P3)},
  //PT USB20 PORT4
  {IDSNVID_DBG_PT_PROM1_USB20_P4, OFFSET_OF(CBS_CONFIG,CbsDbgPTProm1Usb20P4)},
  //PT USB20 PORT5
  {IDSNVID_DBG_PT_PROM1_USB20_P5, OFFSET_OF(CBS_CONFIG,CbsDbgPTProm1Usb20P5)},
  //PT PCIE Port 0 Enable
  {IDSNVID_CMN_PT_PCIE_PORT0_ENABLE, OFFSET_OF(CBS_CONFIG,CbsCmnPTPciePort0Enable)},
  //PT PCIE Port 1 Enable
  {IDSNVID_CMN_PT_PCIE_PORT1_ENABLE, OFFSET_OF(CBS_CONFIG,CbsCmnPTPciePort1Enable)},
  //PT PCIE Port 2 Enable
  {IDSNVID_CMN_PT_PCIE_PORT2_ENABLE, OFFSET_OF(CBS_CONFIG,CbsCmnPTPciePort2Enable)},
  //PT PCIE Port 3 Enable
  {IDSNVID_CMN_PT_PCIE_PORT3_ENABLE, OFFSET_OF(CBS_CONFIG,CbsCmnPTPciePort3Enable)},
  //PT PCIE Port 4 Enable
  {IDSNVID_CMN_PT_PCIE_PORT4_ENABLE, OFFSET_OF(CBS_CONFIG,CbsCmnPTPciePort4Enable)},
  //PT PCIE Port 5 Enable
  {IDSNVID_CMN_PT_PCIE_PORT5_ENABLE, OFFSET_OF(CBS_CONFIG,CbsCmnPTPciePort5Enable)},
  //PT PCIE Port 6 Enable
  {IDSNVID_CMN_PT_PCIE_PORT6_ENABLE, OFFSET_OF(CBS_CONFIG,CbsCmnPTPciePort6Enable)},
  //PT PCIE Port 7 Enable
  {IDSNVID_CMN_PT_PCIE_PORT7_ENABLE, OFFSET_OF(CBS_CONFIG,CbsCmnPTPciePort7Enable)},
  //End
  {0xFFFF, 0}};

STATIC IDS_NV_RECORD mIdsNv16[] = {
  //Trcpage
  {IDSNVID_CMN_MEM_TIMING_TRCPAGE_DDR4, OFFSET_OF(CBS_CONFIG,CbsCmnMemTimingTrcpageDdr4)},
  //Trfc
  {IDSNVID_CMN_MEM_TIMING_TRFC_DDR4, OFFSET_OF(CBS_CONFIG,CbsCmnMemTimingTrfcDdr4)},
  //Trfc2
  {IDSNVID_CMN_MEM_TIMING_TRFC2_DDR4, OFFSET_OF(CBS_CONFIG,CbsCmnMemTimingTrfc2Ddr4)},
  //Trfc4
  {IDSNVID_CMN_MEM_TIMING_TRFC4_DDR4, OFFSET_OF(CBS_CONFIG,CbsCmnMemTimingTrfc4Ddr4)},
  //End
  {0xFFFF, 0}};

STATIC IDS_NV_RECORD mIdsNv32[] = {
  //Frequency (MHz)
  {IDSNVID_CPU_COF_P0, OFFSET_OF(CBS_CONFIG,CbsCpuCofP0)},
  //Voltage (uV)
  {IDSNVID_CPU_VOLTAGE_P0, OFFSET_OF(CBS_CONFIG,CbsCpuVoltageP0)},
  //Frequency (MHz)
  {IDSNVID_CPU_COF_P1, OFFSET_OF(CBS_CONFIG,CbsCpuCofP1)},
  //Voltage (uV)
  {IDSNVID_CPU_VOLTAGE_P1, OFFSET_OF(CBS_CONFIG,CbsCpuVoltageP1)},
  //Frequency (MHz)
  {IDSNVID_CPU_COF_P2, OFFSET_OF(CBS_CONFIG,CbsCpuCofP2)},
  //Voltage (uV)
  {IDSNVID_CPU_VOLTAGE_P2, OFFSET_OF(CBS_CONFIG,CbsCpuVoltageP2)},
  //Frequency (MHz)
  {IDSNVID_CPU_COF_P3, OFFSET_OF(CBS_CONFIG,CbsCpuCofP3)},
  //Voltage (uV)
  {IDSNVID_CPU_VOLTAGE_P3, OFFSET_OF(CBS_CONFIG,CbsCpuVoltageP3)},
  //Frequency (MHz)
  {IDSNVID_CPU_COF_P4, OFFSET_OF(CBS_CONFIG,CbsCpuCofP4)},
  //Voltage (uV)
  {IDSNVID_CPU_VOLTAGE_P4, OFFSET_OF(CBS_CONFIG,CbsCpuVoltageP4)},
  //Frequency (MHz)
  {IDSNVID_CPU_COF_P5, OFFSET_OF(CBS_CONFIG,CbsCpuCofP5)},
  //Voltage (uV)
  {IDSNVID_CPU_VOLTAGE_P5, OFFSET_OF(CBS_CONFIG,CbsCpuVoltageP5)},
  //Frequency (MHz)
  {IDSNVID_CPU_COF_P6, OFFSET_OF(CBS_CONFIG,CbsCpuCofP6)},
  //Voltage (uV)
  {IDSNVID_CPU_VOLTAGE_P6, OFFSET_OF(CBS_CONFIG,CbsCpuVoltageP6)},
  //Frequency (MHz)
  {IDSNVID_CPU_COF_P7, OFFSET_OF(CBS_CONFIG,CbsCpuCofP7)},
  //Voltage (uV)
  {IDSNVID_CPU_VOLTAGE_P7, OFFSET_OF(CBS_CONFIG,CbsCpuVoltageP7)},
  //Persistent memory Base Low
  {IDSNVID_CMN_PERSISTENT_LOW, OFFSET_OF(CBS_CONFIG,CbsCmnPersistentLow)},
  //Persistent memory Base High
  {IDSNVID_CMN_PERSISTENT_HIGH, OFFSET_OF(CBS_CONFIG,CbsCmnPersistentHigh)},
  //Persistent memory reserved size Low
  {IDSNVID_CMN_PERSISTENT_SIZE_LOW, OFFSET_OF(CBS_CONFIG,CbsCmnPersistentSizeLow)},
  //Persistent memory reserved size High
  {IDSNVID_CMN_PERSISTENT_SIZE_HIGH, OFFSET_OF(CBS_CONFIG,CbsCmnPersistentSizeHigh)},
  //System Temperature Tracking
  {IDSNVID_CMN_SYSTEM_TEMPERATURE_TRACKING, OFFSET_OF(CBS_CONFIG,CbsCmnSystemTemperatureTracking)},
  //UMA Frame buffer Size
  {IDSNVID_CMN_GNB_GFX_UMA_FRAME_BUFFER_SIZE, OFFSET_OF(CBS_CONFIG,CbsCmnGnbGfxUmaFrameBufferSize)},
  //End
  {0xFFFF, 0}};

STATIC IDS_NV_RECORD mIdsNv64[] = {
  //End
  {0xFFFF, 0}};


BOOLEAN
PrepareIdsNvTable (
  IN       VOID *CbsVariable,
  IN OUT   VOID *IdsNvTable,
  IN OUT   UINT32 *IdsNvTableSize
  )
{
  IDS_NV_TABLE_HEADER *IdsNvTblHdr;
  UINT8               *IdsNvRecord;
  UINTN               Setup_Config;
  IDS_NV_RECORD       *IdsNv;

  //Check if IdsNvTableSize size satisfied
  if ((*IdsNvTableSize) <  sizeof (IDS_NV_TABLE_HEADER) + GetIdsNvRecordsSize () ) {
    *IdsNvTableSize = sizeof (IDS_NV_TABLE_HEADER) + GetIdsNvRecordsSize ();
    return FALSE;
  }

  Setup_Config = (UINTN) CbsVariable;

  //Fill the IDS_NV_TABLE_HEADER
  IdsNvTblHdr = IdsNvTable;
  IdsNvTblHdr->Signature = IDS_NV_TABLE_SIGNATURE; //$INV
  IdsNvTblHdr->Revision = IDS_NV_TABLE_REV_1; //0x00000001ul

  //Fill the IDS_NV_RECORD
  IdsNvRecord = &(((IDS_NV_TABLE *)IdsNvTable)->NvRecords);

  IdsNv = mIdsNv8;
  while (IdsNv->Id != 0xFFFF) {
    ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Id = IdsNv->Id;
    ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_BYTE;
    ((IDS_NV_RECORD_U8 *) IdsNvRecord)->Value = *(UINT8*)(Setup_Config+IdsNv->Offset);
    IdsNvRecord += sizeof (IDS_NV_RECORD_U8);
    IdsNv++;
  }

  IdsNv = mIdsNv16;
  while (IdsNv->Id != 0xFFFF) {
    ((IDS_NV_RECORD_U16 *) IdsNvRecord)->Id = IdsNv->Id;
    ((IDS_NV_RECORD_U16 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_WORD;
    ((IDS_NV_RECORD_U16 *) IdsNvRecord)->Value = *(UINT16*)(Setup_Config+IdsNv->Offset);
    IdsNvRecord += sizeof (IDS_NV_RECORD_U16);
    IdsNv++;
  }

  IdsNv = mIdsNv32;
  while (IdsNv->Id != 0xFFFF) {
    ((IDS_NV_RECORD_U32 *) IdsNvRecord)->Id = IdsNv->Id;
    ((IDS_NV_RECORD_U32 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_DWORD;
    ((IDS_NV_RECORD_U32 *) IdsNvRecord)->Value = *(UINT32*)(Setup_Config+IdsNv->Offset);
    IdsNvRecord += sizeof (IDS_NV_RECORD_U32);
    IdsNv++;
  }

  IdsNv = mIdsNv64;
  while (IdsNv->Id != 0xFFFF) {
    ((IDS_NV_RECORD_U64 *) IdsNvRecord)->Id = IdsNv->Id;
    ((IDS_NV_RECORD_U64 *) IdsNvRecord)->Attrib.size = IDS_NV_ATTRIB_SIZE_QWORD;
    ((IDS_NV_RECORD_U64 *) IdsNvRecord)->Value = *(UINT64*)(Setup_Config+IdsNv->Offset);
    IdsNvRecord += sizeof (IDS_NV_RECORD_U64);
    IdsNv++;
  }

  //Fill the end of IDS_NV_RECORD
  ((IDS_NV_RECORD_CMN *) IdsNvRecord)->Id = IDS_NV_ID_END;

  return TRUE;
}

#define IDS_NV_RECORDS_SIZE (1169)

UINT32
GetIdsNvRecordsSize (
  )
{
  return IDS_NV_RECORDS_SIZE;
}