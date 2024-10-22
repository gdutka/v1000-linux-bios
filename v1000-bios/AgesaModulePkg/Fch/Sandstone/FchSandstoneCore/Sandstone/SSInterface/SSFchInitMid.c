/* $NoKeywords:$ */
/**
 * @file
 *
 * FCH Initialization.
 *
 * Init IOAPIC/IOMMU/Misc NB features.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: FCH
 * @e \$Revision: 309083 $   @e \$Date: 2014-12-09 09:28:24 -0800 (Tue, 09 Dec 2014) $
 *
 */
/*;********************************************************************************
;
; Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
;
;******************************************************************************
;*/

#include "FchPlatform.h"
#include "SSFchTaskLauncher.h"
#include "Filecode.h"
#define FILECODE PROC_FCH_SANDSTONE_SSINTERFACE_SSFCHINITMID_FILECODE

FCH_TASK_ENTRY  *FchInitMidTasksTable[] = {
        &FchInitMidHwAcpi,
        &FchInitMidUsbXhci,
        &FchInitMidEmmc,
        NULL
      };

AGESA_STATUS
FchInitMid (
  IN      FCH_DATA_BLOCK     *FchParams
  );

/**
 *  FchInitMid - Config Fch after PCI emulation
 *
 *
 *
 * @param[in] FchParams Fch configuration structure pointer.
 *
 */
AGESA_STATUS
FchInitMid (
  IN      FCH_DATA_BLOCK     *FchParams
  )
{
  AGESA_STATUS        Status;

  IDS_HDT_CONSOLE (FCH_TRACE, "  FchInitMid Enter... \n");
  Status = FchTaskLauncher (&FchInitMidTasksTable[0], FchParams, TpFchInitMidDispatching);
  IDS_HDT_CONSOLE (FCH_TRACE, "  FchInitMid Exit... Status = [0x%x]\n", Status);
  return Status;
}


