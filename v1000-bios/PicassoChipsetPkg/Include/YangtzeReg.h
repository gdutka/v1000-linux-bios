/** @file

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _YANGTZE_REG_H_
#define _YANGTZE_REG_H_

#include <CommonReg.h>
#include <Fch.h>

//
//  FCH ACPI IO Register (PMBASE)
//
#define R_FCH_ACPI_PM1_STATUS              0x00
#define   B_TMR_STATUS                      BIT0
#define   B_BM_STATUS                       BIT4
#define   B_GLB_STATUS                      BIT5
#define   B_PWR_BTN_STATUS                  BIT8
#define   N_PWR_BTN_STATUS                  0x08
#define   B_RTC_STATUS                      BIT10
#define   B_WAKEUP_STATUS                   BIT15
#define   S_FCH_ACPI_PM1_STATUS            0x02

#define R_FCH_ACPI_PM1_ENABLE              0x02
#define   B_TMR_EN                          BIT0
#define     V_TMR_EN                        BIT0
#define   B_GLB_EN                          BIT5
#define     V_GLB_EN                        BIT5
#define   B_PWR_BTN_EN                      BIT8
#define     V_PWR_BTN_EN                    BIT8
#define     N_PWR_BTN_EN                    0x08
#ifndef _ICH_REGS_H_
  #define   B_RTC_EN                          BIT10
#endif
#define     V_RTC_EN                        BIT10
#define   S_FCH_ACPI_PM1_ENABLE            0x02

#define R_FCH_ACPI_PM_CONTROL              0x04
#ifndef _ICH_REGS_H_
#define   B_SCI_EN                          BIT0
#endif
#define     V_SCI_EN                        BIT0
#ifndef _ICH_REGS_H_
#define   B_BM_RLD                          BIT1
#endif
#define     V_BM_RLD                        BIT1

#define   B_SLP_TYPE                        (BIT10 + BIT11 + BIT12)
#define     V_SLP_TYPE_S0                   (0 << 10)
#define     V_SLP_TYPE_S1                   (1 << 10)
#define     V_SLP_TYPE_S3                   (3 << 10)
#define     V_SLP_TYPE_S4                   (4 << 10)
#define     V_SLP_TYPE_S5                   (5 << 10)
#define   B_ACPI_SLP_EN                     BIT13
#define     V_ACPI_SLP_EN                   BIT13

//
// SATA Memory Address BAR5 Register
//

#define R_FCH_SATA_MEM_PORT_IMPLEMENTED              0x0c

#define V_FCH_SATA_MEM_PORT_REGISTER_START_OFFSET    0x100
#define V_FCH_SATA_MEM_PORT_REGISTER_RANGE           0x80

#define R_FCH_SATA_MEM_PORT_SATAUS                   0x28
#define B_INTERFACE_POWER_MANAGEMENT                 (BIT8+BIT9+BIT10+BIT11)

//
// EHCI Memory Access rance Register
//
#define R_FCH_EHCI_LEGACY_CONTROL                    0x4
#define R_FCH_EHCI_LEGACY_STATUS                     0x6

#define R_FCH_EHCI_MEM_CAPLENGTH                     0x0
#define R_FCH_EHCI_MEM_EECP_CAPABILITY_PTR           0x9

#define R_FCH_EHCI_MEM_COMMAND                       0x0
#define   B_FCH_EHCI_MEM_RUN_STOP                      BIT0
#define   B_FCH_EHCI_MEM_RESET_HC                      BIT1
#define R_FCH_EHCI_MEM_STATUS                        0x4
#define R_FCH_EHCI_MEM_CONFIGURATION                 0x40
#define   B_CONFIGURATION                             BIT0

#define R_FCH_EHCI_MISC_COLTROL2                     0x64
#define   B_FCH_EHCI_MISC_COLTROL2_SHARE_REG_ENABLE  BIT1
#define R_FCH_EHCI_HUB_CONFIG4                       0x90
#define   B_FCH_EHCI_PORT_DISABLE_PORT0               BIT20
#define   B_FCH_EHCI_PORT_DISABLE_PORT1               BIT21
#define   B_FCH_EHCI_PORT_DISABLE_PORT2               BIT22
#define   B_FCH_EHCI_PORT_DISABLE_PORT3               BIT23

//
// USB OHCI memory access range register
//
#define R_FCH_OHCI_MEM_HC_CONTROL                    0x04
#define R_FCH_OHCI_MEM_HC_COMMAND_STATUS             0x08
#define R_FCH_OHCI_MEM_HC_INT_STATUS                 0x0c
#define R_FCH_OHCI_MEM_HC_INT_ENABLE                 0x10
#define R_FCH_OHCI_MEM_HC_CONTROL_HEAD_ED            0x20
#define R_FCH_OHCI_MEM_HC_BULK_HEAD_ED               0x28

#define R_FCH_OHCI_PORT_DISABLE                      0x42


#define  R_XHCI_INDIRECT_INDEX_REGISTER              0x48
#define  R_XHCI_INDIRECT_DATA_REGISTER               0x4C;
#define  R_XHCI_INDIRECT_DISABLE_PORT_REGISTER         0x0C
#define    B_XHCI_PORT_DISABLE_PORT0                   BIT0
#define    B_XHCI_PORT_DISABLE_PORT1                   BIT1

#endif
