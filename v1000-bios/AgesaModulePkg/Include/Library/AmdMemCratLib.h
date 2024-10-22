/* $NoKeywords:$ */
/**
 * @file
 *
 * mfCrat.h
 *
 * Feature CRAT table support
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem)
 * @e \$Revision: 309090 $ @e \$Date: 2014-12-10 02:28:05 +0800 (Wed, 10 Dec 2014) $
 *
 **/
/*****************************************************************************
  *
  * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
  *
  *******************************************************************************
  *
 */


#ifndef _MFCRAT_H_
#define _MFCRAT_H_

/*----------------------------------------------------------------------------
 *   Mixed (DEFINITIONS AND MACROS / TYPEDEFS, STRUCTURES, ENUMS)
 *
 *----------------------------------------------------------------------------
 */

/*-----------------------------------------------------------------------------
 *                         DEFINITIONS AND MACROS
 *
 *-----------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------
 *                         TYPEDEFS, STRUCTURES, ENUMS
 *
 *----------------------------------------------------------------------------
 */

/// DCT Select Interleaving Address Mapping
typedef struct {
  UINT8                 DctSelIntLvAddr;        ///< F2x110 DctSelIntLvAddr
  UINT8                 DctSelFunctionBit;      ///< Lowest function bit to select DCT for interleaving
} DCTSELBIT_ENTRY;

/// CRAT Memory Affinity Info Header
typedef struct {
  UINT8                 NumOfMemAffinityInfoEntries;  ///< Integer that represents the proximity domain to
  UINT32                MemoryWidth;                  ///< Specifies the number of parallel bits of the memory interface
} CRAT_MEMORY_AFFINITY_INFO_HEADER;

/// CRAT Memory Affinity Info Entry
typedef struct {
  UINT8                 Domain;                 ///< Integer that represents the proximity domain to
                                                ///< which the memory belongs
  UINT32                BaseAddressLow;         ///< Low 32Bits of the Base Address of the memory range
  UINT32                BaseAddressHigh;        ///< High 32Bits of the Base Address of the memory range
  UINT32                LengthLow;              ///< Low 32Bits of the length of the memory range
  UINT32                LengthHigh;             ///< High 32Bits of the length of the memory range
} CRAT_MEMORY_AFFINITY_INFO_ENTRY;

CRAT_MEMORY_AFFINITY_INFO_ENTRY *
MakeMemAffinityInfoEntry (
  IN UINT8                            Domain,
  IN UINT64                           Base,
  IN UINT64                           Size,
  IN CRAT_MEMORY_AFFINITY_INFO_ENTRY  *BufferLocPtr
);

UINT32
MemFCratApproximateSize (
  IN OUT   MEM_NB_BLOCK        *NBPtr
  );

UINT64
GetDctInterleavedMemSize (
  IN OUT   MEM_NB_BLOCK        *NBPtr
  );

UINT64
GetBankInterleavedAddress (
  IN OUT   MEM_NB_BLOCK        *NBPtr,
  IN       UINT64              DctBaseAddress,
  IN       UINT8               InterleavingPosition
  );

UINT64
GetChannelInterleavedAddress (
  IN OUT   MEM_NB_BLOCK        *NBPtr,
  IN       UINT64              NodeBaseAddress,
  IN       UINT8               InterleavingPosition
  );

UINT64
GetBaseAddrOfNonInterleavedRegion (
  IN OUT   MEM_NB_BLOCK        *NBPtr,
  IN       UINT8               DctLimit
  );

BOOLEAN
MemFCratSupport (
  IN OUT   MEM_MAIN_DATA_BLOCK *MemMainPtr
  );
/*----------------------------------------------------------------------------
 *                           FUNCTIONS PROTOTYPE
 *
 *----------------------------------------------------------------------------
 */

#endif  /* _MFCRAT_H_ */

