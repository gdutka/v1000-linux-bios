/** @file
 PEI Platform Spd Read PEIM.

 It produces an instance of the AMD Platform DIMM PPI to provide the SPD Data Read related functions
 which will be used by AGESA Call Out.

***************************************************************************
* Copyright (c) 2012, Insyde Software Corp. All Rights Reserved.
*
* You may not reproduce, distribute, publish, display, perform, modify, adapt,
* transmit, broadcast, present, recite, release, license or otherwise exploit
* any part of this publication in any form, by any means, without the prior
* written permission of Insyde Software Corporation.
*
******************************************************************************
*/

#include <PiPei.h>
#include <AGESA.h>

//
// Libraries
//
#include <Library/PeiServicesLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DebugLib.h>
#include <Library/SmbusLib.h>
#include <Library/HobLib.h>
#include <Library/BaseMemoryLib.h>

#include <Guid/DimmSmbusAddrHob.h>
//
// Produced Ppi
//
#include <Ppi/AmdMemPpi.h>

#include <Ppi/SpdDataRead.h>

#include <DimmSmbusAddrHobData.h>

extern EFI_GUID gAmdPlatformDimmSpdPpiGuid;

DIMM_INFO_SMBUS DimmSmbus[] =
{
  //Socket, Channel, Dimm, SmbusAddr
  {0, 0, 0, FixedPcdGet8 (PcdSmbusAddrChA1)},
  {0, 0, 1, FixedPcdGet8 (PcdSmbusAddrChA2)},
  {0, 1, 0, FixedPcdGet8 (PcdSmbusAddrChB1)},
  {0, 1, 1, FixedPcdGet8 (PcdSmbusAddrChB2)}    // both DIMM on one channel
};

//
// Structure for AMD Platform DIMM PPI Descriptor
//
typedef struct {
  EFI_PEI_PPI_DESCRIPTOR         PpiDescriptor;
  PEI_AMD_PLATFORM_DIMM_SPD_PPI  AmdPlatformDimmSpdPpi;
} PEI_AMD_PLATFORM_DIMM_SPD_PPI_DESCRIPTOR;

EFI_STATUS
EfiMemInitReadSpd (
  IN  EFI_PEI_SERVICES    **PeiServices,
  IN  UINT8               SlaveAddr,
  IN  UINT8               MemChannelId,   ///< Address of SPD - memory channel ID
  IN  UINT8               DimmId,         ///< Address of SPD - DIMM ID
  IN  UINT8               *Buffer         ///< Location where to place the SPD content
  )
{
  EFI_STATUS              Status;
  BOOLEAN                 SameAsPrevious;
  SPD_DATA_READ_PPI       *SpdDataReadPpi;

  Status = EFI_SUCCESS;
  SameAsPrevious = FALSE;

  Status = (**PeiServices).LocatePpi (
                             (CONST EFI_PEI_SERVICES**)PeiServices,
                             &gSpdDataReadPpiGuid,
                             0,
                             NULL,
                             (VOID**)&SpdDataReadPpi
                             );
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "EfiMemInitReadSpd LocatePpi gSpdDataReadPpiGuid FAIL.\n"));
    return Status;
  }

  if (FixedPcdGet32 (PcdFlashSpdDataStoreBase)) {
    //
    // Read Spd data from PcdFlashSpdDataStoreBase
    //
    SameAsPrevious = SpdDataReadPpi->SpdDataReadRom (
                                       PeiServices,
                                       SlaveAddr,
                                       MemChannelId,
                                       DimmId,
                                       Buffer
                                       );
    if (SameAsPrevious) {
      return EFI_SUCCESS;
    }
  }

  //
  // Read Spd data from Smbus
  //
  Status = SpdDataReadPpi->SpdDataReadSmbus (
                             PeiServices,
                             SlaveAddr,
                             Buffer
                             );

  DEBUG ((EFI_D_ERROR, "EfiMemInitReadSpd SpdDataReadSmbus Status = %r\n", Status));
  return Status;
}

/**
 PlatformDimmSpdRead function instance, This is the interface function to initiate SPD Data Read for each DIMM through AGESA Call Out
 AgesaReadSpd.

 @param[in]         PeiServices         Describes the list of possible PEI Services.
 @param[in]         This                A pointer to the PPI structure.
 @param[in]         SpdData             SPD Data Buffer

 @retval            EFI_SUCCESS         The operation completed successfully.
 @retval            Others              An unexpected error occurred.
*/
EFI_STATUS
PlatformDimmSpdRead (
  IN      EFI_PEI_SERVICES                                **PeiServices,
  IN      PEI_AMD_PLATFORM_DIMM_SPD_PPI                   *This,
  IN OUT  AGESA_READ_SPD_PARAMS                           *SpdData
  )
{
  EFI_STATUS           Status;
  UINT8                SmbusAddress;
  EFI_HOB_GUID_TYPE    *GuidHob;
  UINTN                Index;
  DIMM_INFO_SMBUS      *DimmInfoSmbusArray;

  DimmInfoSmbusArray = NULL;
  GuidHob = GetFirstGuidHob (&gDimmSmbusAddrHobGuid);
  if (GuidHob != NULL) {
    DimmInfoSmbusArray = (DIMM_INFO_SMBUS *) ((UINT8 *)GuidHob + \
                                                       sizeof (EFI_HOB_GUID_TYPE) + \
                                                       FixedPcdGet8 (PcdMaxDimms)
                                                       );
  }

  for (Index = 0; Index < FixedPcdGet8 (PcdMaxDimms); Index++) {
    if ((SpdData->MemChannelId == DimmInfoSmbusArray[Index].ChannelId) && (SpdData->DimmId == DimmInfoSmbusArray[Index].DimmId)) {
      break;
    }
  }

  //
  // GetSpecificDimmSpdAddr
  //
  switch (SpdData->MemChannelId) {
    case 0:
      SmbusAddress = (SpdData->DimmId == 0) ? FixedPcdGet8 (PcdSmbusAddrChA1) : FixedPcdGet8 (PcdSmbusAddrChA2);
      break;

    case 1:
      SmbusAddress = (SpdData->DimmId == 0) ? FixedPcdGet8 (PcdSmbusAddrChB1) : FixedPcdGet8 (PcdSmbusAddrChB2);
      break;

    default:
      SmbusAddress = 0;
      break;
  }

  //
  // Get Smbus address done, process smbus read
  //
  Status = EfiMemInitReadSpd (PeiServices, SMBUS_LIB_SLAVE_ADDRESS(SmbusAddress), SpdData->MemChannelId, SpdData->DimmId, SpdData->Buffer);

  DEBUG ((EFI_D_ERROR, " PlatformDimmSpdRead: Channel=%x, DIMM=%x, SPD=%x, Status=%r\n", SpdData->MemChannelId, SpdData->DimmId, SmbusAddress, Status));

  //ReadSpdDone:
  //
  // update DimmInfoSmbusArray DimmPresent
  //
  if (EFI_ERROR (Status)) {
    DimmInfoSmbusArray[Index].DimmPresent = FALSE;
    DEBUG ((EFI_D_ERROR, "PlatformDimmSpdRead-DimmPresent           = FALSE\n"));
  } else {
    DimmInfoSmbusArray[Index].DimmPresent = TRUE;
    DEBUG ((EFI_D_ERROR, "PlatformDimmSpdRead-DimmPresent           = TRUE\n"));
  }

  DEBUG ((EFI_D_ERROR, "PlatformDimmSpdRead Status = %r\n", Status));
  return Status;
}

/**
 This is the declaration of PEI Platform Spd Read PEIM entry point.

 @param[in]         FileHandle          Handle of the file being invoked.
 @param[in]         PeiServices         Describes the list of possible PEI Services.

 @retval            EFI_SUCCESS         The operation completed successfully.
 @retval            Others              An unexpected error occurred.
*/
EFI_STATUS
EFIAPI
PlatformSpdReadPeiEntryPoint (
  IN EFI_PEI_FILE_HANDLE             FileHandle,
  IN CONST EFI_PEI_SERVICES        **PeiServices
  )
{
  EFI_STATUS                                Status;
  UINT32                                    Size;
  PEI_AMD_PLATFORM_DIMM_SPD_PPI_DESCRIPTOR  *AmdPlatformDimmSpdPpiDescriptor;
  EFI_PEI_PPI_DESCRIPTOR                    *PpiDescriptor;
  VOID                                      *Ppi;
  UINT8                                     *DimmSmbusAddrArray;
  DIMM_INFO_SMBUS                           *DimmInfoSmbusArray;
  UINT8                                     Index;

  PpiDescriptor = NULL;
  Ppi = NULL;

  Status = (**PeiServices).AllocatePool (
                             PeiServices,
                             (FixedPcdGet8 (PcdMaxDimms) + (FixedPcdGet8 (PcdMaxDimms) * sizeof (DIMM_INFO_SMBUS))),
                             (VOID**)&DimmSmbusAddrArray
                             );
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "PlatformSpdReadPeiEntryPoint-End-1-Status=%r\n", Status));
    return Status;
  }
  (**PeiServices).SetMem (
                    DimmSmbusAddrArray,
                    (FixedPcdGet8 (PcdMaxDimms) + (FixedPcdGet8 (PcdMaxDimms) * sizeof (DIMM_INFO_SMBUS))),
                    0
                    );
  DimmInfoSmbusArray = (DIMM_INFO_SMBUS*)(UINTN)(DimmSmbusAddrArray + FixedPcdGet8 (PcdMaxDimms));
  //
  // Default setting
  //
  for (Index = 0; Index < (sizeof (DimmSmbus) / sizeof (DIMM_INFO_SMBUS)); Index++) {
    DimmSmbusAddrArray [Index] = DimmSmbus[Index].SmbusAddress;
  }
  (**PeiServices).CopyMem (DimmInfoSmbusArray, &DimmSmbus, sizeof (DimmSmbus));
  BuildGuidDataHob (
    &gDimmSmbusAddrHobGuid,
    DimmSmbusAddrArray,
    (FixedPcdGet8 (PcdMaxDimms) + (FixedPcdGet8 (PcdMaxDimms) * sizeof (DIMM_INFO_SMBUS)))
    );

  //
  // Register so it will be automatically shadowed to memory
  //
  Status = PeiServicesRegisterForShadow (FileHandle);

  //
  // Create an instance of the AMD Platform DIMM ppi,
  //
  AmdPlatformDimmSpdPpiDescriptor = AllocateZeroPool (sizeof (PEI_AMD_PLATFORM_DIMM_SPD_PPI_DESCRIPTOR));
  if (AmdPlatformDimmSpdPpiDescriptor == NULL) {
    DEBUG ((EFI_D_ERROR, "PlatformSpdReadPeiEntryPoint-End-2-EFI_OUT_OF_RESOURCES\n"));
    return EFI_OUT_OF_RESOURCES;
  }

  Size = sizeof (PEI_AMD_PLATFORM_DIMM_SPD_PPI_DESCRIPTOR);
  if (Size < sizeof (UINT32)) {         // must at least contain Size field.
    DEBUG ((EFI_D_ERROR, "PlatformSpdReadPeiEntryPoint-End-3-EFI_INVALID_PARAMETER\n"));
    return EFI_INVALID_PARAMETER;
  }

  //
  // Build AmdPlatformDimmSpdPpi Private Data
  //
  AmdPlatformDimmSpdPpiDescriptor->PpiDescriptor.Flags   = (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST);
  AmdPlatformDimmSpdPpiDescriptor->PpiDescriptor.Guid    = &gAmdPlatformDimmSpdPpiGuid;
  AmdPlatformDimmSpdPpiDescriptor->PpiDescriptor.Ppi     = &AmdPlatformDimmSpdPpiDescriptor->AmdPlatformDimmSpdPpi;
  AmdPlatformDimmSpdPpiDescriptor->AmdPlatformDimmSpdPpi.TotalNumberOfSocket = 1;                             ///< Total Number of Physical Socket.
  AmdPlatformDimmSpdPpiDescriptor->AmdPlatformDimmSpdPpi.TotalNumberOfDimms  = FixedPcdGet8 (PcdMaxDimms);    ///< Total Number of DIMMS
  AmdPlatformDimmSpdPpiDescriptor->AmdPlatformDimmSpdPpi.PlatformDimmSpdRead = PlatformDimmSpdRead;           ///< Function to be called

  if (Status == EFI_ALREADY_STARTED) {
    //
    // Shadow completed and running from memory
    //
    Status = PeiServicesLocatePpi (
               &gAmdPlatformDimmSpdPpiGuid,
               0,
               &PpiDescriptor,
               &Ppi
               );
    if (!EFI_ERROR (Status)) {
      DEBUG ((EFI_D_ERROR, "PlatformSpdReadPeiEntryPoint-PeiServicesReInstallPpi-gAmdPlatformDimmSpdPpiGuid\n"));
      Status = PeiServicesReInstallPpi (
                 PpiDescriptor,
                 &AmdPlatformDimmSpdPpiDescriptor->PpiDescriptor
                 );
    }
  } else {
    //
    // Install the Ppi.
    //
    DEBUG ((EFI_D_ERROR, "PlatformSpdReadPeiEntryPoint-PeiServicesInstallPpi-gAmdPlatformDimmSpdPpiGuid\n"));
    Status = PeiServicesInstallPpi (&AmdPlatformDimmSpdPpiDescriptor->PpiDescriptor);
  }

  DEBUG ((EFI_D_ERROR, "PlatformSpdReadPeiEntryPoint-End-4-Status=%r\n", Status));
  return Status;
}
