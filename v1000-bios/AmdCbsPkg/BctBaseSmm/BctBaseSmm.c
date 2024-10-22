/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */
#include <Library/SmmServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include "BctBaseSmm.h"

CBS_BCT_SMM_PROTOCOL CbsBctSmmProtocol = {
  CbsBctSetVariable,
  CbsBctWriteFlash
};

 EFI_STATUS
 EFIAPI
 InstallCbsBctSmmProtocol(
 )
 {
  EFI_STATUS        Status;
  EFI_HANDLE        Handle;

  Handle = NULL;
  Status = gSmst->SmmInstallProtocolInterface (
                    &Handle,
                    &gCbsBctSmmProtocolGuid,
                    EFI_NATIVE_INTERFACE,
                    &CbsBctSmmProtocol
                    );

  return Status;
 }

 EFI_STATUS
 EFIAPI
 BctBaseSmmEntry(
  IN       EFI_HANDLE         ImageHandle,
  IN       EFI_SYSTEM_TABLE   *SystemTable
 )
 {

  if (CbsFamilyIdentify() == FALSE) {
    return EFI_SUCCESS;
  }
  InstallCbsBctSmmProtocol();
  return EFI_SUCCESS;
 }

