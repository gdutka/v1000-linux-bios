/* $NoKeywords:$ */
/**
 * @file
 *
 * FCH platform definition
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
#ifndef  _FCH_PLATFORM_H_
#define  _FCH_PLATFORM_H_

#define MAX_SATA_PORTS 8

//<Embedded_Override_Start>
#define ESPI_SIO_SUPPORT 0
//<Embedded_Override_End>

//#include "Porting.h"
//#include "AMD.h"
#include "AGESA.h"

#ifndef FCHOEM_ACPI_RESTORE_SWSMI
  #define FCHOEM_BEFORE_PCI_RESTORE_SWSMI    0xD3
  #define FCHOEM_AFTER_PCI_RESTORE_SWSMI     0xD4
  #define FCHOEM_ENABLE_ACPI_SWSMI           0xA0
  #define FCHOEM_DISABLE_ACPI_SWSMI          0xA1
  #define FCHOEM_START_TIMER_SMI             0xBC
  #define FCHOEM_STOP_TIMER_SMI              0xBD
#endif

#ifndef FCHOEM_SPI_UNLOCK_SWSMI
  #define FCHOEM_SPI_UNLOCK_SWSMI            0xAA
#endif
#ifndef FCHOEM_SPI_LOCK_SWSMI
  #define FCHOEM_SPI_LOCK_SWSMI              0xAB
#endif

#ifndef FCHOEM_ACPI_TABLE_RANGE_LOW
  #define FCHOEM_ACPI_TABLE_RANGE_LOW        0xE0000ul
#endif

#ifndef FCHOEM_ACPI_TABLE_RANGE_HIGH
  #define FCHOEM_ACPI_TABLE_RANGE_HIGH       0xFFFF0ul
#endif

#ifndef FCHOEM_ACPI_BYTE_CHECHSUM
  #define FCHOEM_ACPI_BYTE_CHECHSUM          0x100
#endif

#ifndef FCHOEM_IO_DELAY_PORT
  #define FCHOEM_IO_DELAY_PORT               0x80
#endif

#ifndef FCHOEM_OUTPUT_DEBUG_PORT
  #define FCHOEM_OUTPUT_DEBUG_PORT           0x80
#endif

#define FCH_PCIRST_BASE_IO                   0xCF9
#define FCH_PCI_RESET_COMMAND06              0x06
#define FCH_PCI_RESET_COMMAND0E              0x0E
#define FCH_KBDRST_BASE_IO                   0x64
#define FCH_KBC_RESET_COMMAND                0xFE
#define FCH_ROMSIG_BASE_IO                   0x20000l
#define FCH_ROMSIG_SIGNATURE                 0x55AA55AAul
#define FCH_MAX_TIMER                        0xFFFFFFFFul
#define FCH_GEC_INTERNAL_REG                 0x6804
#define FCH_HPET_REG_MASK                    0xFFFFF800ul
#define FCH_FAKE_USB_BAR_ADDRESS             0x58830000ul


#ifndef FCHOEM_ELAPSED_TIME_UNIT
  #define FCHOEM_ELAPSED_TIME_UNIT           28
#endif

#ifndef FCHOEM_ELAPSED_TIME_DIVIDER
  #define FCHOEM_ELAPSED_TIME_DIVIDER        100
#endif

#include "IdsLib.h"
#include "Filecode.h"
#include "AmdBaseLib.h"
#include "Fch.h"
#include "SSFch.h"
#include "FchInterface.h"
#include "FchCommonCfg.h"
#include "FchRegistersSS.h"
#include "AcpiLib.h"
#include "FchDef.h"
#include "FchAoacLib.h"
#include <Library/IoLib.h>
#include <Library/PciLib.h>
#include <Library/BaseLib.h>

#endif // _FCH_PLATFORM_H_

