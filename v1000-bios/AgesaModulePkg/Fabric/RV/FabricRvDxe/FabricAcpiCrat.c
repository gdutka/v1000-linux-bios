/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Fabric ACPI CRAT.
 *
 * This funtion collect CRAT information.
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
#include "FabricAcpiTable.h"
#include <Library/BaseLib.h>
#include "Library/AmdBaseLib.h"
#include <Library/BaseFabricTopologyLib.h>
#include <FabricRegistersRV.h>
#include <Library/FabricRegisterAccLib.h>
#include <Library/AmdIdsHookLib.h>
#include "Library/UefiBootServicesTableLib.h"
#include <Protocol/AmdAcpiCratServicesProtocol.h>
#include <Protocol/FabricNumaServicesProtocol.h>
#include "Filecode.h"


#define FILECODE FABRIC_RV_FABRICRVDXE_FABRICACPICRAT_FILECODE

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
FabricGetCratMemoryInfo (
  IN       AMD_FABRIC_ACPI_CRAT_SERVICES_PROTOCOL     *This,
  IN       CRAT_HEADER                                *CratHeaderStructPtr,
  IN OUT   UINT8                                     **TableEnd
  );

VOID
CreateCratMemoryEntry (
  IN       CRAT_HEADER  *CratHeaderStructPtr,
  IN OUT   UINT8   **TablePointer,
  IN       UINT32  Domain,
  IN       UINT64  RegionBase,
  IN       UINT64  RegionSize
  );

/*----------------------------------------------------------------------------------------
 *                           G L O B A L   V A R I A B L E S
 *----------------------------------------------------------------------------------------
 */
STATIC AMD_FABRIC_ACPI_CRAT_SERVICES_PROTOCOL  mFabricAcpiCratServicesProtocol = {
  0x1,
  FabricGetCratMemoryInfo
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
FabricRvAcpiCratProtocolInstall (
  IN       EFI_HANDLE        ImageHandle,
  IN       EFI_SYSTEM_TABLE  *SystemTable
  )
{
  return (gBS->InstallProtocolInterface (
               &ImageHandle,
               &gAmdFabricAcpiCratServicesProtocolGuid,
               EFI_NATIVE_INTERFACE,
               &mFabricAcpiCratServicesProtocol
               ));
}

EFI_STATUS
EFIAPI
FabricGetCratMemoryInfo (
  IN       AMD_FABRIC_ACPI_CRAT_SERVICES_PROTOCOL     *This,
  IN       CRAT_HEADER                                *CratHeaderStructPtr,
  IN OUT   UINT8                                     **TableEnd
  )
{
  UINT32                         Domain;
  UINT32                         SocketId;
  UINT32                         DieId;
  UINT32                         DramMapIndex;
  UINT64                         MemoryLength;
  UINT64                         MemoryBase;
  UINT64                         SizeBelowHole;
  UINT64                         HoleBase;
  DRAM_BASE_ADDRESS_REGISTER     DramBaseAddr;
  DRAM_LIMIT_ADDRESS_REGISTER    DramLimitAddr;
  DRAM_HOLE_CONTROL_REGISTER     DramHoleCtrl;
  FABRIC_NUMA_SERVICES_PROTOCOL  *FabricNuma;

  if (gBS->LocateProtocol (&gAmdFabricNumaServicesProtocolGuid, NULL, (VOID **) &FabricNuma) != EFI_SUCCESS) {
    return EFI_ABORTED;
  }

  for (DramMapIndex = 0; DramMapIndex < NUMBER_OF_DRAM_REGIONS; DramMapIndex++) {
    DramBaseAddr.Value = FabricRegisterAccRead (0, 0, DRAMBASEADDR0_FUNC, (DRAMBASEADDR0_REG + (DramMapIndex * DRAM_REGION_REGISTER_OFFSET)), RV_IOMS0_INSTANCE_ID);
    if (DramBaseAddr.Field.AddrRngVal == 1) {
      DramLimitAddr.Value = FabricRegisterAccRead (0, 0, DRAMLIMITADDR0_FUNC, (DRAMLIMITADDR0_REG + (DramMapIndex * DRAM_REGION_REGISTER_OFFSET)), RV_IOMS0_INSTANCE_ID);

      SocketId = ((DramLimitAddr.Field.DstFabricID >> FABRIC_ID_SOCKET_SHIFT) & FABRIC_ID_SOCKET_SIZE_MASK);
      DieId = ((DramLimitAddr.Field.DstFabricID >> FABRIC_ID_DIE_SHIFT) & FABRIC_ID_DIE_SIZE_MASK);

      if (FabricNuma->DomainXlat (FabricNuma, (UINTN) SocketId, (UINTN) DieId, &Domain) != EFI_SUCCESS) {
        return EFI_ABORTED;
      }

      MemoryLength = LShiftU64 (((UINT64) (DramLimitAddr.Field.DramLimitAddr + 1 - DramBaseAddr.Field.DramBaseAddr)), 28);
      MemoryBase = LShiftU64 (((UINT64) (DramBaseAddr.Field.DramBaseAddr)), 28);

      if (DramBaseAddr.Field.DramBaseAddr == 0) {
        CreateCratMemoryEntry (CratHeaderStructPtr, TableEnd, Domain, 0ull, 0xA0000ull);
        MemoryBase = (1 * 1024 * 1024);
        MemoryLength -= MemoryBase;
      }

      if (DramBaseAddr.Field.LgcyMmioHoleEn == 1) {
        DramHoleCtrl.Value = FabricRegisterAccRead (0, 0, DRAMHOLECTRL_FUNC, DRAMHOLECTRL_REG, RV_IOMS0_INSTANCE_ID);
        ASSERT (DramHoleCtrl.Field.DramHoleValid == 1);

        HoleBase = LShiftU64 (((UINT64) (DramHoleCtrl.Field.DramHoleBase)), 24);
        SizeBelowHole = HoleBase - MemoryBase;

        CreateCratMemoryEntry (CratHeaderStructPtr, TableEnd, Domain, MemoryBase, SizeBelowHole);

        MemoryBase = 0x100000000;
        MemoryLength = LShiftU64 (((UINT64) (DramLimitAddr.Field.DramLimitAddr + 1 - 0x10)), 28);
      }

      CreateCratMemoryEntry (CratHeaderStructPtr, TableEnd, Domain, MemoryBase, MemoryLength);
    }
  }

  return EFI_SUCCESS;
}


VOID
CreateCratMemoryEntry (
  IN       CRAT_HEADER  *CratHeaderStructPtr,
  IN OUT   UINT8   **TablePointer,
  IN       UINT32  Domain,
  IN       UINT64  RegionBase,
  IN       UINT64  RegionSize
  )
{
  CRAT_MEMORY  *MemoryEntry;

  MemoryEntry = (CRAT_MEMORY *) *TablePointer;
  *TablePointer += sizeof (CRAT_MEMORY);
  CratHeaderStructPtr->TotalEntries++;

  MemoryEntry->Type = CRAT_MEMORY_TYPE;
  MemoryEntry->Length = sizeof (CRAT_MEMORY);
  MemoryEntry->ProximityDomain = Domain;
  MemoryEntry->BaseAddressLow = (UINT32) (RegionBase & 0xFFFFFFFF);
  MemoryEntry->BaseAddressHigh = (UINT32) (RShiftU64 (RegionBase, 32) & 0xFFFFFFFF);
  MemoryEntry->LengthLow = (UINT32) (RegionSize & 0xFFFFFFFF);
  MemoryEntry->LengthHigh = (UINT32) (RShiftU64 (RegionSize, 32) & 0xFFFFFFFF);
  MemoryEntry->Flags.Enabled = 1;
  ///@TODO get from memory protocol
  MemoryEntry->Width = 64;
}


