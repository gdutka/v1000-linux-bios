/** @file

;******************************************************************************
;* Copyright (c) 2015 - 2020, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/
/*
 * Copyright (c) 1999, 2000
 * Intel Corporation.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. All advertising materials mentioning features or use of this software must
 *    display the following acknowledgement:
 *
 *    This product includes software developed by Intel Corporation and its
 *    contributors.
 *
 * 4. Neither the name of Intel Corporation or its contributors may be used to
 *    endorse or promote products derived from this software without specific
 *    prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY INTEL CORPORATION AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.  IN NO EVENT SHALL INTEL CORPORATION OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
#ifndef __RAM_DISK_H__
#define __RAM_DISK_H__

#include <Uefi.h>

#include <Guid/H2ORamDisk.h>

#include <Protocol/BlockIo.h>
#include <Protocol/DevicePath.h>
#include <Protocol/LoadedImage.h>
#include <Protocol/H2ORamDiskServices.h>

#include <Library/DebugLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/BaseLib.h>
#include <Library/UefiLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DevicePathLib.h>

#define DEFAULT_DISK_SIZE  8    // in MBs

#ifndef MIN
#define MIN(x, y)  ((x) < (y) ? (x) : (y))
#endif
#ifndef MAX
#define MAX(x, y)  ((x) > (y) ? (x) : (y))
#endif

#define  MIN_DISK_SIZE  1
#define  MAX_DISK_SIZE  512

#pragma pack(1)

/**
  FAT16 boot sector definition
**/
typedef struct
{
  UINT8  BS_jmpBoot[3];
  UINT8  BS_OEMName[8];
  UINT16 BPB_BytsPerSec;
  UINT8  BPB_SecPerClus;
  UINT16 BPB_RsvdSecCnt;
  UINT8  BPB_NumFATs;
  UINT16 BPB_RootEntCnt;
  UINT16 BPB_TotSec16;
  UINT8  BPB_Media;
  UINT16 BPB_FATSz16;
  UINT16 BPB_SecPerTrk;
  UINT16 BPB_NumHeads;
  UINT32 BPB_HiddSec;
  UINT32 BPB_TotSec32;
  UINT8  BS_DrvNum;
  UINT8  BS_Reserved1;
  UINT8  BS_BootSig;
  UINT32 BS_VolID;
  UINT8  BS_VolLab[11];
  UINT8  BS_FilSysType[8];
  UINT8  BS_Code[448];
  UINT16 BS_Sig;
} BOOTSEC;

/**
  structure for total sectors to cluster size lookup
**/
typedef struct
{
  UINTN size;
  UINT8  spc;
} FAT16TABLE;

#define PBLOCK_DEVICE_SIGNATURE 'rdsk'

/**
  RAM Disk device info structure
**/
typedef struct {
  UINTN                         Signature;
  LIST_ENTRY                    Link;
  EFI_HANDLE                    Handle;
  EFI_PHYSICAL_ADDRESS          Start;
  EFI_BLOCK_IO_PROTOCOL         BlkIo;
  EFI_BLOCK_IO_MEDIA            Media;
  EFI_DEVICE_PATH_PROTOCOL      *DevicePath;
} RAM_DISK_DEV;

/**
  Macro finds the device info structure given a RAM Disk BlkIo interface
**/
#define RAM_DISK_FROM_THIS(a) CR(a, RAM_DISK_DEV, BlkIo, PBLOCK_DEVICE_SIGNATURE)

/**
  Macro finds the device info structure given a RAM Disk Link List interface
**/
#define RAM_DISK_FROM_LINK(a) CR(a, RAM_DISK_DEV, Link, PBLOCK_DEVICE_SIGNATURE)

/**
  The entry point for EDK II RamDiskDxe driver.


  @retval EFI_ALREADY_STARTED     The driver already exists in system.
  @retval EFI_OUT_OF_RESOURCES    Fail to execute entry point due to lack of
                                  resources.
  @retval EFI_SUCCES              All the related protocols are installed on
                                  the driver.

**/
EFI_STATUS
EFIAPI
InstallRamDiskProtocol (
  VOID
  );



/**
  Register a RAM disk with specified address, size and type.

  @param[in]  RamDiskBase    The base address of registered RAM disk.
  @param[in]  RamDiskSize    The size of registered RAM disk.
  @param[in]  RamDiskType    The type of registered RAM disk. The GUID can be
                             any of the values defined in section 9.3.6.9, or a
                             vendor defined GUID.
  @param[in]  ParentDevicePath
                             Pointer to the parent device path. If there is no
                             parent device path then ParentDevicePath is NULL.
  @param[out] DevicePath     On return, points to a pointer to the device path
                             of the RAM disk device.
                             If ParentDevicePath is not NULL, the returned
                             DevicePath is created by appending a RAM disk node
                             to the parent device path. If ParentDevicePath is
                             NULL, the returned DevicePath is a RAM disk device
                             path without appending. This function is
                             responsible for allocating the buffer DevicePath
                             with the boot service AllocatePool().

  @retval EFI_SUCCESS             The RAM disk is registered successfully.
  @retval EFI_INVALID_PARAMETER   DevicePath or RamDiskType is NULL.
                                  RamDiskSize is 0.
  @retval EFI_ALREADY_STARTED     A Device Path Protocol instance to be created
                                  is already present in the handle database.
  @retval EFI_OUT_OF_RESOURCES    The RAM disk register operation fails due to
                                  resource limitation.

**/
EFI_STATUS
EFIAPI
RamDiskRegister (
  IN UINT64                       RamDiskBase,
  IN UINT64                       RamDiskSize,
  IN EFI_GUID                     *RamDiskType,
  IN EFI_DEVICE_PATH              *ParentDevicePath     OPTIONAL,
  OUT EFI_DEVICE_PATH_PROTOCOL    **DevicePath
  );



/**
  Unregister a RAM disk specified by DevicePath.

  @param[in] DevicePath      A pointer to the device path that describes a RAM
                             Disk device.

  @retval EFI_SUCCESS             The RAM disk is unregistered successfully.
  @retval EFI_INVALID_PARAMETER   DevicePath is NULL.
  @retval EFI_UNSUPPORTED         The device specified by DevicePath is not a
                                  valid ramdisk device path and not supported
                                  by the driver.
  @retval EFI_NOT_FOUND           The RAM disk pointed by DevicePath doesn't
                                  exist.

**/
EFI_STATUS
EFIAPI
RamDiskUnregister (
  IN  EFI_DEVICE_PATH_PROTOCOL    *DevicePath
  );


/**
  Implementation of block I/O read.

  @param[in]   This       Indicates a pointer to the calling context.
  @param[in]   MediaId    Id of the media, changes every time the media is replaced.
  @param[in]   Lba        The starting Logical Block Address to read from
  @param[in]   BufferSize Size of Buffer, must be a multiple of device block size.
  @param[out]  Buffer     A pointer to the destination buffer for the data. The caller is
                          responsible for either having implicit or explicit ownership of the buffer.

  @retval EFI_SUCCESS           The data was read correctly from the device.
  @retval EFI_DEVICE_ERROR      The device reported an error while performing the read.
  @retval EFI_NO_MEDIA          There is no media in the device.
  @retval EFI_MEDIA_CHANGED     The MediaId does not matched the current device.
  @retval EFI_BAD_BUFFER_SIZE   The Buffer was not a multiple of the block size of the device.
  @retval EFI_INVALID_PARAMETER The read request contains LBAs that are not valid,
                                or the buffer is not on proper alignment.
**/
EFI_STATUS
RamDiskReadBlocks (
  IN EFI_BLOCK_IO_PROTOCOL *This,
  IN UINT32                MediaId,
  IN EFI_LBA               LBA,
  IN UINTN                 BufferSize,
  OUT VOID                 *Buffer
  );

/**
  Implementation of block I/O write

  @param[in]   This       Indicates a pointer to the calling context.
  @param[in]   MediaId    The media ID that the write request is for.
  @param[in]   Lba        The starting logical block address to be written. The caller is
                          responsible for writing to only legitimate locations.
  @param[in]   BufferSize Size of Buffer, must be a multiple of device block size.
  @param[in]   Buffer     A pointer to the source buffer for the data.

  @retval EFI_SUCCESS           The data was written correctly to the device.
  @retval EFI_WRITE_PROTECTED   The device can not be written to.
  @retval EFI_DEVICE_ERROR      The device reported an error while performing the write.
  @retval EFI_NO_MEDIA          There is no media in the device.
  @retval EFI_MEDIA_CHNAGED     The MediaId does not matched the current device.
  @retval EFI_BAD_BUFFER_SIZE   The Buffer was not a multiple of the block size of the device.
  @retval EFI_INVALID_PARAMETER The write request contains LBAs that are not valid,
                                or the buffer is not on proper alignment.
**/
EFI_STATUS
RamDiskWriteBlocks (
  IN EFI_BLOCK_IO_PROTOCOL *This,
  IN UINT32                MediaId,
  IN EFI_LBA               LBA,
  IN UINTN                 BufferSize,
  IN VOID                  *Buffer
  );

/**
  Implementation of block I/O flush

  @param[in] This         Indicates the EFI_BLOCK_IO instance.

**/
EFI_STATUS
RamDiskFlushBlocks (
  IN EFI_BLOCK_IO_PROTOCOL *This
  );

#pragma pack()

#endif    //__RAM_DISK_H__
