/*****************************************************************************
 *
 * Copyright (C) 2017-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 **/
#ifndef _AMD_MBIST_PPI_H_
#define _AMD_MBIST_PPI_H_


//----------------------------------------------------
//
//  AMD MBIST PEI Initialization Complete PPI
//
//-------------------------------------------------------

typedef struct _MBIST_TEST_STATUS {
  UINT8	ResultValid[MAX_MBIST_SUBTEST][MAX_CHANNELS_PER_DIE];
  UINT8	ErrorStatus[MAX_MBIST_SUBTEST][MAX_CHANNELS_PER_DIE];
} MBIST_TEST_STATUS;

//
// MBIST DATA EYE
//
typedef struct _MBIST_MARGIN {
  UINT8  PositiveEdge;
  UINT8  NegativeEdge;
} MBIST_MARGIN;

typedef struct _MBIST_DATA_EYE_MARGIN {
  MBIST_MARGIN  ReadDqDelay;    ///< Smallest Positive and Negative Read Dq Delay margin
  MBIST_MARGIN  ReadVref;       ///< Smallest Positive and Negative Read Vref delay
  MBIST_MARGIN  WriteDqDelay;   ///< Smallest Positive and Negative Write Dq Delay margin
  MBIST_MARGIN  WriteVref;      ///< Smallest Positive and Negative Write Vref delay
} MBIST_DATA_EYE_MARGIN;

typedef struct _MBIST_DATA_EYE_MARGIN_RECORD {
  BOOLEAN                IsDataEyeValid;    ///< Is Data Eye Record Valid
  MBIST_DATA_EYE_MARGIN  DataEyeMargin;     ///< DataEyeRecord
} MBIST_DATA_EYE_MARGIN_RECORD;

typedef struct _MBIST_DATA_EYE {
  MBIST_DATA_EYE_MARGIN_RECORD  MbistDataEyeRecord[MAX_CHANNELS_PER_DIE][MAX_CHIPSELECT_PER_CHANNEL];
} MBIST_DATA_EYE;
//
// PPI function Prototype
//
typedef EFI_STATUS (EFIAPI * PEI_GET_MBIST_TEST_RESULTS) (
  IN CONST EFI_PEI_SERVICES  **PeiServices,
  IN UINT32 *BufferSize,
  IN OUT MBIST_TEST_STATUS  *MbistTestResults
);

typedef EFI_STATUS (EFIAPI * PEI_GET_MBIST_DATA_EYE) (
  IN CONST EFI_PEI_SERVICES  **PeiServices,
  IN UINT32 *BufferSize,
  IN OUT MBIST_DATA_EYE  *MbistDataEye
);

typedef struct _AMD_MEMORY_MBIST_TEST_RESULTS_PPI {
  UINT32                        Revision;
  MBIST_TEST_STATUS             MbistTestStatus [MAX_SOCKETS][MAX_DIES_PER_SOCKET];
  MBIST_DATA_EYE                MbistDataEyeMargin[ MAX_SOCKETS][MAX_DIES_PER_SOCKET];
  PEI_GET_MBIST_TEST_RESULTS    GetMbistTestResults;
  PEI_GET_MBIST_DATA_EYE        GetMbistDataEyeMargin;
} AMD_MEMORY_MBIST_TEST_RESULTS_PPI;

// Current PPI revision
#define AMD_MBIST_PPI_REVISION   0x03

extern EFI_GUID gAmdMbistPeiPpiGuid;

#endif  //_AMD_MBIST_PPI_H_

