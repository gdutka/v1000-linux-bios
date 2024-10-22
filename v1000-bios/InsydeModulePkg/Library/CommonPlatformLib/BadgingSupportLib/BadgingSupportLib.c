/** @file
  BadgingSupportLib will produces badging services for Oem badging driver.

;******************************************************************************
;* Copyright (c) 2013 - 2018, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <Uefi.h>
#include <Library/PcdLib.h>
#include <Library/HobLib.h>
#include <Library/HiiLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DxeServicesLib.h>
#include <Library/BadgingSupportLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Protocol/SetupUtility.h>
#include <KernelSetupConfig.h>
#include <Library/DxeOemSvcKernelLib.h>
#include <Library/VariableLib.h>
#include <Library/OemGraphicsLib.h>

STATIC BOOLEAN  mShowWarningLogo = FALSE;

#define EFI_OS_INDICATIONS_BOOT_TO_FW_UI            0x0000000000000001
STATIC EFI_HII_HANDLE    mHiiHandle;
extern UINT8      OemBadgingSupportDxeStrings[];


/**
  Initialize and Install OEM Badging support protocol.

  @param[in] ImageHandle                             image handle of this driver
  @param[in] SystemTable                             pointer to standard EFI system table
  @param[in] BadgingData                             Pointer of EFI_OEM_BADGING_LOGO_DATA structure
  @param[in] NumberOfLogo                            Number of LOGO to be used.
  @param[in] OemVidoeModeScreenStringData            Pointer of OEM_VIDEO_MODE_SCR_STR_DATA
  @param[in] NumberOfVidoeModeScreenStringDataCount  Number of OEM_VIDEO_MODE_SCR_STR_DATA data.
  @param[in] OemBadgingString                        Pointer of OEM_BADGING_STRING before user selection
  @param[in] OemBadgingStringAfterSelect             Pointer of OEM_BADGING_STRING after user selection
  @param[in] NumberOfString                          Number of OEM BADGING String

  @retval EFI_SUCCESS       The entry point is executed successfully.
  @retval other             Some error occurred when executing this entry point.

**/
EFI_STATUS
InitializeBadgingSupportProtocol (
  IN    EFI_HANDLE                      ImageHandle,
  IN    EFI_SYSTEM_TABLE                *SystemTable,
  IN    EFI_OEM_BADGING_LOGO_DATA       *BadgingData,
  IN    UINTN                           NumberOfLogo,
  IN    OEM_VIDEO_MODE_SCR_STR_DATA     *OemVidoeModeScreenStringData,
  IN    UINTN                           NumberOfVidoeModeScreenStringDataCount,
  IN    OEM_BADGING_STRING              *OemBadgingString,
  IN    OEM_BADGING_STRING              **OemBadgingStringAfterSelect,
  IN    UINTN                           NumberOfString
)
{
  EFI_STATUS                            Status;
  OEM_BADGING_INFO                      *OemBadgingInfo;

  OemBadgingInfo = AllocateZeroPool (sizeof(OEM_BADGING_INFO));
  if (OemBadgingInfo == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  OemBadgingInfo->Signature                      = EFI_OEM_BADGING_INFO_SIGNATURE;
  OemBadgingInfo->BadgingData                    = BadgingData;
  OemBadgingInfo->InstanceCount                  = NumberOfLogo;
  OemBadgingInfo->OemVidoeModeScreenStringData   = OemVidoeModeScreenStringData;
  OemBadgingInfo->VidoeModeScreenStringDataCount = NumberOfVidoeModeScreenStringDataCount;
  OemBadgingInfo->OemBadgingString               = OemBadgingString;
  OemBadgingInfo->OemBadgingStringAfterSelect    = OemBadgingStringAfterSelect;
  OemBadgingInfo->BadginStringCount              = NumberOfString;

  OemBadgingInfo->OemBadging.GetImage            = BadgingGetImage;
  OemBadgingInfo->OemBadging.OemVideoModeScrStrXY= OemVideoModeScrStrLocation;
  OemBadgingInfo->OemBadging.GetOemString        = BadgingGetOemString;
  OemBadgingInfo->OemBadging.GetStringCount      = BadgingGetStringCount;

  OemBadgingInfo->Handle = NULL;
  Status = gBS->InstallProtocolInterface (
                  &OemBadgingInfo->Handle,
                  &gEfiOEMBadgingSupportProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &OemBadgingInfo->OemBadging
                  );

  if (EFI_ERROR (Status)) {
    ASSERT_EFI_ERROR (Status);
    return Status;
  }

  mHiiHandle = HiiAddPackages (&gEfiCallerIdGuid, NULL, OemBadgingSupportDxeStrings, NULL);
  ASSERT (mHiiHandle != NULL);

  return Status;
}

/**
  Get Image related information according to Instance and Type data.

  @param[in] This         EFI_OEM_BADGING_SUPPORT_PROTOCOL Interface
  @param[in] Instance     Instace number of mBadgingData structure
  @param[in] Type         Image type
  @param[in] Format       LOGO format
  @param[in] ImageData    Logo data pointer
  @param[in] ImageSize    Logo size
  @param[in] Attribute    Display attribute
  @param[in] CoordinateX  Display location
  @param[in] CoordinateY  Display location

  @retval EFI_SUCCESS      Get Logo Image succesfully
  @retval EFI_NOT_FOUND    Cannot find imaga

**/
EFI_STATUS
EFIAPI
BadgingGetImage (
  IN     EFI_OEM_BADGING_SUPPORT_PROTOCOL      *This,
  IN OUT UINT32                                *Instance,
  IN OUT EFI_BADGING_SUPPORT_IMAGE_TYPE        *Type,
  OUT EFI_BADGING_SUPPORT_FORMAT               *Format,
  OUT UINT8                                    **ImageData,
  OUT UINTN                                    *ImageSize,
  OUT EFI_BADGING_SUPPORT_DISPLAY_ATTRIBUTE    *Attribute,
  OUT UINTN                                    *CoordinateX,
  OUT UINTN                                    *CoordinateY
  )
{
  EFI_OEM_BADGING_LOGO_DATA  *logos;
  OEM_BADGING_INFO           *OemBadgingInfo;
  EFI_OEM_BADGING_LOGO_DATA  *TempLogo;
  EFI_STATUS                 OemSvcStatus;

  if (This == NULL || Instance == NULL || Type == NULL || Format == NULL || ImageData == NULL ||
      ImageSize == NULL || Attribute == NULL || CoordinateX == NULL || CoordinateY == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  //
  //init locals
  //
  logos = NULL;
  OemBadgingInfo = NULL;
  TempLogo = NULL;

  OemBadgingInfo = EFI_OEM_BADGING_INFO_FROM_THIS (This);
  logos = OemBadgingInfo->BadgingData;

  while ((*Instance) < OemBadgingInfo->InstanceCount) {
    if (((logos[*Instance].Visible == NULL) ||
         (logos[*Instance].Visible())) &&
         (logos[*Instance].Type == *Type) &&
         !EFI_ERROR (ReadLogoImage (&logos[*Instance], ImageData, ImageSize))) {

      TempLogo = AllocateCopyPool (sizeof (EFI_OEM_BADGING_LOGO_DATA), &logos[*Instance]);
      if (!mShowWarningLogo) {
        DEBUG ((DEBUG_INFO, "OemKernelServices Call: OemSvcChangeDefaultLogoImage \n"));
        OemSvcStatus = OemSvcChangeDefaultLogoImage (TempLogo, ImageData, ImageSize);
        DEBUG ((DEBUG_INFO, "OemKernelServices OemSvcChangeDefaultLogoImage Status: %r\n", OemSvcStatus));
      }

      if ((TempLogo != NULL) && (*ImageSize != 0)) {
        *Format       = TempLogo->Format;
        *CoordinateX  = TempLogo->CoordinateX;
        *CoordinateY  = TempLogo->CoordinateY;
        *Attribute    = TempLogo->Attribute;
        (*Instance)++;
        FreePool (TempLogo);
        return EFI_SUCCESS;
      } else {
        FreePool (TempLogo);
        return EFI_NOT_FOUND;
      }
    }
    (*Instance)++;
  }

  return EFI_NOT_FOUND;
}

/**
  Read Logo Image from Firmware Volumn

  @param[in] logo       Logo file GUID
  @param[in] ImageData  Logo data found in ROM
  @param[in] ImageSize  Logo size

  @retval EFI_SUCCESS           Get Logo succesfully
  @retval EFI_BUFFER_TOO_SMALL  Input buffer size too small
  @retval EFI_NOT_FOUND         Cannot find Image in ROM.

**/
EFI_STATUS
EFIAPI
ReadLogoImage (
  IN EFI_OEM_BADGING_LOGO_DATA        *logo,
  OUT UINT8                           **ImageData,
  OUT UINTN                           *ImageSize
  )
{
  EFI_STATUS                       Status;
  EFI_BADGING_SUPPORT_IMAGE_TYPE   BadgeImageType;

  //
  // Check if QA certificate is used in BIOS. If QA certificate exists, show warning logo!
  //
  if (FeaturePcdGet(PcdH2OAcpiBgrtSupported) == TRUE) {
    //
    // BGRT feature is enabled.
    //
    BadgeImageType = EfiBadgingSupportImageBoot;
  } else {
    BadgeImageType = EfiBadgingSupportImageBadge;
  }

  if ((logo->Type == BadgeImageType) && IsReplacedByWarningLogo()) {
    CopyMem (&logo->FileName, PcdGetPtr (PcdWarningLogoFile), sizeof (EFI_GUID));
    logo->Format      = EfiBadgingSupportFormatJPEG;
    logo->CoordinateX = 0;
    logo->CoordinateY = 0;
    if (FeaturePcdGet(PcdH2OAcpiBgrtSupported) == TRUE) {
      logo->Attribute   = EfiBadgingSupportDisplayAttributeCustomized;
    } else {
      logo->Attribute   = EfiBadgingSupportDisplayAttributeCenter;
    }
    mShowWarningLogo   = TRUE;
  }
  Status = GetSectionFromAnyFv (&logo->FileName, EFI_SECTION_RAW, 0, (VOID **)ImageData, ImageSize);

  return Status;
}

/**
  Provide oem logo related services.
  1.) check and return status of supported video resolution mode.
  2.) return XY location of the string based on the video resoultion.

  @param[in] This                 - Protocol instance pointer.
  @param[in] ServiceType          - Type of service needed. Refer to type list.
                                      OemSupportedVideoMode
                                      OemEnterSetupStr
                                      OemPort80CodeStr
                                      OemBuildQualityStr
  @param[in] VideoWidth           - input parameter, screen width in pixel.
  @param[in] VideoHeight          - input parameter, screen height in pixel.
  @param[in] *StringLocationX     - pointer to hold the result of the String X location.
                                      This arguments is ignore for service type "OemSupportedVideoMode".
  @param[in] *StringLocationY     - pointer to hold the result of the String Y location.
                                      This arguments is ignore for service type "OemSupportedVideoMode".

  @retval TRUE   - Video mode is supported or String XY location is found.
  @retval FALSE  - video mode not supported or String XY location not found.

**/
BOOLEAN
EFIAPI
OemVideoModeScrStrLocation (
  IN EFI_OEM_BADGING_SUPPORT_PROTOCOL   *This,
  IN EFI_OEM_VIDEO_MODE_SCREEN_XY_TYPE  ServiceType,
  IN UINT32                             VideoWidth,
  IN UINT32                             VideoHeight,
  IN OUT UINTN                          *StringLocationX,
  IN OUT UINTN                          *StringLocationY
)
{
  OEM_VIDEO_MODE_SCR_STR_DATA           *CurrentEntry;
  UINTN                                 EntryCount;
  UINTN                                 Index;
  OEM_BADGING_INFO                      *OemBadgingInfo;

  OemBadgingInfo = EFI_OEM_BADGING_INFO_FROM_THIS (This);

  *StringLocationX = 0;
  *StringLocationY = 0;
  EntryCount = OemBadgingInfo->VidoeModeScreenStringDataCount;
  CurrentEntry = OemBadgingInfo->OemVidoeModeScreenStringData;

  switch (ServiceType) {
    case OemSupportedVideoMode:
      for (Index=0; Index < EntryCount; Index++) {
         if ((VideoWidth == CurrentEntry[Index].XPixel) &&
             (VideoHeight == CurrentEntry[Index].YPixel)) {
           return TRUE;
         }
      }
      break;

    case OemEnterSetupStr:
    case OemPort80CodeStr:
    case OemBuildQualityStr:
        for (Index=0; Index < EntryCount; Index++) {
          if ((VideoWidth == CurrentEntry[Index].XPixel) &&
              (VideoHeight == CurrentEntry[Index].YPixel) &&
              (ServiceType == CurrentEntry[Index].ServiceType)) {
            *StringLocationX = CurrentEntry[Index].StrLocX;
            *StringLocationY = CurrentEntry[Index].StrLocY;
            return TRUE;
          }
        }
      break;

    default:
      break;
  }

  return FALSE;
}

/**
  Get String information which should be shown on screen

  @param[in] This                  Protocol instance pointer.
  @param[in] Index                 String Index of mOemBadgingString array
  @param[in] AfterSelect           Indicate the which string should be shown
  @param[in] SelectedStringNum     String number that should be changed according to user hot key selection
  @param[in] StringData            String data to output on screen
  @param[in] CoordinateX           String data location on screen
  @param[in] CoordinateY           String data location on screen
  @param[in] Foreground            Foreground color information of the string
  @param[in] Background            Background color information of the string

  @retval TRUE   - Preferred string has be gotten
  @retval FALSE  - Cannot find preferred string.

**/
BOOLEAN
EFIAPI
BadgingGetOemString (
  IN     EFI_OEM_BADGING_SUPPORT_PROTOCOL  *This,
  IN     UINTN                             Index,
  IN     BOOLEAN                           AfterSelect,
  IN     UINT8                             SelectedStringNum,
     OUT CHAR16                            **StringData,
     OUT UINTN                             *CoordinateX,
  OUT    UINTN                             *CoordinateY,
  OUT    EFI_UGA_PIXEL                     *Foreground,
  OUT    EFI_UGA_PIXEL                     *Background
)
{
  BOOLEAN                                  Flag = FALSE;
  EFI_BOOT_MODE                            BootMode;
  UINTN                                    Size;
  OEM_BADGING_INFO                         *OemBadgingInfo;
  OEM_BADGING_STRING                       *OemBadgingString;
  OEM_BADGING_STRING                       *OemBadgingStringAfterSelect;
  EFI_STATUS                               Status;
  UINT64                                   OsIndications;
  UINT64                                   OsIndicationsSupported;
  KERNEL_CONFIGURATION                     *SystemConfiguration = NULL;

  OemBadgingInfo = EFI_OEM_BADGING_INFO_FROM_THIS (This);
  if (Index > OemBadgingInfo->BadginStringCount) {
    return FALSE;
  }

  Size = sizeof(OsIndicationsSupported);
  Status = CommonGetVariable (L"OsIndicationsSupported", &gEfiGlobalVariableGuid, &Size, &OsIndicationsSupported);
  if (EFI_ERROR (Status)) {
    OsIndicationsSupported = 0;
  }
  Size = sizeof(OsIndications);
  Status = CommonGetVariable (L"OsIndications", &gEfiGlobalVariableGuid, &Size, &OsIndications);
  if (EFI_ERROR (Status)) {
    OsIndications = 0;
  }
  if (OsIndicationsSupported & OsIndications & EFI_OS_INDICATIONS_BOOT_TO_FW_UI) {
    return FALSE;
  }

  BootMode = GetBootModeHob();
  SystemConfiguration = CommonGetVariableData (SETUP_VARIABLE_NAME, &gSystemConfigurationGuid);
  if (BootMode == BOOT_ON_S4_RESUME && SystemConfiguration != NULL && SystemConfiguration->BootType != EFI_BOOT_TYPE) {
    FreePool (SystemConfiguration);
    return FALSE;
  }

  if (!AfterSelect) {
    if (SystemConfiguration == NULL) {
      DEBUG ((EFI_D_INFO, "System get system configuration data failed.\n"));
      return FALSE;
    }
    if (!FeaturePcdGet (PcdDisplayOemHotkeyString) && (SystemConfiguration->QuietBoot) && (SystemConfiguration->BootType == EFI_BOOT_TYPE)) {
      FreePool (SystemConfiguration);
      return FALSE;
    }
    OemBadgingString = OemBadgingInfo->OemBadgingString;
    CopyMem (Foreground, &(OemBadgingString[Index].Foreground), sizeof (EFI_UGA_PIXEL));
    CopyMem (Background, &(OemBadgingString[Index].Background), sizeof (EFI_UGA_PIXEL));
    *CoordinateX = OemBadgingString[Index].X;
    *CoordinateY = OemBadgingString[Index].Y;
    if (OemBadgingString[Index].Fun == NULL) {
      *StringData = HiiGetPackageString (&gEfiCallerIdGuid, OemBadgingString[Index].StringToken, NULL);
      Flag = TRUE;
    } else {
      Flag = OemBadgingString[Index].Fun(&OemBadgingString[Index], StringData);
    }
  } else {
    OemBadgingStringAfterSelect = (OEM_BADGING_STRING*)OemBadgingInfo->OemBadgingStringAfterSelect;
    if (SelectedStringNum != 0) {
      OemBadgingString = (OEM_BADGING_STRING*)&(OemBadgingStringAfterSelect[(SelectedStringNum-1) * OemBadgingInfo->BadginStringCount].X);
      CopyMem (Foreground, &(OemBadgingString[Index].Foreground), sizeof (EFI_UGA_PIXEL));
      CopyMem (Background, &(OemBadgingString[Index].Background), sizeof (EFI_UGA_PIXEL));
      *CoordinateX = OemBadgingString[Index].X;
      *CoordinateY = OemBadgingString[Index].Y;

      if (OemBadgingString[Index].Fun == NULL) {
        *StringData = HiiGetPackageString (&gEfiCallerIdGuid, OemBadgingString[Index].StringToken, NULL);
        Flag = TRUE;
      } else {
        Flag = OemBadgingString[Index].Fun(&OemBadgingString[Index], StringData);
      }
    }
  }
  if (SystemConfiguration != NULL) {
    FreePool (SystemConfiguration);
  }
  if (*StringData == NULL) {
    return FALSE;
  }

  return Flag;
}

/**
  Get Number of string that should be shown on screen

  @param[in]     This                 Protocol instance pointer.
  @param[in]     StringCount          Number of string that should be shown on screen

  @retval EFI_SUCCESS       Get String Count Succesfully.
  @retval other             There is no string should be shown.

**/
EFI_STATUS
EFIAPI
BadgingGetStringCount (
  IN      EFI_OEM_BADGING_SUPPORT_PROTOCOL   *This,
     OUT  UINTN                              *StringCount
)
{
  OEM_BADGING_INFO          *OemBadgingInfo;

  OemBadgingInfo = EFI_OEM_BADGING_INFO_FROM_THIS (This);

  *StringCount = OemBadgingInfo->BadginStringCount;

  if ( *StringCount == 0 ) {
    return EFI_UNSUPPORTED;
  }

  return EFI_SUCCESS;
}
