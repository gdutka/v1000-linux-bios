/* $NoKeywords:$ */
/**
 * @file
 *
 * Fabric Topology Base Lib implementation for ZP
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
#include <Library/BaseLib.h>
#include <Library/BaseFabricTopologyLib.h>
#include <Library/AmdBaseLib.h>
#include <Library/CcxBaseX86Lib.h>
#include <Library/FabricRegisterAccLib.h>
#include <FabricRegistersZP.h>

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
#define FILECODE  LIBRARY_BASEFABRICTOPOLOGYZPLIB_BASEFABRICTOPOLOGYZPLIB_FILECODE

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

CONST DEVICE_IDS ROMDATA  ZpCsMap [] = {
  {0x00000000, 0x00000000},
  {0x00000001, 0x00000001}
};

CONST DEVICE_IDS ROMDATA  ZpCcmMap [] = {
  {0x00000002, 0x00000002},
  {0x00000003, 0x00000003}
};

CONST DEVICE_IDS ROMDATA  ZpIomsMap [] = {
  {0x00000004, 0x00000004}
};

CONST DEVICE_IDS ROMDATA  ZpPieMap [] = {
  {0x00000005, 0x00000005}
};

CONST DEVICE_IDS ROMDATA  ZpCakeMap [] = {
  {0xFFFFFFFF, 0x00000006},
  {0xFFFFFFFF, 0x00000007},
  {0xFFFFFFFF, 0x00000008},
  {0xFFFFFFFF, 0x00000009},
  {0xFFFFFFFF, 0x0000000A},
  {0xFFFFFFFF, 0x0000000B}
};

CONST DEVICE_IDS ROMDATA  ZpTcdxMap [] = {
  {0xFFFFFFFF, 0x0000000C},
  {0xFFFFFFFF, 0x0000000D},
  {0xFFFFFFFF, 0x0000000E},
  {0xFFFFFFFF, 0x0000000F},
  {0xFFFFFFFF, 0x00000010},
  {0xFFFFFFFF, 0x00000011},
  {0xFFFFFFFF, 0x00000012}
};

CONST AMD_FABRIC_TOPOLOGY_DIE_DEVICE_MAP ROMDATA ZpDeviceMap[] = {
  {Cs,   (UINTN) (sizeof (ZpCsMap) / sizeof (ZpCsMap[0])),     &ZpCsMap[0]},
  {Ccm,  (UINTN) (sizeof (ZpCcmMap) / sizeof (ZpCcmMap[0])),   &ZpCcmMap[0]},
  {Ioms, (UINTN) (sizeof (ZpIomsMap) / sizeof (ZpIomsMap[0])), &ZpIomsMap[0]},
  {Pie,  (UINTN) (sizeof (ZpPieMap) / sizeof (ZpPieMap[0])),   &ZpPieMap[0]},
  {Cake, (UINTN) (sizeof (ZpCakeMap) / sizeof (ZpCakeMap[0])), &ZpCakeMap[0]},
  {Tcdx, (UINTN) (sizeof (ZpTcdxMap) / sizeof (ZpTcdxMap[0])), &ZpTcdxMap[0]},
};

UINTN
FabricTopologyGetNumberOfProcessorsPresent (
  VOID
  )
{
  SYS_CFG_REGISTER  SysCfg;

  SysCfg.Value = FabricRegisterAccRead (0, 0, SYSCFG_FUNC, SYSCFG_REG, FABRIC_REG_ACC_BC);

  return (UINTN) (SysCfg.Field.OtherSocketPresent + 1);
}

UINTN
FabricTopologyGetNumberOfSystemDies (
  VOID
  )
{
  SYS_COMP_COUNT_REGISTER  SystemCompCount;

  SystemCompCount.Value = FabricRegisterAccRead (0, 0, SYSCOMPCOUNT_FUNC, SYSCOMPCOUNT_REG, FABRIC_REG_ACC_BC);

  return (UINTN) SystemCompCount.Field.PIECount;
}

UINTN
FabricTopologyGetNumberOfSystemRootBridges (
  VOID
  )
{
  return FabricTopologyGetNumberOfSystemDies ();
}

UINTN
FabricTopologyGetNumberOfDiesOnSocket (
  IN       UINTN Socket
  )
{
  SYS_CFG_REGISTER  SysCfg;

  SysCfg.Value = FabricRegisterAccRead (Socket, 0, SYSCFG_FUNC, SYSCFG_REG, FABRIC_REG_ACC_BC);

  return (UINTN) LowBitSet32 (SysCfg.Field.LocalDiePresent + 1);
}

UINTN
FabricTopologyGetNumberOfRootBridgesOnSocket (
  IN       UINTN Socket
  )
{
  return FabricTopologyGetNumberOfDiesOnSocket (Socket);
}

UINTN
FabricTopologyGetNumberOfRootBridgesOnDie (
  IN       UINTN Socket,
  IN       UINTN Die
  )
{
  return (UINTN) (sizeof (ZpIomsMap) / sizeof (ZpIomsMap[0]));
}

UINTN
FabricTopologyGetDieSystemOffset (
  IN       UINTN Socket,
  IN       UINTN Die
  )
{
  return ((Socket << 7) | (Die << 5));
}

CONST
AMD_FABRIC_TOPOLOGY_DIE_DEVICE_MAP *
FabricTopologyGetDeviceMapOnDie (
  IN       UINTN Socket,
  IN       UINTN Die
  )
{
  return &ZpDeviceMap[0];
}

UINTN
FabricTopologyGetHostBridgeSystemFabricID (
  IN       UINTN Socket,
  IN       UINTN Die,
  IN       UINTN Index
  )
{
  return (FabricTopologyGetDieSystemOffset (Socket, Die) + ZpIomsMap[Index].FabricID);
}

UINTN
FabricTopologyGetHostBridgeBusBase (
  IN       UINTN Socket,
  IN       UINTN Die,
  IN       UINTN Index
  )
{
  CFG_ADDRESS_CTRL_REGISTER  CfgAddrCtrl;

  CfgAddrCtrl.Value = FabricRegisterAccRead (Socket, Die, CFGADDRESSCTRL_FUNC, CFGADDRESSCTRL_REG, FABRIC_REG_ACC_BC);
  return (UINTN) (CfgAddrCtrl.Field.SecBusNum);
}

UINTN
FabricTopologyGetHostBridgeBusLimit (
  IN       UINTN Socket,
  IN       UINTN Die,
  IN       UINTN Index
  )
{
  UINTN                     BusLimit;
  UINTN                     Register;
  CFG_ADDRESS_MAP_REGISTER  BusMap;

  BusLimit = 0xFF;
  for (Register = CFGADDRESSMAP0_REG; Register <= CFGADDRESSMAP7_REG; Register += 4) {
    BusMap.Value = FabricRegisterAccRead (0, 0, CFGADDRESSMAP_FUNC, Register, FABRIC_REG_ACC_BC);
    if ((BusMap.Field.RE == 1) && (BusMap.Field.WE == 1) && (((BusMap.Field.DstFabricID >> 7) & 1) == Socket) && (((BusMap.Field.DstFabricID >> 5) & 3) == Die)) {
      BusLimit = (UINTN) BusMap.Field.BusNumLimit;
      break;
    }
  }

  return BusLimit;
}

BOOLEAN
FabricTopologyGetSystemComponentRootBridgeLocation (
  IN       COMPONENT_TYPE Component,
  IN OUT   ROOT_BRIDGE_LOCATION *Location
  )
{
  Location->Socket = 0;
  Location->Die = 0;
  Location->Index = 0;

  return TRUE;
}

BOOLEAN
FabricTopologyHasFch (
  IN       UINTN Socket,
  IN       UINTN Die,
  IN       UINTN Index
  )
{
  return TRUE;
}

BOOLEAN
FabricTopologyHasSmu (
  IN       UINTN Socket,
  IN       UINTN Die,
  IN       UINTN Index
  )
{
  return TRUE;
}

UINTN
FabricTopologyGetPhysRootBridgeNumber (
  IN       UINTN Socket,
  IN       UINTN Die,
  IN       UINTN Index
  )
{
  return Index;
}


