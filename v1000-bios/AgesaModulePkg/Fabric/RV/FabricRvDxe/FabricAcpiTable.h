/* $NoKeywords:$ */
/**
 * @file
 *
 * Fabric ACPI CDIT  definitions
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Fabric
 * @e \$Revision$   @e \$Date$
 *
 */
/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */
#ifndef _FABRIC_ACPI_TABLE_H_
#define _FABRIC_ACPI_TABLE_H_
#include <FabricRegistersRV.h>
#include <Protocol/FabricNumaServicesProtocol.h>

#pragma pack (push, 1)

EFI_STATUS
EFIAPI
FabricRvNumaServicesProtocolInstall (
  IN       EFI_HANDLE        ImageHandle,
  IN       EFI_SYSTEM_TABLE  *SystemTable
  );

EFI_STATUS
EFIAPI
FabricRvAcpiSlitProtocolInstall (
  IN       EFI_HANDLE        ImageHandle,
  IN       EFI_SYSTEM_TABLE  *SystemTable
  );

EFI_STATUS
EFIAPI
FabricRvAcpiSratProtocolInstall (
  IN       EFI_HANDLE        ImageHandle,
  IN       EFI_SYSTEM_TABLE  *SystemTable
  );

EFI_STATUS
EFIAPI
FabricRvAcpiCditProtocolInstall (
  IN       EFI_HANDLE        ImageHandle,
  IN       EFI_SYSTEM_TABLE  *SystemTable
  );

EFI_STATUS
EFIAPI
FabricRvAcpiCratProtocolInstall (
  IN       EFI_HANDLE        ImageHandle,
  IN       EFI_SYSTEM_TABLE  *SystemTable
  );

EFI_STATUS
EFIAPI
FabricGetDistanceInfo (
  IN OUT   UINT8        *Distance,
  IN       UINT32        DomainCount,
  IN       DOMAIN_INFO  *DomainInfo
  );

#pragma pack (pop)
#endif // _FABRIC_ACPI_TABLE_H_
