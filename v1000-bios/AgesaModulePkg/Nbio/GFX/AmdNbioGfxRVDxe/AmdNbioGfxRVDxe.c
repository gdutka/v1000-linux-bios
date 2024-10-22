/* $NoKeywords:$ */
/**
 * @file
 *
 * AmdNbioGfxRVDxe Driver
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  AmdNbioGfxRVDxe
 * @e \$Revision: 312065 $   @e \$Date: 2015-01-30 04:23:05 -0600 (Fri, 30 Jan 2015) $
 *
 */
/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */
#include <PiDxe.h>
#include <Filecode.h>
#include <Library/AmdBaseLib.h>
#include <Library/IdsLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/AmdNbioPcieServicesProtocol.h>
#include <Guid/GnbPcieInfoHob.h>
#include <Library/UefiLib.h>
#include <Library/HobLib.h>
#include <Library/BaseMemoryLib.h>
#include <Guid/HobList.h>
#include <Guid/GnbIntegratedSysInfoHob.h>

#define FILECODE NBIO_GFX_AMDNBIOGFXRVDXE_AMDNBIOGFXRVDXE_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *           T A B L E S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */


EFI_STATUS
EFIAPI
AmdNbioGfxRVDxeEntry (
  IN       EFI_HANDLE         ImageHandle,
  IN       EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS                           Status;
  EFI_HANDLE                           Handle;
  NBIO_Frame_Buffer_DATA_HOB           *FrambufferHob;
  UINT64                               DestinationAddress;
  AGESA_TESTPOINT (TpNbioGfxDxeEntry, NULL);
  IDS_HDT_CONSOLE (MAIN_FLOW, "AmdNbioGfxRVDxeEntry\n");

  Status = EFI_SUCCESS;
  Handle = NULL;

  //
  // Get information data from HOB.
  //
  Status = EfiGetSystemConfigurationTable (&gEfiHobListGuid, (VOID**)&FrambufferHob);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  FrambufferHob = GetNextGuidHob (&gGnbIntegratedSysInfoHobGuid, FrambufferHob);
  if (FrambufferHob == NULL) {
    return EFI_NOT_FOUND;
  }

  DestinationAddress = (UINT64)((FrambufferHob->UMAInfo.Base + FrambufferHob->UMAInfo.Size) - sizeof(ATOM_FUSION_SYSTEM_INFO_V9));
  IDS_HDT_CONSOLE (MAIN_FLOW, "DXE FrambufferHob->UMAInfo.Base: %lx\n", FrambufferHob->UMAInfo.Base);
  IDS_HDT_CONSOLE (MAIN_FLOW, "DXE FrambufferHob->UMAInfo.Size: %lx\n", FrambufferHob->UMAInfo.Size);

  CopyMem ((VOID*)DestinationAddress,
    &(FrambufferHob->AtomFusionSystemInfoV9),
    sizeof(ATOM_FUSION_SYSTEM_INFO_V9));

  IDS_HDT_CONSOLE (MAIN_FLOW, "Real Integrated Table: %lx\n", (UINT64)((FrambufferHob->UMAInfo.Base + FrambufferHob->UMAInfo.Size) - sizeof(ATOM_FUSION_SYSTEM_INFO_V9)));

  IDS_HDT_CONSOLE (MAIN_FLOW, "AmdNbioGfxRVDxeExit\n");
  AGESA_TESTPOINT (TpNbioGfxDxeExit, NULL);
  return Status;
}

