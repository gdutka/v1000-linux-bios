/* $NoKeywords:$ */
/**
 * @file
 *
 * Generate SMBIOS type 19
 *
 * Contains code that generate SMBIOS type 19
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

#define FILECODE UNIVERSAL_SMBIOS_AMDSMBIOSTYPE19_FILECODE

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
 *  Generate SMBIOS type 19
 *
 *  Parameters:
 *    @param[in]       Smbios                       Pointer to EfiSmbiosProtocol
 *    @param[in]       MemDmiInfo                   Pointer to Memory DMI information
 *    @param[in]       MemoryArrayHandle            Handle of the array that maps to the address range
 *    @param[in]       ArrayMappedHandle            Handle of the array mapped address that maps to the address range
 *    @param[in]       StdHeader                    Handle to config for library and services
 *
 *    @retval          EFI_SUCCESS
 *
 *---------------------------------------------------------------------------------------
 */
EFI_STATUS
EFIAPI
AmdAddSmbiosType19 (
  IN       EFI_SMBIOS_PROTOCOL  *Smbios,
  IN       DMI_INFO             *MemDmiInfo,
  IN       EFI_SMBIOS_HANDLE     MemoryArrayHandle,
  IN       EFI_SMBIOS_HANDLE    *ArrayMappedHandle,
  IN       AMD_CONFIG_PARAMS    *StdHeader,
  IN       UINT8                RegionIndex
  )
{
  UINTN                             StructureSize;
  UINTN                             TotalSize;
  EFI_STATUS                        Status;
  EFI_STATUS                        CalledStatus;
  AMD_SMBIOS_TABLE_TYPE19          *SmbiosTableType19;

  Status = EFI_SUCCESS;

  if ((MemDmiInfo->T19[RegionIndex].StartingAddr == MemDmiInfo->T19[RegionIndex].EndingAddr) &&
      (MemDmiInfo->T19[RegionIndex].ExtStartingAddr == MemDmiInfo->T19[RegionIndex].ExtEndingAddr)) {
    return Status;
  }
  // Generate DMI type 19 --- Start

  SmbiosTableType19 = NULL;
  StructureSize = sizeof (AMD_SMBIOS_TABLE_TYPE19);
  TotalSize = StructureSize + 2; // Additional null (00h), End of strings

  // Allocate zero pool
  SmbiosTableType19 = NULL;
  SmbiosTableType19 = AllocateZeroPool (TotalSize);
  if (SmbiosTableType19 == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  SmbiosTableType19->Hdr.Handle                        = AMD_SMBIOS_HANDLE_PI_RESERVED;
  SmbiosTableType19->Hdr.Type                          = AMD_EFI_SMBIOS_TYPE_MEMORY_ARRAY_MAPPED_ADDRESS;
  SmbiosTableType19->Hdr.Length                        = (UINT8) StructureSize;

  SmbiosTableType19->StartingAddress                   = MemDmiInfo->T19[RegionIndex].StartingAddr;
  SmbiosTableType19->EndingAddress                     = MemDmiInfo->T19[RegionIndex].EndingAddr;
  SmbiosTableType19->MemoryArrayHandle                 = MemoryArrayHandle;
  SmbiosTableType19->PartitionWidth                    = MemDmiInfo->T19[RegionIndex].PartitionWidth;
  SmbiosTableType19->ExtendedStartingAddress           = MemDmiInfo->T19[RegionIndex].ExtStartingAddr;
  SmbiosTableType19->ExtendedEndingAddress             = MemDmiInfo->T19[RegionIndex].ExtEndingAddr;

  // Add DMI type 19
  CalledStatus = Smbios->Add (Smbios, NULL, &SmbiosTableType19->Hdr.Handle, (EFI_SMBIOS_TABLE_HEADER *) SmbiosTableType19);
  Status = (CalledStatus > Status) ? CalledStatus : Status;

  *ArrayMappedHandle = SmbiosTableType19->Hdr.Handle;

  // Free pool
  FreePool (SmbiosTableType19);

  return Status;
}

/*----------------------------------------------------------------------------------------
 *                          L O C A L    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

