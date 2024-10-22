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

#include <PiDxe.h>
#include <Library/DebugLib.h>
#include <Protocol/AmdIdsDebugPrintProtocol.h>
#include <Library/AmdIdsDebugPrintLib.h>
#include <Library/AmdBaseLib.h>
#include <Library/PrintLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Filecode.h>

#define FILECODE  DEBUG_AMDIDSDEBUGPRINTDXE_AMDIDSDEBUGPRINTDXE_FILECODE


AMD_IDS_DEBUG_PRINT_PROTOCOL AmdIdsDebugPrintProtocol = {
  &AmdIdsDebugPrintf
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
AmdIdsDebugPrintDxeDriverEntry (
  IN EFI_HANDLE ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  EFI_STATUS            Status;
  EFI_HANDLE            Handle;

  Status = EFI_SUCCESS;
  Handle = NULL;
  if (FeaturePcdGet (PcdAmdIdsDebugPrintEnable) == TRUE) {
  //Install Protocol
    Status = gBS->InstallProtocolInterface (
                    &Handle,
                    &gAmdIdsDebugPrintProtocolGuid,
                    EFI_NATIVE_INTERFACE,
                    &AmdIdsDebugPrintProtocol
                    );
    ASSERT_EFI_ERROR (Status);
  }
  return (Status);
}


