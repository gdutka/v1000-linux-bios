/** @file
  Type definition for the IhisiSmm module

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

#ifndef _IHISI_SERVICES_SMM_H_
#define _IHISI_SERVICES_SMM_H_

#include "IhisiRegistration.h"
#include "IhisiCommonFunctions.h"
#include <Guid/GlobalVariable.h>
#include <Guid/H2OCp.h>
#include <Library/SmmChipsetSvcLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/VariableLib.h>
#include <Library/H2OCpLib.h>
#include <Protocol/SysPasswordService.h>

EFI_STATUS
InstallVatsServices (
  VOID
  );

EFI_STATUS
InstallFbtsServices (
  VOID
  );

EFI_STATUS
InstallFetsServices (
  VOID
  );

EFI_STATUS
InstallCommonRegionCommunicationServices (
  VOID
  );

EFI_STATUS
InstallOemExtraDataCommunicationServices (
  VOID
  );

EFI_STATUS
InstallSecureFlashServices (
  VOID
  );

EFI_STATUS
InstallIhisiCommonServices (
  VOID
  );

#endif
