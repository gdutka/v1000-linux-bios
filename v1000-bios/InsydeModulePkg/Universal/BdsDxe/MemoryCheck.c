/** @file
  BDS Lib functions which relate with check memory consistency

;******************************************************************************
;* Copyright (c) 2013 - 2020, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "Bds.h"

#ifndef MDEPKG_NDEBUG
typedef struct {
  UINT32       Type;        ///< EFI memory type defined in UEFI specification.
  CHAR16       *TypeStr;    ///< memory type names for Standard-Format Output
} MEMORY_TYPE_STRING;

STATIC MEMORY_TYPE_STRING    mMemoryTypeDesc[] = {
                               {EfiReservedMemoryType       ,L"Reserved  "},
                               {EfiLoaderCode               ,L"LoaderCode"},
                               {EfiLoaderData               ,L"LoaderData"},
                               {EfiBootServicesCode         ,L"BS_code   "},
                               {EfiBootServicesData         ,L"BS_data   "},
                               {EfiRuntimeServicesCode      ,L"RT_code   "},
                               {EfiRuntimeServicesData      ,L"RT_data   "},
                               {EfiConventionalMemory       ,L"Available "},
                               {EfiUnusableMemory           ,L"Unusable  "},
                               {EfiACPIReclaimMemory        ,L"ACPI_recl "},
                               {EfiACPIMemoryNVS            ,L"ACPI_NVS  "},
                               {EfiMemoryMappedIO           ,L"MemMapIO  "},
                               {EfiMemoryMappedIOPortSpace  ,L"MemPortIO "},
                               {EfiPalCode                  ,L"PAL_code  "},
                               {EfiPersistentMemory         ,L"Persistent"},
                               //
                               // NOTE: the EfiMaxMemoryType must be put in the last of the array.
                               //
                               {EfiMaxMemoryType            ,L"Unknown   "}
                              };

/**
  Get memory type string.

  @param[in] MemoryType   EFI memory type defined in UEFI specification.

  @return  Get corresponding memory type or L"Unknown" to indicate this is a unsupported memory type.
**/
STATIC
CHAR16 *
GetMemoryTypeString (
  IN  UINTN    MemoryType
  )
{
  UINTN          Index;
  UINTN          MemoryInfoCnt;

  MemoryInfoCnt = ARRAY_SIZE (mMemoryTypeDesc);
  for (Index = 0; Index < MemoryInfoCnt - 1; Index++) {
    if (MemoryType == mMemoryTypeDesc[Index].Type) {
      return mMemoryTypeDesc[Index].TypeStr;
    }
  }
  return mMemoryTypeDesc[MemoryInfoCnt - 1].TypeStr;
}

#endif
STATIC EFI_MEMORY_TYPE      mRtMemoryType[] = {
                               EfiReservedMemoryType,
                               EfiACPIMemoryNVS,
                               EfiACPIReclaimMemory,
                               EfiRuntimeServicesData,
                               EfiRuntimeServicesCode
                               };
STATIC CONST UINTN          mRtMemoryTypeCnt = ARRAY_SIZE (mRtMemoryType);
EFI_GET_MEMORY_MAP          mBdsOrgGetMemoryMap;


/**
  This function uses to check the allocated memory size of runtime types memory is whether over than
  pre-allocated memory size of runtime memory types.

  @param[out] MemoryMapSize   A pointer to the size, in bytes, of the MemoryMap buffer.
  @param[out] DescriptorSize  A pointer to the location in which firmware returns the size, in
                              bytes, of an individual EFI_MEMORY_DESCRIPTOR.

  @return  A pointer to the current memory map is returned.
           NULL is returned if space for the memory map could not be allocated from pool.
           It is up to the caller to free the memory if they are no longer needed.
**/
STATIC
EFI_MEMORY_DESCRIPTOR *
GetMemoryMap (
  OUT UINTN                  *MemoryMapSize,
  OUT UINTN                  *DescriptorSize
  )
{
  EFI_STATUS                Status;
  EFI_MEMORY_DESCRIPTOR     *EfiMemoryMap;
  UINTN                     EfiMemoryMapSize;
  UINTN                     EfiMapKey;
  UINTN                     EfiDescriptorSize;
  UINT32                    EfiDescriptorVersion;

  EfiMemoryMapSize = 0;
  EfiMemoryMap     = NULL;
  Status = mBdsOrgGetMemoryMap (
                  &EfiMemoryMapSize,
                  EfiMemoryMap,
                  &EfiMapKey,
                  &EfiDescriptorSize,
                  &EfiDescriptorVersion
                  );
  if (Status != EFI_BUFFER_TOO_SMALL) {
    return NULL;
  }

  EfiMemoryMapSize += SIZE_1KB;
  EfiMemoryMap = AllocatePool (EfiMemoryMapSize);
  if (EfiMemoryMap == NULL) {
    return NULL;
  }

  Status = mBdsOrgGetMemoryMap (
                  &EfiMemoryMapSize,
                  EfiMemoryMap,
                  &EfiMapKey,
                  &EfiDescriptorSize,
                  &EfiDescriptorVersion
                  );
  if (Status != EFI_SUCCESS) {
    FreePool (EfiMemoryMap);
    return NULL;
  }

  *MemoryMapSize  = EfiMemoryMapSize;
  *DescriptorSize = EfiDescriptorSize;
  return EfiMemoryMap;
}

/**
 Check if it is runtime memory type

 @param[in] Type       Memory type

 @retval TRUE          It is runtime memory type
 @retval FALSE         It is not runtime memory type
**/
STATIC
BOOLEAN
IsRtMemType (
  IN EFI_MEMORY_TYPE   Type
  )
{

 UINTN                    Index;

  for (Index = 0; Index < mRtMemoryTypeCnt; Index++) {
    if (Type == mRtMemoryType[Index]) {
      return TRUE;
    }
  }
  return FALSE;
}

/**
 Add a memory range into the list of skip memory ranges.

 @param[in]      MemStart           Memory start address.
 @param[in]      MemEnd             Memory end address.
 @param[in, out] SkipMemRangeList   Pointer to list of skip memory range

 @retval EFI_SUCCESS                Add new skip memory range successfully.
 @retval EFI_INVALID_PARAMETER      Input parameter is NULL.
 @retval EFI_OUT_OF_RESOURCES       Allocate memory is fail.
**/
STATIC
EFI_STATUS
AddSkipMemRange (
  IN     EFI_PHYSICAL_ADDRESS          MemStart,
  IN     EFI_PHYSICAL_ADDRESS          MemEnd,
  IN OUT LIST_ENTRY                    *SkipMemRangeList
  )
{
  LIST_ENTRY                       *StartLink;
  LIST_ENTRY                       *CurrentLink;
  SKIP_MEMORY_RANGE                *SkipMemRange;
  SKIP_MEMORY_RANGE                *NewSkipMemRange;

  if (SkipMemRangeList == NULL) {
    return EFI_INVALID_PARAMETER;;
  }

  //
  // Check if input memory range is adjacent with stored skip memory range or not.
  // If yes, combine the input memory range with the stored skip memory range then return.
  //
  StartLink   = SkipMemRangeList;
  CurrentLink = StartLink->ForwardLink;

  while (CurrentLink != NULL && CurrentLink != StartLink) {
    SkipMemRange = SKIP_MEMORY_RANGE_FROM_THIS (CurrentLink);

    if (MemStart == SkipMemRange->MemEnd + 1) {
      SkipMemRange->MemEnd = MemEnd;
      return EFI_SUCCESS;
    } else if (MemEnd == SkipMemRange->MemStart - 1) {
      SkipMemRange->MemStart = MemStart;
      return EFI_SUCCESS;
    }

    CurrentLink = CurrentLink->ForwardLink;
  }

  //
  // Create a new skip memory range.
  //
  NewSkipMemRange = AllocatePool (sizeof (SKIP_MEMORY_RANGE));
  if (NewSkipMemRange == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  NewSkipMemRange->Signature = SKIP_MEMORY_RANGE_SIGNATURE;
  NewSkipMemRange->MemStart  = MemStart;
  NewSkipMemRange->MemEnd    = MemEnd;

  InsertTailList (SkipMemRangeList, &NewSkipMemRange->Link);

  return EFI_SUCCESS;
}

/**
 Get default page number by EFI memory type.

 @param[in]  Type            EFI memory type
 @param[out] NumberOfPages   Pointer to the default page number of EFI memory type

 @retval EFI_SUCCESS                Get default page size of memory type successfully.
 @retval EFI_INVALID_PARAMETER      Input memory type is invalid or input pointer is NULL.
 @retval EFI_NOT_FOUND              Can not find the hob of memory type information or the info of this memory type.
**/
STATIC
EFI_STATUS
GetMemDefaultPageNum (
  IN  EFI_MEMORY_TYPE   Type,
  OUT UINTN             *NumberOfPages
  )
{
  EFI_HOB_GUID_TYPE             *GuidHob;
  EFI_MEMORY_TYPE_INFORMATION   *MemTypeInfo;
  UINTN                         MemTypeInfoCnt;
  UINTN                         Index;
  UINTN                         Size;

  if (Type >= EfiMaxMemoryType || NumberOfPages == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  GuidHob =  GetFirstGuidHob (&gEfiMemoryTypeInformationGuid);
  if (GuidHob == NULL) {
    return EFI_NOT_FOUND;
  }


  MemTypeInfo    = GET_GUID_HOB_DATA (GuidHob);
  Size           = GET_GUID_HOB_DATA_SIZE (GuidHob);
  MemTypeInfoCnt = Size / sizeof (EFI_MEMORY_TYPE_INFORMATION);
  for (Index = 0; Index < MemTypeInfoCnt; Index++) {
    if ((EFI_MEMORY_TYPE) MemTypeInfo[Index].Type == Type) {
      *NumberOfPages = (UINTN) MemTypeInfo[Index].NumberOfPages;
      return EFI_SUCCESS;
    }
  }

  return EFI_NOT_FOUND;
}
/**
  Add New memory skip range to skip memory range list from MemoryTypeUpdateInfo variable.

  @param[in, out] SkipMemRangeList  Pointer to list of skip memory range

  @retval EFI_SUCCESS            Add skip memory range successfully.
  @retval EFI_INVALID_PARAMETER  Input parameter is NULL.
**/
STATIC
EFI_STATUS
AddNewSkipMemRangeFromVariable (
  IN OUT LIST_ENTRY          *SkipMemRangeList
  )
{
  UINT32                            *UpdateInfo;
  UINTN                             InfoVarSize;
  MEMORY_RANGE                      *MemoryRange;
  UINTN                             MemoryRangeCnt;
  UINTN                             Index;

  if (SkipMemRangeList == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  //
  // MemoryTypeUpdateInfo variable format:
  //   UINT32        AdjustMemoryTimes;
  //   MEMORY_RANGE  SkipMemoryRanges[];
  //
  UpdateInfo = BdsLibGetVariableAndSize (
                H2O_MEMORY_TYPE_UPDATE_INFO_VARIABLE_NAME,
                &gEfiMemoryTypeInformationGuid,
                &InfoVarSize
                );
  if (UpdateInfo == NULL) {
    return EFI_SUCCESS;
  }

  if (InfoVarSize <= sizeof (UINT32) || (InfoVarSize - sizeof (UINT32)) % sizeof (MEMORY_RANGE) != 0) {
    FreePool (UpdateInfo);
    return EFI_SUCCESS;
  }

  MemoryRangeCnt = (InfoVarSize - sizeof (UINT32)) / sizeof (MEMORY_RANGE);
  MemoryRange = (MEMORY_RANGE *) (UpdateInfo + 1);
  for (Index = 0; Index < MemoryRangeCnt; Index++) {
    AddSkipMemRange (MemoryRange[Index].MemStart, MemoryRange[Index].MemEnd, SkipMemRangeList);
  }

  FreePool (UpdateInfo);
  return EFI_SUCCESS;
}

/**
  Add New memory skip range to skip memory range list from PcdH2OSkipMemRangeList.

  @param[in, out] SkipMemRangeList  Pointer to list of skip memory range

  @retval EFI_SUCCESS            Add skip memory range successfully.
  @retval EFI_INVALID_PARAMETER  Input parameter is NULL.
**/
STATIC
EFI_STATUS
AddSkipMemRangeFromPCd (
  IN OUT LIST_ENTRY          *SkipMemRangeList
  )
{
  MEMORY_RANGE       *SkipMemRange;

  if (SkipMemRangeList == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  SkipMemRange = (MEMORY_RANGE *) PcdGetPtr (PcdH2OSkipMemRangeList);
  if (SkipMemRange == NULL) {
    return EFI_SUCCESS;
  }

  for (;SkipMemRange->MemEnd != 0; SkipMemRange++) {
    AddSkipMemRange (SkipMemRange->MemStart, SkipMemRange->MemEnd, SkipMemRangeList);
  }
  return EFI_SUCCESS;
}

/**
 Create the list of skip memory ranges.

 @param[in]      EfiMemoryMap       Pointer to current memory map
 @param[in]      EfiMemoryMapSize   The size, in bytes, of the EfiMemoryMap buffer
 @param[in]      EfiDescriptorSize  The size, in bytes, of an individual EFI_MEMORY_DESCRIPTOR
 @param[in, out] SkipMemRangeList   Pointer to list of skip memory range

 @retval EFI_SUCCESS                Create the list of skip memory range successfully.
 @retval EFI_INVALID_PARAMETER      Input pointer is NULL or the size of memory map or descriptor is zero.
**/
STATIC
EFI_STATUS
CreateSkipMemRangeList (
  IN EFI_MEMORY_DESCRIPTOR      *EfiMemoryMap,
  IN UINTN                      EfiMemoryMapSize,
  IN UINTN                      EfiDescriptorSize,
  IN OUT LIST_ENTRY             *SkipMemRangeList
  )
{
  EFI_PEI_HOB_POINTERS          Hob;
  UINT64                        Length;
  EFI_PHYSICAL_ADDRESS          iGfxBaseAddress[] = {0x20000000, 0x40000000};
  EFI_STATUS                    Status;
  UINTN                         NumberOfHandles;
  EFI_HANDLE                    *HandleBuffer;
  UINTN                         Index;
  EFI_LOADED_IMAGE_PROTOCOL     *LoadedImage;
  EFI_GUID                      FileName;
  EFI_GUID                      FileGuidUsbDebugIo  = {0xD719DBFC, 0xA4ED, 0x4cab, {0xA4, 0x78, 0xA7, 0x01, 0xB5, 0x1B, 0x95, 0xD6}};
  EFI_GUID                      FileGuidDebugEngine = {0x4803B88E, 0x9E66, 0x45dc, {0x87, 0x09, 0xfc, 0x75, 0xd3, 0x9c, 0xaf, 0x1d}};
  EFI_PHYSICAL_ADDRESS          UsbDebugIoBaseAddress;
  UINTN                         UsbDebugIoSize;
  EFI_PHYSICAL_ADDRESS          DebugEngineBaseAddress;
  UINTN                         DebugEngineSize;
  FIRMWARE_PERFORMANCE_VARIABLE PerformanceVariable;
  UINTN                         Size;
  UINTN                         MemDefaultPageNum;
  EFI_MEMORY_DESCRIPTOR         *EfiMemoryMapEnd;
  EFI_MEMORY_DESCRIPTOR         *MemDescriptor;

  if (EfiMemoryMap == NULL || EfiMemoryMapSize == 0 || EfiDescriptorSize == 0 || SkipMemRangeList == NULL) {
    return EFI_INVALID_PARAMETER;;
  }

  InitializeListHead (SkipMemRangeList);

  //
  // Add the Skip memory range below 1M Bytes for legacy BIOS.
  //
  AddSkipMemRange (
    0,
    0x100000 - 1,
    SkipMemRangeList
    );

  //
  // Skip the memory range for the memory is allocated before DXE pre-allocation operation.
  //
  Hob.Raw = GetFirstHob (EFI_HOB_TYPE_MEMORY_ALLOCATION);
  ASSERT (Hob.Raw != NULL);
  while ((Hob.Raw != NULL) && (!END_OF_HOB_LIST (Hob))) {
    if (Hob.Header->HobType == EFI_HOB_TYPE_MEMORY_ALLOCATION &&
        IsRtMemType (Hob.MemoryAllocation->AllocDescriptor.MemoryType)) {
      AddSkipMemRange (
        Hob.MemoryAllocation->AllocDescriptor.MemoryBaseAddress,
        Hob.MemoryAllocation->AllocDescriptor.MemoryBaseAddress + Hob.MemoryAllocation->AllocDescriptor.MemoryLength - 1,
        SkipMemRangeList
        );
    }
    Hob.Raw = GET_NEXT_HOB (Hob);
    Hob.Raw = GetNextHob (EFI_HOB_TYPE_MEMORY_ALLOCATION, Hob.Raw);
  }

  //
  // Add the skip memory range of iGfx memory resources, because this memory is allocated by fixed address.
  //
  Length = 0x200000;
  AddSkipMemRange (
    iGfxBaseAddress[0],
    iGfxBaseAddress[0] + Length - 1,
    SkipMemRangeList
    );
  AddSkipMemRange (
    iGfxBaseAddress[1],
    iGfxBaseAddress[1] + Length - 1,
    SkipMemRangeList
    );

  //
  // Add the skip memory range of H2O debug.
  //
  UsbDebugIoBaseAddress  = 0;
  DebugEngineBaseAddress = 0;
  NumberOfHandles = 0;
  HandleBuffer = NULL;
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiLoadedImageProtocolGuid,
                  NULL,
                  &NumberOfHandles,
                  &HandleBuffer
                  );
  if (!EFI_ERROR(Status)) {
    for (Index = 0; Index < NumberOfHandles; Index++) {
      if (UsbDebugIoBaseAddress != 0 && DebugEngineBaseAddress != 0) {
        break;
      }

      Status = gBS->HandleProtocol (
                      HandleBuffer[Index],
                      &gEfiLoadedImageProtocolGuid,
                      (VOID **) &LoadedImage
                      );
      if (EFI_ERROR (Status)) {
        continue;
      }

      FileName = ((MEDIA_FW_VOL_FILEPATH_DEVICE_PATH *) (LoadedImage->FilePath))->FvFileName;

      if (CompareGuid (&FileName, &FileGuidUsbDebugIo)) {
        UsbDebugIoBaseAddress = (EFI_PHYSICAL_ADDRESS) (UINTN) LoadedImage->ImageBase;
        UsbDebugIoSize        = (UINTN) EFI_PAGES_TO_SIZE((UINTN) EFI_SIZE_TO_PAGES((UINTN) LoadedImage->ImageSize));
        AddSkipMemRange (
          UsbDebugIoBaseAddress,
          UsbDebugIoBaseAddress + UsbDebugIoSize - 1,
          SkipMemRangeList
          );
        continue;
      }

      if (CompareGuid (&FileName, &FileGuidDebugEngine)) {
        DebugEngineBaseAddress = (EFI_PHYSICAL_ADDRESS) (UINTN) LoadedImage->ImageBase;
        DebugEngineSize        = (UINTN) EFI_PAGES_TO_SIZE((UINTN) EFI_SIZE_TO_PAGES((UINTN) LoadedImage->ImageSize));
        AddSkipMemRange (
          DebugEngineBaseAddress,
          DebugEngineBaseAddress + DebugEngineSize - 1,
          SkipMemRangeList
          );
        continue;
      }
    }

    if (NumberOfHandles != 0 && HandleBuffer != NULL) {
      FreePool (HandleBuffer);
    }
  }

  //
  // Add the skip memory range of firmware performance data table if it is not in pre-allocated region.
  //
  Size = sizeof (PerformanceVariable);
  Status = gRT->GetVariable (
                  EFI_FIRMWARE_PERFORMANCE_VARIABLE_NAME,
                  &gEfiFirmwarePerformanceGuid,
                  NULL,
                  &Size,
                  &PerformanceVariable
                  );
  if (!EFI_ERROR (Status)) {
    MemDefaultPageNum = 0;
    Status = GetMemDefaultPageNum (EfiReservedMemoryType, &MemDefaultPageNum);
    if (EFI_ERROR (Status)) {
      return EFI_ABORTED;
    }

    MemDescriptor   = EfiMemoryMap;
    EfiMemoryMapEnd = (EFI_MEMORY_DESCRIPTOR *) ((UINT8 *) EfiMemoryMap + EfiMemoryMapSize);

    while (MemDescriptor < EfiMemoryMapEnd) {
      if (MemDescriptor->PhysicalStart == PerformanceVariable.S3PerformanceTablePointer &&
          MemDescriptor->NumberOfPages < MemDefaultPageNum) {
        //
        // Memory page size is smaller than default size means that user plug more memory on system and
        // it makes FPDT allocate memory which is not in pre-allocated region.
        //
        AddSkipMemRange (
          MemDescriptor->PhysicalStart,
          MemDescriptor->PhysicalStart + (UINT64) EFI_PAGES_TO_SIZE(MemDescriptor->NumberOfPages) - 1,
          SkipMemRangeList
          );
        break;
      }

      MemDescriptor = NEXT_MEMORY_DESCRIPTOR (MemDescriptor, EfiDescriptorSize);
    }
  }

  AddSkipMemRangeFromPCd (SkipMemRangeList);
  AddNewSkipMemRangeFromVariable (SkipMemRangeList);
  return EFI_SUCCESS;
}

/**
 Destroy the list of skip memory ranges.

 @param[in, out] SkipMemRangeList   Pointer to list of skip memory range

 @retval EFI_SUCCESS                Destroy the list of skip memory range successfully.
 @retval EFI_INVALID_PARAMETER      Input parameter is NULL.
**/
STATIC
EFI_STATUS
DestroySkipMemRangeList (
  IN OUT LIST_ENTRY                *SkipMemRangeList
  )
{
  LIST_ENTRY                       *StartLink;
  LIST_ENTRY                       *CurrentLink;
  SKIP_MEMORY_RANGE                *SkipMemRange;

  if (SkipMemRangeList == NULL) {
    return EFI_INVALID_PARAMETER;;
  }

  StartLink   = SkipMemRangeList;
  CurrentLink = StartLink->ForwardLink;

  while (CurrentLink != NULL && CurrentLink != StartLink) {
    SkipMemRange = SKIP_MEMORY_RANGE_FROM_THIS (CurrentLink);
    CurrentLink = CurrentLink->ForwardLink;

    FreePool (SkipMemRange);
  }

  InitializeListHead (SkipMemRangeList);

  return EFI_SUCCESS;
}

/**
 Check if need to skip the memory descriptior based on the list of skip memory ranges.

 @param[in] SkipMemRangeList        Pointer to list of skip memory range
 @param[in] MemDescriptor           Pointer to memory descriptor

 @retval TRUE                       Need to skip memory descriptor.
 @retval FALSE                      Does not need to skip memory descriptor.
**/
STATIC
BOOLEAN
IsSkipMemDescriptor (
  IN LIST_ENTRY               *SkipMemRangeList,
  IN EFI_MEMORY_DESCRIPTOR    *MemDescriptor
  )
{
  EFI_PHYSICAL_ADDRESS             PhysicalStart;
  EFI_PHYSICAL_ADDRESS             PhysicalEnd;
  LIST_ENTRY                       *StartLink;
  LIST_ENTRY                       *CurrentLink;
  SKIP_MEMORY_RANGE                *SkipMemRange;
  EFI_GCD_MEMORY_SPACE_DESCRIPTOR  GcdDescriptor;
  EFI_STATUS                       Status;

  if (MemDescriptor == NULL || SkipMemRangeList == NULL) {
    return FALSE;
  }

  //
  // Skip the memory range of non-system memory.
  //
  if (MemDescriptor->Type == EfiReservedMemoryType &&
      MemDescriptor->Attribute == 0) {
    return TRUE;
  }

  Status = gDS->GetMemorySpaceDescriptor (MemDescriptor->PhysicalStart, &GcdDescriptor);
  if (Status == EFI_SUCCESS && GcdDescriptor.GcdMemoryType == EfiGcdMemoryTypeReserved) {
    return TRUE;
  }

  PhysicalStart = MemDescriptor->PhysicalStart;
  PhysicalEnd   = MemDescriptor->PhysicalStart + (UINT64) EFI_PAGES_TO_SIZE(MemDescriptor->NumberOfPages) - 1;

  StartLink   = SkipMemRangeList;
  CurrentLink = StartLink->ForwardLink;

  while (CurrentLink != StartLink) {
    SkipMemRange = SKIP_MEMORY_RANGE_FROM_THIS (CurrentLink);

    if (PhysicalStart >= SkipMemRange->MemStart &&
        PhysicalEnd   <= SkipMemRange->MemEnd) {
      return TRUE;
    }

    CurrentLink = CurrentLink->ForwardLink;
  }

  return FALSE;
}


/**
  Calculate suggestion page number and the policy is below.
  if UsedPageNum page number is larger than MAX_INCREASE_PAGES * 2 pages, increase MAX_INCREASE_PAGES pages.
  Otherwise using l.5 times of used page number as suggested page number.

  Note: This policy can prevent from preserving too much memory if used page number
        is very large.

  @param[in] UsedPageNum        Used page number

  @return Suggestion page number.
**/
STATIC
UINT32
CalcSuggestedValue (
  IN UINT32             UsedPageNum,
  IN BOOLEAN            RuntimeMemory
  )
{
  UINT32                SuggestionValue;

  if (RuntimeMemory && UsedPageNum > MAX_INCREASE_PAGES * 2) {
    SuggestionValue = UsedPageNum + MAX_INCREASE_PAGES;
  } else {
    SuggestionValue = UsedPageNum * 3 / 2;
  }

  return (SuggestionValue & 0xF) == 0 ? SuggestionValue : (SuggestionValue & ~0xF) + 0x10;
}

/**
  Internal function uses to get memory type information and the count of memory information.

  @param[out] MemoryInfoCnt  A pointer to the number of memory type information in returned pointer to EFI_MEMORY_TYPE_INFORMATION.

  @return    Pointer to EFI_MEMORY_TYPE_INFORMATION buffer.

**/
STATIC
EFI_MEMORY_TYPE_INFORMATION *
GetMemoryTypeInfo (
  OUT UINTN       *MemoryInfoCnt
  )
{
  EFI_HOB_GUID_TYPE               *GuidHob;
  EFI_MEMORY_TYPE_INFORMATION     *MemTypeInfo;
  UINTN                           Size;

  GuidHob = GetFirstGuidHob ((CONST EFI_GUID *) &gEfiMemoryTypeInformationGuid);
  if (GuidHob == NULL) {
    return NULL;
  }

  MemTypeInfo = GET_GUID_HOB_DATA (GuidHob);
  Size        = GET_GUID_HOB_DATA_SIZE (GuidHob);
  if (MemoryInfoCnt != NULL) {
    *MemoryInfoCnt = Size / sizeof (EFI_MEMORY_TYPE_INFORMATION);
  }

  return MemTypeInfo;

}

/**
  Determine the memory entry information is whether need add to used memory.

  @param[in] SkipMemRangeList  Pointer to list of skip memory range
  @param[in] MemDescriptor     Pointer to memory descriptor

  @retval TRUE                 Get used memory page number successfully.
  @retval FALSE                MemoryMap or UsedPageNum is NULL.
**/
STATIC
BOOLEAN
IsNeedAddToUsedMemory (
  IN LIST_ENTRY               *SkipMemRangeList,
  IN EFI_MEMORY_DESCRIPTOR    *MemDescriptor
  )
{
  if (IsSkipMemDescriptor (SkipMemRangeList, MemDescriptor)) {
    return FALSE;
  }
  return TRUE;
}


/**
  Internal function uses to get current used page number of memory by specifc memory type.
  The number of used memory DOES NOT include the memory which is located in skip memory range list.

  @param[in]      Type              The memory type.
  @param[in]      MemoryMap         A pointer to the memory map.
  @param[in]      MemoryMapSize     The size, in bytes, of the MemoryMap buffer.
  @param[in]      DescriptorSize    The size, in bytes, of an individual EFI_MEMORY_DESCRIPTOR.
  @param[in]      SkipMemRangeList  Pointer to list of skip memory range.
  @param[in, out] UsedPageNum       Pointer to used memory page number for input memory type.

  @retval EFI_SUCCESS               Get used memory page number successfully.
  @retval EFI_INVALID_PARAMETER     MemoryMap or UsedPageNum is NULL.
**/
STATIC
EFI_STATUS
GetUsedPageNum (
  IN  UINT32                   Type,
  IN  EFI_MEMORY_DESCRIPTOR    *MemoryMap,
  IN  UINTN                    MemoryMapSize,
  IN  UINTN                    DescriptorSize,
  IN  LIST_ENTRY               *SkipMemRangeList,
  OUT UINT32                   *UsedPageNum
  )
{
  EFI_MEMORY_DESCRIPTOR             *EfiMemoryMapEnd;
  EFI_MEMORY_DESCRIPTOR             *EfiEntry;


  if (MemoryMap == NULL || UsedPageNum == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  *UsedPageNum    = 0;
  EfiEntry        = MemoryMap;
  EfiMemoryMapEnd = (EFI_MEMORY_DESCRIPTOR *) ((UINT8 *) MemoryMap + MemoryMapSize);
  while (EfiEntry < EfiMemoryMapEnd) {
    if (EfiEntry->Type == Type) {
      if (IsNeedAddToUsedMemory (SkipMemRangeList, EfiEntry)) {
        *UsedPageNum +=  (UINT32) (UINTN) EfiEntry->NumberOfPages;
      }
    }
    EfiEntry = NEXT_MEMORY_DESCRIPTOR (EfiEntry, DescriptorSize);
  }

  return EFI_SUCCESS;
}

/**
  This function uses to check the allocated memory size of input type memory is whether over than
  pre-allocated memory size of this memory type.

  @param[in] Type              The memory type.
  @param[in] MemoryMap         A pointer to the memory map.
  @param[in] MemoryMapSize     The size, in bytes, of the MemoryMap buffer.
  @param[in] DescriptorSize    The size, in bytes, of an individual EFI_MEMORY_DESCRIPTOR.
  @param[in] SkipMemRangeList  Pointer to list of skip memory range.

  @retval TRUE   This memory type is over usage.
  @retval FALSE  This memory type isn't over usage.
**/
STATIC
BOOLEAN
IsMemoryOverUsage (
  IN  UINT32                   Type,
  IN  EFI_MEMORY_DESCRIPTOR    *MemoryMap,
  IN  UINTN                    MemoryMapSize,
  IN  UINTN                    DescriptorSize,
  IN  LIST_ENTRY               *SkipMemRangeList
  )
{
  EFI_MEMORY_TYPE_INFORMATION       *MemTypeInfo;
  UINTN                             MemoryInfoCnt;
  UINTN                             Index;
  UINT32                            PreAllocPageNum;
  UINT32                            UsedPageNum;

  if (MemoryMap == NULL) {
    return TRUE;
  }

  MemTypeInfo = GetMemoryTypeInfo (&MemoryInfoCnt);
  if (MemTypeInfo == NULL) {
    return TRUE;
  }

  PreAllocPageNum = 0;
  for (Index = 0; Index < MemoryInfoCnt; Index++) {
    if (MemTypeInfo[Index].Type == Type) {
      PreAllocPageNum = MemTypeInfo[Index].NumberOfPages;
      break;
    }
  }

  UsedPageNum = 0;
  GetUsedPageNum (Type, MemoryMap, MemoryMapSize, DescriptorSize, SkipMemRangeList, &UsedPageNum);

  return (BOOLEAN) (UsedPageNum > PreAllocPageNum);
}

/**
  This function uses to check the allocated memory size of runtime types memory is whether over than
  pre-allocated memory size of runtime memory types.

  @param[in] MemoryMap         A pointer to the memory map.
  @param[in] MemoryMapSize     The size, in bytes, of the MemoryMap buffer.
  @param[in] DescriptorSize    The size, in bytes, of an individual EFI_MEMORY_DESCRIPTOR.
  @param[in] SkipMemRangeList  Pointer to list of skip memory range.

  @retval TRUE   Runtime memory types is over usage.
  @retval FALSE  Runtime memory type isn't over usage.
**/
STATIC
BOOLEAN
IsRtMemoryOverUsage (
  IN  EFI_MEMORY_DESCRIPTOR    *MemoryMap,
  IN  UINTN                    MemoryMapSize,
  IN  UINTN                    DescriptorSize,
  IN  LIST_ENTRY               *SkipMemRangeList
  )
{
  UINTN                    Index;

  for (Index = 0; Index < mRtMemoryTypeCnt; Index++) {
    if (IsMemoryOverUsage ((UINT32) mRtMemoryType[Index], MemoryMap, MemoryMapSize, DescriptorSize, SkipMemRangeList)) {
      return TRUE;
    }
  }
  return FALSE;
}

/**
  This function uses to check the allocated memory size of EfiBootServicesCode is whether
  over than pre-allocated memory size of EfiBootServicesCode memory.

  @param[in] MemoryMap         A pointer to the memory map.
  @param[in] MemoryMapSize     The size, in bytes, of the MemoryMap buffer.
  @param[in] DescriptorSize    The size, in bytes, of an individual EFI_MEMORY_DESCRIPTOR.
  @param[in] SkipMemRangeList  Pointer to list of skip memory range.

  @retval TRUE   Boot services memory type is over usage.
  @retval FALSE  Boot services memory type isn't over usage.
**/
STATIC
BOOLEAN
IsBsMemoryOverUsage (
  IN  EFI_MEMORY_DESCRIPTOR    *MemoryMap,
  IN  UINTN                    MemoryMapSize,
  IN  UINTN                    DescriptorSize,
  IN  LIST_ENTRY               *SkipMemRangeList
  )
{
  if (IsMemoryOverUsage (EfiBootServicesCode, MemoryMap, MemoryMapSize, DescriptorSize, SkipMemRangeList)) {
    return TRUE;
  }
  return FALSE;
}

/**
  This function uses to check the allocated memory size of runtime and boot services memory types are whether
  over than pre-allocated memory size of runtime and boot services memory types.

  @param[in] MemoryMap         A pointer to the memory map.
  @param[in] MemoryMapSize     The size, in bytes, of the MemoryMap buffer.
  @param[in] DescriptorSize    The size, in bytes, of an individual EFI_MEMORY_DESCRIPTOR.
  @param[in] SkipMemRangeList  Pointer to list of skip memory range.

  @retval TRUE   Runtime or boot services memory types is over usage.
  @retval FALSE  Both Runtime and boot services memory types aren't over usage.
**/
STATIC
BOOLEAN
IsBsOrRtMemoryOverUsage (
  IN  EFI_MEMORY_DESCRIPTOR    *MemoryMap,
  IN  UINTN                    MemoryMapSize,
  IN  UINTN                    DescriptorSize,
  IN  LIST_ENTRY               *SkipMemRangeList
  )
{
  if (IsBsMemoryOverUsage (MemoryMap, MemoryMapSize, DescriptorSize, SkipMemRangeList) ||
      IsRtMemoryOverUsage (MemoryMap, MemoryMapSize, DescriptorSize, SkipMemRangeList)) {
    return TRUE;
  }
  return FALSE;
}


/**
  Add new skip memory ranges to update information buffer and increase the information size for
  specific memory type.

  @param[in]      Type              The memory type.
  @param[in]      SkipMemRangeList  Pointer to list of skip memory range.
  @param[in, out] UpdateInfo        On input, Pointer to original information buffer.
                                    On output, pointer to updated information buffer.
  @param[in, out] InfoVarSize       On input, Pointer to the size by bytes of original information buffer.
                                    On output, Pointer to the size by bytes of updated information buffer.
  @param[out]     PageNum           Number of pages which add to new skip memory range.

  @retval EFI_SUCCESS               Add new memory range information successfully.
  @retval EFI_INVALID_PARAMETER     UpdateInfo, *UpdateInfo, InfoVarSize, PageNum is NULL.
  @retval EFI_OUT_OF_RESOURCES      Allocate pool failed.
  @retval EFI_UNSUPPORTED           Not find any new skip range in input memory type.
**/
STATIC
EFI_STATUS
AddNewSkipMemRange (
  IN      UINT32             Type,
  IN      LIST_ENTRY         *SkipMemRangeList,
  IN OUT  UINT32             **UpdateInfo,
  IN OUT  UINTN              *InfoVarSize,
  OUT     UINT32             *PageNum
  )
{

  EFI_MEMORY_DESCRIPTOR         *EfiMemoryMap;
  UINTN                         EfiMemoryMapSize;
  UINTN                         EfiDescriptorSize;
  EFI_MEMORY_DESCRIPTOR         *EfiMemoryMapEnd;
  EFI_MEMORY_DESCRIPTOR         *EfiEntry;
  UINTN                         NumOfSkipMemRange;
  UINTN                         NumOfMemRange;
  UINT32                        *CurrentUpdateInfo;
  UINTN                         CurrentInfoVarSize;
  MEMORY_RANGE                  *MemoryRangeEntry;
  UINT64                        SkipPageNum;

  if (UpdateInfo == NULL || *UpdateInfo == NULL || InfoVarSize == NULL || PageNum == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  EfiMemoryMap = GetMemoryMap (&EfiMemoryMapSize, &EfiDescriptorSize);
  if (EfiMemoryMap == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // Calculate number of memory ranges.
  //
  NumOfMemRange = 0;
  EfiEntry      = EfiMemoryMap;
  EfiMemoryMapEnd = (EFI_MEMORY_DESCRIPTOR *) ((UINT8 *) EfiMemoryMap + EfiMemoryMapSize);
  while (EfiEntry < EfiMemoryMapEnd) {
    if (EfiEntry->Type == Type) {
      if (!IsSkipMemDescriptor (SkipMemRangeList, EfiEntry)) {
        NumOfMemRange++;
      }
    }
    EfiEntry = NEXT_MEMORY_DESCRIPTOR (EfiEntry, EfiDescriptorSize);
  }
  //
  // Cannot find pre-allocated memory and new skip memory range, just return EFI_NOT_FOUND.
  //
  if (NumOfMemRange < 1) {
    FreePool (EfiMemoryMap);
    return EFI_NOT_FOUND;
  }
  //
  // System doesn't have any skip memory range, just return EFI_SUCCESS.
  //
  NumOfSkipMemRange = NumOfMemRange - 1;
  if (NumOfSkipMemRange == 0) {
    FreePool (EfiMemoryMap);
    *PageNum = 0;
    return EFI_SUCCESS;
  }
  //
  // Copy original data to new buffer and
  //
  CurrentInfoVarSize = *InfoVarSize + NumOfSkipMemRange * sizeof (MEMORY_RANGE);
  CurrentUpdateInfo = AllocateZeroPool (CurrentInfoVarSize);
  if (CurrentUpdateInfo == NULL) {
    FreePool (EfiMemoryMap);
    return EFI_OUT_OF_RESOURCES;
  }
  CopyMem (CurrentUpdateInfo, *UpdateInfo, *InfoVarSize);

  //
  // Append new skip memory ranges.
  //
  MemoryRangeEntry = (MEMORY_RANGE *) (((UINT8 *) CurrentUpdateInfo) + *InfoVarSize);
  EfiEntry         = EfiMemoryMap;
  EfiMemoryMapEnd  = (EFI_MEMORY_DESCRIPTOR *) ((UINT8 *) EfiMemoryMap + EfiMemoryMapSize);
  SkipPageNum      = 0;
  while (EfiEntry < EfiMemoryMapEnd && NumOfSkipMemRange != 0) {
    if (EfiEntry->Type == Type) {
      if (!IsSkipMemDescriptor (SkipMemRangeList, EfiEntry)) {
        MemoryRangeEntry->MemStart = EfiEntry->PhysicalStart;
        MemoryRangeEntry->MemEnd = EfiEntry->PhysicalStart + MultU64x32(EfiEntry->NumberOfPages , EFI_PAGE_SIZE) - 1;
        DEBUG ((EFI_D_INFO | EFI_D_ERROR, "\n%s memory located at 0x%08x is fixed address and the size is 0x%08x!!!\n",   \
               GetMemoryTypeString (Type), MemoryRangeEntry->MemStart, MultU64x32(EfiEntry->NumberOfPages, EFI_PAGE_SIZE)) \
              );
        SkipPageNum += EfiEntry->NumberOfPages;
        MemoryRangeEntry++;
        NumOfSkipMemRange--;
      }
    }
    EfiEntry = NEXT_MEMORY_DESCRIPTOR (EfiEntry, EfiDescriptorSize);
  }

  FreePool (EfiMemoryMap);
  FreePool (*UpdateInfo);
  *UpdateInfo  = CurrentUpdateInfo;
  *InfoVarSize = CurrentInfoVarSize;
  *PageNum     = (UINT32) (UINTN) SkipPageNum;

  return EFI_SUCCESS;
}

/**
  Check the input memory type information is whether valid.

  @param[in] MemTypeInfo    Pointer to input EFI_MEMORY_TYPE_INFORMATION array
  @param[in] MemTypeInfoCnt The count of EFI_MEMORY_TYPE_INFORMATION instance.

  @retval TRUE              The input EFI_MEMORY_TYPE_INFORMATION is valid.
  @retval FALSE             Any of EFI_MEMORY_TYPE_INFORMATION instance in input array is invalid.
--*/
STATIC
BOOLEAN
IsMemoryTypeInfoValid (
  IN EFI_MEMORY_TYPE_INFORMATION       *MemTypeInfo,
  IN UINTN                             MemTypeInfoCnt
  )
{
  UINTN         Index;

  if (MemTypeInfo == NULL && MemTypeInfoCnt != 0) {
    return FALSE;
  }

  for (Index = 0; Index < MemTypeInfoCnt; Index++) {
    if ((MemTypeInfo[Index].NumberOfPages & 0x80000000) != 0 || MemTypeInfo[Index].Type > EfiMaxMemoryType) {
        DEBUG ((EFI_D_INFO | EFI_D_ERROR, "\n%s memory size is too large (0x%08x pages )!!!\n",   \
               GetMemoryTypeString (Index), MemTypeInfo[Index].NumberOfPages) \
              );
      return FALSE;
    }
  }

  return TRUE;
}

/**
  Check the total memory size in memory type information is whether to large.

  @param[in] MemTypeInfo     Pointer to input EFI_MEMORY_TYPE_INFORMATION array
  @param[in] MemTypeInfoCnt  The count of EFI_MEMORY_TYPE_INFORMATION instance.

  @retval TRUE               The total memory size in memory type information is too large.
  @retval FALSE              The total memory size in memory type information isn't too large.
**/
STATIC
BOOLEAN
IsTotalMemoryTooLarge (
  IN EFI_MEMORY_TYPE_INFORMATION       *MemTypeInfo,
  IN UINTN                             MemTypeInfoCnt
  )
{
  UINTN         Index;
  UINT32        TotalPages;

  TotalPages = 0;
  for (Index = 0; Index < MemTypeInfoCnt; Index++) {
    TotalPages += MemTypeInfo[Index].NumberOfPages;
  }
  if (TotalPages >= MAX_PRE_ALLOCATE_PAGES) {
    DEBUG ((EFI_D_INFO | EFI_D_ERROR, "\nTotal memory size need pre-allocate is too large(0x%08x pages )!!!\n", TotalPages));
    REPORT_STATUS_CODE (
        EFI_ERROR_CODE,
        EFI_SOFTWARE_EFI_BOOT_SERVICE | H2O_TOTAL_RESERVED_MEMORY_TOO_LARGE
        );
    return TRUE;
  }
  return FALSE;
}

/**
  For RT types memory, if the real used pages is smaller than preserved pages, memory map will report
  preserved pages. Use AllocatePage and FreePage to calculate free pages for RT type memory.

  If the input memory type is not RT type memory this function will return 0

  @param[in] Type              The memory type.
  @param[in] MemoryMap         A pointer to the memory map.
  @param[in] MemoryMapSize     The size, in bytes, of the MemoryMap buffer.
  @param[in] DescriptorSize    The size, in bytes, of an individual EFI_MEMORY_DESCRIPTOR.
  @param[in] SkipMemRangeList  Pointer to list of skip memory range.

  @return  The number of contiguous 4 KB pages to reduce or 0 if needn't reduce memory size.
**/
STATIC
UINTN
CalculateRtTypeFreePageNumber (
  IN  UINT32                   Type,
  IN  EFI_MEMORY_DESCRIPTOR    *MemoryMap,
  IN  UINTN                    MemoryMapSize,
  IN  UINTN                    DescriptorSize,
  IN  LIST_ENTRY               *SkipMemRangeList
  )
{
  EFI_MEMORY_DESCRIPTOR             *EfiMemoryMapEnd;
  EFI_MEMORY_DESCRIPTOR             *EfiEntry;
  EFI_PHYSICAL_ADDRESS              Address;
  EFI_PHYSICAL_ADDRESS              MemoryLength;
  EFI_STATUS                        Status;
  UINTN                             PageNum;
  UINTN                             LastAllocateSuccessPageNum;
  UINTN                             LastAllocateFailedPageNum;
  UINTN                             AllocatePages;

  if (MemoryMap == NULL || !IsRtMemType (Type)) {
    return 0;
  }

  AllocatePages   = 0;
  EfiEntry        = MemoryMap;
  EfiMemoryMapEnd = (EFI_MEMORY_DESCRIPTOR *) ((UINT8 *) MemoryMap + MemoryMapSize);
  while (EfiEntry < EfiMemoryMapEnd) {
    if (EfiEntry->Type != Type || IsSkipMemDescriptor (SkipMemRangeList, EfiEntry)) {
      EfiEntry = NEXT_MEMORY_DESCRIPTOR (EfiEntry, DescriptorSize);
      continue;
    }

    //
    // Using gBS->AllocatePage() to find specific type memory to check if the preserved memory size is too large.
    // If allocated memory is located in preserved region, it indicates the preserved memory is too large.
    // otherwise, the preserved memory size is not too large.
    //
    LastAllocateSuccessPageNum = 0;
    LastAllocateFailedPageNum  = 0;
    PageNum                    = 2;
    while (TRUE) {
      Status = gBS->AllocatePages (AllocateAnyPages, Type, PageNum, &Address);
      if (EFI_ERROR (Status)) {
        break;
      }
      gBS->FreePages (Address, PageNum);
      MemoryLength = (UINT64) EFI_PAGES_TO_SIZE(EfiEntry->NumberOfPages); 
      if (Address < EfiEntry->PhysicalStart || Address + PageNum > EfiEntry->PhysicalStart + MemoryLength) {
        if (LastAllocateSuccessPageNum == 0) {
          break;
        }
        if (PageNum - LastAllocateSuccessPageNum <= 1) {
          break;
        }
        LastAllocateFailedPageNum = PageNum;
        PageNum -= ((PageNum - LastAllocateSuccessPageNum) / 2);
      } else {
        if (LastAllocateFailedPageNum != 0 && LastAllocateFailedPageNum - PageNum <= 1) {
          break;
        }
        AllocatePages = PageNum;
        LastAllocateSuccessPageNum = PageNum;
        if (LastAllocateFailedPageNum == 0) {
          PageNum *= 2;
        } else {
          PageNum += ((LastAllocateFailedPageNum - PageNum) / 2);
        }
      }
    }
    if (AllocatePages != 0) {
      break;
    }
    EfiEntry = NEXT_MEMORY_DESCRIPTOR (EfiEntry, DescriptorSize);
  }

  return  AllocatePages;

}


/**
  Internal function uses to get current used page number of memory by specific memory type from memory map.
  For RT types memory, if the real used pages is smaller than preserved pages, memory map will report
  preserved pages. Use AllocatePage and FreePage to calculate free pages for RT type memory to get
  the real used pages.

  The number of used memory DOES NOT include the memory which is located in skip memory range list.

  @param[in]      Type              The memory type.
  @param[in]      MemoryMap         A pointer to the memory map.
  @param[in]      MemoryMapSize     The size, in bytes, of the MemoryMap buffer.
  @param[in]      DescriptorSize    The size, in bytes, of an individual EFI_MEMORY_DESCRIPTOR.
  @param[in]      SkipMemRangeList  Pointer to list of skip memory range.
  @param[in, out] UsedPageNum       Pointer to used memory page number for input memory type.

  @retval EFI_SUCCESS               Get used memory page number successfully.
  @retval EFI_INVALID_PARAMETER     MemoryMap or UsedPageNum is NULL.
**/
STATIC
EFI_STATUS
GetRealUsedPageNum (
  IN  UINT32                   Type,
  IN  EFI_MEMORY_DESCRIPTOR    *MemoryMap,
  IN  UINTN                    MemoryMapSize,
  IN  UINTN                    DescriptorSize,
  IN  LIST_ENTRY               *SkipMemRangeList,
  OUT UINT32                   *UsedPageNum
  )
{
  EFI_STATUS          Status;
  UINTN               FreePages;

  Status = GetUsedPageNum (
             Type,
             MemoryMap,
             MemoryMapSize,
             DescriptorSize,
             SkipMemRangeList,
             UsedPageNum
             );
  if (!EFI_ERROR (Status) && IsRtMemType (Type)) {
    FreePages = CalculateRtTypeFreePageNumber (
                  Type,
                  MemoryMap,
                  MemoryMapSize,
                  DescriptorSize,
                  SkipMemRangeList
                  );
    if (*UsedPageNum > (UINT32) FreePages) {
      *UsedPageNum -= (UINT32) FreePages;
    }
  }

  return Status;
}

/**
  Internal function to get default preserved memory page numbers from PcdPreserveMemoryTable PCD.

  @param[in] Type        The memory type.

  @return  The default preserved memory page numbers for input memory type.
**/
STATIC
UINT32
GetPcdPreserveMemoryTablePageNum (
  IN  UINT32       Type
  )
{
  EFI_MEMORY_TYPE_INFORMATION       *MemoryTypeInformation;
  UINTN                             MemoryTableSize;
  UINTN                             Index;

  MemoryTypeInformation = (EFI_MEMORY_TYPE_INFORMATION *)PcdGetPtr (PcdPreserveMemoryTable);
  MemoryTableSize =  PcdGetSize (PcdPreserveMemoryTable);

  for (Index = 0; Index < (MemoryTableSize / sizeof (EFI_MEMORY_TYPE_INFORMATION)); Index++) {
    if (Type == MemoryTypeInformation[Index].Type) {
      return MemoryTypeInformation[Index].NumberOfPages;
    }
  }
  return 0;
}

/**
  This function uses to get the pre-allocated memory size to reduce if the pre-allocated
  memory size is too large (Current definition for preserved memory too large is double of
  MIN_DECREASE_PAGES and the size is larger than PcdDefaultPageNum).

  @param[in] Type              The memory type.
  @param[in] MemoryMap         A pointer to the memory map.
  @param[in] MemoryMapSize     The size, in bytes, of the MemoryMap buffer.
  @param[in] DescriptorSize    The size, in bytes, of an individual EFI_MEMORY_DESCRIPTOR.
  @param[in] SkipMemRangeList  Pointer to list of skip memory range.

  @return  The number of contiguous 4 KiB pages to reduce or 0 if needn't reduce memory size.
**/
STATIC
UINTN
GetMemoryPagesToReduce (
  IN  UINT32                   Type,
  IN  EFI_MEMORY_DESCRIPTOR    *MemoryMap,
  IN  UINTN                    MemoryMapSize,
  IN  UINTN                    DescriptorSize,
  IN  LIST_ENTRY               *SkipMemRangeList
  )
{
  UINT32                            UsedPageNum;
  UINT32                            PcdDefaultPageNum;
  UINTN                             ReducePages;
  UINTN                             FreePages;

  if (MemoryMap == NULL || !IsRtMemType (Type)) {
    return 0;
  }

  UsedPageNum = 0;
  GetUsedPageNum (Type, MemoryMap, MemoryMapSize, DescriptorSize, SkipMemRangeList, &UsedPageNum);
  PcdDefaultPageNum = GetPcdPreserveMemoryTablePageNum (Type);
  if (UsedPageNum <= MIN_DECREASE_PAGES * 2|| UsedPageNum <= PcdDefaultPageNum) {
    return 0;
  }
  //
  // NOTE: To prevent from the algorithm to increase and decrease preserved memory size conflicts, System
  // must have at least 0x200 pages (the max increase pages number is 0x180). it can make sure system
  // will not increase preserved memory size in the next POST after reducing preserved memory size.
  //
  FreePages = CalculateRtTypeFreePageNumber (
                Type,
                MemoryMap,
                MemoryMapSize,
                DescriptorSize,
                SkipMemRangeList
                );
  //
  // Make sure the preserved memory page numbers must be equal or larger than preserved memory Page
  // numbers which defined in PcdPreserveMemoryTable.
  //
  ReducePages = FreePages > MIN_DECREASE_PAGES ? FreePages - MIN_DECREASE_PAGES : 0;
  if (PcdDefaultPageNum > UsedPageNum - ReducePages) {
    ReducePages = UsedPageNum - PcdDefaultPageNum;
  }
  return ReducePages;
}

/**
  Trigger gH2OBdsCpUpdateMemoryTypeInfoGuid checkpoint.

  @param[in, out] MemoryTypeInfo      A pointer to memory type information table.
  @param[in]      MemoryInfoCnt       The number of entries in the memory type information table.
  @param[in]      MemoryMap           A pointer to the memory map.
  @param[in]      MemoryMapSize       The size, in bytes, of the MemoryMap buffer.
  @param[in]      DescriptorSize      The size, in bytes, of an individual EFI_MEMORY_DESCRIPTOR.
  @param[in]      SkipMemRangeList    Pointer to list of skip memory range.

  @retval H2O_CP_TASK_NORMAL          Not trigger this checkpoint or this checkpoint return H2O_CP_TASK_NORMAL.
  @retval H2O_CP_TASK_UPDATE          Checkpoint return H2O_CP_TASK_UPDATE and the data in MemoryTypeInfo has been updated.
  @retval H2O_CP_TASK_SKIP            Checkpoint return H2O_CP_TASK_SKIP and kernel code should skip default behavior.
**/
UINTN
TriggerUpdateMemoryTypeInfoCp (
  IN OUT  EFI_MEMORY_TYPE_INFORMATION       *MemoryTypeInfo,
  IN      UINTN                             MemoryInfoCnt,
  IN      EFI_MEMORY_DESCRIPTOR             *MemoryMap,
  IN      UINTN                             MemoryMapSize,
  IN      UINTN                             DescriptorSize,
  IN      LIST_ENTRY                        *SkipMemRangeList
  )
{
  H2O_BDS_CP_UPDATE_MEMORY_TYPE_INFO_DATA      UpdateMemoryTypeInfoData;
  H2O_MEMORY_TYPE_INFORMATION                  *H2OMemoryTypeInfo;
  UINTN                                        Index;
  UINTN                                        OrgIndex;
  EFI_MEMORY_TYPE_INFORMATION                  *OrgMemTypeInfo;
  UINTN                                        OrgMemoryInfoCnt;


  if (MemoryTypeInfo == NULL || MemoryInfoCnt == 0 || MemoryMap == NULL) {
    return H2O_CP_TASK_NORMAL;
  }



  H2OMemoryTypeInfo = AllocateZeroPool (MemoryInfoCnt * sizeof (H2O_MEMORY_TYPE_INFORMATION));
  if (H2OMemoryTypeInfo ==  NULL) {
    return H2O_CP_TASK_NORMAL;
  }

  //
  // Set the adjusted page numbers of specific memory type.
  //
  for (Index = 0; Index < MemoryInfoCnt; Index++) {
    H2OMemoryTypeInfo[Index].Type                  = MemoryTypeInfo[Index].Type;
    H2OMemoryTypeInfo[Index].UpdatedNumberOfPages  = MemoryTypeInfo[Index].NumberOfPages;
  }
  //
  // Set the page numbers of specific memory type really used by system.
  //
  for (Index = 0; Index < MemoryInfoCnt; Index++) {
    GetRealUsedPageNum (
      H2OMemoryTypeInfo[Index].Type,
      MemoryMap,
      MemoryMapSize,
      DescriptorSize,
      SkipMemRangeList,
      &H2OMemoryTypeInfo[Index].UsedNumberOfPages
      );
  }
  //
  // Set the page numbers of specific memory type defined in original memory information table.
  //
  OrgMemTypeInfo = GetMemoryTypeInfo (&OrgMemoryInfoCnt);
  if (OrgMemTypeInfo == NULL) {
    FreePool (H2OMemoryTypeInfo);
    return H2O_CP_TASK_NORMAL;
  }
  for (OrgIndex = 0; OrgIndex < OrgMemoryInfoCnt; OrgIndex++) {
    for (Index = 0; Index < MemoryInfoCnt; Index++) {
      if (H2OMemoryTypeInfo[Index].Type == OrgMemTypeInfo[OrgIndex].Type) {
        H2OMemoryTypeInfo[Index].NumberOfPages = OrgMemTypeInfo[OrgIndex].NumberOfPages;
        break;
      }
    }
  }

  UpdateMemoryTypeInfoData.Size                = sizeof (H2O_PEI_CP_CRISIS_RECOVERY_GET_PROTECT_TABLE_DATA);
  UpdateMemoryTypeInfoData.Status              = H2O_CP_TASK_NORMAL;
  UpdateMemoryTypeInfoData.MemoryTypeInfoCount = (UINT32)MemoryInfoCnt;
  UpdateMemoryTypeInfoData.MemoryTypeInfoTable = H2OMemoryTypeInfo;
  DEBUG ((DEBUG_INFO, "Checkpoint Trigger: %g\n", &gH2OBdsCpUpdateMemoryTypeInfoGuid));
  H2OCpTrigger (&gH2OBdsCpUpdateMemoryTypeInfoGuid, &UpdateMemoryTypeInfoData);
  DEBUG ((DEBUG_INFO, "Checkpoint Result: %x\n", UpdateMemoryTypeInfoData.Status));

  if (UpdateMemoryTypeInfoData.Status == H2O_CP_TASK_UPDATE) {
    for (Index = 0; Index < MemoryInfoCnt; Index++) {
      MemoryTypeInfo[Index].NumberOfPages = H2OMemoryTypeInfo[Index].UpdatedNumberOfPages;
    }
  }

  FreePool (H2OMemoryTypeInfo);
  return UpdateMemoryTypeInfoData.Status;
}

/**
  Update MemoryTypeInformation and MemoeryTypeUpdateInfo variable.

  @param[in] MemoryMap           A pointer to the memory map.
  @param[in] MemoryMapSize       The size, in bytes, of the MemoryMap buffer.
  @param[in] DescriptorSize      The size, in bytes, of an individual EFI_MEMORY_DESCRIPTOR.
  @param[in] SkipMemRangeList    Pointer to list of skip memory range.

  @retval EFI_SUCCESS            Update memory information related variables successfully.
  @retval EFI_INVALID_PARAMETER  MemoryMap is NULL.
  @retval EFI_ABORTED            Memory information isn't changed .
  @retval EFI_UNSUPPORTED        The times of adjust memory exceeds.
  @retval EFI_OUT_OF_RESOURCES   Allocate pool failed.
  @return Other                  Set variable failed.
**/
STATIC
EFI_STATUS
SetMemoryInfoVariable (
  IN  EFI_MEMORY_DESCRIPTOR    *MemoryMap,
  IN  UINTN                    MemoryMapSize,
  IN  UINTN                    DescriptorSize,
  IN  LIST_ENTRY               *SkipMemRangeList
  )
{
  EFI_MEMORY_TYPE_INFORMATION       *MemTypeInfo;
  UINTN                             MemoryInfoCnt;
  EFI_MEMORY_TYPE_INFORMATION       *CurrentMemTypeInfo;
  UINTN                             CurrentMemoryInfoCnt;
  UINTN                             Index;
  UINTN                             RtIndex;
  UINTN                             BsIndex;
  UINT32                            *UpdateInfo;
  UINTN                             InfoVarSize;
  EFI_STATUS                        Status;
  UINT32                            SkipPageNum;
  UINTN                             RtTypeMemCnt;
  UINTN                             BsTypeMemCnt;
  UINTN                             CpStatus;
  UINT32                            FreePages;
  BOOLEAN                           EnlargeSize;
  EFI_MEMORY_TYPE_INFORMATION       RtTypeMem[] = {
                                      {EfiReservedMemoryType,  0},
                                      {EfiACPIMemoryNVS,       0},
                                      {EfiACPIReclaimMemory,   0},
                                      {EfiRuntimeServicesData, 0},
                                      {EfiRuntimeServicesCode, 0}
                                      };
  EFI_MEMORY_TYPE_INFORMATION       BsTypeMem[] = {
                                      {EfiBootServicesCode,    0}
                                      };

  if (MemoryMap == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  UpdateInfo         = NULL;
  CurrentMemTypeInfo = NULL;
  //
  // MemoeryTypeUpdateInfo variable format:
  //   UINT32        AdjustMemoryTimes;
  //   MEMORY_RANGE  SkipMemoryRanges[];
  //
  // Check the adjust memory times.
  //
  UpdateInfo = BdsLibGetVariableAndSize (
                H2O_MEMORY_TYPE_UPDATE_INFO_VARIABLE_NAME,
                &gEfiMemoryTypeInformationGuid,
                &InfoVarSize
                );
  if (UpdateInfo == NULL) {
    UpdateInfo = AllocateZeroPool (sizeof (UINT32));
    if (UpdateInfo == NULL) {
      Status = EFI_OUT_OF_RESOURCES;
      goto Done;
    }
    *UpdateInfo = 0;
    InfoVarSize = sizeof (UINT32);
  }

  EnlargeSize = FALSE;
  //
  // Calculate recommended runtime types memory size
  //
  RtTypeMemCnt = sizeof (RtTypeMem) / sizeof (EFI_MEMORY_TYPE_INFORMATION);
  for (Index = 0; Index < RtTypeMemCnt; Index++) {
    GetUsedPageNum (RtTypeMem[Index].Type, MemoryMap, MemoryMapSize, DescriptorSize, SkipMemRangeList, &RtTypeMem[Index].NumberOfPages);
    if (IsMemoryOverUsage (RtTypeMem[Index].Type, MemoryMap, MemoryMapSize, DescriptorSize, SkipMemRangeList)) {
      RtTypeMem[Index].NumberOfPages = CalcSuggestedValue (RtTypeMem[Index].NumberOfPages, TRUE);
      EnlargeSize = TRUE;
    } else if (PcdGetBool (PcdH2OReducePrservedMemorySupported)) {
      FreePages = (UINT32) GetMemoryPagesToReduce (
                             RtTypeMem[Index].Type,
                             MemoryMap,
                             MemoryMapSize,
                             DescriptorSize,
                             SkipMemRangeList
                             );
      if (RtTypeMem[Index].NumberOfPages > FreePages) {
        RtTypeMem[Index].NumberOfPages -= FreePages;
      }
    }
  }
  //
  // Calculate recommended boot services types memory size
  //
  BsTypeMemCnt = sizeof (BsTypeMem) / sizeof (EFI_MEMORY_TYPE_INFORMATION);
  for (Index = 0; Index < BsTypeMemCnt; Index++) {
    GetUsedPageNum (BsTypeMem[Index].Type, MemoryMap, MemoryMapSize, DescriptorSize, SkipMemRangeList, &BsTypeMem[Index].NumberOfPages);
    if (IsMemoryOverUsage (BsTypeMem[Index].Type, MemoryMap, MemoryMapSize, DescriptorSize, SkipMemRangeList)) {
      BsTypeMem[Index].NumberOfPages = CalcSuggestedValue (BsTypeMem[Index].NumberOfPages, FALSE);
      EnlargeSize = TRUE;
    }
  }
  if (EnlargeSize && *UpdateInfo >= MAX_ADJUSTABLE_MEMORY_TYPES) {
    DEBUG ((EFI_D_INFO | EFI_D_ERROR, "\nSystem cannot adjust runtime type memory automatically!!!\n"));
    Status = EFI_UNSUPPORTED;
    goto Done;
  }
  //
  // Three steps to update memory infomation:
  // 1. Create copied original memory information.
  // 2. Merge recommended memory size to specific memory type or update new skip memory range.
  // 3. Write updated memory information and new skip memory range to variable.
  //

  //
  // 1. Create copied original memory information.
  //
  MemTypeInfo = GetMemoryTypeInfo (&MemoryInfoCnt);
  if (MemTypeInfo == NULL) {
    Status = EFI_UNSUPPORTED;
    goto Done;
  }
  CurrentMemTypeInfo = AllocateZeroPool ((MemoryInfoCnt + RtTypeMemCnt + BsTypeMemCnt) * sizeof (EFI_MEMORY_TYPE_INFORMATION));
  if (CurrentMemTypeInfo == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto Done;
  }
  CopyMem (CurrentMemTypeInfo, MemTypeInfo, MemoryInfoCnt * sizeof (EFI_MEMORY_TYPE_INFORMATION));
  CurrentMemoryInfoCnt = MemoryInfoCnt;

  //
  // 2. Merge recommended memory size to specific memory type or update new skip memory range.
  // <1> The steps for runtime types memory:
  //   (1) First time to enlarge pre-allocated memory size if it isn't large enough.
  //   (2) Since first time enlarges pre-allocated memory size, we can check memory which is allocated by
  //       AllocateAddress at second time. And then add these skip memory ranges to variable.
  //   (3) Third time to prevent from pre-allocated memory isn't enough after the adjustment of first and second time.
  //   (4) System should not enter fourth time adjustment. If occurred, it will assert by EFI_DEBUG
  // <2> For Boot services memory type, we only need enlarge pre-allocate memory size if it isn't large enough.
  //
  *UpdateInfo += 1;

  //
  // Check Runtime types memory.
  //
  for (RtIndex = 0; RtIndex < RtTypeMemCnt; RtIndex++) {
    for (Index = 0; Index < CurrentMemoryInfoCnt; Index++) {
      if (CurrentMemTypeInfo[Index].Type == RtTypeMem[RtIndex].Type) {
        //
        // used memory page numbers and pre-allocated memory page numbers is the same, just check next memory type.
        //
        if (CurrentMemTypeInfo[Index].NumberOfPages == RtTypeMem[RtIndex].NumberOfPages) {
          break;
        }

        if (*UpdateInfo != 2 || !EnlargeSize) {
          DEBUG ((EFI_D_INFO | EFI_D_ERROR, "\nIncrease %s memory from 0x%x pages to 0x%x pages!!!\n",   \
                 GetMemoryTypeString (RtTypeMem[RtIndex].Type), CurrentMemTypeInfo[Index].NumberOfPages, \
                 RtTypeMem[RtIndex].NumberOfPages) \
                );
          CurrentMemTypeInfo[Index].NumberOfPages = RtTypeMem[RtIndex].NumberOfPages;
        } else {
          SkipPageNum = 0;
          Status      = AddNewSkipMemRange (CurrentMemTypeInfo[Index].Type, SkipMemRangeList, &UpdateInfo, &InfoVarSize, &SkipPageNum);
          if (!EFI_ERROR (Status)) {
            DEBUG ((EFI_D_INFO | EFI_D_ERROR, "\nReduce %s memory from 0x%x pages to 0x%x pages!!!\n",   \
                   GetMemoryTypeString (RtTypeMem[RtIndex].Type), CurrentMemTypeInfo[Index].NumberOfPages, \
                   CurrentMemTypeInfo[Index].NumberOfPages - CalcSuggestedValue (SkipPageNum, TRUE)) \
                  );
           if (CurrentMemTypeInfo[Index].NumberOfPages > CalcSuggestedValue (SkipPageNum, TRUE)) {
              CurrentMemTypeInfo[Index].NumberOfPages -= CalcSuggestedValue (SkipPageNum, TRUE);
            }
          } else {
            Status = EFI_OUT_OF_RESOURCES;
            goto Done;
          }
        }
        break;
      }
    }
    //
    // Add new memory type information if it doesn't exist.
    //
    if (Index == CurrentMemoryInfoCnt) {
      CopyMem (&CurrentMemTypeInfo[Index], &CurrentMemTypeInfo[Index - 1], sizeof(EFI_MEMORY_TYPE_INFORMATION));
      CurrentMemTypeInfo[Index - 1].NumberOfPages = RtTypeMem[RtIndex].NumberOfPages;
      CurrentMemTypeInfo[Index - 1].Type = RtTypeMem[RtIndex].Type;
      CurrentMemoryInfoCnt++;
    }
  }

  //
  // Check boot services type memory
  //
  for (BsIndex = 0; BsIndex < BsTypeMemCnt; BsIndex++) {
    for (Index = 0; Index < CurrentMemoryInfoCnt; Index++) {
      if (CurrentMemTypeInfo[Index].Type == BsTypeMem[BsIndex].Type) {
        //
        // used memory page numbers is smaller than or equal pre-allocated memory page numbers, just check next memory type.
        //
        if (BsTypeMem[BsIndex].NumberOfPages <= CurrentMemTypeInfo[Index].NumberOfPages) {
          break;
        }
        DEBUG ((EFI_D_INFO | EFI_D_ERROR, "\nIncrease %s memory from 0x%x pages to 0x%x pages!!!\n",   \
               GetMemoryTypeString (BsTypeMem[BsIndex].Type), CurrentMemTypeInfo[Index].NumberOfPages, \
               BsTypeMem[BsIndex].NumberOfPages) \
              );
        CurrentMemTypeInfo[Index].NumberOfPages = BsTypeMem[BsIndex].NumberOfPages;
        break;
      }
    }
    //
    // Add new memory type information if it doesn't exist.
    //
    if (Index == CurrentMemoryInfoCnt) {
      CopyMem (&CurrentMemTypeInfo[Index], &CurrentMemTypeInfo[Index - 1], sizeof(EFI_MEMORY_TYPE_INFORMATION));
      CurrentMemTypeInfo[Index - 1].NumberOfPages = BsTypeMem[BsIndex].NumberOfPages;
      CurrentMemTypeInfo[Index - 1].Type = BsTypeMem[BsIndex].Type;
      CurrentMemoryInfoCnt++;
    }
  }

  //
  // 3. Update MemoryTypeInformation variable first and then update MemoeryTypeUpdateInfo variable.
  //    Note: We MUST follow this sequence to (update MemoryTypeInformation first and then update MemoeryTypeUpdateInfo).
  //    If we don't follow this sequence, system may save incorrect MemoryTypeInformation variable data if power off system
  //    when MemoeryTypeUpdateInfo variable is set but MemoryTypeInformation variable isn't set.
  //
  if (IsMemoryTypeInfoValid (CurrentMemTypeInfo, CurrentMemoryInfoCnt)) {
    if (PcdGetBool (PcdH2OBdsCpUpdateMemoryTypeInfoSupported)) {
      CpStatus = TriggerUpdateMemoryTypeInfoCp (
                   CurrentMemTypeInfo,
                   CurrentMemoryInfoCnt,
                   MemoryMap,
                   MemoryMapSize,
                   DescriptorSize,
                   SkipMemRangeList
                   );
      if (CpStatus == H2O_CP_TASK_SKIP) {
        Status = EFI_UNSUPPORTED;
        goto Done;
      }
    }

    if (CurrentMemoryInfoCnt == MemoryInfoCnt &&
        CompareMem (CurrentMemTypeInfo, MemTypeInfo, MemoryInfoCnt * sizeof (EFI_MEMORY_TYPE_INFORMATION)) == 0) {
      Status = EFI_ABORTED;
      goto Done;
    }

    if (IsTotalMemoryTooLarge (CurrentMemTypeInfo, CurrentMemoryInfoCnt)) {
      ASSERT (FALSE);
      Status = EFI_UNSUPPORTED;
      goto Done;
    }

    Status = SetVariableToSensitiveVariable (
               EFI_MEMORY_TYPE_INFORMATION_VARIABLE_NAME,
               &gEfiMemoryTypeInformationGuid,
               EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
               CurrentMemoryInfoCnt * sizeof (EFI_MEMORY_TYPE_INFORMATION),
               CurrentMemTypeInfo
               );
    ASSERT_EFI_ERROR (Status);
    if (EFI_ERROR (Status)) {
        REPORT_STATUS_CODE (
        EFI_ERROR_CODE,
        EFI_SOFTWARE_EFI_BOOT_SERVICE | H2O_SET_MEMORY_INFO_VARIABLE_FAILED
        );
    }
    if (EnlargeSize) {
      Status = gRT->SetVariable (
                      H2O_MEMORY_TYPE_UPDATE_INFO_VARIABLE_NAME,
                      &gEfiMemoryTypeInformationGuid,
                      EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                      InfoVarSize,
                      UpdateInfo
                      );
      ASSERT_EFI_ERROR (Status);
      if (EFI_ERROR (Status)) {
          REPORT_STATUS_CODE (
          EFI_ERROR_CODE,
          EFI_SOFTWARE_EFI_BOOT_SERVICE | H2O_SET_MEMORY_INFO_VARIABLE_FAILED
          );
      }
    }
  } else {
    //
    // Code should never enter this condition. We assert the code in debug mode. But for prevent from system hang
    // in normal mode, we clear MemoryTypeInformation and MemoeryTypeUpdateInfo variable and reset system
    //
    Status = gRT->SetVariable (
                    H2O_MEMORY_TYPE_UPDATE_INFO_VARIABLE_NAME,
                    &gEfiMemoryTypeInformationGuid,
                    EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                    0,
                    NULL
                    );
    DEBUG ((EFI_D_INFO | EFI_D_ERROR, "\nClear MemoeryTypeUpdateInfo variable!!!\n"));
    Status = SetVariableToSensitiveVariable (
               EFI_MEMORY_TYPE_INFORMATION_VARIABLE_NAME,
               &gEfiMemoryTypeInformationGuid,
               EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
               0,
               NULL
               );
    DEBUG ((EFI_D_INFO | EFI_D_ERROR, "\nClear MemoryTypeInformation variable!!!\n"));
    ASSERT (FALSE);
  }

Done:
  if (UpdateInfo != NULL) {
    FreePool (UpdateInfo);
  }
  if (CurrentMemTypeInfo != NULL) {
    FreePool (CurrentMemTypeInfo);
  }
  return Status;
}

/**
  Function uses to clear adjust memory times to zero.

  @retval EFI_SUCCESS    Clear Update memory information times to zero successfully.
  @return Other          Set MemoeryTypeUpdateInfo variable failed.
**/
STATIC
EFI_STATUS
ClearUpdateMemoryInfoTimes (
  VOID
  )
/*++

Routine Description:

  Function uses to clear adjust memory times to zero.

Arguments:

Returns:

  EFI_SUCCESS  - Clear Update memory information times to zero successfully.
  Other        - Set MemoeryTypeUpdateInfo variable failed.

--*/
{
  UINT32                            *UpdateInfo;
  UINTN                             InfoVarSize;
  EFI_STATUS                        Status;

  //
  // MemoeryTypeUpdateInfo variable format:
  //   UINT32        AdjustMemoryTimes;
  //   MEMORY_RANGE  SkipMemoryRanges[];
  //
  UpdateInfo = BdsLibGetVariableAndSize (
                H2O_MEMORY_TYPE_UPDATE_INFO_VARIABLE_NAME,
                &gEfiMemoryTypeInformationGuid,
                &InfoVarSize
                );
  if (UpdateInfo == NULL) {
    return EFI_SUCCESS;
  }

  *UpdateInfo = 0;
  Status = gRT->SetVariable (
                  H2O_MEMORY_TYPE_UPDATE_INFO_VARIABLE_NAME,
                  &gEfiMemoryTypeInformationGuid,
                  EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                  InfoVarSize,
                  UpdateInfo
                  );
  FreePool (UpdateInfo);
  return Status;
}


/**
  Check the usage size of each runtime and boot services memory type.
  If the usage size exceeds the default size, adjust memory type information automatically.
  And then reset system.
**/
VOID
CheckRtAndBsMemUsage (
  VOID
  )
{
  EFI_MEMORY_DESCRIPTOR     *EfiMemoryMap;
  UINTN                     EfiMemoryMapSize;
  UINTN                     EfiDescriptorSize;
  LIST_ENTRY                SkipMemRangeList;
  EFI_STATUS                Status;
  EFI_BOOT_MODE             BootMode;
  BOOLEAN                   NeedReset;
  STATIC BOOLEAN            MemoryChecked = FALSE;

  BdsLibGetBootMode (&BootMode);
  if (BootMode == BOOT_IN_RECOVERY_MODE) {
    return;
  }

  if (MemoryChecked) {
    return;
  }

  EfiMemoryMap = GetMemoryMap (&EfiMemoryMapSize, &EfiDescriptorSize);
  if (EfiMemoryMap == NULL) {
    return;
  }

  NeedReset = FALSE;
  CreateSkipMemRangeList (EfiMemoryMap, EfiMemoryMapSize, EfiDescriptorSize, &SkipMemRangeList);
  if (IsBsOrRtMemoryOverUsage (EfiMemoryMap, EfiMemoryMapSize, EfiDescriptorSize, &SkipMemRangeList) ||
      PcdGetBool (PcdH2OReducePrservedMemorySupported)) {
    Status = SetMemoryInfoVariable (EfiMemoryMap, EfiMemoryMapSize, EfiDescriptorSize, &SkipMemRangeList);
    if (!EFI_ERROR (Status)) {
      NeedReset = TRUE;
    } else if (Status != EFI_ABORTED) {
      DEBUG ((EFI_D_INFO | EFI_D_ERROR, "\nSet memory information to variable failed!!!\n"));
    }
  }
  if (NeedReset) {
    gRT->ResetSystem (EfiResetCold, EFI_SUCCESS, 0, NULL);
  }

  ClearUpdateMemoryInfoTimes ();
  DestroySkipMemRangeList (&SkipMemRangeList);
  FreePool (EfiMemoryMap);
  MemoryChecked = TRUE;
}

#ifndef MDEPKG_NDEBUG
STATIC
BOOLEAN
IsRealMemory (
  IN  EFI_MEMORY_TYPE   Type
  )
{
  if (Type == EfiLoaderCode ||
      Type == EfiLoaderData ||
      Type == EfiBootServicesCode ||
      Type == EfiBootServicesData ||
      Type == EfiRuntimeServicesCode ||
      Type == EfiRuntimeServicesData ||
      Type == EfiConventionalMemory ||
      Type == EfiACPIReclaimMemory ||
      Type == EfiACPIMemoryNVS ||
      Type == EfiPalCode
      ) {
    return TRUE;
  }

  return FALSE;
}

/**
  Dump memory information if EFI_DEBUG is enabled.
**/
VOID
DumpMemoryMap (
  VOID
  )
{
  UINTN                             Index;
  EFI_MEMORY_DESCRIPTOR             *EfiMemoryMap;
  EFI_MEMORY_DESCRIPTOR             *EfiMemoryMapEnd;
  EFI_MEMORY_DESCRIPTOR             *EfiEntry;
  UINTN                             EfiMemoryMapSize;
  UINTN                             EfiDescriptorSize;
  UINT64                            MemoryBlockLength;
  UINT64                            NoPages[EfiMaxMemoryType];
  INT64                             TotalMemory;

  EfiMemoryMap = GetMemoryMap (&EfiMemoryMapSize, &EfiDescriptorSize);
  if (EfiMemoryMap == NULL) {
    return;
  }
  //
  // Calculate the system memory size from EFI memory map and resourceHob
  //
  EfiEntry        = EfiMemoryMap;
  EfiMemoryMapEnd = (EFI_MEMORY_DESCRIPTOR *) ((UINT8 *) EfiMemoryMap + EfiMemoryMapSize);
  SetMem (NoPages, EfiMaxMemoryType * sizeof (UINT64) , 0);
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, "Dump Memory Map: \n"));
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, "Type \t\t Start \t\t End \t\t #page \t Attributes\n"));
  while (EfiEntry < EfiMemoryMapEnd) {
    MemoryBlockLength = (UINT64) EFI_PAGES_TO_SIZE((UINTN) EfiEntry->NumberOfPages);

    DEBUG (\
      (EFI_D_INFO | EFI_D_ERROR, "%s \t %10lx \t %10lx \t %x \t %x\n",\
      GetMemoryTypeString (EfiEntry->Type),\
      EfiEntry->PhysicalStart,\
      EfiEntry->PhysicalStart + (UINTN) MemoryBlockLength - 1,\
      EfiEntry->NumberOfPages,\
      EfiEntry->Attribute)\
      );

    //
    // count pages of each type memory
    //
    NoPages[EfiEntry->Type] += EfiEntry->NumberOfPages;
    EfiEntry = NEXT_MEMORY_DESCRIPTOR (EfiEntry, EfiDescriptorSize);
  }
  DEBUG ((EFI_D_INFO | EFI_D_ERROR, "\n"));

  //
  // Print each memory type summary
  //
  for (Index = 0, TotalMemory = 0; Index < EfiMaxMemoryType; Index += 1) {
    if (NoPages[Index]) {
      DEBUG (\
        (EFI_D_INFO | EFI_D_ERROR,"  %s: %8d Pages(%ld) \n",\
        GetMemoryTypeString (Index),\
        NoPages[Index],\
        EFI_PAGES_TO_SIZE((UINTN) NoPages[Index]))\
        );
      //
      // Count total memory
      //
      if (IsRealMemory ((EFI_MEMORY_TYPE) (Index))) {
        TotalMemory += NoPages[Index];
      }
    }
  }
  DEBUG (\
    (EFI_D_INFO | EFI_D_ERROR, "Total Memory: %d MB (%ld) Bytes \n",\
    RShiftU64 (TotalMemory, 8),\
    EFI_PAGES_TO_SIZE ((UINTN) TotalMemory))\
    );
  FreePool (EfiMemoryMap);
}
#endif
