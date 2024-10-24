/** @file
  Definitions for FAT File Time functions

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

#ifndef _FAT_FILE_TIME_H_
#define _FAT_FILE_TIME_H_

#include <Library/IoLib.h>
#include <Library/CmosLib.h>
#include <Library/TimerLib.h>

#define R_CMOS_INDEX                 0x70
#define R_CMOS_DATA                  0x71

//
// The maximum retry for RTC reading
//
#define MAX_RETRY       10000

#define DELAY_TIME      10

//
// Dallas DS12C887 Real Time Clock
//
#define RTC_ADDRESS_SECONDS           0   // R/W  Range 0..59
#define RTC_ADDRESS_SECONDS_ALARM     1   // R/W  Range 0..59
#define RTC_ADDRESS_MINUTES           2   // R/W  Range 0..59
#define RTC_ADDRESS_MINUTES_ALARM     3   // R/W  Range 0..59
#define RTC_ADDRESS_HOURS             4   // R/W  Range 1..12 or 0..23 Bit 7 is AM/PM
#define RTC_ADDRESS_HOURS_ALARM       5   // R/W  Range 1..12 or 0..23 Bit 7 is AM/PM
#define RTC_ADDRESS_DAY_OF_THE_WEEK   6   // R/W  Range 1..7
#define RTC_ADDRESS_DAY_OF_THE_MONTH  7   // R/W  Range 1..31
#define RTC_ADDRESS_MONTH             8   // R/W  Range 1..12
#define RTC_ADDRESS_YEAR              9   // R/W  Range 0..99
#define RTC_ADDRESS_REGISTER_A        10  // R/W[0..6]  R0[7]
#define RTC_ADDRESS_REGISTER_B        11  // R/W
#define RTC_ADDRESS_REGISTER_C        12  // RO
#define RTC_ADDRESS_REGISTER_D        13  // RO
#define RTC_ADDRESS_CENTURY           50  // R/W  Range 19..20 Bit 8 is R/W

#define CMOS_DAYLIGHT                 0x36
#define CMOS_TIME_ZONE                0x34

typedef struct {
  UINT8  Address; ///< RTC offset address.
  UINT8  Data;    ///< Corresponding data value.
} RTC_COMPONENT;


#pragma pack(1)
//
// Register A
//
typedef struct {
  UINT8 Rs : 4;   // Rate Selection Bits
  UINT8 Dv : 3;   // Divisor
  UINT8 Uip : 1;  // Update in progress
} RTC_REGISTER_A_BITS;

typedef union {
  RTC_REGISTER_A_BITS Bits;
  UINT8               Data;
} RTC_REGISTER_A;

//
// Register B
//
typedef struct {
  UINT8 Dse : 1;  // 0 - Daylight saving disabled  1 - Daylight savings enabled
  UINT8 Mil : 1;  // 0 - 12 hour mode              1 - 24 hour mode
  UINT8 Dm : 1;   // 0 - BCD Format                1 - Binary Format
  UINT8 Sqwe : 1; // 0 - Disable SQWE output       1 - Enable SQWE output
  UINT8 Uie : 1;  // 0 - Update INT disabled       1 - Update INT enabled
  UINT8 Aie : 1;  // 0 - Alarm INT disabled        1 - Alarm INT Enabled
  UINT8 Pie : 1;  // 0 - Periodic INT disabled     1 - Periodic INT Enabled
  UINT8 Set : 1;  // 0 - Normal operation.         1 - Updates inhibited
} RTC_REGISTER_B_BITS;

typedef union {
  RTC_REGISTER_B_BITS Bits;
  UINT8               Data;
} RTC_REGISTER_B;

#pragma pack()

#endif

