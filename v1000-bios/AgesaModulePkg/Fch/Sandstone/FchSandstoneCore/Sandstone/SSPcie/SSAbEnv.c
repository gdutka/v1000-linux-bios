/* $NoKeywords:$ */
/**
 * @file
 *
 * Config Fch Ab Bridge
 *
 * Init Ab Bridge features.
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
#define FILECODE PROC_FCH_SANDSTONE_SSPCIE_SSABENV_FILECODE

/**
 * FchInitEnvAb - Config Ab Bridge before PCI emulation
 *
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
FchInitEnvAb (
  IN  VOID     *FchDataPtr
  )
{
  AGESA_TESTPOINT (TpFchInitEnvAb, NULL);
  FchInitEnvAbLinkInit (FchDataPtr);
}

/**
 * FchInitEnvAbSpecial - Config Ab Bridge special timing
 *
 *  This routine must separate with FchInitEnvAb and give Ab
 *  bridge little time to get ready
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
FchInitEnvAbSpecial (
  IN  VOID     *FchDataPtr
  )
{
}


