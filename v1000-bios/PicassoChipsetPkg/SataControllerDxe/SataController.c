/** @file
  This driver module produces IDE_CONTROLLER_INIT protocol or
  gEfiSataControllerInitGuid/gSataRaidControllerGuid Guid.

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
  license, no part o f this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

@file
  SataController.c

@brief
  This driver module produces IDE_CONTROLLER_INIT protocol for serial ATA
  driver and will be used by IDE Bus driver to support chipset dependent timing
  information, config SATA control/status registers. This driver
  is responsible for early initialization of serial ATA controller.

  Serial ATA spec requires SATA controller compatible with parallel IDE
  controller. That's why lots of code here is the same with IDE controller
  driver. However, We need this driver to optimize timing settings for SATA
  device and set SATA config/error/status registers.

**/

#include "SataController.h"

EFI_DRIVER_BINDING_PROTOCOL gSataControllerDriverBinding = {
  SataControllerSupported,
  SataControllerStart,
  SataControllerStop,
  1,
  NULL,
  NULL
};

EFI_COMPONENT_NAME_PROTOCOL     gSataControllerName = {
  SataControllerGetDriverName,
  SataControllerGetControllerName,
  "eng"
};

STATIC EFI_UNICODE_STRING_TABLE mSataControllerDriverNameTable[] = {
  {
    "eng",
    L"AMD SATA Controller Initialization Driver"
  },
  {
    NULL,
    NULL
  }
};

STATIC EFI_UNICODE_STRING_TABLE mSataControllerControllerNameTable[] = {
  {
    "eng",
    L"AMD Serial ATA Controller"
  },
  {
    NULL,
    NULL
  }
};

/**
  Chipset SATA Driver EntryPoint function. It follows the standard EFI driver
  model. It's called by StartImage() of DXE Core

  @param[in] ImageHandle          While the driver image loaded be the ImageLoader(),
                                  an image handle is assigned to this driver binary,
                                  all activities of the driver is tied to this ImageHandle
  @param[in] SystemTable          A pointer to the system table, for all BS(Boot Services) and
                                  RT(Runtime Services)

  @retval EFI_SUCCESS             Function completes successfully
**/
EFI_STATUS
InitializeSataControllerDriverEntryPoint (
  IN EFI_HANDLE                         ImageHandle,
  IN EFI_SYSTEM_TABLE                   *SystemTable
  )
{
  return EfiLibInstallDriverBindingComponentName2 (
           ImageHandle,
           SystemTable,
           &gSataControllerDriverBinding,
           ImageHandle,
           &gSataControllerName,
           NULL
           );
}

/**
  This function checks to see if the driver supports a device specified by
  "Controller handle" parameter. It is called by DXE Core StartImage() or
  ConnectController() routines. The driver uses 'device path' and/or
  'services' from the Bus I/O abstraction attached to the controller handle
  to determine if the driver support this controller handle.
  Note: In the BDS (Boot Device Selection) phase, the DXE core enumerate all
  devices (or, controller) and assigns GUIDs to them.

  @param[in] This                 A pointer points to the Binding Protocol instance
  @param[in] Controller           The handle of controller to be tested.
  @param[in] RemainingDevicePath  A pointer to the device path. Ignored by device
                                  driver but used by bus driver

  @retval EFI_SUCCESS             The device is supported
  @exception EFI_UNSUPPORTED      The device is not supported
**/
EFI_STATUS
SataControllerSupported (
  IN EFI_DRIVER_BINDING_PROTOCOL        *This,
  IN EFI_HANDLE                         Controller,
  IN EFI_DEVICE_PATH_PROTOCOL           *RemainingDevicePath
  )
{
  EFI_STATUS                            Status;
  EFI_DEVICE_PATH_PROTOCOL              *DevicePath;
  EFI_PCI_IO_PROTOCOL                   *PciIo;
  PCI_TYPE00                            PciDevice;

  Status = gBS->OpenProtocol (
                  Controller,
                  &gEfiDevicePathProtocolGuid,
                  (VOID *) &DevicePath,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_BY_DRIVER
                  );

  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }

  gBS->CloseProtocol (
         Controller,
         &gEfiDevicePathProtocolGuid,
         This->DriverBindingHandle,
         Controller
         );

  Status = gBS->OpenProtocol (
                  Controller,
                  &gEfiPciIoProtocolGuid,
                  (VOID **) &PciIo,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_BY_DRIVER
                  );

  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = PciIo->Pci.Read (
                        PciIo,
                        EfiPciIoWidthUint8,
                        0,
                        sizeof (PciDevice),
                        &PciDevice
                        );

  if (!EFI_ERROR (Status)) {
    // ASMedia ASM160X PCIE to SATA controller DID/VID = 0x06111B21 & 0x06121B21
    if ((PciDevice.Hdr.VendorId != FCH_SATA_VID) &&
        (PciDevice.Hdr.VendorId != 0x1B21)) {
      Status = EFI_UNSUPPORTED;
    } else {
      if ((PciDevice.Hdr.DeviceId != SANDSTONE_FCH_SATA_AHCI_DID) &&
          (PciDevice.Hdr.DeviceId != SANDSTONE_FCH_SATA_DID) &&
          (PciDevice.Hdr.DeviceId != SANDSTONE_FCH_SATA_AMDAHCI_DID) &&
          (PciDevice.Hdr.DeviceId != 0x0612) &&
          (PciDevice.Hdr.DeviceId != 0x0611)) {
        Status = EFI_UNSUPPORTED;
      }
    }
  }

  gBS->CloseProtocol (
         Controller,
         &gEfiPciIoProtocolGuid,
         This->DriverBindingHandle,
         Controller
         );

  return Status;
}

/**
  This routine is called right after the .Supported() is called and returns
  EFI_SUCCESS. Notes: The supported protocols are checked but the Protocols
  are closed.

  @param[in] This                 A pointer points to the Binding Protocol instance
  @param[in] Controller           The handle of controller to be tested. Parameter
                                  passed by the caller
  @param[in] RemainingDevicePath  A pointer to the device path. Should be ignored by
                                  device driver

  @retval EFI_SUCCESS             The device is started
  @retval Other values            Something error happened
**/
EFI_STATUS
SataControllerStart (
  IN EFI_DRIVER_BINDING_PROTOCOL        *This,
  IN EFI_HANDLE                         Controller,
  IN EFI_DEVICE_PATH_PROTOCOL           *RemainingDevicePath
  )
{
  EFI_STATUS                            Status;
  EFI_PCI_IO_PROTOCOL                   *PciIo;
  EFI_SATA_CONTROLLER_PRIVATE_DATA      *SataPriData;
  PCI_TYPE00                            PciDevice;

  POST_CODE (BDS_START_SATA_CONTROLLER);
  Status = gBS->OpenProtocol (
                  Controller,
                  &gEfiPciIoProtocolGuid,
                  (VOID **)&PciIo,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_BY_DRIVER
                  );

  if (EFI_ERROR (Status)) {
    return EFI_DEVICE_ERROR;
  }

  SataPriData = AllocatePool (sizeof (EFI_SATA_CONTROLLER_PRIVATE_DATA));

  if (SataPriData == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto Done;
  }

  ZeroMem (SataPriData, sizeof(EFI_SATA_CONTROLLER_PRIVATE_DATA));

  SataPriData->Signature              = SATA_CONTROLLER_SIGNATURE;
  SataPriData->PciIo                  = PciIo;
  SataPriData->IdeInit.GetChannelInfo = SataInitGetChannelInfo;
  SataPriData->IdeInit.NotifyPhase    = SataInitNotifyPhase;
  SataPriData->IdeInit.SubmitData     = SataInitSubmitData;
  SataPriData->IdeInit.DisqualifyMode = SataInitDisqualifyMode;
  SataPriData->IdeInit.CalculateMode  = SataInitCalculateMode;
  SataPriData->IdeInit.SetTiming      = SataInitSetTiming;
  SataPriData->IdeInit.EnumAll        = FCH_SATA_ENUMER_ALL;
  SataPriData->IdeInit.ChannelCount   = FCH_SATA_MAX_CHANNEL;

  Status = PciIo->Pci.Read (
                        PciIo,
                        EfiPciIoWidthUint8,
                        0,
                        sizeof (PciDevice),
                        &PciDevice
                        );

  if (EFI_ERROR (Status)) {
    goto Done;
  }

  //
  // Install IDE_CONTROLLER_INIT protocol & private data to this instance
  //
  switch (PciDevice.Hdr.ClassCode[1]) {

  case PCI_CLASS_MASS_STORAGE_IDE:
    if (PciDevice.Hdr.ClassCode[0] != 0x8A) {
      //
      // Enable SATA controller working in IDE mode
      //
      if (EFI_ERROR (EnableSataController (PciIo))) {
        Status = EFI_DEVICE_ERROR;
        goto Done;
      }
    }
    //
    // For IDE or Storage, let IDE bus driver to enumerate
    //
    Status = gBS->InstallMultipleProtocolInterfaces (
                    &Controller,
                    &gSataControllerDriverGuid,
                    SataPriData,
                    &gEfiIdeControllerInitProtocolGuid,
                    &(SataPriData->IdeInit),
                    NULL
                    );
    break;

  case PCI_CLASS_MASS_STORAGE_SATADPA:
    if (FeaturePcdGet (PcdH2OAhciSupported)) {
      //
      // For AHCI or Storage, let AHCI bus driver to enumerate, when setting
      // config.template switch SEAMLESS_AHCI_SUPPORT as YES and SATA Configure
      // as AHCI.
      //
      Status = gBS->InstallMultipleProtocolInterfaces (
                      &Controller,
                      &gSataControllerDriverGuid,
                      SataPriData,
                      &gEfiSataControllerInitGuid,
                      NULL,
                      NULL
                      );
    }
    break;


  case PCI_CLASS_MASS_STORAGE_RAID:
    //
    // For RAID, let OPROM to handle
    //
    Status = gBS->InstallMultipleProtocolInterfaces (
                    &Controller,
                    &gSataRaidControllerGuid,
                    NULL,
                    NULL
                    );
    if (EFI_ERROR (Status)) {
      return Status;
    }

    FreePool (SataPriData);

    //
    // Close protocols opened by SATA controller driver
    //
    Status = gBS->CloseProtocol (
                    Controller,
                    &gEfiPciIoProtocolGuid,
                    This->DriverBindingHandle,
                    Controller
                    );
    if (EFI_ERROR (Status)) {
      return Status;
    }
    break;

  default:
    break;
  }

Done:

  if (EFI_ERROR (Status)) {
    if (SataPriData != NULL) {
      FreePool (SataPriData);
    }
    //
    // Close protocols opened by SATA controller driver
    //
    gBS->CloseProtocol (
           Controller,
           &gEfiPciIoProtocolGuid,
           This->DriverBindingHandle,
           Controller
           );
  }
  return Status;
}

/**
  Stop managing the target device

  @param[in] This                 A pointer pointing to the Binding Protocol instance
  @param[in] Controller           The handle of controller to be stopped
  @param[in] NumberOfChildren     Number of child devices
  @param[in] ChildHandleBuffer    Buffer holding child device handles

  @retval EFI_SUCCESS             The target device is stopped
**/
EFI_STATUS
SataControllerStop (
  IN  EFI_DRIVER_BINDING_PROTOCOL       *This,
  IN  EFI_HANDLE                        Controller,
  IN  UINTN                             NumberOfChildren,
  IN  EFI_HANDLE                        *ChildHandleBuffer
  )
{
  EFI_STATUS                            Status;
  EFI_SATA_CONTROLLER_PRIVATE_DATA      *SataPriData;
  PCI_TYPE00                            PciDevice;

  Status = gBS->OpenProtocol (
                  Controller,
                  &gSataControllerDriverGuid,
                  (VOID**)&SataPriData,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_GET_PROTOCOL
                  );

  if (!EFI_ERROR (Status)) {
    SataPriData->PciIo->Pci.Read (
                              SataPriData->PciIo,
                              EfiPciIoWidthUint8,
                              0,
                              sizeof (PciDevice),
                              &PciDevice
                              );

    switch (PciDevice.Hdr.ClassCode[1]) {

    case PCI_CLASS_MASS_STORAGE_IDE:
      gBS->OpenProtocol (
             Controller,
             &gEfiIdeControllerInitProtocolGuid,
             (VOID **) &(SataPriData->IdeInit),
             This->DriverBindingHandle,
             Controller,
             EFI_OPEN_PROTOCOL_GET_PROTOCOL
             );

      gBS->UninstallMultipleProtocolInterfaces (
             Controller,
             &gSataControllerDriverGuid,
             SataPriData,
             &gEfiIdeControllerInitProtocolGuid,
             &(SataPriData->IdeInit),
             NULL
             );
      break;

    case PCI_CLASS_MASS_STORAGE_SATADPA:
      if (FeaturePcdGet (PcdH2OAhciSupported)) {
        gBS->UninstallMultipleProtocolInterfaces (
               &Controller,
               &gSataControllerDriverGuid,
               SataPriData,
               &gEfiSataControllerInitGuid,
               NULL,
               NULL
               );
      }
      break;

    default:
      break;
    }
  }

  if (SataPriData != NULL) {
    FreePool (SataPriData);
  }

  gBS->CloseProtocol (
         Controller,
         &gEfiPciIoProtocolGuid,
         This->DriverBindingHandle,
         Controller
         );

  return EFI_SUCCESS;
}

/**
  This function can be used to obtain information about a specified channel.
  It's usually used by IDE Bus driver during enumeration process.

  @param[in] This                 the EFI_IDE_CONTROLLER_INIT_PROTOCOL instance.
  @param[in] Channel              Channel number (0 based, either 0 or 1)
  @param[out] Enabled             TRUE if the channel is enabled. If the channel is disabled,
                                  then it will no be enumerated.
  @param[out] MaxDevices          The Max number of IDE devices that the bus driver can expect
                                  on this channel. For ATA/ATAPI, this number is either 1 or 2.

  @retval EFI_SUCCESS             Function completes successfully
  @retval Other Values            Something error happened
  @retval EFI_INVALID_PARAMETER   The Channel parameter is invalid
**/
EFI_STATUS
SataInitGetChannelInfo (
  IN   EFI_IDE_CONTROLLER_INIT_PROTOCOL *This,
  IN   UINT8                            Channel,
  OUT  BOOLEAN                          *Enabled,
  OUT  UINT8                            *MaxDevices
  )
{
  if (Channel < FCH_SATA_MAX_CHANNEL) {
    *Enabled    = TRUE;
    *MaxDevices = FCH_SATA_MAX_DEVICES;
    return EFI_SUCCESS;
  } else {
    *Enabled = FALSE;
    return EFI_INVALID_PARAMETER;
  }
}

/**
  This function is called by IdeBus driver before executing certain actions.
  This allows IDE Controller Init to prepare for each action.

  @param[in] This                 the EFI_IDE_CONTROLLER_INIT_PROTOCOL instance.
  @param[in] Phase                phase indicator defined by IDE_CONTROLLER_INIT protocol
  @param[in] Channel              Channel number (0 based, either 0 or 1)

  @retval EFI_SUCCESS             Function completes successfully
  @retval EFI_INVALID_PARAMETER   Channel parameter is out of range
  @exception EFI_UNSUPPORTED      Phase is not supported
**/
EFI_STATUS
SataInitNotifyPhase (
  IN  EFI_IDE_CONTROLLER_INIT_PROTOCOL  *This,
  IN  EFI_IDE_CONTROLLER_ENUM_PHASE     Phase,
  IN  UINT8                             Channel
  )
{
  EFI_SATA_CONTROLLER_PRIVATE_DATA      *SataPriData;

  if (Channel >= FCH_SATA_MAX_CHANNEL) {
    return EFI_INVALID_PARAMETER;
  }

  SataPriData = SATA_CONTROLLER_PRIVATE_DATA_FROM_THIS (This);

  switch (Phase) {
    case EfiIdeBeforeChannelEnumeration:
    case EfiIdeAfterChannelEnumeration:
    case EfiIdeBeforeChannelReset:
    case EfiIdeAfterChannelReset:
    case EfiIdeBusBeforeDevicePresenceDetection:
    case EfiIdeBusAfterDevicePresenceDetection:
    case EfiIdeResetMode:
      break;

    case EfiIdeBusPhaseMaximum:
    default:
      return EFI_UNSUPPORTED;
      break;
  }

  return EFI_SUCCESS;
}

/**
  This function is called by IdeBus driver to submit EFI_IDENTIFY_DATA data structure
  obtained from IDE deivce. This structure is used to set IDE timing

  @param[in] This                 the EFI_IDE_CONTROLLER_INIT_PROTOCOL instance.
  @param[in] Channel              IDE channel number (0 based, either 0 or 1)
  @param[in] Device               IDE device number
  @param[in] IdentifyData         A pointer to EFI_IDENTIFY_DATA data structure

  @retval EFI_SUCCESS             Function completes successfully
  @retval EFI_INVALID_PARAMETER   Channel or Device parameter is out of range
**/
EFI_STATUS
SataInitSubmitData (
  IN  EFI_IDE_CONTROLLER_INIT_PROTOCOL  *This,
  IN  UINT8                             Channel,
  IN  UINT8                             Device,
  IN  EFI_IDENTIFY_DATA                 *IdentifyData
  )
{
  EFI_SATA_CONTROLLER_PRIVATE_DATA      *SataPriData;

  if (Channel >= FCH_SATA_MAX_CHANNEL || Device >= FCH_SATA_MAX_DEVICES) {
    return EFI_INVALID_PARAMETER;
  }

  SataPriData = SATA_CONTROLLER_PRIVATE_DATA_FROM_THIS (This);
  ASSERT (SataPriData);

  if (IdentifyData != NULL) {
    CopyMem (
      &(SataPriData->IdentifyData[Channel][Device]),
      IdentifyData,
      sizeof (EFI_IDENTIFY_DATA)
      );

    SataPriData->IdentifyValid [Channel][Device] = TRUE;
  } else {
    SataPriData->IdentifyValid [Channel][Device] = FALSE;
  }

  return EFI_SUCCESS;
}

/**
  This function is called by IdeBus driver to disqualify unsupported operation
  mode on specfic IDE device

  @param[in] This                 The EFI_IDE_CONTROLLER_INIT_PROTOCOL instance.
  @param[in] Channel              IDE channel number (0 based, either 0 or 1)
  @param[in] Device               IDE device number
  @param[in] BadModes             Operation mode indicator

  @retval EFI_SUCCESS             Function completes successfully
  @retval EFI_INVALID_PARAMETER   Channel parameter or Devicde parameter is out of range,
                                  or BadModes is NULL
**/
EFI_STATUS
SataInitDisqualifyMode (
  IN  EFI_IDE_CONTROLLER_INIT_PROTOCOL  *This,
  IN  UINT8                             Channel,
  IN  UINT8                             Device,
  IN  EFI_ATA_COLLECTIVE_MODE           *BadModes
  )
{
  EFI_SATA_CONTROLLER_PRIVATE_DATA      *SataPriData;

  if (Channel >= FCH_SATA_MAX_CHANNEL || Device >= FCH_SATA_MAX_DEVICES || BadModes == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  SataPriData = SATA_CONTROLLER_PRIVATE_DATA_FROM_THIS (This);
  ASSERT (SataPriData);

  CopyMem (
    &(SataPriData->DisqulifiedModes [Channel][Device]),
    BadModes,
    sizeof (EFI_ATA_COLLECTIVE_MODE)
    );

  return EFI_SUCCESS;
}

/**
  This function is called by IdeBus driver to calculate the best operation mode
  supported by specific IDE device

  @param[in] This                 The EFI_IDE_CONTROLLER_INIT_PROTOCOL instance.
  @param[in] Channel              IDE channel number (0 based, either 0 or 1)
  @param[in] Device               IDE device number
  @param[in, out] SupportedModes  Modes collection supported by IDE device

  @retval EFI_SUCCESS             Function completes successfully
  @retval EFI_INVALID_PARAMETER   Channel parameter or Device parameter is out of range;
                                  Or SupportedModes is NULL
  @retval EFI_NOT_READY           Identify data is not valid
  @retval EFI_OUT_OF_RESOURCES    SupportedModes is out of range
**/
EFI_STATUS
SataInitCalculateMode (
  IN     EFI_IDE_CONTROLLER_INIT_PROTOCOL  *This,
  IN     UINT8                             Channel,
  IN     UINT8                             Device,
  IN OUT EFI_ATA_COLLECTIVE_MODE           **SupportedModes
  )
{
  EFI_STATUS                            Status;
  EFI_SATA_CONTROLLER_PRIVATE_DATA      *SataPriData;
  BOOLEAN                               IdentifyValid;
  EFI_IDENTIFY_DATA                     *IdentifyData;
  EFI_ATA_COLLECTIVE_MODE               *DisqulifiedModes;
  UINT16                                SelectedMode;

  if (Channel >= FCH_SATA_MAX_CHANNEL || Device >= FCH_SATA_MAX_DEVICES) {
    return EFI_INVALID_PARAMETER;
  }

  *SupportedModes = AllocateZeroPool (sizeof (EFI_ATA_COLLECTIVE_MODE));
  if (*SupportedModes == NULL) {
    ASSERT (FALSE);
    return EFI_OUT_OF_RESOURCES;
  }

  SataPriData = SATA_CONTROLLER_PRIVATE_DATA_FROM_THIS (This);
  ASSERT (SataPriData);

  IdentifyData      = &(SataPriData->IdentifyData [Channel][Device]);
  DisqulifiedModes  = &(SataPriData->DisqulifiedModes [Channel][Device]);
  IdentifyValid     = SataPriData->IdentifyValid [Channel][Device];

  if (!IdentifyValid) {
    return EFI_NOT_READY;
  }

  //
  // Detect PIO Mode
  //
  Status = CalculateBestPioMode (
             IdentifyData,
             (DisqulifiedModes->PioMode.Valid ? ((UINT16 *) &(DisqulifiedModes->PioMode.Mode)) : NULL),
             &SelectedMode
             );

  if (!EFI_ERROR (Status)) {
    (*SupportedModes)->PioMode.Valid  = TRUE;
    (*SupportedModes)->PioMode.Mode   = SelectedMode;
  } else {
    (*SupportedModes)->PioMode.Valid  = FALSE;
  }

  //
  // Detect UDMA Mode
  //
  Status = CalculateBestUdmaMode (
             IdentifyData,
             (DisqulifiedModes->UdmaMode.Valid ? ((UINT16 *) &(DisqulifiedModes->UdmaMode.Mode)) : NULL),
             &SelectedMode
             );

  if (!EFI_ERROR (Status)) {
    (*SupportedModes)->UdmaMode.Valid = TRUE;
    (*SupportedModes)->UdmaMode.Mode  = SelectedMode;
  } else {
    (*SupportedModes)->UdmaMode.Valid = FALSE;
  }

  ///
  /// The modes other than PIO and UDMA are not supported by SATA controller
  ///
  return EFI_SUCCESS;
}

/**
  This function is called by IdeBus driver to set appropriate timing on IDE
  controller according supported operation mode

  @param[in] This                 The EFI_IDE_CONTROLLER_INIT_PROTOCOL instance.
  @param[in] Channel              IDE channel number (0 based, either 0 or 1)
  @param[in] Device               IDE device number
  @param[in] Modes                Operation modes

  @retval EFI_SUCCESS             This function always returns EFI_SUCCESS
**/
EFI_STATUS
SataInitSetTiming (
  IN  EFI_IDE_CONTROLLER_INIT_PROTOCOL  *This,
  IN  UINT8                             Channel,
  IN  UINT8                             Device,
  IN  EFI_ATA_COLLECTIVE_MODE           *Modes
  )
{
  EFI_SATA_CONTROLLER_PRIVATE_DATA      *SataPriData;

  if (Channel >= FCH_SATA_MAX_CHANNEL || Device >= FCH_SATA_MAX_DEVICES || Modes == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  SataPriData = SATA_CONTROLLER_PRIVATE_DATA_FROM_THIS (This);
  ASSERT (SataPriData);

  if (!(SataPriData->IdentifyValid[Channel][Device])) {
    return EFI_NOT_READY;
  }

  //
  // Set UDMA timing
  //
  if (Modes->UdmaMode.Valid) {
    //
    // Program UDMA Mode (PCI register, bus master)
    //
    IdeInitSetUdmaTiming (Channel, Device, SataPriData->PciIo, Modes);
  } else if (!Modes->PioMode.Valid) {
    return EFI_UNSUPPORTED;
  }
  return EFI_SUCCESS;
}

/**
  Enable SATA controller to work in IDE mode

  @param[in] PciIo                A pointer points to the PcI IO Protocol instance

  @retval EFI_SUCCESS             The device is started
  @retval Other values            Something error happened
**/
EFI_STATUS
EnableSataController (
  IN  EFI_PCI_IO_PROTOCOL               *PciIo
  )
{
  EFI_STATUS        Status;
  UINT32            Value32;
  UINT8             Value8;
  UINTN             Segment, Bus, Dev, Func;
  UINTN             SataBar5;
  UINT16            IoBase;
  UINTN             Index, Index2;
  UINT8             BarIndex;
  UINT16            OldCfgCommand;
  UINTN             PciAddress;

  Value32 = 0;
  Value8 = 0;
  SataBar5 = 0;
  IoBase = 0;
  Index = Index2 = 0;
  BarIndex = 0;

  Status = PciIo->GetLocation (
                    PciIo,
                    &Segment,
                    &Bus,
                    &Dev,
                    &Func
                    );
  if (EFI_ERROR (Status)) {
    ASSERT_EFI_ERROR (Status);
    return Status;
  }

  PciAddress = PCI_EXPRESS_LIB_ADDRESS (Bus, Dev, Func, 0);

  OldCfgCommand = PciExpressRead16 (PciAddress + R_PCI_CFG_COMMAND);

  PciExpressOr16 (PciAddress + R_PCI_CFG_COMMAND, (V_IO_ACCESS_ENABLE + V_MEMORY_ACCESS_ENABLE));

  Status = PciIo->Io.Read (
                       PciIo,
                       EfiPciIoWidthUint8,
                       0x04,
                       0x02,
                       0x01,
                       &Value8
                       );
  if (!EFI_ERROR (Status)) {
    Value8 |= BIT5+BIT6;
    Status = PciIo->Io.Write (
                         PciIo,
                         EfiPciIoWidthUint8,
                         0x04,
                         0x02,
                         0x01,
                         &Value8
                         );
  }

  Status = PciIo->Io.Read (
                       PciIo,
                       EfiPciIoWidthUint8,
                       0x04,
                       0x0a,
                       0x01,
                       &Value8
                       );
  if (!EFI_ERROR (Status)) {
    Value8 |= BIT5+BIT6;
    Status = PciIo->Io.Write (
                         PciIo,
                         EfiPciIoWidthUint8,
                         0x04,
                         0x0a,
                         0x01,
                         &Value8
                         );
  }

  PciExpressWrite8 (PciAddress + R_CACHE_LINE, 0x10);
  PciExpressWrite8 (PciAddress + R_LATENCY_TIME, 0x40);

  SataBar5 = PciExpressRead32 (PciAddress + R_BAR5) & 0xFFFFFFF0;

  //
  // Detect Device
  //
  Value32 = 0x128;
  Index = 0x04;
  do {
    if ((MmioRead8 (SataBar5 + Value32) & 0x0f) == 0x03) {
      //
      //  GetIdeIoBaseCmd
      //
      Value8 = 0xA0;
      if (Index <= 2) {
        Value8 = 0xB0;
      }
      BarIndex = 0x0;
      if (Index & BIT0) {
        BarIndex = 0x2;
      }
      Status = PciIo->Io.Write (PciIo,
                                EfiPciIoWidthUint8,
                                BarIndex,
                                0x06,
                                0x01,
                                &Value8
                               );
      //
      // WaitDriverReady
      //
      for (Index2 = 0; Index2 < 1000; Index2++) {
        Status = PciIo->Io.Read (PciIo,
                                 EfiPciIoWidthUint8,
                                 BarIndex,
                                 0x07,
                                 0x01,
                                 &Value8
                                );
        if (Value8 == 0x50) break;
        if (Value8 == 0x00) break;
        MicroSecondDelay (10); //Wait 10ms
      }
    }
    Value32 += 0x80;
    Index --;
  } while (Index);

  PciExpressWrite16 (PciAddress + R_PCI_CFG_COMMAND, OldCfgCommand);

  return EFI_SUCCESS;
}

/**
  This function is used to calculate the best PIO mode supported by
  specific IDE device

  @param[in] IdentifyData         The identify data of specific IDE device
  @param[in] DisPioMode           Disqualified PIO modes collection
  @param[out] SelectedMode        Available PIO modes collection

  @retval EFI_SUCCESS             Function completes successfully
  @exception EFI_UNSUPPORTED      Some invalid condition
**/
EFI_STATUS
CalculateBestPioMode (
  IN  EFI_IDENTIFY_DATA                 *IdentifyData,
  IN  UINT16                            *DisPioMode OPTIONAL,
  OUT UINT16                            *SelectedMode
  )
{
  UINT16                                PioMode;
  UINT16                                AdvancedPioMode;
  UINT16                                Temp;
  UINT16                                Index;
  UINT16                                MinimumPioCycleTime;

  Temp    = 0xff;

  PioMode = (UINT8) (IdentifyData->AtaData.obsolete_51_52[0] >> 8);

  //
  // see whether Identify Data word 64 - 70 are valid
  //
  if ((IdentifyData->AtaData.field_validity & 0x02) == 0x02) {
    AdvancedPioMode = IdentifyData->AtaData.advanced_pio_modes;
    for (Index = 0; Index < 8; Index++) {
      if ((AdvancedPioMode & 0x01) != 0) {
        Temp = Index;
      }
      AdvancedPioMode >>= 1;
    }

    //
    // if Temp is modified, meant the advanced_pio_modes is not zero;
    // if Temp is not modified, meant the no advanced PIO Mode is supported,
    // the best PIO Mode is the value in pio_cycle_timing.
    //
    if (Temp != 0xff) {
      AdvancedPioMode = (UINT16) (Temp + 3);
    } else {
      AdvancedPioMode = PioMode;
    }

    //
    // Limit the PIO mode to at most PIO4.
    //
    PioMode             = (UINT16) (AdvancedPioMode < 4 ? AdvancedPioMode : 4);
    MinimumPioCycleTime = IdentifyData->AtaData.min_pio_cycle_time_with_flow_control;

    if (MinimumPioCycleTime <= 120) {
      PioMode = (UINT16) (4 < PioMode ? 4 : PioMode);
    } else if (MinimumPioCycleTime <= 180) {
      PioMode = (UINT16) (3 < PioMode ? 3 : PioMode);
    } else if (MinimumPioCycleTime <= 240) {
      PioMode = (UINT16) (2 < PioMode ? 2 : PioMode);
    } else {
      PioMode = 0;
    }

    //
    // Degrade the PIO mode if the mode has been disqualified
    //
    if (DisPioMode != NULL) {
      if (*DisPioMode < 2) {
        //
        // no mode below ATA_PIO_MODE_BELOW_2
        //
        return EFI_UNSUPPORTED;
      }

      if (PioMode >= *DisPioMode) {
        PioMode = (UINT16) (*DisPioMode - 1);
      }
    }

    if (PioMode < 2) {
      //
      // ATA_PIO_MODE_BELOW_2;
      //
      *SelectedMode = 1;
    } else {
      //
      // ATA_PIO_MODE_2 to ATA_PIO_MODE_4;
      //
      *SelectedMode = PioMode;
    }
  } else {
    //
    // Identify Data word 64 - 70 are not valid
    // Degrade the PIO mode if the mode has been disqualified
    //
    if (DisPioMode != NULL) {
      if (*DisPioMode < 2) {
        return EFI_UNSUPPORTED;
      }
      if (PioMode == *DisPioMode) {
        PioMode--;
      }
    }

    if (PioMode < 2) {
      *SelectedMode = 1;
    } else {
      *SelectedMode = 2;
    }
  }
  return EFI_SUCCESS;
}

/**
  This function is used to calculate the best UDMA mode supported by
  specific IDE device

  @param[in] IdentifyData         The identify data of specific IDE device
  @param[in] DisUDmaMode          Disqualified UDMA modes collection
  @param[out] SelectedMode        Available UMDA modes collection

  @retval EFI_SUCCESS             Function completes successfully
  @exception EFI_UNSUPPORTED      Some invalid condition
**/
EFI_STATUS
CalculateBestUdmaMode (
  IN  EFI_IDENTIFY_DATA                 *IdentifyData,
  IN  UINT16                            *DisUDmaMode OPTIONAL,
  OUT UINT16                            *SelectedMode
  )
{
  UINT16                                TempMode;
  UINT16                                UDmaMode;

  //
  // flag for 'Udma mode is not supported'
  //
  UDmaMode = 0;

  //
  // Check whether the WORD 88 (supported UltraDMA by drive) is valid
  //
  if ((IdentifyData->AtaData.field_validity & 0x04) == 0x00) {
    return EFI_UNSUPPORTED;
  }

  UDmaMode = IdentifyData->AtaData.ultra_dma_mode;
  UDmaMode &= 0x3f;

  //
  // initialize it to UDMA-0
  //
  TempMode = 0;

  while ((UDmaMode >>= 1) != 0) {
    TempMode ++;
  }

  //
  // Degrade the UDMA mode if the mode has been disqualified
  //
  if (DisUDmaMode != NULL) {
    if (*DisUDmaMode == 0) {
      *SelectedMode = 0;
      return EFI_UNSUPPORTED;
    }

    if (TempMode >= *DisUDmaMode) {
      TempMode = (UINT16) (*DisUDmaMode - 1);
    }
  }

  //
  // Possible returned mode is between ATA_UDMA_MODE_0 and ATA_UDMA_MODE_5
  //
  *SelectedMode = TempMode;

  return EFI_SUCCESS;
}

/**
  This function is used to set appropriate UDMA timing on Ide
  controller according supported UDMA modes

  @param[in] Channel              IDE channel number (0 based, either 0 or 1).
                                  For LPT IDE-R there is only one (See IDER_MAX_CHANNEL).
  @param[in] Device               IDE device number
  @param[in] PciIo                Pointer to PciIo protocol opened by Ide controller driver
  @param[in] Modes                The UDMA mode collection supported by IDE device

  @retval                         Status code returned by PciIo operations
**/
EFI_STATUS
IdeInitSetUdmaTiming (
  IN  UINT8                             Channel,
  IN  UINT8                             Device,
  IN  EFI_PCI_IO_PROTOCOL               *PciIo,
  IN  EFI_ATA_COLLECTIVE_MODE           *Modes
  )
{
  UINT16                                PciCommandReg;

  //
  // Now set the FCH IDE Bus Master Enable bit, one bit for FCH controller
  // If BME bit is not set, set it
  //
  PciIo->Pci.Read (
                   PciIo,
                   EfiPciIoWidthUint16,
                   R_PCI_CFG_COMMAND,
                   1,
                   &PciCommandReg
                  );

  if (!(PciCommandReg & BME_BUS_MASTER_ENABLE_BIT)) {
    PciCommandReg |= BME_BUS_MASTER_ENABLE_BIT;

    PciIo->Pci.Write (
                      PciIo,
                      EfiPciIoWidthUint16,
                      R_PCI_CFG_COMMAND,
                      1,
                      &PciCommandReg
                     );
  }
  return EFI_SUCCESS;
}

/**
  Retrieves a Unicode string that is the user readable name of the EFI Driver.

  @param[in] This                 A pointer to the EFI_COMPONENT_NAME_PROTOCOL instance.
  @param[in] Language             A pointer to a three character ISO 639-2 language identifier.
                                  This is the language of the driver name that that the caller
                                  is requesting, and it must match one of the languages specified
                                  in SupportedLanguages.  The number of languages supported by a
                                  driver is up to the driver writer.
  @param[out] DriverName          A pointer to the Unicode string to return.  This Unicode string
                                  is the name of the driver specified by This in the language
                                  specified by Language.

  @retval EFI_SUCCESS             The Unicode string for the Driver specified by This
                                  and the language specified by Language was returned
                                  in DriverName.
  @retval EFI_INVALID_PARAMETER   Language is NULL.
  @retval EFI_INVALID_PARAMETER   DriverName is NULL.
  @exception EFI_UNSUPPORTED      The driver specified by This does not support the
                                  language specified by Language.
**/
EFI_STATUS
EFIAPI
SataControllerGetDriverName (
  IN  EFI_COMPONENT_NAME_PROTOCOL       *This,
  IN  CHAR8                             *Language,
  OUT CHAR16                            **DriverName
  )
{
  return LookupUnicodeString (
           Language,
           gSataControllerName.SupportedLanguages,
           mSataControllerDriverNameTable,
           DriverName
           );
}

/**
  Retrieves a Unicode string that is the user readable name of the controller
  that is being managed by an EFI Driver.

  @param[in] This                 A pointer to the EFI_COMPONENT_NAME_PROTOCOL instance.
  @param[in] ControllerHandle     The handle of a controller that the driver specified by
                                  This is managing.  This handle specifies the controller
                                  whose name is to be returned.
  @param[in] ChildHandle          The handle of the child controller to retrieve the name
                                  of.  This is an optional parameter that may be NULL.  It
                                  will be NULL for device drivers.  It will also be NULL
                                  for a bus drivers that wish to retrieve the name of the
                                  bus controller.  It will not be NULL for a bus driver
                                  that wishes to retrieve the name of a child controller.
  @param[in] Language             A pointer to a three character ISO 639-2 language
                                  identifier.  This is the language of the controller name
                                  that that the caller is requesting, and it must match one
                                  of the languages specified in SupportedLanguages.  The
                                  number of languages supported by a driver is up to the
                                  driver writer.
  @param[out] ControllerName      A pointer to the Unicode string to return.  This Unicode
                                  string is the name of the controller specified by
                                  ControllerHandle and ChildHandle in the language
                                  specified by Language from the point of view of the
                                  driver specified by This.

  @retval EFI_SUCCESS             The Unicode string for the user readable name in the
                                  language specified by Language for the driver
                                  specified by This was returned in DriverName.
  @retval EFI_INVALID_PARAMETER   ControllerHandle is not a valid EFI_HANDLE.
                                  ChildHandle is not NULL and it is not a valid EFI_HANDLE.
  @retval EFI_INVALID_PARAMETER   Language is NULL.
                                  ControllerName is NULL.
  @exception EFI_UNSUPPORTED      The driver specified by This is not currently
                                  managing the controller specified by
                                  ControllerHandle and ChildHandle.
                                  The driver specified by This does not support the
                                  language specified by Language.
**/
EFI_STATUS
EFIAPI
SataControllerGetControllerName (
  IN  EFI_COMPONENT_NAME_PROTOCOL       *This,
  IN  EFI_HANDLE                        ControllerHandle,
  IN  EFI_HANDLE                        ChildHandle OPTIONAL,
  IN  CHAR8                             *Language,
  OUT CHAR16                            **ControllerName
  )
{
  return LookupUnicodeString (
           Language,
           gSataControllerName.SupportedLanguages,
           mSataControllerControllerNameTable,
           ControllerName
           );
}

