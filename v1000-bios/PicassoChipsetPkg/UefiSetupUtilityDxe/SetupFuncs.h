/** @file

;******************************************************************************
;* Copyright (c) 2012 - 2017, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/
#ifndef _SETUP_FUNCS_H_
#define _SETUP_FUNCS_H_

#include "SetupUtility.h"
#include <PortNumberMap.h>


EFI_STATUS
LoadCustomOption (
  IN CONST EFI_HII_CONFIG_ACCESS_PROTOCOL        *This
  );

EFI_STATUS
SaveCustomOption (
  IN CONST EFI_HII_CONFIG_ACCESS_PROTOCOL        *This
  );

EFI_STATUS
DiscardChange (
  IN CONST EFI_HII_CONFIG_ACCESS_PROTOCOL        *This
  );

VOID *
GetVariableAndSize (
  IN  CHAR16              *Name,
  IN  EFI_GUID            *VendorGuid,
  OUT UINTN               *VariableSize
  );

EFI_STATUS
CheckLanguage (
  VOID
  );

EFI_STATUS
UpdateAtaString(
  IN      EFI_ATAPI_IDENTIFY_DATA     *IdentifyDriveInfo,
  IN OUT  CHAR16                      **NewString
  );

EFI_STATUS
AsciiToUnicode (
  IN    CHAR8     *AsciiString,
  IN    CHAR16    *UnicodeString
  );

EFI_STATUS
EventTimerControl (
  IN UINT64                     Timeout
  );

EFI_STATUS
SearchMatchedPortNum (
  IN     UINT32                              Bus,
  IN     UINT32                              Device,
  IN     UINT32                              Function,
  IN     UINT8                               PrimarySecondary,
  IN     UINT8                               SlaveMaster,
  OUT    UINTN                               *PortNum
  );

EFI_STATUS
CheckSataPort (
  IN UINTN                                  PortNum
  );

CHAR16 *
GetTokenStringByLanguage (
  IN EFI_HII_HANDLE                             HiiHandle,
  IN EFI_STRING_ID                              Token,
  IN CHAR8                                      *LanguageString
  );

EFI_STATUS
SaveSetupConfig (
  IN     CHAR16             *VariableName,
  IN     EFI_GUID           *VendorGuid,
  IN     UINT32             Attributes,
  IN     UINTN              DataSize,
  IN     VOID               *Buffer
  );

EFI_STATUS
AddNewString (
  IN   EFI_HII_HANDLE           InputHiiHandle,
  IN   EFI_HII_HANDLE           OutputHiiHandle,
  IN   EFI_STRING_ID            InputToken,
  OUT  EFI_STRING_ID            *OutputToken
  );

EFI_STATUS
DefaultSetup (
  OUT CHIPSET_CONFIGURATION          *SetupNvData
  );

VOID
ClearSetupVariableInvalid (
  VOID
  );

EFI_STATUS
SetupRuntimeDetermination (
  IN OUT  CHIPSET_CONFIGURATION        *SetupNvData
  );

EFI_STATUS
SetupRuntimeUpdateEveryBoot (
  IN OUT  CHIPSET_CONFIGURATION        *SetupNvData
  );

#endif
