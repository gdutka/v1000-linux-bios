/* $NoKeywords:$ */
/**
 * @file
 *
 * Gather Pstate Data Services.
 *
 * Contains implementation of the interfaces: Pstate Gather Data API in AmdAcpiCpuSsdt.h.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CCX
 * @e \$Revision$   @e \$Date$
 *
 */
/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */
#ifndef _CCX_ZEN_ZP_ACPI_SERVICES_DXE_H_
#define _CCX_ZEN_ZP_ACPI_SERVICES_DXE_H_

#pragma pack (push, 1)

EFI_STATUS
EFIAPI
CcxZenZpAcpiCpuSsdtServicesProtocolInstall (
  IN       EFI_HANDLE        ImageHandle,
  IN       EFI_SYSTEM_TABLE  *SystemTable
  );

EFI_STATUS
EFIAPI
CcxZenZpCratServicesProtocolInstall (
  IN       EFI_HANDLE        ImageHandle,
  IN       EFI_SYSTEM_TABLE  *SystemTable
  );

EFI_STATUS
EFIAPI
CcxZenZpSratServicesProtocolInstall (
  IN       EFI_HANDLE        ImageHandle,
  IN       EFI_SYSTEM_TABLE  *SystemTable
  );

EFI_STATUS
EFIAPI
CcxZenZpRasServicesProtocolInstall (
  IN       EFI_HANDLE        ImageHandle,
  IN       EFI_SYSTEM_TABLE  *SystemTable
  );

#pragma pack (pop)
#endif // _CCX_ZEN_ZP_ACPI_SERVICES_DXE_H_

