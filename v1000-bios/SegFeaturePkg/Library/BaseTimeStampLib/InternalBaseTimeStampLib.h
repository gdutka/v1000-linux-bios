/** @file
  Private header of BaseTimeStampLib

;******************************************************************************
;* Copyright (c) 2021, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef __BASE_TIME_STAMP_LIB_H__
#define __BASE_TIME_STAMP_LIB_H__

#include <Uefi.h>

#include <Library/PcdLib.h>

#define MAX_RETRY                     5

#define CMOS_YEAR                     0x09
#define CMOS_MONTH                    0x08
#define CMOS_DAY                      0x07
#define CMOS_HOUR                     0x04
#define CMOS_MINUTE                   0x02
#define CMOS_SECOND                   0x00
#define CMOS_CENTURY                  0x32
#define CMOS_REGISTER_A               0x0A
#define CMOS_REGISTER_B               0x0B

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

typedef struct {
  UINT8  Address; ///< RTC offset address.
  UINT8  Data;    ///< Corresponding data value.
} RTC_COMPONENT;

#pragma pack(1)
//
// Register A
//
typedef struct {
  UINT8 Rs : 4;   ///< Rate Selection Bits
  UINT8 Dv : 3;   ///< Divisor
  UINT8 Uip : 1;  ///< Update in progress
} RTC_REGISTER_A_BITS;

typedef union {
  RTC_REGISTER_A_BITS Bits;
  UINT8               Data;
} RTC_REGISTER_A;

//
// Register B
//
typedef struct {
  UINT8 Dse : 1;  ///< 0 - Daylight saving disabled  1 - Daylight savings enabled
  UINT8 Mil : 1;  ///< 0 - 12 hour mode              1 - 24 hour mode
  UINT8 Dm : 1;   ///< 0 - BCD Format                1 - Binary Format
  UINT8 Sqwe : 1; ///< 0 - Disable SQWE output       1 - Enable SQWE output
  UINT8 Uie : 1;  ///< 0 - Update INT disabled       1 - Update INT enabled
  UINT8 Aie : 1;  ///< 0 - Alarm INT disabled        1 - Alarm INT Enabled
  UINT8 Pie : 1;  ///< 0 - Periodic INT disabled     1 - Periodic INT Enabled
  UINT8 Set : 1;  ///< 0 - Normal operation.         1 - Updates inhibited
} RTC_REGISTER_B_BITS;

typedef union {
  RTC_REGISTER_B_BITS Bits;
  UINT8               Data;
} RTC_REGISTER_B;
#pragma pack()

#endif