/* $NoKeywords:$ */
/**
 * @file
 *
 * GNB function to create/locate PCIe configuration data area
 *
 * Contain code that create/locate and rebase configuration data area.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 313706 $   @e \$Date: 2015-02-25 21:00:43 -0600 (Wed, 25 Feb 2015) $
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

#ifndef _PCIECONFIGDATA_LIB_H_
#define _PCIECONFIGDATA_LIB_H_


AGESA_STATUS
PcieLocateConfigurationData (
  IN      AMD_CONFIG_PARAMS               *StdHeader,
  OUT     PCIe_PLATFORM_CONFIG            **Pcie
  );

AGESA_STATUS
PcieConfigurationInit (
  IN       AMD_CONFIG_PARAMS               *StdHeader
  );

AGESA_STATUS
PcieConfigurationMap (
  IN       AMD_CONFIG_PARAMS               *StdHeader
  );

#endif


