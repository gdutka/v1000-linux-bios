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
* Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
*
*******************************************************************************
*/

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include <Library/AmdBaseLib.h>
#include <Library/FchBaseLib.h>
#include <Filecode.h>
#include "FchRegistersCommon.h"

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
#define FILECODE LIBRARY_FCHBASELIB_FCHACPILIB_FILECODE
#define TEMP_ACPI_PM_CNT     0x404


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
/**
 * FchGetAcpiPmEvtBlk - Get the PM event block base address
 *
 *
 *
 * @param[out] AcpiPmEvtBlkBase    - the base address of PM Event Block
 *
 */
VOID
FchGetAcpiPmEvtBlk (
     OUT   UINT16              *AcpiPmEvtBlkBase
  )
{
  FchReadPmio (FCH_PMIOA_REG60, AccessWidth16, AcpiPmEvtBlkBase);
}

/*----------------------------------------------------------------------------------------*/
/**
 * FchReadSleepType - Get Sleep Type
 *
 *
 */
UINT8
FchReadSleepType (
  VOID
  )
{
  UINT16     AcpiPmCntPort;
  UINT16     Value16;
  AMD_CONFIG_PARAMS  *StdHeader;

  StdHeader = NULL;
  FchReadPmio (FCH_PMIOA_REG62, AccessWidth16, &AcpiPmCntPort);

  if ((AcpiPmCntPort == 0) || (AcpiPmCntPort == 0xFFFF)) {
    return AMD_ACPI_UNDEFINED;
  }

  LibAmdIoRead (AccessWidth16, AcpiPmCntPort, &Value16, StdHeader);
  return (UINT8) ((Value16 >> 10) & 7);
}

/*----------------------------------------------------------------------------------------*/
/**
 * FchWriteSleepType - Set Sleep Type
 *
 *
 *
 * @param[in] SleepType    - the sleep type to set
 *
 */
VOID
FchWriteSleepType (
  IN       UINT16  SleepType
  )
{
  UINT16     AcpiPmCntPort;
  UINT16     Value16;
  AMD_CONFIG_PARAMS  *StdHeader;

  if (SleepType > AMD_ACPI_S5) {
    return;
  }

  StdHeader = NULL;
  FchReadPmio (FCH_PMIOA_REG62, AccessWidth16, &AcpiPmCntPort);

  if ((AcpiPmCntPort == 0) || (AcpiPmCntPort == 0xFFFF)) {
    return;
  }

  LibAmdIoRead (AccessWidth16, AcpiPmCntPort, &Value16, StdHeader);
  Value16 &= ~(BIT12 + BIT11 + BIT10);
  Value16 |= (UINT16) (SleepType << 10);
  LibAmdIoWrite (AccessWidth16, AcpiPmCntPort, &Value16, StdHeader);
}

/*----------------------------------------------------------------------------------------*/
/**
 * FchSetFchAcpiMmioTbl - Program FCH ACPI MMIO register by table (8 bits data)
 *
 *
 *
 * @param[in] pAcpiTbl   - Table data pointer
 * @param[in] StdHeader
 *
 */
VOID
FchSetFchAcpiMmioTbl (
  IN       ACPI_REG8_MASK      *pAcpiTbl,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{
  UINT8   i;
  UINT8   Or8;
  UINT8   Mask8;
  UINT32  ddtempVar;

  if (pAcpiTbl != NULL) {
    if ((pAcpiTbl->MmioReg == 0) && (pAcpiTbl->MmioBase == 0) && (pAcpiTbl->DataAndMask == 0xB0) && (pAcpiTbl->DataOrMask == 0xAC)) {
      // Signature Checking
      pAcpiTbl++;
      for ( i = 1; pAcpiTbl->MmioBase < 0x1D; i++ ) {
        ddtempVar = ACPI_MMIO_BASE | (pAcpiTbl->MmioBase) << 8 | pAcpiTbl->MmioReg;
        Or8 = pAcpiTbl->DataOrMask;
        Mask8 = ~pAcpiTbl->DataAndMask;
        LibAmdMemRMW (AccessWidth8, (UINT64) ddtempVar, &Or8, &Mask8, StdHeader);
        pAcpiTbl++;
      }
    }
  }
}


