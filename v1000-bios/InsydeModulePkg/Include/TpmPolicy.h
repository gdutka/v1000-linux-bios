/** @file
  TPM policy definitions

;******************************************************************************
;* Copyright (c) 2013 - 2020, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _TPM_POLICY_H_
#define _TPM_POLICY_H_

//
// For PcdPeiTpmPolicy
//
#define SKIP_TPM_STARTUP                         ( 1 << 0  ) //BIT0
#define SKIP_MEASURE_CRTM_VERSION                ( 1 << 1  ) //BIT1
#define SKIP_MEASURE_MAIN_BIOS                   ( 1 << 2  ) //BIT2
#define SKIP_MEASURE_FV_IMAGE                    ( 1 << 3  ) //BIT3
#define SKIP_TPM_SELF_TEST                       ( 1 << 4  ) //BIT4
#define TPM2_STARTUP_IN_MP                       ( 1 << 5  ) //BIT5
#define SKIP_PEI_TPM_PROPERTY_CHECK              ( 1 << 6  ) //BIT6 // skip TPM property check everytime memory overwrite requested
#define SKIP_PEI_TPM_AUTO_DETECTION              ( 1 << 7  ) //BIT7 // skip TPM auto detection and return the result TPM_DEVICE_NULL
#define DMA_PROTECTION_MEASUREMENT               ( 1 << 8  ) //BIT8 // enable DMA protection measurement; if DMA protection is disabled, an EFI_EV_ACTION event string "DMA Protection Disabled" will be extended into PCR7.

//
// For PcdDxeTpmPolicy
//
#define SKIP_TPM_REVOKE_TRUST                    ( 1 << 0  ) //BIT0
#define SKIP_INSTALL_TCG_ACPI_TABLE              ( 1 << 1  ) //BIT1
#define SKIP_TCG_DXE_LOG_EVENT_I                 ( 1 << 2  ) //BIT2
#define SKIP_MEASURE_HANDOFF_TABLES              ( 1 << 3  ) //BIT3
#define SKIP_MEASURE_ALL_BOOT_VARIABLES          ( 1 << 4  ) //BIT4
#define SKIP_MEASURE_ACTION                      ( 1 << 5  ) //BIT5
#define SKIP_MEASURE_SECURE_BOOT_VARIABLES       ( 1 << 6  ) //BIT6
#define SKIP_MEASURE_SEPARATOR_EVENT             ( 1 << 7  ) //BIT7
#define SKIP_MEASURE_LAUNCH_OF_FIRMWARE_DEBUGGER ( 1 << 8  ) //BIT8
#define SKIP_MEASURE_OS_LOADER_AUTHORITY         ( 1 << 9  ) //BIT9 // obsolete
#define SKIP_MEASURE_GPT_TABLE                   ( 1 << 10 ) //BIT10
#define SKIP_MEASURE_PE_IMAGE                    ( 1 << 11 ) //BIT11
#define SKIP_MEASURE_WAKE_EVENT                  ( 1 << 12 ) //BIT12
#define SKIP_DXE_TPM_PROPERTY_CHECK              ( 1 << 13 ) //BIT13 // skip TPM property check everytime memory overwrite requested
#define SKIP_TPM_FMP_INSTALLATION                ( 1 << 14 ) //BIT14 // don't support TPM FMP
#define SET_UP_MSFT_INDEX_C0                     ( 1 << 30 ) //BIT30 // set up TPM NV index for use by the OS with Handle: 0x01C101C0
#define CUSTOMIZE_FIREWARE_DEBUGGER_CHECK        ( 1 << 31 ) //BIT31 // customize firmware debugger existence check

#endif

