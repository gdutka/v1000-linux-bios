/*****************************************************************************
 *
 * Copyright (C) 2016-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 ******************************************************************************
 */

#include <Uefi.h>
#include <PiPei.h>
#include <AGESA.h>
#include <AgesaReadSpd.h>
#include <Ppi/AmdMemPpi.h>

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