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
#define FILECODE PROC_FCH_TAISHAN_TSEMMC_TSEMMCLATE_FILECODE

/**
 * FchInitLateEmmc - Config eMMC controller before OS Boot
 * emulation
 *
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
FchInitLateEmmc (
  IN  VOID     *FchDataPtr
  )
{
  FCH_DATA_BLOCK         *LocalCfgPtr;
  AMD_CONFIG_PARAMS      *StdHeader;

  LocalCfgPtr = (FCH_DATA_BLOCK *) FchDataPtr;
  StdHeader = LocalCfgPtr->StdHeader;

  //if ( LocalCfgPtr->Misc.S3Resume ) {
    if ( LocalCfgPtr->FchRunTime.FchDeviceEnableMap & BIT28 ) { //eMMC is enabled for OS
      //Configure all IoMux and clear status here
      RwPci (((LPC_BUS_DEV_FUN << 16) + FCH_LPC_REG44), AccessWidth32, 0, 0,StdHeader);
      RwPci (((LPC_BUS_DEV_FUN << 16) + FCH_LPC_REG48), AccessWidth32, 0xFF00, 0,StdHeader);
      if (LocalCfgPtr->Emmc.EmmcEnable <= 5 ) { //PLAT-37137
        ProgramLpcSdPins (TRUE);
      } else {
        ProgramLpcEmmcPins (TRUE);
      }
    } else {
      //if ( LocalCfgPtr->Emmc.EmmcD3Support ) { //eMMC D3 is enabled for OS
      if (FchCheckPicasso ()) {
        if (LocalCfgPtr->Misc.FchCsSupport.FchModernStandby == FALSE) {          
          if ((( LocalCfgPtr->FchRunTime.FchDeviceEnableMap & (BIT7 + BIT8 + BIT11 + BIT12 + BIT16 + BIT26))) == 0)  { //No I2C/Uart is Enabled
            FchAoacPowerOnDev (FCH_AOAC_AMBA, 0);
            FchAoacPowerOnDev (FCH_AOAC_SD1, 0);
          }
        }
      }
      //}
    }
  //}
}


