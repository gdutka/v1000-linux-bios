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
#define FILECODE PROC_FCH_SANDSTONE_SSINTERFACE_SSFCHINITENV_FILECODE

extern FCH_INTERFACE     FchInterfaceDefault;

FCH_TASK_ENTRY *FchInitEnvTasksTable[] = {
        &FchInitEnvHwAcpiP,
        &FchInitEnvHwAcpi,
        &FchInitEnvSpi,
        &FchInitEnvSd,
        &FchInitEnvUsbXhci,
        &FchInitEnvSata,
        &FchInitEnvXGbe,
        &FchInitEnvEmmc,
        &FchInitEnvAb,
        NULL
};

AGESA_STATUS
FchInitEnv (
  IN       FCH_DATA_BLOCK            *FchDataBlock
  );

/**
 * FchInitEnv - Config Fch before PCI emulation
 *
 *
 *
 * @param[in] FchDataBlock Fch configuration structure pointer
 *
 */
AGESA_STATUS
FchInitEnv (
  IN       FCH_DATA_BLOCK            *FchDataBlock
  )
{
  AGESA_STATUS        Status;


  // Override internal data with IDS (Optional, internal build only)
  //IDS_OPTION_CALLOUT (IDS_CALLOUT_FCH_INIT_ENV, FchParams, FchParams->StdHeader);

  //AgesaFchOemCallout (FchParams);
  Status = FchTaskLauncher (&FchInitEnvTasksTable[0], FchDataBlock, TpFchInitEnvDispatching);
  IDS_HDT_CONSOLE (FCH_TRACE, "  FchInitEnv Exit... Status = [0x%x]\n", Status);
  return Status;
}



