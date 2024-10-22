/* $NoKeywords:$ */
/**
 * @file
 *
 * Generate SMBIOS type 4 7 16 17 18 19 20
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Universal
 * @e \$Revision$   @e \$Date$
 *
 */
/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */
#ifndef _UNIVERSAL_SMBIOS_AMD_SMBIOS_DXE_H_
#define _UNIVERSAL_SMBIOS_AMD_SMBIOS_DXE_H_
#pragma pack (push, 1)

BOOLEAN
AmdSmbiosVersionCheck (
  IN       EFI_SMBIOS_PROTOCOL *Smbios,
  IN       UINT8                MajorVersion,
  IN       UINT8                MinorVersion
  );

EFI_STATUS
EFIAPI
AmdAddSmbiosType4 (
  IN       UINTN                 Socket,
  IN       UINT16                CacheHandle[CpuLmaxCache],
  IN       EFI_SMBIOS_PROTOCOL  *Smbios,
  IN       AMD_CONFIG_PARAMS    *StdHeader
  );

EFI_STATUS
EFIAPI
AmdAddSmbiosType4ForNoProcInstalled (
  IN       UINTN                 Socket,
  IN       UINT16                CacheHandle[CpuLmaxCache],
  IN       EFI_SMBIOS_PROTOCOL  *Smbios,
  IN       AMD_CONFIG_PARAMS    *StdHeader
  );

EFI_STATUS
EFIAPI
AmdAddSmbiosType7 (
  IN       UINTN                 Socket,
     OUT   UINT16              (*CacheHandle)[CpuLmaxCache],
  IN       EFI_SMBIOS_PROTOCOL  *Smbios,
  IN       AMD_CONFIG_PARAMS    *StdHeader
  );

EFI_STATUS
EFIAPI
AmdAddSmbiosType16 (
  IN       EFI_SMBIOS_PROTOCOL  *Smbios,
  IN       DMI_INFO             *MemDmiInfo,
  IN       EFI_SMBIOS_HANDLE     MemoryErrorInfoHandle,
  IN       EFI_SMBIOS_HANDLE    *MemoryArrayHandle,
  IN       AMD_CONFIG_PARAMS    *StdHeader
  );

EFI_STATUS
EFIAPI
AmdAddSmbiosType17 (
  IN       EFI_SMBIOS_PROTOCOL  *Smbios,
  IN       DMI_INFO             *MemDmiInfo,
  IN       UINT8                 Socket,
  IN       UINT8                 Channel,
  IN       UINT8                 Dimm,
  IN       EFI_SMBIOS_HANDLE     MemoryArrayHandle,
  IN       EFI_SMBIOS_HANDLE     MemoryErrorInfoHandle,
     OUT   EFI_SMBIOS_HANDLE    *MemoryDeviceHandle,
  IN       AMD_CONFIG_PARAMS    *StdHeader
  );

EFI_STATUS
EFIAPI
AmdAddSmbiosType18 (
  IN       EFI_SMBIOS_PROTOCOL  *Smbios,
     OUT   EFI_SMBIOS_HANDLE    *MemoryErrorInfoHandle,
  IN       AMD_CONFIG_PARAMS    *StdHeader
  );

EFI_STATUS
EFIAPI
AmdAddSmbiosType19 (
  IN       EFI_SMBIOS_PROTOCOL  *Smbios,
  IN       DMI_INFO             *MemDmiInfo,
  IN       EFI_SMBIOS_HANDLE     MemoryArrayHandle,
  IN       EFI_SMBIOS_HANDLE    *ArrayMappedHandle,
  IN       AMD_CONFIG_PARAMS    *StdHeader,
  IN       UINT8                RegionIndex
  );

EFI_STATUS
EFIAPI
AmdAddSmbiosType20 (
  IN       EFI_SMBIOS_PROTOCOL  *Smbios,
  IN       DMI_INFO             *MemDmiInfo,
  IN       UINT8                 Socket,
  IN       UINT8                 Channel,
  IN       UINT8                 Dimm,
  IN       EFI_SMBIOS_HANDLE     MemoryDeviceHandle,
  IN       EFI_SMBIOS_HANDLE     ArrayMappedHandle,
  IN       AMD_CONFIG_PARAMS    *StdHeader
  );

#pragma pack (pop)
#endif // _UNIVERSAL_SMBIOS_AMD_SMBIOS_DXE_H_
