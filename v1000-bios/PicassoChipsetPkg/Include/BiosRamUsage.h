/** @file

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

//
//NOTICE!!, This file must be sync with BiosRamUseag.inc
//

#ifndef _BIOS_RAM_USAGE_H_
#define _BIOS_RAM_USAGE_H_

#define BIOS_RAM_INDEX      0xCD4
#define BIOS_RAM_DATA       0xCD5
#define RESTORE_MEMORY_CONTROLLER_START 0
#define HWWDT_STATUS                    0xEF
#define   HWWDT_STATUS_SHUTDOWN_BY_WDT  0x01
#define SPI_LOCK_PROTECT_STATUS       0xEE
  #define SPI_LOCK_SMI_READY            0x1  // BIT0
  #define SPI_PROTECT_ENABLED           0x2  // BIT1
#define BOOT_TYPE_RECORD              0xEC
#define RTC_WORKAROUND_DATA_START     0xF0
#define BOOT_TIME_FLAG_SEC            0xF8
#define BOOT_TIME_FLAG_INT19          0xFC

#endif

