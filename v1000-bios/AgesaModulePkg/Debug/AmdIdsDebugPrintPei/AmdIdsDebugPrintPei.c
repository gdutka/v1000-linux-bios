/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD HDTOUT PEI Driver
 *
 * Contains code that initialize the HDTOUT register and buffer mode
 * and public a PPI for HDTOUT print service
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  HDTOUT
 * @e \$Revision: 317842 $   @e \$Date: 2015-04-29 15:06:52 +0800 (Wed, 29 Apr 2015) $
 *
 */
/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */

#include <PiPei.h>
#include <Ppi/AmdIdsDebugPrintPpi.h>
#include <Library/AmdIdsDebugPrintLib.h>
#include <Library/DebugLib.h>
#include <Library/AmdBaseLib.h>
#include <Library/PrintLib.h>
#include <Filecode.h>

#define FILECODE  DEBUG_AMDIDSDEBUGPRINTPEI_AMDIDSDEBUGPRINTPEI_FILECODE


#define MAX_LOCAL_BUFFER_SIZE   512


STATIC AMD_IDS_DEBUG_PRINT_PPI mAmdIdsDebugPrintPpi = {
  &AmdIdsDebugPrintf
};


STATIC EFI_PEI_PPI_DESCRIPTOR mAmdIdsDebugPrintPpiList =
{
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gAmdIdsDebugPrintPpiGuid,
  &mAmdIdsDebugPrintPpi
};

/*++

Routine Description:

  AmdIdsDebugPrint Driver Entry. Initialize the Debug Print and buffer mode
  and public a PPI for Debug print print service

Arguments:

Returns:

  EFI_STATUS

--*/
EFI_STATUS
EFIAPI
AmdIdsDebugPrintPeiDriverEntry (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS            Status = EFI_SUCCESS;

  if (FeaturePcdGet (PcdAmdIdsDebugPrintEnable) == TRUE) {
    if (PcdGetBool (PcdAmdIdsDebugPrintHdtOutEnable) == TRUE) {
      //HDTOUT Initial include buffer mode
      AmdIdsDpHdtOutBufferModeInit ();
    }
    //Install PPI
    Status = (**PeiServices).InstallPpi (PeiServices, &mAmdIdsDebugPrintPpiList);
    ASSERT_EFI_ERROR (Status);
  }
  return (Status);
}


