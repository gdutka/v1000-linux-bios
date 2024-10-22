/* $NoKeywords:$ */
/**
 * @file
 *
 * Config Fch xGbE controller
 *
 * Init USB features.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: FCH
 * @e \$Revision: 312516 $   @e \$Date: 2015-02-06 17:37:45 -0800 (Fri, 06 Feb 2015) $
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
#define FILECODE FCH_SANDSTONE_FCHSANDSTONECORE_SANDSTONE_SSXGBE_SSXGBEENV_FILECODE


/**
 * FchInitEnvXGbe - Config xGbE controller before PCI emulation
 *
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
FchInitEnvXGbe (
  IN  VOID     *FchDataPtr
  )
{
  FCH_DATA_BLOCK         *LocalCfgPtr;
  AMD_CONFIG_PARAMS      *StdHeader;
  UINT8                  Port;
  UINT32                 SystemPort;
  UINT32                 XgbeNbifStrap0;
  UINT32                 XgbeNbifStrap1;
  FCH_XGBE_PORT          *FchXgbePort;
  FCH_XGBE_MAC           *FchXgbePortMac;

  LocalCfgPtr = (FCH_DATA_BLOCK *) FchDataPtr;
  StdHeader = LocalCfgPtr->StdHeader;
  FchXgbePort = &LocalCfgPtr->Xgbe.Port[0];
  FchXgbePortMac = &LocalCfgPtr->Xgbe.Mac[0];

  AGESA_TESTPOINT (TpFchInitEnvXgbe, NULL);
  //PLAT-17946
  FchSmnRead (0, FCH_SS_SMN_XGBE0_NBIF_STRAP0, &XgbeNbifStrap0, NULL);
  FchSmnRead (0, FCH_SS_SMN_XGBE0_NBIF_STRAP0 + 0x200, &XgbeNbifStrap1, NULL);
  if ((((XgbeNbifStrap0 & BIT28) == 0) && ((XgbeNbifStrap1 & BIT28) == 0))  || ((LocalCfgPtr->Xgbe.Port[0].XgbePortConfig == FALSE) && (LocalCfgPtr->Xgbe.Port[1].XgbePortConfig == FALSE))) {
  //  FchSmnRW (0, FCH_NBIFSHUBMM0x00011000_SYSHUB_DS_CTRL_SHUBCLK, ~ (UINT32) (FCH_DMA_CL3_SHUBCLK_DEEPSLEEP_ALLOW_ENABLE), FCH_DMA_CL3_SHUBCLK_DEEPSLEEP_ALLOW_ENABLE, NULL);  //Disable XGbe to pass Os install
  //  FchSmnRW (0, 0x4EC20, 0xFFFF0000, 0x0101, NULL);  //
  }

  for (Port = 0; Port < 4; Port++) {
    //
    // Check if GMAC is enabled
    //
    //PLAT-16038
    if (FchXgbePort->XgbePortConfig == FALSE) {
     FchSmnRW (0, FCH_SS_RCC_DEV1_EPF1_STRAP0 + Port * 0x200, ~ (UINT32) (BIT28), 0, NULL);  //Disable XGbe to pass Os install
    }

    FchSmnRead (0, FCH_SS_SMN_XGBE0_NBIF_STRAP0 + Port * 0x200, &XgbeNbifStrap0, NULL);

    if (XgbeNbifStrap0 & BIT28) {
      SystemPort = LocalCfgPtr->Xgbe.MaxPortNum;
      LocalCfgPtr->Xgbe.MaxPortNum++;
      if (FchXgbePort->XgbePortConfig) {
        FchSsXgbePortPlatformConfig (0, Port, SystemPort, FchXgbePort);
        FchSsXgbePortMacAddress (0, Port, FchXgbePortMac);
        FchSsXgbePortPtpInit (0, Port, FchXgbePort);
      }
    } else {
      FchSsXgbePortClockGate (0, Port);
    }
    FchXgbePort++;
    FchXgbePortMac++;
  }

  FchSsXgbePadEnable (0, &LocalCfgPtr->Xgbe);
}

