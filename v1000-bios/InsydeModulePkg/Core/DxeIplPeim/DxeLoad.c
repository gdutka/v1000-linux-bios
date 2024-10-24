/** @file
  Last PEIM.
  Responsibility of this module is to load the DXE Core from a Firmware Volume.
;******************************************************************************
;* Copyright (c) 2013 - 2019, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
**/
/** @file
  Last PEIM.
  Responsibility of this module is to load the DXE Core from a Firmware Volume.

Copyright (c) 2006 - 2012, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "DxeIpl.h"
#include <Uefi.h>
#include <Ppi/RomCorrupt.h>
#include <Guid/H2OCp.h>
#include <Library/H2OCpLib.h>
#include <Library/VariableLib.h>
#include <Library/PeiOemSvcKernelLib.h>
#include <Library/HobLib.h>
#ifndef BACKUP_BIOS
#include <Library/FdSupportLib.h>
#include <Library/ResetSystemLib.h>
#endif
#include <Library/IoLib.h>
#include <Library/CmosLib.h>
#include <PostCode.h>

#define FAIL_REASON                            0x7B
#define FAIL_COUNTS                            0x7A
#define FAIL_CODE                              0xFF00
#define WATCHDOG                               0x01
#define CHECKSUM                               0x10

#ifndef BACKUP_BIOS
#define CRC32                                  0x7C

#define WatchdogTimerBase                      (UINT32)PcdGet32(PcdWatchdogTimerBase)
#define WatchdogControl                        0x00
#define WatchdogCounter                        0x04
#define WatchdogFired                          BIT1
#define WatchdogTimerEnable                    0x81
#define Minutes                                60

#define BackupImageBase                        0x00800000
#define ROMSIG_HEADER_FILED_NUM                0
#define ROMSIG_XHCI_FILED_NUM                  3
#define ROMSIG_PSP_DIRECTORY_FIELD_NUM         5
#define ROMSIG_BIOS_DIRECTORY_FIELD_NUM        7
#endif

//
// Module Globals used in the DXE to PEI hand off
// These must be module globals, so the stack can be switched
//
CONST EFI_DXE_IPL_PPI mDxeIplPpi = {
  DxeLoadCore
};

CONST EFI_PEI_GUIDED_SECTION_EXTRACTION_PPI mCustomGuidedSectionExtractionPpi = {
  CustomGuidedSectionExtract
};

CONST EFI_PEI_DECOMPRESS_PPI mDecompressPpi = {
  Decompress
};

CONST EFI_PEI_PPI_DESCRIPTOR mPpiList[] = {
  {
    EFI_PEI_PPI_DESCRIPTOR_PPI,
    &gEfiDxeIplPpiGuid,
    (VOID *) &mDxeIplPpi
  },
  {
    (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gEfiPeiDecompressPpiGuid,
    (VOID *) &mDecompressPpi
  }
};

CONST EFI_PEI_PPI_DESCRIPTOR gEndOfPeiSignalPpi = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gEfiEndOfPeiSignalPpiGuid,
  NULL
};

CONST EFI_PEI_PPI_DESCRIPTOR mPpiRomCorrupt = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gPeiRomCorruptPpiGuid,
  NULL
};

/**
 Install End of PEI Phase PPI.

 @retval EFI_SUCESS  Install End of PEI Phase PPI successfully.
 @retval Others      Some error returned from PPI installation function.
**/
EFI_STATUS
InstallEndOfPeiPhasePpi (
  VOID
  )
{
  EFI_STATUS             Status;

  if (FeaturePcdGet (PcdH2OPeiCpEndOfPeiBeforeSupported)) {
    H2O_PEI_CP_END_OF_PEI_BEFORE_DATA                     CpEndOfPeiBeforeData;

    CpEndOfPeiBeforeData.Size   = sizeof (H2O_PEI_CP_END_OF_PEI_BEFORE_DATA);
    CpEndOfPeiBeforeData.Status = H2O_CP_TASK_NORMAL;

    DEBUG ((DEBUG_INFO, "Checkpoint Trigger: %g\n", &gH2OPeiCpEndOfPeiBeforeGuid));
    H2OCpTrigger (&gH2OPeiCpEndOfPeiBeforeGuid, &CpEndOfPeiBeforeData);
    DEBUG ((DEBUG_INFO, "Checkpoint Result: %x\n", CpEndOfPeiBeforeData.Status));
  }

  Status = PeiServicesInstallPpi (&gEndOfPeiSignalPpi);

  if (FeaturePcdGet (PcdH2OPeiCpEndOfPeiAfterSupported)) {
    H2O_PEI_CP_END_OF_PEI_AFTER_DATA                     CpEndOfPeiAfterData;

    CpEndOfPeiAfterData.Size   = sizeof (H2O_PEI_CP_END_OF_PEI_AFTER_DATA);
    CpEndOfPeiAfterData.Status = H2O_CP_TASK_NORMAL;

    DEBUG ((DEBUG_INFO, "Checkpoint Trigger: %g\n", &gH2OPeiCpEndOfPeiAfterGuid));
    H2OCpTrigger (&gH2OPeiCpEndOfPeiAfterGuid, &CpEndOfPeiAfterData);
    DEBUG ((DEBUG_INFO, "Checkpoint Result: %x\n", CpEndOfPeiAfterData.Status));
  }

  return Status;
}

/**
  Entry point of DXE IPL PEIM.

  This function installs DXE IPL PPI and Decompress PPI.  It also reloads
  itself to memory on non-S3 resume boot path.

  @param  FileHandle  Handle of the file being invoked.
  @param  PeiServices Describes the list of possible PEI Services.

  @retval EFI_SUCESS  The entry point of DXE IPL PEIM executes successfully.
  @retval Others      Some error occurs during the execution of this function.

**/
EFI_STATUS
EFIAPI
PeimInitializeDxeIpl (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS                                Status;
  EFI_BOOT_MODE                             BootMode;
  EFI_GUID                                  *ExtractHandlerGuidTable;
  UINTN                                     ExtractHandlerNumber;
  EFI_PEI_PPI_DESCRIPTOR                    *GuidPpi;

  BootMode = GetBootModeHob ();

  if (BootMode != BOOT_ON_S3_RESUME) {
    Status = PeiServicesRegisterForShadow (FileHandle);
    if (Status == EFI_SUCCESS) {
      //
      // EFI_SUCESS means it is the first time to call register for shadow.
      //
      return Status;
    }

    //
    // Ensure that DXE IPL is shadowed to permanent memory.
    //
    ASSERT (Status == EFI_ALREADY_STARTED);
  }

  //
  // Get custom extract guided section method guid list
  //
  ExtractHandlerNumber = ExtractGuidedSectionGetGuidList (&ExtractHandlerGuidTable);

  //
  // Install custom extraction guid PPI
  //
  if (ExtractHandlerNumber > 0) {
    GuidPpi = (EFI_PEI_PPI_DESCRIPTOR *) AllocatePool (ExtractHandlerNumber * sizeof (EFI_PEI_PPI_DESCRIPTOR));
    ASSERT (GuidPpi != NULL);
    if (GuidPpi == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
    while (ExtractHandlerNumber-- > 0) {
      GuidPpi->Flags = EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST;
      GuidPpi->Ppi   = (VOID *) &mCustomGuidedSectionExtractionPpi;
      GuidPpi->Guid  = &ExtractHandlerGuidTable[ExtractHandlerNumber];
      Status = PeiServicesInstallPpi (GuidPpi++);
      ASSERT_EFI_ERROR(Status);
    }
  }

  //
  // Install DxeIpl and Decompress PPIs.
  //
  Status = PeiServicesInstallPpi (mPpiList);
  ASSERT_EFI_ERROR(Status);

  return Status;
}

/**
   Validate variable data for the MemoryTypeInformation.

   @param MemoryData       Variable data.
   @param MemoryDataSize   Variable data length.

   @return TRUE            The variable data is valid.
   @return FALSE           The variable data is invalid.

**/
BOOLEAN
ValidateMemoryTypeInfoVariable (
  IN EFI_MEMORY_TYPE_INFORMATION      *MemoryData,
  IN UINTN                            MemoryDataSize
  )
{
  UINTN                       Count;
  UINTN                       Index;

  // Check the input parameter.
  if (MemoryData == NULL) {
    return FALSE;
  }

  // Get Count
  Count = MemoryDataSize / sizeof (*MemoryData);

  // Check Size
  if (Count * sizeof(*MemoryData) != MemoryDataSize) {
    return FALSE;
  }

  // Check last entry type filed.
  if (MemoryData[Count - 1].Type != EfiMaxMemoryType) {
    return FALSE;
  }

  // Check the type filed.
  for (Index = 0; Index < Count - 1; Index++) {
    if (MemoryData[Index].Type >= EfiMaxMemoryType) {
      return FALSE;
    }
  }

  return TRUE;
}

/**
   Main entry point to last PEIM.

   This function finds DXE Core in the firmware volume and transfer the control to
   DXE core.

   @param This          Entry point for DXE IPL PPI.
   @param PeiServices   General purpose services available to every PEIM.
   @param HobList       Address to the Pei HOB list.

   @return EFI_SUCCESS              DXE core was successfully loaded.
   @return EFI_OUT_OF_RESOURCES     There are not enough resources to load DXE core.

**/
EFI_STATUS
EFIAPI
DxeLoadCore (
  IN CONST EFI_DXE_IPL_PPI *This,
  IN EFI_PEI_SERVICES      **PeiServices,
  IN EFI_PEI_HOB_POINTERS  HobList
  )
{
  EFI_STATUS                                Status;
  EFI_FV_FILE_INFO                          DxeCoreFileInfo;
  EFI_PHYSICAL_ADDRESS                      DxeCoreAddress;
  UINT64                                    DxeCoreSize;
  EFI_PHYSICAL_ADDRESS                      DxeCoreEntryPoint;
  EFI_BOOT_MODE                             BootMode;
  EFI_PEI_FILE_HANDLE                       FileHandle;
  EFI_PEI_LOAD_FILE_PPI                     *LoadFile;
  UINTN                                     Instance;
  UINT32                                    AuthenticationState;
  UINTN                                     DataSize;
  EFI_PEI_S3_RESUME2_PPI                    *S3Resume;
  EFI_PEI_RECOVERY_MODULE_PPI               *PeiRecovery;
  EFI_MEMORY_TYPE_INFORMATION               MemoryData[EfiMaxMemoryType + 1];
  EFI_STATUS                                OemSvcStatus;

#ifndef BACKUP_BIOS
#ifndef EFI_DEBUG
  UINT32  RomSigData[8] = {0,0,0,0,0,0,0,0};
  UINT8   *Dest;
  UINT8   *Src;
  UINTN   NumBytes;
  UINT8   Counts;

  DEBUG ((EFI_D_ERROR, "PcdFlashFvMainCRCBase %08X, CRC32 %08X\n", MmioRead32(PcdGet32(PcdFlashFvMainCRCBase)), ReadCmos32(CRC32)));

  if((MmioRead32(WatchdogTimerBase) && WatchdogFired) || (MmioRead32(PcdGet32(PcdFlashFvMainCRCBase)) != ReadCmos32(CRC32))) {

    if(MmioRead32(WatchdogTimerBase) && WatchdogFired) {
      Counts = ReadCmos8(FAIL_COUNTS);
      Counts++;
      WriteCmos8(FAIL_COUNTS, Counts);
      if(Counts > 1) {
        WriteCmos8(FAIL_REASON, WATCHDOG); 
      } else {
        MmioWrite32(WatchdogTimerBase + WatchdogControl, WatchdogTimerEnable);
        MmioWrite32(WatchdogTimerBase + WatchdogCounter, 2 * Minutes);
        MmioWrite32(WatchdogTimerBase + WatchdogControl, WatchdogTimerEnable);
      }
    } 
    
    if((MmioRead32(PcdGet32(PcdFlashFvMainCRCBase)) != ReadCmos32(CRC32))) {
      WriteCmos8(FAIL_REASON, CHECKSUM); 
    }

    if (ReadCmos8(FAIL_REASON) != 0) {
      Status = FlashErase(
                        (PcdGet32 (PcdFlashFvSigBase) & ~(0xFFFF)), 
                        GetFlashBlockSize()
                        );
      if (EFI_ERROR(Status)) {
        DEBUG ((EFI_D_ERROR, "FlashErase() failed: %r\n", Status));
      }

      RomSigData[ROMSIG_HEADER_FILED_NUM] = 0x55AA55AA;
      RomSigData[ROMSIG_XHCI_FILED_NUM] = PcdGet32(PcdFlashUsbFirmwareBase) + BackupImageBase;
      RomSigData[ROMSIG_PSP_DIRECTORY_FIELD_NUM] = PcdGet32(PcdFlashRVPspDirBase) + BackupImageBase;
      RomSigData[ROMSIG_BIOS_DIRECTORY_FIELD_NUM] = PcdGet32(PcdFlashRVBiosDirBase) + BackupImageBase;
  
      NumBytes = sizeof(RomSigData);
      Dest = (UINT8 *)(UINTN)(PcdGet32 (PcdFlashFvSigBase));
      Src  = (UINT8 *)(UINTN) RomSigData;
      
      Status = FlashProgram (
                          Dest,
                          Src,
                          &NumBytes,
                          (PcdGet32 (PcdFlashFvSigBase) & ~(0xFFFF))
                          );
      *Dest = 0;
      if (EFI_ERROR (Status)) {
        DEBUG ((EFI_D_ERROR, "FlashProgram() failed: %r\n", Status));
      }

      ResetCold();
    }
  } else {
	MmioWrite32(0xfed81520, MmioRead32(0xfed81520)& 0xFFBFFFFF);
    MmioWrite32(WatchdogTimerBase + WatchdogControl, WatchdogTimerEnable);
    MmioWrite32(WatchdogTimerBase + WatchdogCounter, 2 * Minutes);
    MmioWrite32(WatchdogTimerBase + WatchdogControl, WatchdogTimerEnable);
  }
#endif
#else
  POST_CODE (0xFFFF);
  if (ReadCmos8(FAIL_REASON) == WATCHDOG) {
    POST_CODE (FAIL_CODE + 0x0D);
    POST_CODE (FAIL_CODE + 0x00);
    POST_CODE (FAIL_CODE + 0x09);
  }
  if (ReadCmos8(FAIL_REASON) == CHECKSUM) {
    POST_CODE (FAIL_CODE + 0x05);
    POST_CODE (FAIL_CODE + 0x0C);
    POST_CODE (FAIL_CODE + 0x03);
  }
  POST_CODE (0xFFFF);

  WriteCmos8(FAIL_COUNTS, 0x00); 
  WriteCmos8(FAIL_REASON, 0x00);
#endif

  //
  // if in S3 Resume, restore configure
  //
  BootMode = GetBootModeHob ();

  if (BootMode == BOOT_ON_S3_RESUME) {
    Status = PeiServicesLocatePpi (
               &gEfiPeiS3Resume2PpiGuid,
               0,
               NULL,
               (VOID **) &S3Resume
               );
    if (EFI_ERROR (Status)) {
      //
      // Report Status code that S3Resume PPI can not be found
      //
      REPORT_STATUS_CODE (
        EFI_ERROR_CODE | EFI_ERROR_MAJOR,
        (EFI_SOFTWARE_PEI_MODULE | EFI_SW_PEI_EC_S3_RESUME_PPI_NOT_FOUND)
        );
    }
    ASSERT_EFI_ERROR (Status);

    Status = S3Resume->S3RestoreConfig2 (S3Resume);
    ASSERT_EFI_ERROR (Status);
  } else if (BootMode == BOOT_IN_RECOVERY_MODE) {
    REPORT_STATUS_CODE (EFI_PROGRESS_CODE, (EFI_SOFTWARE_PEI_MODULE | EFI_SW_PEI_PC_RECOVERY_BEGIN));
    Status = PeiServicesLocatePpi (
               &gEfiPeiRecoveryModulePpiGuid,
               0,
               NULL,
               (VOID **) &PeiRecovery
               );

    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "Locate Recovery PPI Failed.(Status = %r)\n", Status));
      //
      // Report Status code the failure of locating Recovery PPI
      //
      REPORT_STATUS_CODE (
        EFI_ERROR_CODE | EFI_ERROR_MAJOR,
        (EFI_SOFTWARE_PEI_MODULE | EFI_SW_PEI_EC_RECOVERY_PPI_NOT_FOUND)
        );
     DEBUG ((DEBUG_INFO, "OemKernelServices Call: OemSvcHookWhenRecoveryFail \n"));
     OemSvcStatus = OemSvcHookWhenRecoveryFail ();
     DEBUG ((DEBUG_INFO, "OemKernelServices OemSvcHookWhenRecoveryFail Status: %r\n", OemSvcStatus));
     if (EFI_ERROR (OemSvcStatus)) {
       (*PeiServices)->ResetSystem ((CONST EFI_PEI_SERVICES **)PeiServices);
     }
    }

    REPORT_STATUS_CODE (EFI_PROGRESS_CODE, (EFI_SOFTWARE_PEI_MODULE | EFI_SW_PEI_PC_CAPSULE_LOAD));
    Status = PeiRecovery->LoadRecoveryCapsule (PeiServices, PeiRecovery);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "Load Recovery Capsule Failed.(Status = %r)\n", Status));
      //
      // Report Status code that recovery image can not be found
      //
      REPORT_STATUS_CODE (
        EFI_ERROR_CODE | EFI_ERROR_MAJOR,
        (EFI_SOFTWARE_PEI_MODULE | EFI_SW_PEI_EC_NO_RECOVERY_CAPSULE)
        );
      DEBUG ((DEBUG_INFO, "OemKernelServices Call: OemSvcHookWhenRecoveryFail \n"));
      OemSvcStatus = OemSvcHookWhenRecoveryFail ();
      DEBUG ((DEBUG_INFO, "OemKernelServices OemSvcHookWhenRecoveryFail Status: %r\n", OemSvcStatus));
      if (EFI_ERROR (OemSvcStatus)) {
        (*PeiServices)->ResetSystem ((CONST EFI_PEI_SERVICES **)PeiServices);
      }
    }
    REPORT_STATUS_CODE (EFI_PROGRESS_CODE, (EFI_SOFTWARE_PEI_MODULE | EFI_SW_PEI_PC_CAPSULE_START));
    //
    // Now should have a HOB with the DXE core
    //
  }
  DataSize = sizeof (MemoryData);
  Status = CommonGetVariable (
             EFI_MEMORY_TYPE_INFORMATION_VARIABLE_NAME,
             &gEfiMemoryTypeInformationGuid,
             &DataSize,
             &MemoryData
             );
  if (!EFI_ERROR (Status) && ValidateMemoryTypeInfoVariable (MemoryData, DataSize)) {
    //
    // Build the GUID'd HOB for DXE
    //
    BuildGuidDataHob (
      &gEfiMemoryTypeInformationGuid,
      MemoryData,
      DataSize
      );
  }

  //
  // Look in all the FVs present in PEI and find the DXE Core FileHandle
  //
  FileHandle = DxeIplFindDxeCore ();

  if (FileHandle == NULL) {
    Status = PeiServicesInstallPpi (&mPpiRomCorrupt);
  }
  //
  // Load the DXE Core from a Firmware Volume.
  //
  Instance = 0;
  do {
    Status = PeiServicesLocatePpi (&gEfiPeiLoadFilePpiGuid, Instance++, NULL, (VOID **) &LoadFile);
    //
    // These must exist an instance of EFI_PEI_LOAD_FILE_PPI to support to load DxeCore file handle successfully.
    //
    ASSERT_EFI_ERROR (Status);

    Status = LoadFile->LoadFile (
                         LoadFile,
                         FileHandle,
                         &DxeCoreAddress,
                         &DxeCoreSize,
                         &DxeCoreEntryPoint,
                         &AuthenticationState
                         );
  } while (EFI_ERROR (Status));

  //
  // Get the DxeCore File Info from the FileHandle for the DxeCore GUID file name.
  //
  Status = PeiServicesFfsGetFileInfo (FileHandle, &DxeCoreFileInfo);
  ASSERT_EFI_ERROR (Status);

  //
  // Add HOB for the DXE Core
  //
  BuildModuleHob (
    &DxeCoreFileInfo.FileName,
    DxeCoreAddress,
    ALIGN_VALUE (DxeCoreSize, EFI_PAGE_SIZE),
    DxeCoreEntryPoint
    );

  //
  // Report Status Code EFI_SW_PEI_PC_HANDOFF_TO_NEXT
  //
  REPORT_STATUS_CODE (EFI_PROGRESS_CODE, (EFI_SOFTWARE_PEI_CORE | EFI_SW_PEI_CORE_PC_HANDOFF_TO_NEXT));

  DEBUG ((DEBUG_INFO | DEBUG_LOAD, "Loading DXE CORE at 0x%11p EntryPoint=0x%11p\n", (VOID *)(UINTN)DxeCoreAddress, FUNCTION_ENTRY_POINT (DxeCoreEntryPoint)));

  //
  // Transfer control to the DXE Core
  // The hand off state is simply a pointer to the HOB list
  //
  HandOffToDxeCore (DxeCoreEntryPoint, HobList);
  //
  // If we get here, then the DXE Core returned.  This is an error
  // DxeCore should not return.
  //
  ASSERT (FALSE);
  CpuDeadLoop ();

  return EFI_OUT_OF_RESOURCES;
}


/**
  Install FvInfoPpi For FV HOBs 

  @param Hob                    FV Hob
  
  @retval EFI_SUCCESS          Install FvInfo Ppi for this FV HOB.  

  @retval EFI_ALREADY_STARTED  FvInfo Ppi has been installed for this FV HOB before.
  
**/
EFI_STATUS
InstallFvInfoPpiForFvHob (
  EFI_PEI_HOB_POINTERS  Hob
)
{

  UINTN                             FvInfoInstance;
  EFI_STATUS                        Status;
  BOOLEAN                           IsFvInoPpiHasInstalled; 
  EFI_PEI_FIRMWARE_VOLUME_INFO_PPI  *FvInfoPpi;
  EFI_FIRMWARE_VOLUME_HEADER        *FvHeader;

  FvInfoInstance = 0;  
  IsFvInoPpiHasInstalled = FALSE;

  while (TRUE){
    Status = PeiServicesLocatePpi (&gEfiPeiFirmwareVolumeInfoPpiGuid, FvInfoInstance, NULL, &FvInfoPpi);
    if (EFI_ERROR(Status)){
      break;
    }

    if ((Hob.FirmwareVolume->BaseAddress == (UINT64)(UINTN) FvInfoPpi->FvInfo) && (FvInfoPpi->FvInfoSize == (UINT32) Hob.FirmwareVolume->Length)){
      IsFvInoPpiHasInstalled = TRUE;
      break;
    }
    FvInfoInstance++;
  }

  if (!IsFvInoPpiHasInstalled){
    FvHeader = (EFI_FIRMWARE_VOLUME_HEADER*)(UINTN) Hob.FirmwareVolume->BaseAddress;
    PeiServicesInstallFvInfoPpi (
      &FvHeader->FileSystemGuid,
      (VOID*) (UINTN) Hob.FirmwareVolume->BaseAddress,
      (UINT32) Hob.FirmwareVolume->Length,
      NULL,
      NULL
      );
    return EFI_SUCCESS;
  }
  return EFI_ALREADY_STARTED;

}


/**
   Searches DxeCore in all firmware Volumes and loads the first
   instance that contains DxeCore.

   @return FileHandle of DxeCore to load DxeCore.

**/
EFI_PEI_FILE_HANDLE
DxeIplFindDxeCore (
  VOID
  )
{
  EFI_STATUS            Status;
  UINTN                 Instance;
  EFI_PEI_FV_HANDLE     VolumeHandle;
  EFI_PEI_FILE_HANDLE   FileHandle;
  EFI_PEI_HOB_POINTERS  Hob;
  EFI_PEI_FV_HANDLE     FvHandle;

  Instance    = 0;
  while (TRUE) {
    //
    // Traverse all firmware volume instances
    //
    Status = PeiServicesFfsFindNextVolume (Instance, (VOID **)&VolumeHandle);
    //
    // If some error occurs here, then we cannot find any firmware
    // volume that may contain DxeCore.
    //
    if (EFI_ERROR (Status)) {
      break;
    }
    ASSERT_EFI_ERROR (Status);

    //
    // Find the DxeCore file type from the beginning in this firmware volume.
    //
    FileHandle = NULL;
    Status = PeiServicesFfsFindNextFile (EFI_FV_FILETYPE_DXE_CORE, VolumeHandle, (VOID **)&FileHandle);
    if (!EFI_ERROR (Status)) {
      //
      // Find DxeCore FileHandle in this volume, then we skip other firmware volume and
      // return the FileHandle.
      //
      return FileHandle;
    }
    //
    // We cannot find DxeCore in this firmware volume, then search the next volume.
    //
    Instance++;
  }
  
  //
  // Install FvInfo Ppi for FV HOBs
  //
  Hob.Raw = GetHobList ();
  while ((Hob.Raw = GetNextHob (EFI_HOB_TYPE_FV, Hob.Raw)) != NULL) {
    FvHandle = (VOID *)(UINTN) (Hob.FirmwareVolume->BaseAddress);
    Status = InstallFvInfoPpiForFvHob (Hob);
    Hob.Raw = GET_NEXT_HOB (Hob);
  }

  Hob.Raw = GetHobList ();
  while ((Hob.Raw = GetNextHob (EFI_HOB_TYPE_FV2, Hob.Raw)) != NULL) {
    FvHandle = (VOID *)(UINTN) (Hob.FirmwareVolume2->BaseAddress);
    Status = InstallFvInfoPpiForFvHob (Hob);
    Hob.Raw = GET_NEXT_HOB (Hob);
  }

  Hob.Raw = GetHobList ();
  while ((Hob.Raw = GetNextHob (EFI_HOB_TYPE_FV3, Hob.Raw)) != NULL) {
    FvHandle = (VOID *)(UINTN) (Hob.FirmwareVolume3->BaseAddress);
    Status = InstallFvInfoPpiForFvHob (Hob);
    Hob.Raw = GET_NEXT_HOB (Hob);
  }

  //
  // Try to find DxeCore again
  //
  while (TRUE) {
    //
    // Traverse all firmware volume instances
    //
    Status = PeiServicesFfsFindNextVolume (Instance, (VOID **)&VolumeHandle);
    //
    // If some error occurs here, then we cannot find any firmware
    // volume that may contain DxeCore.
    //
    if (EFI_ERROR (Status)) {
      break;
    }
    ASSERT_EFI_ERROR (Status);

    //
    // Find the DxeCore file type from the beginning in this firmware volume.
    //
    FileHandle = NULL;
    Status = PeiServicesFfsFindNextFile (EFI_FV_FILETYPE_DXE_CORE, VolumeHandle, (VOID **)&FileHandle);
    if (!EFI_ERROR (Status)) {
      //
      // Find DxeCore FileHandle in this volume, then we skip other firmware volume and
      // return the FileHandle.
      //
      return FileHandle;
    }
    //
    // We cannot find DxeCore in this firmware volume, then search the next volume.
    //
    Instance++;
  }  

  REPORT_STATUS_CODE (EFI_PROGRESS_CODE, (EFI_SOFTWARE_PEI_MODULE | EFI_SW_PEI_CORE_EC_DXE_CORRUPT));
  FileHandle = NULL;
  return FileHandle;
}



/**
  The ExtractSection() function processes the input section and
  returns a pointer to the section contents. If the section being
  extracted does not require processing (if the section
  GuidedSectionHeader.Attributes has the
  EFI_GUIDED_SECTION_PROCESSING_REQUIRED field cleared), then
  OutputBuffer is just updated to point to the start of the
  section's contents. Otherwise, *Buffer must be allocated
  from PEI permanent memory.

  @param This                   Indicates the
                                EFI_PEI_GUIDED_SECTION_EXTRACTION_PPI instance.
                                Buffer containing the input GUIDed section to be
                                processed. OutputBuffer OutputBuffer is
                                allocated from PEI permanent memory and contains
                                the new section stream.
  @param InputSection           A pointer to the input buffer, which contains
                                the input section to be processed.
  @param OutputBuffer           A pointer to a caller-allocated buffer, whose
                                size is specified by the contents of OutputSize.
  @param OutputSize             A pointer to a caller-allocated
                                UINTN in which the size of *OutputBuffer
                                allocation is stored. If the function
                                returns anything other than EFI_SUCCESS,
                                the value of OutputSize is undefined.
  @param AuthenticationStatus   A pointer to a caller-allocated
                                UINT32 that indicates the
                                authentication status of the
                                output buffer. If the input
                                section's GuidedSectionHeader.
                                Attributes field has the
                                EFI_GUIDED_SECTION_AUTH_STATUS_VALID
                                bit as clear,
                                AuthenticationStatus must return
                                zero. These bits reflect the
                                status of the extraction
                                operation. If the function
                                returns anything other than
                                EFI_SUCCESS, the value of
                                AuthenticationStatus is
                                undefined.

  @retval EFI_SUCCESS           The InputSection was
                                successfully processed and the
                                section contents were returned.

  @retval EFI_OUT_OF_RESOURCES  The system has insufficient
                                resources to process the request.

  @retval EFI_INVALID_PARAMETER The GUID in InputSection does
                                not match this instance of the
                                GUIDed Section Extraction PPI.

**/
EFI_STATUS
EFIAPI
CustomGuidedSectionExtract (
  IN CONST  EFI_PEI_GUIDED_SECTION_EXTRACTION_PPI *This,
  IN CONST  VOID                                  *InputSection,
  OUT       VOID                                  **OutputBuffer,
  OUT       UINTN                                 *OutputSize,
  OUT       UINT32                                *AuthenticationStatus
)
{
  EFI_STATUS      Status;
  UINT8           *ScratchBuffer;
  UINT32          ScratchBufferSize;
  UINT32          OutputBufferSize;
  UINT16          SectionAttribute;

  //
  // Init local variable
  //
  ScratchBuffer = NULL;

  //
  // Call GetInfo to get the size and attribute of input guided section data.
  //
  Status = ExtractGuidedSectionGetInfo (
             InputSection,
             &OutputBufferSize,
             &ScratchBufferSize,
             &SectionAttribute
             );

  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "GetInfo from guided section Failed - %r\n", Status));
    return Status;
  }

  if (ScratchBufferSize != 0) {
    //
    // Allocate scratch buffer
    //
    ScratchBuffer = AllocatePages (EFI_SIZE_TO_PAGES (ScratchBufferSize));
    if (ScratchBuffer == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
  }

  if (((SectionAttribute & EFI_GUIDED_SECTION_PROCESSING_REQUIRED) != 0) && OutputBufferSize > 0) {
    //
    // Allocate output buffer
    //
    *OutputBuffer = AllocatePages (EFI_SIZE_TO_PAGES (OutputBufferSize) + 1);
    if (*OutputBuffer == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
    DEBUG ((DEBUG_INFO, "Customized Guided section Memory Size required is 0x%x and address is 0x%p\n", OutputBufferSize, *OutputBuffer));
    //
    // *OutputBuffer still is one section. Adjust *OutputBuffer offset,
    // skip EFI section header to make section data at page alignment.
    //
    *OutputBuffer = (VOID *)((UINT8 *) *OutputBuffer + EFI_PAGE_SIZE - sizeof (EFI_COMMON_SECTION_HEADER));
  }

  Status = ExtractGuidedSectionDecode (
             InputSection,
             OutputBuffer,
             ScratchBuffer,
             AuthenticationStatus
             );
  if (EFI_ERROR (Status)) {
    //
    // Decode failed
    //
    DEBUG ((DEBUG_ERROR, "Extract guided section Failed - %r\n", Status));
    return Status;
  }

  *OutputSize = (UINTN) OutputBufferSize;

  return EFI_SUCCESS;
}



/**
   Decompresses a section to the output buffer.

   This function looks up the compression type field in the input section and
   applies the appropriate compression algorithm to compress the section to a
   callee allocated buffer.

   @param  This                  Points to this instance of the
                                 EFI_PEI_DECOMPRESS_PEI PPI.
   @param  CompressionSection    Points to the compressed section.
   @param  OutputBuffer          Holds the returned pointer to the decompressed
                                 sections.
   @param  OutputSize            Holds the returned size of the decompress
                                 section streams.

   @retval EFI_SUCCESS           The section was decompressed successfully.
                                 OutputBuffer contains the resulting data and
                                 OutputSize contains the resulting size.

**/
EFI_STATUS
EFIAPI
Decompress (
  IN CONST  EFI_PEI_DECOMPRESS_PPI  *This,
  IN CONST  EFI_COMPRESSION_SECTION *CompressionSection,
  OUT       VOID                    **OutputBuffer,
  OUT       UINTN                   *OutputSize
 )
{
  EFI_STATUS                      Status;
  UINT8                           *DstBuffer;
  UINT8                           *ScratchBuffer;
  UINT32                          DstBufferSize;
  UINT32                          ScratchBufferSize;
  VOID                            *CompressionSource;
  UINT32                          CompressionSourceSize;
  UINT32                          UncompressedLength;
  UINT8                           CompressionType;

  if (CompressionSection->CommonHeader.Type != EFI_SECTION_COMPRESSION) {
    ASSERT (FALSE);
    return EFI_INVALID_PARAMETER;
  }

  if (IS_SECTION2 (CompressionSection)) {
    CompressionSource = (VOID *) ((UINT8 *) CompressionSection + sizeof (EFI_COMPRESSION_SECTION2));
    CompressionSourceSize = (UINT32) (SECTION2_SIZE (CompressionSection) - sizeof (EFI_COMPRESSION_SECTION2));
    UncompressedLength = ((EFI_COMPRESSION_SECTION2 *) CompressionSection)->UncompressedLength;
    CompressionType = ((EFI_COMPRESSION_SECTION2 *) CompressionSection)->CompressionType;
  } else {
    CompressionSource = (VOID *) ((UINT8 *) CompressionSection + sizeof (EFI_COMPRESSION_SECTION));
    CompressionSourceSize = (UINT32) (SECTION_SIZE (CompressionSection) - sizeof (EFI_COMPRESSION_SECTION));
    UncompressedLength = CompressionSection->UncompressedLength;
    CompressionType = CompressionSection->CompressionType;
  }

  //
  // This is a compression set, expand it
  //
  switch (CompressionType) {
  case EFI_STANDARD_COMPRESSION:
    if (FeaturePcdGet(PcdDxeIplSupportUefiDecompress)) {
      //
      // Load EFI standard compression.
      // For compressed data, decompress them to destination buffer.
      //
      Status = UefiDecompressGetInfo (
                 CompressionSource,
                 CompressionSourceSize,
                 &DstBufferSize,
                 &ScratchBufferSize
                 );
      if (EFI_ERROR (Status)) {
        //
        // GetInfo failed
        //
        DEBUG ((DEBUG_ERROR, "Decompress GetInfo Failed - %r\n", Status));
        return EFI_NOT_FOUND;
      }
      //
      // Allocate scratch buffer
      //
      ScratchBuffer = AllocatePages (EFI_SIZE_TO_PAGES (ScratchBufferSize));
      if (ScratchBuffer == NULL) {
        return EFI_OUT_OF_RESOURCES;
      }
      //
      // Allocate destination buffer, extra one page for adjustment
      //
      DstBuffer = AllocatePages (EFI_SIZE_TO_PAGES (DstBufferSize) + 1);
      if (DstBuffer == NULL) {
        return EFI_OUT_OF_RESOURCES;
      }
      //
      // DstBuffer still is one section. Adjust DstBuffer offset, skip EFI section header
      // to make section data at page alignment.
      //
      DstBuffer = DstBuffer + EFI_PAGE_SIZE - sizeof (EFI_COMMON_SECTION_HEADER);
      //
      // Call decompress function
      //
      Status = UefiDecompress (
                  CompressionSource,
                  DstBuffer,
                  ScratchBuffer
                  );
      if (EFI_ERROR (Status)) {
        //
        // Decompress failed
        //
        DEBUG ((DEBUG_ERROR, "Decompress Failed - %r\n", Status));
        return EFI_NOT_FOUND;
      }
      break;
    } else {
      //
      // PcdDxeIplSupportUefiDecompress is FALSE
      // Don't support UEFI decompression algorithm.
      //
      ASSERT (FALSE);
      return EFI_NOT_FOUND;
    }

  case EFI_NOT_COMPRESSED:
    //
    // Allocate destination buffer
    //
    DstBufferSize = UncompressedLength;
    DstBuffer     = AllocatePages (EFI_SIZE_TO_PAGES (DstBufferSize) + 1);
    if (DstBuffer == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
    //
    // Adjust DstBuffer offset, skip EFI section header
    // to make section data at page alignment.
    //
    DstBuffer = DstBuffer + EFI_PAGE_SIZE - sizeof (EFI_COMMON_SECTION_HEADER);
    //
    // stream is not actually compressed, just encapsulated.  So just copy it.
    //
    CopyMem (DstBuffer, CompressionSource, DstBufferSize);
    break;

  default:
    //
    // Don't support other unknown compression type.
    //
    ASSERT (FALSE);
    return EFI_NOT_FOUND;
  }

  *OutputSize = DstBufferSize;
  *OutputBuffer = DstBuffer;

  return EFI_SUCCESS;
}


/**
   Updates the Stack HOB passed to DXE phase.

   This function traverses the whole HOB list and update the stack HOB to
   reflect the real stack that is used by DXE core.

   @param BaseAddress           The lower address of stack used by DxeCore.
   @param Length                The length of stack used by DxeCore.

**/
VOID
UpdateStackHob (
  IN EFI_PHYSICAL_ADDRESS        BaseAddress,
  IN UINT64                      Length
  )
{
  EFI_PEI_HOB_POINTERS           Hob;

  Hob.Raw = GetHobList ();
  while ((Hob.Raw = GetNextHob (EFI_HOB_TYPE_MEMORY_ALLOCATION, Hob.Raw)) != NULL) {
    if (CompareGuid (&gEfiHobMemoryAllocStackGuid, &(Hob.MemoryAllocationStack->AllocDescriptor.Name))) {
      //
      // Build a new memory allocation HOB with old stack info with EfiBootServicesData type. Need to
      // avoid this region be reclaimed by DXE core as the IDT built in SEC might be on stack, and some
      // PEIMs may also keep key information on stack
      //
      BuildMemoryAllocationHob (
        Hob.MemoryAllocationStack->AllocDescriptor.MemoryBaseAddress,
        Hob.MemoryAllocationStack->AllocDescriptor.MemoryLength,
        EfiBootServicesData
        );
      //
      // Update the BSP Stack Hob to reflect the new stack info.
      //
      Hob.MemoryAllocationStack->AllocDescriptor.MemoryBaseAddress = BaseAddress;
      Hob.MemoryAllocationStack->AllocDescriptor.MemoryLength = Length;
      break;
    }
    Hob.Raw = GET_NEXT_HOB (Hob);
  }
}
