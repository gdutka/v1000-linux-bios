/** @file
  PCI I/O Abstraction Driver for USB

;******************************************************************************
;* Copyright (c) 2012 - 2020, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "UsbCoreDxe.h"
#include "UsbPciIo.h"

STATIC EFI_PCI_IO_PROTOCOL            **mPciIos = NULL;
STATIC UINTN                          mMaxPciIoInstances = 0;

/**
  gEfiPciIoProtocolGuid callback function to update stored PCI IO protocol handle.

  @param[in] Event    Event whose notification function is being invoked.
  @param[in] Context  Pointer to the notification function's context.
**/
VOID
EFIAPI
UsbCorePciIoCallback (
  IN EFI_EVENT                         Event,
  IN VOID                              *Context
  )
{
  if (mPciIos != NULL) {
    ZeroMem (mPciIos, mMaxPciIoInstances * sizeof (EFI_PCI_IO_PROTOCOL *));
  }
}

/**

  Get corresponding EFI_PCI_IO_PROTOCOL according to input Segment/Bus/Device/Function.

  @param[in]  Segment       PCI Segment number.
  @param[in]  Bus           PCI Bus number.
  @param[in]  Device        PCI Device number.
  @param[in]  Function      PCI Function number.

  @return Pointer to EFI_PCI_IO_PROTOCOL instance or NULL if not found.
**/
STATIC
EFI_PCI_IO_PROTOCOL *
GetPciIoProtocol (
  IN UINTN      Segment,
  IN UINTN      Bus,
  IN UINTN      Device,
  IN UINTN      Function
  )
{
  EFI_STATUS                            Status;
  UINTN                                 HandleCount;
  EFI_HANDLE                            *HandleBuffer;
  UINTN                                 HandleIndex;
  UINTN                                 CurrentSegment;
  UINTN                                 CurrentBus;
  UINTN                                 CurrentDevice;
  UINTN                                 CurrentFunction;
  UINTN                                 Index;
  EFI_PCI_IO_PROTOCOL                   **NewPciIos;
  EFI_PCI_IO_PROTOCOL                   *PciIo;
  VOID                                  *Registration;

  for (Index = 0; Index < mMaxPciIoInstances; Index++) {
    if (mPciIos[Index] == NULL) {
      break;
    }
    Status = mPciIos[Index]->GetLocation (
                              mPciIos[Index],
                              &CurrentSegment,
                              &CurrentBus,
                              &CurrentDevice,
                              &CurrentFunction
                              );
    if (!EFI_ERROR (Status) && Segment == CurrentSegment && Bus == CurrentBus && Device == CurrentDevice &&
        Function == CurrentFunction) {
      return mPciIos[Index];
    }
  }

  if (Index == mMaxPciIoInstances) {
    NewPciIos = AllocateZeroPool ((mMaxPciIoInstances + 10) * sizeof (EFI_PCI_IO_PROTOCOL *));
    if (NewPciIos == NULL) {
      return NULL;
    }
    if (mPciIos != NULL) {
      CopyMem (NewPciIos, mPciIos, mMaxPciIoInstances * sizeof (EFI_PCI_IO_PROTOCOL *));
      gBS->FreePool (mPciIos);
    } else {
      EfiCreateProtocolNotifyEvent (
        &gEfiPciIoProtocolGuid,
        TPL_CALLBACK,
        UsbCorePciIoCallback,
        NULL,
        &Registration
        );
    }
    mPciIos = NewPciIos;
    mMaxPciIoInstances += 10;
  }

  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiPciIoProtocolGuid,
                  NULL,
                  &HandleCount,
                  &HandleBuffer
                  );
  if (EFI_ERROR (Status)) {
    return NULL;
  }
  for (HandleIndex = 0; HandleIndex < HandleCount; HandleIndex++) {
    Status = gBS->HandleProtocol (
                    HandleBuffer[HandleIndex],
                    &gEfiPciIoProtocolGuid,
                    (VOID **)&PciIo
                    );
    if (EFI_ERROR (Status)) {
      continue;
    }

    Status = PciIo->GetLocation (
                      PciIo,
                      &CurrentSegment,
                      &CurrentBus,
                      &CurrentDevice,
                      &CurrentFunction
                      );
    if (EFI_ERROR (Status)) {
      continue;
    }
    if (Segment == CurrentSegment && Bus == CurrentBus && Device == CurrentDevice &&
        Function == CurrentFunction) {
      gBS->FreePool (HandleBuffer);
      mPciIos[Index] = PciIo;
      return PciIo;
    }
  }
  gBS->FreePool (HandleBuffer);
  return NULL;
}

/**

  Performs a PCI Configuration Read Cycle

  @param  PciAddress            Seg/Bus/Dev/Fun
  @param  Width                 8/16/32
  @param  Offset                Offset of Register
  @param  Buffer                Output Buffer Ptr

**/
EFI_STATUS
EFIAPI
PciIoPciRead (
  IN     UINTN                          PciAddress,
  IN     EFI_PCI_IO_PROTOCOL_WIDTH      Width,
  IN     UINTN                          Offset,
  IN OUT VOID                           *Buffer
  )
{
  UINT64                   Address;
  EFI_PCI_IO_PROTOCOL      *PciIo;

  if (mInSmram) {
    //
    // Use PciExpressBase address MMIO to read PCI configuration data 
    //
    if (PciAddress & 0xff000000) return EFI_UNSUPPORTED;
    Address = mPrivate->PciExpressBaseAddress + ((PciAddress & 0x00ffff00) << 4) + (PciAddress & 0x000000ff) + Offset;
    switch (Width) {
      case EfiPciIoWidthUint8:
        *(UINT8*)Buffer = *(UINT8*)(UINTN)Address;
        break;
      case EfiPciIoWidthUint16:
        *(UINT16*)Buffer = *(UINT16*)(UINTN)Address;
        break;
      case EfiPciIoWidthUint32:
        *(UINT32*)Buffer = *(UINT32*)(UINTN)Address;
        break;
      default:
        break;
    }
  } else {
    //
    // Use UEFI PCI IO protocol for native system
    //
    PciIo = GetPciIoProtocol (
              (UINTN) ((PciAddress & 0xff000000) >> 24),
              (UINTN) ((PciAddress & 0xff0000) >> 16),
              (UINTN) ((PciAddress & 0xf800) >> 11),
              (UINTN) ((PciAddress & 0x0700) >> 8)
              );
    if (PciIo == NULL) {
      return EFI_UNSUPPORTED;
    }
    PciIo->Pci.Read (PciIo, Width, (UINT32) Offset, 1, Buffer);
  }
  return EFI_SUCCESS;
}

/**

  Performs a PCI Configuration Write Cycle

  @param  PciAddress            Seg/Bus/Dev/Fun
  @param  Width                 8/16/32
  @param  Offset                Offset of Register
  @param  Buffer                Output Buffer Ptr

**/
EFI_STATUS
EFIAPI
PciIoPciWrite (
  IN     UINTN                          PciAddress,
  IN     EFI_PCI_IO_PROTOCOL_WIDTH      Width,
  IN     UINTN                          Offset,
  IN OUT VOID                           *Buffer
  )
{
  UINT64                   Address;
  EFI_PCI_IO_PROTOCOL      *PciIo;

  if (mInSmram) {
    //
    // Use PciExpressBase address MMIO to read PCI configuration data 
    //
    if (PciAddress & 0xff000000) return EFI_UNSUPPORTED;
    Address = mPrivate->PciExpressBaseAddress + ((PciAddress & 0x00ffff00) << 4) + (PciAddress & 0x000000ff) + Offset;
    //
    // Write data to PCI data port
    //
    switch (Width) {
      case EfiPciIoWidthUint8:
        *(UINT8*)(UINTN)Address = *(UINT8*)Buffer;
        break;
      case EfiPciIoWidthUint16:
        *(UINT16*)(UINTN)Address = *(UINT16*)Buffer;
        break;
      case EfiPciIoWidthUint32:
        *(UINT32*)(UINTN)Address = *(UINT32*)Buffer;
        break;
      default:
        break;
    }
  } else {
    //
    // Use UEFI PCI IO protocol for native system
    //
    PciIo = GetPciIoProtocol (
              (UINTN) ((PciAddress & 0xff000000) >> 24),
              (UINTN) ((PciAddress & 0xff0000) >> 16),
              (UINTN) ((PciAddress & 0xf800) >> 11),
              (UINTN) ((PciAddress & 0x0700) >> 8)
              );
    if (PciIo == NULL) {
      return EFI_UNSUPPORTED;
    }
    PciIo->Pci.Write (PciIo, Width, (UINT32) Offset, 1, Buffer);
  }
  return EFI_SUCCESS;
}

/**

  Performs a PCI Memory Read Cycle

  @param  PciAddress            Seg/Bus/Dev/Fun
  @param  Width                 8/16/32
  @param  BarIndex              Bar Index from 0x10
  @param  Offset                Offset of Register
  @param  Buffer                Output Buffer Ptr

**/
EFI_STATUS
EFIAPI
PciIoMemRead (
  IN     UINTN                          PciAddress,
  IN     EFI_PCI_IO_PROTOCOL_WIDTH      Width,
  IN     UINT8                          BarIndex,
  IN     UINT64                         Offset,
  IN OUT VOID                           *Buffer
  )
{
  UINT8                    *Bar, Buf[8];
  UINT64                   Address;
  EFI_PCI_IO_PROTOCOL      *PciIo;

  if (mInSmram) {
    //
    // Use PciExpressBase address MMIO to read PCI configuration data 
    //
    if (PciAddress & 0xff000000) return EFI_UNSUPPORTED;
    Address = mPrivate->PciExpressBaseAddress + ((PciAddress & 0x00ffff00) << 4) + (PciAddress & 0x000000ff) + PCI_CONFIG_BAR_BASE + (BarIndex * 4);
    //
    // Read BAR from PciExpress MMIO
    //
    Bar = (UINT8*)(((*(UINT32*)(UINTN)Address) & ~0x0F) + (UINTN)Offset);
    *(UINT32*)Buf = *(UINT32*)((UINTN)Bar & ~0x03);
    if (((UINTN)Bar & 0x03) && (Width > EfiPciIoWidthUint8)) {
      *(UINT32*)(Buf + 4) = *(UINT32*)((UINTN)(Bar + 0x04) & ~0x03);
    }
    Offset &= 0x03;
    //
    // Read data from PCI data port
    //
    switch (Width) {
      case EfiPciIoWidthUint8:
        *(UINT8*)Buffer = Buf[Offset];
        break;
      case EfiPciIoWidthUint16:
        *(UINT16*)Buffer = *(UINT16*)&Buf[Offset];
        break;
      case EfiPciIoWidthUint32:
        *(UINT32*)Buffer = *(UINT32*)&Buf[Offset];
        break;
      default:
        return EFI_UNSUPPORTED;
    }
  } else {
    //
    // Use UEFI PCI IO protocol for native system
    //
    PciIo = GetPciIoProtocol (
              (UINTN) ((PciAddress & 0xff000000) >> 24),
              (UINTN) ((PciAddress & 0xff0000) >> 16),
              (UINTN) ((PciAddress & 0xf800) >> 11),
              (UINTN) ((PciAddress & 0x0700) >> 8)
              );
    if (PciIo == NULL) {
      return EFI_UNSUPPORTED;
    }
    PciIo->Mem.Read (PciIo, Width, BarIndex, (UINT64) Offset, 1, Buffer);
  }
  return EFI_SUCCESS;
}

/**

  Performs a PCI Memory Write Cycle

  @param  PciAddress            Seg/Bus/Dev/Fun
  @param  Width                 8/16/32
  @param  BarIndex              Bar Index from 0x10
  @param  Offset                Offset of Register
  @param  Buffer                Output Buffer Ptr

**/
EFI_STATUS
EFIAPI
PciIoMemWrite (
  IN     UINTN                          PciAddress,
  IN     EFI_PCI_IO_PROTOCOL_WIDTH      Width,
  IN     UINT8                          BarIndex,
  IN     UINT64                         Offset,
  IN OUT VOID                           *Buffer
  )
{
  UINT8                    *Bar, Buf[8];
  UINT64                   Address;
  EFI_PCI_IO_PROTOCOL      *PciIo;

  if (mInSmram) {
    //
    // Use PciExpressBase address MMIO to read PCI configuration data 
    //
    if (PciAddress & 0xff000000) return EFI_UNSUPPORTED;
    Address = mPrivate->PciExpressBaseAddress + ((PciAddress & 0x00ffff00) << 4) + (PciAddress & 0x000000ff) + PCI_CONFIG_BAR_BASE + (BarIndex * 4);
    //
    // Read BAR from PciExpress MMIO
    //
    Bar = (UINT8*)(((*(UINT32*)(UINTN)Address) & ~0x0F) + (UINTN)Offset);
    if ((UINTN)Bar & 0x03) {
      *(UINT32*)Buf = *(UINT32*)((UINTN)Bar & ~0x03);
      if (Width > EfiPciIoWidthUint8) {
        *(UINT32*)(Buf + 4) = *(UINT32*)((UINTN)(Bar + 0x04) & ~0x03);
      }
    }
    Offset &= 0x03;
    //
    // Merge data to temp buffer
    //
    switch (Width) {
      case EfiPciIoWidthUint8:
        Buf[Offset] = *(UINT8*)Buffer;
        break;
      case EfiPciIoWidthUint16:
        *(UINT16*)&Buf[Offset] = *(UINT16*)Buffer;
        break;
      case EfiPciIoWidthUint32:
        *(UINT32*)&Buf[Offset] = *(UINT32*)Buffer;
        break;
      default:
        return EFI_UNSUPPORTED;
    }
    *(UINT32*)((UINTN)Bar & ~0x03) = *(UINT32*)&Buf[0];
    if (((UINTN)Bar & 0x03) && (Width > EfiPciIoWidthUint8)) {
      *(UINT32*)((UINTN)(Bar + 0x04) & ~0x03) = *(UINT32*)&Buf[4];
    }
  } else {
    //
    // Use UEFI PCI IO protocol for native system
    //
    PciIo = GetPciIoProtocol (
              (UINTN) ((PciAddress & 0xff000000) >> 24),
              (UINTN) ((PciAddress & 0xff0000) >> 16),
              (UINTN) ((PciAddress & 0xf800) >> 11),
              (UINTN) ((PciAddress & 0x0700) >> 8)
              );
    if (PciIo == NULL) {
      return EFI_UNSUPPORTED;
    }
    PciIo->Mem.Write (PciIo, Width, BarIndex, (UINT64) Offset, 1, Buffer);
  }
  return EFI_SUCCESS;
}

/**

  Performs a PCI I/O Read Cycle

  @param  PciAddress            Seg/Bus/Dev/Fun, -1 indicates pure IO access
  @param  Width                 8/16/32
  @param  BarIndex              Bar Index from 0x10
  @param  Offset                Offset of Register
  @param  Buffer                Output Buffer Ptr

**/
EFI_STATUS
EFIAPI
PciIoIoRead (
  IN     UINTN                          PciAddress,
  IN     EFI_PCI_IO_PROTOCOL_WIDTH      Width,
  IN     UINT8                          BarIndex,
  IN     UINT64                         Offset,
  IN OUT VOID                           *Buffer
  )
{
  UINT16                   Port;
  UINT64                   Address;
  EFI_PCI_IO_PROTOCOL      *PciIo;

  if (mInSmram) {
    //
    // Use PciExpressBase address MMIO to read PCI configuration data 
    //
    if (PciAddress & 0xff000000) return EFI_UNSUPPORTED;
    Address = mPrivate->PciExpressBaseAddress + ((PciAddress & 0x00ffff00) << 4) + (PciAddress & 0x000000ff) + PCI_CONFIG_BAR_BASE + (BarIndex * 4);
    //
    // Read BAR from PciExpress MMIO
    //
    Port = (UINT16)(((*(UINT32*)(UINTN)Address) & ~0x03) + Offset);
    //
    // Read data from IO BAR
    //
    switch (Width) {
      case EfiPciIoWidthUint8:
        *(UINT8*)Buffer = IoRead8 (Port);
        break;
      case EfiPciIoWidthUint16:
        *(UINT16*)Buffer = IoRead16 (Port);
        break;
      case EfiPciIoWidthUint32:
        *(UINT32*)Buffer = IoRead32 (Port);
        break;
      default:
        break;
    }
  } else {
    //
    // Use UEFI PCI IO protocol for native system
    //
    PciIo = GetPciIoProtocol (
              (UINTN) ((PciAddress & 0xff000000) >> 24),
              (UINTN) ((PciAddress & 0xff0000) >> 16),
              (UINTN) ((PciAddress & 0xf800) >> 11),
              (UINTN) ((PciAddress & 0x0700) >> 8)
              );
    if (PciIo == NULL) {
      return EFI_UNSUPPORTED;
    }
    PciIo->Io.Read (PciIo, Width, BarIndex, (UINT64) Offset, 1, Buffer);
  }
  return EFI_SUCCESS;
}

/**

  Performs a PCI I/O Write Cycle

  @param  PciAddress            Seg/Bus/Dev/Fun
  @param  Width                 8/16/32
  @param  BarIndex              Bar Index from 0x10
  @param  Offset                Offset of Register
  @param  Buffer                Output Buffer Ptr

**/
EFI_STATUS
EFIAPI
PciIoIoWrite (
  IN     UINTN                          PciAddress,
  IN     EFI_PCI_IO_PROTOCOL_WIDTH      Width,
  IN     UINT8                          BarIndex,
  IN     UINT64                         Offset,
  IN OUT VOID                           *Buffer
  )
{
  UINT16                   Port;
  UINT64                   Address;
  EFI_PCI_IO_PROTOCOL      *PciIo;

  if (mInSmram) {
    //
    // Use PciExpressBase address MMIO to read PCI configuration data 
    //
    if (PciAddress & 0xff000000) return EFI_UNSUPPORTED;
    Address = mPrivate->PciExpressBaseAddress + ((PciAddress & 0x00ffff00) << 4) + (PciAddress & 0x000000ff) + PCI_CONFIG_BAR_BASE + (BarIndex * 4);
    //
    // Read BAR from PciExpress MMIO
    //
    Port = (UINT16)(((*(UINT32*)(UINTN)Address) & ~0x03) + Offset);
    //
    // Read data from IO BAR
    //
    switch (Width) {
      case EfiPciIoWidthUint8:
        IoWrite8 (Port, *(UINT8*)Buffer);
        break;
      case EfiPciIoWidthUint16:
        IoWrite16 (Port, *(UINT16*)Buffer);
        break;
      case EfiPciIoWidthUint32:
        IoWrite32 (Port, *(UINT32*)Buffer);
        break;
      default:
        break;
    }
  } else {
    //
    // Use UEFI PCI IO protocol for native system
    //
    PciIo = GetPciIoProtocol (
              (UINTN) ((PciAddress & 0xff000000) >> 24),
              (UINTN) ((PciAddress & 0xff0000) >> 16),
              (UINTN) ((PciAddress & 0xf800) >> 11),
              (UINTN) ((PciAddress & 0x0700) >> 8)
              );
    if (PciIo == NULL) {
      return EFI_UNSUPPORTED;
    }
    PciIo->Io.Write (PciIo, Width, BarIndex, (UINT64) Offset, 1, Buffer);
  }
  return EFI_SUCCESS;
}