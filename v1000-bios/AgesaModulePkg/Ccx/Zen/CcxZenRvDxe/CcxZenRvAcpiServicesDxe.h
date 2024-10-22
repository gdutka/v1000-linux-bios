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
#ifndef _CCX_ZEN_RV_ACPI_SERVICES_DXE_H_
#define _CCX_ZEN_RV_ACPI_SERVICES_DXE_H_

#pragma pack (push, 1)

EFI_STATUS
EFIAPI
CcxZenRvAcpiCpuSsdtServicesProtocolInstall (
  IN       EFI_HANDLE        ImageHandle,
  IN       EFI_SYSTEM_TABLE  *SystemTable
  );

EFI_STATUS
EFIAPI
CcxZenRvCratServicesProtocolInstall (
  IN       EFI_HANDLE        ImageHandle,
  IN       EFI_SYSTEM_TABLE  *SystemTable
  );

EFI_STATUS
EFIAPI
CcxZenRvSratServicesProtocolInstall (
  IN       EFI_HANDLE        ImageHandle,
  IN       EFI_SYSTEM_TABLE  *SystemTable
  );

#pragma pack (pop)
#endif // _CCX_ZEN_RV_ACPI_SERVICES_DXE_H_

