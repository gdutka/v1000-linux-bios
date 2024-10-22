/** @file
  Kernel Setup Configuration Definitions

;******************************************************************************
;* Copyright (c) 2012 - 2019, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _KERNEL_SETUP_CONFIG_H_
#define _KERNEL_SETUP_CONFIG_H_

#include <Guid/H2OSetup.h>

//
// PSID Length
//
#define PSID_CHARACTER_LENGTH            0x20
#define PSID_CHARACTER_STRING_END_LENGTH 0x21


//
// Setup varaiable offset definition
//
#define SETUP_VAR_OFFSET(Field)              ((UINT16)((UINTN)&(((KERNEL_CONFIGURATION *)0)->Field)))


//
// definition for dynamic create form which need question ID
//
#define SETUP_DYNAMIC_QUESTION_ID(Field)     (SETUP_VAR_OFFSET(Field)+SETUP_DYNAMIC_CREATE_KEY_VALUE_BASE)

#pragma pack(1)
//
//  Kernel Setup Configuration Structure
//
typedef struct {
  //
  // Import kernel setup configuration definition from KernelSetupData.h
  //
  #define _IMPORT_KERNEL_SETUP_
  #include <KernelSetupData.h>
  #undef _IMPORT_KERNEL_SETUP_

} KERNEL_CONFIGURATION;

//
//  Password Configuration Structure
//
typedef struct {
  UINT8     HddPasswordSupport;


  UINT8     SelectedScuDataIndex;
  UINT8     StoragePasswordCallbackState;

  UINT8     SetHddPasswordFlag;
  UINT8     UnlockHddPasswordFlag;
  UINT8     MasterHddPasswordFlag;
  UINT8     UseMasterPassword;

  UINT8     SetAllHddPasswordFlag;
  UINT8     SetAllMasterHddPasswordFlag;

  UINT8     TcgStorageSecuritySupported;
  UINT16    PsidString[PSID_CHARACTER_STRING_END_LENGTH];
  UINT32    TcgStorageAction;
  UINT8     BlockSidEnabled;
  UINT8     PpRequiredForEnableBlockSid;
  UINT8     PpRequiredForDisableBlockSid;
} PASSWORD_CONFIGURATION;

//
//  TCG2 Configuration Info Structure
//
typedef struct {
  UINT8  Sha1Supported;
  UINT8  Sha256Supported;
  UINT8  Sha384Supported;
  UINT8  Sha512Supported;
  UINT8  Sm3Supported;
  UINT8  Sha1Activated;
  UINT8  Sha256Activated;
  UINT8  Sha384Activated;
  UINT8  Sha512Activated;
  UINT8  Sm3Activated;
} TCG2_CONFIGURATION_INFO;

typedef struct {
  UINT8   NoBootDevs[MAX_BOOT_ORDER_NUMBER];
  BOOLEAN EfiBootDevSwitch[MAX_BOOT_DEVICES_NUMBER];
  BOOLEAN LegacyAdvBootDevSwitch[MAX_LEGACY_BOOT_DEV_NUM];
  BOOLEAN LegacyTypeDevSwitch[MAX_LEGACY_BOOT_DEV_NUM];

  UINT8  HaveLegacyBootDevTypeOrder;
  UINT8  HaveLegacyBootDev;
  UINT8  HaveEfiBootDev;
} BOOT_CONFIGURATION;

#pragma pack()

#endif
