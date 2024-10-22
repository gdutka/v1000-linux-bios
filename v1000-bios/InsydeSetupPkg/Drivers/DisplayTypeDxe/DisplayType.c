/** @file
Display type

;******************************************************************************
;* Copyright (c) 2013 - 2021, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "InternalDisplayType.h"
#include <Guid/DebugMask.h>

EFI_EVENT         mStoRegistEvent;
VOID              *mStoRegistration;

EFI_EVENT         mGopRegistEvent;
VOID              *mGopRegistration;

UINT8             mPrimaryDisplay = DisplayTypeMax;

DISPLAY_TYPE_PCDGUID_MAP mDisplayTypePcdGuidMap [] = {
                           {Igfx          , (EFI_GUID *)PcdGetPtr (PcdDisplayEngineIgfx)},
                           {Peg           , (EFI_GUID *)PcdGetPtr (PcdDisplayEnginePeg )},
                           {Pci           , (EFI_GUID *)PcdGetPtr (PcdDisplayEnginePci )},
                           {Serial        , (EFI_GUID *)&gH2ODisplayEngineLocalTextGuid},
                           {DisplayTypeMax, NULL                                       }
                           };

/**
  Find device path node.

  @param  [in] DevPath          Given device path instance
  @param  [in]     DevPathType      Device path type
  @param  [in]     DevPathSubType   Device path sub type

  @return   Device path
**/
EFI_DEVICE_PATH_PROTOCOL *
LocateDevicePathNode (
  IN OUT EFI_DEVICE_PATH_PROTOCOL   *DevicePath,
  IN UINT8                          DevPathType,
  IN UINT8                          DevPathSubType
  )
{
  if (DevicePath == NULL) {
    return NULL;
  }

  while (!IsDevicePathEnd (DevicePath)) {
    if (DevicePath->Type == DevPathType && DevicePath->SubType == DevPathSubType) {
      return DevicePath;
    }
    DevicePath = NextDevicePathNode(DevicePath);
  }

  if (DevPathType == END_DEVICE_PATH_TYPE && DevPathSubType == END_ENTIRE_DEVICE_PATH_SUBTYPE) {
    return DevicePath;
  }

  return NULL;
}

EFI_STATUS
GetDisplayTypeByDp (
  IN  EFI_DEVICE_PATH_PROTOCOL                *DevicePath,
  OUT UINT8                                   *DisplayType
  )
{
  EFI_STATUS                   Status;
  EFI_HANDLE                   Handle;
  EFI_PCI_IO_PROTOCOL          *PciIo;
  PCI_TYPE00                   PciType;
  BOOLEAN                      IsRootBus;
  EFI_DEVICE_PATH_PROTOCOL     *UartDevicePath;
  EFI_DEVICE_PATH_PROTOCOL     *OrgDevicePath;

  *DisplayType  = Pci;

  UartDevicePath = LocateDevicePathNode (DevicePath, MESSAGING_DEVICE_PATH, MSG_UART_DP);
  if (UartDevicePath != NULL) {
    *DisplayType = Serial;
    return EFI_SUCCESS;
  }

  OrgDevicePath = DevicePath;
  Status = gBS->LocateDevicePath (
                  &gEfiPciIoProtocolGuid,
                  &DevicePath,
                  &Handle
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  Status = gBS->HandleProtocol (
                  Handle,
                  &gEfiPciIoProtocolGuid,
                  (VOID **) &PciIo
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  Status = PciIo->Pci.Read (PciIo, EfiPciIoWidthUint32, 0, sizeof (PciType) / sizeof (UINT32), &PciType);

  IsRootBus = FALSE;
  DevicePath = OrgDevicePath;
  while (!IsDevicePathEnd (DevicePath)) {
    if (DevicePath->Type == HARDWARE_DEVICE_PATH && DevicePath->SubType == HW_PCI_DP) {
      if (!IsRootBus) {
        IsRootBus = TRUE;
      } else {
        IsRootBus = FALSE;
        break;
      }
    }
    DevicePath = NextDevicePathNode(DevicePath);
  }

  if (IsRootBus) {
    *DisplayType = Igfx;
  } else if (IS_PCI_VGA (&PciType)) {
    *DisplayType = Peg;
  } else {
    *DisplayType = Pci;
  }

  return EFI_SUCCESS;
}

EFI_STATUS
GetPrimaryDisplay (
  IN OUT UINT8                          *PrimaryDisplay
  )
{
  EFI_STATUS                            Status;
  UINTN                                 BufferSize;
  UINT8                                 PrimaryDisplayVar;

  if (PrimaryDisplay == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  *PrimaryDisplay = Peg;

  BufferSize = sizeof (PrimaryDisplayVar);
  Status = gRT->GetVariable (
                  L"PrimaryDisplay",
                  &gEfiGenericVariableGuid,
                  NULL,
                  &BufferSize,
                  &PrimaryDisplayVar
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if ((PrimaryDisplayVar == DisplayModeIgfx) ||
      (PrimaryDisplayVar == DisplayModeSg)) {
    *PrimaryDisplay = Igfx;
  } else if ((PrimaryDisplayVar == DisplayModePeg) ||
      (PrimaryDisplayVar == DisplayModeAuto)) {
    *PrimaryDisplay = Peg;
  } else if (PrimaryDisplayVar == DisplayModePci) {
    *PrimaryDisplay = Pci;
  }

  return EFI_SUCCESS;
}

EFI_STATUS
InstallDEGuidByPcd (
  IN EFI_HANDLE                               Handle,
  IN VOID                                     *CheckProtocolGuid,
  IN VOID                                     *DEGuid
  )
{
  EFI_STATUS                                  Status;
  EFI_DEVICE_PATH_PROTOCOL                    *DevicePath;
  UINT8                                       DisplayType;
  UINT32                                      Index;
  VOID                                        *Interface;

  Status = gBS->HandleProtocol (
                  Handle,
                  CheckProtocolGuid,
                  (VOID **)&Interface
                  );
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = gBS->HandleProtocol (
                  Handle,
                  &gEfiDevicePathProtocolGuid,
                  (VOID **)&DevicePath
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if (mPrimaryDisplay == DisplayTypeMax) {
    GetPrimaryDisplay (&mPrimaryDisplay);
  }

  GetDisplayTypeByDp (DevicePath, &DisplayType);
  if (DisplayType == mPrimaryDisplay) {
    //
    // Install gH2ODisplayTypePrimaryDisplayGuid in this handle
    //
    Status = gBS->HandleProtocol (Handle, &gH2ODisplayTypePrimaryDisplayGuid, (VOID **) &Interface);
    if (EFI_ERROR (Status)) {
      Status = gBS->InstallMultipleProtocolInterfaces (
                      &Handle,
                      &gH2ODisplayTypePrimaryDisplayGuid,
                      NULL,
                      NULL
                      );
      ASSERT_EFI_ERROR (Status);
      if (EFI_ERROR (Status)) {
        return Status;
      }
    }
  }

  Index = 0;
  while (mDisplayTypePcdGuidMap[Index].PcdGuid != NULL) {
    if (DisplayType == mDisplayTypePcdGuidMap[Index].DisplayType && CompareGuid (DEGuid, mDisplayTypePcdGuidMap[Index].PcdGuid)){
      Status = gBS->HandleProtocol (Handle, DEGuid, (VOID **) &Interface);
      if (EFI_ERROR (Status)) {
        //
        // Install DEGuid in this handle
        //
        Status = gBS->InstallMultipleProtocolInterfaces (
                        &Handle,
                        DEGuid,
                        NULL,
                        NULL
                        );
        ASSERT_EFI_ERROR (Status);
        if (EFI_ERROR (Status)) {
          return Status;
        }
      }
      break;
    }
    Index ++;
  }

  return EFI_SUCCESS;
}

EFI_STATUS
RegisterNotify (
  IN VOID                                     *Registration,
  IN EFI_GUID                                 *CheckProtocolGuid,
  IN EFI_GUID                                 *DEGuid
  )
{
  EFI_STATUS                                  Status;
  UINTN                                       BufferSize;
  EFI_HANDLE                                  Handle;

  Status = EFI_SUCCESS;
  while (TRUE) {
    BufferSize = sizeof (EFI_HANDLE);
    Status = gBS->LocateHandle (
                    ByRegisterNotify,
                    NULL,
                    Registration,
                    &BufferSize,
                    &Handle
                    );
    if (EFI_ERROR (Status)) {
      //
      // If no more notification events exist
      //
      return Status;
    }

    Status = InstallDEGuidByPcd (Handle, CheckProtocolGuid, DEGuid);
    if (EFI_ERROR (Status)) {
      continue;
    }
  }

  return EFI_SUCCESS;
}

VOID
EFIAPI
StoRegistEvent (
  IN EFI_EVENT                                Event,
  IN VOID                                     *Context
  )
{
  RegisterNotify (
    mStoRegistration,
    &gEfiSimpleTextOutProtocolGuid,
    &gH2ODisplayEngineLocalTextGuid
    );
}

VOID
EFIAPI
GopRegistEvent (
  IN EFI_EVENT                                Event,
  IN VOID                                     *Context
  )
{
  RegisterNotify (
    mGopRegistration,
    &gEfiGraphicsOutputProtocolGuid,
    &gH2ODisplayEngineLocalMetroGuid
    );
}

EFI_STATUS
CreateRegisterProtocolEvent (
  IN VOID                                     **Registration,
  IN VOID                                     *CheckProtocolGuid,
  IN EFI_EVENT                                *Event,
  IN EFI_EVENT_NOTIFY                         NotifyFunction
  )
{
  EFI_STATUS                                  Status;

  //
  // Register Protocol notify for Registration
  //
  Status = gBS->CreateEvent (
                  EVT_NOTIFY_SIGNAL,
                  TPL_CALLBACK,
                  NotifyFunction,
                  NULL,
                  Event
                  );
  ASSERT_EFI_ERROR (Status);

  //
  // Register for protocol notifications on this event
  //
  Status = gBS->RegisterProtocolNotify (
                  CheckProtocolGuid,
                  *Event,
                  Registration
                  );
  ASSERT_EFI_ERROR (Status);

  return Status;
}

EFI_STATUS
DisplayTypeEntryPoint (
  IN EFI_HANDLE                               ImageHandle,
  IN EFI_SYSTEM_TABLE                         *SystemTable
  )
{
  EFI_STATUS                                  Status;

  //
  // Register Protocol notify for Sto Registration
  //
  Status = CreateRegisterProtocolEvent (
             &mStoRegistration,
             &gEfiSimpleTextOutProtocolGuid,
             &mStoRegistEvent,
             StoRegistEvent
             );
  ASSERT_EFI_ERROR (Status);

  //
  // Register Protocol notify for Gop Registration
  //
  Status = CreateRegisterProtocolEvent (
             &mGopRegistration,
             &gEfiGraphicsOutputProtocolGuid,
             &mGopRegistEvent,
             GopRegistEvent
             );
  ASSERT_EFI_ERROR (Status);

  return Status;
}

