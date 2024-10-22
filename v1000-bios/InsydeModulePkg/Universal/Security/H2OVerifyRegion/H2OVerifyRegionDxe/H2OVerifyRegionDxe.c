/** @file
  This module is used to verify FVs in DXe phase.

;******************************************************************************
;* Copyright (c) 2018 - 2020, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <Uefi.h>

#include <Protocol/FirmwareVolumeBlock.h>

#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseLib.h>
#include <Library/UefiLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PcdLib.h>
#include <Library/FlashRegionLib.h>
#include <Library/PostCodeLib.h>
#include <Library/HobLib.h>


typedef struct _VERIFIED_FV_LIST {
  LIST_ENTRY               Link;
  EFI_PHYSICAL_ADDRESS     FvBase;
  UINT64                   FvLength;
} VERIFIED_FV_LIST;

LIST_ENTRY              mVerifiedFvList;


/**
  Internal function to add new FV to verified list.

  @param[in]  FvBase       Base address of FV image.
  @param[in]  FvLength     Length of FV image.

  @return TRUE     A pointer to new added FV entry or null if lack of memory.
**/
STATIC
VERIFIED_FV_LIST *
AddFvToVerifiedList (
  IN  EFI_PHYSICAL_ADDRESS     FvBase,
  IN  UINT64                   FvLength
  )
{
  VERIFIED_FV_LIST         *FvEntry;

  FvEntry = AllocatePool (sizeof (VERIFIED_FV_LIST));
  if (FvEntry == NULL) {
    return NULL;
  }
  FvEntry->FvBase   = FvBase;
  FvEntry->FvLength = FvLength;
  InsertTailList (&mVerifiedFvList, &FvEntry->Link);

  return FvEntry;
}

/**
  Internal function to check if the FV is already located in verified list.

  @param[in]  FvBase              Base address of FV image.
  @param[in]  FvLength            Length of FV image.

  @retval TRUE     Can find FV in verified list.
  @retval FALSE    Can not find FV in verified list.
**/
STATIC
BOOLEAN
IsFvInVerifiedList (
  IN  EFI_PHYSICAL_ADDRESS     FvBase,
  IN  UINT64                   FvLength
  )
{
  LIST_ENTRY               *Link;
  VERIFIED_FV_LIST         *FvEntry;

  for (Link = GetFirstNode (&mVerifiedFvList);
       !IsNull (&mVerifiedFvList, Link);
       Link  = GetNextNode (&mVerifiedFvList, Link)) {
    FvEntry = (VERIFIED_FV_LIST *) Link;
    if (FvEntry->FvBase == FvBase && FvEntry->FvLength == FvLength) {
      return TRUE;
    }
  }

  return FALSE;
}

/**
  Internal function to initialize the FV list. the FVs in the list
  are verified in PEI phase.

  @retval EFI_SUCCESS           Initialize verified FV list successfully.
  @retval EFI_OUT_OF_RESOURCES  Lack of memory resource.
**/
STATIC
EFI_STATUS
InitializeVerifiedFvList (
  VOID
  )
{
  EFI_HOB_FIRMWARE_VOLUME  *FvHob;

  InitializeListHead (&mVerifiedFvList);

  for (FvHob = (EFI_HOB_FIRMWARE_VOLUME *)GetFirstHob (EFI_HOB_TYPE_FV);
       FvHob != NULL;
       FvHob = (EFI_HOB_FIRMWARE_VOLUME *)GetNextHob (EFI_HOB_TYPE_FV, (VOID *)((UINTN)FvHob + FvHob->Header.HobLength))) {
    AddFvToVerifiedList (FvHob->BaseAddress, FvHob->Length);
  }
  return EFI_SUCCESS;
}

/**
  Verify FV image according to hash value in FDM.

  @param[in]  FvBase              Base address of FV image.
  @param[in]  FvLength            Length of FV image.

  @retval EFI_SUCCESS             FV image is verified successfully.
  @retval EFI_SECURITY_VIOLATION  FV image is untrusted.
  @retval EFI_NOT_FOUND           Cannot find flash base address.

**/
EFI_STATUS
EFIAPI
VerifyFvImage (
  IN EFI_PHYSICAL_ADDRESS           FvBase,
  IN UINT64                         FvLength
  )
{
  UINT8                    Count;
  EFI_STATUS               Status;
  UINT8                    Index;
  EFI_PHYSICAL_ADDRESS     FlashBase;
  EFI_GUID                 RegionType;
  UINT8                    RegionId[FDM_ENTRY_REGION_ID_SIZE];
  UINT64                   RegionOffset;
  UINT64                   RegionSize;
  UINT32                   Attribs;


  Status = FdmGetCount (&Count);
  if (EFI_ERROR (Status)) {
    return EFI_SUCCESS;
  }
  FlashBase = FdmGetBaseAddr ();
  if (FlashBase == 0) {
    return EFI_NOT_FOUND;
  }
  for (Index = 1; Index <= Count; Index++) {
    Status = FdmGetAt (
               Index,
               &RegionType,
               RegionId,
               &RegionOffset,
               &RegionSize,
               &Attribs
               );
    if (EFI_ERROR (Status)){
      continue;
    }
    if (FvBase == FlashBase + RegionOffset && FvLength == RegionSize) {
      return FdmVerifyAt (Index);
    }
  }
  return EFI_SUCCESS;
}

/**
  This notification function is invoked when an instance of the
  EFI_FIRMWARE_VOLUME_BLOCK_PROTOCOL is produced.

  @param[in]  Event                 The event that occurred
  @param[in]  Context               Pointer to the notification function's context.
**/
STATIC
VOID
EFIAPI
NotifyFwVolBlock (
  IN  EFI_EVENT Event,
  IN  VOID      *Context
  )
{
  EFI_STATUS                              Status;
  UINTN                                   NumberHandles;
  EFI_HANDLE                              *Buffer;
  UINTN                                   Index;
  EFI_FIRMWARE_VOLUME_BLOCK_PROTOCOL      *FvBlock;
  EFI_PHYSICAL_ADDRESS                    FvBase;
  UINT64                                  FvLength;
  UINTN                                   BlockSize;
  UINTN                                   NumberOfBlocks;

  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiFirmwareVolumeBlockProtocolGuid,
                  NULL,
                  &NumberHandles,
                  &Buffer
                  );
  if (EFI_ERROR (Status)) {
    return;
  }

  for (Index = 0; Index < NumberHandles; Index++) {
    Status = gBS->HandleProtocol (
                    Buffer[Index],
                    &gEfiFirmwareVolumeBlockProtocolGuid,
                    (VOID **) &FvBlock
                    );
    if (EFI_ERROR (Status)) {
      continue;
    }
    Status = FvBlock->GetPhysicalAddress (FvBlock, &FvBase);
    if (EFI_ERROR (Status)) {
      continue;
    }
    Status = FvBlock->GetBlockSize (FvBlock, 0, &BlockSize, &NumberOfBlocks);
    if (EFI_ERROR (Status)) {
      continue;
    }
    FvLength = BlockSize * NumberOfBlocks;
    if (IsFvInVerifiedList (FvBase, FvLength)) {
      continue;
    }
    VerifyFvImage (FvBase, FvLength);
    AddFvToVerifiedList (FvBase, FvLength);
  }

  FreePool (Buffer);

}

/**
  The HSTI driver's entry point.

  Create IBV gAdapterInfoPlatformSecurityGuid protocol

  @param[in] ImageHandle        The firmware allocated handle for the EFI image.
  @param[in] SystemTable        A pointer to the EFI System Table.

  @retval EFI_SUCCESS           The entry point is executed successfully.
**/
EFI_STATUS
EFIAPI
VerifyRegionDxeEntryPoint (
  IN  EFI_HANDLE           ImageHandle,
  IN  EFI_SYSTEM_TABLE     *SystemTable
  )
{
  VOID              *Registration;
  EFI_STATUS        Status;

  if (GetBootModeHob () == BOOT_IN_RECOVERY_MODE) {
    return EFI_SUCCESS;
  }
  Status = InitializeVerifiedFvList ();
  ASSERT_EFI_ERROR (Status);
  EfiCreateProtocolNotifyEvent (
    &gEfiFirmwareVolumeBlockProtocolGuid,
    TPL_CALLBACK,
    NotifyFwVolBlock,
    NULL,
    &Registration
    );
  return EFI_SUCCESS;
}
