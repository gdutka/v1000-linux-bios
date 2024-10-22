/** @file
 This file contains the interface of SioXXXprivateLib.

 This is meant to only be used by this SIO package.

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

#ifndef _SIO_DUMMY_PRIVATE_LIB_H_
#define _SIO_DUMMY_PRIVATE_LIB_H_

#include <SioFeaturePolicy.h>

/**
  Free the space used by "ChipInformation" structure

  Free pool that is used by "ChipInformation" structure

  @param[in]       ChipInformation          The pointer to address of structure "SIO_CHIP_INFORMATION"

**/
VOID
FreeChipInfoData (
  IN OUT SIO_CHIP_INFORMATION    **ChipInformation
  );

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
  );

/**
  Program ISA device UID if ODM/OEM intend to customize.

  Customize ISA device UID via modifying PcdH2OSioXXXChip1IsaDeviceUid or PcdH2OSioXXXChip2IsaDeviceUid

  @param[in]       PcdIsaDeviceUid          PcdH2OSioXXXChip1IsaDeviceUid or PcdH2OSioXXXChip2IsaDeviceUid
  @param[in]       PcdIsaDeviceUidCount     How many bytes of PcdH2OSioXXXChip1IsaDeviceUid or PcdH2OSioXXXChip2IsaDeviceUid
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
  );

#endif
