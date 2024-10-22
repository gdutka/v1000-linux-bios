/* $NoKeywords:$ */
/**
 * @file
 *
 * Fabric Topology Services V2 Protocol prototype definition
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Fabric
 * @e \$Revision$   @e \$Date$
 */
/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 **/
#ifndef _FABRIC_TOPOLOGY_SERVICES2_H_
#define _FABRIC_TOPOLOGY_SERVICES2_H_

#include <Library/BaseFabricTopologyLib.h>

///
/// Forward declaration for the AMD_FABRIC_TOPOLOGY_SERVICES2_PROTOCOL.
///
typedef struct _AMD_FABRIC_TOPOLOGY_SERVICES2_PROTOCOL AMD_FABRIC_TOPOLOGY_SERVICES2_PROTOCOL;

typedef
EFI_STATUS
(EFIAPI *AMD_FABRIC_TOPOLOGY_SERVICES2_GET_SYSTEM_INFO) (
  IN       AMD_FABRIC_TOPOLOGY_SERVICES2_PROTOCOL     *This,
     OUT   UINTN                                      *NumberOfInstalledProcessors,
     OUT   UINTN                                      *TotalNumberOfDie,
     OUT   UINTN                                      *TotalNumberOfRootBridges,
     OUT   ROOT_BRIDGE_LOCATION                       *SystemFchRootBridgeLocation,
     OUT   ROOT_BRIDGE_LOCATION                       *SystemSmuRootBridgeLocation
  );

typedef
EFI_STATUS
(EFIAPI *AMD_FABRIC_TOPOLOGY_SERVICES2_GET_PROCESSOR_INFO) (
  IN       AMD_FABRIC_TOPOLOGY_SERVICES2_PROTOCOL     *This,
  IN       UINTN                                       Socket,
     OUT   UINTN                                      *NumberOfDie,
     OUT   UINTN                                      *NumberOfRootBridges
  );

typedef
EFI_STATUS
(EFIAPI *AMD_FABRIC_TOPOLOGY_SERVICES2_GET_DIE_INFO) (
  IN       AMD_FABRIC_TOPOLOGY_SERVICES2_PROTOCOL     *This,
  IN       UINTN                                       Socket,
  IN       UINTN                                       Die,
     OUT   UINTN                                      *NumberOfRootBridges,
     OUT   UINTN                                      *SystemIdOffset,
     OUT   AMD_FABRIC_TOPOLOGY_DIE_DEVICE_MAP        **FabricIdMap
  );

typedef
EFI_STATUS
(EFIAPI *AMD_FABRIC_TOPOLOGY_SERVICES2_GET_ROOT_BRIDGE_INFO) (
  IN       AMD_FABRIC_TOPOLOGY_SERVICES2_PROTOCOL     *This,
  IN       UINTN                                       Socket,
  IN       UINTN                                       Die,
  IN       UINTN                                       Index,
     OUT   UINTN                                      *SystemFabricID,
     OUT   UINTN                                      *BusNumberBase,
     OUT   UINTN                                      *BusNumberLimit,
     OUT   UINTN                                      *PhysicalRootBridgeNumber,
     OUT   BOOLEAN                                    *HasFchDevice,
     OUT   BOOLEAN                                    *HasSystemMgmtUnit
      );

///
/// When installed, the Fabric Topology Services produces a collection of
/// services that provide information on Fabric Topology.
///
struct _AMD_FABRIC_TOPOLOGY_SERVICES2_PROTOCOL {
  AMD_FABRIC_TOPOLOGY_SERVICES2_GET_SYSTEM_INFO        GetSystemInfo;
  AMD_FABRIC_TOPOLOGY_SERVICES2_GET_PROCESSOR_INFO     GetProcessorInfo;
  AMD_FABRIC_TOPOLOGY_SERVICES2_GET_DIE_INFO           GetDieInfo;
  AMD_FABRIC_TOPOLOGY_SERVICES2_GET_ROOT_BRIDGE_INFO   GetRootBridgeInfo;
};

extern EFI_GUID gAmdFabricTopologyServices2ProtocolGuid;
extern EFI_GUID gAmdFabricTopologyServices2SmmProtocolGuid;

#endif

