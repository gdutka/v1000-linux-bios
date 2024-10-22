/** @file
  This driver provides optionally protocol to override or provide EDID
  information and/or output device display properties to the producer
  of the Graphics Output protocol.

;******************************************************************************
;* Copyright (c) 2013 - 2019, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "EdidOverrideDxe.h"

EFI_STATUS
EFIAPI
GetEdid (
  IN  EFI_EDID_OVERRIDE_PROTOCOL        *This,
  IN  EFI_HANDLE                        *ChildHandle,
  OUT UINT32                            *Attributes,
  IN OUT UINTN                          *EdidSize,
  IN OUT UINT8                          **Edid
  );

EFI_EDID_OVERRIDE_PROTOCOL mEdidOverrideProtocolTemplate = { GetEdid };

EFI_STATUS
EFIAPI
EdidOverrideEntryPoint (
  IN EFI_HANDLE                         ImageHandle,
  IN EFI_SYSTEM_TABLE                   *SystemTable
  )
{
  EFI_STATUS                            Status;
  EFI_HANDLE                            EdidOverrideHandle;

  EdidOverrideHandle = NULL;
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &EdidOverrideHandle,
                  &gEfiEdidOverrideProtocolGuid,
                  &mEdidOverrideProtocolTemplate,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
GetEdid (
  IN  EFI_EDID_OVERRIDE_PROTOCOL        *This,
  IN  EFI_HANDLE                        *ChildHandle,
  OUT UINT32                            *Attributes,
  IN OUT UINTN                          *EdidSize,
  IN OUT UINT8                          **Edid
  )
{
  EFI_STATUS                            Status;
  EFI_STATUS                            EdidOverrideStatus;
  OEM_GET_EDID_OVERRIDE_DEFINITION      GetEdidOverride;
  EFI_DEVICE_PATH_PROTOCOL              *ChildDevPath;
  EFI_DEVICE_PATH_PROTOCOL              *SelectedVideoPortPath;
  EFI_EDID_OVERRIDE_PROTOCOL            *EdidOverride;
  UINTN                                 Size;

  EdidOverrideStatus = EFI_UNSUPPORTED;

  Status =  gBS->HandleProtocol (
                   *ChildHandle,
                   &gEfiDevicePathProtocolGuid,
                   (VOID*)&ChildDevPath
                   );
  if (EFI_ERROR (Status)) {
    goto Done;
  }
  ZeroMem (&GetEdidOverride, sizeof(GetEdidOverride));
  if ((*Edid != NULL) && (*EdidSize != 0)) {
    GetEdidOverride.EdidSize = *EdidSize;
    GetEdidOverride.EdidData = *Edid;
  }

  DEBUG ((DEBUG_INFO, "OemKernelServices Call: OemSvcGetEdidOverride \n"));
  Status = OemSvcGetEdidOverride (&GetEdidOverride);
  DEBUG ((DEBUG_INFO, "OemKernelServices OemSvcGetEdidOverride Status: %r\n", Status));
  if (EFI_ERROR (Status) && Status != EFI_MEDIA_CHANGED) {
    goto Done;
  }

  if ((GetEdidOverride.EdidSize != 0)) {
    //
    // return edid override for only a single child port
    //
    SelectedVideoPortPath = GetNextDevicePathInstance (&GetEdidOverride.DisplayDevPath, &Size);
    if (CompareMem (SelectedVideoPortPath, ChildDevPath, Size) == 0) {
      *Attributes = GetEdidOverride.Attributes;
      *EdidSize = GetEdidOverride.EdidSize;
      *Edid = GetEdidOverride.EdidData;
      Status = gBS->HandleProtocol (
                      *ChildHandle,
                      &gEfiEdidOverrideProtocolGuid,
                      (VOID*)&EdidOverride
                      );
      if (EFI_ERROR (Status)) {
        Status = gBS->InstallMultipleProtocolInterfaces (
                        ChildHandle,
                        &gEfiEdidOverrideProtocolGuid,
                        &mEdidOverrideProtocolTemplate,
                        NULL
                        );
      }

      EdidOverrideStatus = EFI_SUCCESS;
    }
  }

Done:
  return EdidOverrideStatus;
}
