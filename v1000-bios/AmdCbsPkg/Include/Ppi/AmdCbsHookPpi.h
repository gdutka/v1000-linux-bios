/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef _PEI_CBSHOOK_PPI_H_
#define _PEI_CBSHOOK_PPI_H_

///
/// Global ID for the PEI_CAPSULE_PPI.
///
#define AMD_AGESA_HOOK_PPI_GUID \
  { 0x4126c85, 0x5c4e, 0x458f, 0xab, 0xc1, 0x29, 0xf1, 0xf7, 0xda, 0x36, 0x5e }

///
/// Forward declaration for the PEI_CAPSULE_PPI.
///
typedef struct _PEI_CBS_HOOK_PPI PEI_CBS_HOOK_PPI;


///
/// PPI Interface prototype
///
typedef EFI_STATUS (EFIAPI *PEI_CBS_HOOK_INTERFACE) (
  IN  EFI_PEI_SERVICES       **PeiServices,
  IN  OUT VOID               *AMD_PARAMS,
  IN      UINTN              TimePoint
);

typedef struct _PEI_CBS_HOOK_PPI {
  PEI_CBS_HOOK_INTERFACE     CbsHookInterface;
} PEI_CBS_HOOK_PPI;

extern EFI_GUID gAmdCbsHookPpiGuid;

#endif // _PEI_CBSHOOK_PPI_H_

