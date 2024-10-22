/* $NoKeywords:$ */
/**
 * @file
 *
 * Config Fch SATA controller (Raid mode)
 *
 * Init SATA Raid features.
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
#define FILECODE PROC_FCH_SANDSTONE_SSSATA_SSRAIDENV_FILECODE

extern VOID  FchInitEnvSataRaidProgram  (IN VOID  *FchDataPtr);

//
// Declaration of local functions
//

/**
 * FchInitEnvSataRaid - Config SATA Raid controller before PCI
 * emulation
 *
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
FchInitEnvSataRaid  (
  IN  UINT32   DieBusNum,
  IN  VOID     *FchDataPtr
  )
{
  FCH_DATA_BLOCK         *LocalCfgPtr;
  AMD_CONFIG_PARAMS      *StdHeader;

  LocalCfgPtr = (FCH_DATA_BLOCK *) FchDataPtr;
  StdHeader = LocalCfgPtr->StdHeader;

  //
  // Class code
  //
  FchSmnRW (DieBusNum, FCH_SS_SMN_SATA_NBIF_STRAP13, 0x00, 0x00010400, StdHeader);
  //
  // Device ID
  //
  FchSmnRW (DieBusNum, FCH_SS_SMN_SATA_NBIF_STRAP0, 0xFFFF0000, SANDSTONE_FCH_SATA_RAID_DID, StdHeader);

  //
  // SSID
  //
  if (LocalCfgPtr->Sata.SataRaidSsid != NULL ) {
    //RwPci ((SATA_BUS_DEV_FUN << 16) + FCH_SATA_REG2C, AccessWidth32, 0x00, LocalCfgPtr->Sata.SataAhciSsid, StdHeader);
    FchSmnRW (DieBusNum, FCH_SS_SMN_SATA_PCICFG + 0x4C, 0x00, LocalCfgPtr->Sata.SataRaidSsid, StdHeader);
  }
}

