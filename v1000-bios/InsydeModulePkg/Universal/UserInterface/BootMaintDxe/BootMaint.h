/** @file
 Header file for boot maintenance module.

;******************************************************************************
;* Copyright (c) 2012 - 2017, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

Copyright (c) 2004 - 2011, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

*/

#ifndef _BOOT_MAINT_H_
#define _BOOT_MAINT_H_

#include <Protocol/SimpleFileSystem.h>
#include <Protocol/BlockIo.h>
#include <Protocol/HiiConfigAccess.h>
#include <Protocol/FormBrowser2.h>
#include <Protocol/HiiConfigRouting.h>
#include <Protocol/HiiDatabase.h>
#include <Protocol/LegacyBios.h>
#include <Protocol/SerialIo.h>
#include <Protocol/LoadFile.h>

#include <Guid/MdeModuleHii.h>
#include <Guid/GlobalVariable.h>
#include <Guid/FileSystemVolumeLabelInfo.h>
#include <Guid/LegacyDevOrder.h>
#include <Guid/InsydeModuleHii.h>

#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/BaseLib.h>
#include <Library/HiiLib.h>
#include <Library/PrintLib.h>
#include <Library/DevicePathLib.h>
#include <Library/OemGraphicsLib.h>
#include <Library/GenericBdsLib.h>
#include <Library/GenericUtilityLib.h>
#include <Library/PcdLib.h>
#include <Library/H2OCpLib.h>
#include <Library/UefiHiiServicesLib.h>
#include "FormGuid.h"

//
// Constants which are variable names used to access variables
//
#define VAR_CON_OUT_MODE L"ConOutMode"

//
// String Contant
//
#define STR_FLOPPY       L"Floppy Drive #%02x"
#define STR_HARDDISK     L"HardDisk Drive #%02x"
#define STR_CDROM        L"ATAPI CDROM Drive #%02x"
#define STR_NET          L"NET Drive #%02x"
#define STR_BEV          L"BEV Drive #%02x"
#define STR_FLOPPY_HELP   L"Select Floppy Drive #%02x"
#define STR_HARDDISK_HELP L"Select HardDisk Drive #%02x"
#define STR_CDROM_HELP    L"Select ATAPI CDROM Drive #%02x"
#define STR_NET_HELP      L"NET Drive #%02x"
#define STR_BEV_HELP      L"BEV Drive #%02x"

//
// Variable created with this flag will be "Efi:...."
//
#define VAR_FLAG  EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE

//
// These are the VFR compiler generated data representing our VFR data.
//
extern UINT8    BmBin[];
extern UINT8    FEBin[];

//
// Below are the number of options in Baudrate, Databits,
// Parity and Stopbits selection for serial ports.
//
#define BM_COM_ATTR_BUADRATE  19
#define BM_COM_ATTR_DATABITS  4
#define BM_COM_ATTR_PARITY    5
#define BM_COM_ATTR_STOPBITS  3

//
// Callback function helper
//
#define BMM_CALLBACK_DATA_SIGNATURE     SIGNATURE_32 ('C', 'b', 'c', 'k')
#define BMM_CALLBACK_DATA_FROM_THIS(a)  CR (a, BMM_CALLBACK_DATA, BmmConfigAccess, BMM_CALLBACK_DATA_SIGNATURE)

#define FE_CALLBACK_DATA_FROM_THIS(a)   CR (a, BMM_CALLBACK_DATA, FeConfigAccess, BMM_CALLBACK_DATA_SIGNATURE)

//
// Enumeration type definition
//
typedef enum _TYPE_OF_TERMINAL {
  TerminalTypePcAnsi                             = 0,
  TerminalTypeVt100,
  TerminalTypeVt100Plus,
  TerminalTypeVtUtf8
} TYPE_OF_TERMINAL;

typedef enum _FILE_EXPLORER_STATE {
  FileExplorerStateInActive                      = 0,
  FileExplorerStateBootFromFile,
  FileExplorerStateAddBootOption,
  FileExplorerStateAddDriverOptionState,
  FileExplorerStateUnknown
} FILE_EXPLORER_STATE;

typedef enum _FILE_EXPLORER_DISPLAY_CONTEXT {
  FileExplorerDisplayFileSystem,
  FileExplorerDisplayDirectory,
  FileExplorerDisplayUnknown
} FILE_EXPLORER_DISPLAY_CONTEXT;

//
// All of the signatures that will be used in list structure
//
#define BM_MENU_OPTION_SIGNATURE      SIGNATURE_32 ('m', 'e', 'n', 'u')
#define BM_LOAD_OPTION_SIGNATURE      SIGNATURE_32 ('l', 'o', 'a', 'd')
#define BM_CONSOLE_OPTION_SIGNATURE   SIGNATURE_32 ('c', 'n', 's', 'l')
#define BM_FILE_OPTION_SIGNATURE      SIGNATURE_32 ('f', 'i', 'l', 'e')
#define BM_HANDLE_OPTION_SIGNATURE    SIGNATURE_32 ('h', 'n', 'd', 'l')
#define BM_TERMINAL_OPTION_SIGNATURE  SIGNATURE_32 ('t', 'r', 'm', 'l')
#define BM_MENU_ENTRY_SIGNATURE       SIGNATURE_32 ('e', 'n', 't', 'r')

#define BM_LOAD_CONTEXT_SELECT        0x0
#define BM_CONSOLE_CONTEXT_SELECT     0x1
#define BM_FILE_CONTEXT_SELECT        0x2
#define BM_HANDLE_CONTEXT_SELECT      0x3
#define BM_TERMINAL_CONTEXT_SELECT    0x5

#define BM_CONSOLE_IN_CONTEXT_SELECT  0x6
#define BM_CONSOLE_OUT_CONTEXT_SELECT 0x7
#define BM_CONSOLE_ERR_CONTEXT_SELECT 0x8
#define BM_LEGACY_DEV_CONTEXT_SELECT  0x9

//
// Buffer size for update data
//
#define UPDATE_DATA_SIZE        0x100000

//
// Namespace of callback keys used in display and file system navigation
//
#define MAX_BBS_OFFSET          0xE000
#define NET_OPTION_OFFSET       0xD800
#define BEV_OPTION_OFFSET       0xD000
#define FD_OPTION_OFFSET        0xC000
#define HD_OPTION_OFFSET        0xB000
#define CD_OPTION_OFFSET        0xA000
#define FILE_OPTION_OFFSET      0x8000
#define FILE_OPTION_MASK        0x7FFF
#define HANDLE_OPTION_OFFSET    0x7000
#define CONSOLE_OPTION_OFFSET   0x6000
#define TERMINAL_OPTION_OFFSET  0x5000
#define CONFIG_OPTION_OFFSET    0x1200
#define KEY_VALUE_OFFSET        0x1100
#define FORM_ID_OFFSET          0x1000

//
// VarOffset that will be used to create question
// all these values are computed from the structure
// defined below
//
#define VAR_OFFSET(Field)              ((UINT16) ((UINTN) &(((BMM_FAKE_NV_DATA *) 0)->Field)))

//
// Question Id of Zero is invalid, so add an offset to it
//
#define QUESTION_ID(Field)             (VAR_OFFSET (Field) + CONFIG_OPTION_OFFSET)

#define BOOT_TIME_OUT_VAR_OFFSET        VAR_OFFSET (BootTimeOut)
#define BOOT_NEXT_VAR_OFFSET            VAR_OFFSET (BootNext)
#define COM1_BAUD_RATE_VAR_OFFSET       VAR_OFFSET (COM1BaudRate)
#define COM1_DATA_RATE_VAR_OFFSET       VAR_OFFSET (COM1DataRate)
#define COM1_STOP_BITS_VAR_OFFSET       VAR_OFFSET (COM1StopBits)
#define COM1_PARITY_VAR_OFFSET          VAR_OFFSET (COM1Parity)
#define COM1_TERMINAL_VAR_OFFSET        VAR_OFFSET (COM2TerminalType)
#define COM2_BAUD_RATE_VAR_OFFSET       VAR_OFFSET (COM2BaudRate)
#define COM2_DATA_RATE_VAR_OFFSET       VAR_OFFSET (COM2DataRate)
#define COM2_STOP_BITS_VAR_OFFSET       VAR_OFFSET (COM2StopBits)
#define COM2_PARITY_VAR_OFFSET          VAR_OFFSET (COM2Parity)
#define COM2_TERMINAL_VAR_OFFSET        VAR_OFFSET (COM2TerminalType)
#define DRV_ADD_HANDLE_DESC_VAR_OFFSET  VAR_OFFSET (DriverAddHandleDesc)
#define DRV_ADD_ACTIVE_VAR_OFFSET       VAR_OFFSET (DriverAddActive)
#define DRV_ADD_RECON_VAR_OFFSET        VAR_OFFSET (DriverAddForceReconnect)
#define CON_IN_COM1_VAR_OFFSET          VAR_OFFSET (ConsoleInputCOM1)
#define CON_IN_COM2_VAR_OFFSET          VAR_OFFSET (ConsoleInputCOM2)
#define CON_OUT_COM1_VAR_OFFSET         VAR_OFFSET (ConsoleOutputCOM1)
#define CON_OUT_COM2_VAR_OFFSET         VAR_OFFSET (ConsoleOutputCOM2)
#define CON_ERR_COM1_VAR_OFFSET         VAR_OFFSET (ConsoleErrorCOM1)
#define CON_ERR_COM2_VAR_OFFSET         VAR_OFFSET (ConsoleErrorCOM2)
#define CON_MODE_VAR_OFFSET             VAR_OFFSET (ConsoleOutMode)
#define CON_DEVICE_VAR_OFFSET           VAR_OFFSET (ConsoleCheck)
#define OPTION_ORDER_VAR_OFFSET         VAR_OFFSET (OptionOrder)
#define OPTION_DEL_VAR_OFFSET           VAR_OFFSET (OptionDel)
#define DRIVER_ADD_OPTION_VAR_OFFSET    VAR_OFFSET (DriverAddHandleOptionalData)
#define COM_BAUD_RATE_VAR_OFFSET        VAR_OFFSET (COMBaudRate)
#define COM_DATA_RATE_VAR_OFFSET        VAR_OFFSET (COMDataRate)
#define COM_STOP_BITS_VAR_OFFSET        VAR_OFFSET (COMStopBits)
#define COM_PARITY_VAR_OFFSET           VAR_OFFSET (COMParity)
#define COM_TERMINAL_VAR_OFFSET         VAR_OFFSET (COMTerminalType)
#define COM_FLOWCONTROL_VAR_OFFSET      VAR_OFFSET (COMFlowControl)
#define LEGACY_FD_VAR_OFFSET            VAR_OFFSET (LegacyFD)
#define LEGACY_HD_VAR_OFFSET            VAR_OFFSET (LegacyHD)
#define LEGACY_CD_VAR_OFFSET            VAR_OFFSET (LegacyCD)
#define LEGACY_NET_VAR_OFFSET           VAR_OFFSET (LegacyNET)
#define LEGACY_BEV_VAR_OFFSET           VAR_OFFSET (LegacyBEV)

#define BOOT_TIME_OUT_QUESTION_ID       QUESTION_ID (BootTimeOut)
#define BOOT_NEXT_QUESTION_ID           QUESTION_ID (BootNext)
#define COM1_BAUD_RATE_QUESTION_ID      QUESTION_ID (COM1BaudRate)
#define COM1_DATA_RATE_QUESTION_ID      QUESTION_ID (COM1DataRate)
#define COM1_STOP_BITS_QUESTION_ID      QUESTION_ID (COM1StopBits)
#define COM1_PARITY_QUESTION_ID         QUESTION_ID (COM1Parity)
#define COM1_TERMINAL_QUESTION_ID       QUESTION_ID (COM2TerminalType)
#define COM2_BAUD_RATE_QUESTION_ID      QUESTION_ID (COM2BaudRate)
#define COM2_DATA_RATE_QUESTION_ID      QUESTION_ID (COM2DataRate)
#define COM2_STOP_BITS_QUESTION_ID      QUESTION_ID (COM2StopBits)
#define COM2_PARITY_QUESTION_ID         QUESTION_ID (COM2Parity)
#define COM2_TERMINAL_QUESTION_ID       QUESTION_ID (COM2TerminalType)
#define DRV_ADD_HANDLE_DESC_QUESTION_ID QUESTION_ID (DriverAddHandleDesc)
#define DRV_ADD_ACTIVE_QUESTION_ID      QUESTION_ID (DriverAddActive)
#define DRV_ADD_RECON_QUESTION_ID       QUESTION_ID (DriverAddForceReconnect)
#define CON_IN_COM1_QUESTION_ID         QUESTION_ID (ConsoleInputCOM1)
#define CON_IN_COM2_QUESTION_ID         QUESTION_ID (ConsoleInputCOM2)
#define CON_OUT_COM1_QUESTION_ID        QUESTION_ID (ConsoleOutputCOM1)
#define CON_OUT_COM2_QUESTION_ID        QUESTION_ID (ConsoleOutputCOM2)
#define CON_ERR_COM1_QUESTION_ID        QUESTION_ID (ConsoleErrorCOM1)
#define CON_ERR_COM2_QUESTION_ID        QUESTION_ID (ConsoleErrorCOM2)
#define CON_MODE_QUESTION_ID            QUESTION_ID (ConsoleOutMode)
#define CON_DEVICE_QUESTION_ID          QUESTION_ID (ConsoleCheck)
#define OPTION_ORDER_QUESTION_ID        QUESTION_ID (OptionOrder)
#define OPTION_DEL_QUESTION_ID          QUESTION_ID (OptionDel)
#define DRIVER_ADD_OPTION_QUESTION_ID   QUESTION_ID (DriverAddHandleOptionalData)
#define COM_BAUD_RATE_QUESTION_ID       QUESTION_ID (COMBaudRate)
#define COM_DATA_RATE_QUESTION_ID       QUESTION_ID (COMDataRate)
#define COM_STOP_BITS_QUESTION_ID       QUESTION_ID (COMStopBits)
#define COM_PARITY_QUESTION_ID          QUESTION_ID (COMParity)
#define COM_TERMINAL_QUESTION_ID        QUESTION_ID (COMTerminalType)
#define COM_FLOWCONTROL_QUESTION_ID     QUESTION_ID (COMFlowControl)
#define LEGACY_FD_QUESTION_ID           QUESTION_ID (LegacyFD)
#define LEGACY_HD_QUESTION_ID           QUESTION_ID (LegacyHD)
#define LEGACY_CD_QUESTION_ID           QUESTION_ID (LegacyCD)
#define LEGACY_NET_QUESTION_ID          QUESTION_ID (LegacyNET)
#define LEGACY_BEV_QUESTION_ID          QUESTION_ID (LegacyBEV)

#define STRING_DEPOSITORY_NUMBER        8

///
/// Serial Ports attributes, first one is the value for
/// return from callback function, stringtoken is used to
/// display the value properly
///
typedef struct {
  UINTN   Value;
  UINT16  StringToken;
} COM_ATTR;

#pragma pack(1)
///
/// For each legacy boot option in BBS table, a corresponding Boot#### variables is created.
/// The structure saves the mapping relationship between #### and the index in the BBS table.
///
typedef struct {
  UINT16    BootOptionNumber;
  UINT16    BbsIndex;
  UINT16    BbsType;
} BOOT_OPTION_BBS_MAPPING;
#pragma pack()

typedef struct {
  UINT64                    BaudRate;
  UINT8                     DataBits;
  UINT8                     Parity;
  UINT8                     StopBits;

  UINT8                     BaudRateIndex;
  UINT8                     DataBitsIndex;
  UINT8                     ParityIndex;
  UINT8                     StopBitsIndex;

  UINT8                     FlowControl;

  UINT8                     IsConIn;
  UINT8                     IsConOut;
  UINT8                     IsStdErr;
  UINT8                     TerminalType;

  EFI_DEVICE_PATH_PROTOCOL  *DevicePath;
} BM_TERMINAL_CONTEXT;

typedef struct {
  BOOLEAN                   IsBootNext;
  BOOLEAN                   LoadOptionModified;
  BOOLEAN                   Deleted;

  BOOLEAN                   IsLegacy;
  BOOLEAN                   IsActive;
  BOOLEAN                   ForceReconnect;
  UINTN                     OptionalDataSize;

  UINTN                     LoadOptionSize;
  UINT8                     *LoadOption;

  UINT32                    Attributes;
  UINT16                    FilePathListLength;
  UINT16                    *Description;
  EFI_DEVICE_PATH_PROTOCOL  *FilePathList;
  UINT8                     *OptionalData;

  UINT16                    BbsIndex;
} BM_LOAD_CONTEXT;

typedef struct {
  BBS_TABLE *BbsEntry;
  UINT16    BbsIndex;
  UINT16    BbsCount;
  CHAR16    *Description;
} BM_LEGACY_DEVICE_CONTEXT;

typedef struct {
  BOOLEAN                   IsActive;

  BOOLEAN                   IsTerminal;

  EFI_DEVICE_PATH_PROTOCOL  *DevicePath;
} BM_CONSOLE_CONTEXT;

typedef struct {
  UINTN   Column;
  UINTN   Row;
} CONSOLE_OUT_MODE;

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

typedef struct {
  EFI_HANDLE                Handle;
  EFI_DEVICE_PATH_PROTOCOL  *DevicePath;
} BM_HANDLE_CONTEXT;

typedef struct {
  UINTN           Signature;
  LIST_ENTRY      Head;
  UINTN           MenuNumber;
} BM_MENU_OPTION;

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
} BM_MENU_ENTRY;

typedef struct {
  //
  // Shared callback data.
  //
  UINTN                          Signature;

  BM_MENU_ENTRY                  *MenuEntry;
  BM_HANDLE_CONTEXT              *HandleContext;
  BM_FILE_CONTEXT                *FileContext;
  BM_LOAD_CONTEXT                *LoadContext;
  BM_TERMINAL_CONTEXT            *TerminalContext;
  UINTN                          CurrentTerminal;
  BBS_TYPE                       BbsType;

  //
  // BMM main formset callback data.
  //
  EFI_HII_HANDLE                 BmmHiiHandle;
  EFI_HANDLE                     BmmDriverHandle;
  EFI_HII_CONFIG_ACCESS_PROTOCOL BmmConfigAccess;
  EFI_FORM_ID                    BmmCurrentPageId;
  EFI_FORM_ID                    BmmPreviousPageId;
  BOOLEAN                        BmmAskSaveOrNot;
  BMM_FAKE_NV_DATA               BmmFakeNvData;
  BMM_FAKE_NV_DATA               BmmOldFakeNVData;

  //
  // File explorer formset callback data.
  //
  EFI_HII_HANDLE                 FeHiiHandle;
  EFI_HANDLE                     FeDriverHandle;
  EFI_HII_CONFIG_ACCESS_PROTOCOL FeConfigAccess;
  FILE_EXPLORER_STATE            FeCurrentState;
  FILE_EXPLORER_DISPLAY_CONTEXT  FeDisplayContext;
  FILE_EXPLORER_NV_DATA          FeFakeNvData;
} BMM_CALLBACK_DATA;

typedef struct _STRING_LIST_NODE STRING_LIST_NODE;

struct _STRING_LIST_NODE {
  EFI_STRING_ID             StringToken;
  STRING_LIST_NODE          *Next;
};

typedef struct _STRING_DEPOSITORY {
  UINTN             TotalNodeNumber;
  STRING_LIST_NODE  *CurrentNode;
  STRING_LIST_NODE  *ListHead;
} STRING_DEPOSITORY;

//
// #pragma pack()
//
// For initializing File System menu
//

/**
 This function build the FsOptionMenu list which records all
 available file system in the system. They includes all instances
 of EFI_SIMPLE_FILE_SYSTEM_PROTOCOL, all instances of EFI_LOAD_FILE_SYSTEM
 and all type of legacy boot device.

 @param[in] CallbackData    BMM context data

 @retval EFI_SUCCESS             Success find the file system
 @retval EFI_OUT_OF_RESOURCES    Can not create menu entry
**/
EFI_STATUS
BOpt_FindFileSystem (
  IN BMM_CALLBACK_DATA          *CallbackData
  );

/**
 Find files under current directory
 All files and sub-directories in current directory
 will be stored in DirectoryMenu for future use.

 @param[in] CallbackData  The BMM context data.
 @param[in] MenuEntry     The Menu Entry.

 @retval EFI_SUCCESS         Get files from current dir successfully.
 @return Other value if can't get files from current dir.
**/
EFI_STATUS
BOpt_FindFiles (
  IN BMM_CALLBACK_DATA          *CallbackData,
  IN BM_MENU_ENTRY              *MenuEntry
  );

/**
 Find drivers that will be added as Driver#### variables from handles
 in current system environment
 All valid handles in the system except those consume SimpleFs, LoadFile
 are stored in DriverMenu for future use.

 @retval EFI_SUCCESS The function complets successfully.
 @return Other value if failed to build the DriverMenu.
**/
EFI_STATUS
BOpt_FindDrivers (
  VOID
  );

/**
 Build the BootOptionMenu according to BootOrder Variable.
 This Routine will access the Boot#### to get EFI_LOAD_OPTION.

 @param[in] CallbackData The BMM context data.

 @return The number of the Var Boot####.
**/
EFI_STATUS
BOpt_GetBootOptions (
  IN  BMM_CALLBACK_DATA         *CallbackData
  );

/**
 Build up all DriverOptionMenu

 @param[in] CallbackData The BMM context data.

 @return EFI_SUCESS The functin completes successfully.
 @retval EFI_OUT_OF_RESOURCES Not enough memory to compete the operation.
**/
EFI_STATUS
BOpt_GetDriverOptions (
  IN  BMM_CALLBACK_DATA         *CallbackData
  );

/**
 Build the LegacyFDMenu LegacyHDMenu LegacyCDMenu according to LegacyBios.GetBbsInfo().

 @retval EFI_SUCCESS The function complete successfully.
 @retval EFI_OUT_OF_RESOURCES No enough memory to complete this function.
**/
EFI_STATUS
BOpt_GetLegacyOptions (
  VOID
  );

/**
 Free out resouce allocated from Legacy Boot Options.
**/
VOID
BOpt_FreeLegacyOptions (
  VOID
  );

/**
 Free resources allocated in Allocate Rountine.

 @param FreeMenu        Menu to be freed
**/
VOID
BOpt_FreeMenu (
  BM_MENU_OPTION        *FreeMenu
  );


/**
 Append file name to existing file name.

 @param[in] Str1  The existing file name
 @param[in] Str2  The file name to be appended

 @return Allocate a new string to hold the appended result.
         Caller is responsible to free the returned string.
**/
CHAR16*
BOpt_AppendFileName (
  IN  CHAR16  *Str1,
  IN  CHAR16  *Str2
  );

/**
 Check whether current FileName point to a valid
 Efi Image File.

 @param[in] FileName  File need to be checked.

 @retval TRUE  Is Efi Image
 @retval FALSE Not a valid Efi Image
**/
BOOLEAN
BOpt_IsEfiImageName (
  IN UINT16  *FileName
  );

/**
 Check whether current FileName point to a valid Efi Application

 @param[in] Dir       Pointer to current Directory
 @param[in] FileName  Pointer to current File name.

 @retval TRUE      Is a valid Efi Application
 @retval FALSE     not a valid Efi Application
**/
BOOLEAN
BOpt_IsEfiApp (
  IN EFI_FILE_HANDLE Dir,
  IN UINT16          *FileName
  );

/**
 Get the Option Number that has not been allocated for use.

 @param Type  The type of Option.

 @return The available Option Number.
**/
UINT16
BOpt_GetOptionNumber (
  CHAR16        *Type
  );

/**
 Get the Option Number for Boot#### that does not used.

 @return The available Option Number.
**/
UINT16
BOpt_GetBootOptionNumber (
  VOID
  );

/**
 Get the Option Number for Driver#### that does not used.

 @return The unused Option Number.
**/
UINT16
BOpt_GetDriverOptionNumber (
  VOID
  );

/**
 Create a menu entry give a Menu type.

 @param MenuType        The Menu type to be created.

 @retval NULL           If failed to create the menu.
 @return                The menu.
**/
BM_MENU_ENTRY                     *
BOpt_CreateMenuEntry (
  UINTN           MenuType
  );

/**
 Free up all resource allocated for a BM_MENU_ENTRY.

 @param MenuEntry   A pointer to BM_MENU_ENTRY.
**/
VOID
BOpt_DestroyMenuEntry (
  BM_MENU_ENTRY         *MenuEntry
  );

/**
 Get the Menu Entry from the list in Menu Entry List.

 If MenuNumber is great or equal to the number of Menu
 Entry in the list, then ASSERT.

 @param MenuOption      The Menu Entry List to read the menu entry.
 @param MenuNumber      The index of Menu Entry.

 @return The Menu Entry.
**/
BM_MENU_ENTRY                     *
BOpt_GetMenuEntry (
  BM_MENU_OPTION      *MenuOption,
  UINTN               MenuNumber
  );

//
// Locate all serial io devices for console
//
/**
 Build a list containing all serial devices.

 @retval EFI_SUCCESS The function complete successfully.
 @retval EFI_UNSUPPORTED No serial ports present.
**/
EFI_STATUS
LocateSerialIo (
  VOID
  );

//
// Initializing Console menu
//
/**
 Build up ConsoleOutMenu, ConsoleInpMenu and ConsoleErrMenu

 @retval EFI_SUCCESS    The function always complete successfully.
**/
EFI_STATUS
GetAllConsoles(
  VOID
  );

//
// Get current mode information
//
/**
 Get mode number according to column and row

 @param[in] CallbackData    The BMM context data.
**/
VOID
GetConsoleOutMode (
  IN  BMM_CALLBACK_DATA    *CallbackData
  );

//
// Cleaning up console menu
//
/**
 Free ConsoleOutMenu, ConsoleInpMenu and ConsoleErrMenu

 @retval EFI_SUCCESS    The function always complete successfully.
**/
EFI_STATUS
FreeAllConsoles (
  VOID
  );

/**
 Update the device path that describing a terminal device
 based on the new BaudRate, Data Bits, parity and Stop Bits
 set.

 @param[in, out] DevicePath     The devicepath protocol instance wanted to be updated.
**/
VOID
ChangeVariableDevicePath (
  IN OUT EFI_DEVICE_PATH_PROTOCOL  *DevicePath
  );

/**
 Update the multi-instance device path of Terminal Device based on
 the global TerminalMenu. If ChangeTernimal is TRUE, the terminal
 device path in the Terminal Device in TerminalMenu is also updated.

 @param[in, out] DevicePath      The multi-instance device path.
 @param[in]      ChangeTerminal  TRUE, then device path in the Terminal Device
                                 in TerminalMenu is also updated; FALSE, no update.

 @return EFI_SUCCESS    The function completes successfully.
**/
EFI_STATUS
ChangeTerminalDevicePath (
  IN OUT EFI_DEVICE_PATH_PROTOCOL  **DevicePath,
  IN BOOLEAN                   ChangeTerminal
  );

//
// Variable operation by menu selection
//
/**
 This function create a currently loaded Boot Option from
 the BMM. It then appends this Boot Option to the end of
 the "BootOrder" list. It also append this Boot Opotion to the end
 of BootOptionMenu.

 @param[in] CallbackData           The BMM context data.
 @param[in] NvRamMap               The file explorer formset internal state.

 @retval EFI_OUT_OF_RESOURCES  If not enought memory to complete the operation.
 @retval EFI_SUCCESS           If function completes successfully.
**/
EFI_STATUS
Var_UpdateBootOption (
  IN  BMM_CALLBACK_DATA                   *CallbackData,
  IN  FILE_EXPLORER_NV_DATA               *NvRamMap
  );

/**
 Delete Boot Option that represent a Deleted state in BootOptionMenu.
 After deleting this boot option, call Var_ChangeBootOrder to
 make sure BootOrder is in valid state.

 @retval EFI_SUCCESS   If all boot load option EFI Variables corresponding to
                       BM_LOAD_CONTEXT marked for deletion is deleted
 @return Others        If failed to update the "BootOrder" variable after deletion.
**/
EFI_STATUS
Var_DelBootOption (
  VOID
  );

/**
 After any operation on Boot####, there will be a discrepancy in BootOrder.
 Since some are missing but in BootOrder, while some are present but are
 not reflected by BootOrder. Then a function rebuild BootOrder from
 scratch by content from BootOptionMenu is needed.

 @retval  EFI_SUCCESS  The boot order is updated successfully.
 @return  other than EFI_SUCCESS if failed to change the "BootOrder" EFI Variable.
**/
EFI_STATUS
Var_ChangeBootOrder (
  VOID
  );

/**
 This function create a currently loaded Drive Option from
 the BMM. It then appends this Driver Option to the end of
 the "DriverOrder" list. It append this Driver Opotion to the end
 of DriverOptionMenu.

 @param[in] CallbackData    The BMM context data.
 @param[in] HiiHandle       The HII handle associated with the BMM formset.
 @param[in] DescriptionData The description of this driver option.
 @param[in] DescriptionDataSize    Size of the description of this driver option.
 @param[in] OptionalData    The optional load option.
 @param[in] ForceReconnect  If to force reconnect.

 @retval EFI_OUT_OF_RESOURCES If not enought memory to complete the operation.
 @retval EFI_SUCCESS          If function completes successfully.
**/
EFI_STATUS
Var_UpdateDriverOption (
  IN  BMM_CALLBACK_DATA         *CallbackData,
  IN  EFI_HII_HANDLE            HiiHandle,
  IN  UINT16                    *DescriptionData,
  IN  UINTN                     DescriptionDataSize,
  IN  UINT16                    *OptionalData,
  IN  UINT8                     ForceReconnect
  );

/**
 Delete Load Option that represent a Deleted state in BootOptionMenu.
 After deleting this Driver option, call Var_ChangeDriverOrder to
 make sure DriverOrder is in valid state.

 @retval EFI_SUCCESS Load Option is successfully updated.
 @return Other value than EFI_SUCCESS if failed to update "Driver Order" EFI Variable.
**/
EFI_STATUS
Var_DelDriverOption (
  VOID
  );

/**
 After any operation on Driver####, there will be a discrepancy in
 DriverOrder. Since some are missing but in DriverOrder, while some
 are present but are not reflected by DriverOrder. Then a function
 rebuild DriverOrder from scratch by content from DriverOptionMenu is
 needed.

 @retval  EFI_SUCCESS  The driver order is updated successfully.
 @return  other than EFI_SUCCESS if failed to set the "DriverOrder" EFI Variable.
**/
EFI_STATUS
Var_ChangeDriverOrder (
  VOID
  );

/**
 This function delete and build multi-instance device path ConIn
 console device.

 @retval EFI_SUCCESS    The function complete successfully.
 @return The EFI variable can not be saved. See gRT->SetVariable for detail return information.
**/
EFI_STATUS
Var_UpdateConsoleInpOption (
  VOID
  );

/**
 This function delete and build multi-instance device path ConOut console device.

 @retval EFI_SUCCESS    The function complete successfully.
 @return The EFI variable can not be saved. See gRT->SetVariable for detail return information.
**/
EFI_STATUS
Var_UpdateConsoleOutOption (
  VOID
  );

/**
 This function delete and build multi-instance device path ErrOut console device.

 @retval EFI_SUCCESS    The function complete successfully.
 @return The EFI variable can not be saved. See gRT->SetVariable for detail return information.
**/
EFI_STATUS
Var_UpdateErrorOutOption (
  VOID
  );

/**
 Update the device path of "ConOut", "ConIn" and "ErrOut" based on the new BaudRate, Data Bits,
 parity and stop Bits set.
**/
VOID
Var_UpdateAllConsoleOption (
  VOID
  );

/**
 This function update the "BootNext" EFI Variable. If there is no "BootNex" specified in BMM,
 this EFI Variable is deleted.
 It also update the BMM context data specified the "BootNext" value.

 @param[in] CallbackData    The BMM context data.

 @retval EFI_SUCCESS    The function complete successfully.
 @return The EFI variable can not be saved. See gRT->SetVariable for detail return information.
**/
EFI_STATUS
Var_UpdateBootNext (
  IN BMM_CALLBACK_DATA            *CallbackData
  );

/**
 This function update the "BootOrder" EFI Variable based on BMM Formset's NV map. It then refresh
 BootOptionMenu with the new "BootOrder" list.

 @param[in] CallbackData           The BMM context data.

 @retval EFI_SUCCESS           The function complete successfully.
 @retval EFI_OUT_OF_RESOURCES  Not enough memory to complete the function.
 @return not The EFI variable can not be saved. See gRT->SetVariable for detail return information.
**/
EFI_STATUS
Var_UpdateBootOrder (
  IN BMM_CALLBACK_DATA            *CallbackData
  );

/**
 This function update the "DriverOrder" EFI Variable based on
 BMM Formset's NV map. It then refresh DriverOptionMenu
 with the new "DriverOrder" list.

 @param[in] CallbackData    The BMM context data.

 @retval EFI_SUCCESS           The function complete successfully.
 @retval EFI_OUT_OF_RESOURCES  Not enough memory to complete the function.
 @return The EFI variable can not be saved. See gRT->SetVariable for detail return information.
**/
EFI_STATUS
Var_UpdateDriverOrder (
  IN BMM_CALLBACK_DATA            *CallbackData
  );

/**
 Update the legacy BBS boot option. VAR_LEGACY_DEV_ORDER and gEfiLegacyDevOrderVariableGuid EFI Variable
 is udpated with the new Legacy Boot order. The EFI Variable of "Boot####" and gEfiGlobalVariableGuid
 is also updated.

 @param[in] CallbackData    The context data for BMM.

 @return EFI_SUCCESS    The function completed successfully.
 @retval EFI_NOT_FOUND  If VAR_LEGACY_DEV_ORDER and gEfiLegacyDevOrderVariableGuid EFI Variable can not be found.
**/
EFI_STATUS
Var_UpdateBBSOption (
  IN BMM_CALLBACK_DATA            *CallbackData
  );

/**
 Update the Text Mode of Console.

 @param[in] CallbackData  The context data for BMM.

 @retval EFI_SUCCSS If the Text Mode of Console is updated.
 @return Other value if the Text Mode of Console is not updated.
**/
EFI_STATUS
Var_UpdateConMode (
  IN BMM_CALLBACK_DATA            *CallbackData
  );

//
// Following are page create and refresh functions
//
/**
 Refresh the global UpdateData structure.
**/
VOID
RefreshUpdateData (
  VOID
  );

/**
 Clean up the dynamic opcode at label and form specified by both LabelId.

 @param[in] LabelId         It is both the Form ID and Label ID for opcode deletion.
 @param[in] CallbackData    The BMM context data.
**/
VOID
CleanUpPage (
  IN UINT16                           LabelId,
  IN BMM_CALLBACK_DATA                *CallbackData
  );

/**
 Create a lit of boot option from global BootOptionMenu. It
 allow user to delete the boot option.

 @param[in] CallbackData    The BMM context data.
**/
VOID
UpdateBootDelPage (
  IN BMM_CALLBACK_DATA                *CallbackData
  );

/**
 Create a lit of driver option from global DriverMenu.

 @param[in] CallbackData    The BMM context data.
**/
VOID
UpdateDrvAddHandlePage (
  IN BMM_CALLBACK_DATA                *CallbackData
  );

/**
 Create a lit of driver option from global DriverOptionMenu. It
 allow user to delete the driver option.

 @param[in] CallbackData    The BMM context data.
**/
VOID
UpdateDrvDelPage (
  IN BMM_CALLBACK_DATA                *CallbackData
  );

/**
 Prepare the page to allow user to add description for a Driver Option.

 @param[in] CallbackData    The BMM context data.
**/
VOID
UpdateDriverAddHandleDescPage (
  IN BMM_CALLBACK_DATA                *CallbackData
  );

/**
 Dispatch the correct update page function to call based on the UpdatePageId.

 @param[in] UpdatePageId    The form ID.
 @param[in] CallbackData    The BMM context data.
**/
VOID
UpdatePageBody (
  IN UINT16                           UpdatePageId,
  IN BMM_CALLBACK_DATA                *CallbackData
  );

/**
 Create the dynamic page to allow user to set the "BootNext" vaule.

 @param[in] CallbackData    The BMM context data.
**/
VOID
UpdateBootNextPage (
  IN BMM_CALLBACK_DATA                *CallbackData
  );

/**
 Create the dynamic page to allow user to set the "TimeOut" vaule.

 @param[in] CallbackData    The BMM context data.
**/
VOID
UpdateTimeOutPage (
  IN BMM_CALLBACK_DATA                *CallbackData
  );

/**
 Create the dynamic page which allows user to set the property such as Baud Rate, Data Bits,
 Parity, Stop Bits, Terminal Type.

 @param[in] CallbackData    The BMM context data.
**/
VOID
UpdateTerminalPage (
  IN BMM_CALLBACK_DATA                *CallbackData
  );

/**
 Refresh the text mode page

 @param[in] CallbackData    The BMM context data.
**/
VOID
UpdateConModePage (
  IN BMM_CALLBACK_DATA                *CallbackData
  );

/**
 Create a list of Goto Opcode for all terminal devices logged
 by TerminaMenu. This list will be inserted to form FORM_CON_COM_SETUP_ID.

 @param[in] CallbackData    The BMM context data.
**/
VOID
UpdateConCOMPage (
  IN BMM_CALLBACK_DATA                *CallbackData
  );

/**
 Create a dynamic page so that Legacy Device boot order
 can be set for specified device type.

 @param[in] UpdatePageId    The form ID. It also spefies the legacy device type.
 @param[in] CallbackData    The BMM context data.
**/
VOID
UpdateSetLegacyDeviceOrderPage (
  IN UINT16                           UpdatePageId,
  IN BMM_CALLBACK_DATA                *CallbackData
  );


/**
 Create string tokens for a menu from its help strings and display strings


 @param[in] CallbackData    The BMM context data.
 @param[in] HiiHandle       Hii Handle of the package to be updated.
 @param[in] MenuOption      The Menu whose string tokens need to be created

 @retval  EFI_SUCCESS      string tokens created successfully
 @retval  others           contain some errors
**/
EFI_STATUS
CreateMenuStringToken (
  IN BMM_CALLBACK_DATA                *CallbackData,
  IN EFI_HII_HANDLE                   HiiHandle,
  IN BM_MENU_OPTION                   *MenuOption
  );

/**
 Get the index number (#### in Boot####) for the boot option pointed to a BBS legacy device type
 specified by DeviceType.

 @param[in]  DeviceType      The legacy device type. It can be floppy, network, harddisk, cdrom, etc.
 @param[out] OptionIndex     Returns the index number (#### in Boot####).
 @param[out] OptionSize      Return the size of the Boot### variable.
**/
VOID *
GetLegacyBootOptionVar (
  IN  UINTN                            DeviceType,
  OUT UINTN                            *OptionIndex,
  OUT UINTN                            *OptionSize
  );

/**
 Initialize the Boot Maintenance Utitliy.

 @retval  EFI_SUCCESS      utility ended successfully.
 @retval  others           contain some errors.
**/
EFI_STATUS
InitializeBM (
  VOID
  );

VOID
ShutdownBM (
  VOID
  );

/**
 Intialize all the string depositories.
**/
VOID
InitializeStringDepository (
  VOID
  );

/**
 Fetch a usable string node from the string depository and return the string token.

 @param[in] CallbackData       The BMM context data.
 @param[in] StringDepository   Pointer of the string depository.

 @retval  EFI_STRING_ID    String token.
**/
EFI_STRING_ID
GetStringTokenFromDepository (
  IN   BMM_CALLBACK_DATA     *CallbackData,
  IN   STRING_DEPOSITORY     *StringDepository
  );

/**
 Reclaim string depositories by moving the current node pointer to list head..
**/
VOID
ReclaimStringDepository (
  VOID
  );

/**
 Release resource for all the string depositories.
**/
VOID
CleanUpStringDepository (
  VOID
  );

/**
 Function handling request to apply changes for BMM pages.

 @param[in] Private            Pointer to callback data buffer.
 @param[in] CurrentFakeNVMap   Pointer to buffer holding data of various values used by BMM
 @param[in] FormId             ID of the form which has sent the request to apply change.

 @retval  EFI_SUCCESS      Change successfully applied.
 @retval  Other            Error occurs while trying to apply changes.
**/
EFI_STATUS
ApplyChangeHandler (
  IN  BMM_CALLBACK_DATA               *Private,
  IN  BMM_FAKE_NV_DATA                *CurrentFakeNVMap,
  IN  EFI_FORM_ID                     FormId
  );

/**
 Discard all changes done to the BMM pages such as Boot Order change,
 Driver order change.

 @param[in] Private          The BMM context data.
 @param[in] CurrentFakeNVMap The current Fack NV Map.
**/
VOID
DiscardChangeHandler (
  IN  BMM_CALLBACK_DATA               *Private,
  IN  BMM_FAKE_NV_DATA                *CurrentFakeNVMap
  );

/**
 Dispatch the display to the next page based on NewPageId.

 @param Private         The BMM context data.
 @param NewPageId       The original page ID.
**/
VOID
UpdatePageId (
  BMM_CALLBACK_DATA              *Private,
  UINT16                         NewPageId
  );

/**
 Boot a file selected by user at File Expoloer of BMM.

 @param[in] FileContext     The file context data, which contains the device path
                            of the file to be boot from.

 @retval EFI_SUCCESS    The function completed successfull.
 @retval                Other value if the boot from the file fails.
**/
EFI_STATUS
BootThisFile (
  IN BM_FILE_CONTEXT                   *FileContext
  );

/**
 Update the file explower page with the refershed file system.

 @param[in] CallbackData    BMM context data
 @param[in] KeyValue        Key value to identify the type of data to expect.

 @retval  TRUE           Inform the caller to create a callback packet to exit file explorer.
 @retval  FALSE          Indicate that there is no need to exit file explorer.
**/
BOOLEAN
UpdateFileExplorer (
  IN BMM_CALLBACK_DATA            *CallbackData,
  IN UINT16                       KeyValue
  );

/**
 This function processes the results of changes in configuration.
 When user select a interactive opcode, this callback will be triggered.
 Based on the Question(QuestionId) that triggers the callback, the corresponding
 actions is performed. It handles:

 1) the addition of boot option.
 2) the addition of driver option.
 3) exit from file browser
 4) update of file content if a dir is selected.
 5) boot the file if a file is selected in "boot from file"

 @param[in]  This            Points to the EFI_HII_CONFIG_ACCESS_PROTOCOL.
 @param[in]  Action          Specifies the type of action taken by the browser.
 @param[in]  QuestionId      A unique value which is sent to the original exporting driver
                             so that it can identify the type of data to expect.
 @param[in]  Type            The type of value for the question.
 @param[in]  Value           A pointer to the data being sent to the original exporting driver.
 @param[out] ActionRequest   On return, points to the action requested by the callback function.

 @retval EFI_SUCCESS           The callback successfully handled the action.
 @retval EFI_OUT_OF_RESOURCES  Not enough storage is available to hold the variable and its data.
 @retval EFI_DEVICE_ERROR      The variable could not be saved.
 @retval EFI_UNSUPPORTED       The specified Action is not supported by the callback.
**/
EFI_STATUS
EFIAPI
FileExplorerCallback (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  EFI_BROWSER_ACTION                     Action,
  IN  EFI_QUESTION_ID                        QuestionId,
  IN  UINT8                                  Type,
  IN  EFI_IFR_TYPE_VALUE                     *Value,
  OUT EFI_BROWSER_ACTION_REQUEST             *ActionRequest
  );

/**
 Function returns the value of the specified variable.

 @param[in] Name            A Null-terminated Unicode string that is
                            the name of the vendor's variable.
 @param[in] VendorGuid      A unique identifier for the vendor.

 @return               The payload of the variable.
 @return NULL          If the variable can't be read.
**/
VOID *
BootMaintGetVariable (
  IN CHAR16               *Name,
  IN EFI_GUID             *VendorGuid
  );

EFI_STATUS
EFIAPI
BootMaintLoadFile (
  IN EFI_LOAD_FILE_PROTOCOL           *This,
  IN EFI_DEVICE_PATH_PROTOCOL         *FilePath,
  IN BOOLEAN                          BootPolicy,
  IN OUT UINTN                        *BufferSize,
  IN VOID                             *Buffer OPTIONAL
  );

//
// Global variable in this program (defined in data.c)
//
extern BM_MENU_OPTION             BootOptionMenu;
extern BM_MENU_OPTION             DriverOptionMenu;
extern BM_MENU_OPTION             FsOptionMenu;
extern BM_MENU_OPTION             ConsoleInpMenu;
extern BM_MENU_OPTION             ConsoleOutMenu;
extern BM_MENU_OPTION             ConsoleErrMenu;
extern BM_MENU_OPTION             DirectoryMenu;
extern BM_MENU_OPTION             DriverMenu;
extern BM_MENU_OPTION             TerminalMenu;
extern BM_MENU_OPTION             LegacyFDMenu;
extern BM_MENU_OPTION             LegacyHDMenu;
extern BM_MENU_OPTION             LegacyCDMenu;
extern BM_MENU_OPTION             LegacyNETMenu;
extern BM_MENU_OPTION             LegacyBEVMenu;
extern UINT16                     TerminalType[];
extern COM_ATTR                   BaudRateList[19];
extern COM_ATTR                   DataBitsList[4];
extern COM_ATTR                   ParityList[5];
extern COM_ATTR                   StopBitsList[3];
extern EFI_GUID                   TerminalTypeGuid[4];
extern STRING_DEPOSITORY          *FileOptionStrDepository;
extern STRING_DEPOSITORY          *ConsoleOptionStrDepository;
extern STRING_DEPOSITORY          *BootOptionStrDepository;
extern STRING_DEPOSITORY          *BootOptionHelpStrDepository;
extern STRING_DEPOSITORY          *DriverOptionStrDepository;
extern STRING_DEPOSITORY          *DriverOptionHelpStrDepository;
extern STRING_DEPOSITORY          *TerminalStrDepository;
extern EFI_DEVICE_PATH_PROTOCOL   EndDevicePath[];
extern UINT16                     mFlowControlType[2];
extern UINT32                     mFlowControlValue[2];
extern CHAR16                     mFileExplorerStorageName[];
//
// Shared IFR form update data
//
extern VOID                        *mStartOpCodeHandle;
extern VOID                        *mEndOpCodeHandle;
extern EFI_IFR_GUID_LABEL          *mStartLabel;
extern EFI_IFR_GUID_LABEL          *mEndLabel;
extern BOOLEAN                     mBootMaintSendFormBySelf;

#endif

