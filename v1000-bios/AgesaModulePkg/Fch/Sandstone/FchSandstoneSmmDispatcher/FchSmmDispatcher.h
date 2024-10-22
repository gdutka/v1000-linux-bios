/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Hudson-2 SMI Dispatcher Driver Example
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  UEFI
 * @e \$Revision: 309090 $   @e \$Date: 2014-12-09 10:28:05 -0800 (Tue, 09 Dec 2014) $
 *
 */
/******************************************************************************
*
* Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
*
*******************************************************************************
**/

#ifndef _FCH_SMM_DISPATCHER_H_
#define _FCH_SMM_DISPATCHER_H_

#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/S3BootScriptLib.h>

#include <Protocol/SmmCpu.h>
#include <Protocol/FchSmmSxDispatch2.h>
#include <Protocol/FchSmmSwDispatch2.h>
#include <Protocol/FchSmmPwrBtnDispatch2.h>
#include <Protocol/FchSmmPeriodicalDispatch2.h>
#include <Protocol/FchSmmIoTrapDispatch2.h>
#include <Protocol/FchSmmGpiDispatch2.h>
#include <Protocol/FchSmmUsbDispatch2.h>

#include <Protocol/SmmSwDispatch2.h>
#include <Protocol/SmmSxDispatch2.h>
#include <Protocol/SmmPowerButtonDispatch2.h>
#include <Protocol/SmmPeriodicTimerDispatch2.h>
#include <Protocol/SmmUsbDispatch2.h>
#include <Protocol/SmmGpiDispatch2.h>
#include <Protocol/SmmIoTrapDispatch2.h>


#include "FchSmmSxDispatcher.h"
#include "FchSmmSwDispatcher.h"
#include "FchSmmPwrBtnDispatcher.h"
#include "FchSmmIoTrapDispatcher.h"
#include "FchSmmPeriodicalDispatcher.h"
#include "FchSmmGpiDispatcher.h"
#include "FchSmmUsbDispatcher.h"
#include "FchSmmMiscDispatcher.h"

#include "FchPlatform.h"
#pragma pack (8)

#define Slp_Type    BIT1
#define SmiCmdPort  BIT11
#define UsbSmi      BIT21
#define PwrBtn      BIT19
#define Eos         BIT28
#define IoTrapping0 BIT20
#define IoTrapping1 BIT21
#define IoTrapping2 BIT22
#define IoTrapping3 BIT23
#define ShortTimer  BIT14
#define LongTimer   BIT15
#define Gpe         0xfffffffful
#define Smbus0      BIT15
#define AllGevents  0xfffffffful
#define FchGppSerr0 BIT21
#define FchGppSerr1 BIT22
#define FchGppSerr2 BIT23
#define FchGppSerr3 BIT24
#define FchFakeSts0 BIT1

typedef EFI_STATUS (EFIAPI *AMD_SM_SMM_CHILD_DISPATCHER_HANDLER) (
  IN      EFI_HANDLE     SmmImageHandle,
  IN OUT  VOID           *CommunicationBuffer OPTIONAL,
  IN OUT  UINTN          *SourceSize OPTIONAL
  );


///
/// AMD FCH SMM Dispatcher Structure
///
typedef struct {
  UINT32                                SmiStatusReg;   ///< Status Register
  UINT32                                SmiStatusBit;   ///< Status Bit
} FCH_SMM_COMMUNICATION_BUFFER;

///
/// AMD FCH SMM Dispatcher Structure
///
typedef struct {
  UINT32                                StatusReg;      ///< Status Register
  UINT32                                SmiStatusBit;   ///< Status Bit
  AMD_SM_SMM_CHILD_DISPATCHER_HANDLER   SmiDispatcher;  ///< Dispatcher Address
} FCH_SMM_DISPATCHER_TABLE;

///
/// AMD FCH SMM Dispatcher Structure
///
typedef struct {
  UINT16             ioPort;                            ///< I/O port
  EFI_SMM_IO_WIDTH   ioWidth;                           ///< I/O width
  UINT32             ioValue;                           ///< I/O value
} SAVE_B2B_IO;

///
/// AMD Save/Restore Structure for SMM
///
typedef struct {
  UINT8              Bus;                               ///< PCI Bus
  UINT8              Dev;                               ///< PCI Device
  UINT8              Func;                              ///< PCI Function
  UINT16             Offset;                            ///< Offset registers
  EFI_SMM_IO_WIDTH   DataWidth;                         ///< Data width
  UINT32             DataValue;                         ///< Data value
} SAVE_PCI;

#endif

