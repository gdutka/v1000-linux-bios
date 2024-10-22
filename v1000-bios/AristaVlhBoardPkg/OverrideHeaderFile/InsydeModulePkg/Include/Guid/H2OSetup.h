/** @file
  Definitions for H2O Setup

;******************************************************************************
;* Copyright (c) 2019 - 2021, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef __H2O_SETUP_H__
#define __H2O_SETUP_H__

#define SETUP_VARIABLE_NAME                 L"Setup"
#define SETUP_UTILITY_CLASS                 1
#define SETUP_UTILITY_SUBCLASS              0
#define EFI_USER_ACCESS_TWO                 0x04
#define EFI_USER_ACCESS_THREE               0x05
#define VAR_EQ_GRAY_TEXT                    1
#define MAX_HII_HANDLES                     0x10

//
// The maximum supported number of devices in advanced boot menu.
// It can modify this value directly to change the supported devices.
//
#define MAX_BOOT_DEVICES_NUMBER             500
#define MAX_BOOT_ORDER_NUMBER               16   // Maximum number of legacy boot type
#define MAX_LEGACY_BOOT_DEV_NUM             256  // Maximum number of legacy boot device


//[-start-240426-IB18160212-modify-]//
////////////////////////////////////////
// Formset GUID definitions
////////////////////////////////////////
#define SETUP_UTILITY_FORMSET_CLASS_GUID {0x9f85453e, 0x2f03, 0x4989, {0xad, 0x3b, 0x4a, 0x84, 0x07, 0x91, 0xaf, 0x3a}}
#define FORMSET_ID_GUID_MAIN             {0xc1e0b01a, 0x607e, 0x4b75, {0xb8, 0xbb, 0x06, 0x31, 0xec, 0xfa, 0xac, 0xf2}}
#define FORMSET_ID_GUID_BOOT             {0x2d068309, 0x12ac, 0x45ab, {0x96, 0x00, 0x91, 0x87, 0x51, 0x3c, 0xcd, 0xd8}}
#define FORMSET_ID_GUID_SECURITY         {0x5204f764, 0xdf25, 0x48a2, {0xb3, 0x37, 0x9e, 0xc1, 0x22, 0xb8, 0x5e, 0x0d}}
#define FORMSET_ID_GUID_EXIT             {0xb6936426, 0xfb04, 0x4a7b, {0xaa, 0x51, 0xfd, 0x49, 0x39, 0x7c, 0xdc, 0x01}}
#define FORMSET_ID_GUID_ADVANCE          {0xc6d4769e, 0x7f48, 0x4d2a, {0x98, 0xe9, 0x87, 0xad, 0xcc, 0xf3, 0x5c, 0xcc}}
#define FORMSET_ID_GUID_POWER            {0xa6712873, 0x925f, 0x46c6, {0x90, 0xb4, 0xa4, 0x0f, 0x86, 0xa0, 0x91, 0x7b}}
//[-end-240426-IB18160212-modify-]//


////////////////////////////////////////
// VFR varstore definitions
////////////////////////////////////////
#define CONFIGURATION_VARSTORE_ID           0x1234
#define SYSTEM_CONFIGURATION_GUID           {0xA04A27f4, 0xDF00, 0x4D42, 0xB5, 0x52, 0x39, 0x51, 0x13, 0x02, 0x11, 0x3D}

#define PASSWORD_CONFIGURATION_VARSTORE_ID  0x1235
#define PASSWORD_CONFIGURATION_GUID         {0xf72deef6, 0x13ef, 0x4958, 0xb0, 0x27, 0xe, 0x45, 0xce, 0x7f, 0xa4, 0x5e}

#define PASSWORD_NAME_VALUE_VARSTORE_ID     0x1236

#define TCG2_CONFIGURATION_INFO_VARSTORE_ID 0x1237
#define TCG2_CONFIGURATION_INFO_GUID        {0x07a66697, 0xd400, 0x4903, 0xb3, 0xda, 0x67, 0xa6, 0x1d, 0x2b, 0x70, 0x58}

#define BOOT_VARSTORE_ID    0x1489


////////////////////////////////////////
// Form ID definitions
////////////////////////////////////////
#define ROOT_FORM_ID                        1
//
// For Security formset
//
#define STORAGE_PASSWORD_FORM_ID            0x1901
#define STORAGE_PASSWORD_DEVICE_FORM_ID     0x1902
//
// For Boot formset
//
#define BOOT_OPTION_FORM_ID                 0x530
#define BOOT_DEVICE_FDD_FORM_ID             0x531
#define BOOT_DEVICE_HDD_FORM_ID             0x532
#define BOOT_DEVICE_CD_FORM_ID              0x533
#define BOOT_DEVICE_PCMCIA_FORM_ID          0x534
#define BOOT_DEVICE_USB_FORM_ID             0x535
#define BOOT_EMBED_NETWORK_FORM_ID          0x536
#define BOOT_DEVICE_BEV_FORM_ID             0x537
#define BOOT_DEVICE_OTHER_FORM_ID           0x538
#define BOOT_DEVICE_ADVANCE_FORM_ID         0x539
#define BOOT_DEVICE_EFI_FORM_ID             0x53A
#define BOOT_DEVICE_LEG_NOR_BOOT_ID         0x53B
#define BOOT_DEVICE_LEG_ADV_BOOT_ID         0x53C
#define BOOT_HIDDEN_BOOT_TYPE_ORDER_ID      0x53D


////////////////////////////////////////
// Question ID definitions
////////////////////////////////////////
#define SETUP_DYNAMIC_CREATE_KEY_VALUE_BASE 0x5200

#define KEY_SCAN_ESC                        0xF0D1
#define KEY_SCAN_F9                         0xF0D2
#define KEY_SCAN_F10                        0xF0D3
//
// For Main formset
//
#define KEY_LANGUAGE_UPDATE                 0x527F  // SETUP_DYNAMIC_QUESTION_ID(Language)
#define KEY_LANGUAGE_CARRY_OFF              0x1046
#define KEY_ABOUT_THIS_SOFTWARE             0x1059
//
// For Security formset
//
#define KEY_SUPERVISOR_PASSWORD             0x1037
#define KEY_USER_PASSWORD                   0x1038
#define KEY_CLEAR_USER_PASSWORD             0x1039
#define KEY_SET_ALL_MASTER_HDD_PASSWORD     0x1042
#define KEY_SET_ALL_HDD_PASSWORD            0x1043
#define KEY_PSID_REVERT                     0x1048

#define KEY_TPM_CLEAR                       0x11B5
#define KEY_TPM                             0x11B6
#define KEY_TPM2_ENABLE                     0x11B7
#define KEY_TPM2_CLEAR                      0x11B8
#define KEY_TPM_SELECT                      0x11B9
#define KEY_TPM_HIDE                        0x11BA
#define KEY_TPM2_HIDE                       0x11BB
#define KEY_TPM2_OPERATION                  0x11BF
#define KEY_TREE_PROTOCOL_VERSION           0x11C0

#define KEY_TPM2_PCR_BANKS_REQUEST_0        0x1200
#define KEY_TPM2_PCR_BANKS_REQUEST_1        0x1201
#define KEY_TPM2_PCR_BANKS_REQUEST_2        0x1202
#define KEY_TPM2_PCR_BANKS_REQUEST_3        0x1203
#define KEY_TPM2_PCR_BANKS_REQUEST_4        0x1204

#define KEY_STORAGE_PASSWORD_OPTION_BASE    0x1900
#define KEY_SET_STORAGE_PASSWORD            0x1920
#define KEY_CHECK_STORAGE_PASSWORD          0x1921
#define KEY_SET_MASTER_STORAGE_PASSWORD     0x1922
#define KEY_TCG_STORAGE_PASSWORD            0x1923
#define KEY_CHECK_MASTER_STORAGE_PASSWORD   0x1924
//
// For Boot formset
//
#define KEY_UP_SHIFT                        0x103a
#define KEY_DOWN_SHIFT                      0x103b
#define KEY_BOOT_MENU_TYPE                  0x105A
#define KEY_LEGACY_NORMAL_BOOT_MENU         0x105B
#define KEY_NORMAL_BOOT_PRIORITY            0x105C
#define KEY_NEW_POSITION_POLICY             0x105D
#define KEY_BOOT_MODE_TYPE                  0x105E
#define KEY_PXE_BOOT_TO_LAN                 0x105F
#define KEY_HIDDEN_BOOT_TYPE_ORDER          0x1060
#define KEY_USB_BOOT                        0x1061
#define KEY_STORAGE_OPROM_ACCESS_ENABLE       0x1062
#define KEY_ESATA_BOOT_ACCESS_ENABLE          0x1063
//
// definition for create specific callback question ID doesn't have relative for SYSTEM_CONFIGURATION
//
#define KEY_BOOT_TYPE_ORDER_BASE            0x3200
#define KEY_BOOT_DEVICE_TYPE_BASE           (KEY_BOOT_TYPE_ORDER_BASE  + MAX_BOOT_ORDER_NUMBER)
#define KEY_EFI_BOOT_DEVICE_BASE            (KEY_BOOT_DEVICE_TYPE_BASE + MAX_LEGACY_BOOT_DEV_NUM)
#define KEY_ADV_LEGACY_BOOT_BASE            (KEY_EFI_BOOT_DEVICE_BASE  + MAX_BOOT_DEVICES_NUMBER)
#define KEY_BOOT_DEVICE_BASE                (KEY_ADV_LEGACY_BOOT_BASE  + MAX_LEGACY_BOOT_DEV_NUM)
//
// For Exit formset
//
#define KEY_SAVE_EXIT                       0x1031
#define KEY_EXIT_DISCARD                    0x1032
#define KEY_LOAD_OPTIMAL                    0x1033
#define KEY_LOAD_CUSTOM                     0x1034
#define KEY_SAVE_CUSTOM                     0x1035
#define KEY_DISCARD_CHANGE                  0x1036
#define KEY_SAVE_WITHOUT_EXIT               0x1047


////////////////////////////////////////
// VFR label definitions
////////////////////////////////////////
//
// For Main formset
//
#define SETUP_UTILITY_LANG_MENU                                  0x1001
#define UPDATE_INFO_RAM_SLOT_LABEL                               0x1007
#define MAIN_PAGE_PLATFORM_INFO_LABEL                            0x1008
#define UPDATE_CPU_TYPE_LABEL                                    0x1009
#define UPDATE_SYSTEM_BUS_SPEED_LABEL                            0x100A
#define UPDATE_CACHE_RAM_LABEL                                   0x100B
#define COPYRIGHT_LABEL                                          0x100E
#define UPDATE_CONFIG_ID_LABEL                                   0x100F
#define UPDATE_CONFIG_NAME_LABEL                                 0x1010
#define BIOS_BUILD_TIME_LABEL                                    0x1012
#define BIOS_VERSION_LABEL                                       0x1013
#define BIOS_VERSION_END_LABEL                                   0x1014
#define UPDATE_SYSTEM_BUS_SPEED_END_LABEL                        0x1801
//
// For Security formset
//
#define USER_PASSWORD_LABEL                                      0x1003
#define CLEAR_USER_PASSWORD_LABEL                                0x1004
#define TPM_STATE_LABEL                                          0x11BC
#define TPM_OPERATION_START_LABEL                                0x11BD
#define TPM_OPERATION_END_LABEL                                  0x11BE
#define LABEL_STORAGE_PASSWORD_OPTION                            KEY_STORAGE_PASSWORD_OPTION_BASE
#define LABEL_STORAGE_PASSWORD_OPTION_END                        0x19ff
#define LABEL_STORAGE_PASSWORD_DEVICE_USER_PASSWORD_OPTION       0x1910
#define LABEL_STORAGE_PASSWORD_DEVICE_USER_PASSWORD_OPTION_END   0x1911
#define LABEL_STORAGE_PASSWORD_DEVICE_MASTER_PASSWORD_OPTION     0x1912
#define LABEL_STORAGE_PASSWORD_DEVICE_MASTER_PASSWORD_OPTION_END 0x1913
#define LABEL_STORAGE_PASSWORD_DEVICE_TCG_PASSWORD_OPTION        0x1914
#define LABEL_STORAGE_PASSWORD_DEVICE_TCG_PASSWORD_OPTION_END    0x1915
#define LABEL_STORAGE_PASSWORD_DEVICE_TCG_PSID_OPTION            0x1916
#define LABEL_STORAGE_PASSWORD_DEVICE_TCG_PSID_OPTION_END        0x1917
//
// For Boot formset
//
#define BOOT_DEVICE_LABEL                                        0x1002
#define UPDATE_EFI_OPTION_LABEL                                  0x100C
#define OPROM_STORAGE_DEVICE_BOOT_LABEL                          0x100D
#define BOOT_ORDER_LABEL                                         0x3000
#define FDD_BOOT_DEVICE_LABEL                                    0x3001
#define HDD_BOOT_DEVICE_LABEL                                    0x3002
#define CD_BOOT_DEVICE_LABEL                                     0x3003
#define PCMCIA_BOOT_DEVICE_LABEL                                 0x3004
#define USB_BOOT_DEVICE_LABEL                                    0x3005
#define EMBED_NETWORK_BOOT_DEVICE_LABEL                          0x3006
#define BEV_BOOT_DEVICE_LABEL                                    0x3007
#define OTHER_BOOT_DEVICE_LABEL                                  0x3008
#define EFI_BOOT_DEVICE_LABEL                                    0x3009
#define BOOT_LEGACY_ADV_BOOT_LABEL                               0x300A
#define HIDDEN_BOOT_TYPE_ORDER_LABEL                             0x300B
//
// For Exit formset
//
#define TRIGGER_BROWSER_REFRESH_LABEL                            0x1011

////////////////////////////////////////
// VFR question value definitions
////////////////////////////////////////
//
// Boot type relative definitions
//
#define DUAL_BOOT_TYPE                                      0x00
#define LEGACY_BOOT_TYPE                                    0x01
#define EFI_BOOT_TYPE                                       0x02

#define ADD_POSITION_FIRST                                  0x00
#define ADD_POSITION_LAST                                   0x01
#define ADD_POSITION_AUTO                                   0x02

#define DUAL_VGA_CONTROLLER_ENABLE                          1
#define DUAL_VGA_CONTROLLER_DISABLE                         0

//
// TPM
//
#define TPM_DEVICE_NULL                                     0
#define TPM_DEVICE_1_2                                      1
#define TPM_DEVICE_2_0                                      2
#define TPM_DEVICE_MAX                                      TPM_DEVICE_2_0
#define TPM_OPERATION_DISABLE_DEACTIVATE                    1
#define TPM_OPERATION_ENABLE_ACTIVATE                       2
#define TPM_OPERATION_SET_OWNER_INSTALL_TRUE                3
#define TPM_OPERATION_DISABLE_STORAGE_ENDORSEMENT_HIERARCHY 2
#define TPM_OPERATION_ENABLE_ALL_HIERARCHY                  1

//
// Network
//
#define UEFI_NETWORK_BOOT_OPTION_PXE_IPV4                   0
#define UEFI_NETWORK_BOOT_OPTION_PXE_IPV6                   1
#define UEFI_NETWORK_BOOT_OPTION_PXE_BOTH                   2
#define UEFI_NETWORK_BOOT_OPTION_LEGACY                     3
#define UEFI_NETWORK_BOOT_OPTION_DISABLE                    4
#define UEFI_NETWORK_BOOT_OPTION_HTTP_IPV4                  5
#define UEFI_NETWORK_BOOT_OPTION_HTTP_IPV6                  6
#define UEFI_NETWORK_BOOT_OPTION_HTTP_BOTH                  7
#define UEFI_NETWORK_BOOT_OPTION_HTTP_PXE_IPV4              8
#define UEFI_NETWORK_BOOT_OPTION_HTTP_PXE_IPV6              9
#define UEFI_NETWORK_BOOT_OPTION_HTTP_PXE_BOTH              10

extern EFI_GUID gFormsetIdGuidMain;
extern EFI_GUID gFormsetIdGuidAdvance;
extern EFI_GUID gFormsetIdGuidSecurity;
extern EFI_GUID gFormsetIdGuidPower;
extern EFI_GUID gFormsetIdGuidBoot;
extern EFI_GUID gFormsetIdGuidExit;
extern EFI_GUID gSystemConfigurationGuid;
#endif
