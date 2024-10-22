/* $NoKeywords:$ */
/**
 * @file
 *
 *  PCIe link training
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
#ifndef _PCIETRAININGV2_H_
#define _PCIETRAININGV2_H_

#define TIMESTAMPS_DELTA(Time2, Time1) ((Time2 > Time1) ? (Time2 - Time1) : (0xffffffffull - Time1 + Time2))

AGESA_STATUS
PcieTrainingV2 (
  IN     PCIe_PLATFORM_CONFIG  *Pcie
  );

VOID
PcieTrainingSetPortStateV2 (
  IN      PCIe_ENGINE_CONFIG        *CurrentEngine,
  IN      PCIE_LINK_TRAINING_STATE  State,
  IN      BOOLEAN                   UpdateTimeStamp,
  IN      PCIe_PLATFORM_CONFIG      *Pcie
  );

#endif


