/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Memory Context Save/Restore driver
 *
 * Contains code that performs memory context save/restore
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
#include "Uefi.h"
#include "PiDxe.h"
#include "Library/UefiBootServicesTableLib.h"

#include "AGESA.h"
#include "Library/IdsLib.h"
#include "Library/AmdBaseLib.h"
#include "Library/AmdHeapLib.h"
#include "Library/MemRestore.h"
#include "MemRestoreDxe.h"
#include "Filecode.h"

#include <IndustryStandard/SmBios.h>
#include <Protocol/Smbios.h>

#define FILECODE        MEM_AMDMEMRESTOREDXE_MEMRESTOREDXE_FILECODE

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                           G L O B A L   V A R I A B L E S
 *----------------------------------------------------------------------------------------
 */

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
 * Mark the boot completion by memory context restore
 *
 * This function marks the boot completion by memory context restore
 *
 *  Parameters:
 *    @param[in]     Event
 *    @param[in]     *Context
 *
 *    @retval        VOID
 *
 */
VOID
EFIAPI
MemRestoreBootCompleteCallback (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
{
  EFI_STATUS                  Status;
  EFI_SMBIOS_TABLE_HEADER     *Record;
  EFI_SMBIOS_HANDLE           SmbiosHandle;
  EFI_SMBIOS_PROTOCOL         *Smbios;
  SMBIOS_TABLE_TYPE16         *Type16Record;
  BOOLEAN                     EccCapable;

  //Make sure the hook ONLY called one time.
  if (Event != NULL) {
    gBS->CloseEvent (Event);
  }

  //Ecc WA
  EccCapable = FALSE;
  SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
  Status = gBS->LocateProtocol (
                  &gEfiSmbiosProtocolGuid,
                  NULL,
                  (VOID **) &Smbios
                  );

  if (!EFI_ERROR(Status)) {
    do {
      Status = Smbios->GetNext (Smbios, &SmbiosHandle, NULL, &Record, NULL);
      if (EFI_ERROR(Status)) {
        break;
      }

      if (Record->Type == EFI_SMBIOS_TYPE_PHYSICAL_MEMORY_ARRAY) {
        Type16Record = (SMBIOS_TABLE_TYPE16 *) Record;

        if (Type16Record->MemoryErrorCorrection == 0x06) {
          EccCapable = TRUE;
          break;
        }
      }
    } while(Status == EFI_SUCCESS);

    if (EccCapable == TRUE) {
      IDS_HDT_CONSOLE (MAIN_FLOW, "\t[AMD MemRestore] Feature disabled due to EccCapable\n");
      AmdMemRestoreDiscardCurrentMemContext ();
      return;
    }
  } else {
    ASSERT (FALSE);
  }

  AmdMemRestoreSignOff ();
}

/**
 *---------------------------------------------------------------------------------------
 *
 *  Entry for DxeAmdMemRestoreEntry
 *
 *    @retval          EFI_STATUS
 *
 *---------------------------------------------------------------------------------------
 */
EFI_STATUS
EFIAPI
DxeAmdMemRestoreEntry (
  IN       EFI_HANDLE        ImageHandle,
  IN       EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS                     Status;
  EFI_EVENT                      ReadyToBootEvent;

  IDS_HDT_CONSOLE (MAIN_FLOW, "\tDxeAmdMemRestoreEntry Entry\n");

  IDS_HDT_CONSOLE (MAIN_FLOW, "\t[AMD MemRestore] Install callback to mark boot completion by memory context restore\n");

  Status = gBS->CreateEventEx (
               EVT_NOTIFY_SIGNAL,
               TPL_NOTIFY,
               MemRestoreBootCompleteCallback,
               NULL,
               &gEfiEventReadyToBootGuid,
               &ReadyToBootEvent
             );
  if (EFI_ERROR (Status)) {
    IDS_HDT_CONSOLE_PSP_TRACE ("\tError Installing callback\n");
    return Status;
  }

  IDS_HDT_CONSOLE (MAIN_FLOW, "\tDxeAmdMemRestoreEntry Exit\n");
  return EFI_SUCCESS;
}


/*----------------------------------------------------------------------------------------
 *                          L O C A L    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

