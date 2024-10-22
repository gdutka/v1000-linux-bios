/** @file

;******************************************************************************
;* Copyright (c) 2015, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "VideoOutputPortSelection.h"

EFI_DEVICE_PATH_PROTOCOL *
PartMatchInstance (
  IN EFI_DEVICE_PATH_PROTOCOL  *Multi,
  IN EFI_DEVICE_PATH_PROTOCOL  *Single
  )
{
  EFI_DEVICE_PATH_PROTOCOL  *Instance;
  UINTN                     InstanceSize;
  UINTN                     SingleDpSize;
  UINTN                     Size;
  EFI_DEVICE_PATH_PROTOCOL  *NewDevicePath;

  NewDevicePath = NULL;

  if (Multi == NULL || Single == NULL) {
    return FALSE;
  }

  Instance        =  GetNextDevicePathInstance (&Multi, &InstanceSize);
  SingleDpSize    =  GetDevicePathSize (Single) - END_DEVICE_PATH_LENGTH;
  InstanceSize    -= END_DEVICE_PATH_LENGTH;

  while (Instance != NULL) {
    Size = (SingleDpSize < InstanceSize) ? SingleDpSize : InstanceSize;

    if ((CompareMem (Instance, Single, Size) == 0)) {
      NewDevicePath = AppendDevicePathInstance (NewDevicePath, Instance);
    }
    
    gBS->FreePool (Instance);
    Instance = GetNextDevicePathInstance (&Multi, &InstanceSize);
    InstanceSize  -= END_DEVICE_PATH_LENGTH;
  }

  return NewDevicePath;
}

EFI_STATUS
RemainingDevicePathFromConOutVar (
  IN EFI_HANDLE                               VgaControllerHandle,
  OUT EFI_DEVICE_PATH_PROTOCOL                **RemainingDevicePath
  )
{
  EFI_STATUS                        Status;
  EFI_DEVICE_PATH_PROTOCOL          *ConOutVar;
  UINTN                             ConOutSize;
  EFI_DEVICE_PATH_PROTOCOL          *VideoOutputDevicePath;
  EFI_DEVICE_PATH_PROTOCOL          *ControllerDevicePath;

  ConOutVar = NULL;
  *RemainingDevicePath = NULL;
  VideoOutputDevicePath = NULL;
  
  //
  // Get last run Acpi_ADR Value from ConOut. 
  //
  ConOutVar = BdsLibGetVariableAndSize (
                L"ConOut",
                &gEfiGlobalVariableGuid,  
                &ConOutSize
                );
  if (ConOutVar) {
    Status = gBS->HandleProtocol (
                    VgaControllerHandle, 
                    &gEfiDevicePathProtocolGuid, 
                    (VOID**)&ControllerDevicePath
                    );
    
    VideoOutputDevicePath = PartMatchInstance (ConOutVar, ControllerDevicePath);
    if (VideoOutputDevicePath) {
      while (!IsDevicePathEnd (VideoOutputDevicePath)) {
        if ((DevicePathType (VideoOutputDevicePath) == ACPI_DEVICE_PATH) && (DevicePathSubType (VideoOutputDevicePath) == ACPI_ADR_DP)) {
          if (DevicePathNodeLength(VideoOutputDevicePath) != sizeof (ACPI_ADR_DEVICE_PATH)) {
            return EFI_SUCCESS;
          } else {
            *RemainingDevicePath = VideoOutputDevicePath;
          }
        }
        VideoOutputDevicePath = NextDevicePathNode (VideoOutputDevicePath);
      }
    }      
  }

  gBS->FreePool (ConOutVar);
  return EFI_SUCCESS;
}

EFI_STATUS
AddPlugInVgaHandle (
  IN  EFI_HANDLE    ControllerHandle
  )
{
  EFI_HANDLE                       *HandleList;
  EFI_HANDLE                       *NewHandleList;
  EFI_HANDLE                       *PtrToHandle;
  UINTN                            VarSize;
  UINTN                            NewVarSize;
  EFI_STATUS                       Status;

  NewVarSize = 0;
  NewHandleList = NULL;
  
  HandleList = BdsLibGetVariableAndSize (
                 PLUG_IN_VIDEO_DISPLAY_INFO_VAR_NAME,
                 &gEfiGenericVariableGuid,
                 &VarSize
                 );
  if (HandleList) {
    //
    // Controller was in the list?
    //
    PtrToHandle = HandleList;
    
    while (*PtrToHandle != NULL) {
      if (ControllerHandle == *PtrToHandle) {
        return EFI_SUCCESS;
      }
      PtrToHandle++;
    }
    //
    // Not in the list, add into list
    //
    *PtrToHandle = ControllerHandle;
    NewHandleList = AllocateZeroPool (VarSize + sizeof(EFI_HANDLE));
    CopyMem (NewHandleList, HandleList, VarSize);
    NewVarSize = VarSize + sizeof(EFI_HANDLE);
  } else {
    //
    // The NULL handle is the end of list.
    //
    NewHandleList = AllocateZeroPool (2 * sizeof(EFI_HANDLE));    
    PtrToHandle = NewHandleList;
    *PtrToHandle = ControllerHandle;
    NewVarSize = 2 * sizeof(EFI_HANDLE);
  } 

  Status = gRT->SetVariable (
                  PLUG_IN_VIDEO_DISPLAY_INFO_VAR_NAME,
                  &gEfiGenericVariableGuid,
                  EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                  NewVarSize,
                  NewHandleList
                  );

  if (HandleList) {
    gBS->FreePool (HandleList);
  }

  if (NewHandleList) {
    gBS->FreePool (NewHandleList);
  }

  return Status;
}

EFI_STATUS
GetPlugInVgaADR (
  IN      EFI_HANDLE                          VgaControllerHandle,
  IN OUT  EFI_DEVICE_PATH_PROTOCOL            **RemainingDevicePath
  )
{
  EFI_STATUS                               Status;

  RemainingDevicePathFromConOutVar (VgaControllerHandle, RemainingDevicePath);

  Status = AddPlugInVgaHandle (VgaControllerHandle);  

  return Status;
}

EFI_STATUS
GetIgdADR (
  IN      EFI_HANDLE                       VgaControllerHandle,
  IN OUT  EFI_DEVICE_PATH_PROTOCOL         **RemainingDevicePath
  )
{
  EFI_STATUS                               Status;

//  RemainingDevicePathFromConOutVar (VgaControllerHandle, RemainingDevicePath);

  Status = AddPlugInVgaHandle (VgaControllerHandle);  

  return Status;
}

BOOLEAN
IsIGDController (
  IN EFI_HANDLE  VgaControllerHandle
  )
{
  EFI_STATUS                Status;
  UINTN                     Index;
  EFI_DEVICE_PATH_PROTOCOL  *ControllerDevicePath;
  
  Index = 0;

  Status = gBS->HandleProtocol (
                  VgaControllerHandle, 
                  &gEfiDevicePathProtocolGuid, 
                  (VOID**)&ControllerDevicePath
                  );
  if (EFI_ERROR (Status)) {
    return FALSE;
  }

  while (gPlatformAllPossibleIgfxConsole[Index] != NULL) {
    if (CompareMem (
          ControllerDevicePath,
          gPlatformAllPossibleIgfxConsole[Index],
          GetDevicePathSize (ControllerDevicePath) - END_DEVICE_PATH_LENGTH
          ) == 0) {
      return TRUE;      
    }
    Index++;
  }

  return FALSE;
}

EFI_STATUS 
PlatformBdsDisplayPortSelection (
  IN EFI_HANDLE                                VgaControllerHandle,
  IN OUT EFI_DEVICE_PATH_PROTOCOL              **RemainingDevicePath
  )
{
  EFI_STATUS                                Status;
  EFI_LEGACY_BIOS_PROTOCOL                  *LegacyBios;
  
  //
  // Set RemainingDevicePath as default (NULL). 
  //
  *RemainingDevicePath = NULL;
  //
  // Legacy BIOS exist, return Unsupported.
  //
  Status = gBS->LocateProtocol (
                  &gEfiLegacyBiosProtocolGuid,
                  NULL,
                  (VOID**)&LegacyBios
                  );
  if (Status == EFI_SUCCESS) {
    return EFI_UNSUPPORTED;
  }
  //
  // If the IGD....
  //
  if (IsIGDController(VgaControllerHandle)) {
    Status = GetIgdADR (
               VgaControllerHandle, 
               RemainingDevicePath
               );
  } else {
    //
    // If the Plug-in Vga Card...
    //
    Status = GetPlugInVgaADR (
               VgaControllerHandle, 
               RemainingDevicePath
               );
  }
  
  return Status;
}

EFI_STATUS
PlatformBdsVgaConnectedFailCallback (  
  IN  EFI_HANDLE               VgaControllerHandle, 
  IN  EFI_DEVICE_PATH_PROTOCOL *RemainingDevicePath
  )
{
  EFI_STATUS                          Status;
  EFI_LEGACY_BIOS_PROTOCOL            *LegacyBios;

  Status = gBS->LocateProtocol (
                  &gEfiLegacyBiosProtocolGuid,
                  NULL,
                  (VOID**)&LegacyBios
                  );
  if (EFI_ERROR(Status)) {
    //
    // Use the GOP driver.
    //
    if (RemainingDevicePath != NULL) {
      //
      // The case input RemainingDevicePath unsupported to GOP driver.
      // Set RemainingDevicePath == NULL and connected GOP driver again.
      //
      Status = gBS->ConnectController (VgaControllerHandle, NULL, NULL, FALSE);
      return Status;
     } else {
      //
      // If RemainingDevicePath is NULL, and native GOP driver still connected failed. 
      // We will Rollback CSM and try to connected the UefiBiosVideo by 
      // the function LegacyBiosDependency (NULL);
      //
      return EFI_UNSUPPORTED;
    }
  } else {
    //
    // UefiVideoBios connected failed. 
    //
    DEBUG ((EFI_D_ERROR, "Connected UefiVideoBios Failed"));
    return EFI_UNSUPPORTED;
  }
}

