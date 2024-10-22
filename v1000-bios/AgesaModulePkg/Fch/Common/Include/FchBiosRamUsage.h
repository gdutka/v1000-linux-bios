/* $NoKeywords:$ */
/**
 * @file
 *
 * FCH BIOS Ram usage
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: FCH
 * @e \$Revision: 309090 $   @e \$Date: 2014-12-09 10:28:05 -0800 (Tue, 09 Dec 2014) $
 *
 */
/*
*****************************************************************************
*
* Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
*
*******************************************************************************
*/
#ifndef _FCH_BIOS_RAM_USAGE_H_
#define _FCH_BIOS_RAM_USAGE_H_

/*
* FCH BIOS RAM Usage
* 
* [0x00 ~ 0x1F]    
* [0x20 ~ 0x33]    SMIx00080-SMIx00090[SmiStatus0/1/2/4]
* [0x34 ~ 0x3F]
* [0x40]           SWSmiValue
* [0x41 ~ 0xFF]
*/


#define RESTORE_MEMORY_CONTROLLER_START       0
#define XHCI_REGISTER_BAR00                   0xD0
#define XHCI_REGISTER_BAR01                   0xD1
#define XHCI_REGISTER_BAR02                   0xD2
#define XHCI_REGISTER_BAR03                   0xD3
#define XHCI_REGISTER_04H                     0xD4
#define XHCI_REGISTER_0CH                     0xD5
#define XHCI_REGISTER_3CH                     0xD6
#define XHCI1_REGISTER_BAR00                  0xE0
#define XHCI1_REGISTER_BAR01                  0xE1
#define XHCI1_REGISTER_BAR02                  0xE2
#define XHCI1_REGISTER_BAR03                  0xE3
#define XHCI1_REGISTER_04H                    0xE4
#define XHCI1_REGISTER_0CH                    0xE5
#define XHCI1_REGISTER_3CH                    0xE6
#define RTC_WORKAROUND_DATA_START             0xF0
#define BOOT_TIME_FLAG_SEC                    0xF8
#define BOOT_TIME_FLAG_INT19                  0xFC

#endif


