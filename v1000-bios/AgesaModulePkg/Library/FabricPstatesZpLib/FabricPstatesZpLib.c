/* $NoKeywords:$ */
/**
 * @file
 *
 * Base Fabric Pstates Lib implementation for DF1.0
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Fabric
 * @e \$Revision$   @e \$Date$
 *
 */
/*
 ******************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */


/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include <Library/FabricPstatesLib.h>
#include <Filecode.h>

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
#define FILECODE LIBRARY_FABRICPSTATESZPLIB_FABRICPSTATESZPLIB_FILECODE

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

BOOLEAN
FabricPstatesAreAllSocketsInAUnifiedDomain (
  VOID
  )
{
  return TRUE;
}

BOOLEAN
FabricPstatesAreAllDieOnSocketInAUnifiedDomain (
  IN       UINTN  Socket
  )
{
  return TRUE;
}

BOOLEAN
FabricPstatesIsSwitchingEnabledOnDie (
  IN       UINTN  Socket,
  IN       UINTN  Die
  )
{
  // Read F5x174[NbPstateDis]
  return TRUE;
}

UINTN
FabricPstatesGetNumberOfPstatesOnDie (
  IN       UINTN  Socket,
  IN       UINTN  Die
  )
{
  // Read F5
  return 4;
}

VOID
FabricPstatesGetPstateInfo (
  IN       UINTN               Socket,
  IN       UINTN               Die,
  IN       UINTN               Pstate,
     OUT   FABRIC_PSTATE_INFO  *Info
  )
{
  Info->Frequency = Pstate * 800;
  Info->Voltage = 1550 - (Pstate * 200);
  Info->Power = 400 * Pstate;
  Info->AssociatedMstate = Pstate != 3 ? 0 : 1;
}

BOOLEAN
FabricPstatesForcePstateOnDie (
  IN       UINTN  Socket,
  IN       UINTN  Die,
  IN       UINTN  Pstate
  )
{
  // Check for previous force and save pstatectrl if none?

  // Check SmuCfgLock and return FALSE if appropriate

  // Force
  return TRUE;
}

VOID
FabricPstatesReleaseForceOnDie (
  IN       UINTN  Socket,
  IN       UINTN  Die
  )
{
  // Restore pstatectrl
}

UINTN
FabricPstatesGetCurrentPstateOnDie (
  IN       UINTN  Socket,
  IN       UINTN  Die
  )
{
  // Check F5
  return 0;
}

