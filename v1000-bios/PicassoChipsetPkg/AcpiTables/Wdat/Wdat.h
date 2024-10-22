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

#ifndef _WDAT_H_
#define _WDAT_H_

//
// Statements that include other files
//
#include <Library/PcdLib.h>

#include <BiosRamUsage.h>
#include <AcpiHeaderDefaultValue.h>
#include <IndustryStandard/WatchdogActionTable.h>

//
// TOTAL_NUMBER_ACTION_ENTRIES must match action table entries in Wdat.act
//
#define TOTAL_NUMBER_ACTION_ENTRIES         13

//
// Watchdog Action Table
//
typedef struct {
  EFI_ACPI_WATCHDOG_ACTION_1_0_TABLE                                  WatchdogHeader;
  EFI_ACPI_WATCHDOG_ACTION_1_0_WATCHDOG_ACTION_INSTRUCTION_ENTRY      ActionEntries[TOTAL_NUMBER_ACTION_ENTRIES];
} EFI_ACPI_WATCHDOG_ACTION_1_0_DESCRIPTION_TABLE;

#endif
