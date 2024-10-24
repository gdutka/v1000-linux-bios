/* $NoKeywords:$ */
/**
 * @file
 *
 * Family specific PCIe configuration data
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 312288 $   @e \$Date: 2015-02-04 00:39:01 -0600 (Wed, 04 Feb 2015) $
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

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include  <GnbDxio.h>
#include  <PcieComplexDataRV.h>
#include  <Filecode.h>

#define FILECODE  NBIO_NBIOBASE_AMDNBIOBASERVPEI_PCIECOMPLEXDATARV_FILECODE

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------*/
//
// Complex configuration
//

RV_COMPLEX_CONFIG ComplexDataRV = {
  //Silicon
  {
    {//Header
      DESCRIPTOR_SILICON | DESCRIPTOR_TERMINATE_LIST | DESCRIPTOR_TERMINATE_GNB | DESCRIPTOR_TERMINATE_TOPOLOGY,
      0,
      0,
      offsetof (RV_COMPLEX_CONFIG, Gpp0Wrapper) - offsetof (RV_COMPLEX_CONFIG, Silicon)
    },
    0,    // NodeId
    0x0,  // SocketId
    0x0,  // ModuleId
    0,    // Address
    {0},    // GmmBase
    GPP0_START_PHY_LANE,                           // StartLane
    GPP0_END_PHY_LANE - GPP0_START_PHY_LANE        // EndLane
  },
  //Gpp0 Wrapper
  {
    {
      DESCRIPTOR_PCIE_WRAPPER | DESCRIPTOR_TERMINATE_LIST | DESCRIPTOR_TERMINATE_GNB | DESCRIPTOR_TERMINATE_TOPOLOGY,
      offsetof (RV_COMPLEX_CONFIG, Gpp0Wrapper) - offsetof (RV_COMPLEX_CONFIG, Silicon),
      0,
      offsetof (RV_COMPLEX_CONFIG, PortPBR7) - offsetof (RV_COMPLEX_CONFIG, Gpp0Wrapper)
    },
    GPP0_WRAP_ID,
    GPP0_NUMBER_OF_PIFs,
    GPP0_START_PHY_LANE,
    GPP0_END_PHY_LANE,
    GPP0_CORE_ID,                                   //StartPcieCoreId     [0:3]
    GPP0_CORE_ID,                                   //EndPcieCoreId       [4:7]
    {                                               //Feature
      1,  //PowerOffUnusedLanesEnabled,
      1,  //PowerOffUnusedPllsEnabled
      1,  //ClkGating
      1,  //LclkGating
      1,  //TxclkGatingPllPowerDown
      1,  //PllOffInL1
      1   //AccessEncoding
    },
    GNB_PCIE_MASTERPLL_A //MasterPll
  },
//------------------------------ GPP0 WRAPPER START-------------------------------------
  //Port PBR7
  {
    {
      DESCRIPTOR_PCIE_ENGINE,
      offsetof (RV_COMPLEX_CONFIG, PortPBR7) - offsetof (RV_COMPLEX_CONFIG, Gpp0Wrapper),
      offsetof (RV_COMPLEX_CONFIG, PortPBR6) - offsetof (RV_COMPLEX_CONFIG, PortPBR7),
      0
    },
    { PciePortEngine, UNUSED_LANE_ID, UNUSED_LANE_ID},  // EngineData
    0,                                                  // Initialization Status
    0xFF,                                               // Scratch
    {
      {
        {0},
        UNUSED_LANE_ID,
        UNUSED_LANE_ID,
        PBR7_NATIVE_PCI_DEV,
        PBR7_NATIVE_PCI_FUN,
        PBR7_CORE_ID,
        PBR7_PORT_ID,
        {PBR7_PCI_ADDRESS},
        LinkStateResetExit,
        PBR7,
        PBR7_UNIT_ID,
        0,                    // Power Off Unused Lanes PLL     [0:0]
        0,                    // Power Off Unused Lanes PLL L1  [1:1]
        0,                    // DynamicLanesPowerState         [2:7]
        0,                    // Slot Power Limit
        5,                    // MAX_PAYLOAD
        0,                    // L1 Immediate ACK         [0:0]
        0,                    // TX De-emphasis           [1:4]
        0,                    // TX Margin                [5:7]
        0,                    // Scrambler control        [0:0]
        1,                    // Equalization Search Mode [1:2]
        0,                    // Disable Gen3 EQ Phase2/3 [3:3]
        0,                    // Enable PM L1 SS L1.1     [4:4]
        0,                    // Enable PM L1 SS L1.2     [5:5]
        0,                    // Reserved                 [6:7]
        0xf,                  // ClkReq                   [0:3]
        0,                    // Reserved                 [4:7]
        {0},                  // FlowCntlCredits
        {0,1,1},              // SpcMode
        {0},                  // LaneEqualizationCntl
        0,                    // Figure of Merit time
        0,                    // PortReversal             [0:0]
        0,                    // BypassGen3EQ             [1:1]
        1,                    // Enable3x3Search          [2:2]
        0,                    // EnableSafeSearch         [3:3]
        0,                    // SRIS SRNS                [4:4]
        0,                    // SRIS Enable              [5:5]
        0,                    // SRIS Auto Detect Enable  [6:6]
        0                     // SRIS Lower SKP Support   [7:7]
      },
    },
  },
  //Port PBR6
  {
    {
      DESCRIPTOR_PCIE_ENGINE,
      offsetof (RV_COMPLEX_CONFIG, PortPBR6) - offsetof (RV_COMPLEX_CONFIG, Gpp0Wrapper),
      offsetof (RV_COMPLEX_CONFIG, PortPBR5) - offsetof (RV_COMPLEX_CONFIG, PortPBR6),
      0
    },
    { PciePortEngine, UNUSED_LANE_ID, UNUSED_LANE_ID},  // EngineData
    0,                                                  // Initialization Status
    0xFF,                                               // Scratch
    {
      {
        {0},                  // Port Data  PCIe_PORT_DATA
        UNUSED_LANE_ID,       // Start Core Lane
        UNUSED_LANE_ID,       // End Core Lane
        PBR6_NATIVE_PCI_DEV,  // NativeDevNumber          [0:4]
        PBR6_NATIVE_PCI_FUN,  // NativeFunNumber          [5:7]
        PBR6_CORE_ID,         // CoreID                   [0:3]
        PBR6_PORT_ID,         // PortID                   [4:7]
        {PBR6_PCI_ADDRESS},   // Address PCI_ADDRESS
        LinkStateResetExit,   // PciBridgeId              [0:3]
        PBR6,                 // GfxWrkRetryCount         [4:7]
        PBR6_UNIT_ID,         // LogicalBridgeId
        0,                    // Power Off Unused Lanes PLL     [0:0]
        0,                    // Power Off Unused Lanes PLL L1  [1:1]
        0,                    // DynamicLanesPowerState         [2:7]
        0,                    // Slot Power Limit
        5,                    // MAX_PAYLOAD
        0,                    // L1 Immediate ACK         [0:0]
        0,                    // TX De-emphasis           [1:4]
        0,                    // TX Margin                [5:7]
        0,                    // Scrambler control        [0:0]
        1,                    // Equalization Search Mode [1:2]
        0,                    // Disable Gen3 EQ Phase2/3 [3:3]
        0,                    // Enable PM L1 SS L1.1     [4:4]
        0,                    // Enable PM L1 SS L1.2     [5:5]
        0,                    // Reserved                 [6:7]
        0xf,                  // ClkReq                   [0:3]
        0,                    // Reserved                 [4:7]
        {0},                  // FlowCntlCredits
        {0,1,1},              // SpcMode
        {0},                  // LaneEqualizationCntl
        0,                    // Figure of Merit time
        0,                    // PortReversal             [0:0]
        0,                    // BypassGen3EQ             [1:1]
        1,                    // Enable3x3Search          [2:2]
        0,                    // EnableSafeSearch         [3:3]
        0,                    // SRIS SRNS                [4:4]
        0,                    // SRIS Enable              [5:5]
        0,                    // SRIS Auto Detect Enable  [6:6]
        0                     // SRIS Lower SKP Support   [7:7]
      },
    },
  },
  //Port PBR5
  {
    {
      DESCRIPTOR_PCIE_ENGINE,
      offsetof (RV_COMPLEX_CONFIG, PortPBR5) - offsetof (RV_COMPLEX_CONFIG, Gpp0Wrapper),
      offsetof (RV_COMPLEX_CONFIG, PortPBR4) - offsetof (RV_COMPLEX_CONFIG, PortPBR5),
      0
    },
    { PciePortEngine, UNUSED_LANE_ID, UNUSED_LANE_ID},  // EngineData
    0,                                                  // Initialization Status
    0xFF,                                               // Scratch
    {
      {
        {0},                  // Port Data  PCIe_PORT_DATA
        UNUSED_LANE_ID,       // Start Core Lane
        UNUSED_LANE_ID,       // End Core Lane
        PBR5_NATIVE_PCI_DEV,  // NativeDevNumber          [0:4]
        PBR5_NATIVE_PCI_FUN,  // NativeFunNumber          [5:7]
        PBR5_CORE_ID,         // CoreID                   [0:3]
        PBR5_PORT_ID,         // PortID                   [4:7]
        {PBR5_PCI_ADDRESS},   // Address PCI_ADDRESS
        LinkStateResetExit,   // PciBridgeId              [0:3]
        PBR5,                 // GfxWrkRetryCount         [4:7]
        PBR5_UNIT_ID,         // LogicalBridgeId
        0,                    // Power Off Unused Lanes PLL     [0:0]
        0,                    // Power Off Unused Lanes PLL L1  [1:1]
        0,                    // DynamicLanesPowerState         [2:7]
        0,                    // Slot Power Limit
        5,                    // MAX_PAYLOAD
        0,                    // L1 Immediate ACK         [0:0]
        0,                    // TX De-emphasis           [1:4]
        0,                    // TX Margin                [5:7]
        0,                    // Scrambler control        [0:0]
        1,                    // Equalization Search Mode [1:2]
        0,                    // Disable Gen3 EQ Phase2/3 [3:3]
        0,                    // Enable PM L1 SS L1.1     [4:4]
        0,                    // Enable PM L1 SS L1.2     [5:5]
        0,                    // Reserved                 [6:7]
        0xf,                  // ClkReq                   [0:3]
        0,                    // Reserved                 [4:7]
        {0},                  // FlowCntlCredits
        {0,1,1},              // SpcMode
        {0},                  // LaneEqualizationCntl
        0,                    // Figure of Merit time
        0,                    // PortReversal             [0:0]
        0,                    // BypassGen3EQ             [1:1]
        1,                    // Enable3x3Search          [2:2]
        0,                    // EnableSafeSearch         [3:3]
        0,                    // SRIS SRNS                [4:4]
        0,                    // SRIS Enable              [5:5]
        0,                    // SRIS Auto Detect Enable  [6:6]
        0                     // SRIS Lower SKP Support   [7:7]
      },
    },
  },
  //Port PBR4
  {
    {
      DESCRIPTOR_PCIE_ENGINE,
      offsetof (RV_COMPLEX_CONFIG, PortPBR4) - offsetof (RV_COMPLEX_CONFIG, Gpp0Wrapper),
      offsetof (RV_COMPLEX_CONFIG, PortPBR3) - offsetof (RV_COMPLEX_CONFIG, PortPBR4),
      0
    },
    { PciePortEngine, UNUSED_LANE_ID, UNUSED_LANE_ID},  // EngineData
    0,                                                  // Initialization Status
    0xFF,                                               // Scratch
    {
      {
        {0},                  // Port Data  PCIe_PORT_DATA
        UNUSED_LANE_ID,       // Start Core Lane
        UNUSED_LANE_ID,       // End Core Lane
        PBR4_NATIVE_PCI_DEV,  // NativeDevNumber          [0:4]
        PBR4_NATIVE_PCI_FUN,  // NativeFunNumber          [5:7]
        PBR4_CORE_ID,         // CoreID                   [0:3]
        PBR4_PORT_ID,         // PortID                   [4:7]
        {PBR4_PCI_ADDRESS},   // Address PCI_ADDRESS
        LinkStateResetExit,   // PciBridgeId              [0:3]
        PBR4,                 // GfxWrkRetryCount         [4:7]
        PBR4_UNIT_ID,         // LogicalBridgeId
        0,                    // Power Off Unused Lanes PLL     [0:0]
        0,                    // Power Off Unused Lanes PLL L1  [1:1]
        0,                    // DynamicLanesPowerState         [2:7]
        0,                    // Slot Power Limit
        5,                    // MAX_PAYLOAD
        0,                    // L1 Immediate ACK         [0:0]
        0,                    // TX De-emphasis           [1:4]
        0,                    // TX Margin                [5:7]
        0,                    // Scrambler control        [0:0]
        1,                    // Equalization Search Mode [1:2]
        0,                    // Disable Gen3 EQ Phase2/3 [3:3]
        0,                    // Enable PM L1 SS L1.1     [4:4]
        0,                    // Enable PM L1 SS L1.2     [5:5]
        0,                    // Reserved                 [6:7]
        0xf,                  // ClkReq                   [0:3]
        0,                    // Reserved                 [4:7]
        {0},                  // FlowCntlCredits
        {0,1,1},              // SpcMode
        {0},                  // LaneEqualizationCntl
        0,                    // Figure of Merit time
        0,                    // PortReversal             [0:0]
        0,                    // BypassGen3EQ             [1:1]
        1,                    // Enable3x3Search          [2:2]
        0,                    // EnableSafeSearch         [3:3]
        0,                    // SRIS SRNS                [4:4]
        0,                    // SRIS Enable              [5:5]
        0,                    // SRIS Auto Detect Enable  [6:6]
        0                     // SRIS Lower SKP Support   [7:7]
      },
    },
  },
  //Port PBR3
  {
    {
      DESCRIPTOR_PCIE_ENGINE,
      offsetof (RV_COMPLEX_CONFIG, PortPBR3) - offsetof (RV_COMPLEX_CONFIG, Gpp0Wrapper),
      offsetof (RV_COMPLEX_CONFIG, PortPBR2) - offsetof (RV_COMPLEX_CONFIG, PortPBR3),
      0
    },
    { PciePortEngine, UNUSED_LANE_ID, UNUSED_LANE_ID},  // EngineData
    0,                                                  // Initialization Status
    0xFF,                                               // Scratch
    {
      {
        {0},                  // Port Data  PCIe_PORT_DATA
        UNUSED_LANE_ID,       // Start Core Lane
        UNUSED_LANE_ID,       // End Core Lane
        PBR3_NATIVE_PCI_DEV,  // NativeDevNumber          [0:4]
        PBR3_NATIVE_PCI_FUN,  // NativeFunNumber          [5:7]
        PBR3_CORE_ID,         // CoreID                   [0:3]
        PBR3_PORT_ID,         // PortID                   [4:7]
        {PBR3_PCI_ADDRESS},   // Address PCI_ADDRESS
        LinkStateResetExit,   // PciBridgeId              [0:3]
        PBR3,                 // GfxWrkRetryCount         [4:7]
        PBR3_UNIT_ID,         // LogicalBridgeId
        0,                    // Power Off Unused Lanes PLL     [0:0]
        0,                    // Power Off Unused Lanes PLL L1  [1:1]
        0,                    // DynamicLanesPowerState         [2:7]
        0,                    // Slot Power Limit
        5,                    // MAX_PAYLOAD
        0,                    // L1 Immediate ACK         [0:0]
        0,                    // TX De-emphasis           [1:4]
        0,                    // TX Margin                [5:7]
        0,                    // Scrambler control        [0:0]
        1,                    // Equalization Search Mode [1:2]
        0,                    // Disable Gen3 EQ Phase2/3 [3:3]
        0,                    // Enable PM L1 SS L1.1     [4:4]
        0,                    // Enable PM L1 SS L1.2     [5:5]
        0,                    // Reserved                 [6:7]
        0xf,                  // ClkReq                   [0:3]
        0,                    // Reserved                 [4:7]
        {0},                  // FlowCntlCredits
        {0,1,1},              // SpcMode
        {0},                  // LaneEqualizationCntl
        0,                    // Figure of Merit time
        0,                    // PortReversal             [0:0]
        0,                    // BypassGen3EQ             [1:1]
        1,                    // Enable3x3Search          [2:2]
        0,                    // EnableSafeSearch         [3:3]
        0,                    // SRIS SRNS                [4:4]
        0,                    // SRIS Enable              [5:5]
        0,                    // SRIS Auto Detect Enable  [6:6]
        0                     // SRIS Lower SKP Support   [7:7]
      },
    },
  },
  //Port PBR2
  {
    {
      DESCRIPTOR_PCIE_ENGINE,
      offsetof (RV_COMPLEX_CONFIG, PortPBR2) - offsetof (RV_COMPLEX_CONFIG, Gpp0Wrapper),
      offsetof (RV_COMPLEX_CONFIG, PortPBR1) - offsetof (RV_COMPLEX_CONFIG, PortPBR2),
      0
    },
    { PciePortEngine, UNUSED_LANE_ID, UNUSED_LANE_ID},  // EngineData
    0,                                                  // Initialization Status
    0xFF,                                               // Scratch
    {
      {
        {0},                  // Port Data  PCIe_PORT_DATA
        UNUSED_LANE_ID,       // Start Core Lane
        UNUSED_LANE_ID,       // End Core Lane
        PBR2_NATIVE_PCI_DEV,  // NativeDevNumber          [0:4]
        PBR2_NATIVE_PCI_FUN,  // NativeFunNumber          [5:7]
        PBR2_CORE_ID,         // CoreID                   [0:3]
        PBR2_PORT_ID,         // PortID                   [4:7]
        {PBR2_PCI_ADDRESS},   // Address PCI_ADDRESS
        LinkStateResetExit,   // PciBridgeId              [0:3]
        PBR2,                 // GfxWrkRetryCount         [4:7]
        PBR2_UNIT_ID,         // LogicalBridgeId
        0,                    // Power Off Unused Lanes PLL     [0:0]
        0,                    // Power Off Unused Lanes PLL L1  [1:1]
        0,                    // DynamicLanesPowerState         [2:7]
        0,                    // Slot Power Limit
        5,                    // MAX_PAYLOAD
        0,                    // L1 Immediate ACK         [0:0]
        0,                    // TX De-emphasis           [1:4]
        0,                    // TX Margin                [5:7]
        0,                    // Scrambler control        [0:0]
        1,                    // Equalization Search Mode [1:2]
        0,                    // Disable Gen3 EQ Phase2/3 [3:3]
        0,                    // Enable PM L1 SS L1.1     [4:4]
        0,                    // Enable PM L1 SS L1.2     [5:5]
        0,                    // Reserved                 [6:7]
        0xf,                  // ClkReq                   [0:3]
        0,                    // Reserved                 [4:7]
        {0},                  // FlowCntlCredits
        {0,1,1},              // SpcMode
        {0},                  // LaneEqualizationCntl
        0,                    // Figure of Merit time
        0,                    // PortReversal             [0:0]
        0,                    // BypassGen3EQ             [1:1]
        1,                    // Enable3x3Search          [2:2]
        0,                    // EnableSafeSearch         [3:3]
        0,                    // SRIS SRNS                [4:4]
        0,                    // SRIS Enable              [5:5]
        0,                    // SRIS Auto Detect Enable  [6:6]
        0                     // SRIS Lower SKP Support   [7:7]
      },
    },
  },
  //Port PBR1
  {
    {
      DESCRIPTOR_PCIE_ENGINE,
      offsetof (RV_COMPLEX_CONFIG, PortPBR1) - offsetof (RV_COMPLEX_CONFIG, Gpp0Wrapper),
      offsetof (RV_COMPLEX_CONFIG, PortPBR0) - offsetof (RV_COMPLEX_CONFIG, PortPBR1),
      0
    },
    { PciePortEngine, UNUSED_LANE_ID, UNUSED_LANE_ID},  // EngineData
    0,                                                  // Initialization Status
    0xFF,                                               // Scratch
    {
      {
        {0},                  // Port Data  PCIe_PORT_DATA
        4,                    // Start Core Lane
        7,                    // End Core Lane
        PBR1_NATIVE_PCI_DEV,  // NativeDevNumber          [0:4]
        PBR1_NATIVE_PCI_FUN,  // NativeFunNumber          [5:7]
        PBR1_CORE_ID,         // CoreID                   [0:3]
        PBR1_PORT_ID,         // PortID                   [4:7]
        {PBR1_PCI_ADDRESS},   // Address PCI_ADDRESS
        LinkStateResetExit,   // PciBridgeId              [0:3]
        PBR1,                 // GfxWrkRetryCount         [4:7]
        PBR1_UNIT_ID,         // LogicalBridgeId
        0,                    // Power Off Unused Lanes PLL     [0:0]
        0,                    // Power Off Unused Lanes PLL L1  [1:1]
        0,                    // DynamicLanesPowerState         [2:7]
        0,                    // Slot Power Limit
        5,                    // MAX_PAYLOAD
        0,                    // L1 Immediate ACK         [0:0]
        0,                    // TX De-emphasis           [1:4]
        0,                    // TX Margin                [5:7]
        0,                    // Scrambler control        [0:0]
        1,                    // Equalization Search Mode [1:2]
        0,                    // Disable Gen3 EQ Phase2/3 [3:3]
        0,                    // Enable PM L1 SS L1.1     [4:4]
        0,                    // Enable PM L1 SS L1.2     [5:5]
        0,                    // Reserved                 [6:7]
        0xf,                  // ClkReq                   [0:3]
        0,                    // Reserved                 [4:7]
        {0},                  // FlowCntlCredits
        {0,1,1},              // SpcMode
        {0},                  // LaneEqualizationCntl
        0,                    // Figure of Merit time
        0,                    // PortReversal             [0:0]
        0,                    // BypassGen3EQ             [1:1]
        1,                    // Enable3x3Search          [2:2]
        0,                    // EnableSafeSearch         [3:3]
        0,                    // SRIS SRNS                [4:4]
        0,                    // SRIS Enable              [5:5]
        0,                    // SRIS Auto Detect Enable  [6:6]
        0                     // SRIS Lower SKP Support   [7:7]
      },
    },
  },
  //Port PBR0
  {
    {
      DESCRIPTOR_PCIE_ENGINE | DESCRIPTOR_TERMINATE_LIST | DESCRIPTOR_TERMINATE_GNB | DESCRIPTOR_TERMINATE_TOPOLOGY,
      offsetof (RV_COMPLEX_CONFIG, PortPBR0) - offsetof (RV_COMPLEX_CONFIG, Gpp0Wrapper),
      0,
      0
    },
    { PciePortEngine, 0, 3 },                       // EngineData
    0,                                              // Initialization Status
    0xFF,                                           // Scratch
    {
      {
        {0},                  // Port Data  PCIe_PORT_DATA
        0,                    // Start Core Lane
        3,                    // End Core Lane
        PBR0_NATIVE_PCI_DEV,  // NativeDevNumber          [0:4]
        PBR0_NATIVE_PCI_FUN,  // NativeFunNumber          [5:7]
        PBR0_CORE_ID,         // CoreID                   [0:3]
        PBR0_PORT_ID,         // PortID                   [4:7]
        {PBR0_PCI_ADDRESS},   // Address PCI_ADDRESS
        LinkStateResetExit,   // PciBridgeId              [0:3]
        PBR0,                 // GfxWrkRetryCount         [4:7]
        PBR0_UNIT_ID,         // LogicalBridgeId
        0,                    // Power Off Unused Lanes PLL     [0:0]
        0,                    // Power Off Unused Lanes PLL L1  [1:1]
        0,                    // DynamicLanesPowerState         [2:7]
        0,                    // Slot Power Limit
        5,                    // MAX_PAYLOAD
        0,                    // L1 Immediate ACK         [0:0]
        0,                    // TX De-emphasis           [1:4]
        0,                    // TX Margin                [5:7]
        0,                    // Scrambler control        [0:0]
        1,                    // Equalization Search Mode [1:2]
        0,                    // Disable Gen3 EQ Phase2/3 [3:3]
        0,                    // Enable PM L1 SS L1.1     [4:4]
        0,                    // Enable PM L1 SS L1.2     [5:5]
        0,                    // Reserved                 [6:7]
        0xf,                  // ClkReq                   [0:3]
        0,                    // Reserved                 [4:7]
        {0},                  // FlowCntlCredits
        {0,1,1},              // SpcMode
        {0},                  // LaneEqualizationCntl
        0,                    // Figure of Merit time
        0,                    // PortReversal             [0:0]
        0,                    // BypassGen3EQ             [1:1]
        1,                    // Enable3x3Search          [2:2]
        0,                    // EnableSafeSearch         [3:3]
        0,                    // SRIS SRNS                [4:4]
        0,                    // SRIS Enable              [5:5]
        0,                    // SRIS Auto Detect Enable  [6:6]
        0                     // SRIS Lower SKP Support   [7:7]
      },
    },
  }
//------------------------------ GPP0 WRAPPER END -------------------------------------
};




