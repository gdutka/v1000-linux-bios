/* $NoKeywords:$ */
/**
 * @file
 *
 * PCIe ALIB
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

#ifndef _GNBIVRSLIB_H_
#define _GNBIVRSLIB_H_


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
GnbIvmdAddEntry (
  IN       IVRS_BLOCK_TYPE      Type,
  IN       UINT16               StartDevice,
  IN       UINT16               EndDevice,
  IN       UINT64               BlockAddress,
  IN       UINT64               BlockLength,
  IN       IVRS_IVMD_ENTRY      *Ivmd,
  IN       AMD_CONFIG_PARAMS    *StdHeader
  );

#endif

