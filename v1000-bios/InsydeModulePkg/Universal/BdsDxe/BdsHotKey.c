/** @file
  Process BDS hot key.

;******************************************************************************
;* Copyright (c) 2016 - 2020, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "Bds.h"

LIST_ENTRY                             mBdsHotKeyList         = INITIALIZE_LIST_HEAD_VARIABLE (mBdsHotKeyList);
H2O_HOT_KEY_OPTION                     *mSelectedHotKeyOption = NULL;

STATIC H2O_KEY_DESC_PROTOCOL           *mH2OKeyDesc           = NULL;
STATIC BOOLEAN                         mHotKeySupport         = FALSE;
STATIC BOOLEAN                         mHotKeyEnabled         = TRUE;
STATIC EFI_EVENT                       mHotKeyTimeoutEvent    = NULL;
STATIC UINTN                           mSimpleTextInCount     = 0;
STATIC EFI_HANDLE                      *mSimpleTextInList     = NULL;

typedef struct _BOTH_SIDE_SHIFT_STATE_INFO {
  UINT32                               RightShiftState;
  UINT32                               LeftShiftState;
} BOTH_SIDE_SHIFT_STATE_INFO;
STATIC BOTH_SIDE_SHIFT_STATE_INFO      mBothSideShiftStateList[] = {{EFI_RIGHT_SHIFT_PRESSED  , EFI_LEFT_SHIFT_PRESSED  },
                                                                    {EFI_RIGHT_CONTROL_PRESSED, EFI_LEFT_CONTROL_PRESSED},
                                                                    {EFI_RIGHT_ALT_PRESSED    , EFI_LEFT_ALT_PRESSED    },
                                                                    {EFI_RIGHT_LOGO_PRESSED   , EFI_LEFT_LOGO_PRESSED   },
                                                                    };

STATIC
VOID
HotKeyRegisterKeyNotify (
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL *SimpleTextInEx
  );

/**
 Function to compare 2 value.

 @param[in] Buffer1    Pointer to value to compare (UINTN*).
 @param[in] Buffer2    Pointer to second value to compare (UINTN*).

 @retval 0             Buffer1 equal to Buffer2.
 @retval <0            Buffer1 is less than Buffer2.
 @retval >0            Buffer1 is greater than Buffer2.
**/
STATIC
INTN
ValueCompare (
  IN CONST VOID                        *Buffer1,
  IN CONST VOID                        *Buffer2
  )
{
  if (*((UINTN *) Buffer1) == *((UINTN *) Buffer2)) {
    return 0;
  } else if (*((UINTN *) Buffer1) < *((UINTN *) Buffer2)) {
    return -1;
  } else {
    return 1;
  }
}

/**
 Check if two key data are match.

 @param[in] KeyData1                   Pointer to key data 1
 @param[in] KeyData2                   Pointer to key data 2

 @retval TRUE   The two key data are match.
 @retval FALSE  The two key data are not match.
**/
STATIC
BOOLEAN
IsKeyMatch (
  IN EFI_KEY_DATA                      *KeyData1,
  IN EFI_KEY_DATA                      *KeyData2
  )
{
  UINTN                                Index;
  UINT32                               KeyData1ShiftState;
  UINT32                               KeyData2ShiftState;
  UINT32                               BothSideShiftState;
  UINT32                               KeyData1Result;

  if ((KeyData1->Key.ScanCode    != KeyData2->Key.ScanCode) ||
      (KeyData1->Key.UnicodeChar != KeyData2->Key.UnicodeChar)) {
    return FALSE;
  }

  KeyData1ShiftState = KeyData1->KeyState.KeyShiftState;
  KeyData2ShiftState = KeyData2->KeyState.KeyShiftState;

  if ((KeyData1ShiftState & EFI_MENU_KEY_PRESSED) != (KeyData2ShiftState & EFI_MENU_KEY_PRESSED)) {
    return FALSE;
  }
  if ((KeyData1ShiftState & EFI_SYS_REQ_PRESSED) != (KeyData2ShiftState & EFI_SYS_REQ_PRESSED)) {
    return FALSE;
  }

  for (Index = 0; Index < sizeof(mBothSideShiftStateList) / sizeof(BOTH_SIDE_SHIFT_STATE_INFO); Index++) {
    BothSideShiftState  = (mBothSideShiftStateList[Index].RightShiftState | mBothSideShiftStateList[Index].LeftShiftState);
    KeyData1Result      = (KeyData1ShiftState & BothSideShiftState);

    if (KeyData1Result == 0) {
      if ((KeyData2ShiftState & BothSideShiftState) != 0) {
        return FALSE;
      }
    } else {
      if ((KeyData2ShiftState & KeyData1Result) == 0) {
        return FALSE;
      }
    }
  }

  return TRUE;
}

/**
 Return TRUE when the variable pointed by Name and Guid is a Key#### variable.

 @param[in]  VarName                   The name of the variable.
 @param[in]  VarGuid                   The GUID of the variable.
 @param[out] KeyOptionNum              Return the option number parsed from the Name.

 @retval TRUE   The variable pointed by Name and Guid is a Key#### variable.
 @retval FALSE  The variable pointed by Name and Guid isn't a Key#### variable.
**/
STATIC
BOOLEAN
IsKeyOptionVariable (
  IN  CHAR16                           *VarName,
  IN  EFI_GUID                         *VarGuid,
  OUT UINT16                           *KeyOptionNum
  )
{
  UINTN                                Index;

  if (!CompareGuid (VarGuid, &gEfiGlobalVariableGuid) ||
      (StrSize (VarName) != sizeof(L"Key####")) ||
      (StrnCmp (VarName, L"Key", 3) != 0)) {
    return FALSE;
  }

  *KeyOptionNum = 0;
  for (Index = 3; Index < 7; Index++) {
    if ((VarName[Index] >= L'0') && (VarName[Index] <= L'9')) {
      *KeyOptionNum = *KeyOptionNum * 16 + VarName[Index] - L'0';
    } else if ((VarName[Index] >= L'A') && (VarName[Index] <= L'F')) {
      *KeyOptionNum = *KeyOptionNum * 16 + VarName[Index] - L'A' + 10;
    } else {
      return FALSE;
    }
  }

  return TRUE;
}

/**
 Check if the Key Option is valid or not.

 @param[in] KeyOption                  The Key Option to be checked.

 @retval TRUE   The Key Option is valid.
 @retval FALSE  The Key Option is invalid.
**/
STATIC
BOOLEAN
IsKeyOptionValid (
  IN EFI_KEY_OPTION                    *KeyOption
  )
{
  EFI_STATUS                           Status;
  UINT16                               BootOptionName[10];
  UINT8                                *BootOptionVar;
  UINTN                                BootOptionSize;
  UINT32                               Crc;

  //
  // Check whether corresponding Boot Option exist
  //
  UnicodeSPrint (BootOptionName, sizeof(BootOptionName), L"Boot%04x", KeyOption->BootOption);
  Status = CommonGetVariableDataAndSize (
             BootOptionName,
             &gEfiGlobalVariableGuid,
             &BootOptionSize,
             (VOID **) &BootOptionVar
             );
  if (Status != EFI_SUCCESS) {
    return FALSE;
  }

  //
  // Check CRC for Boot Option
  //
  gBS->CalculateCrc32 (BootOptionVar, BootOptionSize, &Crc);
  FreePool (BootOptionVar);

  return (BOOLEAN) ((KeyOption->BootOptionCrc == Crc) ? TRUE : FALSE);
}

/**
 Check if the chord is already registered in hot key list.

 @param[in] Chord                      Pointer to chord data.

 @retval TRUE   The chord is already registered.
 @retval FALSE  The chord is not registered.
**/
STATIC
BOOLEAN
IsChordRegistered (
  IN H2O_HOT_KEY_CHORD                 *Chord
  )
{
  LIST_ENTRY                           *Link;
  UINT32                               Index;
  UINT32                               KeyIndex;
  H2O_HOT_KEY_OPTION                   *HotKeyOption;
  H2O_HOT_KEY_CHORD                    *HotKeyChord;

  if (Chord == NULL || Chord->KeyCount == 0 || Chord->KeyList == NULL) {
    return FALSE;
  }

  Link = GetFirstNode (&mBdsHotKeyList);
  while (!IsNull (&mBdsHotKeyList, Link)) {
    HotKeyOption = H2O_HOT_KEY_OPTION_FROM_LINK (Link);
    Link         = GetNextNode (&mBdsHotKeyList, Link);

    for (Index = 0; Index < HotKeyOption->HotKeyChordCount; Index++) {
      HotKeyChord = &HotKeyOption->HotKeyChordList[Index];
      if (HotKeyChord->KeyCount != Chord->KeyCount) {
        continue;
      }

      for (KeyIndex = 0; KeyIndex < Chord->KeyCount; KeyIndex++) {
        if (!IsKeyMatch (&HotKeyChord->KeyList[KeyIndex], &Chord->KeyList[KeyIndex])) {
          break;
        }
      }
      if (KeyIndex == Chord->KeyCount) {
        return TRUE;
      }
    }
  }

  return FALSE;
}

/**
 Check if the device path belongs to pre-OS application.

 @param[in] DevicePath                 Pointer to device path.

 @retval TRUE   It is the device path of pre-OS application.
 @retval FALSE  It is not the device path of pre-OS application.
**/
STATIC
BOOLEAN
IsPreOsApp (
  IN EFI_DEVICE_PATH_PROTOCOL          *DevicePath
  )
{
  if (DevicePath == NULL) {
    return FALSE;
  }

  if (DevicePath->Type    == HARDWARE_DEVICE_PATH &&
      DevicePath->SubType == HW_VENDOR_DP) {
    return TRUE;
  }
  if (DevicePath->Type    == MEDIA_DEVICE_PATH &&
      DevicePath->SubType == MEDIA_PIWG_FW_FILE_DP &&
      CompareGuid (&((MEDIA_FW_VOL_FILEPATH_DEVICE_PATH *) DevicePath)->FvFileName, &gH2OBdsLoadOptionSetupUtilityAppFileGuid)) {
    return TRUE;
  }

  return FALSE;
}

/**
 Internal function to convert unicode string to ASCII string into the allocated buffer.

 @param[in] String                     Pointer to unicode string

 @return A pointer to the ASCII string or NULL if input parameter is NULL or allocate memory failed.
**/
STATIC
CHAR8 *
InternalUniStrToAsciiStrBuf (
  IN CHAR16                            *String
  )
{
  UINTN                                AsciiStringSize;
  CHAR8                                *AsciiString;

  if (String == NULL) {
    return NULL;
  }

  AsciiStringSize = (StrLen (String) + 1) * sizeof (CHAR8);
  AsciiString     = AllocateZeroPool (AsciiStringSize);
  if (AsciiString != NULL) {
    UnicodeStrToAsciiStrS (String, AsciiString, AsciiStringSize);
  }

  return AsciiString;
}

/**
 Get all token numbers of specified token space GUID.

 @param[in]  TokenSpaceGuid            The pointer of token space GUID.
 @param[out] TokenNumList              The double pointer to the token number list.
 @param[out] TokenNumCount             The pointer to the count of token number.

 @retval EFI_SUCCESS            Get all token numbers successfully.
 @retval EFI_INVALID_PARAMETER  Input parameter is NULL.
 @retval EFI_OUT_OF_RESOURCES   Allocate pool failed.
 @retval EFI_NOT_FOUND          There is no token number in specified token space GUID.
**/
EFI_STATUS
GetTokenNumList (
  IN  CONST GUID                       *TokenSpaceGuid,
  OUT UINTN                            **TokenNumList,
  OUT UINTN                            *TokenNumCount
  )
{
  UINTN                                Length;
  UINTN                                Count;
  UINTN                                *List;
  UINTN                                TokenNum;

  if (TokenSpaceGuid == NULL || TokenNumCount == NULL || TokenNumList == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Length   = 0;
  List     = NULL;
  TokenNum = 0;
  Count    = 0;
  while (TRUE) {
    TokenNum = LibPcdGetNextToken (TokenSpaceGuid, TokenNum);
    if (TokenNum == 0) {
      break;
    }

    if (Count >= Length) {
      List = ReallocatePool (
               sizeof(UINTN) * Length,
               sizeof(UINTN) * (Length + 10),
               List
               );
      if (List == NULL) {
        return EFI_OUT_OF_RESOURCES;
      }
      Length += 10;
    }

    List[Count] = TokenNum;
    Count++;
  }

  if (Count == 0) {
    return EFI_NOT_FOUND;
  }

  *TokenNumCount = Count;
  *TokenNumList  = List;

  return EFI_SUCCESS;
}

/**
 Get all key option numbers (i.e. #### value of L"Key####" variable).

 @param[out] KeyOptionNumList     The double pointer to the key option number list.
 @param[out] KeyOptionNumCount    The pointer to the count of key option numbers

 @retval EFI_SUCCESS              Get all key option numbers successfully.
 @retval EFI_INVALID_PARAMETER    Input parameter is NULL.
 @retval EFI_OUT_OF_RESOURCES     Allocate pool failed.
 @retval EFI_NOT_FOUND            There is no key option variable in variable store.
**/
STATIC
EFI_STATUS
GetKeyOptionNumList (
  OUT UINT16                           **KeyOptionNumList,
  OUT UINT16                           *KeyOptionNumCount
  )
{
  EFI_STATUS                           Status;
  EFI_GUID                             VarGuid;
  UINTN                                VarNameSize;
  CHAR16                               *VarName;
  UINTN                                BuffeSize;
  UINT16                               OptionNumCount;
  UINT16                               *OptionNumList;
  UINT16                               OptionNum;

  if (KeyOptionNumList == NULL || KeyOptionNumCount == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  BuffeSize = sizeof(CHAR16) * 40;
  VarName   = AllocateZeroPool (BuffeSize);
  if (VarName == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  OptionNumCount = 0;
  OptionNumList  = NULL;
  while (TRUE) {
    VarNameSize = BuffeSize;
    Status = gRT->GetNextVariableName (&VarNameSize, VarName, &VarGuid);
    if (Status == EFI_BUFFER_TOO_SMALL) {
      VarName = ReallocatePool (BuffeSize, VarNameSize, VarName);
      if (VarName == NULL) {
        return EFI_OUT_OF_RESOURCES;
      }
      BuffeSize = VarNameSize;
      Status = gRT->GetNextVariableName (&VarNameSize, VarName, &VarGuid);
    }
    if (EFI_ERROR (Status)) {
      break;
    }

    if (IsKeyOptionVariable (VarName ,&VarGuid, &OptionNum)) {
      OptionNumList = ReallocatePool (
                        sizeof(UINT16) * OptionNumCount,
                        sizeof(UINT16) * (OptionNumCount + 1),
                        OptionNumList
                        );
      if (OptionNumList == NULL) {
        FreePool (VarName);
        return EFI_OUT_OF_RESOURCES;
      }

      OptionNumList[OptionNumCount] = OptionNum;
      OptionNumCount++;
    }
  }

  FreePool (VarName);
  if (OptionNumCount == 0) {
    return EFI_NOT_FOUND;
  }

  *KeyOptionNumList  = OptionNumList;
  *KeyOptionNumCount = OptionNumCount;

  return EFI_SUCCESS;
}

/**
 Get key data list by parsing chord string.

 @param[in]  ChordStr                  The pointer to the chord string.
 @param[out] KeyList                   The double pointer to the key data list.
 @param[out] KeyCount                  The pointer to the count of key data.

 @retval EFI_SUCCESS            Update key list successfully.
 @retval EFI_INVALID_PARAMETER  Input parameter is NULL.
 @retval EFI_OUT_OF_RESOURCES   Allocate pool failed.
 @retval EFI_NOT_FOUND          There is no valid key in chord string.
**/
STATIC
EFI_STATUS
GetKeyDataList (
  IN  CHAR16                           *ChordStr,
  OUT EFI_KEY_DATA                     **KeyList,
  OUT UINT32                           *KeyCount
  )
{
  CHAR16                               *ChordStrBuffer;
  CHAR16                               *CurrKeyStr;
  CHAR16                               *NextKeyStr;
  UINT32                               Count;
  EFI_KEY_DATA                         *List;
  EFI_KEY_DATA                         KeyData;
  EFI_STATUS                           Status;

  if (ChordStr == NULL || *ChordStr == CHAR_NULL || KeyCount == NULL || KeyList == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  ChordStrBuffer = AllocateCopyPool (StrSize (ChordStr), ChordStr);
  if (ChordStrBuffer == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  List       = NULL;
  Count      = 0;
  NextKeyStr = ChordStrBuffer;
  while (NextKeyStr != NULL) {
    CurrKeyStr = NextKeyStr;
    NextKeyStr = StrStr (NextKeyStr, H2O_HOT_KEY_KEY_SEPARATOR_STRING);
    if (NextKeyStr != NULL) {
      *NextKeyStr = CHAR_NULL;
      NextKeyStr++;
    }

    Status = mH2OKeyDesc->KeyFromString (mH2OKeyDesc, CurrKeyStr, &KeyData);
    if (EFI_ERROR (Status)) {
      continue;
    }

    List = ReallocatePool (
             sizeof(EFI_KEY_DATA) * Count,
             sizeof(EFI_KEY_DATA) * (Count + 1),
             List
             );
    if (List == NULL) {
      FreePool (ChordStrBuffer);
      return EFI_OUT_OF_RESOURCES;
    }

    CopyMem (&List[Count], &KeyData, sizeof(EFI_KEY_DATA));
    Count++;
  }
  FreePool (ChordStrBuffer);

  if (Count == 0) {
    return EFI_NOT_FOUND;
  }

  *KeyCount = Count;
  *KeyList  = List;

  return EFI_SUCCESS;
}

/**
 Get chord list by parsing chord ascii string.

 @param[in]  ChordAsciiStr             The pointer to the chord ascii string.
 @param[out] ChordList                 The double pointer to the chord list.
 @param[out] ChordCount                The pointer to the count of chord.

 @retval EFI_SUCCESS            Get chord list successfully.
 @retval EFI_INVALID_PARAMETER  Input parameter is NULL.
 @retval EFI_OUT_OF_RESOURCES   Allocate pool failed.
 @retval EFI_NOT_FOUND          There is no valid chord in hot key string.
**/
STATIC
EFI_STATUS
GetChordList (
  IN  CHAR8                            *ChordAsciiStr,
  OUT H2O_HOT_KEY_CHORD                **ChordList,
  OUT UINT32                           *ChordCount
  )
{
  CHAR16                               *ChordStr;
  CHAR16                               *CurrChordStr;
  CHAR16                               *NextChordStr;
  UINT32                               Count;
  H2O_HOT_KEY_CHORD                    *List;
  UINT32                               KeyCount;
  EFI_KEY_DATA                         *KeyList;
  EFI_STATUS                           Status;
  UINTN                                ChordAsciiStrSize;

  if (ChordAsciiStr == NULL || *ChordAsciiStr == CHAR_NULL || ChordCount == NULL || ChordList == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  ChordAsciiStrSize = AsciiStrSize (ChordAsciiStr);
  ChordStr = AllocateZeroPool (ChordAsciiStrSize * sizeof(CHAR16));
  if (ChordStr == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  AsciiStrToUnicodeStrS (ChordAsciiStr, ChordStr, ChordAsciiStrSize);

  List         = NULL;
  Count        = 0;
  NextChordStr = ChordStr;
  while (NextChordStr != NULL) {
    CurrChordStr = NextChordStr;
    NextChordStr = StrStr (NextChordStr, H2O_HOT_KEY_CHORD_SEPARATOR_STRING);
    if (NextChordStr != NULL) {
      *NextChordStr = CHAR_NULL;
      NextChordStr++;
    }

    KeyCount = 0;
    KeyList  = NULL;
    Status   = GetKeyDataList (CurrChordStr, &KeyList, &KeyCount);
    if (EFI_ERROR (Status)) {
      continue;
    }

    List = ReallocatePool (
             sizeof(H2O_HOT_KEY_CHORD) * Count,
             sizeof(H2O_HOT_KEY_CHORD) * (Count + 1),
             List
             );
    if (List == NULL) {
      FreePool (ChordStr);
      return EFI_OUT_OF_RESOURCES;
    }

    List[Count].KeyCount      = KeyCount;
    List[Count].KeyList       = KeyList;
    List[Count].CurrentIndex  = 0;
    List[Count].ChordAsciiStr = InternalUniStrToAsciiStrBuf (CurrChordStr);
    Count++;
  }
  FreePool (ChordStr);

  if (Count == 0) {
    return EFI_NOT_FOUND;
  }

  *ChordCount = Count;
  *ChordList  = List;

  return EFI_SUCCESS;
}

/**
 Get chord ASCII string.

 @param[in] KeyList                   Pointer to the key list.
 @param[in] KeyCount                  The count of key.

 @return A pointer to the ASCII string or NULL if chord list is incorrect or allocate memory failed.
**/
STATIC
CHAR8 *
GetChordAsciiStr (
  IN EFI_KEY_DATA                      *KeyList,
  IN UINT32                            KeyCount
  )
{
  EFI_STATUS                           Status;
  UINT32                               Index;
  CHAR16                               *KeyStr;
  CHAR16                               *ChordStr;
  CHAR16                               *NewStr;
  CHAR8                                *ChordAsciiStr;

  if (KeyList == NULL) {
    return NULL;
  }

  ChordStr = NULL;
  for (Index = 0; Index < KeyCount; Index++) {
    Status = mH2OKeyDesc->KeyToString (mH2OKeyDesc, &KeyList[Index], &KeyStr);
    if (EFI_ERROR (Status)) {
      continue;
    }

    if (ChordStr == NULL) {
      ChordStr = KeyStr;
    } else {
      NewStr = CatSPrint (NULL, L"%s%s%s", ChordStr, H2O_HOT_KEY_KEY_SEPARATOR_STRING, KeyStr);
      FreePool (KeyStr);
      FreePool (ChordStr);
      ChordStr = NewStr;
    }
  }
  if (ChordStr == NULL) {
    return NULL;
  }

  ChordAsciiStr = InternalUniStrToAsciiStrBuf (ChordStr);
  FreePool (ChordStr);
  return ChordAsciiStr;
}

/**
 Get device path by parsing ascii string.

 @param[in] DevicePathAsciiStr         The pointer to the device path ascii string.

 @return A pointer to the allocated device path or NULL if ascii string is NULL or allocate memory failed.
**/
EFI_DEVICE_PATH_PROTOCOL *
GetDevicePathByAsciiStr (
  IN CHAR8                             *DevicePathAsciiStr
  )
{
  CHAR16                               *DevicePathStr;
  EFI_DEVICE_PATH_PROTOCOL             *DevicePath;
  UINTN                                DevicePathAsciiStrSize;

  if (DevicePathAsciiStr == NULL || *DevicePathAsciiStr == CHAR_NULL) {
    return NULL;
  }

  DevicePathAsciiStrSize = AsciiStrSize (DevicePathAsciiStr);
  DevicePathStr = AllocateZeroPool (DevicePathAsciiStrSize * sizeof(CHAR16));
  if (DevicePathStr == NULL) {
    return NULL;
  }
  AsciiStrToUnicodeStrS (DevicePathAsciiStr, DevicePathStr, DevicePathAsciiStrSize);

  DevicePath = GetDevicePathByStr (DevicePathStr);
  FreePool (DevicePathStr);

  return DevicePath;
}

/**
 Get device path by parsing string.

 @param[in] DevicePathStr              The pointer to the device path string.

 @return A pointer to the allocated device path or NULL if string is NULL or allocate memory failed.
**/
EFI_DEVICE_PATH_PROTOCOL *
GetDevicePathByStr (
  IN CHAR16                            *DevicePathStr
  )
{
  EFI_STATUS                           Status;
  EFI_DEVICE_PATH_FROM_TEXT_PROTOCOL   *DevicePathFromText;

  if (DevicePathStr == NULL || *DevicePathStr == CHAR_NULL) {
    return NULL;
  }

  Status = gBS->LocateProtocol (&gEfiDevicePathFromTextProtocolGuid, NULL, (VOID **) &DevicePathFromText);
  if (EFI_ERROR (Status)) {
    return NULL;
  }

  return DevicePathFromText->ConvertTextToDevicePath (DevicePathStr);
}

/**
 Get current BDS context from BDS service function.

 @param[out] BdsContext                Pointer to the BDS context.

 @retval EFI_SUCCESS                   Get current BDS context successfully.
 @retval EFI_INVALID_PARAMETER         Input parameter is NULL.
**/
STATIC
EFI_STATUS
GetBdsContext (
  OUT H2O_BDS_CONTEXT                  *BdsContext
  )
{
  if (BdsContext == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  ZeroMem (BdsContext, sizeof(H2O_BDS_CONTEXT));

  BdsContext->Size = sizeof(H2O_BDS_CONTEXT);
  gBdsServices->GetTimeout (gBdsServices, &BdsContext->Timeout);
  gBdsServices->GetOsIndications (gBdsServices, &BdsContext->OsIndications, &BdsContext->OsIndicationsSupported);
  gBdsServices->GetBootMode (gBdsServices, &BdsContext->BootMode);
  gBdsServices->GetBootType (gBdsServices, &BdsContext->BootType);

  return EFI_SUCCESS;
}

/**
 Get hot key option by handle.

 @param[in] HotKeyHandle               The pointer to hot key option handle.

 @return A pointer to the hot key option or NULL if there is no match hot key option.
**/
H2O_HOT_KEY_OPTION *
GetHotKeyOption (
  IN EFI_HANDLE                        HotKeyHandle
  )
{
  LIST_ENTRY                           *Link;
  H2O_HOT_KEY_OPTION                   *HotKeyOption;

  if (HotKeyHandle == NULL) {
    return NULL;
  }

  Link = GetFirstNode (&mBdsHotKeyList);
  while (!IsNull (&mBdsHotKeyList, Link)) {
    HotKeyOption = H2O_HOT_KEY_OPTION_FROM_LINK (Link);
    if ((EFI_HANDLE) HotKeyOption == HotKeyHandle) {
      return HotKeyOption;
    }

    Link = GetNextNode (&mBdsHotKeyList, Link);
  }

  return NULL;
}

/**
 Free memory resource for chord list.

 @param[in] ChordList       A pointer to chord list
 @param[in] ChordCount      Number of chord in list
**/
VOID
FreeChordList (
  IN H2O_HOT_KEY_CHORD                 *ChordList,
  IN UINT32                            ChordCount
  )
{
  UINT32                               Index;

  if (ChordCount == 0 || ChordList == NULL) {
    return;
  }

  for (Index = 0; Index < ChordCount; Index++) {
    if (ChordList[Index].KeyList != NULL) {
      FreePool (ChordList[Index].KeyList);
    }
    if (ChordList[Index].ChordAsciiStr != NULL) {
      FreePool (ChordList[Index].ChordAsciiStr);
    }
  }
  FreePool (ChordList);
}

/**
 Determine to enable or disable BDS hot key processing.

 @param[in] Enable      A Boolean that specifies whether hot key processing should be enabled (TRUE) or disabled (FALSE)

 @return A Boolean indicates whether hot key processing was enabled (TRUE) or disabled (FALSE) before this call.
**/
BOOLEAN
EnableBdsHotKey (
  IN BOOLEAN                           Enable
  )
{
  gBdsServices->EnableHotKeys (gBdsServices, &Enable);
  return Enable;
}

/**
 Copy the input load option.

 @param[in] SrcLoadOption              A pointer to the load option.

 @return A pointer to the allocated load option or NULL if input parameter is NULL or allocate memory failed.
**/
STATIC
H2O_BDS_LOAD_OPTION *
CopyLoadOption (
  IN H2O_BDS_LOAD_OPTION               *SrcLoadOption
  )
{
  H2O_BDS_LOAD_OPTION                  *DstLoadOption;

  if (SrcLoadOption == NULL) {
    return NULL;
  }

  DstLoadOption = AllocateCopyPool (sizeof(H2O_BDS_LOAD_OPTION), SrcLoadOption);
  if (DstLoadOption == NULL) {
    return NULL;
  }

  if (SrcLoadOption->DevicePath != NULL) {
    DstLoadOption->DevicePath = AllocateCopyPool (GetDevicePathSize (SrcLoadOption->DevicePath), SrcLoadOption->DevicePath);
  }
  if (SrcLoadOption->Description != NULL) {
    DstLoadOption->Description = AllocateCopyPool (StrSize (SrcLoadOption->Description), SrcLoadOption->Description);
  }
  if (SrcLoadOption->LoadOptionalDataSize != 0) {
    DstLoadOption->LoadOptionalData = AllocateCopyPool (SrcLoadOption->LoadOptionalDataSize, SrcLoadOption->LoadOptionalData);
  }
  if (SrcLoadOption->StatusString != NULL) {
    DstLoadOption->StatusString = AllocateCopyPool (StrSize (SrcLoadOption->StatusString), SrcLoadOption->StatusString);
  }

  return DstLoadOption;
}

/**
 Lunch load option.

 @param[in] LoadOption                 Pointer to load option.

 @retval EFI_SUCCESS                   Perform lunch load option successfully.
 @retval EFI_INVALID_PARAMETER         Input parameter is NULL.
 @retval Other                         LaunchLoadOption function return failed.
**/
STATIC
EFI_STATUS
LaunchLoadOption (
  IN H2O_BDS_LOAD_OPTION               *LoadOption
  )
{
  EFI_STATUS                           Status;
  UINTN                                ExitDataSize;
  CHAR16                               *ExitData;

  if (LoadOption == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  gBdsServices->ExpandLoadOption (gBdsServices, LoadOption);

  if (DevicePathType (LoadOption->DevicePath) != BBS_DEVICE_PATH) {
    BdsLibConnectDevicePath (LoadOption->DevicePath);
  }

  Status = gBdsServices->LaunchLoadOption (gBdsServices, LoadOption, &ExitDataSize, &ExitData);
  if (Status != EFI_SUCCESS) {
    BdsLibBootFailed (LoadOption, Status, ExitData, ExitDataSize);
  } else {
    BdsLibBootSuccess (LoadOption);
  }

  return Status;
}

/**
 Lunch device path.

 @param[in] DevicePath                 Pointer to device path
 @param[in] OptionalData               Pointer to optional data
 @param[in] OptionalDataSize           Optional data size

 @retval EFI_SUCCESS                   Perform lunch load option successfully.
 @retval EFI_INVALID_PARAMETER         Input parameter is NULL.
 @retval Other                         LaunchLoadOption function return failed.
**/
STATIC
EFI_STATUS
LaunchDevicePath (
  IN EFI_DEVICE_PATH_PROTOCOL          *DevicePath,
  IN UINT8                             *OptionalData,
  IN UINT32                            OptionalDataSize
  )
{
  EFI_STATUS                           Status;
  H2O_BDS_LOAD_OPTION                  *LoadOption;
  UINT32                               Attributes;

  if (DevicePath == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Attributes = LOAD_OPTION_ACTIVE;
  if (IsPreOsApp (DevicePath)) {
    Attributes |= LOAD_OPTION_CATEGORY_APP;
  }

  Status = gBdsServices->CreateLoadOption (
                           gBdsServices,
                           BOOT_OPTION,
                           NULL,
                           NULL,
                           Attributes,
                           DevicePath,
                           NULL,
                           OptionalData,
                           OptionalDataSize,
                           &LoadOption
                           );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = LaunchLoadOption (LoadOption);
  gBdsServices->FreeLoadOption (gBdsServices, LoadOption);
  return Status;
}

/**
 Register a notification function for a particular keystroke for the input device.
 It will separately register keystroke if key shift state contains both right and left states.

 @param[in]  SimpleTextInEx           Protocol instance pointer.
 @param[in]  KeyData                  A pointer to a buffer that is filled in with the keystroke
                                      information data for the key that was pressed.
 @param[in]  KeyNotifyFunction        Points to the function to be called when the key sequence is typed specified by KeyData.
 @param[out] NotifyHandle             Points to the unique handle assigned to the registered notification.

 @retval EFI_SUCCESS            The notification function was registered successfully.
 @retval EFI_OUT_OF_RESOURCES   Unable to allocate resources for necesssary data structures.
 @retval EFI_INVALID_PARAMETER  SimpleTextInEx, KeyData or NotifyHandle is NULL.
**/
STATIC
EFI_STATUS
SimpleTextInRegisterKeyNotify (
  IN  EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL     *SimpleTextInEx,
  IN  EFI_KEY_DATA                          *KeyData,
  IN  EFI_KEY_NOTIFY_FUNCTION               KeyNotifyFunction,
  OUT EFI_HANDLE                            *NotifyHandle
  )
{
  EFI_KEY_DATA                              *KeyDataList;
  UINTN                                     KeyDataCount;
  UINTN                                     Index;
  UINTN                                     InfoIndex;
  UINT32                                    BothSideShiftState;

  if (SimpleTextInEx == NULL || KeyData == NULL || KeyNotifyFunction == NULL || NotifyHandle == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  KeyDataCount = 1;
  KeyDataList  = AllocateCopyPool (sizeof(EFI_KEY_DATA), KeyData);
  if (KeyDataList == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // Check if key shift state contains both right and left states. If yes, it should separately register.
  //
  for (InfoIndex = 0; InfoIndex < sizeof(mBothSideShiftStateList) / sizeof(BOTH_SIDE_SHIFT_STATE_INFO); InfoIndex++) {
    BothSideShiftState = (mBothSideShiftStateList[InfoIndex].RightShiftState |
                          mBothSideShiftStateList[InfoIndex].LeftShiftState);

    if ((KeyData->KeyState.KeyShiftState & BothSideShiftState) == BothSideShiftState) {
      KeyDataList = ReallocatePool (
                      sizeof(EFI_KEY_DATA) * KeyDataCount,
                      sizeof(EFI_KEY_DATA) * KeyDataCount * 2,
                      KeyDataList
                      );
      if (KeyDataList == NULL) {
        return EFI_OUT_OF_RESOURCES;
      }
      CopyMem (&KeyDataList[KeyDataCount], KeyDataList, sizeof(EFI_KEY_DATA) * KeyDataCount);

      for (Index = 0; Index < KeyDataCount; Index++) {
        KeyDataList[Index].KeyState.KeyShiftState &= (~ mBothSideShiftStateList[InfoIndex].RightShiftState);
      }
      KeyDataCount *= 2;
      for (; Index < KeyDataCount; Index++) {
        KeyDataList[Index].KeyState.KeyShiftState &= (~ mBothSideShiftStateList[InfoIndex].LeftShiftState);
      }
    }
  }

  for (Index = 0; Index < KeyDataCount; Index++) {
    KeyDataList[Index].KeyState.KeyToggleState = 0;
    SimpleTextInEx->RegisterKeyNotify (
                      SimpleTextInEx,
                      &KeyDataList[Index],
                      KeyNotifyFunction,
                      NotifyHandle
                      );
  }

  FreePool (KeyDataList);

  return EFI_SUCCESS;
}

/**
 Call EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL.Reset funtion to clean keyboard buffer.
**/
STATIC
VOID
SimpleTextInResetKeyBuffer (
  VOID
  )
{
  EFI_STATUS                           Status;
  EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL    *SimpleTextInEx;

  Status = gBS->HandleProtocol (
                  gST->ConsoleInHandle,
                  &gEfiSimpleTextInputExProtocolGuid,
                  (VOID **) &SimpleTextInEx
                  );
  if (!EFI_ERROR (Status)) {
    SimpleTextInEx->Reset (SimpleTextInEx, FALSE);
  }
}

/**
 Callback function for SimpleTextInEx protocol install events.
 Register key notify when system detect a new input device.

 @param[in] Event                      The event that is signaled.
 @param[in] Context                    Pointer to the context data.
**/
STATIC
VOID
SimpleTextInEventFn (
  IN EFI_EVENT                         Event,
  IN VOID                              *Context
  )
{
  EFI_STATUS                           Status;
  UINTN                                HandleCount;
  EFI_HANDLE                           *HandleList;
  EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL    *SimpleTextInEx;
  EFI_DEVICE_PATH_PROTOCOL             *DevicePath;
  UINTN                                Index;
  UINTN                                RecordIndex;

  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiSimpleTextInputExProtocolGuid,
                  NULL,
                  &HandleCount,
                  &HandleList
                  );
  if (EFI_ERROR (Status) || HandleCount == 0) {
    return;
  }

  for (Index = 0; Index < HandleCount; Index++) {
    for (RecordIndex = 0; RecordIndex < mSimpleTextInCount; RecordIndex++) {
      if ((UINTN) mSimpleTextInList[RecordIndex] == (UINTN) HandleList[Index]) {
        break;
      }
    }
    if (RecordIndex < mSimpleTextInCount) {
      continue;
    }

    Status = gBS->HandleProtocol (HandleList[Index], &gEfiDevicePathProtocolGuid, (VOID **) &DevicePath);
    if (EFI_ERROR (Status)) {
      continue;
    }
    Status = gBS->HandleProtocol (HandleList[Index], &gEfiSimpleTextInputExProtocolGuid, (VOID **) &SimpleTextInEx);
    if (EFI_ERROR (Status)) {
      continue;
    }

    //
    // Find a new simple text input device. Register key notify in this device.
    //
    HotKeyRegisterKeyNotify (SimpleTextInEx);

    mSimpleTextInList = ReallocatePool (
                          sizeof(EFI_HANDLE) * mSimpleTextInCount,
                          sizeof(EFI_HANDLE) * (mSimpleTextInCount + 1),
                          mSimpleTextInList
                          );
    if (mSimpleTextInList == NULL) {
      mSimpleTextInCount = 0;
      return;
    }
    mSimpleTextInList[mSimpleTextInCount] = HandleList[Index];
    mSimpleTextInCount++;
  }

  FreePool (HandleList);
}

/**
 Boot to boot option of PCD hot key.

 @retval EFI_SUCCESS        Boot to boot option successfully.
 @retval Other              Launch return failed.
**/
STATIC
EFI_STATUS
PcdHotKeyBoot (
  IN H2O_HOT_KEY_OPTION                *HotKeyOption
  )
{
  EFI_STATUS                           Status;
  EFI_STATUS                           LaunchStatus;
  H2O_BDS_CONTEXT                      BdsContext;
  H2O_BDS_HOT_KEY_PROTOCOL             *HotKeyProtocol;

  LaunchStatus = EFI_NOT_FOUND;

  if (!CompareGuid (&HotKeyOption->TypeValue.HotKeyProtocolGuid, &gZeroGuid)) {
    Status = gBS->LocateProtocol (&HotKeyOption->TypeValue.HotKeyProtocolGuid, NULL, (VOID **) &HotKeyProtocol);
    if (!EFI_ERROR (Status) &&
        HotKeyProtocol != NULL &&
        HotKeyProtocol->Size > (OFFSET_OF (H2O_BDS_HOT_KEY_PROTOCOL, Launch)) &&
        HotKeyProtocol->Launch != NULL) {
      GetBdsContext (&BdsContext);
      LaunchStatus = HotKeyProtocol->Launch (
                                       HotKeyProtocol,
                                       HotKeyOption->Id,
                                       HotKeyOption->Options,
                                       &BdsContext
                                       );
    }
  }

  if (EFI_ERROR (LaunchStatus) && HotKeyOption->DevicePath != NULL) {
    LaunchStatus = LaunchDevicePath (
                     HotKeyOption->DevicePath,
                     (UINT8 *) HotKeyOption->Options,
                     (HotKeyOption->Options == NULL) ? 0 : (UINT32) AsciiStrSize (HotKeyOption->Options)
                     );
  }

  return LaunchStatus;
}

/**
 Create H2O hot key option by PCD token number.

 @param[in] PcdTokenNum                Target hot key PCD token number.

 @return Pointer to the H2O hot key option or NULL if PCD data is invalid or allocate memory failed.
**/
STATIC
H2O_HOT_KEY_OPTION *
PcdHotKeyCreateOption (
  IN UINTN                             PcdTokenNum
  )
{
  PCD_H2O_HOT_KEY                      *PcdHotKey;
  UINTN                                PcdHotKeySize;
  UINT8                                *PcdHotKeyPtr;
  UINTN                                CurrentSize;
  CHAR8                                *KeyAsciiStr;
  UINTN                                KeyAsciiStrSize;
  CHAR8                                *OptionAsciiStr;
  UINTN                                OptionAsciiStrSize;
  CHAR8                                *DevPathAsciiStr;
  EFI_STATUS                           Status;
  UINT32                               ChordCount;
  H2O_HOT_KEY_CHORD                    *ChordList;
  H2O_HOT_KEY_OPTION                   *HotKeyOption;

  PcdHotKey = (PCD_H2O_HOT_KEY *) LibPcdGetExPtr (&gH2OBdsHotKeyGuid, PcdTokenNum);
  if (PcdHotKey == NULL) {
    return NULL;
  }

  //
  // Make sure PCD hot key data contain hot key string.
  //
  PcdHotKeySize = LibPcdGetExSize (&gH2OBdsHotKeyGuid, PcdTokenNum);
  if (PcdHotKeySize <= (sizeof(PCD_H2O_HOT_KEY) + sizeof(CHAR8))) {
    return NULL;
  }

  //
  // Pasre PCD data by PCD_H2O_HOT_KEY definition.
  //
  PcdHotKeyPtr        = ((UINT8 *) PcdHotKey) + sizeof(PCD_H2O_HOT_KEY);
  CurrentSize         = sizeof(PCD_H2O_HOT_KEY);

  KeyAsciiStr         = (CHAR8 *) PcdHotKeyPtr;
  KeyAsciiStrSize     = AsciiStrSize (KeyAsciiStr);
  PcdHotKeyPtr       += KeyAsciiStrSize;
  CurrentSize        += KeyAsciiStrSize;

  OptionAsciiStr      = (PcdHotKeySize > CurrentSize) ? (CHAR8 *) PcdHotKeyPtr : NULL;
  OptionAsciiStrSize  = (OptionAsciiStr != NULL) ? AsciiStrSize (OptionAsciiStr) : 0;
  PcdHotKeyPtr       += OptionAsciiStrSize;
  CurrentSize        += OptionAsciiStrSize;

  DevPathAsciiStr     = (PcdHotKeySize > CurrentSize) ? (CHAR8 *) PcdHotKeyPtr : NULL;

  Status = GetChordList (KeyAsciiStr, &ChordList, &ChordCount);
  if (EFI_ERROR (Status)) {
    return NULL;
  }

  HotKeyOption = AllocateZeroPool (sizeof(H2O_HOT_KEY_OPTION));
  if (HotKeyOption == NULL) {
    FreeChordList (ChordList, ChordCount);
    return NULL;
  }

  HotKeyOption->Signature        = H2O_BDS_HOT_KEY_SIGNATURE;
  HotKeyOption->Type             = PCD_HOT_KEY;
  HotKeyOption->PcdTokenNum      = PcdTokenNum;
  HotKeyOption->Id               = PcdHotKey->Id;
  HotKeyOption->HotKeyChordCount = ChordCount;
  HotKeyOption->HotKeyChordList  = ChordList;
  HotKeyOption->Options          = (OptionAsciiStr != NULL) ? AllocateCopyPool (OptionAsciiStrSize, OptionAsciiStr) : NULL;
  HotKeyOption->DevicePath       = (DevPathAsciiStr != NULL) ? GetDevicePathByAsciiStr (DevPathAsciiStr) : NULL;
  CopyGuid (
    &HotKeyOption->TypeValue.HotKeyProtocolGuid,
    &PcdHotKey->HotKeyProtocolGuid
    );

  return HotKeyOption;
}

/**
 Enumerate H2O BDS PCD hot key.

 @retval EFI_SUCCESS  Enumerate H2O BDS PCD hot key successfully.
 @retval Other        Fail to get H2O BDS PCD token number list.
**/
STATIC
EFI_STATUS
PcdHotKeyEnumeration (
  VOID
  )
{
  EFI_STATUS                           Status;
  UINTN                                *TokenList;
  UINTN                                TokenCount;
  UINTN                                Index;
  H2O_HOT_KEY_OPTION                   *HotKeyOption;

  Status = GetTokenNumList (&gH2OBdsHotKeyGuid, &TokenList, &TokenCount);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if (TokenCount > 1) {
    PerformQuickSort (TokenList, TokenCount, sizeof(UINTN), ValueCompare);
  }

  for (Index = 0; Index < TokenCount; Index++) {
    HotKeyOption = PcdHotKeyCreateOption (TokenList[Index]);
    if (HotKeyOption != NULL) {
      InsertTailList (&mBdsHotKeyList, &HotKeyOption->Link);
    }
  }

  if (TokenList != NULL) {
    FreePool (TokenList);
  }

  return EFI_SUCCESS;
}

/**
 Boot to boot option of UEFI hot key.

 @retval EFI_SUCCESS        Boot to boot option successfully.
 @retval Other              Covert variable to option failed.
**/
STATIC
EFI_STATUS
UefiHotKeyBoot (
  IN H2O_HOT_KEY_OPTION                *HotKeyOption
  )
{
  EFI_STATUS                           Status;
  UINT16                               BootOptionName[10];
  H2O_BDS_LOAD_OPTION                  *LoadOption;

  UnicodeSPrint (BootOptionName, sizeof(BootOptionName), L"Boot%04x", HotKeyOption->TypeValue.BootOptionNum);
  Status = gBdsServices->ConvertVarToLoadOption (
                           gBdsServices,
                           BootOptionName,
                           &gEfiGlobalVariableGuid,
                           &LoadOption
                           );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  LaunchLoadOption (LoadOption);
  gBdsServices->FreeLoadOption (gBdsServices, LoadOption);

  return EFI_SUCCESS;
}

/**
 Create H2O hot key option from the UEFI key option.

 @param[in] KeyOption                  The pointer to the UEFI key option.

 @return Pointer to the H2O hot key option or NULL if allocate pool failed.
**/
STATIC
H2O_HOT_KEY_OPTION *
UefiHotKeyCreateOption (
  IN EFI_KEY_OPTION                    *KeyOption
  )
{
  UINTN                                Index;
  EFI_BOOT_KEY_DATA                    KeyData;
  UINT32                               KeyShiftState;
  EFI_INPUT_KEY                        *InputKey;
  H2O_HOT_KEY_OPTION                   *HotKeyOption;
  H2O_HOT_KEY_CHORD                    *HotKeyChord;

  if (KeyOption == NULL) {
    return NULL;
  }

  //
  // Map key shift state from KeyOptions to EFI_KEY_DATA.KeyState
  //
  KeyData = KeyOption->KeyData;

  KeyShiftState = EFI_SHIFT_STATE_VALID;
  KeyShiftState |= (KeyData.Options.ShiftPressed   ? (EFI_RIGHT_SHIFT_PRESSED   | EFI_LEFT_SHIFT_PRESSED  ) : 0);
  KeyShiftState |= (KeyData.Options.ControlPressed ? (EFI_RIGHT_CONTROL_PRESSED | EFI_LEFT_CONTROL_PRESSED) : 0);
  KeyShiftState |= (KeyData.Options.AltPressed     ? (EFI_RIGHT_ALT_PRESSED     | EFI_LEFT_ALT_PRESSED    ) : 0);
  KeyShiftState |= (KeyData.Options.LogoPressed    ? (EFI_RIGHT_LOGO_PRESSED    | EFI_LEFT_LOGO_PRESSED   ) : 0);
  KeyShiftState |= (KeyData.Options.MenuPressed    ? EFI_MENU_KEY_PRESSED : 0);
  KeyShiftState |= (KeyData.Options.SysReqPressed  ? EFI_SYS_REQ_PRESSED  : 0);

  HotKeyOption = AllocateZeroPool (sizeof(H2O_HOT_KEY_OPTION));
  if (HotKeyOption == NULL) {
    return NULL;
  }

  HotKeyOption->Signature               = H2O_BDS_HOT_KEY_SIGNATURE;
  HotKeyOption->Type                    = UEFI_HOT_KEY;
  HotKeyOption->TypeValue.BootOptionNum = KeyOption->BootOption;
  HotKeyOption->Options                 = NULL;
  HotKeyOption->HotKeyChordCount        = 1;
  HotKeyOption->HotKeyChordList         = AllocateZeroPool (HotKeyOption->HotKeyChordCount * sizeof(H2O_HOT_KEY_CHORD));
  if (HotKeyOption->HotKeyChordList == NULL) {
    FreePool (HotKeyOption);
    return NULL;
  }

  HotKeyChord           = HotKeyOption->HotKeyChordList;
  HotKeyChord->KeyCount = (UINT32) KeyData.Options.InputKeyCount;
  HotKeyChord->KeyList  = AllocateZeroPool (HotKeyChord->KeyCount * sizeof(EFI_KEY_DATA));
  if (HotKeyChord->KeyList == NULL) {
    FreePool (HotKeyOption->HotKeyChordList);
    FreePool (HotKeyOption);
    return NULL;
  }

  Index = 0;
  InputKey = (EFI_INPUT_KEY *) (((UINT8 *) KeyOption) + sizeof(EFI_KEY_OPTION));
  do {
    //
    // If Key CodeCount is 0, then only KeyData[0] is used;
    // if Key CodeCount is n, then KeyData[0]~KeyData[n-1] are used
    //
    HotKeyChord->KeyList[Index].Key.ScanCode           = InputKey[Index].ScanCode;
    HotKeyChord->KeyList[Index].Key.UnicodeChar        = InputKey[Index].UnicodeChar;
    HotKeyChord->KeyList[Index].KeyState.KeyShiftState = KeyShiftState;

    Index++;
  } while (Index < HotKeyChord->KeyCount);
  HotKeyChord->ChordAsciiStr = GetChordAsciiStr (HotKeyChord->KeyList, HotKeyChord->KeyCount);

  return HotKeyOption;
}

/**
 Enumerate UEFI hot key.

 @retval EFI_SUCCESS  Enumerate UEFI hot key successfully.
 @retval Other        Fail to get UEFI hot key list.
**/
STATIC
EFI_STATUS
UefiHotKeyEnumeration (
  VOID
  )
{
  EFI_STATUS                           Status;
  UINT16                               *KeyOptionNumList;
  UINT16                               KeyOptionNumCount;
  UINT16                               Index;
  CHAR16                               KeyOptionName[8];
  EFI_KEY_OPTION                       *KeyOption;
  H2O_HOT_KEY_OPTION                   *HotKeyOption;

  //
  // Enumerate Key#### variable and add it in hot key list if verification is success.
  //
  KeyOptionNumList  = NULL;
  KeyOptionNumCount = 0;
  Status = GetKeyOptionNumList (&KeyOptionNumList, &KeyOptionNumCount);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  for (Index = 0; Index < KeyOptionNumCount; Index++) {
    UnicodeSPrint (KeyOptionName, sizeof(KeyOptionName), L"Key%04x", (UINT16) KeyOptionNumList[Index]);
    KeyOption = CommonGetVariableData (KeyOptionName, &gEfiGlobalVariableGuid);
    if (KeyOption == NULL) {
      continue;
    }

    if (IsKeyOptionValid (KeyOption)) {
      HotKeyOption = UefiHotKeyCreateOption (KeyOption);
      if (HotKeyOption != NULL) {
        InsertTailList (&mBdsHotKeyList, &HotKeyOption->Link);
      }
    }

    FreePool (KeyOption);
  }

  FreePool (KeyOptionNumList);
  return EFI_SUCCESS;
}

/**
 Set BootOptionSupport variable to export the capability of UEFI hot key.

 @retval EFI_SUCCESS        Set BootOptionSupport variable successfully.
 @retval Other              Set BootOptionSupport variable failed.
**/
STATIC
EFI_STATUS
UefiHotKeyExportCapability (
  VOID
  )
{
  EFI_STATUS                           Status;
  UINT32                               BootOptionSupport;
  UINTN                                Size;

  //
  // Export our capability - EFI_BOOT_OPTION_SUPPORT_KEY with maximum number of key presses of 3
  //
  Size = sizeof(BootOptionSupport);
  Status = gRT->GetVariable (
                  EFI_BOOT_OPTION_SUPPORT_VARIABLE_NAME,
                  &gEfiGlobalVariableGuid,
                  NULL,
                  &Size,
                  &BootOptionSupport
                  );
  if (EFI_ERROR (Status)) {
    BootOptionSupport = 0;
  }
  BootOptionSupport |= EFI_BOOT_OPTION_SUPPORT_KEY;
  SET_BOOT_OPTION_SUPPORT_KEY_COUNT (BootOptionSupport, 3);

  return gRT->SetVariable (
                EFI_BOOT_OPTION_SUPPORT_VARIABLE_NAME,
                &gEfiGlobalVariableGuid,
                EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                sizeof(BootOptionSupport),
                &BootOptionSupport
                );
}

/**
 Internal function to initialize H2O_BDS_CP_HOT_KEY_DETECTED_DATA data and trigger PcdH2OBdsCpHotKeyDetectedSupported checkpoint.

 @retval EFI_SUCCESS             Trigger PcdH2OBdsCpHotKeyDetectedSupported checkpoint successfully.
 @return Other                   Other error occurred while triggering PcdH2OBdsCpHotKeyDetectedSupported checkpoint.
**/
STATIC
EFI_STATUS
TriggerCpHotKeyDetected (
  IN CHAR8                             *HotKeyAsciiStr
  )
{
  H2O_BDS_CP_HOT_KEY_DETECTED_DATA     HotKeyDetectedData;
  EFI_STATUS                           Status;

  HotKeyDetectedData.Size   = sizeof (H2O_BDS_CP_HOT_KEY_DETECTED_DATA);
  HotKeyDetectedData.Status = H2O_CP_TASK_NORMAL;
  HotKeyDetectedData.HotKey = HotKeyAsciiStr;

  DEBUG ((DEBUG_INFO, "Checkpoint Trigger: %g\n", &gH2OBdsCpHotKeyDetectedGuid));
  Status = H2OCpTrigger (&gH2OBdsCpHotKeyDetectedGuid, &HotKeyDetectedData);
  DEBUG ((DEBUG_INFO, "Checkpoint Result: %x\n", HotKeyDetectedData.Status));
  return Status;
}

/**
 Reset all hot key option in the list.
**/
STATIC
VOID
HotKeyResetHotKeyList (
  VOID
  )
{
  LIST_ENTRY                           *Link;
  H2O_HOT_KEY_OPTION                   *HotKeyOption;
  UINT32                               Index;

  Link = GetFirstNode (&mBdsHotKeyList);
  while (!IsNull (&mBdsHotKeyList, Link)) {
    HotKeyOption = H2O_HOT_KEY_OPTION_FROM_LINK (Link);
    Link         = GetNextNode (&mBdsHotKeyList, Link);

    for (Index = 0; Index < HotKeyOption->HotKeyChordCount; Index++) {
      HotKeyOption->HotKeyChordList[Index].CurrentIndex = 0;
    }
  }
}

/**
 Registered key notify function to check if hot key is pressed.

 @param[in] KeyData    Pointer to buffer that is filled in with the keystroke information for the key that was pressed.

 @return EFI_SUCCESS   KeyData is successfully processed.
**/
STATIC
EFI_STATUS
HotKeyKeyNotifyFunc (
  IN EFI_KEY_DATA                      *KeyData
  )
{
  LIST_ENTRY                           *Link;
  UINT32                               Index;
  H2O_HOT_KEY_OPTION                   *HotKeyOption;
  BOOLEAN                              HotKeyFound;
  H2O_HOT_KEY_CHORD                    *HotKeyChord;
  EFI_KEY_DATA                         *ChordKeyData;
  CHAR8                                *HotKeyAsciiStr;

  if (!mHotKeyEnabled) {
    return EFI_SUCCESS;
  }

  if (mSelectedHotKeyOption != NULL) {
    //
    // Do not process sequential hotkey stroke until the previous selected hot key is processed.
    //
    return EFI_SUCCESS;
  }

  HotKeyAsciiStr = NULL;

  Link = GetFirstNode (&mBdsHotKeyList);
  while (!IsNull (&mBdsHotKeyList, Link)) {
    HotKeyOption = H2O_HOT_KEY_OPTION_FROM_LINK (Link);
    Link         = GetNextNode (&mBdsHotKeyList, Link);

    for (Index = 0; Index < HotKeyOption->HotKeyChordCount; Index++) {
      HotKeyFound  = FALSE;
      HotKeyChord  = &HotKeyOption->HotKeyChordList[Index];
      ChordKeyData = &HotKeyChord->KeyList[HotKeyChord->CurrentIndex];

      if (IsKeyMatch (ChordKeyData, KeyData)) {
        //
        // For hotkey of key combination, transit to next waiting state
        //
        HotKeyChord->CurrentIndex++;

        if (HotKeyChord->CurrentIndex == HotKeyChord->KeyCount) {
          //
          // Received the whole key stroke sequence
          //
          HotKeyFound = TRUE;
        }
      } else {
        //
        // Receive an unexpected key stroke, reset to initial waiting state
        //
        HotKeyChord->CurrentIndex = 0;
      }

      if (HotKeyFound) {
        //
        // Reset to initial waiting state
        //
        HotKeyChord->CurrentIndex = 0;

        //
        // Launch its BootOption
        //
        mSelectedHotKeyOption = HotKeyOption;
        HotKeyAsciiStr        = HotKeyChord->ChordAsciiStr;
        break;
      }
    }
  }

  if (mSelectedHotKeyOption != NULL) {
    if (FeaturePcdGet (PcdH2OBdsCpHotKeyDetectedSupported)) {
      TriggerCpHotKeyDetected (HotKeyAsciiStr);
    }
    if (BdsLibIsBootOrderHookEnabled ()) {
      BdsLibRestoreBootOrderFromPhysicalBootOrder ();
    }
    HotKeyResetHotKeyList ();
    DisplayHotKeyAfterUserSelection ();
  }

  return EFI_SUCCESS;
}

/**
 Register the hot key notify function to given SimpleTextInEx protocol instance.

 @param[in] SimpleTextInEx             Simple Text Input Ex protocol instance
**/
STATIC
VOID
HotKeyRegisterKeyNotify (
  IN EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL *SimpleTextInEx
  )
{
  LIST_ENTRY                           *Link;
  H2O_HOT_KEY_OPTION                   *HotKeyOption;
  UINTN                                HotKeyIndex;
  UINTN                                Index;
  H2O_HOT_KEY_CHORD                    *HotKeyChord;
  VOID                                 *NotifyHandle;

  Link = GetFirstNode (&mBdsHotKeyList);
  while (!IsNull (&mBdsHotKeyList, Link)) {
    HotKeyOption = H2O_HOT_KEY_OPTION_FROM_LINK (Link);
    Link         = GetNextNode (&mBdsHotKeyList, Link);

    for (HotKeyIndex = 0; HotKeyIndex < HotKeyOption->HotKeyChordCount; HotKeyIndex++) {
      HotKeyChord = &HotKeyOption->HotKeyChordList[HotKeyIndex];

      for (Index = 0; Index < HotKeyChord->KeyCount; Index++) {
        SimpleTextInRegisterKeyNotify (
          SimpleTextInEx,
          &HotKeyChord->KeyList[Index],
          HotKeyKeyNotifyFunc,
          &NotifyHandle
          );
      }
    }
  }
}

/**
 Register key notification in simple text input device for input hot key option.

 @param[in] HotKeyOption               Pointer to hot key option
**/
STATIC
VOID
HotKeyRegisterOptionHotKey (
  IN H2O_HOT_KEY_OPTION                *HotKeyOption
  )
{
  EFI_STATUS                           Status;
  EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL    *SimpleTextInEx;
  UINTN                                Index;
  UINTN                                ChordIndex;
  UINTN                                KeyIndex;
  H2O_HOT_KEY_CHORD                    *Chord;
  VOID                                 *NotifyHandle;

  for (Index = 0; Index < mSimpleTextInCount; Index++) {
    Status = gBS->HandleProtocol (mSimpleTextInList[Index], &gEfiSimpleTextInputExProtocolGuid, (VOID **) &SimpleTextInEx);
    if (EFI_ERROR (Status)) {
      continue;
    }

    for (ChordIndex = 0; ChordIndex < HotKeyOption->HotKeyChordCount; ChordIndex++) {
      Chord = &HotKeyOption->HotKeyChordList[ChordIndex];

      for (KeyIndex = 0; KeyIndex < Chord->KeyCount; KeyIndex++) {
        SimpleTextInRegisterKeyNotify (
          SimpleTextInEx,
          &Chord->KeyList[KeyIndex],
          HotKeyKeyNotifyFunc,
          &NotifyHandle
          );
      }
    }
  }
}

/**
 Callback function to disable BDS hot key functionality

 @param[in] Event                      The event that is signaled.
 @param[in] Context                    Pointer to the context data.
**/
STATIC
VOID
HotKeyStopHotKeyEventtFn (
  IN EFI_EVENT                         Event,
  IN VOID                              *Context
  )
{
  EFI_STATUS                           Status;
  UINT8                                *Instance;

  Status = gBS->LocateProtocol (&gH2OStopHotKeyGuid, NULL, (VOID **) &Instance);
  if (EFI_ERROR (Status)) {
    return;
  }

  EnableBdsHotKey (FALSE);
  gBS->CloseEvent (Event);
}

/**
 Turn on hot ket detection at Boot after check point.

 @param[in] Event                      The Event this notify function registered to.
 @param[in] Handle                     The handle associated with a previously registered checkpoint handler.
**/
STATIC
VOID
EFIAPI
HotKeyBootAfterCpHandler (
  IN EFI_EVENT                         Event,
  IN H2O_CP_HANDLE                     Handle
  )
{
  EnableBdsHotKey (TRUE);
  H2OCpUnregisterHandler (Handle);
}

/**
 Turn off hot key detection at Boot before check point.

 @param[in] Event                      The Event this notify function registered to.
 @param[in] Handle                     The handle associated with a previously registered checkpoint handler.
**/
STATIC
VOID
EFIAPI
HotKeyBootBeforeCpHandler (
  IN EFI_EVENT                         Event,
  IN H2O_CP_HANDLE                     Handle
  )
{
  BOOLEAN                              PreviousValue;
  H2O_CP_HANDLE                        CpHandle;
  EFI_STATUS                           Status;

  PreviousValue = EnableBdsHotKey (FALSE);
  if (FeaturePcdGet (PcdH2OBdsCpBootAfterSupported) && PreviousValue) {
    //
    // Previous hot key status is enabled, it should restore hot key status to enabled after this boot.
    //
    Status = H2OCpRegisterHandler (&gH2OBdsCpBootAfterGuid , HotKeyBootAfterCpHandler , H2O_CP_MEDIUM, &CpHandle);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "Checkpoint Register Fail: %g (%r)\n", &gH2OBdsCpBootAfterGuid, Status));
      return;
    }
    DEBUG ((DEBUG_INFO, "Checkpoint Registered: %g (%r)\n", &gH2OBdsCpBootAfterGuid, Status));
  }
}

/**
 Based on Timeout variable to register timer event. Guarantee that user have enough time to press hot key.

 @param[in] Event                      The Event this notify function registered to.
 @param[in] Handle                     The handle associated with a previously registered checkpoint handler.
**/
STATIC
VOID
EFIAPI
HotKeyDisplayBeforeCpHandler (
  IN EFI_EVENT                         Event,
  IN H2O_CP_HANDLE                     Handle
  )
{
  UINT16                               Timeout;
  EFI_STATUS                           Status;
  UINTN                                Size;
  UINT64                               TimeOutMs;

  if (!mHotKeyEnabled || mHotKeyTimeoutEvent != NULL) {
    return;
  }

  Size   = sizeof (Timeout);
  Status = gRT->GetVariable (L"Timeout", &gEfiGlobalVariableGuid, NULL, &Size, &Timeout);
  if (EFI_ERROR(Status)) {
    Timeout = 0;
  }

  TimeOutMs = MultU64x32 ((UINT64) Timeout, 1000) + PcdGet32 (PcdPlatformBootTimeOutMs);
  if (TimeOutMs == 0) {
    return;
  }

  Status = gBS->CreateEvent (
                  EVT_TIMER,
                  TPL_NOTIFY,
                  NULL,
                  NULL,
                  &mHotKeyTimeoutEvent
                  );
  if (!EFI_ERROR(Status)) {
    Status = gBS->SetTimer (mHotKeyTimeoutEvent, TimerRelative, MultU64x32 (TimeOutMs, TIMER_EVENT_ONE_MILLISEC));
  }
}

/**
 Initialize BDS hot key.

 @retval EFI_SUCCESS    Initialize BDS hot key successfully.
 @retval Other          Locate protocol failed.
**/
EFI_STATUS
BdsHotKeyInit (
  VOID
  )
{
  EFI_STATUS                           Status;
  VOID                                 *Registration;
  H2O_CP_HANDLE                        CpHandle;

  Status = gBS->LocateProtocol (&gH2OKeyDescProtocolGuid, NULL, (VOID **) &mH2OKeyDesc);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  mHotKeySupport = TRUE;
  UefiHotKeyExportCapability ();

  UefiHotKeyEnumeration ();
  PcdHotKeyEnumeration ();

  BdsHotKeyDescInit ();

  //
  // Register protocol notify to register key notify when system detects a new input device.
  //
  EfiCreateProtocolNotifyEvent (
    &gEfiSimpleTextInputExProtocolGuid,
    TPL_CALLBACK,
    SimpleTextInEventFn,
    NULL,
    &Registration
    );

  //
  // Register protocol notify to disable hot key detection for compatibility.
  //
  EfiCreateProtocolNotifyEvent (
    &gH2OStopHotKeyGuid,
    TPL_CALLBACK,
    HotKeyStopHotKeyEventtFn,
    NULL,
    &Registration
    );

  if (FeaturePcdGet (PcdH2OBdsCpDisplayBeforeSupported)) {
    //
    // Register display before check point to record the start time.
    // In order to make sure that user have enough time (defined by Timeout variable) to press hot key.
    // Register a timer event at display before checkpoint and check the signal status of event in BdsHotKeyBoot().
    // Guarantee that the period of display before checkpoint to BdsHotKeyBoot() exceeds Timeout variable value.
    //
    Status = H2OCpRegisterHandler (
               &gH2OBdsCpDisplayBeforeGuid,
               HotKeyDisplayBeforeCpHandler,
               H2O_CP_MEDIUM,
               &CpHandle
               );
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "Checkpoint Register Fail: %g (%r)\n", &gH2OBdsCpDisplayBeforeGuid, Status));
      return Status;
    }
    DEBUG ((DEBUG_INFO, "Checkpoint Registered: %g (%r)\n", &gH2OBdsCpDisplayBeforeGuid, Status));
  }
  if (FeaturePcdGet (PcdH2OBdsCpBootBeforeSupported)) {
    //
    // Register boot before check point to turn off hot key detection.
    //
    Status = H2OCpRegisterHandler (
               &gH2OBdsCpBootBeforeGuid,
               HotKeyBootBeforeCpHandler,
               H2O_CP_MEDIUM,
               &CpHandle
               );
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "Checkpoint Register Fail: %g (%r)\n", &gH2OBdsCpBootBeforeGuid, Status));
      return Status;
    }
    DEBUG ((DEBUG_INFO, "Checkpoint Registered: %g (%r)\n", &gH2OBdsCpBootBeforeGuid, Status));
  }

  return EFI_SUCCESS;
}

/**
 Boot to hot key option if there is one option which key is detected.

 @param[in] TriggerPoint    Trigger point determine what kinds of hot keys are permitted to be performed.

 @retval EFI_SUCCESS        Boot to hot key option successfully.
 @retval EFI_NOT_FOUND      There is no hot key which ket is detected.
 @retval EFI_NOT_READY      Pressed hot key is detected but the option is not permitted to be performed now.
 @retval EFI_UNSUPPORTED    Hot key type is unsupport.
 @retval Other              Boot to hot key option failed.
**/
EFI_STATUS
BdsHotKeyBoot (
  IN UINT32                            TriggerPoint
  )
{
  EFI_STATUS                           Status;
  H2O_BDS_CONTEXT                      BdsContext;
  BOOLEAN                              PreviousValue;

  if (!mHotKeySupport) {
    return EFI_NOT_FOUND;
  }

  //
  // Make sure user has enough time (defined by Timeout variable) to press hot key.
  //
  while (mHotKeyTimeoutEvent != NULL) {
    Status = gBS->CheckEvent (mHotKeyTimeoutEvent);
    if (mSelectedHotKeyOption != NULL || IsHotKeyDetected () || Status != EFI_NOT_READY) {
      gBS->CloseEvent (mHotKeyTimeoutEvent);
      mHotKeyTimeoutEvent = NULL;
    } else {
      gBS->Stall (500);
    }
  }

  if (mSelectedHotKeyOption == NULL) {
    return EFI_NOT_FOUND;
  }

  //
  // Clean keyboard buffer to make sure previous key data doesn't impact on the launching function of hot key.
  //
  SimpleTextInResetKeyBuffer ();

  PreviousValue = EnableBdsHotKey (FALSE);
  switch (mSelectedHotKeyOption->Type) {

  case UEFI_HOT_KEY:
    if ((TriggerPoint & TRIGGER_POINT_BEFORE_CHECK_EACH_BOOT_OPTION) == 0) {
      EnableBdsHotKey (PreviousValue);
      return EFI_NOT_READY;
    }

    Status = UefiHotKeyBoot (mSelectedHotKeyOption);
    break;

  case PCD_HOT_KEY:
    Status = PcdHotKeyBoot (mSelectedHotKeyOption);
    break;

  case REGISTERED_CALLBACK_HOT_KEY:
    GetBdsContext (&BdsContext);
    Status = mSelectedHotKeyOption->TypeValue.HotKeyFn (
                                                NULL,
                                                mSelectedHotKeyOption->Id,
                                                mSelectedHotKeyOption->Options,
                                                &BdsContext
                                                );

    break;

  case REGISTERED_LOAD_OPTION_HOT_KEY:
    Status = LaunchLoadOption (mSelectedHotKeyOption->TypeValue.LoadOption);
    break;

  default:
    Status = EFI_UNSUPPORTED;
    break;
  }
  EnableBdsHotKey (PreviousValue);

  mSelectedHotKeyOption = NULL;

  return Status;
}

/**
 Enable or disable BDS detection of hot keys.
 While enabled, the Boot Manager will continue to check each key pressed against the list of registered hot keys.
 When disabled, any hot key chord detect is suspended and any in-process hot key chords wil be discarded.

 @param[in]  This               A Pointer to current instance of this protocol.
 @param[out] Enable             On entry, a pointer to a Boolean that specifies whether hot key processing should be
                                enabled (TRUE) or disabled (FALSE). On exit, a pointer to a Boolean that indicates
                                whether hot key processing was enabled (TRUE) or disabled (FALSE) before this call.

 @retval EFI_SUCCESS            This function completed successfully.
 @retval EFI_INVALID_PARAMETER  Input parameter is NULL.
**/
EFI_STATUS
EFIAPI
BdsServicesEnableHotKeys (
  IN     H2O_BDS_SERVICES_PROTOCOL     *This,
  IN OUT BOOLEAN                       *Enable
  )
{
  BOOLEAN                              OrgEnable;

  if (!mHotKeySupport) {
    return EFI_ABORTED;
  }

  if (This == NULL || Enable == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  OrgEnable = mHotKeyEnabled;
  mHotKeyEnabled = *Enable;

  if (OrgEnable && !mHotKeyEnabled) {
    HotKeyResetHotKeyList ();
  }

  *Enable = OrgEnable;

  return EFI_SUCCESS;
}

/**
 Register a function to be called when a specified hot key chord is detected.

 @param[in]  This               A pointer to the current instance of this protocol.
 @param[in]  HotKey             A pointer to a null-terminated ASCII string that specifies one or more hot key chords
                                for this hot key.
 @param[in]  HotKeyFn           Pointer to the function that will be called when the hot key chord specified by
                                HotKey is detected.
 @param[in]  Id                 Unsigned integer associated with this specific hot key.
 @param[in]  Options            Optional pointer to a null-terminated ASCII string that contains additional operating
                                parameters.
 @param[out] HotKeyHandle       On return, a pointer to a handle that uniquely specifies this hot key.

 @retval EFI_SUCCESS            Hot key registered.
 @retval EFI_INVALID_PARAMETER  The hot key string was NULL, or had an invalid format.
                                The hot key attributes had bits set that are unsupported.
 @retval EFI_ALREADY_STARTED    A hot key with one of the specified hot key chords has already been registered.
**/
EFI_STATUS
EFIAPI
BdsServicesRegisterCallbackHotKey (
  IN  H2O_BDS_SERVICES_PROTOCOL        *This,
  IN  CONST CHAR8                      *HotKey,
  IN  H2O_BDS_HOT_KEY_LAUNCH           HotKeyFn,
  IN  UINT32                           Id,
  IN  CONST CHAR8                      *Options OPTIONAL,
  OUT EFI_HANDLE                       *HotKeyHandle
  )
{
  EFI_STATUS                           Status;
  UINT32                               Index;
  UINT32                               ChordCount;
  H2O_HOT_KEY_CHORD                    *ChordList;
  H2O_HOT_KEY_OPTION                   *HotKeyOption;

  if (!mHotKeySupport) {
    return EFI_ABORTED;
  }

  if (This == NULL || HotKey == NULL || HotKeyFn == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = GetChordList ((CHAR8 *) HotKey, &ChordList, &ChordCount);
  if (EFI_ERROR (Status)) {
    return EFI_INVALID_PARAMETER;
  }
  for (Index = 0; Index < ChordCount; Index++) {
    if (IsChordRegistered (&ChordList[Index])) {
      DEBUG((EFI_D_INFO, "BdsHotKey: Number %d of hot key (%a) is already registered before.\n", Index + 1, HotKey));
      FreePool (ChordList);
      return EFI_ALREADY_STARTED;
    }
  }

  HotKeyOption = AllocateZeroPool (sizeof(H2O_HOT_KEY_OPTION));
  if (HotKeyOption == NULL) {
    FreePool (ChordList);
    return EFI_OUT_OF_RESOURCES;
  }

  HotKeyOption->Signature          = H2O_BDS_HOT_KEY_SIGNATURE;
  HotKeyOption->Type               = REGISTERED_CALLBACK_HOT_KEY;
  HotKeyOption->TypeValue.HotKeyFn = HotKeyFn;
  HotKeyOption->Id                 = Id;
  HotKeyOption->HotKeyChordCount   = ChordCount;
  HotKeyOption->HotKeyChordList    = ChordList;
  HotKeyOption->Options            = (Options != NULL) ? AllocateCopyPool (AsciiStrSize (Options), Options) : NULL;
  CopyMem (&HotKeyOption->Description     , &mDefaultHotKeyDescPosn     , sizeof(H2O_HOT_KEY_DESCRIPTION));
  CopyMem (&HotKeyOption->QuietDescription, &mDefaultHotKeyDescQuietPosn, sizeof(H2O_HOT_KEY_DESCRIPTION));

  InsertTailList (&mBdsHotKeyList, &HotKeyOption->Link);
  HotKeyRegisterOptionHotKey (HotKeyOption);

  *HotKeyHandle = (EFI_HANDLE *) (UINTN) HotKeyOption;
  return EFI_SUCCESS;
}

/**
 Register a load option to invoke when a specified hot key chord is detected.

 @param[in]  This               A pointer to the current instance of this protocol.
 @param[in]  HotKey             A pointer to a null-terminated ASCII string that specifies one or more hot key chords
                                for this hot key.
 @param[in]  HotKeyLoadOption   A pointer to a load option that specifies the application to load when the hot key is
                                pressed, along with the load options to pass to that application.
 @param[out] HotKeyHandle       On return, a pointer to a handle that uniquely specifies this hot key.

 @retval EFI_SUCCESS            Hot key registered.
 @retval EFI_INVALID_PARAMETER  The hot key string was NULL, or had an invalid format.
                                The hot key attributes had bits set that are unsupported.
 @retval EFI_ALREADY_STARTED    A hot key with one of the specified hot key chords has already been registered.
**/
EFI_STATUS
EFIAPI
BdsServicesRegisterLoadOptionHotKey (
  IN  H2O_BDS_SERVICES_PROTOCOL        *This,
  IN  CONST CHAR8                      *HotKey,
  IN  CONST H2O_BDS_LOAD_OPTION        *HotKeyLoadOption,
  OUT EFI_HANDLE                       *HotKeyHandle
  )
{
  EFI_STATUS                           Status;
  UINT32                               Index;
  UINT32                               ChordCount;
  H2O_HOT_KEY_CHORD                    *ChordList;
  H2O_HOT_KEY_OPTION                   *HotKeyOption;

  if (!mHotKeySupport) {
    return EFI_ABORTED;
  }

  if (This == NULL || HotKey == NULL || HotKeyLoadOption == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = GetChordList ((CHAR8 *) HotKey, &ChordList, &ChordCount);
  if (EFI_ERROR (Status)) {
    return EFI_INVALID_PARAMETER;
  }
  for (Index = 0; Index < ChordCount; Index++) {
    if (IsChordRegistered (&ChordList[Index])) {
      DEBUG((EFI_D_INFO, "BdsHotKey: Number %d of hot key (%a) is already registered before.\n", Index + 1, HotKey));
      FreePool (ChordList);
      return EFI_ALREADY_STARTED;
    }
  }

  HotKeyOption = AllocateZeroPool (sizeof(H2O_HOT_KEY_OPTION));
  if (HotKeyOption == NULL) {
    FreePool (ChordList);
    return EFI_OUT_OF_RESOURCES;
  }

  HotKeyOption->Signature            = H2O_BDS_HOT_KEY_SIGNATURE;
  HotKeyOption->Type                 = REGISTERED_LOAD_OPTION_HOT_KEY;
  HotKeyOption->TypeValue.LoadOption = CopyLoadOption ((H2O_BDS_LOAD_OPTION *) HotKeyLoadOption);
  HotKeyOption->HotKeyChordCount     = ChordCount;
  HotKeyOption->HotKeyChordList      = ChordList;
  CopyMem (&HotKeyOption->Description     , &mDefaultHotKeyDescPosn     , sizeof(H2O_HOT_KEY_DESCRIPTION));
  CopyMem (&HotKeyOption->QuietDescription, &mDefaultHotKeyDescQuietPosn, sizeof(H2O_HOT_KEY_DESCRIPTION));

  InsertTailList (&mBdsHotKeyList, &HotKeyOption->Link);
  HotKeyRegisterOptionHotKey (HotKeyOption);

  *HotKeyHandle = (EFI_HANDLE *) (UINTN) HotKeyOption;
  return EFI_SUCCESS;
}

/**
 Unregister a previously registered hot key chord.

 @param[in] This                A pointer to the current instance of this protocol.
 @param[in] HotKeyHandle        Handle that specifies a previously registered hot key.

 @retval EFI_SUCCESS            Function completed successfully.
 @retval EFI_INVALID_PARAMETER  Hot key handle is invalid.
**/
EFI_STATUS
EFIAPI
BdsServicesUnregisterHotKey (
  IN H2O_BDS_SERVICES_PROTOCOL         *This,
  IN EFI_HANDLE                        HotKeyHandle
  )
{
  H2O_HOT_KEY_OPTION                   *HotKeyOption;

  if (This == NULL || HotKeyHandle == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  HotKeyOption = GetHotKeyOption (HotKeyHandle);
  if (HotKeyOption == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  switch (HotKeyOption->Type) {

  case REGISTERED_CALLBACK_HOT_KEY:
    break;

  case REGISTERED_LOAD_OPTION_HOT_KEY:
    if (HotKeyOption->TypeValue.LoadOption != NULL) {
      gBdsServices->FreeLoadOption (gBdsServices, HotKeyOption->TypeValue.LoadOption);
    }
    break;

  case UEFI_HOT_KEY:
  case PCD_HOT_KEY:
  default:
    //
    // UEFI hot key and PCD hot key should not be unregistered now, becuase caller should not get this Handle value.
    //
    return EFI_INVALID_PARAMETER;
  }

  RemoveEntryList (&HotKeyOption->Link);

  FreeChordList (HotKeyOption->HotKeyChordList, HotKeyOption->HotKeyChordCount);
  if (HotKeyOption->DevicePath != NULL) {
    FreePool (HotKeyOption->DevicePath);
  }
  if (HotKeyOption->Options != NULL) {
    FreePool (HotKeyOption->Options);
  }
  FreePool (HotKeyOption);

  return EFI_SUCCESS;
}

