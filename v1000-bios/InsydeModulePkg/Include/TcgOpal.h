/** @file
  TCG OPAL Definition

;******************************************************************************
;* Copyright (c) 2016, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _TCG_OPAL_H
#define _TCG_OPAL_H

#define L0_DISCOVERY_HEADER_SIZE 48

#define FC_TPER                  0x0001
#define FC_LOCKING               0x0002
#define FC_GEOMETRY              0x0003
#define FC_OPAL20                0x0203
#define FC_OPALITE               0x0301
#define FC_PYRITE                0x0302
#define FC_OPAL10                0x0200
#define FC_SINGLEUSER            0x0201
#define FC_DATASTORE             0x0202

typedef enum _OPAL_TOKEN {
  // Cell Blocks
  TABLE                   = 0x00,
  STARTROW                = 0x01,
  ENDROW                  = 0x02,
  STARTCOLUMN             = 0x03,
  ENDCOLUMN               = 0x04,
  VALUES                  = 0x01,
  // SP table
  LIFECYCLESTATE          = 0x06,
  // Authority Table
  PIN                     = 0x03,
  // Locking Table
  RANGESTART              = 0x03,
  RANGELENGTH             = 0x04,
  READLOCKENABLED         = 0x05,
  WRITELOCKENABLED        = 0x06,
  READLOCKED              = 0x07,
  WRITELOCKED             = 0x08,
  ACTIVEKEY               = 0x0A,
  LOCKONRESET             = 0x09,
  // Locking Info Table
  MAXRANGES               = 0x04,
  // MBR Control
  MBRENABLE               = 0x01,
  MBRDONE                 = 0x02,
  // properties
  HOSTPROPERTIES          = 0x00,
  // Atoms
  STARTLIST               = 0xF0,
  ENDLIST                 = 0xF1,
  STARTNAME               = 0xF2,
  ENDNAME                 = 0xF3,
  CALL                    = 0xF8,
  ENDOFDATA               = 0xF9,
  ENDOFSESSION            = 0xFA,
  STARTTRANSACTON         = 0xFB,
  ENDTRANSACTON           = 0xFC,
  WHERE                   = 0x00,
  TOKEN_MAX
} OPAL_TOKEN;

#pragma pack(1)
typedef struct _L0_DISCOVERY_HEADER{
  UINT32 Length;
  UINT32 Revision;
  UINT8  Reserved[8];
  UINT8  VendorUnique[32];
} L0_DISCOVERY_HEADER;

typedef struct _L0_DISCOVERY_DESCRIPTOR {
  UINT16 FeatureCode;
  UINT8  Reserved     : 4;
  UINT8  Version      : 4;
  UINT8  Length;
  //UINT8 Data[];
} L0_DISCOVERY_DESCRIPTOR;

typedef struct _L0_DISCOVERY_TPER_FEATURE {
  UINT16 FeatureCode; // 0x0001
  UINT8  Reserved0                 : 4;
  UINT8  Version                   : 4;
  UINT8  Length;
  UINT8  Sync                      : 1;
  UINT8  Async                     : 1;
  UINT8  AckNack                   : 1;
  UINT8  BufferManagement          : 1;
  UINT8  Streaming                 : 1;
  UINT8  Reserved1                 : 1;
  UINT8  ComIDManagement           : 1;
  UINT8  Reserved2                 : 1;
  UINT8  Reserved3[11];
} L0_DISCOVERY_TPER_FEATURE;

typedef struct _L0_DISCOVERY_LOCKING_FEATURE {
  UINT16 FeatureCode; // 0x0002
  UINT8  Reserved0                 : 4;
  UINT8  Version                   : 4;
  UINT8  Length;
  UINT8  LockingSupported          : 1;
  UINT8  LockingEnabled            : 1;
  UINT8  Locked                    : 1;
  UINT8  MediaEncryption           : 1;
  UINT8  MBREnabled                : 1;
  UINT8  MBRDone                   : 1;
  UINT8  Reserved1                 : 1;
  UINT8  Reserved2                 : 1;
  UINT8  Reserved3[11];
} L0_DISCOVERY_LOCKING_FEATURE;

typedef struct _L0_DISCOVERY_GEOMETRY_REPORTING_FEATURE {
  UINT16 FeatureCode; // 0x0003
  UINT8  Reserved0                 : 4;
  UINT8  Version                   : 4;
  UINT8  Length;
  UINT8  Align                     : 1;
  UINT8  Reserved1                 : 7;
  UINT8  Reserved2[7];
  UINT32 LogicalBlockSize;
  UINT64 AlignmentGranularity;
  UINT64 LowestAlighedLBA;
} L0_DISCOVERY_GEOMETRY_REPORTING_FEATURE;

typedef struct _L0_DISCOVERY_OPALSSC_V200_FEATURE {
  UINT16 FeatureCode; // 0x0203
  UINT8  Reserved0                 : 4;
  UINT8  Version                   : 4;
  UINT8  Length;
  UINT16 BaseComID;
  UINT16 NumOfComIDs;
  UINT8  RangeCrossing             : 1;
  UINT8  Reserved1                 : 7;
  UINT16 NumOfAdmin;
  UINT16 NumOfUser;
  UINT8  InitialPIN;
  UINT8  RevertedPIN;
  UINT8  Reserved2[5];
} L0_DISCOVERY_OPALSSC_V200_FEATURE;

typedef struct _L0_DISCOVERY_OPALITESSC_FEATURE {
  UINT16 FeatureCode; // 0x0301
  UINT8  Reserved0                 : 4;
  UINT8  Version                   : 4;
  UINT8  Length;
  UINT16 BaseComID;
  UINT16 NumOfComIDs;
  UINT8  Reserved1[5];
  UINT8  InitialPIN;
  UINT8  RevertedPIN;
  UINT8  Reserved2[5];
} L0_DISCOVERY_OPALITESSC_FEATURE;

typedef struct _L0_DISCOVERY_PYRITESSC_FEATURE {
  UINT16 FeatureCode; // 0x0302
  UINT8  Reserved0                 : 4;
  UINT8  Version                   : 4;
  UINT8  Length;
  UINT16 BaseComID;
  UINT16 NumOfComIDs;
  UINT8  Reserved1[5];
  UINT8  InitialPIN;
  UINT8  RevertedPIN;
  UINT8  Reserved2[5];
} L0_DISCOVERY_PYRITESSC_FEATURE;

typedef struct _L0_DISCOVERY_OPALSSC_V100_FEATURE {
  UINT16 FeatureCode; // 0x0200
  UINT8  Reserved0                 : 4;
  UINT8  Version                   : 4;
  UINT8  Length;
  UINT16 BaseComID;
  UINT16 NumOfComIDs;
  UINT8  RangeCrossing             : 1;
  UINT8  Reserved1                 : 7;
  UINT8  Reserved2[11];
} L0_DISCOVERY_OPALSSC_V100_FEATURE;

typedef struct _L0_DISCOVERY_SINGLE_USER_MODE_FEATURE {
  UINT16 FeatureCode; // 0x0201
  UINT8  Reserved0                 : 4;
  UINT8  Version                   : 4;
  UINT8  Length;
  UINT32 NumberOfLockingObjects;
  UINT8  Any                       : 1;
  UINT8  All                       : 1;
  UINT8  Policy                    : 1;
  UINT8  Reserved1                 : 5;
  UINT8  Reserved2[7];
} L0_DISCOVERY_SINGLE_USER_MODE_FEATURE;

typedef struct _L0_DISCOVERY_DATASTORE_TABLE_FEATURE {
  UINT16 FeatureCode; // 0x0202
  UINT8  Reserved0                 : 4;
  UINT8  Version                   : 4;
  UINT8  Length;
  UINT8  Reserved1[2];
  UINT16 MaxTables;
  UINT32 MaxSizeTables;
  UINT32 TableSizeAlignment;
} L0_DISCOVERY_DATASTORE_TABLE_FEATURE;

typedef union _L0_DISCOVERY_FEATURES {
  L0_DISCOVERY_TPER_FEATURE                 TPer;
  L0_DISCOVERY_LOCKING_FEATURE              Locking;
  L0_DISCOVERY_GEOMETRY_REPORTING_FEATURE   Geometry;
  L0_DISCOVERY_OPALSSC_V200_FEATURE         Opal20;
  L0_DISCOVERY_OPALITESSC_FEATURE           Opalite;
  L0_DISCOVERY_PYRITESSC_FEATURE            Pyrite;
  L0_DISCOVERY_OPALSSC_V100_FEATURE         Opal10;
  L0_DISCOVERY_SINGLE_USER_MODE_FEATURE     SingleUser;
  L0_DISCOVERY_DATASTORE_TABLE_FEATURE      DataStore;
} L0_DISCOVERY_FEATURES;

typedef struct {
  UINT8  Reserved[4];
  UINT8  ExtComID[4];
  UINT32 OutstandingData;
  UINT32 MinTransfer;
  UINT32 Length;
  //UINT8 Payload[];
} COMPACKET_HEADER;

typedef struct {
  UINT32 TPerSN;
  UINT32 HostSN;
  UINT32 SeqNumber;
  UINT8  Reserved[2];
  UINT16 AckType;
  UINT32 Ack;
  UINT32 Length;
  //UINT8 Payload[];
} PACKET_HEADER;

typedef struct {
  UINT8  Reserved[6];
  UINT16 Kind;
  UINT32 Length;
  //UINT8 Payload[];
} SUBPACKET_HEADER;

typedef struct {
  COMPACKET_HEADER ComPacket;
  PACKET_HEADER    Packet;
  SUBPACKET_HEADER SubPacket;
} OPAL_HEADER;
#pragma pack()
#endif //_TCG_OPAL_H_

