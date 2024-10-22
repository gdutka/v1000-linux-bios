/* $NoKeywords:$ */
/**
 * @file
 *
 * ApcbV2.h
 *
 * AGESA PSP Customization Block Data Definition
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: ApcbToolV2
 * @e \$Revision: $ @e \$Date: $
 *
 **/
/*****************************************************************************
*
* Copyright (C) 2015-2022 Advanced Micro Devices, Inc. All rights reserved.
*
*******************************************************************************
*
*/

#ifndef _APCB_V2_H_
#define _APCB_V2_H_


#pragma pack(push, 1)

#define APCB_HEADER_VERSION_MAJOR   2
#define APCB_HEADER_VERSION_MINOR   0
//#define APCB_HEADER_VERSION         ((APCB_HEADER_VERSION_MAJOR << 4) | APCB_HEADER_VERSION_MINOR)
/*
//
// APCB Header Definition Version 2
//
typedef struct {
  UINT32  Signature;           // ASCII "APCB", 'A' is LSB
  UINT16  SizeOfHeader;        // Size of header
  UINT16  Version;             // Version, BCD. Version 1.2 is 0x12
  UINT32  SizeOfApcb;          // Size of APCB
  UINT32  UniqueApcbInstance;  // UniuweApcbInstance to ensure
                               // compatibitly for giveen flshed BIOS lifecycle
  UINT8   CheckSumByte;        // APCB Checksum Byte
  UINT8   Reserved1[3];        // Reserved
  UINT32  Reserved2[3];        // Reserved
} APCB_HEADER;


#define APCB_INDEX_HEADER_VERSION_MAJOR   0
#define APCB_INDEX_HEADER_VERSION_MINOR   1
#define APCB_INDEX_HEADER_VERSION         ((APCB_INDEX_HEADER_VERSION_MAJOR << 4) | APCB_INDEX_HEADER_VERSION_MINOR)
//
// APCB Index of Data Type Header Definition
//
typedef struct {
  UINT16  SizeOfHeader;     // Size of header
  UINT8   Version;          // Version, BCD. Version 1.2 is 0x12
  UINT8   Reserved;         // Reserved
  UINT32  SizeOfIndexData;  // Size of index data, absolute address in PSP
                            // NOTE: Will we support it to be relative address to starging address of APCB?
} APCB_INDEX_HEADER;

//
// APCB Index of Data Type  Definition
//
typedef struct {
  UINT16  GroupId;           // Group ID
  UINT16  TypeId;          // Type ID
  UINT32  Address;          // Data address
} APCB_TYPE_INDEX;

//
// APCB Data Type Header Definition
//
typedef struct {
  UINT16  GroupId;           // Group ID
  UINT16  TypeId;            // Type ID
  UINT16  SizeOfType;       // Size of type, in bytes
  UINT16  InstanceId;       // Instance ID
	UINT32	Reserved1;
	UINT32	Reserved2;
} APCB_TYPE_HEADER;

#define ALIGN_SIZE_OF_TYPE    4


#define APCB_GROUP_HEADER_VERSION_MAJOR   0
#define APCB_GROUP_HEADER_VERSION_MINOR   1
#define APCB_GROUP_HEADER_VERSION         ((APCB_GROUP_HEADER_VERSION_MAJOR << 4) | APCB_GROUP_HEADER_VERSION_MINOR)

#define INVALID_GROUP_ID    0xFFFF
#define UNKNOWN_GROUP_ID    0xFFFE

//
// APCB Data Group Header Definition
//
typedef struct {
  UINT32  Signature;        // ASCII Signature
  UINT16  GroupId;          // Group ID
  UINT16  SizeOfHeader;     // Size of header
  UINT16  Version;          // Version, BCD. Version 1.2 is 0x12
  UINT16  Reserved;      // Reserved
  UINT32  SizeOfGroup;      // Size of group
} APCB_GROUP_HEADER;

typedef struct {
  UINT32  Signature;        // ASCII Signature
  UINT16  GroupId;          // Group ID
} APCB_GROUP_ID_SIGN;

//
// APCB Layout Definition
//
typedef struct {
  APCB_HEADER          Header;
  APCB_INDEX_HEADER    Index;
  APCB_GROUP_HEADER    Group;
  APCB_TYPE_HEADER     Type;
  VOID                 *Data;
} APCB_LAYOUT;
*/
#pragma pack(pop)

#endif // _APCB_V2_H_

