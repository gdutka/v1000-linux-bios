/* $NoKeywords:$ */
/**
 * @file
 *
 * AblPostCode.h
 *
 * Contains definition needed for ABL Post Codes
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  PSP
 * @e \$Revision: 83676 $   @e \$Date: 2012-12-07 15:57:01 -0600 (Fri, 07 Dec 2012) $
 *
 */
/*****************************************************************************
 *
 * Copyright 2008 - 2018 ADVANCED MICRO DEVICES, INC.  All Rights Reserved.
 *
 * AMD is granting you permission to use this software (the Materials)
 * pursuant to the terms and conditions of your Software License Agreement
 * with AMD.  This header does *NOT* give you permission to use the Materials
 * or any rights under AMD's intellectual property.  Your use of any portion
 * of these Materials shall constitute your acceptance of those terms and
 * conditions.  If you do not agree to the terms and conditions of the Software
 * License Agreement, please do not use any portion of these Materials.
 *
 * CONFIDENTIALITY:  The Materials and all other information, identified as
 * confidential and provided to you by AMD shall be kept confidential in
 * accordance with the terms and conditions of the Software License Agreement.
 *
 * LIMITATION OF LIABILITY: THE MATERIALS AND ANY OTHER RELATED INFORMATION
 * PROVIDED TO YOU BY AMD ARE PROVIDED "AS IS" WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTY OF ANY KIND, INCLUDING BUT NOT LIMITED TO WARRANTIES OF
 * MERCHANTABILITY, NONINFRINGEMENT, TITLE, FITNESS FOR ANY PARTICULAR PURPOSE,
 * OR WARRANTIES ARISING FROM CONDUCT, COURSE OF DEALING, OR USAGE OF TRADE.
 * IN NO EVENT SHALL AMD OR ITS LICENSORS BE LIABLE FOR ANY DAMAGES WHATSOEVER
 * (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF PROFITS, BUSINESS
 * INTERRUPTION, OR LOSS OF INFORMATION) ARISING OUT OF AMD'S NEGLIGENCE,
 * GROSS NEGLIGENCE, THE USE OF OR INABILITY TO USE THE MATERIALS OR ANY OTHER
 * RELATED INFORMATION PROVIDED TO YOU BY AMD, EVEN IF AMD HAS BEEN ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGES.  BECAUSE SOME JURISDICTIONS PROHIBIT THE
 * EXCLUSION OR LIMITATION OF LIABILITY FOR CONSEQUENTIAL OR INCIDENTAL DAMAGES,
 * THE ABOVE LIMITATION MAY NOT APPLY TO YOU.
 *
 * AMD does not assume any responsibility for any errors which may appear in
 * the Materials or any other related information provided to you by AMD, or
 * result from use of the Materials or any related information.
 *
 * You agree that you will not reverse engineer or decompile the Materials.
 *
 * NO SUPPORT OBLIGATION: AMD is not obligated to furnish, support, or make any
 * further information, software, technical information, know-how, or show-how
 * available to you.  Additionally, AMD retains the right to modify the
 * Materials at any time, without notice, and is not obligated to provide such
 * modified Materials to you.
 *
 * U.S. GOVERNMENT RESTRICTED RIGHTS: The Materials are provided with
 * "RESTRICTED RIGHTS." Use, duplication, or disclosure by the Government is
 * subject to the restrictions as set forth in FAR 52.227-14 and
 * DFAR252.227-7013, et seq., or its successor.  Use of the Materials by the
 * Government constitutes acknowledgement of AMD's proprietary rights in them.
 *
 * EXPORT ASSURANCE:  You agree and certify that neither the Materials, nor any
 * direct product thereof will be exported directly or indirectly, into any
 * country prohibited by the United States Export Administration Act and the
 * regulations thereunder, without the required authorization from the U.S.
 * government nor will be used for any purpose prohibited by the same.
 ******************************************************************************
 */

 #ifndef _ABLPOSTCODE_H_
 #define _ABLPOSTCODE_H_

/// <PostCodePrefix> 0xEA000000
/**
 *  AGESA ABL Test Points
 *
 *  These are the values displayed to the user to indicate progress through boot.
 *
 */
typedef enum {
  StartProcessorTestPoints                   = 0xE000, ///< Entry used for range testing for @b Processor related TPs

  // Memory test points
  TpProcMemBeforeMemDataInit                 = 0xE001, ///< Memory structure initialization (Public interface)
  TpProcMemBeforeSpdProcessing               = 0xE002, ///< SPD Data processing  (Public interface)
  TpProcMemAmdMemAutoPhase1                  = 0xE003, ///< Memory configuration  (Public interface) Phase 1
  TpProcMemDramInit                          = 0xE004, ///< DRAM initialization
  TpProcMemSPDChecking                       = 0xE005, ///< ProcMemSPDChecking
  TpProcMemModeChecking                      = 0xE006, ///< ProcMemModeChecking
  TpProcMemSpeedTclConfig                    = 0xE007, ///< Speed and TCL configuration
  TpProcMemSpdTiming                         = 0xE008, ///< ProcMemSpdTiming
  TpProcMemDramMapping                       = 0xE009, ///< ProcMemDramMapping
  TpProcMemPlatformSpecificConfig            = 0xE00A, ///< ProcMemPlatformSpecificConfig
  TPProcMemPhyCompensation                   = 0xE00B, ///< ProcMemPhyCompensation
  TpProcMemStartDcts                         = 0xE00C, ///< ProcMemStartDcts
  TpProcMemBeforeDramInit                    = 0xE00D, ///< ProcMemBeforeDramInit (Public interface)
  TpProcMemPhyFenceTraining                  = 0xE00E, ///< ProcMemPhyFenceTraining
  TpProcMemSynchronizeDcts                   = 0xE00F, ///< ProcMemSynchronizeDcts
  TpProcMemSystemMemoryMapping               = 0xE010, ///< ProcMemSystemMemoryMapping
  TpProcMemMtrrConfiguration                 = 0xE011, ///< ProcMemMtrrConfiguration
  TpProcMemDramTraining                      = 0xE012, ///< ProcMemDramTraining
  TpProcMemBeforeAnyTraining                 = 0xE013, ///< ProcMemBeforeAnyTraining(Public interface)
  // PMU Test Points
  TpProcMemPmuBeforeFirmwareLoad             = 0xE014, ///< ABL Mem - PMU - Before PMU Firmware load
  TpProcMemPmuAfterFirmwareLoad              = 0xE015, ///< ABL Mem - PMU - After PMU Firmware load
  TpProcMemPmuPopulateSramTimings            = 0xE016, ///< ABL Mem - PMU Populate SRAM Timing
  TpProcMemPmuPopulateSramConfig             = 0xE017, ///< ABL Mem - PMU Populate SRAM Config
  TpProcMemPmuWriteSramMsgBlock              = 0xE018, ///< ABL Mem - PMU Write SRAM Msg Block
  TpProcMemPmuWaitForPhyCalComplete          = 0xE019, ///< ABL Mem - Wait for Phy Cal Complete
  TpProcMemPmuPhyCalComplete                 = 0xE01A, ///< ABL Mem - Phy Cal Complete
  TpProcMemPmuStart                          = 0xE01B, ///< ABL Mem - PMU Start
  TpProcMemPmuStarted                        = 0xE01C, ///< ABL Mem - PMU Started
  TpProcMemPmuWaitingForComplete             = 0xE01D, ///< ABL Mem - PMU Waiting for Complete
  TpProcMemPmuStageDevInit                   = 0xE01E, ///< ABL Mem - PMU Stage Dec Init
  TpProcMemPmuStageTrainWrLvl                = 0xE01F, ///< ABL Mem - PMU Stage Training Wr Lvl
  TpProcMemPmuStageTrainRxEn                 = 0xE020, ///< ABL Mem - PMU Stage Training Rx En
  TpProcMemPmuStageTrainRdDqs1D              = 0xE021, ///< ABL Mem - PMU Stage Training Rd Dqs
  TpProcMemPmuStageTrainRd2D                 = 0xE022, ///< ABL Mem - PMU Stage Traning Rd 2D
  TpProcMemPmuStageTrainWr2D                 = 0xE023, ///< ABL Mem - PMU Stage Training Wr 2D
  TpProcMemPmuStagePMUQEmpty                 = 0xE024, ///< ABL Mem - PMU Queue Empty
  TpProcMemPmuUSMsgStart                     = 0xE025, ///< ABL Mem - PMU US message Start
  TpProcMemPmuUSMsgEnd                       = 0xE026, ///< ABL Mem - PMU US message End
  TpProcMemPmuComplete                       = 0xE027, ///< ABL Mem - PMU Complete
  TpProcMemAfterPmuTraining                  = 0xE028, ///< ABL Mem - PMU - After PMU Training
  TpProcMemBeforeDisablePmu                  = 0xE029, ///< ABL Mem - PMU - Before Disable PMU
  //Original Post code
  TpProcMemTransmitDqsTraining               = 0xE02A, ///< ABL Mem - ProcMemTransmitDqsTraining
  TpProcMemTxDqStartSweep                    = 0xE02B, ///< ABL Mem - Start write sweep
  TpProcMemTxDqSetDelay                      = 0xE02C, ///< ABL Mem - Set Transmit DQ delay
  TpProcMemTxDqWritePattern                  = 0xE02D, ///< ABL Mem - Write test pattern
  TpProcMemTxDqReadPattern                   = 0xE02E, ///< ABL Mem - Read Test pattern
  TpProcMemTxDqTestPattern                   = 0xE02F, ///< ABL Mem - Compare Test pattern
  TpProcMemTxDqResults                       = 0xE030, ///< ABL Mem - Update results
  TpProcMemTxDqFindWindow                    = 0xE031, ///< ABL Mem - Start Find passing window
  TpProcMemMaxRdLatencyTraining              = 0xE032, ///< ABL Mem - ProcMemMaxRdLatencyTraining
  TpProcMemMaxRdLatStartSweep                = 0xE033, ///< ABL Mem - Start sweep
  TpProcMemMaxRdLatSetDelay                  = 0xE034, ///< ABL Mem - Set delay
  TpProcMemMaxRdLatWritePattern              = 0xE035, ///< ABL Mem - Write test pattern
  TpProcMemMaxRdLatReadPattern               = 0xE036, ///< ABL Mem - Read Test pattern
  TpProcMemMaxRdLatTestPattern               = 0xE037, ///< ABL Mem - Compare Test pattern
  TpProcMemOnlineSpareInit                   = 0xE038, ///< ABL Mem - Online Spare init
  TpProcMemBankInterleaveInit                = 0xE039, ///< ABL Mem - Bank Interleave Init
  TpProcMemNodeInterleaveInit                = 0xE03A, ///< ABL Mem - Node Interleave Init
  TpProcMemChannelInterleaveInit             = 0xE03B, ///< ABL Mem - Channel Interleave Init
  TpProcMemEccInitialization                 = 0xE03C, ///< ABL Mem - ECC initialization
  TpProcMemPlatformSpecificInit              = 0xE03D, ///< ABL Mem - Platform Specific Init
  TpProcMemBeforeAgesaReadSpd                = 0xE03E, ///< ABL Mem - Before callout for "AgesaReadSpd"
  TpProcMemAfterAgesaReadSpd                 = 0xE03F, ///< ABL Mem - After callout for "AgesaReadSpd"
  TpProcMemBeforeAgesaHookBeforeDramInit     = 0xE040, ///< ABL Mem - Before optional callout "AgesaHookBeforeDramInit"
  TpProcMemAfterAgesaHookBeforeDramInit      = 0xE041, ///< ABL Mem - After optional callout "AgesaHookBeforeDramInit"
  TpProcMemBeforeAgesaHookBeforeDQSTraining  = 0xE042, ///< ABL Mem - Before optional callout "AgesaHookBeforeDQSTraining"
  TpProcMemAfterAgesaHookBeforeDQSTraining   = 0xE043, ///< ABL Mem - After optional callout "AgesaHookBeforeDQSTraining"
  TpProcMemBeforeAgesaHookBeforeExitSelfRef  = 0xE044, ///< ABL Mem - Before optional callout "AgesaHookBeforeDramInit"
  TpProcMemAfterAgesaHookBeforeExitSelfRef   = 0xE045, ///< ABL Mem - After optional callout "AgesaHookBeforeDramInit"
  TpProcMemAfterMemDataInit                  = 0xE046, ///< ABL Mem - After MemDataInit
  TpProcMemInitializeMCT                     = 0xE047, ///< ABL Mem - Before InitializeMCT
  TpProcMemLvDdr3                            = 0xE048, ///< ABL Mem - Before LV DDR3
  TpProcMemInitMCT                           = 0xE049, ///< ABL Mem - Before InitMCT
  TpProcMemOtherTiming                       = 0xE04A, ///< ABL Mem - Before OtherTiming
  TpProcMemUMAMemTyping                      = 0xE04B, ///< ABL Mem - Before UMAMemTyping
  TpProcMemSetDqsEccTmgs                     = 0xE04C, ///< ABL Mem - Before SetDqsEccTmgs
  TpProcMemMemClr                            = 0xE04D, ///< ABL Mem - Before MemClr
  TpProcMemOnDimmThermal                     = 0xE04E, ///< ABL Mem - Before On DIMM Thermal
  TpProcMemDmi                               = 0xE04F, ///< ABL Mem - Before DMI
  TpProcMemEnd                               = 0xE050, ///< ABL MEM - End of phase 3 memory code

  // CPU test points
  TpProcCpuInitAfterTrainingStart            = 0xE051, ///< Entry point CPU init after training
  TpProcCpuInitAfterTrainingEnd              = 0xE052, ///< Exit point CPU init after training
  TpProcCpuApobCcxMapInitStart               = 0xE053, ///< Entry point CPU APOB CCX map init
  TpProcCpuApobCcxMapInitEnd                 = 0xE054, ///< Exit point CPU APOB CCX map init
  TpProcCpuOptimizedBootStart                = 0xE055, ///< Entry point CPU Optimized boot init
  TpProcCpuOptimizedBootEnd                  = 0xE056, ///< Exit point CPU Optimized boot init
  TpProcCpuApobCcxEdcInitStart               = 0xE057, ///< Entry point CPU APOB EDC info init
  TpProcCpuApobCcxEdcInitEnd                 = 0xE058, ///< Exit point CPU APOB EDC info init

  // Topology test points
  TpProcTopologyEntry                        = 0xE071, ///< ProcTopologyEntry
  TpProcTopologyDone                         = 0xE07C, ///< ProcTopologyDone

  // Extended memory test point
  TpProcMemSendMRS2                          = 0xE080, ///< ProcMemSendMRS2
  TpProcMemSendMRS3                          = 0xE081, ///< Sedding MRS3
  TpProcMemSendMRS1                          = 0xE082, ///< Sending MRS1
  TpProcMemSendMRS0                          = 0xE083, ///< Sending MRS0
  TpProcMemContinPatternGenRead              = 0xE084, ///< Continuous Pattern Read
  TpProcMemContinPatternGenWrite             = 0xE085, ///< Continuous Pattern Write
  TpProcMem2dRdDqsTraining                   = 0xE086, ///< Mem: 2d RdDqs Training begin
  TpProcMemBefore2dTrainExtVrefChange        = 0xE087, ///< Mem: Before optional callout to platform BIOS to change External Vref during 2d Training
  TpProcMemAfter2dTrainExtVrefChange         = 0xE088, ///< Mem: After optional callout to platform BIOS to change External Vref during 2d Training
  TpProcMemConfigureDCTForGeneral            = 0xE089, ///< Configure DCT For General use begin
  TpProcMemProcConfigureDCTForTraining       = 0xE08A, ///< Configure DCT For training begin
  TpProcMemConfigureDCTNonExplicitSeq        = 0xE08B, ///< Configure DCT For Non-Explicit
  TpProcMemSynchronizeChannels               = 0xE08C, ///< Configure to Sync channels
  TpProcMemC6StorageAllocation               = 0xE08D, ///< Allocate C6 Storage
  TpProcMemLvDdr4                            = 0xE08E, ///< Before LV DDR4
  TpProcMemLvLpddr3                          = 0xE08F, ///< Before LV DDR3

  // Gnb Earlier init
  TP0x90                                     = 0xE090, ///< TP0x90
  TP0x91                                     = 0xE091, ///< GNB earlier interface
  TP0x92                                     = 0xE092, ///< GNB internal debug code
  TP0x93                                     = 0xE093, ///< GNB internal debug code
  TP0x94                                     = 0xE094, ///< GNB internal debug code
  TP0x95                                     = 0xE095, ///< GNB internal debug code
  TP0x96                                     = 0xE096, ///< GNB internal debug code
  TP0x97                                     = 0xE097, ///< GNB internal debug code
  TP0x98                                     = 0xE098, ///< GNB internal debug code
  TP0x99                                     = 0xE099, ///< GNB internal debug code
  TP0x9A                                     = 0xE09A, ///< GNB internal debug code
  TP0x9B                                     = 0xE09B, ///< GNB internal debug code
  TP0x9C                                     = 0xE09C, ///< GNB internal debug code
  TP0x9D                                     = 0xE09D, ///< GNB internal debug code
  TP0x9E                                     = 0xE09E, ///< GNB internal debug code
  TP0x9F                                     = 0xE09F, ///< GNB internal debug code
  TP0xA0                                     = 0xE0A0, ///< TP0xA0
  TP0xA1                                     = 0xE0A1, ///< GNB internal debug code
  TP0xA2                                     = 0xE0A2, ///< GNB internal debug code
  TP0xA3                                     = 0xE0A3, ///< GNB internal debug code
  TP0xA4                                     = 0xE0A4, ///< GNB internal debug code
  TP0xA5                                     = 0xE0A5, ///< GNB internal debug code
  TP0xA6                                     = 0xE0A6, ///< GNB internal debug code
  TP0xA7                                     = 0xE0A7, ///< GNB internal debug code
  TP0xA8                                     = 0xE0A8, ///< GNB internal debug code
  TP0xA9                                     = 0xE0A9, ///< GNB internal debug code
  TP0xAA                                     = 0xE0AA, ///< GNB internal debug code
  TP0xAB                                     = 0xE0AB, ///< GNB internal debug code
  TP0xAC                                     = 0xE0AC, ///< GNB internal debug code
  TP0xAD                                     = 0xE0AD, ///< GNB internal debug code
  TP0xAE                                     = 0xE0AE, ///< GNB internal debug code
  TP0xAF                                     = 0xE0AF, ///< GNB internal debug code

  TpAbl1Begin                                = 0xE0B0, ///< Abl1Begin
  TpAbl1Initialization                       = 0xE0B1, ///< ABL 1 Initialization
  TpAbl1DfEarly                              = 0xE0B2, ///< ABL 1 DF Early
  TpAbl1DfPreTraining                        = 0xE0B3, ///< ABL 1 DF Pre Training
  TpAbl1DebugSync                            = 0xE0B4, ///< ABL 1 Debug Synchronization
  TpAbl1ErrorDetected                        = 0xE0B5, ///< ABL 1 Error Detected
  TpAbl1GlobalMemErrorDetected               = 0xE0B6, ///< ABL 1 Global memory error detected
  TpAbl1End                                  = 0xE0B7, ///< ABL 1 End
  TpAbl2Begin                                = 0xE0B8, ///< ABL 2 Begin
  TpAbl2Initialization                       = 0xE0B9, ///< ABL 2 Initialization
  TpAbl2DfAfterTraining                      = 0xE0BA, ///< ABL 2 After Training
  TpAbl2DebugSync                            = 0xE0BB, ///< ABL 2 Debug Synchronization
  TpAbl2ErrorDetected                        = 0xE0BC, ///< ABL 2 Error detected
  TpAbl2GlobalMemErrorDetected               = 0xE0BD, ///< ABL 2 Global memory error detected
  TpAbl2End                                  = 0xE0BE, ///< ABL 2 End
  TpAbl3Begin                                = 0xE0BF, ///< ABL 3 Begin
  TpAbl3Initialization                       = 0xE0C0, ///< ABL 3 Initialziation
  TpAbl3GmiGopInitStage1                     = 0xE1C0, ///< ABL 3 GMI/xGMI Initialization Stage 1
  TpAbl3GmiGopInitStage1Warning              = 0xB1C0, ///< ABL 3 GMI/xGMI Initialization Stage 1 Warning
  TpAbl3GmiGopInitState1Error                = 0xF1C0, ///< ABL 3 GMI/xGMI Initialization Stage 2 Error
  TpAbl3GmiGopInitStage2                     = 0xE2C0, ///< ABL 3 GMI/xGMI Initialization Stage 2
  TpAbl3GmiGopInitStage2Warning              = 0xB2C0, ///< ABL 3 GMI/xGMI Initialization Stage 2 Warning
  TpAbl3GmiGopInitState2Error                = 0xF2C0, ///< ABL 3 GMI/xGMI Initialization Stage 2 Error
  TpAbl3GmiGopInitStage3                     = 0xE3C0, ///< ABL 3 GMI/xGMI Initialization Stage 3
  TpAbl3GmiGopInitStage3Warning              = 0xB3C0, ///< ABL 3 GMI/xGMI Initialization Stage 3 Warning
  TpAbl3GmiGopInitState3Error                = 0xF3C0, ///< ABL 3 GMI/xGMI Initialization Stage 3 Error
  TpAbl3GmiGopInitStage4                     = 0xE4C0, ///< ABL 3 GMI/xGMI Initialization Stage 4
  TpAbl3GmiGopInitStage4Warning              = 0xB4C0, ///< ABL 3 GMI/xGMI Initialization Stage 4 Warning
  TpAbl3GmiGopInitState4Error                = 0xF4C0, ///< ABL 3 GMI/xGMI Initialization Stage 4 Error
  TpAbl3GmiGopInitStage5                     = 0xE5C0, ///< ABL 3 GMI/xGMI Initialization Stage 5
  TpAbl3GmiGopInitStage5Warning              = 0xB5C0, ///< ABL 3 GMI/xGMI Initialization Stage 5 Warning
  TpAbl3GmiGopInitState5Error                = 0xF5C0, ///< ABL 3 GMI/xGMI Initialization Stage 5 Error
  TpAbl3GmiGopInitStage6                     = 0xE6C0, ///< ABL 3 GMI/xGMI Initialization Stage 6
  TpAbl3GmiGopInitStage6Warning              = 0xB6C0, ///< ABL 3 GMI/xGMI Initialization Stage 6 Warning
  TpAbl3GmiGopInitState6Error                = 0xF6C0, ///< ABL 3 GMI/xGMI Initialization Stage 6 Error
  TpAbl3GmiGopInitStage7                     = 0xE7C0, ///< ABL 3 GMI/xGMI Initialization Stage 7
  TpAbl3GmiGopInitStage8                     = 0xE8C0, ///< ABL 3 GMI/xGMI Initialization Stage 8
  TpAbl3ProgramUmcKeys                       = 0xE0C1, ///< Abl3ProgramUmcKeys
  TpAbl3DfFinalInitialization                = 0xE0C2, ///< ABL 3 DF Finial Initalization
  TpAbl3ExecuteSyncFunction                  = 0xE0C3, ///< ABL 3 Execute Synchronization Function
  TpAbl3DebugSync                            = 0xE0C4, ///< ABL 3 Debug Synchronization Function
  TpAbl3ErrorDetected                        = 0xE0C5, ///< ABL 3 Error Detected
  TpAbl3GlobalMemErrorDetected               = 0xE0C6, ///< ABL 3 Global memroy error detected
  TpAbl4ColdInitialization                   = 0xE0C7, ///< ABL 4 Initialiation - cold boot
  TpAbl4MemTest                              = 0xE0C8, ///< ABL 4 Memory test - cold boot
  TpAbl4Apob                                 = 0xE0C9, ///< ABL 4 APOB Initialzation - cold boot
  TpAbl4Finalize                             = 0xE0CA, ///< ABL 4 Finalize memory settings - cold boot
  TpAbl4CpuInizialOptimizedBoot              = 0xE0CB, ///< ABL 4 CPU Initialize Optimized Boot - cold boot
  TpAbl4GmicieTraining                       = 0xE0CC, ///< ABL 4 Gmi Pcie Training - cold boot
  TpAbl4ColdEnd                              = 0xE0CD, ///< ABL 4 Cold boot End
  TpAbl4ResumeInitialization                 = 0xE0CE, ///< ABL 4 Initialization - Resume boot
  TpAbl4ResumeEnd                            = 0xE0CF, ///< ABL 4 Resume End
  TpAbl4End                                  = 0xE0D0, ///< ABL 4 End Cold/Resume boot
  TpProcMemAmdMemAutoPhase2                  = 0xE0D1, ///< ABL 2 memory initialization
  TpProcMemAmdMemAutoPhase3                  = 0xE0D2, ///< ABL 3 memory initialization
  TpAbl3End                                  = 0xE0D3, ///< ABL 3 End
  TpAbl1EnterMemFlow                         = 0xE0D4, ///< ABL 1 Enter Memory Flow
  TpAbl1MemFlowMemClkSync                    = 0xE0D5, ///< Memorry flow memory clock synchronization
  TpIfAmdReadEventLogEntry                   = 0xE0D6, ///< IfAmdReadEventLogEntry
  TpIfAmdReadEventLogExit                    = 0xE0D7, ///< Exiting from AmdReadEventLog
  TpIfAmdGetApicIdEntry                      = 0xE0D8, ///< Entry to AmdGetApicId
  TpIfAmdGetApicIdExit                       = 0xE0D9, ///< Exiting from AmdGetApicId
  TpIfAmdGetPciAddressEntry                  = 0xE0DA, ///< Entry to AmdGetPciAddress
  TpIfAmdGetPciAddressExit                   = 0xE0DB, ///< Exiting from AmdGetPciAddress
  TpIfAmdIdentifyCoreEntry                   = 0xE0DC, ///< Entry to AmdIdentifyCore
  TpIfAmdIdentifyCoreExit                    = 0xE0DD, ///< Exiting from AmdIdentifyCore
  TpIfBeforeRunApFromIds                     = 0xE0DE, ///< After IDS calls out to run code on an AP
  TpIfAfterRunApFromIds                      = 0xE0DF, ///< After IDS calls out to run code on an AP
  TpIfBeforeGetIdsData                       = 0xE0E0, ///< Before IDS calls out to get IDS data
  TpIfAfterGetIdsData                        = 0xE0E1, ///< After IDS calls out to get IDS data
  TpIfBeforeAllocateHeapBuffer               = 0xE0E2, ///< Before the heap manager calls out to allocate a buffer
  TpIfAfterAllocateHeapBuffer                = 0xE0E3, ///< After the heap manager calls out to allocate a buffer
  TpIfBeforeDeallocateHeapBuffer             = 0xE0E4, ///< Before the heap manager calls out to deallocate a buffer
  TpIfAfterDeallocateHeapBuffer              = 0xE0E5, ///< After the heap manager calls out to deallocate a buffer
  TpIfBeforeLocateHeapBuffer                 = 0xE0E6, ///< Before the heap manager calls out to locate a buffer
  TpIfAfterLocateHeapBuffer                  = 0xE0E7, ///< After the heap manager calls out to locate a buffer
  TpAbl1MemFlowPStateSync                    = 0xE0E8, ///< Memory flow P-State synchronization
  TpIfAfterRunApFromAllAps                   = 0xE0E9, ///< After the BSP calls out to run code on an AP
  TpIfBeforeRunApFromAllCore0s               = 0xE0EA, ///< Before the BSP calls out to run code on an AP
  TpIfAfterRunApFromAllCore0s                = 0xE0EB, ///< After the BSP calls out to run code on an AP
  TpIfBeforeAllocateS3SaveBuffer             = 0xE0EC, ///< Before the S3 save code calls out to allocate a buffer
  TpIfAfterAllocateS3SaveBuffer              = 0xE0ED, ///< After the S3 save code calls out to allocate a buffer
  TpIfBeforeAllocateMemoryS3SaveBuffer       = 0xE0EE, ///< Before the memory S3 save code calls out to allocate a buffer
  TpIfAfterAllocateMemoryS3SaveBuffer        = 0xE0EF, ///< After the memory S3 save code calls out to allocate a buffer
  TpIfBeforeLocateS3PciBuffer                = 0xE0F0, ///< Before the memory code calls out to locate a buffer
  TpIfAfterLocateS3PciBuffer                 = 0xE0F1, ///< After the memory code calls out to locate a buffer
  TpIfBeforeLocateS3CPciBuffer               = 0xE0F2, ///< Before the memory code calls out to locate a buffer
  TpIfAfterLocateS3CPciBuffer                = 0xE0F3, ///< After the memory code calls out to locate a buffer
  TpIfBeforeLocateS3MsrBuffer                = 0xE0F4, ///< Before the memory code calls out to locate a buffer
  TpIfAfterLocateS3MsrBuffer                 = 0xE0F5, ///< After the memory code calls out to locate a buffer
  TpIfBeforeLocateS3CMsrBuffer               = 0xE0F6, ///< Before the memory code calls out to locate a buffer
  TpIfAfterLocateS3CMsrBuffer                = 0xE0F7, ///< After the memory code calls out to locate a buffer
  TpReadyToBoot                              = 0xE0F8, ///< Ready to boot event
  // PMU test points
  TpProcMemPmuFailed                         = 0xE0F9, ///< Failed PMU training.
  TpProcMemPhase1End                         = 0xE0FA, ///< End of phase 1 memory code
  TpProcMemPhase2End                         = 0xE0FB, ///< End of phase 2 memory code

  // ABL0 test points
  TpAbl0Begin                                = 0xE0FC, ///< Abl0Begin
  TpAbl0End                                  = 0xE0FD, ///< ABL 0 End

  TpAbl5Initialization                       = 0xE100, ///< ABL 5 Initialization
  TpAbl5DebugSync                            = 0xE101, ///< ABL 5 Debug Sync
  TpAbl5ErrorDetected                        = 0xE102, ///< ABL 5 Error Detected
  TpAbl5GlobalMemErrorDetected               = 0xE103, ///< ABL 5 Mem Error Detected
  TpAbl5End                                  = 0xE104, ///< ABL 5 End
  TpProcMemAmdMemAutoPhase1a                 = 0xE105, ///< ABL 5 MemAuto Phase 1a
  TpProcMemPhase1aEnd                        = 0xE106, ///< ABL 5 MemAuto Phase 1a End

  TpAbl6Initialization                       = 0xE107, ///< ABL 6 Initialization
  TpAbl6DebugSync                            = 0xE108, ///< ABL 6 Debug Sync
  TpAbl6ErrorDetected                        = 0xE109, ///< ABL 6 Error Detected
  TpAbl6GlobalMemErrorDetected               = 0xE10A, ///< ABL 6 Mem Error Detected
  TpAbl6End                                  = 0xE10B, ///< ABL 6 End
  TpProcMemAmdMemAutoPhase2a                 = 0xE10C, ///< ABL 6 MemAuto Phase 2a
  TpProcMemPhase2aEnd                        = 0xE10D, ///< ABL 6 MemAuto Phase 2a End

  TpProcApobHmacFailOnS3                     = 0xE10E, ///< BSP encountered HMAC fail on APOB Header

  TpProcBeforeUmcBasedDeviceInit             = 0xE110, ///< Before UMC based device initialization
  TpProcAfterUmcBasedDeviceInit              = 0xE111, ///< After UMC based device initialization

  TpProcBeforeRrwTest                        = 0xE112, ///< Before RRW Test
  TpProcAfterRrwTest                         = 0xE113, ///< After RRW Test
  TpProcBeforeAblMemTest                     = 0xE114, ///< Before ABL Memory Test
  TpProcAfterAblMemTest                      = 0xE115, ///< After ABL Memory Test

  TpAblErroGeneralAssert                              = 0xE2A0,  ///< ABL Eroor General ASSERT
  TpAblErrorUnknown                                   = 0xE2A1,  ///< Unknown Error
  TpAblErrorLogInitError                              = 0xE2A3,  ///< ABL Error Log Inig Error
  TpAblErrorOdtHeap                                   = 0xE2A4,  ///< ABL Error for On DIMM thermal Heap allocation error
  TpAblErrorMemoryTest                                = 0xE2A5,  ///< ABL Error for memory test error
  TpAblErrorExecutingMemoryTest                       = 0xE2A6,  ///< ABL Error while executing memory test error
  TpAblErrorDpprMemAutoHeapAlocError                  = 0xE2A7,  ///< ABL Error DDR Post Packge Repair Mem Auto Heap Alloc error
  TpAblErrorDpprNoApobHeapAlocError                   = 0xE2A8,  ///< ABL Error for DDR Post Package repair Apob Heap Alloc error
  TpAblErrorDpprNoPprTblHeapAlocError                 = 0xE2A9,  ///< ABL Error for DDR Post Package Repair No PPR Table Heap Aloc error
  TpAblErrorEccMemAutoHeapAlocError                   = 0xE2AA,  ///< ABL Error for Ecc Mem Auto Aloc Error error
  TpAblErrorSocScanHeapAlocError                      = 0xE2AB,  ///< ABL Error for Soc Scan Heap Aloc error
  TpAblErrorSocScanNoDieError                         = 0xE2AC,  ///< ABL Error for Soc Scan No Die error
  TpAblErrorNbTecHeapAlocError                        = 0xE2AD,  ///< ABL Error for Nb Tech Heap Aloc error
  TpAblErrorNoNbConstError                            = 0xE2AE,  ///< ABL Error for No Nb Constructor error
  TpAblErrorApcbRecoveryError                         = 0xE2AF,  ///< ABL Error for APCB recovery error
  TpAblErrorNoTechConstError                          = 0xE2B0,  ///< ABL Error for No Tech Constructor error
  TpAblErrorAbl1bAutoAloc                             = 0xE2B1,  ///< ABL Error for ABL1b Auto Alocation error
  TpAblErrorAbl1bNoNbConst                            = 0xE2B2,  ///< ABL Error for ABL1b No NB Constructor error
  TpAblErrorAbl2NoNbConst                             = 0xE2B3,  ///< ABL Error for ABL2 No Nb Constructor error
  TpAblErrorAbl3AutoAloc                              = 0xE2B4,  ///< ABL Error for ABL3 Auto Allocation error
  TpAblErrorAbl3NoNbConst                             = 0xE2B5,  ///< ABL Error for ABL3 No Nb Constructor error
  TpAblErrorAbl1bGen                                  = 0xE2B6,  ///< ABL Error for ABL1b General error
  TpAblErrorAbl2Gen                                   = 0xE2B7,  ///< ABL Error for ABL2 General error
  TpAblErrorAbl3Gen                                   = 0xE2B8,  ///< ABL Error for ABL3 General error
  TpAblErrorGetTargetSpeed                            = 0xE2B9,  ///< ABL Error for Get Target Speed error
  TpAblErrorFlowP1FamilySupport                       = 0xE2BA,  ///< ABL Error for Flow P1 Family Support error
  TpAblErrorNoValidDdr4Dimms                          = 0xE2BB,  ///< ABL Error for No Valid Ddr4 Dimms error
  TpAblErrorNoDimmPresent                             = 0xE2BC,  ///< ABL Error for No Dimm Present error
  TpAblErrorFlowP2FamilySupport                       = 0xE2BD,  ///< ABL Error for Flow P2 Family Supprot error
  TpAblErrorHeapDealocForPmuSramMsgBlock              = 0xE2BE,  ///< ABL Error for Heap Deallocation for PMU Sram Msg Block error
  TpAblErrorDdrRecovery                               = 0xE2BF,  ///< ABL Error for DDR Recovery error
  TpAblErrorRrwTest                                   = 0xEBC0,  ///< ABL Error for RRW Test error
  TpAblErrorOdtInit                                   = 0xE2C1,  ///< ABL Error for On Die Thermal error
  TpAblErrorHeapAllocForDctStructAndChDefStruct       = 0xE2C2,  ///< ABL Error for Heap Allocation For Dct Struct Amd Ch Def structure error
  TpAblErrorHeapAlocForPmuSramMsgBlock                = 0xE2C3,  ///< ABL Error for Heap Allocation for PMU SRAM Msg block error
  TpAblErrorHeapPhyPllLockFailure                     = 0xE2C4,  ///< ABL Error for Heap Phy PLL lock Flure error
  TpAblErrorPmuTraining                               = 0xE2C5,  ///< ABL Error for Pmu Training error
  TpAblErrorFailureToLoadOrVerifyPmuFw                = 0xE2C6,  ///< ABL Error for Failure to Load or Verify PMU FW error
  TpAblErrorAllocateForPmuSramMsgBlockNoInit          = 0xE2C7,  ///< ABL Error for Allocate for PMU SRAM Msg Block No Init error
  TpAblErrorFailureBiosPmuFwMismatchAgesaPmuFwVersion = 0xE2C8,  ///< ABL Error for Failure BIOS PMU FW Mismatch AGESA PMU FW version error
  TpAblErrorDeallocateForPmuSramMsgBlock              = 0xE2CA,  ///< ABL Error for Deallocate for PMU SRAM Msg Block error
  TpAblErrorModuleTypeMismatchRDimm                   = 0xE2CB,  ///< ABL Error for Module Type Mismatch RDIMM error
  TpAblErrorModuleTypeMismatchLRDimm                  = 0xE2CC,  ///< ABL Error for Module type Mismatch LRDIMM error
  TpAblErrorMemAutoNvdimm                             = 0xE2CD,  ///< ABL Error for MEm Auto NVDIM error
  TpAblErrorUnknownResponse                           = 0xE2CE,  ///< ABL Error for Unknowm Responce error
  TpAblErrorMemOverclockErrorRrwTestResults           = 0xE2CF,  ///< ABL Error for Over Clock Error RRW Test Results Error
  TpAblErrorOverClockErrorPmuTraining                 = 0xE2D0,  ///< ABL Error for Over Clock Error PMU Training Error
  TpAblErrorAbl1GenError                              = 0xE2D1,  ///< ABL Error for ABL1 General Error
  TpAblErrorAbl2GenError                              = 0xE2D2,  ///< ABL Error for ABL2 General Error
  TpAblErrorAbl3GenError                              = 0xE2D3,  ///< ABL Error for ABL3 General Error
  TpAblErrorAbl5GenError                              = 0xE2D4,  ///< ABL Error for ABL4 General Error
  TpAblErrorOverClockMemInit                          = 0xE2D5,  ///< ABL Error over clock Mem Init Error
  TpAblErrorOverClockMemOther                         = 0xE2D6,  ///< ABL Error over clock Mem Other Error
  TpAblErrorAbl6GenError                              = 0xE2D7,  ///< ABL Error for ABL6 General Error
  TpEventLogInit                                      = 0xE2D8,  ///< ABL Error Event Log Error
  TpAblErrorAbl1FatalError                            = 0xE2D9,  ///< ABL Error FATAL ABL1 Log Error
  TpAblErrorAbl2FatalError                            = 0xE2DA,  ///< ABL Error FATAL ABL2 Log Error
  TpAblErrorAbl3FatalError                            = 0xE2DB,  ///< ABL Error FATAL ABL3 Log Error
  TpAblErrorAbl4FatalError                            = 0xE2DC,  ///< ABL Error FATAL ABL4 Log Error
  TpAblErrorSlaveSyncFunctionExecutionError           = 0xE2DD,  ///< ABL Error Slave Sync function execution Error
  TpAblErrorSlaveSyncCommWithDataSentToMasterError    = 0xE2DE,  ///< ABL Error Slave Sync communicaton with data set to master Error
  TpAblErrorSlaveBroadcastCommFromMasterToSlaveError  = 0xE2DF,  ///< ABL Error Slave broadcast communication from master to slave Error
  TpAblErrorAbl6FatalError                            = 0xE2E0,  ///< ABL Error FATAL ABL6 Log Error
  TpAblErrorSlaveOfflineMsgError                      = 0xE2E1,  ///< ABL Error Slave Offline Error
  TpAblErrorSlaveInformsMasterErrorInoError           = 0xE2E2,  ///< ABL Error Slave Informs Master Error Info Error
  TpAblErrorHeapLocateForPmuSramMsgBlock              = 0xE2E3,  ///< ABL Error Error Heap Locate for PMU SRAM Msg Block Error
  TpAblErrorAbl2AutoAloc                              = 0xE2E4,  ///< ABL Error ABL2 Auto Error
  TpAblErrorFlowP3FamilySupport                       = 0xE2E5,  ///< ABL Error Flow P3 Family support Error
  TpAblErrorAbl4GenError                              = 0xE2E5,  ///< ABL Error Abl 4 Gen Error
  TpAblErrorMbistHeapAlloc                            = 0xE2EB,  ///< ABL Error MBIST Heap Allocation Error
  TpAblErrorMbistResultsError                         = 0xE2EC,  ///< ABL Error MBIST Results Error
  TpAblErrorNoDimmSmbusInfoError                      = 0xE2ED,  ///< ABL Error NO Dimm Smcus Info Error
  TpAblErrorPorMaxFreqTblError                        = 0xE2EE,  ///< ABL Error Por Max Freq Table Error
  TpAblErrorUnsupportedDimmConfuglError               = 0xE2EF,  ///< ABL Error Unsupproted DIMM Config Error
  TpAblErrorNoPsTableError                            = 0xE2F0,  ///< ABL Error No Ps Table Error
  TpAblErrorCadBusTmgNoFoundError                     = 0xE2F1,  ///< ABL Error Cad Bus Timing Not Found Error
  TpAblErrorDataBusTmgNoFoundError                    = 0xE2F2,  ///< ABL Error Data Bus Timing Not Found Error
  TpAblErrorLrIbtNotFoundError                        = 0xE2F3,  ///< ABL Error LrDIMM IBT Not Found Error
  TpAblErrorUnsupportedDimmConfigMaxFreqError         = 0xE2F4,  ///< ABL Error Unsupprote Dimm Config Max Freq Error Error
  TpAblErrorMr0NotFoundError                          = 0xE2F5,  ///< ABL Error Mr0 Not Found Error
  TpAblErrorOdtPAtNotFoundError                       = 0xE2F6,  ///< ABL Error Obt Pattern Not found Error
  TpAblErrorRc10OpSpeedNotFoundError                  = 0xE2F7,  ///< ABL Error Rc10 Op Speed Not FOund Error
  TpAblErrorRc2IbtNotFoundError                       = 0xE2F8,  ///< ABL Error Rc2 Ibt Not Found Error
  TpAblErrorRttNotFoundError                          = 0xE2F9,  ///< ABL Error Rtt Not Found Error
  TpAblErrorChecksumReStrtError                       = 0xE2FA,  ///< ABL Error Checksum ReStrt Results Error
  TpAblErrorNoChipselectError                         = 0xE2FB,  ///< ABL Error No Chipselect Results Error
  TpAblErrorNoCommonCasLAtError                       = 0xE2FC,  ///< ABL Error No Common Cas Latency Results Error
  TpAblErrorCasLatXceedsTaaMaxError                   = 0xE2FD,  ///< ABL Error Cas Latecncy exceeds Taa Max Error
  TpAblErrorNvdimmArmMissmatcPowerPolicyError         = 0xE2FE,  ///< ABL Error Nvdimm Arm Missmatch Power Policy Error
  TpAblErrorNvdimmArmMissmatchPowerSouceError         = 0xE2FF,  ///< ABL Error Nvdimm Arm Missmatch Power Source Error
  TpAblErrorS0i3DfRestoreBufferError                  = 0xE300,  ///< ABL Error S0i3 DF restore buffer Error
  EndAgesaTps                                         = 0xEFFF,  ///< EndAgesas
} AGESA_TP;


#endif

