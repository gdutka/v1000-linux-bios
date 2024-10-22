/* $NoKeywords:$ */
/**
 * @file
 *
 * FCH Base lib
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
* Copyright 2008 - 2019 ADVANCED MICRO DEVICES, INC.  All Rights Reserved.
*
* AMD is granting you permission to use this software and documentation (if
* any) (collectively, the "Materials") pursuant to the terms and conditions of
* the Software License Agreement included with the Materials.  If you do not
* have a copy of the Software License Agreement, contact your AMD
* representative for a copy.
*
* You agree that you will not reverse engineer or decompile the Materials, in
* whole or in part, except as allowed by applicable law.
*
* WARRANTY DISCLAIMER:  THE MATERIALS ARE PROVIDED "AS IS" WITHOUT WARRANTY OF
* ANY KIND.  AMD DISCLAIMS ALL WARRANTIES, EXPRESS, IMPLIED, OR STATUTORY,
* INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE, TITLE, NON-INFRINGEMENT, THAT THE
* MATERIALS WILL RUN UNINTERRUPTED OR ERROR-FREE OR WARRANTIES ARISING FROM
* CUSTOM OF TRADE OR COURSE OF USAGE.  THE ENTIRE RISK ASSOCIATED WITH THE USE
* OF THE MATERIAL IS ASSUMED BY YOU.  Some jurisdictions do not allow the
* exclusion of implied warranties, so the above exclusion may not apply to
* You.
*
* LIMITATION OF LIABILITY AND INDEMNIFICATION:  AMD AND ITS LICENSORS WILL
* NOT, UNDER ANY CIRCUMSTANCES BE LIABLE TO YOU FOR ANY PUNITIVE, DIRECT,
* INCIDENTAL, INDIRECT, SPECIAL OR CONSEQUENTIAL DAMAGES ARISING FROM USE OF
* THE MATERIALS OR THIS AGREEMENT EVEN IF AMD AND ITS LICENSORS HAVE BEEN
* ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.  In no event shall AMD's total
* liability to You for all damages, losses, and causes of action (whether in
* contract, tort (including negligence) or otherwise) exceed the amount of
* $100 USD. You agree to defend, indemnify and hold harmless AMD and its
* licensors, and any of their directors, officers, employees, affiliates or
* agents from and against any and all loss, damage, liability and other
* expenses (including reasonable attorneys' fees), resulting from Your use of
* the Materials or violation of the terms and conditions of this Agreement.
*
* U.S. GOVERNMENT RESTRICTED RIGHTS:  The Materials are provided with
* "RESTRICTED RIGHTS." Use, duplication, or disclosure by the Government is
* subject to the restrictions as set forth in FAR 52.227-14 and
* DFAR252.227-7013, et seq., or its successor.  Use of the Materials by the
* Government constitutes acknowledgment of AMD's proprietary rights in them.
*
* EXPORT RESTRICTIONS: The Materials may be subject to export restrictions as
* stated in the Software License Agreement.
*******************************************************************************
*/

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include <Filecode.h>
#include <Library/AmdBaseLib.h>
#include <Library/FchBaseLib.h>
#include "FchRegistersCommon.h"

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
#define FILECODE LIBRARY_FCHBASELIB_FCHSMILIB_FILECODE
#define BIOS_RAM_MMIO_BASE               0xFED10000ul


/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
 /*----------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------*/
/**
 * FchRecordSmiStatus - Record SMI Status
 *
 *
 * @param[in] StdHeader
 *
 */
VOID
FchRecordSmiStatus (
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{
  UINTN   Index;
  UINT8   SwSmiValue;

  //ACPIMMIO8 (0xfed80304) |= 0x01;
  ACPIMMIO8 (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REG04) |= 0x01;
  for ( Index = 0; Index < 20; Index++ ) {
    //ACPIMMIO8 (0xfed10020 + Index) = ACPIMMIO8 (0xfed80280 + Index);
    ACPIMMIO8 (BIOS_RAM_MMIO_BASE + 0x20 + Index) = ACPIMMIO8 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REG80 + Index);
  }
  LibAmdIoRead (AccessWidth8, 0xB0, &SwSmiValue, StdHeader);
  //ACPIMMIO8 (0xfed10040) = SwSmiValue;
  ACPIMMIO8 (BIOS_RAM_MMIO_BASE + 0x40) = SwSmiValue;
}

/*----------------------------------------------------------------------------------------*/
/**
 * FchClearAllSmiStatus - Clear All SMI Status
 *
 *
 * @param[in] StdHeader
 *
 */
VOID
FchClearAllSmiStatus (
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{
  UINTN   Index;

  for ( Index = 0; Index < 20; Index++ ) {
    //ACPIMMIO8 (0xfed80280 + Index) |= 0;
    ACPIMMIO8 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REG80 + Index) |= 0;
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * FchSleepTrapControl - FCH Sleep Trap Control
 *
 *
 *
 * @param[in] SleepTrap    - Whether sleep trap is enabled
 *
 */
VOID
FchSleepTrapControl (
  IN        BOOLEAN          SleepTrap
  )
{
  if (SleepTrap) {
    ACPIMMIO32 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REGB0) &= ~(BIT2 + BIT3);
    ACPIMMIO32 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REGB0) |= BIT2;

    ACPIMMIO8 (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REGBE) &= ~ (BIT5);
    ACPIMMIO8 (ACPI_MMIO_BASE + PMIO_BASE + 0xB) &= ~ (BIT0 + BIT1);
    ACPIMMIO8 (ACPI_MMIO_BASE + PMIO_BASE + 0xB) |= BIT1;
  } else {
    ACPIMMIO8 (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REGBE) |= BIT5;
    ACPIMMIO8 (ACPI_MMIO_BASE + PMIO_BASE + 0xB) &= ~ (BIT0 + BIT1);
    ACPIMMIO8 (ACPI_MMIO_BASE + PMIO_BASE + 0xB) |= BIT0;

    ACPIMMIO32 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REGB0) &= ~(BIT2 + BIT3);
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * FchSmiGenerateControl - FCH SMI Enablement
 *
 *
 *
 * @param[in] SmiControl  - Whether enable or disable SMI function
 *
 */
VOID
FchSmiGenerateControl (
  IN        BOOLEAN          SmiControl
  )
{
  if (SmiControl) {
    ACPIMMIO32 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REG98) &= ~(BIT31);
  } else {
    ACPIMMIO32 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REG98) |= BIT31;
  }
}

/*----------------------------------------------------------------------------------------*/ 
/**
 * FchDisablePowerButtonSmi - Config Fch during ACPI_ON
 *
 *
 * @param[in] StdHeader - Standard configuration header
 *
 */
VOID
FchDisablePowerButtonSmi (
  IN  AMD_CONFIG_PARAMS     *StdHeader
  )
{
  UINT8    Or8;
  UINT8    Mask8;
  //
  // Disable Power Button SMI
  //
  //RwMem (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REGAC, AccessWidth8, ~(UINT32) (BIT6), 0);
  Or8 = 0;
  Mask8 = BIT6;
  LibAmdMemRMW (AccessWidth8, (UINT64) (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REGAC), &Or8, &Mask8, StdHeader);
}


