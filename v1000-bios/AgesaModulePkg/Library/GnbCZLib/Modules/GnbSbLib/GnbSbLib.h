/* $NoKeywords:$ */
/**
 * @file
 *
 * SB services
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
#ifndef _GNBSBLIB_H_
#define _GNBSBLIB_H_

#include "GnbPcie.h"

UINT32
SbGetSbIoApicBaseAddress (
  IN       AMD_CONFIG_PARAMS        *StdHeader
  );

UINT32
SbGetSbMmioBaseAddress (
  IN       AMD_CONFIG_PARAMS        *StdHeader
  );

UINT16
SbGetAlinkIoAddress (
  IN       AMD_CONFIG_PARAMS        *StdHeader
  );

AGESA_STATUS
SbPcieInitAspm (
  IN      PCIE_ASPM_TYPE            Aspm,
  IN      AMD_CONFIG_PARAMS         *StdHeader
  );

AGESA_STATUS
SbPcieLinkAspmControl (
  IN      PCIe_ENGINE_CONFIG        *Engine,
  IN      PCIe_PLATFORM_CONFIG      *Pcie
  );


#endif

