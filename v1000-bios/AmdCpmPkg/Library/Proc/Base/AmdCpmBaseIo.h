/*****************************************************************************
 *
 * Copyright (C) 2012-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 ******************************************************************************
 */

#ifndef _AMD_CPM_BASE_IO_H_
#define _AMD_CPM_BASE_IO_H_

UINT8
CpmIoRead8 (
  IN      UINT16        Address
  );

UINT16
CpmIoRead16 (
  IN      UINT16        Address
  );

UINT32
CpmIoRead32 (
  IN      UINT16        Address
  );

VOID
CpmIoWrite8 (
  IN      UINT16        Address,
  IN      UINT8         Data
  );

VOID
CpmIoWrite16 (
  IN      UINT16        Address,
  IN      UINT16        Data
  );

VOID
CpmIoWrite32 (
  IN      UINT16        Address,
  IN      UINT32        Data
  );

UINT64
CpmReadTsc (
  );

UINT8
CpmBitScanForward (
  IN      UINT32        Value
  );

UINT8
CpmBitScanReverse (
  IN      UINT32        Value
  );

UINT64
CpmMsrRead (
  IN      UINT32        MsrAddress
  );

UINT64
CpmMsrWrite (
  IN      UINT32        MsrAddress,
  IN      UINT64        Value
  );

VOID
CpmCpuidRawRead (
  IN      UINT32        CpuidFcnAddress,
     OUT  CPUID_DATA    *Value
  );

UINT8
EFIAPI
CpmMmioRead8 (
  IN      UINTN         Address
  );

UINT8
EFIAPI
CpmMmioWrite8 (
  IN      UINTN         Address,
  IN      UINT8         Value
  );

UINT16
EFIAPI
CpmMmioRead16 (
  IN      UINTN         Address
  );

UINT16
EFIAPI
CpmMmioWrite16 (
  IN      UINTN         Address,
  IN      UINT16        Value
  );

UINT32
EFIAPI
CpmMmioRead32 (
  IN      UINTN         Address
  );

UINT32
EFIAPI
CpmMmioWrite32 (
  IN      UINTN         Address,
  IN      UINT32        Value
  );

UINT64
EFIAPI
CpmMmioRead64 (
  IN      UINTN         Address
  );

UINT64
EFIAPI
CpmMmioWrite64 (
  IN      UINTN         Address,
  IN      UINT64        Value
  );

UINT8
EFIAPI
CpmMmioOr8 (
  IN      UINTN         Address,
  IN      UINT8         OrData
  );

UINT8
EFIAPI
CpmMmioAnd8 (
  IN      UINTN         Address,
  IN      UINT8         AndData
  );

UINT8
EFIAPI
CpmMmioAndThenOr8 (
  IN      UINTN         Address,
  IN      UINT8         AndData,
  IN      UINT8         OrData
  );

UINT16
EFIAPI
CpmMmioOr16 (
  IN      UINTN         Address,
  IN      UINT16        OrData
  );

UINT16
EFIAPI
CpmMmioAnd16 (
  IN      UINTN         Address,
  IN      UINT16        AndData
  );

UINT16
EFIAPI
CpmMmioAndThenOr16 (
  IN      UINTN         Address,
  IN      UINT16        AndData,
  IN      UINT16        OrData
  );

UINT32
EFIAPI
CpmMmioOr32 (
  IN      UINTN         Address,
  IN      UINT32        OrData
  );

UINT32
EFIAPI
CpmMmioAnd32 (
  IN      UINTN         Address,
  IN      UINT32        AndData
  );

UINT32
EFIAPI
CpmMmioAndThenOr32 (
  IN      UINTN         Address,
  IN      UINT32        AndData,
  IN      UINT32        OrData
  );

UINT64
EFIAPI
CpmMmioOr64 (
  IN      UINTN         Address,
  IN      UINT64        OrData
  );

UINT64
EFIAPI
CpmMmioAnd64 (
  IN      UINTN         Address,
  IN      UINT64        AndData
  );

UINT64
EFIAPI
CpmMmioAndThenOr64 (
  IN      UINTN         Address,
  IN      UINT64        AndData,
  IN      UINT64        OrData
  );

VOID
EFIAPI
CpmPostCode (
  IN      UINT32        Postcode
  );

UINT8
EFIAPI
CpmPciRead8 (
  IN      VOID          *This,
  IN      UINT8         Bus,
  IN      UINT8         Device,
  IN      UINT8         Function,
  IN      UINT16        Offset
  );

UINT16
EFIAPI
CpmPciRead16 (
  IN      VOID          *This,
  IN      UINT8         Bus,
  IN      UINT8         Device,
  IN      UINT8         Function,
  IN      UINT16        Offset
  );

UINT32
EFIAPI
CpmPciRead32 (
  IN      VOID          *This,
  IN      UINT8         Bus,
  IN      UINT8         Device,
  IN      UINT8         Function,
  IN      UINT16        Offset
  );

VOID
EFIAPI
CpmPciWrite8 (
  IN      VOID          *This,
  IN      UINT8         Bus,
  IN      UINT8         Device,
  IN      UINT8         Function,
  IN      UINT16        Offset,
  IN      UINT8         Data
  );

VOID
EFIAPI
CpmPciWrite16 (
  IN      VOID          *This,
  IN      UINT8         Bus,
  IN      UINT8         Device,
  IN      UINT8         Function,
  IN      UINT16        Offset,
  IN      UINT16        Data
  );

VOID
EFIAPI
CpmPciWrite32 (
  IN      VOID          *This,
  IN      UINT8         Bus,
  IN      UINT8         Device,
  IN      UINT8         Function,
  IN      UINT16        Offset,
  IN      UINT32        Data
  );

VOID
EFIAPI
CpmPciAnd8 (
  IN      VOID          *This,
  IN      UINT8         Bus,
  IN      UINT8         Device,
  IN      UINT8         Function,
  IN      UINT16        Offset,
  IN      UINT8         AndData
  );

VOID
EFIAPI
CpmPciAnd16 (
  IN      VOID          *This,
  IN      UINT8         Bus,
  IN      UINT8         Device,
  IN      UINT8         Function,
  IN      UINT16        Offset,
  IN      UINT16        AndData
  );

VOID
EFIAPI
CpmPciAnd32 (
  IN      VOID          *This,
  IN      UINT8         Bus,
  IN      UINT8         Device,
  IN      UINT8         Function,
  IN      UINT16        Offset,
  IN      UINT32        AndData
  );

VOID
EFIAPI
CpmPciOr8 (
  IN      VOID          *This,
  IN      UINT8         Bus,
  IN      UINT8         Device,
  IN      UINT8         Function,
  IN      UINT16        Offset,
  IN      UINT8         OrData
  );

VOID
EFIAPI
CpmPciOr16 (
  IN      VOID          *This,
  IN      UINT8         Bus,
  IN      UINT8         Device,
  IN      UINT8         Function,
  IN      UINT16        Offset,
  IN      UINT16        OrData
  );

VOID
EFIAPI
CpmPciOr32 (
  IN      VOID          *This,
  IN      UINT8         Bus,
  IN      UINT8         Device,
  IN      UINT8         Function,
  IN      UINT16        Offset,
  IN      UINT32        OrData
  );

VOID
EFIAPI
CpmPciAndThenOr8 (
  IN      VOID          *This,
  IN      UINT8         Bus,
  IN      UINT8         Device,
  IN      UINT8         Function,
  IN      UINT16        Offset,
  IN      UINT8         AndData,
  IN      UINT8         OrData
  );

VOID
EFIAPI
CpmPciAndThenOr16 (
  IN      VOID          *This,
  IN      UINT8         Bus,
  IN      UINT8         Device,
  IN      UINT8         Function,
  IN      UINT16        Offset,
  IN      UINT16        AndData,
  IN      UINT16        OrData
  );

VOID
EFIAPI
CpmPciAndThenOr32 (
  IN      VOID          *This,
  IN      UINT8         Bus,
  IN      UINT8         Device,
  IN      UINT8         Function,
  IN      UINT16        Offset,
  IN      UINT32        AndData,
  IN      UINT32        OrData
  );

VOID
EFIAPI
CpmCpuidRead (
  IN      VOID          *This,
     OUT  UINT32        *CpuidFamilyModelStepping,
     OUT  UINT32        *PkgType
  );

UINT32
EFIAPI
CpmGetPcieEndPointDeviceID (
  IN      VOID          *This,
  IN      UINT8         Device,
  IN      UINT8         Function
  );

UINT8
EFIAPI
CpmGetMultiDieBusNumber (
  IN      VOID          *This,
  IN      UINT16        Socket,
  IN      UINT16        Die,
  IN      UINT16        Bridge
  );

UINT8
EFIAPI
CpmCheckPcieDevice (
  IN      VOID          *This,
  IN      UINT8         Socket,
  IN      UINT8         Die,
  IN      UINT8         Bridge,
  IN      UINT8         Device,
  IN      UINT8         Function
  );

BOOLEAN
EFIAPI
CpmCheckPcieVga (
  IN      VOID          *This,
  IN      UINT8         Socket,
  IN      UINT8         Die,
  IN      UINT8         Bridge,
  IN      UINT8         Device,
  IN      UINT8         Function
  );

BOOLEAN
EFIAPI
CpmDetectDevice (
  IN      VOID          *This,
  IN      UINT8         DeviceId,
     OUT  UINT8         *Status
  );

VOID
EFIAPI
CpmPowerOnDevice (
  IN      VOID          *This,
  IN      UINT8         DeviceId,
  IN      UINT8         State
  );

UINT8
EFIAPI
CpmGetDeviceConfig (
  IN      VOID          *This,
  IN      UINT8         DeviceId
  );

EFI_STATUS
EFIAPI
CpmKbcWaitInputBufferEmpty (
  IN      UINT16        BaseAddr
  );

EFI_STATUS
EFIAPI
CpmKbcWaitOutputBufferFull (
  IN      UINT16        BaseAddr
  );

EFI_STATUS
EFIAPI
CpmKbcWaitOutputBufferEmpty (
  IN      UINT16        BaseAddr
  );

EFI_STATUS
EFIAPI
CpmKbcRead (
  IN      UINT16        BaseAddr,
  IN      UINT8         Command,
  IN      UINT8         *Data,
     OUT  UINT8         *Value
  );

EFI_STATUS
EFIAPI
CpmKbcReadDummy (
  IN      UINT16        BaseAddr,
  IN      UINT8         Command,
  IN      UINT8         *Data,
     OUT  UINT8         *Value
  );

EFI_STATUS
EFIAPI
CpmKbcWrite (
  IN      UINT16        BaseAddr,
  IN      UINT8         Command,
  IN      UINT8         *Data
  );

EFI_STATUS
EFIAPI
CpmKbcWriteDummy (
  IN      UINT16        BaseAddr,
  IN      UINT8         Command,
  IN      UINT8         *Data
  );

EFI_STATUS
EFIAPI
CpmEcRamWrite (
  IN      UINT16        BaseAddr,
  IN      UINT8         Command,
  IN      UINT8         *Offset,
  IN      UINT8         *Data
  );

UINT32
EFIAPI
CpmEventAslNameConverter (
  IN      UINT8         EventPin
  );

VOID
EFIAPI
CpmRegisterBaseIo (
  IN      VOID          *This
  );

#endif // _AMD_CPM_BASE_IO_H_
