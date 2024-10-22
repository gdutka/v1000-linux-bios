/* $NoKeywords:$ */
/**
 * @file
 *
 * Fabric Pstate Service definitions
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
#ifndef _AMD_AGESA_FABRIC_PSTATE_SERVICES_PEI_H_
#define _AMD_AGESA_FABRIC_PSTATE_SERVICES_PEI_H_

#include <Ppi/FabricPstateServicesPpi.h>
#include <Library/FabricPstatesLib.h>

EFI_STATUS
EFIAPI
FabricPstateServicePpiInstall (
  IN       CONST EFI_PEI_SERVICES     **PeiServices
  );

EFI_STATUS
EFIAPI
FabricPstateGetSystemInfo (
  IN       AMD_PEI_FABRIC_PSTATE_SERVICES_PPI   *This,
     OUT   BOOLEAN                              *AreAllSocketPstatesInTheSameDomain
  );

EFI_STATUS
EFIAPI
FabricPstateGetSocketInfo (
  IN       AMD_PEI_FABRIC_PSTATE_SERVICES_PPI   *This,
  IN       UINTN                                Socket,
     OUT   BOOLEAN                              *AreAllDiePstatesInTheSameDomain
  );

EFI_STATUS
EFIAPI
FabricPstateGetDieInfo (
  IN       AMD_PEI_FABRIC_PSTATE_SERVICES_PPI   *This,
  IN       UINTN                                Socket,
  IN       UINTN                                Die,
     OUT   BOOLEAN                              *IsSwitchingEnabled,
     OUT   UINTN                                *TotalNumberOfStates
  );

EFI_STATUS
EFIAPI
FabricPstateGetPstateInfo (
  IN       AMD_PEI_FABRIC_PSTATE_SERVICES_PPI   *This,
  IN       UINTN                                Socket,
  IN       UINTN                                Die,
  IN       UINTN                                Pstate,
     OUT   UINTN                                *Frequency,
     OUT   UINTN                                *Voltage,
     OUT   UINTN                                *Power,
     OUT   UINTN                                *AssociatedMstate
  );

EFI_STATUS
EFIAPI
FabricPstateForcePstate (
  IN       AMD_PEI_FABRIC_PSTATE_SERVICES_PPI   *This,
  IN       UINTN                                Socket,
  IN       UINTN                                Die,
  IN       UINTN                                Pstate
  );

EFI_STATUS
EFIAPI
FabricPstateReleasePstateForce (
  IN       AMD_PEI_FABRIC_PSTATE_SERVICES_PPI   *This,
  IN       UINTN                                Socket,
  IN       UINTN                                Die
  );

EFI_STATUS
EFIAPI
FabricPstateGetCurrentPstate (
  IN       AMD_PEI_FABRIC_PSTATE_SERVICES_PPI   *This,
  IN       UINTN                                Socket,
  IN       UINTN                                Die,
     OUT   UINTN                                *Pstate
  );

#endif

