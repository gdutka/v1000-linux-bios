/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Memory API for ZP DDR4, and related functions.
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

  MemSmbiosV2Pei.c

Abstract:
--*/
#include "Uefi.h"
#include "PiPei.h"
#include "AGESA.h"
#include "Library/IdsLib.h"
#include "Library/AmdBaseLib.h"
#include "MemSmbiosV2Pei.h"
#include <Addendum/Apcb/Inc/RV/APOB.h>
#include "Library/AmdPspApobLib.h"
#include "Ppi/AmdMemPpi.h"
#include "Library/MemChanXLat.h"
#include "Library/MemSmbiosV2Lib.h"
#include <Library/AmdCapsuleLib.h>
#include "Library/FchBaseLib.h"
#include "Filecode.h"

#define FILECODE        MEM_AMDMEMSMBIOSV2PEI_MEMSMBIOSV2PEI_FILECODE

extern EFI_GUID gAmdMemChanXLatPpiGuid;

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
/**
 *---------------------------------------------------------------------------------------
 *
 *  Entry for AmdMemSmbiosV2Pei
 *
 *    @retval          EFI_STATUS
 *
 *---------------------------------------------------------------------------------------
 */
EFI_STATUS
EFIAPI
PeiAmdMemSmbiosV2Entry (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS                     Status;
  AMD_CONFIG_PARAMS              StdHeader;
  EFI_STATUS                     ApobStatus;
  APOB_TYPE_HEADER               *ApobSmbiosInfo;
  AMD_MEMORY_CHANNEL_XLAT_PPI    *MemChanXLatPpi;
  HOST_TO_APCB_CHANNEL_XLAT      *MemChanXLatTab;

  if (FchReadSleepType () != 0x03 && AmdCapsuleGetStatus () == FALSE) {
    IDS_HDT_CONSOLE (MAIN_FLOW, "  AmdMemSmbiosV2Pei Entry\n");
    ApobSmbiosInfo = NULL;
    ApobStatus = AmdPspGetApobEntryInstance (APOB_SMBIOS, APOB_MEM_SMBIOS_TYPE, 0, FALSE, &ApobSmbiosInfo);
    if (ApobStatus != EFI_SUCCESS) {
      IDS_HDT_CONSOLE (MAIN_FLOW, "  No SMBIOS data found in APOB\n");
      return ApobStatus;
    }

    MemChanXLatTab = NULL;
    Status = (*PeiServices)->LocatePpi (
                PeiServices,
                &gAmdMemChanXLatPpiGuid,
                0,
                NULL,
                (VOID**)&MemChanXLatPpi
                );
    if (EFI_SUCCESS == Status) {
      IDS_HDT_CONSOLE (MAIN_FLOW, "  Memory Channel Translation Table located\n");
      MemChanXLatTab = MemChanXLatPpi->XLatTab;
    }

    if (AGESA_SUCCESS != GetMemSmbiosV2 (DDR4_TECHNOLOGY, 1, ApobSmbiosInfo, MemChanXLatTab, &StdHeader, NULL)) {
      ASSERT (FALSE);
    }

    IDS_HDT_CONSOLE (MAIN_FLOW, "  AmdMemSmbiosV2Pei Exit\n");
  }

  return EFI_SUCCESS;
}

