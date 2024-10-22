/* $NoKeywords:$ */
/**
 * @file
 *
 * Generate SMBIOS type 20
 *
 * Contains code that generate SMBIOS type 20
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
#include "AmdSmBios.h"
#include "AMD.h"
#include "Porting.h"
#include "Filecode.h"
#include "Library/IdsLib.h"
#include "Library/BaseLib.h"
#include <MemDmi.h>
#include "Library/UefiBootServicesTableLib.h"
#include "Library/MemoryAllocationLib.h"
#include "Protocol/Smbios.h"
#include "Protocol/AmdSmbiosServicesProtocol.h"
#include "PiDxe.h"

#define FILECODE UNIVERSAL_SMBIOS_AMDSMBIOSTYPE20_FILECODE

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
/**
 *---------------------------------------------------------------------------------------
 *
 *  Generate SMBIOS type 20
 *
 *  Parameters:
 *    @param[in]       Smbios                       Pointer to EfiSmbiosProtocol
 *    @param[in]       MemDmiInfo                   Pointer to Memory DMI information
 *    @param[in]       Socket                       Socket number
 *    @param[in]       Channel                      Channel number
 *    @param[in]       Dimm                         Dimm number
 *    @param[in]       MemoryDeviceHandle           Handle of the device where the address range is mapped to
 *    @param[in]       MemoryArrayHandle            Handle of the array where the address range is mapped to
 *    @param[in]       StdHeader                    Handle to config for library and services
 *
 *    @retval          EFI_SUCCESS                  The Type 20 entry is added successfully.
 *
 *---------------------------------------------------------------------------------------
 */
EFI_STATUS
EFIAPI
AmdAddSmbiosType20 (
  IN       EFI_SMBIOS_PROTOCOL  *Smbios,
  IN       DMI_INFO             *MemDmiInfo,
  IN       UINT8                 Socket,
  IN       UINT8                 Channel,
  IN       UINT8                 Dimm,
  IN       EFI_SMBIOS_HANDLE     MemoryDeviceHandle,
  IN       EFI_SMBIOS_HANDLE     ArrayMappedHandle,
  IN       AMD_CONFIG_PARAMS    *StdHeader
  )
{
  UINTN                             StructureSize;
  UINTN                             TotalSize;
  EFI_STATUS                        Status;
  EFI_STATUS                        CalledStatus;
  AMD_SMBIOS_TABLE_TYPE20          *SmbiosTableType20;

  Status = EFI_SUCCESS;

  // Generate DMI type 20 --- Start

  if (MemDmiInfo->T20[Socket][Channel][Dimm].EndingAddr != 0) {
  // Calculate size of DMI type 20
    StructureSize = sizeof (AMD_SMBIOS_TABLE_TYPE20);
    TotalSize = StructureSize + 2; // Additional null (00h), End of strings

    // Allocate zero pool
    SmbiosTableType20 = NULL;
    SmbiosTableType20 = AllocateZeroPool (TotalSize);
    if (SmbiosTableType20 == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }

    SmbiosTableType20->Hdr.Handle                        = AMD_SMBIOS_HANDLE_PI_RESERVED;
    SmbiosTableType20->Hdr.Type                          = AMD_EFI_SMBIOS_TYPE_MEMORY_DEVICE_MAPPED_ADDRESS;
    SmbiosTableType20->Hdr.Length                        = (UINT8) StructureSize;

    SmbiosTableType20->StartingAddress                   = MemDmiInfo->T20[Socket][Channel][Dimm].StartingAddr;
    SmbiosTableType20->EndingAddress                     = MemDmiInfo->T20[Socket][Channel][Dimm].EndingAddr;
    SmbiosTableType20->MemoryDeviceHandle                = MemoryDeviceHandle;
    SmbiosTableType20->MemoryArrayMappedAddressHandle    = ArrayMappedHandle;
    SmbiosTableType20->PartitionRowPosition              = MemDmiInfo->T20[Socket][Channel][Dimm].PartitionRowPosition;
    SmbiosTableType20->InterleavePosition                = MemDmiInfo->T20[Socket][Channel][Dimm].InterleavePosition;
    SmbiosTableType20->InterleavedDataDepth              = MemDmiInfo->T20[Socket][Channel][Dimm].InterleavedDataDepth;
    SmbiosTableType20->ExtendedStartingAddress           = MemDmiInfo->T20[Socket][Channel][Dimm].ExtStartingAddr;
    SmbiosTableType20->ExtendedEndingAddress             = MemDmiInfo->T20[Socket][Channel][Dimm].ExtEndingAddr;

    // Add DMI type 20
    CalledStatus = Smbios->Add (Smbios, NULL, &SmbiosTableType20->Hdr.Handle, (EFI_SMBIOS_TABLE_HEADER *) SmbiosTableType20);
    Status = (CalledStatus > Status) ? CalledStatus : Status;

    // Free pool
    FreePool (SmbiosTableType20);

    return Status;
  } else {
    return EFI_SUCCESS;
  }
}



/*----------------------------------------------------------------------------------------
 *                          L O C A L    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

