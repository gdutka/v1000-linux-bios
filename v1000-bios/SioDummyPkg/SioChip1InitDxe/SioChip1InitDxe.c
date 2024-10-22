/** @file
 Implementation of SioChip1InitDxe.

 This driver would check chip's ID and install an SIO Instance Private Info
 Protocol on its Image Handle (also called Chip Handle).
 Besides, if there's any ISA device presents, it would install an ISA Device
 Info Protocol on Chip Handle.

;******************************************************************************
;* Copyright (c) 2017 - 2019 Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corp.
;*
;******************************************************************************
*/

#include <Uefi.h>
#include <SioFeaturePolicy.h>
#include <SioPrivateDefine.h>
#include <Protocol/SioInstancePrivateInfo.h>
#include <Library/SioDummyPrivateLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>


#if FixedPcdGetBool (PcdH2OSioDummyChip1Uart1Supported)
extern EFI_STATUS InitializeSioUart1     (SIO_CONFIGURED_RESOURCE* Resource);
#endif
#if FixedPcdGetBool (PcdH2OSioDummyChip1Uart2Supported)
extern EFI_STATUS InitializeSioUart2     (SIO_CONFIGURED_RESOURCE* Resource);
#endif
#if FixedPcdGetBool (PcdH2OSioDummyChip1KbcSupported)
extern EFI_STATUS InitializeSioPs2Kb     (SIO_CONFIGURED_RESOURCE* Resource);
extern EFI_STATUS InitializeSioPs2Ms     (SIO_CONFIGURED_RESOURCE* Resource);
#endif

SIO_LOGICAL_DEVICE_PRIVATE_INFO mDevPrivateInfo[] = {
#if FixedPcdGetBool (PcdH2OSioDummyChip1Uart1Supported)
  {SioUartDev,           SioDevInstance1,       InitializeSioUart1, V_UART_PORT1    },
#endif
#if FixedPcdGetBool (PcdH2OSioDummyChip1Uart2Supported)
  {SioUartDev,           SioDevInstance2,       InitializeSioUart2, V_UART_PORT2    },
#endif
#if FixedPcdGetBool (PcdH2OSioDummyChip1KbcSupported)
  {SioPs2KeyboardDev,    SioDevInstance1,       InitializeSioPs2Kb, V_KBC           },
  {SioPs2MouseDev,       SioDevInstance2,       InitializeSioPs2Ms, V_KBC           },
#endif
  {SioDeviceTypeMaximum, SioDevInstanceMaximum, NULL,               SioLdnMaximum   }
};

/**
  The Entry Point for SioChip1InitDxe.

  @param[in] ImageHandle    The firmware allocated handle for the EFI image.
  @param[in] SystemTable    A pointer to the EFI System Table.

  @retval EFI_SUCCESS       The entry point is executed successfully.
  @retval other             Some error occurs when executing this entry point.

**/
EFI_STATUS
SioDummyChip1Init (
  IN  EFI_HANDLE        ImageHandle,
  IN  EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS                          Status;
  SIO_INSTANCE_DATA                   *SioInstanceData;
  SIO_CHIP_INFORMATION                *ChipInformation;
  SIO_INSTANCE_PRIVATE_INFO_PROTOCOL  *PrivateInfoProtocol;
  UINTN                               IsaDevicesCount;

  PrivateInfoProtocol = NULL;
  IsaDevicesCount     = 0;

  Status = GetSioChipInformation (&ChipInformation);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  for (SioInstanceData = ChipInformation->SioInstanceList; SioInstanceData->ChipInstance != SioInstanceMaximum; SioInstanceData++) {
    if (SioInstanceData->ChipInstance == SioChip1 && SioInstanceData->ChipSupported) {
      PrivateInfoProtocol = AllocateZeroPool (sizeof(SIO_INSTANCE_PRIVATE_INFO_PROTOCOL));
      if (PrivateInfoProtocol == NULL) {
        FreeChipInfoData (&ChipInformation);
        return Status;
      }

      PrivateInfoProtocol->ChipVendor     = ChipInformation->ChipVendor;
      PrivateInfoProtocol->Uid            = SioUidMaximum;
      PrivateInfoProtocol->ChipIdMSB      = ChipInformation->ChipIdMSB;
      PrivateInfoProtocol->ChipIdLSB      = ChipInformation->ChipIdLSB;
      PrivateInfoProtocol->ChipInstance   = SioInstanceData->ChipInstance;
      PrivateInfoProtocol->ConfigPort     = SioInstanceData->ConfigPort;
      PrivateInfoProtocol->LogicalDevInfo = mDevPrivateInfo;

      Status = gBS->InstallProtocolInterface (
                      &ImageHandle,
                      &gH2OSioInstancePrivateInfoProtocolGuid,
                      EFI_NATIVE_INTERFACE,
                      PrivateInfoProtocol
                      );
      if (EFI_ERROR (Status)) {
        DEBUG ((EFI_D_ERROR, "SioDummyChip1Init() - Failed to install protocol interface: %r\n", Status));
        FreePool (PrivateInfoProtocol);
        FreeChipInfoData (&ChipInformation);
        return Status;
      }
      break;
    }
  }

  FreeChipInfoData (&ChipInformation);

  IsaDevicesCount =
    (FixedPcdGetBool (PcdH2OSioDummyChip1Uart1Supported) ? 1 : 0) +
    (FixedPcdGetBool (PcdH2OSioDummyChip1Uart2Supported) ? 1 : 0) +
    (FixedPcdGetBool (PcdH2OSioDummyChip1KbcSupported) ? 1 : 0) +
    0;

  return EFI_SUCCESS;
}
