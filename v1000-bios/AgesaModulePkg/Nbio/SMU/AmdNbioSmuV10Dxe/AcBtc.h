/**
 * @file
 *
 * Ac BTC definitions
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Nbio
 * @e \$Revision: 313706 $   @e \$Date: 2015-02-25 21:00:43 -0600 (Wed, 25 Feb 2015) $
 *
 */
/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */
#ifndef _AC_BTC_H_
#define _AC_BTC_H_

/// Table Register Entry
typedef struct {
  UINT64          DataIn1[0x300];          /// <.data_in1 resq 0x40
  UINT64          DataIn2[0x300];          /// <.data_in2 resq 0x40
  UINT64          DataIn3[0x300];          /// <.data_in3 resq 0x40
  UINT64          DataIn4[0x300];          /// <.data_in4 resq 0x40
  UINT64          DataIn5[0x300];          /// <.data_in5 resq 0x40
  UINT64          DataIn6[0x300];          /// <.data_in6 resq 0x40
  UINT64          DataIn7[0x300];          /// <.data_in7 resq 0x40
  UINT64          DataIn8[0x300];          /// <.data_in8 resq 0x40
  UINT64          DataOut1[0x300];         /// <.data_out1 resq 0x40
  UINT64          DataOut2[0x300];         /// <.data_out2 resq 0x40
  UINT64          DataOut3[0x300];         /// <.data_out3 resq 0x40
  UINT64          DataOut4[0x300];         /// <.data_out4 resq 0x40
  UINT64          DataOut5[0x300];         /// <.data_out5 resq 0x40
  UINT64          DataOut6[0x300];         /// <.data_out6 resq 0x40
  UINT64          DataOut7[0x300];         /// <.data_out7 resq 0x40
  UINT64          DataOut8[0x300];         /// <.data_out8 resq 0x40
  UINT64          SynchLine;              /// <.synch_line resq 0x1
  UINT64          ActiveFPLoopCount1[30];  /// <.activeFP_loop_count resq 16
  UINT64          ActiveFPLoopCount2[30];  /// <.activeFP_loop_count resq 16
  UINT64          ActiveFPLoopCount3[30];  /// <.activeFP_loop_count resq 16
  UINT64          ActiveFPLoopCount4[30];  /// <.activeFP_loop_count resq 16
  UINT64          ActiveFPLoopCount5[30];  /// <.activeFP_loop_count resq 16
  UINT64          ActiveFPLoopCount6[30];  /// <.activeFP_loop_count resq 16
  UINT64          ActiveFPLoopCount7[30];  /// <.activeFP_loop_count resq 16
  UINT64          ActiveFPLoopCount8[30];  /// <.activeFP_loop_count resq 16
  UINT64          IdleFPLoopCount1[30];     /// <.idleFP_loop_count resq 16
  UINT64          IdleFPLoopCount2[30];     /// <.idleFP_loop_count resq 16
  UINT64          IdleFPLoopCount3[30];     /// <.idleFP_loop_count resq 16
  UINT64          IdleFPLoopCount4[30];     /// <.idleFP_loop_count resq 16
  UINT64          IdleFPLoopCount5[30];     /// <.idleFP_loop_count resq 16
  UINT64          IdleFPLoopCount6[30];     /// <.idleFP_loop_count resq 16
  UINT64          IdleFPLoopCount7[30];     /// <.idleFP_loop_count resq 16
  UINT64          IdleFPLoopCount8[30];     /// <.idleFP_loop_count resq 16
  UINT64          ThirdDNop1[30];           /// <.thirdD_nop resq 16
  UINT64          ThirdDNop2[30];           /// <.thirdD_nop resq 16
  UINT64          ThirdDNop3[30];           /// <.thirdD_nop resq 16
  UINT64          ThirdDNop4[30];           /// <.thirdD_nop resq 16
  UINT64          ThirdDNop5[30];           /// <.thirdD_nop resq 16
  UINT64          ThirdDNop6[30];           /// <.thirdD_nop resq 16
  UINT64          ThirdDNop7[30];           /// <.thirdD_nop resq 16
  UINT64          ThirdDNop8[30];           /// <.thirdD_nop resq 16
  UINT64          BasePtr[0x500];         /// <.BasePtr2 resq 0x500
  UINTN           tscWaitCount;             /// <. 2mS TSC count
} ACBTC_WORKLOAD_STRUCT;

EFI_STATUS
EFIAPI
StartBtc (
  );

EFI_STATUS
EFIAPI
RunBtc (
  );

VOID
AfterBtc (
  );

VOID
EFIAPI
CallbackAcBtc (
  IN      EFI_EVENT  Event,
  IN      VOID       *Context
  );

#endif

