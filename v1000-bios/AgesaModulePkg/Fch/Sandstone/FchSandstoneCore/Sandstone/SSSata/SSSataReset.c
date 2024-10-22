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
#define FILECODE PROC_FCH_SANDSTONE_SSSATA_SSSATARESET_FILECODE

/**
 * FchInitResetSata - Config Sata controller during Power-On
 *
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
FchInitResetSata (
  IN  VOID     *FchDataPtr
  )
{
  UINT32       SataRegData;
  FCH_RESET_DATA_BLOCK *LocalPtr;
  LocalPtr = (FCH_RESET_DATA_BLOCK *)FchDataPtr;
  AGESA_TESTPOINT (TpFchInitResetSata, NULL);

  if (LocalPtr->FchReset.SataEnable) {
    FchSmnRead (0, FCH_SS_SATA_AOAC_CONTROL, &SataRegData, NULL);
    if (SataRegData & BIT8) {
      FchSmnRW (0, FCH_SS_SATA_AOAC_CONTROL, ~(UINT32)(BIT8 + BIT11), 0, NULL);
      FchSmnRW (0, FCH_SS_SMN_MISC_BASE + 0x4C, ~(UINT32)BIT16, BIT16, NULL);
      FchPciReset (NULL);
    }
  }
  FchInitResetSataProgram (0, FchDataPtr );
}


