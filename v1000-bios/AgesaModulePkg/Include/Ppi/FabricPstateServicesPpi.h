/* $NoKeywords:$ */
/**
 * @file
 *
 * Fabric Pstate Services PPI prototype definition
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

#ifndef _FABRIC_PSTATE_SERVICES_PPI_H_
#define _FABRIC_PSTATE_SERVICES_PPI_H_

#define NB_HIGH 0x80
#define NB_LOW  0X81

///
/// Forward declaration for the AMD_PEI_FABRIC_PSTATE_SERVICES_PPI.
///
typedef struct _AMD_PEI_FABRIC_PSTATE_SERVICES_PPI AMD_PEI_FABRIC_PSTATE_SERVICES_PPI;

typedef
EFI_STATUS
(EFIAPI *AMD_PEI_FABRIC_PSTATE_SERVICES_GET_SYSTEM_INFO) (
  IN       AMD_PEI_FABRIC_PSTATE_SERVICES_PPI   *This,
     OUT   BOOLEAN                              *AreAllSocketPstatesInTheSameDomain
  );

typedef
EFI_STATUS
(EFIAPI *AMD_PEI_FABRIC_PSTATE_SERVICES_GET_SOCKET_INFO) (
  IN       AMD_PEI_FABRIC_PSTATE_SERVICES_PPI   *This,
  IN       UINTN                                Socket,
     OUT   BOOLEAN                              *AreAllDiePstatesInTheSameDomain
  );

typedef
EFI_STATUS
(EFIAPI *AMD_PEI_FABRIC_PSTATE_SERVICES_GET_DIE_INFO) (
  IN       AMD_PEI_FABRIC_PSTATE_SERVICES_PPI   *This,
  IN       UINTN                                Socket,
  IN       UINTN                                Die,
     OUT   BOOLEAN                              *IsSwitchingEnabled,
     OUT   UINTN                                *TotalNumberOfStates
  );

typedef
EFI_STATUS
(EFIAPI *AMD_PEI_FABRIC_PSTATE_SERVICES_GET_PSTATE_INFO) (
  IN       AMD_PEI_FABRIC_PSTATE_SERVICES_PPI   *This,
  IN       UINTN                                Socket,
  IN       UINTN                                Die,
  IN       UINTN                                Pstate,
     OUT   UINTN                                *Frequency,
     OUT   UINTN                                *Voltage,
     OUT   UINTN                                *Power,
     OUT   UINTN                                *AssociatedMstate
  );

typedef
EFI_STATUS
(EFIAPI *AMD_PEI_FABRIC_PSTATE_SERVICES_FORCE_PSTATE) (
  IN       AMD_PEI_FABRIC_PSTATE_SERVICES_PPI   *This,
  IN       UINTN                                Socket,
  IN       UINTN                                Die,
  IN       UINTN                                Pstate
  );

typedef
EFI_STATUS
(EFIAPI *AMD_PEI_FABRIC_PSTATE_SERVICES_RELEASE_PSTATE_FORCE) (
  IN       AMD_PEI_FABRIC_PSTATE_SERVICES_PPI   *This,
  IN       UINTN                                Socket,
  IN       UINTN                                Die
  );

typedef
EFI_STATUS
(EFIAPI *AMD_PEI_FABRIC_PSTATE_SERVICES_GET_CURRENT_PSTATE) (
  IN       AMD_PEI_FABRIC_PSTATE_SERVICES_PPI   *This,
  IN       UINTN                                Socket,
  IN       UINTN                                Die,
     OUT   UINTN                                *Pstate
  );

///
/// When installed, the Fabric P-state Services PPI produces a collection of
/// services that are needed for Fabric P-state management.
///
/// When using the P-state force mechanism, the caller should take care to
/// release the forcing mechanism prior to continuing the boot process.  The
/// Fabric DXE driver will ensure that the force is disabled early in the DXE
/// phase in case the call to release was not made.
///
struct _AMD_PEI_FABRIC_PSTATE_SERVICES_PPI {
  AMD_PEI_FABRIC_PSTATE_SERVICES_GET_SYSTEM_INFO        GetSystemInfo;
  AMD_PEI_FABRIC_PSTATE_SERVICES_GET_SOCKET_INFO        GetSocketInfo;
  AMD_PEI_FABRIC_PSTATE_SERVICES_GET_DIE_INFO           GetDieInfo;
  AMD_PEI_FABRIC_PSTATE_SERVICES_GET_PSTATE_INFO        GetPstateInfo;
  AMD_PEI_FABRIC_PSTATE_SERVICES_FORCE_PSTATE           ForcePstate;
  AMD_PEI_FABRIC_PSTATE_SERVICES_RELEASE_PSTATE_FORCE   ReleasePstateForce;
  AMD_PEI_FABRIC_PSTATE_SERVICES_GET_CURRENT_PSTATE     GetCurrentPstate;
};

extern EFI_GUID gAmdFabricPstateServicesPpiGuid;

#endif

