/** @file
  Crisis Recovery Implementation

;******************************************************************************
;* Copyright (c) 2012 - 2017, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "CrisisRecovery.h"
#include <Library/BaseOemSvcKernelLib.h>
#include <Library/PeiOemSvcKernelLib.h>
#include <Library/FlashRegionLib.h>
//
#include <Library/VariableSupportLib.h>
#include <Library/BaseLib.h>
#include <Library/PcdLib.h>
#include <Library/DebugLib.h>
//
#include <PostCode.h>

FLASH_ENTRY mProtectRegion[] = {
  {
    0,  // RecoveryFV Address to be updated
    0,  // RecoveryFv Size to be updated
    0   // RecoveryFv Offset to be updated
  },
  {0x0, 0x0, 0x0}
};



EFI_FFS_FILE_HEADER  *mFfsHeader;

//
// Module globals
//
EFI_PEI_RECOVERY_MODULE_PPI mRecoveryPpi = {PlatformRecoveryModule};

EFI_PEI_PPI_DESCRIPTOR mRecoveryPpiList = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gEfiPeiRecoveryModulePpiGuid,
  &mRecoveryPpi
};

STATIC
EFI_STATUS
EFIAPI
GetDefaultProtectRegion (
  OUT  FLASH_ENTRY     **DefaultProtectTable,
  OUT  UINTN           *TableSize
  )
{

  FLASH_ENTRY   *Pool;
  UINT64        RecoveryFvAddr;
  UINT64        RecoveryFvSize;
  UINTN         DefaultSize;
  EFI_STATUS    Status;

  RecoveryFvAddr = 0;
  DefaultSize = sizeof(FLASH_ENTRY) * 2;
  *TableSize = DefaultSize;

  Status = PeiServicesAllocatePool (DefaultSize, (VOID**)&Pool);
  *DefaultProtectTable = Pool;

  if (EFI_ERROR(Status)){
    return EFI_OUT_OF_RESOURCES;
  }

  RecoveryFvAddr = FdmGetNAtAddr (
                     &gH2OFlashMapRegionBootFvGuid,
                     1
                     );

  RecoveryFvSize = FdmGetNAtSize (
                     &gH2OFlashMapRegionBootFvGuid,
                     1
                     );

  Pool->WriteAddress = (UINT32) RecoveryFvAddr;
  Pool->WriteSize    = (UINT32) RecoveryFvSize;
  Pool->SourceOffset = (UINT32) (RecoveryFvAddr - FdmGetBaseAddr());
  Pool++;

  Pool->WriteAddress = 0;
  Pool->WriteSize    = 0;
  Pool->SourceOffset = 0;

  return Status;
}

/**
 Pei crisis recovery in PEI stage

 @param [in]   FileHandle
 @param [in]   PeiServices      General purpose services available to every PEIM

 @retval EFI_SUCCESS            Success
 @retval EFI_UNSUPPORTED        Some required PPIs are not available
 @retval EFI_OUT_OF_RESOURCES   Not enough memory to allocate

**/
EFI_STATUS
CrisisRecoveryEntry (
  IN EFI_PEI_FILE_HANDLE             FileHandle,
  IN CONST EFI_PEI_SERVICES          **PeiServices
  )
{
  EFI_STATUS                      Status;
  EFI_PEI_PPI_DESCRIPTOR          *oldRecoveryPpiDescriptor;
  EFI_PEI_RECOVERY_MODULE_PPI     *oldRecoveryPpi;

  Status = (*PeiServices)->LocatePpi (
                             PeiServices,
                             &gEfiPeiRecoveryModulePpiGuid,
                             0,
                             &oldRecoveryPpiDescriptor,
                             (VOID **)&oldRecoveryPpi
                             );

  if (Status == EFI_SUCCESS) {
    Status = (*PeiServices)->ReInstallPpi (
                               PeiServices,
                               oldRecoveryPpiDescriptor,
                               &mRecoveryPpiList
                               );
  } else {
    Status = (*PeiServices)->InstallPpi (
                               PeiServices,
                               &mRecoveryPpiList
                               );
  }

  return Status;
}

/**
 Get BIOS image information from recovery capsule

 @param [in]   Capsule          Recovery capsule buffer
 @param [in]   CapsuleSize      Recovery capsule file size
 @param [out]  BiosImageOffset  The offset of BIOS image from recovery capsule file buffer
 @param [out]  BiosImageSize    The size of the BIOS image


 @retval EFI_SUCCESS            BIOS image information is successfully retrieved
 @return others                 Failed to get BIOS image information from recovery capsule

**/
EFI_STATUS
GetBiosImageFromCapsule (
  IN UINT8    *Capsule,
  IN UINTN    CapsuleSize,
  OUT UINTN   *BiosImageOffset,
  OUT UINTN   *BiosImageSize
  )
{
  UINTN                       Index;
  ISFLASH_DATA_REGION_HEADER  *DataRegion;

  if ( (Capsule == NULL) || (CapsuleSize == 0) ) {
    return EFI_INVALID_PARAMETER;
  }
  if (!((Capsule[0] == (UINT8)'M') && (Capsule[1] == (UINT8)'Z'))) {
    *BiosImageOffset = 0;
    *BiosImageSize = CapsuleSize;
    if (FeaturePcdGet(PcdSecureFlashSupported)) {
      *BiosImageSize -= SIGNATURE_SIZE;
    }
    return EFI_SUCCESS;
  }

  //
  // Capsule image is PE32 image
  // Search for BIOS image
  //
  for (Index = 0; Index < CapsuleSize - ISFLASH_TAG_SIZE; Index++) {
    if (CompareMem(Capsule + Index, ISFLASH_BIOS_IMAGE_TAG_HALF_1, ISFLASH_HALF_TAG_SIZE) == 0){
      if (CompareMem(Capsule + Index + ISFLASH_HALF_TAG_SIZE, ISFLASH_BIOS_IMAGE_TAG_HALF_2, ISFLASH_HALF_TAG_SIZE) == 0) {
        break;
      }
    }
  }
  if (Index == CapsuleSize - ISFLASH_TAG_SIZE) {
    return EFI_NOT_FOUND;
  }

  //
  // Copy BIOS image to the start of FirmwareBin pointer
  //
  DataRegion = (ISFLASH_DATA_REGION_HEADER *)(Capsule + Index);
  *BiosImageOffset = Index + sizeof(ISFLASH_DATA_REGION_HEADER);
  *BiosImageSize = DataRegion->DataSize;
  if (*BiosImageOffset + *BiosImageSize > CapsuleSize) {
    return EFI_NOT_FOUND;
  }
  return EFI_SUCCESS;
}

VOID
ClearFirmwareUpdatingFlag (
  VOID
  )
{
  EFI_STATUS  Status;

  if (!IsFirmwareFailureRecovery()) {
    return;
  }

  Status = FlashErase( (UINTN) FdmGetNAtAddr (&gH2OFlashMapRegionFtwBackupGuid ,1), GetFlashBlockSize());
  if (EFI_ERROR(Status)) {
    ASSERT_EFI_ERROR(Status);
    return;
  }
}

/**
 Provide the functionality of the Ea Recovery Module.

 @param [in]   PeiServices      General purpose services available to every PEIM.
 @param [in]   This

 @retval Status                 EFI_SUCCESS if the interface could be successfully
                                installed

**/
EFI_STATUS
EFIAPI
PlatformRecoveryModule (
  IN EFI_PEI_SERVICES                     **PeiServices,
  IN EFI_PEI_RECOVERY_MODULE_PPI          *This
  )
{
  EFI_STATUS                            Status;
  EFI_PEI_DEVICE_RECOVERY_MODULE_PPI    *DeviceRecoveryModule;
  UINTN                                 NumberOfImageProviders;
  BOOLEAN                               ProviderAvailable;
  UINTN                                 NumberRecoveryCapsules;
  UINTN                                 RecoveryCapsuleSize;
  EFI_GUID                              DeviceId;
  BOOLEAN                               ImageFound;
  EFI_PHYSICAL_ADDRESS                  Address;
  VOID                                  *Buffer;
  UINTN                                 Index;
  UINTN                                 BiosImageOffset;
  UINTN                                 BiosImageSize;
  UINT8                                 DeviceRecoveryModuleFound;
  BOOLEAN                               FindFv;
  EFI_FIRMWARE_VOLUME_HEADER            *FvHeader;
  UINT32                                FvAlignment;
  UINTN                                 FvHeaderAddress;
  VOID                                  *SetupData;
  UINTN                                 VariableSize;
  UINT8                                 *SupervisorPasswordBuf;
  UINT8                                 *UserPasswordBuf;
  EFI_PEI_FV_HANDLE                     FvHandle;
  EFI_PEI_FILE_HANDLE                   FileHandle;
  EFI_STATUS                            FindDxeCoreStatus;
  EFI_PEI_HOB_POINTERS                  Hob;

  Index                   = 0;
  Status                  = EFI_SUCCESS;
  ProviderAvailable       = TRUE;
  ImageFound              = FALSE;
  NumberOfImageProviders  = 0;
  DeviceRecoveryModule    = NULL;
  DeviceRecoveryModuleFound = FALSE;

  POST_CODE (PEI_ENTER_RECOVERY_MODE);
  DEBUG ((EFI_D_INFO | EFI_D_LOAD, "     ############ PEI Crisis Recovery Entry ############\n\n"));

  while (!DeviceRecoveryModuleFound){
    //
    // Search the platform for some recovery capsule if the DXE IPL
    // discovered a recovery condition and has requested a load.
    //
    Status = PeiServicesLocatePpi (
               &gEfiPeiDeviceRecoveryModulePpiGuid,
               Index,
               NULL,
               (VOID **)&DeviceRecoveryModule
               );

    if (EFI_ERROR (Status)) {
      return Status;
    }

    DEBUG ((EFI_D_INFO | EFI_D_LOAD, "Device Recovery PPI located\n"));

    Status = DeviceRecoveryModule->GetNumberRecoveryCapsules (
                                     PeiServices,
                                     DeviceRecoveryModule,
                                     &NumberRecoveryCapsules
                                     );

    DEBUG ((EFI_D_INFO | EFI_D_LOAD, "Get Number of Recovery Capsules Returns: %r\n", Status));
    DEBUG ((EFI_D_INFO | EFI_D_LOAD, "Number Of Recovery Capsules: %d\n", NumberRecoveryCapsules));

    if (EFI_ERROR(Status) || (NumberRecoveryCapsules == 0)) {
      Index++;
      continue;
    }
    DeviceRecoveryModuleFound = TRUE;
  }
  //
  // If there is an image provider, get the capsule ID
  //
  RecoveryCapsuleSize = 0;

  Status = DeviceRecoveryModule->GetRecoveryCapsuleInfo (
                                   PeiServices,
                                   DeviceRecoveryModule,
                                   0,
                                   &RecoveryCapsuleSize,
                                   &DeviceId
                                   );

  DEBUG ((EFI_D_INFO | EFI_D_LOAD, "Get Recovery Capsule Info Returns: %r\n", Status));
  DEBUG ((EFI_D_INFO | EFI_D_LOAD, "Recovery Capsule Size: %d\n", RecoveryCapsuleSize));

  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Only support the 2 capsule types known
  // Future enhancement is to rank-order the selection
  //
  if ((!CompareGuid (&DeviceId, &gRecoveryOnFatIdeDiskGuid)) &&
      (!CompareGuid (&DeviceId, &gRecoveryOnDataCdGuid)) &&
      (!CompareGuid (&DeviceId, &gRecoveryOnFatUsbDiskGuid))
      ) {
    return EFI_UNSUPPORTED;
  }

  Status = PeiServicesAllocatePages (
             EfiBootServicesCode,
             EFI_SIZE_TO_PAGES (RecoveryCapsuleSize),
             &Address
             );

  DEBUG ((EFI_D_INFO | EFI_D_LOAD, "Allocate buffer for loading recovery capsule Returns: %r\n", Status));

  if (EFI_ERROR (Status)) {
    return Status;
  }

  Buffer = (UINT8 *) (UINTN) Address;

  Status = DeviceRecoveryModule->LoadRecoveryCapsule (
                                   PeiServices,
                                   DeviceRecoveryModule,
                                   0,
                                   Buffer
                                   );

  DEBUG ((EFI_D_INFO | EFI_D_LOAD, "LoadRecoveryCapsule Returns: %r\n", Status));
  if (EFI_ERROR(Status)) {
    return Status;
  }

  if (FeaturePcdGet(PcdUseFastCrisisRecovery)) {
    Status = GetBiosImageFromCapsule (
               (UINT8 *)Buffer,
               RecoveryCapsuleSize,
               &BiosImageOffset,
               &BiosImageSize
               );
    if (EFI_ERROR(Status)) {
      return Status;
    }

    DEBUG ((EFI_D_INFO | EFI_D_LOAD, "Fast Crisis to handoff to Dxe phase preparation.\n"));
    if (PcdGetBool (PcdSecureSysPasswordSupported)) {
      SupervisorPasswordBuf = NULL;
      CommonGetVariableDataAndSize (
        EFI_ADMIN_PASSWORD_NAME,
        &gInsydeSecureFirmwarePasswordGuid,
        &VariableSize,
        (VOID **) &SupervisorPasswordBuf
        );

      if (SupervisorPasswordBuf != NULL) {
        BuildGuidDataHob (
        &gInsydeSecureFirmwarePasswordHobGuid,
        SupervisorPasswordBuf,
        VariableSize
        );
      }
    } else {
      SupervisorPasswordBuf = NULL;
      CommonGetVariableDataAndSize (
        L"SystemSupervisorPw",
        &gEfiSupervisorPwGuid,
        &VariableSize,
        (VOID **) &SupervisorPasswordBuf
        );

      if (SupervisorPasswordBuf != NULL) {
        BuildGuidDataHob (
          &gEfiSupervisorPwHobGuid,
          SupervisorPasswordBuf,
          VariableSize
          );
      }
    }

    UserPasswordBuf = NULL;
    CommonGetVariableDataAndSize (
      L"SystemUserPw",
      &gEfiUserPwGuid,
      &VariableSize,
      (VOID **) &UserPasswordBuf
      );
    if (UserPasswordBuf != NULL) {
      BuildGuidDataHob (
        &gEfiUserPwHobGuid,
        UserPasswordBuf,
        VariableSize
        );
    }

    SetupData = NULL;
    CommonGetVariableDataAndSize (
      L"SetupOrg",
      &gSystemConfigurationGuid,
      &VariableSize,
      (VOID **) &SetupData
      );
    if (SetupData == NULL) {
      SetupData = NULL;
      CommonGetVariableDataAndSize (
        SETUP_VARIABLE_NAME,
        &gSystemConfigurationGuid,
        &VariableSize,
        (VOID **) &SetupData
        );
    }

    if (SetupData != NULL) {
      BuildGuidDataHob (
        &gEfiPowerOnPwSCUHobGuid,
        SetupData,
        VariableSize
        );
    }

    //
    //  OemSvcBootModeCreateFv(); could utilize to create customer's FvHobs
    //
    DEBUG ((EFI_D_INFO | EFI_D_LOAD, "Checking any exist of FvHobs with DxeCore Ffs for DxeIpl handoff.\n"));
    Hob.Raw = GetHobList ();
    while ((Hob.Raw = GetNextHob (EFI_HOB_TYPE_FV, Hob.Raw)) != NULL) {
      FileHandle = NULL;
      FvHandle = (VOID *)(UINTN) (Hob.FirmwareVolume->BaseAddress);
      FindDxeCoreStatus = PeiServicesFfsFindNextFile (EFI_FV_FILETYPE_DXE_CORE, FvHandle, &FileHandle);
      if (!EFI_ERROR (FindDxeCoreStatus)) {
        DEBUG ((EFI_D_INFO | EFI_D_LOAD, "Finding the DxeCore existed in FV at memory address %x \n", (UINTN) (Hob.FirmwareVolume->BaseAddress)));
        return FindDxeCoreStatus;
      }
      Hob.Raw = GET_NEXT_HOB (Hob);
    }

    DEBUG ((EFI_D_INFO | EFI_D_LOAD, "No DxeCore Ffs exist at currently FvHobs.\n"));

    //
    //  Creating FV to HOB. It make the FV visible for DxeIplFindDxeCore().
    //
    DEBUG ((EFI_D_INFO | EFI_D_LOAD, "Creating FvHobs for DxeIpl to handoff from image which read from storage\n"));
    FvHeaderAddress = (UINTN) Buffer;
    do {
      FindFv = FALSE;
      FvHeader = (EFI_FIRMWARE_VOLUME_HEADER*)FvHeaderAddress;
      if (FvHeader->Signature == EFI_FVH_SIGNATURE) {
        //
        // Find the usable of Firmware Volume
        //
        if ((CompareGuid (&gEfiFirmwareFileSystem2Guid, &FvHeader->FileSystemGuid)) ||
            (CompareGuid (&gEfiFirmwareFileSystem3Guid, &FvHeader->FileSystemGuid))) {
          if (FeaturePcdGet (PcdH2OPeiCpCrisisRecoveryPublishFvSupported)) {
            H2O_PEI_CP_CRISIS_RECOVERY_PUBLISH_FV_DATA         CrisisRecoveryPublishFvData;

            CrisisRecoveryPublishFvData.Size     = sizeof (H2O_PEI_CP_CRISIS_RECOVERY_PUBLISH_FV_DATA);
            CrisisRecoveryPublishFvData.Status   = H2O_CP_TASK_NORMAL;
            CrisisRecoveryPublishFvData.FvHeader = FvHeader;
            DEBUG ((DEBUG_INFO, "Checkpoint Trigger: %g\n", &gH2OPeiCpCrisisRecoveryPublishFvGuid));
            H2OCpTrigger (&gH2OPeiCpCrisisRecoveryPublishFvGuid, &CrisisRecoveryPublishFvData);
            DEBUG ((DEBUG_INFO, "Checkpoint Result: %x\n", CrisisRecoveryPublishFvData.Status));
            if (CrisisRecoveryPublishFvData.Status == H2O_CP_TASK_SKIP) {
              FvHeaderAddress += (UINTN)FvHeader->FvLength;
              continue;
            }
          }
          FindFv = TRUE;
          FvAlignment = 1 << ((FvHeader->Attributes & EFI_FVB2_ALIGNMENT) >> 16);
          Address = (EFI_PHYSICAL_ADDRESS)(UINTN) AllocateAlignedPages (EFI_SIZE_TO_PAGES ((UINTN)FvHeader->FvLength), (UINTN)FvAlignment);
          if (Address == 0) {
            DEBUG ((DEBUG_ERROR, "Allocate memory for firmware volume failed!!\n"));
            break;
          }
          CopyMem ((VOID *) (UINTN) Address, (VOID *) FvHeaderAddress, (UINTN) FvHeader->FvLength);
          BuildFvHob (Address, FvHeader->FvLength);
          DEBUG ((EFI_D_INFO | EFI_D_LOAD, "Find the valid FVs at memory address %x \n", FvHeaderAddress));
          FvHeaderAddress += (UINTN)FvHeader->FvLength;
        }
      }
      if (!FindFv) {
        FvHeaderAddress += 0x10;
      }
    } while (((FvHeaderAddress - ((UINTN) Buffer))+ sizeof (EFI_FIRMWARE_VOLUME_HEADER)) < RecoveryCapsuleSize);
  } else {
    Status = FlashBios (Buffer, RecoveryCapsuleSize);
  }

  return Status;
}

/**
  This function will delete the variable if the variable size is different between variable region and variable default
  region.

  Some variable size may be changed in different BIOS version. Using the mismatched variable after updating BIOS
  may cause unpredictable behaviors. To prevent from tis situation so delete this type variables.

  @retval EFI_SUCCESS            Delete all of variable which variable is different between variable region and variable default region.
  @retval EFI_OUT_OF_RESOURCES   There are not enough resource to store variable region data.
  @retval Other                  The request could not be completed by other reason.
**/
STATIC
EFI_STATUS
CheckVariableConsistency (
  VOID
  )
{
  EFI_STATUS                  Status;
  UINTN                       VariableStoreHeaderSize;
  UINTN                       VariableRegionSize;
  UINT8                       *VarRegionBuffer;
  VARIABLE_HEADER             *VariableHeader;
  VARIABLE_HEADER             *DefaultVariableHeader;
  VARIABLE_HEADER             *DefaultVariableHeaderbySkuId0;
  UINTN                       DefaultVariableSize;
  BOOLEAN                     FoundMismatchedVariable;

  //
  // Write back invalid related MMIO address to prevent from reading out-of-date data
  //
  WriteBackInvalidateDataCacheRange (
    (VOID *)(UINTN)FdmGetNAtAddr (&gH2OFlashMapRegionVarGuid, 1),
    (UINTN)FdmGetNAtSize (&gH2OFlashMapRegionVarGuid, 1)
    );
  WriteBackInvalidateDataCacheRange (
    (VOID *)(UINTN) FdmGetNAtAddr (&gH2OFlashMapRegionVarDefaultGuid, 1),
    (UINTN)FdmGetNAtSize (&gH2OFlashMapRegionVarDefaultGuid, 1)
    );
  //
  // Get whole variable region and put in memory buffer.
  //
  VariableRegionSize = (UINTN)FdmGetNAtSize (&gH2OFlashMapRegionVarGuid, 1);
  VarRegionBuffer    = AllocatePages (EFI_SIZE_TO_PAGES (VariableRegionSize));
  if (VarRegionBuffer == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  Status = FlashRead (
             (UINT8 *)VarRegionBuffer,
             (UINT8 *)(UINTN)FdmGetNAtAddr (&gH2OFlashMapRegionVarGuid, 1),
             VariableRegionSize
             );

  if (EFI_ERROR(Status)) {
    FreePages(VarRegionBuffer, EFI_SIZE_TO_PAGES(VariableRegionSize));
    return Status;
  }
  //
  // Get each variable in default variable region. If default variable isn't in current SKU, use it in SKU 0.
  //
  VariableStoreHeaderSize       = sizeof (EFI_FIRMWARE_VOLUME_HEADER) + sizeof (EFI_FV_BLOCK_MAP_ENTRY) + GetVariableStoreHeaderSize ();
  DefaultVariableHeaderbySkuId0 = (VARIABLE_HEADER *)(UINTN)FdmGetNAtAddr (&gH2OFlashMapRegionVarDefaultGuid, 1);
  FoundMismatchedVariable       = FALSE;
  for (DefaultVariableHeader = (VARIABLE_HEADER *)((UINT8 *)DefaultVariableHeaderbySkuId0 + GetVariableStoreHeaderSize ());
       IsValidVariableHeader (DefaultVariableHeader);
       DefaultVariableHeader = GetNextVariablePtr (DefaultVariableHeader)) {
    if (DefaultVariableHeader->State != VAR_ADDED) {
      continue;
    }
    DefaultVariableSize = 0;
    Status = CommonGetDefaultVariable (
                GET_VARIABLE_NAME_PTR (DefaultVariableHeader),
                &DefaultVariableHeader->VendorGuid,
                (UINT8)(LibPcdGetSku()),
                NULL,
                (UINTN *)&DefaultVariableSize,
                NULL
                );
    if (Status != EFI_BUFFER_TOO_SMALL) {
      DefaultVariableSize = DefaultVariableHeader->DataSize;
    }
    //
    // Delete variable if the variable size is different between variable region and variable default region.
    //
    for (VariableHeader = (VARIABLE_HEADER *)(VarRegionBuffer + VariableStoreHeaderSize);
        IsValidVariableHeader (VariableHeader);
        VariableHeader = GetNextVariablePtr (VariableHeader)) {
      if (VariableHeader->State != VAR_ADDED && VariableHeader->State != (VAR_ADDED & VAR_IN_DELETED_TRANSITION)) {
        continue;
      }
      if (StrCmp (GET_VARIABLE_NAME_PTR (VariableHeader), GET_VARIABLE_NAME_PTR (DefaultVariableHeader)) != 0 ||
          !CompareGuid(&VariableHeader->VendorGuid, &DefaultVariableHeader->VendorGuid)) {
        continue;
      }
      if (DefaultVariableSize != DataSizeOfVariable (VariableHeader)) {
        VariableHeader->State &= VAR_DELETED;
        FoundMismatchedVariable = TRUE;
      }
    }
  }
  //
  // Write updated variable region data to variable region if finding any mismatched variable.
  //
  Status = EFI_SUCCESS;
  if (FoundMismatchedVariable) {
    Status = FlashProgram (
               (UINT8 *)(UINTN)FdmGetNAtAddr (&gH2OFlashMapRegionVarGuid, 1),
               (UINT8 *)VarRegionBuffer,
               &VariableRegionSize,
               (UINTN)FdmGetNAtAddr (&gH2OFlashMapRegionVarGuid, 1)
               );
  }
  FreePages(VarRegionBuffer, EFI_SIZE_TO_PAGES(VariableRegionSize));

  return Status;
}

/**
 Flash BIOS image from recovery capsule

 @param [in] RecoveryCapsule            Pointer to recovery capsule
 @param [in] RecoveryCapsuleSize        Recovery capsule file size

 @retval EFI_SUCCESS                    Flash BIOS image successfully
 @return Others                         Unable to flash BIOS image because BIOS image is not found in recovery capsule or
                                        failed to allocate memory
**/
EFI_STATUS
FlashBios (
  IN UINT8                              *RecoveryCapsule,
  IN UINTN                              RecoveryCapsuleSize
  )
{
  EFI_STATUS                            Status;
  UINTN                                 WriteCount;
  UINT16                                ProtectNumCount;
  UINT32                                WriteAddress;
  UINT32                                WriteSize;
  UINT32                                SrcAddress;
  PEI_SPEAKER_IF_PPI                    *SpeakerPPI;
  BOOLEAN                               SpeakerOn;
  BOOLEAN                               DescriptorBIOS;
  EFI_PHYSICAL_ADDRESS                  RomBinFile;
  UINTN                                 NumOfProtectRegion;
  FLASH_ENTRY                           *OemProtectTable;
  UINTN                                 TableCount;
  BOOLEAN                               UseEcIdle;
  BOOLEAN                               UseCrisisProtectTable;
  BOOLEAN                               SkipPeiRegion;
  FLASH_ENTRY                           *ProtectRegion;
  UINTN                                 IsMeProtectRegion;
  UINTN                                 RecoveryBlocks;
  EFI_PEI_SERVICES                      **PeiServices;
  UINT8                                 *ReadBuffer;
  UINT64                                RecoveryFvAddr;
  UINT64                                RecoveryFvSize;
  UINTN                                 DefaultProtectTableSize;
  FLASH_ENTRY                           *DefaultProtectTable;
  EFI_STATUS                            OemSvcStatus;
  UINTN                                 BiosImageOffset;
  UINT8                                 *BiosImage;
  UINTN                                 BiosImageSize;
  UINT32                                 RemainingSize;

  TableCount            = 0;
  NumOfProtectRegion    = 0;
  IsMeProtectRegion     = 0;
  RecoveryBlocks        = 0;
  UseEcIdle             = FALSE;
  OemProtectTable       = NULL;
  ProtectRegion         = NULL;
  SpeakerOn             = FALSE;
  DescriptorBIOS        = FALSE;
  UseCrisisProtectTable = TRUE;
  SkipPeiRegion         = FALSE;
  ReadBuffer            = NULL;
  DefaultProtectTable   = NULL;
  PeiServices = (EFI_PEI_SERVICES **) GetPeiServicesTablePointer();

  WriteSize = GetFlashBlockSize ();

  Status = PeiServicesLocatePpi (
             &gPeiSpeakerInterfacePpiGuid,
             0,
             NULL,
             (VOID **)&SpeakerPPI
             );
  if (!EFI_ERROR (Status)) {
    SpeakerOn = TRUE;
  }

  RecoveryFvAddr = FdmGetNAtAddr (
                     &gH2OFlashMapRegionBootFvGuid,
                     1
                     );

  RecoveryFvSize = FdmGetNAtSize (
                     &gH2OFlashMapRegionBootFvGuid,
                     1
                     );

  Status = GetDefaultProtectRegion (&DefaultProtectTable, &DefaultProtectTableSize);
  ASSERT(DefaultProtectTable);

  Status = EFI_SUCCESS;


  //
  // Get OEM protect Regions
  // (OemServices)
  //
  DEBUG ((DEBUG_INFO, "OemKernelServices Call: OemSvcGetProtectTable \n"));
  OemSvcStatus = OemSvcGetProtectTable (
                   &TableCount,
                   &UseEcIdle,
                   &OemProtectTable
                   );
  DEBUG ((DEBUG_INFO, "OemKernelServices OemSvcGetProtectTable Status: %r\n", OemSvcStatus));

  if (FeaturePcdGet (PcdH2OPeiCpCrisisRecoveryGetProtectTableSupported)) {
    H2O_PEI_CP_CRISIS_RECOVERY_GET_PROTECT_TABLE_DATA     CrisisRecoveryGetProtectTableData;

    CrisisRecoveryGetProtectTableData.Size              = sizeof (H2O_PEI_CP_CRISIS_RECOVERY_GET_PROTECT_TABLE_DATA);
    CrisisRecoveryGetProtectTableData.Status            = H2O_CP_TASK_NORMAL;
    CrisisRecoveryGetProtectTableData.ProtectTable      = OemProtectTable;
    CrisisRecoveryGetProtectTableData.ProtectTableCount = (UINT32) TableCount;

    DEBUG ((DEBUG_INFO, "Checkpoint Trigger: %g\n", &gH2OPeiCpCrisisRecoveryGetProtectTableGuid));
    H2OCpTrigger (&gH2OPeiCpCrisisRecoveryGetProtectTableGuid, &CrisisRecoveryGetProtectTableData);
    DEBUG ((DEBUG_INFO, "Checkpoint Result: %x\n", CrisisRecoveryGetProtectTableData.Status));

    if (CrisisRecoveryGetProtectTableData.Status == H2O_CP_TASK_UPDATE) {
      OemProtectTable = CrisisRecoveryGetProtectTableData.ProtectTable;
      TableCount      = CrisisRecoveryGetProtectTableData.ProtectTableCount;
    }
  }

  if (UseEcIdle) {
    DEBUG ((DEBUG_INFO, "OemKernelServices Call: OemSvcEcIdle \n"));
    OemSvcStatus = OemSvcEcIdle (TRUE);
    DEBUG ((DEBUG_INFO, "OemKernelServices OemSvcEcIdle Status: %r\n", OemSvcStatus));
  }
  PeiCsSvcEnableFdWrites (TRUE);

  if (FeaturePcdGet (PcdH2OPeiCpCrisisRecoveryFlashSupported)) {
    H2O_PEI_CP_CRISIS_RECOVERY_FLASH_DATA         CrisisRecoveryFlashData;

    CrisisRecoveryFlashData.Size                = sizeof (H2O_PEI_CP_CRISIS_RECOVERY_FLASH_DATA);
    CrisisRecoveryFlashData.Status              = H2O_CP_TASK_NORMAL;
    CrisisRecoveryFlashData.RecoveryCapsule     = RecoveryCapsule;
    CrisisRecoveryFlashData.RecoveryCapsuleSize = RecoveryCapsuleSize;

    DEBUG ((DEBUG_INFO, "Checkpoint Trigger: %g\n", &gH2OPeiCpCrisisRecoveryFlashGuid));
    H2OCpTrigger (&gH2OPeiCpCrisisRecoveryFlashGuid, &CrisisRecoveryFlashData);
    DEBUG ((DEBUG_INFO, "Checkpoint Result: %x\n", CrisisRecoveryFlashData.Status));

    if (CrisisRecoveryFlashData.Status == H2O_CP_TASK_SKIP) {
      goto Done;
    } else if (CrisisRecoveryFlashData.Status == H2O_CP_TASK_UPDATE) {
      RecoveryCapsule     = CrisisRecoveryFlashData.RecoveryCapsule;
      RecoveryCapsuleSize = CrisisRecoveryFlashData.RecoveryCapsuleSize;
    }
  }
  Status = GetBiosImageFromCapsule (
             RecoveryCapsule,
             RecoveryCapsuleSize,
             &BiosImageOffset,
             &BiosImageSize
             );
  if (EFI_ERROR(Status)) {
    goto Done;
  }
  BiosImage = RecoveryCapsule + BiosImageOffset;
  POST_CODE (PEI_RECOVERY_LOAD_FILE_DONE);
  DEBUG ((EFI_D_INFO | EFI_D_LOAD, "\n     ######### Flash BIOS #########   \n"));

  //
  // Get onboard BIOS base address
  //
  if (((*((UINT32 *)BiosImage)) == FLVALSIG) || ((*((UINT32 *)((UINT8 *)BiosImage + 0x10))) == FLVALSIG)) {
    WriteAddress   = 0xFFFFFFFF - BiosImageSize + 1;
    DescriptorBIOS = TRUE;
  } else {
    WriteAddress = (UINT32 ) FdmGetBaseAddr ();
  }
  DEBUG ((EFI_D_INFO | EFI_D_LOAD, "Write ROM base address %x \n", WriteAddress));

  WriteSize = GetFlashBlockSize ();
  DEBUG ((EFI_D_INFO | EFI_D_LOAD, "SRC add %x \n", (UINT32) (BiosImageSize - (UINT32) (0x100000000 - WriteAddress))));
  MicroSecondDelay(50);

  //
  //OEM protect table is empty or table return whole ROM size, PEI Crisis default not protect any region
  //
  if ((OemProtectTable == NULL) ||
      (OemProtectTable[0].WriteSize == 0xFFFFFFFF)) {
    //
    // Update whole FD image
    //
    UseCrisisProtectTable = FALSE;
    DEBUG ((EFI_D_INFO | EFI_D_LOAD, "Write whole image\n"));
  } else {
    //
    //Analyze the OEM protect table
    //
    for (ProtectNumCount = 0; ProtectNumCount < (TableCount - 1); ProtectNumCount++) {
      if ((OemProtectTable[ProtectNumCount].WriteAddress < FdmGetBaseAddr ())) {
        IsMeProtectRegion ++;
      }
      if (OemProtectTable[ProtectNumCount].WriteAddress == RecoveryFvAddr) {
        SkipPeiRegion = TRUE;
      }
    }
    if (DescriptorBIOS == FALSE) {
      //
      //All of the OEM protect region are ME region for Non-ME bin file then it will use kernel default protect region.
      //
      if (IsMeProtectRegion == ((TableCount - 1))) {
        if (((DefaultProtectTableSize / sizeof (FLASH_ENTRY)) == 2) && (DefaultProtectTable->WriteAddress == RecoveryFvAddr)) {
          UseCrisisProtectTable = FALSE;
          SkipPeiRegion         = TRUE;
        } else {
          NumOfProtectRegion = (DefaultProtectTableSize / sizeof (FLASH_ENTRY)) - 1;
          ProtectRegion = DefaultProtectTable;
        }
      } else {
        NumOfProtectRegion = TableCount - 1;
        ProtectRegion      = OemProtectTable;
      }
    } else {
      NumOfProtectRegion = TableCount - 1;
      ProtectRegion      = OemProtectTable;
    }
  }

  if (SkipPeiRegion) {
//[-start-190417-IB06070050-modify]//
    //
    // Base address add DXE offset
    //
    WriteAddress = WriteAddress + (PcdGet32(PcdFlashFvMainBase) - PcdGet32(PcdFlashAreaBaseAddress));

    //
    //  SrcAddress need add (PcdGet32(PcdFlashFvMainBase) - PcdGet32(PcdFlashAreaBaseAddress))
    //
    SrcAddress = (UINT32) BiosImage + (UINT32) (BiosImageSize - (UINT32) (0x100000000 - WriteAddress));
//[-end-190417-IB06070050-modify]//
  } else {
    SrcAddress = (UINT32) BiosImage;
    WriteAddress = 0xFFFFFFFF - BiosImageSize + 1;
  }

  if (UseCrisisProtectTable) {
    //
    //Read all of the ROM part bin files
    //
    Status = PeiServicesAllocatePages (
               EfiBootServicesCode,
               EFI_SIZE_TO_PAGES (BiosImageSize),
               &RomBinFile
               );

    if (EFI_ERROR (Status)) {
      goto Done;
    }

    Status = FlashRead (
              (UINT8 *)((UINT32)RomBinFile),
              (UINT8 *)WriteAddress,
              BiosImageSize
              );
    if (EFI_ERROR (Status)) {
      goto Done;
    }

    for (ProtectNumCount = 0; ProtectNumCount < NumOfProtectRegion; ProtectNumCount++) {
      //
      //Non-ME bin file need not to reference ME protect region
      //
      if ((ProtectRegion[ProtectNumCount].WriteAddress < FdmGetBaseAddr ()) && (DescriptorBIOS == FALSE)) {
        continue;
      } else {
        //
        //Non-ME bin file to reference the BIOS protect region
        //
        if (DescriptorBIOS == FALSE) {
          CopyMem (
            (((UINT8 *)SrcAddress) + ProtectRegion[ProtectNumCount].SourceOffset),
            (((UINT8 *)((UINT32)RomBinFile)) + ProtectRegion[ProtectNumCount].SourceOffset),
            ProtectRegion[ProtectNumCount].WriteSize
          );
        } else {
          //
          //Include ME bin file to reference ME protect region
          //
          if (ProtectRegion[ProtectNumCount].WriteAddress < FdmGetBaseAddr ()) {
            CopyMem (
              (((UINT8 *)SrcAddress) + ProtectRegion[ProtectNumCount].SourceOffset),
              (((UINT8 *)((UINT32)RomBinFile)) + ProtectRegion[ProtectNumCount].SourceOffset),
              ProtectRegion[ProtectNumCount].WriteSize
            );
          } else {
            //
            //Include ME bin file to reference BIOS protect region
            //
            CopyMem (
              (((UINT8 *)SrcAddress) + (ProtectRegion[ProtectNumCount].SourceOffset + (BiosImageSize - FdmGetFlashAreaSize()))),
              (((UINT8 *)((UINT32)RomBinFile)) + (ProtectRegion[ProtectNumCount].SourceOffset + (BiosImageSize - FdmGetFlashAreaSize ()))),
              ProtectRegion[ProtectNumCount].WriteSize
            );
          }
        }
      }
    }
  }

  if (SkipPeiRegion) {
    RemainingSize = (UINT32)(BiosImageSize - (UINTN) RecoveryFvSize);
    RecoveryBlocks = ((BiosImageSize - (UINTN) RecoveryFvSize) / WriteSize);    
    RecoveryBlocks = ((BiosImageSize - (UINTN) RecoveryFvSize) % WriteSize) == 0 ? RecoveryBlocks: RecoveryBlocks + 1;
  } else {
    RemainingSize = (UINT32)BiosImageSize;
    RecoveryBlocks = ((BiosImageSize) / WriteSize);
    RecoveryBlocks = ((BiosImageSize) % WriteSize) == 0 ? RecoveryBlocks : RecoveryBlocks + 1;    
  }

  ReadBuffer = AllocatePages(EFI_SIZE_TO_PAGES(WriteSize));
  ASSERT(ReadBuffer != NULL);
  if (ReadBuffer == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto Done;
  }

  for (WriteCount = 0; WriteCount < RecoveryBlocks; WriteCount ++) {
    UINTN Retry;

    POST_CODE (START_FLASH);

    //
    //Make a Beep sound
    //
    if (SpeakerOn && ((WriteCount % 0xF) == 0)) {
      SpeakerPPI->GenerateBeep (1, 500000, 0);
    }

    for (Retry = 0; Retry < FLASH_FAILURE_RETRY_COUNT; Retry++) {
      WriteSize = GetFlashBlockSize();

      if (WriteSize > RemainingSize){
        WriteSize = RemainingSize;
      }

      Status = FlashErase (WriteAddress, WriteSize);

      if (EFI_ERROR (Status)) {
        DEBUG ((EFI_D_INFO | EFI_D_LOAD, "FlashErase     : %r\n", Status));
        continue;
      }

      POST_CODE (ERASE_DONE);

      MicroSecondDelay (50);

      Status = FlashProgram (
                 (UINT8 *)WriteAddress,
                 (UINT8 *)SrcAddress,
                 (UINTN *)&WriteSize,
                 (WriteAddress & ~(0xFFFF))
                 );
      if (EFI_ERROR (Status)) {
        DEBUG ((EFI_D_INFO | EFI_D_LOAD, "FlashProgram     : %r\n", Status));
        continue;
      }

      Status = FlashRead (
                 ReadBuffer,
                 (UINT8 *)WriteAddress,
                 WriteSize
                 );
      if (EFI_ERROR (Status)) {
        DEBUG ((EFI_D_INFO | EFI_D_LOAD, "FlashRead     : %r\n", Status));
        continue;
      }

      if (CompareMem (ReadBuffer, (UINT8 *)SrcAddress, WriteSize) == 0) {
        DEBUG ((EFI_D_INFO | EFI_D_LOAD, "SAME\n"));
        break;
      }
    }

    POST_CODE (PROGRAM_DONE);

    WriteAddress += WriteSize;
    SrcAddress += WriteSize;
    RemainingSize -= WriteSize;

    MicroSecondDelay (50);
  }

  MicroSecondDelay (50);

  Status = EFI_SUCCESS;
  DEBUG ((EFI_D_INFO | EFI_D_LOAD, "Flash BIOS Success and Restart System\n"));
  //
  // The status of "EC idle" and  "Flash device be writable" should be cleared, when error occur.
  //
Done:
  CheckVariableConsistency ();
  ClearFirmwareUpdatingFlag();
  if (ReadBuffer != NULL) {
    FreePages(ReadBuffer, EFI_SIZE_TO_PAGES(WriteSize));
  }
  if (UseEcIdle) {
    DEBUG ((DEBUG_INFO, "OemKernelServices Call: OemSvcEcIdle \n"));
    OemSvcStatus = OemSvcEcIdle (FALSE);
    DEBUG ((DEBUG_INFO, "OemKernelServices OemSvcEcIdle Status: %r\n", OemSvcStatus));
  }

  PeiCsSvcEnableFdWrites (FALSE);

  if (EFI_ERROR (Status)) {
    return Status;
  }

  DEBUG ((DEBUG_INFO, "OemKernelServices Call: OemSvcPeiCrisisRecoveryReset \n"));
  Status = OemSvcPeiCrisisRecoveryReset ();
  DEBUG ((DEBUG_INFO, "OemKernelServices OemSvcPeiCrisisRecoveryReset Status: %r\n", Status));
  if (Status == EFI_SUCCESS) {
    return EFI_SUCCESS;
  }
  (*PeiServices)->ResetSystem ((CONST EFI_PEI_SERVICES **)PeiServices);

  //
  // Wait For shutdown
  //
  CpuDeadLoop ();
  return EFI_SUCCESS;
}
