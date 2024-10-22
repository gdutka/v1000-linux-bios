/*****************************************************************************
 *
 * Copyright (C) 2015-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */

#include <Filecode.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Protocol/AmdVerProtocol.h>
#include "AmdVersionDxe.h"

#define FILECODE UNIVERSAL_VERISION_AMDVERSIONDXE_AMDVERSIONDXE_FILECODE

extern  EFI_BOOT_SERVICES       *gBS;

//
// Driver Global Data
//

/*********************************************************************************
 * Name: AmdVersionDxeInit
 *
 * Description
 *   Entry point of the AMD Version DXE driver
 *   Perform the configuration init, resource reservation, early post init
 *   and install all the supported protocol
 *
 * Input
 *   ImageHandle : EFI Image Handle for the DXE driver
 *   SystemTable : pointer to the EFI system table
 *
 * Output
 *   EFI_SUCCESS : Module initialized successfully
 *   EFI_ERROR   : Initialization failed (see error for more details)
 *
 *********************************************************************************/
EFI_STATUS
EFIAPI
AmdVersionDxeInit (
  IN       EFI_HANDLE         ImageHandle,
  IN       EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS          Status = EFI_SUCCESS;
  EFI_HANDLE          Handle;
  UINTN               SigStringSize;
  UINTN               PcdFullSize;
  CONST CHAR8* AmdVersionStringSig;
  UINT8* AmdVersionString;
  AMD_VER_PROTOCOL *AmdVerProtocol;


  //
  // Publish AGESA version string
  //

  //Get Signature and version string size.
  AmdVersionStringSig = (CHAR8*)PcdGetPtr (PcdAmdPackageString);
  SigStringSize = AsciiStrSize (AmdVersionStringSig);
  PcdFullSize = SigStringSize + AsciiStrSize (AmdVersionStringSig + SigStringSize);

  // Allocate memory for version string
  Status = gBS->AllocatePool (
                  EfiRuntimeServicesData,
                  PcdFullSize,
                  (VOID**)&AmdVersionString
                  );
  ASSERT_EFI_ERROR (Status);

  gBS->CopyMem (
        AmdVersionString,
        (VOID *)PcdGetPtr (PcdAmdPackageString),
        PcdFullSize
        );

  // Allocate memory for AGESA Version String Protocol
  Status = gBS->AllocatePool (
                  EfiRuntimeServicesData,
                  sizeof (AMD_VER_PROTOCOL),
                  (VOID**)&AmdVerProtocol
                  );
  ASSERT_EFI_ERROR (Status);

  AmdVerProtocol->Signature = (CHAR8*)AmdVersionString;
  AmdVerProtocol->VersionString = (CHAR8*)(AmdVersionString + SigStringSize);
  //
  // Publish AGESA version string protocol
  //
  Handle = ImageHandle;
  Status = gBS->InstallProtocolInterface (
                  &Handle,
                  &gAmdVersionStringProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &AmdVerProtocol
                  );

  return (Status);
}


