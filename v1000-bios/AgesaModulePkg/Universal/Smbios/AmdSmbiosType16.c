/* $NoKeywords:$ */
/**
 * @file
 *
 * Generate SMBIOS type 16
 *
 * Contains code that generate SMBIOS type 16
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

#define FILECODE UNIVERSAL_SMBIOS_AMDSMBIOSTYPE16_FILECODE

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
 *  Generate SMBIOS type 16
 *
 *  Parameters:
 *    @param[in]       Smbios                       Pointer to EfiSmbiosProtocol
 *    @param[in]       MemDmiInfo                   Pointer to Memory DMI information
 *    @param[in]       MemoryErrorInfoHandle        Handle of the array where error is detected
 *    @param[out]      MemoryArrayHandle            Handle of the current physical array
 *    @param[in]       StdHeader                    Handle to config for library and services
 *
 *    @retval          EFI_STATUS
 *
 *---------------------------------------------------------------------------------------
 */
EFI_STATUS
EFIAPI
AmdAddSmbiosType16 (
  IN       EFI_SMBIOS_PROTOCOL  *Smbios,
  IN       DMI_INFO             *MemDmiInfo,
  IN       EFI_SMBIOS_HANDLE     MemoryErrorInfoHandle,
  IN       EFI_SMBIOS_HANDLE    *MemoryArrayHandle,
  IN       AMD_CONFIG_PARAMS    *StdHeader
  )
{
  UINTN                             StructureSize;
  UINTN                             TotalSize;
  EFI_STATUS                        Status;
  EFI_STATUS                        CalledStatus;
  AMD_SMBIOS_TABLE_TYPE16          *SmbiosTableType16;

  Status = EFI_SUCCESS;
  // Calculate size of DMI type 16
  StructureSize = sizeof (AMD_SMBIOS_TABLE_TYPE16);
  TotalSize = StructureSize + 2; // Additional null (00h), End of strings

  // Allocate zero pool
  SmbiosTableType16 = NULL;
  SmbiosTableType16 = AllocateZeroPool (TotalSize);
  if (SmbiosTableType16 == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  // Generate DMI type 4 --- Start

  SmbiosTableType16->Hdr.Type                     = AMD_EFI_SMBIOS_TYPE_PHYSICAL_MEMORY_ARRAY;
  SmbiosTableType16->Hdr.Length                   = (UINT8) StructureSize;
  SmbiosTableType16->Hdr.Handle                   = AMD_SMBIOS_HANDLE_PI_RESERVED;

  SmbiosTableType16->Location                     = MemDmiInfo->T16.Location;
  SmbiosTableType16->Use                          = MemDmiInfo->T16.Use;
  SmbiosTableType16->MemoryErrorCorrection        = MemDmiInfo->T16.MemoryErrorCorrection;
  SmbiosTableType16->MemoryErrorInformationHandle = MemoryErrorInfoHandle;
  SmbiosTableType16->NumberOfMemoryDevices        = MemDmiInfo->T16.NumberOfMemoryDevices;

  SmbiosTableType16->MaximumCapacity              = PcdGet32 (PcdAmdSmbiosT16MaximumCapacity);
  if (SmbiosTableType16->MaximumCapacity == 0x80000000) {
    SmbiosTableType16->ExtendedMaximumCapacity      = PcdGet64 (PcdAmdSmbiosT16ExtMaximumCapacity);
  } else {
    SmbiosTableType16->ExtendedMaximumCapacity      = 0;
  }

  // Add DMI type 16
  CalledStatus = Smbios->Add (Smbios, NULL, &SmbiosTableType16->Hdr.Handle, (EFI_SMBIOS_TABLE_HEADER *) SmbiosTableType16);
  Status = (CalledStatus > Status) ? CalledStatus : Status;
  *MemoryArrayHandle = SmbiosTableType16->Hdr.Handle;

  // Free pool
  FreePool (SmbiosTableType16);

  return Status;
}



/*----------------------------------------------------------------------------------------
 *                          L O C A L    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */


