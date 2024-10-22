/* $NoKeywords:$ */
/**
 * @file
 *
 * Generate System Locality Distance Information Table (SLIT)
 *
 * Contains code that generate System Locality Distance Information Table (SLIT)
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
#include "AmdAcpiSlit.h"
#include "Library/AmdBaseLib.h"
#include "Library/AmdHeapLib.h"
#include "Library/UefiBootServicesTableLib.h"
#include "Library/BaseLib.h"
#include <Protocol/AmdAcpiSlitServicesProtocol.h>
#include "PiDxe.h"

#define FILECODE UNIVERSAL_ACPI_AMDACPISLIT_FILECODE

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
/*----------------------------------------------------------------------------
 *  All of the DATA should be defined in _CODE segment.
 *  Use ROMDATA to specify that it belongs to _CODE.
 *----------------------------------------------------------------------------
 */
STATIC SLIT_HEADER  ROMDATA SlitHeaderStruct =
{
  {'S','L','I','T'},
  0,
  1,
  0,
  {0},
  {0},
  1,
  {'A','M','D',' '},
  1,
  1
};


/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/**
 *---------------------------------------------------------------------------------------
 *
 *  Generate SLIT
 *
 *  Description:
 *    This function will generate a complete System Locality Distance Information Table
 *    i.e. SLIT into a memory buffer.
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
AmdAcpiSlit (
  IN       EFI_ACPI_TABLE_PROTOCOL  *AcpiTableProtocol,
  IN       AMD_CONFIG_PARAMS        *StdHeader
  )
{
  UINT8               *Distance;
  UINT8                NumberOfDomains;
  UINTN                TableSize;
  UINTN                TableKey;
  EFI_STATUS           Status;
  SLIT_HEADER         *SlitHeaderStructPtr;
  ALLOCATE_HEAP_PARAMS AllocParams;
  AMD_FABRIC_ACPI_SLIT_SERVICES_PROTOCOL *FabricSlitServices;

  // Locate protocol
  if (gBS->LocateProtocol (&gAmdFabricAcpiSlitServicesProtocolGuid, NULL, (VOID **) &FabricSlitServices) != EFI_SUCCESS) {
    return AGESA_ERROR;
  }

  // Allocate a buffer
  AllocParams.RequestedBufferSize = ACPI_TABLE_MAX_LENGTH;
  AllocParams.BufferHandle = AMD_ACPI_TABLE_BUFFER_HANDLE;
  AllocParams.Persist = HEAP_SYSTEM_MEM;

  if (HeapAllocateBuffer (&AllocParams, StdHeader) != AGESA_SUCCESS) {
    return AGESA_ERROR;
  }

  SlitHeaderStructPtr = (SLIT_HEADER *) AllocParams.BufferPtr;
  Distance = (UINT8 *) SlitHeaderStructPtr + sizeof (SLIT_HEADER);

  // Get SLIT from gAmdFabricAcpiSlitServicesProtocolGuid
  Status = FabricSlitServices->GetSlitDistanceInfo (FabricSlitServices, &NumberOfDomains, Distance);

  // Only create SLIT when NumberOfDomains > 1
  if ((NumberOfDomains > 1) && (Status != EFI_ABORTED)) {
    // Copy SlitHeaderStruct -> data buffer
    LibAmdMemCopy ((VOID *) SlitHeaderStructPtr, (VOID *) &SlitHeaderStruct, (UINTN) (sizeof (SLIT_HEADER)), StdHeader);
    // Update table OEM fields.
    ASSERT (AsciiStrnLenS ((CHAR8 *)PcdGetPtr (PcdAmdAcpiTableHeaderOemId), 100) <= 6);
    ASSERT (AsciiStrnLenS ((CHAR8 *)PcdGetPtr (PcdAmdAcpiSlitTableHeaderOemTableId), 100) <= 8);

    LibAmdMemCopy ((VOID *) &SlitHeaderStructPtr->OemId,
                   (VOID *) PcdGetPtr (PcdAmdAcpiTableHeaderOemId),
                   AsciiStrnLenS ((CHAR8 *)PcdGetPtr (PcdAmdAcpiTableHeaderOemId), 6),
                   StdHeader);
    LibAmdMemCopy ((VOID *) &SlitHeaderStructPtr->OemTableId,
                   (VOID *) PcdGetPtr (PcdAmdAcpiSlitTableHeaderOemTableId),
                   AsciiStrnLenS ((CHAR8 *)PcdGetPtr (PcdAmdAcpiSlitTableHeaderOemTableId), 8),
                   StdHeader);

    // Update number of system localities
    SlitHeaderStructPtr->NumberOfSystemLocalities = (UINT64) NumberOfDomains;

    // Store size in table (current buffer offset - buffer start offset)
    SlitHeaderStructPtr->Length = sizeof (SLIT_HEADER) + (UINT32) (NumberOfDomains * NumberOfDomains);

    // Boundary check
    ASSERT (SlitHeaderStructPtr->Length <= ACPI_TABLE_MAX_LENGTH);
    if (SlitHeaderStructPtr->Length > ACPI_TABLE_MAX_LENGTH) {
      IDS_HDT_CONSOLE (CPU_TRACE, "ERROR: ACPI table buffer is overflow\n");
      IDS_DEADLOOP ()
    }

    // Update SLIT header Checksum
    ChecksumAcpiTable ((ACPI_TABLE_HEADER *) SlitHeaderStructPtr, StdHeader);

    // Publish SLIT
    TableSize = SlitHeaderStructPtr->Length;
    TableKey = 0;
    AcpiTableProtocol->InstallAcpiTable (
                    AcpiTableProtocol,
                    SlitHeaderStructPtr,
                    TableSize,
                    &TableKey
                    );
    IDS_HDT_CONSOLE (MAIN_FLOW, "  SLIT is created\n");
  }

  // Deallocate heap
  HeapDeallocateBuffer (AMD_ACPI_TABLE_BUFFER_HANDLE, StdHeader);

  return AGESA_SUCCESS;
}

/*----------------------------------------------------------------------------------------
 *                          L O C A L    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */


