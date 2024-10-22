/** @file
  System informaiton relative code

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

#include "SystemInformation.h"
#include <IndustryStandard/SmBios.h>
#include <Protocol/Smbios.h>
#include <Library/MultiConfigBaseLib.h>
#include <Library/VariableSupportLib.h>
#include <Library/BaseSetupDefaultLib.h>


/**
  Acquire the string associated with the Index from smbios structure and return it.
  The caller is responsible for free the string buffer.

  @param    OptionalStrStart  The start position to search the string
  @param    Index             The index of the string to extract
  @param    String            The string that is extracted

  @retval   EFI_SUCCESS            Get index string successfully.
  @retval   EFI_INVALID_PARAMETER  Index is zero. It is invalid value.
  @retval   EFI_ABORTED            Get missing string fail .
  @retval   EFI_OUT_OF_RESOURCES   Allocate memory fail.
  @retval   Other                  Get setup browser data fail.

**/
EFI_STATUS
GetOptionalStringByIndex (
  IN  CHAR8                        *OptionalStrStart,
  IN  UINT8                        Index,
  OUT CHAR16                       **String
  )
{
  UINTN                                 StrSize;
  SETUP_UTILITY_CONFIGURATION           *SUCInfo;
  SETUP_UTILITY_BROWSER_DATA            *SuBrowser;
  EFI_STATUS                            Status;

  if (Index == 0) {
    return EFI_INVALID_PARAMETER;
  }

  StrSize = 0;
  do {
    Index--;
    OptionalStrStart += StrSize;
    StrSize           = AsciiStrSize (OptionalStrStart);
  } while (OptionalStrStart[StrSize] != 0 && Index != 0);

  if ((Index != 0) || (StrSize == 1)) {
    //
    // Meet the end of strings set but Index is non-zero, or
    // Find an empty string
    //
    Status = GetSetupUtilityBrowserData (&SuBrowser);
    if (EFI_ERROR (Status)) {
      return Status;
    }

    SUCInfo = SuBrowser->SUCInfo;
    *String = HiiGetString (SUCInfo->MapTable[MainHiiHandle].HiiHandle, STRING_TOKEN (STR_MISSING_STRING), NULL);
     if (*String == NULL) {
       return EFI_ABORTED;
     }
  } else {
    *String = AllocatePool (StrSize * sizeof (CHAR16));
     if (*String == NULL) {
       return EFI_OUT_OF_RESOURCES;
     }
    AsciiStrToUnicodeStrS (OptionalStrStart, *String, StrSize);
  }

  return EFI_SUCCESS;
}

/**
  Convert Cache Size to a string

  @param SmbiosType7             Pointer to smbios table type 7
  @param String                  The string that is created

  @retval EFI_SUCCESS            convert cache size value to string successful.

**/
STATIC
EFI_STATUS
ConvertCacheSizeToString (
  IN  SMBIOS_TABLE_TYPE7                *SmbiosType7,
  OUT CHAR16                            **String
  )
{
  CHAR16        *StringBuffer;

  StringBuffer = AllocateZeroPool (0x20);
  if (StringBuffer == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  if (SmbiosType7->InstalledSize & 0x8000) {
    UnicodeValueToStringS (StringBuffer, 0x20, 0, ((SmbiosType7->InstalledSize & ~0x8000) >> 4), 0);
    StrCatS (StringBuffer, 0x20 / sizeof(CHAR16), L" MB");
  } else {
    UnicodeValueToStringS (StringBuffer, 0x20, 0, SmbiosType7->InstalledSize, 0);
    StrCatS (StringBuffer, 0x20 / sizeof(CHAR16), L" KB");
  }

  *String = StringBuffer;

  return EFI_SUCCESS;
}

/**
  Update CPU Type information in VFR

  @param  HiiHandle              Input Hii handle

  @retval EFI_SUCCESS            update successfully.

**/
STATIC
EFI_STATUS
CreateCpuTypeInfo (
  IN  EFI_HII_HANDLE                        HiiHandle
  )
{
  VOID                                  *StartOpCodeHandle;
  EFI_IFR_GUID_LABEL                    *StartLabel;
  EFI_STATUS                            Status;

  StartOpCodeHandle = HiiAllocateOpCodeHandle ();
  ASSERT (StartOpCodeHandle != NULL);

  StartLabel               = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode (StartOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
  StartLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  StartLabel->Number       = UPDATE_CPU_TYPE_LABEL;

  HiiCreateTextOpCode (
    StartOpCodeHandle,
    STRING_TOKEN(STR_CPU_TYPE_STRING),
    0,
    STRING_TOKEN(STR_CPU_TYPE_STRING2)
    );

  Status = HiiUpdateForm (
             HiiHandle,
             NULL,
             ROOT_FORM_ID,
             StartOpCodeHandle,
             NULL
             );

  HiiFreeOpCodeHandle (StartOpCodeHandle);

  return EFI_SUCCESS;
}

/**
  Update System Bus Speed information in VFR

  @param  HiiHandle              Input Hii handle

  @retval EFI_SUCCESS            update successfully.

**/
STATIC
EFI_STATUS
CreateSystemBusSpeedInfo (
  IN  EFI_HII_HANDLE                        HiiHandle
  )
{
  VOID                                  *StartOpCodeHandle;
  EFI_IFR_GUID_LABEL                    *StartLabel;
  EFI_STATUS                            Status;

  StartOpCodeHandle = HiiAllocateOpCodeHandle ();
  ASSERT (StartOpCodeHandle != NULL);

  StartLabel               = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode (StartOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
  StartLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  StartLabel->Number       = UPDATE_SYSTEM_BUS_SPEED_LABEL;

  HiiCreateTextOpCode (
    StartOpCodeHandle,
    STRING_TOKEN(STR_SYSTEM_BUS_SPEED_STRING),
    0,
    STRING_TOKEN(STR_SYSTEM_BUS_SPEED_STRING2)
    );

  Status = HiiUpdateForm (
             HiiHandle,
             NULL,
             ROOT_FORM_ID,
             StartOpCodeHandle,
             NULL
             );

  HiiFreeOpCodeHandle (StartOpCodeHandle);

  return EFI_SUCCESS;
}

/**
  Update Cache RAM information in VFR

  @param  HiiHandle              Input Hii handle

  @retval EFI_SUCCESS            update successfully.

**/
STATIC
EFI_STATUS
CreateCacheRamInfo (
  IN  EFI_HII_HANDLE                        HiiHandle
  )
{
  VOID                                  *StartOpCodeHandle;
  EFI_IFR_GUID_LABEL                    *StartLabel;
  EFI_STATUS                            Status;

  StartOpCodeHandle = HiiAllocateOpCodeHandle ();
  ASSERT (StartOpCodeHandle != NULL);

  StartLabel               = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode (StartOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
  StartLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  StartLabel->Number       = UPDATE_CACHE_RAM_LABEL;

  HiiCreateTextOpCode (
    StartOpCodeHandle,
    STRING_TOKEN(STR_CACHE_RAM_STRING),
    0,
    STRING_TOKEN(STR_CACHE_RAM_STRING2)
    );

  Status = HiiUpdateForm (
             HiiHandle,
             NULL,
             ROOT_FORM_ID,
             StartOpCodeHandle,
             NULL
             );

  HiiFreeOpCodeHandle (StartOpCodeHandle);

  return EFI_SUCCESS;
}

/**
  Update Config Data ID in VFR

  @param  HiiHandle              Input Hii handle

  @retval EFI_SUCCESS            update successfully.

**/
STATIC
EFI_STATUS
CreateConfigIdInfo (
  IN  EFI_HII_HANDLE                        HiiHandle
  )
{
  VOID                                  *StartOpCodeHandle;
  EFI_IFR_GUID_LABEL                    *StartLabel;
  EFI_STATUS                            Status;

  StartOpCodeHandle = HiiAllocateOpCodeHandle ();
  ASSERT (StartOpCodeHandle != NULL);

  StartLabel               = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode (StartOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
  StartLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  StartLabel->Number       = UPDATE_CONFIG_ID_LABEL;

  HiiCreateTextOpCode (
    StartOpCodeHandle,
    STRING_TOKEN(STR_CONFIG_DATA_ID_STRING),
    0,
    STRING_TOKEN(STR_CONFIG_DATA_ID_STRING2)
    );

  Status = HiiUpdateForm (
             HiiHandle,
             NULL,
             ROOT_FORM_ID,
             StartOpCodeHandle,
             NULL
             );

  HiiFreeOpCodeHandle (StartOpCodeHandle);

  return EFI_SUCCESS;
}

/**
  Update Config Data Name in VFR

  @param  HiiHandle              Input Hii handle

  @retval EFI_SUCCESS            update successfully.

**/
STATIC
EFI_STATUS
CreateConfigNameInfo (
  IN  EFI_HII_HANDLE                        HiiHandle
  )
{
  VOID                                  *StartOpCodeHandle;
  EFI_IFR_GUID_LABEL                    *StartLabel;
  EFI_STATUS                            Status;

  StartOpCodeHandle = HiiAllocateOpCodeHandle ();
  ASSERT (StartOpCodeHandle != NULL);

  StartLabel               = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode (StartOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
  StartLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  StartLabel->Number       = UPDATE_CONFIG_NAME_LABEL;

  HiiCreateTextOpCode (
    StartOpCodeHandle,
    STRING_TOKEN(STR_CONFIG_DATA_NAME_STRING),
    0,
    STRING_TOKEN(STR_CONFIG_DATA_NAME_STRING2)
    );

  Status = HiiUpdateForm (
             HiiHandle,
             NULL,
             ROOT_FORM_ID,
             StartOpCodeHandle,
             NULL
             );

  HiiFreeOpCodeHandle (StartOpCodeHandle);

  return EFI_SUCCESS;
}

/**
  Update Multi-Config information, ie Config ID and Name.

  @param  HiiHandle              Input Hii handle

  @retval EFI_SUCCESS            Multi-Config update successful.
  @retval EFI_NOT_FOUND          Get variable fail.
  @retval EFI_OUT_OF_RESOURCES   Allocate memory fail.

**/
STATIC
EFI_STATUS
UpdateMultiConfigInfo (
  IN  EFI_HII_HANDLE                      HiiHandle
  )
{
  EFI_STATUS                  Status;
  CHAR16                      StrBuffer[40];
  EFI_STRING_ID               TokenToUpdate;
  UINT16                      Index2;
  UINT16                      ConfigCount;
  UINTN                       MultiConfigDataSize;
  VOID                        *MultiConfigData;
  UINT8                       ConfigId;
  CONFIG_HEADER               *TempHeader;
  CHAR16                      *TempStrBuffer;
  BOOLEAN                     SettingSame;
  VOID                        *SCUData;
  UINTN                       DataSize;
  UINTN                       NameSize;
  VOID                        *SCUMCDefaultData;

  Status = EFI_NOT_FOUND;
  TokenToUpdate = 0;
  TempStrBuffer = NULL;
  TempHeader = NULL;
  SettingSame = TRUE;
  Index2 = 0;
  ConfigId = 0;
  MultiConfigDataSize = 0;
  SCUData = NULL;
  ConfigCount = GetConfigCount();

  ZeroMem (StrBuffer, sizeof (StrBuffer));

  CommonGetVariableDataAndSize (L"Setup", &gSystemConfigurationGuid, &DataSize, &SCUData);
  if (SCUData == NULL) {
    ASSERT (SCUData != NULL);
    return EFI_NOT_FOUND;
  }

  for ( ; Index2 < ConfigCount; Index2 ++) {

    //
    // Get Active (Attribute: ACTIVE & LOAD_DEFAULT) Config Data from Multi Config Region
    //
    Status = GetSCUSetting (
              SETUP_FOR_BIOS_POST,
              &Index2,
              &MultiConfigData,
              &MultiConfigDataSize,
              &ConfigId,
              NULL
               );
    if (!EFI_ERROR (Status)) {

      //
      // Get Multi-Config Setting
      //
      SCUMCDefaultData = AllocateZeroPool (DataSize);
      if (SCUMCDefaultData == NULL) {
        return EFI_OUT_OF_RESOURCES;
      }
      CopyMem (SCUMCDefaultData, SCUData, DataSize);
      ExtractSetupDefault (SCUMCDefaultData);

      SetSCUDataFromMC (L"Setup", &gSystemConfigurationGuid, SETUP_FOR_BIOS_POST, (VOID *)SCUMCDefaultData, DataSize);

      if (CompareMem (SCUData, SCUMCDefaultData, DataSize) != 0) {
        SettingSame = FALSE;
      }

      FreePool (SCUData);
      SCUData = NULL;
      FreePool (SCUMCDefaultData);
      SCUMCDefaultData = NULL;

      break;
    }

  }

  if (EFI_ERROR (Status)) {
    Index2 = 0;
    for ( ; Index2 < ConfigCount; Index2 ++) {
      //
      // Get Active (Attribute: ACTIVE & LOAD_DEFAULT) Config Data from Multi Config Region
      //
      Status = GetSCUSetting (
                SETUP_FOR_LOAD_DEFAULT,
                &Index2,
                &MultiConfigData,
                &MultiConfigDataSize,
                &ConfigId,
                NULL
                 );
      if (!EFI_ERROR (Status)) {

        //
        // Get Multi-Config Setting
        //
        SCUMCDefaultData = AllocateZeroPool (DataSize);
        if (SCUMCDefaultData == NULL) {
          return EFI_OUT_OF_RESOURCES;
        }
        CopyMem (SCUMCDefaultData, SCUData, DataSize);
        ExtractSetupDefault (SCUMCDefaultData);

        SetSCUDataFromMC (L"Setup", &gSystemConfigurationGuid, SETUP_FOR_LOAD_DEFAULT, (VOID *)SCUMCDefaultData, DataSize);

        if (CompareMem (SCUData, SCUMCDefaultData, DataSize) != 0) {
          SettingSame = FALSE;
        }
        FreePool (SCUData);
        SCUData = NULL;
        FreePool (SCUMCDefaultData);
        SCUMCDefaultData = NULL;

        break;
      }
    }
  }

  if (!EFI_ERROR (Status)) {
    //
    // Display Config ID and Setting is modified or not
    //
    UnicodeValueToStringS (StrBuffer, sizeof (StrBuffer), PREFIX_ZERO | RADIX_HEX, (INT64) ConfigId, 2);
    StrCatS (StrBuffer, sizeof(StrBuffer) / sizeof(CHAR16), L"h");
    if (!SettingSame) {
      StrCatS (StrBuffer, sizeof(StrBuffer) / sizeof(CHAR16), L" (Setting Modified)");
    }
    TokenToUpdate = STRING_TOKEN (STR_CONFIG_DATA_ID_STRING2);

    HiiSetString (HiiHandle, TokenToUpdate, StrBuffer, NULL);
    CreateConfigIdInfo (HiiHandle);

    //
    //  Display Config Name
    //
    ZeroMem (StrBuffer, sizeof (StrBuffer));

    TempHeader = GetConfigHeaderByOrder (Index2);
    NameSize = AsciiStrSize (TempHeader->Name);
    if (NameSize > 1) {
      //
      // Config Name is not NULL
      //
      TempStrBuffer = (CHAR16 *)AllocateZeroPool (NameSize * sizeof (CHAR16));
      if (TempStrBuffer == NULL) {
        ASSERT (TempStrBuffer != NULL);
        return EFI_OUT_OF_RESOURCES;
      }
      AsciiStrToUnicodeStrS (TempHeader->Name, TempStrBuffer, NameSize);
      StrCatS (StrBuffer, sizeof(StrBuffer) / sizeof(CHAR16), TempStrBuffer);
      FreePool (TempStrBuffer);
    }

    TokenToUpdate = STRING_TOKEN (STR_CONFIG_DATA_NAME_STRING2);
    HiiSetString (HiiHandle, TokenToUpdate, StrBuffer, NULL);
    CreateConfigNameInfo (HiiHandle);
  }

  return EFI_SUCCESS;
}

/**
  Update System Infomation, ie CPU Type & Speed, Bus Speed, System Memory Speed, cache, and RAM.

  @param[in] HiiHandle           Input Hii handle

  @retval EFI_SUCCESS            System info update successful.
  @retval EFI_OUT_OF_RESOURCES   Failed to allocate memory.
  @retval EFI_NOT_FOUND          Failed to get string from HII database.
  @retval Other                  SMBIOS protocol instance is not found.
**/
EFI_STATUS
UpdateSystemInfo (
  IN EFI_HII_HANDLE           HiiHandle
  )
{
  EFI_SMBIOS_TABLE_HEADER     *Record;
  EFI_SMBIOS_HANDLE           SmbiosHandle;
  EFI_SMBIOS_PROTOCOL         *Smbios;
  EFI_STATUS                  Status;
  UINT8                       StrIndex;
  CHAR16                      *NewString;
  CHAR16                      *StrBuffer;
  UINTN                       StrBufferSize;
  UINT64                      MemorySize;
  UINT64                      MemorySizeInMB;
  UINT64                      MemorySizeInGB;
  UINT16                      CacheLevel;
  SMBIOS_TABLE_TYPE4          *Type4Record;
  SMBIOS_TABLE_TYPE7          *Type7Record;
  SMBIOS_TABLE_TYPE17         *Type17Record;
  SMBIOS_TABLE_TYPE19         *Type19Record;

  Status = gBS->LocateProtocol (&gEfiSmbiosProtocolGuid, NULL, (VOID **) &Smbios);
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  StrBufferSize = 80;
  StrBuffer     = AllocatePool (StrBufferSize);
  if (StrBuffer == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  MemorySize = 0;
  MemorySizeInGB = 0;
  MemorySizeInMB = 0;
  SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
  do {
    Status = Smbios->GetNext (Smbios, &SmbiosHandle, NULL, &Record, NULL);
    if (EFI_ERROR(Status)) {
      break;
    }

    switch (Record->Type) {

    case EFI_SMBIOS_TYPE_PROCESSOR_INFORMATION:
      Type4Record = (SMBIOS_TABLE_TYPE4 *) Record;

      StrIndex = Type4Record->ProcessorVersion;
      Status = GetOptionalStringByIndex ((CHAR8*)((UINT8*)Type4Record + Type4Record->Hdr.Length), StrIndex, &NewString);
      if (!EFI_ERROR (Status)) {
        HiiSetString (HiiHandle, STRING_TOKEN (STR_CPU_TYPE_STRING2), NewString, NULL);
        FreePool (NewString);
      }

      ZeroMem (StrBuffer, StrBufferSize);
      UnicodeValueToStringS (StrBuffer, StrBufferSize, 0, (INT64) Type4Record->ExternalClock, 0);
      StrCatS (StrBuffer, StrBufferSize / sizeof(CHAR16), L" MHz");
      HiiSetString (HiiHandle, STRING_TOKEN (STR_SYSTEM_BUS_SPEED_STRING2), StrBuffer, NULL);
      break;

    case EFI_SMBIOS_TYPE_MEMORY_DEVICE:
      Type17Record = (SMBIOS_TABLE_TYPE17 *) Record;

      if (Type17Record->ConfiguredMemoryClockSpeed != 0) {
        ZeroMem (StrBuffer, StrBufferSize);
        UnicodeValueToStringS (StrBuffer, StrBufferSize, 0, (INT64) Type17Record->ConfiguredMemoryClockSpeed, 0);

        if (Smbios->MajorVersion < 3 || (Smbios->MajorVersion == 3 && Smbios->MinorVersion < 1)){
          NewString = HiiGetString (HiiHandle, STRING_TOKEN (STR_SYSTEM_MEMORY_SPEED_UNIT_SMBIOS_3_0_0), NULL);
        } else {
          NewString = HiiGetString (HiiHandle, STRING_TOKEN (STR_SYSTEM_MEMORY_SPEED_UNIT), NULL);
        }
        if (NewString == NULL) {
          FreePool (StrBuffer);
          return EFI_NOT_FOUND;
        }

        if (StrBufferSize < StrSize (StrBuffer) + StrSize (NewString) - sizeof (CHAR16)) {
          StrBuffer = ReallocatePool (StrBufferSize, StrBufferSize + StrSize (NewString), StrBuffer);
          if (StrBuffer == NULL) {
            return EFI_OUT_OF_RESOURCES;
          }
          StrBufferSize += StrSize (NewString);
        }

        StrCatS (StrBuffer, StrBufferSize / sizeof(CHAR16), NewString);
        HiiSetString (HiiHandle, STRING_TOKEN (STR_SYSTEM_MEMORY_SPEED_STRING2), StrBuffer, NULL);
        FreePool (NewString);
      }
      break;

    case EFI_SMBIOS_TYPE_CACHE_INFORMATION:
      Type7Record = (SMBIOS_TABLE_TYPE7 *) Record;

      CacheLevel = (UINT16) (Type7Record->CacheConfiguration & 0x7) + 1;
      if (CacheLevel == EFI_CACHE_L2) {
        ConvertCacheSizeToString (Type7Record, &NewString);
        HiiSetString (HiiHandle, STRING_TOKEN (STR_CACHE_RAM_STRING2), NewString, NULL);
        FreePool (NewString);
      }
      break;

    case EFI_SMBIOS_TYPE_MEMORY_ARRAY_MAPPED_ADDRESS:
      Type19Record = (SMBIOS_TABLE_TYPE19 *) Record;

      ZeroMem (StrBuffer, StrBufferSize);
      if (Type19Record->StartingAddress == 0xFFFFFFFF && Type19Record->EndingAddress == 0xFFFFFFFF) {
        MemorySizeInGB += (RShiftU64((Type19Record->ExtendedEndingAddress - Type19Record->ExtendedStartingAddress), 30) + 1);
        UnicodeValueToStringS (StrBuffer, StrBufferSize, 0, MemorySizeInGB, 0);
        StrCatS (StrBuffer, StrBufferSize / sizeof(CHAR16), L" GB");
      } else {
        MemorySizeInMB += (RShiftU64((Type19Record->EndingAddress - Type19Record->StartingAddress), 10) + 1);
        UnicodeValueToStringS (StrBuffer, StrBufferSize, 0, MemorySizeInMB, 0);
        StrCatS (StrBuffer, StrBufferSize / sizeof(CHAR16), L" MB");
      }
      if ((MemorySizeInMB != 0) && (MemorySizeInGB != 0)) {
        ZeroMem (StrBuffer, StrBufferSize);
        MemorySize = MemorySizeInMB + LShiftU64 (MemorySizeInGB, 10);
        UnicodeValueToStringS (StrBuffer, StrBufferSize, 0, MemorySize, 0);
        StrCatS (StrBuffer, StrBufferSize / sizeof(CHAR16), L" MB");
      }
      HiiSetString (HiiHandle, STRING_TOKEN (STR_TOTAL_MEMORY_STRING2), StrBuffer, NULL);
      break;
    }
  } while (TRUE);

  if (FeaturePcdGet (PcdMultiConfigSupported)) {
    UpdateMultiConfigInfo (HiiHandle);
  }

  CreateCpuTypeInfo (HiiHandle);
  CreateSystemBusSpeedInfo (HiiHandle);
  CreateCacheRamInfo (HiiHandle);

  FreePool (StrBuffer);
  return  EFI_SUCCESS;
}
