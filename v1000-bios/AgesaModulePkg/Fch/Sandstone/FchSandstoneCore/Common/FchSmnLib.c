/* $NoKeywords:$ */
/**
 * @file
 *
 * FCH SMN operation lib
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: FCH
 * @e \$Revision: 312516 $   @e \$Date: 2015-02-06 17:37:45 -0800 (Fri, 06 Feb 2015) $
 *
 */
/*
*****************************************************************************
*
* Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
*
*******************************************************************************
*/
#include "FchPlatform.h"
#define FILECODE FCH_TAISHAN_FCHTAISHANCORE_COMMON_FCHSMNLIB_FILECODE

#define IOHC_NB_SMN_INDEX_2_BIOS  0x00B8
#define IOHC_NB_SMN_DATA_2_BIOS   0x00BC
/**
 * FchSmnRead - Read SMN register
 *
 * @param[in] IohcBus     - IOHC (Node) bus number
 * @param[in] SmnAddress  - Register address in SMN 32-bit
 *                           memory space
 * @param[in] Value       - Pointer to register value
 * @param[in] StdHeader   -
 *
 */
VOID
FchSmnRead (
  IN       UINT32              IohcBus,
  IN       UINT32              SmnAddress,
  IN       UINT32              *Value,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{
  UINT32    RegIndex;
  UINTN     PciAddress;

  RegIndex = SmnAddress;
  PciAddress = (IohcBus << 20) + IOHC_NB_SMN_INDEX_2_BIOS;
  PciWrite32 (PciAddress, RegIndex);
  PciAddress = (IohcBus << 20) + IOHC_NB_SMN_DATA_2_BIOS;
  *Value = PciRead32 (PciAddress);
}

VOID
FchSmnWrite (
  IN       UINT32              IohcBus,
  IN       UINT32              SmnAddress,
  IN       UINT32              *Value,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{
  UINT32    RegIndex;
  UINTN     PciAddress;

  RegIndex = SmnAddress;
  PciAddress = (IohcBus << 20) + IOHC_NB_SMN_INDEX_2_BIOS;
  PciWrite32 (PciAddress, RegIndex);
  PciAddress = (IohcBus << 20) + IOHC_NB_SMN_DATA_2_BIOS;
  PciWrite32 (PciAddress, *Value);
}

VOID
FchSmnRW (
  IN       UINT32              IohcBus,
  IN       UINT32              SmnAddress,
  IN       UINT32              AndMask,
  IN       UINT32              OrMask,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{
  UINT32    RegValue;

  FchSmnRead (IohcBus, SmnAddress, &RegValue, StdHeader);
  RegValue &= AndMask;
  RegValue |= OrMask;
  FchSmnWrite (IohcBus, SmnAddress, &RegValue, StdHeader);
}

/**
 * FchSmnRead8 - Read SMN register in Byte
 *
 * @param[in] IohcBus     - IOHC (Node) bus number
 * @param[in] SmnAddress  - Register address in SMN 32-bit
 *                     memory space
 * @param[in] Value8      - Pointer to register value
 * @param[in] StdHeader   -
 *
 */
VOID
FchSmnRead8 (
  IN       UINT32              IohcBus,
  IN       UINT32              SmnAddress,
  IN       UINT8               *Value8,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{
  UINT32    RegIndex;
  UINTN     PciAddress;

  RegIndex = SmnAddress & 0xFFFFFFFC;
  PciAddress = (IohcBus << 20) + IOHC_NB_SMN_INDEX_2_BIOS;
  PciWrite32 (PciAddress, RegIndex);
  PciAddress = (IohcBus << 20) + IOHC_NB_SMN_DATA_2_BIOS;
  //[-start-240426-IB18160212-modify-]//
  *Value8 = PciRead8 (PciAddress + (UINTN)(SmnAddress & 0x3));
  //[end-240426-IB18160212-modify]//
}

VOID
FchSmnWrite8 (
  IN       UINT32              IohcBus,
  IN       UINT32              SmnAddress,
  IN       UINT8               *Value8,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{
  UINT32    RegIndex;
  UINTN     PciAddress;

  RegIndex = SmnAddress & 0xFFFFFFFC;
  PciAddress = (IohcBus << 20) + IOHC_NB_SMN_INDEX_2_BIOS;
  PciWrite32 (PciAddress, RegIndex);
  PciAddress = (IohcBus << 20) + IOHC_NB_SMN_DATA_2_BIOS;
  PciWrite8 (PciAddress + (UINTN)(SmnAddress & 0x3), *Value8);
}

VOID
FchSmnRW8 (
  IN       UINT32              IohcBus,
  IN       UINT32              SmnAddress,
  IN       UINT8               AndMask,
  IN       UINT8               OrMask,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{
  UINT8    RegValue;

  FchSmnRead8 (IohcBus, SmnAddress, &RegValue, StdHeader);
  RegValue &= AndMask;
  RegValue |= OrMask;
  FchSmnWrite8 (IohcBus, SmnAddress, &RegValue, StdHeader);
}

