/* $NoKeywords:$ */
/**
 * @file
 *
 * Family specific PCIe definitions
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 309090 $   @e \$Date: 2014-12-10 02:28:05 +0800 (Wed, 10 Dec 2014) $
 *
 */
/*
*****************************************************************************
*
* Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
*
*******************************************************************************
*
*/
#ifndef _PCIECOMPLEXDATACZ_H_
#define _PCIECOMPLEXDATACZ_H_

//#define MAX_NUM_PHYs            2
#define MAX_NUM_LANE_PER_PHY    8

#define NUMBER_OF_GFX_PORTS       5
#define NUMBER_OF_GPP_PORTS       5
#define NUMBER_OF_DDI_DDIS        3
#define NUMBER_OF_PORTS          (NUMBER_OF_GFX_PORTS + NUMBER_OF_GPP_PORTS)

#define NUMBER_OF_WRAPPERS        2

#define NON_INITIALIZED_PCI_ADDRESS  0

#define GFX_WRAP_ID               1
#define GFX_START_PHY_LANE        8
#define GFX_END_PHY_LANE          15
#define GFX_CORE_ID               1
#define GFX_NUMBER_OF_PIFs        1

#define GPP_WRAP_ID               0
#define GPP_START_PHY_LANE        0
#define GPP_END_PHY_LANE          7
#define GPP_CORE_ID               0
#define GPP_NUMBER_OF_PIFs        1

#define DDI_WRAP_ID               3
#define DDI_START_PHY_LANE        16
#define DDI_END_PHY_LANE          27
//#define DDI_NUMBER_OF_PIFs        1

// PBR0
#define PBR0                      0
#define PBR0_NATIVE_PCI_DEV       2
#define PBR0_NATIVE_PCI_FUN       1
#define PBR0_CORE_ID              GPP_CORE_ID
#define PBR0_PORT_ID              0
#define PBR0_PCI_ADDRESS          NON_INITIALIZED_PCI_ADDRESS
#define PBR0_UNIT_ID              0x4
#define PBR0_NUM_UNIT_IDs         0x1

// PBR1
#define PBR1                      1
#define PBR1_NATIVE_PCI_DEV       2
#define PBR1_NATIVE_PCI_FUN       2
#define PBR1_CORE_ID              GPP_CORE_ID
#define PBR1_PORT_ID              1
#define PBR1_PCI_ADDRESS          NON_INITIALIZED_PCI_ADDRESS
#define PBR1_UNIT_ID              0x5
#define PBR1_NUM_UNIT_IDs         0x1

// PBR2
#define PBR2                      2
#define PBR2_NATIVE_PCI_DEV       2
#define PBR2_NATIVE_PCI_FUN       3
#define PBR2_CORE_ID              GPP_CORE_ID
#define PBR2_PORT_ID              2
#define PBR2_PCI_ADDRESS          NON_INITIALIZED_PCI_ADDRESS
#define PBR2_UNIT_ID              0x6
#define PBR2_NUM_UNIT_IDs         0x1

// PBR3
#define PBR3                      3
#define PBR3_NATIVE_PCI_DEV       2
#define PBR3_NATIVE_PCI_FUN       4
#define PBR3_CORE_ID              GPP_CORE_ID
#define PBR3_PORT_ID              3
#define PBR3_PCI_ADDRESS          NON_INITIALIZED_PCI_ADDRESS
#define PBR3_UNIT_ID              0x7
#define PBR3_NUM_UNIT_IDs         0x1

// PBR4
#define PBR4                      4
#define PBR4_NATIVE_PCI_DEV       2
#define PBR4_NATIVE_PCI_FUN       5
#define PBR4_CORE_ID              GPP_CORE_ID
#define PBR4_PORT_ID              4
#define PBR4_PCI_ADDRESS          NON_INITIALIZED_PCI_ADDRESS
#define PBR4_UNIT_ID              0x8
#define PBR4_NUM_UNIT_IDs         0x1

// PBR5
#define PBR5                      5
#define PBR5_NATIVE_PCI_DEV       3
#define PBR5_NATIVE_PCI_FUN       1
#define PBR5_CORE_ID              GFX_CORE_ID
#define PBR5_PORT_ID              0
#define PBR5_PCI_ADDRESS          NON_INITIALIZED_PCI_ADDRESS
#define PBR5_UNIT_ID              0x9
#define PBR5_NUM_UNIT_IDs         0x1

// PBR6
#define PBR6                      6
#define PBR6_NATIVE_PCI_DEV       3
#define PBR6_NATIVE_PCI_FUN       2
#define PBR6_CORE_ID              GFX_CORE_ID
#define PBR6_PORT_ID              1
#define PBR6_PCI_ADDRESS          NON_INITIALIZED_PCI_ADDRESS
#define PBR6_UNIT_ID              0xA
#define PBR6_NUM_UNIT_IDs         0x1

// PBR7
#define PBR7                      7
#define PBR7_NATIVE_PCI_DEV       3
#define PBR7_NATIVE_PCI_FUN       3
#define PBR7_CORE_ID              GFX_CORE_ID
#define PBR7_PORT_ID              2
#define PBR7_PCI_ADDRESS          NON_INITIALIZED_PCI_ADDRESS
#define PBR7_UNIT_ID              0xB
#define PBR7_NUM_UNIT_IDs         0x1

// PBR8
#define PBR8                      8
#define PBR8_NATIVE_PCI_DEV       3
#define PBR8_NATIVE_PCI_FUN       4
#define PBR8_CORE_ID              GFX_CORE_ID
#define PBR8_PORT_ID              3
#define PBR8_PCI_ADDRESS          NON_INITIALIZED_PCI_ADDRESS
#define PBR8_UNIT_ID              0xC
#define PBR8_NUM_UNIT_IDs         0x1

// PBR9
#define PBR9                      9
#define PBR9_NATIVE_PCI_DEV       3
#define PBR9_NATIVE_PCI_FUN       5
#define PBR9_CORE_ID              GFX_CORE_ID
#define PBR9_PORT_ID              4
#define PBR9_PCI_ADDRESS          NON_INITIALIZED_PCI_ADDRESS
#define PBR9_UNIT_ID              0xD
#define PBR9_NUM_UNIT_IDs         0x1


#define MaxDevNum                 3
#define MaxDevFunc                5

///@todo CZ STRAP_BIF_LINK_CONFIG has not been confirmed
#define GFX_CORE_x8               ((0ull << 8) | (8ull << 0))
//#define GFX_CORE_x4x4             ((4ull << 8) | (4ull << 0))
//#define GFX_CORE_x4x2x2           ((2ull << 16) | (2ull << 8) | (4ull << 0))
//#define GFX_CORE_x4x2x1x1         ((1ull << 24) | (1ull << 16) | (2ull << 8) | (4ull << 0))
//#define GFX_CORE_x4x1x1x1x1       ((1ull << 32) | (1ull << 24) | (1ull << 16) | (1ull << 8) | (4ull << 0))

#define GPP_CORE_x4x4             ((4ull << 8) | (4ull << 0))
#define GPP_CORE_x4x2x2           ((2ull << 16) | (2ull << 8) | (4ull << 0))
#define GPP_CORE_x4x2x1x1         ((1ull << 24) | (1ull << 16) | (2ull << 8) | (4ull << 0))
#define GPP_CORE_x4x1x1x1x1       ((1ull << 32) | (1ull << 24) | (1ull << 16) | (1ull << 8) | (4ull << 0))

///Family specific silicon configuration
typedef struct {
  UINT8     PortDevMap [10];                      ///< Device number that has beed allocated already
} CZ_PCIe_SILICON_CONFIG;



/// Complex Configuration for silicon module
typedef struct {
  PCIe_SILICON_CONFIG     Silicon;                ///< Silicon

  PCIe_WRAPPER_CONFIG     GfxWrapper;             ///< GFX Wrapper
  PCIe_WRAPPER_CONFIG     GppWrapper;             ///< GPP Wrapper
  PCIe_WRAPPER_CONFIG     DdiWrapper;             ///< DDI   Wrapper
  // GFX
  ///@todo CZ STRAP_BIF_LINK_CONFIG has not been confirmed, so, GFX either could be configured 1x8 or 2x4.
  PCIe_ENGINE_CONFIG      PortPBR9;               ///< Port PBR9
  PCIe_ENGINE_CONFIG      PortPBR8;               ///< Port PBR8
  PCIe_ENGINE_CONFIG      PortPBR7;               ///< Port PBR7
  PCIe_ENGINE_CONFIG      PortPBR6;               ///< Port PBR6
  PCIe_ENGINE_CONFIG      PortPBR5;               ///< Port PBR5
  // GPP
  PCIe_ENGINE_CONFIG      PortPBR4;               ///< Port PBR4
  PCIe_ENGINE_CONFIG      PortPBR3;               ///< Port PBR3
  PCIe_ENGINE_CONFIG      PortPBR2;               ///< Port PBR2
  PCIe_ENGINE_CONFIG      PortPBR1;               ///< Port PBR1
  PCIe_ENGINE_CONFIG      PortPBR0;               ///< Port PBR0
  // DDI
  ///@todo CZ may have 4 Ddi
  PCIe_ENGINE_CONFIG      Ddi0;                   ///< Ddi0
  PCIe_ENGINE_CONFIG      Ddi1;                   ///< Ddi1
  PCIe_ENGINE_CONFIG      Ddi2;                   ///< Ddi2
  CZ_PCIe_SILICON_CONFIG  FmSilicon;              ///< Fm silicon config
} CZ_COMPLEX_CONFIG;


VOID
PcieSetPortPciAddressMapCZ (
  IN      PCIe_SILICON_CONFIG     *Silicon
  );
#endif

