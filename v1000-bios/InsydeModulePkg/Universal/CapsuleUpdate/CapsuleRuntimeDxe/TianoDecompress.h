/** @file

;******************************************************************************
;* Copyright (c) 2013 - 2015, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

/** @file
  Internal data structure and interfaces defintions for UEFI and Tiano Decompress Libary.

  Copyright (c) 2006 - 2009, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef __TIANO_DECOMPRESS_H__
#define __TIANO_DECOMPRESS_H__

#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>

//
// Decompression algorithm begins here
//
#define BITBUFSIZ 32
#define MAXMATCH  256
#define THRESHOLD 3
#define CODE_BIT  16
#define BAD_TABLE - 1

//
// C: Char&Len Set; P: Position Set; T: exTra Set
//
#define NC      (0xff + MAXMATCH + 2 - THRESHOLD)
#define CBIT    9
#define MAXPBIT 5
#define TBIT    5
#define MAXNP   ((1U << MAXPBIT) - 1)
#define NT      (CODE_BIT + 3)
#if NT > MAXNP
#define NPT NT
#else
#define NPT MAXNP
#endif

typedef struct {
  UINT8   *mSrcBase;  // Starting address of compressed data
  UINT8   *mDstBase;  // Starting address of decompressed data
  UINT32  mOutBuf;
  UINT32  mInBuf;

  UINT16  mBitCount;
  UINT32  mBitBuf;
  UINT32  mSubBitBuf;
  UINT16  mBlockSize;
  UINT32  mCompSize;
  UINT32  mOrigSize;

  UINT16  mBadTableFlag;

  UINT16  mLeft[2 * NC - 1];
  UINT16  mRight[2 * NC - 1];
  UINT8   mCLen[NC];
  UINT8   mPTLen[NPT];
  UINT16  mCTable[4096];
  UINT16  mPTTable[256];

  ///
  /// The length of the field 'Position Set Code Length Array Size' in Block Header.
  /// For UEFI 2.0 de/compression algorithm, mPBit = 4
  /// For Tiano de/compression algorithm, mPBit = 5
  ///
  UINT8   mPBit;
} SCRATCH_DATA;

#pragma pack(1)
typedef struct {
  UINT32                        SourceSize;
  UINT32                        CompressedSize;
} COMPRESS_DATA_HEADER;

typedef struct {
  UINT32                        NumOfBlock;
  UINT32                        TotalImageSize;
  UINT32                        TotalCompressedDataSize;
} COMPRESS_TABLE_HEADER;
#pragma pack()

/**
  Read NumOfBit of bits from source into mBitBuf.

  Shift mBitBuf NumOfBits left. Read in NumOfBits of bits from source.

  @param  Sd        The global scratch data
  @param  NumOfBits The number of bits to shift and read.

**/
VOID
FillBuf (
  IN  SCRATCH_DATA  *Sd,
  IN  UINT16        NumOfBits
  );

/**
  Get NumOfBits of bits out from mBitBuf.

  Get NumOfBits of bits out from mBitBuf. Fill mBitBuf with subsequent
  NumOfBits of bits from source. Returns NumOfBits of bits that are
  popped out.

  @param  Sd        The global scratch data.
  @param  NumOfBits The number of bits to pop and read.

  @return The bits that are popped out.

**/
UINT32
GetBits (
  IN  SCRATCH_DATA  *Sd,
  IN  UINT16        NumOfBits
  );

/**
  Creates Huffman Code mapping table according to code length array.

  Creates Huffman Code mapping table for Extra Set, Char&Len Set
  and Position Set according to code length array.

  @param  Sd        The global scratch data
  @param  NumOfChar Number of symbols in the symbol set
  @param  BitLen    Code length array
  @param  TableBits The width of the mapping table
  @param  Table     The table to be created.

  @retval  0 OK.
  @retval  BAD_TABLE The table is corrupted.

**/
UINT16
MakeTable (
  IN  SCRATCH_DATA  *Sd,
  IN  UINT16        NumOfChar,
  IN  UINT8         *BitLen,
  IN  UINT16        TableBits,
  OUT UINT16        *Table
  );

/**
  Decodes a position value.

  Get a position value according to Position Huffman Table.

  @param  Sd the global scratch data

  @return The position value decoded.

**/
UINT32
DecodeP (
  IN  SCRATCH_DATA  *Sd
  );

/**
  Reads code lengths for the Extra Set or the Position Set.

  Read in the Extra Set or Pointion Set Length Arrary, then
  generate the Huffman code mapping for them.

  @param  Sd      The global scratch data.
  @param  nn      Number of symbols.
  @param  nbit    Number of bits needed to represent nn.
  @param  Special The special symbol that needs to be taken care of.

  @retval  0 OK.
  @retval  BAD_TABLE Table is corrupted.

**/
UINT16
ReadPTLen (
  IN  SCRATCH_DATA  *Sd,
  IN  UINT16        nn,
  IN  UINT16        nbit,
  IN  UINT16        Special
  );

/**
  Reads code lengths for Char&Len Set.

  Read in and decode the Char&Len Set Code Length Array, then
  generate the Huffman Code mapping table for the Char&Len Set.

  @param  Sd the global scratch data

**/
VOID
ReadCLen (
  SCRATCH_DATA  *Sd
  );

/**
  Decode a character/length value.

  Read one value from mBitBuf, Get one code from mBitBuf. If it is at block boundary, generates
  Huffman code mapping table for Extra Set, Code&Len Set and
  Position Set.

  @param  Sd The global scratch data.

  @return The value decoded.

**/
UINT16
DecodeC (
  SCRATCH_DATA  *Sd
  );

/**
  Decode the source data and put the resulting data into the destination buffer.

  @param  Sd The global scratch data

**/
VOID
Decode (
  SCRATCH_DATA  *Sd
  );

EFI_STATUS
EFIAPI
TianoDecompress (
  IN VOID        *Source,
  IN OUT VOID    *Destination
  );

#endif
