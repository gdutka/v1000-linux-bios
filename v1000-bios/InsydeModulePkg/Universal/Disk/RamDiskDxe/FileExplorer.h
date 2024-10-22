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

#ifndef _ENROLL_HASH_H_
#define _ENROLL_HASH_H_

#include <Library/DevicePathLib.h>

#include <Protocol/SimpleFileSystem.h>
#include <Protocol/LoadFile.h>

#include <Guid/FileSystemVolumeLabelInfo.h>
#include <Guid/WinCertificate.h>
#include <Guid/ImageAuthentication.h>

//
#include <Protocol/FormBrowserEx.h>
#include <Protocol/DevicePath.h>
#include <Protocol/Hash.h>
#include <Protocol/BlockIo.h>
#include <Protocol/HiiConfigAccess.h>
#include <Protocol/FormBrowser2.h>
#include <Protocol/H2ODialog.h>
#include <Protocol/HiiConfigRouting.h>
#include <Protocol/LegacyBios.h>
#include <Protocol/LoadFile.h>
#include <Protocol/NonVolatileVariable.h>

#include <Guid/MdeModuleHii.h>
#include <Guid/AdmiSecureBoot.h>
#include <Guid/DebugMask.h>
#include <Guid/ImageAuthentication.h>
#include <Guid/InsydeModuleHii.h>
#include <Guid/H2OBdsCheckPoint.h>
#include <Guid/ZeroGuid.h>
#include <Guid/AuthenticatedVariableFormat.h>

#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/BaseLib.h>
#include <Library/HiiLib.h>
#include <Library/PcdLib.h>
#include <Library/PrintLib.h>
#include <Library/GenericBdsLib.h>
#include <Library/GenericUtilityLib.h>
#include <Library/H2OCpLib.h>

#include <PiDxe.h>
#include <Guid/FileSystemVolumeLabelInfo.h>
#include <Guid/FileInfo.h>
#include <Guid/MdeModuleHii.h>

#include <Protocol/HiiConfigAccess.h>
#include <Protocol/DevicePath.h>
#include <Protocol/SimpleFileSystem.h>
#include <Protocol/DevicePathToText.h>
#include <Protocol/FormBrowser2.h>

#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseLib.h>
#include <Library/UefiLib.h>
#include <Library/DevicePathLib.h>
#include <Library/FileExplorerLib.h>
#include <Library/HiiLib.h>
#include <Library/PrintLib.h>

#include "RamDiskImpl.h"

#define FILE_EXPLORER_CALLBACK_DATA_SIGNATURE     SIGNATURE_32 ('f', 'e', 'c', 'k')


#pragma pack(1)


typedef struct {
  EFI_HANDLE                        DeviceHandle;
  EFI_DEVICE_PATH_PROTOCOL          *DevicePath;
  EFI_FILE_HANDLE                   FileHandle;
  UINT16                            *FileName;

  BOOLEAN                           IsRoot;
  BOOLEAN                           IsDir;
} FILE_CONTEXT;

typedef struct {
  UINTN           Signature;
  LIST_ENTRY      Link;
  UINT16          *DisplayString;
  UINT16          *HelpString;
  EFI_STRING_ID   DisplayStringToken;
  EFI_STRING_ID   HelpStringToken;
  VOID            *VariableContext;
} MENU_ENTRY;

typedef struct {
  UINTN           Signature;
  LIST_ENTRY      Head;
  UINTN           MenuNumber;
  BOOLEAN         Used;
} MENU_OPTION;


#define FILE_EXPLORER_PRIVATE_FROM_THIS(a)  CR (a, FILE_EXPLORER_CALLBACK_DATA, FeConfigAccess, FILE_EXPLORER_CALLBACK_DATA_SIGNATURE)

#pragma pack()

extern UINT8    FileExplorerVfrBin[];

#define MENU_OPTION_SIGNATURE      SIGNATURE_32 ('m', 'e', 'n', 'u')
#define MENU_ENTRY_SIGNATURE       SIGNATURE_32 ('e', 'n', 't', 'r')

///
/// Define the maximum characters that will be accepted.
///
#define MAX_CHAR                480
#define FILE_OPTION_OFFSET      0x8000
#define FILE_OPTION_MASK        0x7FFF
#define QUESTION_ID_UPDATE_STEP 200
#define MAX_FILE_NAME_LEN       20
#define MAX_FOLDER_NAME_LEN     20
#define NEW_FILE_QUESTION_ID_BASE   0x5000;
#define NEW_FOLDER_QUESTION_ID_BASE 0x6000;


/**
  Initialize file system for CreateFromFile menu

  @param[in] CallbackData    Pointer to SECURE_BOOT_MANAGER_CALLBACK_DATA instance

  @retval EFI_SUCCESS        Initialize enroll menu successful
  @retval Other              Fail to get the file format of enroll menu.
**/
EFI_STATUS
InitializeCreateFromFileMenu (
  IN RAM_DISK_CONFIG_PRIVATE_DATA      *CallbackData
  );



#define EM_MENU_ENTRY_SIGNATURE        SIGNATURE_32 ('E', 'M', 'E', 'T')
#define HASH_LINK_LIST_SIGNATURE       SIGNATURE_32 ('H', 'A', 'L', 'L')
#define EM_FILE_CONTEXT_SELECT         0x1
#define ENROLL_FILE_OPTION_OFFSET      0x8000
#define ENROLL_FILE_OPTION_MASK        0x7FFF

#define EM_UPDATE_DATA_SIZE            0x100000

#define EM_FILE_SYSTEM                 0x0001
#define EM_DIRECTORY                   0x0002
#define EM_UNKNOWN_CONTEXT             0x0003

#define EM_FILE_FORMAT_EFI             0x0001
#define EM_FILE_FORMAT_CER             0x0002
#define EM_FILE_FORMAT_SHA256          0x0003
#define EM_FILE_FORMAT_SHA512          0x0004

#define HASHALG_MD5                    0x00000000
#define HASHALG_SHA1                   0x00000001
#define HASHALG_SHA224                 0x00000002
#define HASHALG_SHA256                 0x00000003
#define HASHALG_SHA384                 0x00000004
#define HASHALG_SHA512                 0x00000005
#define HASHALG_MAX                    0x00000006

typedef struct {
  UINTN           Signature;
  LIST_ENTRY      Head;
  UINTN           MenuNumber;
} ENROLL_MENU_OPTION;

typedef struct {
  UINTN           Signature;
  LIST_ENTRY      Link;
  UINTN           OptionNumber;
  UINT16          *DisplayString;
  UINT16          *HelpString;
  EFI_STRING_ID   DisplayStringToken;
  EFI_STRING_ID   HelpStringToken;
  UINTN           ContextSelection;
  VOID            *VariableContext;
} ENROLL_MENU_ENTRY;

typedef struct {
  EFI_HANDLE                        Handle;
  EFI_DEVICE_PATH_PROTOCOL          *DevicePath;
  EFI_FILE_HANDLE                   FHandle;
  UINT16                            *FileName;
  EFI_FILE_SYSTEM_VOLUME_LABEL      *Info;
  BOOLEAN                           IsRoot;
  BOOLEAN                           IsDir;
  BOOLEAN                           IsRemovableMedia;
  BOOLEAN                           IsLoadFile;
  BOOLEAN                           IsBootLegacy;
} EM_FILE_CONTEXT;

typedef struct {
  WIN_CERTIFICATE Hdr;
  UINT8           CertData[1];
} WIN_CERTIFICATE_EFI_PKCS;

typedef struct {
  EFI_GUID                 *Index;
  CHAR16                   *Name;           ///< Name for Hash Algorithm
  UINTN                    DigestLength;    ///< Digest Length
  UINT8                    *OidValue;       ///< Hash Algorithm OID ASN.1 Value
  UINTN                    OidLength;       ///< Length of Hash OID Value
} HASH_TABLE;

typedef struct {
  UINTN           Signature;
  LIST_ENTRY      Link;
  UINTN           ShaType;
  UINTN           HashSize;
  UINT8           Hash[32];
} HASH_LINK_LIST;




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
  );


/**
  This function will open a file or directory referenced by DevicePath.

  This function opens a file with the open mode according to the file path. The
  Attributes is valid only for EFI_FILE_MODE_CREATE.

  @param[in, out] FilePath   On input, the device path to the file.
                             On output, the remaining device path.
  @param[out]     FileHandle Pointer to the file handle.
  @param[in]      OpenMode   The mode to open the file with.
  @param[in]      Attributes The file's file attributes.

  @retval EFI_SUCCESS             The information was set.
  @retval EFI_INVALID_PARAMETER   One of the parameters has an invalid value.
  @retval EFI_UNSUPPORTED         Could not open the file path.
  @retval EFI_NOT_FOUND           The specified file could not be found on the
                                  device or the file system could not be found
                                  on the device.
  @retval EFI_NO_MEDIA            The device has no medium.
  @retval EFI_MEDIA_CHANGED       The device has a different medium in it or
                                  the medium is no longer supported.
  @retval EFI_DEVICE_ERROR        The device reported an error.
  @retval EFI_VOLUME_CORRUPTED    The file system structures are corrupted.
  @retval EFI_WRITE_PROTECTED     The file or medium is write protected.
  @retval EFI_ACCESS_DENIED       The file was opened read only.
  @retval EFI_OUT_OF_RESOURCES    Not enough resources were available to open
                                  the file.
  @retval EFI_VOLUME_FULL         The volume is full.
**/
EFI_STATUS
EFIAPI
OpenFileByDevicePath(
  IN OUT EFI_DEVICE_PATH_PROTOCOL           **FilePath,
  OUT EFI_FILE_HANDLE                       *FileHandle,
  IN UINT64                                 OpenMode,
  IN UINT64                                 Attributes
  );

typedef struct {
  EFI_HANDLE                        Handle;
  EFI_DEVICE_PATH_PROTOCOL          *DevicePath;
  EFI_FILE_HANDLE                   FHandle;
  UINT16                            *FileName;
  EFI_FILE_SYSTEM_VOLUME_LABEL      *Info;

  BOOLEAN                           IsRoot;
  BOOLEAN                           IsDir;
  BOOLEAN                           IsRemovableMedia;
  BOOLEAN                           IsLoadFile;
  BOOLEAN                           IsBootLegacy;
} BM_FILE_CONTEXT;


#endif
