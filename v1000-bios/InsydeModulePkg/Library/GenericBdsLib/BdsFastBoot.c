/** @file
  BDS fast boot related functions

;******************************************************************************
;* Copyright (c) 2017 - 2022, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "InternalBdsLib.h"

/**
 Get specific device path node from input device path.

 @param[in] DevicePath                The pointer of device path
 @param[in] TargetDevicePathType      Type of target device path node
 @param[in] TargetDevicePathSubType   Subtype of target device path node

 @return The pointer of target device path node or NULL if not found
**/
STATIC
EFI_DEVICE_PATH_PROTOCOL *
GetDevicePathNode (
  IN EFI_DEVICE_PATH_PROTOCOL   *DevicePath,
  IN UINT8                      TargetDevicePathType,
  IN UINT8                      TargetDevicePathSubType
  )
{
  EFI_DEVICE_PATH_PROTOCOL      *DevicePathNode;

  if (DevicePath == NULL) {
    return NULL;
  }

  DevicePathNode = DevicePath;
  while (!IsDevicePathEnd (DevicePathNode)) {
    if ((DevicePathType (DevicePathNode) == TargetDevicePathType) &&
        (DevicePathSubType (DevicePathNode) == TargetDevicePathSubType)) {
      return DevicePathNode;
    }

    DevicePathNode = NextDevicePathNode (DevicePathNode);
  }

  return NULL;
}

/**
 Get version type resource directory

 @param[in]  ResourceData       The pointer of resource data
 @param[out] VersionDir         The double pointer of version type resource directory
 @param[out] VersionDirEntry    The double pointer of version type resource directory entry

 @retval EFI_SUCCESS            Successfully get version type resource directory
 @retval EFI_INVALID_PARAMETER  Input pointer is NULL
 @retval EFI_NOT_FOUND          There is no version type resource directory
**/
STATIC
EFI_STATUS
GetVersionResourceDir (
  IN  UINT8                                 *ResourceData,
  OUT EFI_IMAGE_RESOURCE_DIRECTORY          **VersionDir,
  OUT EFI_IMAGE_RESOURCE_DIRECTORY_ENTRY    **VersionDirEntry
  )
{
  EFI_IMAGE_RESOURCE_DIRECTORY              *Dir;
  EFI_IMAGE_RESOURCE_DIRECTORY_ENTRY        *DirEntry;
  UINT16                                    Index;
  UINT16                                    Count;

  if (ResourceData == NULL || VersionDir == NULL || VersionDirEntry == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Dir      = (EFI_IMAGE_RESOURCE_DIRECTORY *) ResourceData;
  DirEntry = (EFI_IMAGE_RESOURCE_DIRECTORY_ENTRY *) ((UINT8 *) Dir + sizeof (EFI_IMAGE_RESOURCE_DIRECTORY));

  Count = Dir->NumberOfNamedEntries + Dir->NumberOfIdEntries;
  for (Index = 0; Index < Count; Index++) {
    if (DirEntry[Index].u1.Id == RESOURCE_SECTION_RESOURCE_TYPE_VERSION &&
        DirEntry[Index].u2.s.DataIsDirectory) {
      *VersionDir      = (EFI_IMAGE_RESOURCE_DIRECTORY *) (ResourceData + DirEntry[Index].u2.s.OffsetToDirectory);
      *VersionDirEntry = (EFI_IMAGE_RESOURCE_DIRECTORY_ENTRY *) ((UINT8 *) *VersionDir + sizeof (EFI_IMAGE_RESOURCE_DIRECTORY));
      return EFI_SUCCESS;
    }
  }

  return EFI_NOT_FOUND;
}

/**
 Get the binary version number of Windows OS

 @param[in]  ResourceDir        The pointer of resource directory of image
 @param[in]  ResourceData       The pointer of resource data of image
 @param[out] VersionMS          The most significant 32 bits of the binary version number of Windows OS
 @param[out] VersionLS          The least significant 32 bits of the binary version number of Windows OS

 @retval EFI_SUCCESS            Successfully get binary version number
 @retval EFI_INVALID_PARAMETER  Input pointer is NULL or data size is zero
 @retval EFI_NOT_FOUND          There is no version number information in image
**/
STATIC
EFI_STATUS
GetWindowsOsVer (
  IN  EFI_IMAGE_DATA_DIRECTORY          *ResourceDir,
  IN  UINT8                             *ResourceData,
  OUT UINT32                            *VersionMS,
  OUT UINT32                            *VersionLS
  )
{
  EFI_STATUS                            Status;
  EFI_IMAGE_RESOURCE_DIRECTORY          *VersionDir;
  EFI_IMAGE_RESOURCE_DIRECTORY_ENTRY    *VersionDirEntry;
  EFI_IMAGE_RESOURCE_DIRECTORY          *Dir;
  EFI_IMAGE_RESOURCE_DIRECTORY_ENTRY    *DirEntry;
  EFI_IMAGE_RESOURCE_DATA_ENTRY         *DataEntry;
  UINT16                                Index;
  UINT16                                Count;
  UINT8                                 *Ptr;
  UINT8                                 *VersionInfo;
  CHAR16                                *KeyStr;
  UINT16                                MaxPaddingCount;
  VS_FIXEDFILEINFO                      *FixedFileInfo;

  if (ResourceDir == NULL || ResourceData == NULL || VersionMS == NULL || VersionLS == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = GetVersionResourceDir (ResourceData, &VersionDir, &VersionDirEntry);
  if (Status != EFI_SUCCESS) {
    return EFI_NOT_FOUND;
  }

  Count = VersionDir->NumberOfNamedEntries + VersionDir->NumberOfIdEntries;
  for (Index = 0; Index < Count; Index++) {
    if (!VersionDirEntry[Index].u2.s.DataIsDirectory) {
      continue;
    }

    Dir      = (EFI_IMAGE_RESOURCE_DIRECTORY *) (ResourceData + VersionDirEntry[Index].u2.s.OffsetToDirectory);
    DirEntry = (EFI_IMAGE_RESOURCE_DIRECTORY_ENTRY *) ((UINT8 *) Dir + sizeof (EFI_IMAGE_RESOURCE_DIRECTORY));
    if (DirEntry->u2.s.DataIsDirectory) {
      continue;
    }

    DataEntry = (EFI_IMAGE_RESOURCE_DATA_ENTRY *) (ResourceData + DirEntry->u2.OffsetToData);
    if (DataEntry->OffsetToData < ResourceDir->VirtualAddress) {
      continue;
    }

    //
    // Get version info data entry VS_VERSIONINFO by comparing key string.
    // Key string is located after VersionInfoSize (UINT16), ValueSize (UINT16) and Type (UINT16).
    //
    VersionInfo = ResourceData + (DataEntry->OffsetToData - ResourceDir->VirtualAddress);
    KeyStr      = (CHAR16 *) (VersionInfo + sizeof (UINT16) + sizeof (UINT16) + sizeof (UINT16));
    if (StrCmp (KeyStr, L"VS_VERSION_INFO") != 0) {
      continue;
    }
    Ptr  = (UINT8 *) KeyStr;
    Ptr += StrSize (KeyStr);

    MaxPaddingCount = 3;
    while (*Ptr == 0 && MaxPaddingCount > 0) {
      Ptr++;
      MaxPaddingCount--;
    }
    if (MaxPaddingCount == 0) {
      continue;
    }

    FixedFileInfo = (VS_FIXEDFILEINFO *) Ptr;
    if (FixedFileInfo->Signature != VS_FIXEDFILEINFO_SIGNATURE) {
      continue;
    }

    *VersionMS = FixedFileInfo->ProductVersionMS;
    *VersionLS = FixedFileInfo->ProductVersionLS;
    return EFI_SUCCESS;
  }

  return EFI_NOT_FOUND;
}

/**
 Get resource section header

 @param[in]  ThisFile               File Handle
 @param[in]  DosHeader              The pointer of image DOS header
 @param[in]  NumOfSections          Number of section header in image
 @param[out] ResourceSectionHeader  The pointer of resouces section header

 @retval EFI_SUCCESS                Successfully get resource section header
 @retval EFI_INVALID_PARAMETER      Input pointer parameter is NULL
 @retval EFI_BUFFER_TOO_SMALL       Allocate pool fail
 @retval EFI_NOT_FOUND              Can not find resource section
 @retval Other                      Fail to set file position or read data
**/
STATIC
EFI_STATUS
GetResourceSectionHeader (
  IN  EFI_FILE_HANDLE                   ThisFile,
  IN  EFI_IMAGE_DOS_HEADER              *DosHeader,
  IN  UINT16                            NumOfSections,
  OUT EFI_IMAGE_SECTION_HEADER          *ResourceSectionHeader
  )
{
  EFI_STATUS                            Status;
  UINT8                                 *Buffer;
  UINTN                                 BufferSize;
  UINT16                                Index;
  EFI_IMAGE_SECTION_HEADER              *SectionHdrPtr;
  EFI_IMAGE_OPTIONAL_HEADER_UNION       PeHdr;
  UINTN                                 PeHdrSize;

  if (ThisFile == NULL || DosHeader == NULL || ResourceSectionHeader == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = ThisFile->SetPosition (ThisFile, DosHeader->e_lfanew);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  BufferSize = sizeof(PeHdr);
  ZeroMem (&PeHdr, BufferSize);
  Status = ThisFile->Read (ThisFile, &BufferSize, &PeHdr);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if (PeHdr.Pe32.OptionalHeader.Magic == EFI_IMAGE_NT_OPTIONAL_HDR32_MAGIC) {
    PeHdrSize = sizeof(EFI_IMAGE_NT_HEADERS32);
  } else if (PeHdr.Pe32.OptionalHeader.Magic == EFI_IMAGE_NT_OPTIONAL_HDR64_MAGIC) {
    PeHdrSize = sizeof(EFI_IMAGE_NT_HEADERS64);
  } else {
    return EFI_UNSUPPORTED;
  }

  Status = ThisFile->SetPosition (ThisFile, DosHeader->e_lfanew + PeHdrSize);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  BufferSize = NumOfSections * sizeof (EFI_IMAGE_SECTION_HEADER);
  Buffer     = AllocatePool (BufferSize);
  if (Buffer == NULL) {
    return EFI_BUFFER_TOO_SMALL;
  }

  Status = ThisFile->Read (ThisFile, &BufferSize, Buffer);
  if (EFI_ERROR (Status)) {
    FreePool (Buffer);
    return Status;
  }

  SectionHdrPtr = (EFI_IMAGE_SECTION_HEADER *) Buffer;
  for (Index = 0; Index < NumOfSections; Index++) {
    if (AsciiStrnCmp ((CHAR8 *) SectionHdrPtr, ".rsrc", 5) == 0) {
      CopyMem (ResourceSectionHeader, SectionHdrPtr, sizeof (EFI_IMAGE_SECTION_HEADER));
      break;
    }
    SectionHdrPtr++;
  }

  FreePool (Buffer);
  return (Index < NumOfSections) ? EFI_SUCCESS : EFI_NOT_FOUND;
}

/**
 Get raw data of resource section

 @param[in]  ThisFile               File Handle
 @param[in]  SectionHeader          The pointer of seaction header
 @param[out] RawData                The double pointer of raw data
 @param[out] RawDataSize            The pointer of raw data size

 @retval EFI_SUCCESS                Successfully get raw data
 @retval EFI_INVALID_PARAMETER      Input pointer parameter is NULL
 @retval EFI_BUFFER_TOO_SMALL       Allocate pool fail
 @retval Other                      Fail to set file position or read data
**/
STATIC
EFI_STATUS
GetResourceSectionRawData (
  IN  EFI_FILE_HANDLE                    ThisFile,
  IN  EFI_IMAGE_SECTION_HEADER           *SectionHeader,
  OUT UINT8                              **RawData,
  OUT UINT32                             *RawDataSize
  )
{
  EFI_STATUS                            Status;
  UINT8                                 *Buffer;
  UINTN                                 BufferSize;

  if (ThisFile == NULL || SectionHeader == NULL || RawData == NULL || RawDataSize == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = ThisFile->SetPosition (ThisFile, SectionHeader->PointerToRawData);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Read real file size. VirtualSize is the real file size and SizeOfRawData is the size with file alignment
  //
  BufferSize = SectionHeader->Misc.VirtualSize;
  Buffer     = AllocatePool (BufferSize);
  if (Buffer == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  Status = ThisFile->Read (ThisFile, &BufferSize, Buffer);
  if (EFI_ERROR (Status)) {
    FreePool (Buffer);
    return Status;
  }

  *RawData     = Buffer;
  *RawDataSize = (UINT32) BufferSize;

  return EFI_SUCCESS;
}

/**
 Get resource section data from image file

 @param[in]  Device               Device handle
 @param[in]  FileName             Target file name
 @param[out] ResourceDataDir      The pointer of resource data directory
 @param[out] ResourceData         The double pointer of resource data
 @param[out] ResourceDataSize     The pointer of resource data size

 @retval EFI_SUCCESS              Successfully get resource section data
 @retval EFI_NOT_FOUND            There is no resource data directory in image file
 @retval EFI_INVALID_PARAMETER    The pointer of input parameter is NULL
 @retval Other                    Fail to get resource section data
**/
STATIC
EFI_STATUS
GetResourceSectionData (
  IN  EFI_HANDLE                        Device,
  IN  CHAR16                            *FileName,
  OUT EFI_IMAGE_DATA_DIRECTORY          *ResourceDataDir,
  OUT UINT8                             **ResourceData,
  OUT UINT32                            *ResourceDataSize
  )
{
  EFI_STATUS                            Status;
  EFI_IMAGE_DOS_HEADER                  DosHeader;
  EFI_IMAGE_OPTIONAL_HEADER_PTR_UNION   Hdr;
  EFI_IMAGE_OPTIONAL_HEADER_UNION       HdrData;
  UINT16                                Magic;
  EFI_IMAGE_DATA_DIRECTORY              *DataDir;
  UINT16                                NumOfSections;
  EFI_IMAGE_SECTION_HEADER              ResourceSectionHeader;
  EFI_SIMPLE_FILE_SYSTEM_PROTOCOL       *Volume;
  EFI_FILE_HANDLE                       Root;
  EFI_FILE_HANDLE                       ThisFile;

  if (FileName == NULL || ResourceDataDir == NULL || ResourceData == NULL || ResourceDataSize == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Get resource data directory
  //
  Hdr.Union = &HdrData;
  Status    = BdsLibGetImageHeader (Device, FileName, &DosHeader, Hdr);
  if (EFI_ERROR (Status) || !EFI_IMAGE_MACHINE_TYPE_SUPPORTED (Hdr.Pe32->FileHeader.Machine)) {
    return EFI_NOT_FOUND;
  }

  if (Hdr.Pe32->FileHeader.Machine == EFI_IMAGE_MACHINE_IA64 && Hdr.Pe32->OptionalHeader.Magic == EFI_IMAGE_NT_OPTIONAL_HDR32_MAGIC) {
    Magic = EFI_IMAGE_NT_OPTIONAL_HDR64_MAGIC;
  } else {
    Magic = Hdr.Pe32->OptionalHeader.Magic;
  }

  if (Magic == EFI_IMAGE_NT_OPTIONAL_HDR32_MAGIC) {
    DataDir       = (EFI_IMAGE_DATA_DIRECTORY *) &Hdr.Pe32->OptionalHeader.DataDirectory[EFI_IMAGE_DIRECTORY_ENTRY_RESOURCE];
    NumOfSections = Hdr.Pe32->FileHeader.NumberOfSections;
  } else {
    DataDir       = (EFI_IMAGE_DATA_DIRECTORY *) &Hdr.Pe32Plus->OptionalHeader.DataDirectory[EFI_IMAGE_DIRECTORY_ENTRY_RESOURCE];
    NumOfSections = Hdr.Pe32Plus->FileHeader.NumberOfSections;
  }

  if (DataDir->Size == 0) {
    return EFI_NOT_FOUND;
  }

  //
  // Open file and read resource section data
  //
  Root     = NULL;
  ThisFile = NULL;
  Status = gBS->HandleProtocol (Device, &gEfiSimpleFileSystemProtocolGuid, (VOID *) &Volume);
  if (EFI_ERROR (Status)) {
    goto Done;
  }

  Status = Volume->OpenVolume (Volume, &Root);
  if (EFI_ERROR (Status)) {
    Root = NULL;
    goto Done;
  }

  Status = Root->Open (Root, &ThisFile, FileName, EFI_FILE_MODE_READ, 0);
  if (EFI_ERROR (Status)) {
    goto Done;
  }

  Status = GetResourceSectionHeader (ThisFile, &DosHeader, NumOfSections, &ResourceSectionHeader);
  if (EFI_ERROR (Status)) {
    goto Done;
  }

  Status = GetResourceSectionRawData (ThisFile, &ResourceSectionHeader, ResourceData, ResourceDataSize);
  if (EFI_ERROR (Status)) {
    goto Done;
  }

  CopyMem (ResourceDataDir, DataDir, sizeof (EFI_IMAGE_DATA_DIRECTORY));

Done:
  if (ThisFile != NULL) {
    ThisFile->Close (ThisFile);
  }
  if (Root != NULL) {
    Root->Close (Root);
  }

  return Status;
}

/**
 Indicates if the OS version of boot option device path matches, or is greater than, the Windows 8 version

 @param[in] BootOptionDevicePath   The device path of boot option

 @retval TRUE   The OS version of boot option device path matches, or is greater than, the Windows 8 version
 @retval FALSE  The OS version of boot option device path does not matches, or is not greater than, the Windows 8 version
**/
STATIC
BOOLEAN
IsWindows8OrGreater (
  IN EFI_DEVICE_PATH_PROTOCOL   *BootOptionDevicePath
  )
{
  EFI_STATUS                    Status;
  EFI_DEVICE_PATH_PROTOCOL      *WorkingDevicePath;
  EFI_DEVICE_PATH_PROTOCOL      *FullDevicePath;
  EFI_IMAGE_DATA_DIRECTORY      ResourceDataDir;
  FILEPATH_DEVICE_PATH          *FilePath;
  UINT8                         *ResourceData;
  UINT32                        ResourceDataSize;
  UINT32                        VersionMS;
  UINT32                        VersionLS;
  EFI_HANDLE                    Handle;

  if (BootOptionDevicePath == NULL) {
    return FALSE;
  }

  //
  // Get device path handle
  //
  FullDevicePath = NULL;
  if ((DevicePathType (BootOptionDevicePath) == MEDIA_DEVICE_PATH) &&
      (DevicePathSubType (BootOptionDevicePath) == MEDIA_HARDDRIVE_DP)) {
    FullDevicePath = BdsExpandPartitionPartialDevicePathToFull ((HARDDRIVE_DEVICE_PATH *) BootOptionDevicePath);
    if (FullDevicePath == NULL) {
      return FALSE;
    }

    WorkingDevicePath = FullDevicePath;
  } else {
    WorkingDevicePath = BootOptionDevicePath;
  }

  Status = gBS->LocateDevicePath (
                  &gEfiSimpleFileSystemProtocolGuid,
                  &WorkingDevicePath,
                  &Handle
                  );
  if (FullDevicePath != NULL) {
    FreePool (FullDevicePath);
  }
  if (EFI_ERROR (Status)) {
    return FALSE;
  }

  //
  // Check if the OS version of boot option device path matches, or is greater than, the Windows 8 version
  //
  FilePath = (FILEPATH_DEVICE_PATH *) GetDevicePathNode (BootOptionDevicePath, MEDIA_DEVICE_PATH, MEDIA_FILEPATH_DP);
  if (FilePath == NULL) {
    return FALSE;
  }

  ResourceData = NULL;
  Status = GetResourceSectionData (Handle, FilePath->PathName, &ResourceDataDir, &ResourceData, &ResourceDataSize);
  if (Status != EFI_SUCCESS) {
    return FALSE;
  }

  Status = GetWindowsOsVer (&ResourceDataDir, ResourceData, &VersionMS, &VersionLS);
  if (Status != EFI_SUCCESS) {
    return FALSE;
  }

  return (VersionMS >= MICROSOFT_OS_VERSION_WINDOWS_8) ? TRUE : FALSE;
}

/**
  Check if the file of boot option device path belongs to fast boot UEFI OS.

  @param[in] BootOptionDevicePath   The device path of boot option

  @retval TRUE   The file of boot option device path belongs to fast boot UEFI OS
  @retval FALSE  The file of boot option device path does not belong to fast boot UEFI OS
**/
STATIC
BOOLEAN
IsFastBootUefiOs (
  IN EFI_DEVICE_PATH_PROTOCOL   *BootOptionDevicePath
  )
{
  BOOLEAN                       Result;
  FILEPATH_DEVICE_PATH          *FilePath;
  CHAR16                        *PcdFastBootOsList;
  CHAR16                        *FastBootOsList;
  CHAR16                        *FastBootOs;
  CHAR16                        *NextFastBootOs;

  Result = FALSE;

  FilePath = (FILEPATH_DEVICE_PATH *) GetDevicePathNode (BootOptionDevicePath, MEDIA_DEVICE_PATH, MEDIA_FILEPATH_DP);
  if (FilePath == NULL) {
    return Result;
  }

  PcdFastBootOsList = PcdGetPtr (PcdUefiOsFastBootList);
  FastBootOsList    = AllocateCopyPool (StrSize (PcdFastBootOsList), PcdFastBootOsList);
  if (FastBootOsList == NULL) {
    return Result;
  }

  NextFastBootOs = FastBootOsList;
  while (NextFastBootOs != NULL) {
    FastBootOs = NextFastBootOs;
    //
    // Skip front space characters of file path string
    //
    while (*FastBootOs == ' ') {
      FastBootOs++;
    }
    if (*FastBootOs == CHAR_NULL) {
      break;
    }

    //
    // Because two files are separated by space char, modify space char to null and set NextFastBootOs pointer to next char.
    //
    NextFastBootOs = StrStr (FastBootOs, L" ");
    if (NextFastBootOs != NULL) {
      *NextFastBootOs = CHAR_NULL;
      NextFastBootOs++;
    }
    if (StrStr (FastBootOs, FilePath->PathName) != NULL) {
      if (StrStr (FilePath->PathName, L"\\EFI\\Microsoft\\Boot\\bootmgfw.efi") != NULL) {
        Result = IsWindows8OrGreater (BootOptionDevicePath);
      } else {
        Result = TRUE;
      }
      break;
    }
  }

  FreePool (FastBootOsList);

  return Result;
}

/**
 Update the BlockIo device path of target boot device into TargetHddDevPath variable.
 Assume target boot device is HDD in the first boot option in BootOrder.

 @retval EFI_SUCCESS            Update target HDD variable success.
 @retval EFI_NOT_FOUND          BootOrder or Boot#### variable does not exist.
**/
EFI_STATUS
UpdateTargetHddVariable (
  VOID
  )
{
  EFI_STATUS                    Status;
  UINT16                        *BootOrder;
  UINTN                         Size;
  EFI_DEVICE_PATH_PROTOCOL      *OptionDevicePath;
  UINTN                         BlockIndex;
  UINTN                         BlockIoHandleCount;
  EFI_HANDLE                    *BlockIoBuffer;
  EFI_DEVICE_PATH_PROTOCOL      *BlockIoDevicePath;


  BootOrder = BdsLibGetVariableAndSize (
                L"BootOrder",
                &gEfiGlobalVariableGuid,
                &Size
                );
  if (BootOrder == NULL) {
    return EFI_NOT_FOUND;
  }

  OptionDevicePath = BdsLibGetDevicePathFromBootOption (BootOrder[0]);
  if (OptionDevicePath == NULL) {
    FreePool (BootOrder);
    return EFI_NOT_FOUND;
  }

  if (((DevicePathType (OptionDevicePath) == MEDIA_DEVICE_PATH) &&
       (DevicePathSubType (OptionDevicePath) == MEDIA_HARDDRIVE_DP)) &&
      IsFastBootUefiOs (OptionDevicePath)) {
    BlockIoBuffer = NULL;
    Status = gBS->LocateHandleBuffer (
                    ByProtocol,
                    &gEfiBlockIoProtocolGuid,
                    NULL,
                    &BlockIoHandleCount,
                    &BlockIoBuffer
                    );
    if (!EFI_ERROR (Status)) {
      for (BlockIndex = 0; BlockIndex < BlockIoHandleCount; BlockIndex++) {
        Status = gBS->HandleProtocol (
                        BlockIoBuffer[BlockIndex],
                        &gEfiDevicePathProtocolGuid,
                        (VOID **) &BlockIoDevicePath
                        );
        if (EFI_ERROR (Status) || BlockIoDevicePath == NULL) {
          continue;
        }

        if (MatchPartitionDevicePathNode (BlockIoDevicePath, (HARDDRIVE_DEVICE_PATH *) OptionDevicePath)) {
          SetVariableToSensitiveVariable (
            L"TargetHddDevPath",
            &gEfiGenericVariableGuid,
            EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS,
            GetDevicePathSize (BlockIoDevicePath),
            BlockIoDevicePath
            );
          break;
        }
      }

      if (BlockIoBuffer != NULL) {
        FreePool (BlockIoBuffer);
      }
    }
  }

  FreePool (OptionDevicePath);
  FreePool (BootOrder);

  return EFI_SUCCESS;
}

/**
 Set target HDD in boot list as connected.
**/
VOID
SetTargetHddConnected (
  VOID
  )
{
  EFI_DEVICE_PATH_PROTOCOL      *BlockIoDevicePath;
  UINTN                         Size;
  LIST_ENTRY                    *OptionList;
  LIST_ENTRY                    *Link;
  H2O_BDS_LOAD_OPTION           *CurrentLoadOption;
  EFI_STATUS                    Status;

  BlockIoDevicePath = BdsLibGetVariableAndSize (
                        L"TargetHddDevPath",
                        &gEfiGenericVariableGuid,
                        &Size
                        );
  if (BlockIoDevicePath == NULL) {
    return;
  }
  if (GetDevicePathSizeEx (BlockIoDevicePath, Size) == 0) {
    FreePool (BlockIoDevicePath);
    return;
  }

  Status = gBdsServices->GetBootList (gBdsServices, &OptionList);
  if (Status != EFI_SUCCESS) {
    return;
  }

  for (Link = GetFirstNode (OptionList); !IsNull (OptionList, Link); Link = GetNextNode (OptionList, Link)) {
    CurrentLoadOption = BDS_OPTION_FROM_LINK (Link);
    if (((DevicePathType (CurrentLoadOption->DevicePath) == MEDIA_DEVICE_PATH) &&
         (DevicePathSubType (CurrentLoadOption->DevicePath) == MEDIA_HARDDRIVE_DP)) &&
        MatchPartitionDevicePathNode (BlockIoDevicePath, (HARDDRIVE_DEVICE_PATH *) CurrentLoadOption->DevicePath)) {
      CurrentLoadOption->Connected = TRUE;
    }
  }
  FreePool (BlockIoDevicePath);
}

/**
 Check target device is valid or not by comparing device path from TargetHddDevPath variable.

 @retval TRUE                   Target device is valid.
 @retval FALSE                  Target device is not valid.
**/
STATIC
BOOLEAN
TargetDevIsValid (
  VOID
  )
{
  BOOLEAN                       Valid;
  UINT16                        *BootOrder;
  UINTN                         Size;
  EFI_DEVICE_PATH_PROTOCOL      *OptionDevicePath;
  EFI_DEVICE_PATH_PROTOCOL      *BlockIoDevicePath;
  CHAR16                        BootOption[BOOT_OPTION_MAX_CHAR];
  EFI_LOAD_OPTION               *BootOptionVar;

  BootOrder = BdsLibGetVariableAndSize (
                L"BootOrder",
                &gEfiGlobalVariableGuid,
                &Size
                );
  if (BootOrder == NULL) {
    return FALSE;
  }

  OptionDevicePath = BdsLibGetDevicePathFromBootOption (BootOrder[0]);
  if (OptionDevicePath == NULL) {
    FreePool (BootOrder);
    return FALSE;
  }
  
  UnicodeSPrint (BootOption, sizeof (BootOption), L"Boot%04x", BootOrder[0]);
  BootOptionVar = (EFI_LOAD_OPTION *) BdsLibGetVariableAndSize (
                                        BootOption,
                                        &gEfiGlobalVariableGuid,
                                        &Size
                                        );
  if (BootOptionVar == NULL || !IS_LOAD_OPTION_TYPE (BootOptionVar->Attributes, LOAD_OPTION_ACTIVE)) {
    FreePool ((VOID*)BootOptionVar);
    return FALSE;
  }  

  Valid = FALSE;

  if (((DevicePathType (OptionDevicePath) == MEDIA_DEVICE_PATH) &&
       (DevicePathSubType (OptionDevicePath) == MEDIA_HARDDRIVE_DP))) {
    BlockIoDevicePath = BdsLibGetVariableAndSize (
                          L"TargetHddDevPath",
                          &gEfiGenericVariableGuid,
                          &Size
                          );
    if (BlockIoDevicePath != NULL &&
        GetDevicePathSizeEx (BlockIoDevicePath, Size) != 0 &&
        MatchPartitionDevicePathNode (BlockIoDevicePath, (HARDDRIVE_DEVICE_PATH *) OptionDevicePath)) {
      Valid = TRUE;
      FreePool (BlockIoDevicePath);
    }
  }

  FreePool (OptionDevicePath);
  FreePool (BootOrder);
  FreePool ((VOID*)BootOptionVar);

  return Valid;
}

/**
 Check Win 8 Fast Boot feature is active or not.

 @retval TRUE                   Win 8 Fast Boot feature is active.
 @retval FALSE                  Win 8 Fast Boot feature is not active.
**/
BOOLEAN
BdsLibIsWin8FastBootActive (
  VOID
  )
{
  EFI_STATUS                    Status;
  KERNEL_CONFIGURATION          SystemConfiguration;
  UINT16                        BootNext;
  UINTN                         Size;

  Size = sizeof (BootNext);
  Status = gRT->GetVariable (
                  L"BootNext",
                  &gEfiGlobalVariableGuid,
                  NULL,
                  &Size,
                  &BootNext
                  );
  if (!EFI_ERROR (Status)) {
    return FALSE;
  }

  Status = GetKernelConfiguration (&SystemConfiguration);
  if (EFI_ERROR (Status)) {
    return FALSE;
  }

  if (SystemConfiguration.Win8FastBoot == 0 &&
      BdsLibGetBootType () == EFI_BOOT_TYPE) {

    switch (SystemConfiguration.NewPositionPolicy) {

    case ADD_POSITION_AUTO:
      if (TargetDevIsValid ()) {
        return TRUE;
      }
      break;

    case ADD_POSITION_FIRST:
    case ADD_POSITION_LAST:
    default:
      break;
    }
  }

  return FALSE;
}

