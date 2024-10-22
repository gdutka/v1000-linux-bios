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
#ifndef _PEI_SATA_H_
#define _PEI_SATA_H_

#include <PiPei.h>
#include <Library/DebugLib.h>

#include <IndustryStandard/Pci.h>
#include <IndustryStandard/Atapi.h>
#include <Ppi/AtaController.h>
#include <Ppi/PciCfg2.h>
#include <Ppi/Stall.h>
#include <Ppi/EndOfPeiPhase.h>
#include <Ppi/AhciController.h>

#define MAX_SATA_CONTROLLERS        1

#define PEI_SATA_SIGNATURE          SIGNATURE_32 ('S', 'A', 'T', 'A')
#define PEI_AHCI_SIGNATURE          SIGNATURE_32 ('P', 'A', 'H', 'C')

#define SATA_AHCI_BAR               0x24

#define AHCI_CLASSCODE              0x01060100
#define IDE_CLASSCODE               0x01010000

typedef struct {
  UINTN                         Signature;
  PEI_ATA_CONTROLLER_PPI        AtaControllerPpi;
  EFI_PEI_PPI_DESCRIPTOR        PpiList;
  EFI_PEI_NOTIFY_DESCRIPTOR     NotifyList;
  EFI_PEI_PCI_CFG2_PPI          *PciCfgPpi;
  EFI_PEI_STALL_PPI             *StallPpi;
  UINTN                         TotalSataControllers;
  UINTN                         MemBase[MAX_SATA_CONTROLLERS];
  UINTN                         IoBase[MAX_SATA_CONTROLLERS];
  UINTN                         *PciCfgAddress;
} PEI_SATA_DEVICE;

typedef struct _PEI_AHCI_DEVICE {
  UINTN                         Signature;
  PEI_AHCI_CONTROLLER_PPI       AhciControllerPpi;
  EFI_PEI_PPI_DESCRIPTOR        PpiList;
  EFI_PEI_NOTIFY_DESCRIPTOR     NotifyList;
  UINTN                         TotalAhciControllers;
  EFI_PHYSICAL_ADDRESS          MemBase[MAX_SATA_CONTROLLERS];
  UINTN                         *PciCfgAddress;
} PEI_AHCI_DEVICE;

#define PEI_SATA_DEVICE_FROM_THIS(a) \
  CR(a, PEI_SATA_DEVICE, AtaControllerPpi, PEI_SATA_SIGNATURE)

#define PEI_SATA_DEVICE_FROM_NOTIFY_DESC(a) \
  CR(a, PEI_SATA_DEVICE, NotifyList, PEI_SATA_SIGNATURE)

#define PEI_AHCI_DEVICE_FROM_THIS(a) \
  CR(a, PEI_AHCI_DEVICE, AhciControllerPpi, PEI_AHCI_SIGNATURE)

#define PEI_AHCI_DEVICE_FROM_NOTIFY_DESC(a) \
  CR(a, PEI_AHCI_DEVICE, NotifyList, PEI_AHCI_SIGNATURE)

#endif
