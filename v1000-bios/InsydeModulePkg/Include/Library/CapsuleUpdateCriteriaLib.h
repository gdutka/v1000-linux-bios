/** @file
  Definition for Capsule Update Criteria Library Class

;******************************************************************************
;* Copyright (c) 2012 - 2015, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _CAPSULE_UPDATE_CRITERIA_LIB_H_
#define _CAPSULE_UPDATE_CRITERIA_LIB_H_

#include <Uefi.h>
#include <Guid/EfiSystemResourceTable.h>
#include <Guid/H2OCp.h>

#define  EFI_CAPSULE_FILE_PATH  L"EFI\\UpdateCapsule"
#define  EFI_CAPSULE_FILE_NAME  L"CapsuleUpdateFile"

/**
  Get system firmware revision for ESRT from capsule image

  @param  CapsuleHeader         Points to a capsule header.

  @return                       The system firmware revision from the capsule image
                                If the signature cannot be found, 0x00000000 will
                                be returned
**/
UINT32
EFIAPI
GetCapsuleSystemFirmwareVersion (
  IN EFI_CAPSULE_HEADER  *CapsuleHeader
  );

/**
  Pre-installation check for Capsule Update

  @param  CapsuleHeader                      Points to a capsule header.

  @retval ESRT_SUCCESS                       The Capsule passed the pre-installation criteria
  @retval ESRT_ERROR_UNSUCCESSFUL            The pre-installation criteria check failed
  @retval ESRT_ERROR_INSUFFICIENT_RESOURCES  Out of memory or persistent storage
  @retval ESRT_ERROR_INCORRECT_VERSION       Incorrect/incompatible firmware version
  @retval ESRT_ERROR_INVALID_IMAGE_FORMAT    Invalid Capsule image format
  @retval ESRT_ERROR_AUTHENTICATION          Capsule image authentication failed
  @retval ESRT_ERROR_AC_NOT_CONNECTED        The system is not connected to the AC power
  @retval ESRT_ERROR_INSUFFICIENT_BATTERY    The battery capacity is low

**/
ESRT_STATUS
EFIAPI
PreInstallationCheck (
  EFI_CAPSULE_HEADER *Capsule
  );

/**
  Post-installation check for Capsule Update

  @param  CapsuleHeader                      Points to a capsule header.

  @retval ESRT_SUCCESS                       The Capsule passed the pre-installation criteria
  @retval ESRT_ERROR_UNSUCCESSFUL            The pre-installation criteria check failed
  @retval ESRT_ERROR_INSUFFICIENT_RESOURCES  Out of memory or persistent storage
  @retval ESRT_ERROR_AUTHENTICATION          Capsule image authentication failed

**/
ESRT_STATUS
EFIAPI
PostInstallationCheck (
  EFI_CAPSULE_HEADER *Capsule
  );

/**
 Convert capsule update capability error bits to error status.

 @param[in] ErrorBits              Capsule update capability error bits which is defined in H2OCp.h.

 @return Capsule update capability error status.
**/
ESRT_STATUS
EFIAPI
ConvertCapsuleErrorBitsToStatus (
  IN UINT32          ErrorBits
  );

/**
 Check capsule update capability and return the error bits which is defined in H2OCp.h.

 @param[in] Capsule              Points to a capsule.

 @return Capsule update capability error bits or zero if all checks are pass.
**/
UINT32
EFIAPI
CheckCapsuleUpdateCapability (
  IN VOID            *Capsule
  );

#endif

