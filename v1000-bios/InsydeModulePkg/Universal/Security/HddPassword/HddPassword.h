/** @file
  EFI HDD Password Protocol

;******************************************************************************
;* Copyright (c) 2024, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _HDD_PASSWORD_H_
#define _HDD_PASSWORD_H_

#include <PiSmm.h>

#include <PortNumberMap.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/DevicePathLib.h>
#include <Library/DxeChipsetSvcLib.h>
#include <Library/DxeServicesTableLib.h>
#include <Library/HiiLib.h>
#include <Library/HobLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/OemGraphicsLib.h>
#include <Library/PciLib.h>
#include <Library/PostCodeLib.h>
#include <Library/PrintLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiHiiServicesLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/VariableLib.h>
#include <Library/DxeOemSvcKernelLib.h>
#include <Library/SmmOemSvcKernelLib.h>
#include <Library/BdsCpLib.h>

#include <Library/TcgStorageOpalLib.h>
#include <Library/TcgStorageOpalLibEx.h>
#include <Library/Tcg2PhysicalPresenceLib.h>
#include <Library/DxeServicesTableLib.h>
#include <Library/SavedPasswordStringProcessLib.h>

#include <Protocol/AcpiRestoreCallbackDone.h>
#include <Protocol/AdapterInformation.h>
#include <Protocol/AtaPassThru.h>
#include <Protocol/ComponentName2.h>
#include <Protocol/DevicePath.h>
#include <Protocol/DiskInfo.h>
#include <Protocol/HddPasswordService.h>
#include <Protocol/LegacyBios.h>
#include <Protocol/LoadedImage.h>
#include <Protocol/Metronome.h>
#include <Protocol/H2ODialog.h>
#include <Protocol/PciIo.h>
#include <Protocol/SmmBase2.h>
#include <Protocol/SimpleTextIn.h>
#include <Protocol/SimpleTextOut.h>
#include <Protocol/UnicodeCollation.h>
#include <Protocol/BlockIo.h>
#include <Protocol/StorageSecurityCommand.h>
#include <Protocol/SmmCommunication.h>

#include <Guid/HddPasswordVariable.h>
#include <Guid/HobList.h>
#include <Guid/MdeModuleHii.h>
#include <Guid/AdmiSecureBoot.h>
#include <Guid/DebugMask.h>

#include <Guid/H2OBdsCheckPoint.h>

#include <IndustryStandard/Pci.h>
#include <IndustryStandard/Atapi.h>
#include <IndustryStandard/TcgStorageCore.h>
#include <IndustryStandard/Nvme.h>

#include <Uefi/UefiInternalFormRepresentation.h>
#include <Uefi/UefiBaseType.h>

#include <MmioAccess.h>

#include <H2OStorageSecurityCommand.h>

#include "HddPasswordStrDefs.h"

#define ATA_SECURITY_UNSUPPORTED_COMMAND      0xFFFF
#define ATA_SECURITY_SET_PASSWORD      1
#define ATA_SECURITY_UNLOCK            2
#define ATA_SECURITY_ERASE_PREPARE     3
#define ATA_SECURITY_ERASE             4
#define ATA_SECURITY_FREEZE_LOCK       5
#define ATA_SECURITY_DISABLE_PASSWORD  6
#define CONTROLLER_INDEX_START         1


#define TRUSTED_COMMAND_TIMEOUT_NS      ((UINT64) 5 * ((UINT64)(1000000)) * 1000) // 5 seconds
#define EBDA(a)                           (*(UINT8*)(UINTN)(((*(UINT16*)(UINTN)0x40e) << 4) + (a)))
#define EBDA_HDD_LOCKED_FLAG              0x3d9
#define BIT(a)                            (1 << (a))
#define ATAPI_DEVICE_BIT                  BIT15

#define MAXPASSWORD_LENGTH_SUPPORTED_FOR_TCG_SECURITY 0x20
#define INVALID_SATA_PORT_NUMBER                      0xFFFF
#define INVALID_CONTROLLER_INDEX_NUMBER               0xFFFF
//
//  Status Reg
//
#define PCI_CLASS_CODE                    0x09
#define PCI_AHCI_BAR                      0x24
#define PCI_CLASS_MASS_STORAGE            0x01
#define PCI_SUB_CLASS_IDE                 0x01
#define PCI_SUB_CLASS_AHCI                0x06
#define PCI_SUB_CLASS_RAID                0x04
#define PCI_CLASSC_PI_AHCI                0x01
#define PCI_CLASSC_PI_RAID                0x00
#define PCI_SUB_CLASS_NVME                0x08
#define PCI_IF_NVMHCI                     0x02
#define PCI_IF_AHCI                       0x01
#define PCI_IF_RAID                       0x00
#define PCI_IF_SD                         0x01
#define PCI_CLASS_SD_HC                   0x08
#define PCI_SUB_CLASS_SDHC                0x05

#define PCI_AHCI_BAR                      0x24

#define HBA_PORTS_START                   0x0100
#define HBA_PORTS_REG_WIDTH               0x0080
#define HBA_PORTS_SCTL                    0x002C
#define   HBA_PORTS_SCTL_DET_COMRESET     0x01
#define   HBA_PORTS_SCTL_DET_OFFLINE      0x04


#define MAX_HDD_PASSWORD_LENGTH           32
#define MODEL_NUMBER_LENGTH               40
#define SUPPORT_SATA_PORTS_NUM             8

#define HDD_TITLE_STRING_LENGTH           (MODEL_NUMBER_LENGTH + 30)

//
// Hot Key for skipping dialog
//
#define DEFAULT_SKIP_DIALOG_KEY_INDEX     2
#define DEFAULT_SKIP_DIALOG_KEY_SCANCODE  SCAN_ESC
#define DEFAULT_SKIP_DIALOG_KEY_UNICHAR   CHAR_NULL

//
// Character definitions
//
#define CHAR_SPACE                        0x0020

//
//  Option Icon
//
#define OPTION_ICON_MAX_STR_SIZE          80

//
//  Memory Alignment
//
#define ALIGN_SIZEOF_UINTN(a)             ((((a) - 1) | (sizeof (UINTN) - 1)) + 1)

//
//  Status flag definition of HddDataInfo
//
#define DEFAULT_HDD_PASSWORD_MAX_NUMBER   HDD_PASSWORD_MAX_NUMBER

EFI_HII_HANDLE                            gStringPackHandle;

EFI_HII_DATABASE_PROTOCOL                 *HiiDatabase;
EFI_HII_STRING_PROTOCOL                   *IfrLibHiiString;


#define STR_TOKEN_NUMBERS                  10

//
//  TCG related definition
//
#define SID_LENGTH                        0x20
#define OPAL_MSID_LENGHT                  128
typedef struct {
  UINT8                         Byte0;
  //
  // Bit0:
  // Master password Flag
  //
  //
  UINT8                         Byte1;
  UINT8                         Password[32];
  UINT8                         MasterPasswordIdentifier[2];
} ATA_DEVICE_SERVER_SEND_CMD_DATA;



//
// Structure for security protocol
//
typedef struct {
  UINT8                         Byte0Reserved;
  UINT8                         ParameterLength;
  UINT16                        SecurityEraseTime;
  UINT16                        EnhancedSecurityEraseTime;
  UINT16                        MasterPasswordIdentifier;
  UINT8                         Byte8;
  UINT8                         Byte9;
  UINT8                         Reserved[6];
} ATA_DEVICE_SERVER_SECURITY_STATE_DATA;


typedef enum _TCG_PASSWORD_TYPE {
  SID = 0,
  ADMIN1,
  USER1,
  USER2,
  MAX_TYPE
} TCG_PASSWORD_TYPE;

typedef struct _HDD_SECURITY_COMMAND_PACKET {
  UINT16                                  SecurityCmdType;
  UINT8                                   HddPassword[32];
  UINT16                                  MasterPasswordIdentifier;
  UINT16                                  Reserved[238];
} HDD_SECURITY_COMMAND_PACKET;

typedef struct {
  UINT8                                   PI;
  UINT8                                   SubClassCode;
  UINT8                                   BaseCode;
} SATA_CLASS_CODE;

typedef struct {
  EFI_HANDLE                              RaidController;
  EFI_PCI_IO_PROTOCOL                     *PciIo;
  UINT32                                  *AhciAbarIoSave;
  UINTN                                   NumOfBackupPorts;
  UINTN                                   BackupPortStart;
  UINT8                                   DiskStart;
  UINT8                                   DiskEnd;
} RAID_SETUP_INFO;

typedef struct {
  CHAR16                                  **StrTokenArray;
} STR_TOKEN_INFO;

typedef struct {
  LIST_ENTRY                              Link;
  EFI_HANDLE                              AtaControllerHandle;
  UINT8                                   AtaMode;
  UINTN                                   PciSeg;
  UINTN                                   PciBus;
  UINTN                                   PciDevice;
  UINTN                                   PciFunction;
  UINT8                                   PI;
  UINT8                                   SubClassCode;
  UINT8                                   BaseCode;
  RAID_SETUP_INFO                         RaidSetupInfo;
} ATA_CONTROLLER_INFO;

typedef struct {
  EFI_HANDLE                              DiskInfoHandle;
  EFI_DISK_INFO_PROTOCOL                  *DiskInfo;
  ATA_IDENTIFY_DATA                       IdentifyData;
} DISKINFO_DATA;

typedef struct {
  EFI_GUID                                FileGuid;
}AGENT_FILE_GUID_LIST;

typedef struct {
  UINT8                                   BaseClassCode;
  UINT8                                   SubClassCode;
  UINT8                                   Interface;
}CONTROLER_CLASS_CODE_LIST;

typedef struct {
  UINT8                                   NumOfController;
  BOOLEAN                                 AtaControllerSearched;
  STR_TOKEN_INFO                          StrTokenInfo;
  EFI_LEGACY_BIOS_PROTOCOL                *LegacyBios;
  LIST_ENTRY                              AtaControllerInfoListHead;
} DRIVER_INSTALL_INFO;

typedef struct {
  UINT8                                   NumOfController;
  BOOLEAN                                 SmmMode;
} HDD_PASSWORD_S3_DATA;

//
// Structure that is used to represent an OPAL_DISK.
//
typedef struct {
  UINT32                                          MsidLength;             // Byte length of MSID Pin for device
  UINT8                                           Msid[OPAL_MSID_LENGHT]; // MSID Pin for device
  UINT32                                          MediaId;                // MediaId is used by Ssc Protocol.
  EFI_DEVICE_PATH_PROTOCOL                        *OpalDevicePath;
  UINT16                                          OpalBaseComId;          // Opal SSC 1 base com id.
  OPAL_OWNER_SHIP                                 Owner;
  OPAL_DISK_SUPPORT_ATTRIBUTE                     SupportedAttributes;
  TCG_LOCKING_FEATURE_DESCRIPTOR                  LockingFeature;         // Locking Feature Descriptor retrieved from performing a Level 0 Discovery
  TCG_BLOCK_SID_FEATURE_DESCRIPTOR                BlockSidFeature;
  UINT32                                          EstimateTimeCost;
} OPAL_DISK_INFO;

typedef struct {
  UINTN                                   Signature;
  LIST_ENTRY                              Link;
  UINT32                                  BlockIoMediaId;
  EFI_HANDLE                              DeviceHandleInDxe;
  EFI_HANDLE                              DeviceHandleInSmm;
  EFI_STORAGE_SECURITY_COMMAND_PROTOCOL   *StorageSecurityCommandInDxe;
  EFI_STORAGE_SECURITY_COMMAND_PROTOCOL   *StorageSecurityCommandInSmm;
  HDD_PASSWORD_HDD_INFO                   HddInfo;
  OPAL_DISK_INFO                          OpalDisk;
  UINT8                                   CachedPassword[HDD_PASSWORD_MAX_NUMBER];
  UINTN                                   CachedPasswordLength;
  EFI_DEVICE_PATH_PROTOCOL                *DevicePath;
  BOOLEAN                                 InstalledByInsyde;
} HDD_PASSWORD_HDD_INFO_PRIVATE;

#define HDD_PASSWORD_HDD_INFO_PRIVATE_SIGNATURE SIGNATURE_32 ('h', 'd', 'i', 'p')
#define GET_HDD_INFO_PRIVATE_FROM_LINK(a)  CR (a, HDD_PASSWORD_HDD_INFO_PRIVATE, Link, HDD_PASSWORD_HDD_INFO_PRIVATE_SIGNATURE)

typedef struct {
  UINTN                                   NumOfHdd;
  LIST_ENTRY                              HddInfoPrivateListHead;
} HDD_INFO_MANAGER;

typedef struct {
  UINTN                                   Signature;
  EFI_HDD_PASSWORD_SERVICE_PROTOCOL       HddPasswordProtocol;
  HDD_INFO_MANAGER                        *HddInfoManager;
  HDD_PASSWORD_S3_DATA                    HddPasswordS3Data;
  BOOLEAN                                 UefiOsFastBootSupported;
  BOOLEAN                                 ControllerReconnected;
} HDD_PASSWORD_PRIVATE;

#define HDD_PASSWORD_SIGNATURE            SIGNATURE_32 ('h', 'd', 'p', 'w')
#define GET_PRIVATE_FROM_HDD_PASSWORD(a)  CR (a, HDD_PASSWORD_PRIVATE, HddPasswordProtocol, HDD_PASSWORD_SIGNATURE)


typedef struct {
  UINTN                                   Signature;
  EFI_HDD_PASSWORD_DIALOG_PROTOCOL        HddPasswordDialogProtocol;
  EFI_HDD_PASSWORD_SERVICE_PROTOCOL       *HddPasswordService;
  H2O_DIALOG_PROTOCOL                     *H2oDialogProtocol;
  BOOLEAN                                 PostUnlocked;
} HDD_PASSWORD_POST_DIALOG_PRIVATE;



#define HDD_PASSWORD_POST_DIALOG_SIGNATURE SIGNATURE_32 ('h', 'd', 'p', 'd')
#define GET_PRIVATE_FROM_HDD_PASSWORD_POST_DIALOG(a)  CR (a, HDD_PASSWORD_POST_DIALOG_PRIVATE, HddPasswordDialogProtocol, HDD_PASSWORD_POST_DIALOG_SIGNATURE)

//
// SMM communicate header
//
typedef struct {
  UINTN                         Function;
  EFI_STATUS                    ReturnStatus;
  UINT8                         Data[1];
} HDD_PASSWORD_SMM_PARAMETER_HEADER;

//
// SMM function
//
#define HDD_PASSWORD_ACPI_RESTORE_CALLBACK_NOTIFY_INSTANCE  2
#define HDD_PASSWORD_SMM_COLLECT_DEVICE_INSTANCE            3
#define HDD_PASSWORD_SMM_BLOCK_SID_INSTANCE                 4

//
// Size of SMM communicate header, without including the payload.
//
#define SMM_COMMUNICATE_HEADER_SIZE  (OFFSET_OF (EFI_SMM_COMMUNICATE_HEADER, Data))

//
// Definitions for Memory Management
//
#define PAGE_FREE                0x00000000
#define PAGE_USED                0x00000001

#define MIN_REQUIRED_BLOCKS      6512
#define PAGE_BLOCK_SIZE          16
#define MEMORY_PREALLOCATE_SIZE  (MIN_REQUIRED_BLOCKS * PAGE_BLOCK_SIZE)

#define PAGE_SIZE                0x10
#define PAGE_MASK                0x0F
#define PAGE_SHIFT               4

#define SIZE_TO_PAGES(a)         (((a) >> PAGE_SHIFT) + (((a) & PAGE_MASK) ? 1 : 0))
#define PAGES_TO_SIZE(a)         ((a) << PAGE_SHIFT)

typedef struct {
  UINTN                 StartPageOffset;      // Offset of the starting page allocated.
                                              // Only available for USED pages.
  UINT8                 PageFlag;             // Page Attributes.
} MEMORY_PAGE_ENTRY;

typedef struct {
  VOID                  *CommBuffPtr;
  UINTN                 PageCount;
  UINTN                 LastEmptyPageOffset;
  UINT8                 *DataAreaBase;         // Pointer to data Area.
  MEMORY_PAGE_ENTRY     Pages[1];              // Page Table Entries. Please don't add anything behind it
} MEMORY_PAGE_TABLE;

CHAR16 *
GetStringById (
  IN EFI_STRING_ID                           Id
  );

EFI_STATUS
InitializeStringSupport  (
  VOID
  );

EFI_STATUS
InitDialogStringTokenArray (
  VOID
  );


EFI_STATUS
ReleaseStringTokenArray (
  VOID
  );



EFI_STATUS
GetModelNumber (
  IN VOID                                 *DescBufferPtr,
  IN VOID                                 *SourceBufferPtr
  );

EFI_STATUS
ChangeChannelDevice2PortNum (
  IN UINT32                               Bus,
  IN UINT32                               Device,
  IN UINT32                               Function,
  IN UINT8                                PrimarySecondary,
  IN UINT8                                SlaveMaster,
  OUT UINTN                               *PortNum
  );

VOID
ProcessLegacyRaidSupport (
  VOID
  );

EFI_STATUS
InitLegacyRaidSupport (
  VOID
  );


BOOLEAN
IsOnBoardPciDevice (
  IN UINT32                               Bus,
  IN UINT32                               Device,
  IN UINT32                               Function
  );

EFI_STATUS
GetMem (
  IN  UINTN                               RequestSize,
  IN  OUT  VOID                           **AllocAddr
  );

EFI_STATUS
EFIAPI
CollectStorageController (
  IN BOOLEAN                             NeedConnectController
  );

UINT16
EFIAPI
FindMappedController (
  IN UINTN                               Seg,
  IN UINTN                               Bus,
  IN UINTN                               Device,
  IN UINTN                               Function
  );

BOOLEAN
IsValidComponentNameProtocol (
  IN  EFI_COMPONENT_NAME_PROTOCOL    *ComponentName,
  IN  EFI_HANDLE                     Handle
  );

EFI_STATUS
HddUnlockDialogInit (
  IN EFI_HDD_PASSWORD_SERVICE_PROTOCOL  *HddPasswordService
  );



extern HDD_PASSWORD_PRIVATE             *mHddPasswordPrivate;
extern DRIVER_INSTALL_INFO              *mDriverInstallInfo;
extern MEMORY_PAGE_TABLE                *mMemoryBuffer;
extern DRIVER_INSTALL_INFO              *mDriverInstallInfo;
extern VOID                             *mCommBuffer;

/**
  Returns the next entry of the HDD password table.

  @param[in]        HddPasswordTable         Pointer to HddPasswordTable.

  @retval           The pointer to the next table.

**/
HDD_PASSWORD_TABLE *
GetNextTableEntry (
  IN  HDD_PASSWORD_TABLE                *HddPasswordTablePtr
  );

/**
 Find all agent handles that support HDD Password feature.
 @param[in]        AgentFileGuidList         Pointer to AGENT_FILE_GUID_LIST.
 @param[in]        size                      size of the AGENT_FILE_GUID_LIST.

 @retval *EFI_HANDLE  A list of agent handles that discovered from file Guids
**/
EFI_HANDLE*
FindAgentHandleListByFileGUID (
IN AGENT_FILE_GUID_LIST*   AgentFileGuidList,
IN UINTN                   Size
  );

/**
  Initializes pre-allocated memory.

  @param[out]  MemoryPageTable      Pointer to memory page table.

  @retval EFI_SUCCESS  Successful initialization.
  @retval Others       Initialization failed.

**/
EFI_STATUS
InitializeMemoryBuffer (
  OUT MEMORY_PAGE_TABLE     **MemoryPageTable
  );

/**
  Look-up Free memory Region for object allocation.

  @param[in]  AllocationSize  Bytes to be allocated.

  @return  Return available page offset for object allocation.

**/
UINTN
LookupFreeMemRegion (
  IN  UINTN  AllocationSize
  );

/**
  Allocates a buffer at runtime phase.

  @param[in]  AllocationSize    Bytes to be allocated.

  @return  A pointer to the allocated buffer or NULL if allocation fails.

**/
VOID *
AllocateBuffer (
  IN  UINTN  AllocationSize
  );

/**
  Frees a buffer that was previously allocated at runtime phase.

  @param[in]  Buffer  Pointer to the buffer to free.

**/
VOID
FreeBuffer (
  IN  VOID  *Buffer
  );

/**
  Initialize the communicate buffer using DataSize and Function.

  @param[out]      DataPtr          Points to the data in the communicate buffer.
  @param[in]       DataSize         The data size to send to SMM.
  @param[in]       Function         The function number to initialize the communicate header.

  @retval The communicate buffer. Caller should free it after use.

**/
VOID *
InitCommunicateBuffer (
     OUT  VOID                              **DataPtr OPTIONAL,
  IN      UINTN                             DataSize,
  IN      UINTN                             Function
  );

/**
  Creates a new copy of an existing device path.

  @param[in]  DevicePath    A pointer to a device path data structure.

  @retval NULL          DevicePath is NULL or invalid.
  @retval Others        A pointer to the duplicated device path.

**/
EFI_DEVICE_PATH_PROTOCOL *
HddPasswordDuplicateDevicePath (
  IN CONST EFI_DEVICE_PATH_PROTOCOL  *DevicePath
  );

/**
 Find mapped controller.

 @retval Others        found.
 @retval 0xFFFF        not found.
**/
UINT16
EFIAPI
FindMappedController (
  IN UINTN                                 Seg,
  IN UINTN                                 Bus,
  IN UINTN                                 Device,
  IN UINTN                                 Function
  );

#endif

