/** @file

;******************************************************************************
;* Copyright (c) 2012 - 2017, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

/**
  Set up ROM Table for PCI Bus module.

Copyright (c) 2006 - 2017, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "PciBus.h"

typedef struct {
  EFI_HANDLE  ImageHandle;
  UINTN       Seg;
  UINT8       Bus;
  UINT8       Dev;
  UINT8       Func;
  VOID        *RomImage;
  UINT64      RomSize;
} PCI_ROM_IMAGE;

UINTN          mNumberOfPciRomImages     = 0;
UINTN          mMaxNumberOfPciRomImages  = 0;
PCI_ROM_IMAGE  *mRomImageTable           = NULL;

/**
  Add the Rom Image to internal database for later PCI light enumeration.

  @param ImageHandle    Option Rom image handle.
  @param Seg            Segment of PCI space.
  @param Bus            Bus NO of PCI space.
  @param Dev            Dev NO of PCI space.
  @param Func           Func NO of PCI space.
  @param RomImage       Option Rom buffer.
  @param RomSize        Size of Option Rom buffer.

**/
VOID
PciRomAddImageMapping (
  IN  EFI_HANDLE  ImageHandle,
  IN  UINTN       Seg,
  IN  UINT8       Bus,
  IN  UINT8       Dev,
  IN  UINT8       Func,
  IN  VOID        *RomImage,
  IN  UINT64      RomSize
  )
{
  UINTN           Index;
  PCI_ROM_IMAGE   *NewTable;

  for (Index = 0; Index < mNumberOfPciRomImages; Index++) {
    if (mRomImageTable[Index].Seg  == Seg &&
        mRomImageTable[Index].Bus  == Bus &&
        mRomImageTable[Index].Dev  == Dev &&
        mRomImageTable[Index].Func == Func) {
      //
      // Expect once RomImage and RomSize are recorded, they will be passed in
      // later when updating ImageHandle
      //
      ASSERT ((mRomImageTable[Index].RomImage == NULL) || (RomImage == mRomImageTable[Index].RomImage));
      ASSERT ((mRomImageTable[Index].RomSize  == 0   ) || (RomSize  == mRomImageTable[Index].RomSize ));
      break;
    }
  }

  if (Index == mNumberOfPciRomImages) {
    //
    // Rom Image Table buffer needs to grow.
    //
    if (mNumberOfPciRomImages == mMaxNumberOfPciRomImages) {
      NewTable = ReallocatePool (
                   mMaxNumberOfPciRomImages * sizeof (PCI_ROM_IMAGE),
                   (mMaxNumberOfPciRomImages + 0x20) * sizeof (PCI_ROM_IMAGE),
                   mRomImageTable
                   );
      if (NewTable == NULL) {
        return ;
      }

      mRomImageTable            = NewTable;
      mMaxNumberOfPciRomImages += 0x20;
    }
    //
    // Record the new PCI device
    //
    mRomImageTable[Index].Seg  = Seg;
    mRomImageTable[Index].Bus  = Bus;
    mRomImageTable[Index].Dev  = Dev;
    mRomImageTable[Index].Func = Func;
    mNumberOfPciRomImages++;
  }

  mRomImageTable[Index].ImageHandle = ImageHandle;
  mRomImageTable[Index].RomImage    = RomImage;
  mRomImageTable[Index].RomSize     = RomSize;
}

/**
  Get Option rom driver's mapping for PCI device.

  @param PciIoDevice Device instance.

  @retval TRUE   Found Image mapping.
  @retval FALSE  Cannot found image mapping.

**/
BOOLEAN
PciRomGetImageMapping (
  IN  PCI_IO_DEVICE                       *PciIoDevice
  )
{
  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL *PciRootBridgeIo;
  UINTN                           Index;

  PciRootBridgeIo = PciIoDevice->PciRootBridgeIo;

  for (Index = 0; Index < mNumberOfPciRomImages; Index++) {
    if (mRomImageTable[Index].Seg  == PciRootBridgeIo->SegmentNumber &&
        mRomImageTable[Index].Bus  == PciIoDevice->BusNumber         &&
        mRomImageTable[Index].Dev  == PciIoDevice->DeviceNumber      &&
        mRomImageTable[Index].Func == PciIoDevice->FunctionNumber    ) {

      if (mRomImageTable[Index].ImageHandle != NULL) {
        AddDriver (PciIoDevice, mRomImageTable[Index].ImageHandle, NULL);
      }
      PciIoDevice->PciIo.RomImage = mRomImageTable[Index].RomImage;
      PciIoDevice->PciIo.RomSize  = mRomImageTable[Index].RomSize;
      return TRUE;
    }
  }

  return FALSE;
}
