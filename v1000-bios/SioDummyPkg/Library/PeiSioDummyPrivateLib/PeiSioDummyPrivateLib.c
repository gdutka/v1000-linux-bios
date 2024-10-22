/** @file
 This file contains the implementation of SioXXXPrivateLib

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
#include <SioPrivateDefine.h>
#include <SioFeaturePolicy.h>
#include <Library/PcdLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Protocol/SioIsaPnpDevice.h>

SIO_CHIP_INFORMATION mSioChipInformation = {
  SIO_CHIP_VENDOR,
  0x00,
  0x00,
  NULL
};

SIO_INSTANCE_DATA mSioInstanceList[] = {
  {SioChip1,           0, FALSE, NULL, 0, NULL, 0},
  {SioInstanceMaximum, 0, FALSE, NULL, 0, NULL, 0}
};

/**
  Free the space used by "ChipInformation" structure

  Free pool that is used by "ChipInformation" structure

  @param[in]       ChipInformation          The pointer to address of structure "SIO_CHIP_INFORMATION"

**/
VOID
FreeChipInfoData (
  IN OUT SIO_CHIP_INFORMATION    **ChipInformation
  )
{
  if (*ChipInformation == NULL) {
    return;
  }

  if ((*ChipInformation)->SioInstanceList != NULL) {
    FreePool ((*ChipInformation)->SioInstanceList);
  }

  if (*ChipInformation != NULL) {
    FreePool (*ChipInformation);
  }

  *ChipInformation = NULL;
}

/**
  Create and update SCU settings for "ChipInformation" structure

  Allocate pool for "ChipInformation" structure.
  Base on "SioXXXChip1Setup" or "SioXXXChip2Setup" variable and
  update data with PcdH2OSioXXXLogicalDevConfig and SCU settings.

  @param[out]      ChipInformation          The pointer to address of structure "SIO_CHIP_INFORMATION"

  @retval          EFI_SUCCESS              Function complete successfully.
  @retval          EFI_INVALID_PARAMETER    ChipInformation is NULL or where it is pointing is not NULL.
  @retval          EFI_OUT_OF_RESOURCES     Allocate pool failed.

**/
EFI_STATUS
GetSioChipInformation (
  OUT SIO_CHIP_INFORMATION    **ChipInformation
  )
{
  SIO_INSTANCE_DATA             *ChipInstanceList;

  if (ChipInformation == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  *ChipInformation = (SIO_CHIP_INFORMATION*)AllocateCopyPool (sizeof(mSioChipInformation), &mSioChipInformation);
  if (*ChipInformation == NULL) {
    *ChipInformation = NULL;
    return EFI_OUT_OF_RESOURCES;
  }

  ChipInstanceList = (SIO_INSTANCE_DATA*)AllocateCopyPool (sizeof(mSioInstanceList), mSioInstanceList);
  if (ChipInstanceList == NULL) {
    FreeChipInfoData (ChipInformation);
    return EFI_OUT_OF_RESOURCES;
  }

  (*ChipInformation)->SioInstanceList = ChipInstanceList;

  for (; ChipInstanceList->ChipInstance != SioInstanceMaximum; ChipInstanceList++) {
    switch (ChipInstanceList->ChipInstance) {

    case SioChip1:
      ChipInstanceList->ChipSupported = FeaturePcdGet (PcdH2OSioDummyChip1Supported);
      break;

    default:
      break;
    }
  }

  return EFI_SUCCESS;
}

/**
  Program ISA device UID if ODM/OEM intend to customize.

  Customize ISA device UID via modifying PcdH2OSioXXXChip1IsaDeviceUid or PcdH2OSioXXXChip2IsaDeviceUid

  @param[in]       PcdIsaDeviceUid          PcdH2OXXXChip1IsaDeviceUid or PcdH2OXXXChip2IsaDeviceUid
  @param[in]       PcdIsaDeviceUidCount     How many bytes of PcdH2OXXXChip1IsaDeviceUid or PcdH2OSioXXXChip2IsaDeviceUid
  @param[in]       LogicalDeviceType        Logical device type, e.g., floppy, parallel port, UART
  @param[in]       DeviceInstance           Instance of the same logical device, e.g., UART1, UART2, UART3

  @retval          SIO_ISA_DEVICE_DEFAULT_UID   ISA device is not customized at PCD, ISA Bus will automatically allocate UID
                                                for ISA Device.
  @retval          DeviceUid                    Customized ISA device UID.

**/
UINT16
GetIsaDeviceUid (
  IN SIO_PCD_ISA_DEVICE_UID  *PcdIsaDeviceUid,
  IN UINTN                   PcdIsaDeviceUidCount,
  IN SIO_LOGICAL_DEVICE_TYPE LogicalDeviceType,
  IN UINT8                   DeviceInstance
  )
{
  UINTN                  Index;
  SIO_PCD_ISA_DEVICE_UID *TempPcdIsaDeviceUid;

  //
  // Find DeviceUid through DeviceType and DeviceInstance
  //
  for (
    Index = 0, TempPcdIsaDeviceUid = &PcdIsaDeviceUid[Index];
    Index < PcdIsaDeviceUidCount / sizeof(SIO_PCD_ISA_DEVICE_UID) &&
    !(PcdIsaDeviceUid[Index].LogicalDeviceType == (UINT8) LogicalDeviceType &&
      PcdIsaDeviceUid[Index].LogicalDeviceType < SioIsaDeviceTypeEnd &&
      PcdIsaDeviceUid[Index].DeviceInstance == DeviceInstance);
    Index++
    )
  {
    TempPcdIsaDeviceUid = &PcdIsaDeviceUid[Index];
  }
  if (Index >= (PcdIsaDeviceUidCount / sizeof(SIO_PCD_ISA_DEVICE_UID)) || PcdIsaDeviceUidCount <= 1) {
    return SIO_ISA_DEVICE_DEFAULT_UID;
  }
  return PcdIsaDeviceUid[Index].DeviceUid;
}
