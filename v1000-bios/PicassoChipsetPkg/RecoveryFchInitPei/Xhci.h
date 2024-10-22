/** @file
  Include file for RecoveryFchInit Peim.

;******************************************************************************
;* Copyright (c) 2013 - 2016, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/
#ifndef _PEI_XHCI_H_
#define _PEI_XHCI_H_
#include <PiPei.h>
#include <Library/DebugLib.h>

#include <IndustryStandard/Pci.h>
#include <Ppi/Usb3HostController.h>
#include <Ppi/UsbController.h>
#include <Ppi/PciCfg2.h>
#include <Ppi/Stall.h>
#include <Ppi/EndOfPeiPhase.h>

#define XHCI_CLASSCODE              0x0C033000

#define MAX_XHCI_CONTROLLERS        2
#define MAX_XHCI_CONTROLLERS_RV2    1

#define PEI_XHCI_SIGNATURE          SIGNATURE_32 ('X', 'H', 'C', 'I')

#define USBHC_RECOVERY_TEMP_MEM_BASE_SIZE    SIZE_32KB

typedef struct {
  UINTN                         Signature;
  PEI_USB_CONTROLLER_PPI        UsbControllerPpi;
  EFI_PEI_PPI_DESCRIPTOR        PpiList;
  EFI_PEI_NOTIFY_DESCRIPTOR     NotifyList;
  EFI_PEI_STALL_PPI             *StallPpi;
  UINTN                         TotalUsbControllers;
  UINTN                         MemBase[MAX_XHCI_CONTROLLERS];
  UINTN                         *PciCfgAddress;
} PEI_XHCI_DEVICE;

#define PEI_XHCI_DEVICE_FROM_THIS(a) \
  CR(a, PEI_XHCI_DEVICE, UsbControllerPpi, PEI_XHCI_SIGNATURE)

#define PEI_XHCI_DEVICE_FROM_NOTIFY_DESC(a) \
  CR(a, PEI_XHCI_DEVICE, NotifyList, PEI_XHCI_SIGNATURE)

typedef PEI_XHCI_DEVICE PEI_USBHC_DEVICE;

#define PEI_USBHC_SIGNATURE                  PEI_XHCI_SIGNATURE
#define PEI_USBHC_DEVICE_FROM_THIS(a)        PEI_XHCI_DEVICE_FROM_THIS(a)
#define PEI_USBHC_DEVICE_FROM_NOTIFY_DESC(a) PEI_XHCI_DEVICE_FROM_NOTIFY_DESC(a)

#endif
