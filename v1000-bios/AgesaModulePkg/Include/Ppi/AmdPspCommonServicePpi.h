/* $NoKeywords:$ */
/**
 * @file
 *
 * PSP common service PPI prototype definition
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  PSP
 * @e \$Revision: 309090 $   @e \$Date: 2014-12-10 02:28:05 +0800 (Wed, 10 Dec 2014) $
 */
/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 **/
#ifndef _AMD_PSP_COMMONSERVICE_PPI_H_
#define _AMD_PSP_COMMONSERVICE_PPI_H_

#include <AmdPspDirectory.h>

///Function used to get Ftpm control area address
typedef
BOOLEAN
(*FP_GET_FTPM_CONTROLAREA) (
  IN OUT   VOID **FtpmControlArea
  );

///Function used to turn on/restore PSP MMIO decode
typedef
VOID
(*FP_SWITCH_PSP_MMIO_DECODE) (
  IN       BOOLEAN SwitchFlag,
  IN OUT   UINT32 *RegisterCopy
  );

///Function used to check PSP Device present by checking fuse receipt copy
typedef
BOOLEAN
(*FP_CHECK_PSP_DEVICE_PRESENT) (
  VOID
  );

///Function used to check Ftpm support
typedef
EFI_STATUS
(*FP_CHECK_FTPM_CAPS) (
  UINT32 *Caps
  );

typedef
BOOLEAN
(*FP_PSPENTRYINFO) (
  IN      UINT32                       EntryType,
  IN OUT  UINT64                      *EntryAddress,
  IN      UINT32                      *EntrySize
  );

typedef
BOOLEAN
(* FP_CONDITIONER) (
  IN       VOID        *Context
  );

typedef
BOOLEAN
(*FP_PSP_LIB_TIMEOUT) (
  IN       UINT64              uSec,
  IN       FP_CONDITIONER      Conditioner,
  IN       VOID                *Context
  );

typedef
VOID
(*FP_ACQUIRE_PSPSMIREGMUTEX) (
  VOID
  );

typedef
VOID
(*FP_RELEASE_PSPSMIREGMUTEX) (
  VOID
  );

typedef
EFI_STATUS
(*FP_CHECK_PSP_CAPS) (
  IN OUT   UINT32 *PspCaps
  );
///
/// PPI prototype
///
/// Defines AMD_PSP_COMMON_SERVICE_PPI, which public the common PSP service across all programs
///
typedef struct _AMD_PSP_COMMON_SERVICE_PPI {
  FP_GET_FTPM_CONTROLAREA       GetFtpmControlArea;     ///< Get FTPM control Area
  FP_SWITCH_PSP_MMIO_DECODE     SwitchPspMmioDecode;     ///< Turn on/off Psp MMIO
  FP_CHECK_PSP_DEVICE_PRESENT   CheckPspDevicePresent;     ///< Check if PSP Device Present
  FP_CHECK_FTPM_CAPS            CheckFtpmCaps;     ///< Check if Ftpm supported
  FP_PSPENTRYINFO               PSPEntryInfo;           ///< Get specific PSP Entry information
  FP_PSP_LIB_TIMEOUT            PspLibTimeOut;     ///< Delay function
  FP_ACQUIRE_PSPSMIREGMUTEX     AcquirePspSmiRegMutex;  ///< Acquire Psp SmiReg Mutex
  FP_RELEASE_PSPSMIREGMUTEX     ReleasePspSmiRegMutex;  ///< Release Psp SmiReg Mutex
  FP_CHECK_PSP_CAPS             CheckPspCaps;     ///< Check PSP caps
} AMD_PSP_COMMON_SERVICE_PPI;

extern EFI_GUID gAmdPspCommonServicePpiGuid;

#endif //_AMD_PSP_COMMONSERVICE_PPI_H_

