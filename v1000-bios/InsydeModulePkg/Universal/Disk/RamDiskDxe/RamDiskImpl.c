/** @file

;******************************************************************************
;* Copyright (c) 2020, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

/** @file
  HII Config Access protocol implementation of RamDiskDxe driver.

  Copyright (c) 2016, Intel Corporation. All rights reserved.<BR>
  (C) Copyright 2016-2018 Hewlett Packard Enterprise Development LP<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "RamDiskImpl.h"
#include "RamDisk.h"
#include "FileExplorer.h"


extern LIST_ENTRY  mRegisteredRamDisks;
extern LIST_ENTRY                  mRamDiskDeviceList;
extern H2O_RAM_DISK_DEVICE_PATH    mH2ORamDiskDevicePath;
extern UINT32                      mRamDiskDeviceLNum;
extern EFI_RAM_DISK_PROTOCOL           *mRamDiskProtocol;

RAM_DISK_CONFIG_PRIVATE_DATA mRamDiskConfigPrivateDataTemplate = {
  RAM_DISK_CONFIG_PRIVATE_DATA_SIGNATURE,
  {
    EFI_PAGE_SIZE,
    RAM_DISK_BOOT_SERVICE_DATA_MEMORY
  },
  {
    RamDiskExtractConfig,
    RamDiskRouteConfig,
    RamDiskCallback
  }
};

HII_VENDOR_DEVICE_PATH       mRamDiskHiiVendorDevicePath = {
  {
    {
      HARDWARE_DEVICE_PATH,
      HW_VENDOR_DP,
      {
        (UINT8) (sizeof (VENDOR_DEVICE_PATH)),
        (UINT8) ((sizeof (VENDOR_DEVICE_PATH)) >> 8)
      }
    },
    H2O_RAM_DISK_FORMSET_GUID
  },
  {
    END_DEVICE_PATH_TYPE,
    END_ENTIRE_DEVICE_PATH_SUBTYPE,
    {
      (UINT8) (END_DEVICE_PATH_LENGTH),
      (UINT8) ((END_DEVICE_PATH_LENGTH) >> 8)
    }
  }
};




/**
  Insert a RAM Disk into link list of H2O RAM Disk Protocol.

  @param[in]  StartingAddress     Base Address of the RAM Disk
  @param[in]  Size                Size of the RAM Disk

  @retval EFI_SUCCESS             HII Form is installed successfully.
  @retval EFI_OUT_OF_RESOURCES    Not enough resource for updating link list of RAM Disk of H2O RAM Disk Protocol.

**/
EFI_STATUS
UpdateRamDiskDeviceList(
  IN UINT64                     StartingAddress,
  IN UINT64                     Size
)
{

  EFI_STATUS           Status;
  UINT64               DiskId;
  UINT32               BlockSize;
  RAM_DISK_DEV         *RamDiskDev = NULL;

  //
  //  Allocate storage for RAM Disk device info on the heap.
  //
  RamDiskDev = AllocateZeroPool (sizeof(RAM_DISK_DEV));
  if (RamDiskDev == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  BlockSize   = 512;

  RamDiskDev->Start = StartingAddress;


  //
  //  Initialize the RAM Disk's device info.
  //
  Status = gBS->GetNextMonotonicCount (&DiskId);
  CopyMem (&mH2ORamDiskDevicePath.DiskId, &DiskId, sizeof(DiskId));

  RamDiskDev->Signature              = PBLOCK_DEVICE_SIGNATURE;
  RamDiskDev->BlkIo.Revision         = EFI_BLOCK_IO_INTERFACE_REVISION;
  RamDiskDev->BlkIo.Media            = &RamDiskDev->Media;
  RamDiskDev->Media.RemovableMedia   = FALSE;
  RamDiskDev->Media.MediaPresent     = TRUE;

  RamDiskDev->Media.LastBlock        = Size / BlockSize - 1;
  RamDiskDev->Media.BlockSize        = BlockSize;
  RamDiskDev->Media.LogicalPartition = TRUE;
  RamDiskDev->Media.ReadOnly         = FALSE;
  RamDiskDev->Media.WriteCaching     = TRUE;

  RamDiskDev->BlkIo.ReadBlocks       = RamDiskReadBlocks;
  RamDiskDev->BlkIo.WriteBlocks      = RamDiskWriteBlocks;
  RamDiskDev->BlkIo.FlushBlocks      = RamDiskFlushBlocks;

  RamDiskDev->DevicePath = DuplicateDevicePath ((EFI_DEVICE_PATH_PROTOCOL*)&mH2ORamDiskDevicePath);

  InsertTailList (&mRamDiskDeviceList, &RamDiskDev->Link);
  mRamDiskDeviceLNum += 1;

  return EFI_SUCCESS;
}



/**
  This function publish the RAM disk configuration Form.

  @param[in, out]  ConfigPrivateData
                             Points to RAM disk configuration private data.

  @retval EFI_SUCCESS             HII Form is installed successfully.
  @retval EFI_OUT_OF_RESOURCES    Not enough resource for HII Form installation.
  @retval Others                  Other errors as indicated.

**/
EFI_STATUS
InstallRamDiskConfigForm (
  IN OUT RAM_DISK_CONFIG_PRIVATE_DATA       *ConfigPrivateData
  )
{
  EFI_STATUS                      Status;
  EFI_HII_HANDLE                  HiiHandle;
  EFI_HANDLE                      DriverHandle;
  EFI_HII_CONFIG_ACCESS_PROTOCOL  *ConfigAccess;

  DriverHandle = NULL;
  ConfigAccess = &ConfigPrivateData->ConfigAccess;
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &DriverHandle,
                  &gEfiDevicePathProtocolGuid,
                  &mRamDiskHiiVendorDevicePath,
                  &gEfiHiiConfigAccessProtocolGuid,
                  ConfigAccess,
                  NULL
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  ConfigPrivateData->DriverHandle = DriverHandle;

  //
  // Publish the HII package list
  //
  HiiHandle = HiiAddPackages (
                &gH2ORamDiskFormSetGuid,
                DriverHandle,
                RamDiskDxeDriverStrings,
                RamDiskHiiBin,
                NULL
                );
  if (HiiHandle == NULL) {
    gBS->UninstallMultipleProtocolInterfaces (
           DriverHandle,
           &gEfiDevicePathProtocolGuid,
           &mRamDiskHiiVendorDevicePath,
           &gEfiHiiConfigAccessProtocolGuid,
           ConfigAccess,
           NULL
           );
    return EFI_OUT_OF_RESOURCES;
  }

  ConfigPrivateData->HiiHandle = HiiHandle;

  return EFI_SUCCESS;
}


/**
  This function removes RAM disk configuration Form.

  @param[in, out]  ConfigPrivateData
                             Points to RAM disk configuration private data.

**/
VOID
UninstallRamDiskConfigForm (
  IN OUT RAM_DISK_CONFIG_PRIVATE_DATA       *ConfigPrivateData
  )
{
  //
  // Uninstall HII package list
  //
  if (ConfigPrivateData->HiiHandle != NULL) {
    HiiRemovePackages (ConfigPrivateData->HiiHandle);
    ConfigPrivateData->HiiHandle = NULL;
  }

  //
  // Uninstall HII Config Access Protocol
  //
  if (ConfigPrivateData->DriverHandle != NULL) {
    gBS->UninstallMultipleProtocolInterfaces (
           ConfigPrivateData->DriverHandle,
           &gEfiDevicePathProtocolGuid,
           &mRamDiskHiiVendorDevicePath,
           &gEfiHiiConfigAccessProtocolGuid,
           &ConfigPrivateData->ConfigAccess,
           NULL
           );
    ConfigPrivateData->DriverHandle = NULL;
  }

  FreePool (ConfigPrivateData);
}


/**
  This function allows a caller to extract the current configuration for one
  or more named elements from the target driver.

  @param[in]  This           Points to the EFI_HII_CONFIG_ACCESS_PROTOCOL.
  @param[in]  Request        A null-terminated Unicode string in
                             <ConfigRequest> format.
  @param[out] Progress       On return, points to a character in the Request
                             string. Points to the string's null terminator if
                             request was successful. Points to the most recent
                             '&' before the first failing name/value pair (or
                             the beginning of the string if the failure is in
                             the first name/value pair) if the request was not
                             successful.
  @param[out] Results        A null-terminated Unicode string in
                             <ConfigAltResp> format which has all values filled
                             in for the names in the Request string. String to
                             be allocated by the called function.

  @retval EFI_SUCCESS             The Results is filled with the requested
                                  values.
  @retval EFI_OUT_OF_RESOURCES    Not enough memory to store the results.
  @retval EFI_INVALID_PARAMETER   Request is illegal syntax, or unknown name.
  @retval EFI_NOT_FOUND           Routing data doesn't match any storage in
                                  this driver.

**/
EFI_STATUS
EFIAPI
RamDiskExtractConfig (
  IN CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN CONST EFI_STRING                       Request,
       OUT EFI_STRING                       *Progress,
       OUT EFI_STRING                       *Results
  )
{
  if (Progress == NULL || Results == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  *Progress = Request;
  return EFI_NOT_FOUND;
}


/**
  This function processes the results of changes in configuration.

  @param[in]  This           Points to the EFI_HII_CONFIG_ACCESS_PROTOCOL.
  @param[in]  Configuration  A null-terminated Unicode string in <ConfigResp>
                             format.
  @param[out] Progress       A pointer to a string filled in with the offset of
                             the most recent '&' before the first failing
                             name/value pair (or the beginning of the string if
                             the failure is in the first name/value pair) or
                             the terminating NULL if all was successful.

  @retval EFI_SUCCESS             The Results is processed successfully.
  @retval EFI_INVALID_PARAMETER   Configuration is NULL.
  @retval EFI_NOT_FOUND           Routing data doesn't match any storage in
                                  this driver.

**/
EFI_STATUS
EFIAPI
RamDiskRouteConfig (
  IN CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN CONST EFI_STRING                       Configuration,
       OUT EFI_STRING                       *Progress
  )
{
  if (Configuration == NULL || Progress == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  *Progress = Configuration;

  return EFI_NOT_FOUND;
}


/**
  Allocate memory and register the RAM disk created within RamDiskDxe
  driver HII.

  @param[in] Size            If creating raw, size of the RAM disk to create.
                             If creating from file, zero.
  @param[in] FileHandle      If creating raw, NULL. If creating from file, the
                             file handle.
  @param[in] MemoryType      Type of memory to be used to create RAM Disk.

  @retval EFI_SUCCESS             RAM disk is created and registered.
  @retval EFI_OUT_OF_RESOURCES    Not enough storage is available to match the
                                  size required.

**/
EFI_STATUS
HiiCreateRamDisk (
  IN UINT64                                 Size,
  IN EFI_FILE_HANDLE                        FileHandle,
  IN UINT8                                  MemoryType
  )
{
  EFI_STATUS                      Status;
  UINTN                           BufferSize;
  UINT64                          *StartingAddr;
  EFI_INPUT_KEY                   Key;
  EFI_DEVICE_PATH_PROTOCOL        *DevicePath;
  RAM_DISK_PRIVATE_DATA           *PrivateData;
  EFI_FILE_INFO                   *FileInformation;

  FileInformation = NULL;
  StartingAddr    = NULL;

  if (FileHandle != NULL) {
    //
    // Create from file.
    //
    FileInformation = FileInfo (FileHandle);
    if (NULL == FileInformation) {
      do {
        CreatePopUp (
          EFI_LIGHTGRAY | EFI_BACKGROUND_BLUE,
          &Key,
          L"",
          L"Not enough memory to get the file information!",
          L"Press ENTER to continue ...",
          L"",
          NULL
          );
      } while (Key.UnicodeChar != CHAR_CARRIAGE_RETURN);

      return EFI_OUT_OF_RESOURCES;
    }

    //
    // Update the size of RAM disk according to the file size.
    //
    Size = FileInformation->FileSize;
  }

  if (Size > (UINTN) -1) {
    do {
      CreatePopUp (
        EFI_LIGHTGRAY | EFI_BACKGROUND_BLUE,
        &Key,
        L"",
        L"The given RAM disk size is too large!",
        L"Press ENTER to continue ...",
        L"",
        NULL
        );
    } while (Key.UnicodeChar != CHAR_CARRIAGE_RETURN);

    return EFI_OUT_OF_RESOURCES;
  }

  if (MemoryType == RAM_DISK_BOOT_SERVICE_DATA_MEMORY) {
    Status = gBS->AllocatePool (
                    EfiBootServicesData,
                    (UINTN)Size,
                    (VOID**)&StartingAddr
                    );
  } else if (MemoryType == RAM_DISK_RESERVED_MEMORY) {
    Status = gBS->AllocatePool (
                    EfiReservedMemoryType,
                    (UINTN)Size,
                    (VOID**)&StartingAddr
                    );
  } else {
    Status = EFI_INVALID_PARAMETER;
  }

  if ((StartingAddr == NULL) || EFI_ERROR(Status)) {
    do {
      CreatePopUp (
        EFI_LIGHTGRAY | EFI_BACKGROUND_BLUE,
        &Key,
        L"",
        L"Not enough memory to create the RAM disk!",
        L"Press ENTER to continue ...",
        L"",
        NULL
        );
    } while (Key.UnicodeChar != CHAR_CARRIAGE_RETURN);

    return EFI_OUT_OF_RESOURCES;
  }

  if (FileHandle != NULL) {
    //
    // Copy the file content to the RAM disk.
    //
    BufferSize = (UINTN) Size;
    FileHandle->Read (
                  FileHandle,
                  &BufferSize,
                  (VOID *)(UINTN) StartingAddr
                  );
    if (BufferSize != FileInformation->FileSize) {
      do {
        CreatePopUp (
          EFI_LIGHTGRAY | EFI_BACKGROUND_BLUE,
          &Key,
          L"",
          L"File content read error!",
          L"Press ENTER to continue ...",
          L"",
          NULL
          );
      } while (Key.UnicodeChar != CHAR_CARRIAGE_RETURN);

      return EFI_DEVICE_ERROR;
    }
  }

  //
  // Register the newly created RAM disk.
  //
  Status = mRamDiskProtocol->Register(
             ((UINT64)(UINTN) StartingAddr),
             Size,
             &gEfiVirtualDiskGuid,
             NULL,
             &DevicePath
             );
  if (EFI_ERROR (Status)) {
    do {
      CreatePopUp (
        EFI_LIGHTGRAY | EFI_BACKGROUND_BLUE,
        &Key,
        L"",
        L"Fail to register the newly created RAM disk!",
        L"Press ENTER to continue ...",
        L"",
        NULL
        );
    } while (Key.UnicodeChar != CHAR_CARRIAGE_RETURN);

    return Status;
  }

  //
  // Update link list of RAM Disk of H2O RAM Disk protocol for RamDiskApp
  //
  Status = UpdateRamDiskDeviceList((UINT64)(UINTN)StartingAddr, Size);

  //
  // If RAM disk is created within HII, memory should be freed when the
  // RAM disk is unregisterd.
  //
  PrivateData = RAM_DISK_PRIVATE_FROM_THIS (mRegisteredRamDisks.BackLink);
  PrivateData->CreateMethod = RamDiskCreateHii;

  return EFI_SUCCESS;
}


/**
  This function updates the registered RAM disks list on the main form.

  @param[in, out] ConfigPrivate
                             Private data for configurating hii data for RAM
                             disks.

**/
VOID
UpdateMainForm (
  IN OUT RAM_DISK_CONFIG_PRIVATE_DATA       *ConfigPrivate
  )
{
  VOID                      *StartOpCodeHandle;
  VOID                      *EndOpCodeHandle;
  EFI_IFR_GUID_LABEL        *StartLabel;
  EFI_IFR_GUID_LABEL        *EndLabel;
  LIST_ENTRY                *Entry;
  UINTN                     Index;
  RAM_DISK_PRIVATE_DATA     *PrivateData;
  CHAR16                    *String;
  CHAR16                    RamDiskStr[128];
  EFI_STRING_ID             StringId;

  //
  // Init OpCode Handle
  //
  StartOpCodeHandle = HiiAllocateOpCodeHandle ();
  ASSERT (StartOpCodeHandle != NULL);

  EndOpCodeHandle = HiiAllocateOpCodeHandle ();
  ASSERT (EndOpCodeHandle != NULL);

  //
  // Create Hii Extend Label OpCode as the start opcode
  //
  StartLabel = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode (
                                        StartOpCodeHandle,
                                        &gEfiIfrTianoGuid,
                                        NULL,
                                        sizeof (EFI_IFR_GUID_LABEL)
                                        );
  StartLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  StartLabel->Number       = MAIN_LABEL_LIST_START;

  //
  // Create Hii Extend Label OpCode as the end opcode
  //
  EndLabel = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode (
                                      EndOpCodeHandle,
                                      &gEfiIfrTianoGuid,
                                      NULL,
                                      sizeof (EFI_IFR_GUID_LABEL)
                                      );
  EndLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  EndLabel->Number       = MAIN_LABEL_LIST_END;

  Index = 0;
  EFI_LIST_FOR_EACH (Entry, &mRegisteredRamDisks) {
    PrivateData                  = RAM_DISK_PRIVATE_FROM_THIS (Entry);
    PrivateData->CheckBoxId      = (EFI_QUESTION_ID)
                                   (MAIN_CHECKBOX_QUESTION_ID_START + Index);
    //
    // CheckBox is unchecked by default.
    //
    PrivateData->CheckBoxChecked = FALSE;
    String                       = RamDiskStr;

    UnicodeSPrint (
      String,
      sizeof (RamDiskStr),
      L"  RAM Disk %d  Size: 0x%lx\n",
      Index,
      PrivateData->Size
      );

    StringId = HiiSetString (ConfigPrivate->HiiHandle, 0, RamDiskStr, NULL);
    ASSERT (StringId != 0);

    HiiCreateCheckBoxOpCode (
      StartOpCodeHandle,
      PrivateData->CheckBoxId,
      0,
      0,
      StringId,
      STRING_TOKEN (STR_RAM_DISK_LIST_HELP),
      EFI_IFR_FLAG_CALLBACK,
      0,
      NULL
      );

    Index++;
  }

  HiiUpdateForm (
    ConfigPrivate->HiiHandle,
    &gH2ORamDiskFormSetGuid,
    MAIN_FORM_ID,
    StartOpCodeHandle,
    EndOpCodeHandle
    );

  HiiFreeOpCodeHandle (StartOpCodeHandle);
  HiiFreeOpCodeHandle (EndOpCodeHandle);
}


/**
  This function processes the results of changes in configuration.

  @param[in]  This           Points to the EFI_HII_CONFIG_ACCESS_PROTOCOL.
  @param[in]  Action         Specifies the type of action taken by the browser.
  @param[in]  QuestionId     A unique value which is sent to the original
                             exporting driver so that it can identify the type
                             of data to expect.
  @param[in]  Type           The type of value for the question.
  @param[in]  Value          A pointer to the data being sent to the original
                             exporting driver.
  @param[out] ActionRequest  On return, points to the action requested by the
                             callback function.

  @retval EFI_SUCCESS             The callback successfully handled the action.
  @retval EFI_OUT_OF_RESOURCES    Not enough storage is available to hold the
                                  variable and its data.
  @retval EFI_DEVICE_ERROR        The variable could not be saved.
  @retval EFI_UNSUPPORTED         The specified Action is not supported by the
                                  callback.

**/
EFI_STATUS
EFIAPI
RamDiskCallback (
  IN CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN     EFI_BROWSER_ACTION                 Action,
  IN     EFI_QUESTION_ID                    QuestionId,
  IN     UINT8                              Type,
  IN     EFI_IFR_TYPE_VALUE                 *Value,
     OUT EFI_BROWSER_ACTION_REQUEST         *ActionRequest
  )
{
  EFI_STATUS                      Status;
  RAM_DISK_PRIVATE_DATA           *PrivateData;
  RAM_DISK_CONFIG_PRIVATE_DATA    *ConfigPrivate;
  LIST_ENTRY                      *Entry;
  LIST_ENTRY                      *NextEntry;

  if ((This == NULL) || (Value == NULL) || (ActionRequest == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  ConfigPrivate = RAM_DISK_CONFIG_PRIVATE_FROM_THIS (This);

  if (Action == EFI_BROWSER_ACTION_RETRIEVE) {
    Status = EFI_UNSUPPORTED;
    if (QuestionId == CREATE_RAW_SIZE_QUESTION_ID) {
      Value->u64 = EFI_PAGE_SIZE;
      ConfigPrivate->ConfigStore.Size = EFI_PAGE_SIZE;
      Status = EFI_SUCCESS;
    } else if (QuestionId == CREATE_RAW_MEMORY_TYPE_QUESTION_ID) {
      Value->u8 = RAM_DISK_BOOT_SERVICE_DATA_MEMORY;
      ConfigPrivate->ConfigStore.MemType = RAM_DISK_BOOT_SERVICE_DATA_MEMORY;
      Status = EFI_SUCCESS;
    }
    return Status;
  }

  if ((Action != EFI_BROWSER_ACTION_CHANGED) &&
      (Action != EFI_BROWSER_ACTION_CHANGING) &&
      (Action != EFI_BROWSER_ACTION_FORM_OPEN)) {
    return EFI_UNSUPPORTED;
  }

  //
  // Update the RAM disk list show at the main form first.
  //
  if (Action == EFI_BROWSER_ACTION_FORM_OPEN) {
    Status = EFI_UNSUPPORTED;
    if (QuestionId == KEY_FILE_EXPLORER) {
      UpdateMainForm (ConfigPrivate);
      Status = EFI_SUCCESS;
    }
    return Status;
  }

  Status = EFI_SUCCESS;

  if (Action == EFI_BROWSER_ACTION_CHANGING) {
    switch (QuestionId) {
    case KEY_FILE_EXPLORER:
      Status = InitializeCreateFromFileMenu (ConfigPrivate);
      if (EFI_ERROR (Status)) {
        break;
      }
     break;

    default:
      if (QuestionId >= FILE_EXPLORER_RETRIEVE_ID) {
        if (UpdateRdFileExplorer (ConfigPrivate, QuestionId)) {
          ConfigPrivate->DisplayContext  = EM_UNKNOWN_CONTEXT;
          UpdateRdFileExplorer (ConfigPrivate, 0);
        }
      }
      break;
    }
  } else if (Action == EFI_BROWSER_ACTION_CHANGED) {
    switch (QuestionId) {
    case MAIN_REMOVE_RD_QUESTION_ID:
      //
      // Remove the selected RAM disks
      //
      EFI_LIST_FOR_EACH_SAFE (Entry, NextEntry, &mRegisteredRamDisks) {
        PrivateData = RAM_DISK_PRIVATE_FROM_THIS (Entry);
        if (PrivateData->CheckBoxChecked) {
          mRamDiskProtocol->Unregister(
            (EFI_DEVICE_PATH_PROTOCOL *) PrivateData->DevicePath
            );
        }
      }

      UpdateMainForm (ConfigPrivate);

      *ActionRequest = EFI_BROWSER_ACTION_REQUEST_FORM_APPLY;
      break;

    case CREATE_RAW_SIZE_QUESTION_ID:
      ConfigPrivate->ConfigStore.Size = Value->u64;
      break;

    case CREATE_RAW_MEMORY_TYPE_QUESTION_ID:
      ConfigPrivate->ConfigStore.MemType = Value->u8;
      break;

    case CREATE_RAW_SUBMIT_QUESTION_ID:
      //
      // Create raw, FileHandle is NULL.
      //
      Status = HiiCreateRamDisk (
                 ConfigPrivate->ConfigStore.Size,
                 NULL,
                 ConfigPrivate->ConfigStore.MemType
                 );
      if (EFI_ERROR (Status)) {
        break;
      }

      //
      // Refresh the registered RAM disks list.
      //
      UpdateMainForm (ConfigPrivate);

      *ActionRequest = EFI_BROWSER_ACTION_REQUEST_FORM_DISCARD_EXIT;
      break;

    case CREATE_RAW_DISCARD_QUESTION_ID:
      *ActionRequest = EFI_BROWSER_ACTION_REQUEST_FORM_DISCARD_EXIT;
      break;

    default:
      //
      // QuestionIds for checkboxes
      //
      if ((QuestionId >= MAIN_CHECKBOX_QUESTION_ID_START) &&
          (QuestionId < CREATE_RAW_RAM_DISK_FORM_ID)) {
        EFI_LIST_FOR_EACH (Entry, &mRegisteredRamDisks) {
          PrivateData = RAM_DISK_PRIVATE_FROM_THIS (Entry);
          if (PrivateData->CheckBoxId == QuestionId) {
            PrivateData->CheckBoxChecked = (BOOLEAN) (Value->u8 != 0);
          }
        }
      }
      break;
    }
  }

  return EFI_SUCCESS;
}
