/*****************************************************************************
 *
 * Copyright (C) 2016-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */
#ifndef _AMD_NBIO_RV_PPI_H_
#define _AMD_NBIO_RV_PPI_H_

// Current PPI revision
#define AMD_RV_STATUS   TRUE

typedef struct _PEI_AMD_RV_PPI PEI_AMD_RV_PPI;

///
/// The Ppi of RV
///
typedef struct _PEI_AMD_NBIO_SMU_SERVICES_PPI {
  BOOLEAN   IsRV;            ///<
};

extern EFI_GUID gAmdNbioBaseRVPeiDepexPpiGuid;

#endif //
