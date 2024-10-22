/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Core Topology Services PPI prototype definition
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

#ifndef _AMD_CORE_TOPOLOGY_SERVICES_PPI_H_
#define _AMD_CORE_TOPOLOGY_SERVICES_PPI_H_

#include <AGESA.h>

///
/// Forward declaration for the AMD_CORE_TOPOLOGY_SERVICES_PPI.
///
typedef struct _AMD_CORE_TOPOLOGY_SERVICES_PPI AMD_CORE_TOPOLOGY_SERVICES_PPI;

typedef
EFI_STATUS
(EFIAPI *AMD_CORE_TOPOLOGY_SERVICES_GET_CORE_TOPOLOGY_ON_DIE) (
  IN       AMD_CORE_TOPOLOGY_SERVICES_PPI   *This,
  IN       UINTN                            Socket,
  IN       UINTN                            Die,
     OUT   UINTN                            *NumberOfComplexes,
     OUT   UINTN                            *NumberOfCores,
     OUT   UINTN                            *NumberOfThreads
  );

typedef
EFI_STATUS
(EFIAPI *AMD_CORE_TOPOLOGY_SERVICES_LAUNCH_THREAD) (
  IN       AMD_CORE_TOPOLOGY_SERVICES_PPI   *This,
  IN       UINTN                            Socket,
  IN       UINTN                            Die,
  IN       UINTN                            LogicalComplex,
  IN       UINTN                            LogicalThread
  );

///
/// When installed, the AMD Core Topology Services PPI produces a
/// collection of services that provide information on Core Topology.
///
struct _AMD_CORE_TOPOLOGY_SERVICES_PPI {
  AMD_CORE_TOPOLOGY_SERVICES_GET_CORE_TOPOLOGY_ON_DIE  GetCoreTopologyOnDie;
  AMD_CORE_TOPOLOGY_SERVICES_LAUNCH_THREAD             LaunchThread;
};

extern EFI_GUID gAmdCoreTopologyServicesPpiGuid;

#endif

