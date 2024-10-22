/** @file
  OEM can provide a way to get replace string for OEM depend.
  It's like DMI Tool but difference.

;******************************************************************************
;* Copyright (c) 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <Library/DxeOemSvcKernelLib.h>
#include <Library/BaseLib.h>
#include <Library/CommonPciLib.h>
#include <Protocol/Smbios.h>
#include <IndustryStandard/SmBios.h>

/**
  This service will be call by each time process SMBIOS string.
  OEM can provide a way to get replace string for OEM depend.

  @param[in]  Type           SMBIOS type number.
  @param[in]  FieldOffset    Field offset.
  @param[out] StrLength      String length.
  @param[out] String         String point.

  @retval    EFI_UNSUPPORTED       Returns unsupported by default.
  @retval    EFI_SUCCESS           N/A.
  @retval    EFI_MEDIA_CHANGED     The value of OUT parameter is changed.
**/
EFI_STATUS
OemSvcDxeGetSmbiosReplaceString (
  IN  EFI_SMBIOS_TYPE   Type,
  IN  UINT8             FieldOffset,
  OUT UINTN            *StrLength,
  OUT CHAR8           **String
  )
{
  STATIC  CHAR8         Tambourine[10] = { 'T', 'a', 'm', 'b', 'o', 'u', 'r', 'i', 'n', 'e' };
  UINTN                 StrLenTam = (UINTN) (sizeof (Tambourine) / sizeof (CHAR8));

  STATIC CHAR8          Bilby[5] = {'B', 'i', 'l', 'b', 'y'};
  UINTN                 StrLenBil = (UINTN) (sizeof (Bilby) / sizeof (CHAR8));

  if ((Type == EFI_SMBIOS_TYPE_SYSTEM_INFORMATION && (FieldOffset == 0x05)) || ((Type == EFI_SMBIOS_TYPE_BASEBOARD_INFORMATION) && (FieldOffset == 0x05))) {

    //
    // Update Product name
    //
    if ((PcdGet16 (OriginalPlatformId)) & BIT2) { // OriginalBoardId BIT2 = 1 is Tambourine
      *String = Tambourine;
	  *StrLength = StrLenTam;
    } else {
      *String = Bilby;
	  *StrLength = StrLenBil;
    }

    return EFI_SUCCESS;
  }


  return EFI_UNSUPPORTED;
}
