/** @file
  Chipset Setup Configuration Definitions

;******************************************************************************
;* Copyright (c) 2015, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _CHIPSET_SETUP_CONFIG_H_
#define _CHIPSET_SETUP_CONFIG_H_
#ifndef VFRCOMPILE
#include <Uefi/UefiInternalFormRepresentation.h>
#endif
#include <KernelSetupConfig.h>

#define FORMSET_ID_GUID_ADVANCE   {0xc6d4769e, 0x7f48, 0x4d2a, {0x98, 0xe9, 0x87, 0xad, 0xcc, 0xf3, 0x5c, 0xcc}}
#define FORMSET_ID_GUID_POWER     {0xa6712873, 0x925f, 0x46c6, {0x90, 0xb4, 0xa4, 0x0f, 0x86, 0xa0, 0x91, 0x7b}}

//
// Setup Formset label
//

#define IDE_UPDATE_LABEL                0x1005
#define OTHER_VFR_GOTO_LABEL                    0x1d04
#define OTHER_VFR_GOTO_LABEL_END                0x1d05

//
// Callback key value
//
#define KEY_SERIAL_PORT_EN              0x3d
#define KEY_CHIPSET_EXTENDED_CONFIG     0x3e
#define KEY_CHIPSET_SDRAM_TIME_CTRL     0x3f
#define KEY_PCI_SLOT_4_IRQ_SET          0x2048
#define KEY_PCI_SLOT_5_IRQ_SET          0x2049
#define KEY_PCI_SLOT_6_IRQ_SET          0x204A

#define KEY_PARALLEL_PORT_EN            0x49

#define KEY_SATA_CNFIGURE_MODE          0x90

///#define KEY_SPECIAL_VGA_FEATURE         0xE0
///#define KEY_XGP_FEATURE                 0xE1
///#define KEY_HTF_FEATURE                 0xE2
///#define KEY_LASSO_SUPPORT               0xE3

#define KEY_MP_SUPPORT                  0xE4
#define KEY_UMA_MODE                    0xE5
#define KEY_CS_SUPPORT                  0xE6
#define KEY_S3_SUPPORT                  0xE7
#define KEY_RH_SUPPORT                  0xE8

#define IDE_MODE                        0
#define NATIVE_IDE_MODE                 0
#define RAID_MODE                       1
#define AHCI_MODE                       2
#define LEGACY_IDE_MODE                 3
#define IDE_TO_AHCI_MODE                4
#define AHCI_MODE_7804                  5
#define IDE_TO_AHCI_MODE_7804           6

//
// Form ID
//
#define SERIAL_ATA_PORT0_FORM_ID        0x230
#define SERIAL_ATA_PORT1_FORM_ID        0x231
#define SERIAL_ATA_PORT2_FORM_ID        0x232
#define SERIAL_ATA_PORT3_FORM_ID        0x233
#define SERIAL_ATA_PORT4_FORM_ID        0x234
#define SERIAL_ATA_PORT5_FORM_ID        0x235

#define KEY_OTHER_VFR_GOTO_BASE                 0x3000
#define CHANNEL_4_MASTER_FORM_ID        0x236
#define CHANNEL_4_SLAVE_FORM_ID         0x237

#define FAST_RECLAIM_COUNT              35
#define KEY_VERSION_PAGE_CALLBACK       0x800
#define ADVANCE_VARSTORE_ID                     0x1233

#define PLUG_IN_DISPLAY_SELECTION_START_LABEL   0x1d00
#define PLUG_IN_DISPLAY_SELECTION_END_LABEL     0x1d01
#define KEY_PLUG_IN_DISPLAY_SELECTION1          0x21d0
#define KEY_PLUG_IN_DISPLAY_SELECTION2          0x21d1


#pragma pack(1)

//
//  Setup Utility Structure
//
//-----------------------------------------------------------------------------------------------------------------
// Important!!! The following setup utility structure should be syncronize with OperationRegion MBOX in mailbox.asi.
// If you do NOT follow it, you may face on unexpected issue. The total size are 1200bytes.
// (Common 146bytes + Kernel 84bytes + Chipset 300bytes + OEM 70bytes + ODM 100bytes + Platform 500bytes)
//-----------------------------------------------------------------------------------------------------------------

typedef struct {
  //
  // Kernel system configuration (offset 0~229, total 230 bytes)
  //
  #define _IMPORT_KERNEL_SETUP_
  #include <KernelSetupData.h>
  #undef _IMPORT_KERNEL_SETUP_

  //
  // Chipset system configuration (offset 229~529, total 300 bytes)
  //
  #define _IMPORT_CHIPSET_COMMON_SETUP_
  #include <ChipsetCommonSetupData.h>
  #undef _IMPORT_CHIPSET_COMMON_SETUP_

//OEM_Start
//Offset(530);                 // This area must sync to SetupConfig.h
  UINT8         OEMRSV[70];    // Dummy area. Reserve for OEM team, really structure will fill in SetupConfig.h
//OEM_End

//OEM_Start
//Offset(600);                 // This are must sync to SetupConfig.h
  UINT8         ODMRSV[100];   // Dummy area. Reserve for ODM, really structure will fill in SetupConfig.h
//OEM_End

  //
  // Chipset system configuration (offset 700~1200, total 500 bytes)
  //
//   #define _IMPORT_CHIPSET_SPECIFIC_SETUP_
//   #include <ChipsetSpecificSetupData.h>
//   #undef _IMPORT_CHIPSET_SPECIFIC_SETUP_

} CHIPSET_CONFIGURATION;

typedef struct {
  UINT8         IdeDevice0;
  UINT8         IdeDevice1;
  UINT8         IdeDevice2;
  UINT8         IdeDevice3;
  UINT8         IdeDevice4;
  UINT8         IdeDevice5;
  UINT8         IdeDevice6;
  UINT8         IdeDevice7;
} ADVANCE_CONFIGURATION;
#pragma pack()

#endif

