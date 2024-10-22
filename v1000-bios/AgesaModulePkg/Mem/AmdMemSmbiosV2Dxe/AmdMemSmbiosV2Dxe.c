/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Memory API for CZ DDR4, and related functions.
 *
 * Contains code that initializes memory
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  PSP
 * @e \$Revision: 309090 $   @e \$Date: 2014-12-09 12:28:05 -0600 (Tue, 09 Dec 2014) $
 *
 */
/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */
/*++
Module Name:

  AmdMemS3Dxe.c
  Memory Context S3 Save interface

Abstract:
--*/
#include "Uefi.h"
#include "PiDxe.h"
#include "AGESA.h"
#include "Library/IdsLib.h"
#include "Library/AmdBaseLib.h"
#include "Library/AmdHeapLib.h"
#include "AmdMemSmbiosV2Dxe.h"
#include "Library/UefiBootServicesTableLib.h"
#include "Filecode.h"

#define FILECODE        MEM_AMDMEMSMBIOSV2DXE_AMDMEMSMBIOSV2DXE_FILECODE

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                           G L O B A L   V A R I A B L E S
 *----------------------------------------------------------------------------------------
 */
STATIC AMD_MEM_SMBIOS_SERVICES_PROTOCOL   mMemDmiServicesProtocol = {
  SMBIOS_3_1_1,      // Support SMBIOS 3.1.1
  GetMemDmiInfo
};

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
/**
 *---------------------------------------------------------------------------------------
 *
 *  Entry for AmdMemSmbiosCzDxe
 *  Install the protocol of Memory DMI information
 *
 *    @retval          EFI_STATUS
 *
 *---------------------------------------------------------------------------------------
 */
EFI_STATUS
EFIAPI
DxeAmdMemSmbiosV2Entry (
  IN       EFI_HANDLE        ImageHandle,
  IN       EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS                     Status;

  IDS_HDT_CONSOLE (MAIN_FLOW, "  AmdMemSmbiosV2Dxe Entry\n");

  // Install SMBIOS services protocol
  Status = gBS->InstallProtocolInterface (
                &ImageHandle,
                &gAmdMemSmbiosServicesProtocolGuid,
                EFI_NATIVE_INTERFACE,
                &mMemDmiServicesProtocol
                );

  IDS_HDT_CONSOLE (MAIN_FLOW, "  AmdMemSmbiosV2Dxe Exit\n");
  return Status;
}


/*----------------------------------------------------------------------------------------
 *                          L O C A L    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
/**
 * This service retrieves information about memory.
 *
 * @param[in]  This                                 A pointer to the
 *                                                  AMD_MEM_SMBIOS_SERVICES_PROTOCOL instance.
 * @param[in, out] MemDmiInfo                           Contains Memory DMI information
 *
 * @retval EFI_SUCCESS                              The Memory DMI information was successfully retrieved.
 * @retval EFI_INVALID_PARAMETER                    MemDmiInfo is NULL.
 *
 **/
EFI_STATUS
EFIAPI
GetMemDmiInfo (
  IN       AMD_MEM_SMBIOS_SERVICES_PROTOCOL       *This,
  IN OUT   DMI_INFO                               *MemDmiInfo
  )
{
  AMD_CONFIG_PARAMS             StdHeader;
  LOCATE_HEAP_PTR               LocateHeapParams;
  AGESA_STATUS                  Status;

  LocateHeapParams.BufferHandle = AMD_DMI_INFO_BUFFER_HANDLE;
  Status = HeapLocateBuffer (&LocateHeapParams, NULL);
  if (Status == AGESA_SUCCESS) {
    // Retrieve the memory DMI info from heap data
    IDS_HDT_CONSOLE (MAIN_FLOW, "  Memory DMI information retrieved from heap data.\n");
    LibAmdMemCopy (MemDmiInfo, LocateHeapParams.BufferPtr, LocateHeapParams.BufferSize, &StdHeader);

    return EFI_SUCCESS;
  } else {
    return EFI_UNSUPPORTED;
  }
}

