/*****************************************************************************
 *
 * Copyright (C) 2012-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 ******************************************************************************
 */

#include <AmdCpmDxe.h>

VOID
EFIAPI
AmdCpmDisplayFeatureInitMid (
  IN        EFI_EVENT       Event,
  IN        VOID            *Context
  );

VOID
EFIAPI
AmdCpmDisplayFeatureInitLateEntry (
  IN        EFI_EVENT       Event,
  IN        VOID            *Context
  );

VOID
EFIAPI
AmdCpmDisplayFeatureVbiosShadow (
  IN        EFI_EVENT       Event,
  IN        VOID            *Context
  );

/*----------------------------------------------------------------------------------------*/
/**
 * Entry point of the AMD CPM Display Feature DXE driver
 *
 * This function installs AMD CPM DISPLAY FEATURE PROTOCOL and initialize display features.
 *
 * @param[in]     ImageHandle    Pointer to the firmware file system header
 * @param[in]     SystemTable    Pointer to System table
 *
 * @retval        EFI_SUCCESS    Module initialized successfully
 * @retval        EFI_ERROR      Initialization failed (see error for more details)
 */

EFI_STATUS
EFIAPI
AmdCpmDisplayFeatureDxeEntryPoint (
  IN        EFI_HANDLE                      ImageHandle,
  IN        EFI_SYSTEM_TABLE                *SystemTable
  )
{
  EFI_STATUS                                Status;
  EFI_EVENT                                 ReadyToBootEvent;
  EFI_EVENT                                 PciIoEvent;
  EFI_HANDLE                                Handle;
  AMD_CPM_DISPLAY_FEATURE_PROTOCOL          *CpmDisplayFeatureProtocolPtr;
  AMD_CPM_TABLE_PROTOCOL                    *CpmTableProtocolPtr;
  EFI_PCI_IO_PROTOCOL                       *PciIo;
  AMD_CPM_ALL_PCI_IO_PROTOCOLS_INSTALLED_PROTOCOL *Tmp;
  VOID                                      *RegistrationForPciIoPtr;

  //
  // Initialize Global Variable
  //
  CpmInitializeDriverLib (ImageHandle, SystemTable);

  RECORD_TIME_DXE (BeginAmdCpmDisplayFeatureDxeDriver);

  Status = gBS->LocateProtocol (  &gAmdCpmTableProtocolGuid,
                                  NULL,
                                  (VOID**)&CpmTableProtocolPtr
                                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  CpmTableProtocolPtr->CommonFunction.PostCode (CpmTpDisplayFeatureDxeDriverBegin);

  Status = gBS->AllocatePool (
                    EfiACPIMemoryNVS,
                    sizeof (AMD_CPM_DISPLAY_FEATURE_PROTOCOL),
                    (VOID**)&CpmDisplayFeatureProtocolPtr
                    );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  CpmDisplayFeatureProtocolPtr = ZeroMem (CpmDisplayFeatureProtocolPtr, sizeof (AMD_CPM_DISPLAY_FEATURE_PROTOCOL));

  CpmDisplayFeatureProtocolPtr->Revision                                      = AMD_CPM_DISPLAY_FEATURE_PROTOCOL_REV;
  CpmDisplayFeatureProtocolPtr->TableProtocolPtr                              = CpmTableProtocolPtr;
  CpmDisplayFeatureProtocolPtr->DisplayFeatureData.DisplayFeatureTablePtr     = (VOID *)CpmTableProtocolPtr->CommonFunction.GetTablePtr (CpmTableProtocolPtr, CPM_SIGNATURE_DISPLAY_FEATURE);
  CpmDisplayFeatureProtocolPtr->DisplayFeatureData.DevicePathTablePtr         = (VOID *)CpmTableProtocolPtr->CommonFunction.GetTablePtr (CpmTableProtocolPtr, CPM_SIGNATURE_DEVICE_PATH);
  CpmDisplayFeatureProtocolPtr->DisplayFeatureData.SpecificSsidTablePtr       = (VOID *)CpmTableProtocolPtr->CommonFunction.GetTablePtr (CpmTableProtocolPtr, CPM_SIGNATURE_SPECIFIC_SSID);
  CpmDisplayFeatureProtocolPtr->DisplayFeatureData.RebrandDualGraphicsSsidTablePtr  = (VOID *)CpmTableProtocolPtr->CommonFunction.GetTablePtr (CpmTableProtocolPtr, CPM_SIGNATURE_REBRAND_DUAL_GRAPHICS_SSID);

  Handle = ImageHandle;
  Status = gBS->InstallProtocolInterface (
                    &Handle,
                    &gAmdCpmDisplayFeatureProtocolGuid,
                    EFI_NATIVE_INTERFACE,
                    CpmDisplayFeatureProtocolPtr
                    );
  if (EFI_ERROR (Status)) {
    return (Status);
  }

  Status = gBS->LocateProtocol (
                    &gEfiPciIoProtocolGuid,
                    NULL,
                    (VOID**)&PciIo
                    );

  if (EFI_ERROR (Status)) {
    Status = gBS->LocateProtocol (
                    &gAmdCpmAllPciIoProtocolsInstalledProtocolGuid,
                    NULL,
                    (VOID**)&Tmp
                    );
    if (EFI_ERROR (Status)) {
      AmdCpmDisplayFeatureInitMid (NULL, CpmDisplayFeatureProtocolPtr);
    } else {
      AmdCpmDisplayFeatureVbiosShadow (NULL, CpmDisplayFeatureProtocolPtr);
    }
  } else {
    Status = gBS->CreateEvent (
                    CPM_EVENT_NOTIFY_SIGNAL,
                    CPM_TPL_NOTIFY,
                    AmdCpmDisplayFeatureInitMid,
                    CpmDisplayFeatureProtocolPtr,
                    (VOID**)&PciIoEvent
                    );

    Status = gBS->RegisterProtocolNotify (
                    &gEfiPciIoProtocolGuid,
                    PciIoEvent,
                    (VOID**)&RegistrationForPciIoPtr
                    );
  }
  Status = gBS->CreateEventEx (
                    CPM_EVENT_NOTIFY_SIGNAL,
                    CPM_TPL_NOTIFY,
                    AmdCpmDisplayFeatureInitLateEntry,
                    CpmDisplayFeatureProtocolPtr,
                    &gEfiEventReadyToBootGuid,
                    (VOID**)&ReadyToBootEvent
                    );

  CpmTableProtocolPtr->CommonFunction.PostCode (CpmTpDisplayFeatureDxeDriverEnd);

  RECORD_TIME_DXE (EndAmdCpmDisplayFeatureDxeDriver);

  return EFI_SUCCESS;
}
