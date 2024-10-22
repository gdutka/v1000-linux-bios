/* $NoKeywords:$ */
/**
 * @file
 *
 * Generate Component Locality Distance Information Table (CDIT)
 *
 * Contains code that generate Component Locality Distance Information Table (CDIT)
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
#include "AmdAcpiCdit.h"
#include "Library/AmdBaseLib.h"
#include "Library/AmdHeapLib.h"
#include "Library/UefiBootServicesTableLib.h"
#include "Library/BaseLib.h"
#include <Protocol/AmdAcpiCditServicesProtocol.h>
#include "PiDxe.h"

#define FILECODE UNIVERSAL_ACPI_AMDACPICDIT_FILECODE

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
STATIC CDIT_HEADER  ROMDATA CditHeaderStruct =
{
  {'C','D','I','T'},
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
 *  Generate CDIT
 *
 *  Description:
 *    This function will generate a complete Component Locality Distance Information Table
 *    i.e. CDIT into a memory buffer.
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
AmdAcpiCdit (
  IN       EFI_ACPI_TABLE_PROTOCOL  *AcpiTableProtocol,
  IN       AMD_CONFIG_PARAMS        *StdHeader
  )
{
  UINT8               *Distance;
  UINT8                NumberOfDomains;
  UINTN                TableSize;
  UINTN                TableKey;
  CDIT_HEADER         *CditHeaderStructPtr;
  ALLOCATE_HEAP_PARAMS AllocParams;
  AMD_FABRIC_ACPI_CDIT_SERVICES_PROTOCOL *FabricCditServices;

  // Locate protocol
  if (gBS->LocateProtocol (&gAmdFabricAcpiCditServicesProtocolGuid, NULL, (VOID **) &FabricCditServices) != EFI_SUCCESS) {
    return AGESA_ERROR;
  }

  // Allocate a buffer
  AllocParams.RequestedBufferSize = ACPI_TABLE_MAX_LENGTH;
  AllocParams.BufferHandle = AMD_ACPI_TABLE_BUFFER_HANDLE;
  AllocParams.Persist = HEAP_SYSTEM_MEM;


  if (HeapAllocateBuffer (&AllocParams, StdHeader) != AGESA_SUCCESS) {
    return AGESA_ERROR;
  }

  CditHeaderStructPtr = (CDIT_HEADER *) AllocParams.BufferPtr;
  Distance = (UINT8 *) CditHeaderStructPtr + sizeof (CDIT_HEADER);

  // Copy CditHeaderStruct -> data buffer
  LibAmdMemCopy ((VOID *) CditHeaderStructPtr, (VOID *) &CditHeaderStruct, (UINTN) (sizeof (CDIT_HEADER)), StdHeader);
  // Update table OEM fields.
  ASSERT (AsciiStrnLenS ((CHAR8 *)PcdGetPtr (PcdAmdAcpiTableHeaderOemId), 100) <= 6);
  ASSERT (AsciiStrnLenS ((CHAR8 *)PcdGetPtr (PcdAmdAcpiCditTableHeaderOemTableId), 100) <= 8);

  LibAmdMemCopy ((VOID *) &CditHeaderStructPtr->OemId,
                 (VOID *) PcdGetPtr (PcdAmdAcpiTableHeaderOemId),
                 AsciiStrnLenS ((CHAR8 *)PcdGetPtr (PcdAmdAcpiTableHeaderOemId), 6),
                 StdHeader);
  LibAmdMemCopy ((VOID *) &CditHeaderStructPtr->OemTableId,
                 (VOID *) PcdGetPtr (PcdAmdAcpiCditTableHeaderOemTableId),
                 AsciiStrnLenS ((CHAR8 *)PcdGetPtr (PcdAmdAcpiCditTableHeaderOemTableId), 8),
                 StdHeader);

  // Get CDID from gAmdFabricAcpiCditServicesProtocolGuid
  FabricCditServices->GetCditDistanceInfo (FabricCditServices, &NumberOfDomains, Distance);

  // Update number of system localities
  CditHeaderStructPtr->NumDomains = (UINT64) NumberOfDomains;

  // Store size in table (current buffer offset - buffer start offset)
  CditHeaderStructPtr->Length = sizeof (CDIT_HEADER) + (UINT32) (NumberOfDomains * NumberOfDomains);


  // Boundary check
  ASSERT (CditHeaderStructPtr->Length <= ACPI_TABLE_MAX_LENGTH);
  if (CditHeaderStructPtr->Length > ACPI_TABLE_MAX_LENGTH) {
    IDS_HDT_CONSOLE (CPU_TRACE, "ERROR: ACPI table buffer is overflow\n");
    IDS_DEADLOOP ()
  }

  // Update SSDT header Checksum
  ChecksumAcpiTable ((ACPI_TABLE_HEADER *) CditHeaderStructPtr, StdHeader);

  // Publish SSDT
  TableSize = CditHeaderStructPtr->Length;
  TableKey = 0;
  AcpiTableProtocol->InstallAcpiTable (
                  AcpiTableProtocol,
                  CditHeaderStructPtr,
                  TableSize,
                  &TableKey
                  );

  // Deallocate heap
  HeapDeallocateBuffer (AMD_ACPI_TABLE_BUFFER_HANDLE, StdHeader);

  IDS_HDT_CONSOLE (MAIN_FLOW, "  CDIT is created\n");
  return AGESA_SUCCESS;
}

/*----------------------------------------------------------------------------------------
 *                          L O C A L    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */


