/* $NoKeywords:$ */
/**
 * @file
 *
 * Header file for PCIe ARI Support
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
#ifndef _PCIEARIINITCZ_H_
#define _PCIEARIINITCZ_H_

AGESA_STATUS
PcieAriInitCZ (
  IN       AMD_CONFIG_PARAMS     *StdHeader,
  IN       PCIe_PLATFORM_CONFIG  *Pcie
  );

#endif

