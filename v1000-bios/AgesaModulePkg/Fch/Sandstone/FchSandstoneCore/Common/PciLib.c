/* $NoKeywords:$ */
/**
 * @file
 *
 * FCH PCI access lib
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: FCH
 * @e \$Revision: 309090 $   @e \$Date: 2014-12-09 10:28:05 -0800 (Tue, 09 Dec 2014) $
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

VOID
ReadPci (
  IN       UINT32                  Address,
  IN       UINT8                   OpFlag,
  IN       VOID*                   Value,
  IN       AMD_CONFIG_PARAMS       *StdHeader
  )
{
  UINTN  PciAddress;

  PciAddress = ((Address >> 4) & ~0xFFF) + (Address & 0xFFF);

  switch ((ACCESS_WIDTH) OpFlag) {
  case AccessWidth8:
  case AccessS3SaveWidth8:
    *((UINT8 *) Value) = PciRead8 (PciAddress);
    break;
  case AccessWidth16:
  case AccessS3SaveWidth16:
    *((UINT16 *) Value) = PciRead16 (PciAddress);
    break;
  case AccessWidth32:
  case AccessS3SaveWidth32:
    *((UINT32 *) Value) = PciRead32 (PciAddress);
    break;
  default:
    break;
  }
}


VOID
WritePci (
  IN       UINT32                  Address,
  IN       UINT8                   OpFlag,
  IN       VOID                    *Value,
  IN       AMD_CONFIG_PARAMS       *StdHeader
  )
{
  UINTN  PciAddress;

  PciAddress = ((Address >> 4) & ~0xFFF) + (Address & 0xFFF);

  switch ((ACCESS_WIDTH) OpFlag) {
  case AccessWidth8:
  case AccessS3SaveWidth8:
    PciWrite8 (PciAddress, *((UINT8 *) Value));
    break;
  case AccessWidth16:
  case AccessS3SaveWidth16:
    PciWrite16 (PciAddress, *((UINT16 *) Value));
    break;
  case AccessWidth32:
  case AccessS3SaveWidth32:
    PciWrite32 (PciAddress, *((UINT32 *) Value));
    break;
  default:
    break;
  }
}


VOID
RwPci (
  IN       UINT32                  Address,
  IN       UINT8                   OpFlag,
  IN       UINT32                  Mask,
  IN       UINT32                  Data,
  IN       AMD_CONFIG_PARAMS       *StdHeader
  )
{
  UINTN     PciAddress;
  UINT32    TempData;
  UINT32    TempMask;

  PciAddress = ((Address >> 4) & ~0xFFF) + (Address & 0xFFF);

  LibAmdGetDataFromPtr ((ACCESS_WIDTH) OpFlag, &Data,  &Mask, &TempData, &TempMask);

  switch ((ACCESS_WIDTH) OpFlag) {
  case AccessWidth8:
  case AccessS3SaveWidth8:
    PciAndThenOr8 (PciAddress, (UINT8) TempMask, (UINT8) TempData);
    break;
  case AccessWidth16:
  case AccessS3SaveWidth16:
    PciAndThenOr16 (PciAddress, (UINT16) TempMask, (UINT16) TempData);
    break;
  case AccessWidth32:
  case AccessS3SaveWidth32:
    PciAndThenOr32 (PciAddress, TempMask, TempData);
    break;
  default:
    break;
  }
}



