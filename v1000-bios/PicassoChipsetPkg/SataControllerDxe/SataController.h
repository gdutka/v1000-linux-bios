/** @file
  Header file for chipset Serial ATA controller driver.

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

/**
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement

@copyright
  Copyright (c) 1999 - 2012 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

@file
  SataController.h

@brief
  Header file for chipset Serial ATA controller driver.

**/

#ifndef _SERIAL_ATA_CONTROLLER_H
#define _SERIAL_ATA_CONTROLLER_H

#include <Uefi.h>
#include <IndustryStandard/Pci.h>
#include <CommonReg.h>

#include <Library/PcdLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseLib.h>
#include <Library/IoLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PciExpressLib.h>
#include <Library/TimerLib.h>
#include <Library/DebugLib.h>

#include <Protocol/DriverBinding.h>
#include <Protocol/ComponentName.h>
#include <Protocol/PciIo.h>
#include <Protocol/IdeControllerInit.h>
#include <Protocol/PciRootBridgeIo.h>

#include <Guid/SataControllerInit.h>
#include <Guid/SataControllerDriver.h>

#include <Fch.h>
#include <SSFch.h>
#include <PostCode.h>

#define SATA_CONTROLLER_SIGNATURE       SIGNATURE_32 ('s', 'a', 't', 'a')

#define FCH_SATA_MAX_CHANNEL            0x02
#define FCH_SATA_MAX_DEVICES            0x02
#define FCH_SATA_ENUMER_ALL             FALSE
#define BME_BUS_MASTER_ENABLE_BIT       (1 << 2)

#define SATA_CONTROLLER_PRIVATE_DATA_FROM_THIS(a) \
  CR (a, \
      EFI_SATA_CONTROLLER_PRIVATE_DATA, \
      IdeInit, \
      SATA_CONTROLLER_SIGNATURE \
      )

typedef struct {
  UINT32                                Signature;
  EFI_IDE_CONTROLLER_INIT_PROTOCOL      IdeInit;
  EFI_PCI_IO_PROTOCOL                   *PciIo;
  EFI_ATA_COLLECTIVE_MODE               DisqulifiedModes [FCH_SATA_MAX_CHANNEL][FCH_SATA_MAX_DEVICES];
  EFI_IDENTIFY_DATA                     IdentifyData [FCH_SATA_MAX_CHANNEL][FCH_SATA_MAX_DEVICES];
  BOOLEAN                               IdentifyValid [FCH_SATA_MAX_CHANNEL][FCH_SATA_MAX_DEVICES];
} EFI_SATA_CONTROLLER_PRIVATE_DATA;

EFI_STATUS
SataControllerSupported (
  IN EFI_DRIVER_BINDING_PROTOCOL        *This,
  IN EFI_HANDLE                         Controller,
  IN EFI_DEVICE_PATH_PROTOCOL           *RemainingDevicePath
  );

EFI_STATUS
SataControllerStart (
  IN EFI_DRIVER_BINDING_PROTOCOL        *This,
  IN EFI_HANDLE                         Controller,
  IN EFI_DEVICE_PATH_PROTOCOL           *RemainingDevicePath
  );

EFI_STATUS
SataControllerStop (
  IN  EFI_DRIVER_BINDING_PROTOCOL       *This,
  IN  EFI_HANDLE                        Controller,
  IN  UINTN                             NumberOfChildren,
  IN  EFI_HANDLE                        *ChildHandleBuffer
  );

EFI_STATUS
SataInitGetChannelInfo (
  IN   EFI_IDE_CONTROLLER_INIT_PROTOCOL *This,
  IN   UINT8                            Channel,
  OUT  BOOLEAN                          *Enabled,
  OUT  UINT8                            *MaxDevices
  );

EFI_STATUS
SataInitGetDeviceInfo (
  IN  EFI_IDE_CONTROLLER_INIT_PROTOCOL  *This,
  IN  UINT8                             Channel,
  IN  UINT8                             Device,
  OUT UINT8                             *Type
  );

EFI_STATUS
SataInitNotifyPhase (
  IN  EFI_IDE_CONTROLLER_INIT_PROTOCOL  *This,
  IN  EFI_IDE_CONTROLLER_ENUM_PHASE     Phase,
  OUT UINT8                             Channel
  );

EFI_STATUS
SataInitSubmitData (
  IN  EFI_IDE_CONTROLLER_INIT_PROTOCOL  *This,
  IN  UINT8                             Channel,
  IN  UINT8                             Device,
  IN  EFI_IDENTIFY_DATA                 *IdentifyData
  );

EFI_STATUS
SataInitSubmitFailingModes (
  IN  EFI_IDE_CONTROLLER_INIT_PROTOCOL  *This,
  IN  UINT8                             Channel,
  IN  UINT8                             Device
  );

EFI_STATUS
SataInitDisqualifyMode (
  IN  EFI_IDE_CONTROLLER_INIT_PROTOCOL  *This,
  IN  UINT8                             Channel,
  IN  UINT8                             Device,
  IN  EFI_ATA_COLLECTIVE_MODE           *BadModes
  );

EFI_STATUS
SataInitCalculateMode (
  IN  EFI_IDE_CONTROLLER_INIT_PROTOCOL  *This,
  IN  UINT8                             Channel,
  IN  UINT8                             Device,
  IN  EFI_ATA_COLLECTIVE_MODE           **SupportedModes
  );

EFI_STATUS
SataInitSetTiming (
  IN  EFI_IDE_CONTROLLER_INIT_PROTOCOL  *This,
  IN  UINT8                             Channel,
  IN  UINT8                             Device,
  IN  EFI_ATA_COLLECTIVE_MODE           *Modes
  );

EFI_STATUS
EFIAPI
SataControllerGetDriverName (
  IN  EFI_COMPONENT_NAME_PROTOCOL       *This,
  IN  CHAR8                             *Language,
  OUT CHAR16                            **DriverName
  );

EFI_STATUS
EFIAPI
SataControllerGetControllerName (
  IN  EFI_COMPONENT_NAME_PROTOCOL       *This,
  IN  EFI_HANDLE                        ControllerHandle,
  IN  EFI_HANDLE                        ChildHandle OPTIONAL,
  IN  CHAR8                             *Language,
  OUT CHAR16                            **ControllerName
  );

EFI_STATUS
EnableSataController (
  IN  EFI_PCI_IO_PROTOCOL               *PciIo
  );

EFI_STATUS
CalculateBestPioMode (
  IN  EFI_IDENTIFY_DATA                 *IdentifyData,
  IN  UINT16                            *DisPioMode OPTIONAL,
  OUT UINT16                            *SelectedMode
  );

EFI_STATUS
CalculateBestUdmaMode (
  IN  EFI_IDENTIFY_DATA                 *IdentifyData,
  IN  UINT16                            *DisUDmaMode OPTIONAL,
  OUT UINT16                            *SelectedMode
  );

EFI_STATUS
IdeInitSetUdmaTiming (
  IN  UINT8                             Channel,
  IN  UINT8                             Device,
  IN  EFI_PCI_IO_PROTOCOL               *PciIo,
  IN  EFI_ATA_COLLECTIVE_MODE           *Modes
  );

EFI_STATUS
EFIAPI
SataControllerGetDriverName (
  IN  EFI_COMPONENT_NAME_PROTOCOL       *This,
  IN  CHAR8                             *Language,
  OUT CHAR16                            **DriverName
  );

EFI_STATUS
EFIAPI
SataControllerGetControllerName (
  IN  EFI_COMPONENT_NAME_PROTOCOL       *This,
  IN  EFI_HANDLE                        ControllerHandle,
  IN  EFI_HANDLE                        ChildHandle OPTIONAL,
  IN  CHAR8                             *Language,
  OUT CHAR16                            **ControllerName
  );

#endif
