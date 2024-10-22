/* $NoKeywords:$ */
/**
 * @file
 *
 * AgesaHookBeforeExitSelfRefresh.c
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project:
 * @e \$Revision: 309090 $ @e \$Date: 2014-12-10 02:28:05 +0800 (Wed, 10 Dec 2014) $
 *
 **/
/*****************************************************************************
  *
  * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
  *
  *******************************************************************************
  *
 */

#include <Uefi.h>
#include <PiPei.h>
#include <AGESA.h>
#include <Ppi/AmdMemPpi.h>

#define FILECODE LIBRARY_AMDCALLOUTLIB_AGESAHOOKBEFOREEXITSELFREFRESH_FILECODE

/*---------------------------------------------------------------------------------------*/
/**
 *  Call the host environment interface to provide a user hook opportunity.
 *
 *  @param[in]      FcnData
 *  @param[in,out]  MemData
 *
 * @return   The AGESA Status returned from the callout.
 *
 */
AGESA_STATUS
AgesaHookBeforeExitSelfRefresh (
  IN       UINTN           FcnData,
  IN OUT   MEM_DATA_STRUCT *MemData
  )
{
  AGESA_STATUS Status;

  Status =  AGESA_SUCCESS;

  return Status;
}
