/*****************************************************************************
 *
 * Copyright (C) 2015-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */
/*++
Module Name:

  AmdVersionPei.c
  Init AGESA version string interface

Abstract:
--*/

#include <Filecode.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Ppi/AmdVerPpi.h>
#include "AmdVersionPei.h"


#define FILECODE UNIVERSAL_VERISION_AMDVERSIONPEI_AMDVERSIONPEI_FILECODE

/*++

Routine Description:

  Version Driver Entry. Initialize and publish AGESA version string PPI

Arguments:

Returns:

  EFI_STATUS

--*/
EFI_STATUS
EFIAPI
AmdVersionPeiInit (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS          Status = EFI_SUCCESS;
  PEI_AMD_VER_PPI    *mAmdVersionStringPpi;
  EFI_PEI_PPI_DESCRIPTOR    *mAmdVersionString;

  UINTN SignatureSize;

  //Init VER_PEI_PPI
  Status = (*PeiServices)->AllocatePool (
                             PeiServices,
                             sizeof (PEI_AMD_VER_PPI),
                             (VOID**)&mAmdVersionStringPpi
                             );
  ASSERT_EFI_ERROR ( Status);

  mAmdVersionStringPpi->Signature = (CHAR8*)PcdGetPtr (PcdAmdPackageString);
  SignatureSize = AsciiStrSize (mAmdVersionStringPpi->Signature);
  mAmdVersionStringPpi->VersionString = mAmdVersionStringPpi->Signature + SignatureSize;

  // Allocate memory for the PPI descriptor
  Status = (*PeiServices)->AllocatePool (
                             PeiServices,
                             sizeof (EFI_PEI_PPI_DESCRIPTOR),
                             (VOID **)&mAmdVersionString
                             );
  ASSERT_EFI_ERROR ( Status);

  mAmdVersionString->Flags = (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST);
  mAmdVersionString->Guid  = &gAmdVersionStringPpiGuid;
  mAmdVersionString->Ppi   = mAmdVersionStringPpi;

  Status = (*PeiServices)->InstallPpi (
                             PeiServices,
                             mAmdVersionString
                             );
  return (Status);
}


