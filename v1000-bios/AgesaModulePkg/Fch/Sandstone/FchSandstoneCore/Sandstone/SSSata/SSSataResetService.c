/* $NoKeywords:$ */
/**
 * @file
 *
 * Config Fch Sata controller
 *
 * Init Sata Controller features (PEI phase).
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
#define FILECODE PROC_FCH_SANDSTONE_SSSATA_SSSATARESETSERVICE_FILECODE

/**
 * FchInitResetSataProgram - Config Sata controller during
 * Power-On
 *
 *
 * @param[in] DieBusNum  IOCH bus number on current Die.
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
FchInitResetSataProgram (
  IN  UINT32   DieBusNum,
  IN  VOID     *FchDataPtr
  )
{
  UINT32                    PortNum;
  FCH_RESET_DATA_BLOCK      *LocalCfgPtr;

  LocalCfgPtr = (FCH_RESET_DATA_BLOCK *) FchDataPtr;

  //
  // staggered spin-up step 2 Optional
  //
  for (PortNum = 0; PortNum < SANDSTONE_SATA_PORT_NUM; PortNum++) {
    FchSmnRW (DieBusNum, FCH_SS_SATA_AOAC_CONTROL, ~(UINT32) (BIT0 << PortNum), 0x00, LocalCfgPtr->StdHeader);
  }

  //
  // Enable the SATA controller.
  //
  if ( LocalCfgPtr->FchReset.SataEnable ) {
    FchSmnRW (DieBusNum, FCH_SS_SATA_MISC_CONTROL, 0xfffffffe, 0, LocalCfgPtr->StdHeader);
  } else {
    FchSmnRW (DieBusNum, FCH_SS_SATA_MISC_CONTROL, 0xfffffffe, 0, LocalCfgPtr->StdHeader); //Enable on Reset Phase to make SATA accessible
    return;
  }

  //
  // Put all ports to offline mode
  //
  for (PortNum = 0; PortNum < SANDSTONE_SATA_PORT_NUM; PortNum++) {
    FchSSSataInitPortOffline (DieBusNum, PortNum, FchDataPtr);
  }
}



