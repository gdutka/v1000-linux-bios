/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD CCX Base Library
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Lib
 * @e \$Revision$   @e \$Date$
 *
 */
/*
 ******************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 **/

#ifndef _DXE_CCX_BASE_X86_SERVICES_LIB_H_
#define _DXE_CCX_BASE_X86_SERVICES_LIB_H_
#include "AGESA.h"

EFI_STATUS
EFIAPI
CcxBaseServicesProtocolInstall (
  IN       EFI_HANDLE         ImageHandle,
  IN       EFI_SYSTEM_TABLE   *SystemTable
  );

#endif // _DXE_CCX_BASE_X86_SERVICES_LIB_H_

