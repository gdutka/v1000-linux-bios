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

#include "FileExplorer.h"
#include "RamDiskImpl.h"
#include "RamDiskNVData.h"


#define TWO_BYTE_ENCODE                   0x82
#define SHORT_FORM_MASK                   0x80

LIST_ENTRY                           mRamDiskLinkList = INITIALIZE_LIST_HEAD_VARIABLE (mRamDiskLinkList);
STRING_DEPOSITORY                    *mRamDiskFileOptionStrDepository;

extern H2O_DIALOG_PROTOCOL           *mH2ODialog;
STATIC EFI_TEXT_STRING               mOriginalOutputString;

EFI_GUID mRamDiskGuid = H2O_RAM_DISK_FORMSET_GUID;

ENROLL_MENU_OPTION         mRamDiskFsOptionMenu = {
  EM_MENU_ENTRY_SIGNATURE,
  NULL,
  0
};

ENROLL_MENU_OPTION         mRamDiskDirectoryMenu =  {
  EM_MENU_ENTRY_SIGNATURE,
  NULL,
  0
};

typedef struct {
  UINTN               FileFormat;
  EFI_STRING_ID       StringId;
  EFI_GUID            *SignatureTypeGuid;
} EM_FILE_FORMAT_INFO;


STATIC
ENROLL_MENU_ENTRY *
RdCreateFileMenuEntry (
  VOID
  );

STATIC
VOID
RdDestroyMenuEntry (
  ENROLL_MENU_ENTRY         *MenuEntry
  )
;


EFI_DEVICE_PATH_PROTOCOL  EndDevicePath[] = {
  {
    END_DEVICE_PATH_TYPE,
    END_ENTIRE_DEVICE_PATH_SUBTYPE,
    {
      END_DEVICE_PATH_LENGTH,
      0
    }
  }
};



/**
  Reclaim string depository by moving the current node pointer to list head.

  @param[in] StringDepository   Pointer to the string repository
**/
VOID
RamDiskReclaimStrDepository (
  IN STRING_DEPOSITORY                 *StrDepository
  )
{
  if (StrDepository != NULL) {
    StrDepository->CurrentNode = StrDepository->ListHead;
  }
}

/**
  Fetch a usable string node from the string depository and return the string token.

  @param[in] CallbackData             Pointer to RAM_DISK_CONFIG_PRIVATE_DATA instance
  @param[in] StringDepository         Pointer of the string depository

  @return String token or zero if input parameter is NULL or fail to allocate memory.
**/
EFI_STRING_ID
RamDiskGetStrTokenFromDepository (
  IN RAM_DISK_CONFIG_PRIVATE_DATA *CallbackData,
  IN STRING_DEPOSITORY                 *StringDepository
  )
{
  STRING_LIST_NODE                     *CurrentListNode;
  STRING_LIST_NODE                     *NextListNode;

  if (CallbackData == NULL || StringDepository == NULL) {
    return 0;
  }

  CurrentListNode = StringDepository->CurrentNode;

  if ((NULL != CurrentListNode) && (NULL != CurrentListNode->Next)) {
    //
    // Fetch one reclaimed node from the list.
    //
    NextListNode = StringDepository->CurrentNode->Next;
  } else {
    //
    // If there is no usable node in the list, update the list.
    //
    NextListNode = AllocateZeroPool (sizeof (STRING_LIST_NODE));
    if (NextListNode == NULL) {
      return 0;
    }
    NextListNode->StringToken = HiiSetString (
                                  CallbackData->HiiHandle,
                                  NextListNode->StringToken,
                                  L" ",
                                  NULL
                                  );
    ASSERT (NextListNode->StringToken != 0);

    StringDepository->TotalNodeNumber++;

    if (NULL == CurrentListNode) {
      StringDepository->ListHead = NextListNode;
    } else {
      CurrentListNode->Next = NextListNode;
    }
  }

  StringDepository->CurrentNode = NextListNode;

  return StringDepository->CurrentNode->StringToken;
}



/**
  Wrap original FreePool gBS call in order to decrease code length.
**/
VOID
RamDiskSafeFreePool (
  IN VOID                              **Buffer
  )
{
  if (Buffer != NULL && *Buffer != NULL) {
    gBS->FreePool (*Buffer);
    *Buffer = NULL;
  }
}

/**
  Destroy the menu entry passed in

  @param  MenuEntry         The menu entry need to be destroyed

**/
STATIC
VOID
RdDestroyMenuEntry (
  ENROLL_MENU_ENTRY         *MenuEntry
  )
{
  EM_FILE_CONTEXT           *FileContext;

  FileContext = (EM_FILE_CONTEXT *) MenuEntry->VariableContext;

  if (!FileContext->IsRoot) {
    RamDiskSafeFreePool ((VOID **) &FileContext->DevicePath);
  } else {
    if (FileContext->FHandle != NULL) {
      FileContext->FHandle->Close (FileContext->FHandle);
    }
  }

  RamDiskSafeFreePool ((VOID **) &FileContext->FileName);
  RamDiskSafeFreePool ((VOID **) &FileContext->Info);
  RamDiskSafeFreePool ((VOID **) &FileContext);
  RamDiskSafeFreePool ((VOID **) &MenuEntry->DisplayString);
  RamDiskSafeFreePool ((VOID **) &MenuEntry->HelpString);
  RamDiskSafeFreePool ((VOID **) &MenuEntry);
}



/**
  Dummy function uses to hook original OutputString ().

  @param[in] This       The protocol instance pointer.
  @param[in] String     The NULL-terminated string to be displayed on the output
                        device(s). All output devices must also support the Unicode
                        drawing character codes defined in this file.

  @retval EFI_SUCCESS   Always return EFI_SUCCESS.
**/
EFI_STATUS
EFIAPI
DummyOutputString (
  IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL        *This,
  IN CHAR16                                 *String
  )
{
  return EFI_SUCCESS;
}

/**
  Fuction to hook original Outputstring function to disable all of
  output strings.

  @retval EFI_SUCCESS   Always return EFI_SUCCESS.
**/
STATIC
EFI_STATUS
HookOutputString (
  VOID
  )
{
  mOriginalOutputString = gST->ConOut->OutputString;
  gST->ConOut->OutputString = DummyOutputString;

  return EFI_SUCCESS;
}

/**
  Fuction to restore original Outputstring function to ConOut in
  system table.

  @retval EFI_SUCCESS   Always return EFI_SUCCESS.
**/
STATIC
EFI_STATUS
RestoreOutputString (
  VOID
  )
{
  gST->ConOut->OutputString = mOriginalOutputString;

  return EFI_SUCCESS;
}


/**
  Append file name to existing file name.

  @param  Str1         existing file name
  @param  Str2         file name to be appended

  @return Allocate a new string to hold the appended result.
          Caller is responsible to free the returned string.

**/
CHAR16 *
RdAppendFileName (
  IN  CHAR16  *Str1,
  IN  CHAR16  *Str2
  )
{
  UINTN   Size1;
  UINTN   Size2;
  CHAR16  *Str;
  CHAR16  *TmpStr;
  CHAR16  *Ptr;
  CHAR16  *LastSlash;

  Size1 = StrSize (Str1);
  Size2 = StrSize (Str2);
  //
  // Integer Overflow check
  //
  if (Size1 > Size1 + Size2 + sizeof (CHAR16) || Size2 > Size1 + Size2 + sizeof (CHAR16)) {
    ASSERT (FALSE);
    return NULL;
  }
  Str   = AllocateZeroPool (Size1 + Size2 + sizeof (CHAR16));
  ASSERT (Str != NULL);
  if (Str == NULL) {
    return NULL;
  }
  TmpStr = AllocateZeroPool (Size1 + Size2 + sizeof (CHAR16));
  ASSERT (TmpStr != NULL);
  if (TmpStr == NULL) {
    return NULL;
  }

  StrCatS (Str, (Size1 + Size2 + sizeof (CHAR16)) / sizeof(CHAR16), Str1);
  if (!((*Str == '\\') && (*(Str + 1) == 0))) {
    StrCatS (Str, (Size1 + Size2 + sizeof (CHAR16)) / sizeof(CHAR16), L"\\");
  }

  StrCatS (Str, (Size1 + Size2 + sizeof (CHAR16)) / sizeof(CHAR16), Str2);

  Ptr       = Str;
  LastSlash = Str;
  while (*Ptr != 0) {
    if (*Ptr == '\\' && *(Ptr + 1) == '.' && *(Ptr + 2) == '.' && *(Ptr + 3) == L'\\') {
      //
      // Convert "\Name\..\" to "\"
      // DO NOT convert the .. if it is at the end of the string. This will
      // break the .. behavior in changing directories.
      //

      //
      // Use TmpStr as a backup, as StrCpy in BaseLib does not handle copy of two strings
      // that overlap.
      //
      StrCpyS (TmpStr, (Size1 + Size2 + sizeof (CHAR16)) / sizeof(CHAR16), Ptr + 3);
      StrCpyS (LastSlash,  (Size1 + Size2 + sizeof (CHAR16) - ((UINT8 *) LastSlash - (UINT8 *) Str)) / sizeof(CHAR16), TmpStr);
      Ptr = LastSlash;
    } else if (*Ptr == '\\' && *(Ptr + 1) == '.' && *(Ptr + 2) == '\\') {
      //
      // Convert a "\.\" to a "\"
      //

      //
      // Use TmpStr as a backup, as StrCpy in BaseLib does not handle copy of two strings
      // that overlap.
      //
      StrCpyS (TmpStr,  (Size1 + Size2 + sizeof (CHAR16))/sizeof(CHAR16), Ptr + 2);
      StrCpyS (Ptr,  (Size1 + Size2 + sizeof (CHAR16)) / sizeof(CHAR16), TmpStr);
      Ptr = LastSlash;
    } else if (*Ptr == '\\') {
      LastSlash = Ptr;
    }

    Ptr++;
  }

  FreePool (TmpStr);

  return Str;
}


/**
  Create Menu Entry for future use, make all types together
  in order to reduce code size

  @param  MenuType      Use this parameter to identify current
                        Menu type

  @return A valid pointer pointing to the allocated memory pool for current menu entry
          or NULL if it cannot allocate memory for current menu entry

**/
STATIC
ENROLL_MENU_ENTRY *
RdCreateFileMenuEntry (
  VOID
  )
{
  ENROLL_MENU_ENTRY *MenuEntry;
  UINTN             ContextSize;


  ContextSize = sizeof (BM_FILE_CONTEXT);

  MenuEntry = AllocateZeroPool (sizeof (ENROLL_MENU_ENTRY));
  if (NULL == MenuEntry) {
    return MenuEntry;
  }

  MenuEntry->VariableContext = AllocateZeroPool (ContextSize);
  if (NULL == MenuEntry->VariableContext) {
    RamDiskSafeFreePool ((VOID**) &MenuEntry);
    MenuEntry = NULL;
    return MenuEntry;
  }

  MenuEntry->Signature        = EM_MENU_ENTRY_SIGNATURE;
  MenuEntry->ContextSelection = EM_FILE_CONTEXT_SELECT;
  return MenuEntry;
}


/**
  Find files under current directory
  All files and sub-directories in current directory
  will be stored in DirectoryMenu for future use.

  @param  FileOption    Pointer for Dir to explore

  @retval TRUE          Get files from current dir successfully
  @retval FALSE         Can't get files from current dir

**/
EFI_STATUS
RdFindFiles (
  IN RAM_DISK_CONFIG_PRIVATE_DATA      *CallbackData,
  IN ENROLL_MENU_ENTRY                      *MenuEntry
  )
{
  EFI_FILE_HANDLE     NewDir;
  EFI_FILE_HANDLE     Dir;
  EFI_FILE_INFO       *DirInfo;
  UINTN               BufferSize;
  UINTN               DirBufferSize;
  ENROLL_MENU_ENTRY   *NewMenuEntry;
  EM_FILE_CONTEXT     *FileContext;
  EM_FILE_CONTEXT     *NewFileContext;
  UINTN               Pass;
  EFI_STATUS          Status;
  UINTN               OptionNumber;

  FileContext   = (EM_FILE_CONTEXT *) MenuEntry->VariableContext;
  Dir           = FileContext->FHandle;
  OptionNumber  = 0;
  //
  // Open current directory to get files from it
  //
  Status = Dir->Open (
                  Dir,
                  &NewDir,
                  FileContext->FileName,
                  EFI_FILE_READ_ONLY,
                  0
                  );
  if (!FileContext->IsRoot) {
    Dir->Close (Dir);
  }

  if (EFI_ERROR (Status)) {
    return Status;
  }

  DirInfo = UtilityLibFileInfo (NewDir);
  if (!DirInfo) {
    return EFI_NOT_FOUND;
  }

  if (!(DirInfo->Attribute & EFI_FILE_DIRECTORY)) {
    return EFI_INVALID_PARAMETER;
  }

  FileContext->DevicePath = FileDevicePath (
                              FileContext->Handle,
                              FileContext->FileName
                              );

  DirBufferSize = sizeof (EFI_FILE_INFO) + 1024;
  DirInfo       = AllocateZeroPool (DirBufferSize);
  if (!DirInfo) {
    return EFI_OUT_OF_RESOURCES;
  }
  //
  // Get all files in current directory
  // Pass 1 to get Directories
  // Pass 2 to get files that are EFI images
  //
  for (Pass = 1; Pass <= 2; Pass++) {
    NewDir->SetPosition (NewDir, 0);
    for (;;) {
      BufferSize  = DirBufferSize;
      Status      = NewDir->Read (NewDir, &BufferSize, DirInfo);
      if (EFI_ERROR (Status) || BufferSize == 0) {
        break;
      }

      if ((DirInfo->Attribute & EFI_FILE_DIRECTORY && Pass == 2) ||
          (!(DirInfo->Attribute & EFI_FILE_DIRECTORY) && Pass == 1)
          ) {
        //
        // Pass 1 is for Directories
        // Pass 2 is for file names
        //
        continue;
      }

      NewMenuEntry = RdCreateFileMenuEntry ();
      if (NULL == NewMenuEntry) {
        return EFI_OUT_OF_RESOURCES;
      }

      NewFileContext          = (EM_FILE_CONTEXT *) NewMenuEntry->VariableContext;
      NewFileContext->Handle  = FileContext->Handle;
      NewFileContext->FileName = RdAppendFileName (
                                  FileContext->FileName,
                                  DirInfo->FileName
                                  );
      if (NewFileContext->FileName == NULL) {
        return EFI_OUT_OF_RESOURCES;
      }
      NewFileContext->FHandle = NewDir;
      NewFileContext->DevicePath = FileDevicePath (
                                    NewFileContext->Handle,
                                    NewFileContext->FileName
                                    );
      NewMenuEntry->HelpString = NULL;

      MenuEntry->DisplayStringToken = RamDiskGetStrTokenFromDepository (
                                        CallbackData,
                                        mRamDiskFileOptionStrDepository
                                        );

      NewFileContext->IsDir = (BOOLEAN) ((DirInfo->Attribute & EFI_FILE_DIRECTORY) == EFI_FILE_DIRECTORY);

      if (NewFileContext->IsDir) {
        BufferSize                  = StrLen (DirInfo->FileName) * 2 + 6;
        NewMenuEntry->DisplayString = AllocateZeroPool (BufferSize);

        UnicodeSPrint (
          NewMenuEntry->DisplayString,
          BufferSize,
          L"<%s>",
          DirInfo->FileName
          );

      } else {
        NewMenuEntry->DisplayString = AllocateCopyPool (StrSize (DirInfo->FileName), DirInfo->FileName);
      }

      NewFileContext->IsRoot            = FALSE;
      NewFileContext->IsLoadFile        = FALSE;
      NewFileContext->IsRemovableMedia  = FALSE;

      NewMenuEntry->OptionNumber        = OptionNumber;
      OptionNumber++;
      InsertTailList (&mRamDiskDirectoryMenu.Head, &NewMenuEntry->Link);
    }
  }

  mRamDiskDirectoryMenu.MenuNumber = OptionNumber;
  RamDiskSafeFreePool ((VOID**) &DirInfo);
  return EFI_SUCCESS;
}


/**
  Use this routine to get one particular menu entry in specified menu

  @param  MenuOption         The menu that we will search
  @param  MenuNumber         The menunubmer that we want

  @return The desired menu entry

**/
ENROLL_MENU_ENTRY *
RdGetMenuEntry (
  ENROLL_MENU_OPTION     *MenuOption,
  UINTN                  MenuNumber
  )
{
  ENROLL_MENU_ENTRY   *NewMenuEntry;
  UINTN               Index;
  LIST_ENTRY          *List;

  if (MenuNumber >= MenuOption->MenuNumber) {
    return NULL;
  }

  List = MenuOption->Head.ForwardLink;
  for (Index = 0; Index < MenuNumber; Index++) {
    List = List->ForwardLink;
  }

  NewMenuEntry = CR (List, ENROLL_MENU_ENTRY, Link, EM_MENU_ENTRY_SIGNATURE);

  return NewMenuEntry;
}


/**
  Update the CreateFromFile Menu page.

  @param  CallbackData         Pointer to RAM_DISK_CONFIG_PRIVATE_DATA instance
  @param  MenuOption           Pointer to menu options to display.

**/
VOID
UpdateCreateFromFileMenuPage (
  IN RAM_DISK_CONFIG_PRIVATE_DATA     *CallbackData,
  IN ENROLL_MENU_OPTION                    *MenuOption
  )
{
  UINTN                       Index;
  ENROLL_MENU_ENTRY           *NewMenuEntry;
  EM_FILE_CONTEXT             *NewFileContext;
  VOID                        *StartOpCodeHandle;
  VOID                        *EndOpCodeHandle;
  EFI_IFR_GUID_LABEL          *StartLabel;
  EFI_IFR_GUID_LABEL          *EndLabel;

  NewMenuEntry    = NULL;
  NewFileContext  = NULL;

  //
  // Allocate space for creation of UpdateData Buffer
  //
  StartOpCodeHandle = HiiAllocateOpCodeHandle ();
  ASSERT (StartOpCodeHandle != NULL);

  EndOpCodeHandle = HiiAllocateOpCodeHandle ();
  ASSERT (EndOpCodeHandle != NULL);
  //
  // Create Hii Extend Label OpCode as the start opcode
  //
  StartLabel = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode (StartOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
  StartLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  StartLabel->Number       = FORM_FILE_LIST_ID;

  //
  // Create Hii Extend Label OpCode as the end opcode
  //
  EndLabel = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode (EndOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
  EndLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  EndLabel->Number       = FORM_FILE_LIST_END_ID;

  for (Index = 0; Index < MenuOption->MenuNumber; Index++) {
    NewMenuEntry    = RdGetMenuEntry (MenuOption, Index);
    NewFileContext  = (EM_FILE_CONTEXT *) NewMenuEntry->VariableContext;

    HiiCreateActionOpCode (
      StartOpCodeHandle,
      (UINT16) (FILE_EXPLORER_RETRIEVE_ID + Index),
      NewMenuEntry->DisplayStringToken,
      STRING_TOKEN (STR_RAM_DISK_NULL_STRING),
      EFI_IFR_FLAG_CALLBACK,
      0
      );

  }
  HiiUpdateForm (
    CallbackData->HiiHandle,
    &mRamDiskGuid,
    MAIN_GOTO_FILE_EXPLORER_ID,
    StartOpCodeHandle,
    EndOpCodeHandle
    );
  HiiFreeOpCodeHandle (StartOpCodeHandle);
  HiiFreeOpCodeHandle (EndOpCodeHandle);

}


/**
  Create string tokens for a menu from its help strings and display strings

  @param  HiiHandle         Hii Handle of the package to be updated.
  @param  MenuOption        The Menu whose string tokens need to be created

  @retval EFI_SUCCESS   string tokens created successfully
  @retval Other         contain some errors

**/
EFI_STATUS
RdCreateMenuStringToken (
  IN EFI_HII_HANDLE                   HiiHandle,
  IN ENROLL_MENU_OPTION               *MenuOption
  )
{
  ENROLL_MENU_ENTRY *NewMenuEntry;
  UINTN             Index;

  for (Index = 0; Index < MenuOption->MenuNumber; Index++) {
    NewMenuEntry = RdGetMenuEntry (MenuOption, Index);
    NewMenuEntry->DisplayStringToken = HiiSetString (
                                         HiiHandle,
                                         NewMenuEntry->DisplayStringToken,
                                         NewMenuEntry->DisplayString,
                                         NULL
                                         );
    if (NULL == NewMenuEntry->HelpString) {
      NewMenuEntry->HelpStringToken = NewMenuEntry->DisplayStringToken;
    } else {
      NewMenuEntry->HelpStringToken = HiiSetString (
                                        HiiHandle,
                                        NewMenuEntry->HelpStringToken,
                                        NewMenuEntry->HelpString,
                                        NULL
                                        );
    }
  }

  return EFI_SUCCESS;
}

/**
  Free resources allocated in Allocate Rountine

  @param  FreeMenu      Menu to be freed

**/
VOID
RdFreeMenu (
  ENROLL_MENU_OPTION       *FreeMenu
  )
{
  ENROLL_MENU_ENTRY    *MenuEntry;

  while (!IsListEmpty (&FreeMenu->Head)) {
    MenuEntry = CR (
                  FreeMenu->Head.ForwardLink,
                  ENROLL_MENU_ENTRY,
                  Link,
                  EM_MENU_ENTRY_SIGNATURE
                  );
    RemoveEntryList (&MenuEntry->Link);
    RdDestroyMenuEntry (MenuEntry);
  }
}


/**
  Find file systems for current Extensible Firmware
  Including Handles that support Simple File System
  protocol, Load File protocol.

  Building up the FileSystem Menu for user selection
  All file system will be stored in FsOptionMenu
  for future use.

  @param  CallbackData  BMM context data

  @retval EFI_SUCCESS               Success find the file system
  @retval EFI_OUT_OF_RESOURCES      Can not create menu entry

**/
EFI_STATUS
RdFindFileSystem (
  VOID
  )
{
  UINTN                     NoBlkIoHandles;
  UINTN                     NoSimpleFsHandles;
  UINTN                     NoLoadFileHandles;
  EFI_HANDLE                *BlkIoHandle;
  EFI_HANDLE                *SimpleFsHandle;
  EFI_HANDLE                *LoadFileHandle;
  UINT16                    *VolumeLabel;
  EFI_BLOCK_IO_PROTOCOL     *BlkIo;
  UINTN                     Index;
  EFI_STATUS                Status;
  ENROLL_MENU_ENTRY          *MenuEntry;
  EM_FILE_CONTEXT           *FileContext;
  UINT16                    *TempStr;
  UINTN                     OptionNumber;
  VOID                      *Buffer;
  EFI_LEGACY_BIOS_PROTOCOL  *LegacyBios;
  UINT16                    DeviceType;
  BBS_BBS_DEVICE_PATH       BbsDevicePathNode;
  EFI_DEVICE_PATH_PROTOCOL  *DevicePath;
  BOOLEAN                   RemovableMedia;



  NoSimpleFsHandles = 0;
  NoLoadFileHandles = 0;
  OptionNumber      = 0;
  InitializeListHead (&mRamDiskFsOptionMenu.Head);

  //
  // Locate Handles that support BlockIo protocol
  //
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiBlockIoProtocolGuid,
                  NULL,
                  &NoBlkIoHandles,
                  &BlkIoHandle
                  );
  if (!EFI_ERROR (Status)) {

    for (Index = 0; Index < NoBlkIoHandles; Index++) {
      Status = gBS->HandleProtocol (
                      BlkIoHandle[Index],
                      &gEfiBlockIoProtocolGuid,
                      (VOID **) &BlkIo
                      );

      if (EFI_ERROR (Status)) {
        continue;
      }

      //
      // Issue a dummy read to trigger reinstall of BlockIo protocol for removable media
      //
      if (BlkIo->Media->RemovableMedia) {
        Buffer = AllocateZeroPool (BlkIo->Media->BlockSize);
        if (NULL == Buffer) {
          RamDiskSafeFreePool ((VOID**) &BlkIoHandle);
          return EFI_OUT_OF_RESOURCES;
        }

        BlkIo->ReadBlocks (
                 BlkIo,
                 BlkIo->Media->MediaId,
                 0,
                 BlkIo->Media->BlockSize,
                 Buffer
                 );
        RamDiskSafeFreePool ((VOID**) &Buffer);
      }
    }
    RamDiskSafeFreePool ((VOID**) &BlkIoHandle);
  }

  //
  // Locate Handles that support Simple File System protocol
  //
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiSimpleFileSystemProtocolGuid,
                  NULL,
                  &NoSimpleFsHandles,
                  &SimpleFsHandle
                  );
  if (!EFI_ERROR (Status)) {
    //
    // Find all the instances of the File System prototocol
    //
    for (Index = 0; Index < NoSimpleFsHandles; Index++) {
      Status = gBS->HandleProtocol (
                      SimpleFsHandle[Index],
                      &gEfiBlockIoProtocolGuid,
                      (VOID **) &BlkIo
                      );
      if (EFI_ERROR (Status)) {
        //
        // If no block IO exists assume it's NOT a removable media
        //
        RemovableMedia = FALSE;
      } else {
        //
        // If block IO exists check to see if it's remobable media
        //
        RemovableMedia = BlkIo->Media->RemovableMedia;
      }

      //
      // Allocate pool for this load option
      //
      MenuEntry = RdCreateFileMenuEntry ();
      if (NULL == MenuEntry) {
        RamDiskSafeFreePool ((VOID**) &SimpleFsHandle);
        return EFI_OUT_OF_RESOURCES;
      }

      FileContext = (EM_FILE_CONTEXT *) MenuEntry->VariableContext;

      FileContext->Handle     = SimpleFsHandle[Index];
      MenuEntry->OptionNumber = Index;
      FileContext->FHandle    = UtilityLibOpenRoot (FileContext->Handle);
      if (!FileContext->FHandle) {
        RdDestroyMenuEntry (MenuEntry);
        continue;
      }

      MenuEntry->HelpString = DevicePathToStr (DevicePathFromHandle (FileContext->Handle));
      FileContext->Info = UtilityLibFileSystemVolumeLabelInfo (FileContext->FHandle);
      FileContext->FileName = UtilityLibStrDuplicate (L"\\");
      if (FileContext->FileName == NULL) {
        RdDestroyMenuEntry (MenuEntry);
        RamDiskSafeFreePool ((VOID**) &SimpleFsHandle);
        return EFI_OUT_OF_RESOURCES;
      }
      FileContext->DevicePath = FileDevicePath (
                                  FileContext->Handle,
                                  FileContext->FileName
                                  );
      FileContext->IsDir            = TRUE;
      FileContext->IsRoot           = TRUE;
      FileContext->IsRemovableMedia = FALSE;
      FileContext->IsLoadFile       = FALSE;

      //
      // Get current file system's Volume Label
      //
      if (FileContext->Info == NULL) {
        VolumeLabel = L"NO FILE SYSTEM INFO";
      } else {
        VolumeLabel = FileContext->Info->VolumeLabel;
        if (*VolumeLabel == 0x0000) {
          VolumeLabel = L"NO VOLUME LABEL";
        }
      }

      TempStr                   = MenuEntry->HelpString;
      MenuEntry->DisplayString  = AllocateZeroPool (MAX_CHAR);
      ASSERT (MenuEntry->DisplayString != NULL);
      UnicodeSPrint (
        MenuEntry->DisplayString,
        MAX_CHAR,
        L"%s, [%s]",
        VolumeLabel,
        TempStr
        );
      OptionNumber++;
      InsertTailList (&mRamDiskFsOptionMenu.Head, &MenuEntry->Link);
    }
  }

  if (NoSimpleFsHandles != 0) {
    RamDiskSafeFreePool ((VOID**) &SimpleFsHandle);
  }
  //
  // Searching for handles that support Load File protocol
  //
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiLoadFileProtocolGuid,
                  NULL,
                  &NoLoadFileHandles,
                  &LoadFileHandle
                  );

  if (!EFI_ERROR (Status)) {
    for (Index = 0; Index < NoLoadFileHandles; Index++) {
      if (IsHwVendorDevicePath (LoadFileHandle[Index])) {
        continue;
      }
      MenuEntry = RdCreateFileMenuEntry ();
      if (NULL == MenuEntry) {
        RamDiskSafeFreePool ((VOID**) &LoadFileHandle);
        return EFI_OUT_OF_RESOURCES;
      }

      FileContext                   = (EM_FILE_CONTEXT *) MenuEntry->VariableContext;
      FileContext->IsRemovableMedia = FALSE;
      FileContext->IsLoadFile       = TRUE;
      FileContext->Handle           = LoadFileHandle[Index];
      FileContext->IsRoot           = TRUE;

      FileContext->DevicePath = DevicePathFromHandle (FileContext->Handle);

      MenuEntry->HelpString     = DevicePathToStr (FileContext->DevicePath);

      TempStr                   = MenuEntry->HelpString;
      MenuEntry->DisplayString  = AllocateZeroPool (MAX_CHAR);
      ASSERT (MenuEntry->DisplayString != NULL);
      UnicodeSPrint (
        MenuEntry->DisplayString,
        MAX_CHAR,
        L"Load File [%s]",
        TempStr
        );

      MenuEntry->OptionNumber = OptionNumber;
      OptionNumber++;
      InsertTailList (&mRamDiskFsOptionMenu.Head, &MenuEntry->Link);
    }
  }

  if (NoLoadFileHandles != 0) {
    RamDiskSafeFreePool ((VOID**) &LoadFileHandle);
  }

  //
  // Add Legacy Boot Option Support Here
  //
  Status = gBS->LocateProtocol (
                  &gEfiLegacyBiosProtocolGuid,
                  NULL,
                  (VOID **) &LegacyBios
                  );
  if (!EFI_ERROR (Status)) {

    for (Index = BBS_TYPE_FLOPPY; Index <= BBS_TYPE_EMBEDDED_NETWORK; Index++) {
      MenuEntry = RdCreateFileMenuEntry ();
      if (NULL == MenuEntry) {
        return EFI_OUT_OF_RESOURCES;
      }

      FileContext                       = (EM_FILE_CONTEXT *) MenuEntry->VariableContext;

      FileContext->IsRemovableMedia     = FALSE;
      FileContext->IsLoadFile           = TRUE;
      FileContext->IsBootLegacy         = TRUE;
      DeviceType                        = (UINT16) Index;
      BbsDevicePathNode.Header.Type     = BBS_DEVICE_PATH;
      BbsDevicePathNode.Header.SubType  = BBS_BBS_DP;
      SetDevicePathNodeLength (
        &BbsDevicePathNode.Header,
        sizeof (BBS_BBS_DEVICE_PATH)
        );
      BbsDevicePathNode.DeviceType  = DeviceType;
      BbsDevicePathNode.StatusFlag  = 0;
      BbsDevicePathNode.String[0]   = 0;
      DevicePath = AppendDevicePathNode (
                    EndDevicePath,
                    (EFI_DEVICE_PATH_PROTOCOL *) &BbsDevicePathNode
                    );

      FileContext->DevicePath   = DevicePath;
      MenuEntry->HelpString     = DevicePathToStr (FileContext->DevicePath);

      TempStr                   = MenuEntry->HelpString;
      MenuEntry->DisplayString  = AllocateZeroPool (MAX_CHAR);
      ASSERT (MenuEntry->DisplayString != NULL);
      UnicodeSPrint (
        MenuEntry->DisplayString,
        MAX_CHAR,
        L"Boot Legacy [%s]",
        TempStr
        );
      MenuEntry->OptionNumber = OptionNumber;
      OptionNumber++;
      InsertTailList (&mRamDiskFsOptionMenu.Head, &MenuEntry->Link);
    }
  }
  //
  // Remember how many file system options are here
  //
  mRamDiskFsOptionMenu.MenuNumber = OptionNumber;
  return EFI_SUCCESS;
}


/**
  Update the file explower page with the refershed file system.

  @param  CallbackData      BRAM_DISK_CONFIG_PRIVATE_DATA instance
  @param  KeyValue          Key value to identify the type of data to expect.

  @retval TRUE              Inform the caller to create a callback packet to exit file explorer.
  @retval FALSE             Indicate that there is no need to exit file explorer.

**/
BOOLEAN
UpdateRdFileExplorer (
  IN RAM_DISK_CONFIG_PRIVATE_DATA      *CallbackData,
  IN UINT16                                 KeyValue
  )
{
  UINT16              FileOptionMask;
  ENROLL_MENU_ENTRY   *NewMenuEntry;
  EM_FILE_CONTEXT     *NewFileContext;
  EFI_STATUS          Status;
  EFI_DEVICE_PATH_PROTOCOL        *FileDevPath;
  EFI_FILE_HANDLE                 FileHandle;
  CHAR16                               *StringPtr;
  EFI_INPUT_KEY                        Key;

  FileOptionMask    = (UINT16) (ENROLL_FILE_OPTION_MASK & KeyValue);
  NewMenuEntry      = NULL;

  if (EM_UNKNOWN_CONTEXT == CallbackData->DisplayContext) {
    RdFindFileSystem ();
    RdCreateMenuStringToken (CallbackData->HiiHandle, &mRamDiskFsOptionMenu);
    UpdateCreateFromFileMenuPage (CallbackData, &mRamDiskFsOptionMenu);
    CallbackData->DisplayContext = EM_FILE_SYSTEM;
  } else {
    if (EM_FILE_SYSTEM == CallbackData->DisplayContext) {
      NewMenuEntry = RdGetMenuEntry (&mRamDiskFsOptionMenu, FileOptionMask);
    } else if (EM_DIRECTORY == CallbackData->DisplayContext) {
      NewMenuEntry = RdGetMenuEntry (&mRamDiskDirectoryMenu, FileOptionMask);
    }
    if (NewMenuEntry == NULL) {
      return FALSE;
    }
    NewFileContext = (EM_FILE_CONTEXT *) NewMenuEntry->VariableContext;

    if (NewFileContext->IsDir ) {
      CallbackData->DisplayContext = EM_DIRECTORY;

      RemoveEntryList (&NewMenuEntry->Link);

      //
      // Free Menu will cause hang
      //
      RdFreeMenu (&mRamDiskDirectoryMenu);


      RamDiskReclaimStrDepository (mRamDiskFileOptionStrDepository);
      Status = RdFindFiles (CallbackData, NewMenuEntry);
      RdDestroyMenuEntry (NewMenuEntry);
      if (EFI_ERROR (Status)) {
        return TRUE;
      }

      RdCreateMenuStringToken (CallbackData->HiiHandle, &mRamDiskDirectoryMenu);
      UpdateCreateFromFileMenuPage (CallbackData, &mRamDiskDirectoryMenu);
    } else {

      StringPtr = HiiGetString (
                    CallbackData->HiiHandle,
                    STRING_TOKEN (STR_CREATE_FROM_THIS_FILE),
                    NULL
                    );

      mH2ODialog->ConfirmDialog (
                    DlgYesNo,
                    FALSE,
                    0,
                    NULL,
                    &Key,
                    StringPtr
                    );
      gBS->FreePool (StringPtr);
      if (Key.UnicodeChar == CHAR_CARRIAGE_RETURN) {

        FileDevPath = NewFileContext->DevicePath;

        if (FileDevPath != NULL) {

          //
          // Open the file.
          //
          Status = OpenFileByDevicePath (
                     &FileDevPath,
                     &FileHandle,
                     EFI_FILE_MODE_READ,
                     0
                     );
          if (EFI_ERROR (Status)) {
            return FALSE;
          }

          //
          // Create from file, RAM disk size is zero. It will be updated
          // according to the file size.
          //
          Status = HiiCreateRamDisk (
                     0,
                     FileHandle,
                     CallbackData->ConfigStore.MemType
                     );
          if (EFI_ERROR (Status)) {
            return FALSE;
          }

          //
          // Refresh the registered RAM disks list.
          //
          UpdateMainForm (CallbackData);

        }
      }
    }
  }

  return FALSE;


}


/**
  Initialize file system for CreateFromFile menu

  @param[in] CallbackData    Pointer to RAM_DISK_CONFIG_PRIVATE_DATA instance

  @retval EFI_SUCCESS        Initialize enroll menu successful
  @retval Other              Fail to get the file format of enroll menu.
**/
EFI_STATUS
InitializeCreateFromFileMenu (
  IN RAM_DISK_CONFIG_PRIVATE_DATA      *CallbackData
  )
{

  CallbackData->DisplayContext = EM_UNKNOWN_CONTEXT;
  UpdateRdFileExplorer (CallbackData, 0);
  return EFI_SUCCESS;
}
