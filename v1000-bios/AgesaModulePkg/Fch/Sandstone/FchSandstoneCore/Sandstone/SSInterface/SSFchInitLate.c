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
#define FILECODE PROC_FCH_SANDSTONE_SSINTERFACE_SSFCHINITLATE_FILECODE

FCH_TASK_ENTRY  *FchInitLateTasksTable[] = {
        &FchInitLateSpi,
        &FchInitLateSata,
        &FchInitLateHwAcpi,
        &FchInitLateEmmc,
        NULL
      };

AGESA_STATUS
FchInitLate (
  IN      FCH_DATA_BLOCK     *LateParams
  );

/*----------------------------------------------------------------------------------------*/
/**
 *  FchInitLate - Prepare Fch to boot to OS.
 *
 *
 *
 * @param[in] LateParams
 *
 */
AGESA_STATUS
FchInitLate (
  IN      FCH_DATA_BLOCK     *LateParams
  )
{
  AGESA_STATUS        Status;

  IDS_HDT_CONSOLE (FCH_TRACE, "  FchInitLate Enter... \n");
  Status = FchTaskLauncher (&FchInitLateTasksTable[0], LateParams, TpFchInitLateDispatching);
  IDS_HDT_CONSOLE (FCH_TRACE, "  FchInitLate Exit... Status = [0x%x]\n", Status);
  return Status;
}


