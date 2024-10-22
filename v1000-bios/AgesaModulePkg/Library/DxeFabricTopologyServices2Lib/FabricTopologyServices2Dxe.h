/* $NoKeywords:$ */
/**
 * @file
 *
 * Fabric Topology Service definitions
 *
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
#ifndef _FABRIC_TOPOLOGY_SERVICES2_DXE_H_
#define _FABRIC_TOPOLOGY_SERVICES2_DXE_H_

EFI_STATUS
EFIAPI
FabricTopologyGetSystemInfo2 (
  IN       AMD_FABRIC_TOPOLOGY_SERVICES2_PROTOCOL *This,
     OUT   UINTN                                  *NumberOfInstalledProcessors,
     OUT   UINTN                                  *TotalNumberOfDie,
     OUT   UINTN                                  *TotalNumberOfRootBridges,
     OUT   ROOT_BRIDGE_LOCATION                   *SystemFchRootBridgeLocation,
     OUT   ROOT_BRIDGE_LOCATION                   *SystemSmuRootBridgeLocation
  );

EFI_STATUS
EFIAPI
FabricTopologyGetProcessorInfo2 (
  IN       AMD_FABRIC_TOPOLOGY_SERVICES2_PROTOCOL *This,
  IN       UINTN                                   Socket,
     OUT   UINTN                                  *NumberOfDie,
     OUT   UINTN                                  *NumberOfRootBridges
  );

EFI_STATUS
EFIAPI
FabricTopologyGetDieInfo2 (
  IN       AMD_FABRIC_TOPOLOGY_SERVICES2_PROTOCOL *This,
  IN       UINTN                                   Socket,
  IN       UINTN                                   Die,
     OUT   UINTN                                  *NumberOfRootBridges,
     OUT   UINTN                                  *SystemIdOffset,
     OUT   AMD_FABRIC_TOPOLOGY_DIE_DEVICE_MAP    **FabricIdMap
  );

EFI_STATUS
EFIAPI
FabricTopologyGetRootBridgeInfo2 (
  IN       AMD_FABRIC_TOPOLOGY_SERVICES2_PROTOCOL *This,
  IN       UINTN                                   Socket,
  IN       UINTN                                   Die,
  IN       UINTN                                   Index,
     OUT   UINTN                                  *SystemFabricID,
     OUT   UINTN                                  *BusNumberBase,
     OUT   UINTN                                  *BusNumberLimit,
     OUT   UINTN                                  *PhysicalRootBridgeNumber,
     OUT   BOOLEAN                                *HasFchDevice,
     OUT   BOOLEAN                                *HasSystemMgmtUnit
  );

#endif

