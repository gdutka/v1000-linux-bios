/* $NoKeywords:$ */
/**
 * @file
 *
 * IOMMU IVRS Library
 *
 *
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

#ifndef _NBIOIOMMUIVRSLIB_H_
#define _NBIOIOMMUIVRSLIB_H_

#include <GnbIommu.h>
#include <GnbDxio.h>
VOID
GnbIvhdAddDeviceRangeEntry (
  IN       PCI_ADDR             StartRange,
  IN       PCI_ADDR             EndRange,
  IN       UINT8                DataSetting,
  IN       IVRS_IVHD_ENTRY      *Ivhd,
  IN       AMD_CONFIG_PARAMS    *StdHeader
  );

VOID
GnbIvhdAddDeviceAliasRangeEntry (
  IN       PCI_ADDR             StartRange,
  IN       PCI_ADDR             EndRange,
  IN       PCI_ADDR             Alias,
  IN       UINT8                DataSetting,
  IN       IVRS_IVHD_ENTRY      *Ivhd,
  IN       AMD_CONFIG_PARAMS    *StdHeader
  );

VOID
GnbIvhdAddSpecialDeviceEntry (
  IN       IVHD_SPECIAL_DEVICE  SpecialDevice,
  IN       PCI_ADDR             Device,
  IN       UINT8                Id,
  IN       UINT8                DataSetting,
  IN       IVRS_IVHD_ENTRY      *Ivhd,
  IN       AMD_CONFIG_PARAMS    *StdHeader
  );

VOID
GnbIvhdAddF0DeviceEntry (
  IN       PCI_ADDR             Device,
  IN       UINT8                DataSetting,
  IN       IVRS_IVHD_ENTRY      *Ivhd,
  IN       AMD_CONFIG_PARAMS    *StdHeader
  );

VOID
GnbIvmdAddEntry (
  IN       IVRS_BLOCK_TYPE      Type,
  IN       UINT16               StartDevice,
  IN       UINT16               EndDevice,
  IN       UINT64               BlockAddress,
  IN       UINT64               BlockLength,
  IN       IVRS_IVMD_ENTRY      *Ivmd,
  IN       AMD_CONFIG_PARAMS    *StdHeader
  );

AGESA_STATUS
GnbIommuIvrsTable (
  );

VOID
GnbIvhdAddApicEntry (
  IN       GNB_HANDLE                 *GnbHandle,
     OUT   IVRS_IVHD_ENTRY            *Ivhd,
  IN       AMD_CONFIG_PARAMS          *StdHeader
  );

AGESA_STATUS
GnbIommuScratchMemoryRangeInterface (
  );

VOID
SbCreateIvhdEntries (
     OUT   IVRS_IVHD_ENTRY            *Ivhd,
  IN       AMD_CONFIG_PARAMS          *StdHeader
  );

VOID
SbCreateIvhd40hEntries (
     OUT   IVRS_IVHD_ENTRY            *Ivhd,
  IN       AMD_CONFIG_PARAMS          *StdHeader
  );

BOOLEAN
GnbCheckIommuPresent (
  IN       GNB_HANDLE                 *GnbHandle,
  IN       AMD_CONFIG_PARAMS          *StdHeader
  );
#endif

