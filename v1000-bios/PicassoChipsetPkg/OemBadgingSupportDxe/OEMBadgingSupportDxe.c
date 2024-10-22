/** @file
  Source file for EFI OEM badging support driver.

;******************************************************************************
;* Copyright (c) 2012 - 2017, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <Uefi.h>
#include <KernelSetupConfig.h>
#include <OEMBadgingString.h>
#include <Guid/Pcx.h>

#include <OEMBadgingSupportDxe.h>
#include <Library/BadgingSupportLib.h>
#include <Library/BaseLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/KernelConfigLib.h>

extern OEM_BADGING_STRING mOemBadgingString[];
extern OEM_BADGING_STRING mOemBadgingStringAfterSelect[];
extern OEM_BADGING_STRING mOemBadgingStringInTextMode[];
extern OEM_BADGING_STRING mOemBadgingStringAfterSelectInTextMode[];

EFI_STATUS
InitializeBadgingSupportProtocol (
  IN    EFI_HANDLE                      ImageHandle,
  IN    EFI_SYSTEM_TABLE                *SystemTable,
  IN    EFI_OEM_BADGING_LOGO_DATA       *BadgingData,
  IN    UINTN                           NumberOfLogo,
  IN    OEM_VIDEO_MODE_SCR_STR_DATA     *OemVideoModeScreenStringData,
  IN    UINTN                           NumberOfVidoeModeScreenStringDataCount,
  IN    OEM_BADGING_STRING              *OemBadgingString,
  IN    OEM_BADGING_STRING              **OemBadgingStringAfterSelect,
  IN    UINTN                           NumberOfString
);


STATIC EFI_OEM_BADGING_LOGO_DATA mBadgingData[] = {
  {EFI_DEFAULT_PCX_LOGO_GUID,
   EfiBadgingSupportFormatPCX,
   EfiBadgingSupportDisplayAttributeCenter,
   0,
   0,
   NULL,
   EfiBadgingSupportImageLogo
  },
  //
  // BIOS Vendor Insyde Badge
  //
  {EFI_INSYDE_BOOT_BADGING_GUID,
   EfiBadgingSupportFormatBMP,
   EfiBadgingSupportDisplayAttributeCustomized,
   0,
   0,
   NULL,
   EfiBadgingSupportImageBoot
  },
  {EFI_INSYDE_BOOT_BADGING_GUID,
   EfiBadgingSupportFormatBMP,
   EfiBadgingSupportDisplayAttributeCenter,
   0,
   0,
   NULL,
   EfiBadgingSupportImageBadge
  }
};

STATIC OEM_VIDEO_MODE_SCR_STR_DATA mOemVideoModeScreenStringData[] = {
  //Bios Setup String
  {
    640,
    480,
    OemEnterSetupStr,
    400,
    392
  },
  {
    800,
    600,
    OemEnterSetupStr,
    500,
    490
  },
  {
    1024,
    768,
    OemEnterSetupStr,
    640,
    628
  },
  //Port80String
  {
    640,
    480,
    OemPort80CodeStr,
    600,
    440
  },
  {
    800,
    600,
    OemPort80CodeStr,
    750,
    550
  },
  {
    1024,
    768,
    OemPort80CodeStr,
    960,
    704
  },
  //Quality String
  {
    640,
    480,
    OemBuildQualityStr,
    280,
    232
  },
  {
    800,
    600,
    OemBuildQualityStr,
    350,
    290
  },
  {
    1024,
    768,
    OemBuildQualityStr,
    448,
    372
  }
};


/**
  Entry point of EFI OEM Badging Support driver

  @param[in]  ImageHandle - image handle of this driver
  @param[in]  SystemTable - pointer to standard EFI system table

  @retval EFI_SUCCESS       The entry point is executed successfully.
  @retval other             Some error occurred when executing this entry point.

**/
EFI_STATUS
OEMBadgingEntryPoint (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS                            Status;
  KERNEL_CONFIGURATION                  KernelSetup;
  UINTN                                 StringCount;
  OEM_BADGING_STRING                    *OemBadgingString;
  
  Status = GetKernelConfiguration (&KernelSetup);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Select OEM badging strings based on graphic mode or text mode.
  //
  StringCount = KernelSetup.QuietBoot ? BADGING_STRINGS_COUNT : BADGING_STRINGS_COUNT_TEXT_MODE;
  OemBadgingString = KernelSetup.QuietBoot ? &mOemBadgingStringAfterSelect[0] : &mOemBadgingStringAfterSelectInTextMode[0];

  Status = InitializeBadgingSupportProtocol (
             ImageHandle,
             SystemTable,
             mBadgingData,
             sizeof (mBadgingData) / sizeof (EFI_OEM_BADGING_LOGO_DATA),
             mOemVideoModeScreenStringData,
             sizeof (mOemVideoModeScreenStringData) / sizeof (OEM_VIDEO_MODE_SCR_STR_DATA),
             KernelSetup.QuietBoot ? mOemBadgingString : mOemBadgingStringInTextMode,
             (OEM_BADGING_STRING**) OemBadgingString,
             StringCount
             );

  return Status;
}

