/*****************************************************************************
 *
 * Copyright (C) 2016-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */
#ifndef _AMD_NBIO_BASE_SERVICES_PPI_H_
#define _AMD_NBIO_BASE_SERVICES_PPI_H_

#include <Guid/GnbNbioBaseZPInfoHob.h>
#include <GnbDxio.h>

// Current PPI revision
#define AMD_NBIO_BASE_SERVICES_REVISION   0x00

typedef struct _PEI_AMD_NBIO_BASE_SERVICES_PPI PEI_AMD_NBIO_BASE_SERVICES_PPI;

typedef
EFI_STATUS
(EFIAPI * AMD_NBIO_DEBUG_GET_DEBUG_CONFIG) (
  IN  PEI_AMD_NBIO_BASE_SERVICES_PPI   *This,
  OUT GNB_BUILD_OPTIONS_ZP_DATA_HOB    **DebugOptions
);

typedef
EFI_STATUS
(EFIAPI * AMD_NBIO_REGISTER_READ) (
  IN       GNB_HANDLE          *GnbHandle,
  IN       UINT8               RegisterSpaceType,
  IN       UINT32              Address,
     OUT   VOID                *Value,
  IN       UINT32              Flags,
  IN       AMD_CONFIG_PARAMS   *StdHeader
);

typedef
EFI_STATUS
(EFIAPI * AMD_NBIO_REGISTER_WRITE) (
  IN       GNB_HANDLE          *GnbHandle,
  IN       UINT8               RegisterSpaceType,
  IN       UINT32              Address,
  IN       VOID                *Value,
  IN       UINT32              Flags,
  IN       AMD_CONFIG_PARAMS   *StdHeader
);

///
/// The Ppi of Base Services
///
struct _PEI_AMD_NBIO_BASE_SERVICES_PPI {
  UINT32                               Revision;         ///< revision
  OUT UINT32                           *PcieComplexData; ///<
  OUT UINT32                           PcieComplexSize;  ///<
  OUT AMD_NBIO_DEBUG_GET_DEBUG_CONFIG  DebugOptions;     ///<
  OUT AMD_NBIO_REGISTER_READ           Read;             ///<
  OUT AMD_NBIO_REGISTER_WRITE          Write;            ///<
};

extern EFI_GUID gAmdNbioBaseServicesZpPpiGuid;
#endif //

