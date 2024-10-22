/* $NoKeywords:$ */
/**
 * @file
 *
 * Configure Max Payload
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 309090 $
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
#ifndef _PCIEMAXPAYLOAD_H_
#define _PCIEMAXPAYLOAD_H_

VOID
PcieSetMaxPayload (
  IN       PCI_ADDR                DownstreamPort,
  IN       UINT8                   EngineMaxPayload,
  IN       AMD_CONFIG_PARAMS       *StdHeader
  );

AGESA_STATUS
PcieMaxPayloadInterface (
  IN       AMD_CONFIG_PARAMS        *StdHeader
  );

#endif

