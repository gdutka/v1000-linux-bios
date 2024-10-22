/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Fabric ACPI SLIT.
 *
 * This funtion collect SLIT information.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Fabric
 * @e \$Revision$   @e \$Date$
 *
 */
/*
 ****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include "Porting.h"
#include "AMD.h"
#include "Library/AmdBaseLib.h"
#include <Library/BaseFabricTopologyLib.h>
#include <FabricRegistersRV.h>
#include <Library/FabricRegisterAccLib.h>
#include <Library/AmdIdsHookLib.h>
#include "Library/UefiBootServicesTableLib.h"
#include <Protocol/AmdAcpiSlitServicesProtocol.h>
#include <Protocol/FabricNumaServicesProtocol.h>
#include <FabricAcpiTable.h>
#include "Filecode.h"


#define FILECODE FABRIC_RV_FABRICRVDXE_FABRICACPISLIT_FILECODE

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
EFI_STATUS
EFIAPI
FabricGetSlitInfo (
  IN       AMD_FABRIC_ACPI_SLIT_SERVICES_PROTOCOL    *This,
  IN OUT   UINT8                                  *DomainCount,
  IN OUT   UINT8                                  *Distance
  );

/*----------------------------------------------------------------------------------------
 *                           G L O B A L   V A R I A B L E S
 *----------------------------------------------------------------------------------------
 */
STATIC AMD_FABRIC_ACPI_SLIT_SERVICES_PROTOCOL  mFabricAcpiSlitServicesProtocol = {
  0x1,
  FabricGetSlitInfo
};

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

EFI_STATUS
EFIAPI
FabricRvAcpiSlitProtocolInstall (
  IN       EFI_HANDLE        ImageHandle,
  IN       EFI_SYSTEM_TABLE  *SystemTable
  )
{
  return (gBS->InstallProtocolInterface (
               &ImageHandle,
               &gAmdFabricAcpiSlitServicesProtocolGuid,
               EFI_NATIVE_INTERFACE,
               &mFabricAcpiSlitServicesProtocol
               ));
}

EFI_STATUS
EFIAPI
FabricGetSlitInfo (
  IN       AMD_FABRIC_ACPI_SLIT_SERVICES_PROTOCOL  *This,
  IN OUT   UINT8                                   *DomainCount,
  IN OUT   UINT8                                   *Distance
  )
{
  UINT32                          NumOfDomain;
  DOMAIN_INFO                    *DomainInfo;
  FABRIC_NUMA_SERVICES_PROTOCOL  *FabricNuma;

  // Locate Fabric SRAT Services Protocol
  if (gBS->LocateProtocol (&gAmdFabricNumaServicesProtocolGuid, NULL, (VOID **) &FabricNuma) != EFI_SUCCESS) {
    return EFI_ABORTED;
  }

  FabricNuma->GetDomainInfo (FabricNuma, &NumOfDomain, &DomainInfo);
  *DomainCount = (UINT8) NumOfDomain;

  return FabricGetDistanceInfo (Distance, NumOfDomain, DomainInfo);
}


