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

#ifndef _CCX_BASE_X86_SERVICES_DXE_H_
#define _CCX_BASE_X86_SERVICES_DXE_H_
#include "AGESA.h"

/**
 *---------------------------------------------------------------------------------------
 *
 *  Returns the maximum physical address size
 *
 *  Parameters:
 *
 *    @return          UINT8   Maximum physical address size
 *
 *---------------------------------------------------------------------------------------
 */
UINT8
EFIAPI
CcxGetMaxPhysAddr (
  IN       CCX_BASE_SERVICES_PROTOCOL *This
  );

/**
 *---------------------------------------------------------------------------------------
 *
 *  Returns the maximum processor capacity in the system
 *
 *  Parameters:
 *
 *    @return          UINT8   Maximum processor capacity
 *
 *---------------------------------------------------------------------------------------
 */
UINT8
EFIAPI
CcxGetMaxProcCap (
  IN       CCX_BASE_SERVICES_PROTOCOL *This
  );


#endif // _CCX_BASE_X86_SERVICES_DXE_H_

