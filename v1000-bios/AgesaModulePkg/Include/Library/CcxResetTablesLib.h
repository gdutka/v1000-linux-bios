/* $NoKeywords:$ */
/**
 * @file
 *
 *
 * AMD Ccx Reset Tables Library
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Lib
 * @e \$Revision: 312770 $   @e \$Date: 2015-02-11 13:24:42 +0800 (Wed, 11 Feb 2015) $
 *
 */
/*
 ******************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 **/

#ifndef _CCX_RESET_TABLES_LIB_H_
#define _CCX_RESET_TABLES_LIB_H_
#pragma pack (push, 1)

/* -----------------------------------------------------------------------------*/
/**
 *  Program register tables on a core out of reset
 *
 *  @param[in] SleepType       - Sleep type.
 *  @param[in] StdHeader       - Config handle for library and services.
 *
 */
VOID
CcxProgramTablesAtReset (
  IN       UINT8               SleepType,
  IN OUT   AMD_CONFIG_PARAMS  *StdHeader
  );

#pragma pack (pop)
#endif // _CCX_RESET_TABLES_LIB_H_

