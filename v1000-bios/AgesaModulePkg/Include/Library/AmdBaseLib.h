/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Library
 *
 * Contains interface to the AMD AGESA library
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Lib
 * @e \$Revision: 312770 $   @e \$Date: 2015-02-11 13:24:42 +0800 (Wed, 11 Feb 2015) $
 *
 */
/*
 ******************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 **/

#ifndef _AMD_BASE_LIB_H_
#define _AMD_BASE_LIB_H_
#include "Uefi.h"
#include "AMD.h"
#include "Library/IdsLib.h"
#include "SocLogicalId.h"

#define IOCF8 0xCF8
#define IOCFC 0xCFC

// Reg Values for ReadCpuReg and WriteCpuReg
#define    CR4_REG  0x04
#define    DR0_REG  0x10
#define    DR1_REG  0x11
#define    DR2_REG  0x12
#define    DR3_REG  0x13
#define    DR7_REG  0x17

// IO
VOID
LibAmdIoRead (
  IN       ACCESS_WIDTH AccessWidth,
  IN       UINT16 IoAddress,
     OUT   VOID *Value,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  );

VOID
LibAmdIoWrite (
  IN       ACCESS_WIDTH AccessWidth,
  IN       UINT16 IoAddress,
  IN       VOID *Value,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  );

VOID
LibAmdIoRMW (
  IN       ACCESS_WIDTH AccessWidth,
  IN       UINT16 IoAddress,
  IN       VOID *Data,
  IN       VOID *DataMask,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  );

VOID
LibAmdIoPoll (
  IN       ACCESS_WIDTH AccessWidth,
  IN       UINT16 IoAddress,
  IN       VOID *Data,
  IN       VOID *DataMask,
  IN       UINT64 Delay,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  );

// Memory or MMIO
VOID
LibAmdMemRead (
  IN       ACCESS_WIDTH AccessWidth,
  IN       UINT64 MemAddress,
     OUT   VOID *Value,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  );

VOID
LibAmdMemRead32 (
  IN       UINT64 MemAddress,
     OUT   VOID *Value,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  );

VOID
LibAmdMemWrite (
  IN       ACCESS_WIDTH AccessWidth,
  IN       UINT64 MemAddress,
  IN       VOID *Value,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  );

VOID
LibAmdMemWrite32 (
  IN       UINT64 MemAddress,
  IN       VOID *Value,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  );

VOID
LibAmdMemRMW (
  IN       ACCESS_WIDTH AccessWidth,
  IN       UINT64 MemAddress,
  IN       VOID *Data,
  IN       VOID *DataMask,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  );

VOID
LibAmdMemPoll (
  IN       ACCESS_WIDTH AccessWidth,
  IN       UINT64 MemAddress,
  IN       VOID *Data,
  IN       VOID *DataMask,
  IN       UINT64 Delay,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  );

// PCI
BOOLEAN
GetPciMmioAddress (
     OUT   UINT64            *MmioAddress,
     OUT   UINT32            *MmioSize,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

VOID
LibAmdPciFindNextCap (
  IN OUT   PCI_ADDR *Address,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

// Utility Functions
EFI_STATUS
EFIAPI
LibAmdInsertSmbiosString (
  IN OUT  CHAR8       **Destination,
  IN      UINTN       *DestMax,
  IN      CONST CHAR8 *Source
  );

VOID
LibAmdMemFill (
  IN       VOID *Destination,
  IN       UINT8 Value,
  IN       UINTN FillLength,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  );

VOID
LibAmdMemCopy (
  IN       VOID *Destination,
  IN       VOID *Source,
  IN       UINTN CopyLength,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  );

UINT32
LibAmdGetPackageType (
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

UINT16
LibAmdSimNowEnterDebugger (VOID);

VOID
LibAmdHDTBreakPoint (VOID);

VOID
LibAmdGetDataFromPtr (
  IN       ACCESS_WIDTH AccessWidth,
  IN       VOID         *Data,
  IN       VOID         *DataMask,
     OUT   UINT32       *TemData,
     OUT   UINT32       *TempDataMask
  );

UINT8
LibAmdAccessWidth (
  IN       ACCESS_WIDTH AccessWidth
  );

VOID
LibAmdFinit (
  VOID
  );

AGESA_STATUS
GetTscRate (
     OUT   UINT32 *FrequencyInMHz,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

BOOLEAN
GetNbPstateInfo (
  IN       PCI_ADDR               *PciAddress,
  IN       UINT32                 NbPstate,
     OUT   UINT32                 *FreqNumeratorInMHz,
     OUT   UINT32                 *FreqDivisor,
     OUT   UINT32                 *VoltageInuV,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  );
#endif // _AMD_BASE_LIB_H_

