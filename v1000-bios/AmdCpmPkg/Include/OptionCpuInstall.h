/*****************************************************************************
 *
 * Copyright (C) 2012-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 ******************************************************************************
 */

#ifndef _OPTION_CPU_INSTALL_H_
#define _OPTION_CPU_INSTALL_H_

#ifdef AMD_CPM_CPU_FAMILY_0x14_ON_SUPPORT
  #ifndef AMD_CPM_PCIE_GPP0_NAME_ON
    #define AMD_CPM_PCIE_GPP0_NAME_ON               AMD_CPM_PCIE_GPP0_NAME_1
  #endif
  #ifndef AMD_CPM_PCIE_GPP1_NAME_ON
    #define AMD_CPM_PCIE_GPP1_NAME_ON               AMD_CPM_PCIE_GPP1_NAME_1
  #endif
  #ifndef AMD_CPM_PCIE_GPP2_NAME_ON
    #define AMD_CPM_PCIE_GPP2_NAME_ON               AMD_CPM_PCIE_GPP2_NAME_1
  #endif
  #ifndef AMD_CPM_PCIE_GPP3_NAME_ON
    #define AMD_CPM_PCIE_GPP3_NAME_ON               AMD_CPM_PCIE_GPP3_NAME_1
  #endif

  CPU_REVISION_ITEM CpuRevisionTableON[] = {
    { 0x0FFF0F00, 0x00510F00, CPM_CPU_REVISION_ID_ON, CPM_PCIE_REVISION_ID_ON},
    {0}
  };

  PCIE_BRIDGE_NAME PcieBridgeNameON[] = {
    {CPM_PCIE_REVISION_ID_ON, 4, 0, 0x20, AMD_CPM_PCIE_GPP0_NAME_ON},
    {CPM_PCIE_REVISION_ID_ON, 5, 0, 0x21, AMD_CPM_PCIE_GPP1_NAME_ON},
    {CPM_PCIE_REVISION_ID_ON, 6, 0, 0x22, AMD_CPM_PCIE_GPP2_NAME_ON},
    {CPM_PCIE_REVISION_ID_ON, 7, 0, 0x23, AMD_CPM_PCIE_GPP3_NAME_ON},
    {0}
  };

  #define OPTION_CPM_CPU_REVISION_ID_ON           CpuRevisionTableON,
  #define OPTION_CPM_PCIE_BRIDGE_NAME_ON          PcieBridgeNameON,
#else
  #define OPTION_CPM_CPU_REVISION_ID_ON
  #define OPTION_CPM_PCIE_BRIDGE_NAME_ON
#endif

#ifdef AMD_CPM_CPU_FAMILY_0x15_TN_SUPPORT
  #ifndef AMD_CPM_PCIE_GFX0_NAME_TN
    #define AMD_CPM_PCIE_GFX0_NAME_TN               AMD_CPM_PCIE_GFX0_NAME_1
  #endif
  #ifndef AMD_CPM_PCIE_GFX1_NAME_TN
    #define AMD_CPM_PCIE_GFX1_NAME_TN               AMD_CPM_PCIE_GFX1_NAME_1
  #endif
  #ifndef AMD_CPM_PCIE_GPP0_NAME_TN
    #define AMD_CPM_PCIE_GPP0_NAME_TN               AMD_CPM_PCIE_GPP0_NAME_1
  #endif
  #ifndef AMD_CPM_PCIE_GPP1_NAME_TN
    #define AMD_CPM_PCIE_GPP1_NAME_TN               AMD_CPM_PCIE_GPP1_NAME_1
  #endif
  #ifndef AMD_CPM_PCIE_GPP2_NAME_TN
    #define AMD_CPM_PCIE_GPP2_NAME_TN               AMD_CPM_PCIE_GPP2_NAME_1
  #endif
  #ifndef AMD_CPM_PCIE_GPP3_NAME_TN
    #define AMD_CPM_PCIE_GPP3_NAME_TN               AMD_CPM_PCIE_GPP3_NAME_1
  #endif

  CPU_REVISION_ITEM CpuRevisionTableTN[] = {
    { 0x0FFF0F00, 0x00610F00, CPM_CPU_REVISION_ID_TN, CPM_PCIE_REVISION_ID_TN},
    { 0}
  };

  PCIE_BRIDGE_NAME PcieBridgeNameTN[] = {
    {CPM_PCIE_REVISION_ID_TN, 2, 0, 0x10, AMD_CPM_PCIE_GFX0_NAME_TN},
    {CPM_PCIE_REVISION_ID_TN, 3, 0, 0x11, AMD_CPM_PCIE_GFX1_NAME_TN},
    {CPM_PCIE_REVISION_ID_TN, 4, 0, 0x20, AMD_CPM_PCIE_GPP0_NAME_TN},
    {CPM_PCIE_REVISION_ID_TN, 5, 0, 0x21, AMD_CPM_PCIE_GPP1_NAME_TN},
    {CPM_PCIE_REVISION_ID_TN, 6, 0, 0x22, AMD_CPM_PCIE_GPP2_NAME_TN},
    {CPM_PCIE_REVISION_ID_TN, 7, 0, 0x23, AMD_CPM_PCIE_GPP3_NAME_TN},
    {0}
  };

  #define OPTION_CPM_CPU_REVISION_ID_TN           CpuRevisionTableTN,
  #define OPTION_CPM_PCIE_BRIDGE_NAME_TN          PcieBridgeNameTN,
#else
  #define OPTION_CPM_CPU_REVISION_ID_TN
  #define OPTION_CPM_PCIE_BRIDGE_NAME_TN
#endif

#ifdef AMD_CPM_CPU_FAMILY_0x15_KV_SUPPORT
  #ifndef AMD_CPM_PCIE_GFX0_NAME_KV
    #define AMD_CPM_PCIE_GFX0_NAME_KV               AMD_CPM_PCIE_GFX0_NAME_2
  #endif
  #ifndef AMD_CPM_PCIE_GFX1_NAME_KV
    #define AMD_CPM_PCIE_GFX1_NAME_KV               AMD_CPM_PCIE_GFX1_NAME_2
  #endif
  #ifndef AMD_CPM_PCIE_GPP0_NAME_KV
    #define AMD_CPM_PCIE_GPP0_NAME_KV               AMD_CPM_PCIE_GPP0_NAME_2
  #endif
  #ifndef AMD_CPM_PCIE_GPP1_NAME_KV
    #define AMD_CPM_PCIE_GPP1_NAME_KV               AMD_CPM_PCIE_GPP1_NAME_2
  #endif
  #ifndef AMD_CPM_PCIE_GPP2_NAME_KV
    #define AMD_CPM_PCIE_GPP2_NAME_KV               AMD_CPM_PCIE_GPP2_NAME_2
  #endif
  #ifndef AMD_CPM_PCIE_GPP3_NAME_KV
    #define AMD_CPM_PCIE_GPP3_NAME_KV               AMD_CPM_PCIE_GPP3_NAME_2
  #endif
  #ifndef AMD_CPM_PCIE_GPP4_NAME_KV
    #define AMD_CPM_PCIE_GPP4_NAME_KV               AMD_CPM_PCIE_GPP4_NAME_2
  #endif

  CPU_REVISION_ITEM CpuRevisionTableKV[] = {
    { 0x0FFF0F00, 0x00630F00, CPM_CPU_REVISION_ID_KV, CPM_PCIE_REVISION_ID_KV},
    { 0}
  };

  PCIE_BRIDGE_NAME PcieBridgeNameKV[] = {
    {CPM_PCIE_REVISION_ID_KV, 2, 1, 0x10, AMD_CPM_PCIE_GFX0_NAME_KV},
    {CPM_PCIE_REVISION_ID_KV, 2, 2, 0x11, AMD_CPM_PCIE_GFX1_NAME_KV},
    {CPM_PCIE_REVISION_ID_KV, 3, 1, 0x20, AMD_CPM_PCIE_GPP0_NAME_KV},
    {CPM_PCIE_REVISION_ID_KV, 3, 2, 0x21, AMD_CPM_PCIE_GPP1_NAME_KV},
    {CPM_PCIE_REVISION_ID_KV, 3, 3, 0x22, AMD_CPM_PCIE_GPP2_NAME_KV},
    {CPM_PCIE_REVISION_ID_KV, 3, 4, 0x23, AMD_CPM_PCIE_GPP3_NAME_KV},
    {CPM_PCIE_REVISION_ID_KV, 3, 5, 0x24, AMD_CPM_PCIE_GPP4_NAME_KV},
    {0}
  };

  #define OPTION_CPM_CPU_REVISION_ID_KV           CpuRevisionTableKV,
  #define OPTION_CPM_PCIE_BRIDGE_NAME_KV          PcieBridgeNameKV,
#else
  #define OPTION_CPM_CPU_REVISION_ID_KV
  #define OPTION_CPM_PCIE_BRIDGE_NAME_KV
#endif

#ifdef AMD_CPM_CPU_FAMILY_0x15_CZ_SUPPORT
  #ifndef AMD_CPM_PCIE_GFX0_NAME_CZ
    #define AMD_CPM_PCIE_GFX0_NAME_CZ               AMD_CPM_PCIE_GFX0_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GFX1_NAME_CZ
    #define AMD_CPM_PCIE_GFX1_NAME_CZ               AMD_CPM_PCIE_GFX1_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GFX2_NAME_CZ
    #define AMD_CPM_PCIE_GFX2_NAME_CZ               AMD_CPM_PCIE_GFX2_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GFX3_NAME_CZ
    #define AMD_CPM_PCIE_GFX3_NAME_CZ               AMD_CPM_PCIE_GFX3_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GFX4_NAME_CZ
    #define AMD_CPM_PCIE_GFX4_NAME_CZ               AMD_CPM_PCIE_GFX4_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GPP0_NAME_CZ
    #define AMD_CPM_PCIE_GPP0_NAME_CZ               AMD_CPM_PCIE_GPP0_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GPP1_NAME_CZ
    #define AMD_CPM_PCIE_GPP1_NAME_CZ               AMD_CPM_PCIE_GPP1_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GPP2_NAME_CZ
    #define AMD_CPM_PCIE_GPP2_NAME_CZ               AMD_CPM_PCIE_GPP2_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GPP3_NAME_CZ
    #define AMD_CPM_PCIE_GPP3_NAME_CZ               AMD_CPM_PCIE_GPP3_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GPP4_NAME_CZ
    #define AMD_CPM_PCIE_GPP4_NAME_CZ               AMD_CPM_PCIE_GPP4_NAME_4
  #endif

  CPU_REVISION_ITEM CpuRevisionTableCZ[] = {
    { 0x0FFF0F00, 0x00660F00, CPM_CPU_REVISION_ID_CZ, CPM_PCIE_REVISION_ID_CZ},
    { 0}
  };

  PCIE_BRIDGE_NAME PcieBridgeNameCZ[] = {
    {CPM_PCIE_REVISION_ID_CZ, 3, 1, 0x10, AMD_CPM_PCIE_GFX0_NAME_CZ},
    {CPM_PCIE_REVISION_ID_CZ, 3, 2, 0x11, AMD_CPM_PCIE_GFX1_NAME_CZ},
    {CPM_PCIE_REVISION_ID_CZ, 3, 3, 0x12, AMD_CPM_PCIE_GFX2_NAME_CZ},
    {CPM_PCIE_REVISION_ID_CZ, 3, 4, 0x13, AMD_CPM_PCIE_GFX3_NAME_CZ},
    {CPM_PCIE_REVISION_ID_CZ, 3, 5, 0x14, AMD_CPM_PCIE_GFX4_NAME_CZ},
    {CPM_PCIE_REVISION_ID_CZ, 2, 1, 0x20, AMD_CPM_PCIE_GPP0_NAME_CZ},
    {CPM_PCIE_REVISION_ID_CZ, 2, 2, 0x21, AMD_CPM_PCIE_GPP1_NAME_CZ},
    {CPM_PCIE_REVISION_ID_CZ, 2, 3, 0x22, AMD_CPM_PCIE_GPP2_NAME_CZ},
    {CPM_PCIE_REVISION_ID_CZ, 2, 4, 0x23, AMD_CPM_PCIE_GPP3_NAME_CZ},
    {CPM_PCIE_REVISION_ID_CZ, 2, 5, 0x24, AMD_CPM_PCIE_GPP4_NAME_CZ},
    {0}
  };

  #define OPTION_CPM_CPU_REVISION_ID_CZ           CpuRevisionTableCZ,
  #define OPTION_CPM_PCIE_BRIDGE_NAME_CZ          PcieBridgeNameCZ,
#else
  #define OPTION_CPM_CPU_REVISION_ID_CZ
  #define OPTION_CPM_PCIE_BRIDGE_NAME_CZ
#endif

#ifdef AMD_CPM_CPU_FAMILY_0x16_KB_SUPPORT
  #ifndef AMD_CPM_PCIE_GPP0_NAME_KB
    #define AMD_CPM_PCIE_GPP0_NAME_KB               AMD_CPM_PCIE_GPP0_NAME_3
  #endif
  #ifndef AMD_CPM_PCIE_GPP1_NAME_KB
    #define AMD_CPM_PCIE_GPP1_NAME_KB               AMD_CPM_PCIE_GPP1_NAME_3
  #endif
  #ifndef AMD_CPM_PCIE_GPP2_NAME_KB
    #define AMD_CPM_PCIE_GPP2_NAME_KB               AMD_CPM_PCIE_GPP2_NAME_3
  #endif
  #ifndef AMD_CPM_PCIE_GPP3_NAME_KB
    #define AMD_CPM_PCIE_GPP3_NAME_KB               AMD_CPM_PCIE_GPP3_NAME_3
  #endif
  #ifndef AMD_CPM_PCIE_GFX_NAME_KB
    #define AMD_CPM_PCIE_GFX_NAME_KB                AMD_CPM_PCIE_GFX_NAME_3
  #endif

  CPU_REVISION_ITEM CpuRevisionTableKB[] = {
    { 0x0FFF0F00, 0x00700F00, CPM_CPU_REVISION_ID_KB, CPM_PCIE_REVISION_ID_KB},
    { 0}
  };

  PCIE_BRIDGE_NAME PcieBridgeNameKB[] = {
    {CPM_PCIE_REVISION_ID_KB, 2, 1, 0x10, AMD_CPM_PCIE_GFX_NAME_KB},
    {CPM_PCIE_REVISION_ID_KB, 2, 2, 0x20, AMD_CPM_PCIE_GPP0_NAME_KB},
    {CPM_PCIE_REVISION_ID_KB, 2, 3, 0x21, AMD_CPM_PCIE_GPP1_NAME_KB},
    {CPM_PCIE_REVISION_ID_KB, 2, 4, 0x22, AMD_CPM_PCIE_GPP2_NAME_KB},
    {CPM_PCIE_REVISION_ID_KB, 2, 5, 0x23, AMD_CPM_PCIE_GPP3_NAME_KB},
    {0}
  };

  #define OPTION_CPM_CPU_REVISION_ID_KB           CpuRevisionTableKB,
  #define OPTION_CPM_PCIE_BRIDGE_NAME_KB          PcieBridgeNameKB,
#else
  #define OPTION_CPM_CPU_REVISION_ID_KB
  #define OPTION_CPM_PCIE_BRIDGE_NAME_KB
#endif

#ifdef AMD_CPM_CPU_FAMILY_0x16_ML_SUPPORT
  #ifndef AMD_CPM_PCIE_GPP0_NAME_ML
    #define AMD_CPM_PCIE_GPP0_NAME_ML               AMD_CPM_PCIE_GPP0_NAME_3
  #endif
  #ifndef AMD_CPM_PCIE_GPP1_NAME_ML
    #define AMD_CPM_PCIE_GPP1_NAME_ML               AMD_CPM_PCIE_GPP1_NAME_3
  #endif
  #ifndef AMD_CPM_PCIE_GPP2_NAME_ML
    #define AMD_CPM_PCIE_GPP2_NAME_ML               AMD_CPM_PCIE_GPP2_NAME_3
  #endif
  #ifndef AMD_CPM_PCIE_GPP3_NAME_ML
    #define AMD_CPM_PCIE_GPP3_NAME_ML               AMD_CPM_PCIE_GPP3_NAME_3
  #endif
  #ifndef AMD_CPM_PCIE_GFX_NAME_ML
    #define AMD_CPM_PCIE_GFX_NAME_ML                AMD_CPM_PCIE_GFX_NAME_3
  #endif

  CPU_REVISION_ITEM CpuRevisionTableML[] = {
    { 0x0FFF0F00, 0x00730F00, CPM_CPU_REVISION_ID_ML, CPM_PCIE_REVISION_ID_ML},
    { 0}
  };

  PCIE_BRIDGE_NAME PcieBridgeNameML[] = {
    {CPM_PCIE_REVISION_ID_ML, 2, 1, 0x10, AMD_CPM_PCIE_GFX_NAME_ML},
    {CPM_PCIE_REVISION_ID_ML, 2, 2, 0x20, AMD_CPM_PCIE_GPP0_NAME_ML},
    {CPM_PCIE_REVISION_ID_ML, 2, 3, 0x21, AMD_CPM_PCIE_GPP1_NAME_ML},
    {CPM_PCIE_REVISION_ID_ML, 2, 4, 0x22, AMD_CPM_PCIE_GPP2_NAME_ML},
    {CPM_PCIE_REVISION_ID_ML, 2, 5, 0x23, AMD_CPM_PCIE_GPP3_NAME_ML},
    {0}
  };

  #define OPTION_CPM_CPU_REVISION_ID_ML           CpuRevisionTableML,
  #define OPTION_CPM_PCIE_BRIDGE_NAME_ML          PcieBridgeNameML,
#else
  #define OPTION_CPM_CPU_REVISION_ID_ML
  #define OPTION_CPM_PCIE_BRIDGE_NAME_ML
#endif

#ifdef AMD_CPM_CPU_FAMILY_0x16_NL_SUPPORT
  #ifndef AMD_CPM_PCIE_GPP0_NAME_NL
    #define AMD_CPM_PCIE_GPP0_NAME_NL               AMD_CPM_PCIE_GPP0_NAME_3
  #endif
  #ifndef AMD_CPM_PCIE_GPP1_NAME_NL
    #define AMD_CPM_PCIE_GPP1_NAME_NL               AMD_CPM_PCIE_GPP1_NAME_3
  #endif
  #ifndef AMD_CPM_PCIE_GFX_NAME_NL
    #define AMD_CPM_PCIE_GFX_NAME_NL                AMD_CPM_PCIE_GFX_NAME_3
  #endif

  CPU_REVISION_ITEM CpuRevisionTableNL[] = {
    { 0x0FFF0F00, 0x00760F00, CPM_CPU_REVISION_ID_NL, CPM_PCIE_REVISION_ID_NL},
    { 0}
  };

  PCIE_BRIDGE_NAME PcieBridgeNameNL[] = {
    {CPM_PCIE_REVISION_ID_NL, 2, 1, 0x10, AMD_CPM_PCIE_GFX_NAME_NL},
    {CPM_PCIE_REVISION_ID_NL, 2, 2, 0x20, AMD_CPM_PCIE_GPP0_NAME_NL},
    {CPM_PCIE_REVISION_ID_NL, 2, 3, 0x21, AMD_CPM_PCIE_GPP1_NAME_NL},
    {0}
  };

  #define OPTION_CPM_CPU_REVISION_ID_NL           CpuRevisionTableNL,
  #define OPTION_CPM_PCIE_BRIDGE_NAME_NL          PcieBridgeNameNL,
#else
  #define OPTION_CPM_CPU_REVISION_ID_NL
  #define OPTION_CPM_PCIE_BRIDGE_NAME_NL
#endif

#ifdef AMD_CPM_CPU_FAMILY_0x21_AM_SUPPORT
  #ifndef AMD_CPM_PCIE_GPP0_NAME_AM
    #define AMD_CPM_PCIE_GPP0_NAME_AM               AMD_CPM_PCIE_GPP0_NAME_3
  #endif
  #ifndef AMD_CPM_PCIE_GPP1_NAME_AM
    #define AMD_CPM_PCIE_GPP1_NAME_AM               AMD_CPM_PCIE_GPP1_NAME_3
  #endif
  #ifndef AMD_CPM_PCIE_GFX_NAME_AM
    #define AMD_CPM_PCIE_GFX_NAME_AM                AMD_CPM_PCIE_GFX_NAME_3
  #endif

  CPU_REVISION_ITEM CpuRevisionTableAM[] = {
    { 0x0FFF0F00, 0x01200F00, CPM_CPU_REVISION_ID_AM, CPM_PCIE_REVISION_ID_AM},
    { 0}
  };

  PCIE_BRIDGE_NAME PcieBridgeNameAM[] = {
    {CPM_PCIE_REVISION_ID_AM, 2, 1, 0x10, AMD_CPM_PCIE_GFX_NAME_AM},
    {CPM_PCIE_REVISION_ID_AM, 2, 2, 0x20, AMD_CPM_PCIE_GPP0_NAME_AM},
    {CPM_PCIE_REVISION_ID_AM, 2, 3, 0x21, AMD_CPM_PCIE_GPP1_NAME_AM},
    {0}
  };

  #define OPTION_CPM_CPU_REVISION_ID_AM           CpuRevisionTableAM,
  #define OPTION_CPM_PCIE_BRIDGE_NAME_AM          PcieBridgeNameAM,
#else
  #define OPTION_CPM_CPU_REVISION_ID_AM
  #define OPTION_CPM_PCIE_BRIDGE_NAME_AM
#endif

#ifdef AMD_CPM_CPU_FAMILY_0x15_ST_SUPPORT
  #ifndef AMD_CPM_PCIE_GFX0_NAME_ST
    #define AMD_CPM_PCIE_GFX0_NAME_ST               AMD_CPM_PCIE_GFX0_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GFX1_NAME_ST
    #define AMD_CPM_PCIE_GFX1_NAME_ST               AMD_CPM_PCIE_GFX1_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GFX2_NAME_ST
    #define AMD_CPM_PCIE_GFX2_NAME_ST               AMD_CPM_PCIE_GFX2_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GFX3_NAME_ST
    #define AMD_CPM_PCIE_GFX3_NAME_ST               AMD_CPM_PCIE_GFX3_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GFX4_NAME_ST
    #define AMD_CPM_PCIE_GFX4_NAME_ST               AMD_CPM_PCIE_GFX4_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GPP0_NAME_ST
    #define AMD_CPM_PCIE_GPP0_NAME_ST               AMD_CPM_PCIE_GPP0_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GPP1_NAME_ST
    #define AMD_CPM_PCIE_GPP1_NAME_ST               AMD_CPM_PCIE_GPP1_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GPP2_NAME_ST
    #define AMD_CPM_PCIE_GPP2_NAME_ST               AMD_CPM_PCIE_GPP2_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GPP3_NAME_ST
    #define AMD_CPM_PCIE_GPP3_NAME_ST               AMD_CPM_PCIE_GPP3_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GPP4_NAME_ST
    #define AMD_CPM_PCIE_GPP4_NAME_ST               AMD_CPM_PCIE_GPP4_NAME_4
  #endif

  CPU_REVISION_ITEM CpuRevisionTableST[] = {
    { 0x0FFF0F00, 0x00670F00, CPM_CPU_REVISION_ID_ST, CPM_PCIE_REVISION_ID_ST},
    { 0}
  };

  PCIE_BRIDGE_NAME PcieBridgeNameST[] = {
    {CPM_PCIE_REVISION_ID_ST, 3, 1, 0x10, AMD_CPM_PCIE_GFX0_NAME_ST},
    {CPM_PCIE_REVISION_ID_ST, 3, 2, 0x11, AMD_CPM_PCIE_GFX1_NAME_ST},
    {CPM_PCIE_REVISION_ID_ST, 3, 3, 0x12, AMD_CPM_PCIE_GFX2_NAME_ST},
    {CPM_PCIE_REVISION_ID_ST, 3, 4, 0x13, AMD_CPM_PCIE_GFX3_NAME_ST},
    {CPM_PCIE_REVISION_ID_ST, 3, 5, 0x14, AMD_CPM_PCIE_GFX4_NAME_ST},
    {CPM_PCIE_REVISION_ID_ST, 2, 1, 0x20, AMD_CPM_PCIE_GPP0_NAME_ST},
    {CPM_PCIE_REVISION_ID_ST, 2, 2, 0x21, AMD_CPM_PCIE_GPP1_NAME_ST},
    {CPM_PCIE_REVISION_ID_ST, 2, 3, 0x22, AMD_CPM_PCIE_GPP2_NAME_ST},
    {CPM_PCIE_REVISION_ID_ST, 2, 4, 0x23, AMD_CPM_PCIE_GPP3_NAME_ST},
    {CPM_PCIE_REVISION_ID_ST, 2, 5, 0x24, AMD_CPM_PCIE_GPP4_NAME_ST},
    {0}
  };

  #define OPTION_CPM_CPU_REVISION_ID_ST           CpuRevisionTableST,
  #define OPTION_CPM_PCIE_BRIDGE_NAME_ST          PcieBridgeNameST,
#else
  #define OPTION_CPM_CPU_REVISION_ID_ST
  #define OPTION_CPM_PCIE_BRIDGE_NAME_ST
#endif

#ifdef AMD_CPM_CPU_FAMILY_0x15_BR_SUPPORT
  #ifndef AMD_CPM_PCIE_GFX0_NAME_BR
    #define AMD_CPM_PCIE_GFX0_NAME_BR               AMD_CPM_PCIE_GFX0_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GFX1_NAME_BR
    #define AMD_CPM_PCIE_GFX1_NAME_BR               AMD_CPM_PCIE_GFX1_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GFX2_NAME_BR
    #define AMD_CPM_PCIE_GFX2_NAME_BR               AMD_CPM_PCIE_GFX2_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GFX3_NAME_BR
    #define AMD_CPM_PCIE_GFX3_NAME_BR               AMD_CPM_PCIE_GFX3_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GFX4_NAME_BR
    #define AMD_CPM_PCIE_GFX4_NAME_BR               AMD_CPM_PCIE_GFX4_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GPP0_NAME_BR
    #define AMD_CPM_PCIE_GPP0_NAME_BR               AMD_CPM_PCIE_GPP0_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GPP1_NAME_BR
    #define AMD_CPM_PCIE_GPP1_NAME_BR               AMD_CPM_PCIE_GPP1_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GPP2_NAME_BR
    #define AMD_CPM_PCIE_GPP2_NAME_BR               AMD_CPM_PCIE_GPP2_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GPP3_NAME_BR
    #define AMD_CPM_PCIE_GPP3_NAME_BR               AMD_CPM_PCIE_GPP3_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GPP4_NAME_BR
    #define AMD_CPM_PCIE_GPP4_NAME_BR               AMD_CPM_PCIE_GPP4_NAME_4
  #endif

  CPU_REVISION_ITEM CpuRevisionTableBR[] = {
    { 0x0FFF0FF0, 0x00660F50, CPM_CPU_REVISION_ID_BR, CPM_PCIE_REVISION_ID_BR},
    { 0}
  };

  PCIE_BRIDGE_NAME PcieBridgeNameBR[] = {
    {CPM_PCIE_REVISION_ID_BR, 3, 1, 0x10, AMD_CPM_PCIE_GFX0_NAME_BR},
    {CPM_PCIE_REVISION_ID_BR, 3, 2, 0x11, AMD_CPM_PCIE_GFX1_NAME_BR},
    {CPM_PCIE_REVISION_ID_BR, 3, 3, 0x12, AMD_CPM_PCIE_GFX2_NAME_BR},
    {CPM_PCIE_REVISION_ID_BR, 3, 4, 0x13, AMD_CPM_PCIE_GFX3_NAME_BR},
    {CPM_PCIE_REVISION_ID_BR, 3, 5, 0x14, AMD_CPM_PCIE_GFX4_NAME_BR},
    {CPM_PCIE_REVISION_ID_BR, 2, 1, 0x20, AMD_CPM_PCIE_GPP0_NAME_BR},
    {CPM_PCIE_REVISION_ID_BR, 2, 2, 0x21, AMD_CPM_PCIE_GPP1_NAME_BR},
    {CPM_PCIE_REVISION_ID_BR, 2, 3, 0x22, AMD_CPM_PCIE_GPP2_NAME_BR},
    {CPM_PCIE_REVISION_ID_BR, 2, 4, 0x23, AMD_CPM_PCIE_GPP3_NAME_BR},
    {CPM_PCIE_REVISION_ID_BR, 2, 5, 0x24, AMD_CPM_PCIE_GPP4_NAME_BR},
    {0}
  };

  #define OPTION_CPM_CPU_REVISION_ID_BR           CpuRevisionTableBR,
  #define OPTION_CPM_PCIE_BRIDGE_NAME_BR          PcieBridgeNameBR,
#else
  #define OPTION_CPM_CPU_REVISION_ID_BR
  #define OPTION_CPM_PCIE_BRIDGE_NAME_BR
#endif

#ifdef AMD_CPM_CPU_FAMILY_0x17_ZP_SUPPORT
  #ifndef AMD_CPM_PCIE_GPP0_NAME_ZP
    #define AMD_CPM_PCIE_GPP0_NAME_ZP               AMD_CPM_PCIE_GPP0_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GPP1_NAME_ZP
    #define AMD_CPM_PCIE_GPP1_NAME_ZP               AMD_CPM_PCIE_GPP1_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GPP2_NAME_ZP
    #define AMD_CPM_PCIE_GPP2_NAME_ZP               AMD_CPM_PCIE_GPP2_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GPP3_NAME_ZP
    #define AMD_CPM_PCIE_GPP3_NAME_ZP               AMD_CPM_PCIE_GPP3_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GPP4_NAME_ZP
    #define AMD_CPM_PCIE_GPP4_NAME_ZP               AMD_CPM_PCIE_GPP4_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GPP5_NAME_ZP
    #define AMD_CPM_PCIE_GPP5_NAME_ZP               AMD_CPM_PCIE_GPP5_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GPP6_NAME_ZP
    #define AMD_CPM_PCIE_GPP6_NAME_ZP               AMD_CPM_PCIE_GPP6_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GPP7_NAME_ZP
    #define AMD_CPM_PCIE_GPP7_NAME_ZP               AMD_CPM_PCIE_GPP7_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GPP8_NAME_ZP
    #define AMD_CPM_PCIE_GPP8_NAME_ZP               AMD_CPM_PCIE_GPP8_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GPP9_NAME_ZP
    #define AMD_CPM_PCIE_GPP9_NAME_ZP               AMD_CPM_PCIE_GPP9_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GPPA_NAME_ZP
    #define AMD_CPM_PCIE_GPPA_NAME_ZP               AMD_CPM_PCIE_GPPA_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GPPB_NAME_ZP
    #define AMD_CPM_PCIE_GPPB_NAME_ZP               AMD_CPM_PCIE_GPPB_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GPPC_NAME_ZP
    #define AMD_CPM_PCIE_GPPC_NAME_ZP               AMD_CPM_PCIE_GPPC_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GPPD_NAME_ZP
    #define AMD_CPM_PCIE_GPPD_NAME_ZP               AMD_CPM_PCIE_GPPD_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GPPE_NAME_ZP
    #define AMD_CPM_PCIE_GPPE_NAME_ZP               AMD_CPM_PCIE_GPPE_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GPPF_NAME_ZP
    #define AMD_CPM_PCIE_GPPF_NAME_ZP               AMD_CPM_PCIE_GPPF_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GP17_NAME_ZP
    #define AMD_CPM_PCIE_GP17_NAME_ZP               AMD_CPM_PCIE_GP17_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GP18_NAME_ZP
    #define AMD_CPM_PCIE_GP18_NAME_ZP               AMD_CPM_PCIE_GP18_NAME_4
  #endif

  CPU_REVISION_ITEM CpuRevisionTableZP[] = {
    { 0x0FFF0F00, 0x00800F00, CPM_CPU_REVISION_ID_ZP, CPM_PCIE_REVISION_ID_ZP},
    { 0}
  };

  PCIE_BRIDGE_NAME PcieBridgeNameZP[] = {
    {CPM_PCIE_REVISION_ID_ZP, 1, 1, 0x20, AMD_CPM_PCIE_GPP0_NAME_ZP},
    {CPM_PCIE_REVISION_ID_ZP, 1, 2, 0x21, AMD_CPM_PCIE_GPP1_NAME_ZP},
    {CPM_PCIE_REVISION_ID_ZP, 1, 3, 0x22, AMD_CPM_PCIE_GPP2_NAME_ZP},
    {CPM_PCIE_REVISION_ID_ZP, 1, 4, 0x23, AMD_CPM_PCIE_GPP3_NAME_ZP},
    {CPM_PCIE_REVISION_ID_ZP, 1, 5, 0x24, AMD_CPM_PCIE_GPP4_NAME_ZP},
    {CPM_PCIE_REVISION_ID_ZP, 1, 6, 0x25, AMD_CPM_PCIE_GPP5_NAME_ZP},
    {CPM_PCIE_REVISION_ID_ZP, 1, 7, 0x26, AMD_CPM_PCIE_GPP6_NAME_ZP},
    {CPM_PCIE_REVISION_ID_ZP, 2, 1, 0x27, AMD_CPM_PCIE_GPP7_NAME_ZP},
    {CPM_PCIE_REVISION_ID_ZP, 3, 1, 0x28, AMD_CPM_PCIE_GPP8_NAME_ZP},
    {CPM_PCIE_REVISION_ID_ZP, 3, 2, 0x29, AMD_CPM_PCIE_GPP9_NAME_ZP},
    {CPM_PCIE_REVISION_ID_ZP, 3, 3, 0x2A, AMD_CPM_PCIE_GPPA_NAME_ZP},
    {CPM_PCIE_REVISION_ID_ZP, 3, 4, 0x2B, AMD_CPM_PCIE_GPPB_NAME_ZP},
    {CPM_PCIE_REVISION_ID_ZP, 3, 5, 0x2C, AMD_CPM_PCIE_GPPC_NAME_ZP},
    {CPM_PCIE_REVISION_ID_ZP, 3, 6, 0x2D, AMD_CPM_PCIE_GPPD_NAME_ZP},
    {CPM_PCIE_REVISION_ID_ZP, 3, 7, 0x2E, AMD_CPM_PCIE_GPPE_NAME_ZP},
    {CPM_PCIE_REVISION_ID_ZP, 4, 1, 0x2F, AMD_CPM_PCIE_GPPF_NAME_ZP},
    {CPM_PCIE_REVISION_ID_ZP, 7, 1, 0x30, AMD_CPM_PCIE_GP17_NAME_ZP},
    {CPM_PCIE_REVISION_ID_ZP, 8, 1, 0x31, AMD_CPM_PCIE_GP18_NAME_ZP},
    {0}
  };

  #define OPTION_CPM_CPU_REVISION_ID_ZP           CpuRevisionTableZP,
  #define OPTION_CPM_PCIE_BRIDGE_NAME_ZP          PcieBridgeNameZP,
#else
  #define OPTION_CPM_CPU_REVISION_ID_ZP
  #define OPTION_CPM_PCIE_BRIDGE_NAME_ZP
#endif

#ifdef AMD_CPM_CPU_FAMILY_0x17_RV_SUPPORT
  #ifndef AMD_CPM_PCIE_GPP0_NAME_RV
    #define AMD_CPM_PCIE_GPP0_NAME_RV               AMD_CPM_PCIE_GPP0_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GPP1_NAME_RV
    #define AMD_CPM_PCIE_GPP1_NAME_RV               AMD_CPM_PCIE_GPP1_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GPP2_NAME_RV
    #define AMD_CPM_PCIE_GPP2_NAME_RV               AMD_CPM_PCIE_GPP2_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GPP3_NAME_RV
    #define AMD_CPM_PCIE_GPP3_NAME_RV               AMD_CPM_PCIE_GPP3_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GPP4_NAME_RV
    #define AMD_CPM_PCIE_GPP4_NAME_RV               AMD_CPM_PCIE_GPP4_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GPP5_NAME_RV
    #define AMD_CPM_PCIE_GPP5_NAME_RV               AMD_CPM_PCIE_GPP5_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GPP6_NAME_RV
    #define AMD_CPM_PCIE_GPP6_NAME_RV               AMD_CPM_PCIE_GPP6_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GP17_NAME_RV
    #define AMD_CPM_PCIE_GP17_NAME_RV               AMD_CPM_PCIE_GP17_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GP18_NAME_RV
    #define AMD_CPM_PCIE_GP18_NAME_RV               AMD_CPM_PCIE_GP18_NAME_4
  #endif

  CPU_REVISION_ITEM CpuRevisionTableRV[] = {
    { 0x0FFF0F00, 0x00810F00, CPM_CPU_REVISION_ID_RV, CPM_PCIE_REVISION_ID_RV},
    { 0}
  };

  PCIE_BRIDGE_NAME PcieBridgeNameRV[] = {
    {CPM_PCIE_REVISION_ID_RV, 1, 1, 0x20, AMD_CPM_PCIE_GPP0_NAME_RV},
    {CPM_PCIE_REVISION_ID_RV, 1, 2, 0x21, AMD_CPM_PCIE_GPP1_NAME_RV},
    {CPM_PCIE_REVISION_ID_RV, 1, 3, 0x22, AMD_CPM_PCIE_GPP2_NAME_RV},
    {CPM_PCIE_REVISION_ID_RV, 1, 4, 0x23, AMD_CPM_PCIE_GPP3_NAME_RV},
    {CPM_PCIE_REVISION_ID_RV, 1, 5, 0x24, AMD_CPM_PCIE_GPP4_NAME_RV},
    {CPM_PCIE_REVISION_ID_RV, 1, 6, 0x25, AMD_CPM_PCIE_GPP5_NAME_RV},
    {CPM_PCIE_REVISION_ID_RV, 1, 7, 0x26, AMD_CPM_PCIE_GPP6_NAME_RV},
    {CPM_PCIE_REVISION_ID_RV, 8, 1, 0x30, AMD_CPM_PCIE_GP17_NAME_RV},
    {CPM_PCIE_REVISION_ID_RV, 8, 2, 0x31, AMD_CPM_PCIE_GP18_NAME_RV},
    {0}
  };

  #define OPTION_CPM_CPU_REVISION_ID_RV           CpuRevisionTableRV,
  #define OPTION_CPM_PCIE_BRIDGE_NAME_RV          PcieBridgeNameRV,
#else
  #define OPTION_CPM_CPU_REVISION_ID_RV
  #define OPTION_CPM_PCIE_BRIDGE_NAME_RV
#endif

#ifdef AMD_CPM_CPU_FAMILY_0x17_RV2_SUPPORT
  #ifndef AMD_CPM_PCIE_GPP0_NAME_RV2
    #define AMD_CPM_PCIE_GPP0_NAME_RV2              AMD_CPM_PCIE_GPP0_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GPP1_NAME_RV2
    #define AMD_CPM_PCIE_GPP1_NAME_RV2              AMD_CPM_PCIE_GPP1_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GPP2_NAME_RV2
    #define AMD_CPM_PCIE_GPP2_NAME_RV2              AMD_CPM_PCIE_GPP2_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GPP3_NAME_RV2
    #define AMD_CPM_PCIE_GPP3_NAME_RV2              AMD_CPM_PCIE_GPP3_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GPP4_NAME_RV2
    #define AMD_CPM_PCIE_GPP4_NAME_RV2              AMD_CPM_PCIE_GPP4_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GP17_NAME_RV2
    #define AMD_CPM_PCIE_GP17_NAME_RV2              AMD_CPM_PCIE_GP17_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GP18_NAME_RV2
    #define AMD_CPM_PCIE_GP18_NAME_RV2              AMD_CPM_PCIE_GP18_NAME_4
  #endif

  CPU_REVISION_ITEM CpuRevisionTableRV2[] = {
    { 0x0FFF0F00, 0x00820F00, CPM_CPU_REVISION_ID_RV2, CPM_PCIE_REVISION_ID_RV2},
    { 0}
  };

  PCIE_BRIDGE_NAME PcieBridgeNameRV2[] = {
    {CPM_PCIE_REVISION_ID_RV2, 1, 1, 0x20, AMD_CPM_PCIE_GPP0_NAME_RV2},
    {CPM_PCIE_REVISION_ID_RV2, 1, 2, 0x21, AMD_CPM_PCIE_GPP1_NAME_RV2},
    {CPM_PCIE_REVISION_ID_RV2, 1, 3, 0x22, AMD_CPM_PCIE_GPP2_NAME_RV2},
    {CPM_PCIE_REVISION_ID_RV2, 1, 4, 0x23, AMD_CPM_PCIE_GPP3_NAME_RV2},
    {CPM_PCIE_REVISION_ID_RV2, 1, 5, 0x24, AMD_CPM_PCIE_GPP4_NAME_RV2},
    {CPM_PCIE_REVISION_ID_RV2, 8, 1, 0x30, AMD_CPM_PCIE_GP17_NAME_RV2},
    {CPM_PCIE_REVISION_ID_RV2, 8, 2, 0x31, AMD_CPM_PCIE_GP18_NAME_RV2},
    {0}
  };

  #define OPTION_CPM_CPU_REVISION_ID_RV2          CpuRevisionTableRV2,
  #define OPTION_CPM_PCIE_BRIDGE_NAME_RV2         PcieBridgeNameRV2,
#else
  #define OPTION_CPM_CPU_REVISION_ID_RV2
  #define OPTION_CPM_PCIE_BRIDGE_NAME_RV2
#endif

#ifdef AMD_CPM_CPU_FAMILY_0x17_SSP_SUPPORT
  #ifndef AMD_CPM_PCIE_GPP0_NAME_SSP
    #define AMD_CPM_PCIE_GPP0_NAME_SSP              AMD_CPM_PCIE_GPP0_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GPP1_NAME_SSP
    #define AMD_CPM_PCIE_GPP1_NAME_SSP              AMD_CPM_PCIE_GPP1_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GPP2_NAME_SSP
    #define AMD_CPM_PCIE_GPP2_NAME_SSP              AMD_CPM_PCIE_GPP2_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GPP3_NAME_SSP
    #define AMD_CPM_PCIE_GPP3_NAME_SSP              AMD_CPM_PCIE_GPP3_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GPP4_NAME_SSP
    #define AMD_CPM_PCIE_GPP4_NAME_SSP              AMD_CPM_PCIE_GPP4_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GPP5_NAME_SSP
    #define AMD_CPM_PCIE_GPP5_NAME_SSP              AMD_CPM_PCIE_GPP5_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GPP6_NAME_SSP
    #define AMD_CPM_PCIE_GPP6_NAME_SSP              AMD_CPM_PCIE_GPP6_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GPP7_NAME_SSP
    #define AMD_CPM_PCIE_GPP7_NAME_SSP              AMD_CPM_PCIE_GPP7_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GPP8_NAME_SSP
    #define AMD_CPM_PCIE_GPP8_NAME_SSP              AMD_CPM_PCIE_GPP8_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GPP9_NAME_SSP
    #define AMD_CPM_PCIE_GPP9_NAME_SSP              AMD_CPM_PCIE_GPP9_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GPPA_NAME_SSP
    #define AMD_CPM_PCIE_GPPA_NAME_SSP              AMD_CPM_PCIE_GPPA_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GPPB_NAME_SSP
    #define AMD_CPM_PCIE_GPPB_NAME_SSP              AMD_CPM_PCIE_GPPB_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GPPC_NAME_SSP
    #define AMD_CPM_PCIE_GPPC_NAME_SSP              AMD_CPM_PCIE_GPPC_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GPPD_NAME_SSP
    #define AMD_CPM_PCIE_GPPD_NAME_SSP              AMD_CPM_PCIE_GPPD_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GPPE_NAME_SSP
    #define AMD_CPM_PCIE_GPPE_NAME_SSP              AMD_CPM_PCIE_GPPE_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GPPF_NAME_SSP
    #define AMD_CPM_PCIE_GPPF_NAME_SSP              AMD_CPM_PCIE_GPPF_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GP17_NAME_SSP
    #define AMD_CPM_PCIE_GP17_NAME_SSP              AMD_CPM_PCIE_GP17_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GP18_NAME_SSP
    #define AMD_CPM_PCIE_GP18_NAME_SSP              AMD_CPM_PCIE_GP18_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GP28_NAME_SSP
    #define AMD_CPM_PCIE_GP28_NAME_SSP              AMD_CPM_PCIE_GP28_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GP38_NAME_SSP
    #define AMD_CPM_PCIE_GP38_NAME_SSP              AMD_CPM_PCIE_GP38_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GPPG_NAME_SSP
    #define AMD_CPM_PCIE_GPPG_NAME_SSP              AMD_CPM_PCIE_GPPG_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GPPH_NAME_SSP
    #define AMD_CPM_PCIE_GPPH_NAME_SSP              AMD_CPM_PCIE_GPPH_NAME_4
  #endif

  CPU_REVISION_ITEM CpuRevisionTableSSP[] = {
    { 0x0FFF0F00, 0x00830F00, CPM_CPU_REVISION_ID_SSP, CPM_PCIE_REVISION_ID_SSP},
    { 0}
  };

  PCIE_BRIDGE_NAME PcieBridgeNameSSP[] = {
    {CPM_PCIE_REVISION_ID_SSP, 1, 1, 0x20, AMD_CPM_PCIE_GPP0_NAME_SSP},
    {CPM_PCIE_REVISION_ID_SSP, 1, 2, 0x21, AMD_CPM_PCIE_GPP1_NAME_SSP},
    {CPM_PCIE_REVISION_ID_SSP, 1, 3, 0x22, AMD_CPM_PCIE_GPP2_NAME_SSP},
    {CPM_PCIE_REVISION_ID_SSP, 1, 4, 0x23, AMD_CPM_PCIE_GPP3_NAME_SSP},
    {CPM_PCIE_REVISION_ID_SSP, 1, 5, 0x24, AMD_CPM_PCIE_GPP4_NAME_SSP},
    {CPM_PCIE_REVISION_ID_SSP, 1, 6, 0x25, AMD_CPM_PCIE_GPP5_NAME_SSP},
    {CPM_PCIE_REVISION_ID_SSP, 1, 7, 0x26, AMD_CPM_PCIE_GPP6_NAME_SSP},
    {CPM_PCIE_REVISION_ID_SSP, 2, 1, 0x27, AMD_CPM_PCIE_GPP7_NAME_SSP},
    {CPM_PCIE_REVISION_ID_SSP, 3, 1, 0x28, AMD_CPM_PCIE_GPP8_NAME_SSP},
    {CPM_PCIE_REVISION_ID_SSP, 3, 2, 0x29, AMD_CPM_PCIE_GPP9_NAME_SSP},
    {CPM_PCIE_REVISION_ID_SSP, 3, 3, 0x2A, AMD_CPM_PCIE_GPPA_NAME_SSP},
    {CPM_PCIE_REVISION_ID_SSP, 3, 4, 0x2B, AMD_CPM_PCIE_GPPB_NAME_SSP},
    {CPM_PCIE_REVISION_ID_SSP, 3, 5, 0x2C, AMD_CPM_PCIE_GPPC_NAME_SSP},
    {CPM_PCIE_REVISION_ID_SSP, 3, 6, 0x2D, AMD_CPM_PCIE_GPPD_NAME_SSP},
    {CPM_PCIE_REVISION_ID_SSP, 3, 7, 0x2E, AMD_CPM_PCIE_GPPE_NAME_SSP},
    {CPM_PCIE_REVISION_ID_SSP, 4, 1, 0x2F, AMD_CPM_PCIE_GPPF_NAME_SSP},
    {CPM_PCIE_REVISION_ID_SSP, 7, 1, 0x30, AMD_CPM_PCIE_GP17_NAME_SSP},
    {CPM_PCIE_REVISION_ID_SSP, 8, 1, 0x31, AMD_CPM_PCIE_GP18_NAME_SSP},
    {CPM_PCIE_REVISION_ID_SSP, 8, 2, 0x32, AMD_CPM_PCIE_GP28_NAME_SSP},
    {CPM_PCIE_REVISION_ID_SSP, 8, 3, 0x33, AMD_CPM_PCIE_GP38_NAME_SSP},
    {CPM_PCIE_REVISION_ID_SSP, 5, 1, 0x40, AMD_CPM_PCIE_GPPG_NAME_SSP},
    {CPM_PCIE_REVISION_ID_SSP, 5, 2, 0x41, AMD_CPM_PCIE_GPPH_NAME_SSP},
    {0}
  };

  #define OPTION_CPM_CPU_REVISION_ID_SSP          CpuRevisionTableSSP,
  #define OPTION_CPM_PCIE_BRIDGE_NAME_SSP         PcieBridgeNameSSP,
#else
  #define OPTION_CPM_CPU_REVISION_ID_SSP
  #define OPTION_CPM_PCIE_BRIDGE_NAME_SSP
#endif

#ifdef AMD_CPM_CPU_FAMILY_0x17_MTS_SUPPORT
  #ifndef AMD_CPM_PCIE_GPP0_NAME_MTS
    #define AMD_CPM_PCIE_GPP0_NAME_MTS              AMD_CPM_PCIE_GPP0_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GPP1_NAME_MTS
    #define AMD_CPM_PCIE_GPP1_NAME_MTS              AMD_CPM_PCIE_GPP1_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GPP2_NAME_MTS
    #define AMD_CPM_PCIE_GPP2_NAME_MTS              AMD_CPM_PCIE_GPP2_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GPP3_NAME_MTS
    #define AMD_CPM_PCIE_GPP3_NAME_MTS              AMD_CPM_PCIE_GPP3_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GPP4_NAME_MTS
    #define AMD_CPM_PCIE_GPP4_NAME_MTS              AMD_CPM_PCIE_GPP4_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GPP5_NAME_MTS
    #define AMD_CPM_PCIE_GPP5_NAME_MTS              AMD_CPM_PCIE_GPP5_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GPP6_NAME_MTS
    #define AMD_CPM_PCIE_GPP6_NAME_MTS              AMD_CPM_PCIE_GPP6_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GPP7_NAME_MTS
    #define AMD_CPM_PCIE_GPP7_NAME_MTS              AMD_CPM_PCIE_GPP7_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GPP8_NAME_MTS
    #define AMD_CPM_PCIE_GPP8_NAME_MTS              AMD_CPM_PCIE_GPP8_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GPP9_NAME_MTS
    #define AMD_CPM_PCIE_GPP9_NAME_MTS              AMD_CPM_PCIE_GPP9_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GPPA_NAME_MTS
    #define AMD_CPM_PCIE_GPPA_NAME_MTS              AMD_CPM_PCIE_GPPA_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GPPB_NAME_MTS
    #define AMD_CPM_PCIE_GPPB_NAME_MTS              AMD_CPM_PCIE_GPPB_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GPPC_NAME_MTS
    #define AMD_CPM_PCIE_GPPC_NAME_MTS              AMD_CPM_PCIE_GPPC_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GPPD_NAME_MTS
    #define AMD_CPM_PCIE_GPPD_NAME_MTS              AMD_CPM_PCIE_GPPD_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GPPE_NAME_MTS
    #define AMD_CPM_PCIE_GPPE_NAME_MTS              AMD_CPM_PCIE_GPPE_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GPPF_NAME_MTS
    #define AMD_CPM_PCIE_GPPF_NAME_MTS              AMD_CPM_PCIE_GPPF_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GP17_NAME_MTS
    #define AMD_CPM_PCIE_GP17_NAME_MTS              AMD_CPM_PCIE_GP17_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GP18_NAME_MTS
    #define AMD_CPM_PCIE_GP18_NAME_MTS              AMD_CPM_PCIE_GP18_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GP28_NAME_MTS
    #define AMD_CPM_PCIE_GP28_NAME_MTS              AMD_CPM_PCIE_GP28_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GP38_NAME_MTS
    #define AMD_CPM_PCIE_GP38_NAME_MTS              AMD_CPM_PCIE_GP38_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GPPG_NAME_MTS
    #define AMD_CPM_PCIE_GPPG_NAME_MTS              AMD_CPM_PCIE_GPPG_NAME_4
  #endif
  #ifndef AMD_CPM_PCIE_GPPH_NAME_MTS
    #define AMD_CPM_PCIE_GPPH_NAME_MTS              AMD_CPM_PCIE_GPPH_NAME_4
  #endif

  CPU_REVISION_ITEM CpuRevisionTableMTS[] = {
    { 0x0FFF0F00, 0x00870F00, CPM_CPU_REVISION_ID_MTS, CPM_PCIE_REVISION_ID_MTS},
    { 0}
  };

  PCIE_BRIDGE_NAME PcieBridgeNameMTS[] = {
    {CPM_PCIE_REVISION_ID_MTS, 1, 1, 0x20, AMD_CPM_PCIE_GPP0_NAME_MTS},
    {CPM_PCIE_REVISION_ID_MTS, 1, 2, 0x21, AMD_CPM_PCIE_GPP1_NAME_MTS},
    {CPM_PCIE_REVISION_ID_MTS, 1, 3, 0x22, AMD_CPM_PCIE_GPP2_NAME_MTS},
    {CPM_PCIE_REVISION_ID_MTS, 1, 4, 0x23, AMD_CPM_PCIE_GPP3_NAME_MTS},
    {CPM_PCIE_REVISION_ID_MTS, 1, 5, 0x24, AMD_CPM_PCIE_GPP4_NAME_MTS},
    {CPM_PCIE_REVISION_ID_MTS, 1, 6, 0x25, AMD_CPM_PCIE_GPP5_NAME_MTS},
    {CPM_PCIE_REVISION_ID_MTS, 1, 7, 0x26, AMD_CPM_PCIE_GPP6_NAME_MTS},
    {CPM_PCIE_REVISION_ID_MTS, 2, 1, 0x27, AMD_CPM_PCIE_GPP7_NAME_MTS},
    {CPM_PCIE_REVISION_ID_MTS, 3, 1, 0x28, AMD_CPM_PCIE_GPP8_NAME_MTS},
    {CPM_PCIE_REVISION_ID_MTS, 3, 2, 0x29, AMD_CPM_PCIE_GPP9_NAME_MTS},
    {CPM_PCIE_REVISION_ID_MTS, 3, 3, 0x2A, AMD_CPM_PCIE_GPPA_NAME_MTS},
    {CPM_PCIE_REVISION_ID_MTS, 3, 4, 0x2B, AMD_CPM_PCIE_GPPB_NAME_MTS},
    {CPM_PCIE_REVISION_ID_MTS, 3, 5, 0x2C, AMD_CPM_PCIE_GPPC_NAME_MTS},
    {CPM_PCIE_REVISION_ID_MTS, 3, 6, 0x2D, AMD_CPM_PCIE_GPPD_NAME_MTS},
    {CPM_PCIE_REVISION_ID_MTS, 3, 7, 0x2E, AMD_CPM_PCIE_GPPE_NAME_MTS},
    {CPM_PCIE_REVISION_ID_MTS, 4, 1, 0x2F, AMD_CPM_PCIE_GPPF_NAME_MTS},
    {CPM_PCIE_REVISION_ID_MTS, 7, 1, 0x30, AMD_CPM_PCIE_GP17_NAME_MTS},
    {CPM_PCIE_REVISION_ID_MTS, 8, 1, 0x31, AMD_CPM_PCIE_GP18_NAME_MTS},
    {CPM_PCIE_REVISION_ID_MTS, 8, 2, 0x32, AMD_CPM_PCIE_GP28_NAME_MTS},
    {CPM_PCIE_REVISION_ID_MTS, 8, 3, 0x33, AMD_CPM_PCIE_GP38_NAME_MTS},
    {CPM_PCIE_REVISION_ID_MTS, 5, 1, 0x40, AMD_CPM_PCIE_GPPG_NAME_MTS},
    {CPM_PCIE_REVISION_ID_MTS, 5, 2, 0x41, AMD_CPM_PCIE_GPPH_NAME_MTS},
    {0}
  };

  #define OPTION_CPM_CPU_REVISION_ID_MTS          CpuRevisionTableMTS,
  #define OPTION_CPM_PCIE_BRIDGE_NAME_MTS         PcieBridgeNameMTS,
#else
  #define OPTION_CPM_CPU_REVISION_ID_MTS
  #define OPTION_CPM_PCIE_BRIDGE_NAME_MTS
#endif

#endif