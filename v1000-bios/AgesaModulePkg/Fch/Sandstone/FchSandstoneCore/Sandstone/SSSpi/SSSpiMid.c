/* $NoKeywords:$ */
/**
 * @file
 *
 * Config Fch Spi (Lpc) controller
 *
 * Init Spi (Lpc) Controller features.
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
#define FILECODE PROC_FCH_SANDSTONE_SSSPI_SSSPIMID_FILECODE

/**
 * FchInitMidSpi - Config Spi controller after PCI emulation
 *
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
FchInitMidSpi (
  IN  VOID     *FchDataPtr
  )
{
  FchInitMidLpc (FchDataPtr);
}
