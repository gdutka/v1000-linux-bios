/** @file

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _EC_LIBRARY_H_
#define _EC_LIBRARY_H_

#include <Uefi.h>

EFI_STATUS
EcIdle (
  IN BOOLEAN                      EnableEcIdle
  );

EFI_STATUS
EcWait (
  IN BOOLEAN                      EnableEcWait
  );

EFI_STATUS
EcInit(
  );

EFI_STATUS
EcAcpiMode (
  IN BOOLEAN                      EnableEcAcpiMode
  );

EFI_STATUS
EcVersion(
  IN OUT  CHAR16    *EcVersion
  );

EFI_STATUS
EcLibVersion(
  IN OUT  CHAR16    *EcLib_Version
  );

UINT8
EcRead (
  IN UINT8                        Index
  );

VOID
EcWrite (
  IN UINT8                        Index,
  IN UINT8                        Value
  );

EFI_STATUS
EcAcDcSwitch (
  IN UINT8                        EnableAcDcSwitch,
  IN UINT8                        DcTimer,
  IN UINT8                        AcTimer
  );

#endif
