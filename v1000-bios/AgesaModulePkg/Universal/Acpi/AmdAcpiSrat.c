/* $NoKeywords:$ */
/**
 * @file
 *
 * Generate System Resource Affinity Table (SRAT)
 *
 * Contains code that generate System Resource Affinity Table (SRAT)
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Universal
 * @e \$Revision$   @e \$Date$
 *
 */
/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */
#include "AGESA.h"
#include "Filecode.h"
#include "AmdAcpiDxe.h"
#include "AmdAcpiSrat.h"
#include "Library/AmdBaseLib.h"
#include "Library/AmdHeapLib.h"
#include "Library/UefiBootServicesTableLib.h"
#include "Library/BaseLib.h"
#include "Protocol/AmdAcpiSratServicesProtocol.h"
#include "PiDxe.h"

#define FILECODE UNIVERSAL_ACPI_AMDACPISRAT_FILECODE

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */
EFI_STATUS
MakeProcLocalApicStruct (
  IN       SRAT_HEADER       *SratHeaderStructPtr,
  IN OUT   UINT8            **TableEnd
  );

EFI_STATUS
MakeMemoryStruct (
  IN       SRAT_HEADER       *SratHeaderStructPtr,
  IN OUT   UINT8            **TableEnd
  );

EFI_STATUS
MakeProcX2LocalApicStruct (
  IN       SRAT_HEADER       *SratHeaderStructPtr,
  IN OUT   UINT8            **TableEnd
  );

EFI_STATUS
MakeGiccStruct (
  IN       SRAT_HEADER       *SratHeaderStructPtr,
  IN OUT   UINT8            **TableEnd
  );

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
/*----------------------------------------------------------------------------
 *  All of the DATA should be defined in _CODE segment.
 *  Use ROMDATA to specify that it belongs to _CODE.
 *----------------------------------------------------------------------------
 */

// Version 3 (ACPI 6.0)
STATIC SRAT_HEADER  ROMDATA SratHeaderStruct =
{
  {'S','R','A','T'},
  0,
  3,
  0,
  {0},
  {0},
  1,
  {'A','M','D',' '},
  1,
  1,
  {0}
};

/// All entries that SRAT needs
CONST S_MAKE_SRAT_STRUCT ROMDATA MakeSratStructTable[] =
{
  {MakeProcLocalApicStruct},
  {MakeMemoryStruct},
  {MakeProcX2LocalApicStruct},
  {MakeGiccStruct},
  {NULL}
};

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/**
 *---------------------------------------------------------------------------------------
 *
 *  Generate SRAT
 *
 *  Description:
 *    This function will generate a complete System Resource Affinity Table
 *    i.e. SRAT into a memory buffer.
 *
 *  Parameters:
 *    @param[in]       AcpiTableProtocol          Pointer to gEfiAcpiTableProtocolGuid protocol
 *    @param[in]       StdHeader                  Header for library and services
 *
 *    @retval          AGESA_STATUS
 *
 *---------------------------------------------------------------------------------------
 */
AGESA_STATUS
AmdAcpiSrat (
  IN       EFI_ACPI_TABLE_PROTOCOL  *AcpiTableProtocol,
  IN       AMD_CONFIG_PARAMS        *StdHeader
  )
{
  UINT8                i;
  UINT8               *TableEnd;
  UINTN                TableSize;
  UINTN                TableKey;
  EFI_STATUS           Status;
  SRAT_HEADER         *SratHeaderStructPtr;
  ALLOCATE_HEAP_PARAMS AllocParams;

  // Allocate a buffer
  AllocParams.RequestedBufferSize = ACPI_TABLE_MAX_LENGTH;
  AllocParams.BufferHandle = AMD_ACPI_TABLE_BUFFER_HANDLE;
  AllocParams.Persist = HEAP_SYSTEM_MEM;


  if (HeapAllocateBuffer (&AllocParams, StdHeader) != AGESA_SUCCESS) {
    return AGESA_ERROR;
  }

  SratHeaderStructPtr = (SRAT_HEADER *) AllocParams.BufferPtr;
  TableEnd = (UINT8 *) SratHeaderStructPtr + sizeof (SRAT_HEADER);

  // Copy SratHeaderStruct -> data buffer
  LibAmdMemCopy ((VOID *) SratHeaderStructPtr, (VOID *) &SratHeaderStruct, (UINTN) (sizeof (SRAT_HEADER)), StdHeader);
  // Update table OEM fields.
  ASSERT (AsciiStrnLenS ((CHAR8 *)PcdGetPtr (PcdAmdAcpiTableHeaderOemId), 100) <= 6);
  ASSERT (AsciiStrnLenS ((CHAR8 *)PcdGetPtr (PcdAmdAcpiSratTableHeaderOemTableId), 100) <= 8);

  LibAmdMemCopy ((VOID *) &SratHeaderStructPtr->OemId,
                 (VOID *) PcdGetPtr (PcdAmdAcpiTableHeaderOemId),
                 AsciiStrnLenS ((CHAR8 *)PcdGetPtr (PcdAmdAcpiTableHeaderOemId), 6),
                 StdHeader);
  LibAmdMemCopy ((VOID *) &SratHeaderStructPtr->OemTableId,
                 (VOID *) PcdGetPtr (PcdAmdAcpiSratTableHeaderOemTableId),
                 AsciiStrnLenS ((CHAR8 *)PcdGetPtr (PcdAmdAcpiSratTableHeaderOemTableId), 8),
                 StdHeader);

  // Add all SRAT entries.
  for (i = 0; MakeSratStructTable[i].MakeSratStruct != NULL; i++) {
    Status = MakeSratStructTable[i].MakeSratStruct (SratHeaderStructPtr, &TableEnd);
    if (Status == EFI_ABORTED) {
      // Deallocate heap
      IDS_HDT_CONSOLE (MAIN_FLOW, "  SRAT is NOT created since MakeSratStructTable[%d] returns EFI_ABORTED\n", i);
      HeapDeallocateBuffer (AMD_ACPI_TABLE_BUFFER_HANDLE, StdHeader);
      return AGESA_UNSUPPORTED;
    }
  }

  // Store size in table (current buffer offset - buffer start offset)
  SratHeaderStructPtr->Length = (UINT32) (TableEnd - (UINT8 *) SratHeaderStructPtr);

  // Boundary check
  ASSERT (SratHeaderStructPtr->Length <= ACPI_TABLE_MAX_LENGTH);
  if (SratHeaderStructPtr->Length > ACPI_TABLE_MAX_LENGTH) {
    IDS_HDT_CONSOLE (CPU_TRACE, "ERROR: ACPI table buffer is overflow\n");
    IDS_DEADLOOP ()
  }

  // Update SRAT header Checksum
  ChecksumAcpiTable ((ACPI_TABLE_HEADER *) SratHeaderStructPtr, StdHeader);

  // Publish SRAT
  TableSize = SratHeaderStructPtr->Length;
  TableKey = 0;
  AcpiTableProtocol->InstallAcpiTable (
                  AcpiTableProtocol,
                  SratHeaderStructPtr,
                  TableSize,
                  &TableKey
                  );

  // Deallocate heap
  HeapDeallocateBuffer (AMD_ACPI_TABLE_BUFFER_HANDLE, StdHeader);

  IDS_HDT_CONSOLE (MAIN_FLOW, "  SRAT is created\n");
  return AGESA_SUCCESS;
}

/*----------------------------------------------------------------------------------------
 *                          L O C A L    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

EFI_STATUS
MakeProcLocalApicStruct (
  IN       SRAT_HEADER       *SratHeaderStructPtr,
  IN OUT   UINT8            **TableEnd
  )
{
  EFI_STATUS Status;
  AMD_CCX_ACPI_SRAT_SERVICES_PROTOCOL *SratLocalApic;

  Status = EFI_ABORTED;
  if (gBS->LocateProtocol (&gAmdCcxAcpiSratServicesProtocolGuid, NULL, (VOID **) &SratLocalApic) == EFI_SUCCESS) {
    Status = SratLocalApic->CreateLapic (SratLocalApic, SratHeaderStructPtr, TableEnd);
  }

  return Status;
}

EFI_STATUS
MakeMemoryStruct (
  IN       SRAT_HEADER       *SratHeaderStructPtr,
  IN OUT   UINT8            **TableEnd
  )
{
  EFI_STATUS Status;
  AMD_FABRIC_ACPI_SRAT_SERVICES_PROTOCOL *SratMemory;

  Status = EFI_ABORTED;
  if (gBS->LocateProtocol (&gAmdFabricAcpiSratServicesProtocolGuid, NULL, (VOID **) &SratMemory) == EFI_SUCCESS) {
    Status = SratMemory->CreateMemory (SratMemory, SratHeaderStructPtr, TableEnd);
  }

  return Status;
}

EFI_STATUS
MakeProcX2LocalApicStruct (
  IN       SRAT_HEADER       *SratHeaderStructPtr,
  IN OUT   UINT8            **TableEnd
  )
{
  EFI_STATUS Status;
  AMD_CCX_ACPI_SRAT_SERVICES_PROTOCOL *SratX2Apic;

  Status = EFI_ABORTED;
  if (gBS->LocateProtocol (&gAmdCcxAcpiSratServicesProtocolGuid, NULL, (VOID **) &SratX2Apic) == EFI_SUCCESS) {
    Status = SratX2Apic->CreateX2Apic (SratX2Apic, SratHeaderStructPtr, TableEnd);
  }

  return Status;
}

EFI_STATUS
MakeGiccStruct (
  IN       SRAT_HEADER       *SratHeaderStructPtr,
  IN OUT   UINT8            **TableEnd
  )
{
  return EFI_SUCCESS;
}


