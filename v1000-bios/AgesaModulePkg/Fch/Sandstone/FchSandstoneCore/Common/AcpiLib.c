/* $NoKeywords:$ */
/**
 * @file
 *
 * FCH ACPI lib
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
#include "FchPlatform.h"
#define FILECODE PROC_FCH_COMMON_ACPILIB_FILECODE
//
//
// Routine Description:
//
//  Locate ACPI table
//
// Arguments:
//
//  Signature - table signature
//
//Returns:
//
//  pointer to ACPI table
//
//
VOID*
AcpiLocateTable (
  IN  UINT32     Signature
  )
{
  UINT32              Index;
  UINT32              *RsdPtr;
  UINT32              *Rsdt;
  UINTN               TableOffset;
  DESCRIPTION_HEADER  *CurrentTable;

  RsdPtr = (UINT32*) (UINTN) FCHOEM_ACPI_TABLE_RANGE_LOW;
  Rsdt = NULL;
  do {
//[-start-240426-IB18160212-modify-]//
    if ( *RsdPtr == SIGNATURE_32 ('R', 'S', 'D', ' ') && *(RsdPtr + 1) == SIGNATURE_32 ('P', 'T', 'R', ' ')) {
//[-start-240426-IB18160212-modify-]//
      Rsdt = (UINT32*) (UINTN) ((RSDP_HEADER*)RsdPtr)->RsdtAddress;
      break;
    }
    RsdPtr += 4;
  } while ( RsdPtr <= (UINT32*) (UINTN) FCHOEM_ACPI_TABLE_RANGE_HIGH );

  if ( Rsdt != NULL && AcpiGetTableCheckSum (Rsdt) == 0 ) {
    for ( Index = 0; Index < (((DESCRIPTION_HEADER*)Rsdt)->Length - sizeof (DESCRIPTION_HEADER)) / 4; Index++ ) {
      TableOffset = *(UINTN*) ((UINT8*)Rsdt + sizeof (DESCRIPTION_HEADER) + Index * 4);
      CurrentTable = (DESCRIPTION_HEADER*)TableOffset;
      if ( CurrentTable->Signature == Signature ) {
        return CurrentTable;
      }
    }
  }
  return NULL;
}

//
//
//  Routine Description:
//
//  Update table CheckSum
//
//  Arguments:
//
//  TablePtr - table pointer
//
//  Returns:
//
//  none
//
//
VOID
AcpiSetTableCheckSum (
  IN  VOID     *TablePtr
  )
{
  UINT8   CheckSum;

  CheckSum = 0;
  ((DESCRIPTION_HEADER*)TablePtr)->CheckSum = 0;
  CheckSum = AcpiGetTableCheckSum (TablePtr);
  ((DESCRIPTION_HEADER*)TablePtr)->CheckSum = (UINT8) (FCHOEM_ACPI_BYTE_CHECHSUM - CheckSum);
}

//
//
//  Routine Description:
//
//  Get table CheckSum - Get ACPI table checksum
//
//  Arguments:
//
//  TablePtr - table pointer
//
//  Returns:
//
//  none
//
//
UINT8
AcpiGetTableCheckSum (
  IN  VOID     *TablePtr
  )
{
  return GetByteSum (TablePtr, ((DESCRIPTION_HEADER*)TablePtr)->Length);
}


//
//
//  Routine Description:
//
//  GetByteSum - Get BYTE checksum value
//
//  Arguments:
//
//  DataPtr - table pointer
//  Length  - table length
//
//  Returns:
//
//  CheckSum - CheckSum value
//
//
UINT8
GetByteSum (
  IN  VOID       *DataPtr,
  IN  UINT32     Length
  )
{
  UINT32   Index;
  UINT8    CheckSum;

  CheckSum = 0;
  for ( Index = 0; Index < Length; Index++ ) {
    CheckSum = CheckSum + (*((UINT8*)DataPtr + Index));
  }
  return CheckSum;
}

//
//
//  Routine Description:
//
//  GetFchAcpiMmioBase - Get FCH HwAcpi MMIO Base Address
//
//  Arguments:
//
//  AcpiMmioBase - HwAcpi MMIO Base Address
//  StdHeader    - Amd Stand Header
//
//  Returns:
//
//  AcpiMmioBase - HwAcpi MMIO Base Address
//
//
VOID
GetFchAcpiMmioBase (
     OUT   UINT32                *AcpiMmioBase,
  IN       AMD_CONFIG_PARAMS     *StdHeader
  )
{
  UINT32    AcpiMmioBaseAddressDword;

  ReadPmio (FCH_PMIOA_REG24 + 2, AccessWidth16, &AcpiMmioBaseAddressDword, StdHeader);
  *AcpiMmioBase = AcpiMmioBaseAddressDword << 16;
}

//
//
//  Routine Description:
//
//  GetFchAcpiPmBase - Get FCH HwAcpi PM Base Address
//
//  Arguments:
//
//  AcpiPmBase   - HwAcpi PM Base Address
//  StdHeader    - Amd Stand Header
//
//  Returns:
//
//  AcpiPmBase   - HwAcpi PM Base Address
//
//
VOID
GetFchAcpiPmBase (
     OUT   UINT16                *AcpiPmBase,
  IN       AMD_CONFIG_PARAMS     *StdHeader
  )
{
  ReadPmio (FCH_PMIOA_REG60, AccessWidth16, AcpiPmBase, StdHeader);
}


UINT8
ReadFchSleepType (
  IN       AMD_CONFIG_PARAMS     *StdHeader
  )
{
  UINT16     Value16;
  ReadPmio (FCH_PMIOA_REG62, AccessWidth16, &Value16, StdHeader);
  LibAmdIoRead (AccessWidth16, Value16, &Value16, StdHeader);
  return (UINT8) ((Value16 >> 10) & 7);
}


