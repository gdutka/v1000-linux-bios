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

  AmdMemPei.c
  Init Memory interface

Abstract:
--*/

#include "PiPei.h"
#include <Library/BaseMemoryLib.h>
#include <Library/BaseLib.h>
#include <Ppi/AmdMemPpi.h>
#include <Library/AmdMemBaseLib/AmdMemBaseLib.h>
#include <Uefi.h>
#include <Library/AmdBaseLib.h>
#include <Library/IdsLib.h>
#include <Library/AmdMemMainLib/AmdMemMainLib.h>
#include <Library/AmdMemServicesLib.h>
#include "Filecode.h"

#define FILECODE MEM_AMDMEMPEI_AMDMEMPEI_FILECODE

STATIC PEI_INITIALIZE_AGESA_MEMORY_PPI mInitializeAgesaMemoryPpi = {
  AMD_MEMORY_INITIALIZE_AGESA_MEMORY_REVISION
};

STATIC EFI_PEI_PPI_DESCRIPTOR mInitializeAgesaMemoryPiList =
{
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gAmdMemoryInitializeAgesaMemoryPpiGuid,
  &mInitializeAgesaMemoryPpi
};

STATIC PEI_MEMORY_INIT_COMPLETE_PPI mMemoryInitCompletePpi = {
  AMD_MEMORY_INIT_COMPLETE_REVISION,
  AmdGetMemoryAbove4gbService,
  AmdGetMemoryBelow4gbService,
  AmdGetMemoryBelow1TbService,
  AmdGetBottomIoService,
  AmdGetTotalMemorySizeService,
  AmdGetMemoryFrequencyService,
  AmdGetUmaInfoService
};

STATIC EFI_PEI_PPI_DESCRIPTOR mMemoryInitCompleteiList =
{
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gAmdMemoryInitCompletePpiGuid,
  &mMemoryInitCompletePpi
};

STATIC AMD_MEM_POOL_PTR_TABLE_PPI mMemPoolPtrTablePpi = {
  AMD_MEM_POOL_PTR_TABLE_REVISION
};

STATIC EFI_PEI_PPI_DESCRIPTOR mMemPolPtrTablePiList =
{
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gAmdMemoryPoolPointerTablePpiGuid,
  &mMemPoolPtrTablePpi
};
/*++

Routine Description:

  PSP Driver Entry. Initialize PSP device and report DRAM info to PSP once found

Arguments:

Returns:

  EFI_STATUS

--*/
EFI_STATUS
EFIAPI
PeiAmdMemEntry (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS            Status = EFI_SUCCESS;
  MEM_DATA_STRUCT       *MemDataStructure;
  //PEI_AMD_SOC_ID_CHECK_PPI *AmdSocIdPpiPtr;
  AMD_MEM_POOL_PTR_TABLE_PPI *AmdPoolTablePtr;
  IN AMD_CONFIG_PARAMS  *StdHeader;

  //-----------------------------------------
  //
  //              Check SOC ID
  //
  //-----------------------------------------
  //Status = (*PeiServices)->LocatePpi (PeiServices,
  //                                    &AmdSocIdCheckPpi,
  //                                    0,
  //                                    NULL,
  //                                    &AmdSocIdPpiPtr);
  //if (EFI_SUCCESS != Status) {
  //  // return error message.
  //  return Status;
  //}
  //Status = AmdSocIdPpiPtr->CheckSocId (PeiServices, SocId);
  //if (Status != EFI_SUCCESS) {
  //  return EFI_NOT_FOUND;
  //}

  StdHeader = NULL;
  AGESA_TESTPOINT (TpProcMemAmdMemAuto, StdHeader);
  //----------------------------------------------------------------
  //
  // Initialize Pool Pointer table - keep track of pointers allocated form the pool
  //
  //----------------------------------------------------------------
  Status = (**PeiServices).InstallPpi (PeiServices, &mMemPolPtrTablePiList);
  if (EFI_SUCCESS != Status) {
    return Status;
  }
  //----------------------------------------------------------------
  //
  //            Allocate space for data structure
  //
  //----------------------------------------------------------------
  Status = (*PeiServices)->AllocatePool (
                           PeiServices,
                           sizeof (MEM_DATA_STRUCT),
                           &MemDataStructure
                           );
  if (EFI_SUCCESS != Status) {
    return Status;
  }
  //----------------------------------------------------------------
  //
  //            Save allocate space for data structure
  //
  //----------------------------------------------------------------
  Status = (*PeiServices)->LocatePpi (PeiServices,
                                    &gAmdMemoryPoolPointerTablePpiGuid,
                                    0,
                                    NULL,
                                    &AmdPoolTablePtr);
  if (EFI_SUCCESS != Status) {
    return Status;
  }
  AmdPoolTablePtr->PtrTable[AMD_MEM_DATA_STRUCT_POOL_PTR] = (UINT32*)MemDataStructure;

  //----------------------------------------------------------------
  //
  //            Initialize PCDs values in memory data structure
  //
  //----------------------------------------------------------------

  // Initialize Parameters
  // MemMainInitParameters(MemDataStructure);

  //----------------------------------------------------------------
  //
  //            Initialize Memory
  //
  //----------------------------------------------------------------
  IDS_HDT_CONSOLE (MAIN_FLOW, "Memory Driver\n");

  //----------------------------------------------------------------
  //
  //            Initiate Program specific Memory initialization
  //
  //----------------------------------------------------------------
  Status = (**PeiServices).InstallPpi (PeiServices, &mInitializeAgesaMemoryPiList);
  if (EFI_SUCCESS != Status) {
    return Status;
  }
  //----------------------------------------------------------------
  //
  //            De-allocate space for data structure
  //
  //----------------------------------------------------------------
  AmdPoolTablePtr->PtrTable[AMD_MEM_DATA_STRUCT_POOL_PTR] = (UINT32*)NULL;
  //----------------------------------------------------------------
  //
  //          Indicate that Memory initialization complete
  //
  //----------------------------------------------------------------
  Status = (**PeiServices).InstallPpi (PeiServices, &mMemoryInitCompleteiList);
  if (EFI_SUCCESS != Status) {
    return Status;
  }
  AGESA_TESTPOINT (TpProcMemEnd, StdHeader);
  return (Status);
}


