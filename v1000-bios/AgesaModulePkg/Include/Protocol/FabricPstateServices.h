/* $NoKeywords:$ */
/**
 * @file
 *
 * Fabric Pstate Services Protocol prototype definition
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

#ifndef _FABRIC_PSTATE_SERVICES_PROTOCOL_H_
#define _FABRIC_PSTATE_SERVICES_PROTOCOL_H_

///
/// Forward declaration for the AMD_FABRIC_PSTATE_SERVICES_PROTOCOL.
///
typedef struct _AMD_FABRIC_PSTATE_SERVICES_PROTOCOL AMD_FABRIC_PSTATE_SERVICES_PROTOCOL;

typedef
EFI_STATUS
(EFIAPI *AMD_FABRIC_PSTATE_SERVICES_GET_SYSTEM_INFO) (
  IN       AMD_FABRIC_PSTATE_SERVICES_PROTOCOL   *This,
     OUT   BOOLEAN                               *AreAllSocketPstatesInTheSameDomain
  );

typedef
EFI_STATUS
(EFIAPI *AMD_FABRIC_PSTATE_SERVICES_GET_SOCKET_INFO) (
  IN       AMD_FABRIC_PSTATE_SERVICES_PROTOCOL   *This,
  IN       UINTN                                 Socket,
     OUT   BOOLEAN                               *AreAllDiePstatesInTheSameDomain
  );

typedef
EFI_STATUS
(EFIAPI *AMD_FABRIC_PSTATE_SERVICES_GET_DIE_INFO) (
  IN       AMD_FABRIC_PSTATE_SERVICES_PROTOCOL   *This,
  IN       UINTN                                 Socket,
  IN       UINTN                                 Die,
     OUT   BOOLEAN                               *IsSwitchingEnabled,
     OUT   UINTN                                 *TotalNumberOfStates
  );

typedef
EFI_STATUS
(EFIAPI *AMD_FABRIC_PSTATE_SERVICES_GET_PSTATE_INFO) (
  IN       AMD_FABRIC_PSTATE_SERVICES_PROTOCOL   *This,
  IN       UINTN                                 Socket,
  IN       UINTN                                 Die,
  IN       UINTN                                 Pstate,
     OUT   UINTN                                 *Frequency,
     OUT   UINTN                                 *Voltage,
     OUT   UINTN                                 *Power,
     OUT   UINTN                                 *AssociatedMstate
  );

typedef
EFI_STATUS
(EFIAPI *AMD_FABRIC_PSTATE_SERVICES_GET_CURRENT_PSTATE) (
  IN       AMD_FABRIC_PSTATE_SERVICES_PROTOCOL   *This,
  IN       UINTN                                 Socket,
  IN       UINTN                                 Die,
     OUT   UINTN                                 *Pstate
  );

///
/// When installed, the Fabric P-state Services Protocol produces a collection of
/// services that return various information about the fabric P-states.
///
struct _AMD_FABRIC_PSTATE_SERVICES_PROTOCOL {
  AMD_FABRIC_PSTATE_SERVICES_GET_SYSTEM_INFO      GetSystemInfo;
  AMD_FABRIC_PSTATE_SERVICES_GET_SOCKET_INFO      GetSocketInfo;
  AMD_FABRIC_PSTATE_SERVICES_GET_DIE_INFO         GetDieInfo;
  AMD_FABRIC_PSTATE_SERVICES_GET_PSTATE_INFO      GetPstateInfo;
  AMD_FABRIC_PSTATE_SERVICES_GET_CURRENT_PSTATE   GetCurrentPstate;
};

extern EFI_GUID gAmdFabricPstateServicesProtocolGuid;

#endif

