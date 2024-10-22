/** @file
  Header file for IHISI common functions

;******************************************************************************
;* Copyright (c) 2023, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/
#ifndef _IHISI_COMMON_FUNCTIONS_H_
#define _IHISI_COMMON_FUNCTIONS_H_

#include "IhisiServicesSmm.h"


/**
  AH=83h, Get IHISI command buffer.

  @retval EFI_SUCCESS        Command successful returned.
**/
EFI_STATUS
EFIAPI
KernelIhisiGetCmdBuffer (
  VOID
  );

/**
  AH=83h, Get IHISI image buffer.

  @retval EFI_SUCCESS        Command successful returned.
**/
EFI_STATUS
EFIAPI
KernelGetImageBuffer (
  VOID
  );

/**
  AH=80h, Get IHISI image buffer.

  @retval EFI_SUCCESS        Command successful returned.
**/
EFI_STATUS
EFIAPI
KernelIhisiAuthStatus (
  VOID
  );

/**
  AH=81h, Get IHISI image buffer.

  @retval EFI_SUCCESS        Command successful returned.
**/
EFI_STATUS
EFIAPI
KernelIhisiAuthLock (
  VOID
  );

/**
  AH=82h, Get IHISI image buffer.

  @retval EFI_SUCCESS        Command successful returned.
**/
EFI_STATUS
EFIAPI
KernelIhisiAuthUnlock (
  VOID
  );

/**
  AH=84h, locks the IHISI interface and returns the first session token.

  @retval EFI_SUCCESS        Command successful returned.
**/
EFI_STATUS
EFIAPI
KernelIhisiAuth (
  VOID
  );

/**
  Initial IHISI security status.
**/
VOID
InitIhisiSecurityStatus (
  VOID
  );

/**
  Get Supported IHISI security levels

  return Bitmask that indicates the security levels supported by IHISI on this platform
**/
UINT32
GetSupportedIhisiSecurityLevels (
  VOID
  );

/**
  Get current IHISI security levels

  return Bitmask that indicates which security level was used when the IHISI session was opened.
**/
UINT32
GetCurrentIhisiSecurityLevel (
  VOID
  );

extern BOOLEAN              mSmmReadyToBoot;

#endif
