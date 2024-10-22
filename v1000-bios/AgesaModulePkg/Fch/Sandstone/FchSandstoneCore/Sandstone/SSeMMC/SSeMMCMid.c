/* $NoKeywords:$ */
/**
 * @file
 *
 * Config Fch eMMC controller
 *
 * Init USB features.
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
#include "Filecode.h"
#define FILECODE PROC_FCH_TAISHAN_TSEMMC_TSEMMCMID_FILECODE

/**
 * FchInitMidEmmc - Config eMMC controller after PCI emulation
 *
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
FchInitMidEmmc (
  IN  VOID     *FchDataPtr
  )
{
  FCH_DATA_BLOCK         *LocalCfgPtr;

  LocalCfgPtr = (FCH_DATA_BLOCK *) FchDataPtr;

  AGESA_TESTPOINT (TpFchInitMidEmmc, NULL);

  if ( LocalCfgPtr->Emmc.EmmcEnable ) {
    if ( LocalCfgPtr->Emmc.EmmcBoot ) {
      if (LocalCfgPtr->Emmc.EmmcEnable <= 5 ) {
        ProgramLpcSdPins (TRUE);
      } else {
        ProgramLpcEmmcPins (TRUE);
      }
    }
  }
}


