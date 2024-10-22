/* $NoKeywords:$ */
/**
 * @file
 *
 * Config Fch eMMC controller
 *
 * Init Usb Controller features (PEI phase).
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
#define FILECODE PROC_FCH_TAISHAN_TSEMMC_TSEMMCRESET_FILECODE

/**
 * FchInitResetEmmc - Config eMMC controller during Power-On
 *
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
FchInitResetEmmc (
  IN  VOID     *FchDataPtr
  )
{
  //ProgramLpcEmmcPins (FALSE);
}


