/* $NoKeywords:$ */
/**
 * @file
 *
 * Supporting services to access PCIe port indirect register space.
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
#ifndef _PCIEPORTREGACC_H_
#define _PCIEPORTREGACC_H_

UINT32
PciePortRegisterRead (
  IN      PCIe_ENGINE_CONFIG    *Engine,
  IN      UINT32                Address,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  );

VOID
PciePortRegisterWrite (
  IN      PCIe_ENGINE_CONFIG    *Engine,
  IN      UINT32                Address,
  IN      UINT32                Value,
  IN      BOOLEAN               S3Save,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  );

VOID
PciePortRegisterWriteField (
  IN      PCIe_ENGINE_CONFIG    *Engine,
  IN      UINT32                Address,
  IN      UINT8                 FieldOffset,
  IN      UINT8                 FieldWidth,
  IN      UINT32                Value,
  IN      BOOLEAN               S3Save,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  );

UINT32
PciePortRegisterReadField (
  IN      PCIe_ENGINE_CONFIG    *Engine,
  IN      UINT32                Address,
  IN      UINT8                 FieldOffset,
  IN      UINT8                 FieldWidth,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  );

VOID
PciePortRegisterRMW (
  IN      PCIe_ENGINE_CONFIG    *Engine,
  IN      UINT32                Address,
  IN      UINT32                AndMask,
  IN      UINT32                OrMask,
  IN      BOOLEAN               S3Save,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  );

#endif

