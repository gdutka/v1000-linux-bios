/** @file
  FchRegs.h.

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


#ifndef _FCH_REGS_H_
#define _FCH_REGS_H_

#include <CommonReg.h>
#include <Fch.h>

//
// Bit Definitions
//

//
// Definitions beginning with "R_" are registers
// Definitions beginning with "B_" are bits within registers
// Definitions beginning with "V_" are meaningful values of bits within the registers
// Definitions beginning with "S_" are register sizes
// Definitions beginning with "N_" are the bit position
//

/*------------------------------------------------------------------
; DEBUG_PORT = 8-bit I/O Port Address for POST Code Display
;------------------------------------------------------------------ */
// ASIC VendorID and DeviceIDs
#define FCH_USB_OHCI_VID             AMD_FCH_VID      // Dev 18 Func 0, Dev 19 Func 0
#define FCH_USB_OHCI_DID             0x7807
#define FCH_USB_EHCI_VID             AMD_FCH_VID      // Dev 18 Func 2, Dev 19 Func 2
#define FCH_USB_EHCI_DID             0x7808
#define FCH_USB_XHCI_VID             AMD_FCH_VID      // Dev 10 Func 0, Dev 10 Func 1
#define FCH_USB_XHCI_DID             0x7812
#define FCH_SMBUS_VID                AMD_FCH_VID      // Dev 20 Func 0
#define FCH_SMBUS_DID                0x780B
#define FCH_IDE_VID                  AMD_FCH_VID      // Dev 20 Func 1
#define FCH_IDE_DID                  0x780C
#define FCH_AZALIA_VID               AMD_FCH_VID      // Dev 20 Func 2
#define FCH_AZALIA_DID               0x780D
#define FCH_LPC_VID                  AMD_FCH_VID      // Dev 20 Func 3
#define FCH_LPC_DID                  0x780E
#define FCH_PCIB_VID                 AMD_FCH_VID      // Dev 20 Func 4
#define FCH_PCIB_DID                 0x780F
#define FCH_USB_OHCIF_VID            AMD_FCH_VID      // dev 20 Func 5
#define FCH_USB_OHCIF_DID            0x7809
#define FCH_NIC_VID                  0x14E4          // Dev 20 Func 6
#define FCH_NIC_DID                  0x1699
#define FCH_SD_VID                   AMD_FCH_VID      // Dev 20 Func 7
#define FCH_SD_DID                   0x7806

//FCH Variant
#define FCH_M2                            0x01
#define FCH_M3                            0x03
#define FCH_M3T                           0x07
#define FCH_D2                            0x0F
#define FCH_D3                            0x1F
#define FCH_D4                            0x3F

//Misc
#define AMD_FCH_A11                        0x11
#define AMD_FCH_A12                        0x12
#define ACPI_SMI_CMD_PORT                  0xB0
#define ACPI_SMI_DATA_PORT                 0xB1
#define R_FCH_ACPI_PM1_STATUS              0x00
#define R_FCH_ACPI_PM1_ENABLE              0x02
#define R_FCH_ACPI_PM_CONTROL              0x04
#define R_FCH_ACPI_EVENT_STATUS            0x20
#define R_FCH_ACPI_EVENT_ENABLE            0x24
#define R_FCH_PM_ACPI_PMA_CNT_BLK_LO       0x2C

//#define SATA_BUS_DEV_FUN_FPGA            0x228
#define SATA_BUS_DEV_FUN            ((0x11 << 3) + 0)
#define FCH_SATA1_BUS        0
#define FCH_SATA1_DEV        17
#define FCH_SATA1_FUNC       0

#define FC_BUS_DEV_FUN              ((0x11 << 3) + 1)
#define FCH_XHCI_BUS        0
#define FCH_XHCI_DEV        16
#define FCH_XHCI_FUNC       0
//LM--#define USB_XHCI_BUS_DEV_FUN        ((SB_XHCI_DEV << 3) + SB_XHCI_FUNC)
#define FCH_XHCI1_BUS        0
#define FCH_XHCI1_DEV        16
#define FCH_XHCI1_FUNC       1
//--#define USB_XHCI1_BUS_DEV_FUN       ((FCH_XHCI1_DEV << 3) + FCH_XHCI1_FUNC)
#define USB1_OHCI_BUS_DEV_FUN       ((0x12 << 3) + 0)    // PORT 0-4
#define FCH_OHCI1_BUS        0
#define FCH_OHCI1_DEV        18
#define FCH_OHCI1_FUNC       0
#define USB2_OHCI_BUS_DEV_FUN       ((0x13 << 3) + 0)    // PORT 5-9
#define FCH_OHCI2_BUS        0
#define FCH_OHCI2_DEV        19
#define FCH_OHCI2_FUNC       0
#define USB3_OHCI_BUS_DEV_FUN       ((0x16 << 3) + 0)    // PORT 10-13
#define FCH_OHCI3_BUS        0
#define FCH_OHCI3_DEV        22
#define FCH_OHCI3_FUNC       0
#define USB1_EHCI_BUS_DEV_FUN       ((0x12 << 3) + 2)    // PORT 0-4
#define FCH_EHCI1_BUS        0
#define FCH_EHCI1_DEV        18
#define FCH_EHCI1_FUNC       2
#define USB2_EHCI_BUS_DEV_FUN       ((0x13 << 3) + 2)    // PORT 5-9
#define FCH_EHCI2_BUS        0
#define FCH_EHCI2_DEV        19
#define FCH_EHCI2_FUNC       2
#define USB3_EHCI_BUS_DEV_FUN       ((0x16 << 3) + 2)  // PORT 10-13
#define FCH_EHCI3_BUS        0
#define FCH_EHCI3_DEV        22
#define FCH_EHCI3_FUNC       2
#define SMBUS_BUS_DEV_FUN           ((0x14 << 3) + 0)
#define FCH_ISA_BUS          0
#define FCH_ISA_DEV          20
#define FCH_ISA_FUNC         0
#define IDE_BUS_DEV_FUN             ((0x14 << 3) + 1)
#define FCH_IDE_BUS          0
#define FCH_IDE_DEV          20
#define FCH_IDE_FUNC         1
#define AZALIA_BUS_DEV_FUN          ((0x14 << 3) + 2)
#define FCH_AZALIA_BUS       0
#define FCH_AZALIA_DEV       20
#define FCH_AZALIA_FUNC      2
#define LPC_BUS_DEV_FUN             ((0x14 << 3) + 3)
#define FCH_LPC_BUS          0
#define FCH_LPC_DEV          20
#define FCH_LPC_FUNC         3
#define PCIB_BUS_DEV_FUN            ((0x14 << 3) + 4)    // P2P in SB700
#define FCH_PCI_BUS          0
#define FCH_PCI_DEV          20
#define FCH_PCI_FUNC         4
#define USB4_OHCI_BUS_DEV_FUN       ((0x14 << 3) + 5)    // PORT FL0 - FL1
#define FCH_OHCI4_BUS        0
#define FCH_OHCI4_DEV        20
#define FCH_OHCI4_FUNC       5
//Customer Ethernet Controller
#define GEC_BUS_DEV_FUN             ((0x14 << 3) + 6)
#define FCH_GBEC_BUS         0
#define FCH_GBEC_DEV         20
#define FCH_GBEC_FUNC        6

#define SD_BUS_DEV_FUN              ((0x14 << 3) + 7)    // SD in SB900
#define SD_PCI_BUS           0
#define SD_PCI_DEV           20
#define SD_PCI_FUNC          7
#define SD_PCI_REGA4         0xA4
#define SD_PCI_REGB0         0xB0


#define FCH_GPP_BUS          0
#define FCH_GPP_DEV          21
#define FCH_GPP_FUNC         0
#define GPP0_BUS_DEV_FUN            ((0x15 << 3) + 0)    // GPP P2P bridge PORT0
#define GPP1_BUS_DEV_FUN            ((0x15 << 3) + 1)    // GPP P2P bridge PORT1
#define GPP2_BUS_DEV_FUN            ((0x15 << 3) + 2)    // GPP P2P bridge PORT2
#define GPP3_BUS_DEV_FUN            ((0x15 << 3) + 3)    // GPP P2P bridge PORT3



//
// South Bridge Internal PCI Device Configuration Space Define
//

//
// SATA Controller
//
#define     V_SATA_MODE_IDE                           0x018F
#define     V_SATA_MODE_RAID                          0x0400
#define     V_SATA_MODE_AHCI                          0x0601

#define R_FCH_SATA_CONFIGURATION                     0x40
#define   B_PCI_HDR_WR_ENA                            BIT0
#define     V_PCI_HDR_WR_ENABLE                       BIT0
#define     V_PCI_HDR_WR_DISABLE                      0
#define   B_BA5_IND_ACC_ENA                           BIT1
#define     V_BA5_IND_ACC_ENABLE                      BIT1
#define     V_BA5_IND_ACC_DISABLE                     0
#define   B_SATA_DMAT_CTL                             BIT2
#define     V_SATA_DMAT_CTL_ENABLE                    BIT2
#define     V_SATA_DMAT_CTL_DISABLE                   0
#define   B_MEM_WR_PWR_SV_DIS                         BIT3
#define     V_MEM_WR_PWR_SV_DISABLE                   BIT3
#define     V_MEM_WR_PWR_SV_ENABLE                    0
#define   B_FASTER_IDE_BOOT_UP_ENABLE                 BIT3
#define     V_FASTER_IDE_BOOT_UP_ENABLE               BIT3
#define     V_FASTER_IDE_BOOT_UP_DISABLE              0
#define   B_MEM_WR_PWR_SV_DIS_SB450                   BIT4
#define     V_MEM_WR_PWR_SV_DISABLE_SB450             BIT4
#define     V_MEM_WR_PWR_SV_ENABLE_SB450              0
#define R_FCH_SATA_MSI_CAPABILITY                    0x42
#define   B_SATA_MSI_CAPABILITY                       BIT7
#define     V_SATA_MSI_CAPABILITY_DISABLE             BIT7
#define     V_SATA_MSI_CAPABILITY_ENABLE              0
#define R_FCH_SATA_WATCHDOG_CONTROL_AND_STATUS       0x44
#define   B_SATA_WATCHDOG_ENABLE                      BIT0
#define     V_SATA_WATCHDOG_ENABLE                    BIT0
#define     V_SATA_WATCHDOG_DISABLE                   0
#define   B_SATA_WATCHDOG_TIMEOUT_STATUS              BIT1

#define R_FCH_SATA_PHY_GLOBAL_CONTROL                0x86
#define R_FCH_SATA_PHY_PORT0_CONTROL                 0x88
#define R_FCH_SATA_PHY_PORT1_CONTROL                 0x8C
#define R_FCH_SATA_PHY_PORT2_CONTROL                 0x90
#define R_FCH_SATA_PHY_PORT3_CONTROL                 0x94

#define R_FCH_SATA_IDE0_TASK_FILE_CFG                0xA0
#define R_FCH_SATA_IDE1_TASK_FILE_CFG                0xB0
#define   B_CHANNEL_RST                               BIT2
#define     V_CHANNEL_RST_ENABLE                      BIT2
#define     V_CHANNEL_RST_DISABLE                     0

#define R_FCH_SATA_PORT0_BIST_CONTROL_STATUS         0xA4
#define R_FCH_SATA_PORT1_BIST_CONTROL_STATUS         0xAC
#define R_FCH_SATA_PORT2_BIST_CONTROL_STATUS         0xB4
#define R_FCH_SATA_PORT3_BIST_CONTROL_STATUS         0xBC

//
// SATA Memory Address BAR5 Register
//

#define R_FCH_SATA_MEM_PORT_IMPLEMENTED              0x0c

#define V_FCH_SATA_MEM_PORT_REGISTER_START_OFFSET    0x100
#define V_FCH_SATA_MEM_PORT_REGISTER_RANGE           0x80

#define R_FCH_SATA_MEM_PORT_SATAUS                   0x28
#define   B_INTERFACE_POWER_MANAGEMENT                (BIT8+BIT9+BIT10+BIT11)

#define R_FCH_SATA_MEM_PORT_CONTROL                  0x2C

#define R_FCH_SATA_MEM_SYSTEM_CONFIGURATIUON_STATUS  0x48
#define   B_IDE0_INT_BLOCK                            BIT22
#define     V_IDE0_INT_BLOCK_ENABLE                   BIT22
#define     V_IDE0_INT_BLOCK_DISABLE                  0
#define   B_IDE1_INT_BLOCK                            BIT23
#define     V_IDE1_INT_BLOCK_ENABLE                   BIT23
#define     V_IDE1_INT_BLOCK_DISABLE                  0

#define R_FCH_SATA_MEM_SATA_SCONTROL_CH1             0x100
#define R_FCH_SATA_MEM_SATA_SCONTROL_CH2             0x180
#define   B_SATA_RESET                                BIT0
#define     V_SATA_RESET_ENABLE                       BIT0
#define     V_SATA_RESET_DISABLE                      0


#define R_FCH_SATA_MEM_SATA_PHY_CONFIGURATION        0x144
#define   B_TX_SWING_0                                BIT13
#define     V_TX_SWING_0_ENABLE                       BIT13
#define     V_TX_SWING_0_DISABLE                      0
#define   B_TX_SWING_1                                BIT19
#define     V_TX_SWING_1_ENABLE                       BIT19
#define     V_TX_SWING_1_DISABLE                      0
#define   B_BYPASS_OOB_SEQUENCE                       BIT21
#define     V_BYPASS_OOB_SEQUENCE_ENABLE              BIT21
#define     V_BYPASS_OOB_SEQUENCE_DISABLE             0

#define   V_FCH_SATA_MEM_SATA_PHY_CONFIGURATION_A11    0x2008A0B3
#define   V_FCH_SATA_MEM_SATA_PHY_CONFIGURATION_A12_ABOVE 0x2000A0B2


//
// USB OHCI Controller Configuration Space
//

#define R_FCH_OHCI_REG40                             0x40

#define R_FCH_OHCI_PORT_DISABLE                      0x42

#define R_FCH_USB01_ATI_MISC_CONTROL                 0x50
#define   B_HT_MSI_SUPPORT                            BIT5
#define     V_HT_MSI_SUPPORT_ENABLE                   BIT5
#define     V_HT_MSI_SUPPORT_DISABLE                  0
#define   B_OHCI_64_BYTE_FETCH_ENABLE                 BIT7
#define     V_OHCI_64_BYTE_FETCH_ENABLE               BIT7
#define     V_OHCI_64_BYTE_FETCH_DISABLE              0
#define   B_DIS_USB_S3_OVR_CUR                        BIT8
#define     V_DIS_USB_S3_OVR_CUR                      BIT8
#define     V_EN_USB_S3_OVR_CUR                       0
#define   B_EN_OHCI_SPND                              BIT8
#define     V_EN_OHCI_SPND                            BIT8
#define     V_DIS_OHCI_SPND                           0
#define   B_SPLIT_INTERRUPT_FIX                       BIT9
#define     V_SPLIT_INTERRUPT_FIX_ENABLE              BIT9
#define     V_SPLIT_INTERRUPT_FIX_DISABLE             0
#define   B_PLL_CONTROL_IPDC                          (BIT10 + BIT11)
#define   B_PLL_CONTROL_IPVG                          (BIT12 + BIT13 + BIT14)
#define   B_PLL_CONTROL_IPCP                          (BIT12 + BIT13 + BIT14)

#define   B_SMI_HANDSHAKE                             BIT12
#define     V_SMI_HANDSHAKE_DISABLE                   BIT12
#define     V_SMI_HANDSHAKE_ENABLE                    0   
//
// USB OHCI memory access range register
//
#define R_FCH_OHCI_MEM_HC_CONTROL                    0x04
#define R_FCH_OHCI_MEM_HC_COMMAND_STATUS             0x08
#define R_FCH_OHCI_MEM_HC_INT_STATUS                 0x0c
#define R_FCH_OHCI_MEM_HC_INT_ENABLE                 0x10
#define R_FCH_OHCI_MEM_HC_CONTROL_HEAD_ED            0x20
#define R_FCH_OHCI_MEM_HC_BULK_HEAD_ED               0x28

//
// USB EHCI Controller Configuration Space
//
#define R_FCH_EHCI_ATI_MISC_CONTROL                  0x50
#define   B_DISABLE_PME                               BIT5
#define     V_ENABLE_PME                              0
#define     V_DISABLE_PME                             BIT5
#define   B_DISABLE_MSI                               BIT6
#define     V_ENABLE_MSI                              0
#define     V_DISABLE_MSI                             BIT6
#define   B_EHCI_MEM_XFER_BREAK                       BIT9
#define     V_EHCI_HIGH_PRIORITY                      0
#define     V_EHCI_EQUAL_PRIORITY                     BIT9
#define   B_ENABLE_RECLAMATION_FIX                    BIT11
#define     V_DISABLE_RECLAMATION_FIX                 0
#define     V_ENABLE_RECLAMATION_FIX                  BIT11
#define   B_TIMEOUT_RETRY_ENABLE                      BIT13
#define     V_TIMEOUT_RETRY_DISABLE                   0
#define     V_TIMEOUT_RETRY_ENABLE                    BIT13

#define R_FCH_EHCI_REG54                             0x54

#define R_FCH_EHCI_LEGACY_CONTROL                    0x4
#define R_FCH_EHCI_LEGACY_STATUS                     0x6

//
// EHCI Memory Access rance Register
//
#define R_FCH_EHCI_MEM_CAPLENGTH                     0x0
#define R_FCH_EHCI_MEM_EECP_CAPABILITY_PTR           0x9

#define R_FCH_EHCI_MEM_COMMAND                       0x0
#define   B_FCH_EHCI_MEM_RUN_STOP                      BIT0
#define   B_FCH_EHCI_MEM_RESET_HC                      BIT1
#define R_FCH_EHCI_MEM_STATUS                        0x4
#define R_FCH_EHCI_MEM_CONFIGURATION                 0x40
#define   B_CONFIGURATION                             BIT0


//
// ISA, SMBUS Module and ACPI Block ( D:20, F:0 )
//
#define R_FCH_REVISION                               0x08
#define   V_FCH_REVISION_A12                           0x01
#define   V_FCH_REVISION_A21                           0x03
#define   V_FCH_REVISION_A23                           0x04
#define   V_FCH_REVISION_A31                           0x10
#define   V_FCH_REVISION_A32                           0x11
#define   V_FCH_REVISION_A11                           0x80
#define   V_FCH_REVISION_Axx                           0x81


//
// IDE Controller ( D:20, F:1 )
//
#define R_FCH_IDE_HC_OPERATION_MODE_SELECTION        0x09
#define   B_MASTER_IDE_DEVICE                         BIT7
#define   B_SECONDARY_IDE_ENABLE                      BIT3
#define     V_SECONDARY_IDE_ENABLE                    BIT3
#define     V_SECONDARY_IDE_DISABLE                   0
#define   B_SECONDARY_IDE_OPERATION_MODE              BIT2
#define     V_SECONDARY_IDE_OPERATION_MODE_NATIVE     BIT2
#define     V_SECONDARY_IDE_OPERATION_MODE_COMPATIBLE 0
#define   B_PRIMARY_IDE_ENABLE                        BIT1
#define     V_PRIMARY_IDE_ENABLE                      BIT1
#define     V_PRIMARY_IDE_DISABLE                     0
#define   B_PRIMARY_IDE_OPERATION_MODE                BIT0
#define     V_PRIMARY_IDE_OPERATION_MODE_NATIVE       BIT0
#define     V_PRIMARY_IDE_OPERATION_MODE_COMPATIBLE   0

#define R_FCH_IDE_BUS_MASTER_INTERFACE_BASE_ADDRESS  0x20
#define   B_IDE_BUS_MASTER_INTERFACE_BASE_ADDRESS     0xFFF0        //BIT4~15

#define R_FCH_IDE_MSI_PROGRAMMABLE_WEIGHT            0x68
#define   V_MSI_INTERRUPT_WEIGHT                      0x01

#define R_FCH_IDE_DYNAMIC_CLOCKING                   0x6C
#define   B_IDE_POWER_DOWN_COUNTER                    0x000FFFFFh
#define R_FCH_IDE_REG70                              0x70

//
// HD Audio Controller ( D:20, F:2 )
//
#define     V_AZALIA_BAR1_SIZE                        0x4000

#define R_FCH_AZALIA_MISC_CONTROL                    0x40
#define   B_STATIC_OUTPUT_FIFO_SIZE_SELECT            (BIT0 + BIT1)
#define     V_STATIC_OUTPUT_FIFO_SIZE_ONE_EIGHT       0
#define     V_STATIC_OUTPUT_FIFO_SIZE_ONE_QUARTER     BIT0
#define     V_STATIC_OUTPUT_FIFO_SIZE_ONE_HALF        BIT1
#define     V_STATIC_OUTPUT_FIFI_SIZE_MAXIMUM         (BIT0 + BIT1)
#define   B_STATIC_OUTPUT_FIFO_SIZE_ENABLE            BIT8
#define     V_STATIC_OUTPUT_FIFO_SIZE_ENABLE          BIT8
#define     V_STATIC_OUTPUT_FIFO_SIZE_DISABLE         0

#define R_FCH_AZALIA_MISC_CONTROL_2                  0x42
#define   B_DISABLE_NO_SNOOP                          BIT0
#define     V_DISABLE_NO_SNOOP                        BIT0
#define     V_ENABLE_NO_SNOOP                         0
#define   B_DISABLE_NO_SNOOP_OVERRIDE                 BIT1
#define     V_DISABLE_NO_SNOOP_OVERRIDE               BIT1
#define     V_ENABLE_NO_SNOOP_OVERRIDE                0
#define   B_ENABLE_NO_SNOOP_REQUEST                   BIT2
#define     V_ENABLE_NO_SNOOP_REQUEST                 BIT2
#define     V_DISABLE_NO_SNOOP_REQUEST                0

#define R_FCH_AZALIA_MISC_CONTROL_3                  0x43
#define   B_48MHZ_CLOCK_SOURCE_SELECT                 BIT0
#define     V_48MHZ_CLOCK_SOURCE_SELECT_USBCLK        0
#define     V_48MHZ_CLOCK_SOURCE_SELECT_AZ_48M        BIT0

#define R_FCH_AZALIA_INT_PIN_CONTROL                 0x44
#define   B_FCH_AZALIA_INT_PIN_CONTROL                 0x0F

  //
  // HD Audio Controller Memory Base Register
  //
#define R_FCH_AZALIA_MEMORY_GLOBAL_CONTROL           0x08
#define   B_CONTROLLER_RESET                          BIT0
#define     V_CONTROLLER_RESET_EXIT                   BIT0

#define R_FCH_AZALIA_MEMORY_STATE_CHAGNE_STATUS      0x0E
#define   B_AZALIA_CODEC0_CHANGE                      BIT0
#define   B_AZALIA_CODEC1_CHANGE                      BIT1
#define   B_AZALIA_CODEC2_CHANGE                      BIT2
#define   B_AZALIA_CODEC3_CHANGE                      BIT3

#define R_FCH_AZALIA_MEMORY_IMMEDIATE_COMMAND_OUTPUT 0x60
#define R_FCH_AZALIA_MEMORY_IMMEDIATE_COMMAND_INPUT  0x64

#define R_FCH_AZALIA_MEMORY_IMMEDIATE_COMMAND_STATUS 0x68
#define   B_IMMEDIATE_COMMAND_STATUS                  BIT0
#define     V_IMMEDIATE_COMMAND_ACCEPT                0
#define     V_IMMEDIATE_COMMAND_DENIED                BIT0


//
// LPC Controller ( D:20, F:3 )
//

#define R_FCH_LPC_PCI_CONTROL                        0x40
#define   B_DMA_ENABLE                                BIT2
#define     V_DMA_ENABLE                              BIT2
#define     V_DMA_DISABLE                             0

#define R_FCH_LPC_IO_PORT_DECODE_ENABLE_REGISTER1    0x44
#define   B_PARALLEL_PORT_ENABLE_0                    BIT0
#define     V_PARALLEL_PORT_ENABLE_0_ENABLE           BIT0
#define     V_PARALLEL_PORT_ENABLE_0_DISABLE          0
#define   B_PARALLEL_PORT_ENABLE_1                    BIT1
#define     V_PARALLEL_PORT_ENABLE_1_ENABLE           BIT1
#define     V_PARALLEL_PORT_ENABLE_1_DISABLE          0
#define   B_PARALLEL_PORT_ENABLE_2                    BIT2
#define     V_PARALLEL_PORT_ENABLE_2_ENABLE           BIT2
#define     V_PARALLEL_PORT_ENABLE_2_DISABLE          0
#define   B_PARALLEL_PORT_ENABLE_3                    BIT3
#define     V_PARALLEL_PORT_ENABLE_3_ENABLE           BIT3
#define     V_PARALLEL_PORT_ENABLE_3_DISABLE          0
#define   B_PARALLEL_PORT_ENABLE_4                    BIT4
#define     V_PARALLEL_PORT_ENABLE_4_ENABLE           BIT4
#define     V_PARALLEL_PORT_ENABLE_4_DISABLE          0
#define   B_PARALLEL_PORT_ENABLE_5                    BIT5
#define     V_PARALLEL_PORT_ENABLE_5_ENABLE           BIT5
#define     V_PARALLEL_PORT_ENABLE_5_DISABLE          0
#define   B_SERIAL_PORT_ENABLE_0                      BIT6
#define     V_SERIAL_PORT_ENABLE_0_ENABLE             BIT6
#define     V_SERIAL_PORT_ENABLE_0_DISABLE            0
#define   B_SERIAL_PORT_ENABLE_1                      BIT7
#define     V_SERIAL_PORT_ENABLE_1_ENABLE             BIT7
#define     V_SERIAL_PORT_ENABLE_1_DISABLE            0

#define R_FCH_LPC_IO_PORT_DECODE_ENABLE_REGISTER2    0x45
#define   B_SERIAL_PORT_ENABLE_2                      BIT0
#define     V_SERIAL_PORT_ENABLE_2_ENABLE             BIT0
#define     V_SERIAL_PORT_ENABLE_2_DISABLE            0
#define   B_SERIAL_PORT_ENABLE_3                      BIT1
#define     V_SERIAL_PORT_ENABLE_3_ENABLE             BIT1
#define     V_SERIAL_PORT_ENABLE_3_DISABLE            0
#define   B_SERIAL_PORT_ENABLE_4                      BIT2
#define     V_SERIAL_PORT_ENABLE_4_ENABLE             BIT2
#define     V_SERIAL_PORT_ENABLE_4_DISABLE            0
#define   B_SERIAL_PORT_ENABLE_5                      BIT3
#define     V_SERIAL_PORT_ENABLE_5_ENABLE             BIT3
#define     V_SERIAL_PORT_ENABLE_5_DISABLE            0
#define   B_SERIAL_PORT_ENABLE_6                      BIT4
#define     V_SERIAL_PORT_ENABLE_6_ENABLE             BIT4
#define     V_SERIAL_PORT_ENABLE_6_DISABLE            0
#define   B_SERIAL_PORT_ENABLE_7                      BIT5
#define     V_SERIAL_PORT_ENABLE_7_ENABLE             BIT5
#define     V_SERIAL_PORT_ENABLE_7_DISABLE            0
#define   B_AUDIO_PORT_ENABLE_0                       BIT6
#define     V_AUDIO_PORT_ENABLE_0_ENABLE              BIT6
#define     V_AUDIO_PORT_ENABLE_0_DISABLE             0
#define   B_AUDIO_PORT_ENABLE_1                       BIT7
#define     V_AUDIO_PORT_ENABLE_1_ENABLE              BIT7
#define     V_AUDIO_PORT_ENABLE_1_DISABLE             0

#define R_FCH_LPC_IO_PORT_DECODE_ENABLE_REGISTER3    0x46
#define   B_AUDIO_PORT_ENABLE_2                       BIT0
#define     V_AUDIO_PORT_ENABLE_2_ENABLE              BIT0
#define     V_AUDIO_PORT_ENABLE_2_DISABLE             0
#define   B_AUDIO_PORT_ENABLE_3                       BIT1
#define     V_AUDIO_PORT_ENABLE_3_ENABLE              BIT1
#define     V_AUDIO_PORT_ENABLE_3_DISABLE             0
#define   B_MIDI_PORT_ENABLE_0                        BIT2
#define     V_MIDI_PORT_ENABLE_0_ENABLE               BIT2
#define     V_MIDI_PORT_ENABLE_0_DISABLE              0
#define   B_MIDI_PORT_ENABLE_1                        BIT3
#define     V_MIDI_PORT_ENABLE_1_ENABLE               BIT3
#define     V_MIDI_PORT_ENABLE_1_DISABLE              0
#define   B_MIDI_PORT_ENABLE_2                        BIT4
#define     V_MIDI_PORT_ENABLE_2_ENABLE               BIT4
#define     V_MIDI_PORT_ENABLE_2_DISABLE              0
#define   B_MIDI_PORT_ENABLE_3                        BIT5
#define     V_MIDI_PORT_ENABLE_3_ENABLE               BIT5
#define     V_MIDI_PORT_ENABLE_3_DISABLE              0
#define   B_MSS_PORT_ENABLE_0                         BIT6
#define     V_MSS_PORT_ENABLE_0_ENABLE                BIT6
#define     V_MSS_PORT_ENABLE_0_DISABLE               0
#define   B_MSS_PORT_ENABLE_1                         BIT7
#define     V_MSS_PORT_ENABLE_1_ENABLE                BIT7
#define     V_MSS_PORT_ENABLE_1_DISABLE               0

#define R_FCH_LPC_IO_PORT_DECODE_ENABLE_REGISTER4    0x47
#define   B_MSS_PORT_ENABLE_2                         BIT0
#define     V_MSS_PORT_ENABLE_2_ENABLE                BIT0
#define     V_MSS_PORT_ENABLE_2_DISABLE               0
#define   B_MSS_PORT_ENABLE_3                         BIT1
#define     V_MSS_PORT_ENABLE_3_ENABLE                BIT1
#define     V_MSS_PORT_ENABLE_3_DISABLE               0
#define   B_FDC_PORT_ENABLE_0                         BIT2
#define     V_FDC_PORT_ENABLE_0_ENABLE                BIT2
#define     V_FDC_PORT_ENABLE_0_DISABLE               0
#define   B_FDC_PORT_ENABLE_1                         BIT3
#define     V_FDC_PORT_ENABLE_1_ENABLE                BIT3
#define     V_FDC_PORT_ENABLE_1_DISABLE               0
#define   B_GAME_PORT_ENABLE                          BIT4
#define     V_GAME_PORT_ENABLE                        BIT4
#define     V_GAME_PORT_DISABLE                       0
#define   B_KBC_PORT_ENABLE                           BIT5
#define     V_KBC_PORT_ENABLE                         BIT5
#define     V_KBC_PORT_DISABLE                        0
#define   B_ACPI_MICRO_CONTROLLER_PORT_ENABLE         BIT6
#define     V_ACPI_MICRO_CONTROLLER_PORT_ENABLE       BIT6
#define     V_ACPI_MICRO_CONTROLLER_PORT_DISABLE      0
#define   B_AD_PID_PORT_ENABLE                        BIT7
#define     V_AD_PID_PORT_ENABLE                      BIT7
#define     V_AD_PID_PORT_DISABLE                     0

#define R_FCH_LPC_IO_PORT_DECODE_ENABLE_REGISTER5    0x48
#define   B_SIO_CFG_PORT_ENABLE                       BIT0
#define     V_SIO_CFG_PORT_ENABLE                     BIT0
#define     V_SIO_CFG_PORT_DISABLE                    0
#define   B_ALTEMATE_SIO_CFG_PORT_ENABLE              BIT1
#define     V_ALTERMATE_SIO_CFG_PORT_ENABLE           BIT1
#define     V_ALTERMATE_SIO_CFG_PORT_DISABLE          0
#define   B_WIDE_GENERIC_IO_PORT_ENABLE               BIT2
#define     V_WIDE_GENERIC_IO_PORT_ENABLE             BIT2
#define     V_WIDE_GENERIC_IO_PORT_DISABLE            0
#define   B_ROM_RANGE1_PORT_ENABLE                    BIT3
#define     V_ROM_RANGE1_PORT_ENABLE                  BIT3
#define     V_ROM_RANGE1_PORT_DISABLE                 0
#define   B_ROM_RANGE2_PORT_ENABLE                    BIT4
#define     V_ROM_RANGE2_PORT_ENABLE                  BIT4
#define     V_ROM_RANGE2_PORT_DISABLE                 0
#define   B_MEMORY_RANGE_PORT_ENABLE                  BIT5
#define     V_MEMORY_RANGE_PORT_ENABLE                BIT5
#define     V_MEMORY_RANGE_PORT_DISABLE               0
#define   B_RTC_IO_RANGE_PORT_ENABLE                  BIT6
#define     V_RTC_IO_RANGE_PORT_ENABLE                BIT6
#define     V_RTC_IO_RANGE_PORT_DISABLE               0
#define   B_SYNC_TIMEOUT_COUNTER_ENABLE               BIT7
#define     V_SYNC_TIMEOUT_COUNTER_ENABLE             BIT7
#define     V_SYNC_TIMEOUT_COUNTER_DISABLE            0

#define R_FCH_LPC_SYNC_TIMEOUT_COUNT                 0x49

#define R_FCH_LPC_IOMEM_PORT_DECODE_ENABLE_REGISTER6 0x4A
#define   B_IO_PORT_ENABLE0                           BIT0
#define     V_IO_PORT_ENABLE0_ENABLE                  BIT0
#define     V_IO_PORT_ENABLE0_DISABLE                 0
#define   B_IO_PORT_ENABLE1                           BIT1
#define     V_IO_PORT_ENABLE1_ENABLE                  BIT1
#define     V_IO_PORT_ENABLE1_DISABLE                 0
#define   B_IO_PORT_ENABLE2                           BIT2
#define     V_IO_PORT_ENABLE2_ENABLE                  BIT2
#define     V_IO_PORT_ENABLE2_DISABLE                 0
#define   B_IO_PORT_ENABLE3                           BIT3
#define     V_IO_PORT_ENABLE3_ENABLE                  BIT3
#define     V_IO_PORT_ENABLE3_DISABLE                 0
#define   B_MEM_PORT_ENABLE                           BIT4
#define     V_MEM_PORT_ENABLE                         BIT4
#define     V_MEM_PORT_DISABLE                        0
#define   B_IO_PORT_ENABLE4                           BIT5
#define     V_IO_PORT_ENABLE4_ENABLE                  BIT5
#define     V_IO_PORT_ENABLE4_DISABLE                 0
#define   B_IO_PORT_ENABLE5                           BIT6
#define     V_IO_PORT_ENABLE5_ENABLE                  BIT6
#define     V_IO_PORT_ENABLE5_DISABLE                 0
#define   B_IO_PORT_ENABLE6                           BIT7
#define     V_IO_PORT_ENABLE6_ENABLE                  BIT7
#define     V_IO_PORT_ENABLE6_DISABLE                 0

#define R_FCH_LPC_IOMEM_PORT_DECODE_ENABLE_REGISTER7 0x4B
#define   B_WIDE_IO_1_ENABLE                          BIT0
#define     V_WIDE_IO_1_ENABLE                        BIT0
#define     V_WIDE_IO_1_DISALBE                       0
#define   B_WIDE_IO_2_ENABLE                          BIT1
#define     V_WIDE_IO_2_ENABLE                        BIT1
#define     V_WIDE_IO_2_DISABLE                       0

#define R_FCH_LPC_MEMORY_RANGE_REGISTER              0x4C

#define R_FCH_LPC_ROM_PROTECT0                       0x50
#define R_FCH_LPC_ROM_PROTECT1                       0x54
#define R_FCH_LPC_ROM_PROTECT2                       0x58
#define R_FCH_LPC_ROM_PROTECT3                       0x5C
#define   B_ROM_WRITE_PROTECT                         BIT0
#define     V_ROM_WRITE_PROTECT_ENABLE                BIT0
#define     V_ROM_WRITE_PROTECT_DISABLE               0
#define   B_ROM_READ_PROTECT                          BIT1
#define     V_ROM_READ_PROTECT_ENABLE                 BIT1
#define     V_ROM_READ_PROTECT_DISABLE                0

#define R_FCH_LPC_PCI_MEMORY_START_ADDRESS_FOR_LPC_TARGET_CYCLES 0x60
#define R_FCH_LPC_PCI_MEMORY_END_ADDRESS_FOR_LPC_TARGET_CYCLES 0x62

#define R_FCH_LPC_PCI_IO_BASE_ADDRESS_FOR_WIDE_GENERIC_PORT 0x64

#define R_FCH_LPC_ROM_ADDRESS_RANGE1_START           0x68
#define R_FCH_LPC_ROM_ADDRESS_RANGE1_END             0x6A

#define R_FCH_LPC_ROM_ADDRESS_RANGE2_START           0x6C
#define R_FCH_LPC_ROM_ADDRESS_RANGE2_END             0x6E

#define R_FCH_LPC_FIRMWARE_HUB_SELECT                0x70

#define R_FCH_LPC_ALTERNATIVE_WIDE_IO_RANGE_ENABLE   0x74
#define   B_ALTERNATIVE_WIDE_TO_RANGE_ENABLE          BIT0
#define     V_ALTERNATIVE_WIDE_TO_RANGE_ENABLE        BIT0
#define     V_ALTERNATIVE_WIDE_TO_RANGE_DISABLE       0

#define R_FCH_LPC_MISC_CONTROL_BITS                  0x78
#define   B_NO_HOG                                    BIT0
#define     V_NO_HOG_ENABLE                           BIT0
#define     V_NO_HOG_DISABLE                          0
#define   B_MSI_ON                                    BIT1
#define     V_MSI_ON_ENABLE                           BIT1
#define     V_MSI_ON_DISABLE                          0

#define R_FCH_LPC_TPM_REGISTER                       0x7C
#define   B_TPM12_EN                                  BIT0
#define     V_TPM12_ENABLE                            BIT0
#define     V_TPM12_DISABLE                           0
#define   B_TPM_AMD                                   BIT1
#define     V_TPM_AMD_ENABLE                          BIT1
#define     V_TPM_AMD_DISABLE                         0
#define   B_TPM_LEGACY                                BIT2
#define     V_TPM_LEGACY_ENABLE                       BIT2
#define     V_TPM_LEGACY_DISABLE                      0
#define   B_WIDER_TPM_EN                              BIT7
#define     V_WIDER_TPM_ENABLE                        BIT7
#define     V_WIDER_TPM_DISABLE                       0

#define R_FCH_LPC_WIDE_IO_2_REGISTER                 0x90
#define R_FCH_GECROM_SHADOW_BASE_ADDRESS             0x9C
#define R_FCH_LPC_SPI_BASE_ADDRESS                   0xA0
#define     V_ROUTE_TPM_TO_SPI                        BIT3
//
// HOST PCI Controller ( D:20, F:4 )
//

#define R_FCH_PCI_PBN                                0x18
#define R_FCH_PCI_SBN                                0x19
#define R_FCH_PCI_SUBBN                              0x1A

#define R_FCH_PCI_CPCTRL                             0x40
#define   B_MEM_WRITE_SIZE_CTRL                       BIT1
#define     V_MEM_WRITE_SIZE_CTRL_CACHELINE           BIT1
#define     V_MEM_WRITE_SIZE_CTRL_32BYTE              0
#define   B_LOCK_ENABLE                               BIT2
#define     V_LOCK_ENABLE                             BIT2
#define     V_LOCK_DISABLE                            0
#define   B_MSI_CAP_ENABLE                            BIT3
#define     V_MSI_CAP_ENABLE                          BIT3
#define     V_MSI_CAP_DISABLE                         0
#define   B_MEM_RD_PREFETCH_DISABLE                   BIT4
#define     V_MEM_RD_PREFETCH_DISABLE                 BIT4
#define     V_MEM_RD_PREFETCH_ENABLE                  0
#define   B_CPCTRL_SUB_DECODE_ENABLE                  BIT5
#define     V_CPCTRL_SUB_DECODE_ENABLE                BIT5
#define     V_CPCTRL_SUB_DECODE_DISABLE               0
#define   B_BRIDGE_LOCK_STATE                         (BIT6 + BIT7)
#define     V_BRIDGE_LOCK_STATE_FREE                  0
#define     V_BRIDGE_LOCK_STATE_BUSY                  BIT6
#define     V_BRIDGE_LOCK_STATE_REQ                   BIT7
#define     V_BRIDGE_LOCK_STATE_LOCKED                (BIT6 + BIT7)

#define R_FCH_PCI_REG42                              0x42

#define R_FCH_PCI_ARCTRL                             0x43
#define   B_REQ0_PRIORITY                             BIT0
#define     V_REQ0_PRIORITY_HIGH                      BIT0
#define   B_REQ1_PRIORITY                             BIT1
#define     V_REQ1_PRIORITY_HIGH                      BIT1
#define   B_REQ2_PRIORITY                             BIT2
#define     V_REQ2_PRIORITY_HIGH                      BIT2
#define   B_REQ3_PRIORITY                             BIT3
#define     V_REQ3_PRIORITY_HIGH                      BIT3
#define   B_REQ4_PRIORITY                             BIT4
#define     V_REQ4_PRIORITY_HIGH                      BIT4
#define   B_REQ5_PRIORITY                             BIT5
#define     V_REQ5_PRIORITY_HIGH                      BIT5
#define   B_ROM_PRIORITY                              BIT6
#define     V_ROM_PRIORITY_HIGH                       BIT6
#define   B_ARBITER_ENABLE                            BIT7
#define     V_ARBITER_ENABLE                          BIT7
#define     V_ARBITER_DISABLE                         0

#define R_FCH_PCI_PCDMA                              0x48
#define   B_PCDMA_DEVICE_ENABLE_A                     BIT0
#define   B_PCDMA_DEVICE_ENABLE_B                     BIT1
#define   B_FAST_BACK_TO_BACK_RETRY_ENABLE            BIT2
#define     V_FAST_BACK_TO_BACK_RETRY_DISABLE         0
#define     V_FAST_BACK_TO_BACK_RETRY_ENABLE          BIT2
#define   B_LOCK_OPERATION_ENABLE                     BIT3
#define     V_LOCK_OPERAtiON_ENABLE                   BIT3
#define     V_LOCK_OPERATION_DISABLE                  0

#define R_FCH_PCI_ADDITIONAL_PRIORITY                0x49
#define   B_REQ6_PRIORITY                             BIT0
#define     V_REQ6_PRIORITY_HIGH                      BIT0
#define   B_PCDMA_PRIORITY                            BIT1
#define     V_PCDMA_PRIORITY_HIGH                     BIT1

#define R_FCH_PCI_REG4A                              0x4A

#define R_FCH_PCI_MISC_CONTROL                       0x4B
#define   B_GNT_BUS_IDLE_CHECK_ENABLE                 BIT0
#define     V_GNT_BUS_IDLE_CHECK_ENABLE               BIT0
#define     V_GNT_BUS_IDLE_CHECK_DISABLE              0
#define   B_BURST_SIZE_2DW                            BIT1
#define     V_BURST_SIZE_2DW_ENABLE                   BIT1
#define     V_BURST_SIZE_2DW_DISABLE                  0
#define   B_BURST_SIZE_4DW                            BIT2
#define     V_BURST_SIZE_4DW_ENABLE                   BIT2
#define     V_BURST_SIZE_4DW_DISABLE                  0
#define   B_BURST_SIZE_8DW                            BIT3
#define     V_BURST_SIZE_8DW_ENABLE                   BIT3
#define     V_BURST_SIZE_8DW_DISABLE                  0
#define   B_BURST_SIZE_16DW                           BIT4
#define     V_BURST_SIZE_16DW_ENABLE                  BIT4
#define     V_BURST_SIZE_16DW_DISABLE                 0
#define   B_IO_MODE                                   BIT5
#define     V_IO_MODE_16BIT                           0
#define     V_IO_MODE_32BIT                           BIT5
#define   B_MEM_READ_CMD_MATCH                        BIT6
#define     V_MEM_READ_CMD_MATCH_ENABLE               BIT6
#define     V_MEM_READ_CMD_MATCH_DISABLE              0
#define   B_SUB_DECODE_ENABLE                         BIT7
#define     V_SUB_DECODE_ENABLE                       BIT7
#define     V_SUB_DECODE_DISABLE                      0

#define R_FCH_PCI_REG4C                              0x4C

#define R_FCH_PCI_DUAL_ADDRESS_CYCLE_ENABLE          0x50
#define   B_PCIB_DUAL_EN_UP                           BIT0
#define     V_PCIB_DUAL_EN_UP_ENABLE                  BIT0
#define     V_PCIB_DUAL_EN_UP_DISABLE                 0
#define   B_PCIB_DUAL_EN_DN                           BIT1
#define     V_PCIB_DUAL_EN_DN_ENABLE                  BIT1
#define     V_PCIB_DUAL_EN_DN_DISABLE                 0
#define   B_CLKRUN_OVERRIDE_PCICLK                    BIT6
#define     V_CLKRUN_OVERRIDE_PCICLK_ENABLE           BIT6
#define     V_CLKRUN_OVERRIDE_PCICLK_DISABLE          0
#define   B_CLKRUN_OVERRIDE_PCICLK1                   BIT7
#define     V_CLKRUN_OVERRIDE_PCICLK1_ENABLE          BIT7
#define     V_CLKRUN_OVERRIDE_PCICLK1_DISABLE         0
#define   B_CLKRUN_OVERRIDE_PCICLK2                   BIT8
#define     V_CLKRUN_OVERRIDE_PCICLK2_ENABLE          BIT8
#define     V_CLKRUN_OVERRIDE_PCICLK2_DISABLE         0
#define   B_CLKRUN_OVERRIDE_PCICLK3                   BIT9
#define     V_CLKRUN_OVERRIDE_PCICLK3_ENABLE          BIT9
#define     V_CLKRUN_OVERRIDE_PCICLK3_DISABLE         0
#define   B_CLKRUN_OVERRIDE_PCICLK4                   BIT10
#define     V_CLKRUN_OVERRIDE_PCICLK4_ENABLE          BIT10
#define     V_CLKRUN_OVERRIDE_PCICLK4_DISABLE         0
#define   B_CLKRUN_OVERRIDE_LPCCLK                    BIT11
#define     V_CLKRUN_OVERRIDE_LPCCLK_ENABLE           BIT11
#define     V_CLKRUN_OVERRIDE_LPCCLK_DISABLE          0
#define   B_CLKRUN_OVERRIDE_LPCCLK1                   BIT12
#define     V_CLKRUN_OVERRIDE_LPCCLK1_ENABLE          BIT12
#define     V_CLKRUN_OVERRIDE_LPCCLK1_DISABLE         0
#define   B_CLKRUN_OVERRIDE_PCICLK7                   BIT13
#define     V_CLKRUN_OVERRIDE_PCICLK7_ENABLE          BIT13
#define     V_CLKRUN_OVERRIDE_PCICLK7_DISABLE         0
#define   B_CLKRUN_OVERRIDE_PCICLK8                   BIT14
#define     V_CLKRUN_OVERRIDE_PCICLK8_ENABLE          BIT14
#define     V_CLKRUN_OVERRIDE_PCICLK8_DISABLE         0
#define   B_CLKRUN_OVERRIDE_PCICLK9                   BIT15
#define     V_CLKRUN_OVERRIDE_PCICLK9_ENABLE          BIT15
#define     V_CLKRUN_OVERRIDE_PCICLK9_DISABLE         0

#define R_FCH_PCI_MISC_CONTROL_REGISTER              0x64
#define   B_DOWNSTREAM_CONFIG_CYCLE_FLUSH_ENABLE      BIT0
#define     V_DOWNSTREAM_CONFIG_CYCLE_FLUSH_ENABLE    BIT0
#define     V_DOWNSTREAM_CONFIG_CYCLE_FLUSH_DISABLE   0
#define   B_DOWNSTREAM_WRITE_CYCLE_FLUSH_ENABLE       BIT1
#define     V_DOWNSTREAM_WRITE_CYCLE_FLUSH_ENABLE     BIT1
#define     V_DOWNSTREAM_WRITE_CYCLE_FLUSH_DISABLE    0
#define   B_DOWNSTREAM_READ_CYCLE_FLUSH_ENABLE        BIT2
#define     V_DOWNSTREAM_READ_CYCLE_FLUSH_ENABLE      BIT2
#define     V_DOWNSTREAM_READ_CYCLE_FLUSH_DISABLE     0
#define   B_PREFETCH_BUFFER_TIMEOUT_ENABLE            BIT3
#define     V_PREFETCH_BUFFER_TIMEOUT_ENABLE          BIT3
#define     V_PREFETCH_BUFFER_TIMEOUT_DISABLE         0
#define   B_AB_MASKING_PREFETCH_REQUEST_ENABLE        BIT4
#define     V_AB_MASKING_PREFETCH_REQUEST_ENABLE      BIT4
#define     V_AB_MASKING_PREFETCH_REQUEST_DISABLE     0
#define   B_AB_MASKING_NONPREFETCH_REQUEST_ENABLE     BIT5
#define     V_AB_MASKING_NONPREFETCH_REQUEST_ENABLE   BIT5
#define     V_AB_MASKING_NONPREFETCH_REQUEST_DISABLE  0
#define   B_DOWNSTREAM_CYCLE_FLUSH_CONTROL            BIT6
#define   B_PREFETCH_ENABLE                           BIT7
#define     V_PREFETCH_ENABLE                         BIT7
#define     V_PREFETCH_DISABLE                        0
#define   B_PCI5_ENABLE                               BIT8
#define     V_PCI5_ENABLE                             BIT8
#define     V_PCI5_DISABLE                            0
#define   B_ARBITER_2_ENABLE                          BIT9
#define     V_ARBITER_2_ENABLE                        BIT9
#define     V_ARBITER_2_DISABLE                       0
#define   B_HOLD_CURRENT_GRANT                        BIT10
#define     V_HOLD_CURRENT_GRANT_ENABLE               BIT10
#define     V_HOLD_CURRENT_GRANT_DISABLE              0
#define   B_SINGLE_CYCLE_PREFETCH_CONTROL             BIT11
#define     V_SINGLE_CYCLE_PREFETCH_CONTROL_ENABLE    BIT11
#define     V_SINGLE_CYCLE_PREFETCH_CONTROL_DISABLE   0
#define   B_FAST_GRANT_DEASSERT                       BIT12
#define     V_FAST_GRANT_DEASSERT_ENABLE              BIT12
#define     V_FAST_GRANT_DEASSERT_DISABLE             0

//
// South Bridge I/O Resource
//
#define FCH_PORT_92              0x92
// FORREST CHOI
#define ACPI_SMI_CMD_PORT        0xB0
#define ACPI_SMI_DATA_PORT       0xB1

#define IRQ_ROUTING_INDEX_PORT   0xC00
#define IRQ_ROUTING_DATA_PORT    0xC01

#define FCH_GPM_PORT             0xC52
#define FCH_ISA_MISC_PORT        0xC6F

#define FCH_PM2_INDEX_PORT       0xCD0
#define FCH_PM2_DATA_PORT        0xCD1

#define FCH_BIOS_RAM_INDEX_PORT  0xCD4
#define FCH_BIOS_RAM_DATA_PORT   0xCD5

#define FCH_PM_INDEX_PORT        0xCD6
#define FCH_PM_DATA_PORT         0xCD7

#define FCH_ALINK_INDEX_PORT     0xCD8
#define FCH_ALINK_DATA_PORT      (FCH_ALINK_INDEX_PORT + 4)

//
// South Bridge Programable IRQ Routing Table (C00/C01)
//
#define V_IRQ0                            0x00
#define V_IRQ1                            0x01
#define V_IRQ2                            0x02
#define V_IRQ3                            0x03
#define V_IRQ4                            0x04
#define V_IRQ5                            0x05
#define V_IRQ6                            0x06
#define V_IRQ7                            0x07
#define V_IRQ8                            0x08
#define V_IRQ9                            0x09
#define V_IRQ10                           0x0a
#define V_IRQ11                           0x0b
#define V_IRQ12                           0x0c
#define V_IRQ13                           0x0d
#define V_IRQ14                           0x0e
#define V_IRQ15                           0x0f

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

#define R_FCH_ACPI_TMR_VALUE               0x08
#define R_FCH_ACPI_CLK_VALUE               0x10
#define R_FCH_ACPI_PLVL_2                  (R_FCH_ACPI_CLK_VALUE + 0x04)
#define R_FCH_ACPI_PLVL_3                  (R_FCH_ACPI_CLK_VALUE + 0x05)
#define R_FCH_ACPI_PLVL_4                  (R_FCH_ACPI_CLK_VALUE + 0x06)


#define R_FCH_ACPI_EVENT_STATUS            0x20
#define   B_G0_EVENT_STATUS                 BIT0
#define   B_G1_EVENT_STATUS                 BIT1
#define   B_G2_EVENT_STATUS                 BIT2
#define   B_G3_EVENT_STATUS                 BIT3
#define   B_G4_EVENT_STATUS                 BIT4
#define   B_G5_EVENT_STATUS                 BIT5
#define   B_G6_EVENT_STATUS                 BIT6
#define   B_G7_EVENT_STATUS                 BIT7
#define   B_L_EVENT_STATUS                  BIT8
#define   B_T_WARM_STATUS                   BIT9
#define   B_USB_STATUS                      BIT11
#define   N_USB_STATUS                      11
#define   B_AC97_STATUS                     BIT12
#define   B_FAKE_AB_PME_STATUS              (BIT13 + BIT14 + BIT15)
#define   B_EXT_EVENT_0_STATUS              BIT16
#define   B_EXT_EVENT_1_STATUS              BIT17
#define   B_PCI_EXPRESS_STATUS              BIT18
#define   B_GPM0_STATUS                     BIT19
#define   B_GPM1_STATUS                     BIT20
#define   B_GPM2_STATUS                     BIT21
#define   B_GPM3_STATUS                     BIT22
#define   B_GPIO0_STATUS                    BIT24
#define   B_GPM4_STATUS                     BIT25
#define   B_GPM5_STATUS                     BIT26
#define   B_AZALIA_STATUS                   BIT27
#define   B_GPM6_STATUS                     BIT28
#define   B_GPM7_STATUS                     BIT29
#define   B_GPIO2_STATUS                    BIT30
#define   B_SATA_SCI_STATUS                 BIT31
#define   S_FCH_ACPI_EVENT_STATUS            0x4

#define R_FCH_ACPI_EVENT_ENABLE            0x24
#define   B_G0_EVENT_ENABLE                 BIT0
#define   B_G1_EVENT_ENABLE                 BIT1
#define   B_G2_EVENT_ENABLE                 BIT2
#define   B_G3_EVENT_ENABLE                 BIT3
#define   B_G4_EVENT_ENABLE                 BIT4
#define   B_G5_EVENT_ENABLE                 BIT5
#define   B_G6_EVENT_ENABLE                 BIT6
#define   B_G7_EVENT_ENABLE                 BIT7
#define   B_L_EVENT_ENABLE                  BIT8
#define   B_T_WARM_ENABLE                   BIT9
#define   B_USB_ENABLE                      BIT11
#define   N_USB_ENABLE                      11
#define   B_AC97_ENABLE                     BIT12
#define   B_FAKE_AB_PME_ENABLE              (BIT13 + BIT14 + BIT15)
#define   B_EXT_EVENT_0_ENABLE              BIT16
#define   B_EXT_EVENT_1_ENABLE              BIT17
#define   B_PCI_EXPRESS_ENABLE              BIT18
#define   B_GPM0_ENABLE                     BIT19
#define   B_GPM1_ENABLE                     BIT20
#define   B_GPM2_ENABLE                     BIT21
#define   B_GPM3_ENABLE                     BIT22
#define   B_GPIO0_ENABLE                    BIT24
#define   B_GPM4_ENABLE                     BIT25
#define   B_GPM5_ENABLE                     BIT26
#define   B_AZALIA_ENABLE                   BIT27
#define   B_GPM6_ENABLE                     BIT28
#define   B_GPM7_ENABLE                     BIT29
#define   B_GPIO2_ENABLE                    BIT30
#define   B_SATA_SCI_ENABLE                 BIT31
#define   S_FCH_ACPI_EVENT_ENABLE            0x4

#define R_FCH_ACPI_SS_CNT                  0x30

#define R_FCH_ACPI_PMA_CONTROL             0x80

#define R_FCH_ALINK_AXCFG_REGSPACE         0x80000000
#define R_FCH_ALINK_AX_COMMAND               0x04
#define   B_BUS_MASTER_EN                   BIT2
#define     V_BUS_MASTER_DISABLE            0
#define     V_BUS_MASTER_ENABLE             BIT2

#define R_FCH_ALINK_AX_INDXC		            0x30
#define R_FCH_ALINK_AX_DATAC               0x34
#define   R_FCH_ALINK_PCIE_CTRL              0x10
#define     B_PCIE_HT_NP_MEM_WRITE          BIT9
#define       V_PCIE_HT_NP_MEM_WRITE_EN     BIT9
#define       V_PCIE_HT_NP_MEM_WRITE_DIS    0
#define   R_FCH_ALINK_AXINDC_PCIE_BUS_CTL    0x21
#define     B_BUS_DBL_RESYNC                BIT0
#define       V_BUS_DBL_RESYNC_ENABLE       BIT0
#define       V_BUS_DBL_RESYNC_DISABLE      0

#define R_FCH_ALINK_ABCFG_REGSPACE         0xC0000000
#define R_FCH_ALINK_RAB_CONTROL              0x58
#define R_FCH_ALINK_DMA_PREFETCH_EN0         0x60
#define R_FCH_ALINK_DMA_PREFETCH_FULSH0      0x64
#define R_FCH_ALINK_DMA_PREFETCH_CTL1        0x80
#define R_FCH_ALINK_DMA_PREFETCH_CTL2        0x88
#define R_FCH_ALINK_BIF_CONTROL              0x90

#define R_FCH_PM2_REG00                    0x00
#define R_FCH_PM2_REG01                    0x01
#define R_FCH_PM2_REG02                    0x02
#define R_FCH_PM2_REG03                    0x03
#define R_FCH_PM2_REG04										0x04
#define R_FCH_PM2_REG10                    0x10
#define R_FCH_PM2_REG11                    0x11
#define R_FCH_PM2_REG12                    0x12
#define R_FCH_PM2_REG13                    0x13
#define R_FCH_PM2_REG14										0x14
#define R_FCH_PM2_REG20                    0x20
#define R_FCH_PM2_REG21                    0x21
#define R_FCH_PM2_REG22                    0x22
#define R_FCH_PM2_REG23                    0x23
#define R_FCH_PM2_REG24										0x24
#define R_FCH_PM2_REG31                    0x31
#define R_FCH_PM2_REG36                    0x36
#define R_FCH_PM2_REG3B                    0x3B
#define R_FCH_PM2_REG42                    0x42
#define R_FCH_PM2_REG56                    0x56
#define R_FCH_PM2_REG57                    0x57
#define R_FCH_PM2_REGEA                    0xEA
#define R_FCH_PM2_REGF8                    0xF8

//
//   FCH MMIO Base (SMI)
//    offset : 0x200
//
#define FCH_SMI_REG00            0x00         // EventStatus
#define FCH_SMI_REG04            0x04         // EventEnable
#define FCH_SMI_REG08            0x08         // SciTrig
#define FCH_SMI_REG0C            0x0C         // SciLevl
#define FCH_SMI_REG10            0x10         // SmiSciStatus
#define FCH_SMI_REG14            0x14         // SmiSciEn
#define FCH_SMI_REG18            0x18         // ForceSciEn
#define FCH_SMI_REG1C            0x1C         // SciRwData
#define FCH_SMI_REG20            0x20         // SciS0En
#define FCH_SMI_Gevent0          0x40         // SciMap0
#define FCH_SMI_Gevent1          0x41         // SciMap1
#define FCH_SMI_Gevent2          0x42         // SciMap2
#define FCH_SMI_Gevent3          0x43         // SciMap3
#define FCH_SMI_Gevent4          0x44         // SciMap4
#define FCH_SMI_Gevent5          0x45         // SciMap5
#define FCH_SMI_Gevent6          0x46         // SciMap6
#define FCH_SMI_Gevent7          0x47         // SciMap7
#define FCH_SMI_Gevent8          0x48         // SciMap8
#define FCH_SMI_Gevent9          0x49         // SciMap9
#define FCH_SMI_Gevent10         0x4A         // SciMap10
#define FCH_SMI_Gevent11         0x4B         // SciMap11
#define FCH_SMI_Gevent12         0x4C         // SciMap12
#define FCH_SMI_Gevent13         0x4D         // SciMap13
#define FCH_SMI_Gevent14         0x4E         // SciMap14
#define FCH_SMI_Gevent15         0x4F         // SciMap15
#define FCH_SMI_Gevent16         0x50         // SciMap16
#define FCH_SMI_Gevent17         0x51         // SciMap17
#define FCH_SMI_Gevent18         0x52         // SciMap18
#define FCH_SMI_Gevent19         0x53         // SciMap19
#define FCH_SMI_Gevent20         0x54         // SciMap20
#define FCH_SMI_Gevent21         0x55         // SciMap21
#define FCH_SMI_Gevent22         0x56         // SciMap22
#define FCH_SMI_Gevent23         0x57         // SciMap23
#define FCH_SMI_Usbwakup0        0x58         // SciMap24
#define FCH_SMI_Usbwakup1        0x59         // SciMap25
#define FCH_SMI_Usbwakup2        0x5A         // SciMap26
#define FCH_SMI_Usbwakup3        0x5B         // SciMap27
#define FCH_SMI_FCHGppPme0        0x5C         // SciMap28
#define FCH_SMI_FCHGppPme1        0x5D         // SciMap29
#define FCH_SMI_FCHGppPme2        0x5E         // SciMap30
#define FCH_SMI_FCHGppPme3        0x5F         // SciMap31
#define FCH_SMI_FCHGppHp0         0x60         // SciMap32
#define FCH_SMI_Reserved0        0x61         // SciMap33
#define FCH_SMI_Reserved1        0x62         // SciMap34
#define FCH_SMI_Reserved2        0x63         // SciMap35
#define FCH_SMI_AzaliaPme        0x64         // SciMap36
#define FCH_SMI_SataGevent0      0x65         // SciMap37
#define FCH_SMI_SataGevent1      0x66         // SciMap38
#define FCH_SMI_GecPme           0x67         // SciMap39
#define FCH_SMI_IMCGevent0       0x68         // SciMap40
#define FCH_SMI_IMCGevent1       0x69         // SciMap41
#define FCH_SMI_CIRPme           0x6A         // SciMap42
#define FCH_SMI_Reserved3        0x6B         // SciMap43
#define FCH_SMI_FanThGevent      0x6C         // SciMap44    //FanThermalGevent
#define FCH_SMI_ASFMasterIntr    0x6D         // SciMap45
#define FCH_SMI_ASFSlaveIntr     0x6E         // SciMap46
#define FCH_SMI_SMBUS0           0x6F         // SciMap47
#define FCH_SMI_TWARN            0x70         // SciMap48
#define FCH_SMI_TMI              0x71         // SciMap49    // TrafficMonitorIntr
#define FCH_SMI_iLLB             0x72         // SciMap50
#define FCH_SMI_PowerButton      0x73         // SciMap51
#define FCH_SMI_ProcHot          0x74         // SciMap52
#define FCH_SMI_APUHwAssertion   0x75         // SciMap53
#define FCH_SMI_APUSciAssertion  0x76         // SciMap54
#define FCH_SMI_RAS              0x77         // SciMap55
#define FCH_SMI_xHC0Pme          0x78         // SciMap56
#define FCH_SMI_xHC1Pme          0x79         // SciMap57

// Empty from 0x72-0x7F
//#Define FCH_SMI_REG7C            0x7F         // SciMap63  ***

#define FCH_SMI_REG80            0x80         //  SmiStatus0
#define FCH_SMI_REG84            0x84         // SmiStatus1
#define FCH_SMI_REG88            0x88         // SmiStatus2
#define FCH_SMI_REG8C            0x8C         // SmiStatus3
#define FCH_SMI_REG90            0x90         // SmiStatus4
#define FCH_SMI_REG94            0x94         // SmiPointer
#define FCH_SMI_REG96            0x96         // SmiTimer
#define FCH_SMI_REG98            0x98         // SmiTrig
#define FCH_SMI_REG9C            0x9C         // SmiTrig
#define FCH_SMI_REGA0            0xA0
#define FCH_SMI_REGA1            0xA1
#define FCH_SMI_REGA2            0xA2
#define FCH_SMI_REGA3            0xA3
#define FCH_SMI_REGA4            0xA4
#define FCH_SMI_REGA5            0xA5
#define FCH_SMI_REGA6            0xA6
#define FCH_SMI_REGA7            0xA7
#define FCH_SMI_REGA8            0xA8
#define FCH_SMI_REGA9            0xA9
#define FCH_SMI_REGAA            0xAA
#define FCH_SMI_REGAB            0xAB
#define FCH_SMI_REGAC            0xAC
#define FCH_SMI_REGAD            0xAD
#define FCH_SMI_REGAE            0xAE
#define FCH_SMI_REGAF            0xAF
#define FCH_SMI_REGB0            0xB0
#define FCH_SMI_REGB1            0xB1
#define FCH_SMI_REGB2            0xB2
#define FCH_SMI_REGB3            0xB3
#define FCH_SMI_REGB4            0xB4
#define FCH_SMI_REGB5            0xB5
#define FCH_SMI_REGB6            0xB6
#define FCH_SMI_REGB7            0xB7
#define FCH_SMI_REGB8            0xB8
#define FCH_SMI_REGB9            0xB9
#define FCH_SMI_REGBA            0xBA
#define FCH_SMI_REGBB            0xBB
#define FCH_SMI_REGBC            0xBC
#define FCH_SMI_REGBD            0xBD
#define FCH_SMI_REGBE            0xBE
#define FCH_SMI_REGBF            0xBF
#define FCH_SMI_REGC0            0xC0
#define FCH_SMI_REGC1            0xC1
#define FCH_SMI_REGC2            0xC2
#define FCH_SMI_REGC3            0xC3
#define FCH_SMI_REGC4            0xC4
#define FCH_SMI_REGC5            0xC5
#define FCH_SMI_REGC6            0xC6
#define FCH_SMI_REGC7            0xC7
#define FCH_SMI_REGC8            0xC8
#define FCH_SMI_REGCA            0xCA         //  IoTrapping1
#define FCH_SMI_REGCC            0xCC         //  IoTrapping2
#define FCH_SMI_REGCE            0xCE         //  IoTrapping3
#define FCH_SMI_REGD0            0xD0         //  MemTrapping0
#define FCH_SMI_REGD4            0xD4         //  MemRdOvrData0
#define FCH_SMI_REGD8            0xD8         //  MemTrapping1
#define FCH_SMI_REGDC            0xDC         //  MemRdOvrData1
#define FCH_SMI_REGE0            0xE0         //  MemTrapping2
#define FCH_SMI_REGE4            0xE4         //  MemRdOvrData2
#define FCH_SMI_REGE8            0xE8         //  MemTrapping3
#define FCH_SMI_REGEC            0xEC         //  MemRdOvrData3
#define FCH_SMI_REGF0            0xF0         //  CfgTrapping0
#define FCH_SMI_REGF4            0xF4         //  CfgTrapping1
#define FCH_SMI_REGF8            0xF8         //  CfgTrapping2
#define FCH_SMI_REGFC            0xFC         //  CfgTrapping3



//
//   FCH MMIO Base (PMIO)
//    offset : 0x300
//
#define FCH_PMIOA_REG00          0x00        // ISA Decode
#define FCH_PMIOA_REG04          0x04        // ISA Control
#define FCH_PMIOA_REG08          0x08        // PCI Control
#define FCH_PMIOA_REG0C          0x0C        // StpClkSmaf
#define FCH_PMIOA_REG10          0x10        // RetryDetect
#define FCH_PMIOA_REG14          0x14        // StuckDetect
#define FCH_PMIOA_REG20          0x20        // BiosRamEn
#define FCH_PMIOA_REG24          0x24        //  AcpiMmioEn
#define FCH_PMIOA_REG28          0x28         // AsfEn
#define FCH_PMIOA_REG2C          0x2C         // Smbus0En
#define FCH_PMIOA_REG2E          0x2E         // Smbus0Sel
#define FCH_PMIOA_REG34          0x34         // IoApicEn
#define FCH_PMIOA_REG3C          0x3C         // SmartVoltEn
#define FCH_PMIOA_REG40          0x40         // SmartVolt2En
#define FCH_PMIOA_REG44          0x44         // BootTimerEn
#define FCH_PMIOA_REG48          0x48         // WatchDogTimerEn
#define FCH_PMIOA_REG4C          0x4C         // WatchDogTimerConfig
#define FCH_PMIOA_REG50          0x50         // HPETEn
#define FCH_PMIOA_REG54          0x54         // SerialIrqConfig
#define FCH_PMIOA_REG56          0x56         // RtcControl
#define FCH_PMIOA_REG58          0x58         // VRT_T1
#define FCH_PMIOA_REG59          0x59         // VRT_T2
#define FCH_PMIOA_REG5A          0x5A         // IntruderControl
#define FCH_PMIOA_REG5B          0x5B         // RtcShadow
#define FCH_PMIOA_REG5C          0x5C
#define FCH_PMIOA_REG5D          0x5D
#define FCH_PMIOA_REG5E          0x5E         // RtcExtIndex
#define FCH_PMIOA_REG5F          0x5F         // RtcExtData
#define FCH_PMIOA_REG60          0x60         // AcpiPm1EvtBlk
#define FCH_PMIOA_REG62          0x62         // AcpiPm1CntBlk
#define FCH_PMIOA_REG64          0x64         // AcpiPmTmrBlk
#define FCH_PMIOA_REG66          0x66         // P_CNTBlk
#define FCH_PMIOA_REG68          0x68         // AcpiGpe0Blk
#define FCH_PMIOA_REG6A          0x6A         // AcpiSmiCmd
#define FCH_PMIOA_REG6C          0x6C         // AcpiPm2CntBlk
#define FCH_PMIOA_REG6E          0x6E         // AcpiPmaCntBlk
#define FCH_PMIOA_REG74          0x74         // AcpiConfig
#define FCH_PMIOA_REG78          0x78         // WakeIoAddr
#define FCH_PMIOA_REG7A          0x7A         // HaltCountEn
#define FCH_PMIOA_REG7C          0x7C         // C1eWrPortAdr
#define FCH_PMIOA_REG7E          0x7E         // CStateEn
#define FCH_PMIOA_REG80          0x80         // BreakEvent
#define FCH_PMIOA_REG84          0x84         // AutoArbEn
#define FCH_PMIOA_REG88          0x88         // CStateControl
#define FCH_PMIOA_REG89          0x89         //
#define FCH_PMIOA_REG8C          0x8C         // StpClkHoldTime
#define FCH_PMIOA_REG8E          0x8E         // PopUpEndTime
#define FCH_PMIOA_REG90          0x90         // C4Control
#define FCH_PMIOA_REG94          0x94         // CStateTiming0
#define FCH_PMIOA_REG96          0x96         //
#define FCH_PMIOA_REG97          0x97         //
#define FCH_PMIOA_REG98          0x98         // CStateTiming1
#define FCH_PMIOA_REG99          0x99         //
#define FCH_PMIOA_REG9B          0x9B         //
#define FCH_PMIOA_REG9C          0x9C         // C2Count
#define FCH_PMIOA_REG9D          0x9D         // C3Count
#define FCH_PMIOA_REG9E          0x9E         // C4Count
#define FCH_PMIOA_REGA0          0xA0         // MessageCState
#define FCH_PMIOA_REGA4          0xA4         //
#define FCH_PMIOA_REGA8          0xA8         // TrafficMonitorIdleTime
#define FCH_PMIOA_REGAA          0xAA         // TrafficMonitorIntTime
#define FCH_PMIOA_REGAC          0xAC         // TrafficMonitorTrafficCount
#define FCH_PMIOA_REGAE          0xAE         // TrafficMonitorIntrCount
#define FCH_PMIOA_REGB0          0xB0         // TrafficMonitorTimeTick
#define FCH_PMIOA_REGB4          0xB4         // FidVidControl
#define FCH_PMIOA_REGB6          0xB6         // TPRESET1
#define FCH_PMIOA_REGB7          0xB7         // Tpreset1b
#define FCH_PMIOA_REGB8          0xB8         // TPRESET2
#define FCH_PMIOA_REGB9          0xB9         // Test0
#define FCH_PMIOA_REGBA          0xBA         // S_StateControl
#define FCH_PMIOA_REGBB          0xBB         //
#define FCH_PMIOA_REGBC          0xBC         // ThrottlingControl
#define FCH_PMIOA_REGBE          0xBE         // ResetControl
#define FCH_PMIOA_REGBF          0xBF         // ResetControl
#define FCH_PMIOA_REGC0          0xC0         // S5Status
#define FCH_PMIOA_REGC2          0xC2         // ResetStatus
#define FCH_PMIOA_REGC4          0xC4         // ResetCommand
#define FCH_PMIOA_REGC5          0xC5         // CF9Shadow
#define FCH_PMIOA_REGC6          0xC6         // HTControl
#define FCH_PMIOA_REGC8          0xC8         // Misc
#define FCH_PMIOA_REGCC          0xCC         // IoDrvSth
#define FCH_PMIOA_REGD0          0xD0         // CLKRunEn
#define FCH_PMIOA_REGD2          0xD2         // PmioDebug
#define FCH_PMIOA_REGD3          0xD3         // SD
#define FCH_PMIOA_REGD6          0xD6         // IMCGating
#define FCH_PMIOA_REGD8          0xD8         // MiscIndex
#define FCH_PMIOA_REGD9          0xD9         // MiscData
#define FCH_PMIOA_REGDA          0xDA         // SataConfig
#define FCH_PMIOA_REGDC          0xDC         // HyperFlashConfig
#define FCH_PMIOA_REGDE          0xDE         // ABConfig
#define FCH_PMIOA_REGE0          0xE0         // ABRegBar
#define FCH_PMIOA_REGE6          0xE6         // FcEn
#define FCH_PMIOA_REGE7          0xE7
#define FCH_PMIOA_REGEA          0xEA         // PcibConfig
#define FCH_PMIOA_REGEB          0xEB         // AzEn
#define FCH_PMIOA_REGEC          0xEC         // LpcGating
#define FCH_PMIOA_REGED          0xED         // UsbGating
#define FCH_PMIOA_REGEE          0xEE         // UsbCntrl
#define FCH_PMIOA_REGEF          0xEF         // UsbEnable
#define FCH_PMIOA_REGF0          0xF0         // UsbControl
#define FCH_PMIOA_REGF3          0xF3         // UsbDebug
#define FCH_PMIOA_REGF6          0xF6         // GecEn
#define FCH_PMIOA_REGF8          0xF8         // GecConfig
#define FCH_PMIOA_REGFC          0xFC         // TraceMemoryEn

//
//   FCH MMIO Base (PMIO2)
//    offset : 0x400
//
#define FCH_PMIO2_REG00          0x00        // Fan0InputControl
#define FCH_PMIO2_REG01          0x01        // Fan0Control
#define FCH_PMIO2_REG02          0x02        // Fan0Freq
#define FCH_PMIO2_REG03          0x03        // LowDuty0
#define FCH_PMIO2_REG04          0x04        // MidDuty0

#define FCH_PMIO2_REG10          0x00        // Fan1InputControl
#define FCH_PMIO2_REG11          0x01        // Fan1Control
#define FCH_PMIO2_REG12          0x02        // Fan1Freq
#define FCH_PMIO2_REG13          0x03        // LowDuty1
#define FCH_PMIO2_REG14          0x04        // MidDuty1

#define FCH_PMIO2_REG63          0x63        // SampleFreqDiv
#define FCH_PMIO2_REG69          0x69        // Fan0 Speed
#define FCH_PMIO2_REG95          0x95        // Temperature
#define FCH_PMIO2_REGB8          0xB8        // Voltage
#define FCH_PMIO2_REGEA          0xEA        // Hwm_Calibration

#define FCH_PMIO2_REG92          0x92        //
#define FCH_PMIO2_REGF8          0xF8        // VoltageSamleSel
#define FCH_PMIO2_REGF9          0xF9        // TempSampleSel

#define FCH_PMIO2_REG          0xFC         // TraceMemoryEn


//
//   FCH MMIO Base (GPIO/IoMux)
//    offset : 0x100/0xD00
//
/*
GPIO from 0 ~ 67, (GEVENT 0-23) 128 ~ 150, 160 ~ 226.
*/
#define FCH_GPIO_REG00           0x00
#define FCH_GPIO_REG06           0x06
#define FCH_GPIO_REG09           0x09
#define FCH_GPIO_REG10           0x0A
#define FCH_GPIO_REG15           0x0F
#define FCH_GPIO_REG17           0x11
#define FCH_GPIO_REG21           0x15
#define FCH_GPIO_REG28           0x1C
#define FCH_GPIO_REG32           0x20
#define FCH_GPIO_REG33           0x21
#define FCH_GPIO_REG34           0x22
#define FCH_GPIO_REG35           0x23
#define FCH_GPIO_REG36           0x24
#define FCH_GPIO_REG37           0x25
#define FCH_GPIO_REG38           0x26
#define FCH_GPIO_REG39           0x27
#define FCH_GPIO_REG40           0x28
#define FCH_GPIO_REG41           0x29
#define FCH_GPIO_REG42           0x2A
#define FCH_GPIO_REG43           0x2B
#define FCH_GPIO_REG44           0x2C
#define FCH_GPIO_REG45           0x2D
#define FCH_GPIO_REG46           0x2E
#define FCH_GPIO_REG47           0x2F
#define FCH_GPIO_REG48           0x30
#define FCH_GPIO_REG49           0x31
#define FCH_GPIO_REG50           0x32
#define FCH_GPIO_REG51           0x33
#define FCH_GPIO_REG52           0x34
#define FCH_GPIO_REG53           0x35
#define FCH_GPIO_REG54           0x36
#define FCH_GPIO_REG55           0x37
#define FCH_GPIO_REG56           0x38
#define FCH_GPIO_REG57           0x39
#define FCH_GPIO_REG58           0x3A
#define FCH_GPIO_REG59           0x3B
#define FCH_GPIO_REG60           0x3C
#define FCH_GPIO_REG61           0x3D
#define FCH_GPIO_REG62           0x3E
#define FCH_GPIO_REG63           0x3F
#define FCH_GPIO_REG64           0x40
#define FCH_GPIO_REG65           0x41
#define FCH_GPIO_REG66           0x42
#define FCH_GPIO_REG67           0x43

#define FCH_GEVENT_REG00         0x60
#define FCH_GEVENT_REG01         0x61
#define FCH_GEVENT_REG02         0x62
#define FCH_GEVENT_REG03         0x63
#define FCH_GEVENT_REG04         0x64
#define FCH_GEVENT_REG05         0x65
#define FCH_GEVENT_REG06         0x66
#define FCH_GEVENT_REG07         0x67
#define FCH_GEVENT_REG08         0x68
#define FCH_GEVENT_REG09         0x69
#define FCH_GEVENT_REG10         0x6A
#define FCH_GEVENT_REG11         0x6B
#define FCH_GEVENT_REG12         0x6C
#define FCH_GEVENT_REG13         0x6D
#define FCH_GEVENT_REG14         0x6E
#define FCH_GEVENT_REG15         0x6F
#define FCH_GEVENT_REG16         0x70
#define FCH_GEVENT_REG17         0x71
#define FCH_GEVENT_REG18         0x72
#define FCH_GEVENT_REG19         0x73
#define FCH_GEVENT_REG20         0x74
#define FCH_GEVENT_REG21         0x75
#define FCH_GEVENT_REG22         0x76
#define FCH_GEVENT_REG23         0x77
// S5-DOMAIN GPIO
#define FCH_GPIO_REG160          0xA0
#define FCH_GPIO_REG161          0xA1
#define FCH_GPIO_REG162          0xA2
#define FCH_GPIO_REG163          0xA3
#define FCH_GPIO_REG164          0xA4
#define FCH_GPIO_REG165          0xA5
#define FCH_GPIO_REG166          0xA6
#define FCH_GPIO_REG167          0xA7
#define FCH_GPIO_REG168          0xA8
#define FCH_GPIO_REG169          0xA9
#define FCH_GPIO_REG170          0xAA
#define FCH_GPIO_REG171          0xAB
#define FCH_GPIO_REG172          0xAC
#define FCH_GPIO_REG173          0xAD
#define FCH_GPIO_REG174          0xAE
#define FCH_GPIO_REG175          0xAF
#define FCH_GPIO_REG176          0xB0
#define FCH_GPIO_REG177          0xB1
#define FCH_GPIO_REG178          0xB2
#define FCH_GPIO_REG179          0xB3
#define FCH_GPIO_REG180          0xB4
#define FCH_GPIO_REG181          0xB5
#define FCH_GPIO_REG182          0xB6
#define FCH_GPIO_REG183          0xB7
#define FCH_GPIO_REG184          0xB8
#define FCH_GPIO_REG185          0xB9
#define FCH_GPIO_REG186          0xBA
#define FCH_GPIO_REG187          0xBB
#define FCH_GPIO_REG188          0xBC
#define FCH_GPIO_REG189          0xBD
#define FCH_GPIO_REG190          0xBE
#define FCH_GPIO_REG191          0xBF
#define FCH_GPIO_REG192          0xC0
#define FCH_GPIO_REG193          0xC1
#define FCH_GPIO_REG194          0xC2
#define FCH_GPIO_REG195          0xC3
#define FCH_GPIO_REG196          0xC4
#define FCH_GPIO_REG197          0xC5
#define FCH_GPIO_REG198          0xC6
#define FCH_GPIO_REG199          0xC7
#define FCH_GPIO_REG200          0xC8
#define FCH_GPIO_REG201          0xC9
#define FCH_GPIO_REG202          0xCA
#define FCH_GPIO_REG203          0xCB
#define FCH_GPIO_REG204          0xCC
#define FCH_GPIO_REG205          0xCD
#define FCH_GPIO_REG206          0xCE
#define FCH_GPIO_REG207          0xCF
#define FCH_GPIO_REG208          0xD0
#define FCH_GPIO_REG209          0xD1
#define FCH_GPIO_REG210          0xD2
#define FCH_GPIO_REG211          0xD3
#define FCH_GPIO_REG212          0xD4
#define FCH_GPIO_REG213          0xD5
#define FCH_GPIO_REG214          0xD6
#define FCH_GPIO_REG215          0xD7
#define FCH_GPIO_REG216          0xD8
#define FCH_GPIO_REG217          0xD9
#define FCH_GPIO_REG218          0xDA
#define FCH_GPIO_REG219          0xDB
#define FCH_GPIO_REG220          0xDC
#define FCH_GPIO_REG221          0xDD
#define FCH_GPIO_REG222          0xDE
#define FCH_GPIO_REG223          0xDF
#define FCH_GPIO_REG224          0xF0
#define FCH_GPIO_REG225          0xF1
#define FCH_GPIO_REG226          0xF2
#define FCH_GPIO_REG227          0xF3
#define FCH_GPIO_REG228          0xF4

//
//   FCH MMIO Base (SMBUS)
//    offset : 0xA00
//
#define FCH_SMBUS_REG12           0x12        //   I2CbusConfig

//
//   FCH MMIO Base (MISC)
//    offset : 0xE00
//
#define FCH_MISC_REG00           0x00        //  ClkCntrl0
/*
FCH_MISC_REG00                   EQU     000h
  ClkCntrl0                   EQU     0FFFFFFFFh
*/
#define FCH_MISC_REG04           0x04        //  ClkCntrl1
/*
FCH_MISC_REG04                   EQU     004h
  ClkCntrl1                   EQU     0FFFFFFFFh
*/
#define FCH_MISC_REG08           0x08        //  ClkCntrl2
/*
FCH_MISC_REG08                   EQU     008h
  ClkCntrl2                   EQU     0FFFFFFFFh
*/
#define FCH_MISC_REG0C           0x0C        //  ClkCntrl3
/*
FCH_MISC_REG0C                   EQU     00Ch
  ClkCntrl3                   EQU     0FFFFFFFFh
*/
#define FCH_MISC_REG10           0x10        //  ClkCntrl4
/*
FCH_MISC_REG10                   EQU     010h
  ClkCntrl4                   EQU     0FFFFFFFFh
*/
#define FCH_MISC_REG14           0x14        //   ClkCntrl5
/*
FCH_MISC_REG14                   EQU     014h
  ClkCntrl5                   EQU     0FFFFFFFFh
*/
#define FCH_MISC_REG18           0x18        //   ClkCntrl6
/*
FCH_MISC_REG18                   EQU     018h
  ClkCntrl6                   EQU     0FFFFFFFFh
*/
#define FCH_MISC_REG30           0x30         // OscFreqCounter
/*
FCH_MISC_REG30                   EQU     030h
  OscCounter                  EQU     0FFFFFFFFh                  ; The 32bit register shows the number of OSC clock per second.
*/
#define FCH_MISC_REG34           0x34         //  HpetClkPeriod
/*
FCH_MISC_REG34                   EQU     034h
  HpetClkPeriod               EQU     0FFFFFFFFh                  ; default - 0x429B17Eh (14.31818M).
*/
#define FCH_MISC_REG40           0x40         //  MiscCntrl      for clock only
#define FCH_MISC_REG41           0x41         //  MiscCntr2
#define FCH_MISC_REG42           0x42         //  MiscCntr3
#define FCH_MISC_REG50           0x50         //
#define FCH_MISC_REGB6           0xB6         //
/*
FCH_MISC_REG40                   EQU     040h
*/

#define FCH_MISC_REG80           0x80        /**< FCH_MISC_REG80
                                   *   @par
                                   *   StrapStatus [15.0] -  chip Strap Status
                                   *    @li <b>0001</b> - Not USED FWH
                                   *    @li <b>0002</b> - Not USED LPC ROM
                                   *    @li <b>0004</b> - EC enabled
                                   *    @li <b>0008</b> - Reserved
                                   *    @li <b>0010</b> - Internal Clock mode
                                   */

#define  ChipSysNotUseFWHRom    0x0001                            // EcPwm3 pad
#define  ChipSysNotUseLpcRom    0x0002                            // Inverted version from EcPwm2 pad (default - 1)
                                                                  // Note: Both EcPwm3 and EcPwm2 straps pins are used to select boot ROM type.
#define  ChipSysEcEnable        0x0004                            // Enable Embedded Controller (EC)
#define  ChipSysBootFailTmrEn   0x0008                            // Enable Watchdog function
#define  ChipSysIntClkGen       0x0010                            // Select 25Mhz crystal clock or 100Mhz PCI-E clock **

#define FCH_MISC_REG84           0x84         //  StrapOverride
/*
FCH_MISC_REG84                   EQU     084h
  Override FWHDisableStrap    EQU     BIT0                        ; Override FWHDiableStrap value from external pin.
  Override UseLpcRomStrap     EQU     BIT1                        ; Override UseLpcRomStrap value from external pin.
  Override EcEnableStrap      EQU     BIT2                        ; Override EcEnableStrap value from external pin.
  Override BootFailTmrEnStrap EQU     BIT3                        ; Override BootFailTmrEnStrap value from external pin.
  Override DefaultModeStrap   EQU     BIT5                        ; Override DefaultModeStrap value from external pin.
  Override I2CRomStrap        EQU     BIT7                        ; Override I2CRomStrap value from external pin.
  Override ILAAutorunEnBStrap EQU     BIT8                        ; Override ILAAutorunEnBStrap value from external pin.
  Override FcPllBypStrap      EQU     BIT9                        ; Override FcPllBypStrap value from external pin.
  Override PciPllBypStrap     EQU     BIT10                       ; Override PciPllBypStrap value from external pin.
  Override ShortResetStrap    EQU     BIT11                       ; Override ShortResetStrap value from external pin.
  Override FastBif2ClkStrap   EQU     BIT13                       ; Override FastBif2ClkStrap value from external pin'
  PciRomBootStrap             EQU     BIT15                       ; Override PCI Rom Boot Strap value from external pin ?? Not match 0x80 reg ??
  BlinkSlowModestrap          EQU     BIT16                       ; Override Blink Slow mode (100Mhz) from external pin'
  ClkGenStrap                 EQU     BIT17                       ; Override CLKGEN from external pin.
  BIF_GEN2_COMPL_Strap        EQU     BIT18                       ; Override BIF_ GEN2_COMPLIANCE strap from external pin.
  StrapOverrideEn             EQU     BIT31                       ; Enable override strapping feature.
*/
#define FCH_MISC_REGC0           0xC0        //  CPU_Pstate0
/*
FCH_MISC_REGC0                   EQU     0C0h
  Core0_PState                EQU     BIT0+BIT1+BIT2              ; 000: P0  001: P1  010: P2 011: P3 100: P4 101: P5 110: P6 111: P7
  Core1_PState                EQU     BIT4+BIT5+BIT6
  Core2_PState                EQU     BIT8+BIT9+BIT10
  Core3_PState                EQU     BIT12+BIT13+BIT14
  Core4_PState                EQU     BIT16++BIT17+BIT18
  Core5_PState                EQU     BIT20+BIT21+BIT22
  Core6_PState                EQU     BIT24+BIT25+BIT26
  Core7_PState                EQU     BIT28+BIT29+BIT30
*/
#define FCH_MISC_REGC4           0xC4         //  CPU_Pstate1
/*
FCH_MISC_REGC4                   EQU     0C4h
  Core8_PState                EQU     BIT0+BIT1+BIT2              ; 000: P0  001: P1  010: P2 011: P3 100: P4 101: P5 110: P6 111: P7
  Core9_PState                EQU     BIT4+BIT5+BIT6
  Core10_PState               EQU     BIT8+BIT9+BIT10
  Core11_PState               EQU     BIT12+BIT13+BIT14
  Core12_PState               EQU     BIT16++BIT17+BIT18
  Core13_PState               EQU     BIT20+BIT21+BIT22
  Core14_PState               EQU     BIT24+BIT25+BIT26
  Core15_PState               EQU     BIT28+BIT29+BIT30
*/
#define FCH_MISC_REGD0           0xD0         //  CPU_Cstate0
/*
FCH_MISC_REGD0                   EQU     0D0h
  Core0_CState                EQU     BIT0+BIT1+BIT2              ; 000: C0  001: C1  010: C2 011: C3 100: C4 101: C5 110: C6 111: C7
  Core1_CState                EQU     BIT4+BIT5+BIT6
  Core2_CState                EQU     BIT8+BIT9+BIT10
  Core3_CState                EQU     BIT12+BIT13+BIT14
  Core4_CState                EQU     BIT16++BIT17+BIT18
  Core5_CState                EQU     BIT20+BIT21+BIT22
  Core6_CState                EQU     BIT24+BIT25+BIT26
  Core7_CState                EQU     BIT28+BIT29+BIT30
*/
#define FCH_MISC_REGD4           0xD4         //  CPU_Cstate1
/*
FCH_MISC_REGD4                   EQU     0D4h
  Core8_CState                EQU     BIT0+BIT1+BIT2              ; 000: C0  001: C1  010: C2 011: C3 100: C4 101: C5 110: C6 111: C7
  Core9_CState                EQU     BIT4+BIT5+BIT6
  Core10_CState               EQU     BIT8+BIT9+BIT10
  Core11_CState               EQU     BIT12+BIT13+BIT14
  Core12_CState               EQU     BIT16++BIT17+BIT18
  Core13_CState               EQU     BIT20+BIT21+BIT22
  Core14_CState               EQU     BIT24+BIT25+BIT26
  Core15_CState               EQU     BIT28+BIT29+BIT30
*/
#define FCH_MISC_REGF0           0xF0         //  SataPortSts    ?? EC touch only
/*
FCH_MISC_REGF0                   EQU     0F0h
  Port0Sts                    EQU     BIT0                        ; The selected status of Port 0.
  Port1Sts                    EQU     BIT1                        ; The selected status of Port 1
  Port2Sts                    EQU     BIT2                        ; The selected status of Port 2.
  Port3Sts                    EQU     BIT3                        ; The selected status of Port 3
  Port4Sts                    EQU     BIT4                        ; The selected status of Port 4.
  Port5Sts                    EQU     BIT5                        ; The selected status of Port 5
  SataPortSel                 EQU     BIT24+BIT25                 ; 00  - Select "led" for Port 0 to 5
                                  ; 01 - Select "delete" for Port 0 to 5
                                  ; 10 - Select "err" for Port 0 to 5
                                  ; 11 - Select "led" for Port 0 to 5
*/

#define FCH_SPI_MMIO_REG1D       0x1D

#endif
