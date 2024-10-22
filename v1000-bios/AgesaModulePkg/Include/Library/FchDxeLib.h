/* $NoKeywords:$ */
/**
 * @file
 *
 * FCH DXE lib
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
#ifndef _FCH_DXE_LIB_H_
#define _FCH_DXE_LIB_H_

#include <PiDxe.h>
#include <Library/DebugLib.h>

#include <Protocol/PciRootBridgeIo.h>


VOID
LibFchIoRead (
  IN       EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH    EfiWidth,
  IN       UINT16                                   Address,
     OUT   VOID                                     *Value
  );


VOID
LibFchIoWrite (
  IN       EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH    EfiWidth,
  IN       UINT16                                   Address,
     OUT   VOID                                     *Value
  );


VOID
LibFchMemRead (
  IN       EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH    EfiWidth,
  IN       UINT32                                   Address,
     OUT   VOID                                     *Value
  );


VOID
LibFchMemWrite (
  IN       EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH    EfiWidth,
  IN       UINT32                                   Address,
  IN       VOID                                     *Value
  );


VOID
LibFchIoRw (
  IN       EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH    EfiWidth,
  IN       UINT16                                   Address,
  IN       VOID                                     *AndMask,
  IN       VOID                                     *OrMask
  );


VOID
LibFchMemRw (
  IN       EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH    EfiWidth,
  IN       UINT32                                   Address,
  IN       VOID                                     *AndMask,
  IN       VOID                                     *OrMask
  );


VOID
LibFchIndirectIoRead (
  IN       EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH    EfiWidth,
  IN       UINT16                                   IoBase,
  IN       UINT8                                    IndexAddress,
     OUT   VOID                                     *Value
  );


VOID
LibFchIndirectIoWrite (
  IN       EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH    EfiWidth,
  IN       UINT16                                   IoBase,
  IN       UINT8                                    IndexAddress,
  IN       VOID                                     *Value
  );


VOID
LibFchGetAcpiMmioBase (
  OUT   UINT32        *AcpiMmioBase
  );


VOID
LibFchGetAcpiPmBase (
  OUT  UINT16         *AcpiPmBase
  );


VOID
LibFchPciRead (
  IN       EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH    EfiWidth,
  IN       UINT32                                   Address,
  IN       VOID                                     *Value
  );


VOID
LibFchPciWrite (
  IN       EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH    EfiWidth,
  IN       UINT32                                   Address,
  IN       VOID                                     *Value
  );

#endif

