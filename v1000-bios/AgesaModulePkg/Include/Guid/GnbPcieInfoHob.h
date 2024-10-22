/* $NoKeywords:$ */
/**
 * @file
 *
 * Gnb Pcie Data Hob GUID definition
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Gnb
 * @e \$Revision: 309090 $   @e \$Date: 2015-09-09 04:30:05 +0800 (Wed, 9 Sep 2015) $
 */
/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 **/
#ifndef _GNB_PCIE_HOB_INFO_H_
#define _GNB_PCIE_HOB_INFO_H_

#define _GNB_PCIE_HOB_INFO_GUID \
{ \
  0x3eb1d90, 0xce14, 0x40d8, 0xa6, 0xba, 0x10, 0x3a, 0x8d, 0x7b, 0xd3, 0x2d \
}
extern EFI_GUID gGnbPcieHobInfoGuid;

#include <GnbDxio.h>

#pragma pack (push, 1)
#define MAX_NUMBER_OF_COMPLEXES_SUPPORTED         8
#define MAX_NUMBER_OF_CORES_PER_COMPLEX           2
#define MAX_NUMBER_OF_PORTS_PER_COMPLEX           16

/// Complex Configuration for silicon module
typedef struct {
  PCIe_SILICON_CONFIG     Silicon;                                        ///< Silicon
  PCIe_WRAPPER_CONFIG     Wrapper[MAX_NUMBER_OF_CORES_PER_COMPLEX];       ///< PCIe Core Descriptors
  PCIe_ENGINE_CONFIG      Ports[MAX_NUMBER_OF_PORTS_PER_COMPLEX];         ///< PCIe Port Descriptors
  UINT8                   PortDevMap[MAX_NUMBER_OF_PORTS_PER_COMPLEX];    ///< PortDevMap Allocation
} COMPLEX_CONFIG_MODEL;

/// PCIe information HOB data
typedef struct _GNB_PCIE_INFORMATION_DATA_HOB {
  EFI_HOB_GUID_TYPE           EfiHobGuidType;                             ///< GUID Hob type structure
  PCIe_PLATFORM_CONFIG        PciePlatformConfigHob;                      ///< Platform Config Structure
  COMPLEX_CONFIG_MODEL        ComplexConfigs[MAX_NUMBER_OF_COMPLEXES];    ///< Allocation for Max Complex Structure suported
} GNB_PCIE_INFORMATION_DATA_HOB;

#pragma pack (pop)

#endif

