/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD HSTI DXE Driver Header file
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  PSP
 * @e \$Revision: 312065 $   @e \$Date: 2015-01-30 18:23:05 +0800 (Fri, 30 Jan 2015) $
 *
 */
/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */
#define  HSTI_AMD_FEATUERS_SIZE_IN_BYTES        (MAX_HSTI_AMD_FEATURE_BYTE_INDEX + 1)
#define  HSTI_AMD_ERROR_STRING_SIZE             0x100



#define MAX_HSTI_AMD_FEATURE_BYTE_INDEX                             HSTI_AMD_FEATURE_BYTE_INDEX_5

//Security Feature bitmap
//Security Feature Byte Index 0
#define HSTI_AMD_FEATURE_BYTE_INDEX_0                               0     ///< Byte index 0
#define HSTI_AMD_CRYPTO_STRENGTH                                    BIT0  ///< Do you use RSA 2048 and SHA256 only (or similar crypto strength)

///2. Firmware Code must be present in protected storage
///Security Feature Byte Index 1
#define HSTI_AMD_FEATURE_BYTE_INDEX_1                               1     ///< Byte index 1
#define HSTI_AMD_FWCODE_PROTECT_PROTECT_SPI                         BIT0  ///< Do you protect spiflash?
#define HSTI_AMD_FWCODE_PROTECT_SIGNED_FW_CHECK                     BIT2  ///< Do you support Signed Firmware Check Firmware that is installed by OEM is either readonly or protected by secure firmware update process

///3. Secure firmware update process
///Security Feature Byte Index 2
#define HSTI_AMD_FEATURE_BYTE_INDEX_2                               2     ///< Byte index 2
#define HSTI_AMD_SECURE_FW_UPDATE_DFT_TESTKEY                       BIT0  ///< Is secure firmware update process on by default with test keys?  (RECOMMENDED)
#define HSTI_AMD_SECURE_FW_UPDATE_CHECK_TESTKEY_IN_PRODUCTION       BIT1  ///< Do you check if test keys are used in production?
#define HSTI_AMD_SECURE_FW_UPDATE_ROLLBACK_CHECK                    BIT2  ///< Do you allow rollback to insecure firmware  version? If yes then what is the protection mechanism? Do you clear TPM when rollback happens?


///4.Do you have backdoors to override SecureBoot
///Security Feature Byte Index 3
#define HSTI_AMD_FEATURE_BYTE_INDEX_3                               3     ///< Byte index 3
#define HSTI_AMD_SECUREBOOT_BACKDOOR_INLINE_PROMPT_CHECK            BIT0  ///< a. Does your system support inline prompting to bypass Secureboot? If yes then is it disabled while SecureBoot is enabled
#define HSTI_AMD_SECUREBOOT_BACKDOOR_MANUFACTURE_CHECK              BIT1  ///< b. Do  you  have  manufacturing  backdoors?  Do  you  check  for  them  to  be  disabled  while SecureBoot is enabled and always disabled in production system?

///Security Feature Byte Index 4
#define HSTI_AMD_FEATURE_BYTE_INDEX_4                               4     ///< Byte index 4
#define HSTI_AMD_FEATURE_PSP_SECURE_EN                              BIT0  ///< Fuse SecureEnable to ensure silicon level secure
#define HSTI_AMD_FEATURE_PSP_PLATFORM_SECURE_BOOT_EN                BIT1  ///< Boot Integrity Support
#define HSTI_AMD_FEATURE_PSP_DEBUG_LOCK_ON                          BIT2  ///< Protection against external hardware debugger
///Compatibility Support Modules (CSM)
///Security Feature Byte Index 5
#define HSTI_AMD_FEATURE_BYTE_INDEX_5                               5     ///< Byte index 5
#define HSTI_AMD_CSM_DISABLE_IF_SECUREBOOT_EN                       BIT0  ///< b.  Do you check blocking of loading CSM when SecureBoot is enabled
#define HSTI_AMD_CSM_DISABLE_ON_CS_SYSTEM                           BIT1  ///< c.  [CS]Do you check if CSM is not present on CS systems permanently
