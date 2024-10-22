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

#ifndef _SPCR_H_
#define _SPCR_H_

#include <AcpiHeaderDefaultValue.h>
#include <IndustryStandard/SerialPortConsoleRedirectionTable.h>


//
// Apply to SPCR.ACT
//
#define SPCR_INTERFACE_TYPE         EFI_ACPI_SERIAL_PORT_CONSOLE_REDIRECTION_TABLE_INTERFACE_TYPE_16550
#define SPCR_RESERVED01             {0, 0, 0}
#define SPCR_BASE_ADDRESS           {0x01, 0x08, 0x00, 0x00, 0x00000000000003F8}
#define SPCR_INTERRUPT_TYPE         EFI_ACPI_SERIAL_PORT_CONSOLE_REDIRECTION_TABLE_INTERRUPT_TYPE_8259
#define SPCR_IRQ_NUM                0x04
#define SPCR_GLOBAL_SYS_INTERRUPT   0x0
#define SPCR_BAUD_RATE              EFI_ACPI_SERIAL_PORT_CONSOLE_REDIRECTION_TABLE_BAUD_RATE_115200
#define SPCR_PARITY                 EFI_ACPI_SERIAL_PORT_CONSOLE_REDIRECTION_TABLE_PARITY_NO_PARITY
#define SPCR_STOP_BITS              EFI_ACPI_SERIAL_PORT_CONSOLE_REDIRECTION_TABLE_STOP_BITS_1
#define SPCR_FLOW_CONTROL           0
#define SPCR_TERMINAL_TYPE          EFI_ACPI_SERIAL_PORT_CONSOLE_REDIRECTION_TABLE_TERMINAL_TYPE_ANSI
#define SPCR_RESERVED02             0
#define SPCR_PCI_DID                0xFFFF
#define SPCR_PCI_VID                0xFFFF
#define SPCR_PCI_BUS                0x0
#define SPCR_PCI_DEV                0x0
#define SPCR_PCI_FUNC               0x0
#define SPCR_PCI_FLAGS              0x0
#define SPCR_PCI_SEG                0x0
#define SPCR_RESERVED03             0

#endif

