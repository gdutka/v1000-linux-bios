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
#define FILECODE PROC_FCH_SANDSTONE_SSSATA_SSSATALATE_FILECODE

/**
 * FchInitLateSata - Prepare SATA controller to boot to OS.
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
FchInitLateSata (
  IN  VOID     *FchDataPtr
  )
{
  FCH_DATA_BLOCK         *LocalCfgPtr;
  AMD_CONFIG_PARAMS      *StdHeader;

  LocalCfgPtr = (FCH_DATA_BLOCK *) FchDataPtr;
  StdHeader = LocalCfgPtr->StdHeader;

  AGESA_TESTPOINT (TpFchInitLateSata, NULL);
  //
  //Return immediately is sata controller is not enabled
  //
  if ( LocalCfgPtr->Sata.SataMode.SataEnable == 0 ) {
    return;
  }

  //
  // Set Sata PCI Configuration Space Write enable
  //
  SataEnableWriteAccess (0, LocalCfgPtr);
  //
  // Call Sub-function for each Sata mode
  //
  if (( LocalCfgPtr->Sata.SataClass == SataAhci7804) || (LocalCfgPtr->Sata.SataClass == SataAhci )) {
    FchInitLateSataAhci (0, LocalCfgPtr );
  }

  if ( LocalCfgPtr->Sata.SataClass == SataRaid) {
    FchInitLateSataRaid (0, LocalCfgPtr );
  }

  FchInitLateProgramSataRegs  (0, LocalCfgPtr );

  //
  // Set Sata PCI Configuration Space Write disable
  //
  SataDisableWriteAccess (0, LocalCfgPtr);
}


