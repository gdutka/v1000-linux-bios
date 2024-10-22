/** @file
  Provide OEM to modifying each MISC BIOS record.

;******************************************************************************
;* Copyright (c) 2013 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <Library/BaseLib.h>
#include <Library/DxeOemSvcKernelLib.h>
#include <IndustryStandard/SmBios.h>

#define MAX_STRING_LEN 64

STATIC
CHAR8*
EFIAPI
GotoNextSmbiosStr (
  IN CHAR8                                *Destination
  )
{
  return Destination + AsciiStrSize (Destination);
}

/**
  This service will be call by each time add MISC SMBIOS record.
  OEM can modifying MISC BIOS in run time.

  @param[in, out]  *RecordBuffer          Each MISC SMBIOS record data.

  @retval      EFI_UNSUPPORTED            Returns unsupported by default.
  @retval      EFI_SUCCESS                Updatable SMBIOS success.
  @retval      EFI_MEDIA_CHANGED          The value of IN OUT parameter is changed.
**/
EFI_STATUS
OemSvcDxeUpdateSmbiosRecord (
  IN OUT EFI_SMBIOS_TABLE_HEADER          *RecordBuffer
  )
{
  CHAR8                                   *StrPtr;
  CHAR8                                   FwRevisionStr[MAX_STRING_LEN] = {0};
  CHAR8                                   FwReleaseDateStr[MAX_STRING_LEN] = {0};

  if (RecordBuffer->Type == EFI_SMBIOS_TYPE_BIOS_INFORMATION) {
    StrPtr = (CHAR8*) RecordBuffer + sizeof (SMBIOS_TABLE_TYPE0);

    //
    // Firmware version.
    //
    UnicodeStrToAsciiStr ((CHAR16*) PcdGetPtr (PcdFirmwareVersionString), FwRevisionStr);
    StrPtr = GotoNextSmbiosStr (StrPtr);
    AsciiStrCpy (StrPtr, FwRevisionStr);

    //
    // Firmware release date.
    //
    UnicodeStrToAsciiStr ((CHAR16*) PcdGetPtr (PcdFirmwareReleaseDateString), FwReleaseDateStr);
    StrPtr = GotoNextSmbiosStr (StrPtr);
    AsciiStrCpy (StrPtr, FwReleaseDateStr);
  }

  return EFI_UNSUPPORTED;
}
