/* $NoKeywords:$ */
/**
 * @file
 *
 * SnpDxe.c
 *
 * Contains Ethernet Driver for the DXE phase.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: FDK
 * @e sub-project: UEFI
 * @e version: $Revision: 300489 $ @e date: $Date: 2014-08-08 14:36:46 -0700 (Fri, 08 Aug 2014) $
 *
 */
/*
 ****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */
#include <Library/UefiLib.h>
#include <Protocol/PciIo.h>
#include <Library/PciLib.h>
#include <IndustryStandard/Pci22.h>
#include "SnpDxe.h"
#include <Dxe/AmdPbsSetupDxe/AmdPbsConfig.h>
#include <Library/AmdPbsConfigLib/AmdPbsConfigLib.h>


XGBE_PLATFORM_DATA  mXgbeData = {0};

EFI_STATUS
STATIC
EFIAPI
AmdXgbeDriverBindingSupported (
  IN EFI_DRIVER_BINDING_PROTOCOL *This,
  IN EFI_HANDLE                  DeviceHandle,
  IN EFI_DEVICE_PATH_PROTOCOL    *RemainingDevicePath
  );

EFI_STATUS
STATIC
EFIAPI
AmdXgbeDriverBindingStart (
  IN EFI_DRIVER_BINDING_PROTOCOL *This,
  IN EFI_HANDLE                  ControllerHandle,
  IN EFI_DEVICE_PATH_PROTOCOL    *RemainingDevicePath
  );

EFI_STATUS
STATIC
EFIAPI
AmdXgbeDriverBindingStop (
  IN EFI_DRIVER_BINDING_PROTOCOL *This,
  IN EFI_HANDLE                  DeviceHandle,
  IN UINTN                       NumberOfChildren,
  IN EFI_HANDLE                  *ChildHandleBuffer
  );

EFI_STATUS
STATIC
EFIAPI
AmdXgbeGetDriverName (
  IN  EFI_COMPONENT_NAME_PROTOCOL *This,
  IN  CHAR8                       *Language,
  OUT CHAR16                      **DriverName
  );

EFI_STATUS
STATIC
EFIAPI
AmdXgbeGetControllerName (
  IN  EFI_COMPONENT_NAME_PROTOCOL *This,
  IN  EFI_HANDLE                  ControllerHandle,
  IN  EFI_HANDLE                  ChildHandle,
  IN  CHAR8                       *Language,
  OUT CHAR16                      **ControllerName
  );

extern EFI_STATUS
EFIAPI
AmdXgbeInit (
  VOID
        );

extern EFI_STATUS
EFIAPI
EthStop (
  IN EFI_SIMPLE_NETWORK_PROTOCOL *This
  );

extern ENET_DXE_PRIVATE_DATA    *gPrivate;
extern EFI_HANDLE               gControllerHandle;
EFI_DEVICE_PATH_PROTOCOL        *PciDevicePath;

EFI_DRIVER_BINDING_PROTOCOL gAmdXgbeDriverBinding = {
  AmdXgbeDriverBindingSupported,
  AmdXgbeDriverBindingStart,
  AmdXgbeDriverBindingStop,
  0x10,
  NULL,
  NULL
};

STATIC
EFI_UNICODE_STRING_TABLE mAmdXgbeDriverNameTable[] = {
  { "eng; en", L"AmdXgbe Network Driver" },
  { "eng; en", L"AmdXgbe Network Driver1" },
  { "eng; en", L"AmdXgbe Network Driver2" },
  { "eng; en", L"AmdXgbe Network Driver3" },
  { NULL,     NULL                     }
};

STATIC
EFI_UNICODE_STRING_TABLE mAmdXgbeControllerNameTable[] = {
  { "eng; en", L"AmdXgbe Network Device" },
  { "eng; en", L"AmdXgbe Network Device1" },
  { "eng; en", L"AmdXgbe Network Device2" },
  { "eng; en", L"AmdXgbe Network Device3" },
  { NULL,     NULL                     }
};

EFI_COMPONENT_NAME_PROTOCOL gAmdXgbeComponentName = {
  &AmdXgbeGetDriverName,
  &AmdXgbeGetControllerName,
  "eng" // SupportedLanguages, ISO 639-2 language codes
};

EFI_COMPONENT_NAME2_PROTOCOL gAmdXgbeComponentName2 = {
  (EFI_COMPONENT_NAME2_GET_DRIVER_NAME)     &AmdXgbeGetDriverName,
  (EFI_COMPONENT_NAME2_GET_CONTROLLER_NAME) &AmdXgbeGetControllerName,
  "en" // SupportedLanguages, RFC 4646 language codes
};

/**
  This is the declaration of an EFI image entry point. This entry point is the
  same for UEFI Applications, UEFI OS Loaders, and UEFI Drivers including both
  device drivers and bus drivers.

  @param  ImageHandle           The firmware allocated handle for the UEFI
                                image.
  @param  SystemTable           A pointer to the EFI System Table.

  @retval EFI_SUCCESS           The operation completed successfully.
  @retval Others                An unexpected error occurred.
**/

EFI_STATUS
EFIAPI
AmdXgbeEntryPoint (
  IN EFI_HANDLE       ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  AMD_PBS_SETUP_OPTION              AmdPbsConfiguration;
  EFI_STATUS                        Status;  

  Status = GetAmdPbsConfiguration (&AmdPbsConfiguration);
  if (EFI_ERROR (Status)) {
    DEBUG((DEBUG_INFO, "AmdXgbeEntryPoint=%r\n", Status));
    return Status;
  }
  
  // If PXE Disabled -> return unsupported.
  if (AmdPbsConfiguration.PxeBoot == 0x00) 
		return EFI_UNSUPPORTED;
  
  return EfiLibInstallDriverBindingComponentName2 (
           ImageHandle,
           SystemTable,
           &gAmdXgbeDriverBinding,
           ImageHandle,
           &gAmdXgbeComponentName,
           &gAmdXgbeComponentName2
           );
}


EFI_STATUS
STATIC
EFIAPI
AmdXgbeDriverBindingSupported (
  IN EFI_DRIVER_BINDING_PROTOCOL *This,
  IN EFI_HANDLE                  DeviceHandle,
  IN EFI_DEVICE_PATH_PROTOCOL    *RemainingDevicePath
  )
{
  EFI_STATUS          Status;
  EFI_PCI_IO_PROTOCOL *PciIo;
  AMD_FABRIC_TOPOLOGY_SERVICES2_PROTOCOL *FabricTopology;
  PCI_ADDR            PciAddress;
  UINT32              VidDid;
  UINTN               SegmentNumber;
  UINTN               BusNumber;
  UINTN               DeviceNumber;
  UINTN               FunctionNumber;
  UINTN               BusNumberBase;
  UINTN               SocketNum;
  UINTN               DieNum;
  UINT8               XgbeBusDie0;
  AMD_PBS_SETUP_OPTION              AmdPbsConfiguration;

  SocketNum = 0;
  DieNum = 0;

  Status = GetAmdPbsConfiguration (&AmdPbsConfiguration);
  if (EFI_ERROR (Status)) {
    DEBUG((DEBUG_INFO, "AmdXgbeEntryPoint=%r\n", Status));
    return Status;
  }
  
  Status = gBS->LocateProtocol (
                &gAmdFabricTopologyServices2ProtocolGuid,
                NULL,
                (VOID**)&FabricTopology
                );
  if (FabricTopology->GetRootBridgeInfo (FabricTopology, SocketNum, DieNum, 0, NULL, &BusNumberBase, NULL, NULL, NULL, NULL) != EFI_SUCCESS) {
    Status = EFI_DEVICE_ERROR;
  }
  PciAddress.AddressValue = (MAKE_SBDFO (0, BusNumberBase, 0x8, FUNC_2, 0x19));
  XgbeBusDie0 = PciRead8 (PciAddress.AddressValue);

  Status = gBS->OpenProtocol (
    DeviceHandle,
    &gEfiPciIoProtocolGuid,
    (VOID **) &PciIo,
    This->DriverBindingHandle,
    DeviceHandle,
    EFI_OPEN_PROTOCOL_BY_DRIVER
    );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = PciIo->Pci.Read (
    PciIo,
    EfiPciIoWidthUint32,
    PCI_VENDOR_ID_OFFSET,
    1,
    &VidDid
    );
  if (!EFI_ERROR (Status)) {
    if (VidDid == 0x14581022) {
      PciIo->GetLocation (
                        PciIo,
                        &SegmentNumber,
                        &BusNumber,
                        &DeviceNumber,
                        &FunctionNumber
      );
      if ((FunctionNumber == 1) && (BusNumber == XgbeBusDie0)) {
	      if (AmdPbsConfiguration.PxeBoot == 0x01){
        		mXgbeData.XgbeBusFunc = 0x0001;
        			Status = EFI_SUCCESS;
        	DEBUG ((DEBUG_ERROR, "=AmdXgbe SUPPORTED Port 0 \n"));
	      }else{
			Status = EFI_UNSUPPORTED;	
			DEBUG ((DEBUG_ERROR, "=AmdXgbe NOT SUPPORTED Port 0 \n"));
		  }
      } else if ((FunctionNumber == 2) && (BusNumber == XgbeBusDie0)) {
	      if (AmdPbsConfiguration.PxeBoot == 0x02){
        	mXgbeData.XgbeBusFunc = 0x0002;
        	Status = EFI_SUCCESS;
			DEBUG ((DEBUG_ERROR, "=AmdXgbe SUPPORTED Port 1 \n"));
		}else{
			Status = EFI_UNSUPPORTED;
			DEBUG ((DEBUG_ERROR, "=AmdXgbe NOT SUPPORTED Port 1 \n"));
		}        
      } else {
        Status = EFI_UNSUPPORTED;
      }
    } else if (VidDid == 0x14591022) {
      PciIo->GetLocation (
                        PciIo,
                        &SegmentNumber,
                        &BusNumber,
                        &DeviceNumber,
                        &FunctionNumber
      );
      if ((FunctionNumber == 1) && (BusNumber == XgbeBusDie0)) {
	  	if (AmdPbsConfiguration.PxeBoot == 0x01){
        	mXgbeData.XgbeBusFunc = 0x0001;
        	Status = EFI_SUCCESS;
        	DEBUG ((DEBUG_ERROR, "=AmdXgbe SUPPORTED Func 1\n"));
	      }else{
			Status = EFI_UNSUPPORTED;		
			DEBUG ((DEBUG_ERROR, "=AmdXgbe NOT SUPPORTED Port 0 \n"));
		  }		
      } else if ((FunctionNumber == 2) && (BusNumber == XgbeBusDie0)) {
	    if (AmdPbsConfiguration.PxeBoot == 0x02){
        	mXgbeData.XgbeBusFunc = 0x0002;
        	Status = EFI_SUCCESS;
        	DEBUG ((DEBUG_ERROR, "=AmdXgbe SUPPORTED Func 2\n"));
		}else{
	        Status = EFI_UNSUPPORTED;
			DEBUG ((DEBUG_ERROR, "=AmdXgbe NOT SUPPORTED Port 1 \n"));
		}
      } else {
        Status = EFI_UNSUPPORTED;
      }
    } else {
      Status = EFI_UNSUPPORTED;
    }
  }

  gBS->CloseProtocol (
    DeviceHandle,
    &gEfiPciIoProtocolGuid,
    This->DriverBindingHandle,
    DeviceHandle);

  return Status;
}

EFI_STATUS
STATIC
EFIAPI
AmdXgbeDriverBindingStart (
  IN EFI_DRIVER_BINDING_PROTOCOL *This,
  IN EFI_HANDLE                  ControllerHandle,
  IN EFI_DEVICE_PATH_PROTOCOL    *RemainingDevicePath
  )
{
  EFI_STATUS          Status;
  VOID                *ChildPciIo;
  gControllerHandle = ControllerHandle;

  DEBUG ((DEBUG_ERROR, "AmdXgbeDriverBindingStart \n"));

  // get PCI Device Path
  Status = gBS->OpenProtocol (
          ControllerHandle,
          &gEfiDevicePathProtocolGuid,
          (VOID **) &PciDevicePath,
          This->DriverBindingHandle,
          ControllerHandle,
          EFI_OPEN_PROTOCOL_BY_DRIVER
          );

  if (Status == EFI_ALREADY_STARTED) {
	    return EFI_ALREADY_STARTED;
  }
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "AmdXgbeStart: cannot open Device Path: Status: %x \n", Status));
  }

  Status = AmdXgbeInit ();

  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "AmdXgbeStart: AmdXgbeInit failed: Status: %x \n", Status));
    goto ErrorExit1;
  }

  Status = gBS->OpenProtocol (ControllerHandle, &gEfiPciIoProtocolGuid,
                    &ChildPciIo, This->DriverBindingHandle,
                    gPrivate->DeviceHandle, EFI_OPEN_PROTOCOL_BY_CHILD_CONTROLLER);

  if (!EFI_ERROR (Status)) {
    return Status;
  }

  ErrorExit1:
    gBS->CloseProtocol (
      ControllerHandle,
      &gEfiDevicePathProtocolGuid,
      This->DriverBindingHandle,
      ControllerHandle);
  return Status;
}

EFI_STATUS
STATIC
EFIAPI
AmdXgbeDriverBindingStop (
  IN EFI_DRIVER_BINDING_PROTOCOL *This,
  IN EFI_HANDLE                  DeviceHandle,
  IN UINTN                       NumberOfChildren,
  IN EFI_HANDLE                  *ChildHandleBuffer
  )
{
  EFI_STATUS                        Status;
  EFI_PCI_IO_PROTOCOL               *PciIo;
  EFI_SIMPLE_NETWORK_PROTOCOL       *SimpleNetwork;
  BOOLEAN                           AllChildrenStopped;
  UINTN                             Index;

  DEBUG ((DEBUG_ERROR, "AmdXgbeDriverBindingStop \n"));

  Status = gBS->OpenProtocol (
          DeviceHandle,
          &gEfiPciIoProtocolGuid,
          (VOID **) &PciIo,
          This->DriverBindingHandle,
          DeviceHandle,
          EFI_OPEN_PROTOCOL_GET_PROTOCOL
          );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "AmdXgbeDriverBindingStop OpenProtocol error: %r\n", Status));
  }

  if (gPrivate != NULL) {
    Status = EthStop (&gPrivate->Snp);
  }
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "AmdXgbeDriverBindingStop EthStop error: %r\n", Status));
  }

  AllChildrenStopped = TRUE;

  for (Index = 0; Index < NumberOfChildren; Index++) {
    Status = gBS->OpenProtocol (
                      ChildHandleBuffer[Index],
                      &gEfiSimpleNetworkProtocolGuid,
                      (VOID **) &SimpleNetwork,
                      This->DriverBindingHandle,
                      DeviceHandle,
                      EFI_OPEN_PROTOCOL_GET_PROTOCOL
                      );
    DEBUG ((DEBUG_ERROR, "AmdXgbeDriverBindingStop Open SNP Protocol Status: %r\n", Status));

    if (!EFI_ERROR (Status)) {
      Status = gBS->UninstallMultipleProtocolInterfaces (
                        ChildHandleBuffer[Index],
                        &gEfiDevicePathProtocolGuid,
                        gPrivate->FullDevicePath,
                        &gEfiSimpleNetworkProtocolGuid,
                        &gPrivate->Snp,
                        NULL
                        );
      DEBUG ((DEBUG_ERROR, "AmdXgbeDriverBindingStop UninstallMultipleProtocolInterfaces Status: %r\n", Status));
    }

    if (EFI_ERROR (Status)) {
      AllChildrenStopped = FALSE;
    }
  }

  if (!AllChildrenStopped) {
    return EFI_DEVICE_ERROR;
  }

    Status = gBS->CloseProtocol (
        DeviceHandle,
        &gEfiPciIoProtocolGuid,
        This->DriverBindingHandle,
        DeviceHandle
        );

  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "AmdXgbeDriverBindingStop ClosePciIoProtocol for DeviceHandle error: %r\n", Status));
    return Status;
  }

  if (gPrivate != NULL) {
    FreePool (gPrivate);
    gPrivate = NULL;
  }

  return Status;
}

EFI_STATUS
STATIC
EFIAPI
AmdXgbeGetDriverName (
  IN       EFI_COMPONENT_NAME_PROTOCOL *This,
  IN       CHAR8                       *Language,
  OUT       CHAR16                      **DriverName
  )
{
  return (Language == NULL || DriverName == NULL) ?
         EFI_INVALID_PARAMETER :
         LookupUnicodeString2 (
           Language,
           This->SupportedLanguages,
           mAmdXgbeDriverNameTable,
           DriverName,
           (BOOLEAN) (This == &gAmdXgbeComponentName) // Iso639Language
           );
}

EFI_STATUS
STATIC
EFIAPI
AmdXgbeGetControllerName (
  IN       EFI_COMPONENT_NAME_PROTOCOL *This,
  IN       EFI_HANDLE                  ControllerHandle,
  IN       EFI_HANDLE                  ChildHandle,
  IN       CHAR8                       *Language,
  OUT       CHAR16                     **ControllerName
  )
{

  EFI_STATUS                                  Status;
  EFI_PCI_IO_PROTOCOL                         *PciIo;

  if (ControllerHandle == NULL || Language == NULL || ControllerName == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Check Controller's handle
  //
  Status = gBS->OpenProtocol (
                  ControllerHandle,
                  &gEfiPciIoProtocolGuid,
                  (VOID **) &PciIo,
                  gAmdXgbeDriverBinding.DriverBindingHandle,
                  ControllerHandle,
                  EFI_OPEN_PROTOCOL_BY_DRIVER
                  );

  if (!EFI_ERROR (Status)) {
    gBS->CloseProtocol (
           ControllerHandle,
           &gEfiPciIoProtocolGuid,
           gAmdXgbeDriverBinding.DriverBindingHandle,
           ControllerHandle
           );

    return EFI_UNSUPPORTED;
  }

 if (Status != EFI_ALREADY_STARTED) {
    return EFI_UNSUPPORTED;
 }

  return LookupUnicodeString2 (
           Language,
           This->SupportedLanguages,
           mAmdXgbeControllerNameTable,
           ControllerName,
           (BOOLEAN) (This == &gAmdXgbeComponentName) // Iso639Language
           );
}
