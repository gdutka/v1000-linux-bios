/* $NoKeywords:$ */
/**
 * @file
 *
 * Procedure to map user define topology to processor configuration
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
#ifndef _PCIEMAPTOPOLOGY_H_
#define _PCIEMAPTOPOLOGY_H_

AGESA_STATUS
PcieMapTopologyOnComplex (
  IN      PCIe_COMPLEX_DESCRIPTOR     *ComplexDescriptor,
  IN      PCIe_COMPLEX_CONFIG         *Complex,
  IN      PCIe_PLATFORM_CONFIG        *Pcie
 );

#endif



