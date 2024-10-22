/**
 * @file
 *
 * AMD Integrated Debug Hook Routines
 *
 * Contains all functions related to IDS Hook
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  IDS
 * @e \$Revision: 309090 $   @e \$Date: 2014-12-10 02:28:05 +0800 (Wed, 10 Dec 2014) $
 */
/*****************************************************************************
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */
#ifndef _AMD_IDS_NV_TABLE_H_
#define _AMD_IDS_NV_TABLE_H_

#pragma pack(push, 1)

#define IDS_NV_TABLE_SIGNATURE    0x564E4924ul      ///< $INV
#define IDS_NV_TABLE_REV_1        0x00000001ul
#define IDS_NV_TABLE_HEADER_REV1_DATA \
{\
    IDS_NV_TABLE_SIGNATURE, \
    IDS_NV_TABLE_REV_1, \
}
/// IDS NV table header
typedef struct _IDS_NV_TABLE_HEADER {
  UINT32 Signature;   ///< Signature
  UINT32 Revision;    ///< Revision
  UINT8  Reserved[24];    ///< Rsvd
} IDS_NV_TABLE_HEADER;

typedef UINT16 IDS_NV_ID;
#define IDS_NV_ATTRIB_SIZE_BYTE     0     ///< byte
#define IDS_NV_ATTRIB_SIZE_WORD     1     ///< word
#define IDS_NV_ATTRIB_SIZE_DWORD    2     ///< dword
#define IDS_NV_ATTRIB_SIZE_QWORD    3     ///< qword

/// IDS NV attribute
typedef struct _IDS_NV_ATTRIB {
  UINT8 size:2;   ///< Size of a NV record
  UINT8 reserved:6; ///< reserved
} IDS_NV_ATTRIB;

#define IDS_NV_ID_END  0xfffful
#define IDS_NV_VALUE_END 0xfffffffful
#define IDS_NV_RECORD_END \
{\
    IDS_NV_ID_END, \
    IDS_NV_VALUE_END\
}
/// IDS NV record generic
typedef struct  _IDS_NV_RECORD_CMN {
  IDS_NV_ID     Id;    ///< IDS NV ID
  IDS_NV_ATTRIB Attrib; ///< IDS NV Attribute
} IDS_NV_RECORD_CMN;

/// IDS NV record for 8 bits
typedef struct  IDS_NV_RECORD_U8 {
  IDS_NV_ID     Id;    ///< IDS NV ID
  IDS_NV_ATTRIB Attrib; ///< IDS NV Attribute
  UINT8         Value; ///< Value
} IDS_NV_RECORD_U8;

/// IDS NV record for 16 bits
typedef struct  IDS_NV_RECORD_U16 {
  IDS_NV_ID     Id;    ///< IDS NV ID
  IDS_NV_ATTRIB Attrib; ///< IDS NV Attribute
  UINT16        Value; ///< Value
} IDS_NV_RECORD_U16;

/// IDS NV record for 32 bits
typedef struct  _IDS_NV_RECORD_U32 {
  IDS_NV_ID     Id;    ///< IDS NV ID
  IDS_NV_ATTRIB Attrib; ///< IDS NV Attribute
  UINT32        Value; ///< Value
} IDS_NV_RECORD_U32;

/// IDS NV record for 64 bits
typedef struct  _IDS_NV_RECORD_U64 {
  IDS_NV_ID     Id;    ///< IDS NV ID
  IDS_NV_ATTRIB Attrib; ///< IDS NV Attribute
  UINT64        Value; ///< Value
} IDS_NV_RECORD_U64;

/// IDS NV table
typedef struct  _IDS_NV_TABLE {
  IDS_NV_TABLE_HEADER Header;   ///< Header
  UINT8         NvRecords;    ///< Data start
} IDS_NV_TABLE;

#pragma pack(pop)

#endif //#define _AMD_IDS_NV_TABLE_H_

