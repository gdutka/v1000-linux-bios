/** @file

  Cache PPI definition.

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _PEI_CACHE_PPI_H_
#define _PEI_CACHE_PPI_H_

#define PEI_CACHE_PPI_GUID \
  { \
    0xc153205a, 0xe898, 0x4c24, {0x86, 0x89, 0xa4, 0xb4, 0xbc, 0xc5, 0xc8, 0xa2 } \
  }

typedef struct _PEI_CACHE_PPI PEI_CACHE_PPI;

//
// EFI_MEMORY_CACHE_TYPE
//
typedef INT32 EFI_MEMORY_CACHE_TYPE;

//
// PEI_SET_CACHE_PPI
//
typedef
EFI_STATUS
(EFIAPI *PEI_SET_CACHE_PPI) (
  IN  EFI_PEI_SERVICES       **PeiServices,
  IN  PEI_CACHE_PPI          *This,
  IN  EFI_PHYSICAL_ADDRESS   MemoryAddress,
  IN  UINT64                 MemoryLength,
  IN  EFI_MEMORY_CACHE_TYPE  MemoryCacheType
  );

//
// PEI_RESET_CACHE_PPI
//
typedef
EFI_STATUS
(EFIAPI *PEI_RESET_CACHE_PPI) (
  IN  EFI_PEI_SERVICES   **PeiServices,
  IN  PEI_CACHE_PPI      *This
  );

//
// PEI_CACHE_PPI
//
struct _PEI_CACHE_PPI {
  PEI_SET_CACHE_PPI    SetCache;
  PEI_RESET_CACHE_PPI  ResetCache;
};

extern EFI_GUID gPeiCachePpiGuid;

#endif
