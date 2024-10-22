/* $NoKeywords:$ */
/**
 * @file
 *
 * AmdNbioALibRVDxe Driver
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  AmdNbioALibRVDxe
 * @e \$Revision: 312065 $   @e \$Date: 2015-01-30 04:23:05 -0600 (Fri, 30 Jan 2015) $
 *
 */
/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */
#include <Filecode.h>
#include <PiDxe.h>
#include <Library/AmdBaseLib.h>
#include <Library/IdsLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/AmdNbioBaseServicesProtocol.h>
#include <Library/UefiLib.h>
#include <Protocol/AcpiTable.h>
#include <Library/NbioIommuIvrsLib.h>
#include <Protocol/AmdNbioAlibServicesProtocol.h>
#include <AlibSsdtRV.h>
#include "AmdNbioAlibCallback.h"

#define FILECODE NBIO_NBIOALIB_AMDNBIOALIBRVDXE_AMDNBIOALIBRVDXE_FILECODE

/*----------------------------------------------------------------------------------------
 *                         E X T E R N   D E F I N I T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */
VOID
AmdGetAlibAddress (
  IN       DXE_AMD_NBIO_ALIB_SERVICES_PROTOCOL      *This,
     OUT   VOID                                     **AlibAddressOutput
  )
{
  *AlibAddressOutput = &AlibSsdtRV;
}

DXE_AMD_NBIO_ALIB_SERVICES_PROTOCOL mDxeAmdNbioAlibServicesProtocol = {
  AMD_ALIB_SERVICES_DXE_REVISION,
  AmdGetAlibAddress
};

/**
 *---------------------------------------------------------------------------------------
 *  InstallALibAcpiTable
 *
 *  Description:
 *     notification event handler for install ALib Acpi Table
 *  Parameters:
 *    @param[in]     Event      Event whose notification function is being invoked.
 *    @param[in]     *Context   Pointer to the notification function's context.
 *
 *---------------------------------------------------------------------------------------
 **/
VOID
EFIAPI
InstallALibAcpiTable (
  IN       EFI_EVENT  Event,
  IN       VOID       *Context
  )
{
  EFI_ACPI_TABLE_PROTOCOL *AcpiTable;
  EFI_STATUS              Status;
  VOID                    *AlibBuffer;

  IDS_HDT_CONSOLE (MAIN_FLOW, "InstallALibAcpiTable Start\n");
  AlibBuffer = NULL;

  Status = gBS->LocateProtocol (&gEfiAcpiTableProtocolGuid, NULL, (VOID **)&AcpiTable);
  if (EFI_ERROR (Status)) {
    return;
  }
  //
  // Create ACPI ALIB SSDT table
  //
  PcieAlibBuildAcpiTable ((VOID **)&AlibBuffer);


  //
  // Close event, so it will not be invoked again.
  //
  gBS->CloseEvent (Event);
  IDS_HDT_CONSOLE (MAIN_FLOW, "InstallALibAcpiTable End\n");
}


EFI_STATUS
EFIAPI
AmdNbioAlibRVDxeEntry (
  IN       EFI_HANDLE         ImageHandle,
  IN       EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS                           Status;
  EFI_HANDLE                           Handle;
  VOID *Registration;

  AGESA_TESTPOINT (TpNbioAlibDxeEntry, NULL);
  IDS_HDT_CONSOLE (MAIN_FLOW, "AmdNbioAlibRVDxeEntry Enter\n");
  Handle = NULL;

  Status = gBS->InstallProtocolInterface (
                  &Handle,
                  &gAmdNbioALibServicesProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &mDxeAmdNbioAlibServicesProtocol
                  );
  EfiCreateProtocolNotifyEvent (&gEfiAcpiTableProtocolGuid, TPL_CALLBACK, InstallALibAcpiTable, NULL, &Registration);

  IDS_HDT_CONSOLE (MAIN_FLOW, "AmdNbioAlibRVDxeEntry Exit\n");
  AGESA_TESTPOINT (TpNbioAlibDxeExit, NULL);

  return Status;
}

