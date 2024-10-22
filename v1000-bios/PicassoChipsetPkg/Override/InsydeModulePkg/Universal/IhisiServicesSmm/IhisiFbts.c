/** @file
  FbtsLib Library Instance implementation

;******************************************************************************
;* Copyright (c) 2023, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/
#include "IhisiFbts.h"
#include "IhisiRegistration.h"
#include <EfiFlashMap.h>
#include <Protocol/Pcd.h>
#include <Library/HobLib.h>
#include <Library/BvdtLib.h>
#include <Library/FdSupportLib.h>
#include <Library/SmmOemSvcKernelLib.h>
#include <Library/FlashRegionLib.h>
#include <Library/CacheMaintenanceLib.h>
#include <Library/VariableSupportLib.h>
#include <Library/LockBoxLib.h>
#include <Guid/FlashMapHob.h>
#include <Guid/AuthenticatedVariableFormat.h>

#define                                     DEFAULT_VARIABLE_NAME_SIZE    0x50
STATIC EFI_GUID                             mDefaultPreservedVendorGuid = { 0x77fa9abd, 0x0359, 0x4d32, {0xbd, 0x60, 0x28, 0xf4, 0xe7, 0x8f, 0x78, 0x4b} };
STATIC EFI_GUID                             mPreservedVariableTable2OtherAllGuid = {0xffffffff, 0xffff, 0xffff, {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff}};

STATIC FBTS_PLATFORM_ROM_MAP_BUFFER         mRomMapBuffer;
STATIC FBTS_PLATFORM_PRIVATE_ROM_BUFFER     mPrivateRomMapBuffer;
extern BOOLEAN                              mInPOST;
extern EFI_GUID                             gInsydeTokenSpaceGuid;
//
// FBTS Support Functions
//
STATIC
IHISI_REGISTER_TABLE
FBTS_REGISTER_TABLE[] = {
  //
  // AH=10h
  //
  { FBTSGetSupportVersion, "S10Cs_GetPermission", ChipsetFbtsGetPermission      }, \
  { FBTSGetSupportVersion, "S10OemGetPermission", OemFbtsGetPermission          }, \
  { FBTSGetSupportVersion, "S10OemGetAcStatus00", OemFbtsGetAcStatus            }, \
  { FBTSGetSupportVersion, "S10OemBatterylife00", OemFbtsGetBatteryLife         }, \
  { FBTSGetSupportVersion, "S10Kn_GetVersion000", KernelFbtsGetVersion          }, \
  { FBTSGetSupportVersion, "S10Kn_InitOemHelp00", KernelFbtsInitOemHelp         }, \
  { FBTSGetSupportVersion, "S10Kn_GetVendorID00", KernelFbtsGetVendorId         }, \
  { FBTSGetSupportVersion, "S10Kn_GetBatteryLow", KernelFbtsGetBatteryLoawBound }, \

  //
  // AH=11h
  //
  { FBTSGetPlatformInfo, "S11Kn_GetModelName0", KernelFbtsGetModelName        }, \
  { FBTSGetPlatformInfo, "S11Kn_GModelVersion", KernelFbtsGetModelVersion     }, \
  { FBTSGetPlatformInfo, "S11OemFbtsApCheck00", OemFbtsApCheck                }, \
  { FBTSGetPlatformInfo, "S11Kn_UpExtPlatform", KernelFbtsUpdateExtendPlatform}, \

  //
  // AH=12h
  //
  { FBTSGetPlatformRomMap, "S12Kn_ProtectRomMap", KernelFbtsUpdateProtectRomMap }, \
  { FBTSGetPlatformRomMap, "S12Kn_PrivateRomMap", KernelFbtsUpadtePrivateRomMap }, \
  { FBTSGetPlatformRomMap, "S12Cs_PlatformRomMp", ChipsetFbtsGetPlatformRomMap  }, \
  { FBTSGetPlatformRomMap, "S12OemPlatformRomMp", OemGetPlatformRomMap          }, \

  //
  // AH=13h
  //
  { FBTSGetFlashPartInfo, "S13Kn_FlashPartInfo", KernelFbtsGetFlashPartInfo }, \

  //
  // AH=14h
  //
  { FBTSRead, "S14Cs_DoBeforeRead0", ChipsetFbtsDoBeforeReadProcess }, \
  { FBTSRead, "S14Kn_FbtsReadProce", KernelFbtsRead                 }, \
  { FBTSRead, "S14Cs_DoAfterRead00", ChipsetFbtsDoAfterReadProcess  }, \

  //
  // AH=15h
  //
  { FBTSWrite, "S15Cs_DoBeforeWrite", ChipsetFbtsDoBeforeWriteProcess }, \
  { FBTSWrite, "S15Kn_FbtsWriteProc", KernelFbtsWrite                 }, \
  { FBTSWrite, "S15Cs_DoAfterWrite0", ChipsetFbtsDoAfterWriteProcess  }, \

  //
  // AH=16h
  //
  { FBTSComplete, "S16Cs_CApTerminalte", ChipsetFbtsCompleteApTerminated}, \
  { FBTSComplete, "S16Cs_CNormalFlash0", ChipsetFbtsCompleteNormalFlash }, \
  { FBTSComplete, "S16Cs_CPartialFlash", ChipsetFbtsCompletePartialFlash}, \
  { FBTSComplete, "S16Kn_PurifyVariabl", KernelFbtsPurifyVariable       }, \
  { FBTSComplete, "S16Cs_FbtsComplete0", ChipsetFbtsComplete            }, \
  { FBTSComplete, "S16Cs_FbtsReboot000", ChipsetFbtsReboot              }, \
  { FBTSComplete, "S16Cs_FbtsShutDown0", ChipsetFbtsShutDown            }, \
  { FBTSComplete, "S16Cs_FbtsDoNothing", ChipsetFbtsDoNothing           }, \

  //
  // AH=1Bh
  //
  { FBTSSkipMcCheckAndBinaryTrans, "S1BKn_SkipMcCheck00", KernelSkipMcCheckAndBinaryTrans }, \

  //
  // AH=1Eh
  //
  { FBTSGetWholeBiosRomMap, "S1EKn_WholeBiosRomp", KernelFbtsGetWholeBiosRomMap }, \
  { FBTSGetWholeBiosRomMap, "S1EOemWholeBiosRomp", OemFbtsGetWholeBiosRomMap    }, \

  //
  // AH=1Fh
  //
  { FBTSApHookPoint, "S1FKn_ApHookforBios", OemFbtsApHookForBios}
};

STATIC
FLASH_MAP_MAPPING_TABLE mFlashMapTypeMappingTable[] = {
  {FbtsRomMapPei,           EFI_FLASH_AREA_RECOVERY_BIOS},
  {FbtsRomMapCpuMicrocode,  EFI_FLASH_AREA_CPU_MICROCODE},
  {FbtsRomMapNVRam,         EFI_FLASH_AREA_EFI_VARIABLES},
  {FbtsRomMapDxe,           EFI_FLASH_AREA_MAIN_BIOS},
  {FbtsRomMapEc,            EFI_FLASH_AREA_FV_EC},
  {FbtsRomMapNvStorage,     EFI_FLASH_AREA_GUID_DEFINED},
  {FbtsRomMapFtwBackup,     EFI_FLASH_AREA_FTW_BACKUP},
  {FbtsRomMapFtwState,      EFI_FLASH_AREA_FTW_STATE},
  {FbtsRomMapSmbiosLog,     EFI_FLASH_AREA_SMBIOS_LOG},
  {FbtsRomMapOemData,       EFI_FLASH_AREA_OEM_BINARY},
  {FbtsRomMapGpnv,          EFI_FLASH_AREA_GPNV},
  {FbtsRomMapDmiFru,        EFI_FLASH_AREA_DMI_FRU},
  {FbtsRomMapPalB,          EFI_FLASH_AREA_PAL_B},
  {FbtsRomMapMcaLog,        EFI_FLASH_AREA_MCA_LOG},
  {FbtsRomMapPassword,      EFI_FLASH_AREA_RESERVED_03},
  {FbtsRomMapOemNvs,        EFI_FLASH_AREA_RESERVED_04},
  {FbtsRomMapReserved07,    EFI_FLASH_AREA_RESERVED_07},
  {FbtsRomMapReserved08,    EFI_FLASH_AREA_RESERVED_08},
  {FbtsRomMapReserved0A,    EFI_FLASH_AREA_RESERVED_0A},
  {FbtsRomMapUnused,        EFI_FLASH_AREA_UNUSED},
  {FbtsRomMapUndefined,     EFI_FLASH_AREA_GUID_DEFINED},
  {FbtsRomMapFactoryCopy,   EFI_FLASH_AREA_RESERVED_09}, // factory default
  {FbtsRomMapEos,           FbtsRomMapEos}               //End of table
};
EFI_STATUS
UpdateOemFlashMap (
  VOID
);
/**
 Fill platform protect ROM map information to module ROM map buffer.

 @retval            EFI_SUCCESS         Set platform protect ROM map information to module ROM map buffer successful.
 @retval            EFI_UNSUPPORTED     Module ROM map buffer is full.
*/
EFI_STATUS
FillPlatformRomMapBuffer (
  IN UINT8      Type,
  IN UINT32     Address,
  IN UINT32     Length,
  IN UINT8      Entry
  )
{
  EFI_STATUS    Status;
  UINTN         ConvertedAddress;

  if (Entry >= (sizeof (FBTS_PLATFORM_ROM_MAP_BUFFER) / sizeof (FBTS_PLATFORM_ROM_MAP))) {
   return EFI_UNSUPPORTED;
  }

  if (Type != FbtsRomMapEos) {
    Status = mSmmFwBlockService->ConvertToSpiAddress(
                                   mSmmFwBlockService,
                                   (UINTN) Address,
                                   &ConvertedAddress
                                   );
    if (!EFI_ERROR (Status)) {
      Address = (UINT32) ConvertedAddress;
    }
  }

  mRomMapBuffer.PlatFormRomMap[Entry].Type    = Type;
  mRomMapBuffer.PlatFormRomMap[Entry].Address = Address;
  mRomMapBuffer.PlatFormRomMap[Entry].Length  = Length;

  return EFI_SUCCESS;
}

/**
 Fill platform private ROM map information to module ROM map buffer.

 @retval            EFI_SUCCESS         Set platform ROM map information to module ROM map buffer successful.
 @retval            EFI_UNSUPPORTED     Module ROM map buffer is full.
*/
EFI_STATUS
FillPlatformPrivateRomMapBuffer (
  IN UINT32   Address,
  IN UINT32   Length,
  IN UINT8    Entry
  )
{
  EFI_STATUS    Status;
  UINTN         ConvertedAddress;

  if (Entry >= (sizeof (FBTS_PLATFORM_PRIVATE_ROM_BUFFER) / sizeof (FBTS_PLATFORM_PRIVATE_ROM))) {
    return EFI_UNSUPPORTED;
  }

  if (Address != FbtsRomMapEos) {
    Status = mSmmFwBlockService->ConvertToSpiAddress(
                                   mSmmFwBlockService,
                                   (UINTN) Address,
                                   &ConvertedAddress
                                   );
    if (!EFI_ERROR (Status)) {
      Address = (UINT32) ConvertedAddress;
    }
  }
  mPrivateRomMapBuffer.PlatFormRomMap[Entry].LinearAddress = Address;
  mPrivateRomMapBuffer.PlatFormRomMap[Entry].Size = Length;

  return EFI_SUCCESS;
}

/**
  Get IHISI status.translated from EFI status

  @param[in] Status  EFI_STATUS

  @return UINT32     IHISI status
**/
UINT32
GetFbtsIhisiStatus (
  EFI_STATUS                             Status
  )
{
  switch (Status) {
    case EFI_SUCCESS:
      return (UINT32) IHISI_SUCCESS;
      break;

    case EFI_BUFFER_TOO_SMALL:
      return (UINT32) IHISI_OB_LEN_TOO_SMALL;
      break;

    case EFI_UNSUPPORTED:
      return (UINT32) IHISI_UNSUPPORTED_FUNCTION;
      break;

    default:
      return (UINT32) IHISI_FBTS_PERMISSION_DENIED;
      break;
  }
}

BOOLEAN
IsVariableServiceSupported (
  )
{
  EFI_STATUS                    Status;
  UINTN                         VariableDataSize;

  Status = CommonGetVariable (NULL, NULL, &VariableDataSize, NULL);

  return Status == EFI_UNSUPPORTED ? FALSE : TRUE;
}

/**
  Check if this variable should be deleted. Search from the preserved list.

  @param[in]  VariableName               The variable name.
  @param[in]  VendorGuid                 The varialbe GUID.
  @param[in]  VariablePreservedTablePtr  The preserved table.
  @param[in]  IsKeepVariableInList       This flag determines the property of the preserved table.

  @retval TRUE                           This variable should be delteted
  @retval FALSE                          Keep this variable
**/
BOOLEAN
CheckVariableDelete (
  IN CHAR16                      *VariableName,
  IN EFI_GUID                    *VendorGuid,
  IN PRESERVED_VARIABLE_TABLE    *VariablePreservedTablePtr,
  IN BOOLEAN                      IsKeepVariableInList
  )
{
  UINTN                         Index;
  BOOLEAN                       IsVariableFound;

  //
  // According to logo requirement, should preserve all UEFI variables with VendorGuid
  // {77fa9abd-0359-4d32-bd60-28f4e78f784b}
  //
  if (CompareGuid (VendorGuid, &mDefaultPreservedVendorGuid)) {
    return FALSE;
  }
  //
  // Check if the variable is in the preserved list or not.
  //
  Index = 0;
  IsVariableFound = FALSE;
  while (VariablePreservedTablePtr[Index].VariableName != NULL) {
    if ((CompareGuid (VendorGuid, &VariablePreservedTablePtr[Index].VendorGuid)) &&
        (StrCmp (VariableName, VariablePreservedTablePtr[Index].VariableName) == 0)) {
      IsVariableFound = TRUE;
      break;
    }
    if ((CompareGuid (&VariablePreservedTablePtr[Index].VendorGuid, &mPreservedVariableTable2OtherAllGuid)) &&
        (StrCmp (VariablePreservedTablePtr[Index].VariableName, L"PreservedTable2OtherAllName") != 0)) {
      //
      // Only check VariableName
      //
      if ((StrCmp (VariableName, VariablePreservedTablePtr[Index].VariableName) == 0)) {
        IsVariableFound = TRUE;
        break;
      }
    }

    if ((!CompareGuid (&VariablePreservedTablePtr[Index].VendorGuid, &mPreservedVariableTable2OtherAllGuid)) &&
        (StrCmp (VariablePreservedTablePtr[Index].VariableName, L"PreservedTable2OtherAllName") == 0)) {
      //
      // Only check VendorGUID
      //
      if(CompareGuid (VendorGuid, &VariablePreservedTablePtr[Index].VendorGuid)) {
          IsVariableFound = TRUE;
          break;
      }
    }
    Index++;
  }

  //
  //  IsKeepVariableInList | IsVariableFound | result
  // ------------------------------------------------
  //         TRUE          |      TRUE       | Keep
  //         TRUE          |      FALSE      | Delete
  //         FALSE         |      TRUE       | Delete
  //         FALSE         |      FALSE      | Keep
  //
  if (IsKeepVariableInList != IsVariableFound) {
    return TRUE;
  }

  return FALSE;
}

/**
  Use variable service to get the next variable. If the variable name size is not enough, re-allocate memory for it.
  return the memory buffer size to "MaxVariableNameSize".

  @param[in, out]  VariableSize         The variable size.
  @param[in, out]  VariableName         The varialbe name.
  @param[in, out]  VendorGuid           The variable GUID.
  @param[in, out]  MaxVariableNameSize  The max variable name size. Will allocate memory according to this size.

  @retval EFI_INVALID_PARAMETER         Invalid parameters
  @retval EFI_OUT_OF_RESOURCES          Not enough memory
  @retval EFI_SUCCESS                   Successfully
**/
EFI_STATUS
RelocateNextVariableName (
  IN OUT UINTN                 *VariableSize,
  IN OUT CHAR16               **VariableName,
  IN OUT EFI_GUID              *VendorGuid,
  IN OUT UINTN                 *MaxVariableNameSize
  )
{
  EFI_STATUS      Status;
  EFI_GUID        NextVendorGuid;
  UINTN           NextVariableSize;
  CHAR16         *NextVariableName;

  if (VariableSize == NULL || VariableName == NULL || VendorGuid == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  NextVariableName = *VariableName;
  CopyGuid (&NextVendorGuid, VendorGuid);
  if (*VariableName == NULL) {
    NextVariableName = AllocateZeroPool (*MaxVariableNameSize);
    if (NextVariableName == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
  }

  NextVariableSize = *MaxVariableNameSize;
  Status = CommonGetNextVariableName (&NextVariableSize, NextVariableName, &NextVendorGuid);
  if (Status == EFI_BUFFER_TOO_SMALL) {
    NextVariableName = ReallocatePool (*MaxVariableNameSize, NextVariableSize, NextVariableName);
    if (NextVariableName == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
    *MaxVariableNameSize = NextVariableSize;
    Status = CommonGetNextVariableName (&NextVariableSize, NextVariableName, &NextVendorGuid);
  }

  if (!EFI_ERROR (Status)) {
    *VariableSize = NextVariableSize;
    *VariableName = NextVariableName;
    CopyGuid (VendorGuid, &NextVendorGuid);
  }

  return Status;
}

UINTN
GetNumberOfVariable (
  IN UINT8                      *TablePtr,
  IN BOOLEAN                    NewTableFlag
  )
{
  UINTN         NumberOfVariable;

  NumberOfVariable = 0;
  while (!IsZeroGuid ((EFI_GUID *) TablePtr)) {
    TablePtr += sizeof (EFI_GUID);
    if (NewTableFlag) {
      TablePtr += sizeof (BOOLEAN);
    }
    NumberOfVariable++;
    TablePtr += StrSize ((CHAR16 *) TablePtr);
  }

  //
  // Add one for the end of data
  //
  NumberOfVariable++;

  return NumberOfVariable;
}
/**
  Get corresponding IHISI area type by flash area type accordingly.

  @param[in] FlashAreaType  Flash area type.

  @return UINT8      Corresponding IHISI area type.
**/
STATIC
UINT8
GetMappingType (
  UINT8  FlashAreaType
  )
{
  UINTN     Index = 0;

  while (mFlashMapTypeMappingTable[Index].FlashAreaType != FbtsRomMapEos) {
    if (FlashAreaType == mFlashMapTypeMappingTable[Index].FlashAreaType) {
      return mFlashMapTypeMappingTable[Index].IhisiAreaType;
    } else {
      Index++;
    }
  }

  return FbtsRomMapUndefined;
}


/**
  Get flash map from HOB.

  @retval EFI_SUCCESS        Get flash map from HOB successful.
  @return Others             Any error occurred.
**/
EFI_STATUS
GetFlashMapByHob (
  VOID
  )
{
  EFI_STATUS                    Status;
  UINT8                         Index;
  VOID                         *HobList;
  EFI_FLASH_MAP_ENTRY_DATA     *FlashMapEntryData;

  HobList = GetHobList ();
  ASSERT (HobList != NULL);

  Index = 0;
  do {
    HobList = GetNextGuidHob (&gEfiFlashMapHobGuid, HobList);
    if (HobList == NULL) {
      //
      // Fill end of structure ROM map if all of flash map HOBs have been filled to ROM map buffer.
      //
      Status = FillPlatformRomMapBuffer ((UINT8)FbtsRomMapEos, 0, 0, Index);
      break;
    }

    FlashMapEntryData = (EFI_FLASH_MAP_ENTRY_DATA *) HobList;
    Status = FillPlatformRomMapBuffer (GetMappingType (
               FlashMapEntryData->AreaType),
               (UINT32) (FlashMapEntryData->Entries[0].Base),
               (UINT32) (FlashMapEntryData->Entries[0].Length),
               Index
               );
    Index++;
  } while (!EFI_ERROR (Status));

  return Status;
}

/**
  Get the default preserved variable table from PCD.

  @param[out]  TablePtr             The pointer to the default table.
  @param[out]  TableNewType         The flag is new or old structure of PCD preserved table.

  @return IHISI_SUCCESS               Get default table successfully.
  @return IHISI_INVALID_PARAMETER     TablePtr or TableNewType is NULL.
  @return IHISI_OUT_OF_RESOURCES      Allocate memory failed.
**/
STATIC
UINT32
GetDefaultTable (
  OUT VOID       **TablePtr,
  OUT BOOLEAN    *TableNewType
  )
{
  UINTN                         Index;
  UINTN                         NumberOfVariable;
  UINT8                         TempValue;
  UINT8                         *StringPtr;
  PRESERVED_VARIABLE_TABLE      *TempTablePtr;
  PRESERVED_VARIABLE_TABLE_2    *TempTablePtr2;

  if (TablePtr == NULL || TableNewType == NULL) {
    return IHISI_INVALID_PARAMETER;
  }

  StringPtr = NULL;
  TempTablePtr = NULL;
  TempTablePtr2 = NULL;
  //
  // Check the table is new or old structure
  //
  *TablePtr = (VOID *) PcdGetPtr (PcdDefaultPreservedVariableList);
  StringPtr = (UINT8 *) *TablePtr;
  StringPtr += sizeof (EFI_GUID);
  //
  // Assume the table is new, check the DeleteVariable value
  //
  TempValue = (UINT8) (*StringPtr);

  if ((TempValue == 0x00) || (TempValue == 0x01)) {
    //
    // Process new structure
    //
    if (!IsZeroGuid ((EFI_GUID *) *TablePtr)) {
      *TableNewType = TRUE;
      NumberOfVariable = GetNumberOfVariable ((UINT8 *) *TablePtr, TRUE);
      TempTablePtr2 = AllocatePool (NumberOfVariable * sizeof (PRESERVED_VARIABLE_TABLE_2));
      if (TempTablePtr2 == NULL) {
        return IHISI_OUT_OF_RESOURCES;
      }

      StringPtr = (UINT8 *) *TablePtr;
      for (Index = 0; (!IsZeroGuid ((EFI_GUID *) StringPtr)); Index++) {
        CopyMem (&TempTablePtr2[Index].VendorGuid, StringPtr, sizeof (EFI_GUID));

        StringPtr += sizeof (EFI_GUID);
        TempTablePtr2[Index].DeleteVariable = (UINT8) (*StringPtr);

        StringPtr += sizeof (BOOLEAN);
        TempTablePtr2[Index].VariableName = (CHAR16 *) StringPtr;
        //
        // Go to the next variable.
        //
        StringPtr += StrSize (TempTablePtr2[Index].VariableName);
      }
      //
      // Fill end of the table
      //
      CopyMem (&TempTablePtr2[Index].VendorGuid, StringPtr, sizeof (EFI_GUID));
      TempTablePtr2[Index].DeleteVariable = FALSE;
      TempTablePtr2[Index].VariableName = NULL;
    }
    *TablePtr = TempTablePtr2;
  } else {
    //
    // Process old structure
    //
    if (!IsZeroGuid ((EFI_GUID *) *TablePtr)) {
      *TableNewType = FALSE;
      NumberOfVariable = GetNumberOfVariable ((UINT8 *) *TablePtr, FALSE);
      TempTablePtr = AllocatePool (NumberOfVariable * sizeof (PRESERVED_VARIABLE_TABLE));
      if (TempTablePtr == NULL) {
        return IHISI_OUT_OF_RESOURCES;
      }
      StringPtr = (UINT8 *) *TablePtr;
      for (Index = 0; (!IsZeroGuid ( (EFI_GUID *) StringPtr)); Index++) {
        CopyMem (&TempTablePtr[Index].VendorGuid, StringPtr, sizeof (EFI_GUID));

        StringPtr += sizeof (EFI_GUID);
        TempTablePtr[Index].VariableName = (CHAR16 *) StringPtr;
        //
        // Go to the next variable.
        //
        StringPtr += StrSize (TempTablePtr[Index].VariableName);
      }
      //
      // Fill end of the table
      //
      CopyMem (&TempTablePtr[Index].VendorGuid, StringPtr, sizeof (EFI_GUID));
      TempTablePtr[Index].VariableName = NULL;
    }
    *TablePtr = TempTablePtr;
  }

  return IHISI_SUCCESS;
}

/**
  Check target variable repeated of the VariablePreservedTable2, the range is from Index 0 to specific target index.

  @param[in]       VariablePreservedTable2      The pointer to the new structure table
  @param[in]       TargetIndex                  The specific index of target variable in VariablePreservedTable2
  @param[out]      IsRepeated                   This flag determines the target variable is repeated or not from Index 0 to targetIndex

**/
VOID
IsVariableRepeatedOfPreservedTable2(
  IN  PRESERVED_VARIABLE_TABLE_2              *VariablePreservedTable2,
  IN  UINTN                                   TargetIndex,
  OUT BOOLEAN                                 *IsRepeated
  )
{
  UINTN                         TempIndex;
  //
  // Compare from index 0 to specific target index
  //
  *IsRepeated = FALSE;
  for (TempIndex=0; TempIndex < TargetIndex; TempIndex++) {
    if (CompareGuid(&VariablePreservedTable2[TargetIndex].VendorGuid, &VariablePreservedTable2[TempIndex].VendorGuid)) {
      if ((StrCmp(VariablePreservedTable2[TargetIndex].VariableName, VariablePreservedTable2[TempIndex].VariableName) == 0) ||
          (StrCmp(VariablePreservedTable2[TempIndex].VariableName, L"PreservedTable2OtherAllName") == 0)) {
        *IsRepeated = TRUE;
        break;
      }
    }

    if (StrCmp(VariablePreservedTable2[TargetIndex].VariableName, VariablePreservedTable2[TempIndex].VariableName) == 0) {
      if (CompareGuid(&VariablePreservedTable2[TempIndex].VendorGuid, &mPreservedVariableTable2OtherAllGuid)) {
        *IsRepeated = TRUE;
        break;
      }
    }

    if ((StrCmp(VariablePreservedTable2[TempIndex].VariableName, L"PreservedTable2OtherAllName") == 0) &&
        (CompareGuid(&VariablePreservedTable2[TempIndex].VendorGuid, &mPreservedVariableTable2OtherAllGuid))) {
      *IsRepeated = TRUE;
      break;
    }
  }
}

/**
  Parse the new preserved variable table to old structure.

  @param[in]       VariablePreservedTable2      The pointer to the new structure table
  @param[out]      VariablePreservedTable       The pointer to the old structure table
  @param[out]      IsKeepVariableInList         This flag determines the property of the old dtructure preserved table

**/
VOID
ParsePreservedTable(
  IN  PRESERVED_VARIABLE_TABLE_2              *VariablePreservedTable2,
  OUT PRESERVED_VARIABLE_TABLE                **VariablePreservedTable,
  OUT BOOLEAN                                 *IsKeepVariableInList
  )
{
  UINTN                         Index;
  UINTN                         Index1;
  UINTN                         NumberOfVariable;
  BOOLEAN                       DeleteVariable;
  BOOLEAN                       IsRepeated;
  PRESERVED_VARIABLE_TABLE      *TempBuffer;

  //
  // Get the DeleteVariable when below two conditions are both ture,
  // GUID = {0xffffffff, 0xffff, 0xffff, { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,0xff, 0xff}
  // Name = PreservedTable2OtherAllName
  //
  DeleteVariable = FALSE;
  for (Index=0; VariablePreservedTable2[Index].VariableName != NULL; Index++) {
    if (CompareGuid (&VariablePreservedTable2[Index].VendorGuid, &mPreservedVariableTable2OtherAllGuid) &&
       (StrCmp (VariablePreservedTable2[Index].VariableName, L"PreservedTable2OtherAllName") == 0)) {
      DeleteVariable = VariablePreservedTable2[Index].DeleteVariable;
      break;
    }
  }

  Index = 0;
  while(VariablePreservedTable2[Index].VariableName != NULL) {
    Index++;
  }
  NumberOfVariable = (Index + 1);
  TempBuffer = AllocateZeroPool (sizeof (PRESERVED_VARIABLE_TABLE) * NumberOfVariable);
  if (TempBuffer == NULL) {
    VariablePreservedTable = NULL;
    return;
  }

  //
  // Collect declarations if there DeleteVariable are different and also skip repeated of declarations
  //
  Index1 = 0;
  for (Index=0; VariablePreservedTable2[Index].VariableName != NULL; Index++) {
    if ((!CompareGuid (&VariablePreservedTable2[Index].VendorGuid, &mPreservedVariableTable2OtherAllGuid)) ||
        (StrCmp (VariablePreservedTable2[Index].VariableName, L"PreservedTable2OtherAllName") != 0)) {
      //
      // Check target variable repeated of the VariablePreservedTable2, the range is from index 0 to specific target index.
      //
      IsVariableRepeatedOfPreservedTable2(VariablePreservedTable2, Index, &IsRepeated);
      if ((IsRepeated != TRUE) && (VariablePreservedTable2[Index].DeleteVariable != DeleteVariable)) {
        TempBuffer[Index1].VariableName = AllocateCopyPool ( StrSize ( (CHAR16 *) VariablePreservedTable2[Index].VariableName), VariablePreservedTable2[Index].VariableName);
        CopyMem (&TempBuffer[Index1].VendorGuid, &VariablePreservedTable2[Index].VendorGuid, sizeof (EFI_GUID));
        Index1++;
      }
    }
  }
  //
  // Fill the end of table
  //
  TempBuffer[Index1].VariableName = NULL;
  CopyMem (&TempBuffer[Index1].VendorGuid, &VariablePreservedTable2[Index].VendorGuid, sizeof (EFI_GUID));

  *IsKeepVariableInList   = DeleteVariable;
  *VariablePreservedTable = TempBuffer;
}

/**
 Purify the variables if needed. If there is NO OemService "OemSvcVariablePreservedTable",
 do nothing in this function.

 @retval IHISI_SUCCESS               Success
 @retval IHISI_UNSUPPORTED_FUNCTION  there is no SmmVariable service
 @retval IHISI_OUT_OF_RESOURCES      not enough memory

**/
UINT32
PurifyVariable (
  VOID
  )
{
  UINT32                        Status;
  EFI_STATUS                    EfiStatus;
  EFI_STATUS                    OemSVc2Status;
  EFI_STATUS                    OemSVcStatus;
  EFI_GUID                      VendorGuid;
  EFI_GUID                      NextVendorGuid;
  UINTN                         VariableNameSize;
  UINTN                         MaxVariableNameSize;
  BOOLEAN                       IsKeepVariableInList;
  BOOLEAN                       TableNewType;
  UINTN                         NextVariableNameSize;
  CHAR16                        *VariableName;
  CHAR16                        *NextVariableName;
  PRESERVED_VARIABLE_TABLE      *VariablePreservedTablePtr;
  PRESERVED_VARIABLE_TABLE_2    *VariablePreservedTablePtr2;
  VOID                          *VoidTempPtr;

  if (!IsVariableServiceSupported ()) {
    return IHISI_UNSUPPORTED_FUNCTION;
  }
  //
  // Get default preserved variable table from PCD, and check the table is new or old structure
  //
  IsKeepVariableInList = PcdGetBool (PcdKeepVariableInList);
  TableNewType = FALSE;
  VariablePreservedTablePtr  = NULL;
  VariablePreservedTablePtr2 = NULL;
  Status = GetDefaultTable (&VoidTempPtr, &TableNewType);
  if (Status != IHISI_SUCCESS) {
    return Status;
  }

  if (TableNewType == TRUE) {
    //
    // The data is new structure from PCD
    //
    VariablePreservedTablePtr2 = (PRESERVED_VARIABLE_TABLE_2 *) VoidTempPtr;
  } else {
    //
    // The data is old structure from PCD
    //
    VariablePreservedTablePtr = (PRESERVED_VARIABLE_TABLE *) VoidTempPtr;
  }

  //
  // Get variable preserved table from OemServices, the oemServices only support new structure
  //
  DEBUG ((DEBUG_INFO, "OemKernelServices Call: OemSvcVariablePreservedTable2 \n"));
  OemSVc2Status = OemSvcVariablePreservedTable2 (
                    &VariablePreservedTablePtr2
                    );
  DEBUG ((DEBUG_INFO, "OemKernelServices OemSvcVariablePreservedTable2 Status: %r\n", OemSVc2Status));
  if (!EFI_ERROR (OemSVc2Status)) {
    return IHISI_SUCCESS;
  }
  //
  // Check old table OemSVCCariablePreserved If OemSvcVariablePreservedTable2 return EFI_UNSUPPORT
  //
  OemSVcStatus = EFI_SUCCESS;
  if (OemSVc2Status == EFI_UNSUPPORTED) {
    DEBUG ((DEBUG_INFO, "OemKernelServices Call: OemSvcVariablePreservedTable \n"));
    OemSVcStatus = OemSvcVariablePreservedTable (
                     &VariablePreservedTablePtr,
                     &IsKeepVariableInList
                   );
    DEBUG ((DEBUG_INFO, "OemKernelServices OemSvcVariablePreservedTable Status: %r\n", OemSVcStatus));
    if (!EFI_ERROR (OemSVcStatus)) {
      return IHISI_SUCCESS;
    }
  }

  if ((OemSVc2Status == RETURN_MEDIA_CHANGED) ||
      ((OemSVc2Status == RETURN_UNSUPPORTED) && (OemSVcStatus == RETURN_UNSUPPORTED) && (TableNewType == TRUE))) {
    if (VariablePreservedTablePtr2 == NULL) {
    //
    // This OemService does not exist, so do nothing.
    //
      return IHISI_SUCCESS;
    }
    //
    // Parse data from new to old structure
    //
    ParsePreservedTable (VariablePreservedTablePtr2, &VariablePreservedTablePtr, &IsKeepVariableInList);
  }

  if (VariablePreservedTablePtr == NULL) {
    //
    // This OemService does not exist, so do nothing.
    //
    return IHISI_SUCCESS;
  }

  if ( (!IsKeepVariableInList) && (VariablePreservedTablePtr->VariableName == NULL)) {
    //
    // Clear an empty table, so do nothing.
    //
    return IHISI_SUCCESS;
  }

  NextVariableName = NULL;
  VariableNameSize = sizeof (UINT8);
  VariableName = AllocatePool (VariableNameSize);
  if (VariableName == NULL) {
    Status = IHISI_OUT_OF_RESOURCES;
    goto Done;
  }

  MaxVariableNameSize = DEFAULT_VARIABLE_NAME_SIZE;
  EfiStatus = RelocateNextVariableName (&NextVariableNameSize, &NextVariableName, &NextVendorGuid, &MaxVariableNameSize);

  while (!EFI_ERROR (EfiStatus)) {
    if (VariableNameSize < MaxVariableNameSize) {
      VariableName = ReallocatePool (VariableNameSize, MaxVariableNameSize, VariableName);
      VariableNameSize = MaxVariableNameSize;
      if (VariableName == NULL) {
        Status = IHISI_OUT_OF_RESOURCES;
        goto Done;
      }
    }
    CopyMem (VariableName, NextVariableName, NextVariableNameSize);
    CopyGuid (&VendorGuid, &NextVendorGuid);
    EfiStatus = RelocateNextVariableName (&NextVariableNameSize, &NextVariableName, &NextVendorGuid, &MaxVariableNameSize);
    if (EfiStatus == EFI_OUT_OF_RESOURCES) {
      Status = IHISI_OUT_OF_RESOURCES;
      goto Done;
    }

    if (CheckVariableDelete (VariableName, &VendorGuid, VariablePreservedTablePtr, IsKeepVariableInList)) {
      CommonSetVariable (VariableName, &VendorGuid, 0, 0, NULL);
    }
  }
  Status = IHISI_SUCCESS;

Done:

  if (VariableName != NULL) {
    FreePool (VariableName);
  }

  if (NextVariableName != NULL) {
    FreePool (NextVariableName);
  }

  return Status;
}

STATIC
EFI_STATUS
UpdateExtendPlatformBuildDateTimeInfo (
  IN OUT EXTEND_PLATFORM_DATA_ITEM                *ExtInfoDataItemPtr
  )
{
  UINTN                                     StringSize;
  EFI_STATUS                                Status;
  CHAR16                                    StringDate[MODEL_DATE_SIZE];
  CHAR16                                    StringTime[MODEL_TIME_SIZE];
  UINT32                                    BufferSize;

  BufferSize = ExtInfoDataItemPtr->DataSize;
  ExtInfoDataItemPtr->DataSize = 0;

  //
  // Get build date string
  //
  StringSize = MODEL_DATE_SIZE;
  Status  = GetBvdtInfo ((BVDT_TYPE) BvdtBuildDate, &StringSize, StringDate);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Get build time string
  //
  Status  = GetBvdtInfo ((BVDT_TYPE) BvdtBuildTime, &StringSize, StringTime);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  ExtInfoDataItemPtr->DataSize = (UINT32)(StrSize (StringDate) + StrSize (StringTime));
  if (ExtInfoDataItemPtr->DataSize > BufferSize) {
    return EFI_BUFFER_TOO_SMALL;
  }

  //Merge Build date String . Build time string
  StringSize = StrSize (StringDate);
  StringDate[(StrSize (StringDate)/sizeof (StringDate[0])) - 1] = L' ';
  CopyMem ( ExtInfoDataItemPtr->Data, StringDate, StringSize);
  //
  // Update build time string
  //
  ExtInfoDataItemPtr = (EXTEND_PLATFORM_DATA_ITEM *)(ExtInfoDataItemPtr->Data + StringSize);
  StringSize = StrSize (StringTime);
  CopyMem ( ExtInfoDataItemPtr, StringTime, StringSize);

  return EFI_SUCCESS;
}

/**
  AH=10h, Get permission,hook OemSvcIhisiS10HookGetPermission.

  @retval EFI_SUCCESS        Command successful returned.
**/
EFI_STATUS
EFIAPI
OemFbtsGetPermission (
  VOID
  )
{
  EFI_STATUS                        Status;
  UINT32                            Eax;
  UINT16                            Permission;
  FBTS_TOOLS_VERSION_BUFFER        *VersionPtr;

  VersionPtr = (FBTS_TOOLS_VERSION_BUFFER *)(UINTN) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RDI);
   if (VersionPtr->Signature != FBTS_VERSION_SIGNATURE) {
    return IHISI_UNSUPPORTED_FUNCTION;
  }
  //
  //Get Permission
  //
  Permission = (UINT16)IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RAX);
  DEBUG ((DEBUG_INFO, "OemKernelServices Call: OemSvcIhisiS10HookGetPermission \n"));
  Status = OemSvcIhisiS10HookGetPermission (&Permission);
  DEBUG ((DEBUG_INFO, "OemKernelServices OemSvcIhisiS10HookGetPermission Status: %r\n", Status));

  Eax = IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RAX);
  Eax = (UINT32)((Eax & 0xffff0000) | Permission);
  IhisiProtWriteCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RAX, Eax);

  return IHISI_SUCCESS;
}

/**
  AH=10h, Get Ac status,hook OemSvcIhisiS10HookGetAcStatus


  @retval EFI_SUCCESS        Command successful returned.
**/
EFI_STATUS
EFIAPI
OemFbtsGetAcStatus (
  VOID
  )
{
  EFI_STATUS                        Status;
  UINT8                             AcStatus;
  FBTS_PLATFORM_STATUS_BUFFER      *PlatformInfoPtr;
  //
  //Get AC Status.
  //
  PlatformInfoPtr = (FBTS_PLATFORM_STATUS_BUFFER *)(UINTN) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RDI);

  AcStatus = AC_PlugIn;
  DEBUG ((DEBUG_INFO, "OemKernelServices Call: OemSvcIhisiS10HookGetAcStatus \n"));
  Status = OemSvcIhisiS10HookGetAcStatus (&AcStatus);
  DEBUG ((DEBUG_INFO, "OemKernelServices OemSvcIhisiS10HookGetAcStatus Status: %r\n", Status));

  PlatformInfoPtr->AcStatus = AcStatus;
  return IHISI_SUCCESS;
}


/**
  AH=10h, Get battery life,hook OemSvcIhisiS10HookGetBatterylife.

  @retval EFI_SUCCESS        Command successful returned.
**/
EFI_STATUS
EFIAPI
OemFbtsGetBatteryLife (
  VOID
  )
{
  EFI_STATUS                        Status;
  UINT8                             BattLife;
  FBTS_PLATFORM_STATUS_BUFFER      *PlatformInfoPtr;

  //
  //Get AC Status and BattLife.
  //
  PlatformInfoPtr = (FBTS_PLATFORM_STATUS_BUFFER *)(UINTN) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RDI);

  BattLife = 100;
  DEBUG ((DEBUG_INFO, "OemKernelServices Call: OemSvcIhisiS10HookGetBatterylife \n"));
  Status = OemSvcIhisiS10HookGetBatterylife (&BattLife);
  DEBUG ((DEBUG_INFO, "OemKernelServices OemSvcIhisiS10HookGetBatterylife Status: %r\n", Status));

  PlatformInfoPtr->Battery = BattLife;
  return IHISI_SUCCESS;
}

/**
  AH=11h, AP check ,hook OemSvcIhisiS11HookFbtsApCheck.

@retval
  BIT 0: Model name check
  BIT 1: Model version check
  BIT 2: Allow same version file.
  BIT 3: Verify file checksum
  BIT 4: Disable display model name
  BIT 5: Disable display model version
  BIT 6: Disable read comparison while flash process
  BIT 7: Enable ECX to return extended flag.
**/
EFI_STATUS
EFIAPI
OemFbtsApCheck (
  VOID
  )
{
  EFI_STATUS                                Status;
  UINT32                                    Eax;
  UINT32                                    Ecx;
  UINT8                                     ApCheck;

  //
  //Udpate AP check capability
  //
  ApCheck = AP_DO_NOTHING | ENABLE_EXTENDED_FLAG;
  DEBUG ((DEBUG_INFO, "OemKernelServices Call: OemSvcIhisiS11HookFbtsApCheck \n"));
  Status = OemSvcIhisiS11HookFbtsApCheck (&ApCheck);
  DEBUG ((DEBUG_INFO, "OemKernelServices OemSvcIhisiS11HookFbtsApCheck Status: %r\n", Status));

  Eax = IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RAX);
  Eax = (UINT32) ((Eax & 0xffff00ff) | (ApCheck<<8));
  IhisiProtWriteCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RAX, Eax);

  Ecx = IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RCX);
  Ecx |= ALLOW_ALP_COMMAND;
  IhisiProtWriteCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RCX, Ecx);

  return IHISI_SUCCESS;
}

/**
  AH=12h(FbtsGetPlatformRomMap), Get Oem define flash map.

  @retval EFI_SUCCESS       Get OEM flash map successful.
  @retval EFI_UNSUPPORTED   FBTS_OEM_ROM_MAP_COUNT is 0 or module rom map buffer is full.
**/
EFI_STATUS
EFIAPI
OemGetPlatformRomMap (
  VOID
  )
{
  EFI_STATUS                          Status;
  FBTS_PLATFORM_ROM_MAP               *OemRomMap;
  UINTN                               OemRomMapSize;
  FBTS_PLATFORM_ROM_MAP_BUFFER        *InputRomMapBuffer;
  FBTS_PLATFORM_PRIVATE_ROM           *OemPrivateRomMap;
  UINTN                               OemPrivateRomMapSize;
  FBTS_PLATFORM_PRIVATE_ROM_BUFFER    *InputPrivateRomMapBuffer;
  FBTS_PLATFORM_ROM_MAP               *OemRomMapBuffer;
  FBTS_PLATFORM_PRIVATE_ROM           *OemPrivateRomMapBuffer;
  UINTN                               Index;

  InputRomMapBuffer = (FBTS_PLATFORM_ROM_MAP_BUFFER *)(UINTN) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RDI);
  InputPrivateRomMapBuffer = (FBTS_PLATFORM_PRIVATE_ROM_BUFFER *)(UINTN) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RSI);

  OemRomMap = AllocatePool (sizeof(FBTS_PLATFORM_ROM_MAP_BUFFER));
  if (OemRomMap == NULL) {
    return IHISI_OB_LEN_TOO_SMALL;
  }

  CopyMem (OemRomMap, InputRomMapBuffer, sizeof (FBTS_PLATFORM_ROM_MAP_BUFFER));
  for (Index = 0, OemRomMapSize = 0; Index < sizeof(FBTS_PLATFORM_ROM_MAP_BUFFER)/sizeof(FBTS_PLATFORM_ROM_MAP); Index++) {
    if(OemRomMap[Index].Type == FbtsRomMapEos) {
      OemRomMapSize = Index++;
      break;
    }
  }

  OemPrivateRomMap = AllocatePool (sizeof (FBTS_PLATFORM_PRIVATE_ROM_BUFFER));
  if (OemPrivateRomMap == NULL) {
    return IHISI_OB_LEN_TOO_SMALL;
  }

  CopyMem (OemPrivateRomMap, InputPrivateRomMapBuffer, sizeof (FBTS_PLATFORM_PRIVATE_ROM_BUFFER));
  for (Index = 0, OemPrivateRomMapSize = 0; Index < sizeof(FBTS_PLATFORM_PRIVATE_ROM_BUFFER)/sizeof(FBTS_PLATFORM_PRIVATE_ROM); Index++) {
    if(OemPrivateRomMap[Index].LinearAddress == FbtsRomMapEos) {
      OemPrivateRomMapSize = Index++;
      break;
    }
  }

  //
  // Record currently allocate buffer
  //
  OemRomMapBuffer        = OemRomMap;
  OemPrivateRomMapBuffer = OemPrivateRomMap;

  DEBUG ((DEBUG_INFO, "OemKernelServices Call: OemSvcIhisiS12HookGetOemFlashMap \n"));
  Status = OemSvcIhisiS12HookGetOemFlashMap (
             &OemRomMapSize,
             &OemRomMap,
             &OemPrivateRomMapSize,
             &OemPrivateRomMap
             );
  DEBUG ((DEBUG_INFO, "OemKernelServices OemSvcIhisiS12HookGetOemFlashMap Status: %r\n", Status));

  if (OemRomMapSize == 0 || OemPrivateRomMapSize == 0) {
    // Error , not overwrite default data;
    FreePool (OemRomMap);
    FreePool (OemPrivateRomMap);
    return IHISI_SUCCESS;
  }

  if ((Status == EFI_SUCCESS) || (Status == EFI_MEDIA_CHANGED)) {
    if (OemPrivateRomMap != NULL) {
      for (Index = 0; Index < sizeof(FBTS_PLATFORM_PRIVATE_ROM_BUFFER)/sizeof(FBTS_PLATFORM_PRIVATE_ROM); Index++ ) {
        Status = FillPlatformPrivateRomMapBuffer (
                   OemPrivateRomMap[Index].LinearAddress,
                   OemPrivateRomMap[Index].Size,
                   (UINT8) Index
                   );
        if (EFI_ERROR(Status)) {
          FillPlatformPrivateRomMapBuffer ((UINT8) FbtsRomMapEos, 0, 0);
          break;
        }
        if (OemPrivateRomMap[Index].LinearAddress == (UINT32)FbtsRomMapEos) {
          break;
        }
      }
    }

    if (OemRomMap != NULL) {
      for (Index = 0 ; Index < sizeof(FBTS_PLATFORM_ROM_MAP_BUFFER)/sizeof(FBTS_PLATFORM_ROM_MAP); Index++) {
        Status = FillPlatformRomMapBuffer (
                   OemRomMap[Index].Type,
                   OemRomMap[Index].Address,
                   OemRomMap[Index].Length,
                   (UINT8) Index
                   );
        if (EFI_ERROR(Status)) {
          FillPlatformRomMapBuffer ((UINT8) FbtsRomMapEos, 0, 0, 0);
          break;
        }
        if (OemRomMap[Index].Type == FbtsRomMapEos) {
          break;
        }
      }
    }

    //
    // Update protect ROM map
    //
    CopyMem (InputRomMapBuffer, &mRomMapBuffer, sizeof (FBTS_PLATFORM_ROM_MAP_BUFFER));

    //
    // Update private ROM map
    //
    CopyMem (InputPrivateRomMapBuffer, &mPrivateRomMapBuffer, sizeof (FBTS_PLATFORM_PRIVATE_ROM_BUFFER));
  }

  //
  // Compare the buffer address same with allocate address
  //
  if (OemRomMapBuffer == OemRomMap) {
    FreePool (OemRomMap);
  }

  if (OemPrivateRomMapBuffer == OemPrivateRomMap) {
    FreePool (OemPrivateRomMap);
  }

  return IHISI_SUCCESS;
}

/**
  AH=1Eh,Get whole BIOS ROM map, hook OemSvcIhisiGetWholeBiosRomMap;

  @retval EFI_SUCCESS                 FBTS get BIOS ROM map success.
  @return Others                      FBTS get BIOS ROM map failed.
**/
EFI_STATUS
EFIAPI
OemFbtsGetWholeBiosRomMap (
  VOID
  )
{
  EFI_STATUS                            Status;
  UINTN                                 RomMapSize;
  UINTN                                 NumberOfRegions;
  UINT8                                *RomMapPtr;
  FBTS_INTERNAL_BIOS_ROM_MAP           *BiosRomMap;

  NumberOfRegions = 0;
  BiosRomMap  = (FBTS_INTERNAL_BIOS_ROM_MAP *) (UINTN) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RDI);
  if (BiosRomMap == NULL) {
    return IHISI_ACCESS_PROHIBITED;
  }

  //
  // checck buffer in cmd buffer before accessing the data
  //
  for (NumberOfRegions = 0; ;NumberOfRegions++) {
    if (BiosRomMap[NumberOfRegions].Type == FbtsRomMapEos) {
      NumberOfRegions++;
      break;
    }
  }

  //
  // Save default Rommap to RomMapPtr
  //
  RomMapSize = NumberOfRegions * sizeof (FBTS_INTERNAL_BIOS_ROM_MAP);
  RomMapPtr = NULL;
  RomMapPtr = AllocatePool (RomMapSize);
  if (RomMapPtr == NULL) {
    return IHISI_OUT_OF_RESOURCES;
  }
  CopyMem ((VOID *)RomMapPtr, (VOID *)BiosRomMap, RomMapSize);

  //
  // OemSvcIhisiGetWholeBiosRomMap update default Rommap
  //
  DEBUG ((DEBUG_INFO, "OemKernelServices Call: OemSvcIhisiGetWholeBiosRomMap \n"));
  Status = OemSvcIhisiGetWholeBiosRomMap ((VOID **)&BiosRomMap, &NumberOfRegions);
  DEBUG ((DEBUG_INFO, "OemKernelServices OemSvcIhisiGetWholeBiosRomMap Status: %r\n", Status));
  if (Status == EFI_SUCCESS) {
    FreePool (RomMapPtr);
    return IHISI_SUCCESS;
  }

  if (BiosRomMap[NumberOfRegions - 1].Type != FbtsRomMapEos) {
    //Error,  restore default rommap
    BiosRomMap  = (FBTS_INTERNAL_BIOS_ROM_MAP *) (UINTN) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RDI);
    CopyMem ((VOID *)BiosRomMap, (VOID *)RomMapPtr, RomMapSize);
    FreePool (RomMapPtr);
    return IHISI_FBTS_UNKNOWN_PLATFORM_ROM_MAP;
  }

  RomMapSize = NumberOfRegions * sizeof (FBTS_INTERNAL_BIOS_ROM_MAP);
  CopyMem ((VOID *)(UINTN)IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RDI), (VOID *)BiosRomMap, RomMapSize);
  FreePool (RomMapPtr);

  return IHISI_SUCCESS;
}

/**
  AH=1Fh, AP Hook Point for BIOS

  @retval EFI_SUCCESS                 FBTS get BIOS ROM map success.
  @return Others                      FBTS get BIOS ROM map failed.
**/
EFI_STATUS
EFIAPI
OemFbtsApHookForBios (
  VOID
  )
/*++

Routine Description:

  AH=1Fh, This call back function will be invoked several times during flash process.
  BIOS can know which step is running now.
  BIOS can base on it to do specific hook such as EC idle and weak up.

Arguments:

  CL - denote the start of AP process.
       CL = 0x00, AP terminate. (Before IHISI 0x16)
       CL = 0x01, AP start. (After IHISI 0x10)
       CL = 0x02, Start to read ROM. (Before IHISI 0x14)
       CL = 0x03, Start to write ROM. (Before IHISI 0x15)
       CL = 0x04, Start to write EC. (Before IHISI 0x20)
       CL = 0x05, Before dialog popup.
       CL = 0x06, After dialog close and continue running.

Returns:

--*/
{
  UINT8                                 ApState;
  EFI_STATUS                            Status;

  ApState = (UINT8) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RCX); // CL

  DEBUG ((DEBUG_INFO, "OemKernelServices Call: OemSvcIhisiS1FHookFbtsApHookForBios \n"));
  Status = OemSvcIhisiS1FHookFbtsApHookForBios (ApState);
  DEBUG ((DEBUG_INFO, "OemKernelServices OemSvcIhisiS1FHookFbtsApHookForBios Status: %r\n", Status));

  return IHISI_SUCCESS;
}

/**
  AH=10h,Get Permission, hook SmmCsSvcIhisiFbtsGetPermission.

  @retval EFI_SUCCESS        Command successful returned.
**/
EFI_STATUS
EFIAPI
ChipsetFbtsGetPermission (
  VOID
  )
{
  UINT32                            Eax;
  UINT16                            Permission;
  FBTS_TOOLS_VERSION_BUFFER        *VersionPtr;

  VersionPtr = (FBTS_TOOLS_VERSION_BUFFER *)(UINTN) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RDI);
  if (VersionPtr->Signature != FBTS_VERSION_SIGNATURE) {
     return IHISI_UNSUPPORTED_FUNCTION;
  }
  //
  //Get Permission
  //
  Permission = FBTS_PERMISSION_ALLOWED;
  SmmCsSvcIhisiFbtsGetPermission (VersionPtr, &Permission);
  Eax = IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RAX);
  Eax = (UINT32)((Eax & 0xffff0000) | Permission);
  IhisiProtWriteCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RAX, Eax);

  return IHISI_SUCCESS;
}


/**
  AH=14h ,FbtsRead before process , hook SmmCsSvcIhisiFbtsDoBeforeReadProcess

  @retval EFI_SUCCESS            Chipset FBTS read success.
  @return Others                 FBTS read failed.
**/
EFI_STATUS
EFIAPI
ChipsetFbtsDoBeforeReadProcess (
  VOID
  )
{
  EFI_STATUS                      Status;
  UINTN                           Size;
  UINTN                           Offset;
  UINTN                           Address;
  UINT8                          *DataBuffer;

  //
  // ECX    - Size to read.
  // DS:ESI - Pointer to returned data buffer. Size in ECX.
  // EDI    - Target linear address to read.
  //
  DataBuffer = (UINT8 *)(UINTN) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RSI);
  Size       = (UINTN) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RCX);
  Address    = (UINTN) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RDI);
  Offset     = 0;

  Status = SmmCsSvcIhisiFbtsDoBeforeReadProcess(&Address, &Size, DataBuffer);
  if (Status != EFI_SUCCESS) {
    return Status;
  }

  IhisiProtWriteCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RCX, (UINT32)Size       );
  IhisiProtWriteCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RDI, (UINT32)Address    );

  return IHISI_SUCCESS;
}

/**
  AH=14h ,FbtsRead after process , hook SmmCsSvcIhisiFbtsDoAfterReadProcess
**/
EFI_STATUS
EFIAPI
ChipsetFbtsDoAfterReadProcess (
  VOID
  )
{
  EFI_STATUS                      Status;

  Status = (EFI_STATUS) (IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RAX) & 0xff);

  if (Status != EFI_SUCCESS) {
    Status = ENCODE_ERROR (Status);
  }

  Status = SmmCsSvcIhisiFbtsDoAfterReadProcess(Status);

  return Status;
}

/**
  AH=15h ,FbtsWrite before process , hook SmmCsSvcIhisiFbtsDoBeforeWriteProcess

  @retval EFI_SUCCESS            Chipset FBTS read success.
  @return Others                 FBTS read failed.
**/
EFI_STATUS
EFIAPI
ChipsetFbtsDoBeforeWriteProcess (
  VOID
  )
{
  EFI_STATUS                      Status;
  UINTN                           WriteSize;
  UINTN                           RomBaseAddress;
  UINT8                          *WriteDataBuffer;

  //
  // ECX    - Size to write.
  // DS:ESI - Pointer to returned data buffer. Size in ECX.
  // EDI    - Target linear address to write.
  //
  WriteDataBuffer = (UINT8 *)(UINTN) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RSI);
  WriteSize       = (UINTN) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RCX);
  RomBaseAddress  = (UINTN) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RDI);


  Status = SmmCsSvcIhisiFbtsDoBeforeWriteProcess(WriteDataBuffer, &WriteSize, &RomBaseAddress);
  if (Status != EFI_SUCCESS) {
    return Status;
  }

  IhisiProtWriteCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RCX, (UINT32)WriteSize       );
  IhisiProtWriteCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RDI, (UINT32)RomBaseAddress    );

  return IHISI_SUCCESS;
}

/**
  AH=15h ,FbtsWrite after process , hook SmmCsSvcIhisiFbtsDoAfterWriteProcess
**/
EFI_STATUS
EFIAPI
ChipsetFbtsDoAfterWriteProcess (
  VOID
  )
{
  EFI_STATUS                      Status;

  Status = (EFI_STATUS) (IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RAX) & 0xff);

  if (Status != EFI_SUCCESS) {
    Status = ENCODE_ERROR (Status);
  }

  Status = SmmCsSvcIhisiFbtsDoAfterWriteProcess(Status);

  return Status;
}


/**
  AH=16h, FbtsComplete, hook SmmCsSvcIhisiFbtsApTerminated.

  @retval EFI_SUCCESS        Function succeeded.
**/
EFI_STATUS
EFIAPI
ChipsetFbtsCompleteApTerminated (
  VOID
  )
{
  EFI_STATUS                    Status;
  FBTS_FLASH_COMPLETE_STATUS   *FlashCompleteStatus;


  *(mSmmFwBlockService->FlashMode) = SMM_FW_DEFAULT_MODE;

  FlashCompleteStatus = (FBTS_FLASH_COMPLETE_STATUS *)(UINTN) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RSI);

  if ((FlashCompleteStatus->Signature != FLASH_COMPLETE_STATUS_SIGNATURE) ||
      (FlashCompleteStatus->StructureSize != sizeof (FBTS_FLASH_COMPLETE_STATUS))) {
    return IHISI_INVALID_PARAMETER;
  }

  Status = EFI_SUCCESS;
  if (FlashCompleteStatus->CompleteStatus == ApTerminated) {
    Status = SmmCsSvcIhisiFbtsApTerminated ();
    IhisiErrorCodeHandler((UINT32)Status);
  }

  IhisiErrorCodeHandler((UINT32)Status);
  return IHISI_SUCCESS;
}

/**
  AH=16h, FbtsComplete, hook SmmCsSvcIhisiFbtsNormalFlash.

  @retval EFI_SUCCESS        Function succeeded.
**/
EFI_STATUS
EFIAPI
ChipsetFbtsCompleteNormalFlash (
  VOID
  )
{
  EFI_STATUS                    Status;
  FBTS_FLASH_COMPLETE_STATUS   *FlashCompleteStatus;


  *(mSmmFwBlockService->FlashMode) = SMM_FW_DEFAULT_MODE;
  FlashCompleteStatus = (FBTS_FLASH_COMPLETE_STATUS *)(UINTN) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RSI);

  if ((FlashCompleteStatus->Signature != FLASH_COMPLETE_STATUS_SIGNATURE) ||
      (FlashCompleteStatus->StructureSize != sizeof (FBTS_FLASH_COMPLETE_STATUS))) {
    return IHISI_INVALID_PARAMETER;
  }

  Status = EFI_SUCCESS;
  if (FlashCompleteStatus->CompleteStatus == NormalFlash) {
    Status = SmmCsSvcIhisiFbtsNormalFlash ();
    IhisiErrorCodeHandler((UINT32)Status);
  }

  return IHISI_SUCCESS;
}

/**
  AH=16h, FbtsComplete, hook SmmCsSvcIhisiFbtsPartialFlash.

  @retval EFI_SUCCESS        Function succeeded.
**/
EFI_STATUS
EFIAPI
ChipsetFbtsCompletePartialFlash (
  VOID
  )
{
  EFI_STATUS                    Status;
  FBTS_FLASH_COMPLETE_STATUS   *FlashCompleteStatus;


  *(mSmmFwBlockService->FlashMode) = SMM_FW_DEFAULT_MODE;

  FlashCompleteStatus = (FBTS_FLASH_COMPLETE_STATUS *)(UINTN) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RSI);

  if ((FlashCompleteStatus->Signature != FLASH_COMPLETE_STATUS_SIGNATURE) ||
      (FlashCompleteStatus->StructureSize != sizeof (FBTS_FLASH_COMPLETE_STATUS))) {
    return IHISI_INVALID_PARAMETER;
  }

  Status = EFI_SUCCESS;
  if (FlashCompleteStatus->CompleteStatus == PartialFlash) {
      Status = SmmCsSvcIhisiFbtsPartialFlash ();
      IhisiErrorCodeHandler((UINT32)Status);
  }

  return IHISI_SUCCESS;
}

/**
  AH=16h, FbtsComplete, hook SmmCsSvcIhisiFbtsOemComplete.

  @retval EFI_SUCCESS        Function succeeded.
**/
EFI_STATUS
EFIAPI
ChipsetFbtsComplete (
  VOID
  )
{
  EFI_STATUS                    Status;
  UINT8                         ApRequest;

  ApRequest = (UINT8) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RCX);

  if (ApRequest > (UINT8) FlashCompleteS3) {
    return IHISI_INVALID_PARAMETER;
  }

  Status = SmmCsSvcIhisiFbtsOemComplete(ApRequest);
  if (Status == EFI_SUCCESS) {
    return IHISI_END_FUNCTION_CHAIN;
  }
  return IHISI_SUCCESS;
}

/**
  AH=16h, FbtsComplete, hook SmmCsSvcIhisiFbtsShutDown/SmmCsSvcIhisiFbtsReboot/SmmCsSvcIhisiFbtsApRequestDoNothing.

  @retval EFI_SUCCESS        Function succeeded.
**/
EFI_STATUS
EFIAPI
ChipsetFbtsReboot (
  VOID
  )
{
  EFI_STATUS                    Status;
  UINT8                         ApRequest;

  ApRequest = (UINT8) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RCX);

  if (ApRequest > (UINT8) FlashCompleteS3) {
    return IHISI_INVALID_PARAMETER;
  }
  Status = EFI_SUCCESS;
  if (ApRequest == FlashCompleteReboot) {
    Status = SmmCsSvcIhisiFbtsReboot();
  }
  return Status;
}
/**
  AH=16h, FbtsComplete, hook SmmCsSvcIhisiFbtsShutDown.

  @retval EFI_SUCCESS        Function succeeded.
**/
EFI_STATUS
EFIAPI
ChipsetFbtsShutDown (
  VOID
  )
{
  EFI_STATUS                    Status;
  UINT8                         ApRequest;

  ApRequest = (UINT8) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RCX);
  if (ApRequest > (UINT8) FlashCompleteS3) {
    return IHISI_INVALID_PARAMETER;
  }

  Status = EFI_SUCCESS;
  if (ApRequest == FlashCompleteShutdown) {
    Status = SmmCsSvcIhisiFbtsShutDown();
  }

  return Status;
}
/**
  AH=16h, FbtsComplete, hook SmmCsSvcIhisiFbtsApRequestDoNothing.

  @retval EFI_SUCCESS        Function succeeded.
**/
EFI_STATUS
EFIAPI
ChipsetFbtsDoNothing (
  VOID
  )
{
  EFI_STATUS                    Status;
  UINT8                         ApRequest;

  ApRequest = (UINT8) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RCX);
  if (ApRequest > (UINT8) FlashCompleteS3) {
    return IHISI_INVALID_PARAMETER;
  }

  Status = EFI_SUCCESS;
  if (ApRequest == FlashCompleteDoNothing) {
    Status = SmmCsSvcIhisiFbtsApRequestDoNothing();
  }
  return Status;
}

/**
  AH=12h, Get Platform ROM map protection, hook SmmCsSvcIhisiFbtsGetOemFlashMap

  @retval EFI_SUCCESS        Get Platform ROM map protection successful.
**/

EFI_STATUS
EFIAPI
ChipsetFbtsGetPlatformRomMap (
  VOID
  )
{
  EFI_STATUS                               Status;
  UINTN                                    Index;
  FBTS_PLATFORM_ROM_MAP                    *OemRomMapBuffer;
  FBTS_PLATFORM_PRIVATE_ROM                *OemPrivateRomMap;
  FBTS_PLATFORM_ROM_MAP                    *OemRomMapTempBuffer;
  FBTS_PLATFORM_PRIVATE_ROM                *OemPrivateRomMapTempBuffer;
  FBTS_PLATFORM_ROM_MAP_BUFFER             *InputRomMapBuffer;
  FBTS_PLATFORM_PRIVATE_ROM_BUFFER         *InputPrivateRomMapBuffer;

  InputRomMapBuffer = (FBTS_PLATFORM_ROM_MAP_BUFFER *)(UINTN) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RDI);
  InputPrivateRomMapBuffer = (FBTS_PLATFORM_PRIVATE_ROM_BUFFER *)(UINTN) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RSI);
  //
  // Update Chipset Flash Map
  //
  OemRomMapBuffer = AllocatePool (sizeof(FBTS_PLATFORM_ROM_MAP_BUFFER));
  if (OemRomMapBuffer == NULL) {
    return IHISI_OB_LEN_TOO_SMALL;
  }

  OemPrivateRomMap = AllocatePool (sizeof (FBTS_PLATFORM_PRIVATE_ROM_BUFFER));
  if (OemPrivateRomMap == NULL) {
    return IHISI_OB_LEN_TOO_SMALL;
  }

  //
  // Record currently allocate buffer
  //
  OemRomMapTempBuffer        = OemRomMapBuffer;
  OemPrivateRomMapTempBuffer = OemPrivateRomMap;

  Status = SmmCsSvcIhisiFbtsGetOemFlashMap (&OemRomMapBuffer, &OemPrivateRomMap);
  if (Status == EFI_SUCCESS) {
    if (OemPrivateRomMap != NULL) {
      for (Index = 0; Index < sizeof(FBTS_PLATFORM_PRIVATE_ROM_BUFFER)/sizeof(FBTS_PLATFORM_PRIVATE_ROM); Index++ ) {
        Status = FillPlatformPrivateRomMapBuffer (
                   OemPrivateRomMap[Index].LinearAddress,
                   OemPrivateRomMap[Index].Size,
                   (UINT8) Index
                   );
        if (EFI_ERROR(Status)) {
          FillPlatformPrivateRomMapBuffer ((UINT8) FbtsRomMapEos, 0, 0);
          break;
        }
        if (OemPrivateRomMap[Index].LinearAddress == (UINT32)FbtsRomMapEos) {
          break;
        }
      }
    }

    if (OemRomMapBuffer != NULL) {
      for (Index = 0 ; Index < sizeof(FBTS_PLATFORM_ROM_MAP_BUFFER)/sizeof(FBTS_PLATFORM_ROM_MAP); Index++) {
        Status = FillPlatformRomMapBuffer (
                   OemRomMapBuffer[Index].Type,
                   OemRomMapBuffer[Index].Address,
                   OemRomMapBuffer[Index].Length,
                   (UINT8) Index
                   );
        if (EFI_ERROR(Status)) {
          FillPlatformRomMapBuffer ((UINT8) FbtsRomMapEos, 0, 0, 0);
          break;
        }
        if (OemRomMapBuffer[Index].Type == FbtsRomMapEos) {
          break;
        }
      }
    }

    //
    // Update protect ROM map
    //
    CopyMem (InputRomMapBuffer, &mRomMapBuffer, sizeof (FBTS_PLATFORM_ROM_MAP_BUFFER));

    //
    // Update private ROM map
    //
    CopyMem (InputPrivateRomMapBuffer, &mPrivateRomMapBuffer, sizeof (FBTS_PLATFORM_PRIVATE_ROM_BUFFER));
  }

  //
  // Compare the buffer address same with allocate address
  //
  if (OemRomMapTempBuffer == OemRomMapBuffer) {
    FreePool (OemRomMapBuffer);
  }

  if (OemPrivateRomMapTempBuffer == OemPrivateRomMap) {
    FreePool (OemPrivateRomMap);
  }

  return IHISI_SUCCESS;
}

/**
  AH=10h, Get IHISI version/Battery low bound/VendorId.

  @retval EFI_SUCCESS        Command successful returned.
**/
EFI_STATUS
EFIAPI
KernelFbtsGetVersion (
  VOID
  )
{
  UINT32                            Ecx;

  //
  //Get IHISI version
  //
  Ecx = IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RCX);
  Ecx = (UINT32)((Ecx & 0xffff0000) | PcdGet16 (PcdIhisiFbtsVersion));
  IhisiProtWriteCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RCX, Ecx);

  return IHISI_SUCCESS;
}

/**
  AH=10h, Init OemHelp1/2.

  @retval EFI_SUCCESS        Command successful returned.
**/
EFI_STATUS
EFIAPI
KernelFbtsInitOemHelp (
  VOID
  )
{
  FBTS_PLATFORM_STATUS_BUFFER      *PlatformInfoPtr;
  UINTN                             PlatformInfoSize;
  //
  //Init OemHelp1/OemHelp2
  //
  PlatformInfoPtr = (FBTS_PLATFORM_STATUS_BUFFER *)(UINTN) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RDI);
  PlatformInfoSize = EXTEND_OFFSET (FBTS_PLATFORM_STATUS_BUFFER, Signature);


  PlatformInfoPtr->OemHelp1.Data = 0;
  PlatformInfoPtr->OemHelp2.Data = 0;
  return IHISI_SUCCESS;

}

/**
  AH=10h, Get IHISI VendorId.

  @retval EFI_SUCCESS        Command successful returned.
**/
EFI_STATUS
EFIAPI
KernelFbtsGetVendorId (
  VOID
  )
{
  FBTS_PLATFORM_STATUS_BUFFER      *PlatformInfoPtr;
  UINTN                             PlatformInfoSize;

  PlatformInfoPtr = (FBTS_PLATFORM_STATUS_BUFFER *)(UINTN) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RDI);
  PlatformInfoSize = EXTEND_OFFSET (FBTS_PLATFORM_STATUS_BUFFER, Signature);


  PlatformInfoPtr->Customer = PcdGet16 (PcdIhisiFbtsVendorId);

  return IHISI_SUCCESS;
}

/**
  AH=10h, Get Battery low bound.

  @retval EFI_SUCCESS        Command successful returned.
**/
EFI_STATUS
EFIAPI
KernelFbtsGetBatteryLoawBound (
  VOID
  )
{
  FBTS_PLATFORM_STATUS_BUFFER      *PlatformInfoPtr;
  UINTN                             PlatformInfoSize;

  PlatformInfoPtr = (FBTS_PLATFORM_STATUS_BUFFER *)(UINTN) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RDI);
  PlatformInfoSize = EXTEND_OFFSET (FBTS_PLATFORM_STATUS_BUFFER, Signature);


  PlatformInfoPtr->Bound    = PcdGet8 (PcdIhisiFbtsBatteryLowBound);

  return IHISI_SUCCESS;
}

/**
  AH=11h, Get platform information. Get Model Name.

  @retval EFI_SUCCESS        Get platform information successful.
  @return Other              Get platform information failed.
**/
EFI_STATUS
EFIAPI
KernelFbtsGetModelName (
  VOID
  )
{
  EFI_STATUS                                Status;
  UINTN                                     StrLen;
  FBTS_PLATFORM_INFO_BUFFER                *PlatformInfoPtr;

  //
  // DS:EDI - Pointer to platform information structure as below.
  //
  //  Offset | Size | Item          | Description
  // --------|------|---------------|--------------
  //   00h   | 40h  | Model Name    | Unicode string, end with '00h'.
  //   40h   | FFh  | Model Version | Unicode string, end with '00h'.
  //
  PlatformInfoPtr = (FBTS_PLATFORM_INFO_BUFFER *)(UINTN) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RDI);

  StrLen = MODEL_NAME_SIZE;
  ZeroMem (PlatformInfoPtr->ModelName, StrLen * sizeof (CHAR16));

  //
  //Update Model name
  //
  Status  = GetBvdtInfo ((BVDT_TYPE) BvdtProductName, &StrLen, PlatformInfoPtr->ModelName);
  if (EFI_ERROR (Status)) {
    return IHISI_FBTS_UNKNOWN_PLATFORM_INFO;
  }
  return IHISI_SUCCESS;
  }

/**
  AH=11h, Get platform information. Get Model Version

  @retval EFI_SUCCESS        Get platform information successful.
  @return Other              Get platform information failed.
**/
EFI_STATUS
EFIAPI
KernelFbtsGetModelVersion (
  VOID
  )
{
  EFI_STATUS                                Status;
  UINTN                                     StrLen;
  FBTS_PLATFORM_INFO_BUFFER                *PlatformInfoPtr;

  //
  // DS:EDI - Pointer to platform information structure as below.
  //
  //  Offset | Size | Item          | Description
  // --------|------|---------------|--------------
  //   00h   | 40h  | Model Name    | Unicode string, end with '00h'.
  //   40h   | FFh  | Model Version | Unicode string, end with '00h'.
  //
  PlatformInfoPtr = (FBTS_PLATFORM_INFO_BUFFER *)(UINTN) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RDI);
  StrLen = MODEL_VERSION_SIZE;
  ZeroMem (PlatformInfoPtr->ModelVersion, StrLen * sizeof (CHAR16));

  //
  //Update Model version
  //
  Status  = GetBvdtInfo ((BVDT_TYPE) BvdtBiosVer, &StrLen, PlatformInfoPtr->ModelVersion);
  if (EFI_ERROR (Status)) {
    return IHISI_FBTS_UNKNOWN_PLATFORM_INFO;
  }

  return IHISI_SUCCESS;
}

/**
  AH=11h, Get platform information.Update Extend Platform

  @retval EFI_SUCCESS        Get platform information successful.
  @return Other              Get platform information failed.
**/
EFI_STATUS
EFIAPI
KernelFbtsUpdateExtendPlatform (
  VOID
  )
{
  EFI_STATUS                                Status;
  UINT8                                     Index;
  UINT32                                    BufferSize;
  UINT32                                    DataItemUseSize;
  UINT32                                    DataItemTotalSize;
  EXTEND_PLATFORM_DATA_ITEM                *DataItemPtr;
  EXTEND_PLATFORM_DATA_ITEM                *TempDataItemPtr;
  FBTS_EXTEND_PLATFORM_INFO_TABLE_OUTPUT   *ExtendPlatformInfoOutput;
  FBTS_EXTEND_PLATFORM_INFO_TABLE_INPUT    *ExternPlatformInfoInput;
  UPDATE_EXT_ITEM_FUN_TABLE                 UpdateFunTable[] = {
                                                                {BuildDateTimeID, UpdateExtendPlatformBuildDateTimeInfo}
                                                               };

  ExternPlatformInfoInput = (FBTS_EXTEND_PLATFORM_INFO_TABLE_INPUT *)(UINTN)IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RSI);

  if (ExternPlatformInfoInput->Sigature != EXTEND_PLATFORM_INPUT_BUFFER_SIGNATURE) {
    //
    // Tool do not support Extend Platform Info, no change , return success.
    //
    return IHISI_SUCCESS;
  }


  BufferSize = ExternPlatformInfoInput->StructureSize;

  ExtendPlatformInfoOutput = (FBTS_EXTEND_PLATFORM_INFO_TABLE_OUTPUT *)(UINTN)IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RSI);
  DataItemPtr = ExtendPlatformInfoOutput->DataItem;
  DataItemTotalSize = BufferSize - EXTEND_OFFSET(FBTS_EXTEND_PLATFORM_INFO_TABLE_OUTPUT, DataItem);
  TempDataItemPtr = AllocatePool (DataItemTotalSize);
  if (TempDataItemPtr == NULL) {
    return IHISI_OUT_OF_RESOURCES;
  }
  ExtendPlatformInfoOutput->DataItemCount = 0;

  for (Index = 0, DataItemUseSize = 0; Index < sizeof(UpdateFunTable)/sizeof(UpdateFunTable[0]); Index++) {
    ZeroMem (TempDataItemPtr, DataItemTotalSize);
    TempDataItemPtr->DataID = UpdateFunTable[Index].DataID;
    //
    //DataSize :Indicates how many available data size in Data field (offset 05h). Not include Data_ID (offset 00h) and Data_Size (offset 01h) fields.
    //
    TempDataItemPtr->DataSize = DataItemTotalSize - EXTEND_OFFSET(EXTEND_PLATFORM_DATA_ITEM, Data);
    Status = UpdateFunTable[Index].UpdateExtItemFun(TempDataItemPtr);

    if (Status == EFI_SUCCESS || Status == EFI_BUFFER_TOO_SMALL) {
      //
      //Add each function dataitem use size to DataItemUseSize
      //
      ExtendPlatformInfoOutput->Sigature = EXTEND_PLATFORM_OUTPUT_BUFFER_SIGNATURE;
      DataItemUseSize += TempDataItemPtr->DataSize + EXTEND_OFFSET(EXTEND_PLATFORM_DATA_ITEM, Data);
      if (DataItemUseSize < DataItemTotalSize) {
        //
        //Copy and point to next DataItem start address.
        //
        ExtendPlatformInfoOutput->DataItemCount ++;
        CopyMem (DataItemPtr, TempDataItemPtr, TempDataItemPtr->DataSize + EXTEND_OFFSET(EXTEND_PLATFORM_DATA_ITEM, Data));
        DataItemPtr = (EXTEND_PLATFORM_DATA_ITEM *)((UINT8 *)DataItemPtr + TempDataItemPtr->DataSize + EXTEND_OFFSET(EXTEND_PLATFORM_DATA_ITEM, Data));
      }
    }
  }
  FreePool (TempDataItemPtr);
  //
  //if DataItemUseSize over AP provide buffer size(DataItemTotalSize),restore sigature and return buffer to small.
  //
  if (DataItemUseSize > DataItemTotalSize) {
    ExtendPlatformInfoOutput->DataItemCount = DataItemUseSize;
    return IHISI_OB_LEN_TOO_SMALL;
  }
  return IHISI_SUCCESS;
}

/**
  AH=12h, Get Platform ROM map protection.

  @retval EFI_SUCCESS        Get Platform ROM map protection successful.
**/
EFI_STATUS
EFIAPI
KernelFbtsUpdateProtectRomMap (
  VOID
  )
{
  FBTS_PLATFORM_ROM_MAP_BUFFER             *InputRomMapBuffer;

  //
  // Get ROM map protection structure.
  //
  InputRomMapBuffer = (FBTS_PLATFORM_ROM_MAP_BUFFER *)(UINTN) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RDI);

  //
  // Update protect ROM map
  //
  CopyMem (InputRomMapBuffer, &mRomMapBuffer, sizeof (FBTS_PLATFORM_ROM_MAP_BUFFER));

  return IHISI_SUCCESS;
}

/**
  AH=12h, Get Platform ROM map protection.

  @retval EFI_SUCCESS        Get Platform ROM map protection successful.
**/
EFI_STATUS
EFIAPI
KernelFbtsUpadtePrivateRomMap (
  VOID
  )
{
  FBTS_PLATFORM_PRIVATE_ROM_BUFFER         *InputPrivateRomMapBuffer;

  //
  // Get ROM map protection structure.
  //
  InputPrivateRomMapBuffer = (FBTS_PLATFORM_PRIVATE_ROM_BUFFER *)(UINTN) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RSI);

  //
  // Update private ROM map
  //
  CopyMem (InputPrivateRomMapBuffer, &mPrivateRomMapBuffer, sizeof (FBTS_PLATFORM_PRIVATE_ROM_BUFFER));

  return IHISI_SUCCESS;
}

/**
   AH=13h, Get Flash part information.

  @retval EFI_SUCCESS        Get Flash part information successful.
  @return Other              Get Flash part information failed.
**/
EFI_STATUS
EFIAPI
KernelFbtsGetFlashPartInfo (
  VOID
  )
{
  FLASH_DEVICE                              *Buffer;
  EFI_STATUS                                Status;
  FBTS_FLASH_DEVICE                         FlashDevice;
  UINT16                                    BlockMap[3];
  UINT8                                     SpiFlashNumber;
  UINT8                                     *FlashInfo;
  UINTN                                     FlashInfoSize;
  UINT8                                     *FlashBlockMap;

  //
  // DS:EDI - Pointer to flash part information structure.
  //
  FlashInfo     = (UINT8 *) (UINTN) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RDI);
  FlashInfoSize = sizeof(FBTS_FLASH_DEVICE);

  //
  // DS:ESI - Pointer to flash part block map structure.
  //
  FlashBlockMap     = (UINT8 *) (UINTN) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RSI);

  //
  // CL = 00h => default, no choice from AP.
  //      01h => AP need flash SPI flash part.
  //      02h => AP need flash non-SPI flash part (LPC, FWH).
  //
  Buffer = AllocatePool (sizeof (FLASH_DEVICE));
  if (Buffer == NULL) {
    return IHISI_OUT_OF_RESOURCES;
  }

  Status = mSmmFwBlockService->DetectDevice (
                                 mSmmFwBlockService,
                                 (UINT8*)Buffer
                                 );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  ZeroMem (&FlashDevice, sizeof (FBTS_FLASH_DEVICE));
  FlashDevice.Id = (UINT32)Buffer->DeviceInfo.Id;
  FlashDevice.Size = Buffer->DeviceInfo.Size;
  FlashDevice.SpecifiedSize = 0;
  AsciiStrnCpyS (FlashDevice.VendorName, 31, Buffer->DeviceInfo.VendorName, 31);
  AsciiStrnCpyS (FlashDevice.DeviceName, 32, Buffer->DeviceInfo.DeviceName, 32);
  CopyMem ((VOID *) BlockMap, &(Buffer->DeviceInfo.BlockMap), sizeof (FD_BLOCK_MAP));
  if (FlashDevice.Size == 0xFF) {
    //
    // The BlockSize unit is 256(0x100) byte.
    //
    FlashDevice.SpecifiedSize = (Buffer->DeviceInfo.BlockMap.Mutiple * Buffer->DeviceInfo.BlockMap.BlockSize) * 0x100;
  } else {
    Status = mSmmFwBlockService->GetSpiFlashNumber (
                                   mSmmFwBlockService,
                                   &SpiFlashNumber
                                   );

    if (!EFI_ERROR (Status)) {
      FlashDevice.Size += SpiFlashNumber - 1;
      BlockMap[1] *= 1 << (SpiFlashNumber - 1);
    }
  }

  CopyMem (
    (VOID *) FlashInfo,
    &FlashDevice,
    FlashInfoSize
    );

  CopyMem ((VOID *) FlashBlockMap, BlockMap, sizeof (FD_BLOCK_MAP));

  FreePool (Buffer);
  return IHISI_SUCCESS;
}

/**
  AH=1Bh,Skip module check allows and binary file transmissions.

  @retval EFI_SUCCESS   Success returns.
**/
EFI_STATUS
EFIAPI
KernelSkipMcCheckAndBinaryTrans (
  VOID
  )
{
  //
  // return IHISI_SUCCESS to allow skip module check. If doesn't allow
  // skip module check, please return IHISI_FBTS_CANNOT_SKIP_MODULE_CHECK (0x27)
  //
  return IHISI_SUCCESS;
}

/**
  Get default BIOS ROM map

  @param[out] BiosRomMap              Pointer to the returned (FBTS_INTERNAL_BIOS_ROM_MAP *) data
  @param[out] NumberOfRegions         The total number of regions in BiosRomMap

  @retval EFI_SUCCESS                 FBTS get BIOS ROM map success.
  @return Others                      FBTS get BIOS ROM map failed.
**/
EFI_STATUS
GetDefaultBiosRomMap (
  OUT FBTS_INTERNAL_BIOS_ROM_MAP    **BiosRomMap,
  OUT UINTN                          *NumberOfRegions
  )
{
  UINTN     Index;
  extern    FBTS_INTERNAL_BIOS_ROM_MAP mDefaultBiosRomMap[];
  UINT32    NvStorageRegionSize;
  UINTN     Conuter;

  if ((BiosRomMap == NULL) || (NumberOfRegions == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

#define ADD_ROM_MAP_ENTRY(RegionType, RegionAddress, RegionSize, RegionAttr)        \
            mDefaultBiosRomMap[Index].Type = (UINT8)RegionType;                     \
            mDefaultBiosRomMap[Index].Address = RegionAddress;                      \
            mDefaultBiosRomMap[Index].Size = RegionSize;                            \
            mDefaultBiosRomMap[Index].Attribute = RegionAttr;                       \
            Index++;                                                                \
            Conuter = __COUNTER__;

#define ADD_ROM_MAP_ENTRY_FROM_PCD(RegionType, AddressPcd, SizePcd, RegionAttr)                 \
          if (PcdGet32(SizePcd) > 0) {                                                          \
            ADD_ROM_MAP_ENTRY(RegionType, PcdGet32(AddressPcd), PcdGet32(SizePcd), RegionAttr); \
          }


#define ADD_ROM_MAP_ENTRY_FROM_FDM(RegionType, FdmRegionType, Instance, RegionAttr)  \
          {                                                                          \
            UINT64  Size;                                                            \
            if ((Size = FdmGetNAtSize(&FdmRegionType, Instance)) > 0) {              \
              ADD_ROM_MAP_ENTRY(RegionType, (UINT32) FdmGetNAtAddr(&FdmRegionType, Instance), (UINT32) Size, RegionAttr); \
            }  \
          }

#define ADD_DXE_FV_ROM_ENTRY_FROM_FDM(RegionAttr)  \
          {                                                    \
            UINT64  Size;                                      \
            if ((Size = FdmGetSizeById (&gH2OFlashMapRegionFvGuid, &gH2OFlashMapRegionDxeFvGuid, 1)) > 0) {  \
              ADD_ROM_MAP_ENTRY(FbtsRomMapDxe, (UINT32) FdmGetAddressById(&gH2OFlashMapRegionFvGuid, &gH2OFlashMapRegionDxeFvGuid, 1), (UINT32) Size, RegionAttr); \
            }  \
          }

#define ADD_PEI_FV_ROM_ENTRY_FROM_FDM(RegionAttr)  \
          {                                                    \
            UINT64  Size;                                      \
            if ((Size = FdmGetNAtSize (&gH2OFlashMapRegionBootFvGuid, 1)) > 0) {  \
              ADD_ROM_MAP_ENTRY(FbtsRomMapPei, (UINT32) FdmGetNAtSize(&gH2OFlashMapRegionBootFvGuid, 1), (UINT32) Size, RegionAttr); \
            }  \
          }

  Index = 0;
  ADD_DXE_FV_ROM_ENTRY_FROM_FDM   (0);
  ADD_ROM_MAP_ENTRY_FROM_FDM      (FbtsRomMapCpuMicrocode, gH2OFlashMapRegionMicrocodeGuid,        1,   0);
  ADD_ROM_MAP_ENTRY_FROM_FDM      (FbtsRomMapOemData,      gH2OFlashMapRegionBvdtGuid,             1,   0);
  ADD_ROM_MAP_ENTRY_FROM_FDM      (FbtsRomMapDmiFru,       gH2OFlashMapRegionSmbiosUpdateGuid,     1,   0);
  ADD_ROM_MAP_ENTRY_FROM_FDM      (FbtsRomMapOemData,      gH2OFlashMapRegionMsdmGuid,             1,   0);
  ADD_ROM_MAP_ENTRY_FROM_FDM      (FbtsRomMapOemData,      gH2OFlashMapRegionBvdtGuid,             1,   0);
  ADD_PEI_FV_ROM_ENTRY_FROM_FDM   (0);

  NvStorageRegionSize = (UINT32) FdmGetNAtSize (&gH2OFlashMapRegionVarGuid, 1) +
                        (UINT32) FdmGetNAtSize (&gH2OFlashMapRegionFtwStateGuid,1) +
                        (UINT32) FdmGetNAtSize (&gH2OFlashMapRegionFtwBackupGuid, 1) +
                        (UINT32) FdmGetSizeById (&gH2OFlashMapRegionVarDefaultGuid, &gH2OFlashMapRegionFactoryCopyGuid, 1);
  if (NvStorageRegionSize > 0) {
    ADD_ROM_MAP_ENTRY (FbtsRomMapNvStorage, (UINT32) FdmGetNAtAddr (&gH2OFlashMapRegionVarGuid, 1), NvStorageRegionSize, 0);
  }

  //
  // End of ROM map
  //
  ADD_ROM_MAP_ENTRY (FbtsRomMapEos, 0, 0, 0);

  *BiosRomMap = (FBTS_INTERNAL_BIOS_ROM_MAP *)mDefaultBiosRomMap;
  *NumberOfRegions = Index;

  return IHISI_SUCCESS;
}
//
// the mDefaultBiosRomMap is declared after GetDefaultRomMap() on purpose to
// ensure the array size of mDefaultBiosRomMap is enough, DO NOT move it to the top
//
FBTS_INTERNAL_BIOS_ROM_MAP mDefaultBiosRomMap[__COUNTER__];

/**
  AH=1Eh, Get whole BIOS ROM map.

  @retval EFI_SUCCESS                 FBTS get BIOS ROM map success.
  @return Others                      FBTS get BIOS ROM map failed.
**/
EFI_STATUS
EFIAPI
KernelFbtsGetWholeBiosRomMap (
  VOID
  )
{
  UINTN                                 RomMapSize;
  UINTN                                 NumberOfRegions;
  UINT8                                *RomMapPtr;
  FBTS_INTERNAL_BIOS_ROM_MAP           *BiosRomMap;

  GetDefaultBiosRomMap ((FBTS_INTERNAL_BIOS_ROM_MAP **)&BiosRomMap, &NumberOfRegions);

  if (BiosRomMap[NumberOfRegions - 1].Type != FbtsRomMapEos) {
    return IHISI_FBTS_UNKNOWN_PLATFORM_ROM_MAP;
  }

  //
  // Check input address and whole input buffer isn't located in SM RAM.
  //
  RomMapPtr  = (UINT8 *) (UINTN) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RDI);
  RomMapSize = NumberOfRegions * sizeof (FBTS_INTERNAL_BIOS_ROM_MAP);
  //
  // Get ROM map protection structure.
  //
  CopyMem ((VOID *)RomMapPtr, (VOID *)BiosRomMap, RomMapSize);

  return IHISI_SUCCESS;
}

/**
  AH=14h, FBTS Read.

  @retval EFI_SUCCESS            FBTS read success.
  @return Others                 FBTS read failed.
**/
EFI_STATUS
EFIAPI
KernelFbtsRead (
  VOID
  )
{
  EFI_STATUS                      Status;
  UINTN                           Size;
  UINTN                           Offset;
  UINTN                           Address;
  UINT8                          *DataBuffer;

  //
  // ECX    - Size to read.
  // DS:ESI - Pointer to returned data buffer. Size in ECX.
  // EDI    - Target linear address to read.
  //
  DataBuffer = (UINT8 *)(UINTN) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RSI);
  Size       = (UINTN) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RCX);
  Address    = (UINTN) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RDI);
  Offset     = 0;

  Status = mSmmFwBlockService->Read (
                                 mSmmFwBlockService,
                                 Address,
                                 Offset,
                                 &Size,
                                 DataBuffer
                                 );
  if (EFI_ERROR (Status)) {
    Status = IHISI_FBTS_READ_FAILED;
  }

  IhisiErrorCodeHandler((UINT32)Status);

  return IHISI_SUCCESS;
}

/**
  AH=15h,FBTS write.

  @retval EFI_SUCCESS            FBTS write success.
  @return Others                 FBTS write failed.
**/
EFI_STATUS
EFIAPI
KernelFbtsWrite (
  VOID
  )
{
  EFI_STATUS                      Status;
  UINTN                           WriteSize;
  UINT8                           EraseCount;
  UINT8                           WriteCount;
  UINTN                           RomBaseAddress;
  BOOLEAN                         InUnsignedRegion;
  UINTN                           UnsignedRegionBase;
  UINT8                          *WriteDataBuffer;

  //
  // ECX    - Size to write.
  // DS:ESI - Pointer to returned data buffer. Size in ECX.
  // EDI    - Target linear address to write.
  //
  WriteDataBuffer = (UINT8 *)(UINTN) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RSI);
  WriteSize       = (UINTN) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RCX);
  RomBaseAddress  = (UINTN) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RDI);


  InUnsignedRegion = FALSE;
  if (PcdGetBool (PcdSecureFlashSupported)) {
    Status = mSmmFwBlockService->ConvertToSpiAddress(
                                   mSmmFwBlockService,
                                   (UINTN) FdmGetAddressById(&gH2OFlashMapRegionFvGuid, &gH2OFlashMapRegionUnsignedFvGuid, 1),
                                   &UnsignedRegionBase
                                   );
    if (!EFI_ERROR (Status)) {
      //
      // Check Write address is in the Unsigned Region or not
      //
      if ((RomBaseAddress >= UnsignedRegionBase) && \
          ((RomBaseAddress + WriteSize) <= (UnsignedRegionBase + PcdGet32 (PcdEndOfFlashUnsignedFvRegionTotalSize)))) {
        InUnsignedRegion = TRUE;
      }
    }

    if (!mInPOST && !InUnsignedRegion) {
      Status = IHISI_FBTS_WRITE_FAILED;
      goto WriteDone;
    }
  }

  EraseCount      = 0;
  WriteCount      = 0;
  do {
    Status = mSmmFwBlockService->EraseBlocks (
                                   mSmmFwBlockService,
                                   RomBaseAddress,
                                   (UINTN *) &WriteSize
                                   );
    if (!EFI_ERROR (Status)) {
      EraseCount = 0;
      Status = mSmmFwBlockService->Write (
                                     mSmmFwBlockService,
                                     RomBaseAddress,
                                     (UINTN *) &WriteSize,
                                     WriteDataBuffer
                                     );
      if (!EFI_ERROR (Status)) {
        goto WriteDone;
      } else {
        Status = IHISI_FBTS_WRITE_FAILED;
        WriteCount++;
      }
    } else {
      Status = IHISI_FBTS_ERASE_FAILED;
      EraseCount++;
    }
  } while ((EraseCount < 100) && (WriteCount < 100));

WriteDone:
  IhisiErrorCodeHandler((UINT32)Status);

  return IHISI_SUCCESS;
}

/**
  This function will delete the variable if the variable size is different between variable region and variable default
  region.

  Some variable size may be changed in different BIOS version. Using the mismatched variable after updating BIOS
  may cause unpredictable behaviors. To prevent from tis situation so delete this type variables.

  @retval EFI_SUCCESS            Delete all of variable which variable is different between variable region and variable default region.
  @retval EFI_OUT_OF_RESOURCES   There are not enough resource to store variable region data.
  @retval Other                  The request could not be completed by other reason.
**/
STATIC
EFI_STATUS
CheckVariableConsistency (
  VOID
  )
{
  EFI_STATUS                  Status;
  UINTN                       VariableStoreHeaderSize;
  UINTN                       VariableRegionSize;
  UINT8                       *VarRegionBuffer;
  VARIABLE_HEADER             *VariableHeader;
  VARIABLE_HEADER             *DefaultVariableHeader;
  VARIABLE_HEADER             *DefaultVariableHeaderbySkuId0;
  UINTN                       DefaultVariableSize;
  BOOLEAN                     FoundMismatchedVariable;

  //
  // Write back invalid related MMIO address to prevent from reading out-of-date data
  //
  WriteBackInvalidateDataCacheRange (
    (VOID *)(UINTN)FdmGetNAtAddr (&gH2OFlashMapRegionVarGuid, 1),
    (UINTN)FdmGetNAtSize (&gH2OFlashMapRegionVarGuid, 1)
    );
  WriteBackInvalidateDataCacheRange (
    (VOID *)(UINTN) FdmGetNAtAddr (&gH2OFlashMapRegionVarDefaultGuid, 1),
    (UINTN)FdmGetNAtSize (&gH2OFlashMapRegionVarDefaultGuid, 1)
    );
  //
  // Get whole variable region and put in memory buffer.
  //
  VariableRegionSize = (UINTN)FdmGetNAtSize (&gH2OFlashMapRegionVarGuid, 1);
  VarRegionBuffer    = AllocatePool (VariableRegionSize);
  if (VarRegionBuffer == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  Status = mSmmFwBlockService->Read (
                                 mSmmFwBlockService,
                                 (UINTN)FdmGetNAtAddr (&gH2OFlashMapRegionVarGuid, 1),
                                 0,
                                 &VariableRegionSize,
                                 VarRegionBuffer
                                 );
  if (EFI_ERROR(Status)) {
    FreePool (VarRegionBuffer);
    return Status;
  }
  //
  // Get each variable in default variable region. If default variable isn't in current SKU, use it in SKU 0.
  //
  VariableStoreHeaderSize       = sizeof (EFI_FIRMWARE_VOLUME_HEADER) + sizeof (EFI_FV_BLOCK_MAP_ENTRY) + GetVariableStoreHeaderSize ();
  DefaultVariableHeaderbySkuId0 = (VARIABLE_HEADER *)(UINTN)FdmGetNAtAddr (&gH2OFlashMapRegionVarDefaultGuid, 1);
  FoundMismatchedVariable       = FALSE;
  for (DefaultVariableHeader = (VARIABLE_HEADER *)((UINT8 *)DefaultVariableHeaderbySkuId0 + GetVariableStoreHeaderSize ());
       IsValidVariableHeader (DefaultVariableHeader);
       DefaultVariableHeader = GetNextVariablePtr (DefaultVariableHeader)) {
    if (DefaultVariableHeader->State != VAR_ADDED) {
      continue;
    }
    DefaultVariableSize = 0;
    Status = CommonGetDefaultVariable (
                GET_VARIABLE_NAME_PTR (DefaultVariableHeader),
                &DefaultVariableHeader->VendorGuid,
                (UINT8)(LibPcdGetSku()),
                NULL,
                (UINTN *)&DefaultVariableSize,
                NULL
                );
    if (Status != EFI_BUFFER_TOO_SMALL) {
      DefaultVariableSize = DefaultVariableHeader->DataSize;
    }
    //
    // Delete variable if the variable size is different between variable region and variable default region.
    //
    for (VariableHeader = (VARIABLE_HEADER *)(VarRegionBuffer + VariableStoreHeaderSize);
        IsValidVariableHeader (VariableHeader);
        VariableHeader = GetNextVariablePtr (VariableHeader)) {
      if (VariableHeader->State != VAR_ADDED && VariableHeader->State != (VAR_ADDED & VAR_IN_DELETED_TRANSITION)) {
        continue;
      }
      if (StrCmp (GET_VARIABLE_NAME_PTR (VariableHeader), GET_VARIABLE_NAME_PTR (DefaultVariableHeader)) != 0 ||
          !CompareGuid(&VariableHeader->VendorGuid, &DefaultVariableHeader->VendorGuid)) {
        continue;
      }
      if (DefaultVariableSize != DataSizeOfVariable (VariableHeader)) {
        VariableHeader->State &= VAR_DELETED;
        FoundMismatchedVariable = TRUE;
      }
    }
  }
  //
  // Write updated variable region data to variable region if finding any mismatched variable.
  //
  Status = EFI_SUCCESS;
  if (FoundMismatchedVariable) {
    Status = mSmmFwBlockService->Write (
                                   mSmmFwBlockService,
                                   (UINTN)FdmGetNAtAddr (&gH2OFlashMapRegionVarGuid, 1),
                                   &VariableRegionSize,
                                   VarRegionBuffer
                                   );
  }
  FreePool (VarRegionBuffer);

  return Status;
}

/**
  AH=16h, This function uses to execute some specific action after the flash process is completed.

  @retval EFI_SUCCESS        Function succeeded.
  @return Other              Error occurred in this function.
**/
EFI_STATUS
EFIAPI
KernelFbtsPurifyVariable (
  VOID
  )
{
  UINT32                        Status;
  FBTS_FLASH_COMPLETE_STATUS   *FlashCompleteStatus;

  *(mSmmFwBlockService->FlashMode) = SMM_FW_DEFAULT_MODE;

  // Only execute kernel purify variable in NormalFlash and chipset SmmCsSvcIhisiFbtsNormalFlash not Succes.
  FlashCompleteStatus = (FBTS_FLASH_COMPLETE_STATUS *)(UINTN) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RSI);

  if ((FlashCompleteStatus->Signature != FLASH_COMPLETE_STATUS_SIGNATURE) ||
      (FlashCompleteStatus->StructureSize != sizeof (FBTS_FLASH_COMPLETE_STATUS))) {
    return IHISI_INVALID_PARAMETER;
  }

  if (!mPurifyVariable) {
    return IHISI_SUCCESS;
  }
  Status = IHISI_SUCCESS;
  if (FlashCompleteStatus->CompleteStatus == NormalFlash) {
    Status = IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RAX) & 0xff;
    if (Status != IHISI_SUCCESS) { // Execute PurifyVariable when SmmCsSvcIhisiFbtsNormalFlash not success;
      Status = PurifyVariable();
      CheckVariableConsistency ();
    }
  }

  return Status;
}

EFI_STATUS
EFIAPI
SmmFwBlockNotification (
  IN     CONST EFI_GUID                *Protocol,
  IN     VOID                          *Interface,
  IN     EFI_HANDLE                     Handle
  )
{
  EFI_STATUS            Status;

  Status = gSmst->SmmLocateProtocol (
                  &gEfiSmmFwBlockServiceProtocolGuid,
                  NULL,
                  (VOID **) &mSmmFwBlockService
                  );

  if (EFI_ERROR(Status)) {
    ASSERT_EFI_ERROR (Status);
  }
  UpdateOemFlashMap();
  return EFI_SUCCESS;
}

EFI_STATUS
UpdateOemFlashMap (
  VOID
  )
{
  EFI_STATUS                    Status;
  VOID                          *Registration;

  if (mSmmFwBlockService == NULL) {
    //
    // If FwBlockService is not installed, register Protocol notification to update Oem Flash map
    //
    Status = gSmst->SmmLocateProtocol (
                      &gEfiSmmFwBlockServiceProtocolGuid,
                      NULL,
                      (VOID **) &mSmmFwBlockService
                      );

    if (EFI_ERROR (Status)) {
      Status = gSmst->SmmRegisterProtocolNotify (
                        &gEfiSmmFwBlockServiceProtocolGuid,
                        SmmFwBlockNotification,
                        &Registration
                        );
      ASSERT_EFI_ERROR (Status);
    }
    return EFI_SUCCESS;
  }

  //
  //Update Oem Flash Map
  //
  ZeroMem ((VOID *)&mRomMapBuffer, sizeof (mRomMapBuffer));
  FillPlatformRomMapBuffer ((UINT8) FbtsRomMapEos, 0, 0, 0);
  ZeroMem ((VOID *)&mPrivateRomMapBuffer, sizeof (mPrivateRomMapBuffer));
  FillPlatformPrivateRomMapBuffer ((UINT8) FbtsRomMapEos, 0, 0);

  //Default mRomMapBuffer from GetFlashMapByHob
  Status = GetFlashMapByHob ();

  return Status;
}

EFI_STATUS
InstallFbtsServices (
  VOID
  )
{
  EFI_STATUS              Status;
  IHISI_REGISTER_TABLE   *SubFuncTable;
  UINT16                  TableCount;

  //
  //Update Oem Flash Map
  //
  Status = UpdateOemFlashMap ();
  if (EFI_ERROR(Status)) {
    ASSERT_EFI_ERROR (Status);
  }

  SubFuncTable = FBTS_REGISTER_TABLE;
  TableCount = sizeof(FBTS_REGISTER_TABLE)/sizeof(FBTS_REGISTER_TABLE[0]);
  Status = RegisterIhisiSubFunction (SubFuncTable, TableCount);
  if (EFI_ERROR(Status)) {
    ASSERT_EFI_ERROR (Status);
  }

  return EFI_SUCCESS;
}

