/** @file
  Chipset Setup Configuration Data

;******************************************************************************
;* Copyright (c) 2015, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _CHIPSET_SMI_TABLE_DEFINE_H_
#define _CHIPSET_SMI_TABLE_DEFINE_H_

#define SW_SMI_PORT          0xB0

typedef enum { 
  SW_SMI_ASPM_OVERRIDE                  = 0x73,
  // Please refer FCHOEM_ENABLE_ACPI_SWSMI and FCHOEM_DISABLE_ACPI_SWSMI in platform.env, the both define is not used.
  FCH_FCHOEM_ENABLE_ACPI_SWSMI          = 0XA0,
  FCH_FCHOEM_DISABLE_ACPI_SWSMI         = 0xA1,
  // Please refer FCHOEM_START_TIMER_SMI and FCHOEM_STOP_TIMER_SMI in platform.env, the both define is not used.
  FCH_FCHOEM_START_TIMER_SMI            = 0xBC,
  FCH_FCHOEM_STOP_TIMER_SMI             = 0xBD,
  EFI_C1E_SW_SMI 	                      = 0xAC,
  ENABLE_S4_INTERNALKBCWAK              = 0xAD,  
  AGESA_MEMORY_INFO_TRANSLATE           = 0xAE,
  BIOS_ACPI_SW_SMI                      = 0xBE,
  CPU_SMI_IO_TRAP_SMI                   = 0xBF,
//
// OEM must use 0xc0~0xDF as SW_SMI command number.
// And define SW_SMI at OemSwSmi.h.
//
  ACPI_OEM_SW_SMI                       = 0xE4,  
  ACPI_SHOW_DEBUG_PORT                  = 0xE5,
  EFI_BRIGHTNESS_RESTORE_SW_SMI         = 0xE6,
  
  SPI_ROM_PROTECT_LOCK       = 0xE7,
  SPI_ROM_PROTECT_UNLOCK     = 0xE8,
  SPI_ROM_PROTECT_PROGRAM    = 0xE9,

  // FCH uses 0xD3, 0xD4 of Software SMI numbers, but the both numbers must be reserved for OEM.
  // So,assign three new numbers of Software SMI to both PCDs in package.dsc of ChipsetPkg to instead of PcdFchOemBeforePciRestoreSwSmi, 
  // PcdFchOemAfterPciRestoreSwSmi and AcpiDisableDgpuAudioSwSmi which are defined by AGESA . 
  // We don't use follow both defines. The both defines identify Software SMI numbers that have been used.
  FCH_FCHOEM_BEFORE_PCI_RESTORE_SWSMI   = 0xEA,
  FCH_FCHOEM_AFTER_PCI_RESTORE_SWSMI    = 0xEB,
  
  ACPI_DISABLE_DGPU_AUDIO_SW_SMI        = 0xED,
  
}CHIPSET_SW_SMI_PORT_TABLE;

#endif 

