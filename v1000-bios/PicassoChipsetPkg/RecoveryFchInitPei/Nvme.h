/** @file
  Include file for RecoveryFchInit Peim.

;******************************************************************************
;* Copyright (c) 2013 - 2017, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/
#ifndef _PEI_NVME_H_
#define _PEI_NVME_H_
#include <PiPei.h>
#include <Library/DebugLib.h>

#include <IndustryStandard/Pci.h>
#include <Ppi/NvmeController.h>
#include <Ppi/PciCfg2.h>
#include <Ppi/Stall.h>
#include <Ppi/EndOfPeiPhase.h>

#define MAX_NVME_CONTROLLERS        1

#define PEI_NVME_SIGNATURE          SIGNATURE_32 ('N', 'V', 'M', 'C')

#define NVME_RECOVERY_TEMP_MEM_BASE_SIZE    SIZE_32KB

#define NVME_CLASSCODE              0x01080200

typedef struct _PEI_NVME_DEVICE {
  UINTN                         Signature;
  PEI_NVME_CONTROLLER_PPI       NvmeControllerPpi;
  EFI_PEI_PPI_DESCRIPTOR        PpiList;
  EFI_PEI_NOTIFY_DESCRIPTOR     NotifyList;
  EFI_PEI_STALL_PPI             *StallPpi;
  UINTN                         TotalNvmeControllers;
  UINTN                         MemBase;
  UINTN                         *PciCfgAddress;
} PEI_NVME_DEVICE;


#define PEI_NVME_DEVICE_FROM_THIS(a) \
  CR(a, PEI_NVME_DEVICE, NvmeControllerPpi, PEI_NVME_SIGNATURE)

#define PEI_NVME_DEVICE_FROM_NOTIFY_DESC(a) \
  CR(a, PEI_NVME_DEVICE, NotifyList, PEI_NVME_SIGNATURE)

#endif
