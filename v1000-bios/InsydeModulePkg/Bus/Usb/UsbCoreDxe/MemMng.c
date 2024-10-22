/** @file
  Memory Management for USB

;******************************************************************************
;* Copyright (c) 2022, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "UsbCoreDxe.h"
#include "MemMng.h"
#include "Legacy.h"

STATIC EFI_GUID                 mUsbWorkingRegionHobGuid = USB_WORKING_REGION_HOB_GUID;
STATIC EFI_GUID                 mIommuProtocolGuid       = IOMMU_PROTOCOL_GUID;
STATIC VOID                     *mRegistration           = NULL;
STATIC IOMMU_PROTOCOL           *mIommu                  = NULL;
STATIC VOID                     *mDxeWorkingMemory       = NULL;
STATIC VOID                     *mSmmWorkingMemory       = NULL;

STATIC
VOID*
SearchGuidHob(
  IN EFI_HOB_HANDOFF_INFO_TABLE *HobTable,
  IN EFI_GUID                   *Guid,
  OUT UINTN                     *Length
  )
{
  VOID                          *HobStart;
  EFI_PEI_HOB_POINTERS          GuidHob;

  HobStart = HobTable + 1;
  while (TRUE) {
    GuidHob.Raw = HobStart;
    if (END_OF_HOB_LIST (GuidHob)) break;
    if (GuidHob.Header->HobType == EFI_HOB_TYPE_GUID_EXTENSION) {
      if (*(UINT64*)((UINT8*)Guid + 0) == *(UINT64*)((UINT8*)&GuidHob.Guid->Name + 0) &&
          *(UINT64*)((UINT8*)Guid + 8) == *(UINT64*)((UINT8*)&GuidHob.Guid->Name + 8)) {
        if (Length) *Length = GuidHob.Header->HobLength - sizeof(EFI_HOB_GUID_TYPE);
        return (VOID*)(GuidHob.Guid + 1);
      }
    }
    HobStart = GET_NEXT_HOB (GuidHob);
  }
  return NULL;
}

/**

  Memory copy.

  @param  Destination           Target address.
  @param  Source                Orignal address.
  @param  Length                Data length
  
**/
STATIC
UINTN
Memcmp (
  IN VOID   *Destination,
  IN VOID   *Source,
  IN UINT8  Length
  )
{
  UINT8 *Destination8;
  UINT8 *Source8;
  
  Destination8 = Destination;
  Source8 = Source;
  while (Length--) {
    if (*(Destination8++) != *(Source8++)) {
     return 0;
    }
  }
  return 1;
}

/*++

  Allocate Buffer

  @param  AllocSize             Alloc Size
  @param  Pool                  Place to store pointer to the memory buffer

  @retval EFI_SUCCESS           Success
  @retval EFI_DEVICE_ERROR      Fail

--*/
EFI_STATUS
EFIAPI
AllocateBuffer (
  IN  UINTN                             AllocSize,
  IN  UINTN                             Alignment,
  OUT VOID                              **Pool
  )
{
  EFI_STATUS            Status;
  UINT8                 ByteValue;
  UINT8                 BitValue;
  UINTN                 TempBytePos;
  UINTN                 FoundBytePos;
  UINTN                 Index;
  UINTN                 FoundBitPos;
  UINTN                 NumberOfZeros;
  UINTN                 Count;
  UINTN                 RealAllocSize;
  UINTN                 Stepping;
  UINTN                 BitArraySizeInBits;
  UINTN                 CurrBitPos;
  UINTN                 MemoryType;
  UINT8                 *MemoryBitArray;
  EXTEND_MEMORY_TOKEN   *ExtendMemoryToken;
  VOID                  *Ptr;
  EFI_PHYSICAL_ADDRESS  Addr;
  VOID                  *Map;
  UINTN                 Size;
  //
  // Parameter checking
  //
  if ((Alignment & BOUNDARY_CHECK) && AllocSize > 0x1000) {
    return EFI_INVALID_PARAMETER;
  }
  Status        = EFI_SUCCESS;
  RealAllocSize = 0;
  //
  // Prevent SMI interfere memory allocation
  //
  EnterCriticalSection();
  *Pool         = NULL;
  //
  // Allocate extern memory buffer for large page aligned memory request to reduce the consumption of common pool
  //
  if (!mInSmram &&
      (mIommu != NULL) &&
      ((Alignment & (NON_SMM_ADDRESS + ALIGNMENT_PAGE)) == (NON_SMM_ADDRESS + ALIGNMENT_PAGE)) &&
      (AllocSize >= 0x10000)) {
    Ptr = (VOID*)(UINTN)0xffffffff;
    Status = mIommu->AllocateBuffer (
                       mIommu,
                       AllocateMaxAddress,
                       EfiRuntimeServicesData,
                       EFI_SIZE_TO_PAGES (AllocSize),
                       &Ptr,
                       IOMMU_ATTRIBUTE_MEMORY_WRITE_COMBINE | IOMMU_ATTRIBUTE_MEMORY_CACHED
                       );
    if (EFI_ERROR (Status)) {
      goto ON_EXIT;
    }
    Size = AllocSize;
    Status = mIommu->Map (
                       mIommu,
                       IoMmuOperationBusMasterCommonBuffer,
                       Ptr,
                       &Size,
                       &Addr,
                       &Map
                       );
    if (EFI_ERROR (Status)) {
      goto ON_EXIT;
    }
    Status = AllocateBuffer (
               sizeof (EXTEND_MEMORY_TOKEN),
               ALIGNMENT_32 | NON_SMM_ADDRESS,
               (VOID **)&ExtendMemoryToken
               );
    if (EFI_ERROR (Status)) {
      goto ON_EXIT;
    }
    ExtendMemoryToken->HostAddr   = Ptr;
    ExtendMemoryToken->DeviceAddr = (VOID*)(UINTN)Addr;
    ExtendMemoryToken->Size       = Size;
    ExtendMemoryToken->Map        = Map;
    *Pool                         = Ptr;
    RealAllocSize                 = Size >> 5;
    InsertTailList (&mPrivate->ExtendMemoryPoolListHead, (LIST_ENTRY*)ExtendMemoryToken);
    goto ON_EXIT;
  }
  //
  // Decide the memory type is Non-Smm or Smm
  //
  if (Alignment & (NON_SMM_ADDRESS | SMM_ADDRESS)) {
    //
    // Force memory type
    //
    MemoryType = (Alignment & NON_SMM_ADDRESS) ? 0 : 1;
    Alignment &= ~(UINTN)(NON_SMM_ADDRESS | SMM_ADDRESS);
  } else {
    //
    // Default, decided by current mode
    //
    MemoryType = (mPrivate->CurrMode != USB_CORE_RUNTIME_MODE) ? 0 : 1;
  }
  MemoryBitArray     = mPrivate->MemoryBitArray[MemoryType];
  BitArraySizeInBits = BITARRAY_SIZE_IN_BYTES << 3;
  if (MemoryType == 1) BitArraySizeInBits = BitArraySizeInBits / SMM_MEMORY_BLOCK_DEVIDER;
  //
  // allocate unit is 32 bytes (align on 32 byte)
  //
  RealAllocSize = (AllocSize + 0x1f) >> 5;
  //
  // There may be linked MemoryHeaders.
  // To allocate a free pool in Memory blocks,
  // must search in the MemoryHeader link list
  // until enough free pool is found.
  //
  Stepping           = (Alignment & (UINTN)0xffff) >> 5;
  FoundBitPos        = (mPrivate->MemoryLastBitPos[MemoryType] + (Stepping - 1)) & ~(Stepping - 1);
  if (FoundBitPos >= BitArraySizeInBits) FoundBitPos = 0;
  ByteValue          = MemoryBitArray[FoundBitPos >> 3];
  NumberOfZeros      = 0;

  for (Index = 0, CurrBitPos = FoundBitPos; Index < BitArraySizeInBits; Index ++) {
    //
    // Page boundary checking if BOUNDARY_CHECK
    //
    if ((Alignment & BOUNDARY_CHECK) && NumberOfZeros == 0) {
      if ((CurrBitPos >> 7) != ((CurrBitPos + RealAllocSize) >> 7)) {
        Count        = ((CurrBitPos + RealAllocSize) & (UINTN)~0x7f) - CurrBitPos;
        Index       += Count;
        FoundBitPos += Count;
        CurrBitPos   = FoundBitPos;
        if (CurrBitPos >= BitArraySizeInBits) {
          CurrBitPos  = 0;
          FoundBitPos = 0;
        }
      }
    }
    if ((CurrBitPos & 0x07) == 0) {
      //
      // step forward a byte, getting the byte value,
      // and reset the bit pos.
      //
      ByteValue = MemoryBitArray[CurrBitPos >> 3];
    }
    //
    // Pop out BitValue from a byte in TempBytePos.
    //
    BitValue = (UINT8) (ByteValue & (0x01 << (CurrBitPos & 0x07)));

    if (BitValue == 0) {
      //
      // Found a free bit, the NumberOfZeros only record the number
      // of those consecutive zeros
      //
      NumberOfZeros++;
      //
      // Found enough consecutive free space, break the loop
      //
      if (NumberOfZeros >= RealAllocSize) {
        break;
      }
    } else {
      //
      // Encountering a '1', meant the bit is ocupied.
      //
      if (NumberOfZeros >= RealAllocSize) {
        //
        // Found enough consecutive free space,break the loop
        //
        break;
      } else {
        //
        // the NumberOfZeros only record the number of those consecutive zeros,
        // so reset the NumberOfZeros to 0 when encountering '1' before finding
        // enough consecutive '0's
        //
        NumberOfZeros = 0;
        //
        // reset the CurrBitPos and FoundBitPos
        //
        Count        = ((CurrBitPos - FoundBitPos + 1) + (Stepping - 1)) & ~(Stepping - 1);
        Index       += Count - (CurrBitPos - FoundBitPos + 1);
        FoundBitPos += Count;
        CurrBitPos   = FoundBitPos - 1;
        ByteValue    = MemoryBitArray[FoundBitPos >> 3];
      }
    }

    //
    // step forward a bit
    //
    CurrBitPos ++;
    if (CurrBitPos >= BitArraySizeInBits) {
      CurrBitPos    = 0;
      FoundBitPos   = 0;
      NumberOfZeros = 0;
    }
  }
  if (NumberOfZeros < RealAllocSize) {
    Status = EFI_OUT_OF_RESOURCES;
    goto ON_EXIT;
  }
  //
  // Make the MemoryLastBitPos from start position if alloc size is 4k.
  //
  mPrivate->MemoryLastBitPos[MemoryType] = (RealAllocSize < 128) ? FoundBitPos + RealAllocSize : 0;
  //
  // Round down to 0 if size exceed
  //
  if (mPrivate->MemoryLastBitPos[MemoryType] >= BitArraySizeInBits) mPrivate->MemoryLastBitPos[MemoryType] = 0;
  //
  // Setup FoundBytePos and FoundBitPos
  //
  FoundBytePos = FoundBitPos >> 3;
  FoundBitPos &= (UINTN)0x07;
  //
  // Set the memory as allocated
  //
  for (TempBytePos = FoundBytePos, Index = FoundBitPos, Count = 0; Count < RealAllocSize; Count++) {
    MemoryBitArray[TempBytePos] |= (1 << Index);
    Index++;
    if (Index == 8) {
      TempBytePos += 1;
      Index = 0;
    }
  }
  *Pool = mPrivate->MemoryBlockPtr[MemoryType] + (FoundBytePos * 8 + FoundBitPos) * 32;
ON_EXIT:
  if (!EFI_ERROR (Status)) {
    ZeroMem(*Pool, RealAllocSize << 5);
  }
  LeaveCriticalSection();
  return Status;
}

/**

  Free Buffer

  @param  AllocSize             Pool size
  @param  Pool                  Pool to free

**/
EFI_STATUS
EFIAPI
FreeBuffer (
  IN UINTN                              AllocSize,
  IN VOID                               *Addr
  )
{
  UINTN                 StartBytePos;
  UINTN                 Index;
  UINTN                 StartBitPos;
  UINTN                 Index2;
  UINTN                 Count;
  UINTN                 RealAllocSize;
  UINTN                 Pool;
  UINTN                 MemoryBase;
  UINTN                 MemoryType;
  UINT8                 *MemoryBitArray;
  UINTN                 MemoryBlockSize;
  EXTEND_MEMORY_TOKEN   *ExtendMemoryToken;
  //
  // Prevent SMI interfere memory allocation
  //
  EnterCriticalSection();
  Pool = (UINTN)Addr;
  //
  // Free extern memory buffer if it was allocated for large page aligned memory
  //
  if (!mInSmram && (mIommu != NULL) && (AllocSize >= 0x10000) && !IsListEmpty (&mPrivate->ExtendMemoryPoolListHead)) {
    ExtendMemoryToken = (EXTEND_MEMORY_TOKEN*)&mPrivate->ExtendMemoryPoolListHead;
    if (!IsInUsbMemory (ExtendMemoryToken->Link.ForwardLink, sizeof (EXTEND_MEMORY_TOKEN)) || !IsInUsbMemory (ExtendMemoryToken->Link.BackLink, sizeof (EXTEND_MEMORY_TOKEN))) return EFI_SECURITY_VIOLATION;
    do {
      if (IsListEmpty (&mPrivate->ExtendMemoryPoolListHead)) break;
      ExtendMemoryToken = (EXTEND_MEMORY_TOKEN*)GetFirstNode((LIST_ENTRY*)ExtendMemoryToken);
      if (!IsInUsbMemory (ExtendMemoryToken->Link.ForwardLink, sizeof (EXTEND_MEMORY_TOKEN)) || !IsInUsbMemory (ExtendMemoryToken->Link.BackLink, sizeof (EXTEND_MEMORY_TOKEN))) return EFI_SECURITY_VIOLATION;
      if (ExtendMemoryToken->HostAddr == Addr) {
        mIommu->Unmap (
                  mIommu,
                  ExtendMemoryToken->Map
                  );
        mIommu->FreeBuffer (
                  mIommu,
                  EFI_SIZE_TO_PAGES (AllocSize),
                  ExtendMemoryToken->HostAddr
                  );
        RemoveEntryList((LIST_ENTRY*)ExtendMemoryToken);
        FreeBuffer (
          sizeof (EXTEND_MEMORY_TOKEN),
          ExtendMemoryToken
          );
        goto ON_EXIT;
      }
    } while (!IsNodeAtEnd (&mPrivate->ExtendMemoryPoolListHead, (LIST_ENTRY*)ExtendMemoryToken));
  }
  //
  // Decide the memory type is Non-Smm or Smm
  //
  if (Pool >= (UINTN)mPrivate->MemoryBlockPtr[0] && 
      Pool < (UINTN)(mPrivate->MemoryBlockPtr[0] + MEMORY_BLOCK_SIZE_IN_BYTES)) {
    MemoryType      = 0;
    MemoryBlockSize = MEMORY_BLOCK_SIZE_IN_BYTES;
  } else {
    MemoryType      = 1;
    MemoryBlockSize = MEMORY_BLOCK_SIZE_IN_BYTES / SMM_MEMORY_BLOCK_DEVIDER;
  }
  MemoryBitArray = mPrivate->MemoryBitArray[MemoryType];
  MemoryBase     = (UINTN)mPrivate->MemoryBlockPtr[MemoryType];
  //
  // allocate unit is 32 byte (align on 32 byte)
  //
  RealAllocSize = (AllocSize + 0x1f) & ~0x1f;
  //
  // scan the memory header linked list for
  // the asigned memory to free.
  //
  if ((Pool >= MemoryBase) &&
      ((Pool + RealAllocSize) <= (MemoryBase + MemoryBlockSize))
      ) {
    //
    // Pool is in the Memory Block area,
    // find the start byte and bit in the bit array
    //
    StartBitPos                            = (Pool - MemoryBase) >> 5;
    if (mPrivate->MemoryLastBitPos[MemoryType] > StartBitPos) {
      //
      // Restrict the grow direction as negative to reduce the memory fragmental
      //
      mPrivate->MemoryLastBitPos[MemoryType] = StartBitPos;
    }
    StartBytePos                           = StartBitPos >> 3;
    StartBitPos                           &= 0x07;

    //
    // reset associated bits in bit arry
    //
    for (Index = StartBytePos, Index2 = StartBitPos, Count = 0; Count < (RealAllocSize / 32); Count++) {
      MemoryBitArray[Index] ^= (UINT8) (1 << Index2);
      Index2++;
      if (Index2 == 8) {
        Index += 1;
        Index2 = 0;
      }
    }
  }
ON_EXIT:
  LeaveCriticalSection();
  return EFI_SUCCESS;
}

/**

  Build the USB working region record for use of other drivers

  @param  Address               Region start address
  @param  Length                Region length

**/
STATIC
VOID
BuildUsbWorkingRegionRecord (
  IN UINT64                     Address,
  IN UINT64                     Length
  )
{
  EFI_STATUS            Status;
  UINTN                 Index;
  USB_WORKING_REGION    *UsbWorkingRegion;
  
  for (Index = 0, UsbWorkingRegion = NULL; Index < gST->NumberOfTableEntries; Index ++) {
    if (Memcmp (&mUsbWorkingRegionHobGuid, &(gST->ConfigurationTable[Index].VendorGuid), sizeof(EFI_GUID))) {
      UsbWorkingRegion = (USB_WORKING_REGION*)gST->ConfigurationTable[Index].VendorTable;
      break;
    }
  }
  if (!UsbWorkingRegion) {
    //
    // The working region record did not exist so add one.
    //
    Status = AllocateBuffer (
               sizeof (USB_WORKING_REGION),
               ALIGNMENT_32 | NON_SMM_ADDRESS,
               (VOID **)&UsbWorkingRegion
               );
    if (Status != EFI_SUCCESS || UsbWorkingRegion == NULL) return;
    UsbWorkingRegion->DxeWorkingMemoryStart  = Address;
    UsbWorkingRegion->DxeWorkingMemoryLength = Length;
    UsbWorkingRegion->SmmWorkingMemoryStart  = 0;
    UsbWorkingRegion->SmmWorkingMemoryLength = 0;
    gBS->InstallConfigurationTable (&mUsbWorkingRegionHobGuid, UsbWorkingRegion);
  } else {
    //
    // The Hob exist, set the Hob content
    //
    UsbWorkingRegion->SmmWorkingMemoryStart  = Address;
    UsbWorkingRegion->SmmWorkingMemoryLength = Length;
  }
}

/**

  Memory Map for 4G concern

  @param  Direction             Direction type
  @param  Addr                  Data address
  @param  Length                Data size
  @param  AdjustedAddr          Adjusted data address for output
  @param  Handle                Handle for output

**/
EFI_STATUS
EFIAPI
MemoryMapping (
  IN  UINTN                             Direction,
  IN  EFI_PHYSICAL_ADDRESS              Addr,
  IN  UINTN                             Length,
  OUT VOID                              **AdjustedAddr,
  OUT VOID                              **Handle
  )
{
  EFI_STATUS            Status;
  MEMORY_MAPPING        *HandleBuffer;
  UINT32                PacketSize;
  BOOLEAN               IsCross64K;
  UINT8                 *Ptr;
  EXTEND_MEMORY_TOKEN   *ExtendMemoryToken;

  IsCross64K = FALSE;
  if ((Direction & 0xffff0000) != 0) {
    //
    // Crossing 64K boundary with PacketSize alignment check to preventing portantial babble error 
    //
    PacketSize = (UINT32)Direction >> 16;
    if ((Addr & ~0xffff) != ((Addr + Length) & ~0xffff) && (((UINT32)Addr % PacketSize) != 0)) {
      IsCross64K = TRUE;
    }
  }
  if (!IsCross64K && (Addr < 0x100000000LL) &&
      (Addr < 0xa0000 || Addr >= 0x100000) && 
      !((Direction & USB_CORE_BUS_MASTER_ALIGN4) && (Addr & 0x03)) &&
      !((Addr >= mPrivate->SmmRegionStart[0] && Addr < mPrivate->SmmRegionEnd[0]) || 
        (Addr >= mPrivate->SmmRegionStart[1] && Addr < mPrivate->SmmRegionEnd[1])) &&
      mPrivate->IommuMap == NULL) {
    *AdjustedAddr = *(VOID**)&Addr;
    *Handle = NULL;
  } else {
    Ptr = (UINT8*)(UINTN)Addr;
    if (mPrivate->IommuMap) {
      if (Ptr >= mPrivate->MemoryBlockPtr[0] && Ptr < (mPrivate->MemoryBlockPtr[0] + MEMORY_BLOCK_SIZE_IN_BYTES)) {
        *AdjustedAddr = (Ptr - mPrivate->MemoryBlockPtr[0]) + (UINT8*)(UINTN)mPrivate->IommuDma;
        *Handle = NULL;
        return EFI_SUCCESS;
      } else if (!IsListEmpty (&mPrivate->ExtendMemoryPoolListHead)) { 
        ExtendMemoryToken = (EXTEND_MEMORY_TOKEN*)&mPrivate->ExtendMemoryPoolListHead;
        if (!IsInUsbMemory (ExtendMemoryToken->Link.ForwardLink, sizeof (EXTEND_MEMORY_TOKEN)) || !IsInUsbMemory (ExtendMemoryToken->Link.BackLink, sizeof (EXTEND_MEMORY_TOKEN))) return EFI_SECURITY_VIOLATION;
        do {
          ExtendMemoryToken = (EXTEND_MEMORY_TOKEN*)GetFirstNode((LIST_ENTRY*)ExtendMemoryToken);
          if (!IsInUsbMemory (ExtendMemoryToken->Link.ForwardLink, sizeof (EXTEND_MEMORY_TOKEN)) || !IsInUsbMemory (ExtendMemoryToken->Link.BackLink, sizeof (EXTEND_MEMORY_TOKEN))) return EFI_SECURITY_VIOLATION;
          if (Ptr >= (UINT8*)ExtendMemoryToken->HostAddr && Ptr < (UINT8*)((UINTN)ExtendMemoryToken->HostAddr + ExtendMemoryToken->Size)) {
            *AdjustedAddr = ((UINTN)Ptr - (UINTN)ExtendMemoryToken->HostAddr) + (UINT8*)(UINTN)ExtendMemoryToken->DeviceAddr;
            *Handle = NULL;
            return EFI_SUCCESS;
          }
        } while (!IsNodeAtEnd (&mPrivate->ExtendMemoryPoolListHead, (LIST_ENTRY*)ExtendMemoryToken));
      }
    }
    Status = AllocateBuffer(
               Length,
               ALIGNMENT_64 | NON_SMM_ADDRESS,
               AdjustedAddr
               );
    if (Status != EFI_SUCCESS) {
      if (IsCross64K) {
        //
        // Don't allocate memory when crossing 64K boundary if out of resource 
        //
        *AdjustedAddr = *(VOID**)&Addr;
        *Handle = NULL;
        return EFI_SUCCESS;
      }
      return EFI_OUT_OF_RESOURCES;
    }
    Direction &= 1;
    if (Direction == USB_CORE_BUS_MASTER_READ) {
      CopyMem(
        *AdjustedAddr,
        *(VOID**)&Addr,
        Length
        );
    }
    Status = AllocateBuffer(
               sizeof (MEMORY_MAPPING),
               ALIGNMENT_32,
               (VOID**)&HandleBuffer
               );
    if (Status != EFI_SUCCESS || HandleBuffer == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
    HandleBuffer->Address         = Addr;
    HandleBuffer->Length          = Length;
    HandleBuffer->AdjustedAddress = *AdjustedAddr;
    HandleBuffer->Direction       = Direction;
    *Handle = (VOID*)HandleBuffer;
    if (mPrivate->IommuMap) {
      *AdjustedAddr = ((UINT8*)(*AdjustedAddr) - mPrivate->MemoryBlockPtr[0]) + (UINT8*)(UINTN)mPrivate->IommuDma;
    }
  }
  return EFI_SUCCESS;
}

/**

  Memory Unmapping

  @param  Handle                Handle produced by MemoryMapping

**/
EFI_STATUS
EFIAPI
MemoryUnmapping (
  IN VOID                               *Handle
  )
{
  MEMORY_MAPPING        *HandleBuffer;

  if (Handle != NULL) {
    HandleBuffer = (MEMORY_MAPPING*)Handle;
    if (HandleBuffer->Direction == USB_CORE_BUS_MASTER_WRITE) {
      CopyMem(
        *(VOID**)&HandleBuffer->Address,
        HandleBuffer->AdjustedAddress,
        HandleBuffer->Length
        );
    }
    FreeBuffer(
      sizeof (MEMORY_MAPPING),
      Handle
      );
    FreeBuffer(
      HandleBuffer->Length,
      HandleBuffer->AdjustedAddress
      );
  }
  return EFI_SUCCESS;
}

/**

  Signal handlers for Usb3Hc Protocol event

  @param  Event                 Event type                
  @param  Context               Context for the event

**/
VOID
EFIAPI
Usb3HcNotifyFunction (
  IN EFI_EVENT          Event,
  IN VOID               *Context
  )
{
  EFI_STATUS            Status;
  UINTN                 HandleSize;
  EFI_HANDLE            Handle;
  EXTEND_MEMORY_TOKEN   *ExtendMemoryToken;
  
  while (TRUE) {
    HandleSize = sizeof(EFI_HANDLE);
    Status = gBS->LocateHandle(
                    ByRegisterNotify,
                    &gEfiUsb3HcProtocolGuid,
                    mRegistration,
                    &HandleSize,
                    &Handle
                    );
    if (Status != EFI_SUCCESS) break;
    mIommu->SetAttribute (
              mIommu,
              Handle,
              mPrivate->IommuMap,
              IOMMU_ACCESS_READ | IOMMU_ACCESS_WRITE
              );
    if (!IsListEmpty (&mPrivate->ExtendMemoryPoolListHead)) { 
      ExtendMemoryToken = (EXTEND_MEMORY_TOKEN*)&mPrivate->ExtendMemoryPoolListHead;
      if (!IsInUsbMemory (ExtendMemoryToken->Link.ForwardLink, sizeof (EXTEND_MEMORY_TOKEN)) || !IsInUsbMemory (ExtendMemoryToken->Link.BackLink, sizeof (EXTEND_MEMORY_TOKEN))) return;
      do {
        ExtendMemoryToken = (EXTEND_MEMORY_TOKEN*)GetFirstNode((LIST_ENTRY*)ExtendMemoryToken);
        if (!IsInUsbMemory (ExtendMemoryToken->Link.ForwardLink, sizeof (EXTEND_MEMORY_TOKEN)) || !IsInUsbMemory (ExtendMemoryToken->Link.BackLink, sizeof (EXTEND_MEMORY_TOKEN))) return;
        mIommu->SetAttribute (
                  mIommu,
                  Handle,
                  ExtendMemoryToken->Map,
                  IOMMU_ACCESS_READ | IOMMU_ACCESS_WRITE
                  );
      } while (!IsNodeAtEnd (&mPrivate->ExtendMemoryPoolListHead, (LIST_ENTRY*)ExtendMemoryToken));
    }
  }
}

/**

  Signal handlers for IOMMU Protocol event

  @param  Event                 Event type                
  @param  Context               Context for the event

**/
VOID
EFIAPI
IommuNotifyFunction (
  IN EFI_EVENT          Event,
  IN VOID               *Context
  )
{
  EFI_STATUS            Status;
  VOID                  *Ptr;
  EFI_PHYSICAL_ADDRESS  Addr;
  VOID                  *Map;
  UINTN                 Size;
  //
  // Try IOMMU protocol and use it if installed
  //
  Status = gBS->LocateProtocol (
                  &mIommuProtocolGuid,
                  NULL,
                  (VOID **)&mIommu
                  );
  if (EFI_ERROR (Status)) {
    return;
  }
  if (mPrivate->MemoryBlockPtr[0] == NULL) {
    //
    // The commom buffer doesn't allocated, allocate buffer from Iommu protocol
    //
    Ptr = (VOID*)(UINTN)0xffffffff;
    Status = mIommu->AllocateBuffer (
                       mIommu,
                       AllocateMaxAddress,
                       EfiRuntimeServicesData,
                       MEMORY_BLOCK_UNIT_IN_PAGES,
                       &Ptr,
                       IOMMU_ATTRIBUTE_MEMORY_WRITE_COMBINE | IOMMU_ATTRIBUTE_MEMORY_CACHED
                       );
    if (EFI_ERROR (Status)) {
      return;
    }
  } else {
    //
    // The common buffer allocated by gBS->AllocatePages, use it directly
    //
    Ptr = mPrivate->MemoryBlockPtr[0];
  }
  Size = MEMORY_BLOCK_SIZE_IN_BYTES;
  Status = mIommu->Map (
                     mIommu,
                     IoMmuOperationBusMasterCommonBuffer,
                     Ptr,
                     &Size,
                     &Addr,
                     &Map
                     );
  if (EFI_ERROR (Status)) {
    goto ON_ERROR;
  }
  mPrivate->MemoryBlockPtr[0] = Ptr;
  mPrivate->IommuDma = Addr;
  mPrivate->IommuMap = Map;
  //
  // Registers the Usb3Hc protocol notification
  //
  Status = gBS->CreateEvent (
                  EVT_NOTIFY_SIGNAL,
                  TPL_NOTIFY,
                  Usb3HcNotifyFunction,
                  NULL,
                  &Event
                  );
  if (EFI_ERROR (Status)) {
    goto ON_ERROR;
  }
  Status = gBS->RegisterProtocolNotify (
                  &gEfiUsb3HcProtocolGuid,
                  Event,
                  &mRegistration
                  );
  if (!EFI_ERROR (Status)) {
    return;
  }
ON_ERROR:
  mIommu->FreeBuffer (
            mIommu,
            MEMORY_BLOCK_UNIT_IN_PAGES,
            Ptr
            );
}

/*++

  Initialize Memory Management

  @param  Type                  0 : NON-SMM initialization, 1 : SMM initialization

  @retval EFI_SUCCESS           Success
  @retval EFI_DEVICE_ERROR      Fail
  
--*/
EFI_STATUS
EFIAPI
InitialMemoryManagement (
  IN UINTN                              Type
  )
{
  EFI_STATUS                            Status;
  EFI_PHYSICAL_ADDRESS                  Addr;
  EFI_SMRAM_HOB_DESCRIPTOR_BLOCK        *SmramBlock;
  EFI_HOB_HANDOFF_INFO_TABLE            *HobTable;
  UINTN                                 Index;
  UINTN                                 Index1;
  UINTN                                 SmramCounter;
  UINTN                                 Length;
  USB_CORE_PRIVATE                      *Private;
  UINT64                                WorkingMemoryStart;
  UINT64                                WorkingMemoryLength;
  IOMMU_PROTOCOL                        *Iommu;
  EFI_EVENT                             Event;
  VOID                                  *Registration;
  
  if (!Type) {
    //
    // Pre-allocate private storage before memory management initialize
    //
    mPrivate = AllocateZeroPool (sizeof (USB_CORE_PRIVATE));
    if (mPrivate == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
    //
    // Try IOMMU protocol and use it if installed
    //
    Status = gBS->LocateProtocol (
                    &mIommuProtocolGuid,
                    NULL,
                    (VOID **)&Iommu
                    );
    if (!EFI_ERROR (Status)) {
      //
      // Notify the Legacy8259 callback directly
      //
      IommuNotifyFunction (0, 0);
    } else {
      //
      // Registers the Iommu protocol notification
      //
      Status = gBS->CreateEvent (
                      EVT_NOTIFY_SIGNAL,
                      TPL_CALLBACK,
                      IommuNotifyFunction,
                      NULL,
                      &Event
                      );
      if (EFI_ERROR (Status)) {
        return Status;
      }
      Status = gBS->RegisterProtocolNotify (
                      &mIommuProtocolGuid,
                      Event,
                      &Registration
                      );
      if (EFI_ERROR (Status)) {
        return Status;
      }
    }
    if (mPrivate->MemoryBlockPtr[0] == NULL) {
      //
      // Memory Block uses MemoryBlockSizeInPages pages,
      // and it is allocated as common buffer use.
      //
      Addr = 0xffffffff;
      Status = gBS->AllocatePages (
                      AllocateMaxAddress,
                      EfiReservedMemoryType,
                      MEMORY_BLOCK_UNIT_IN_PAGES,
                      &Addr
                      );
      if (EFI_ERROR (Status)) {
        return EFI_OUT_OF_RESOURCES;
      }
      mPrivate->MemoryBlockPtr[0] = *(UINT8**)&Addr;
    }
    ZeroMem ((VOID*)((UINTN)(mPrivate->MemoryBlockPtr[0])), MEMORY_BLOCK_SIZE_IN_BYTES);
    //
    // Get Smm Memory Region through Hob
    //
    for (Index = 0, HobTable = NULL; Index < gST->NumberOfTableEntries; Index ++) {
      if (Memcmp(&gEfiHobListGuid, &(gST->ConfigurationTable[Index].VendorGuid), sizeof(EFI_GUID))) {
        HobTable = (EFI_HOB_HANDOFF_INFO_TABLE*)gST->ConfigurationTable[Index].VendorTable;
        break;
      }
    }
    if (HobTable) {
      SmramBlock = SearchGuidHob(HobTable, &gEfiSmmPeiSmramMemoryReserveGuid, &Length);
      if (SmramBlock) {
        for (Index = 0, SmramCounter = 0; Index < SmramBlock->NumberOfSmmReservedRegions; Index ++) {
          for (Index1 = 0; Index1 < SmramCounter; Index1 ++) {
            if (mPrivate->SmmRegionEnd[Index1] == SmramBlock->Descriptor[Index].PhysicalStart) {
              mPrivate->SmmRegionEnd[Index1] += SmramBlock->Descriptor[Index].PhysicalSize;
              SmramCounter = Index1;
            }
          }
          if (SmramCounter < 2 && mPrivate->SmmRegionStart[SmramCounter] == 0) {
            mPrivate->SmmRegionStart[SmramCounter] = SmramBlock->Descriptor[Index].PhysicalStart;
            mPrivate->SmmRegionEnd[SmramCounter]   = mPrivate->SmmRegionStart[SmramCounter] + SmramBlock->Descriptor[Index].PhysicalSize;
          }
          SmramCounter ++;
        }
      }
    }
    //
    // Initial linking head(in the temporary mPrivate) in used for AllocateBuffer
    //
    InitializeListHead(&mPrivate->DeferredHcCallbackListHead);
    //
    // Allocate private storage inside managed memory
    //
    Status = AllocateBuffer(
               sizeof (USB_CORE_PRIVATE),
               ALIGNMENT_32 | NON_SMM_ADDRESS,
               (VOID **)&Private
               );
    if (Status != EFI_SUCCESS || Private == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
    //
    // Copy the previous private data into permanent private storage
    //
    CopyMem (Private, mPrivate, sizeof (USB_CORE_PRIVATE));
    //
    // Setup permanent private storage
    //
    gBS->FreePool (mPrivate);
    mPrivate = Private;
    //
    // Initial linking head(in the permanent mPrivate) in used for AllocateBuffer
    //
    InitializeListHead(&mPrivate->DeferredHcCallbackListHead);
    //
    // Setting the memory address of DXE working region
    //
    WorkingMemoryStart  = (UINT64)(UINTN)mPrivate->MemoryBlockPtr[0];
    WorkingMemoryLength = MEMORY_BLOCK_SIZE_IN_BYTES;
    mDxeWorkingMemory = (VOID*)(UINTN)mPrivate->MemoryBlockPtr[0];
    if (mSmmWorkingMemory == NULL) mSmmWorkingMemory = (VOID*)(UINTN)mPrivate->MemoryBlockPtr[1];
  } else {
    //
    // Allocate SMM memory as quarter of NON-SMM memmory in used for SMM
    //
    Status = mSmst->SmmAllocatePages (
                      AllocateAnyPages,
                      EfiRuntimeServicesData,
                      MEMORY_BLOCK_UNIT_IN_PAGES / SMM_MEMORY_BLOCK_DEVIDER,
                      &Addr
                      );
    if (EFI_ERROR (Status)) {
      return EFI_OUT_OF_RESOURCES;
    }
    mPrivate->MemoryBlockPtr[1] = *(UINT8**)&Addr;
    ZeroMem ((VOID*)((UINTN)(mPrivate->MemoryBlockPtr[1])), MEMORY_BLOCK_SIZE_IN_BYTES / 4);
    //
    // Setting the memory address of SMM working region
    //
    WorkingMemoryStart  = (UINT64)(UINTN)mPrivate->MemoryBlockPtr[1];
    WorkingMemoryLength = MEMORY_BLOCK_SIZE_IN_BYTES / SMM_MEMORY_BLOCK_DEVIDER;
    mSmmWorkingMemory = (VOID*)(UINTN)mPrivate->MemoryBlockPtr[1];
    if (mDxeWorkingMemory == NULL) mDxeWorkingMemory = (VOID*)(UINTN)mPrivate->MemoryBlockPtr[0];
  }
  //
  // Create Hob for storing the memory address of DXE/SMM working region
  //
  BuildUsbWorkingRegionRecord (WorkingMemoryStart, WorkingMemoryLength);
  return EFI_SUCCESS;
}

/*++

  Check is the address in USB pre-allocated memory area

  @param  Address               Memory address for checking
  @param  Length                Memory block length

  @retval TRUE                  Memory address is inside of USB pre-allocated memory area
  @retval FALSE                 Memory address is outside of USB pre-allocated memory area
  
--*/
BOOLEAN
EFIAPI
IsInUsbMemory (
  IN VOID                       *Address,
  IN UINTN                      Length
  )
{
  if (mDxeWorkingMemory == NULL) mDxeWorkingMemory = (VOID*)(UINTN)mPrivate->MemoryBlockPtr[0];
  if (mSmmWorkingMemory == NULL) mSmmWorkingMemory = (VOID*)(UINTN)mPrivate->MemoryBlockPtr[1];
  if ((Address >= mDxeWorkingMemory &&
       Address < (VOID*)((UINTN)mDxeWorkingMemory + MEMORY_BLOCK_SIZE_IN_BYTES - Length)) ||
      (Address >= mSmmWorkingMemory &&
       Address < (VOID*)((UINTN)mSmmWorkingMemory + (MEMORY_BLOCK_SIZE_IN_BYTES / SMM_MEMORY_BLOCK_DEVIDER) - Length))) {
    return TRUE;
  }
  return FALSE;
}