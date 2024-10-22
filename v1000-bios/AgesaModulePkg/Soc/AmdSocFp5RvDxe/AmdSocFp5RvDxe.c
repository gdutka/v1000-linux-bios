/*****************************************************************************
 *
 * Copyright (C) 2015-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */

#include <Filecode.h>
#include "AGESA.h"
#include <Library/DebugLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/AmdSocBaseLib.h>
#include <Library/AmdIdsHookLib.h>
#include <Library/DxeSocLogicalIdServicesLib.h>
#include <Protocol/AmdSocProtocol.h>
#include "AmdSocFp5RvDxe.h"

#define FILECODE SOC_AMDSOCFP5RVDXE_AMDSOCFP5RVDXE_FILECODE

extern  EFI_BOOT_SERVICES       *gBS;

//
// Driver Global Data
//

/*********************************************************************************
 * Name: AmdSocFp5DxeInit
 *
 * Description
 *   Entry point of the AMD SOC Raven FP5 DXE driver
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
AmdSocFp5RvDxeInit (
  IN       EFI_HANDLE         ImageHandle,
  IN       EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS          Status = EFI_SUCCESS;
  EFI_HANDLE          Handle;
  SOC_ID_STRUCT       SocId;
  SOC_ID_STRUCT       SocId2;
  SOC_ID_STRUCT       SocId3;
  UINT8 i;

  DEBUG ((EFI_D_ERROR, "*****************************DXE SOC FP5 RV Driver Entry*********************\n"));

  SocId.SocFamilyID = F17_RV_RAW_ID;
  SocId.PackageType = RV_FP5;

  SocId2.SocFamilyID = F17_RV2_RAW_ID;
  SocId2.PackageType = RV_FP5;

  SocId3.SocFamilyID = F17_PIC_RAW_ID;
  SocId3.PackageType = RV_FP5;

  // Check Hardware Identification
  if (SocHardwareIdentificationCheck (&SocId) || SocHardwareIdentificationCheck (&SocId2) || SocHardwareIdentificationCheck (&SocId3)) {
    //
    // Publish the logical ID protocol
    //
    Status = SocLogicalIdServiceProtocolInstall (ImageHandle, SystemTable);

    IDS_HOOK (IDS_HOOK_BEGINNING_OF_AGESA, NULL, NULL);

    //
    // Publish the Silicon installation protocol
    //

    Handle = ImageHandle;
    for (i = 0 ; i < SiliconDriverProtocolListNumber; i++) {
      Status = gBS->InstallProtocolInterface (
                    &Handle,
                    SiliconDriverProtocolList[i],
                    EFI_NATIVE_INTERFACE,
                    NULL
                    );
    }
  }
  DEBUG ((EFI_D_ERROR, "*****************************DXE SOC FP5 RV Driver Exit*********************\n"));
  return (Status);
}


