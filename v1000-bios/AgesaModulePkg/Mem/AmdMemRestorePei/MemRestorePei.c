/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Memory Restore API for DDR4, and related functions.
 *
 * Contains code that initializes memory restore feature
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  MEM
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

  MemRestorePei.c

Abstract:
--*/
#include "Uefi.h"
#include "PiPei.h"

#include "AGESA.h"
#include "Library/IdsLib.h"
#include "Library/AmdBaseLib.h"
#include "Ppi/AmdMemPpi.h"
#include "Library/MemRestore.h"
#include "Library/FchBaseLib.h"
#include "MemRestorePei.h"
#include "Filecode.h"

#define FILECODE        MEM_AMDMEMRESTOREPEI_MEMRESTOREPEI_FILECODE

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
EFI_STATUS
EFIAPI
MemRestoreS3CompleteCallback (
  IN  EFI_PEI_SERVICES                **PeiServices,
  IN  EFI_PEI_NOTIFY_DESCRIPTOR       *NotifyDesc,
  IN  VOID                            *InvokePpi
  );

/*----------------------------------------------------------------------------------------
 *                           G L O B A L   V A R I A B L E S
 *----------------------------------------------------------------------------------------
 */
EFI_PEI_NOTIFY_DESCRIPTOR   mMemRestoreS3CompleteCallback = {
  (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gEfiEndOfPeiSignalPpiGuid,
  MemRestoreS3CompleteCallback
};

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
/*---------------------------------------------------------------------------------------*/
/**
 * Mark the S3 completion by memory context restore
 *
 * This function marks the S3 completion by memory context restore
 *
 */
EFI_STATUS
EFIAPI
MemRestoreS3CompleteCallback (
  IN  EFI_PEI_SERVICES                **PeiServices,
  IN  EFI_PEI_NOTIFY_DESCRIPTOR       *NotifyDesc,
  IN  VOID                            *InvokePpi
  )
{
  AmdMemRestoreSignOff ();

  return EFI_SUCCESS;
}

/**
 *---------------------------------------------------------------------------------------
 *
 *  Entry for AmdMemRestorePei
 *
 *    @retval          EFI_STATUS
 *
 *---------------------------------------------------------------------------------------
 */
EFI_STATUS
EFIAPI
AmdMemRestorePeiEntry (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS                     Status;

  IDS_HDT_CONSOLE (MAIN_FLOW, "\tAmdMemRestorePei Entry\n");

  if (FchReadSleepType () == 0x03) {

    IDS_HDT_CONSOLE (MAIN_FLOW, "\t[AMD MemRestore] Install callback to mark S3 completion by memory context restore\n");

    Status = (**PeiServices).NotifyPpi (
                               PeiServices,
                               &mMemRestoreS3CompleteCallback
                               );
    if (EFI_ERROR (Status)) {
      IDS_HDT_CONSOLE_PSP_TRACE ("\tError Installing callback\n");
      return Status;
    }
  }

  IDS_HDT_CONSOLE (MAIN_FLOW, "\tAmdMemRestorePei Exit\n");

  return EFI_SUCCESS;
}

