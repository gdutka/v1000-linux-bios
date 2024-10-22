/** @file

;*******************************************************************************
;* Copyright (c) 2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;*******************************************************************************
*/

#ifndef _VGA_H_
#define _VGA_H_

#include <Base.h>

// For ADDL
#define CRT_AVAILABLE   BIT0
#define LCD_AVAILABLE   BIT1
#define TV_AVAILABLE    BIT2
#define HDMI_AVAILABLE  BIT3

// For CSTE  use
#define CRT_ACTIVE  BIT0
#define LCD_ACTIVE  BIT1
#define TV_ACTIVE   BIT2
#define HDMI_ACTIVE BIT3

// For NSTE
#define DISPLAY_STATE_1 0x01//CRT_ACTIVE
#define DISPLAY_STATE_2 0x02//LCD_ACTIVE
#define DISPLAY_STATE_3 0x03//CRT_ACTIVE|LCD_ACTIVE
#define DISPLAY_STATE_4 0x04//TV_ACTIVE
#define DISPLAY_STATE_5 0x05//CRT_ACTIVE|TV_ACTIVE
#define DISPLAY_STATE_6 0x06//CRT_ACTIVE|LCD_ACTIVE
#define DISPLAY_STATE_7 0x07//CRT_ACTIVE|LCD_ACTIVE|TV_ACTIVE
#define DISPLAY_STATE_8 0x08//HDMI_ACTIVE
#define DISPLAY_STATE_9 0x09//CRT_ACTIVE|HDMI_ACTIVE
#define DISPLAY_STATE_A 0x0a//LCD_ACTIVE|HDMI_ACTIVE
#define DISPLAY_STATE_B 0x0b//CRT_ACTIVE|LCD_ACTIVE|HDMI_ACTIVE
#define DISPLAY_STATE_C 0x0c//TV_ACTIVE|HDMI_ACTIVE
#define DISPLAY_STATE_D 0x0d//CRT_ACTIVE|TV_ACTIVE|HDMI_ACTIVE
#define DISPLAY_STATE_E 0x0e//LCD_ACTIVE|TV_ACTIVE|HDMI_ACTIVE
#define DISPLAY_STATE_F 0x0f//CRT_ACTIVE|LCD_ACTIVE|TV_ACTIVE|HDMI_ACTIVE

#define START_STATE     DISPLAY_STATE_1
#define FINAL_STATE     DISPLAY_STATE_F

#define BIOS_GET_DISPLAY_STATE          0x70
#define BIOS_SET_DISPLAY_STATE          0x71
#define BIOS_SET_LCD_BRIGHTNESS         0x72
#define BIOS_GET_LCD_BRIGHTNESS         0x73
#define BIOS_SET_LCD_BRIGHTNESS_XP      0x8A
#define AMD_ATIF_FUNCTION               0x8D

#endif

