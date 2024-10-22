/* $NoKeywords:$ */
/**
 * @file
 *
 * Low-level Fabric P-state Services library
 *
 * Contains interface to the family specific fabric p-state library
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

#ifndef _FABRIC_PSTATES_LIB_H_
#define _FABRIC_PSTATES_LIB_H_
#pragma pack (push, 1)

/// Fabric Pstate Information
typedef struct {
  UINTN   Frequency;          ///< Frequency
  UINTN   Voltage;            ///< Voltage
  UINTN   Power;              ///< Power
  UINTN   AssociatedMstate;   ///< AssociatedMstate
} FABRIC_PSTATE_INFO;

BOOLEAN
FabricPstatesAreAllSocketsInAUnifiedDomain (
  VOID
  );

BOOLEAN
FabricPstatesAreAllDieOnSocketInAUnifiedDomain (
  IN       UINTN  Socket
  );

BOOLEAN
FabricPstatesIsSwitchingEnabledOnDie (
  IN       UINTN  Socket,
  IN       UINTN  Die
  );

UINTN
FabricPstatesGetNumberOfPstatesOnDie (
  IN       UINTN  Socket,
  IN       UINTN  Die
  );

VOID
FabricPstatesGetPstateInfo (
  IN       UINTN               Socket,
  IN       UINTN               Die,
  IN       UINTN               Pstate,
     OUT   FABRIC_PSTATE_INFO  *Info
  );

BOOLEAN
FabricPstatesForcePstateOnDie (
  IN       UINTN  Socket,
  IN       UINTN  Die,
  IN       UINTN  Pstate
  );

VOID
FabricPstatesReleaseForceOnDie (
  IN       UINTN  Socket,
  IN       UINTN  Die
  );

UINTN
FabricPstatesGetCurrentPstateOnDie (
  IN       UINTN  Socket,
  IN       UINTN  Die
  );

#pragma pack (pop)
#endif // _FABRIC_PSTATES_LIB_H_

