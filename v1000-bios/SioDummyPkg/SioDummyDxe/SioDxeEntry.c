/** @file
 This module initialize Chip1 and Chip2's settings

;******************************************************************************
;* Copyright 2017 Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corp.
;*
;******************************************************************************
*/

#include <Uefi.h>
#include <Protocol/SioInstancePrivateInfo.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/SioDummyPrivateLib.h>

/**
  SIO XXX DXE driver entry point.

  Get "ChipInformation" structure
  Locate protocol gH2OSioInstancePrivateInfoProtocolGuid
  Get "SioXXXChip1Setup" or "SioXXXChip2Setup" variable
  Update PrivateInfoProtocol
  Program OEM customized demands

  @param[in]       ImageHandle              The image handle.
  @param[in]       SystemTable              The system table.

  @retval          EFI_SUCCESS              Function complete successfully.
  @retval          EFI_NO_MEDIA             Get Sio chip information failed.

**/
EFI_STATUS
EFIAPI
SioDummyDriverEntry (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  )
{
  UINTN                                   Index;
  UINTN                                   NumberOfHandles;
  EFI_HANDLE                              *HandleBuffer;
  EFI_STATUS                              Status;
  SIO_INSTANCE_PRIVATE_INFO_PROTOCOL      *PrivateInfoProtocol;
  SIO_LOGICAL_DEVICE_PRIVATE_INFO         *DevPrivateInfo;
  SIO_CHIP_INFORMATION                    *ChipInformation;
  SIO_CONFIGURED_RESOURCE                 DeviceList;

  DEBUG ((EFI_D_INFO, "--SIO DXE Module: %a Start--\n", __FUNCTION__));

  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gH2OSioInstancePrivateInfoProtocolGuid,
                  NULL,
                  &NumberOfHandles,
                  &HandleBuffer
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_INFO, "Warning! Don't exist any Super I/O.\n"));
    return EFI_SUCCESS;
  }

  Status = GetSioChipInformation (&ChipInformation);
  ASSERT_EFI_ERROR (Status);

  for (Index = 0; Index < NumberOfHandles; Index++) {
    //
    // Get correct gH2OSioInstancePrivateInfoProtocolGuid for the SIO
    //
    Status = gBS->HandleProtocol (
                    HandleBuffer[Index],
                    &gH2OSioInstancePrivateInfoProtocolGuid,
                    (VOID**)&PrivateInfoProtocol
                    );
    if (EFI_ERROR (Status) ||
        PrivateInfoProtocol->ChipIdLSB != ChipInformation->ChipIdLSB ||
        PrivateInfoProtocol->ChipIdMSB != ChipInformation->ChipIdMSB ||
        ChipInformation->SioInstanceList[PrivateInfoProtocol->ChipInstance - 1].ChipInstance != PrivateInfoProtocol->ChipInstance ||
        ChipInformation->SioInstanceList[PrivateInfoProtocol->ChipInstance - 1].ConfigPort != PrivateInfoProtocol->ConfigPort
        )
    {
      continue;
    }

    //
    // Initialize each device in this SIO
    //
    for (DevPrivateInfo = PrivateInfoProtocol->LogicalDevInfo; DevPrivateInfo->LogicalDevType != SioDeviceTypeMaximum; DevPrivateInfo++) {
      DeviceList.ChipVendor   = PrivateInfoProtocol->ChipVendor;
      DeviceList.ChipInstance = PrivateInfoProtocol->ChipInstance;
      DeviceList.ConfigPort   = PrivateInfoProtocol->ConfigPort;
      DeviceList.DevInstance  = DevPrivateInfo->DevInstance;
      DeviceList.Handle       = HandleBuffer[Index];
      Status = DevPrivateInfo->DevInitialize (&DeviceList);
      DEBUG ((EFI_D_INFO, "%a:%d Status:%r\n", __FUNCTION__, __LINE__, Status));
    }
  }
  FreePool (HandleBuffer);
  FreeChipInfoData (&ChipInformation);
  DEBUG ((EFI_D_INFO, "--SIO DXE Module: %a End--\n", __FUNCTION__));
  return EFI_SUCCESS;
}
