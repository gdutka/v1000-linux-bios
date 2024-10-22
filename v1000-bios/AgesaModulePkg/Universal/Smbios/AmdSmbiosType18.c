/* $NoKeywords:$ */
/**
 * @file
 *
 * Generate SMBIOS type 18
 *
 * Contains code that generate SMBIOS type 18
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

#define FILECODE UNIVERSAL_SMBIOS_AMDSMBIOSTYPE18_FILECODE

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
 *  Generate SMBIOS type 18
 *
 *  Parameters:
 *    @param[in]       Smbios                       Pointer to EfiSmbiosProtocol
 *    @param[out]      MemoryErrorInfoHandle        Handle of the device where error is detected
 *    @param[in]       StdHeader                    Handle to config for library and services
 *
 *    @retval          EFI_STATUS
 *
 *---------------------------------------------------------------------------------------
 */
EFI_STATUS
EFIAPI
AmdAddSmbiosType18 (
  IN       EFI_SMBIOS_PROTOCOL  *Smbios,
     OUT   EFI_SMBIOS_HANDLE    *MemoryErrorInfoHandle,
  IN       AMD_CONFIG_PARAMS    *StdHeader
  )
{
  UINTN                             StructureSize;
  UINTN                             TotalSize;
  EFI_STATUS                        Status;
  EFI_STATUS                        CalledStatus;
  AMD_SMBIOS_TABLE_TYPE18          *SmbiosTableType18;

  Status = EFI_SUCCESS;

  // Calculate size of DMI type 18

  StructureSize = sizeof (AMD_SMBIOS_TABLE_TYPE18);
  TotalSize = StructureSize + 2; // Additional null (00h), End of strings

  // Allocate zero pool
  SmbiosTableType18 = NULL;
  SmbiosTableType18 = AllocateZeroPool (TotalSize);
  if (SmbiosTableType18 == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  SmbiosTableType18->Hdr.Handle                        = AMD_SMBIOS_HANDLE_PI_RESERVED;
  SmbiosTableType18->Hdr.Type                          = AMD_EFI_SMBIOS_TYPE_32BIT_MEMORY_ERROR_INFORMATION;
  SmbiosTableType18->Hdr.Length                        = (UINT8) StructureSize;

  SmbiosTableType18->ErrorType                         = AmdMemoryErrorOk;
  SmbiosTableType18->ErrorGranularity                  = AmdMemoryGranularityOtherUnknown;
  SmbiosTableType18->ErrorOperation                    = AmdMemoryErrorOperationUnknown;
  SmbiosTableType18->VendorSyndrome                    = 0;
  SmbiosTableType18->MemoryArrayErrorAddress           = 0x80000000;
  SmbiosTableType18->DeviceErrorAddress                = 0x80000000;
  SmbiosTableType18->ErrorResolution                   = 0x80000000;

  // Add DMI type 18
  CalledStatus = Smbios->Add (Smbios, NULL, &SmbiosTableType18->Hdr.Handle, (EFI_SMBIOS_TABLE_HEADER *) SmbiosTableType18);
  Status = (CalledStatus > Status) ? CalledStatus : Status;
  *MemoryErrorInfoHandle = SmbiosTableType18->Hdr.Handle;

  // Free pool
  FreePool (SmbiosTableType18);

  return Status;
}



/*----------------------------------------------------------------------------------------
 *                          L O C A L    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */


