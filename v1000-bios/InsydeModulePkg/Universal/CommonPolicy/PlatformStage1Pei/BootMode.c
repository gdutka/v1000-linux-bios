/** @file
  Get the proper Boot Mode.

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

#include <PiPei.h>
#include <FastRecoveryData.h>
#include <Ppi/BootInRecoveryMode.h>
#include <Ppi/SStateBootMode.h>
#include <Ppi/CpuIo.h>
#include <Ppi/Capsule.h>
#include <Ppi/EndOfPeiPhase.h>
#include <Ppi/FirmwareVolumeInfo.h>
#include <Library/PeiOemSvcKernelLib.h>
#include <Library/DebugLib.h>
#include <Library/PeiChipsetSvcLib.h>
#include <Library/PrintLib.h>
#include <Library/BvdtLib.h>
#include <Library/SeamlessRecoveryLib.h>
#include <Library/PeiCapsuleLib.h>
#include <Library/HobLib.h>
#include <Library/CapsuleUpdateCriteriaLib.h>
#include <Library/FlashRegionLib.h>
#include <Library/H2OCpLib.h>
#include <Library/PostCodeLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/PerformanceLib.h>
#include <Guid/H2OCp.h>
#include <Guid/H2OPeiStorage.h>

#define MAX_STRING_LENGTH     128

/**
  Output debug message if any FV HOB isn't defined in FDM.

  @param[in] PeiServices       An indirect pointer to the EFI_PEI_SERVICES table published by the PEI Foundation.
  @param[in] NotifyDescriptor  Address of the notification descriptor data structure.
  @param[in] Ppi               Address of the PPI that was installed.

  @retval EFI_SUCCESS          Always return EFI_SUCCESS.

**/
EFI_STATUS
EFIAPI
EndofPeiSignalNotifyCallBack (
  IN EFI_PEI_SERVICES              **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR     *NotifyDescriptor,
  IN VOID                          *Ppi
  );

//
// Priority of our boot modes, highest priority first
//
static const EFI_BOOT_MODE  mBootModePriority[] = {
  BOOT_IN_RECOVERY_MODE,
  BOOT_ON_FLASH_UPDATE,
  BOOT_ON_S3_RESUME,
  BOOT_ON_S4_RESUME,
  BOOT_WITH_MINIMAL_CONFIGURATION,
  BOOT_ASSUMING_NO_CONFIGURATION_CHANGES,
  BOOT_WITH_FULL_CONFIGURATION,
  BOOT_WITH_FULL_CONFIGURATION_PLUS_DIAGNOSTICS,
  BOOT_WITH_DEFAULT_SETTINGS,
  BOOT_ON_S5_RESUME,
  BOOT_ON_S2_RESUME
};

EFI_PEI_PPI_DESCRIPTOR      mPpiListRecoveryBootMode = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gEfiPeiBootInRecoveryModePpiGuid,
  NULL
};

EFI_PEI_PPI_DESCRIPTOR      mPpiListSStateBootMode = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gPeiSStateBootModePpiGuid,
  NULL
};

EFI_PEI_NOTIFY_DESCRIPTOR           mNotifyListEndOfPei[] = {
  (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gEfiEndOfPeiSignalPpiGuid,
  EndofPeiSignalNotifyCallBack
};



/**
  Internal function to check if the FV is defined in FDM.

  @param[in]  FvBase              Base address of FV image.
  @param[in]  FvLength            Length of FV image.

  @retval TRUE     Can find FV in FDM.
  @retval FALSE    Can not find FV in FDM.
**/
STATIC
BOOLEAN
IsFvDefinedInFdm (
  IN  EFI_PHYSICAL_ADDRESS     FvBase,
  IN  UINT64                   FvLength
  )
{
  UINT8                    Count;
  EFI_STATUS               Status;
  UINT8                    Index;
  EFI_PHYSICAL_ADDRESS     FlashBase;
  EFI_GUID                 RegionType;
  UINT8                    RegionId[FDM_ENTRY_REGION_ID_SIZE];
  UINT64                   RegionOffset;
  UINT64                   RegionSize;
  UINT32                   Attribs;


  Status = FdmGetCount (&Count);
  if (EFI_ERROR (Status)) {
    return EFI_SUCCESS;
  }

  FlashBase = FdmGetBaseAddr ();
  for (Index = 1; Index <= Count; Index++) {
    Status = FdmGetAt (
               Index,
               &RegionType,
               RegionId,
               &RegionOffset,
               &RegionSize,
               &Attribs
               );
    if (EFI_ERROR (Status)){
      continue;
    }
    //
    // Some FV HOB may combine multiple region which is defined in FDM so only
    // check FvBase here is enough.
    //
    if (FvBase == FlashBase + RegionOffset) {
      return TRUE;
    }
  }
  return FALSE;
}

/**
  Output debug message if any FV HOB isn't defined in FDM.

  @param[in] PeiServices       An indirect pointer to the EFI_PEI_SERVICES table published by the PEI Foundation.
  @param[in] NotifyDescriptor  Address of the notification descriptor data structure.
  @param[in] Ppi               Address of the PPI that was installed.

  @retval EFI_SUCCESS          Always return EFI_SUCCESS.

**/
EFI_STATUS
EFIAPI
EndofPeiSignalNotifyCallBack (
  IN EFI_PEI_SERVICES              **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR     *NotifyDescriptor,
  IN VOID                          *Ppi
  )
{
  EFI_HOB_FIRMWARE_VOLUME  *FvHob;

  for (FvHob = (EFI_HOB_FIRMWARE_VOLUME *)GetFirstHob (EFI_HOB_TYPE_FV);
       FvHob != NULL;
       FvHob = (EFI_HOB_FIRMWARE_VOLUME *)GetNextHob (EFI_HOB_TYPE_FV, (VOID *)((UINTN)FvHob + FvHob->Header.HobLength))) {
     if (!IsFvDefinedInFdm (FvHob->BaseAddress, FvHob->Length)) {
       DEBUG((DEBUG_WARN, "WARNING!!! Firmware Volume Base Address: 0x%08x, Size: 0x%08x isn't defined in FDM.\n", FvHob->BaseAddress, FvHob->Length));
     }
  }
  return EFI_SUCCESS;
}


STATIC
EFI_STATUS
PrioritizeBootMode (
  IN OUT  EFI_BOOT_MODE     *CurrentBootMode,
  IN      EFI_BOOT_MODE     NewBootMode
  );

STATIC
BOOLEAN
IsBootWithNoChange (
  IN CONST EFI_PEI_SERVICES  **PeiServices
  );

BOOLEAN
IsRecoveryDetected (
  IN CONST EFI_PEI_SERVICES  **PeiServices
  );

/**
  Perform firmware update failure detection

  @param[in]  None

  @retval   Returns TRUE if the previous firmware update failed, otherwise returns FALSE

**/
BOOLEAN
DetectFirmwareUpdateFailure (
  VOID
  )
{
  CHAR16                          RecoveryFile[128];
  UINTN                           Size;
  EFI_STATUS                      Status;

  if (IsFirmwareFailureRecovery()) {
    //
    // If the previous firmware update is not successful (FirmwareUpdateOngoing variable is not cleared),
    // then update the recovery file path and update BootMode to activate recovery
    //
    UnicodeSPrint (RecoveryFile, MAX_STRING_LENGTH, L"%s\\%s1000.bin", EFI_CAPSULE_FILE_PATH, EFI_CAPSULE_FILE_NAME);
    Size = StrSize(RecoveryFile);
    Status = PcdSetPtrS (PcdPeiRecoveryFile, &Size, RecoveryFile);
    ASSERT_EFI_ERROR (Status);
    return TRUE;
  }
  return FALSE;

}

/**
  Internal function to build FV HOB and verify the FV is whether valid according to information
  in FDM.

  @param[in] RegionType      EFI_GUID that specifies the type of region that N-th entry is.
  @param[in] Instance        Unsigned integer that specifies entry instance of FDM

**/
STATIC
VOID
BuildVerifiedFvHob (
  CONST IN  EFI_GUID  *RegionType,
  CONST IN  UINT8     Instance
  )
{

 EFI_FIRMWARE_VOLUME_HEADER        *FvHeader;
 UINT64                            FvAddr;
 UINT64                            FvSize; 
 UINTN                             FvInfoInstance;
 EFI_STATUS                        Status;
 BOOLEAN                           IsFvInoPpiHasInstalled; 
 EFI_PEI_FIRMWARE_VOLUME_INFO_PPI  *FvInfoPpi;
 
 
  if (FeaturePcdGet (PcdH2OFdmChainOfTrustSupported)) {
    FdmVerifyNAt (RegionType, Instance);
  }

  FvAddr = FdmGetNAtAddr (RegionType, Instance);
  FvSize = FdmGetNAtSize (RegionType, Instance);

  BuildFvHob (
    (EFI_PHYSICAL_ADDRESS) FvAddr,
    FvSize
    );

  FvInfoInstance = 0;
  IsFvInoPpiHasInstalled = FALSE;

  while (TRUE){
    Status = PeiServicesLocatePpi (&gEfiPeiFirmwareVolumeInfoPpiGuid, FvInfoInstance, NULL, &FvInfoPpi);
    if (EFI_ERROR(Status)){
      break;
    }

    if ((FvAddr == (UINT64)(UINTN) FvInfoPpi->FvInfo) && (FvInfoPpi->FvInfoSize == (UINT32) FvSize)){
      IsFvInoPpiHasInstalled = TRUE;
      break;
    }
    FvInfoInstance++;
  }

  if (!IsFvInoPpiHasInstalled){
    FvHeader = (EFI_FIRMWARE_VOLUME_HEADER*)(UINTN) FvAddr;
    PeiServicesInstallFvInfoPpi (
      &FvHeader->FileSystemGuid,
      (VOID*) (UINTN) FvAddr,
      (UINT32) FvSize,
      NULL,
      NULL
      );
  }
}

/**
  Peform the boot mode determination logic

  @param[in]

  @retval   Peform the boot mode determination logic

**/
EFI_STATUS
EFIAPI
UpdateBootMode (
  IN CONST EFI_PEI_SERVICES     **PeiServices,
  IN       EFI_PEI_CPU_IO_PPI   *CpuIo,
  IN       EFI_BOOT_MODE        *BootMode
  )
{
  EFI_STATUS            Status;
  PEI_CAPSULE_PPI       *Capsule;
  BOOLEAN               SkipPriorityPolicy;
  UINT8                 SleepState;
  PEI_STORAGE_HOB_DATA  StorageData;
  UINT8                 Index;
  CHAR16                *StrBootMode;
  //
  // Assume boot mode are OK if not told otherwise
  //
  *BootMode = BOOT_WITH_FULL_CONFIGURATION;
  SkipPriorityPolicy = FALSE;
  SleepState = 0;

  //
  // OemServices
  //
  DEBUG ((DEBUG_INFO, "OemKernelServices Call: OemSvcChangeBootMode \n"));
  Status = OemSvcChangeBootMode (
             BootMode,
             &SkipPriorityPolicy
             );
  DEBUG ((DEBUG_INFO, "OemKernelServices OemSvcChangeBootMode Status: %r\n", Status));

  if (!SkipPriorityPolicy) {

    if (IsBootWithNoChange (PeiServices)) {
      Status = PrioritizeBootMode (BootMode, BOOT_ASSUMING_NO_CONFIGURATION_CHANGES);
      ASSERT_EFI_ERROR (Status);
    }

    PeiCsSvcGetSleepState (&SleepState);

    switch (SleepState) {

    case 3:
      Status = PrioritizeBootMode (BootMode, BOOT_ON_S3_RESUME);
      //
      // Determine if we're in capsule update mode
      //
      Status = (**PeiServices).LocatePpi (
                                          PeiServices,
                                          &gPeiCapsulePpiGuid,
                                          0,
                                          NULL,
                                          (VOID **) &Capsule
                                          );
      if (Status == EFI_SUCCESS) {
        Status = Capsule->CheckCapsuleUpdate ((EFI_PEI_SERVICES **)PeiServices);
        if (Status == EFI_SUCCESS) {
          Status = PrioritizeBootMode (BootMode, BOOT_ON_FLASH_UPDATE);
        }
      }

      if (DetectPendingUpdateImage()) {
        Status = PrioritizeBootMode (BootMode, BOOT_ON_FLASH_UPDATE);
      }

      break;

    case 4:
      Status = PrioritizeBootMode (BootMode, BOOT_ON_S4_RESUME);
      break;

    default:
      break;
    }
    Status = (**PeiServices).SetBootMode (PeiServices, *BootMode);
    ASSERT_EFI_ERROR (Status);
    if (IsRecoveryDetected (PeiServices)) {
      Status = PrioritizeBootMode (BootMode, BOOT_IN_RECOVERY_MODE);
    }
  } // if (!SkipPriorityPolicy)

  if (FeaturePcdGet (PcdH2OPeiCpSetBootModeBeforeSupported)) {
    H2O_PEI_CP_SET_BOOT_MODE_BEFORE_DATA       SetBootModeBeforeData;

    SetBootModeBeforeData.Size                 = sizeof (H2O_PEI_CP_SET_BOOT_MODE_BEFORE_DATA);
    SetBootModeBeforeData.Status               = H2O_CP_TASK_NORMAL;
    SetBootModeBeforeData.BootMode             = *BootMode;
    SetBootModeBeforeData.CustomBootModePolicy = SkipPriorityPolicy;

    DEBUG ((DEBUG_INFO, "Checkpoint Trigger: %g\n", &gH2OPeiCpSetBootModeBeforeGuid));
    H2OCpTrigger (&gH2OPeiCpSetBootModeBeforeGuid, &SetBootModeBeforeData);
    DEBUG ((DEBUG_INFO, "Checkpoint Result: %x\n", SetBootModeBeforeData.Status));

    if (SetBootModeBeforeData.Status == H2O_CP_TASK_UPDATE) {
      *BootMode = SetBootModeBeforeData.BootMode;
    } else if (SetBootModeBeforeData.Status == H2O_CP_TASK_SKIP) {
      DEBUG ((EFI_D_ERROR, "WARNING! It doesn't support the H2O_CP_TASK_SKIP status of H2O_PEI_CP_SET_BOOT_MODE_BEFORE checkpoint.\n"));
    }
  }

  switch (*BootMode) {
    case BOOT_WITH_FULL_CONFIGURATION:
      StrBootMode = L"BOOT_WITH_FULL_CONFIGURATION";
      break;
    case BOOT_WITH_MINIMAL_CONFIGURATION:
      StrBootMode = L"BOOT_WITH_MINIMAL_CONFIGURATION";
      break;
    case BOOT_ASSUMING_NO_CONFIGURATION_CHANGES:
      StrBootMode = L"BOOT_ASSUMING_NO_CONFIGURATION_CHANGES";
      break;
    case BOOT_WITH_FULL_CONFIGURATION_PLUS_DIAGNOSTICS:
      StrBootMode = L"BOOT_WITH_FULL_CONFIGURATION_PLUS_DIAGNOSTICS";
      break;
    case BOOT_WITH_DEFAULT_SETTINGS:
      StrBootMode = L"BOOT_WITH_DEFAULT_SETTINGS";
      break;
    case BOOT_ON_S4_RESUME:
      StrBootMode = L"BOOT_ON_S4_RESUME";
      break;
    case BOOT_ON_S5_RESUME:
      StrBootMode = L"BOOT_ON_S5_RESUME";
      break;
    case BOOT_ON_S2_RESUME:
      StrBootMode = L"BOOT_ON_S2_RESUME";
      break;
    case BOOT_ON_S3_RESUME:
      StrBootMode = L"BOOT_ON_S3_RESUME";
      break;
    case BOOT_ON_FLASH_UPDATE:
      StrBootMode = L"BOOT_ON_FLASH_UPDATE";
      break;
    case BOOT_IN_RECOVERY_MODE:
      StrBootMode = L"BOOT_IN_RECOVERY_MODE";
      break;
    default:
      StrBootMode = L"Unknown boot mode";
  }
  DEBUG ((EFI_D_INFO, "Setting BootMode to %s\n", StrBootMode));
  Status = (**PeiServices).SetBootMode (PeiServices, *BootMode);
  ASSERT_EFI_ERROR (Status);

  if (*BootMode == BOOT_ON_S3_RESUME) {
    Status = (**PeiServices).InstallPpi (PeiServices, &mPpiListSStateBootMode);
    ASSERT_EFI_ERROR (Status);

  } else if (*BootMode == BOOT_IN_RECOVERY_MODE ){
    //
    //  Enable usb & sata module in pei phase.
    //
    StorageData.Data            = 0;
    StorageData.Bits.UsbEnable  = PcdGetBool(PcdH2OCrisisRecoveryUsbMassStoragePeiSupported);
    StorageData.Bits.SataEnable = PcdGetBool(PcdH2OCrisisRecoverySataPeiSupported);
    BuildGuidDataHob ( &gH2OPeiStorageHobGuid, (VOID *) &StorageData, sizeof (PEI_STORAGE_HOB_DATA));

    Status = (**PeiServices).InstallPpi (PeiServices, &mPpiListRecoveryBootMode);
    ASSERT_EFI_ERROR (Status);
  }

  //
  // OemServices
  //
  DEBUG ((DEBUG_INFO, "OemKernelServices Call: OemSvcBootModeCreateFv \n"));
  Status = OemSvcBootModeCreateFv (
            (*BootMode)
            );
  DEBUG ((DEBUG_INFO, "OemKernelServices OemSvcBootModeCreateFv Status: %r\n", Status));
  if (Status == EFI_MEDIA_CHANGED) {
    return EFI_SUCCESS;
  }
  DEBUG_CODE (PeiServicesNotifyPpi (&mNotifyListEndOfPei[0]););
  if (*BootMode != BOOT_ON_S3_RESUME && *BootMode != BOOT_IN_RECOVERY_MODE) {
    for (Index = 1; FdmGetNAtSize (&gH2OFlashMapRegionFvGuid, Index) != 0; Index++){
      BuildVerifiedFvHob (&gH2OFlashMapRegionFvGuid, Index);
    }
    BuildFvHob (
      FdmGetNAtAddr (&gH2OFlashMapRegionVarGuid, 1),
      FdmGetNAtSize (&gH2OFlashMapRegionVarGuid, 1)
       + FdmGetNAtSize(&gH2OFlashMapRegionFtwStateGuid, 1)
       + FdmGetNAtSize(&gH2OFlashMapRegionFtwBackupGuid, 1)
       + FdmGetSizeById(&gH2OFlashMapRegionVarDefaultGuid, &gH2OFlashMapRegionFactoryCopyGuid, 1)
    );

  }

  return EFI_SUCCESS;
}

/**
  Given the current boot mode, and a proposed new boot mode, determine
  which has priority. If the new boot mode has higher priority, then
  make it the current boot mode.

  @param[in]

  @retval EFI_SUCCESS     - if both boot mode values were recognized and were processed.

**/
STATIC
EFI_STATUS
PrioritizeBootMode (
  IN OUT EFI_BOOT_MODE    *CurrentBootMode,
  IN EFI_BOOT_MODE        NewBootMode
  )
{
  UINT32  CurrentIndex;
  UINT32  NewIndex;

  //
  // Find the position of the current boot mode in our priority array
  //
  for (CurrentIndex = 0; CurrentIndex < sizeof (mBootModePriority) / sizeof (EFI_BOOT_MODE); CurrentIndex++) {
    if (mBootModePriority[CurrentIndex] == *CurrentBootMode) {
      break;
    }
  }


  if (CurrentIndex >= (sizeof (mBootModePriority) / sizeof (EFI_BOOT_MODE))) {
    return EFI_NOT_FOUND;
  }
  //
  // Find the position of the new boot mode in our priority array
  //
  for (NewIndex = 0; NewIndex < sizeof (mBootModePriority) / sizeof (EFI_BOOT_MODE); NewIndex++) {
    if (mBootModePriority[NewIndex] == NewBootMode) {
      if (NewIndex < CurrentIndex) {
        *CurrentBootMode = NewBootMode;
      }

      return EFI_SUCCESS;
    }
  }

  return EFI_NOT_FOUND;
}

/**
  Determine if Boot with no change is true.

  @param[in]  PeiServices    - pointer to the PEI Service Tabl

  @retval     TRUE           - No Change
  @retval     FALSE          - Change

**/
STATIC
BOOLEAN
IsBootWithNoChange (
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS                        Status;
  BOOLEAN                           IsNoChange;

  IsNoChange = FALSE;

  //
  // TODO : IsNoChange = SmartBoot
  //

  //
  // OemServices
  //
  DEBUG ((DEBUG_INFO, "OemKernelServices Call: OemSvcIsBootWithNoChange \n"));
  Status = OemSvcIsBootWithNoChange (
             &IsNoChange
             );
  DEBUG ((DEBUG_INFO, "OemKernelServices OemSvcIsBootWithNoChange Status: %r\n", Status));

  if(Status != EFI_MEDIA_CHANGED) {
    //
    // Default setting
    //
    IsNoChange = FALSE;
  }

  return IsNoChange;
}

BOOLEAN
IsRecoveryDetected (
  IN CONST EFI_PEI_SERVICES  **PeiServices
  )
{
  EFI_STATUS                        Status;
  BOOLEAN                           IsRecovery = FALSE;


  //
  // OemServices
  //
  DEBUG ((DEBUG_INFO, "OemKernelServices Call: OemSvcDetectRecoveryRequest \n"));
  Status = OemSvcDetectRecoveryRequest (
             &IsRecovery
             );
  DEBUG ((DEBUG_INFO, "OemKernelServices OemSvcDetectRecoveryRequest Status: %r\n", Status));

  if (!IsRecovery) {
    return DetectFirmwareUpdateFailure();
  }

  return (BOOLEAN) (IsRecovery && (EFI_ERROR (Status)));
}

