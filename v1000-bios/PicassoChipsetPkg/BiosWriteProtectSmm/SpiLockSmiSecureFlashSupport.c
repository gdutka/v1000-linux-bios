/** @file
  This driver provides Spi Lock in SMM mode

;*******************************************************************************
;* Copyright (c) 2015, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;*******************************************************************************
*/

#include <PiDxe.h>
#include <Uefi.h>
#include <SecureFlash.h>

#include <Guid/HobList.h>

#include <Library/BaseMemoryLib.h>
#include <Library/HobLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/VariableLib.h>
/**
  Return the EFI 1.0 System Tabl entry with TableGuid

  @param[in]       TableGuid   Name of entry to return in the system table
  @param[in, out]  Table       Pointer in EFI system table associated with TableGuid

  @return EFI_SUCCESS          Table returned;
  @return EFI_NOT_FOUND        TableGuid not in EFI system table

**/
EFI_STATUS
EfiLibGetSystemConfigurationTable (
  IN EFI_GUID *TableGuid,
  IN OUT VOID **Table
  )
{
  UINTN Index;

  for (Index = 0; Index < gST->NumberOfTableEntries; Index++) {
    if (CompareGuid (TableGuid, &(gST->ConfigurationTable[Index].VendorGuid))) {
      *Table = gST->ConfigurationTable[Index].VendorTable;
      return EFI_SUCCESS;
    }
  }

  return EFI_NOT_FOUND;
}

/**
 Find the next Capsule volume HOB

 @param [in, out] HobStart      start of HOBs
 @param [out]  BaseAddress      returned base address of capsule volume
 @param [out]  Length           length of capsule volume pointed to by BaseAddress

 @retval EFI_SUCCESS            one found
 @retval EFI_NOT_FOUND          did not find one

**/
EFI_STATUS
GetNextCapsuleVolumeHob (
  IN OUT VOID                   **HobStart,
  OUT EFI_PHYSICAL_ADDRESS      *BaseAddress,
  OUT UINT64                    *Length
  )
{
  EFI_PEI_HOB_POINTERS          Hob;

  Hob.Raw = *HobStart;
  if (END_OF_HOB_LIST (Hob)) {
    return EFI_NOT_FOUND;
  }

  Hob.Raw = GetNextHob (EFI_HOB_TYPE_UEFI_CAPSULE, *HobStart);
  if (Hob.Header->HobType != EFI_HOB_TYPE_UEFI_CAPSULE) {
    return EFI_NOT_FOUND;
  }

  *BaseAddress = Hob.Capsule->BaseAddress;
  *Length      = Hob.Capsule->Length;
  *HobStart    = GET_NEXT_HOB (Hob);

  return EFI_SUCCESS;
}


EFI_STATUS
RelocateSecureFlashImageAddress (
  VOID
  )
{
  EFI_STATUS                  Status;
  EFI_HOB_HANDOFF_INFO_TABLE  *HobList;
  EFI_PHYSICAL_ADDRESS        BaseAddress;
  UINT64                      BaseSize;
  IMAGE_INFO                  ImageInfo;
  UINTN                       Size;

  Size = sizeof (IMAGE_INFO);
  Status = gRT->GetVariable (
                  L"SecureFlashInfo",
                  &gSecureFlashInfoGuid,
                  NULL,
                  &Size,
                  &ImageInfo
                  );
  
  if ((Status == EFI_SUCCESS) && (ImageInfo.FlashMode)) {
    Status = EfiLibGetSystemConfigurationTable (&gEfiHobListGuid, (VOID**)&HobList);
    if (EFI_ERROR (Status)) {
      return Status;
    }

    Status = GetNextCapsuleVolumeHob ((VOID**)&HobList, &BaseAddress, &BaseSize);
    if (EFI_ERROR (Status)) {
      return Status;
    }

    ImageInfo.ImageAddress = BaseAddress;
    ImageInfo.ImageSize = (UINT32)BaseSize;
  
    Status = SetVariableToSensitiveVariable (
                    L"SecureFlashInfo",
                    &gSecureFlashInfoGuid,
                    EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                    sizeof (IMAGE_INFO),
                    &ImageInfo
                    );
  }

  return Status;
}
