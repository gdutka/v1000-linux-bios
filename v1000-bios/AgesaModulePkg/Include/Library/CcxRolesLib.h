/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD CCX Roles Library
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

#ifndef _CCX_ROLES_LIB_H_
#define _CCX_ROLES_LIB_H_
#include "AGESA.h"

/*---------------------------------------------------------------------------------------*/
/**
 *  Is this the BSP core?
 *
 *  @param[in,out]   StdHeader        Header for library and services
 *
 *  @retval          TRUE             Is BSP core
 *  @retval          FALSE            Is not BSP Core
 *
 */
BOOLEAN
CcxIsBsp (
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

/*---------------------------------------------------------------------------------------*/
/**
 *  Is the current core a primary core of it's compute unit?
 *
 *  @param[in]  StdHeader   Config handle for library and services.
 *
 *  @retval        TRUE  Is Primary Core
 *  @retval        FALSE Is not Primary Core
 *
 */
BOOLEAN
CcxIsComputeUnitPrimary (
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

/*---------------------------------------------------------------------------------------*/
/**
 *  Is the current core a primary core of its complex?
 *
 *  @param[in]  StdHeader   Config handle for library and services.
 *
 *  @retval        TRUE  Is Primary Core
 *  @retval        FALSE Is not Primary Core
 *
 */
BOOLEAN
CcxIsComplexPrimary (
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

/*---------------------------------------------------------------------------------------*/
/**
 *  Is the current core a primary core of it's node?
 *
 *  @param[in]  StdHeader   Config handle for library and services.
 *
 *  @retval        TRUE  Is Primary Core
 *  @retval        FALSE Is not Primary Core
 *
 */
BOOLEAN
CcxIsDiePrimary (
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

/*---------------------------------------------------------------------------------------*/
/**
 *  Is the current core a primary core of it's node?
 *
 *  @param[in]  StdHeader   Config handle for library and services.
 *
 *  @retval        TRUE  Is Primary Core
 *  @retval        FALSE Is not Primary Core
 *
 */
BOOLEAN
CcxIsSocketPrimary (
  IN       AMD_CONFIG_PARAMS *StdHeader
  );
#endif // _CCX_ROLES_LIB_H_

