/* $NoKeywords:$ */
/**
 * @file
 *
 * Config Fch ESPI controller
 *
 * Init ESPI Controller features (PEI phase).
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: FCH
 * @e \$Revision: 309083 $   @e \$Date: 2014-12-09 09:28:24 -0800 (Tue, 09 Dec 2014) $
 *
 */
/*
*****************************************************************************
*
* Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
*
*******************************************************************************
*/
#include "FchPlatform.h"
#define FILECODE FCH_SANDSTONE_FCHSANDSTONECORE_SANDSTONE_SSESPI_SSESPIRESET_FILECODE

/**
 * FchInitResetEspi - Config ESPI controller during Power-On
 *
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
FchInitResetEspi (
  IN  VOID     *FchDataPtr
  )
{
  FCH_RESET_DATA_BLOCK      *LocalCfgPtr;

  LocalCfgPtr = (FCH_RESET_DATA_BLOCK *) FchDataPtr;

  AGESA_TESTPOINT (TpFchInitResetEspi, NULL);

  if (LocalCfgPtr->EspiEnable) {
    FchAoacPowerOnDev (FCH_AOAC_ESPI, 1);
    FchinitSSEspiIoMux ();
  } else {
    FchAoacPowerOnDev (FCH_AOAC_ESPI, 0);
    return;
  }

  if (LocalCfgPtr->EspiEc0Enable) {
    FchinitSSEspiEc0 (LocalCfgPtr);
  }

  if (LocalCfgPtr->EspiIo80Enable) {
    FchinitSSEspiEnableIo80 (TRUE);
  }

  if (LocalCfgPtr->EspiKbc6064Enable) {
    FchinitSSEspiEnableKbc6064 (TRUE);
  } else {
    FchinitSSEspiEnableKbc6064 (FALSE);
  }
}


