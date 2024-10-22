/** @file

;*******************************************************************************
;* Copyright (c) 2017-2018, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;*******************************************************************************
*/

#ifndef _AMD_GOP_CONFIG_H_
#define _AMD_GOP_CONFIG_H_

#include <Uefi.h>

#include <Guid/GlobalVariable.h>
#include <Guid/DebugMask.h>
#include <Protocol/DevicePath.h>
#include <Protocol/PlatformToDriverConfiguration.h>

#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DevicePathLib.h>

#include <Protocol/AmdCpmTableProtocol/AmdCpmTableProtocol.h>
#include <AmdCpmCommon.h>

//[-start-240426-IB18160212-modify-]//
#define EFI_PLATFORM_TO_AMDGOP_CONFIGURATION_GUID \
  {0x18b95167, 0x2ed8, 0x447d, {0xa6, 0x5d, 0xd0, 0xe5, 0xe6, 0xf5, 0xd9, 0x89}}
//[-end-240426-IB18160212-modify-]//

EFI_GUID gEfiPlatformtoAmdgopConfigurationGuid = EFI_PLATFORM_TO_AMDGOP_CONFIGURATION_GUID;

#define IS_AMD_VIDEO (p)           ((p)->Hdr.VendorId == 0x1002)) && ((p)->Hdr.ClassCode[1] == 3))

#define AMDGOP_PRIORITY_COUNT  6
//*******************************************************************************
//  The Parameter Description of PLATFORM_TO_AMDGOP_CONFIGURATION_COMMON_1 structure
//    - This structur will be returned when Instance == 0
//
//  PrioriyxActiveDisplay:
//    Each "x" is an ACPI_ADR entry. The prior one has precedence
//    to be driven by the AMD GOP UEFI display driver.
//    A default logic will be applied if all of them are set to 0x0.
//  PlatformFeatureEnabled
//    TBD
//
//*******************************************************************************
typedef struct _PLATFORM_TO_AMDGOP_CONFIGURATION_COMMON_1 {
  UINT32 RevisionCommonConfig;
  UINT32 Priority1ActiveDisplay;
  UINT32 Priority2ActiveDisplay;
  UINT32 Priority3ActiveDisplay;
  UINT32 Priority4ActiveDisplay;
  UINT32 Priority5ActiveDisplay;
  UINT32 Priority6ActiveDisplay;
  UINT32 ulReserved1[6];
  UINT32 PlatformFeatureEnabled; 
  UINT32 ulReserved2[3];
} PLATFORM_TO_AMDGOP_CONFIGURATION_COMMON_1;

//*******************************************************************************
//  The Parameter Description of _PLATFORM_TO_AMDGOP_CONFIGURATION_DISPLAY_1 structure
//    - This structur will be returned when Instance == 1
//
//  GPU_Controlled_LCD_PWM_FREQ_InHz:
//    The frequency (Hz) output from the CPU to control the LCD brightness level change.
//    A default frequency of 200 Hz will be used, if this is set to 0x0.
//  LCD_BootUp_BL_Level:
//    The initial LCD brightness level that the GOP driver will set to during boot-up.
//    Full brightness (255) will be set if this field is set to 0x0.
//  LCD_Min_BL_Level:
//    The minimum LCD brightness level allowed.
//    This is a value between 0 to 255.
//
//Note: Please ignore the LVDSMisc portion, this becomes TBD.
//
//  LVDSMiscConfiguration:
//    #define LVDSMiscConfiguration_FPDI 0x00000001
//      If the LVDS panel is a 888-bit panel in FPDI mode.
//      The default is LDI, if it is not set.
//    #define LVDSMiscConfiguration_Ch_SWAP 0x00000002
//      If the dual-channel LVDS panel has the upper and lower channels swapped.
//      The default is no swap, if it is not set.
//    #define LVDSMiscConfiguration_FPolarity 0x00000004
//      Set this if the frame pulse/VSYNC is reversed from what the LCD EDID describes.
//      The default is not reversed, if it is not set.
//    #define LVDSMiscConfiguration_LPolarity 0x00000008
//      If the line pulse/HSYNC is reversed from what the LCD EDID describes.
//      The default is not reversed, if it is not set.
//    #define LVDSMiscConfiguration_BLON_Polarity 0x00000010
//      If the BLON output priority from the GPU needs to be inverted.
//      The default is not inverted, if it is not set.
//
//Note: The following settings are only applicable to LVDS LCD panels. eDP is
//      not affected, except for the last item, "LCDOffToOnDelay_in4Ms."
//
//  LVDSPwrOnDIGONtoDE_in4Ms:
//    The delay time from DIGON signal active (LCDVCC On) to data enable signal active (DE)
//    in units of 4 ms.
//    The default is 32 ms, if it is set to 0x0.
//  LVDSPwrOnDEtoVARY_BL_in4Ms:
//    The delay time from DE (data enable) active to BL_PWM enable (VARY_BL)
//    in units of 4 ms.
//    The default is 360 ms, if it is set to 0x0.
//  LVDSPwrOnVARY_BLtoBLON_in4Ms:
//    The delay time from BL_PWM enable (VARY_BL) to BLON active
//    in units of 4 ms.
//    The default is 0, if it is set to 0x0.
//  LVDSPwrOffBLOFFtoVARY_BL_in4Ms:
//    The delay time from BLON inactive to BL_PWM disable (VARY_BL)
//    in units of 4 ms.
//    The default is 0.
//  LVDSPwrOffVARY_BLtoDE_in4Ms:
//    The delay time from BL_PWM disable (VARY_BL) to data enable signal inactive
//    in units of 4 ms.
//    The default is 360 ms, if it is set to 0x0.
//  LVDSPwrOffDEtoDIGON_in4Ms:
//    The delay time from data enable signal inactive to DIGON signal inactive (LCDVDDC Off)
//    in units of 4 ms.
//    The default is 32 ms, if it is set to 0x0.
//  LCDOffToOnDelay_in4Ms:
//    The delay time from DIGON signal inactive (LCDVCC Off) to the next DIGON signal active (LCDVCC On).
//    The default is 500 ms, if it is set to 0x0.
//
//Note: The following settings are only applicable to LVDS LCD panels. eDP is
//      not affected as the spread-spectrum (SS) settings are obtained from DPCD from an eDP panel.
//
//  LVDSSpreadSpectrumRateIn10Hz:
//    The SS frequency in units of 10 Hz used for this LVDS panel.
//    Use the default setting of 40 Hz, if it is set to 0x0.
//  LVDSSpreadSpectrumPercentage:
//    The SS percentage in units of 0.01%.
//    SS is disabled when the SS percentage is set to 0x0.
//
//*******************************************************************************
typedef struct _PLATFORM_TO_AMDGOP_CONFIGURATION_DISPLAY_1 {
  UINT32 Revision;
  UINT32 GPU_Controlled_LCD_PWM_FREQ_InHz;
  UINT8  LCD_BootUp_BL_Level;
  UINT8  Reserved00;
  UINT8  LCD_Min_BL_Level;
  UINT8  Reserved01;
  UINT32 LVDSMiscConfiguration;
  UINT8  LVDSPwrOnDIGONtoDE_in4Ms;
  UINT8  LVDSPwrOnDEtoVARY_BL_in4Ms;
  UINT8  LVDSPwrOnVARY_BLtoBLON_in4Ms;
  UINT8  LVDSPwrOffBLOFFtoVARY_BL_in4Ms;
  UINT8  LVDSPwrOffVARY_BLtoDE_in4Ms;
  UINT8  LVDSPwrOffDEtoDIGON_in4Ms;
  UINT8  LCDOffToOnDelay_in4Ms;
  UINT8  Reserved02[7];
  UINT16 LVDSSpreadSpectrumRateIn10Hz;
  UINT16 LVDSSpreadSpectrumPercentage;
  UINT32 Reserved03[12];
} PLATFORM_TO_AMDGOP_CONFIGURATION_DISPLAY_1;


typedef struct _PLATFORM_TO_AMDGOP_CONFIGURATION1 {
  PLATFORM_TO_AMDGOP_CONFIGURATION_COMMON_1  AmdGopConfigCommon1;
  PLATFORM_TO_AMDGOP_CONFIGURATION_DISPLAY_1 AmdGopConfigDisplay1;
} PLATFORM_TO_AMDGOP_CONFIGURATION1;

//
// Function Prototype
//
EFI_STATUS
AmdGopConfigDrvInit (
  IN EFI_HANDLE                                      ImageHandle,
  IN EFI_SYSTEM_TABLE                                *SystemTable
  );

BOOLEAN
IsAmdGop (
  IN EFI_HANDLE                                      ControllerHandle
  );

EFI_STATUS
AmdGopConfigQuery (
  IN CONST  EFI_PLATFORM_TO_DRIVER_CONFIGURATION_PROTOCOL *This,
  IN CONST  EFI_HANDLE                                    ControllerHandle,
  IN CONST  EFI_HANDLE                                    ChildHandle OPTIONAL,
  IN CONST  UINTN                                         *Instance,
  OUT       EFI_GUID                                      **ParameterTypeGuid,
  OUT       VOID                                          **ParameterBlock,
  OUT       UINTN                                         *ParameterBlockSize
  );

EFI_STATUS
AmdGopConfigResponse (
  IN CONST  EFI_PLATFORM_TO_DRIVER_CONFIGURATION_PROTOCOL *This,
  IN CONST  EFI_HANDLE                                    ControllerHandle,
  IN CONST  EFI_HANDLE                                    ChildHandle OPTIONAL,
  IN CONST  UINTN                                         *Instance,
  IN CONST  EFI_GUID                                      *ParameterTypeGuid,
  IN CONST  VOID                                          *ParameterBlock,
  IN CONST  UINTN                                         ParameterBlockSize,
  IN CONST  EFI_PLATFORM_CONFIGURATION_ACTION             ConfigurationAction
  );

#endif