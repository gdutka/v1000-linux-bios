/* $NoKeywords:$ */
/**
 * @file
 *
 * Family specific PCIe definitions
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 312288 $   @e \$Date: 2015-02-04 00:39:01 -0600 (Wed, 04 Feb 2015) $
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
#ifndef _AMDNBIOPCIEAERRV_H_
#define _AMDNBIOPCIEAERRV_H_
#include <GnbPcieAer.h>

#define NUMBER_OF_PORTS          7

VOID
NbioPcieAerFeatureEnable (
  IN       GNB_HANDLE                 *GnbHandle,
  IN       NBIO_PCIe_PORT_AER_CONFIG  *PortAerConfig
  );

VOID
NbioPcieAerInitRV (
  IN       GNB_HANDLE  *GnbHandle
  );

#endif

