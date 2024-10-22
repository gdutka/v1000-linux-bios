/** @file

;******************************************************************************
;* Copyright (c) 2012 - 2019, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <Uefi.h>
#include <FastRecoveryData.h>
#include <Library/PeiOemSvcKernelLib.h>
#include <Library/DebugLib.h>
#include <Library/HobLib.h>
#include <Library/PcdLib.h>
#include <Library/PeiServicesLib.h>
#include <PostCode.h>
#include <Library/PeiChipsetSvcLib.h>
#include <Library/FlashRegionLib.h>
#include <Library/RetrieveSpecificFfsInFv.h>


/**
  Install Firmware Volume Hob's once there is main memory.It installs
  firmware volume Hobs when main memory is available.

  @param PeiServices            General purpose services available to every PEIM.
  @param NotifyDescriptor       Pointer to EFI_PEI_NOTIFY_DESCRIPTOR
  @param Ppi                    EFI_PEI_FV_FILE_LOADER_PPI

  @retval   EFI_SUCCESS         if the interface could be successfully installed.

**/
EFI_STATUS
EFIAPI
MemoryDiscoveredPpiNotifyPlatformStage2Callback (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  )
{
  EFI_STATUS                          Status;
  EFI_BOOT_MODE                       BootMode;
  UINT8                               *PublicKey;
  UINT32                              PublicKeySize;
  EFI_STATUS                          OemSvcStatus;

  POST_CODE (PEI_MEMORY_CALLBACK);
  DEBUG ((EFI_D_INFO, "Enter MemoryDiscoveredPpiNotifyCallback\n"));

  //
  // Get boot mode
  //
  Status = PeiServicesGetBootMode (&BootMode);
  ASSERT_EFI_ERROR (Status);

  if (BootMode == BOOT_ON_S3_RESUME) {
    POST_CODE (S3_MEMORY_CALLBACK);
  }

  PeiCsSvcStage2MemoryDiscoverCallback (&BootMode);

  Status = RetrieveSpecificFfsInFv (PcdGetPtr (PcdSecureFlashPublicKeyFile), (VOID **)&PublicKey, &PublicKeySize);
  if (Status == EFI_SUCCESS) {
    BuildGuidDataHob ((EFI_GUID *)PcdGetPtr (PcdSecureFlashPublicKeyFile), PublicKey, PublicKeySize);
  }

  //
  // OemServices
  //
  DEBUG ((DEBUG_INFO, "OemKernelServices Call: OemSvcInitMemoryDiscoverCallback \n"));
  OemSvcStatus = OemSvcInitMemoryDiscoverCallback (&BootMode);
  DEBUG ((DEBUG_INFO, "OemKernelServices OemSvcInitMemoryDiscoverCallback Status: %r\n", OemSvcStatus));

  BuildResourceDescriptorHob (
    EFI_RESOURCE_FIRMWARE_DEVICE,
    (EFI_RESOURCE_ATTRIBUTE_PRESENT | EFI_RESOURCE_ATTRIBUTE_INITIALIZED | EFI_RESOURCE_ATTRIBUTE_UNCACHEABLE),
    FdmGetBaseAddr (),
    FdmGetFlashAreaSize ()
    );

  //
  // Create a CPU hand-off information
  //
  BuildCpuHob (FixedPcdGet8 (PcdMemorySpaceSize), FixedPcdGet8 (PcdIoSpaceSize));
  DEBUG ((EFI_D_INFO, "MemoryDiscoveredPpiNotifyCallback Exit\n"));

  return Status;
}

/**
  To force system into Recovery mode.

  @param PeiServices            General purpose services available to every PEIM.
  @param NotifyDescriptor       Pointer to EFI_PEI_NOTIFY_DESCRIPTOR
  @param Ppi                    EFI_PEI_FV_FILE_LOADER_PPI

  @retval EFI_SUCCESS           Enter Recovery mode OK.

**/
EFI_STATUS
ForceEnterRecoveryEntry (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  )
{
  EFI_STATUS                          OemSvcStatus;
  //
  // Due to there is no common method to set a recovery request, so let OEM to decide the method.
  // (OemServices)
  //
  DEBUG ((DEBUG_INFO, "OemKernelServices Call: OemSvcSetRecoveryRequest \n"));
  OemSvcStatus = OemSvcSetRecoveryRequest ();
  DEBUG ((DEBUG_INFO, "OemKernelServices OemSvcSetRecoveryRequest Status: %r\n", OemSvcStatus));

  //
  // Trigger a reset to force system enter recovery mode.
  //
  PeiServicesResetSystem ();

  return EFI_SUCCESS;
}
