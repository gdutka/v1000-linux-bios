/* $NoKeywords:$ */
/**
 * @file
 *
 * Config Fch SATA controller
 *
 * Init SATA features.
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
#include "Filecode.h"

#define FILECODE PROC_FCH_SANDSTONE_SSSATA_SSSATAENV_FILECODE

/**
 * FchInitEnvSata - Config SATA controller before PCI emulation
 *
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
FchInitEnvSata (
  IN  VOID     *FchDataPtr
  )
{
  FCH_DATA_BLOCK         *LocalCfgPtr;
  AMD_CONFIG_PARAMS      *StdHeader;

  LocalCfgPtr = (FCH_DATA_BLOCK *) FchDataPtr;
  StdHeader = LocalCfgPtr->StdHeader;

  AGESA_TESTPOINT (TpFchInitEnvSata, NULL);

  if ( LocalCfgPtr->Sata.SataMode.SataEnable == 0 ) {
    FchSSSataDisableSata (0, FchDataPtr);
    return;                                                //return if SATA controller is disabled.
  }

  SataEnableWriteAccess (0, LocalCfgPtr);

  FchInitEnvProgramSata (0, FchDataPtr);                  // Init Sata parameters

  //
  // Call Sub-function for each Sata mode
  //
  if (( LocalCfgPtr->Sata.SataClass == SataAhci7804) || (LocalCfgPtr->Sata.SataClass == SataAhci )) {
    FchInitEnvSataAhci ( 0, FchDataPtr );
  }

  if ( LocalCfgPtr->Sata.SataClass == SataRaid) {
    FchInitEnvSataRaid ( 0, FchDataPtr );
  }

  //
  // SATA IRQ Resource
  //
  SataSetIrqIntResource (LocalCfgPtr, StdHeader);

  SataDisableWriteAccess (0, LocalCfgPtr);

  FchSSSataAutoShutdownController (0, FchDataPtr);        // check connection after FchInitEnvProgramSata
}


