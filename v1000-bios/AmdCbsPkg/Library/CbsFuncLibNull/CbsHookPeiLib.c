/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */

#include "CbsHookPeiLib.h"
STATIC PEI_CBS_HOOK_PPI mCbsHookInstance =
{
  CbsHookFunc
};

STATIC EFI_PEI_PPI_DESCRIPTOR mCbsHookInstancePpiList =
{
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gAmdCbsHookPpiGuid,
  &mCbsHookInstance
};

EFI_STATUS
CbsHookFunc (
  IN       EFI_PEI_SERVICES     **PeiServices,
  IN OUT   VOID                 *AMD_PARAMS,
  IN       UINTN                IdsHookId
  )
{
    return EFI_SUCCESS;
}

EFI_STATUS CbsInitEntry(
  IN CONST  EFI_PEI_SERVICES       **PeiServices
  )
{
  EFI_STATUS   Status;
  PEI_CBS_HOOK_PPI    *CbsHookPtr;

  Status = (*PeiServices)->LocatePpi (
                             PeiServices,
                             &gAmdCbsHookPpiGuid,
                             0,
                             NULL,
                             &CbsHookPtr
                             );
  if (EFI_ERROR (Status)) {
    Status = (**PeiServices).InstallPpi (PeiServices, &mCbsHookInstancePpiList);
  }

  return Status;
}

EFI_STATUS
CbsInterfaceFunc (
  IN  EFI_PEI_SERVICES       **PeiServices,
  IN  OUT VOID               *AMD_PARAMS,
  IN      UINTN              IdsHookId
  )
{
  return EFI_SUCCESS;
}

BOOLEAN
CbsFamilyIdentify (
  )
{
  return TRUE;
}

