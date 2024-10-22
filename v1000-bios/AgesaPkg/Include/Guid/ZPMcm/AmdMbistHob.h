/* $NoKeywords:$ */
 /**
 * @file
 *
 * Amd MBIST Hob GUID.
 *
 * Contains GUID Declaration for Memory Info Hob
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  UEFI
 * @e \$Revision: $   @e \$ $
 *
 */
/*
 ****************************************************************************
 * AMD Generic Encapsulated Software Architecture
 *
 * Description: AmdMbistHob.h - Memory Mbist Hob GUID
 *
 *****************************************************************************
 *
 * Copyright (C) 2018-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef _AMD_MBIST_HOB_H_
#define _AMD_MBIST_HOB_H_

extern EFI_GUID gAmdMemoryMbistHobGuid;

#ifndef MAX_SOCKETS
  #define MAX_SOCKETS    2
#endif

#ifndef MAX_DIES_PER_SOCKET
  #define MAX_DIES_PER_SOCKET     4
#endif

#ifndef MAX_CHANNELS_PER_DIE
  #define MAX_CHANNELS_PER_DIE  2
#endif

#ifndef MAX_CHIPSELECT_PER_CHANNEL
  #define MAX_CHIPSELECT_PER_CHANNEL 4
#endif

#ifndef MAX_MBIST_SUBTEST
  #define MAX_MBIST_SUBTEST  5
#endif

//
// MBIST DATA EYE
//
typedef struct _MBIST_HOB_MARGIN {
  UINT8  PositiveEdge;
  UINT8  NegativeEdge;
} MBIST_HOB_MARGIN;

typedef struct _MBIST_DEYE_MARGIN {
  BOOLEAN       IsDataEyeValid;     ///< Is Data Eye Record Valid
  MBIST_HOB_MARGIN  ReadDqDelay;    ///< Smallest Positive and Negative Read Dq Delay margin
  MBIST_HOB_MARGIN  ReadVref;       ///< Smallest Positive and Negative Read Vref delay
  MBIST_HOB_MARGIN  WriteDqDelay;   ///< Smallest Positive and Negative Write Dq Delay margin
  MBIST_HOB_MARGIN  WriteVref;      ///< Smallest Positive and Negative Write Vref delay
} MBIST_DEYE_MARGIN;

#pragma pack (push, 1)
typedef struct _MBIST_TEST_RESULTS {
  UINT8  ResultValid;
  UINT8  ErrorStatus;
} MBIST_TEST_RESULTS;

typedef struct _AMD_MEMORY_MBIST_HOB {
  UINT8  NumSockets;
  UINT8  NumDiePerSocket;
  UINT8  NumSubTests;
  UINT8  NumChannelPerDie;
  MBIST_TEST_RESULTS MbistTestStatus [MAX_SOCKETS][MAX_DIES_PER_SOCKET][MAX_MBIST_SUBTEST][MAX_CHANNELS_PER_DIE];
  MBIST_DEYE_MARGIN  MbistDataEyeMargin[ MAX_SOCKETS][MAX_DIES_PER_SOCKET][MAX_CHANNELS_PER_DIE][MAX_CHIPSELECT_PER_CHANNEL];
} AMD_MEMORY_MBIST_HOB;

#pragma pack (pop)

#define AMD_MEMORY_MBIST_HOB_VERISION        0x00000101ul  // Ver: 00.00.01.01

#endif // _AMD_MBIST_HOB_H_

