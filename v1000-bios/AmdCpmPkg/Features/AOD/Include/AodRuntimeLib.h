/*****************************************************************************
 *
 * Copyright (C) 2017-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 ******************************************************************************
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


#ifndef _AMD_AOD_RUNTIME_LIB_H_
#define _AMD_AOD_RUNTIME_LIB_H_

#include <Uefi.h>
#include <Features/AOD/Aod.h>
#include <Features/AOD/Include/SmbusLightSmm.h>

VOID
AodSmnRead (
  IN       UINT8               DieBusNum,
  IN       UINT32              SmnAddress,
  IN       UINT32              *Value
  );

VOID
AodSmnWrite (
  IN       UINT8               DieBusNum,
  IN       UINT32              SmnAddress,
  IN       UINT32              *Value
  );

BOOLEAN
VerifyAcpiSmiTransferBuffer (
  AodNvAreaStruct *AodNvAreaPtr
  );

VOID
FinalizeTransferBuffer (
  AodNvAreaStruct *AodNvAreaPtr,
  EFI_STATUS  Status
  );

EFI_STATUS
GetMemFreqIndexByClk (
  UINT16    MemFreqClk,
  UINT8     *FreqIndex
  );

EFI_STATUS
GetMemFreqClkByIndex (
  UINT8       FreqIndex,
  UINT16     *MemFreqClk
  );


EFI_STATUS
EnableSmMasterDecode (
  );

EFI_STATUS
DisableSmMasterDecode (
  );

EFI_STATUS
InitSmbusData (
  );

EFI_STATUS
EFIAPI
SmbusExecute (
  IN       UINT8                              SmbusSelect,
  IN       EFI_SMBUS_DEVICE_ADDRESS           SlaveAddress,
  IN       EFI_SMBUS_DEVICE_COMMAND           Command,
  IN       EFI_SMBUS_OPERATION                Operation,
  IN       BOOLEAN                            PecCheck,
  IN OUT   UINTN                              *Length,
  IN OUT   VOID                               *Buffer
  );

UINT8
HammingWeight (
  IN UINT32 Value
  );


#endif
