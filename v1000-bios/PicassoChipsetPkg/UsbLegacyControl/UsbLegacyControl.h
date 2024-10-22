/** @file
  SmmPlatform.h.

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

#ifndef _USB_LEGACY_CONTROL_H
#define _USB_LEGACY_CONTROL_H

#include <PiSmm.h>

#include <Protocol/SmmUsbDispatch2.h>
#include <Protocol/SmmBase2.h>
#include <Protocol/UsbLegacyPlatform.h>

#include <Library/UefiBootServicesTableLib.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/PciExpressLib.h>
#include <Library/IoLib.h>
#include <Library/BaseLib.h>
#include <Library/DevicePathLib.h>
#include <Library/BaseMemoryLib.h>

#include <Fch.h>

#define  USB_LEGACY_CONTROL_PROTOCOL_GUID \
 { 0x3084d2bd, 0xf589, 0x4be1, {0x8e, 0xf0, 0x26, 0xc6, 0xd6, 0x8a, 0x1b, 0xc8} }

//#define  LPC_BUS                                        0x00
//#define  LPC_DEV                                        0x1f
//#define  LPC_FUN                                        0x00
//#define    R_ACPI_PM_BASE                               0x40
//#define    ACPI_PM_BASE_MASK                            0xFFF8
//#define    R_LUKMC                                      0x94
//#define    B_LUKMC_TRAP_60R                             0x0001
//#define    B_LUKMC_TRAP_60W                             0x0002
//#define    B_LUKMC_TRAP_64R                             0x0004
//#define    B_LUKMC_TRAP_64W                             0x0008
//#define    B_LUKMC_USBSMIEN                             0x0010
//#define    B_LUKMC_TRAP_STATUS                          0x0f00
//#define    N_LUKMC_TRAP_STATUS                          8
//#define    B_LUKMC_USBPIRQEN                            0x2000
//
//#define  LPC_PCI_PMBASE                                 ((LPC_BUS << 16) | (LPC_DEV << 11) | (LPC_FUN << 8) | (R_ACPI_PM_BASE))
//#define  LPC_PCI_LUKMC                                  ((LPC_BUS << 16) | (LPC_DEV << 11) | (LPC_FUN << 8) | (R_LUKMC))

#define  IRQ1                                           0x00
#define  IRQ12                                          0x01

#define  USB_LEGACY_CONTROL_SETUP_EMULATION             0
#define  USB_LEGACY_CONTROL_GET_KBC_DATA_POINTER        1
#define  USB_LEGACY_CONTROL_GET_KBC_STATUS_POINTER      2
#define  USB_LEGACY_CONTROL_GENERATE_IRQ                3
#define  USB_LEGACY_CONTROL_GET_CAPABILITY              4
#define    CAP_ASYNC_CPU_TRAP                           1

#define  HCE_BASE_ADDRESS  0xFED80000

#define  HCE_CONTROL  0x40
#define  HCE_INPUT    0x44
#define  HCE_OUTPUT   0x48
#define  HCE_STATUS   0x4C
#define  HCE_INTREN   0x50

#define bit(a)   (1 << (a))
//
// The HCE_CONTROL OFFSET Define
//
#define HCE_CTRL_EMULATION_ENABLE          bit(0)
#define HCE_CTRL_EMULATION_INTERRUPT       bit(1)
#define HCE_CTRL_CHARACTER_PENDING         bit(2)
#define HCE_CTRL_IRQEN                     bit(3)
#define HCE_CTRL_EXTERNAL_IRQEN            bit(4)
#define HCE_CTRL_GATEA20_SEQUENCE          bit(5)
#define HCE_CTRL_IRQ1_ACTIVE               bit(6)
#define HCE_CTRL_IRQ12_ACTIVE              bit(7)
#define HCE_CTRL_A20_STATE                 bit(8)

//
// The HCE_INTREN OFFSET Define
//
#define HCE_INTREN_ENIRQ1                  bit(0)
#define HCE_INTREN_ENIRQ12                 bit(1)
#define HCE_INTREN_BLOCKIRQ1               bit(2)
#define HCE_INTREN_BLOCKIRQ12              bit(3)
#define HCE_INTREN_EMULATION_SMIEN         bit(4)
#define HCE_INTREN_SOF_COUNTER             bit(5)
#define HCE_INTREN_SOF_SRC                 bit(8)

//
// The HCE_STATUS OFFSET Define
//
#define HCE_STATUS_OUTPUT_FULL             bit(0)
#define HCE_STATUS_INPUT_FULL              bit(1)
#define HCE_STATUS_FLAG                    bit(2)
#define HCE_STATUS_CMD_DATA                bit(3)
#define HCE_STATUS_INHIBIT_SWITCH          bit(4)
#define HCE_STATUS_AUX_OUTPUT_FULL         bit(5)
#define HCE_STATUS_TIMEOUT                 bit(6)
#define HCE_STATUS_PARITY                  bit(7)

#define KBC_TRAP_READ_DATA_PORT         0x01
#define KBC_TRAP_WRITE_DATA_PORT        0x02
#define KBC_TRAP_READ_STATUS_PORT       0x04
#define KBC_TRAP_WRITE_COMMAND_PORT     0x08
#define KBC_TRAP_A20GATE_CHANGED        0x10

typedef
VOID
(EFIAPI *USB_LEGACY_CONTROL_SMI_HANDLER) (
  IN     UINTN                                          Event,
  IN     VOID                                           *Context
  );

typedef
EFI_STATUS
(EFIAPI *USB_LEGACY_CONTROL) (
  IN     UINTN                                          Command,
  IN     VOID                                           *Param
  );

typedef struct {
  USB_LEGACY_CONTROL                                    UsbLegacyControl;
  USB_LEGACY_CONTROL_SMI_HANDLER                        SmiHandler;
  VOID                                                  *SmiContext;
  BOOLEAN                                               InSmm;
} USB_LEGACY_CONTROL_PROTOCOL;

EFI_STATUS
EFIAPI
UsbLegacyControl (
  IN     UINTN                                          Command,
  IN     VOID                                           *Param
  );

VOID
UsbLegacyControlSmiHandler (
  IN EFI_HANDLE  Handle,
  IN CONST VOID  *Context,
  IN OUT VOID    *CommBuffer      OPTIONAL,
  IN OUT UINTN   *CommBufferSize  OPTIONAL
  );

#endif
