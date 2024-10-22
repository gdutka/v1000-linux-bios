/* $NoKeywords:$ */
/**
 * @file
 *
 * FCH Aoac operation lib
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: FCH
 * @e \$Revision: 312516 $   @e \$Date: 2015-02-06 17:37:45 -0800 (Fri, 06 Feb 2015) $
 *
 */
/*
*****************************************************************************
*
* Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
*
*******************************************************************************
*/
#include "FchPlatform.h"
#define FILECODE PROC_FCH_COMMON_FCHAOACLIB_FILECODE


/**
 * FchAoacPowerOnDev - Power On/Off Fch Device
 *
 * @param[in] Device     - Device to be turn on or off
 * @param[in] On         - 1 to turn on, 0 to turn off
 *
 */
VOID
FchAoacPowerOnDev (
  IN  UINT8      Device,
  IN  UINT8      On
  )
{
  if (On == 1) {
    if ((ACPIMMIO8 (ACPI_MMIO_BASE + AOAC_BASE + 0x40 + (Device << 1)) & 3) != 0) {
      ACPIMMIO8 (ACPI_MMIO_BASE + AOAC_BASE + 0x40 + (Device << 1)) &= ~(UINT8) (BIT0 + BIT1);
      ACPIMMIO8 (ACPI_MMIO_BASE + AOAC_BASE + 0x40 + (Device << 1)) |= BIT3;
      do {
      } while ((ACPIMMIO8 (ACPI_MMIO_BASE + AOAC_BASE + 0x41 + (Device << 1)) & 7) != 7);
    }
  } else {
    if ((ACPIMMIO8 (ACPI_MMIO_BASE + AOAC_BASE + 0x40 + (Device << 1)) & 3) != 3) {
      ACPIMMIO8 (ACPI_MMIO_BASE + AOAC_BASE + 0x40 + (Device << 1)) &= ~(UINT8) (BIT3);
      do {
      } while ((ACPIMMIO8 (ACPI_MMIO_BASE + AOAC_BASE + 0x41 + (Device << 1)) & 7) != 0);
      if (Device != FCH_AOAC_ESPI) {  //PLAT-26858 
        ACPIMMIO8 (ACPI_MMIO_BASE + AOAC_BASE + 0x40 + (Device << 1)) |= BIT0 + BIT1;
      }
    }
  }
}


