/* $NoKeywords:$ */
/**
 * @file
 *
 * FCH Init during Power-On Reset
 *
 * Prepare FCH environment during power on stage
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

#include    "FchPlatform.h"
#include    "SSFchTaskLauncher.h"
#include    "Filecode.h"
#define FILECODE PROC_FCH_SANDSTONE_SSINTERFACE_SSFCHINITRESET_FILECODE

extern FCH_RESET_INTERFACE   FchResetInterfaceDefault;
FCH_TASK_ENTRY *FchInitResetTaskTable[] = {
  FchInitResetEmmc,
  FchInitResetHwAcpiP,
  FchInitResetAb,
  FchInitResetSpi,
  FchInitResetHwAcpi,
  FchInitResetSata,
  FchInitResetLpc,
  FchInitResetEspi,
  FchInitResetXhci,
  NULL
};

/**
 * FchInitReset - Config Fch during power on stage.
 *
 *
 *
 * @param[in] FchParams Fch configuration structure pointer
 *
 */
AGESA_STATUS
FchInitReset (
  FCH_RESET_DATA_BLOCK         *FchParams
  )
{
  // Override internal data with IDS (Optional, internal build only)
  //IDS_OPTION_CALLOUT (IDS_CALLOUT_FCH_INIT_RESET, FchParams, &ResetParams->StdHeader);

  //AgesaFchOemCallout (FchParams);
  return FchTaskLauncher (&FchInitResetTaskTable[0], FchParams, TpFchInitResetDispatching);
}



