/*****************************************************************************
 *
 * Copyright (C) 2016-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */
#ifndef _AMD_NBIO_BASE_SERVICES_PPI_H_
#define _AMD_NBIO_BASE_SERVICES_PPI_H_

// Current PPI revision
#define AMD_NBIO_DEBUG_OPTION_SERVICES_REVISION   0x00

typedef struct _PEI_AMD_NBIO_DEBUG_SERVICES_PPI PEI_AMD_NBIO_DEBUG_SERVICES_PPI;

//
// PPI prototype
//
/**
  Returns the NBIO debug options configuration structure

  This
    A pointer to the PEI_AMD_NBIO_SMU_SERVICES_PPI instance.
  DebugOptions
    A pointer to a pointer to store the address of the debug options configuration structure
**/
typedef
EFI_STATUS
(EFIAPI * AMD_NBIO_DEBUG_GET_DEBUG_CONFIG) (
  IN  PEI_AMD_NBIO_DEBUG_SERVICES_PPI  *This,
  OUT UINT32  **DebugOptions
);


///
/// The Ppi of Base Debug sevices
///
typedef struct _PEI_AMD_NBIO_DEBUG_SERVICES_PPI {
  UINT32 Revision;                      ///< revision
  AMD_NBIO_DEBUG_GET_DEBUG_CONFIG       DebugGetConfig; ///<
};

extern EFI_GUID gAmdNbioDebugServicesPpiGuid;
#endif //

