/*****************************************************************************
 *
 * Copyright (C) 2016-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef _APCB_DF_XGMI_TX_EQ_H_
#define _APCB_DF_XGMI_TX_EQ_H_

//
// @APCB_START
//

//
// XGMI TX EQ definitions
//
#define XGMI_BIT_POS(Position)                  (1 << Position)

// NOTE: Bit28~31 are used as NodeType in common.
#define XGMI_TX_EQ_COMMON_NODE_TYPE_SHIFT       28
#define XGMI_TX_EQ_COMMON_NODE_TYPE_SIZE_MASK   0x0F
#define XGMI_TX_EQ_COMMON_NODE_TYPE_MASK        (XGMI_TX_EQ_COMMON_NODE_TYPE_SIZE_MASK << XGMI_TX_EQ_COMMON_NODE_TYPE_SHIFT)
// Shared node type definitions
#define   XGMI_TX_EQ_NODE_TYPE_RSVD             0x0
#define   XGMI_TX_EQ_NODE_TYPE_DATA             0x1
#define   XGMI_TX_EQ_NODE_TYPE_LOC              0x2
#define   XGMI_TX_EQ_NODE_TYPE_FREQ             0x3
#define   XGMI_TX_EQ_NODE_TYPE_FREQ_TBL         0x4
#define   XGMI_TX_EQ_NODE_TYPE_FREQ_END         0x5
#define   XGMI_TX_EQ_NODE_TYPE_END              0xF

// XGMI_TX_EQ MAKE_TX_EQ_FREQ_TABLE attribute
typedef struct _XGMI_TX_EQ_COMMON {
  UINT32  :24;                                   ///< Reserved
  UINT32  :4;                                    ///< Reserved
  UINT32  NodeType:4;                            ///< Node type
} XGMI_TX_EQ_COMMON;

#define XGMI_TX_EQ_FREQ_SPEED_MASK_SHIFT        0
#define XGMI_TX_EQ_FREQ_SPEED_MASK_SIZE_MASK    0xFF
#define XGMI_TX_EQ_FREQ_SPEED_MASK_MASK         (XGMI_TX_EQ_FREQ_SPEED_MASK_SIZE_MASK << XGMI_TX_EQ_FREQ_SPEED_MASK_SHIFT)
#define   XGMI_TX_EQ_SPEED_53                   XGMI_BIT_POS(0)   // Instance ID of 5.3G
#define   XGMI_TX_EQ_SPEED_64                   XGMI_BIT_POS(1)   // Instance ID of 6.4G
#define   XGMI_TX_EQ_SPEED_75                   XGMI_BIT_POS(2)   // Instance ID of 7.5G
#define   XGMI_TX_EQ_SPEED_85                   XGMI_BIT_POS(3)   // Instance ID of 8.5G
#define   XGMI_TX_EQ_SPEED_96                   XGMI_BIT_POS(4)   // Instance ID of 9.6G
#define   XGMI_TX_EQ_SPEED_107                  XGMI_BIT_POS(5)   // Instance ID of 10.7G
#define   XGMI_TX_EQ_SPEED_117                  XGMI_BIT_POS(6)   // Instance ID of 11.7G
#define   XGMI_TX_EQ_SPEED_128                  XGMI_BIT_POS(7)   // Instance ID of 12.8G
#define   XGMI_TX_EQ_SPEED_ALL                  (XGMI_TX_EQ_SPEED_53 | XGMI_TX_EQ_SPEED_64 | XGMI_TX_EQ_SPEED_75 | XGMI_TX_EQ_SPEED_85 | XGMI_TX_EQ_SPEED_96 | XGMI_TX_EQ_SPEED_107 | XGMI_TX_EQ_SPEED_117 | XGMI_TX_EQ_SPEED_128)

// XGMI_TX_EQ MAKE_TX_EQ_FREQ_TABLE attribute
typedef struct _XGMI_TX_EQ_FREQ {
  UINT32  SpeedMask:8;                          ///< Speed mask
  UINT32  :16;                                  ///< Reserved
  UINT32  :4;                                   ///< Reserved
  UINT32  NodeType:4;                           ///< Node type: MAKE_TX_EQ_FREQ_TABLE
} XGMI_TX_EQ_FREQ;

#define MAKE_TX_EQ_FREQ(SpeedMask)               ((UINT32) (((XGMI_TX_EQ_NODE_TYPE_FREQ & XGMI_TX_EQ_COMMON_NODE_TYPE_SIZE_MASK) << XGMI_TX_EQ_COMMON_NODE_TYPE_SHIFT) | \
                                                                   ((SpeedMask & XGMI_TX_EQ_FREQ_SPEED_MASK_SIZE_MASK) << XGMI_TX_EQ_FREQ_SPEED_MASK_SHIFT)))
#define MAKE_TX_EQ_FREQ_TBL(SpeedMask)           ((UINT32) (((XGMI_TX_EQ_NODE_TYPE_FREQ_TBL & XGMI_TX_EQ_COMMON_NODE_TYPE_SIZE_MASK) << XGMI_TX_EQ_COMMON_NODE_TYPE_SHIFT) | \
                                                                   ((SpeedMask & XGMI_TX_EQ_FREQ_SPEED_MASK_SIZE_MASK) << XGMI_TX_EQ_FREQ_SPEED_MASK_SHIFT)))


#define XGMI_TX_EQ_ATTR_LANE_MASK_SHIFT         0
#define XGMI_TX_EQ_ATTR_LANE_MASK_SIZE_MASK     0xFFFF
#define XGMI_TX_EQ_ATTR_LANE_MASK_MASK          (XGMI_TX_EQ_ATTR_LANE_MASK_SIZE_MASK << XGMI_TX_EQ_ATTR_LANE_MASK_SHIFT)
#define XGMI_TX_EQ_ATTR_DIE_MASK_SHIFT          16
#define XGMI_TX_EQ_ATTR_DIE_MASK_SIZE_MASK      0xFF
#define XGMI_TX_EQ_ATTR_DIE_MASK_MASK           (XGMI_TX_EQ_ATTR_DIE_MASK_SIZE_MASK << XGMI_TX_EQ_ATTR_DIE_MASK_SHIFT)
#define XGMI_TX_EQ_ATTR_SOCKET_MASK_SHIFT       24
#define XGMI_TX_EQ_ATTR_SOCKET_MASK_SIZE_MASK   0x03
#define XGMI_TX_EQ_ATTR_SOCKET_MASK_MASK        (XGMI_TX_EQ_ATTR_SOCKET_MASK_SIZE_MASK << XGMI_TX_EQ_ATTR_SOCKET_MASK_SHIFT)
#define XGMI_TX_EQ_ATTR_DATA_MODE_SHIFT         26
#define XGMI_TX_EQ_ATTR_DATA_MODE_SIZE_MASK     0x03
#define XGMI_TX_EQ_ATTR_DATA_MODE_MASK          (XGMI_TX_EQ_ATTR_DATA_MODE_SIZE_MASK << XGMI_TX_EQ_ATTR_DATA_MODE_SHIFT)
// Data mode definitions
#define   XGMI_TX_EQ_DATA_MODE_PAIR             0        ///< Pair mode, same value for all matched lanes.
#define   XGMI_TX_EQ_DATA_MODE_LIST             1        ///< List mode, list each value for each matched lanes from low to high socket/die/lane.
#define   XGMI_TX_EQ_DATA_MODE_LIST_COMPACT     2        ///< List compact mode, list each value for each matched lanes from low to high socket/die/lane in compact format
#define   XGMI_TX_EQ_DATA_MODE_MAX              3        ///< Maximum data mode
// NOTE: Bit28~31 are used as NodeType in common.

#define XGMI_ALL_SOCKETS                        0x03     ///< 2 sockets per system
#define XGMI_MAX_SOCKET_COUNT                   2
#define XGMI_ALL_DIES                           0x0F     ///< 4 dies per socket
#define XGMI_MAX_DIE_COUNT_PER_SOCKET           4
#define XGMI_ALL_LANES                          0xFFFF   ///< 16 lanes per die
#define XGMI_MAX_LANE_COUNT_PER_DIE             16
#define XGMI_SOCKET0                            XGMI_BIT_POS(0)
#define XGMI_SOCKET1                            XGMI_BIT_POS(1)
#define XGMI_DIE0                               XGMI_BIT_POS(0)
#define XGMI_DIE1                               XGMI_BIT_POS(1)
#define XGMI_DIE2                               XGMI_BIT_POS(2)
#define XGMI_DIE3                               XGMI_BIT_POS(3)
#define XGMI_DIE4                               XGMI_BIT_POS(4)
#define XGMI_DIE5                               XGMI_BIT_POS(5)
#define XGMI_DIE6                               XGMI_BIT_POS(6)
#define XGMI_DIE7                               XGMI_BIT_POS(7)
#define XGMI_LANE0                              XGMI_BIT_POS(0)
#define XGMI_LANE1                              XGMI_BIT_POS(1)
#define XGMI_LANE2                              XGMI_BIT_POS(2)
#define XGMI_LANE3                              XGMI_BIT_POS(3)
#define XGMI_LANE4                              XGMI_BIT_POS(4)
#define XGMI_LANE5                              XGMI_BIT_POS(5)
#define XGMI_LANE6                              XGMI_BIT_POS(6)
#define XGMI_LANE7                              XGMI_BIT_POS(7)
#define XGMI_LANE8                              XGMI_BIT_POS(8)
#define XGMI_LANE9                              XGMI_BIT_POS(9)
#define XGMI_LANE10                             XGMI_BIT_POS(10)
#define XGMI_LANE11                             XGMI_BIT_POS(11)
#define XGMI_LANE12                             XGMI_BIT_POS(12)
#define XGMI_LANE13                             XGMI_BIT_POS(13)
#define XGMI_LANE14                             XGMI_BIT_POS(14)
#define XGMI_LANE15                             XGMI_BIT_POS(15)

// XGMI_TX_EQ apply attribute
typedef struct _XGMI_TX_EQ_ATTR {
  UINT32  LaneMask:16;                           ///< Lane mask per die       (2^4)
  UINT32  DieMask:8;                             ///< Die mask per socket     (2^3)
  UINT32  SocketMask:2;                          ///< Socket mask per system  (2^1)
  UINT32  DataMode:2;                            ///< Data mode. Pair or List
  UINT32  NodeType:4;                            ///< Node type: Location
} XGMI_TX_EQ_ATTR;

#define MAKE_TX_EQ_ATTR(SocketMask, DieMask, LaneMask, DataMode)  ((UINT32) (((XGMI_TX_EQ_NODE_TYPE_LOC & XGMI_TX_EQ_COMMON_NODE_TYPE_SIZE_MASK) << XGMI_TX_EQ_COMMON_NODE_TYPE_SHIFT) | \
                                                                   ((DataMode & XGMI_TX_EQ_ATTR_DATA_MODE_SIZE_MASK) << XGMI_TX_EQ_ATTR_DATA_MODE_SHIFT) | \
                                                                   ((SocketMask & XGMI_TX_EQ_ATTR_SOCKET_MASK_SIZE_MASK) << XGMI_TX_EQ_ATTR_SOCKET_MASK_SHIFT) | \
                                                                   ((DieMask & XGMI_TX_EQ_ATTR_DIE_MASK_SIZE_MASK) << XGMI_TX_EQ_ATTR_DIE_MASK_SHIFT) | \
                                                                   ((LaneMask & XGMI_TX_EQ_ATTR_LANE_MASK_SIZE_MASK) << XGMI_TX_EQ_ATTR_LANE_MASK_SHIFT)))

#define XGMI_TX_EQ_DATA_EQ_PRE_SHIFT            0
#define XGMI_TX_EQ_DATA_EQ_PRE_SIZE_MASK        0x3F
#define XGMI_TX_EQ_DATA_EQ_PRE_MASK             (XGMI_TX_EQ_DATA_EQ_PRE_SIZE_MASK << XGMI_TX_EQ_DATA_EQ_PRE_SHIFT)
#define XGMI_TX_EQ_DATA_EQ_MAIN_SHIFT           8
#define XGMI_TX_EQ_DATA_EQ_MAIN_SIZE_MASK       0x3F
#define XGMI_TX_EQ_DATA_EQ_MAIN_MASK            (XGMI_TX_EQ_DATA_EQ_MAIN_SIZE_MASK << XGMI_TX_EQ_DATA_EQ_MAIN_SHIFT)
#define XGMI_TX_EQ_DATA_EQ_POST_SHIFT           16
#define XGMI_TX_EQ_DATA_EQ_POST_SIZE_MASK       0x3F
#define XGMI_TX_EQ_DATA_EQ_POST_MASK            (XGMI_TX_EQ_DATA_EQ_POST_SIZE_MASK << XGMI_TX_EQ_DATA_EQ_POST_SHIFT)
// NOTE: Bit28~31 are used as NodeType in common.

// XGMI TX EQ settings
typedef struct _XGMI_TX_EQ_DATA {
  UINT32  EqPre:6;                              ///< EQ_PRE/TxCoeffPre
  UINT32  :2;                                   ///< Reserved
  UINT32  EqMain:6;                             ///< EQ_MAIN/TxCoeffCurs
  UINT32  :2;                                   ///< Reserved
  UINT32  EqPost:6;                             ///< EQ_POST/TxCoeffPost
  UINT32  :2;                                   ///< Reserved
  UINT32  :4;                                   ///< Reserved
  UINT32  NodeType:4;                           ///< Node type: Data
} XGMI_TX_EQ_DATA;

#define MAKE_TX_EQ_DATA(EqMain, EqPost, EqPre)          ((UINT32) (((XGMI_TX_EQ_NODE_TYPE_DATA & XGMI_TX_EQ_COMMON_NODE_TYPE_SIZE_MASK) << XGMI_TX_EQ_COMMON_NODE_TYPE_SHIFT) | \
                                                                   ((EqPost & XGMI_TX_EQ_DATA_EQ_POST_SIZE_MASK) << XGMI_TX_EQ_DATA_EQ_POST_SHIFT) | \
                                                                   ((EqMain & XGMI_TX_EQ_DATA_EQ_MAIN_SIZE_MASK) << XGMI_TX_EQ_DATA_EQ_MAIN_SHIFT) | \
                                                                   ((EqPre & XGMI_TX_EQ_DATA_EQ_PRE_SIZE_MASK) << XGMI_TX_EQ_DATA_EQ_PRE_SHIFT)))

#define XGMI_TX_EQ_DATA_COMPACT_EQ_PRE_SHIFT            0
#define XGMI_TX_EQ_DATA_COMPACT_EQ_PRE_SIZE_MASK        XGMI_TX_EQ_DATA_EQ_PRE_SIZE_MASK
#define XGMI_TX_EQ_DATA_COMPACT_EQ_PRE_MASK             (XGMI_TX_EQ_DATA_COMPACT_EQ_PRE_SIZE_MASK << XGMI_TX_EQ_DATA_COMPACT_EQ_PRE_SHIFT)
#define XGMI_TX_EQ_DATA_COMPACT_EQ_MAIN_SHIFT           6
#define XGMI_TX_EQ_DATA_COMPACT_EQ_MAIN_SIZE_MASK       XGMI_TX_EQ_DATA_EQ_MAIN_SIZE_MASK
#define XGMI_TX_EQ_DATA_COMPACT_EQ_MAIN_MASK            (XGMI_TX_EQ_DATA_COMPACT_EQ_MAIN_SIZE_MASK << XGMI_TX_EQ_DATA_COMPACT_EQ_MAIN_SHIFT)
#define XGMI_TX_EQ_DATA_COMPACT_EQ_POST_SHIFT           12
#define XGMI_TX_EQ_DATA_COMPACT_EQ_POST_SIZE_MASK       XGMI_TX_EQ_DATA_EQ_POST_SIZE_MASK
#define XGMI_TX_EQ_DATA_COMPACT_EQ_POST_MASK            (XGMI_TX_EQ_DATA_COMPACT_EQ_POST_SIZE_MASK << XGMI_TX_EQ_DATA_COMPACT_EQ_POST_SHIFT)

typedef struct _XGMI_TX_EQ_DATA_LIST_COMPACT {
  UINT32  EqPre0:6;                           ///< EQ_PRE/TxCoeffPre for 0
  UINT32  EqMain0:6;                          ///< EQ_MAIN/TxCoeffCurs for 0
  UINT32  EqPost0:6;                          ///< EQ_POST/TxCoeffPost for 0
  UINT32  EqPre1:6;                           ///< EQ_PRE/TxCoeffPre for 1
  UINT32  EqMain1:6;                          ///< EQ_MAIN/TxCoeffCurs for 1
  UINT32  EqPost1_1_0:2;                      ///< EQ_POST/TxCoeffPost for 1

  UINT32  EqPost1_5_2:4;                       ///< EQ_POST/TxCoeffPost for 1
  UINT32  EqPre2:6;                           ///< EQ_PRE/TxCoeffPre for 2
  UINT32  EqMain2:6;                          ///< EQ_MAIN/TxCoeffCurs for 2
  UINT32  EqPost2:6;                          ///< EQ_POST/TxCoeffPost for 2
  UINT32  EqPre3:6;                           ///< EQ_PRE/TxCoeffPre for 3
  UINT32  EqMain3_3_0:4;                      ///< EQ_MAIN/TxCoeffCurs for 3

  UINT32  EqMain3_5_4:2;                      ///< EQ_MAIN/TxCoeffCurs for 3
  UINT32  EqPost3:6;                          ///< EQ_POST/TxCoeffPost for 3
  UINT32  EqPre4:6;                           ///< EQ_PRE/TxCoeffPre for 4
  UINT32  EqMain4:6;                          ///< EQ_MAIN/TxCoeffCurs for 4
  UINT32  EqPost4:6;                          ///< EQ_POST/TxCoeffPost for 4
  UINT32  EqPre5:6;                           ///< EQ_PRE/TxCoeffPre for 5

  UINT32  EqMain5:6;                          ///< EQ_MAIN/TxCoeffCurs for 5
  UINT32  EqPost5:6;                          ///< EQ_POST/TxCoeffPost for 5
  UINT32  EqPre6:6;                           ///< EQ_PRE/TxCoeffPre for 6
  UINT32  EqMain6:6;                          ///< EQ_MAIN/TxCoeffCurs for 6
  UINT32  EqPost6:6;                          ///< EQ_POST/TxCoeffPost for 6
  UINT32  EqPre7_1_0:2;                       ///< EQ_PRE/TxCoeffPre for 7

  UINT32  EqPre7_5_2:4;                       ///< EQ_PRE/TxCoeffPre for 7
  UINT32  EqMain7:6;                          ///< EQ_MAIN/TxCoeffCurs for 7
  UINT32  EqPost7:6;                          ///< EQ_POST/TxCoeffPost for 7
  UINT32  EqPre8:6;                           ///< EQ_PRE/TxCoeffPre for 8
  UINT32  EqMain8:6;                          ///< EQ_MAIN/TxCoeffCurs for 8
  UINT32  EqPost8_3_0:4;                      ///< EQ_POST/TxCoeffPost for 8

  UINT32  EqPost8_5_4:2;                      ///< EQ_POST/TxCoeffPost for 8
  UINT32  EqPre9:6;                           ///< EQ_PRE/TxCoeffPre for 9
  UINT32  EqMain9:6;                          ///< EQ_MAIN/TxCoeffCurs for 9
  UINT32  EqPost9:6;                          ///< EQ_POST/TxCoeffPost for 9
  UINT32  EqPre10:6;                          ///< EQ_PRE/TxCoeffPre for 10
  UINT32  EqMain10:6;                         ///< EQ_MAIN/TxCoeffCurs for 10

  UINT32  EqPost10:6;                         ///< EQ_POST/TxCoeffPost for 10
  UINT32  EqPre11:6;                          ///< EQ_PRE/TxCoeffPre for 11
  UINT32  EqMain11:6;                         ///< EQ_MAIN/TxCoeffCurs for 11
  UINT32  EqPost11:6;                         ///< EQ_POST/TxCoeffPost for 11
  UINT32  EqPre12:6;                          ///< EQ_PRE/TxCoeffPre for 12
  UINT32  EqMain12_1_0:2;                     ///< EQ_MAIN/TxCoeffCurs for 12

  UINT32  EqMain12_5_2:4;                     ///< EQ_MAIN/TxCoeffCurs for 12
  UINT32  EqPost12:6;                         ///< EQ_POST/TxCoeffPost for 12
  UINT32  EqPre13:6;                          ///< EQ_PRE/TxCoeffPre for 13
  UINT32  EqMain13:6;                         ///< EQ_MAIN/TxCoeffCurs for 13
  UINT32  EqPost13:6;                         ///< EQ_POST/TxCoeffPost for 13
  UINT32  EqPre14_3_0:4;                      ///< EQ_PRE/TxCoeffPre for 14

  UINT32  EqPre14_5_4:2;                      ///< EQ_PRE/TxCoeffPre for 14
  UINT32  EqMain14:6;                         ///< EQ_MAIN/TxCoeffCurs for 14
  UINT32  EqPost14:6;                         ///< EQ_POST/TxCoeffPost for 14
  UINT32  EqPre15:6;                          ///< EQ_PRE/TxCoeffPre for 15
  UINT32  EqMain15:6;                         ///< EQ_MAIN/TxCoeffCurs for 15
  UINT32  EqPost15:6;                         ///< EQ_POST/TxCoeffPost for 15

} XGMI_TX_EQ_DATA_LIST_COMPACT;

#define MAKE_TX_EQ_LIST_COMPACT_DATA_SUB(EqMain, EqPost, EqPre)     ((UINT32) ( \
                                                                                ((EqPost & XGMI_TX_EQ_DATA_COMPACT_EQ_POST_SIZE_MASK) << XGMI_TX_EQ_DATA_COMPACT_EQ_POST_SHIFT) | \
                                                                                ((EqMain & XGMI_TX_EQ_DATA_COMPACT_EQ_MAIN_SIZE_MASK) << XGMI_TX_EQ_DATA_COMPACT_EQ_MAIN_SHIFT) | \
                                                                                ((EqPre & XGMI_TX_EQ_DATA_COMPACT_EQ_PRE_SIZE_MASK) << XGMI_TX_EQ_DATA_COMPACT_EQ_PRE_SHIFT)))
#define MAKE_TX_EQ_LIST_COMPACT_DATA_SUB_POS_DW0_1ST(EqDw0)         ((UINT32)  EqDw0 & 0x3FFFF)         // Lane0 18 bits
#define MAKE_TX_EQ_LIST_COMPACT_DATA_SUB_POS_DW0_2ND(EqDw1)         (((UINT32) EqDw1 & 0x03FFF) << 18)  // Lane1 14 bits

#define MAKE_TX_EQ_LIST_COMPACT_DATA_SUB_POS_DW1_1ST(EqDw1)         (((UINT32) EqDw1 & 0x3C000) >> 14)  // Lane1  4 bits
#define MAKE_TX_EQ_LIST_COMPACT_DATA_SUB_POS_DW1_2ND(EqDw2)         (((UINT32) EqDw2 & 0x3FFFF) << 4)   // Lane2 18 bits
#define MAKE_TX_EQ_LIST_COMPACT_DATA_SUB_POS_DW1_3RD(EqDw3)         (((UINT32) EqDw3 & 0x003FF) << 22)  // Lane3 10 bits

#define MAKE_TX_EQ_LIST_COMPACT_DATA_SUB_POS_DW2_1ST(EqDw3)         (((UINT32) EqDw3 & 0x3FC00) >> 10)  // Lane3  8 bits
#define MAKE_TX_EQ_LIST_COMPACT_DATA_SUB_POS_DW2_2ND(EqDw4)         (((UINT32) EqDw4 & 0x3FFFF) << 8)   // Lane4 18 bits
#define MAKE_TX_EQ_LIST_COMPACT_DATA_SUB_POS_DW2_3RD(EqDw5)         (((UINT32) EqDw5 & 0x0003F) << 26)  // Lane5  6 bits

#define MAKE_TX_EQ_LIST_COMPACT_DATA_SUB_POS_DW3_1ST(EqDw5)         (((UINT32) EqDw5 & 0x3FFC0) >> 6)   // Lane5 12 bits
#define MAKE_TX_EQ_LIST_COMPACT_DATA_SUB_POS_DW3_2ND(EqDw6)         (((UINT32) EqDw6 & 0x3FFFF) << 12)  // Lane6 18 bits
#define MAKE_TX_EQ_LIST_COMPACT_DATA_SUB_POS_DW3_3RD(EqDw7)         (((UINT32) EqDw7 & 0x00003) << 30)  // Lane7  2 bits

#define MAKE_TX_EQ_LIST_COMPACT_DATA_SUB_POS_DW4_1ST(EqDw7)         (((UINT32) EqDw7 & 0x3FFFC) >> 2)   // Lane7 16 bits
#define MAKE_TX_EQ_LIST_COMPACT_DATA_SUB_POS_DW4_2ND(EqDw8)         (((UINT32) EqDw8 & 0x0FFFF) << 16)  // Lane8 16 bits

#define MAKE_TX_EQ_LIST_COMPACT_DATA_SUB_POS_DW5_1ST(EqDw8)         (((UINT32) EqDw8 & 0x30000) >> 16)  // Lane8  2 bits
#define MAKE_TX_EQ_LIST_COMPACT_DATA_SUB_POS_DW5_2ND(EqDw9)         (((UINT32) EqDw9 & 0x3FFFF) << 2)   // Lane9 18 bits
#define MAKE_TX_EQ_LIST_COMPACT_DATA_SUB_POS_DW5_3RD(EqDw10)        (((UINT32) EqDw10 & 0x00FFF) << 20) // Lane10 12 bits

#define MAKE_TX_EQ_LIST_COMPACT_DATA_SUB_POS_DW6_1ST(EqDw10)        (((UINT32) EqDw10 & 0x3F000) >> 12) // Lane10  6 bits
#define MAKE_TX_EQ_LIST_COMPACT_DATA_SUB_POS_DW6_2ND(EqDw11)        (((UINT32) EqDw11 & 0x3FFFF) << 6)  // Lane11 18 bits
#define MAKE_TX_EQ_LIST_COMPACT_DATA_SUB_POS_DW6_3RD(EqDw12)        (((UINT32) EqDw12 & 0x000FF) << 24) // Lane12  8 bits

#define MAKE_TX_EQ_LIST_COMPACT_DATA_SUB_POS_DW7_1ST(EqDw12)        (((UINT32) EqDw12 & 0x3FF00) >> 8)  // Lane12 10 bits
#define MAKE_TX_EQ_LIST_COMPACT_DATA_SUB_POS_DW7_2ND(EqDw13)        (((UINT32) EqDw13 & 0x3FFFF) << 10) // Lane13 18 bits
#define MAKE_TX_EQ_LIST_COMPACT_DATA_SUB_POS_DW7_3RD(EqDw14)        (((UINT32) EqDw14 & 0x0000F) << 28) // Lane14  4 bits

#define MAKE_TX_EQ_LIST_COMPACT_DATA_SUB_POS_DW8_1ST(EqDw14)        (((UINT32) EqDw14 & 0x3FFF0) >> 4)  // Lane14 14 bits
#define MAKE_TX_EQ_LIST_COMPACT_DATA_SUB_POS_DW8_2ND(EqDw15)        (((UINT32) EqDw15 & 0x3FFFF) << 14) // Lane15 18 bits

#define MAKE_TX_EQ_LIST_COMPACT_DATA_DW0(EqDw0, EqDw1)              (MAKE_TX_EQ_LIST_COMPACT_DATA_SUB_POS_DW0_1ST(EqDw0) | MAKE_TX_EQ_LIST_COMPACT_DATA_SUB_POS_DW0_2ND(EqDw1))
#define MAKE_TX_EQ_LIST_COMPACT_DATA_DW1(EqDw1, EqDw2, EqDw3)       (MAKE_TX_EQ_LIST_COMPACT_DATA_SUB_POS_DW1_1ST(EqDw1) | MAKE_TX_EQ_LIST_COMPACT_DATA_SUB_POS_DW1_2ND(EqDw2) | MAKE_TX_EQ_LIST_COMPACT_DATA_SUB_POS_DW1_3RD(EqDw3))
#define MAKE_TX_EQ_LIST_COMPACT_DATA_DW2(EqDw3, EqDw4, EqDw5)       (MAKE_TX_EQ_LIST_COMPACT_DATA_SUB_POS_DW2_1ST(EqDw3) | MAKE_TX_EQ_LIST_COMPACT_DATA_SUB_POS_DW2_2ND(EqDw4) | MAKE_TX_EQ_LIST_COMPACT_DATA_SUB_POS_DW2_3RD(EqDw5))
#define MAKE_TX_EQ_LIST_COMPACT_DATA_DW3(EqDw5, EqDw6, EqDw7)       (MAKE_TX_EQ_LIST_COMPACT_DATA_SUB_POS_DW3_1ST(EqDw5) | MAKE_TX_EQ_LIST_COMPACT_DATA_SUB_POS_DW3_2ND(EqDw6) | MAKE_TX_EQ_LIST_COMPACT_DATA_SUB_POS_DW3_3RD(EqDw7))
#define MAKE_TX_EQ_LIST_COMPACT_DATA_DW4(EqDw7, EqDw8)              (MAKE_TX_EQ_LIST_COMPACT_DATA_SUB_POS_DW4_1ST(EqDw7) | MAKE_TX_EQ_LIST_COMPACT_DATA_SUB_POS_DW4_2ND(EqDw8))
#define MAKE_TX_EQ_LIST_COMPACT_DATA_DW5(EqDw8, EqDw9, EqDw10)      (MAKE_TX_EQ_LIST_COMPACT_DATA_SUB_POS_DW5_1ST(EqDw8) | MAKE_TX_EQ_LIST_COMPACT_DATA_SUB_POS_DW5_2ND(EqDw9) | MAKE_TX_EQ_LIST_COMPACT_DATA_SUB_POS_DW5_3RD(EqDw10))
#define MAKE_TX_EQ_LIST_COMPACT_DATA_DW6(EqDw10, EqDw11, EqDw12)    (MAKE_TX_EQ_LIST_COMPACT_DATA_SUB_POS_DW6_1ST(EqDw10) | MAKE_TX_EQ_LIST_COMPACT_DATA_SUB_POS_DW6_2ND(EqDw11) | MAKE_TX_EQ_LIST_COMPACT_DATA_SUB_POS_DW6_3RD(EqDw12))
#define MAKE_TX_EQ_LIST_COMPACT_DATA_DW7(EqDw12, EqDw13, EqDw14)    (MAKE_TX_EQ_LIST_COMPACT_DATA_SUB_POS_DW7_1ST(EqDw12) | MAKE_TX_EQ_LIST_COMPACT_DATA_SUB_POS_DW7_2ND(EqDw13) | MAKE_TX_EQ_LIST_COMPACT_DATA_SUB_POS_DW7_3RD(EqDw14))
#define MAKE_TX_EQ_LIST_COMPACT_DATA_DW8(EqDw14, EqDw15)            (MAKE_TX_EQ_LIST_COMPACT_DATA_SUB_POS_DW8_1ST(EqDw14) | MAKE_TX_EQ_LIST_COMPACT_DATA_SUB_POS_DW8_2ND(EqDw15))

#define MAKE_TX_EQ_LIST_COMPACT_DATA(EqDw0, EqDw1, EqDw2, EqDw3, EqDw4, EqDw5, EqDw6, EqDw7, EqDw8, EqDw9, EqDw10, EqDw11, EqDw12, EqDw13, EqDw14, EqDw15) \
                                                                    MAKE_TX_EQ_LIST_COMPACT_DATA_DW0(EqDw0, EqDw1),           \
                                                                    MAKE_TX_EQ_LIST_COMPACT_DATA_DW1(EqDw1, EqDw2, EqDw3),    \
                                                                    MAKE_TX_EQ_LIST_COMPACT_DATA_DW2(EqDw3, EqDw4, EqDw5),    \
                                                                    MAKE_TX_EQ_LIST_COMPACT_DATA_DW3(EqDw5, EqDw6, EqDw7),    \
                                                                    MAKE_TX_EQ_LIST_COMPACT_DATA_DW4(EqDw7, EqDw8),           \
                                                                    MAKE_TX_EQ_LIST_COMPACT_DATA_DW5(EqDw8, EqDw9, EqDw10),   \
                                                                    MAKE_TX_EQ_LIST_COMPACT_DATA_DW6(EqDw10, EqDw11, EqDw12), \
                                                                    MAKE_TX_EQ_LIST_COMPACT_DATA_DW7(EqDw12, EqDw13, EqDw14), \
                                                                    MAKE_TX_EQ_LIST_COMPACT_DATA_DW8(EqDw14, EqDw15)


// Termination node of XGMI TX EQ setting of frequency table
#define MAKE_TX_EQ_FREQ_END()                           ((UINT32) (((XGMI_TX_EQ_NODE_TYPE_FREQ_END & XGMI_TX_EQ_COMMON_NODE_TYPE_SIZE_MASK) << XGMI_TX_EQ_COMMON_NODE_TYPE_SHIFT)))

// Termination node of XGMI TX EQ setting table
#define MAKE_TX_EQ_END()                                ((UINT32) (((XGMI_TX_EQ_NODE_TYPE_END & XGMI_TX_EQ_COMMON_NODE_TYPE_SIZE_MASK) << XGMI_TX_EQ_COMMON_NODE_TYPE_SHIFT)))


// XGMI TX EQ union
typedef union _XGMI_TX_EQ_UNION {
  UINT32                  Raw;                  ///< Raw data
  XGMI_TX_EQ_COMMON       Common;               ///< Common structure
  XGMI_TX_EQ_FREQ         Freq;                 ///< Frequency structure
  XGMI_TX_EQ_ATTR         Attr;                 ///< Attribute structure
  XGMI_TX_EQ_DATA         Data;                 ///< Data structure
} XGMI_TX_EQ_UNION;

//
// @MACRO
//
// Macro for all system lanes
#define MAKE_TX_EQ_ALL_SYS_LANES()                      MAKE_TX_EQ_ATTR(XGMI_ALL_SOCKETS, XGMI_ALL_DIES, XGMI_ALL_LANES, XGMI_TX_EQ_DATA_MODE_PAIR)
#define MAKE_TX_EQ_EACH_SYS_LANES()                     MAKE_TX_EQ_ATTR(XGMI_ALL_SOCKETS, XGMI_ALL_DIES, XGMI_ALL_LANES, XGMI_TX_EQ_DATA_MODE_LIST)
// Macros for socket 0
#define MAKE_TX_EQ_SOCKET0_DIE_LANE(DieMask, LaneMask, DataMode)    MAKE_TX_EQ_ATTR(XGMI_SOCKET0, DieMask, LaneMask, DataMode)
#define MAKE_TX_EQ_SOCKET0_DIE_LANE_PAIR(DieMask, LaneMask)         MAKE_TX_EQ_ATTR(XGMI_SOCKET0, DieMask, LaneMask, XGMI_TX_EQ_DATA_MODE_PAIR)
#define MAKE_TX_EQ_SOCKET0_DIE_LANE_LIST(DieMask, LaneMask)         MAKE_TX_EQ_ATTR(XGMI_SOCKET0, DieMask, LaneMask, XGMI_TX_EQ_DATA_MODE_LIST)
#define MAKE_TX_EQ_SOCKET0_DIE0_ALL_LANES()             MAKE_TX_EQ_ATTR(XGMI_SOCKET0, XGMI_DIE0, XGMI_ALL_LANES, XGMI_TX_EQ_DATA_MODE_PAIR)
#define MAKE_TX_EQ_SOCKET0_DIE1_ALL_LANES()             MAKE_TX_EQ_ATTR(XGMI_SOCKET0, XGMI_DIE1, XGMI_ALL_LANES, XGMI_TX_EQ_DATA_MODE_PAIR)
#define MAKE_TX_EQ_SOCKET0_DIE2_ALL_LANES()             MAKE_TX_EQ_ATTR(XGMI_SOCKET0, XGMI_DIE2, XGMI_ALL_LANES, XGMI_TX_EQ_DATA_MODE_PAIR)
#define MAKE_TX_EQ_SOCKET0_DIE3_ALL_LANES()             MAKE_TX_EQ_ATTR(XGMI_SOCKET0, XGMI_DIE3, XGMI_ALL_LANES, XGMI_TX_EQ_DATA_MODE_PAIR)
#define MAKE_TX_EQ_SOCKET0_DIE4_ALL_LANES()             MAKE_TX_EQ_ATTR(XGMI_SOCKET0, XGMI_DIE4, XGMI_ALL_LANES, XGMI_TX_EQ_DATA_MODE_PAIR)
#define MAKE_TX_EQ_SOCKET0_DIE5_ALL_LANES()             MAKE_TX_EQ_ATTR(XGMI_SOCKET0, XGMI_DIE5, XGMI_ALL_LANES, XGMI_TX_EQ_DATA_MODE_PAIR)
#define MAKE_TX_EQ_SOCKET0_DIE6_ALL_LANES()             MAKE_TX_EQ_ATTR(XGMI_SOCKET0, XGMI_DIE6, XGMI_ALL_LANES, XGMI_TX_EQ_DATA_MODE_PAIR)
#define MAKE_TX_EQ_SOCKET0_DIE7_ALL_LANES()             MAKE_TX_EQ_ATTR(XGMI_SOCKET0, XGMI_DIE7, XGMI_ALL_LANES, XGMI_TX_EQ_DATA_MODE_PAIR)
#define MAKE_TX_EQ_SOCKET0_DIE0_EACH_LANES()             MAKE_TX_EQ_ATTR(XGMI_SOCKET0, XGMI_DIE0, XGMI_ALL_LANES, XGMI_TX_EQ_DATA_MODE_LIST)
#define MAKE_TX_EQ_SOCKET0_DIE1_EACH_LANES()             MAKE_TX_EQ_ATTR(XGMI_SOCKET0, XGMI_DIE1, XGMI_ALL_LANES, XGMI_TX_EQ_DATA_MODE_LIST)
#define MAKE_TX_EQ_SOCKET0_DIE2_EACH_LANES()             MAKE_TX_EQ_ATTR(XGMI_SOCKET0, XGMI_DIE2, XGMI_ALL_LANES, XGMI_TX_EQ_DATA_MODE_LIST)
#define MAKE_TX_EQ_SOCKET0_DIE3_EACH_LANES()             MAKE_TX_EQ_ATTR(XGMI_SOCKET0, XGMI_DIE3, XGMI_ALL_LANES, XGMI_TX_EQ_DATA_MODE_LIST)
#define MAKE_TX_EQ_SOCKET0_DIE4_EACH_LANES()             MAKE_TX_EQ_ATTR(XGMI_SOCKET0, XGMI_DIE4, XGMI_ALL_LANES, XGMI_TX_EQ_DATA_MODE_LIST)
#define MAKE_TX_EQ_SOCKET0_DIE5_EACH_LANES()             MAKE_TX_EQ_ATTR(XGMI_SOCKET0, XGMI_DIE5, XGMI_ALL_LANES, XGMI_TX_EQ_DATA_MODE_LIST)
#define MAKE_TX_EQ_SOCKET0_DIE6_EACH_LANES()             MAKE_TX_EQ_ATTR(XGMI_SOCKET0, XGMI_DIE6, XGMI_ALL_LANES, XGMI_TX_EQ_DATA_MODE_LIST)
#define MAKE_TX_EQ_SOCKET0_DIE7_EACH_LANES()             MAKE_TX_EQ_ATTR(XGMI_SOCKET0, XGMI_DIE7, XGMI_ALL_LANES, XGMI_TX_EQ_DATA_MODE_LIST)
#define MAKE_TX_EQ_SOCKET0_DIE0_EACH_LANES_COMPACT()             MAKE_TX_EQ_ATTR(XGMI_SOCKET0, XGMI_DIE0, XGMI_ALL_LANES, XGMI_TX_EQ_DATA_MODE_LIST_COMPACT)
#define MAKE_TX_EQ_SOCKET0_DIE1_EACH_LANES_COMPACT()             MAKE_TX_EQ_ATTR(XGMI_SOCKET0, XGMI_DIE1, XGMI_ALL_LANES, XGMI_TX_EQ_DATA_MODE_LIST_COMPACT)
#define MAKE_TX_EQ_SOCKET0_DIE2_EACH_LANES_COMPACT()             MAKE_TX_EQ_ATTR(XGMI_SOCKET0, XGMI_DIE2, XGMI_ALL_LANES, XGMI_TX_EQ_DATA_MODE_LIST_COMPACT)
#define MAKE_TX_EQ_SOCKET0_DIE3_EACH_LANES_COMPACT()             MAKE_TX_EQ_ATTR(XGMI_SOCKET0, XGMI_DIE3, XGMI_ALL_LANES, XGMI_TX_EQ_DATA_MODE_LIST_COMPACT)
#define MAKE_TX_EQ_SOCKET0_DIE4_EACH_LANES_COMPACT()             MAKE_TX_EQ_ATTR(XGMI_SOCKET0, XGMI_DIE4, XGMI_ALL_LANES, XGMI_TX_EQ_DATA_MODE_LIST_COMPACT)
#define MAKE_TX_EQ_SOCKET0_DIE5_EACH_LANES_COMPACT()             MAKE_TX_EQ_ATTR(XGMI_SOCKET0, XGMI_DIE5, XGMI_ALL_LANES, XGMI_TX_EQ_DATA_MODE_LIST_COMPACT)
#define MAKE_TX_EQ_SOCKET0_DIE6_EACH_LANES_COMPACT()             MAKE_TX_EQ_ATTR(XGMI_SOCKET0, XGMI_DIE6, XGMI_ALL_LANES, XGMI_TX_EQ_DATA_MODE_LIST_COMPACT)
#define MAKE_TX_EQ_SOCKET0_DIE7_EACH_LANES_COMPACT()             MAKE_TX_EQ_ATTR(XGMI_SOCKET0, XGMI_DIE7, XGMI_ALL_LANES, XGMI_TX_EQ_DATA_MODE_LIST_COMPACT)

// Macros for socket 1
#define MAKE_TX_EQ_SOCKET1_DIE_LANE(DieMask, LaneMask, DataMode)    MAKE_TX_EQ_ATTR(XGMI_SOCKET1, DieMask, LaneMask, DataMode)
#define MAKE_TX_EQ_SOCKET1_DIE_LANE_PAIR(DieMask, LaneMask)         MAKE_TX_EQ_ATTR(XGMI_SOCKET1, DieMask, LaneMask, XGMI_TX_EQ_DATA_MODE_PAIR)
#define MAKE_TX_EQ_SOCKET1_DIE_LANE_LIST(DieMask, LaneMask)         MAKE_TX_EQ_ATTR(XGMI_SOCKET1, DieMask, LaneMask, XGMI_TX_EQ_DATA_MODE_LIST)
#define MAKE_TX_EQ_SOCKET1_DIE0_ALL_LANES()             MAKE_TX_EQ_ATTR(XGMI_SOCKET1, XGMI_DIE0, XGMI_ALL_LANES, XGMI_TX_EQ_DATA_MODE_PAIR)
#define MAKE_TX_EQ_SOCKET1_DIE1_ALL_LANES()             MAKE_TX_EQ_ATTR(XGMI_SOCKET1, XGMI_DIE1, XGMI_ALL_LANES, XGMI_TX_EQ_DATA_MODE_PAIR)
#define MAKE_TX_EQ_SOCKET1_DIE2_ALL_LANES()             MAKE_TX_EQ_ATTR(XGMI_SOCKET1, XGMI_DIE2, XGMI_ALL_LANES, XGMI_TX_EQ_DATA_MODE_PAIR)
#define MAKE_TX_EQ_SOCKET1_DIE3_ALL_LANES()             MAKE_TX_EQ_ATTR(XGMI_SOCKET1, XGMI_DIE3, XGMI_ALL_LANES, XGMI_TX_EQ_DATA_MODE_PAIR)
#define MAKE_TX_EQ_SOCKET1_DIE4_ALL_LANES()             MAKE_TX_EQ_ATTR(XGMI_SOCKET1, XGMI_DIE4, XGMI_ALL_LANES, XGMI_TX_EQ_DATA_MODE_PAIR)
#define MAKE_TX_EQ_SOCKET1_DIE5_ALL_LANES()             MAKE_TX_EQ_ATTR(XGMI_SOCKET1, XGMI_DIE5, XGMI_ALL_LANES, XGMI_TX_EQ_DATA_MODE_PAIR)
#define MAKE_TX_EQ_SOCKET1_DIE6_ALL_LANES()             MAKE_TX_EQ_ATTR(XGMI_SOCKET1, XGMI_DIE6, XGMI_ALL_LANES, XGMI_TX_EQ_DATA_MODE_PAIR)
#define MAKE_TX_EQ_SOCKET1_DIE7_ALL_LANES()             MAKE_TX_EQ_ATTR(XGMI_SOCKET1, XGMI_DIE7, XGMI_ALL_LANES, XGMI_TX_EQ_DATA_MODE_PAIR)
#define MAKE_TX_EQ_SOCKET1_DIE0_EACH_LANES()             MAKE_TX_EQ_ATTR(XGMI_SOCKET1, XGMI_DIE0, XGMI_ALL_LANES, XGMI_TX_EQ_DATA_MODE_LIST)
#define MAKE_TX_EQ_SOCKET1_DIE1_EACH_LANES()             MAKE_TX_EQ_ATTR(XGMI_SOCKET1, XGMI_DIE1, XGMI_ALL_LANES, XGMI_TX_EQ_DATA_MODE_LIST)
#define MAKE_TX_EQ_SOCKET1_DIE2_EACH_LANES()             MAKE_TX_EQ_ATTR(XGMI_SOCKET1, XGMI_DIE2, XGMI_ALL_LANES, XGMI_TX_EQ_DATA_MODE_LIST)
#define MAKE_TX_EQ_SOCKET1_DIE3_EACH_LANES()             MAKE_TX_EQ_ATTR(XGMI_SOCKET1, XGMI_DIE3, XGMI_ALL_LANES, XGMI_TX_EQ_DATA_MODE_LIST)
#define MAKE_TX_EQ_SOCKET1_DIE4_EACH_LANES()             MAKE_TX_EQ_ATTR(XGMI_SOCKET1, XGMI_DIE4, XGMI_ALL_LANES, XGMI_TX_EQ_DATA_MODE_LIST)
#define MAKE_TX_EQ_SOCKET1_DIE5_EACH_LANES()             MAKE_TX_EQ_ATTR(XGMI_SOCKET1, XGMI_DIE5, XGMI_ALL_LANES, XGMI_TX_EQ_DATA_MODE_LIST)
#define MAKE_TX_EQ_SOCKET1_DIE6_EACH_LANES()             MAKE_TX_EQ_ATTR(XGMI_SOCKET1, XGMI_DIE6, XGMI_ALL_LANES, XGMI_TX_EQ_DATA_MODE_LIST)
#define MAKE_TX_EQ_SOCKET1_DIE7_EACH_LANES()             MAKE_TX_EQ_ATTR(XGMI_SOCKET1, XGMI_DIE7, XGMI_ALL_LANES, XGMI_TX_EQ_DATA_MODE_LIST)
#define MAKE_TX_EQ_SOCKET1_DIE0_EACH_LANES_COMPACT()             MAKE_TX_EQ_ATTR(XGMI_SOCKET1, XGMI_DIE0, XGMI_ALL_LANES, XGMI_TX_EQ_DATA_MODE_LIST_COMPACT)
#define MAKE_TX_EQ_SOCKET1_DIE1_EACH_LANES_COMPACT()             MAKE_TX_EQ_ATTR(XGMI_SOCKET1, XGMI_DIE1, XGMI_ALL_LANES, XGMI_TX_EQ_DATA_MODE_LIST_COMPACT)
#define MAKE_TX_EQ_SOCKET1_DIE2_EACH_LANES_COMPACT()             MAKE_TX_EQ_ATTR(XGMI_SOCKET1, XGMI_DIE2, XGMI_ALL_LANES, XGMI_TX_EQ_DATA_MODE_LIST_COMPACT)
#define MAKE_TX_EQ_SOCKET1_DIE3_EACH_LANES_COMPACT()             MAKE_TX_EQ_ATTR(XGMI_SOCKET1, XGMI_DIE3, XGMI_ALL_LANES, XGMI_TX_EQ_DATA_MODE_LIST_COMPACT)
#define MAKE_TX_EQ_SOCKET1_DIE4_EACH_LANES_COMPACT()             MAKE_TX_EQ_ATTR(XGMI_SOCKET1, XGMI_DIE4, XGMI_ALL_LANES, XGMI_TX_EQ_DATA_MODE_LIST_COMPACT)
#define MAKE_TX_EQ_SOCKET1_DIE5_EACH_LANES_COMPACT()             MAKE_TX_EQ_ATTR(XGMI_SOCKET1, XGMI_DIE5, XGMI_ALL_LANES, XGMI_TX_EQ_DATA_MODE_LIST_COMPACT)
#define MAKE_TX_EQ_SOCKET1_DIE6_EACH_LANES_COMPACT()             MAKE_TX_EQ_ATTR(XGMI_SOCKET1, XGMI_DIE6, XGMI_ALL_LANES, XGMI_TX_EQ_DATA_MODE_LIST_COMPACT)
#define MAKE_TX_EQ_SOCKET1_DIE7_EACH_LANES_COMPACT()             MAKE_TX_EQ_ATTR(XGMI_SOCKET1, XGMI_DIE7, XGMI_ALL_LANES, XGMI_TX_EQ_DATA_MODE_LIST_COMPACT)

#define MAKE_TX_EQ_PAIR(Attr, Data)                           Attr, Data
#define MAKE_TX_EQ_LIST(Attr, Data0, ...)                     Attr, Data0, __VA_ARGS__
#define MAKE_TX_EQ_LIST_COMPACT(Attr, Lane0, Lane1, Lane2, Lane3, Lane4, Lane5, Lane6, Lane7, Lane8, Lane9, Lane10, Lane11, Lane12, Lane13, Lane14, Lane15)  \
                                                              Attr, MAKE_TX_EQ_LIST_COMPACT_DATA(Lane0, Lane1, Lane2, Lane3, Lane4, Lane5, Lane6, Lane7, Lane8, Lane9, Lane10, Lane11, Lane12, Lane13, Lane14, Lane15)
#define MAKE_TX_EQ_FREQ_TABLE(SpeedMask, TxEqPairOrList, ...)  MAKE_TX_EQ_FREQ_TBL(SpeedMask), TxEqPairOrList, __VA_ARGS__, MAKE_TX_EQ_FREQ_END()

typedef struct _XGMI_TX_EQ_COMPACT_LANE_PER_DIE {
  union {
  UINT32                  AttrLoc;              ///< Must be XGMI_ALL_LANES, XGMI_TX_EQ_DATA_MODE_LIST_COMPACT
  XGMI_TX_EQ_ATTR         Attr;                 ///< Attribute structure
  } unionAttr;
  XGMI_TX_EQ_DATA_LIST_COMPACT    CompactListData;
} XGMI_TX_EQ_COMPACT_LANE_PER_DIE;

///< Total 8 socket die indice, no direct mapping
#define MAKE_TX_EQ_FREQ_TABLE_COMPACT(SpeedMask, SD0, SD1, SD2, SD3, SD4, SD5, SD6, SD7)  \
      MAKE_TX_EQ_FREQ_TBL(SpeedMask), SD0, SD1, SD2, SD3, SD4, SD5, SD6, SD7, MAKE_TX_EQ_FREQ_END()

#define MAKE_TX_EQ_LANE_DATA(EqMain, EqPost, EqPre)         MAKE_TX_EQ_LIST_COMPACT_DATA_SUB(EqMain, EqPost, EqPre)

/** APCB_DF_TYPE_XGMI_TX_EQ
*   @GROUPID: APCB_GROUP_DF
*   @TYPEID: APCB_DF_TYPE_XGMI_TX_EQ (0xD0)
*   @INSTANCEID: APCB_DF_TYPE_XGMI_INSTANCE_SPEED_COMBINED
*   N/A
*/
typedef struct {
  UINT32  XgmiTxEqEntry1st;
  UINT32  XgmiTxEqEntry[];
} XGMI_TX_EQ_SETTING;

#endif // _APCB_DF_XGMI_TX_EQ_H_

