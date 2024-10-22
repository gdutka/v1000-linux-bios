/** @file

;******************************************************************************
;* Copyright (c) 2015 - 2017, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "InternalUtilityLib.h"

H2O_BDS_LOAD_OPTION_VENDOR_DEVICE_PATH      gH2OBootManagerDevicePath = {
  {
    {
      HARDWARE_DEVICE_PATH,
      HW_VENDOR_DP,
      {
        (UINT8) (sizeof (VENDOR_DEVICE_PATH)),
        (UINT8) ((sizeof (VENDOR_DEVICE_PATH)) >> 8)
      }
    },
    H2O_BDS_LOAD_OPTION_BOOT_MANAGER_GUID
  },
  {
    END_DEVICE_PATH_TYPE,
    END_ENTIRE_DEVICE_PATH_SUBTYPE,
    {
      (UINT8) (END_DEVICE_PATH_LENGTH),
      (UINT8) ((END_DEVICE_PATH_LENGTH) >> 8)
    }
  }
};

H2O_BDS_LOAD_OPTION_VENDOR_DEVICE_PATH      gH2OBootMaintenanceDevicePath = {
  {
    {
      HARDWARE_DEVICE_PATH,
      HW_VENDOR_DP,
      {
        (UINT8) (sizeof (VENDOR_DEVICE_PATH)),
        (UINT8) ((sizeof (VENDOR_DEVICE_PATH)) >> 8)
      }
    },
    H2O_BDS_LOAD_OPTION_BOOT_MAINTENANCE_GUID
  },
  {
    END_DEVICE_PATH_TYPE,
    END_ENTIRE_DEVICE_PATH_SUBTYPE,
    {
      (UINT8) (END_DEVICE_PATH_LENGTH),
      (UINT8) ((END_DEVICE_PATH_LENGTH) >> 8)
    }
  }
};

H2O_BDS_LOAD_OPTION_VENDOR_DEVICE_PATH      gH2ODeviceManagerDevicePath = {
  {
    {
      HARDWARE_DEVICE_PATH,
      HW_VENDOR_DP,
      {
        (UINT8) (sizeof (VENDOR_DEVICE_PATH)),
        (UINT8) ((sizeof (VENDOR_DEVICE_PATH)) >> 8)
      }
    },
    H2O_BDS_LOAD_OPTION_DEVICE_MANAGER_GUID
  },
  {
    END_DEVICE_PATH_TYPE,
    END_ENTIRE_DEVICE_PATH_SUBTYPE,
    {
      (UINT8) (END_DEVICE_PATH_LENGTH),
      (UINT8) ((END_DEVICE_PATH_LENGTH) >> 8)
    }
  }
};

H2O_BDS_LOAD_OPTION_VENDOR_DEVICE_PATH      gH2OSecureBootMgrDevicePath = {
  {
    {
      HARDWARE_DEVICE_PATH,
      HW_VENDOR_DP,
      {
        (UINT8) (sizeof (VENDOR_DEVICE_PATH)),
        (UINT8) ((sizeof (VENDOR_DEVICE_PATH)) >> 8)
      }
    },
    H2O_BDS_LOAD_OPTION_SECURE_BOOT_MANAGER_GUID
  },
  {
    END_DEVICE_PATH_TYPE,
    END_ENTIRE_DEVICE_PATH_SUBTYPE,
    {
      (UINT8) (END_DEVICE_PATH_LENGTH),
      (UINT8) ((END_DEVICE_PATH_LENGTH) >> 8)
    }
  }
};

H2O_BDS_LOAD_OPTION_VENDOR_DEVICE_PATH      gH2OFrontPageDevicePath = {
  {
    {
      HARDWARE_DEVICE_PATH,
      HW_VENDOR_DP,
      {
        (UINT8) (sizeof (VENDOR_DEVICE_PATH)),
        (UINT8) ((sizeof (VENDOR_DEVICE_PATH)) >> 8)
      }
    },
    H2O_BDS_LOAD_OPTION_FRONT_PAGE_GUID
  },
  {
    END_DEVICE_PATH_TYPE,
    END_ENTIRE_DEVICE_PATH_SUBTYPE,
    {
      (UINT8) (END_DEVICE_PATH_LENGTH),
      (UINT8) ((END_DEVICE_PATH_LENGTH) >> 8)
    }
  }
};

/**
  Clear specific bits of OsIndications variable.

  @param[in] ClearBits     Specific bits to be cleared.

  @retval EFI_SUCEESS      Clear bits successful.
  @retval Other            Fail to get ot set OsIndications variable.
**/
EFI_STATUS
EFIAPI
ClearOsIndicationsBits (
  IN UINT64                     ClearBits
  )
{
  EFI_STATUS                    Status;
  H2O_BDS_SERVICES_PROTOCOL     *BdsServices;
  UINT64                        OsIndications;
  UINT64                        OsIndicationsSupported;

  Status = gBS->LocateProtocol (&gH2OBdsServicesProtocolGuid, NULL, (VOID **) &BdsServices);
  if (Status != EFI_SUCCESS) {
    return Status;
  }

  Status = BdsServices->GetOsIndications (BdsServices, &OsIndications, &OsIndicationsSupported);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  OsIndications &= (~ClearBits);
  return gRT->SetVariable (
                EFI_OS_INDICATIONS_VARIABLE_NAME,
                &gEfiGlobalVariableGuid,
                EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                sizeof (OsIndications),
                &OsIndications
                );
}

/**
  Get the firmware UI bit of OsIndications variable to check if OS indicate to boot to firmware UI or not.

  @retval TRUE       OS indicate to boot to firmware UI.
  @retval FALSE      OS does not indicate to boot to firmware UI.
**/
BOOLEAN
EFIAPI
DoesOsIndicateBootToFwUI (
  VOID
  )
{
  EFI_STATUS                    Status;
  H2O_BDS_SERVICES_PROTOCOL     *BdsServices;
  UINT64                        OsIndications;
  UINT64                        OsIndicationsSupported;

  Status = gBS->LocateProtocol (&gH2OBdsServicesProtocolGuid, NULL, (VOID **) &BdsServices);
  if (Status != EFI_SUCCESS) {
    return FALSE;
  }

  Status = BdsServices->GetOsIndications (BdsServices, &OsIndications, &OsIndicationsSupported);
  if ((Status == EFI_SUCCESS) &&
      (OsIndications & OsIndicationsSupported & EFI_OS_INDICATIONS_BOOT_TO_FW_UI)) {
    return TRUE;
   }

  return FALSE;
}

/**
  Check if the device path instance of specific handle is hardware vendor device path or not.

  @param[in] Handle     Specific handle.

  @retval TRUE          The device path instance of specific handle is hardware vendor device path.
  @retval FALSE         There is no device path instance on specific handle or it does not hardware vendor device path.
**/
BOOLEAN
EFIAPI
IsHwVendorDevicePath (
  IN EFI_HANDLE                 Handle
  )
{
  EFI_STATUS                    Status;
  EFI_DEVICE_PATH_PROTOCOL      *DevicePath;

  Status = gBS->HandleProtocol (Handle, &gEfiDevicePathProtocolGuid, (VOID **) &DevicePath);
  if (EFI_ERROR (Status)) {
    return FALSE;
  }

  if (DevicePath->Type == HARDWARE_DEVICE_PATH &&
      DevicePath->SubType == HW_VENDOR_DP) {
    return TRUE;
  }

  return FALSE;
}

/**
  Lunch load option with specific device path.

  @param[in] DevicePath           Pointer to device path.

  @retval EFI_SUCCESS             Lunch load option successful.
  @retval EFI_INVALID_PARAMETER   Input device path is invalid.
  @retval EFI_OUT_OF_RESOURCES    Fail to allocate pool.
**/
EFI_STATUS
EFIAPI
LaunchBootOptionByDevicePath (
  IN EFI_DEVICE_PATH_PROTOCOL   *DevicePath
  )
{
  EFI_STATUS                    Status;
  H2O_BDS_SERVICES_PROTOCOL     *BdsServices;
  H2O_BDS_LOAD_OPTION           *BootOption;
  UINTN                         ExitDataSize;
  CHAR16                        *ExitData;

  if (DevicePath == NULL || !IsDevicePathValid (DevicePath, 0)) {
    return EFI_INVALID_PARAMETER;
  }

  Status = gBS->LocateProtocol (&gH2OBdsServicesProtocolGuid, NULL, (VOID **) &BdsServices);
  if (Status != EFI_SUCCESS) {
    return Status;
  }

  BootOption = AllocateZeroPool (sizeof (H2O_BDS_LOAD_OPTION));
  if (BootOption == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  BootOption->Signature    = H2O_BDS_LOAD_OPTION_SIGNATURE;
  BootOption->Attributes   = LOAD_OPTION_ACTIVE | LOAD_OPTION_CATEGORY_APP;
  BootOption->Connected    = TRUE;
  BootOption->DriverOrBoot = TRUE;
  BootOption->DevicePath   = DuplicateDevicePath (DevicePath);
  InitializeListHead (&BootOption->ExpandedLoadOptions);
  InitializeListHead (&BootOption->Link);
  BdsServices->ExpandLoadOption (BdsServices, BootOption);

  BdsServices->LaunchLoadOption (BdsServices, BootOption, &ExitDataSize, &ExitData);
  BdsServices->FreeLoadOption (BdsServices, BootOption);

  return EFI_SUCCESS;
}

/**
  Launch the Boot Manager.

  Deprecated. Use the LaunchLoadOption() function with well-known device path.
**/
VOID
EFIAPI
CallBootManager (
  VOID
  )
{
  LaunchBootOptionByDevicePath ((EFI_DEVICE_PATH_PROTOCOL *) &gH2OBootManagerDevicePath);
}

/**
  Launch the Boot Maintenance.

  Deprecated. Use the LaunchLoadOption() function with well-known device path.
**/
EFI_STATUS
EFIAPI
BdsStartBootMaint (
  VOID
  )
{
  LaunchBootOptionByDevicePath ((EFI_DEVICE_PATH_PROTOCOL *) &gH2OBootMaintenanceDevicePath);

  return EFI_SUCCESS;
}

/**
  Launch the device manager.

  Deprecated. Use the LaunchLoadOption() function with well-known device path.
**/
UINTN
EFIAPI
CallDeviceManager (
  VOID
  )
{
  LaunchBootOptionByDevicePath ((EFI_DEVICE_PATH_PROTOCOL *) &gH2ODeviceManagerDevicePath);

  return 0;
}

/**
  Launch the secure boot manager.

  Deprecated. Use the LaunchLoadOption() function with well-known device path.
**/
VOID
EFIAPI
CallSecureBootMgr (
  VOID
  )
{
  LaunchBootOptionByDevicePath ((EFI_DEVICE_PATH_PROTOCOL *) &gH2OSecureBootMgrDevicePath);
}

