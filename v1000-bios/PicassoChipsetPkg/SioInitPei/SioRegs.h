//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
//;
//; Abstract:
//;

#ifndef _SIO_EC_REGS_H_
#define _SIO_EC_REGS_H_

//
// SioInit
//
#define CONFIG_PORT0                0x2E
#define INDEX_PORT0                 0x2E
#define DATA_PORT0                  0x2F

//
// UNLOCK PORT, Code
//
#define UNLOCK_PORT                 0x2E
#define UNLOCK_CODE0                0x55
#define UNLOCK_CODE1                0x55
#define UNLOCK_CODE2                0x55
#define UNLOCK_CODE3                0x55

#define LOCK_CODE                   0xAA

//
// SioInit2
//
//
// Port address for KBC1100
//
#define CONFIG_PORT 0x02E
#define INDEX_PORT  0x02E
#define DATA_PORT   INDEX_PORT + 1

//
// logical device in KBC1100
//

#define SIO_PM1           0x1
#define SIO_COM1          0x4
#define SIO_FIR           0x5
#define SIO_KBD           0x7
#define SIO_EC0           0x8
#define SIO_MAILBOX       0x9
#define SIO_LGPIO         0xA
#define SIO_SFI           0xB
#define SIO_EC1           0xD
#define SIO_EC2           0xE
#define SIO_IR            0x10
//
// Global register in KBC1100
//
#define R_SIO_GLOBAL_CONFIG_CONTROL         0x02
#define R_SIO_GLOBAL_LDN                    0x07
#define R_SIO_GLOBAL_CHIPID_1               0x20
#define R_SIO_GLOBAL_CHIPID_2               0x21
#define R_SIO_GLOBAL_POWER_CONTROL          0x22
#define R_SIO_GLOBAL_POWER_MGMT             0x23
#define R_SIO_GLOBAL_OSC                    0x24
#define R_SIO_GLOBAL_DEVICE_MODE            0x25
#define R_SIO_GLOBAL_CONFIG_PBA_LSB         0x26
#define R_SIO_GLOBAL_CONFIG_PBA_MSB         0x27

#define R_SIO_ACTIVATE                      0x30
#define R_SIO_BASE_ADDRESS_HIGH             0x60
#define R_SIO_BASE_ADDRESS_LOW              0x61
#define R_SIO_BASE_ADDRESS_HIGH2            0x62
#define R_SIO_BASE_ADDRESS_LOW2             0x63
#define R_SIO_PRIMARY_INTERRUPT_SELECT      0x70
#define R_SIO_PRIMARY_INTERRUPT_TYPE        0x71
#define R_SIO_SECOND_INTERRUPT_SELECT       0x72
#define R_SIO_DMA_CHANNEL_SELECT            0x74

#define R_SIO_FDC_SPECIAL_CONFIG1           0xf0
#define R_SIO_FDC_SPECIAL_CONFIG2           0xf1

#define R_SIO_LPT_SPECIAL_CONFIG1           0xf0

#define R_SIO_KB_SPECIAL_CONFIG1            0xf0

#define R_SIO_MOUSE_SPECIAL_CONFIG1         0xf0

#define R_SIO_GPIO_GP_LED1_PIN_MAPPING      0xf8
#define R_SIO_GPIO_GP_LED1_CONTROL          0xf9

//
// Register for COM1
//
#define SERIAL_MODE 0x00

//
// Register for Parallel Port
//
#define PARALLEL_MODE 0x02

//
// Bit definition for parallel mode
//
#define PARALLEL_MODE_MASK    0x0F
#define PARALLEL_MODE_SPP     0x01
#define PARALLEL_MODE_EPP     0x02
#define PARALLEL_MODE_ECP     0x04
#define PARALLEL_MODE_PRINTER 0x08

//
// Register for Floppy Disk Controller
//
#define FDD_MODE    0xF8
#define FDD_OPTION  0xF8
#define FDD_TYPE    0xF8
#define FDD0        0xF8
#define FDD1        0xF8

//
// Bit definitions for floppy write protect feature
//
#define FDD_WRITE_PROTECT     0x08
#define FDD_WRITE_PROTECT_EN  0x08
#define FDD_WRITE_PROTECT_DIS 0x0

//
// Register for Keyboard Controller
//
#define KRESET_GATE20_SELECT  0xF0

//
// Default Base Addresses for logical devices
//
#define GPIO_BASE_KBC       0x0680
#define PWR_CNTRL_BASE  (GPIO_BASE_KBC + 0x40)
#define FAN_CNTRL_BASE  (PWR_CNTRL_BASE + 0x20)

// Additional definitation for SMSC without including in LpcSio.h ++
#define SMSC_TEST5                  0x2C  // For 8042 KeyBoard reset

// Power Control Runtime registers
#define SIO_PME_STS   0x00
#define SIO_PME_EN    0x04
#define SIO_PME_STS3  0x08
#define SIO_PME_STS2  0x09
#define SIO_PME_STS1  0x0A
#define SIO_PME_EN3   0x0C
#define SIO_PME_EN2   0x0D
#define SIO_PME_EN1   0x0E
#define SIO_LED       0x10
// Additional definitation for SMSC without including in LpcSio.h --

typedef struct {
  UINT8 Register;
  UINT8 Value;
} EFI_SIO_TABLE;

typedef struct {
  UINT8 Register;
  UINT8 Value;
} EFI_SIO_TABLE_8_8;

typedef struct {
  UINT16 Register;
  UINT8 Value;
} EFI_SIO_TABLE_16_8;

EFI_STATUS
SioInit2 (
  IN CONST EFI_PEI_SERVICES          **PeiServices
);
#endif
