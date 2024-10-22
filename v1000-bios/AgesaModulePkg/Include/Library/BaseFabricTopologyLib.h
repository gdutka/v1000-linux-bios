/* $NoKeywords:$ */
/**
 * @file
 *
 * Low-level Fabric Topology Services base library
 *
 * Contains interface to the family specific fabric topology base library
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Lib
 * @e \$Revision$   @e \$Date$
 *
 */
/*
 ******************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 **/

#ifndef _BASE_FABRIC_TOPOLOGY_LIB_H_
#define _BASE_FABRIC_TOPOLOGY_LIB_H_
#pragma pack (push, 1)

/// Root Bridge location
typedef struct {
  UINTN       Socket;
  UINTN       Die;
  UINTN       Index;
} ROOT_BRIDGE_LOCATION;

/// Fabric Device Types
typedef enum {
  PrimaryFch,                  ///< Primary FCH
  SlaveFch,                    ///< Slave FCH
  FchTypeMax,                  ///< Not a valid entry type, use for limit checking.
  PrimarySmu,                  ///< Primary SMU
  SlaveSmu,                    ///< Slave SMU
  SmuTypeMax,                  ///< Not a valid entry type, use for limit checking.
  ComponentTypeMax             ///< Not a valid entry type, use for limit checking.
} COMPONENT_TYPE;

/// Component location
typedef struct {
  UINTN                Socket;
  UINTN                Die;
  UINTN                IomsFabricId;
  COMPONENT_TYPE       Type;
} COMPONENT_LOCATION;

/// Fabric Device Types
typedef enum {
  Ccm,                         ///< Processor Family Specific Workarounds which are @b not practical using the other types.
  Gcm,                         ///< Processor Config Space registers via SMN.
  Ncs,                         ///< Processor Config Space registers via SMN.
  Ncm,                         ///< Processor Config Space registers via SMN.
  Pie,                         ///< Processor Config Space registers via SMN.
  Ioms,                        ///< Processor Config Space registers via SMN.
  Cs,                          ///< Processor Config Space registers via SMN.
  Tcdx,                        ///< Processor Config Space registers via SMN.
  Cake,                        ///< Processor Config Space registers via SMN.
  CsUmc,                       ///< Processor Config Space registers via SMN.
  CsCcix,                      ///< Processor Config Space registers via SMN.
  FabricDeviceTypeMax          ///< Not a valid entry type, use for limit checking.
} FABRIC_DEVICE_TYPE;

/// Device ID structure
typedef struct {
  UINTN   FabricID;           ///< Fabric ID
  UINTN   InstanceID;         ///< Instance ID
} DEVICE_IDS;

/// Processor neighbor information
typedef struct {
  UINTN   SocketNumber;       ///< Socket Number
} AMD_FABRIC_TOPOLOGY_PROCESSOR_NEIGHBOR_INFO;

/// Fabric topology structure
typedef struct {
  FABRIC_DEVICE_TYPE   Type;          ///< Type
  UINTN                Count;         ///< Count
  CONST DEVICE_IDS     *IDs;          ///< Device IDs
} AMD_FABRIC_TOPOLOGY_DIE_DEVICE_MAP;

/// Fabric topology neighbor information structure
typedef struct {
  UINTN   SocketNumber;             ///< Socket Number
  UINTN   InternalDieNumber;        ///< Internal Die Number
  UINTN   HostCake;                 ///< Host Cake
  UINTN   NeighborCake;             ///< Neighbor Cake
} AMD_FABRIC_TOPOLOGY_DIE_NEIGHBOR_INFO;

UINTN
FabricTopologyGetNumberOfProcessorsPresent (
  VOID
  );

UINTN
FabricTopologyGetNumberOfSystemDies (
  VOID
  );

UINTN
FabricTopologyGetNumberOfSystemRootBridges (
  VOID
  );

UINTN
FabricTopologyGetNumberOfDiesOnSocket (
  IN       UINTN Socket
  );

UINTN
FabricTopologyGetNumberOfRootBridgesOnSocket (
  IN       UINTN Socket
  );

UINTN
FabricTopologyGetNumberOfRootBridgesOnDie (
  IN       UINTN Socket,
  IN       UINTN Die
  );

UINTN
FabricTopologyGetDieSystemOffset (
  IN       UINTN Socket,
  IN       UINTN Die
  );

CONST
AMD_FABRIC_TOPOLOGY_DIE_DEVICE_MAP *
FabricTopologyGetDeviceMapOnDie (
  IN       UINTN Socket,
  IN       UINTN Die
  );

UINTN
FabricTopologyGetHostBridgeSystemFabricID (
  IN       UINTN Socket,
  IN       UINTN Die,
  IN       UINTN Index
  );

UINTN
FabricTopologyGetHostBridgeBusBase (
  IN       UINTN Socket,
  IN       UINTN Die,
  IN       UINTN Index
  );

UINTN
FabricTopologyGetHostBridgeBusLimit (
  IN       UINTN Socket,
  IN       UINTN Die,
  IN       UINTN Index
  );

BOOLEAN
FabricTopologyGetSystemComponentRootBridgeLocation (
  IN       COMPONENT_TYPE Component,
  IN OUT   ROOT_BRIDGE_LOCATION *Location
  );

BOOLEAN
FabricTopologyHasFch (
  IN       UINTN Socket,
  IN       UINTN Die,
  IN       UINTN Index
  );

BOOLEAN
FabricTopologyHasSmu (
  IN       UINTN Socket,
  IN       UINTN Die,
  IN       UINTN Index
  );

UINTN
FabricTopologyGetPhysRootBridgeNumber (
  IN       UINTN Socket,
  IN       UINTN Die,
  IN       UINTN Index
  );

#pragma pack (pop)
#endif // _BASE_FABRIC_TOPOLOGY_LIB_H_

