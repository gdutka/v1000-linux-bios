/** @file
  Secure Flash DXE driver.

;******************************************************************************
;* Copyright (c) 2023, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <Library/UefiLib.h>
#include <Library/DevicePathLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/HobLib.h>
#include <Library/DebugLib.h>
#include <Library/PrintLib.h>
#include <Library/VariableLib.h>
#include <Library/H2OCpLib.h>
#include <Library/LockBoxLib.h>
#include <Library/SeamlessRecoveryLib.h>
#include <Library/CapsuleUpdateCriteriaLib.h>
#include <Library/H2OIhisiLib.h>
#include <Protocol/LoadFile.h>
#include <Protocol/FirmwareVolume2.h>
#include <Protocol/ConsoleControl.h>
#include <Protocol/LoadedImage.h>
#include <Protocol/SimpleFileSystem.h>
#include <Protocol/DxeSmmReadyToLock.h>
#include <Guid/ImageAuthentication.h>
#include <Guid/HobList.h>
#include <Guid/UsbEnumeration.h>
#include <Guid/FirmwareFileSystem2.h>
#include <Guid/SystemNvDataGuid.h>
#include <Guid/FileInfo.h>
#include <Guid/H2OBdsCheckPoint.h>
#include <Guid/BootOrderHook.h>
#include <SecureFlash.h>
#include <KernelSetupConfig.h>
#include <H2OIhisi.h>
#include "TianoDecompress.h"

#define SECURE_FLASH_SIGNATURE        SIGNATURE_32 ('$', 'S', 'E', 'C')
#define SECURE_FLASH_LEGACY_SIGN      SIGNATURE_32 ('$', 'S', 'F', 'L')
#define MAX_STRING_LENGTH             128

EFI_GUID mSecureFlashErrorMsgGuid = { 0x2719F233, 0xDD1A, 0xABCD, { 0xA9, 0xDC, 0xAA, 0xE0, 0x7B, 0x67, 0x88, 0xF2 }};
EFI_GUID mSignatureOwnerGuid      = { 0x79736E49, 0x6564, 0xBBAA, { 0xCC, 0xDD, 0xEE, 0xFF, 0x01, 0x23, 0x45, 0x67 }};

typedef struct {
  EFI_SIGNATURE_LIST            SignatureListHeader;
  EFI_SIGNATURE_DATA            SignatureData;
} CERTIFICATE_DATA;

typedef struct {
  MEMMAP_DEVICE_PATH            MemDevicePath;
  EFI_DEVICE_PATH_PROTOCOL      EndDevicePath;
} IMAGE_DEVICE_PATH;

EFI_HANDLE                      mImageHandle;
EFI_HANDLE                      mMemMapLoadImageHandle;
EFI_STATUS                      mErrorStatus;
EFI_RESET_SYSTEM                mOriginalResetSystemPtr;


/**
  Verify if the image is signed(SecureFlashLib.c).

  @param[in] ImageBase       The address of the image.

  @retval EFI_SUCCESS        if the image is not signed
  @retval EFI_ACCESS_DENIED  if the image is signed
**/
EFI_STATUS
VerifySignedImage (
  IN UINT8                      *ImageBase
  )
{
  EFI_IMAGE_DOS_HEADER                *DosHeader;
  UINT32                              PeCoffHeaderOffset;
  UINT16                              Magic;
  EFI_IMAGE_DATA_DIRECTORY            *SectionDataDir;
  EFI_IMAGE_OPTIONAL_HEADER_PTR_UNION PeHeader;


  DosHeader = (EFI_IMAGE_DOS_HEADER *)(UINTN)(ImageBase);
  if (DosHeader->e_magic == EFI_IMAGE_DOS_SIGNATURE) {
    //
    // DOS image header is present,
    // so read the PE header after the DOS image header.
    //
    PeCoffHeaderOffset = DosHeader->e_lfanew;
  } else {
    PeCoffHeaderOffset = 0;
  }
  PeHeader.Pe32 = (EFI_IMAGE_NT_HEADERS32 *) (ImageBase + PeCoffHeaderOffset);
  if (PeHeader.Pe32->Signature != EFI_IMAGE_NT_SIGNATURE) {
    //
    // It is not a valid Pe/Coff file.
    //
    return EFI_ACCESS_DENIED;
  }

  Magic = PeHeader.Pe32->OptionalHeader.Magic;
  if (Magic == EFI_IMAGE_NT_OPTIONAL_HDR32_MAGIC) {
    //
    // Use PE32 offset.
    //
    SectionDataDir = (EFI_IMAGE_DATA_DIRECTORY *) &PeHeader.Pe32->OptionalHeader.DataDirectory[EFI_IMAGE_DIRECTORY_ENTRY_SECURITY];
  } else {
    //
    // Use PE32+ offset.
    //
    SectionDataDir = (EFI_IMAGE_DATA_DIRECTORY *) &PeHeader.Pe32Plus->OptionalHeader.DataDirectory[EFI_IMAGE_DIRECTORY_ENTRY_SECURITY];
  }

  if (SectionDataDir->Size == 0) {
    //
    // This image is not signed.
    //
    return EFI_ACCESS_DENIED;
  }

  return EFI_SUCCESS;
}

EFI_STATUS
GetImageFromEsp (
  OUT    IMAGE_INFO *FlashFie
  )
{
  EFI_STATUS             Status;
  UINTN                  NumberOfHandles;
  EFI_HANDLE             *HandleBuffer;
  UINTN                  Index;
  EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *SimpleFileSystem;
  EFI_FILE_HANDLE        SysDir;
  EFI_FILE_HANDLE        File;
  BOOLEAN                Found;
  CHAR16                 ImagePath[MAX_STRING_LENGTH];
  UINTN                  FlashImageSize;
  UINTN                  BufferSize;
  EFI_FILE_INFO          *FileInfo;
  UINT8                  *FlashImageBuffer;

  HandleBuffer = NULL;
  BufferSize = 0;
  FileInfo = NULL;
  SysDir = NULL;
  File = NULL;
  SimpleFileSystem = NULL;
  //
  // Search all simple file system
  //
  Status = gBS->LocateHandleBuffer(
                  ByProtocol,
                  &gEfiSimpleFileSystemProtocolGuid,
                  NULL,
                  &NumberOfHandles,
                  &HandleBuffer
                  );
  if (EFI_ERROR(Status)) {
    return Status;
  }
  //
  // Combine the image file path to "EFI\\Insyde\\isflash.bin"
  //
  UnicodeSPrint (ImagePath, MAX_STRING_LENGTH, L"%s\\isflash.bin", PcdGetPtr(PcdCapsuleImageFolder));

  Found = FALSE;
  for (Index = 0; Index < NumberOfHandles; Index++) {
    Status = gBS->HandleProtocol(
                    HandleBuffer[Index],
                    &gEfiSimpleFileSystemProtocolGuid,
                    (VOID **)&SimpleFileSystem
                    );
    if (EFI_ERROR(Status)) {
      continue;
    }

    //
    // Get the system dir
    //
    Status = SimpleFileSystem->OpenVolume(
                                 SimpleFileSystem,
                                 &SysDir
                                 );
    if (!EFI_ERROR(Status)) {
      //
      // check the flash image exist
      //
      Status = SysDir->Open (SysDir,
                             &File,
                             ImagePath, // "EFI\\Insyde\\isflash.bin"
                             EFI_FILE_MODE_READ,
                             0
                             );
      if (!EFI_ERROR(Status)) {
        Found = TRUE;
        break;
      }
    }
  }

  if (!Found) {
    goto CloseSysDirAndExit;
  }

  //
  // Get file info to check file attribute
  //
  Status = File->GetInfo (
                   File,
                   &gEfiFileInfoGuid,
                   &BufferSize,
                   FileInfo
                   );

  if (Status == EFI_BUFFER_TOO_SMALL) {
    FileInfo = AllocatePool(BufferSize);
    if (FileInfo == NULL) {
      goto CloseFileAndExit;
    }
    Status = File->GetInfo (
                     File,
                     &gEfiFileInfoGuid,
                     &BufferSize,
                     FileInfo
                     );
  }

  if (EFI_ERROR (Status) || FileInfo == NULL) {
    goto CloseFileAndExit;
  }

  //
  // If the attribute of the file is read only, remove the attribute.
  // Because the file will be remove.
  // The file attributes will be valid the next time the file is opened with Open().
  //
  if ((FileInfo->Attribute & EFI_FILE_READ_ONLY) == EFI_FILE_READ_ONLY) {
    FileInfo->Attribute &= ~EFI_FILE_READ_ONLY;
    Status = File->SetInfo (
                     File,
                     &gEfiFileInfoGuid,
                     BufferSize,
                     FileInfo
                     );
    if (EFI_ERROR(Status)) {
      goto CloseFileAndExit;
    }
  }
  FlashFie->ImageSize = (UINT32)FileInfo->FileSize;

  //
  // Close the file with READ mode
  // Re-Open file with R/W mode.
  //
  Status = File->Close(File);
  Status = SysDir->Open (SysDir,
                         &File,
                         ImagePath, // "EFI\\Insyde\\isflash.bin"
                         EFI_FILE_MODE_READ|EFI_FILE_MODE_WRITE,
                         0
                         );
  if (EFI_ERROR(Status)) {
    goto CloseSysDirAndExit;
  }

  FlashImageBuffer = AllocateZeroPool (FlashFie->ImageSize);
  if (FlashImageBuffer == NULL) {
    goto DeletFileAndExit;
  }
  FlashFie->ImageAddress = (UINTN)FlashImageBuffer;
  FlashImageSize = (UINTN)(FlashFie->ImageSize);
  Status = File->Read (File, &FlashImageSize, FlashImageBuffer);
  if (!EFI_ERROR (Status)) {
    Status = VerifySignedImage ((UINT8*)(UINTN)FlashFie->ImageAddress);
  }

DeletFileAndExit:
  File->Delete(File);
  goto CloseSysDirAndExit;

CloseFileAndExit:
  File->Close(File);

CloseSysDirAndExit:
  if (SysDir != NULL){
    SysDir->Close(SysDir);
  }

  if (FileInfo != NULL) {
    FreePool (FileInfo);
  }

  if (HandleBuffer != NULL) {
    FreePool (HandleBuffer);
  }
  return Status;
}

VOID
EFIAPI
SecureFlashSmmReadyToLockEvent (
  IN EFI_EVENT                  Event,
  IN VOID                       *Context
  )
{
  EFI_STATUS    Status;
  VOID          *Interface;
  //
  // Double check the interface installation
  //
  Status = gBS->LocateProtocol (
                  &gEfiDxeSmmReadyToLockProtocolGuid,
                  NULL,
                  (VOID**)&Interface
                  );
  if (EFI_ERROR (Status)) {
    return;
  }

  H2OIhisiAuthLock ();
  H2OIhisiFbtsOemCapsuleSecureFlashResetPostFlag ();
  H2OIhisiAuthUnlock ();
  gBS->CloseEvent (Event);
}

/**
  Use Console Control to turn on GOP/UGA based Simple Text Out consoles. The GOP/UGA
  Simple Text Out screens will now be synced up with all non GOP/UGA output devices

  @param None

  @retval EFI_SUCCESS            GOP/UGA devices are back in text mode and synced up.
  @retval EFI_UNSUPPORTED        Logo not found

**/
EFI_STATUS
DisableQuietBoot (
  VOID
  )
{
  EFI_STATUS                    Status;
  EFI_CONSOLE_CONTROL_PROTOCOL  *ConsoleControl;

  Status = gBS->LocateProtocol (&gEfiConsoleControlProtocolGuid, NULL, (VOID**)&ConsoleControl);
  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }

  return ConsoleControl->SetMode (ConsoleControl, EfiConsoleControlScreenText);
}

/**
 Find the next Capsule volume HOB

 @param [in, out] HobStart      start of HOBs
 @param [out]  BaseAddress      returned base address of capsule volume
 @param [out]  Length           length of capsule volume pointed to by BaseAddress

 @retval EFI_SUCCESS            one found
 @retval EFI_NOT_FOUND          did not find one

**/
EFI_STATUS
GetNextCapsuleVolumeHob (
  IN OUT VOID                   **HobStart,
  OUT EFI_PHYSICAL_ADDRESS      *BaseAddress,
  OUT UINT64                    *Length
  )
{
  EFI_PEI_HOB_POINTERS          Hob;

  Hob.Raw = *HobStart;
  if (END_OF_HOB_LIST (Hob)) {
    return EFI_NOT_FOUND;
  }

  Hob.Raw = GetNextHob (EFI_HOB_TYPE_UEFI_CAPSULE, *HobStart);
  if (Hob.Header->HobType != EFI_HOB_TYPE_UEFI_CAPSULE) {
    return EFI_NOT_FOUND;
  }

  *BaseAddress = Hob.Capsule->BaseAddress;
  *Length      = Hob.Capsule->Length;
  *HobStart    = GET_NEXT_HOB (Hob);

  return EFI_SUCCESS;
}

/**
 Get the certificate from FV

 @param [in]   NameGuid         The file guid of the certificate
 @param [in, out] Buffer        returned the address of the certificate
 @param [in, out] Size          the size of the certificate

 @retval EFI_SUCCESS            found a certificate
 @retval EFI_NOT_FOUND          did not find one
 @retval EFI_LOAD_ERROR         there is no FV protocol

**/
EFI_STATUS
GetCertificateData (
  IN EFI_GUID                   *NameGuid,
  IN OUT VOID                   **Buffer,
  IN OUT UINTN                  *Size
  )
{
  EFI_STATUS                    Status;
  EFI_HANDLE                    *HandleBuffer;
  UINTN                         HandleCount;
  UINTN                         Index;
  EFI_FIRMWARE_VOLUME2_PROTOCOL *Fv;
  UINT32                        AuthenticationStatus;

  Fv = NULL;
  AuthenticationStatus = 0;
  HandleBuffer = NULL;

  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiFirmwareVolume2ProtocolGuid,
                  NULL,
                  &HandleCount,
                  &HandleBuffer
                  );
  if (EFI_ERROR (Status) || (HandleCount == 0)) {
    return EFI_NOT_FOUND;
  }

  //
  // Find desired image in all Fvs
  //
  for (Index = 0; Index < HandleCount; Index++) {
    Status = gBS->HandleProtocol (
                    HandleBuffer[Index],
                    &gEfiFirmwareVolume2ProtocolGuid,
                    (VOID **)&Fv
                    );

    if (EFI_ERROR (Status)) {
      return EFI_LOAD_ERROR;
    }

    *Buffer = NULL;
    *Size = 0;
    Status = Fv->ReadSection (
                   Fv,
                   NameGuid,
                   EFI_SECTION_RAW,
                   0,
                   Buffer,
                   Size,
                   &AuthenticationStatus
                   );

    if (!EFI_ERROR (Status)) {
      break;
    }
  }

  if (HandleBuffer != NULL){
    FreePool (HandleBuffer);
  }

  if (Index >= HandleCount) {
    return EFI_NOT_FOUND;
  }

  return EFI_SUCCESS;
}

EFI_STATUS
LoadCertToVariable (
  VOID
  )
{
  EFI_STATUS                  Status;
  UINT8                       *FileBuffer;
  UINTN                       FileSize;
  CERTIFICATE_DATA            *CertData;
  UINTN                       CertSize;

  //
  // Make sure there is no SecureFlashCertData variable.
  // if the variable SecureFlashCertData exists, delete it before setting.
  //
  gRT->SetVariable (
         L"SecureFlashCertData",
         &gSecureFlashInfoGuid,
         0,
         0,
         NULL
         );
  if (PcdGetBool (PcdH2OCapsuleSignatureDatabaseSupported)) {
    CertData = NULL;
    Status = GetCertificateData (PcdGetPtr (PcdSecureFlashCertificateFile), (VOID **)&CertData, &CertSize);
    if (EFI_ERROR (Status)) {
      return EFI_NOT_FOUND;
    }
  } else {
    FileBuffer = NULL;
    Status = GetCertificateData (PcdGetPtr (PcdSecureFlashCertificateFile), (VOID **)&FileBuffer, &FileSize);
    if (EFI_ERROR (Status)) {
      return EFI_NOT_FOUND;
    }
    CertSize = sizeof (CERTIFICATE_DATA) + FileSize;
    CertData = AllocatePool (CertSize);
    ASSERT (CertData != NULL);
    if (CertData == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
    CertData->SignatureListHeader.SignatureType = gEfiCertX509Guid;
    CertData->SignatureListHeader.SignatureSize = (UINT32)FileSize + sizeof (EFI_GUID);
    CertData->SignatureListHeader.SignatureListSize = CertData->SignatureListHeader.SignatureSize +
                                                      sizeof (EFI_SIGNATURE_LIST);
    CopyGuid (&CertData->SignatureData.SignatureOwner, &mSignatureOwnerGuid);
    CopyMem (CertData->SignatureData.SignatureData, FileBuffer, FileSize);
    FreePool (FileBuffer);
  }

  Status = gRT->SetVariable (
                  L"SecureFlashCertData",
                  &gSecureFlashInfoGuid,
                  EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                  CertSize,
                  CertData
                  );
  FreePool (CertData);
  return Status;
}

EFI_STATUS
EFIAPI
MemMapLoadFileFunction (
  IN EFI_LOAD_FILE_PROTOCOL     *This,
  IN EFI_DEVICE_PATH_PROTOCOL   *FilePath,
  IN BOOLEAN                    BootPolicy,
  IN OUT UINTN                  *BufferSize,
  IN VOID                       *Buffer OPTIONAL
  )
{
  MEMMAP_DEVICE_PATH            *MemMapFilePath;

  //
  // The FilePath is pointed to EndDevicePath, modify it to the right address(MEMMAP_DEVICE_PATH).
  //
  MemMapFilePath = (MEMMAP_DEVICE_PATH *)((UINTN)FilePath - sizeof (MEMMAP_DEVICE_PATH));

  if ((MemMapFilePath->Header.Type == HARDWARE_DEVICE_PATH) && (MemMapFilePath->Header.SubType == HW_MEMMAP_DP)) {
    if (*BufferSize < (MemMapFilePath->EndingAddress - MemMapFilePath->StartingAddress + 1)) {
      *BufferSize = (UINTN)(MemMapFilePath->EndingAddress - MemMapFilePath->StartingAddress + 1);
      return EFI_BUFFER_TOO_SMALL;
    }

    *BufferSize = (UINTN)(MemMapFilePath->EndingAddress - MemMapFilePath->StartingAddress + 1);
    CopyMem (Buffer, (VOID *)(UINTN)MemMapFilePath->StartingAddress, *BufferSize);
  }

  return EFI_SUCCESS;
}

VOID
EFIAPI
SecureFlashResetSystem (
  IN EFI_RESET_TYPE             ResetType,
  IN EFI_STATUS                 ResetStatus,
  IN UINTN                      DataSize,
  IN VOID                       *ResetData OPTIONAL
  )
{
  return;
}

VOID
SecureFlashHookResetSystem (
  IN BOOLEAN                    ToHook
  )
{

  if (ToHook) {
    mOriginalResetSystemPtr = gRT->ResetSystem;
    gRT->ResetSystem = SecureFlashResetSystem;
  } else {
    gRT->ResetSystem = mOriginalResetSystemPtr;
  }
}

VOID
SecureFlashEndingProcess (
  VOID
  )
{
  EFI_STATUS                    Status;
  EFI_HANDLE                    Handle;
  UINT8                         *Instance;

  //
  // Install USB enumeration protocol to make sure USB keyboard is initialized.
  //
  Status = gBS->LocateProtocol (&gEfiUsbEnumerationGuid, NULL, (VOID **) &Instance);
  if (EFI_ERROR (Status)) {
    Handle = 0;
    gBS->InstallProtocolInterface (
           &Handle,
           &gEfiUsbEnumerationGuid,
           EFI_NATIVE_INTERFACE,
           NULL
           );
  }
  gRT->SetVariable (
         SECURE_FLASH_SETUP_MODE_NAME,
         &gSecureFlashInfoGuid,
         EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
         0,
         NULL
         );
  SetVariableToSensitiveVariable (
         SECURE_FLASH_INFORMATION_NAME,
         &gSecureFlashInfoGuid,
         EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
         0,
         NULL
         );

  //
  // To trigger error message driver if it is error-loaded or it updates failed.
  //
  gBS->InstallProtocolInterface (&mImageHandle, &mSecureFlashErrorMsgGuid, EFI_NATIVE_INTERFACE, &mErrorStatus);

}

STATIC
EFI_STATUS
UninstallSpecificProtocols (
  EFI_GUID         *Protocol
  )
{
  EFI_STATUS       Status;
  UINTN            NoHandles;
  EFI_HANDLE       *Buffer;
  UINTN            Index;

  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  Protocol,
                  NULL,
                  &NoHandles,
                  &Buffer
                  );
  if (!EFI_ERROR (Status)) {
    for (Index = 0; Index < NoHandles; Index++) {
      Status = gBS->UninstallProtocolInterface (
                      Buffer[Index],
                      Protocol,
                      NULL
                      );
      if (EFI_ERROR (Status)) {
        gBS->FreePool (Buffer);
        return Status;
      }
    }
    gBS->FreePool (Buffer);
  }

  return EFI_SUCCESS;

}

/**
  Write flash image to file.

  @param[in] ImageInfo    Pointer to flash image information.

  @return  which file system to write or NULL if cannot write to any file system

**/
EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *
PersistFlashImage (
  IN IMAGE_INFO       *ImageInfo
  )
{
  EFI_SIMPLE_FILE_SYSTEM_PROTOCOL      *SimpleFileSystem;
  EFI_STATUS                           Status;
  UINTN                                NumberOfHandles;
  EFI_HANDLE                          *HandleBuffer;
  CHAR16                              ImagePath[MAX_STRING_LENGTH];
  UINTN                               Index;
  EFI_FILE_HANDLE                     SysRootHandle;
  EFI_FILE_HANDLE                     FlashImageHandle;
  UINTN                               Size;

  Status = gBS->LocateHandleBuffer(
                  ByProtocol,
                  &gEfiSimpleFileSystemProtocolGuid,
                  NULL,
                  &NumberOfHandles,
                  &HandleBuffer
                  );
  if (EFI_ERROR(Status)) {
    return NULL;
  }
  UnicodeSPrint (ImagePath, MAX_STRING_LENGTH, L"%s\\%s1000.bin", EFI_CAPSULE_FILE_PATH, EFI_CAPSULE_FILE_NAME);

  for (Index = 0; Index < NumberOfHandles; Index++) {
    Status = gBS->HandleProtocol(
                    HandleBuffer[Index],
                    &gEfiSimpleFileSystemProtocolGuid,
                    (VOID **) &SimpleFileSystem
                    );
    if (EFI_ERROR (Status)) {
      continue;
    }
    Status = SimpleFileSystem->OpenVolume (SimpleFileSystem, &SysRootHandle);
    if (EFI_ERROR (Status)) {
      continue;
    }
    //
    // Make sure the flash image folder exist.
    //
    Status = SysRootHandle->Open (
                              SysRootHandle,
                              &FlashImageHandle,
                              EFI_CAPSULE_FILE_PATH,
                              EFI_FILE_MODE_CREATE | EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE,
                              EFI_FILE_ARCHIVE | EFI_FILE_DIRECTORY
                              );
    if (EFI_ERROR (Status)) {
      SysRootHandle->Close (SysRootHandle);
      continue;
    }
    FlashImageHandle->Close(FlashImageHandle);

    //
    // Delete the backup file if it already exists.
    //
    Status = SysRootHandle->Open (
                              SysRootHandle,
                              &FlashImageHandle,
                              ImagePath,
                              EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE,
                              0
                              );
    if (Status == EFI_SUCCESS) {
      Status = FlashImageHandle->Delete (FlashImageHandle);
      ASSERT_EFI_ERROR(Status);
    }
    //
    //  Write flash image to backup file.
    //
    Status = SysRootHandle->Open (
                              SysRootHandle,
                              &FlashImageHandle,
                              ImagePath,
                              EFI_FILE_MODE_CREATE | EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE,
                              EFI_FILE_ARCHIVE
                              );
    if (EFI_ERROR (Status)) {
      SysRootHandle->Close (SysRootHandle);
      continue;
    }
    Size = ImageInfo->ImageSize;
    Status = FlashImageHandle->Write (
                                 FlashImageHandle,
                                 &Size,
                                 (VOID *) (UINTN) ImageInfo->ImageAddress
                                 );

    SysRootHandle->Close (SysRootHandle);
    FlashImageHandle->Close (FlashImageHandle);
    if (!EFI_ERROR (Status)) {
      FreePool (HandleBuffer);
      return SimpleFileSystem;
    }
  }
  FreePool (HandleBuffer);
  return NULL;
}

/**

  Delete backup flash image file

  @param[in] SimpleFileSystem    Pointer to system file system protocol instance.

**/
VOID
DeleteFlashImage (
  IN  EFI_SIMPLE_FILE_SYSTEM_PROTOCOL      *SimpleFileSystem
  )
{
  EFI_STATUS         Status;
  EFI_FILE_HANDLE    SysRootHandle;
  EFI_FILE_HANDLE    FlashImageHandle;
  CHAR16             ImagePath[MAX_STRING_LENGTH];

  if (SimpleFileSystem == NULL) {
    return;
  }

  UnicodeSPrint (ImagePath, MAX_STRING_LENGTH, L"%s\\%s1000.bin", EFI_CAPSULE_FILE_PATH, EFI_CAPSULE_FILE_NAME);
  Status = SimpleFileSystem->OpenVolume (SimpleFileSystem, &SysRootHandle);
  if (EFI_ERROR (Status)) {
    return;
  }
  Status = SysRootHandle->Open (
                            SysRootHandle,
                            &FlashImageHandle,
                            ImagePath,
                            EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE,
                            0
                            );
  if (Status == EFI_SUCCESS) {
    Status = FlashImageHandle->Delete (FlashImageHandle);
  }
  SysRootHandle->Close (SysRootHandle);
}

VOID
EFIAPI
LoadIsFlashImageCallback (
  IN EFI_EVENT                              Event,
  IN H2O_CP_HANDLE                          CpHandle
  )
{
  EFI_STATUS                             Status;
  EFI_STATUS                             GetEspStatus;
  UINTN                                  Size;
  EFI_HANDLE                             IsFlashImageHandle;
  IMAGE_DEVICE_PATH                      ImageDP;
  EFI_LOAD_FILE_PROTOCOL                 MemMapLoadFile;
  IMAGE_INFO                             ImageInfo;
  UINT8                                  SetupMode;
  COMPRESS_DATA_HEADER                   *CompressDataHeader;
  COMPRESS_TABLE_HEADER                  *CompressTableHeader;
  UINT8                                  *ImagegBuffer;
  UINT8                                  *CompressDataPtr;
  UINTN                                  Index;
  SCRATCH_DATA                           *Scratch;
  VOID                                   *Interface;
  EFI_HANDLE                             Handle;
  H2O_BDS_CP_NO_BOOT_DEVICE_DATA         *BdsNoBootDeviceData;
  EFI_GUID                               BdsNoBootDeviceGuid;
  H2O_BDS_CP_READY_TO_BOOT_BEFORE_DATA   *BdsReadyToBootBeforeData;
  EFI_GUID                               BdsReadyToBootBeforeGuid;
  BOOLEAN                                SecureFlashFlag;
  UINT8                                  RequestAction;
  EFI_SIMPLE_FILE_SYSTEM_PROTOCOL        *SimpleFileSystem;
  UINTN                                  Offset;

  Status = H2OCpLookup (CpHandle, (VOID **) &BdsNoBootDeviceData, &BdsNoBootDeviceGuid);
  if (EFI_ERROR (Status)) {
    Status = H2OCpLookup (CpHandle, (VOID **) &BdsReadyToBootBeforeData, &BdsReadyToBootBeforeGuid);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "Checkpoint Data Not Found: %x (%r)\n", CpHandle, Status));
      DEBUG ((DEBUG_ERROR, "   %a\n", __FUNCTION__));
      return;
    }
  }
  H2OCpUnregisterHandler (CpHandle);

  ImagegBuffer = NULL;
  Scratch = NULL;
  mErrorStatus = EFI_SUCCESS;
  GetEspStatus = EFI_NOT_FOUND;
  if (FeaturePcdGet (PcdAutoCreateDummyBootOption)) {
    Status = UninstallSpecificProtocols (&gBootOrderHookEnableGuid);
    if (EFI_ERROR (Status)) {
      return;
    }

    Interface = NULL;
    Status = gBS->LocateProtocol (
                    &gBootOrderHookDisableGuid,
                    NULL,
                    (VOID **)&Interface
                    );
    if (EFI_ERROR (Status)) {
      Handle = NULL;
      Status = gBS->InstallProtocolInterface (
                      &Handle,
                      &gBootOrderHookDisableGuid,
                      EFI_NATIVE_INTERFACE,
                      NULL
                      );
    }
  }

  Size = sizeof (IMAGE_INFO);
  Status = gRT->GetVariable (
                  SECURE_FLASH_INFORMATION_NAME,
                  &gSecureFlashInfoGuid,
                  NULL,
                  &Size,
                  &ImageInfo
                  );
  //
  // Clear this variable so it won't hang if it fails or user doesn't flash the NV region.
  //
  SetVariableToSensitiveVariable (
         SECURE_FLASH_INFORMATION_NAME,
         &gSecureFlashInfoGuid,
         EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
         0,
         NULL
         );
  if ((Status == EFI_SUCCESS) && (ImageInfo.FlashMode)) {
    //
    // Retrieve secure flash image from possible resource.
    //
    SecureFlashFlag = TRUE;
    Status = SaveLockBox (&gSecureFlashInfoGuid, &SecureFlashFlag, sizeof (SecureFlashFlag));
    GetEspStatus = GetImageFromEsp(&ImageInfo);
    if (!EFI_ERROR (GetEspStatus)) {
      goto DoSecureFlash;
    }

    if (ImageInfo.Compressed) {
      Size = sizeof (SCRATCH_DATA);
      Scratch = AllocatePool (Size);
      if (Scratch == NULL) {
        mErrorStatus = EFI_BUFFER_TOO_SMALL;
        goto Done;
      }
      //
      // Decompress the image.
      //
      CompressTableHeader = (COMPRESS_TABLE_HEADER *)(UINTN)ImageInfo.ImageAddress;
      ImageInfo.ImageSize = CompressTableHeader->TotalImageSize;
      ImagegBuffer = AllocatePool (ImageInfo.ImageSize);
      if (ImagegBuffer == NULL) {
        mErrorStatus = EFI_BUFFER_TOO_SMALL;
        goto Done;
      }
      CompressDataPtr = (UINT8 *)(UINTN)(ImageInfo.ImageAddress + sizeof (COMPRESS_TABLE_HEADER));
      ImageInfo.ImageAddress = (UINTN)ImagegBuffer;

      for (Index = 0, Offset = 0; Index < CompressTableHeader->NumOfBlock; Index++) {
        CompressDataHeader = (COMPRESS_DATA_HEADER *)(UINTN)CompressDataPtr;
        CompressDataPtr += sizeof (COMPRESS_DATA_HEADER);
        Status = Decompress (CompressDataPtr, ImagegBuffer + Offset, Scratch);
        if (EFI_ERROR (Status)) {
          mErrorStatus = EFI_INVALID_PARAMETER;
          goto Done;
        }
        //
        // Go to next compressed data
        //
        CompressDataPtr += CompressDataHeader->CompressedSize;
        Offset += Scratch->mOutBuf;
      }
    }

DoSecureFlash:
    //
    // Set the information needed so it will verify the image.
    //
    ImageDP.MemDevicePath.Header.Type = HARDWARE_DEVICE_PATH;
    ImageDP.MemDevicePath.Header.SubType = HW_MEMMAP_DP;
    ImageDP.MemDevicePath.Header.Length[0] = (UINT8)(sizeof (MEMMAP_DEVICE_PATH));
    ImageDP.MemDevicePath.Header.Length[1] = (UINT8)(sizeof (MEMMAP_DEVICE_PATH) >> 8);
    ImageDP.MemDevicePath.MemoryType = EfiReservedMemoryType;
    ImageDP.MemDevicePath.StartingAddress = ImageInfo.ImageAddress;
    ImageDP.MemDevicePath.EndingAddress = ImageInfo.ImageAddress + ImageInfo.ImageSize - 1;

    ImageDP.EndDevicePath.Type = END_DEVICE_PATH_TYPE;
    ImageDP.EndDevicePath.SubType = END_ENTIRE_DEVICE_PATH_SUBTYPE;
    ImageDP.EndDevicePath.Length[0] = END_DEVICE_PATH_LENGTH;
    ImageDP.EndDevicePath.Length[1] = 0;

    MemMapLoadFile.LoadFile = MemMapLoadFileFunction;
    Status = gBS->InstallMultipleProtocolInterfaces (
                    &mMemMapLoadImageHandle,
                    &gEfiDevicePathProtocolGuid,
                    &ImageDP.MemDevicePath,
                    &gEfiLoadFileProtocolGuid,
                    &MemMapLoadFile,
                    NULL
                    );


    //
    // Set this variable to trigger the verification process.
    //
    SetupMode = USER_MODE;
    Status = gRT->SetVariable (
                    SECURE_FLASH_SETUP_MODE_NAME,
                    &gSecureFlashInfoGuid,
                    EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                    sizeof (SetupMode),
                    &SetupMode
                    );

    DisableQuietBoot ();
    LoadCertToVariable ();

    Status = gBS->LoadImage (
                    TRUE,
                    mImageHandle,
                    &ImageDP.MemDevicePath.Header,
                    NULL,
                    ImageInfo.ImageSize,
                    &IsFlashImageHandle
                    );
    if (EFI_ERROR (Status)) {
      mErrorStatus = EFI_ACCESS_DENIED;
    } else {
      //
      // Hook resetsystem function so it won't reboot when user press CTRL+ALT+DEL.
      //
      SecureFlashHookResetSystem (TRUE);
      SetFirmwareUpdatingFlag (TRUE);
      SimpleFileSystem = PersistFlashImage (&ImageInfo);
      Status = gBS->StartImage (IsFlashImageHandle, &Size, NULL);
      SetFirmwareUpdatingFlag (FALSE);
      DeleteFlashImage (SimpleFileSystem);
      SecureFlashHookResetSystem (FALSE);
      if (!EFI_ERROR (Status)) {
        RequestAction = 0;
        Size          = sizeof (RequestAction);
        Status        = RestoreLockBox (&gSecureFlashInfoGuid, &RequestAction, &Size);
        if (RequestAction == FlashCompleteReboot) {
          gRT->ResetSystem (EfiResetCold, EFI_SUCCESS, 0, NULL);
        } else if (RequestAction == FlashCompleteShutdown) {
          gRT->ResetSystem (EfiResetShutdown, EFI_SUCCESS, 0, NULL);
        }
      } else {
        mErrorStatus = EFI_ABORTED;
      }
      //
      // Clear this variable to disable the verification process.
      //
      gRT->SetVariable (
             SECURE_FLASH_SETUP_MODE_NAME,
             &gSecureFlashInfoGuid,
             EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
             0,
             NULL
             );
    }
  }


Done:
  if (Scratch != NULL){
    Status = gBS->FreePool (Scratch);
  }
  if (ImagegBuffer != NULL){
    Status = gBS->FreePool (ImagegBuffer);
  }
  SecureFlashFlag = FALSE;
  SaveLockBox (&gSecureFlashInfoGuid, &SecureFlashFlag, sizeof (SecureFlashFlag));
  SecureFlashEndingProcess ();

}

VOID
EFIAPI
SecureFlashCheckBootModeEvent (
  IN EFI_EVENT                       Event,
  IN H2O_CP_HANDLE                   CpHandle
  )
{
  EFI_STATUS                         Status;
  H2O_BDS_CP_NO_BOOT_DEVICE_DATA     *BdsLegacyBootAfterData;
  EFI_GUID                           BdsLegacyBootAfterGuid;

  Status = H2OCpLookup (CpHandle, (VOID **) &BdsLegacyBootAfterData, &BdsLegacyBootAfterGuid);
  if (EFI_ERROR (Status)) {
    return;
  }
  H2OCpUnregisterHandler (CpHandle);

  H2OIhisiAuthLock ();
  H2OIhisiFbtsOemCapsuleSecureFlashDisableEsp ();
  H2OIhisiAuthUnlock ();
}

/**
 This driver receives reserved memory information form Hob that is created by
 PEIM SecureFlashPei and set to the variable "SecureFlashInfo".
 The driver will install a ReadyToBoot event depending on whether system
 will update firmware this time or not.

 @param [in]   ImageHandle      The image handle.
 @param [in]   SystemTable      The system table.

 @retval EFI_SUCCESS            The driver is loaded.

**/
EFI_STATUS
EFIAPI
SecureFlashDxeEntry (
  IN EFI_HANDLE                 ImageHandle,
  IN EFI_SYSTEM_TABLE           *SystemTable
  )
{
  EFI_STATUS                    Status;
  VOID                          *Registration;
  EFI_HOB_HANDOFF_INFO_TABLE    *HobList;
  EFI_PHYSICAL_ADDRESS          BaseAddress;
  UINT64                        BaseSize;
  IMAGE_INFO                    ImageInfo;
  UINTN                         Size;
  EFI_PHYSICAL_ADDRESS          BaseAddressCopy;
  H2O_CP_HANDLE                 CpHandle;

  BaseAddressCopy = 0;
  mImageHandle = ImageHandle;

  Size = sizeof (IMAGE_INFO);
  Status = gRT->GetVariable (
                  SECURE_FLASH_INFORMATION_NAME,
                  &gSecureFlashInfoGuid,
                  NULL,
                  &Size,
                  &ImageInfo
                  );

  if ((Status == EFI_SUCCESS) && (ImageInfo.FlashMode)) {
    if (FeaturePcdGet (PcdH2OBdsCpReadyToBootBeforeSupported)) {
      //
      // Setup the flash update procedure right before ready to boot event
      // Use of TPL_CALLBACK-1 to make the USB hot-plug can work properly as it
      // is using TPL_CALLBACK level
      //
      H2OCpRegisterHandler (
        &gH2OBdsCpReadyToBootBeforeGuid,
        LoadIsFlashImageCallback,
        H2O_CP_MEDIUM_LOW,
        &CpHandle
        );
    }
    if (FeaturePcdGet (PcdH2OBdsCpNoBootDeviceSupported)) {
      //
      // Register NoBootDevice event to process valid of secure flash image event
      // no bootable device connect on platform.
      //
      Status = H2OCpRegisterHandler (
                 &gH2OBdsCpNoBootDeviceGuid,
                 LoadIsFlashImageCallback,
                 H2O_CP_MEDIUM,
                 &CpHandle
                 );
      if (EFI_ERROR (Status)) {
        DEBUG ((DEBUG_ERROR, "Checkpoint Register Fail: %g (%r)\n", &gH2OBdsCpNoBootDeviceGuid, Status));
        return Status;
      }
      DEBUG ((DEBUG_INFO, "Checkpoint Registered: %g (%r)\n", &gH2OBdsCpNoBootDeviceGuid, Status));
    }
  } else {
    ImageInfo.FlashMode = FALSE;
    //
    // Clear this variable to disable the verification process.
    //
    Status = gRT->SetVariable (
                    SECURE_FLASH_SETUP_MODE_NAME,
                    &gSecureFlashInfoGuid,
                    EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                    0,
                    NULL
                    );
    //
    // This is used for modifying the module variable mInPOST in SMM.
    //
    EfiCreateProtocolNotifyEvent (
      &gEfiDxeSmmReadyToLockProtocolGuid,
      TPL_CALLBACK,
      SecureFlashSmmReadyToLockEvent,
      NULL,
      &Registration
      );

    if (FeaturePcdGet (PcdH2OBdsCpLegacyBootAfterSupported)) {
      //
      // Register a legacy boot event to make sure legacy boot device boot.
      //
      H2OCpRegisterHandler (
        &gH2OBdsCpLegacyBootAfterGuid,
        SecureFlashCheckBootModeEvent,
        H2O_CP_MEDIUM_LOW,
        &CpHandle
        );
    }
  }

  Status = EfiGetSystemConfigurationTable (&gEfiHobListGuid, (VOID **)&HobList);
  Status = GetNextCapsuleVolumeHob ((VOID **)&HobList, &BaseAddress, &BaseSize);
  if (!EFI_ERROR (Status)) {

    //
    // Due to PEI core not allowed to allocate EfiReservedMemoryType memory, we allocate
    // EfiACPIMemoryNVS under PEI and then change the memory type to EfiReservedMemoryType
    // in order to preventing legacy E820 table from produce ASSERT error
    //
    if (ImageInfo.FlashMode) {
      //
      // Allocate intermediate memory to backup flash image in case the content of
      // preserved memory destroyed by gBS->FreePages when DEBUG_PROPERTY_CLEAR_MEMORY_ENABLED set
      //
      Status = gBS->AllocatePages (
                      AllocateAnyPages,
                      EfiBootServicesData,
                      (UINTN)((BaseSize + EFI_PAGE_SIZE - 1) / EFI_PAGE_SIZE),
                      &BaseAddressCopy
                      );
      if (!EFI_ERROR (Status)) {
        CopyMem ((VOID*)(UINTN)BaseAddressCopy, (VOID*)(UINTN)BaseAddress, (UINTN)BaseSize);
      }
    }

    Size = PcdGet32 (PcdReservedMemorySizeForSecureFlash);
    Status = gBS->FreePages (
                    BaseAddress,
                    (UINTN)(Size / EFI_PAGE_SIZE)
                    );
    if (!EFI_ERROR (Status)) {
      Status = gBS->AllocatePages (
                      AllocateAddress,
                      EfiReservedMemoryType,
                      (UINTN)(Size / EFI_PAGE_SIZE),
                      &BaseAddress
                      );
    }

    if (ImageInfo.FlashMode && BaseAddressCopy) {
      CopyMem ((VOID*)(UINTN)BaseAddress, (VOID*)(UINTN)BaseAddressCopy, (UINTN)BaseSize);
      gBS->FreePages (
             BaseAddressCopy,
             (UINTN)((BaseSize + EFI_PAGE_SIZE - 1) / EFI_PAGE_SIZE)
             );
    }
    ImageInfo.ImageAddress = BaseAddress;
    ImageInfo.ImageSize = (UINT32)BaseSize;
  }

  Status = SetVariableToSensitiveVariable (
                  SECURE_FLASH_INFORMATION_NAME,
                  &gSecureFlashInfoGuid,
                  EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                  sizeof (IMAGE_INFO),
                  &ImageInfo
                  );

  return EFI_SUCCESS;
}

