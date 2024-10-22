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
#define FILECODE PROC_FCH_SANDSTONE_SSPCIE_SSABLATE_FILECODE

/**
 * FchInitLateAb - Prepare Ab Bridge to boot to OS.
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
FchInitLateAb (
  IN  VOID     *FchDataPtr
  )
{
  AGESA_TESTPOINT (TpFchInitLateAb, NULL);
  FchAbLateProgram (FchDataPtr);
}


