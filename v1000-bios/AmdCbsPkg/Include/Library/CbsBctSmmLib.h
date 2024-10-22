/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef _CBS_BCT_SMM_LIB_H_
#define _CBS_BCT_SMM_LIB_H_

#include "PiSmm.h"
#include "CbsBctSmmProtocol.h"

EFI_STATUS
EFIAPI
InstallCbsBctSmmProtocol();

EFI_STATUS
EFIAPI
CbsBctSetVariable (
  IN CBS_BCT_SMM_PROTOCOL    *This,
  IN UINT8                   *IfrData
);

EFI_STATUS
EFIAPI
CbsBctWriteFlash (
);

BOOLEAN
CbsFamilyIdentify (
  );

 #endif //_CBS_BCT_SMM_LIB_H_

