/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Core Topology Services Protocol prototype definition
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Fabric
 * @e \$Revision: 313706 $   @e \$Date: 2015-02-25 21:00:43 -0600 (Wed, 25 Feb 2015) $
 */
/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 **/

#ifndef _AMD_CORE_TOPOLOGY_SERVICES_V2_PROTOCOL_H_
#define _AMD_CORE_TOPOLOGY_SERVICES_V2_PROTOCOL_H_


///
/// Forward declaration for the AMD_CORE_TOPOLOGY_SERVICES_V2_PROTOCOL.
///
typedef struct _AMD_CORE_TOPOLOGY_SERVICES_V2_PROTOCOL AMD_CORE_TOPOLOGY_SERVICES_V2_PROTOCOL;

typedef
EFI_STATUS
(EFIAPI *AMD_CORE_TOPOLOGY_SERVICES_GET_CORE_TOPOLOGY_ON_DIE_V2) (
  IN       AMD_CORE_TOPOLOGY_SERVICES_V2_PROTOCOL  *This,
  IN       UINTN                                   Socket,
  IN       UINTN                                   Die,
     OUT   UINTN                                   *NumberOfCcds,
     OUT   UINTN                                   *NumberOfComplexes,
     OUT   UINTN                                   *NumberOfCores,
     OUT   UINTN                                   *NumberOfThreads
  );

typedef
EFI_STATUS
(EFIAPI *AMD_CORE_TOPOLOGY_SERVICES_LAUNCH_THREAD_V2) (
  IN       AMD_CORE_TOPOLOGY_SERVICES_V2_PROTOCOL  *This,
  IN       UINTN                                   Socket,
  IN       UINTN                                   Die,
  IN       UINTN                                   LogicalCcd,
  IN       UINTN                                   LogicalComplex,
  IN       UINTN                                   LogicalCore,
  IN       UINTN                                   LogicalThread
  );

///
/// When installed, the AMD Core Topology Services PROTOCOL produces a
/// collection of services that provide information on Core Topology.
///
struct _AMD_CORE_TOPOLOGY_SERVICES_V2_PROTOCOL {
  AMD_CORE_TOPOLOGY_SERVICES_GET_CORE_TOPOLOGY_ON_DIE_V2  GetCoreTopologyOnDie;
  AMD_CORE_TOPOLOGY_SERVICES_LAUNCH_THREAD_V2             LaunchThread;
};

extern EFI_GUID gAmdCoreTopologyServicesV2ProtocolGuid;

#endif

