/** @file
  The device manager implementation

;******************************************************************************
;* Copyright (c) 2012 - 2017, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "DeviceManagerDxe.h"

EFI_LOAD_FILE_PROTOCOL               mDeviceManagerLoadFile = {DeviceManagerLoadFile};

/**
  Enter device manager.

  @param[in]      This          Protocol instance pointer.
  @param[in]      FilePath      The device specific path of the file to load.
  @param[in]      BootPolicy    If TRUE, indicates that the request originates from the boot manager is attempting to
                                load FilePath as a boot selection. If FALSE, then FilePath must match as exact file to be loaded.
  @param[in, out] BufferSize    On input the size of Buffer in bytes. On output with a return code of EFI_SUCCESS,
                                the amount of data transferred to Buffer. On output with a return code of EFI_BUFFER_TOO_SMALL,
                                the size of Buffer required to retrieve the requested file.
  @param[in]  Buffer            The memory buffer to transfer the file to. IF Buffer is NULL, then no the size of the
                                requested file is returned in BufferSize.

  @retval EFI_NOT_FOUND         Always return not found.
**/
EFI_STATUS
EFIAPI
DeviceManagerLoadFile (
  IN EFI_LOAD_FILE_PROTOCOL           *This,
  IN EFI_DEVICE_PATH_PROTOCOL         *FilePath,
  IN BOOLEAN                          BootPolicy,
  IN OUT UINTN                        *BufferSize,
  IN VOID                             *Buffer OPTIONAL
  )
{
  EFI_FORM_BROWSER2_PROTOCOL          *FormBrowser2;
  EFI_STATUS                          Status;
  EFI_HII_HANDLE                      HiiHandle;
  EFI_BROWSER_ACTION_REQUEST          ActionRequest;

  Status = gBS->LocateProtocol (&gEfiFormBrowser2ProtocolGuid, NULL, (VOID **) &FormBrowser2);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  HiiHandle = HiiGetHiiHandleByFormSetGuid (&gDeviceManagerFormSetGuid);
  if (HiiHandle == NULL) {
    return EFI_NOT_FOUND;
  }

  if (IsPasswordExist ()) {
    Status = CheckPassword ();
  } else {
    UnlockPasswordState (NULL, 0);
  }

  ActionRequest = EFI_BROWSER_ACTION_REQUEST_NONE;
  Status = FormBrowser2->SendForm (
                           FormBrowser2,
                           &HiiHandle,
                           1,
                           &gDeviceManagerFormSetGuid,
                           0,
                           NULL,
                           &ActionRequest
                           );
  if (ActionRequest == EFI_BROWSER_ACTION_REQUEST_RESET) {
    EnableResetRequired ();
  }

  LockPasswordState ();

  return EFI_NOT_FOUND;
}

/**
 Install the instance of the EFI_LOAD_FILE_PROTOCOL and EFI_DEVICE_PATH_PROTOCOL on new handle.

 @param[in] ImageHandle    The firmware allocated handle for the EFI image.
 @param[in] SystemTable    A pointer to the EFI System Table.

 @retval EFI_SUCCESS       Install protocol successfully.
 @retval other             Fail to install protocol instance.
**/
EFI_STATUS
EFIAPI
DeviceManagerEntry (
  IN EFI_HANDLE                       ImageHandle,
  IN EFI_SYSTEM_TABLE                 *SystemTable
  )
{
  EFI_HANDLE                          DriverHandle;
  EFI_STATUS                          Status;

  DriverHandle = NULL;
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &DriverHandle,
                  &gEfiDevicePathProtocolGuid,
                  &gH2ODeviceManagerDevicePath,
                  &gEfiLoadFileProtocolGuid,
                  &mDeviceManagerLoadFile,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);

  return Status;
}
