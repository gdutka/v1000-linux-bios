/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Memory API, and related functions.
 *
 * Contains code that initializes channel translation table
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

  MemChanXLatZpPei.c
  Init Memory channel translation table

Abstract:
--*/

#include "PiPei.h"
#include <Ppi/AmdMemPpi.h>
#include <Library/BaseMemoryLib.h>
#include <Library/BaseLib.h>
#include <Library/AmdSocBaseLib.h>
#include <Library/MemChanXLat.h>
#include <Ppi/FabricTopologyServices2Ppi.h>
#include "Filecode.h"

#define FILECODE MEM_AMDMEMCHANXLATZPPEI_MEMCHANXLATZPPEI_FILECODE

extern EFI_GUID gAmdMemChanXLatPpiGuid;

//
// Create a Channel Mapping Transaltion Table per CPU Package
//
HOST_TO_APCB_CHANNEL_XLAT DummyChannelXlatTable[] = {
  // Requested   Translated
  { 0xFF,       0xFF},
};

HOST_TO_APCB_CHANNEL_XLAT Am4ChannelXlatTable[] = {
  // Channel A = 0, Channel B = 1
  // Requested   Translated
  { 0,          1 },
  { 1,          0 },
  { 0xFF,       0xFF},
};

HOST_TO_APCB_CHANNEL_XLAT Sp4ChannelXlatTable[] = {
  // Channel A = 0, Channel B = 1, Channel C = 2, Channel D = 3
  // Requested   Translated
  { 0,          0 },
  { 1,          1 },
  { 2,          2 },
  { 3,          3 },
  { 0xFF,       0xFF},
};

HOST_TO_APCB_CHANNEL_XLAT Sp3ChannelXlatTable[] = {
  // Channel A = 0, Channel B = 1, Channel C = 2, Channel D = 3
  // Channel E = 4, Channel F = 5, Channel G = 6, Channel H = 7
  // Requested   Translated
  { 0,          3 },
  { 1,          2 },
  { 2,          0 },
  { 3,          1 },
  { 4,          7 },
  { 5,          6 },
  { 6,          4 },
  { 7,          5 },
  { 0xFF,       0xFF},
};

HOST_TO_APCB_CHANNEL_XLAT Sp3r2Cf4ChannelXlatTable[] = {
  // Channel A = 0, Channel B = 1, Channel C = 2, Channel D = 3
  // Channel E = 4, Channel F = 5, Channel G = 6, Channel H = 7
  // Requested   Translated
  { 0,          0 },
  { 1,          1 },
  { 2,          0xFF},
  { 3,          0xFF},
  { 4,          2 },
  { 5,          3 },
  { 6,          0xFF},
  { 7,          0xFF},
  { 0xFF,       0xFF},
};

/*++

Routine Description:

  Initialize the memory channel translation table.

Arguments:

Returns:

  EFI_STATUS

--*/
EFI_STATUS
EFIAPI
AmdMemChanXLatZpPeiEntry (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS                            Status;
  SOC_ID_STRUCT                         SocId;
  AMD_MEMORY_CHANNEL_XLAT_PPI           *mMemChanXLatPpi;
  EFI_PEI_PPI_DESCRIPTOR                *mMemChanXLatPpiList;
  AMD_PEI_FABRIC_TOPOLOGY_SERVICES2_PPI *FabricTopologyServices;
  UINTN                                 NumOfDies;

  //----------------------------------------------------------------
  //
  //            Allocate space for PPI data structure
  //
  //----------------------------------------------------------------
  Status = (*PeiServices)->AllocatePool (
                           PeiServices,
                           sizeof (AMD_MEMORY_CHANNEL_XLAT_PPI),
                           (VOID**)&mMemChanXLatPpi
                           );
  if (EFI_SUCCESS != Status) {
    return Status;
  }

  Status = (*PeiServices)->AllocatePool (
                           PeiServices,
                           sizeof (EFI_PEI_PPI_DESCRIPTOR),
                           (VOID**)&mMemChanXLatPpiList
                           );
  if (EFI_SUCCESS != Status) {
    return Status;
  }

  mMemChanXLatPpi->XLatTab = (VOID *)&DummyChannelXlatTable[0];

  // Check Hardware Identification
  SocId.SocFamilyID = F17_ZP_RAW_ID;
  SocId.PackageType = ZP_AM4;

  if (SocHardwareIdentificationCheck (&SocId)) {
    mMemChanXLatPpi->XLatTab = (VOID *)&Am4ChannelXlatTable[0];
  }

  SocId.SocFamilyID = F17_ZP_RAW_ID;
  SocId.PackageType = ZP_SP4;

  if (SocHardwareIdentificationCheck (&SocId)) {
    mMemChanXLatPpi->XLatTab = (VOID *)&Sp4ChannelXlatTable[0];
  }

  SocId.SocFamilyID = F17_ZP_RAW_ID;
  SocId.PackageType = ZP_SP3;

  if (SocHardwareIdentificationCheck (&SocId)) {
    mMemChanXLatPpi->XLatTab = (VOID *)&Sp3ChannelXlatTable[0];
  }

  SocId.SocFamilyID = F17_ZP_RAW_ID;
  SocId.PackageType = TR_SP3r2;

  if (SocHardwareIdentificationCheck (&SocId)) {
    Status = (*PeiServices)->LocatePpi (
                                PeiServices,
                                &gAmdFabricTopologyServices2PpiGuid,
                                0,
                                NULL,
                                (VOID**)&FabricTopologyServices
                               );
    if (EFI_SUCCESS != Status) {
      return Status;
    }
    FabricTopologyServices->GetProcessorInfo (0, &NumOfDies, NULL);

    if (NumOfDies == 4) {
      mMemChanXLatPpi->XLatTab = (VOID *)&Sp3r2Cf4ChannelXlatTable[0];
    }
  }

  mMemChanXLatPpi->Revision = AMD_MEMORY_CHANNEL_XLAT_REVISION;
  mMemChanXLatPpiList->Flags = (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST);
  mMemChanXLatPpiList->Guid  = &gAmdMemChanXLatPpiGuid;
  mMemChanXLatPpiList->Ppi   = mMemChanXLatPpi;

  Status = (**PeiServices).InstallPpi (PeiServices, mMemChanXLatPpiList);

  return (Status);
}

