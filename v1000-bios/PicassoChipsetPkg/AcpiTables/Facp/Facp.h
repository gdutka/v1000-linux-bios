/** @file

;*******************************************************************************
;* Copyright (c) 2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;*******************************************************************************
*/

#ifndef _FACP_H_
#define _FACP_H_

#include <SmiTable.h>
#include <IndustryStandard/Acpi.h>
#include <AcpiHeaderDefaultValue.h>

//
// ACPI table information used to initialize tables.
//
#define INT_MODEL           0x01
#define RESERVED            0x00
#define SCI_INT_VECTOR      0x0009
#define SMI_CMD_IO_PORT     FixedPcdGet16 (PcdSoftwareSmiPort)
#define ACPI_ENABLE         EFI_ACPI_ENABLE_SW_SMI
#define ACPI_DISABLE        EFI_ACPI_DISABLE_SW_SMI
#define S4BIOS_REQ          0x00

#define PSTATE_CNT          0x00

#define PM1a_EVT_BLK        0x00000400
#define PM1b_EVT_BLK        0x00000000
#define PM1a_CNT_BLK        0x00000404
#define PM1b_CNT_BLK        0x00000000
#define PM2_CNT_BLK         0x00000800
#define PM_TMR_BLK          0x00000408
#define GPE0_BLK            0x00000420
#define GPE1_BLK            0x00000000
#define PM1_EVT_LEN         0x04
#define PM1_CNT_LEN         0x02
#define PM2_CNT_LEN         0x01
#define PM_TM_LEN           0x04
#define GPE0_BLK_LEN        0x08
#define GPE1_BLK_LEN        0x00
#define GPE1_BASE           0x00

#define CST_CNT             0x00

#define P_LVL2_LAT          0x0065
#define P_LVL3_LAT          0x03e9
#define FLUSH_SIZE          0x0000
#define FLUSH_STRIDE        0x0000
#define DUTY_OFFSET         0x01
#define DUTY_WIDTH          0x03
#define DAY_ALRM            0x0D
#define MON_ALRM            0x00
#define CENTURY             0x00

#define FLAG                EFI_ACPI_1_0_WBINVD | \
                            EFI_ACPI_1_0_PROC_C1 | \
                            EFI_ACPI_1_0_SLP_BUTTON | \
                            EFI_ACPI_1_0_RTC_S4 | \
                            EFI_ACPI_2_0_TMR_VAL_EXT | \
                            EFI_ACPI_3_0_PCI_EXP_WAK | \
                            EFI_ACPI_3_0_USE_PLATFORM_CLOCK

#endif 
