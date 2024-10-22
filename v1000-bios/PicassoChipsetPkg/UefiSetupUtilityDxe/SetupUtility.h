/** @file

;******************************************************************************
;* Copyright (c) 2015 - 2017, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _UEFI_SETUP_UTILITY_H_
#define _UEFI_SETUP_UTILITY_H_

#include <Uefi.h>
#include <PiPei.h>
#include <ChipsetSetupConfig.h>

#include <Protocol/DataHub.h>
#include <Protocol/HiiConfigAccess.h>
#include <Protocol/IdeControllerInit.h>
#include <Protocol/FormBrowser2.h>
#include <Protocol/H2ODialog.h>
#include <Protocol/SetupUtilityBrowser.h>
#include <Protocol/AlertStandardFormat.h>
#include <Protocol/IdeControllerInit.h>
#include <Protocol/DiskInfo.h>

#include <Ppi/GenericMemoryController.h>

#include <Library/BaseMemoryLib.h>
#include <Library/CmosLib.h>
#include <Library/DebugLib.h>
#include <Library/HobLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/OemGraphicsLib.h>
#include <Library/SetupUtilityLib.h>
#include <Library/PciLib.h>
#include <Library/PciExpressLib.h>
#include <Library/IoLib.h>
#include <Library/PostCodeLib.h>
#include <Library/VariableLib.h>

#include <Guid/DataHubRecords.h>
#include <Guid/HddPswdModified.h>
#include <Guid/GlobalVariable.h>
#include <Guid/SetupDefaultHob.h>
#include <Guid/SystemPasswordVariable.h>
#include <Guid/DebugMask.h>
#include <Guid/HobList.h>
#include <Guid/TpmInstance.h>
#include <Protocol/SetupUtilityApplication.h>
#include <Protocol/NvmExpressPassthru.h>

//
// This is the generated header file which includes whatever needs to be exported (strings + IFR)
//
#include "SetupUtilityStrDefs.h"

#include <PostCode.h>
#include "SetupFuncs.h"
#include "SioConflict.h"
#include "Advance.h"
#include "Power.h"
// #include "Version.h"
#include <ExtCmosUsage.h>
#include <Fch.h>
#include <YangtzeReg.h>

#define bit(a)   (1 << (a))
#define HDD_SECURITY_SUPPORT bit(0)
#define HDD_SECURITY_ENABLE  bit(1)
#define HDD_SECURITY_LOCK    bit(2)
#define HDD_SECURITY_FROZEN  bit(3)
//
// Global externs
//
extern UINT8 SetupUtilityStrings[];
extern UINT32 SetupUtilityStringsTotalSize; 
extern UINT32 SetupUtilityLibStringsTotalSize;
extern UINT8 AdvanceVfrBin[];
extern UINT8 PowerVfrBin[];
extern UINT8 VersionVfrBin[];
extern UINT8 AdvanceVfrSystemConfigDefault0000[];
extern UINT8 PowerVfrSystemConfigDefault0000[];
extern EFI_GUID  mFormSetGuid;
extern CHAR16    mVariableName[];
extern EFI_HII_HANDLE mDriverHiiHandle;

#define EFI_SETUP_UTILITY_SIGNATURE SIGNATURE_32('S','e','t','u')

#define STRING_PACK_GUID  { 0x8160a85f, 0x934d, 0x468b, 0xa2, 0x35, 0x72, 0x89, 0x59, 0x14, 0xf6, 0xfc }
#define HAVE_CREATE_SYSTEM_HEALTH_EVENT   0xCF8F

extern UINT16   gSaveItemMapping[][2];

#define CUSTOM_BOOT_ORDER_SIZE              256
#define TIMEOUT_OF_EVENT                5000000

extern UINT16                          gSCUSystemHealth;
extern EFI_EVENT                       gSCUTimerEvent;

//
// Driver private data
//

#define DIMM0 0
#define DIMM1 4

typedef struct _EFI_MEMORY_CONFIG_DATA {
  PEI_DUAL_CHANNEL_DDR_ROW_INFO      RowInfo;
  PEI_DUAL_CHANNEL_DDR_TIMING_DATA   TimingData;
  PEI_DUAL_CHANNEL_DDR_ROW_CONFIG    RowConfArray;
} EFI_MEMORY_CONFIG_DATA;

typedef struct {
  UINTN                                 Signature;
  EFI_HANDLE                            Handle;
  EFI_SETUP_UTILITY_PROTOCOL            SetupUtility;
} SETUP_UTILITY_DATA;

typedef struct _NEW_PACKAGE_INFO {
  USER_INSTALL_CALLBACK_ROUTINE         CallbackRoutine;
  UINT8                                 *IfrPack;
  UINT8                                 *StringPack;
  UINT8                                 *ImagePack;
} NEW_PACKAGE_INFO;

#define EFI_SETUP_UTILITY_FROM_THIS(a) CR (a, SETUP_UTILITY_DATA, SetupUtility, EFI_SETUP_UTILITY_SIGNATURE)

EFI_STATUS
PowerOnSecurity (
  IN  EFI_SETUP_UTILITY_PROTOCOL        *SetupUtility
  );

EFI_STATUS
CheckIde (
  IN EFI_HII_HANDLE                        HiiHandle,
  IN CHIPSET_CONFIGURATION                 *SetupVariable,
  IN BOOLEAN                               UpdateIde
  );

BOOLEAN
AsfResetSetupData (
  IN  VOID
  );

EFI_STATUS
InstallHiiData (
  VOID
);

EFI_STATUS
RemoveHiiData (
  IN VOID     *StringPack,
  ...
);

EFI_STATUS
GetSystemConfigurationVar (
  IN SETUP_UTILITY_DATA                     *SetupData
  );

EFI_STATUS
InstallSetupUtilityBrowserProtocol (
  IN  EFI_SETUP_UTILITY_PROTOCOL            *This
  );

EFI_STATUS
UninstallSetupUtilityBrowserProtocol (
  VOID
  );

EFI_STATUS
HotKeyCallBack (
  IN CONST  EFI_HII_CONFIG_ACCESS_PROTOCOL  *This,
  IN  EFI_BROWSER_ACTION                    Action,
  IN  EFI_QUESTION_ID                       QuestionId,
  IN  UINT8                                 Type,
  IN  EFI_IFR_TYPE_VALUE                    *Value,
  OUT EFI_BROWSER_ACTION_REQUEST            *ActionRequest
  );

extern SETUP_UTILITY_BROWSER_DATA                *gSUBrowser;

EFI_STATUS
EFIAPI
GenericExtractConfig (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  CONST EFI_STRING                       Request,
  OUT EFI_STRING                             *Progress,
  OUT EFI_STRING                             *Results
  );

EFI_STATUS
EFIAPI
GenericRouteConfig (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  CONST EFI_STRING                       Configuration,
  OUT EFI_STRING                             *Progress
  );

EFI_STATUS
UpdateTransferMode (
  IN  EFI_HII_HANDLE                    HiiHandle,
  IN  EFI_ATAPI_IDENTIFY_DATA           *IdentifyInfo,
  IN  EFI_STRING_ID                     TokenToUpdate,
  IN  CHAR8                             *LanguageString
  );

VOID
EFIAPI
SetupUtilityNotifyFn (
  IN EFI_EVENT                             Event,
  IN VOID                                  *Context
  );

EFI_STATUS
UpdateMemoryInfo (
  IN  SETUP_UTILITY_BROWSER_DATA            *SUBrowser
  );

#pragma pack(1)

typedef struct {
  UINT16 Mp;                /* Maximum Power */
  UINT8  Rsvd1;             /* Reserved as of Nvm Express 1.1 Spec */
  UINT8  Mps:1;             /* Max Power Scale */
  UINT8  Nops:1;            /* Non-Operational State */
  UINT8  Rsvd2:6;           /* Reserved as of Nvm Express 1.1 Spec */
  UINT32 Enlat;             /* Entry Latency */
  UINT32 Exlat;             /* Exit Latency */
  UINT8  Rrt:5;             /* Relative Read Throughput */
  UINT8  Rsvd3:3;           /* Reserved as of Nvm Express 1.1 Spec */
  UINT8  Rrl:5;             /* Relative Read Leatency */
  UINT8  Rsvd4:3;           /* Reserved as of Nvm Express 1.1 Spec */
  UINT8  Rwt:5;             /* Relative Write Throughput */
  UINT8  Rsvd5:3;           /* Reserved as of Nvm Express 1.1 Spec */
  UINT8  Rwl:5;             /* Relative Write Leatency */
  UINT8  Rsvd6:3;           /* Reserved as of Nvm Express 1.1 Spec */
  UINT8  Rsvd7[16];         /* Reserved as of Nvm Express 1.1 Spec */
} NVME_PSDESCRIPTOR;

//
//  Identify Controller Data
//
typedef struct {
  //
  // Controller Capabilities and Features 0-255
  //
  UINT16 Vid;                 /* PCI Vendor ID */
  UINT16 Ssvid;               /* PCI sub-system vendor ID */
  UINT8  Sn[20];              /* Product serial number */

  UINT8  Mn[40];              /* Proeduct model number */
  UINT8  Fr[8];               /* Firmware Revision */
  UINT8  Rab;                 /* Recommended Arbitration Burst */
  UINT8  Ieee_oui[3];         /* Organization Unique Identifier */
  UINT8  Cmic;                /* Multi-interface Capabilities */
  UINT8  Mdts;                /* Maximum Data Transfer Size */
  UINT8  Cntlid[2];           /* Controller ID */
  UINT8  Rsvd1[176];          /* Reserved as of Nvm Express 1.1 Spec */
  //
  // Admin Command Set Attributes
  //
  UINT16 Oacs;                /* Optional Admin Command Support */
  UINT8  Acl;                 /* Abort Command Limit */
  UINT8  Aerl;                /* Async Event Request Limit */
  UINT8  Frmw;                /* Firmware updates */
  UINT8  Lpa;                 /* Log Page Attributes */
  UINT8  Elpe;                /* Error Log Page Entries */
  UINT8  Npss;                /* Number of Power States Support */
  UINT8  Avscc;               /* Admin Vendor Specific Command Configuration */
  UINT8  Apsta;               /* Autonomous Power State Transition Attributes */
  UINT8  Rsvd2[246];          /* Reserved as of Nvm Express 1.1 Spec */
  //
  // NVM Command Set Attributes
  //
  UINT8  Sqes;                /* Submission Queue Entry Size */
  UINT8  Cqes;                /* Completion Queue Entry Size */
  UINT16 Rsvd3;               /* Reserved as of Nvm Express 1.1 Spec */
  UINT32 Nn;                  /* Number of Namespaces */
  UINT16 Oncs;                /* Optional NVM Command Support */
  UINT16 Fuses;               /* Fused Operation Support */
  UINT8  Fna;                 /* Format NVM Attributes */
  UINT8  Vwc;                 /* Volatile Write Cache */
  UINT16 Awun;                /* Atomic Write Unit Normal */
  UINT16 Awupf;               /* Atomic Write Unit Power Fail */
  UINT8  Nvscc;               /* NVM Vendor Specific Command Configuration */
  UINT8  Rsvd4;               /* Reserved as of Nvm Express 1.1 Spec */
  UINT16 Acwu;                /* Atomic Compare & Write Unit */
  UINT16 Rsvd5;               /* Reserved as of Nvm Express 1.1 Spec */
  UINT32 Sgls;                /* SGL Support  */
  UINT8  Rsvd6[164];          /* Reserved as of Nvm Express 1.1 Spec */
  //
  // I/O Command set Attributes
  //
  UINT8 Rsvd7[1344];          /* Reserved as of Nvm Express 1.1 Spec */
  //
  // Power State Descriptors
  //
  NVME_PSDESCRIPTOR PsDescriptor[32];

  UINT8  VendorData[1024];    /* Vendor specific data */
} NVME_ADMIN_CONTROLLER_DATA;

#pragma pack()

#endif

