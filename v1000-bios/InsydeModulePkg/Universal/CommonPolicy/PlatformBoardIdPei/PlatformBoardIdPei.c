/** @file
  Initialize board identifier and PCD SKU identifier.
  After the board identifier has been set, then install board identifier set PPIs.

;******************************************************************************
;* Copyright (c) 2016 - 2020, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <PiPei.h>
#include <PostCode.h>
#include <Ppi/H2OBoardIdSet.h>
#include <Library/PcdLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/PeiOemSvcKernelLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DebugLib.h>

STATIC EFI_PEI_PPI_DESCRIPTOR mPpiH2OBoardIdSet = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gH2OBoardIdSetPpiGuid,
  NULL
};

/**
 Debug print the warning message for unsupported board id.
**/
VOID
DebugPrintBoardIdUnsupportedMsg (
  VOID
  )
{
  UINT8                           *BoardIds;
  UINTN                           BoardIdsSize;
  UINT32                          Index;
  UINT32                          IdBase;
  UINT8                           IdBitMask;
  UINT8                           BitIndex;

  DEBUG ((EFI_D_ERROR, "------------------------ PlatformBoardIdPei Print Begin -------------------\n"));
  DEBUG ((EFI_D_ERROR, "WARNING! This BIOS does not support board %d. ", PcdGet8 (PcdH2OBoardId)));

  DEBUG ((EFI_D_ERROR, "This BIOS only supports Boards"));
  BoardIds     = (UINT8 *) PcdGetPtr (PcdH2OBoardIds);
  BoardIdsSize = PcdGetSize (PcdH2OBoardIds);
  for (Index = 0; Index < BoardIdsSize; Index++) {
    IdBitMask = BoardIds[Index];
    IdBase    = Index * 8;

    for (BitIndex = 0; BitIndex < 8; BitIndex++) {
      if (((IdBitMask >> BitIndex) & 1) != 0) {
        DEBUG ((EFI_D_ERROR, " %d", IdBase + BitIndex));
      }
    }
  }
  DEBUG ((EFI_D_ERROR, ".\n"));

  DEBUG ((EFI_D_ERROR, "Verify that board %%d|%%s is listed in [SkuIds] in Project.dsc "));
  DEBUG ((EFI_D_ERROR, "and this board name is listed in TargetBoards in Project.cfg.\n"));
  DEBUG ((EFI_D_ERROR, "%%d = board number, %%s = board name\n"));
  DEBUG ((EFI_D_ERROR, "------------------------ PlatformBoardIdPei Print End ---------------------\n"));
}

/**
 Initializes board identifier and PCD SKU identifier.

 @param[in] FfsHeader          Header for file of Firmware File System
 @param[in] PeiServices        The PEI core services table.

 @retval EFI_SUCCESS            Initialize board identifier successfully.
 @retval EFI_OUT_OF_RESOURCES   Fail to allocate memory.
**/
EFI_STATUS
EFIAPI
PlatformBoardIdPeiEntryPoint (
  IN EFI_PEI_FILE_HANDLE          FileHandle,
  IN CONST EFI_PEI_SERVICES       **PeiServices
  )
{
  UINT32                          BoardId;
  EFI_STATUS                      Status;
  EFI_PEI_PPI_DESCRIPTOR          *H2OBoardIdsSetPpiDesc;

  UINTN                           PcdSkuId;

  //
  // Initialize H2O board ID and PCD SKU ID.
  //
  DEBUG ((EFI_D_INFO, "------------------------ PlatformBoardId set SKU START -------------------\n"));
  BoardId = PcdGet8 (PcdH2OBoardId);
  DEBUG ((EFI_D_INFO, "  PCD get BoardId = 0x%02x\n", BoardId));
  DEBUG ((DEBUG_INFO, "OemKernelServices Call: OemSvcGetBoardId \n"));
  Status  = OemSvcGetBoardId (&BoardId);
  DEBUG ((DEBUG_INFO, "OemKernelServices OemSvcGetBoardId Status: %r\n", Status));
  DEBUG ((EFI_D_INFO, "  OemSvcGetBoardId get BoardId = 0x%02x,  ReturnStatus = %08x\n", BoardId, Status));
  if (Status == EFI_SUCCESS) {
    return Status;
  }
  LibPcdSetSku ((UINTN) BoardId);
  PcdSkuId = LibPcdGetSku ();
  DEBUG ((EFI_D_INFO, "  LibPcdGetSku = 0x%02x\n", PcdSkuId));
  if (PcdSkuId != BoardId) {
    DEBUG ((EFI_D_ERROR, " Use BoardId 0x%02x to call LibPcdSetSku failed,\n", BoardId));
    DEBUG ((EFI_D_ERROR, " it should check the BoardId value is whether declared in both [SkuIds] and SKUID_IDENTIFIER of Project.dsc.\n"));
    DEBUG ((EFI_D_ERROR, " Report post code 0x8E PEI_BOARD_ID_SETUP_FAILED and then call CpuDeadLoop()\n"));
    POST_CODE (PEI_BOARD_ID_SETUP_FAILED);
    ASSERT (PcdSkuId != BoardId);
    CpuDeadLoop ();
  }
  DEBUG ((EFI_D_INFO, "------------------------ PlatformBoardId set SKU END -------------------\n"));

  Status = PcdSet8S (PcdH2OBoardId, (UINT8) BoardId);
  ASSERT_EFI_ERROR (Status);

  if (!H2O_BOARD_SUPPORTED(BoardId)) {
    DebugPrintBoardIdUnsupportedMsg ();
  }

  //
  // Install H2OBoardIdsSet and H2OBoardIdsSet PPIs.
  //
  Status = PeiServicesInstallPpi (&mPpiH2OBoardIdSet);
  ASSERT_EFI_ERROR (Status);

  H2OBoardIdsSetPpiDesc = AllocatePool (sizeof (EFI_PEI_PPI_DESCRIPTOR) + sizeof (EFI_GUID));
  if (H2OBoardIdsSetPpiDesc == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  H2OBoardIdsSetPpiDesc->Flags = (UINTN) (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST);
  H2OBoardIdsSetPpiDesc->Guid  = (EFI_GUID *) ((UINT8 *) H2OBoardIdsSetPpiDesc + sizeof (EFI_PEI_PPI_DESCRIPTOR));
  H2OBoardIdsSetPpiDesc->Ppi   = NULL;

  CopyGuid (H2OBoardIdsSetPpiDesc->Guid, &gH2OBoardIdsSetPpiGuid);
  H2OBoardIdsSetPpiDesc->Guid->Data1 &= 0xFFFFFF00;
  H2OBoardIdsSetPpiDesc->Guid->Data1 += (UINT8) BoardId;
  Status = PeiServicesInstallPpi (H2OBoardIdsSetPpiDesc);
  ASSERT_EFI_ERROR (Status);

  return EFI_SUCCESS;
}

