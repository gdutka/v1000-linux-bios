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
#define FILECODE PROC_FCH_SANDSTONE_SSSATA_SSSATAMID_FILECODE

/**
 * FchInitMidSata - Config SATA controller after PCI emulation
 *
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
FchInitMidSata (
  IN  VOID     *FchDataPtr
  )
{
  FCH_DATA_BLOCK         *LocalCfgPtr;
  AMD_CONFIG_PARAMS      *StdHeader;

  LocalCfgPtr = (FCH_DATA_BLOCK *) FchDataPtr;
  StdHeader = LocalCfgPtr->StdHeader;

  AGESA_TESTPOINT (TpFchInitMidSata, NULL);

  if ( LocalCfgPtr->Sata.SataMode.SataEnable == 0 ) {
    return;                                                ///return if SATA controller is disabled.
  }

  //
  // Set Sata PCI Configuration Space Write enable
  //
  SataEnableWriteAccess (0, LocalCfgPtr);

  FchInitMidProgramSataRegs (0, LocalCfgPtr );

  //
  // Call Sub-function for each Sata mode
  //
  if (( LocalCfgPtr->Sata.SataClass == SataAhci7804) || (LocalCfgPtr->Sata.SataClass == SataAhci )) {
    FchInitMidSataAhci (0, LocalCfgPtr );
  }

  if ( LocalCfgPtr->Sata.SataClass == SataRaid) {
    FchInitMidSataRaid (0, LocalCfgPtr );
  }

  //
  // Set Sata PCI Configuration Space Write disable
  //
  SataDisableWriteAccess (0, StdHeader);
}

/**
 * SataSetDeviceNumMsi - Program Sata controller support device
 * number cap & MSI cap
 *
 *
 * @param[in] DieBusNum  Bus Number of current Die
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
SataSetDeviceNumMsi (
  IN  UINT32   DieBusNum,
  IN  VOID     *FchDataPtr
  )
{
  FchSataSetDeviceNumMsi (DieBusNum, FchDataPtr );
}

/**
 * SataDriveDetection - Sata drive detection
 *
 *   - Sata Ide & Sata Ide to Ahci only
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 * @param[in] *Bar5Ptr Sata BAR5 base address.
 *
 */
VOID
SataDriveDetection (
  IN  VOID       *FchDataPtr,
  IN  UINT32     *Bar5Ptr
  )
{
  FchSataDriveDetection ( FchDataPtr, Bar5Ptr );
}


