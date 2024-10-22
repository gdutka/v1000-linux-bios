/** @file

;******************************************************************************
;* Copyright (c) 2016 - 2017, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <UpdateMemInfo.h>

EFI_HII_STRING_PROTOCOL   *mIfrLibHiiString;

STATIC CHAR16                      *mMemFormFactorStr[] = {L"Other",
                                                       L"Unknown",
                                                       L"SIMM",
                                                       L"SIP",
                                                       L"Chip",
                                                       L"DIP",
                                                       L"ZIP",
                                                       L"Proprietary Card",
                                                       L"DIMM",
                                                       L"TSOP",
                                                       L"Row of chips",
                                                       L"RIMM",
                                                       L"SODIMM",
                                                       L"SRIMM",
                                                       L"FB-DIMM"
                                                       };

/**

 Update new string to Hii Handle.

 @param [in]   SrcHiiHandle   New Hii Handle.
 @param [in]   SrcStringId    New String Token.
 @param [in]   DstHiiHandle   Hii Handle of the package to be updated.
 @param [out]   DstStringId   String Token to be updated.

 @retval EFI_SUCCESS          String update successfully.

**/
EFI_STATUS
NewStringToHandle (
  IN  EFI_HII_HANDLE           SrcHiiHandle,
  IN  EFI_STRING_ID            SrcStringId,
  IN  EFI_HII_HANDLE           DstHiiHandle,
  OUT EFI_STRING_ID            *DstStringId
  )
{
  EFI_STATUS                   Status;
  CHAR16                       *String;
  UINTN                        Size;
  UINTN                        StringSize;
  CHAR8                        *Languages;
  CHAR8                        *LangStrings;
  CHAR8                        Lang[RFC_3066_ENTRY_SIZE];


  StringSize = 0x200;
  String = AllocateZeroPool (StringSize);
  if (String == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // Use english string as the default string.
  //
  Size = StringSize;
  Status = mIfrLibHiiString->GetString (
                               mIfrLibHiiString,
                               "en-US",
                               SrcHiiHandle,
                               SrcStringId,
                               String,
                               &Size,
                               NULL
                               );
  if (Status == EFI_BUFFER_TOO_SMALL) {
    gBS->FreePool (String);
    StringSize = Size;
    String = AllocateZeroPool (StringSize);
    Status = mIfrLibHiiString->GetString (
                                 mIfrLibHiiString,
                                 "eng",
                                 SrcHiiHandle,
                                 SrcStringId,
                                 String,
                                 &Size,
                                 NULL
                                 );
    if (EFI_ERROR (Status)) {
      gBS->FreePool (String);
      return Status;
    }
  } else if (EFI_ERROR (Status)) {
    return Status;
  }

  *DstStringId = HiiSetString (DstHiiHandle, 0, String, NULL);

  //
  // Update string by each language.
  //
  Languages = HiiGetSupportedLanguages (DstHiiHandle);
  if (Languages == NULL) {
    gBS->FreePool (String);
    return EFI_NOT_FOUND;
  }
  LangStrings = Languages;
  while (*LangStrings != 0) {
    SetupUtilityLibGetNextLanguage (&LangStrings, Lang);

    Size = StringSize;
    Status = mIfrLibHiiString->GetString (
                                 mIfrLibHiiString,
                                 Lang,
                                 SrcHiiHandle,
                                 SrcStringId,
                                 String,
                                 &Size,
                                 NULL
                                 );
    if (!EFI_ERROR (Status)) {
      mIfrLibHiiString->SetString (
                          mIfrLibHiiString,
                          DstHiiHandle,
                          *DstStringId,
                          Lang,
                          String,
                          NULL
                          );
    }
  }

  gBS->FreePool (String);
  gBS->FreePool (Languages);

  return EFI_SUCCESS;
}


/**
  Create Ramslot information. And save string tokens into TransferRamSlotInfo for destination Hii handle 
  after conversion.

  @param[in]       SourceHiiHandle        Input source Hii handle.
  @param[in]       DestinationHiiHandle   Input destination Hii handle.
  @param[in]       NumSlot                Number of memory slots.
  @param[in]       RamSlotInfo            Pointer to the start of Ram slot information arrary.
                                          This arrary saved string tokens for source Hii handle.
  @param[in, out]  TransferRamSlotInfo    Pointer to the start of Ram slot information arrary.
                                          This arrary saved string tokens for destination Hii handle
                                          after conversion.

  @retval          EFI_SUCCESS            If the create Ramslot and save string tokens could be successfully.
  @retval          Others                 Failed to update Form. 

**/
STATIC
EFI_STATUS
CreateRamSlotInfo (
  IN        EFI_HII_HANDLE         SourceHiiHandle,
  IN        EFI_HII_HANDLE         DestinationHiiHandle,
  IN        UINTN                  NumSlot,
  IN        UPDATE_INFO_STRUCT     *RamSlotInfo,
  IN OUT    UPDATE_INFO_STRUCT     *TransferRamSlotInfo
  )
{
  VOID                                  *StartOpCodeHandle;
  VOID                                  *EndOpCodeHandle;
  EFI_IFR_GUID_LABEL                    *StartLabel;
  EFI_IFR_GUID_LABEL                    *EndLabel;
  UINTN                                 Index;
  EFI_STATUS                            Status;
  EFI_STRING_ID                         BlankString;
  EFI_STRING_ID                         StrToken1;
  EFI_STRING_ID                         StrToken2;

  StartOpCodeHandle = HiiAllocateOpCodeHandle ();
  ASSERT (StartOpCodeHandle != NULL);
  
  EndOpCodeHandle = HiiAllocateOpCodeHandle ();
  ASSERT (EndOpCodeHandle != NULL);

  StartLabel               = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode (StartOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
  StartLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  StartLabel->Number       = UPDATE_INFO_RAM_SLOT_LABEL;

  EndLabel                 = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode (EndOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
  EndLabel->ExtendOpCode   = EFI_IFR_EXTEND_OP_LABEL;
  EndLabel->Number         = MAIN_PAGE_PLATFORM_INFO_LABEL;
  
  NewStringToHandle (
    SourceHiiHandle,
    STRING_TOKEN (STR_BLANK_STRING),
    DestinationHiiHandle,
    &BlankString
    );
  //
  // Channel A
  //
  NewStringToHandle (
    SourceHiiHandle,
    STRING_TOKEN (STR_CHANNELA_STRING),
    DestinationHiiHandle,
    &StrToken1
    );

  HiiCreateTextOpCode (
    StartOpCodeHandle,
    StrToken1,
    0,
    BlankString
    );

    for (Index = 0; Index < NumSlot; Index++) {
      if (Index == NumSlot / 2 ) {
        //
        // Channel B
        //
        NewStringToHandle (
          SourceHiiHandle,
          STRING_TOKEN (STR_CHANNELB_STRING),
          DestinationHiiHandle,
          &StrToken1
          );
        HiiCreateTextOpCode (
          StartOpCodeHandle,
          StrToken1,
          0,
          BlankString
        );
      }

      NewStringToHandle (
        SourceHiiHandle,
        RamSlotInfo[Index].TokenToUpdate1,
        DestinationHiiHandle,
        &StrToken1
        );
      TransferRamSlotInfo[Index].TokenToUpdate1 = StrToken1;

      NewStringToHandle (
        SourceHiiHandle,
        RamSlotInfo[Index].TokenToUpdate2,
        DestinationHiiHandle,
        &StrToken2
        );
      TransferRamSlotInfo[Index].TokenToUpdate2 = StrToken2;

      HiiCreateTextOpCode (
        StartOpCodeHandle,
        StrToken1,
        0,
        StrToken2
        );
    }
  //
  // Add Text op-code
  //
  Status = HiiUpdateForm (
             DestinationHiiHandle,
             NULL,
             ROOT_FORM_ID,
             StartOpCodeHandle,
             EndOpCodeHandle
             );

   HiiFreeOpCodeHandle (StartOpCodeHandle);
   HiiFreeOpCodeHandle (EndOpCodeHandle);

   return Status;
}

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
GetStringByIndex (
  IN  CHAR8                        *OptionalStrStart,
  IN  UINT8                        Index,
  OUT CHAR16                       **String
  )
{
  UINTN                                 StrSize;
  CHAR16                                *StringBuffer;

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
    // Meet the end of strings set but Index is non-zero
    //
    return EFI_ABORTED;
  }

  StringBuffer = AllocatePool (StrSize * sizeof (CHAR16));
  if (StringBuffer == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  AsciiStrToUnicodeStr (OptionalStrStart, StringBuffer);

  *String = StringBuffer;

  return EFI_SUCCESS;
}

/**
  Update Memory Infomation.

  @param    SUBrowser              Setup utility bowser data point

  @retval   EFI_SUCCESS            If the Memory Infomation could be successfully updated.
  @retval   Other                  Failed to update Memory Infomation.

**/
EFI_STATUS
UpdateMemoryInfo (
  IN  SETUP_UTILITY_BROWSER_DATA            *SUBrowser
  )
{
  EFI_SMBIOS_TABLE_HEADER     *Record;
  EFI_SMBIOS_HANDLE           SmbiosHandle;
  EFI_SMBIOS_PROTOCOL         *Smbios;
  EFI_HII_HANDLE              LocalMainHiiHandle;
  EFI_HII_HANDLE              LocalAdvanceHiiHandle;
  EFI_STATUS                  Status;
  UINT8                       StrIndex;
  UINTN                       NumSlot;
  UINTN                       Index;
  UINTN                       SlotIndex;
  CHAR16                      *NewString;
  CHAR16                      StrBuffer[40];
  EFI_STRING_ID               TokenToUpdate;
  SMBIOS_TABLE_TYPE17         *Type17Record;
  UPDATE_INFO_STRUCT          *LocalMainRamSlotInfo;
  UPDATE_INFO_STRUCT          UpdateRamSlotInfo[] = {
                                {1, STRING_TOKEN(STR_CHA_DIMM0_STRING), STRING_TOKEN(STR_CHA_DIMM0_STRING2)},
                                {2, STRING_TOKEN(STR_CHA_DIMM1_STRING), STRING_TOKEN(STR_CHA_DIMM1_STRING2)},
                                {3, STRING_TOKEN(STR_CHB_DIMM0_STRING), STRING_TOKEN(STR_CHB_DIMM0_STRING2)},
                                {4, STRING_TOKEN(STR_CHB_DIMM1_STRING), STRING_TOKEN(STR_CHB_DIMM1_STRING2)}
                              };
  
  Status = gBS->LocateProtocol (
                  &gEfiSmbiosProtocolGuid,
                  NULL,
                  (VOID **) &Smbios
                  );
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = gBS->LocateProtocol (&gEfiHiiStringProtocolGuid, NULL, (VOID **)&mIfrLibHiiString);
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }
 
  LocalMainHiiHandle      = SUBrowser->SUCInfo->MapTable[MainHiiHandle].HiiHandle;
  LocalAdvanceHiiHandle   = SUBrowser->SUCInfo->MapTable[AdvanceHiiHandle].HiiHandle;  

  NumSlot = sizeof (UpdateRamSlotInfo) /sizeof (UPDATE_INFO_STRUCT);
  LocalMainRamSlotInfo = AllocateZeroPool(sizeof (UpdateRamSlotInfo)); 
  if (LocalMainRamSlotInfo == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  Status = CreateRamSlotInfo (
             LocalAdvanceHiiHandle,
             LocalMainHiiHandle,
             NumSlot,
             UpdateRamSlotInfo,
             LocalMainRamSlotInfo
             );
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    gBS->FreePool (LocalMainRamSlotInfo);
    return Status;
  }

  Index = 0;
  SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
  do {
    Status = Smbios->GetNext (Smbios, &SmbiosHandle, NULL, &Record, NULL);
    if (EFI_ERROR(Status)) {
      break;
    }
    if (Record->Type == EFI_SMBIOS_TYPE_MEMORY_DEVICE) {
      Type17Record = (SMBIOS_TABLE_TYPE17 *) Record;
      //
      // Update memory device slot string
      //
      SlotIndex = Index;
      if (Type17Record->FormFactor >= MemoryFormFactorOther &&
          Type17Record->FormFactor <= MemoryFormFactorFbDimm) {
        StrIndex = Type17Record->BankLocator;
        Status = GetStringByIndex ((CHAR8*) ((UINT8*) Type17Record + Type17Record->Hdr.Length), StrIndex, &NewString);
        if (Status == EFI_SUCCESS) {
          if (!StrnCmp (NewString, L"P0 CHANNEL A", StrLen (L"P0 CHANNEL A"))) {
            SlotIndex = 0;			
          } else {
            SlotIndex = 2;			
          }
          gBS->FreePool (NewString);
        }

        StrIndex = Type17Record->DeviceLocator;
        Status = GetStringByIndex ((CHAR8*) ((UINT8*) Type17Record + Type17Record->Hdr.Length), StrIndex, &NewString);
        if (Status == EFI_SUCCESS) {
          if (!StrnCmp (NewString, L"DIMM 1", StrLen (L"DIMM 1"))) {
            SlotIndex ++;                                             // For DIMM 1, Need ++ based on channel value
          }
          gBS->FreePool (NewString);
        }

        if (Index == 0) {
          UnicodeSPrint (StrBuffer, sizeof (StrBuffer), L"%s 0", mMemFormFactorStr[Type17Record->FormFactor - 1]);
          HiiSetString (LocalMainHiiHandle, LocalMainRamSlotInfo[0].TokenToUpdate1, StrBuffer, NULL);
          HiiSetString (LocalMainHiiHandle, LocalMainRamSlotInfo[2].TokenToUpdate1, StrBuffer, NULL);
          UnicodeSPrint (StrBuffer, sizeof (StrBuffer), L"%s 1", mMemFormFactorStr[Type17Record->FormFactor - 1]);
          HiiSetString (LocalMainHiiHandle, LocalMainRamSlotInfo[1].TokenToUpdate1, StrBuffer, NULL);
          HiiSetString (LocalMainHiiHandle, LocalMainRamSlotInfo[3].TokenToUpdate1, StrBuffer, NULL);
        }
       }

       if (Type17Record->Size != 0) {
         ZeroMem (StrBuffer, sizeof (StrBuffer));
         if (Type17Record->Size == 0xFFFF) {
           StrCat (StrBuffer, L"Unknown");
         } else if (Type17Record->Size == 0x7FFFF) {
          UnicodeValueToString (StrBuffer, 0, (UINT64)(Type17Record->ExtendedSize & ~BIT31), 0);
          StrCat (StrBuffer, L" MB");
        } else {
          UnicodeValueToString (StrBuffer, 0, (UINT64)(Type17Record->Size & ~BIT15), 0);
          if (Type17Record->Size & BIT15) {
            StrCat (StrBuffer, L" KB");
          } else {
            StrCat (StrBuffer, L" MB");
          }
        }
        TokenToUpdate = (EFI_STRING_ID) (LocalMainRamSlotInfo[SlotIndex].TokenToUpdate2);
        HiiSetString (LocalMainHiiHandle, TokenToUpdate, StrBuffer, NULL);
      }
      Index++;
    }
  } while(Status == EFI_SUCCESS);
 
  gBS->FreePool (LocalMainRamSlotInfo);
  return EFI_SUCCESS;
}

