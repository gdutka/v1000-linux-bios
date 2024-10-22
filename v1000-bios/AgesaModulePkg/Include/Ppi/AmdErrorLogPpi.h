/*****************************************************************************
 *
 * Copyright (C) 2015-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 **/
#ifndef _AMD_ERROR_LOG_PPI_H_
#define _AMD_ERROR_LOG_PPI_H_

#include <AmdErrorLog.h>

typedef struct _PEI_AMD_ERROR_LOG_PPI PEI_AMD_ERROR_LOG_PPI;

//----------------------------------------------------
//
//  Amd Error Log Service Functions
//
//-------------------------------------------------------

typedef
EFI_STATUS
(EFIAPI *AMD_ERROR_LOG_PEI)(
  IN       PEI_AMD_ERROR_LOG_PPI   *This,
  IN       AMD_STATUS ErrorClass,
  IN       UINT32 ErrorInfo,
  IN       UINT32 DataParam1,
  IN       UINT32 DataParam2,
  IN       UINT32 DataParam3,
  IN       UINT32 DataParam4
  );

typedef
EFI_STATUS
(EFIAPI *AMD_ERROR_LOG_IP_COMPLETE_PEI)(
  IN       PEI_AMD_ERROR_LOG_PPI   *This,
  IN CONST EFI_GUID            *SiliconDriverId
  );

//----------------------------------------------------
//
//  AMD Error Log PPI
//
//-------------------------------------------------------

struct _PEI_AMD_ERROR_LOG_PPI {
  AMD_ERROR_LOG_PEI              AmdErrorLogPei;               ///< Amd Error Log Pei
  AMD_ERROR_LOG_IP_COMPLETE_PEI  AmdErrorLogIpCompletePei;
};

//----------------------------------------------------
//
//  AMD Error Log Ready PPI
//
//-------------------------------------------------------

typedef struct _PEI_AMD_ERROR_LOG_READY_PPI {
  UINTN    Revision;               ///< Revision Number
} PEI_AMD_ERROR_LOG_READY_PPI;

// Current PPI revision
#define AMD_ERROR_LOG_PPI_REVISION   0x00

extern EFI_GUID gAmdErrorLogReadyPpiGuid;
extern EFI_GUID gAmdErrorLogPpiGuid;

#endif  //_AMD_ERROR_LOG_PPI_H_

