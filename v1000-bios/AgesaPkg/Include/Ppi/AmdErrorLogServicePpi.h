/*****************************************************************************
 *
 * Copyright (C) 2015-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 **/
#ifndef _AMD_ERROR_LOG_SERVICE_PPI_H_
#define _AMD_ERROR_LOG_SERVICE_PPI_H_

#include <AmdErrorLog.h>

typedef struct _PEI_AMD_ERROR_LOG_SERVICE_PPI PEI_AMD_ERROR_LOG_SERVICE_PPI;

//----------------------------------------------------
//
//  Amd Error Log Service Functions
//
//-------------------------------------------------------

typedef
EFI_STATUS
(EFIAPI *AMD_AQUIRE_ERROR_LOG_PEI)(
  IN       PEI_AMD_ERROR_LOG_SERVICE_PPI   *This,
  OUT      ERROR_LOG_DATA_STRUCT *ErrorLogDataPtr
  );

//----------------------------------------------------
//
//  AMD Error Log PPI
//
//-------------------------------------------------------

struct _PEI_AMD_ERROR_LOG_SERVICE_PPI {
  AMD_AQUIRE_ERROR_LOG_PEI       AmdAquireErrorLogPei;
};

//----------------------------------------------------
//
//  AMD Error Log Full PPI
//
//-------------------------------------------------------

typedef struct _PEI_AMD_ERROR_LOG_FULL_PPI {
  UINTN    Revision;               ///< Revision Number
} PEI_AMD_ERROR_LOG_FULL_PPI;

//----------------------------------------------------
//
//  AMD Error Log Available PPI
//
//-------------------------------------------------------

typedef struct _PEI_AMD_ERROR_LOG_AVAILABLE_PPI {
  UINTN    Revision;               ///< Revision Number
} PEI_AMD_ERROR_LOG_AVAILABLE_PPI;

// Current PPI revision
#define AMD_ERROR_LOG_PPI_REVISION   0x00

extern EFI_GUID gAmdErrorLogServicePpiGuid;
extern EFI_GUID gAmdErrorLogFullPpiGuid;
extern EFI_GUID gAmdErrorLogAvailablePpiGuid;
  
#endif  //_AMD_ERROR_LOG_PPI_H_

