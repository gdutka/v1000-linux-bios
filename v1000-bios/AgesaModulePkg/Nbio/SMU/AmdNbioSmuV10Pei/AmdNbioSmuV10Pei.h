/**
 * @file
 *
 * SMU V10 Service definitions
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Fabric
 * @e \$Revision:
 *
 */
/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */
#ifndef _AMD_NBIO_SMU_V10_PEI_H_
#define _AMD_NBIO_SMU_V10_PEI_H_

#include <Ppi/NbioSmuServicesPpi.h>
#include <Ppi/AmdCoreTopologyServicesPpi.h>

EFI_STATUS
EFIAPI
SmuReadFuseByName (
  IN  PEI_AMD_NBIO_SMU_SERVICES_PPI       *This,
  IN       UINT32                         InstanceId,
  IN       UINTN                          FuseName,
  IN OUT   UINTN                          *BufferSize,
     OUT   UINT8                          *FuseValue
  );

EFI_STATUS
EFIAPI
SmuReadFuseByLocation (
  IN  PEI_AMD_NBIO_SMU_SERVICES_PPI       *This,
  IN  UINT32                              InstanceId,
  IN  UINT32                              FuseChain,
  IN  UINT32                              FuseStartBit,
  IN  UINT32                              FuseSize,
  OUT UINT32                              *FuseValue
  );

EFI_STATUS
EFIAPI
SmuServiceRequest (
  IN  PEI_AMD_NBIO_SMU_SERVICES_PPI       *This,
  IN  UINT32                              InstanceId,
  IN  UINT32                              ServiceRequest,
  IN  UINT32                              *InputParameters,
  OUT UINT32                              *ReturnValues
  );

EFI_STATUS
EFIAPI
SmuRegisterRead (
  IN  PEI_AMD_NBIO_SMU_SERVICES_PPI       *This,
  IN  UINT32                              InstanceId,
  IN  UINT32                              RegisterIndex,
  OUT UINT32                              *RegisterValue
  );

EFI_STATUS
EFIAPI
SmuRegisterWrite (
  IN  PEI_AMD_NBIO_SMU_SERVICES_PPI       *This,
  IN  UINT32                              InstanceId,
  IN  UINT32                              RegisterIndex,
  IN  UINT32                              *RegisterValue
  );

EFI_STATUS
EFIAPI
SmuRegisterRMW (
  IN  PEI_AMD_NBIO_SMU_SERVICES_PPI       *This,
  IN  UINT32                              InstanceId,
  IN  UINT32                              RegisterIndex,
  IN  UINT32                              RegisterANDValue,
  IN  UINT32                              RegisterORValue
  );


EFI_STATUS
EFIAPI
SmuHtcControl (
  IN  PEI_AMD_NBIO_SMU_SERVICES_PPI       *This,
  IN  UINT32                              InstanceId,
  OUT BOOLEAN                             *HtcStatus
  );

EFI_STATUS
EFIAPI
SmuReadCacWeights (
  IN  PEI_AMD_NBIO_SMU_SERVICES_PPI       *This,
  IN  UINT32                              InstanceId,
  IN  UINT32                              MaxNumWeights,
  OUT UINT64                              *ApmWeights,
  OUT UINT32                              *NumWeightsRead,
  OUT UINT32                              *EnergyCountNormalize
  );

EFI_STATUS
EFIAPI
SmuPsiControl (
  IN  PEI_AMD_NBIO_SMU_SERVICES_PPI       *This,
  IN  UINT32                              InstanceId,
  IN  UINT32                              TargetVid,
  IN  UINT32                              TargetNbVid
  );

EFI_STATUS
EFIAPI
SmuGetTdpValue (
  IN  PEI_AMD_NBIO_SMU_SERVICES_PPI       *This,
  IN  UINT32                              InstanceId,
  OUT UINT32                              *TdpValue
  );

EFI_STATUS
EFIAPI
SmuReadBrandString (
  IN  PEI_AMD_NBIO_SMU_SERVICES_PPI       *This,
  IN  UINT32                              InstanceId,
  IN  UINT32                              BrandStringLength,
  OUT UINT8                               *BrandString
  );

EFI_STATUS
EFIAPI
SmuReadCorePllLockTimer (
  IN  PEI_AMD_NBIO_SMU_SERVICES_PPI        *This,
  IN  UINT32                               InstanceId,
  OUT UINT32                               *LockTimerValue
  );

EFI_STATUS
AmdNbioSmuEarlyInit (
  IN       PCIe_PLATFORM_CONFIG            *Pcie
  );

#endif

