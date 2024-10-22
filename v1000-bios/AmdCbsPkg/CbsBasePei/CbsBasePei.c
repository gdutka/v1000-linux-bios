/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */

#include "CbsBasePei.h"

/**
  Main entry for this driver.

  @param FileHandle     pointer to the firmware file system header.
  @param PeiServices     pointer to the PEI service table.

  @retval EFI_SUCESS     This function always complete successfully.

**/
EFI_STATUS
EFIAPI
CbsBasePeimEntry (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS                Status;

  if (CbsFamilyIdentify() == FALSE) {
    return EFI_SUCCESS;
  }
  Status = CbsInitEntry((EFI_PEI_SERVICES **) PeiServices);
  return Status;
}

