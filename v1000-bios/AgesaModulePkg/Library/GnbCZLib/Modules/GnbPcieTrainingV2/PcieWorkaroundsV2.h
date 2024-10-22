/* $NoKeywords:$ */
/**
 * @file
 *
 * Various workarounds
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 309090 $   @e \$Date: 2014-12-10 02:28:05 +0800 (Wed, 10 Dec 2014) $
 *
 */
/*
*****************************************************************************
*
* Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
*
*******************************************************************************
*
*/
#ifndef _PCIEWORKAROUNDSV2_H_
#define _PCIEWORKAROUNDSV2_H_

GFX_WORKAROUND_STATUS
PcieGfxCardWorkaroundV2 (
  IN     PCI_ADDR               Port,
  IN     AMD_CONFIG_PARAMS  *StdHeader
  );


#endif

