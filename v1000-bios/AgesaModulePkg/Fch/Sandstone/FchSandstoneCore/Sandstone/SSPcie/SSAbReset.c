/* $NoKeywords:$ */
/**
 * @file
 *
 * Config Fch Ab Bridge
 *
 * Init Ab Bridge features (PEI phase).
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
#define FILECODE PROC_FCH_SANDSTONE_SSPCIE_SSABRESET_FILECODE

/**
 * FchInitResetAb - Config Ab Bridge during Power-On
 *
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
FchInitResetAb (
  IN  VOID     *FchDataPtr
  )
{
  AGESA_TESTPOINT (TpFchInitResetAb, NULL);
  FchProgramAbPowerOnReset (FchDataPtr);
}


