/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef __CBS_PEI_FUNC_LIB_H__
#define __CBS_PEI_FUNC_LIB_H__

EFI_STATUS CbsInitEntry(
  IN  EFI_PEI_SERVICES       **PeiServices
  );

EFI_STATUS
CbsInterfaceFunc (
  IN  EFI_PEI_SERVICES       **PeiServices,
  IN  OUT VOID               *AMD_PARAMS,
  IN      UINTN              TimePoint
  );

BOOLEAN
CbsFamilyIdentify (
  );

#endif //__CBS_PEI_FUNC_LIB_H__

