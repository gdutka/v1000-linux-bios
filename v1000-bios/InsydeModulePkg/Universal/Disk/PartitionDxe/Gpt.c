/** @file

;******************************************************************************
;* Copyright (c) 2012 - 2018, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

/**
  Decode a hard disk partitioned with the GPT scheme in the UEFI 2.0
  specification.

Copyright (c) 2006 - 2013, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/


#include "Partition.h"

/**
  Install child handles if the Handle supports GPT partition structure.

  @param[in]  BlockIo     Parent BlockIo interface.
  @param[in]  DiskIo      Disk Io protocol.
  @param[in]  Lba         The starting Lba of the Partition Table
  @param[out] PartHeader  Stores the partition table that is read

  @retval TRUE      The partition table is valid
  @retval FALSE     The partition table is not valid

**/
BOOLEAN
PartitionValidGptTable (
  IN  EFI_BLOCK_IO_PROTOCOL       *BlockIo,
  IN  EFI_DISK_IO_PROTOCOL        *DiskIo,
  IN  EFI_LBA                     Lba,
  OUT EFI_PARTITION_TABLE_HEADER  *PartHeader
  );

/**
  Check if the CRC field in the Partition table header is valid
  for Partition entry array.

  @param[in]  BlockIo     Parent BlockIo interface
  @param[in]  DiskIo      Disk Io Protocol.
  @param[in]  PartHeader  Partition table header structure

  @retval TRUE      the CRC is valid
  @retval FALSE     the CRC is invalid

**/
BOOLEAN
PartitionCheckGptEntryArrayCRC (
  IN  EFI_BLOCK_IO_PROTOCOL       *BlockIo,
  IN  EFI_DISK_IO_PROTOCOL        *DiskIo,
  IN  EFI_PARTITION_TABLE_HEADER  *PartHeader
  );


/**
  Restore Partition Table to its alternate place
  (Primary -> Backup or Backup -> Primary).

  @param[in]  BlockIo     Parent BlockIo interface.
  @param[in]  DiskIo      Disk Io Protocol.
  @param[in]  PartHeader  Partition table header structure.

  @retval TRUE      Restoring succeeds
  @retval FALSE     Restoring failed

**/
BOOLEAN
PartitionRestoreGptTable (
  IN  EFI_BLOCK_IO_PROTOCOL       *BlockIo,
  IN  EFI_DISK_IO_PROTOCOL        *DiskIo,
  IN  EFI_PARTITION_TABLE_HEADER  *PartHeader
  );


/**
  Restore Partition Table to its alternate place.
  (Primary -> Backup or Backup -> Primary)

  @param[in]    PartHeader    Partition table header structure
  @param[in]    PartEntry     The partition entry array
  @param[out]   PEntryStatus  the partition entry status array
                              recording the status of each partition

**/
VOID
PartitionCheckGptEntry (
  IN  EFI_PARTITION_TABLE_HEADER  *PartHeader,
  IN  EFI_PARTITION_ENTRY         *PartEntry,
  OUT EFI_PARTITION_ENTRY_STATUS  *PEntryStatus
  );


/**
  Checks the CRC32 value in the table header.

  @param  MaxSize   Max Size limit
  @param  Size      The size of the table
  @param  Hdr       Table to check

  @return TRUE    CRC Valid
  @return FALSE   CRC Invalid

**/
BOOLEAN
PartitionCheckCrcAltSize (
  IN UINTN                 MaxSize,
  IN UINTN                 Size,
  IN OUT EFI_TABLE_HEADER  *Hdr
  );


/**
  Checks the CRC32 value in the table header.

  @param  MaxSize   Max Size limit
  @param  Hdr       Table to check

  @return TRUE      CRC Valid
  @return FALSE     CRC Invalid

**/
BOOLEAN
PartitionCheckCrc (
  IN UINTN                 MaxSize,
  IN OUT EFI_TABLE_HEADER  *Hdr
  );


/**
  Updates the CRC32 value in the table header.

  @param  Size   The size of the table
  @param  Hdr    Table to update

**/
VOID
PartitionSetCrcAltSize (
  IN UINTN                 Size,
  IN OUT EFI_TABLE_HEADER  *Hdr
  );


/**
  Updates the CRC32 value in the table header.

  @param  Hdr    Table to update

**/
VOID
PartitionSetCrc (
  IN OUT EFI_TABLE_HEADER *Hdr
  );

/**
  Internal function to check whether this handle is GPT partition handle.

  @param[in] Handle   Input EFI_HANDLE which may contain gEfiDevicePathProtocolGuid.

  @retval  TRUE       This is GPT partition.
  @retval  FALSE      This isn't a GPT partition.

**/
STATIC
BOOLEAN
IsGptPartition (
  IN  EFI_HANDLE    Handle
  )
{
  EFI_STATUS                   Status;
  EFI_DEVICE_PATH_PROTOCOL     *DevicePath;

  Status = gBS->HandleProtocol (Handle, &gEfiDevicePathProtocolGuid, (VOID *) &DevicePath);
  if (Status != EFI_SUCCESS) {
    return FALSE;
  }
  while (!IsDevicePathEnd (DevicePath)) {
    if (DevicePathType (DevicePath) == MEDIA_DEVICE_PATH &&
        DevicePathSubType (DevicePath) == MEDIA_HARDDRIVE_DP) {
      return ((HARDDRIVE_DEVICE_PATH *) DevicePath)->MBRType == MBR_TYPE_EFI_PARTITION_TABLE_HEADER;
    }
    DevicePath = NextDevicePathNode (DevicePath);
  }

  return FALSE;
}

/**
  Install child handles if the Handle supports GPT partition structure.

  @param[in]  This       Calling context.
  @param[in]  Handle     Parent Handle.
  @param[in]  DiskIo     Parent DiskIo interface.
  @param[in]  DiskIo2    Parent DiskIo2 interface.
  @param[in]  BlockIo    Parent BlockIo interface.
  @param[in]  BlockIo2   Parent BlockIo2 interface.
  @param[in]  DevicePath Parent Device Path.

  @retval EFI_SUCCESS           Valid GPT disk.
  @retval EFI_MEDIA_CHANGED     Media changed Detected.
  @retval other                 Not a valid GPT disk.

**/
EFI_STATUS
PartitionInstallGptChildHandles (
  IN  EFI_DRIVER_BINDING_PROTOCOL  *This,
  IN  EFI_HANDLE                   Handle,
  IN  EFI_DISK_IO_PROTOCOL         *DiskIo,
  IN  EFI_DISK_IO2_PROTOCOL        *DiskIo2,
  IN  EFI_BLOCK_IO_PROTOCOL        *BlockIo,
  IN  EFI_BLOCK_IO2_PROTOCOL       *BlockIo2,
  IN  EFI_DEVICE_PATH_PROTOCOL     *DevicePath
  )
{
  EFI_STATUS                  Status;
  UINT32                      BlockSize;
  EFI_LBA                     LastBlock;
  MASTER_BOOT_RECORD          *ProtectiveMbr;
  EFI_PARTITION_TABLE_HEADER  *PrimaryHeader;
  EFI_PARTITION_TABLE_HEADER  *BackupHeader;
  EFI_PARTITION_TABLE_HEADER  *ValidHeader;
  EFI_PARTITION_ENTRY         *PartEntry;
  EFI_PARTITION_ENTRY_STATUS  *PEntryStatus;
  UINTN                       Index;
  EFI_STATUS                  GptValidStatus;
  HARDDRIVE_DEVICE_PATH       HdDev;
  UINT32                      MediaId;
  BOOLEAN                     PrimaryStoreValid;

  ProtectiveMbr = NULL;
  PrimaryHeader = NULL;
  BackupHeader  = NULL;
  PartEntry     = NULL;
  PEntryStatus  = NULL;

  BlockSize     = BlockIo->Media->BlockSize;
  LastBlock     = BlockIo->Media->LastBlock;
  MediaId       = BlockIo->Media->MediaId;

  DEBUG ((EFI_D_INFO, " BlockSize : %d \n", BlockSize));
  DEBUG ((EFI_D_INFO, " LastBlock : %lx \n", LastBlock));

  if (IsGptPartition (Handle)) {
    return EFI_NOT_FOUND;
  }

  GptValidStatus = EFI_NOT_FOUND;

  //
  // Allocate a buffer for the Protective MBR
  //
  ProtectiveMbr = AllocatePool (sizeof (MASTER_BOOT_RECORD));
  if (ProtectiveMbr == NULL) {
    return EFI_NOT_FOUND;
  }

  //
  // Read the Protective MBR from LBA #0
  //
  Status = DiskIo->ReadDisk (
                     DiskIo,
                     MediaId,
                     0,
                     sizeof (MASTER_BOOT_RECORD),
                     ProtectiveMbr
                     );
  if (EFI_ERROR (Status)) {
    GptValidStatus = Status;
    goto Done;
  }

  //
  // Verify that the Protective MBR is valid
  //
  for (Index = 0; Index < MAX_MBR_PARTITIONS; Index++) {
    if (ProtectiveMbr->Partition[Index].BootIndicator == 0x00 &&
        ProtectiveMbr->Partition[Index].OSIndicator == PMBR_GPT_PARTITION &&
        UNPACK_UINT32 (ProtectiveMbr->Partition[Index].StartingLBA) == 1
        ) {
      break;
    }
  }
  if (Index == MAX_MBR_PARTITIONS) {
    goto Done;
  }

  //
  // Allocate the GPT structures
  //
  PrimaryHeader = AllocateZeroPool (sizeof (EFI_PARTITION_TABLE_HEADER));
  if (PrimaryHeader == NULL) {
    goto Done;
  }

  BackupHeader = AllocateZeroPool (sizeof (EFI_PARTITION_TABLE_HEADER));
  if (BackupHeader == NULL) {
    goto Done;
  }

  PrimaryStoreValid = TRUE;
  if (PcdGetBool (PcdRestoreGptTable)) {
    //
    // Check primary and backup partition tables
    //
    if (!PartitionValidGptTable (BlockIo, DiskIo, PRIMARY_PART_HEADER_LBA, PrimaryHeader)) {
    DEBUG ((EFI_D_ERROR, " Not Valid primary partition table\n"));

      if (!PartitionValidGptTable (BlockIo, DiskIo, LastBlock, BackupHeader)) {
      DEBUG ((EFI_D_ERROR, " Not Valid backup partition table\n"));
        goto Done;
      } else {
        DEBUG ((EFI_D_INFO, " Valid backup partition table\n"));
        DEBUG ((EFI_D_INFO, " Restore primary partition table by the backup\n"));
        if (!PartitionRestoreGptTable (BlockIo, DiskIo, BackupHeader)) {
        DEBUG ((EFI_D_ERROR, " Restore primary partition table error\n"));
        }

        if (PartitionValidGptTable (BlockIo, DiskIo, BackupHeader->AlternateLBA, PrimaryHeader)) {
          DEBUG ((EFI_D_INFO, " Restore backup partition table success\n"));
        } else {
          PrimaryStoreValid = FALSE;
        }
      }
    } else if (!PartitionValidGptTable (BlockIo, DiskIo, PrimaryHeader->AlternateLBA, BackupHeader)) {
      DEBUG ((EFI_D_INFO, " Valid primary and !Valid backup partition table\n"));
      DEBUG ((EFI_D_INFO, " Restore backup partition table by the primary\n"));
      if (!PartitionRestoreGptTable (BlockIo, DiskIo, PrimaryHeader)) {
        DEBUG ((EFI_D_ERROR, " Restore  backup partition table error\n"));
      }

      if (PartitionValidGptTable (BlockIo, DiskIo, PrimaryHeader->AlternateLBA, BackupHeader)) {
        DEBUG ((EFI_D_INFO, " Restore backup partition table success\n"));
      }
    }
  } else {
    if (!PartitionValidGptTable (BlockIo, DiskIo, PRIMARY_PART_HEADER_LBA, PrimaryHeader)) {
      PrimaryStoreValid = FALSE;
    }
    if (!PrimaryStoreValid && !PartitionValidGptTable (BlockIo, DiskIo, LastBlock, BackupHeader)) {
      DEBUG ((EFI_D_ERROR, "Both primary GPT table and backup GPT table are invalid!\n"));
      goto Done;
    }
  }

  DEBUG ((EFI_D_INFO, " Valid primary and Valid backup partition table\n"));

  //
  // Determine valid partition header before using the information in the partition table header.
  //
  ValidHeader = PrimaryStoreValid ? PrimaryHeader : BackupHeader;
  PartEntry = AllocatePool (ValidHeader->NumberOfPartitionEntries * sizeof (EFI_PARTITION_ENTRY));
  if (PartEntry == NULL) {
    DEBUG ((EFI_D_ERROR, "Allocate pool error\n"));
    goto Done;
  }
  Status = DiskIo->ReadDisk (
                     DiskIo,
                     MediaId,
                     MultU64x32(ValidHeader->PartitionEntryLBA, BlockSize),
                     ValidHeader->NumberOfPartitionEntries * sizeof (EFI_PARTITION_ENTRY),
                     PartEntry
                     );

  if (EFI_ERROR (Status)) {
    GptValidStatus = Status;
    DEBUG ((EFI_D_ERROR, " Partition Entry ReadDisk error\n"));
    goto Done;
  }

  DEBUG ((EFI_D_INFO, " Partition entries read block success\n"));
  DEBUG ((EFI_D_INFO, " Number of partition entries: %d\n", ValidHeader->NumberOfPartitionEntries * sizeof (EFI_PARTITION_ENTRY)));

  PEntryStatus = AllocateZeroPool (ValidHeader->NumberOfPartitionEntries * sizeof (EFI_PARTITION_ENTRY_STATUS));
  if (PEntryStatus == NULL) {
    DEBUG ((EFI_D_ERROR, "Allocate pool error\n"));
    goto Done;
  }

  //
  // Check the integrity of partition entries
  //
  PartitionCheckGptEntry (ValidHeader, PartEntry, PEntryStatus);

  //
  // If we got this far the GPT layout of the disk is valid and we should return true
  //
  GptValidStatus = EFI_SUCCESS;

  //
  // Create child device handles
  //
  for (Index = 0; Index < ValidHeader->NumberOfPartitionEntries; Index++) {
    if (CompareGuid (&PartEntry[Index].PartitionTypeGUID, &gEfiPartTypeUnusedGuid) ||
        PEntryStatus[Index].OutOfRange ||
        PEntryStatus[Index].Overlap ||
        PEntryStatus[Index].OsSpecific
        ) {
      //
      // Don't use null EFI Partition Entries, Invalid Partition Entries or OS specific
      // partition Entries
      //
      continue;
    }

    ZeroMem (&HdDev, sizeof (HdDev));
    HdDev.Header.Type     = MEDIA_DEVICE_PATH;
    HdDev.Header.SubType  = MEDIA_HARDDRIVE_DP;
    SetDevicePathNodeLength (&HdDev.Header, sizeof (HdDev));

    HdDev.PartitionNumber = (UINT32) Index + 1;
    HdDev.MBRType         = MBR_TYPE_EFI_PARTITION_TABLE_HEADER;
    HdDev.SignatureType   = SIGNATURE_TYPE_GUID;
    HdDev.PartitionStart  = PartEntry[Index].StartingLBA;
    HdDev.PartitionSize   = PartEntry[Index].EndingLBA - PartEntry[Index].StartingLBA + 1;
    CopyMem (HdDev.Signature, &PartEntry[Index].UniquePartitionGUID, sizeof (EFI_GUID));

    DEBUG ((EFI_D_INFO, " Index : %d\n", (UINT32) Index));
    DEBUG ((EFI_D_INFO, " Start LBA : %lx\n", (UINT64) HdDev.PartitionStart));
    DEBUG ((EFI_D_INFO, " End LBA : %lx\n", (UINT64) PartEntry[Index].EndingLBA));
    DEBUG ((EFI_D_INFO, " Partition size: %lx\n", (UINT64) HdDev.PartitionSize));
    DEBUG ((EFI_D_INFO, " Start : %lx", MultU64x32 (PartEntry[Index].StartingLBA, BlockSize)));
    DEBUG ((EFI_D_INFO, " End : %lx\n", MultU64x32 (PartEntry[Index].EndingLBA, BlockSize)));

    Status = PartitionInstallChildHandle (
               This,
               Handle,
               DiskIo,
               DiskIo2,
               BlockIo,
               BlockIo2,
               DevicePath,
               (EFI_DEVICE_PATH_PROTOCOL *) &HdDev,
               PartEntry[Index].StartingLBA,
               PartEntry[Index].EndingLBA,
               BlockSize,
               &PartEntry[Index].PartitionTypeGUID
               );
  }

  DEBUG ((EFI_D_INFO, "Prepare to Free Pool\n"));

Done:
  if (ProtectiveMbr != NULL) {
    FreePool (ProtectiveMbr);
  }
  if (PrimaryHeader != NULL) {
    FreePool (PrimaryHeader);
  }
  if (BackupHeader != NULL) {
    FreePool (BackupHeader);
  }
  if (PartEntry != NULL) {
    FreePool (PartEntry);
  }
  if (PEntryStatus != NULL) {
    FreePool (PEntryStatus);
  }

  return GptValidStatus;
}

/**
  Install child handles if the Handle supports GPT partition structure.

  @param[in]  BlockIo     Parent BlockIo interface.
  @param[in]  DiskIo      Disk Io protocol.
  @param[in]  Lba         The starting Lba of the Partition Table
  @param[out] PartHeader  Stores the partition table that is read

  @retval TRUE      The partition table is valid
  @retval FALSE     The partition table is not valid

**/
BOOLEAN
PartitionValidGptTable (
  IN  EFI_BLOCK_IO_PROTOCOL       *BlockIo,
  IN  EFI_DISK_IO_PROTOCOL        *DiskIo,
  IN  EFI_LBA                     Lba,
  OUT EFI_PARTITION_TABLE_HEADER  *PartHeader
  )
{
  EFI_STATUS                  Status;
  UINT32                      BlockSize;
  EFI_PARTITION_TABLE_HEADER  *PartHdr;
  UINT32                      MediaId;

  BlockSize = BlockIo->Media->BlockSize;
  MediaId   = BlockIo->Media->MediaId;
  PartHdr   = AllocateZeroPool (BlockSize);

  if (PartHdr == NULL) {
    DEBUG ((EFI_D_ERROR, "Allocate pool error\n"));
    return FALSE;
  }
  //
  // Read the EFI Partition Table Header
  //
  Status = DiskIo->ReadDisk (
                     DiskIo,
                     MediaId,
                     MultU64x32 (Lba, BlockSize),
                     BlockSize,
                     PartHdr
                     );
  if (EFI_ERROR (Status)) {
    FreePool (PartHdr);
    return FALSE;
  }

  if ((PartHdr->Header.Signature != EFI_PTAB_HEADER_ID) ||
      !PartitionCheckCrc (BlockSize, &PartHdr->Header) ||
      PartHdr->MyLBA != Lba
      ) {
    DEBUG ((EFI_D_ERROR, "Invalid efi partition table header\n"));
    FreePool (PartHdr);
    return FALSE;
  }

  CopyMem (PartHeader, PartHdr, sizeof (EFI_PARTITION_TABLE_HEADER));
  if (!PartitionCheckGptEntryArrayCRC (BlockIo, DiskIo, PartHeader)) {
    FreePool (PartHdr);
    return FALSE;
  }

  DEBUG ((EFI_D_INFO, " Valid efi partition table header\n"));
  FreePool (PartHdr);
  return TRUE;
}

/**
  Check if the CRC field in the Partition table header is valid
  for Partition entry array.

  @param[in]  BlockIo     Parent BlockIo interface
  @param[in]  DiskIo      Disk Io Protocol.
  @param[in]  PartHeader  Partition table header structure

  @retval TRUE      the CRC is valid
  @retval FALSE     the CRC is invalid

**/
BOOLEAN
PartitionCheckGptEntryArrayCRC (
  IN  EFI_BLOCK_IO_PROTOCOL       *BlockIo,
  IN  EFI_DISK_IO_PROTOCOL        *DiskIo,
  IN  EFI_PARTITION_TABLE_HEADER  *PartHeader
  )
{
  EFI_STATUS  Status;
  UINT8       *Ptr;
  UINT32      Crc;
  UINTN       Size;

  //
  // Read the EFI Partition Entries
  //
  Ptr = AllocatePool (PartHeader->NumberOfPartitionEntries * PartHeader->SizeOfPartitionEntry);
  if (Ptr == NULL) {
    DEBUG ((EFI_D_ERROR, " Allocate pool error\n"));
    return FALSE;
  }

  Status = DiskIo->ReadDisk (
                    DiskIo,
                    BlockIo->Media->MediaId,
                    MultU64x32(PartHeader->PartitionEntryLBA, BlockIo->Media->BlockSize),
                    PartHeader->NumberOfPartitionEntries * PartHeader->SizeOfPartitionEntry,
                    Ptr
                    );
  if (EFI_ERROR (Status)) {
    FreePool (Ptr);
    return FALSE;
  }

  Size    = (UINTN)PartHeader->NumberOfPartitionEntries * PartHeader->SizeOfPartitionEntry;

  Status  = gBS->CalculateCrc32 (Ptr, Size, &Crc);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "CheckPEntryArrayCRC: Crc calculation failed\n"));
    FreePool (Ptr);
    return FALSE;
  }

  FreePool (Ptr);

  return (BOOLEAN) (PartHeader->PartitionEntryArrayCRC32 == Crc);
}


/**
  Restore Partition Table to its alternate place
  (Primary -> Backup or Backup -> Primary).

  @param[in]  BlockIo     Parent BlockIo interface.
  @param[in]  DiskIo      Disk Io Protocol.
  @param[in]  PartHeader  Partition table header structure.

  @retval TRUE      Restoring succeeds
  @retval FALSE     Restoring failed

**/
BOOLEAN
PartitionRestoreGptTable (
  IN  EFI_BLOCK_IO_PROTOCOL       *BlockIo,
  IN  EFI_DISK_IO_PROTOCOL        *DiskIo,
  IN  EFI_PARTITION_TABLE_HEADER  *PartHeader
  )
{
  EFI_STATUS                  Status;
  UINTN                       BlockSize;
  EFI_PARTITION_TABLE_HEADER  *PartHdr;
  EFI_LBA                     PEntryLBA;
  UINT8                       *Ptr;
  UINT32                      MediaId;

  PartHdr   = NULL;
  Ptr       = NULL;

  BlockSize = BlockIo->Media->BlockSize;
  MediaId   = BlockIo->Media->MediaId;

  PartHdr   = AllocateZeroPool (BlockSize);

  if (PartHdr == NULL) {
    DEBUG ((EFI_D_ERROR, "Allocate pool error\n"));
    return FALSE;
  }

  PEntryLBA = (PartHeader->MyLBA == PRIMARY_PART_HEADER_LBA) ? \
                             (PartHeader->LastUsableLBA + 1) : \
                             (PRIMARY_PART_HEADER_LBA + 1);

  CopyMem (PartHdr, PartHeader, sizeof (EFI_PARTITION_TABLE_HEADER));

  PartHdr->MyLBA              = PartHeader->AlternateLBA;
  PartHdr->AlternateLBA       = PartHeader->MyLBA;
  PartHdr->PartitionEntryLBA  = PEntryLBA;
  PartitionSetCrc ((EFI_TABLE_HEADER *) PartHdr);

  Status = DiskIo->WriteDisk (
                     DiskIo,
                     MediaId,
                     MultU64x32 (PartHdr->MyLBA, (UINT32) BlockSize),
                     BlockSize,
                     PartHdr
                     );
  if (EFI_ERROR (Status)) {
    goto Done;
  }

  Ptr = AllocatePool (PartHeader->NumberOfPartitionEntries * PartHeader->SizeOfPartitionEntry);
  if (Ptr == NULL) {
    DEBUG ((EFI_D_ERROR, " Allocate pool error\n"));
    Status = EFI_OUT_OF_RESOURCES;
    goto Done;
  }

  Status = DiskIo->ReadDisk (
                    DiskIo,
                    MediaId,
                    MultU64x32(PartHeader->PartitionEntryLBA, (UINT32) BlockSize),
                    PartHeader->NumberOfPartitionEntries * PartHeader->SizeOfPartitionEntry,
                    Ptr
                    );
  if (EFI_ERROR (Status)) {
    goto Done;
  }

  Status = DiskIo->WriteDisk (
                    DiskIo,
                    MediaId,
                    MultU64x32(PEntryLBA, (UINT32) BlockSize),
                    PartHeader->NumberOfPartitionEntries * PartHeader->SizeOfPartitionEntry,
                    Ptr
                    );

Done:
  FreePool (PartHdr);

  if (Ptr != NULL) {
    FreePool (Ptr);
  }

  if (EFI_ERROR (Status)) {
    return FALSE;
  }

  return TRUE;
}


/**
  Restore Partition Table to its alternate place.
  (Primary -> Backup or Backup -> Primary)

  @param[in]    PartHeader    Partition table header structure
  @param[in]    PartEntry     The partition entry array
  @param[out]   PEntryStatus  the partition entry status array
                              recording the status of each partition

**/
VOID
PartitionCheckGptEntry (
  IN  EFI_PARTITION_TABLE_HEADER  *PartHeader,
  IN  EFI_PARTITION_ENTRY         *PartEntry,
  OUT EFI_PARTITION_ENTRY_STATUS  *PEntryStatus
  )
{
  EFI_LBA StartingLBA;
  EFI_LBA EndingLBA;
  UINTN   Index1;
  UINTN   Index2;
  UINT64  Attributes;

  DEBUG ((EFI_D_INFO, " start check partition entries\n"));
  for (Index1 = 0; Index1 < PartHeader->NumberOfPartitionEntries; Index1++) {
    if (CompareGuid (&PartEntry[Index1].PartitionTypeGUID, &gEfiPartTypeUnusedGuid)) {
      continue;
    }

    StartingLBA = PartEntry[Index1].StartingLBA;
    EndingLBA   = PartEntry[Index1].EndingLBA;
    if (StartingLBA > EndingLBA ||
        StartingLBA < PartHeader->FirstUsableLBA ||
        StartingLBA > PartHeader->LastUsableLBA ||
        EndingLBA < PartHeader->FirstUsableLBA ||
        EndingLBA > PartHeader->LastUsableLBA
        ) {
      PEntryStatus[Index1].OutOfRange = TRUE;
      continue;
    }

    for (Index2 = Index1 + 1; Index2 < PartHeader->NumberOfPartitionEntries; Index2++) {

      if (CompareGuid (&PartEntry[Index2].PartitionTypeGUID, &gEfiPartTypeUnusedGuid)) {
        continue;
      }

      if (PartEntry[Index2].EndingLBA >= StartingLBA && PartEntry[Index2].StartingLBA <= EndingLBA) {
        //
        // This region overlaps with the Index1'th region
        //
        PEntryStatus[Index1].Overlap  = TRUE;
        PEntryStatus[Index2].Overlap  = TRUE;
        continue;

      }
    }

    Attributes = PartEntry[Index1].Attributes;
    if ((Attributes & BIT1) != 0) {
      //
      // If Bit 1 is set, this indicate that this is an OS specific GUID partition.
      //
      PEntryStatus[Index1].OsSpecific = TRUE;
    }
  }

  DEBUG ((EFI_D_INFO, " End check partition entries\n"));
}


/**
  Updates the CRC32 value in the table header.

  @param  Hdr    Table to update

**/
VOID
PartitionSetCrc (
  IN OUT EFI_TABLE_HEADER *Hdr
  )
{
  PartitionSetCrcAltSize (Hdr->HeaderSize, Hdr);
}


/**
  Updates the CRC32 value in the table header.

  @param  Size   The size of the table
  @param  Hdr    Table to update

**/
VOID
PartitionSetCrcAltSize (
  IN UINTN                 Size,
  IN OUT EFI_TABLE_HEADER  *Hdr
  )
{
  UINT32  Crc;

  Hdr->CRC32 = 0;
  gBS->CalculateCrc32 ((UINT8 *) Hdr, Size, &Crc);
  Hdr->CRC32 = Crc;
}


/**
  Checks the CRC32 value in the table header.

  @param  MaxSize   Max Size limit
  @param  Hdr       Table to check

  @return TRUE      CRC Valid
  @return FALSE     CRC Invalid

**/
BOOLEAN
PartitionCheckCrc (
  IN UINTN                 MaxSize,
  IN OUT EFI_TABLE_HEADER  *Hdr
  )
{
  return PartitionCheckCrcAltSize (MaxSize, Hdr->HeaderSize, Hdr);
}


/**
  Checks the CRC32 value in the table header.

  @param  MaxSize   Max Size limit
  @param  Size      The size of the table
  @param  Hdr       Table to check

  @return TRUE    CRC Valid
  @return FALSE   CRC Invalid

**/
BOOLEAN
PartitionCheckCrcAltSize (
  IN UINTN                 MaxSize,
  IN UINTN                 Size,
  IN OUT EFI_TABLE_HEADER  *Hdr
  )
{
  UINT32      Crc;
  UINT32      OrgCrc;
  EFI_STATUS  Status;

  Crc = 0;

  if (Size == 0) {
    //
    // If header size is 0 CRC will pass so return FALSE here
    //
    return FALSE;
  }

  if ((MaxSize != 0) && (Size > MaxSize)) {
    DEBUG ((EFI_D_ERROR, "CheckCrc32: Size > MaxSize\n"));
    return FALSE;
  }
  //
  // clear old crc from header
  //
  OrgCrc      = Hdr->CRC32;
  Hdr->CRC32  = 0;

  Status      = gBS->CalculateCrc32 ((UINT8 *) Hdr, Size, &Crc);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "CheckCrc32: Crc calculation failed\n"));
    return FALSE;
  }
  //
  // set results
  //
  Hdr->CRC32 = Crc;

  //
  // return status
  //
  DEBUG_CODE_BEGIN ();
    if (OrgCrc != Crc) {
      DEBUG ((EFI_D_ERROR, "CheckCrc32: Crc check failed\n"));
    }
  DEBUG_CODE_END ();

  return (BOOLEAN) (OrgCrc == Crc);
}
