/**
 * @file
 *
 * Fabric Pstate Service definitions
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Fabric
 * @e \$Revision: 313706 $   @e \$Date: 2015-02-25 21:00:43 -0600 (Wed, 25 Feb 2015) $
 *
 */
/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */
#ifndef _AMD_NBIO_SMU_V10_DXE_H_
#define _AMD_NBIO_SMU_V10_DXE_H_

#include <Protocol/AmdNbioSmuServicesProtocol.h>
#include <Protocol/AmdCoreTopologyProtocol.h>

EFI_STATUS
EFIAPI
AmdSmuReadFuseByName (
  IN  DXE_AMD_NBIO_SMU_SERVICES_PROTOCOL         *This,
  IN       UINT32                                InstanceId,
  IN       UINTN                                 FuseName,
  IN OUT   UINTN                                 *BufferSize,
     OUT   UINT8                                 *FuseValue
  );

EFI_STATUS
EFIAPI
AmdSmuReadFuseByNumber (
  IN  DXE_AMD_NBIO_SMU_SERVICES_PROTOCOL         *This,
  IN  UINT32                                     FuseChain,
  IN  UINT32                                     FuseStartBit,
  IN  UINT32                                     FuseSize,
  OUT UINT32                                     *FuseValue
  );

EFI_STATUS
EFIAPI
AmdSmuServiceRequest (
  IN  DXE_AMD_NBIO_SMU_SERVICES_PROTOCOL         *This,
  IN  UINT32                                     InstanceId,
  IN  UINT32                                     ServiceRequest,
  IN  UINT32                                     *InputParameters,
  OUT UINT32                                     *ReturnValues
  );

EFI_STATUS
EFIAPI
AmdSmuRegisterRead (
  IN  DXE_AMD_NBIO_SMU_SERVICES_PROTOCOL         *This,
  IN  UINT32                                     InstanceId,
  IN  UINT32                                     RegisterIndex,
  OUT UINT32                                     *RegisterValue
  );

EFI_STATUS
EFIAPI
AmdSmuRegisterWrite (
  IN  DXE_AMD_NBIO_SMU_SERVICES_PROTOCOL         *This,
  IN  UINT32                                     InstanceId,
  IN  UINT32                                     RegisterIndex,
  IN  UINT32                                     *RegisterValue
  );

EFI_STATUS
EFIAPI
AmdSmuRegisterRMW (
  IN  DXE_AMD_NBIO_SMU_SERVICES_PROTOCOL         *This,
  IN  UINT32                                     InstanceId,
  IN  UINT32                                     RegisterIndex,
  IN  UINT32                                     RegisterANDValue,
  IN  UINT32                                     RegisterORValue
  );

EFI_STATUS
EFIAPI
AmdSmuReadBrandString (
  IN  DXE_AMD_NBIO_SMU_SERVICES_PROTOCOL         *This,
  IN  UINT32                                     InstanceId,
  IN  UINT32                                     BrandStringLength,
  OUT UINT8                                      *BrandString
  );

BOOLEAN
AmdSmuIsSmtEnable (
  IN  DXE_AMD_NBIO_SMU_SERVICES_PROTOCOL         *This
  );

EFI_STATUS
SmuV10GetCoreTopologyOnDie (
  IN       AMD_CORE_TOPOLOGY_SERVICES_PROTOCOL   *This,
  IN       UINTN                                 Socket,
  IN       UINTN                                 Die,
     OUT   UINTN                                 *NumberOfComplexes,
     OUT   UINTN                                 *NumberOfCores,
     OUT   UINTN                                 *NumberOfThreads
  );

EFI_STATUS
SmuV10LaunchThread (
  IN       AMD_CORE_TOPOLOGY_SERVICES_PROTOCOL   *This,
  IN       UINTN                                 Socket,
  IN       UINTN                                 Die,
  IN       UINTN                                 LogicalComplex,
  IN       UINTN                                 LogicalThread
  );

EFI_STATUS
AmdSmuReadCacWeights (
  IN  DXE_AMD_NBIO_SMU_SERVICES_PROTOCOL         *This,
  IN  UINT32                                     MaxNumWeights,
  OUT UINT64                                     *ApmWeights
  );

EFI_STATUS
AmdSmuReadThrottleThresholds (
  IN  DXE_AMD_NBIO_SMU_SERVICES_PROTOCOL         *This,
  OUT UINT32                                     *Thresholds
  );

EFI_STATUS
SmuGetGnbHandle (
  IN  UINT32                                     InstanceId,
  OUT GNB_HANDLE                                 **MyHandle
  );

EFI_STATUS
SmuDxeInitialize (
  OUT PCIe_PLATFORM_CONFIG                       **Pcie
  );

VOID
EFIAPI
SetSmuFeaturePolicy (
  IN       SOC_LOGICAL_ID                        LogicalId,
  IN       UINT32                                PackageType
  );

#endif

