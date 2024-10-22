/* $NoKeywords:$ */
/**
 * @file
 *
 * Supporting services to access PCIe wrapper/core/PIF/PHY indirect register spaces
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

#ifndef _PCIEWRAPPERREGACC_H_
#define _PCIEWRAPPERREGACC_H_

//#define WRAP_SPACE(w, x)   (0x01300000ul | (w << 16) | (x))
//#define CORE_SPACE(c, x)   (0x00010000ul | (c << 24) | (x))
//#define PHY_SPACE(w, p, x) (0x00200000ul | ((p + 1) << 24) | (w << 16) | (x))
//#define PIF_SPACE(w, p, x) (0x00100000ul | ((p + 1) << 24) | (w << 16) | (x))
#define IMP_SPACE(x) (0x01080000ul | (x))

UINT32
PcieRegisterRead (
  IN      PCIe_WRAPPER_CONFIG   *Wrapper,
  IN      UINT32                Address,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  );

VOID
PcieRegisterWrite (
  IN      PCIe_WRAPPER_CONFIG   *Wrapper,
  IN      UINT32                Address,
  IN      UINT32                Value,
  IN      BOOLEAN               S3Save,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  );

UINT32
PcieRegisterReadField (
  IN      PCIe_WRAPPER_CONFIG   *Wrapper,
  IN      UINT32                Address,
  IN      UINT8                 FieldOffset,
  IN      UINT8                 FieldWidth,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  );

VOID
PcieRegisterWriteField (
  IN      PCIe_WRAPPER_CONFIG   *Wrapper,
  IN      UINT32                Address,
  IN      UINT8                 FieldOffset,
  IN      UINT8                 FieldWidth,
  IN      UINT32                Value,
  IN      BOOLEAN               S3Save,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  );

VOID
PcieRegisterRMW (
  IN      PCIe_WRAPPER_CONFIG   *Wrapper,
  IN      UINT32                Address,
  IN      UINT32                AndMask,
  IN      UINT32                OrMask,
  IN      BOOLEAN               S3Save,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  );

UINT32
PcieSiliconRegisterRead (
  IN      PCIe_SILICON_CONFIG   *Silicon,
  IN      UINT32                Address,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  );

VOID
PcieSiliconRegisterWrite (
  IN      PCIe_SILICON_CONFIG   *Silicon,
  IN      UINT32                Address,
  IN      UINT32                Value,
  IN      BOOLEAN               S3Save,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  );

VOID
PcieSiliconRegisterRMW (
  IN      PCIe_SILICON_CONFIG   *Silicon,
  IN      UINT32                Address,
  IN      UINT32                AndMask,
  IN      UINT32                OrMask,
  IN      BOOLEAN               S3Save,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  );

#endif

