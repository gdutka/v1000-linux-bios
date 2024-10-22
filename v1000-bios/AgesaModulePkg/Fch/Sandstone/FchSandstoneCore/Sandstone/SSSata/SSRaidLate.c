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
#define FILECODE PROC_FCH_SANDSTONE_SSSATA_SSRAIDLATE_FILECODE
//
// Declaration of local functions
//

/**
 * FchInitLateSataRaid - Prepare SATA Raid controller to boot to
 * OS.
 *
 * @param[in] DieBusNum  Bus Number of current Die
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
FchInitLateSataRaid (
  IN  UINT32   DieBusNum,
  IN  VOID     *FchDataPtr
  )
{

}


