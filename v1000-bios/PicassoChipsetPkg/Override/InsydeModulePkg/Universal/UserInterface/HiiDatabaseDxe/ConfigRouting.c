/** @file
 Implementation of interfaces function for EFI_HII_CONFIG_ROUTING_PROTOCOL.

;******************************************************************************
;* Copyright (c) 2012 - 2020, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

 Copyright (c) 2007 - 2012, Intel Corporation. All rights reserved.<BR>
 This program and the accompanying materials
 are licensed and made available under the terms and conditions of the BSD License
 which accompanies this distribution.  The full text of the license may be found at
 http://opensource.org/licenses/bsd-license.php

 THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
 WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

*/

#include "HiiDatabase.h"
extern HII_DATABASE_PRIVATE_DATA mPrivate;

#define STRLEN(x) ((sizeof (x) / sizeof (CHAR16)) - 1)
#define StrnCmp HiiStrnCmp
#define StrHexToUint64 HiiStrHexToUint64

INTN
EFIAPI
HiiStrnCmp (
  IN CONST CHAR16              *FirstString,
  IN CONST CHAR16              *SecondString,
  IN UINTN                     Length
  )
{
  if (Length == 0) {
    return 0;
  }

  //
  // ASSERT both strings are less long than PcdMaximumUnicodeStringLength.
  // Length tests are performed inside StrLen().
  //
//ASSERT (StrSize (FirstString) != 0);
//ASSERT (StrSize (SecondString) != 0);
//
//  if (PcdGet32 (PcdMaximumUnicodeStringLength) != 0) {
//    ASSERT (Length <= PcdGet32 (PcdMaximumUnicodeStringLength));
//  }
  ASSERT (FirstString != NULL);
  ASSERT (SecondString != NULL);

  while ((*FirstString != L'\0') &&
         (*FirstString == *SecondString) &&
         (Length > 1)) {
    FirstString++;
    SecondString++;
    Length--;
  }

  return *FirstString - *SecondString;
}

STATIC
CHAR16
EFIAPI
InternalCharToUpper (
  IN      CHAR16                    Char
  )
{
  if (Char >= L'a' && Char <= L'z') {
    return (CHAR16) (Char - (L'a' - L'A'));
  }

  return Char;
}

STATIC
BOOLEAN
EFIAPI
InternalIsDecimalDigitCharacter (
  IN      CHAR16                    Char
  )
{
  return (BOOLEAN) (Char >= L'0' && Char <= L'9');
}

STATIC
UINTN
EFIAPI
InternalHexCharToUintn (
  IN      CHAR16                    Char
  )
{
  if (InternalIsDecimalDigitCharacter (Char)) {
    return Char - L'0';
  }

  return (UINTN) (10 + InternalCharToUpper (Char) - L'A');
}

STATIC
BOOLEAN
EFIAPI
InternalIsHexaDecimalDigitCharacter (
  IN      CHAR16                    Char
  )
{

  return (BOOLEAN) (InternalIsDecimalDigitCharacter (Char) ||
    (Char >= L'A' && Char <= L'F') ||
    (Char >= L'a' && Char <= L'f'));
}

UINT64
EFIAPI
HiiStrHexToUint64 (
  IN CONST CHAR16                   *String
  )
{
  UINT64    Result;

  //
  // ASSERT String is less long than PcdMaximumUnicodeStringLength.
  // Length tests are performed inside StrLen().
  //
  //ASSERT (StrSize (String) != 0);

  //
  // Ignore the pad spaces (space or tab)
  //
  while ((*String == L' ') || (*String == L'\t')) {
    String++;
  }

  //
  // Ignore leading Zeros after the spaces
  //
  while (*String == L'0') {
    String++;
  }

  if (InternalCharToUpper (*String) == L'X') {
    ASSERT (*(String - 1) == L'0');
    if (*(String - 1) != L'0') {
      return 0;
    }
    //
    // Skip the 'X'
    //
    String++;
  }

  Result = 0;

  while (InternalIsHexaDecimalDigitCharacter (*String)) {
    //
    // If the Hex Number represented by String overflows according
    // to the range defined by UINTN, then ASSERT().
    //
    ASSERT (Result <= RShiftU64 (((UINT64) ~0) - InternalHexCharToUintn (*String) , 4));

    Result = LShiftU64 (Result, 4);
    Result = Result + InternalHexCharToUintn (*String);
    String++;
  }

  return Result;
}

/**
 Calculate the number of Unicode characters of the incoming Configuration string,
 not including NULL terminator.

 This is a internal function.

 @param[in] String              String in <MultiConfigRequest> or
                                <MultiConfigResp> format.

 @return The number of Unicode characters.
**/
UINTN
CalculateConfigStringLen (
  IN EFI_STRING                    String
  )
{
  EFI_STRING  TmpPtr;

  //
  // "GUID=" should be the first element of incoming string.
  //
  ASSERT (String != NULL);
  ASSERT (StrnCmp (String, L"GUID=", StrLen (L"GUID=")) == 0);

  //
  // The beginning of next <ConfigRequest>/<ConfigResp> should be "&GUID=".
  // Will meet '\0' if there is only one <ConfigRequest>/<ConfigResp>.
  //
  TmpPtr = StrStr (String, L"&GUID=");
  if (TmpPtr == NULL) {
    return StrLen (String);
  }

  return (TmpPtr - String);
}

/**
 Convert the hex UNICODE %02x encoding of a UEFI device path to binary
 from <PathHdr> of <ConfigHdr>.

 This is a internal function.

 @param[in]  String             UEFI configuration string
 @param[out] DevicePathData     Binary of a UEFI device path.

 @retval EFI_NOT_FOUND          The device path is not invalid.
 @retval EFI_INVALID_PARAMETER  Any incoming parameter is invalid.
 @retval EFI_OUT_OF_RESOURCES   Lake of resources to store neccesary structures.
 @retval EFI_SUCCESS            The device path is retrieved and translated to
                                binary format.
**/
EFI_STATUS
GetDevicePath (
  IN  EFI_STRING                   String,
  OUT UINT8                        **DevicePathData
  )
{
  UINTN                    Length;
  EFI_STRING               PathHdr;
  UINT8                    *DevicePathBuffer;
  CHAR16                   TemStr[2];
  UINTN                    Index;
  UINT8                    DigitUint8;
  EFI_DEVICE_PATH_PROTOCOL *DevicePath;


  if (String == NULL || DevicePathData == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Find the 'PATH=' of <PathHdr> and skip it.
  //
  for (; (*String != 0 && StrnCmp (String, L"PATH=", StrLen (L"PATH=")) != 0); String++);
  if (*String == 0) {
    return EFI_INVALID_PARAMETER;
  }
  //
  // Check whether path data does exist.
  //
  String += StrLen (L"PATH=");
  if (*String == 0) {
    return EFI_INVALID_PARAMETER;
  }
  PathHdr = String;

  //
  // The content between 'PATH=' of <ConfigHdr> and '&' of next element
  // or '\0' (end of configuration string) is the UNICODE %02x bytes encoding
  // of UEFI device path.
  //
  for (Length = 0; *String != 0 && *String != L'&'; String++, Length++);
  //
  // Check DevicePath Length
  //
  if (((Length + 1) / 2) < sizeof (EFI_DEVICE_PATH_PROTOCOL)) {
    return EFI_NOT_FOUND;
  }

  //
  // The data in <PathHdr> is encoded as hex UNICODE %02x bytes in the same order
  // as the device path resides in RAM memory.
  // Translate the data into binary.
  //
  DevicePathBuffer = (UINT8 *) AllocateZeroPool ((Length + 1) / 2);
  if (DevicePathBuffer == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // Convert DevicePath
  //
  ZeroMem (TemStr, sizeof (TemStr));
  for (Index = 0; Index < Length; Index ++) {
    TemStr[0] = PathHdr[Index];
    DigitUint8 = (UINT8) StrHexToUint64 (TemStr);
    if ((Index & 1) == 0) {
      DevicePathBuffer [Index/2] = DigitUint8;
    } else {
      DevicePathBuffer [Index/2] = (UINT8) ((DevicePathBuffer [Index/2] << 4) + DigitUint8);
    }
  }

  //
  // Validate DevicePath
  //
  DevicePath  = (EFI_DEVICE_PATH_PROTOCOL *) DevicePathBuffer;
  while (!IsDevicePathEnd (DevicePath)) {
    if ((DevicePath->Type == 0) || (DevicePath->SubType == 0) || (DevicePathNodeLength (DevicePath) < sizeof (EFI_DEVICE_PATH_PROTOCOL))) {
      //
      // Invalid device path
      //
      FreePool (DevicePathBuffer);
      return EFI_NOT_FOUND;
    }
    DevicePath = NextDevicePathNode (DevicePath);
  }

  //
  // return the device path
  //
  *DevicePathData = DevicePathBuffer;
  return EFI_SUCCESS;
}

/**
 Converts the unicode character of the string from uppercase to lowercase.
 This is a internal function.

 @param[in] ConfigString  String to be converted
**/
VOID
EFIAPI
HiiToLower (
  IN EFI_STRING  ConfigString
  )
{
  EFI_STRING  String;
  BOOLEAN     Lower;

  ASSERT (ConfigString != NULL);

  //
  // Convert all hex digits in range [A-F] in the configuration header to [a-f]
  //
  for (String = ConfigString, Lower = FALSE; *String != L'\0'; String++) {
    if (*String == L'=') {
      Lower = TRUE;
    } else if (*String == L'&') {
      Lower = FALSE;
    } else if (Lower && *String >= L'A' && *String <= L'F') {
      *String = (CHAR16) (*String - L'A' + L'a');
    }
  }

  return;
}

/**
 Generate a sub string then output it.

 This is a internal function.

 @param[in]  String              A constant string which is the prefix of the to be
                                 generated string, e.g. GUID=
 @param[in]  BufferLen           The length of the Buffer in bytes.
 @param[in]  Buffer              Points to a buffer which will be converted to be the
                                 content of the generated string.
 @param[in]  Flag                If 1, the buffer contains data for the value of GUID or PATH stored in
                                 UINT8 *; if 2, the buffer contains unicode string for the value of NAME;
                                 if 3, the buffer contains other data.
 @param[out] SubStr              Points to the output string. It's caller's
                                 responsibility to free this buffer.
**/
VOID
GenerateSubStr (
  IN CONST EFI_STRING              String,
  IN  UINTN                        BufferLen,
  IN  VOID                         *Buffer,
  IN  UINT8                        Flag,
  OUT EFI_STRING                   *SubStr
  )
{
  UINTN       Length;
  EFI_STRING  Str;
  EFI_STRING  StringHeader;
  CHAR16      *TemString;
  CHAR16      *TemName;
  UINT8       *TemBuffer;
  UINTN       Index;

  ASSERT (String != NULL && SubStr != NULL);

  if (Buffer == NULL) {
    *SubStr = AllocateCopyPool (StrSize (String), String);
    ASSERT (*SubStr != NULL);
    return ;
  }

  //
  // Header + Data + '&' + '\0'
  //
  Length = StrLen (String) + BufferLen * 2 + 1 + 1;
  Str    = AllocateZeroPool (Length * sizeof (CHAR16));
  ASSERT (Str != NULL);
  if (Str == NULL) {
    return ;
  }

  StrCpyS (Str, Length, String);

  StringHeader = Str + StrLen (String);
  TemString    = (CHAR16 *) StringHeader;

  switch (Flag) {
  case 1:
    //
    // Convert Buffer to Hex String in reverse order
    //
    TemBuffer = ((UINT8 *) Buffer);
    for (Index = 0; Index < BufferLen; Index ++, TemBuffer ++) {
      UnicodeValueToStringS (
        TemString,
        sizeof (CHAR16) * (Length - StrnLenS (Str, Length)),
        PREFIX_ZERO | RADIX_HEX,
        *TemBuffer,
        2
        );
      TemString += StrnLenS (TemString, Length - StrnLenS (Str, Length));
    }
    break;
  case 2:
    //
    // Check buffer is enough
    //
    TemName = (CHAR16 *) Buffer;
    ASSERT ((BufferLen * 2 + 1) >= (StrLen (TemName) * 4 + 1));
    //
    // Convert Unicode String to Config String, e.g. "ABCD" => "0041004200430044"
    //
    for (; *TemName != L'\0'; TemName++) {
      UnicodeValueToStringS (
        TemString,
        sizeof (CHAR16) * (Length - StrnLenS (Str, Length)),
        PREFIX_ZERO | RADIX_HEX,
        *TemName,
        4
        );
      TemString += StrnLenS (TemString, Length - StrnLenS (Str, Length));
    }
    break;
  case 3:
    //
    // Convert Buffer to Hex String
    //
    TemBuffer = ((UINT8 *) Buffer) + BufferLen - 1;
    for (Index = 0; Index < BufferLen; Index ++, TemBuffer --) {
      UnicodeValueToStringS (
        TemString,
        sizeof (CHAR16) * (Length - StrnLenS (Str, Length)),
        PREFIX_ZERO | RADIX_HEX,
        *TemBuffer,
        2
        );
      TemString += StrnLenS (TemString, Length - StrnLenS (Str, Length));
    }
    break;
  default:
    break;
  }

  //
  // Convert the uppercase to lowercase since <HexAf> is defined in lowercase format.
  //
  StrCatS (Str, Length, L"&");
  HiiToLower (Str);

  *SubStr = Str;
}

/**
 Retrieve the <ConfigBody> from String then output it.

 This is a internal function.

 @param[in]  String             A sub string of a configuration string in
                                <MultiConfigAltResp> format.
 @param[out] ConfigBody         Points to the output string. It's caller's
                                responsibility to free this buffer.

 @retval EFI_INVALID_PARAMETER  There is no form package in current hii database.
 @retval EFI_OUT_OF_RESOURCES   Not enough memory to finish this operation.
 @retval EFI_SUCCESS            All existing storage is exported.
**/
EFI_STATUS
OutputConfigBody (
  IN  EFI_STRING                   String,
  OUT EFI_STRING                   *ConfigBody
  )
{
  EFI_STRING  TmpPtr;
  EFI_STRING  Result;
  UINTN       Length;

  if (String == NULL || ConfigBody == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // The setting information should start OFFSET, not ALTCFG.
  //
  if (StrnCmp (String, L"&ALTCFG=", StrLen (L"&ALTCFG=")) == 0) {
    return EFI_INVALID_PARAMETER;
  }

  TmpPtr = StrStr (String, L"GUID=");
  if (TmpPtr == NULL) {
    //
    // It is the last <ConfigResp> of the incoming configuration string.
    //
    Result = AllocateCopyPool (StrSize (String), String);
    if (Result == NULL) {
      return EFI_OUT_OF_RESOURCES;
    } else {
      *ConfigBody = Result;
      return EFI_SUCCESS;
    }
  }

  Length = TmpPtr - String;
  Result = AllocateCopyPool (Length * sizeof (CHAR16), String);
  if (Result == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  *(Result + Length - 1) = 0;
  *ConfigBody = Result;
  return EFI_SUCCESS;
}

/**
 Append a string to a multi-string format.

 This is a internal function.

 @param[in]      MultiString    String in <MultiConfigRequest>, <MultiConfigAltResp>, or <MultiConfigResp>.
 @param[in]      AppendString   NULL-terminated Unicode string.
 @param[in, out] BufferSize     in:  Total buffer size by bytes of input MultiString.
                                out: Total buffer size by bytes of appended configuration string.
 @param[in, out] UsedSize       in:  Used buffer size by bytes of input MultiString.
                                out: Used buffer size by bytes of appended configuration string.

 @return         Pointer to appended configuration string. NULL indicates append string failed.
**/
EFI_STRING
AppendToMultiString (
  IN     EFI_STRING       MultiString,
  IN     EFI_STRING       AppendString,
  IN OUT UINTN            *BufferSize,
  IN OUT UINTN            *UsedSize
  )
{
  UINTN AppendStringSize;
  UINTN NewStringSize;
  UINTN StrEndIndex;

  if (MultiString == NULL || AppendString == NULL || BufferSize == NULL || UsedSize == NULL) {
    return NULL;
  }


  AppendStringSize = StrSize (AppendString);
  NewStringSize    = AppendStringSize;
  if (*UsedSize != 0) {
    NewStringSize += (*UsedSize - sizeof (CHAR16));
  }

  if (NewStringSize  > *BufferSize) {
    MultiString = (EFI_STRING) ReallocatePool (
                                  *UsedSize,
                                  NewStringSize << 1,
                                  (VOID *) MultiString
                                  );
    ASSERT (MultiString != NULL);
    if (MultiString == NULL) {
      *BufferSize = 0;
      *UsedSize   = 0;
      return MultiString;
    }
    *BufferSize = NewStringSize << 1;
  }
  //
  // Using CopyMem () to append string to prevent from StrCat () spend too much
  // time if EFI_DEBUG enabled.
  //
  StrEndIndex = *UsedSize == 0 ? 0 : *UsedSize / sizeof (CHAR16) - 1;
  CopyMem (&MultiString[StrEndIndex], AppendString, AppendStringSize);
  *UsedSize = NewStringSize;

  return MultiString;
}

/**
 Get the value of <Number> in <BlockConfig> format, i.e. the value of OFFSET
 or WIDTH or VALUE.
 <BlockConfig> ::= 'OFFSET='<Number>&'WIDTH='<Number>&'VALUE'=<Number>

 This is a internal function.

 @param[in]  StringPtr          String in <BlockConfig> format and points to the
                                first character of <Number>.
 @param[out] Number             The output value. Caller takes the responsibility
                                to free memory.
 @param[out] Len                Length of the <Number>, in characters.

 @retval EFI_OUT_OF_RESOURCES   Insufficient resources to store neccessary
                                structures.
 @retval EFI_SUCCESS            Value of <Number> is outputted in Number
                                successfully.
**/
EFI_STATUS
GetValueOfNumber (
  IN EFI_STRING                    StringPtr,
  IN OUT UINT8                     **Number,
  IN OUT UINTN                     *NumberSize,
  OUT UINTN                        *Len
  )
{
  EFI_STRING               TmpPtr;
  UINTN                    Length;
  EFI_STRING               Str;
  UINT8                    *Buf;
  EFI_STATUS               Status;
  UINT8                    DigitUint8;
  UINTN                    Index;
  CHAR16                   TemStr[2];
  CHAR16                   StringBuffer[20];

  if (StringPtr == NULL || *StringPtr == L'\0' || Number == NULL || Len == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Buf = NULL;

  TmpPtr = StringPtr;
  while (*StringPtr != L'\0' && *StringPtr != L'&') {
    StringPtr++;
  }
  *Len   = StringPtr - TmpPtr;
  Length = *Len + 1;

  if (Length < sizeof (StringBuffer) / sizeof (CHAR16)) {
    Str = StringBuffer;
  } else {
    Str = (EFI_STRING) AllocateZeroPool (Length * sizeof (CHAR16));
    if (Str == NULL) {
      Status = EFI_OUT_OF_RESOURCES;
      goto Exit;
    }
  }
  CopyMem (Str, TmpPtr, *Len * sizeof (CHAR16));
  *(Str + *Len) = L'\0';

  Length = (Length + 1) / 2;
  if (Length <= *NumberSize) {
    Buf = *Number;
    ZeroMem (Buf, *NumberSize);
  } else {
    Buf = (UINT8 *) AllocateZeroPool (Length);
    if (Buf == NULL) {
      Status = EFI_OUT_OF_RESOURCES;
      goto Exit;
    }
    FreePool (*Number);
    *NumberSize = Length;
  }

  Length = *Len;
  ZeroMem (TemStr, sizeof (TemStr));
  for (Index = 0; Index < Length; Index ++) {
    TemStr[0] = Str[Length - Index - 1];
    DigitUint8 = (UINT8) StrHexToUint64 (TemStr);
    if ((Index & 1) == 0) {
      Buf [Index/2] = DigitUint8;
    } else {
      Buf [Index/2] = (UINT8) ((DigitUint8 << 4) + Buf [Index/2]);
    }
  }

  *Number = Buf;
  Status  = EFI_SUCCESS;

Exit:
  if (Str != StringBuffer && Str != NULL) {
    FreePool (Str);
  }

  return Status;
}

/**
 To find the BlockName in the string with same value.

  @param  String                 Pointer to a Null-terminated Unicode string.
  @param  BlockName              Pointer to a Null-terminated Unicode string to search for.
  @param  Buffer                 Pointer to the value correspond to the BlockName.
  @param  Found                  The Block whether has been found.
  @param  BufferLen              The length of the buffer.

  @retval EFI_OUT_OF_RESOURCES   Insufficient resources to store necessary structures.
  @retval EFI_SUCCESS            The function finishes successfully.

**/
EFI_STATUS
FindSameBlockElement(
  IN  EFI_STRING   String,
  IN  EFI_STRING   BlockName,
  IN  UINT8        *Buffer,
  OUT BOOLEAN      *Found,
  IN  UINTN        BufferLen
  )
{
  EFI_STRING   BlockPtr;
  UINTN        Length;
  UINT8        *TempBuffer;
  UINTN        TempBufferSize;
  EFI_STATUS   Status;

  TempBufferSize = 10;
  TempBuffer     = AllocateZeroPool (TempBufferSize);
  if (TempBuffer == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  *Found = FALSE;
  BlockPtr = StrStr (String, BlockName);

  while (BlockPtr != NULL) {
    BlockPtr += StrLen (BlockName);
    Status = GetValueOfNumber (BlockPtr, &TempBuffer, &TempBufferSize, &Length);
    if (EFI_ERROR (Status)) {
      return Status;
    }
    ASSERT (TempBuffer != NULL);
    if ((BufferLen == Length) && (0 == CompareMem (Buffer, TempBuffer, Length))) {
      *Found = TRUE;
      FreePool (TempBuffer);
      TempBuffer = NULL;
      return EFI_SUCCESS;
    } else {
      BlockPtr = StrStr (BlockPtr + 1, BlockName);
    }
  }

  FreePool (TempBuffer);
  return EFI_SUCCESS;
}

/**
  Compare the <AltResp> in ConfigAltResp and DefaultAltCfgResp, if the <AltResp>
  in DefaultAltCfgResp but not in ConfigAltResp,add it to the ConfigAltResp.

  @param  DefaultAltCfgResp      Pointer to a null-terminated Unicode string in
                                 <MultiConfigAltResp> format. The default value
                                 string may contain more than one ConfigAltResp
                                 string for the different varstore buffer.
  @param  ConfigAltResp          Pointer to a null-terminated Unicode string in
                                 <ConfigAltResp> format.
  @param  AltConfigHdr           Pointer to a Unicode string in <AltConfigHdr> format.
  @param  ConfigAltRespChanged   Whether the ConfigAltResp has been changed.

  @retval EFI_OUT_OF_RESOURCES   Insufficient resources to store necessary structures.
  @retval EFI_SUCCESS            The function finishes  successfully.

**/
EFI_STATUS
CompareBlockElementDefault (
  IN      EFI_STRING  DefaultAltCfgResp,
  IN OUT  EFI_STRING  *ConfigAltResp,
  IN      EFI_STRING  AltConfigHdr,
  IN OUT  BOOLEAN     *ConfigAltRespChanged
)
{
  EFI_STATUS    Status;
  EFI_STRING    BlockPtr;
  EFI_STRING    BlockPtrStart;
  EFI_STRING    StringPtr;
  EFI_STRING    AppendString;
  EFI_STRING    AltConfigHdrPtr;
  UINT8         *TempBuffer;
  UINTN         TempBufferSize;
  UINTN         OffsetLength;
  UINTN         AppendSize;
  UINTN         TotalSize;
  BOOLEAN       FoundOffset;

  AppendString = NULL;
  TempBufferSize = 10;
  TempBuffer     = AllocateZeroPool (TempBufferSize);
  if (TempBuffer == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto Exit;
  }
  //
  // Make BlockPtr point to the first <BlockConfig> with AltConfigHdr in DefaultAltCfgResp.
  //
  AltConfigHdrPtr = StrStr (DefaultAltCfgResp, AltConfigHdr);
  ASSERT (AltConfigHdrPtr != NULL);
  BlockPtr = StrStr (AltConfigHdrPtr, L"&OFFSET=");
  //
  // Make StringPtr point to the AltConfigHdr in ConfigAltResp.
  //
  StringPtr = StrStr (*ConfigAltResp, AltConfigHdr);
  ASSERT (StringPtr != NULL);

  while (BlockPtr != NULL) {
    //
    // Find the "&OFFSET=<Number>" block and get the value of the Number with AltConfigHdr in DefaultAltCfgResp.
    //
    BlockPtrStart = BlockPtr;
    BlockPtr += StrLen (L"&OFFSET=");
    Status = GetValueOfNumber (BlockPtr, &TempBuffer, &TempBufferSize, &OffsetLength);
    if (EFI_ERROR (Status)) {
      Status = EFI_OUT_OF_RESOURCES;
      goto Exit;
    }
    //
    // To find the same "&OFFSET=<Number>" block in ConfigAltResp.
    //
    Status = FindSameBlockElement (StringPtr, L"&OFFSET=", TempBuffer, &FoundOffset, OffsetLength);
    if (EFI_ERROR (Status)) {
      Status = EFI_OUT_OF_RESOURCES;
      goto Exit;
    }
    if (!FoundOffset) {
      //
      // Don't find the same "&OFFSET=<Number>" block in ConfigAltResp.
      // Calculate the size of <BlockConfig>.
      // <BlockConfig>::='OFFSET='<Number>'&WIDTH='<Number>'&VALUE='<Number>.
      //
      BlockPtr = StrStr (BlockPtr + 1, L"&OFFSET=");
      if (BlockPtr != NULL) {
        AppendSize = (BlockPtr - BlockPtrStart) * sizeof (CHAR16);
      } else {
        AppendSize = StrSize (BlockPtrStart);
      }
      //
      // Copy the <BlockConfig> to AppendString.
      //
      if (AppendString == NULL) {
        AppendString = (EFI_STRING) AllocateZeroPool (AppendSize + sizeof (CHAR16));
        StrnCatS (AppendString, AppendSize / sizeof (CHAR16) + 1, BlockPtrStart, AppendSize / sizeof (CHAR16));
      } else {
        TotalSize = StrSize (AppendString) + AppendSize + sizeof (CHAR16);
        AppendString = (EFI_STRING) ReallocatePool (
                                      StrSize (AppendString),
                                      TotalSize,
                                      AppendString
                                      );
        if (AppendString == NULL) {
          Status = EFI_OUT_OF_RESOURCES;
          goto Exit;
        }
        StrnCatS (AppendString, TotalSize / sizeof (CHAR16), BlockPtrStart, AppendSize / sizeof (CHAR16));
      }
    } else {
      //
      // To find next "&OFFSET=<Number>" block with AltConfigHdr in DefaultAltCfgResp.
      //
      BlockPtr = StrStr (BlockPtr + 1, L"&OFFSET=");
    }
  }

  if (AppendString != NULL) {
    //
    // Reallocate ConfigAltResp to copy the AppendString.
    //
    TotalSize = StrSize (*ConfigAltResp) + StrSize (AppendString) + sizeof (CHAR16);
    *ConfigAltResp = (EFI_STRING) ReallocatePool (
                                    StrSize (*ConfigAltResp),
                                    TotalSize,
                                    *ConfigAltResp
                                    );
    if (*ConfigAltResp == NULL) {
      Status = EFI_OUT_OF_RESOURCES;
      goto Exit;
    }
    StrCatS (*ConfigAltResp, TotalSize / sizeof (CHAR16), AppendString);
    *ConfigAltRespChanged = TRUE;
  }

  Status = EFI_SUCCESS;

Exit:
  if (AppendString != NULL) {
    FreePool (AppendString);
  }
  if (TempBuffer != NULL) {
    FreePool (TempBuffer);
  }

  return Status;
}

/**
  Compare the <AltResp> in ConfigAltResp and DefaultAltCfgResp, if the <AltResp>
  in DefaultAltCfgResp but not in ConfigAltResp,add it to the ConfigAltResp.

  @param  DefaultAltCfgResp      Pointer to a null-terminated Unicode string in
                                 <MultiConfigAltResp> format. The default value
                                 string may contain more than one ConfigAltResp
                                 string for the different varstore buffer.
  @param  ConfigAltResp          Pointer to a null-terminated Unicode string in
                                 <ConfigAltResp> format.
  @param  AltConfigHdr           Pointer to a Unicode string in <AltConfigHdr> format.
  @param  ConfigAltRespChanged   Whether the ConfigAltResp has been changed.

  @retval EFI_OUT_OF_RESOURCES   Insufficient resources to store necessary structures.
  @retval EFI_SUCCESS            The function finishes  successfully.

**/
EFI_STATUS
CompareNameElementDefault (
  IN     EFI_STRING  DefaultAltCfgResp,
  IN OUT EFI_STRING  *ConfigAltResp,
  IN     EFI_STRING  AltConfigHdr,
  IN OUT BOOLEAN     *ConfigAltRespChanged
)
{
  EFI_STATUS    Status;
  EFI_STRING    NvConfigPtr;
  EFI_STRING    NvConfigStart;
  EFI_STRING    NvConfigValuePtr;
  EFI_STRING    StringPtr;
  EFI_STRING    NvConfigExist;
  EFI_STRING    AppendString;
  CHAR16        TempChar;
  UINTN         AppendSize;
  UINTN         TotalSize;

  AppendString = NULL;
  NvConfigExist = NULL;
  //
  // Make NvConfigPtr point to the first <NvConfig> with AltConfigHdr in DefaultAltCfgResp.
  //
  NvConfigPtr = StrStr (DefaultAltCfgResp, AltConfigHdr);
  ASSERT (NvConfigPtr != NULL);
  NvConfigPtr = StrStr (NvConfigPtr + StrLen(AltConfigHdr),L"&");
  //
  // Make StringPtr point to the first <NvConfig> with AltConfigHdr in ConfigAltResp.
  //
  StringPtr = StrStr (*ConfigAltResp, AltConfigHdr);
  ASSERT (StringPtr != NULL);
  StringPtr = StrStr (StringPtr + StrLen (AltConfigHdr), L"&");
  ASSERT (StringPtr != NULL);

  while (NvConfigPtr != NULL) {
    //
    // <NvConfig> ::= <Label>'='<String> | <Label>'='<Number>.
    // Get the <Label> with AltConfigHdr in DefaultAltCfgResp.
    //
    NvConfigStart = NvConfigPtr;
    NvConfigValuePtr = StrStr (NvConfigPtr + 1, L"=");
    ASSERT (NvConfigValuePtr != NULL);
    TempChar = *NvConfigValuePtr;
    *NvConfigValuePtr = L'\0';
    //
    // Get the <Label> with AltConfigHdr in ConfigAltResp.
    //
    NvConfigExist = StrStr (StringPtr, NvConfigPtr);
    if (NvConfigExist == NULL) {
      //
      // Don't find same <Label> in ConfigAltResp.
      // Calculate the size of <NvConfig>.
      //
      *NvConfigValuePtr = TempChar;
      NvConfigPtr = StrStr (NvConfigPtr + 1, L"&");
      if (NvConfigPtr != NULL) {
        AppendSize = (NvConfigPtr - NvConfigStart) * sizeof (CHAR16);
      } else {
        AppendSize = StrSize (NvConfigStart);
      }
      //
      // Copy the <NvConfig> to AppendString.
      //
      if (AppendString == NULL) {
        AppendString = (EFI_STRING) AllocateZeroPool (AppendSize + sizeof (CHAR16));
        StrnCatS (AppendString, AppendSize / sizeof (CHAR16) + 1, NvConfigStart, AppendSize / sizeof (CHAR16));
      } else {
         TotalSize = StrSize (AppendString) + AppendSize + sizeof (CHAR16);
         AppendString = (EFI_STRING) ReallocatePool (
                                       StrSize (AppendString),
                                       TotalSize,
                                       AppendString
                                       );
        if (AppendString == NULL) {
          Status = EFI_OUT_OF_RESOURCES;
          goto Exit;
        }
        StrnCatS (AppendString, TotalSize / sizeof (CHAR16), NvConfigStart, AppendSize / sizeof (CHAR16));
      }
    } else {
      //
      // To find next <Label> in DefaultAltCfgResp.
      //
      *NvConfigValuePtr = TempChar;
      NvConfigPtr = StrStr (NvConfigPtr + 1, L"&");
    }
  }
  if (AppendString != NULL) {
    //
    // Reallocate ConfigAltResp to copy the AppendString.
    //
    TotalSize = StrSize (*ConfigAltResp) + StrSize (AppendString) + sizeof (CHAR16);
    *ConfigAltResp = (EFI_STRING) ReallocatePool (
                                    StrSize (*ConfigAltResp),
                                    StrSize (*ConfigAltResp) + StrSize (AppendString) + sizeof (CHAR16),
                                    *ConfigAltResp
                                    );
    if (*ConfigAltResp == NULL) {
      Status = EFI_OUT_OF_RESOURCES;
      goto Exit;
    }
    StrCatS (*ConfigAltResp, TotalSize / sizeof (CHAR16), AppendString);
    *ConfigAltRespChanged = TRUE;
  }
  Status = EFI_SUCCESS;

Exit:
  if (AppendString != NULL) {
    FreePool (AppendString);
  }
  return Status;
}

/**
  Compare the <AltResp> in AltCfgResp and DefaultAltCfgResp, if the <AltResp>
  in DefaultAltCfgResp but not in AltCfgResp,add it to the AltCfgResp.

  @param  AltCfgResp             Pointer to a null-terminated Unicode string in
                                 <ConfigAltResp> format.
  @param  DefaultAltCfgResp      Pointer to a null-terminated Unicode string in
                                 <MultiConfigAltResp> format. The default value
                                 string may contain more than one ConfigAltResp
                                 string for the different varstore buffer.
  @param  AltConfigHdr           Pointer to a Unicode string in <AltConfigHdr> format.

  @retval EFI_OUT_OF_RESOURCES   Insufficient resources to store necessary
                                 structures.
  @retval EFI_SUCCESS            The function finishes  successfully.

**/
EFI_STATUS
CompareAndMergeDefaultString (
  IN OUT EFI_STRING  *AltCfgResp,
  IN     EFI_STRING  DefaultAltCfgResp,
  IN     EFI_STRING  AltConfigHdr
  )
{
  EFI_STATUS     Status;
  EFI_STRING     AltCfgRespBackup;
  EFI_STRING     AltConfigHdrPtr;
  EFI_STRING     AltConfigHdrPtrNext;
  EFI_STRING     ConfigAltResp;
  EFI_STRING     StringPtr;
  EFI_STRING     StringPtrNext;
  EFI_STRING     BlockPtr;
  UINTN          ReallocateSize;
  CHAR16         TempChar;
  CHAR16         TempCharA;
  BOOLEAN        ConfigAltRespChanged;

  Status = EFI_OUT_OF_RESOURCES;
  BlockPtr             = NULL;
  AltConfigHdrPtrNext  = NULL;
  StringPtrNext        = NULL;
  ConfigAltResp        = NULL;
  AltCfgRespBackup     = NULL;
  TempChar             = L'\0';
  TempCharA            = L'\0';
  ConfigAltRespChanged = FALSE;

  //
  //To find the <AltResp> with AltConfigHdr in DefaultAltCfgResp, ignore other <AltResp> which follow it.
  //
  AltConfigHdrPtr = StrStr (DefaultAltCfgResp, AltConfigHdr);
  ASSERT (AltConfigHdrPtr != NULL);
  AltConfigHdrPtrNext = StrStr (AltConfigHdrPtr + 1, L"&GUID");
  if (AltConfigHdrPtrNext != NULL) {
    TempChar = *AltConfigHdrPtrNext;
    *AltConfigHdrPtrNext = L'\0';
  }
  //
  // To find the <AltResp> with AltConfigHdr in AltCfgResp, ignore other <AltResp> which follow it.
  //
  StringPtr = StrStr (*AltCfgResp, AltConfigHdr);
  StringPtrNext = StrStr (StringPtr + 1, L"&GUID");
  if (StringPtrNext != NULL) {
    TempCharA = *StringPtrNext;
    *StringPtrNext = L'\0';
  }
  //
  // Copy the content of <ConfigAltResp> which contain current AltConfigHdr in AltCfgResp.
  //
  ConfigAltResp = AllocateCopyPool (StrSize (*AltCfgResp), *AltCfgResp);
  if (ConfigAltResp == NULL) {
    goto Exit;
  }
  //
  // To find the <ConfigBody> with AltConfigHdr in DefaultAltCfgResp.
  //
  BlockPtr = StrStr (AltConfigHdrPtr, L"&OFFSET=");
  if (BlockPtr != NULL) {
    //
    // <BlockConfig>::='OFFSET='<Number>'&WIDTH='<Number>'&VALUE='<Number> style.
    // Call function CompareBlockElementDefault to compare the <BlockConfig> in DefaultAltCfgResp and ConfigAltResp.
    // The ConfigAltResp which may contain the new <BlockConfig> get from DefaultAltCfgResp.
    //
    Status = CompareBlockElementDefault (DefaultAltCfgResp, &ConfigAltResp, AltConfigHdr, &ConfigAltRespChanged);
    if (EFI_ERROR(Status)) {
      goto Exit;
    }
  } else {
    //
    // <NvConfig> ::= <Label>'='<String> | <Label>'='<Number> style.
    // Call function CompareNameElementDefault to compare the <NvConfig> in DefaultAltCfgResp and ConfigAltResp.
    // The ConfigAltResp which may contain the new <NvConfig> get from DefaultAltCfgResp.
    //
    Status = CompareNameElementDefault (DefaultAltCfgResp, &ConfigAltResp, AltConfigHdr, &ConfigAltRespChanged);
    if (EFI_ERROR(Status)) {
      goto Exit;
    }
  }
  //
  // Restore the AltCfgResp.
  //
  if (StringPtrNext != NULL) {
    *StringPtrNext = TempCharA;
  }

  //
  // If the ConfigAltResp has no change,no need to update the content in AltCfgResp.
  //
  if (!ConfigAltRespChanged) {
    Status = EFI_SUCCESS;
    goto Exit;
  }
  //
  // ConfigAltResp has been changed, need to update the content in AltCfgResp.
  //
  if (StringPtrNext != NULL) {
    ReallocateSize = StrSize (ConfigAltResp) + StrSize (StringPtrNext) + sizeof (CHAR16);
  } else {
    ReallocateSize = StrSize (ConfigAltResp) + sizeof (CHAR16);
  }

  AltCfgRespBackup = (EFI_STRING) AllocateZeroPool (ReallocateSize);
  if (AltCfgRespBackup == NULL) {
    goto Exit;
  }

  StrCatS (AltCfgRespBackup, ReallocateSize / sizeof (CHAR16), ConfigAltResp);
  if (StringPtrNext != NULL) {
    StrCatS (AltCfgRespBackup, ReallocateSize / sizeof (CHAR16), StringPtrNext);
  }

  FreePool (*AltCfgResp);
  *AltCfgResp = AltCfgRespBackup;

  Status = EFI_SUCCESS;

Exit:
  if (ConfigAltResp != NULL) {
    FreePool(ConfigAltResp);
  }
  //
  // Restore the DefaultAltCfgResp.
  //
  if ( AltConfigHdrPtrNext != NULL) {
    *AltConfigHdrPtrNext = TempChar;
    AltConfigHdrPtrNext = NULL;
  }

  return Status;
}

/**
 This function merges DefaultAltCfgResp string into AltCfgResp string for
 the missing AltCfgId in AltCfgResq.

 @param[in, out] AltCfgResp           Pointer to a null-terminated Unicode string in
                                      <ConfigAltResp> format. The default value string
                                      will be merged into it.
 @param[in]      DefaultAltCfgResp    Pointer to a null-terminated Unicode string in
                                      <MultiConfigAltResp> format. The default value
                                      string may contain more than one ConfigAltResp
                                      string for the different varstore buffer.

 @retval EFI_SUCCESS            The merged string returns.
 @retval EFI_INVALID_PARAMETER  *AltCfgResp is to NULL.
**/
EFI_STATUS
EFIAPI
MergeDefaultString (
  IN OUT EFI_STRING  *AltCfgResp,
  IN     EFI_STRING  DefaultAltCfgResp
  )
{
  EFI_STRING   StringPtrDefault;
  EFI_STRING   StringPtrEnd;
  CHAR16       TempChar;
  EFI_STRING   StringPtr;
  EFI_STRING   AltConfigHdr;
  UINTN        HeaderLength;
  UINTN        SizeAltCfgResp;
  UINTN        MaxLen;
  UINTN        TotalSize;

  if (*AltCfgResp == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Get the requestr ConfigHdr
  //
  SizeAltCfgResp  = 0;
  StringPtr       = *AltCfgResp;

  //
  // Find <ConfigHdr> GUID=...&NAME=...&PATH=...
  //
  if (StrnCmp (StringPtr, L"GUID=", StrLen (L"GUID=")) != 0) {
    return EFI_INVALID_PARAMETER;
  }
  while (*StringPtr != L'\0' && StrnCmp (StringPtr, L"&NAME=", StrLen (L"&NAME=")) != 0) {
    StringPtr++;
  }
  while (*StringPtr != L'\0' && StrnCmp (StringPtr, L"&PATH=", StrLen (L"&PATH=")) != 0) {
    StringPtr++;
  }
  if (*StringPtr == L'\0') {
    return EFI_INVALID_PARAMETER;
  }
  StringPtr += StrLen (L"&PATH=");
  while (*StringPtr != L'\0' && *StringPtr != L'&') {
    StringPtr ++;
  }
  HeaderLength = StringPtr - *AltCfgResp;

  //
  // Construct AltConfigHdr string  "&<ConfigHdr>&ALTCFG=XXXX\0"
  //                                  |1| StrLen (ConfigHdr) | 8 | 4 | 1 |
  //
  MaxLen = 1 + HeaderLength + 8 + 4 + 1;
  AltConfigHdr = AllocateZeroPool (MaxLen * sizeof (CHAR16));
  if (AltConfigHdr == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  StrCpyS (AltConfigHdr, MaxLen, L"&");
  StrnCatS (AltConfigHdr, MaxLen, *AltCfgResp, HeaderLength);
  StrCatS (AltConfigHdr, MaxLen, L"&ALTCFG=");
  HeaderLength = StrLen (AltConfigHdr);

  StringPtrDefault = StrStr (DefaultAltCfgResp, AltConfigHdr);
  while (StringPtrDefault != NULL) {
    //
    // Get AltCfg Name
    //
    StrnCatS (AltConfigHdr, MaxLen, StringPtrDefault + HeaderLength, 4);
    StringPtr = StrStr (*AltCfgResp, AltConfigHdr);

    //
    // Append the found default value string to the input AltCfgResp
    //
    if (StringPtr == NULL) {
      StringPtrEnd   = StrStr (StringPtrDefault + 1, L"&GUID");
      SizeAltCfgResp = StrSize (*AltCfgResp);
      if (StringPtrEnd == NULL) {
        //
        // No more default string is found.
        //
        TotalSize = SizeAltCfgResp + StrSize (StringPtrDefault);
        *AltCfgResp    = (EFI_STRING) ReallocatePool (
                                     SizeAltCfgResp,
                                     TotalSize,
                                     (VOID *) (*AltCfgResp)
                                     );
        if (*AltCfgResp == NULL) {
          FreePool (AltConfigHdr);
          return EFI_OUT_OF_RESOURCES;
        }
        StrCatS (*AltCfgResp, TotalSize / sizeof (CHAR16), StringPtrDefault);
        break;
      } else {
        TempChar = *StringPtrEnd;
        *StringPtrEnd = L'\0';
        TotalSize = SizeAltCfgResp + StrSize (StringPtrDefault);
        *AltCfgResp = (EFI_STRING) ReallocatePool (
                                     SizeAltCfgResp,
                                     TotalSize,
                                     (VOID *) (*AltCfgResp)
                                     );
        if (*AltCfgResp == NULL) {
          FreePool (AltConfigHdr);
          return EFI_OUT_OF_RESOURCES;
        }
        StrCatS (*AltCfgResp, TotalSize / sizeof (CHAR16), StringPtrDefault);
        *StringPtrEnd = TempChar;
      }
    } else {
      //
      // The AltCfgResp contains <AltCfgResp>.
      // If the <ConfigElement> in <AltCfgResp> in the DefaultAltCfgResp but not in the
      // related <AltCfgResp> in AltCfgResp, merge it to AltCfgResp. else no need to merge.
      //
      CompareAndMergeDefaultString (AltCfgResp, DefaultAltCfgResp, AltConfigHdr);
    }

    //
    // Find next AltCfg String
    //
    *(AltConfigHdr + HeaderLength) = L'\0';
    StringPtrDefault = StrStr (StringPtrDefault + 1, AltConfigHdr);
  }

  FreePool (AltConfigHdr);
  return EFI_SUCCESS;
}

/**
 This function inserts new DefaultValueData into the BlockData DefaultValue array.

 @param[in] BlockData         The BlockData is updated to add new default value.
 @param[in] DefaultValueData  The DefaultValue is added.
**/
VOID
InsertDefaultValue (
  IN IFR_BLOCK_DATA         *BlockData,
  IN IFR_DEFAULT_DATA       *DefaultValueData
  )
{
  LIST_ENTRY             *Link;
  IFR_DEFAULT_DATA       *DefaultValueArray;

  for (Link = BlockData->DefaultValueEntry.ForwardLink; Link != &BlockData->DefaultValueEntry; Link = Link->ForwardLink) {
    DefaultValueArray = BASE_CR (Link, IFR_DEFAULT_DATA, Entry);
    if (DefaultValueArray->DefaultId == DefaultValueData->DefaultId) {
      //
      // DEFAULT_VALUE_FROM_OPCODE has high priority, DEFAULT_VALUE_FROM_DEFAULT has low priority.
      //
      if (DefaultValueData->Type > DefaultValueArray->Type) {
        //
        // Update the default value array in BlockData.
        //
        CopyMem (&DefaultValueArray->Value, &DefaultValueData->Value, sizeof (EFI_IFR_TYPE_VALUE));
        DefaultValueArray->Type  = DefaultValueData->Type;
        DefaultValueArray->Cleaned = DefaultValueData->Cleaned;
      }
      return;
    }
  }

  //
  // Insert new default value data in tail.
  //
  DefaultValueArray = AllocateZeroPool (sizeof (IFR_DEFAULT_DATA));
  ASSERT (DefaultValueArray != NULL);
  if (DefaultValueArray == NULL) {
    return;
  }
  CopyMem (DefaultValueArray, DefaultValueData, sizeof (IFR_DEFAULT_DATA));
  InsertTailList (Link, &DefaultValueArray->Entry);
}

/**
 This function inserts new BlockData into the block link

 @param[in] BlockLink   The list entry points to block array.
 @param[in] BlockData   The point to BlockData is added.
**/
VOID
InsertBlockData (
  IN LIST_ENTRY        *BlockLink,
  IN IFR_BLOCK_DATA    **BlockData
  )
{
  LIST_ENTRY      *Link;
  IFR_BLOCK_DATA  *BlockArray;
  IFR_BLOCK_DATA  *BlockSingleData;

  BlockSingleData = *BlockData;

  //
  // Insert block data in its Offset and Width order.
  //
  for (Link = BlockLink->ForwardLink; Link != BlockLink; Link = Link->ForwardLink) {
    BlockArray = BASE_CR (Link, IFR_BLOCK_DATA, Entry);
    if (BlockArray->Offset == BlockSingleData->Offset) {
      if (BlockArray->Width > BlockSingleData->Width) {
        //
        // Insert this block data in the front of block array
        //
        InsertTailList (Link, &BlockSingleData->Entry);
        return;
      }

      if (BlockArray->Width == BlockSingleData->Width) {
        //
        // The same block array has been added.
        //
        FreePool (BlockSingleData);
        *BlockData = BlockArray;
        return;
      }
    } else if (BlockArray->Offset > BlockSingleData->Offset) {
      //
      // Insert new block data in the front of block array
      //
      InsertTailList (Link, &BlockSingleData->Entry);
      return;
    }
  }

  //
  // Add new block data into the tail.
  //
  InsertTailList (Link, &BlockSingleData->Entry);
  return;
}

/**
  Retrieves a pointer to the a Null-terminated ASCII string containing the list
  of languages that an HII handle in the HII Database supports.  The returned
  string is allocated using AllocatePool().  The caller is responsible for freeing
  the returned string using FreePool().  The format of the returned string follows
  the language format assumed the HII Database.

  If HiiHandle is NULL, then ASSERT().

  @param[in]  HiiHandle  A handle that was previously registered in the HII Database.

  @retval NULL   HiiHandle is not registered in the HII database
  @retval NULL   There are not enough resources available to retrieve the suported
                 languages.
  @retval NULL   The list of suported languages could not be retrieved.
  @retval Other  A pointer to the Null-terminated ASCII string of supported languages.

**/
CHAR8 *
GetSupportedLanguages (
  IN EFI_HII_HANDLE           HiiHandle
  )
{
  EFI_STATUS  Status;
  UINTN       LanguageSize;
  CHAR8       TempSupportedLanguages;
  CHAR8       *SupportedLanguages;

  ASSERT (HiiHandle != NULL);

  //
  // Retrieve the size required for the supported languages buffer.
  //
  LanguageSize = 0;
  Status = mPrivate.HiiString.GetLanguages (&mPrivate.HiiString, HiiHandle, &TempSupportedLanguages, &LanguageSize);

  //
  // If GetLanguages() returns EFI_SUCCESS for a zero size,
  // then there are no supported languages registered for HiiHandle.  If GetLanguages()
  // returns an error other than EFI_BUFFER_TOO_SMALL, then HiiHandle is not present
  // in the HII Database
  //
  if (Status != EFI_BUFFER_TOO_SMALL) {
    //
    // Return NULL if the size can not be retrieved, or if HiiHandle is not in the HII Database
    //
    return NULL;
  }

  //
  // Allocate the supported languages buffer.
  //
  SupportedLanguages = AllocateZeroPool (LanguageSize);
  if (SupportedLanguages == NULL) {
    //
    // Return NULL if allocation fails.
    //
    return NULL;
  }

  //
  // Retrieve the supported languages string
  //
  Status = mPrivate.HiiString.GetLanguages (&mPrivate.HiiString, HiiHandle, SupportedLanguages, &LanguageSize);
  if (EFI_ERROR (Status)) {
    //
    // Free the buffer and return NULL if the supported languages can not be retrieved.
    //
    FreePool (SupportedLanguages);
    return NULL;
  }

  //
  // Return the Null-terminated ASCII string of supported languages
  //
  return SupportedLanguages;
}

/**
 This function checks VarOffset and VarWidth is in the block range.

 @param[in] RequestBlockArray  The block array is to be checked.
 @param[in] VarOffset          Offset of var to the structure
 @param[in] VarWidth           Width of var.

 @retval TRUE   This Var is in the block range.
 @retval FALSE  This Var is not in the block range.
**/
BOOLEAN
BlockArrayCheck (
  IN IFR_BLOCK_DATA  *RequestBlockArray,
  IN UINT16          VarOffset,
  IN UINT16          VarWidth
  )
{
  LIST_ENTRY          *Link;
  IFR_BLOCK_DATA      *BlockData;

  //
  // No Request Block array, all vars are got.
  //
  if (RequestBlockArray == NULL) {
    return TRUE;
  }

  //
  // Check the input var is in the request block range.
  //
  for (Link = RequestBlockArray->Entry.ForwardLink; Link != &RequestBlockArray->Entry; Link = Link->ForwardLink) {
    BlockData = BASE_CR (Link, IFR_BLOCK_DATA, Entry);
    if ((VarOffset >= BlockData->Offset) && ((VarOffset + VarWidth) <= (BlockData->Offset + BlockData->Width))) {
      return TRUE;
    }
  }

  return FALSE;
}

/**
 Get form package data from data base.

 @param[in]      DataBaseRecord    The DataBaseRecord instance contains the found Hii handle and package.
 @param[in, out] HiiFormPackage    The buffer saves the package data.
 @param[out]     PackageSize       The buffer size of the package data.
**/
EFI_STATUS
GetFormPackageData (
  IN     HII_DATABASE_RECORD        *DataBaseRecord,
  IN OUT UINT8                      **HiiFormPackage,
  OUT    UINTN                      *PackageSize
  )
{
  EFI_STATUS                   Status;
  UINTN                        Size;
  UINTN                        ResultSize;

  if (DataBaseRecord == NULL || HiiFormPackage == NULL || PackageSize == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Size       = 0;
  ResultSize = 0;
  //
  // 0. Get Hii Form Package by HiiHandle
  //
  Status = ExportFormPackages (
             &mPrivate,
             DataBaseRecord->Handle,
             DataBaseRecord->PackageList,
             0,
             Size,
             HiiFormPackage,
             &ResultSize
           );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  (*HiiFormPackage) = AllocatePool (ResultSize);
  if (*HiiFormPackage == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    return Status;
  }

  //
  // Get HiiFormPackage by HiiHandle
  //
  Size   = ResultSize;
  ResultSize    = 0;
  Status = ExportFormPackages (
             &mPrivate,
             DataBaseRecord->Handle,
             DataBaseRecord->PackageList,
             0,
             Size,
             *HiiFormPackage,
             &ResultSize
           );
  if (EFI_ERROR (Status)) {
    FreePool (*HiiFormPackage);
  }

  *PackageSize = Size;

  return Status;
}

/**
 This function parses Form Package to get the efi varstore info according to the request ConfigHdr.

 @param[in]  DataBaseRecord        The DataBaseRecord instance contains the found Hii handle and package.
 @param[in]  ConfigHdr             Request string ConfigHdr. If it is NULL,
                                   the first found varstore will be as ConfigHdr.
 @param[out] IsEfiVarstore         Whether the request storage type is efi varstore type.
 @param[out] EfiVarStore           The efi varstore info which will return.
**/
EFI_STATUS
GetVarStoreType (
  IN     HII_DATABASE_RECORD        *DataBaseRecord,
  IN     EFI_STRING                 ConfigHdr,
  OUT    BOOLEAN                    *IsEfiVarstore,
  OUT    EFI_IFR_VARSTORE_EFI       **EfiVarStore

  )
{
  EFI_STATUS               Status;
  UINTN                    IfrOffset;
  EFI_IFR_OP_HEADER        *IfrOpHdr;
  CHAR16                   *VarStoreName;
  UINTN                    NameSize;
  EFI_STRING               GuidStr;
  EFI_STRING               NameStr;
  EFI_STRING               TempStr;
  UINTN                    LengthString;
  UINT8                    *HiiFormPackage;
  UINTN                    PackageSize;
  EFI_IFR_VARSTORE_EFI     *IfrEfiVarStore;

  HiiFormPackage = NULL;
  LengthString     = 0;
  Status           = EFI_SUCCESS;
  GuidStr          = NULL;
  NameStr          = NULL;
  TempStr          = NULL;
  *IsEfiVarstore   = FALSE;

  Status = GetFormPackageData(DataBaseRecord, &HiiFormPackage, &PackageSize);
  if (Status != EFI_SUCCESS) {
    return Status;
  }

  IfrOffset   = sizeof (EFI_HII_PACKAGE_HEADER);
  while (IfrOffset < PackageSize) {
    IfrOpHdr  = (EFI_IFR_OP_HEADER *) (HiiFormPackage + IfrOffset);
    IfrOffset += IfrOpHdr->Length;

    if (IfrOpHdr->OpCode == EFI_IFR_VARSTORE_EFI_OP ) {
      IfrEfiVarStore = (EFI_IFR_VARSTORE_EFI *) IfrOpHdr;
      //
      // If the length is small than the structure, this is from old efi
      // varstore definition. Old efi varstore get config directly from
      // GetVariable function.
      //
      if (IfrOpHdr->Length < sizeof (EFI_IFR_VARSTORE_EFI)) {
        continue;
      }

      NameSize = AsciiStrSize ((CHAR8 *)IfrEfiVarStore->Name);
      VarStoreName = AllocateZeroPool (NameSize * sizeof (CHAR16));
      if (VarStoreName == NULL) {
        Status = EFI_OUT_OF_RESOURCES;
        goto Done;
      }
      AsciiStrToUnicodeStrS ((CHAR8 *) IfrEfiVarStore->Name, VarStoreName, NameSize);

      GuidStr = NULL;
      NameStr = NULL;
      GenerateSubStr (L"GUID=", sizeof (EFI_GUID), (VOID *) &IfrEfiVarStore->Guid, 1, &GuidStr);
      if (GuidStr == NULL) {
        FreePool (VarStoreName);
        Status = EFI_OUT_OF_RESOURCES;
        goto Done;
      }
      GenerateSubStr (L"NAME=", StrLen (VarStoreName) * sizeof (CHAR16), (VOID *) VarStoreName, 2, &NameStr);
      if (NameStr == NULL) {
        FreePool (GuidStr);
        FreePool (VarStoreName);
        Status = EFI_OUT_OF_RESOURCES;
        goto Done;
      }
      LengthString = StrLen (GuidStr);
      LengthString = LengthString + StrLen (NameStr) + 1;
      TempStr = AllocateZeroPool (LengthString * sizeof (CHAR16));
      if (TempStr == NULL) {
        FreePool (GuidStr);
        FreePool (NameStr);
        FreePool (VarStoreName);
        Status = EFI_OUT_OF_RESOURCES;
        goto Done;
      }
      StrCpyS (TempStr, LengthString, GuidStr);
      StrCatS (TempStr, LengthString, NameStr);
      if (ConfigHdr == NULL || StrnCmp (ConfigHdr, TempStr, StrLen (TempStr)) == 0) {
        *EfiVarStore = (EFI_IFR_VARSTORE_EFI *) AllocateZeroPool (IfrOpHdr->Length);
        if (*EfiVarStore == NULL) {
          FreePool (VarStoreName);
          FreePool (GuidStr);
          FreePool (NameStr);
          FreePool (TempStr);
          Status = EFI_OUT_OF_RESOURCES;
          goto Done;
        }
        *IsEfiVarstore = TRUE;
        CopyMem (*EfiVarStore, IfrEfiVarStore, IfrOpHdr->Length);
      }

      //
      // Free alllocated temp string.
      //
      FreePool (VarStoreName);
      FreePool (GuidStr);
      FreePool (NameStr);
      FreePool (TempStr);
    }
  }
Done:
  if (HiiFormPackage != NULL) {
    FreePool (HiiFormPackage);
  }

  return Status;
}

/**
 This function parses Form Package to get the block array and the default
 value array according to the request ConfigHdr.

 @param[in]      Package               Pointer to the form package data.
 @param[in]      PackageLength         Length of the pacakge.
 @param[in]      ConfigHdr             Request string ConfigHdr. If it is NULL,
                                       the first found varstore will be as ConfigHdr.
 @param[in]      RequestBlockArray     The block array is retrieved from the request string.
 @param[in, out] VarStorageData        VarStorage structure contains the got block and default value.
 @param[out]     PIfrDefaultIdArray    Point to the got default id and default name array.

 @retval EFI_SUCCESS           The block array and the default value array are got.
 @retval EFI_INVALID_PARAMETER The varstore defintion in the differnt form pacakges
                               are conflicted.
 @retval EFI_OUT_OF_RESOURCES  No enough memory.
**/
EFI_STATUS
EFIAPI
ParseIfrData (
  IN     UINT8               *Package,
  IN     UINT32              PackageLength,
  IN     EFI_STRING          ConfigHdr,
  IN     IFR_BLOCK_DATA      *RequestBlockArray,
  IN OUT IFR_VARSTORAGE_DATA *VarStorageData,
  OUT    IFR_DEFAULT_DATA    *DefaultIdArray
  )
{
  EFI_STATUS               Status;
  UINTN                    IfrOffset;
  EFI_IFR_VARSTORE         *IfrVarStore;
  EFI_IFR_VARSTORE_EFI     *IfrEfiVarStore;
  EFI_IFR_OP_HEADER        *IfrOpHdr;
  EFI_IFR_ONE_OF           *IfrOneOf;
  EFI_IFR_REF4             *IfrRef;
  EFI_IFR_ONE_OF_OPTION    *IfrOneOfOption;
  EFI_IFR_DEFAULT          *IfrDefault;
  EFI_IFR_ORDERED_LIST     *IfrOrderedList;
  EFI_IFR_CHECKBOX         *IfrCheckBox;
  EFI_IFR_PASSWORD         *IfrPassword;
  EFI_IFR_STRING           *IfrString;
  EFI_IFR_DATE             *IfrDate;
  EFI_IFR_TIME             *IfrTime;
  IFR_DEFAULT_DATA         DefaultData;
  IFR_DEFAULT_DATA         *DefaultDataPtr;
  IFR_BLOCK_DATA           *BlockData;
  CHAR16                   *VarStoreName;
  UINTN                    NameSize;
  UINT16                   VarOffset;
  UINT16                   VarWidth;
  UINT16                   VarDefaultId;
  EFI_STRING               GuidStr;
  EFI_STRING               NameStr;
  EFI_STRING               TempStr;
  UINTN                    LengthString;
  BOOLEAN                  FirstOneOfOption;
  LIST_ENTRY               *LinkData;
  LIST_ENTRY               *LinkDefault;

  LengthString     = 0;
  Status           = EFI_SUCCESS;
  GuidStr          = NULL;
  NameStr          = NULL;
  TempStr          = NULL;
  BlockData        = NULL;
  DefaultDataPtr   = NULL;
  FirstOneOfOption = FALSE;
  ZeroMem (&DefaultData, sizeof (IFR_DEFAULT_DATA));

  //
  // Go through the form package to parse OpCode one by one.
  //
  IfrOffset   = sizeof (EFI_HII_PACKAGE_HEADER);
  while (IfrOffset < PackageLength) {
    IfrOpHdr  = (EFI_IFR_OP_HEADER *) (Package + IfrOffset);

    switch (IfrOpHdr->OpCode) {
    case EFI_IFR_VARSTORE_OP:
      //
      // VarStore is found. Don't need to search any more.
      //
      if (VarStorageData->Size != 0) {
        break;
      }

      //
      // Get the requied varstore information
      // Add varstore by Guid and Name in ConfigHdr
      // Make sure Offset is in varstore size and varstoreid
      //
      IfrVarStore = (EFI_IFR_VARSTORE *) IfrOpHdr;

      NameSize = AsciiStrSize ((CHAR8 *)IfrVarStore->Name);
      VarStoreName = AllocateZeroPool (NameSize * sizeof (CHAR16));
      if (VarStoreName == NULL) {
        Status = EFI_OUT_OF_RESOURCES;
        goto Done;
      }
      AsciiStrToUnicodeStrS ((CHAR8 *)IfrVarStore->Name, VarStoreName, NameSize);
      GuidStr = NULL;
      NameStr = NULL;
      GenerateSubStr (L"GUID=", sizeof (EFI_GUID), (VOID *) &IfrVarStore->Guid, 1, &GuidStr);
      if (GuidStr == NULL) {
        FreePool (VarStoreName);
        Status = EFI_OUT_OF_RESOURCES;
        goto Done;
      }
      GenerateSubStr (L"NAME=", StrLen (VarStoreName) * sizeof (CHAR16), (VOID *) VarStoreName, 2, &NameStr);
      if (NameStr == NULL) {
        FreePool (GuidStr);
        FreePool (VarStoreName);
        Status = EFI_OUT_OF_RESOURCES;
        goto Done;
      }
      LengthString = StrLen (GuidStr);
      LengthString = LengthString + StrLen (NameStr) + 1;
      TempStr = AllocateZeroPool (LengthString * sizeof (CHAR16));
      if (TempStr == NULL) {
        FreePool (GuidStr);
        FreePool (NameStr);
        FreePool (VarStoreName);
        Status = EFI_OUT_OF_RESOURCES;
        goto Done;
      }
      StrCpyS (TempStr, LengthString, GuidStr);
      StrCatS (TempStr, LengthString, NameStr);
      if (ConfigHdr == NULL || StrnCmp (ConfigHdr, TempStr, StrLen (TempStr)) == 0) {
        //
        // Find the matched VarStore
        //
        CopyGuid (&VarStorageData->Guid, (EFI_GUID *) (VOID *) &IfrVarStore->Guid);
        VarStorageData->VarStoreId = IfrVarStore->VarStoreId;
        VarStorageData->Size       = IfrVarStore->Size;
        VarStorageData->Name       = VarStoreName;
      } else {
        //
        // No found, free the allocated memory
        //
        FreePool (VarStoreName);
      }
      //
      // Free alllocated temp string.
      //
      FreePool (GuidStr);
      FreePool (NameStr);
      FreePool (TempStr);
      break;

    case EFI_IFR_VARSTORE_EFI_OP:
      //
      // VarStore is found. Don't need to search any more.
      //
      if (VarStorageData->Size != 0) {
        break;
      }

      //
      // Get the requied varstore information
      // Add varstore by Guid and Name in ConfigHdr
      // Make sure Offset is in varstore size and varstoreid
      //
      IfrEfiVarStore = (EFI_IFR_VARSTORE_EFI *) IfrOpHdr;

      //
      // If the length is small than the structure, this is from old efi
      // varstore definition. Old efi varstore get config directly from
      // GetVariable function.
      //
      if (IfrOpHdr->Length < sizeof (EFI_IFR_VARSTORE_EFI)) {
        break;
      }

      NameSize = AsciiStrSize ((CHAR8 *)IfrEfiVarStore->Name);
      VarStoreName = AllocateZeroPool (NameSize * sizeof (CHAR16));
      if (VarStoreName == NULL) {
        Status = EFI_OUT_OF_RESOURCES;
        goto Done;
      }
      AsciiStrToUnicodeStrS ((CHAR8 *)IfrEfiVarStore->Name, VarStoreName, NameSize);
      GuidStr = NULL;
      NameStr = NULL;
      GenerateSubStr (L"GUID=", sizeof (EFI_GUID), (VOID *) &IfrEfiVarStore->Guid, 1, &GuidStr);
      if (GuidStr == NULL) {
        FreePool (VarStoreName);
        Status = EFI_OUT_OF_RESOURCES;
        goto Done;
      }
      GenerateSubStr (L"NAME=", StrLen (VarStoreName) * sizeof (CHAR16), (VOID *) VarStoreName, 2, &NameStr);
      if (NameStr == NULL) {
        FreePool (GuidStr);
        FreePool (VarStoreName);
        Status = EFI_OUT_OF_RESOURCES;
        goto Done;
      }

      LengthString = StrLen (GuidStr);
      LengthString = LengthString + StrLen (NameStr) + 1;
      TempStr = AllocateZeroPool (LengthString * sizeof (CHAR16));
      if (TempStr == NULL) {
        FreePool (GuidStr);
        FreePool (NameStr);
        FreePool (VarStoreName);
        Status = EFI_OUT_OF_RESOURCES;
        goto Done;
      }
      StrCpyS (TempStr, LengthString, GuidStr);
      StrCatS (TempStr, LengthString, NameStr);
      if (ConfigHdr == NULL || StrnCmp (ConfigHdr, TempStr, StrLen (TempStr)) == 0) {
        //
        // Find the matched VarStore
        //
        CopyGuid (&VarStorageData->Guid, (EFI_GUID *) (VOID *) &IfrEfiVarStore->Guid);
        VarStorageData->VarStoreId = IfrEfiVarStore->VarStoreId;
        VarStorageData->Size       = IfrEfiVarStore->Size;
        VarStorageData->Name       = VarStoreName;
      } else {
        //
        // No found, free the allocated memory
        //
        FreePool (VarStoreName);
      }
      //
      // Free alllocated temp string.
      //
      FreePool (GuidStr);
      FreePool (NameStr);
      FreePool (TempStr);
      break;

    case EFI_IFR_DEFAULTSTORE_OP:
      //
      // Add new the map between default id and default name.
      //
      DefaultDataPtr = (IFR_DEFAULT_DATA *) AllocateZeroPool (sizeof (IFR_DEFAULT_DATA));
      if (DefaultDataPtr == NULL) {
        Status = EFI_OUT_OF_RESOURCES;
        goto Done;
      }
      DefaultDataPtr->DefaultId   = ((EFI_IFR_DEFAULTSTORE *) IfrOpHdr)->DefaultId;
      InsertTailList (&DefaultIdArray->Entry, &DefaultDataPtr->Entry);
      DefaultDataPtr = NULL;
      break;

    case EFI_IFR_FORM_OP:
    case EFI_IFR_FORM_MAP_OP:
      //
      // No matched varstore is found and directly return.
      //
      if (VarStorageData->Size == 0) {
        Status = EFI_SUCCESS;
        goto Done;
      }
      break;

    case EFI_IFR_REF_OP:
      //
      // Ref question is not in IFR Form. This IFR form is not valid.
      //
      if (VarStorageData->Size == 0) {
        Status = EFI_INVALID_PARAMETER;
        goto Done;
      }
      //
      // Check whether this question is for the requested varstore.
      //
      IfrRef = (EFI_IFR_REF4 *) IfrOpHdr;
      if (IfrRef->Question.VarStoreId != VarStorageData->VarStoreId) {
        break;
      }

      //
      // Get Offset/Width by Question header.
      //
      VarOffset = IfrRef->Question.VarStoreInfo.VarOffset;
      VarWidth  = (UINT16) (sizeof (EFI_HII_REF));
      //
      // Check whether this question is in requested block array.
      //
      if (!BlockArrayCheck (RequestBlockArray, VarOffset, VarWidth)) {
        //
        // This question is not in the requested string. Skip it.
        //
        break;
      }

      //
      // Check this var question is in the var storage
      //
      if ((VarOffset + VarWidth) > VarStorageData->Size) {
        Status = EFI_INVALID_PARAMETER;
        goto Done;
      }

      //
      // Set Block Data
      //
      BlockData = (IFR_BLOCK_DATA *) AllocateZeroPool (sizeof (IFR_BLOCK_DATA));
      if (BlockData == NULL) {
        Status = EFI_OUT_OF_RESOURCES;
        goto Done;
      }
      BlockData->Offset     = VarOffset;
      BlockData->Width      = VarWidth;
      BlockData->QuestionId = IfrRef->Question.QuestionId;
      BlockData->OpCode     = IfrOpHdr->OpCode;
      BlockData->Scope      = IfrOpHdr->Scope;
      InitializeListHead (&BlockData->DefaultValueEntry);
      //
      // Add Block Data into VarStorageData BlockEntry
      //
      InsertBlockData (&VarStorageData->BlockEntry, &BlockData);
      break;

    case EFI_IFR_ONE_OF_OP:
    case EFI_IFR_NUMERIC_OP:
      //
      // Numeric and OneOf has the same opcode structure.
      //

      //
      // Numeric and OneOf question is not in IFR Form. This IFR form is not valid.
      //
      if (VarStorageData->Size == 0) {
        Status = EFI_INVALID_PARAMETER;
        goto Done;
      }
      //
      // Check whether this question is for the requested varstore.
      //
      IfrOneOf = (EFI_IFR_ONE_OF *) IfrOpHdr;
      if (IfrOneOf->Question.VarStoreId != VarStorageData->VarStoreId) {
        break;
      }

      //
      // Get Offset/Width by Question header and OneOf Flags
      //
      VarOffset = IfrOneOf->Question.VarStoreInfo.VarOffset;
      VarWidth  = (UINT16) (1 << (IfrOneOf->Flags & EFI_IFR_NUMERIC_SIZE));
      //
      // Check whether this question is in requested block array.
      //
      if (!BlockArrayCheck (RequestBlockArray, VarOffset, VarWidth)) {
        //
        // This question is not in the requested string. Skip it.
        //
        break;
      }

      //
      // Check this var question is in the var storage
      //
      if ((VarOffset + VarWidth) > VarStorageData->Size) {
        Status = EFI_INVALID_PARAMETER;
        goto Done;
      }

      //
      // Set Block Data
      //
      BlockData = (IFR_BLOCK_DATA *) AllocateZeroPool (sizeof (IFR_BLOCK_DATA));
      if (BlockData == NULL) {
        Status = EFI_OUT_OF_RESOURCES;
        goto Done;
      }
      BlockData->Offset     = VarOffset;
      BlockData->Width      = VarWidth;
      BlockData->QuestionId = IfrOneOf->Question.QuestionId;
      BlockData->OpCode     = IfrOpHdr->OpCode;
      BlockData->Scope      = IfrOpHdr->Scope;
      InitializeListHead (&BlockData->DefaultValueEntry);
      //
      // Add Block Data into VarStorageData BlockEntry
      //
      InsertBlockData (&VarStorageData->BlockEntry, &BlockData);

      if (IfrOpHdr->OpCode == EFI_IFR_ONE_OF_OP) {
        //
        // Set this flag to TRUE for the first oneof option.
        //
        FirstOneOfOption = TRUE;
      } else if (IfrOpHdr->OpCode == EFI_IFR_NUMERIC_OP) {
        //
        // Numeric minimum value will be used as default value when no default is specified.
        //
        DefaultData.Type        = DefaultValueFromDefault;
        switch (IfrOneOf->Flags & EFI_IFR_NUMERIC_SIZE) {
        case EFI_IFR_NUMERIC_SIZE_1:
          DefaultData.Value.u8 = IfrOneOf->data.u8.MinValue;
          break;

        case EFI_IFR_NUMERIC_SIZE_2:
          CopyMem (&DefaultData.Value.u16, &IfrOneOf->data.u16.MinValue, sizeof (UINT16));
          break;

        case EFI_IFR_NUMERIC_SIZE_4:
          CopyMem (&DefaultData.Value.u32, &IfrOneOf->data.u32.MinValue, sizeof (UINT32));
          break;

        case EFI_IFR_NUMERIC_SIZE_8:
          CopyMem (&DefaultData.Value.u64, &IfrOneOf->data.u64.MinValue, sizeof (UINT64));
          break;
        }
        //
        // Set default value base on the DefaultId list get from IFR data.
        //
        for (LinkData = DefaultIdArray->Entry.ForwardLink; LinkData != &DefaultIdArray->Entry; LinkData = LinkData->ForwardLink) {
          DefaultDataPtr = BASE_CR (LinkData, IFR_DEFAULT_DATA, Entry);
          DefaultData.DefaultId   = DefaultDataPtr->DefaultId;
          InsertDefaultValue (BlockData, &DefaultData);
        }
      }
      break;

    case EFI_IFR_ORDERED_LIST_OP:
      //
      // offset by question header
      // width by EFI_IFR_ORDERED_LIST MaxContainers * OneofOption Type
      // no default value and default id, how to define its default value?
      //

      //
      // OrderedList question is not in IFR Form. This IFR form is not valid.
      //
      if (VarStorageData->Size == 0) {
        Status = EFI_INVALID_PARAMETER;
        goto Done;
      }
      //
      // Check whether this question is for the requested varstore.
      //
      IfrOrderedList = (EFI_IFR_ORDERED_LIST *) IfrOpHdr;
      if (IfrOrderedList->Question.VarStoreId != VarStorageData->VarStoreId) {
        BlockData = NULL;
        break;
      }

      //
      // Get Offset/Width by Question header and OneOf Flags
      //
      VarOffset = IfrOrderedList->Question.VarStoreInfo.VarOffset;
      VarWidth  = IfrOrderedList->MaxContainers;

      //
      // Set Block Data
      //
      BlockData = (IFR_BLOCK_DATA *) AllocateZeroPool (sizeof (IFR_BLOCK_DATA));
      if (BlockData == NULL) {
        Status = EFI_OUT_OF_RESOURCES;
        goto Done;
      }
      BlockData->Offset     = VarOffset;
      BlockData->Width      = VarWidth;
      BlockData->QuestionId = IfrOrderedList->Question.QuestionId;
      BlockData->OpCode     = IfrOpHdr->OpCode;
      BlockData->Scope      = IfrOpHdr->Scope;
      InitializeListHead (&BlockData->DefaultValueEntry);
      break;

    case EFI_IFR_CHECKBOX_OP:
      //
      // EFI_IFR_DEFAULT_OP
      // offset by question header
      // width is 1 sizeof (BOOLEAN)
      // default id by CheckBox Flags if CheckBox flags (Default or Mau) is set, the default value is 1 to be set.
      // value by DefaultOption
      // default id by DeaultOption DefaultId can override CheckBox Flags and Default value.
      //

      //
      // CheckBox question is not in IFR Form. This IFR form is not valid.
      //
      if (VarStorageData->Size == 0) {
        Status = EFI_INVALID_PARAMETER;
        goto Done;
      }
      //
      // Check whether this question is for the requested varstore.
      //
      IfrCheckBox = (EFI_IFR_CHECKBOX *) IfrOpHdr;
      if (IfrCheckBox->Question.VarStoreId != VarStorageData->VarStoreId) {
        break;
      }

      //
      // Get Offset/Width by Question header and OneOf Flags
      //
      VarOffset = IfrCheckBox->Question.VarStoreInfo.VarOffset;
      VarWidth  = (UINT16) sizeof (BOOLEAN);

      //
      // Check whether this question is in requested block array.
      //
      if (!BlockArrayCheck (RequestBlockArray, VarOffset, VarWidth)) {
        //
        // This question is not in the requested string. Skip it.
        //
        break;
      }

      //
      // Check this var question is in the var storage
      //
      if ((VarOffset + VarWidth) > VarStorageData->Size) {
        Status = EFI_INVALID_PARAMETER;
        goto Done;
      }

      //
      // Set Block Data
      //
      BlockData = (IFR_BLOCK_DATA *) AllocateZeroPool (sizeof (IFR_BLOCK_DATA));
      if (BlockData == NULL) {
        Status = EFI_OUT_OF_RESOURCES;
        goto Done;
      }
      BlockData->Offset     = VarOffset;
      BlockData->Width      = VarWidth;
      BlockData->QuestionId = IfrCheckBox->Question.QuestionId;
      BlockData->OpCode     = IfrOpHdr->OpCode;
      BlockData->Scope      = IfrOpHdr->Scope;
      InitializeListHead (&BlockData->DefaultValueEntry);
      //
      // Add Block Data into VarStorageData BlockEntry
      //
      InsertBlockData (&VarStorageData->BlockEntry, &BlockData);

      //
      // Add default value for standard ID by CheckBox Flag
      //
      VarDefaultId = EFI_HII_DEFAULT_CLASS_STANDARD;
      //
      // Prepare new DefaultValue
      //
      DefaultData.DefaultId   = VarDefaultId;
      if ((IfrCheckBox->Flags & EFI_IFR_CHECKBOX_DEFAULT) == EFI_IFR_CHECKBOX_DEFAULT) {
        //
        // When flag is set, defautl value is TRUE.
        //
        DefaultData.Type    = DefaultValueFromFlag;
        DefaultData.Value.b = TRUE;
      } else {
        //
        // When flag is not set, defautl value is FASLE.
        //
        DefaultData.Type    = DefaultValueFromDefault;
        DefaultData.Value.b = FALSE;
      }
      //
      // Add DefaultValue into current BlockData
      //
      InsertDefaultValue (BlockData, &DefaultData);

      //
      // Add default value for Manufacture ID by CheckBox Flag
      //
      VarDefaultId = EFI_HII_DEFAULT_CLASS_MANUFACTURING;
      //
      // Prepare new DefaultValue
      //
      DefaultData.DefaultId   = VarDefaultId;
      if ((IfrCheckBox->Flags & EFI_IFR_CHECKBOX_DEFAULT_MFG) == EFI_IFR_CHECKBOX_DEFAULT_MFG) {
        //
        // When flag is set, defautl value is TRUE.
        //
        DefaultData.Type    = DefaultValueFromFlag;
        DefaultData.Value.b = TRUE;
      } else {
        //
        // When flag is not set, defautl value is FASLE.
        //
        DefaultData.Type    = DefaultValueFromDefault;
        DefaultData.Value.b = FALSE;
      }
      //
      // Add DefaultValue into current BlockData
      //
      InsertDefaultValue (BlockData, &DefaultData);
      break;

    case EFI_IFR_DATE_OP:
      //
      // offset by question header
      // width MaxSize * sizeof (CHAR16)
      // no default value, only block array
      //

      //
      // Date question is not in IFR Form. This IFR form is not valid.
      //
      if (VarStorageData->Size == 0) {
        Status = EFI_INVALID_PARAMETER;
        goto Done;
      }
      //
      // Check whether this question is for the requested varstore.
      //
      IfrDate = (EFI_IFR_DATE *) IfrOpHdr;
      if (IfrDate->Question.VarStoreId != VarStorageData->VarStoreId) {
        break;
      }

      //
      // Get Offset/Width by Question header and OneOf Flags
      //
      VarOffset = IfrDate->Question.VarStoreInfo.VarOffset;
      VarWidth  = (UINT16) sizeof (EFI_HII_DATE);

      //
      // Check whether this question is in requested block array.
      //
      if (!BlockArrayCheck (RequestBlockArray, VarOffset, VarWidth)) {
        //
        // This question is not in the requested array. Skip it.
        //
        break;
      }

      //
      // Check this var question is in the var storage
      //
      if ((VarOffset + VarWidth) > VarStorageData->Size) {
        Status = EFI_INVALID_PARAMETER;
        goto Done;
      }

      //
      // Set Block Data
      //
      BlockData = (IFR_BLOCK_DATA *) AllocateZeroPool (sizeof (IFR_BLOCK_DATA));
      if (BlockData == NULL) {
        Status = EFI_OUT_OF_RESOURCES;
        goto Done;
      }
      BlockData->Offset     = VarOffset;
      BlockData->Width      = VarWidth;
      BlockData->QuestionId = IfrDate->Question.QuestionId;
      BlockData->OpCode     = IfrOpHdr->OpCode;
      BlockData->Scope      = IfrOpHdr->Scope;
      InitializeListHead (&BlockData->DefaultValueEntry);

      //
      // Add Block Data into VarStorageData BlockEntry
      //
      InsertBlockData (&VarStorageData->BlockEntry, &BlockData);
      break;

    case EFI_IFR_TIME_OP:
      //
      // offset by question header
      // width MaxSize * sizeof (CHAR16)
      // no default value, only block array
      //

      //
      // Time question is not in IFR Form. This IFR form is not valid.
      //
      if (VarStorageData->Size == 0) {
        Status = EFI_INVALID_PARAMETER;
        goto Done;
      }
      //
      // Check whether this question is for the requested varstore.
      //
      IfrTime = (EFI_IFR_TIME *) IfrOpHdr;
      if (IfrTime->Question.VarStoreId != VarStorageData->VarStoreId) {
        break;
      }

      //
      // Get Offset/Width by Question header and OneOf Flags
      //
      VarOffset = IfrTime->Question.VarStoreInfo.VarOffset;
      VarWidth  = (UINT16) sizeof (EFI_HII_TIME);

      //
      // Check whether this question is in requested block array.
      //
      if (!BlockArrayCheck (RequestBlockArray, VarOffset, VarWidth)) {
        //
        // This question is not in the requested array. Skip it.
        //
        break;
      }

      //
      // Check this var question is in the var storage
      //
      if ((VarOffset + VarWidth) > VarStorageData->Size) {
        Status = EFI_INVALID_PARAMETER;
        goto Done;
      }

      //
      // Set Block Data
      //
      BlockData = (IFR_BLOCK_DATA *) AllocateZeroPool (sizeof (IFR_BLOCK_DATA));
      if (BlockData == NULL) {
        Status = EFI_OUT_OF_RESOURCES;
        goto Done;
      }
      BlockData->Offset     = VarOffset;
      BlockData->Width      = VarWidth;
      BlockData->QuestionId = IfrTime->Question.QuestionId;
      BlockData->OpCode     = IfrOpHdr->OpCode;
      BlockData->Scope      = IfrOpHdr->Scope;
      InitializeListHead (&BlockData->DefaultValueEntry);

        //
      // Add Block Data into VarStorageData BlockEntry
      //
      InsertBlockData (&VarStorageData->BlockEntry, &BlockData);
      break;

    case EFI_IFR_STRING_OP:
      //
      // offset by question header
      // width MaxSize * sizeof (CHAR16)
      // no default value, only block array
      //

      //
      // String question is not in IFR Form. This IFR form is not valid.
      //
      if (VarStorageData->Size == 0) {
        Status = EFI_INVALID_PARAMETER;
        goto Done;
      }
      //
      // Check whether this question is for the requested varstore.
      //
      IfrString = (EFI_IFR_STRING *) IfrOpHdr;
      if (IfrString->Question.VarStoreId != VarStorageData->VarStoreId) {
        break;
      }

      //
      // Get Offset/Width by Question header and OneOf Flags
      //
      VarOffset = IfrString->Question.VarStoreInfo.VarOffset;
      VarWidth  = (UINT16) (IfrString->MaxSize * sizeof (UINT16));

      //
      // Check whether this question is in requested block array.
      //
      if (!BlockArrayCheck (RequestBlockArray, VarOffset, VarWidth)) {
        //
        // This question is not in the requested string. Skip it.
        //
        break;
      }

      //
      // Check this var question is in the var storage
      //
      if ((VarOffset + VarWidth) > VarStorageData->Size) {
        Status = EFI_INVALID_PARAMETER;
        goto Done;
      }

      //
      // Set Block Data
      //
      BlockData = (IFR_BLOCK_DATA *) AllocateZeroPool (sizeof (IFR_BLOCK_DATA));
      if (BlockData == NULL) {
        Status = EFI_OUT_OF_RESOURCES;
        goto Done;
      }
      BlockData->Offset     = VarOffset;
      BlockData->Width      = VarWidth;
      BlockData->QuestionId = IfrString->Question.QuestionId;
      BlockData->OpCode     = IfrOpHdr->OpCode;
      InitializeListHead (&BlockData->DefaultValueEntry);

      //
      // Add Block Data into VarStorageData BlockEntry
      //
      InsertBlockData (&VarStorageData->BlockEntry, &BlockData);

      //
      // No default value for string.
      //
      BlockData = NULL;
      break;

    case EFI_IFR_PASSWORD_OP:
      //
      // offset by question header
      // width MaxSize * sizeof (CHAR16)
      // no default value, only block array
      //

      //
      // Password question is not in IFR Form. This IFR form is not valid.
      //
      if (VarStorageData->Size == 0) {
        Status = EFI_INVALID_PARAMETER;
        goto Done;
      }
      //
      // Check whether this question is for the requested varstore.
      //
      IfrPassword = (EFI_IFR_PASSWORD *) IfrOpHdr;
      if (IfrPassword->Question.VarStoreId != VarStorageData->VarStoreId) {
        break;
      }

      //
      // Get Offset/Width by Question header and OneOf Flags
      //
      VarOffset = IfrPassword->Question.VarStoreInfo.VarOffset;
      VarWidth  = (UINT16) (IfrPassword->MaxSize * sizeof (UINT16));

      //
      // Check whether this question is in requested block array.
      //
      if (!BlockArrayCheck (RequestBlockArray, VarOffset, VarWidth)) {
        //
        // This question is not in the requested string. Skip it.
        //
        break;
      }

      //
      // Check this var question is in the var storage
      //
      if ((VarOffset + VarWidth) > VarStorageData->Size) {
        Status = EFI_INVALID_PARAMETER;
        goto Done;
      }

      //
      // Set Block Data
      //
      BlockData = (IFR_BLOCK_DATA *) AllocateZeroPool (sizeof (IFR_BLOCK_DATA));
      if (BlockData == NULL) {
        Status = EFI_OUT_OF_RESOURCES;
        goto Done;
      }
      BlockData->Offset     = VarOffset;
      BlockData->Width      = VarWidth;
      BlockData->QuestionId = IfrPassword->Question.QuestionId;
      BlockData->OpCode     = IfrOpHdr->OpCode;
      InitializeListHead (&BlockData->DefaultValueEntry);

      //
      // Add Block Data into VarStorageData BlockEntry
      //
      InsertBlockData (&VarStorageData->BlockEntry, &BlockData);

      //
      // No default value for string.
      //
      BlockData = NULL;
      break;

    case EFI_IFR_ONE_OF_OPTION_OP:
      //
      // No matched block data is ignored.
      //
      if (BlockData == NULL || BlockData->Scope == 0) {
        break;
      }

      IfrOneOfOption = (EFI_IFR_ONE_OF_OPTION *) IfrOpHdr;
      if (BlockData->OpCode == EFI_IFR_ORDERED_LIST_OP) {
        //
        // Get ordered list option data type.
        //
        if (IfrOneOfOption->Type == EFI_IFR_TYPE_NUM_SIZE_8 || IfrOneOfOption->Type == EFI_IFR_TYPE_BOOLEAN) {
          VarWidth = 1;
        } else if (IfrOneOfOption->Type == EFI_IFR_TYPE_NUM_SIZE_16) {
          VarWidth = 2;
        } else if (IfrOneOfOption->Type == EFI_IFR_TYPE_NUM_SIZE_32) {
          VarWidth = 4;
        } else if (IfrOneOfOption->Type == EFI_IFR_TYPE_NUM_SIZE_64) {
          VarWidth = 8;
        } else {
          //
          // Invalid ordered list option data type.
          //
          Status = EFI_INVALID_PARAMETER;
          FreePool (BlockData);
          goto Done;
        }

        //
        // Calculate Ordered list QuestionId width.
        //
        BlockData->Width = (UINT16) (BlockData->Width * VarWidth);
        //
        // Check whether this question is in requested block array.
        //
        if (!BlockArrayCheck (RequestBlockArray, BlockData->Offset, BlockData->Width)) {
          //
          // This question is not in the requested string. Skip it.
          //
          FreePool (BlockData);
          BlockData = NULL;
          break;
        }
        //
        // Check this var question is in the var storage
        //
        if ((BlockData->Offset + BlockData->Width) > VarStorageData->Size) {
          Status = EFI_INVALID_PARAMETER;
          FreePool (BlockData);
          goto Done;
        }
        //
        // Add Block Data into VarStorageData BlockEntry
        //
        InsertBlockData (&VarStorageData->BlockEntry, &BlockData);
        //
        // No default data for OrderedList.
        //
        BlockData = NULL;
        break;
      }

      //
      // 1. Set default value for OneOf option when flag field has default attribute.
      //
      if (((IfrOneOfOption->Flags & EFI_IFR_OPTION_DEFAULT) == EFI_IFR_OPTION_DEFAULT) ||
          ((IfrOneOfOption->Flags & EFI_IFR_OPTION_DEFAULT_MFG) == EFI_IFR_OPTION_DEFAULT_MFG)) {
        //
        // This flag is used to specify whether this option is the first. Set it to FALSE for the following options.
        // The first oneof option value will be used as default value when no default value is specified.
        //
        FirstOneOfOption = FALSE;

        // Prepare new DefaultValue
        //
        DefaultData.Type  = DefaultValueFromFlag;
        CopyMem (&DefaultData.Value.u64, &IfrOneOfOption->Value.u64, sizeof (UINT64));
        if ((IfrOneOfOption->Flags & EFI_IFR_OPTION_DEFAULT) == EFI_IFR_OPTION_DEFAULT) {
          DefaultData.DefaultId = EFI_HII_DEFAULT_CLASS_STANDARD;
          InsertDefaultValue (BlockData, &DefaultData);
        }
        if ((IfrOneOfOption->Flags & EFI_IFR_OPTION_DEFAULT_MFG) == EFI_IFR_OPTION_DEFAULT_MFG) {
          DefaultData.DefaultId = EFI_HII_DEFAULT_CLASS_MANUFACTURING;
          InsertDefaultValue (BlockData, &DefaultData);
        }


      }

      //
      // 2. Set as the default value when this is the first option.
      // The first oneof option value will be used as default value when no default value is specified.
      //
      if (FirstOneOfOption) {
        // This flag is used to specify whether this option is the first. Set it to FALSE for the following options.
        FirstOneOfOption = FALSE;

        //
        // Prepare new DefaultValue
        //
        DefaultData.Type        = DefaultValueFromDefault;
        CopyMem (&DefaultData.Value.u64, &IfrOneOfOption->Value.u64, sizeof (UINT64));
        for (LinkData = DefaultIdArray->Entry.ForwardLink; LinkData != &DefaultIdArray->Entry; LinkData = LinkData->ForwardLink) {
          DefaultDataPtr = BASE_CR (LinkData, IFR_DEFAULT_DATA, Entry);
          DefaultData.DefaultId   = DefaultDataPtr->DefaultId;
          InsertDefaultValue (BlockData, &DefaultData);
        }
      }
      break;

    case EFI_IFR_DEFAULT_OP:
      //
      // Update Current BlockData to the default value.
      //
      if (BlockData == NULL || BlockData->Scope == 0) {
        //
        // No matched block data is ignored.
        //
        break;
      }

      if (BlockData->OpCode == EFI_IFR_ORDERED_LIST_OP) {
        //
        // OrderedList Opcode is no default value.
        //
        break;
      }
      //
      // Get the DefaultId
      //
      IfrDefault     = (EFI_IFR_DEFAULT *) IfrOpHdr;
      VarDefaultId   = IfrDefault->DefaultId;
      //
      // Prepare new DefaultValue
      //
      DefaultData.Type        = DefaultValueFromOpcode;
      DefaultData.DefaultId   = VarDefaultId;
      CopyMem (&DefaultData.Value, &IfrDefault->Value, sizeof (EFI_IFR_TYPE_VALUE));

      // If the value field is expression, set the cleaned flag.
      if (IfrDefault->Type ==  EFI_IFR_TYPE_OTHER) {
        DefaultData.Cleaned = TRUE;
      }
      //
      // Add DefaultValue into current BlockData
      //
      InsertDefaultValue (BlockData, &DefaultData);

      //
      // After insert the default value, reset the cleaned value for next
      // time used. If not set here, need to set the value before everytime
      // use it.
      //
      DefaultData.Cleaned     = FALSE;
      break;
    case EFI_IFR_END_OP:
      //
      // End Opcode is for Var question.
      //
      if (BlockData != NULL && BlockData->Scope > 0) {
        BlockData->Scope--;
      }
      break;
    default:
      if (BlockData != NULL && BlockData->Scope > 0) {
        BlockData->Scope = (UINT8) (BlockData->Scope + IfrOpHdr->Scope);
      }
      break;
    }

    IfrOffset += IfrOpHdr->Length;
  }

Done:
  for (LinkData = VarStorageData->BlockEntry.ForwardLink; LinkData != &VarStorageData->BlockEntry; LinkData = LinkData->ForwardLink) {
    BlockData = BASE_CR (LinkData, IFR_BLOCK_DATA, Entry);
    for (LinkDefault = BlockData->DefaultValueEntry.ForwardLink; LinkDefault != &BlockData->DefaultValueEntry; ) {
      DefaultDataPtr = BASE_CR (LinkDefault, IFR_DEFAULT_DATA, Entry);
      LinkDefault = LinkDefault->ForwardLink;
      if (DefaultDataPtr->Cleaned == TRUE) {
        RemoveEntryList (&DefaultDataPtr->Entry);
        FreePool (DefaultDataPtr);
      }
    }
  }

  return Status;
}

/**
 This function gets the full request string and full default value string by
 parsing IFR data in HII form packages.

 When Request points to NULL string, the request string and default value string
 for each varstore in form package will return.

 @param[in]      DataBaseRecord   The DataBaseRecord instance contains the found Hii handle and package.
 @param[in]      DevicePath       Device Path which Hii Config Access Protocol is registered.
 @param[in, out] Request          Pointer to a null-terminated Unicode string in
                                  <ConfigRequest> format. When it doesn't contain
                                  any RequestElement, it will be updated to return
                                  the full RequestElement retrieved from IFR data.
                                  If it points to NULL, the request string for the first
                                  varstore in form package will be merged into a
                                  <MultiConfigRequest> format string and return.
 @param[in, out] AltCfgResp       Pointer to a null-terminated Unicode string in
                                  <ConfigAltResp> format. When the pointer is to NULL,
                                  the full default value string retrieved from IFR data
                                  will return. When the pinter is to a string, the
                                  full default value string retrieved from IFR data
                                  will be merged into the input string and return.
                                  When Request points to NULL, the default value string
                                  for each varstore in form package will be merged into
                                  a <MultiConfigAltResp> format string and return.
 @param[out]     PointerProgress  Optional parameter, it can be be NULL.
                                  When it is not NULL, if Request is NULL, it returns NULL.
                                  On return, points to a character in the Request
                                  string. Points to the string's null terminator if
                                  request was successful. Points to the most recent
                                  & before the first failing name / value pair (or
                                  the beginning of the string if the failure is in
                                  the first name / value pair) if the request was
                                  not successful.
 @retval EFI_SUCCESS            The Results string is set to the full request string.
                                And AltCfgResp contains all default value string.
 @retval EFI_OUT_OF_RESOURCES   Not enough memory for the return string.
 @retval EFI_NOT_FOUND          The varstore (Guid and Name) in Request string
                                can't be found in Form package.
 @retval EFI_NOT_FOUND          HiiPackage can't be got on the input HiiHandle.
 @retval EFI_INVALID_PARAMETER  Request points to NULL.
**/
EFI_STATUS
EFIAPI
GetFullStringFromHiiFormPackages (
  IN     HII_DATABASE_RECORD        *DataBaseRecord,
  IN     EFI_DEVICE_PATH_PROTOCOL   *DevicePath,
  IN OUT EFI_STRING                 *Request,
  IN OUT EFI_STRING                 *AltCfgResp,
  OUT    EFI_STRING                 *PointerProgress OPTIONAL
  )
{
  EFI_STATUS                   Status;
  UINT8                        *HiiFormPackage;
  UINTN                        PackageSize;
  IFR_BLOCK_DATA               *RequestBlockArray;
  IFR_BLOCK_DATA               *BlockData;
//[-start-221013-IB14860855-remove]//
//  IFR_BLOCK_DATA               *NextBlockData;
//[-end-221013-IB14860855-remove]//
  IFR_DEFAULT_DATA             *DefaultValueData;
  IFR_DEFAULT_DATA             *DefaultId;
  IFR_DEFAULT_DATA             *DefaultIdArray;
  IFR_VARSTORAGE_DATA          *VarStorageData;
  EFI_STRING                   DefaultAltCfgResp;
  EFI_STRING                   FullConfigRequest;
  EFI_STRING                   ConfigHdr;
  EFI_STRING                   GuidStr;
  EFI_STRING                   NameStr;
  EFI_STRING                   PathStr;
  EFI_STRING                   StringPtr;
  EFI_STRING                   Progress;
  UINTN                        Length;
  UINT8                        *BufferPtr;
  UINT8                        *TmpBuffer;
  UINTN                        TmpBufferSize;
  UINT16                       Offset;
  UINT16                       Width;
  LIST_ENTRY                   *Link;
  LIST_ENTRY                   *LinkData;
  LIST_ENTRY                   *LinkDefault;
  BOOLEAN                      DataExist;

  if (DataBaseRecord == NULL || DevicePath == NULL || Request == NULL || AltCfgResp == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Initialize the local variables.
  //
  TmpBuffer         = NULL;
  RequestBlockArray = NULL;
  DefaultIdArray    = NULL;
  VarStorageData    = NULL;
  DefaultAltCfgResp = NULL;
  FullConfigRequest = NULL;
  ConfigHdr         = NULL;
  GuidStr           = NULL;
  NameStr           = NULL;
  PathStr           = NULL;
  HiiFormPackage    = NULL;
  PackageSize       = 0;
  DataExist         = FALSE;
  Progress          = *Request;

  Status = GetFormPackageData (DataBaseRecord, &HiiFormPackage, &PackageSize);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // 1. Get the request block array by Request String when Request string containts the block array.
  //
  StringPtr = NULL;
  if (*Request != NULL) {
    StringPtr = *Request;
    //
    // Jump <ConfigHdr>
    //
    if (StrnCmp (StringPtr, L"GUID=", StrLen (L"GUID=")) != 0) {
      Status   = EFI_INVALID_PARAMETER;
      goto Done;
    }
    StringPtr += StrLen (L"GUID=");
    while (*StringPtr != L'\0' && StrnCmp (StringPtr, L"&NAME=", StrLen (L"&NAME=")) != 0) {
      StringPtr++;
    }
    if (*StringPtr == L'\0') {
      Status = EFI_INVALID_PARAMETER;
      goto Done;
    }
    StringPtr += StrLen (L"&NAME=");
    while (*StringPtr != L'\0' && StrnCmp (StringPtr, L"&PATH=", StrLen (L"&PATH=")) != 0) {
      StringPtr++;
    }
    if (*StringPtr == L'\0') {
      Status = EFI_INVALID_PARAMETER;
      goto Done;
    }
    StringPtr += StrLen (L"&PATH=");
    while (*StringPtr != L'\0' && *StringPtr != L'&') {
      StringPtr ++;
    }
    //
    // Check the following string &OFFSET=
    //
    if (*StringPtr != L'\0' && StrnCmp (StringPtr, L"&OFFSET=", StrLen (L"&OFFSET=")) != 0) {
      Progress = StringPtr;
      Status   = EFI_INVALID_PARAMETER;
      goto Done;
    } else if (*StringPtr == L'\0') {
      //
      // No request block is found.
      //
      StringPtr = NULL;
    }
  }
  if (StringPtr != NULL) {
    //
    // Init RequestBlockArray
    //
    RequestBlockArray = (IFR_BLOCK_DATA *) AllocateZeroPool (sizeof (IFR_BLOCK_DATA));
    if (RequestBlockArray == NULL) {
      Status = EFI_OUT_OF_RESOURCES;
      goto Done;
    }
    InitializeListHead (&RequestBlockArray->Entry);

    //
    // Get the request Block array from the request string
    // Offset and Width
    //

    //
    // Parse each <RequestElement> if exists
    // Only <BlockName> format is supported by this help function.
    // <BlockName> ::= &'OFFSET='<Number>&'WIDTH='<Number>
    //
    TmpBufferSize = 10;
    TmpBuffer     = AllocateZeroPool (TmpBufferSize);
    while (*StringPtr != 0 && StrnCmp (StringPtr, L"&OFFSET=", StrLen (L"&OFFSET=")) == 0) {
      //
      // Skip the OFFSET string
      //
      Progress   = StringPtr;
      StringPtr += StrLen (L"&OFFSET=");
      //
      // Get Offset
      //
      Status = GetValueOfNumber (StringPtr, &TmpBuffer, &TmpBufferSize, &Length);
      if (EFI_ERROR (Status)) {
        goto Done;
      }
      Offset = 0;
      CopyMem (
        &Offset,
        TmpBuffer,
        (((Length + 1) / 2) < sizeof (UINT16)) ? ((Length + 1) / 2) : sizeof (UINT16)
        );

      StringPtr += Length;
      if (StrnCmp (StringPtr, L"&WIDTH=", StrLen (L"&WIDTH=")) != 0) {
        Status = EFI_INVALID_PARAMETER;
        goto Done;
      }
      StringPtr += StrLen (L"&WIDTH=");

      //
      // Get Width
      //
      Status = GetValueOfNumber (StringPtr, &TmpBuffer, &TmpBufferSize, &Length);
      if (EFI_ERROR (Status)) {
        goto Done;
      }
      Width = 0;
      CopyMem (
        &Width,
        TmpBuffer,
        (((Length + 1) / 2) < sizeof (UINT16)) ? ((Length + 1) / 2) : sizeof (UINT16)
        );

      StringPtr += Length;
      if (*StringPtr != 0 && *StringPtr != L'&') {
        Status = EFI_INVALID_PARAMETER;
        goto Done;
      }

      //
      // Set Block Data
      //
      BlockData = (IFR_BLOCK_DATA *) AllocateZeroPool (sizeof (IFR_BLOCK_DATA));
      if (BlockData == NULL) {
        Status = EFI_OUT_OF_RESOURCES;
        goto Done;
      }
      BlockData->Offset = Offset;
      BlockData->Width  = Width;
      InsertBlockData (&RequestBlockArray->Entry, &BlockData);

      //
      // Skip &VALUE string if &VALUE does exists.
      //
      if (StrnCmp (StringPtr, L"&VALUE=", StrLen (L"&VALUE=")) == 0) {
        StringPtr += StrLen (L"&VALUE=");

        //
        // Get Value
        //
        Status = GetValueOfNumber (StringPtr, &TmpBuffer, &TmpBufferSize, &Length);
        if (EFI_ERROR (Status)) {
          Status = EFI_INVALID_PARAMETER;
          goto Done;
        }

        StringPtr += Length;
        if (*StringPtr != 0 && *StringPtr != L'&') {
          Status = EFI_INVALID_PARAMETER;
          goto Done;
        }
      }
      //
      // If '\0', parsing is finished.
      //
      if (*StringPtr == 0) {
        break;
      }
    }

    //
    // Merge the requested block data.
    //
//[-start-221013-IB14860855-remove]//
//    Link = RequestBlockArray->Entry.ForwardLink;
//    while ((Link != &RequestBlockArray->Entry) && (Link->ForwardLink != &RequestBlockArray->Entry)) {
//      BlockData = BASE_CR (Link, IFR_BLOCK_DATA, Entry);
//      NextBlockData = BASE_CR (Link->ForwardLink, IFR_BLOCK_DATA, Entry);
//      if ((NextBlockData->Offset >= BlockData->Offset) && (NextBlockData->Offset <= (BlockData->Offset + BlockData->Width))) {
//        if ((NextBlockData->Offset + NextBlockData->Width) > (BlockData->Offset + BlockData->Width)) {
//          BlockData->Width = (UINT16) (NextBlockData->Offset + NextBlockData->Width - BlockData->Offset);
//        }
//        RemoveEntryList (Link->ForwardLink);
//        FreePool (NextBlockData);
//        continue;
//      }
//      Link = Link->ForwardLink;
//    }
//[-end-221013-IB14860855-remove]//
  }

  //
  // 2. Parse FormPackage to get BlockArray and DefaultId Array for the request BlockArray.
  //

  //
  // Initialize DefaultIdArray to store the map between DeaultId and DefaultName
  //
  DefaultIdArray   = (IFR_DEFAULT_DATA *) AllocateZeroPool (sizeof (IFR_DEFAULT_DATA));
  if (DefaultIdArray == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto Done;
  }
  InitializeListHead (&DefaultIdArray->Entry);

  //
  // Initialize VarStorageData to store the var store Block and Default value information.
  //
  VarStorageData = (IFR_VARSTORAGE_DATA *) AllocateZeroPool (sizeof (IFR_VARSTORAGE_DATA));
  if (VarStorageData == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto Done;
  }
  InitializeListHead (&VarStorageData->Entry);
  InitializeListHead (&VarStorageData->BlockEntry);

  //
  // Parse the opcode in form pacakge to get the default setting.
  //
  Status = ParseIfrData (HiiFormPackage, (UINT32) PackageSize, *Request, RequestBlockArray, VarStorageData, DefaultIdArray);
  if (EFI_ERROR (Status)) {
    goto Done;
  }

  //
  // No requested varstore in IFR data and directly return
  //
  if (VarStorageData->Size == 0) {
    Status = EFI_SUCCESS;
    goto Done;
  }

  //
  // 3. Construct Request Element (Block Name) for 2.1 and 2.2 case.
  //

  //
  // Construct <ConfigHdr> : "GUID=...&NAME=...&PATH=..." by VarStorageData Guid, Name and DriverHandle
  //
  GuidStr = NULL;
  NameStr = NULL;
  PathStr = NULL;
  GenerateSubStr (L"GUID=", sizeof (EFI_GUID), (VOID *) &VarStorageData->Guid, 1, &GuidStr);
  if (GuidStr == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto Done;
  }
  GenerateSubStr (L"NAME=", StrLen (VarStorageData->Name) * sizeof (CHAR16), (VOID *) VarStorageData->Name, 2, &NameStr);
  if (NameStr == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto Done;
  }
  GenerateSubStr (
    L"PATH=",
    GetDevicePathSize ((EFI_DEVICE_PATH_PROTOCOL *) DevicePath),
    (VOID *) DevicePath,
    1,
    &PathStr
    );
  if (PathStr == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto Done;
  }
  Length = StrLen (GuidStr);
  Length = Length + StrLen (NameStr);
  Length = Length + StrLen (PathStr) + 1;
  ConfigHdr = AllocateZeroPool (Length * sizeof (CHAR16));
  if (ConfigHdr == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto Done;
  }
  StrCpyS (ConfigHdr, Length, GuidStr);
  StrCatS (ConfigHdr, Length, NameStr);
  StrCatS (ConfigHdr, Length, PathStr);

  //
  // Remove the last character L'&'
  //
  *(ConfigHdr + StrLen (ConfigHdr) - 1) = L'\0';

  if (RequestBlockArray == NULL) {
    //
    // Append VarStorageData BlockEntry into *Request string
    // Now support only one varstore in a form package.
    //

    //
    // Go through all VarStorageData Entry and get BlockEntry for each one for the multiple varstore in a single form package
    // Then construct them all to return MultiRequest string : ConfigHdr BlockConfig
    //

    //
    // Compute the length of the entire request starting with <ConfigHdr> and a
    // Null-terminator
    //
    DataExist = FALSE;
    Length    = StrLen (ConfigHdr) + 1;

    for (Link = VarStorageData->BlockEntry.ForwardLink; Link != &VarStorageData->BlockEntry; Link = Link->ForwardLink) {
      //
      // Add <BlockName> length for each Offset/Width pair
      //
      // <BlockName> ::= &OFFSET=1234&WIDTH=1234
      //                 |  8   | 4 |   7  | 4 |
      //
      DataExist = TRUE;
      Length = Length + (8 + 4 + 7 + 4);
    }

    //
    // No any request block data is found. The request string can't be constructed.
    //
    if (!DataExist) {
      Status = EFI_SUCCESS;
      goto Done;
    }

    //
    // Allocate buffer for the entire <ConfigRequest>
    //
    FullConfigRequest = AllocateZeroPool (Length * sizeof (CHAR16));
    if (FullConfigRequest == NULL) {
      Status = EFI_OUT_OF_RESOURCES;
      goto Done;
    }
    StringPtr = FullConfigRequest;

    //
    // Start with <ConfigHdr>
    //
    StrCpyS (StringPtr, Length, ConfigHdr);
    StringPtr += StrLen (StringPtr);

    //
    // Loop through all the Offset/Width pairs and append them to ConfigRequest
    //
    for (Link = VarStorageData->BlockEntry.ForwardLink; Link != &VarStorageData->BlockEntry; Link = Link->ForwardLink) {
      BlockData = BASE_CR (Link, IFR_BLOCK_DATA, Entry);
      //
      // Append &OFFSET=XXXX&WIDTH=YYYY\0
      //
      UnicodeSPrint (
        StringPtr,
        (8 + 4 + 7 + 4 + 1) * sizeof (CHAR16),
        L"&OFFSET=%04X&WIDTH=%04X",
        BlockData->Offset,
        BlockData->Width
      );
      StringPtr += StrLen (StringPtr);
    }
    //
    // Set to the got full request string.
    //
    HiiToLower (FullConfigRequest);
    if (*Request != NULL) {
      FreePool (*Request);
    }
    *Request = FullConfigRequest;
  }

  //
  // 4. Construct Default Value string in AltResp according to request element.
  // Go through all VarStorageData Entry and get the DefaultId array for each one
  // Then construct them all to : ConfigHdr AltConfigHdr ConfigBody AltConfigHdr ConfigBody
  //
  DataExist = FALSE;
  //
  // Add length for <ConfigHdr> + '\0'
  //
  Length = StrLen (ConfigHdr) + 1;

  for (Link = DefaultIdArray->Entry.ForwardLink; Link != &DefaultIdArray->Entry; Link = Link->ForwardLink) {
    DefaultId = BASE_CR (Link, IFR_DEFAULT_DATA, Entry);
    //
    // Add length for "&<ConfigHdr>&ALTCFG=XXXX"
    //                |1| StrLen (ConfigHdr) | 8 | 4 |
    //
    Length += (1 + StrLen (ConfigHdr) + 8 + 4);

    for (LinkData = VarStorageData->BlockEntry.ForwardLink; LinkData != &VarStorageData->BlockEntry; LinkData = LinkData->ForwardLink) {
      BlockData = BASE_CR (LinkData, IFR_BLOCK_DATA, Entry);
      for (LinkDefault = BlockData->DefaultValueEntry.ForwardLink; LinkDefault != &BlockData->DefaultValueEntry; LinkDefault = LinkDefault->ForwardLink) {
        DefaultValueData = BASE_CR (LinkDefault, IFR_DEFAULT_DATA, Entry);
        if (DefaultValueData->DefaultId == DefaultId->DefaultId) {
          //
          // Add length for "&OFFSET=XXXX&WIDTH=YYYY&VALUE=zzzzzzzzzzzz"
          //                |    8  | 4 |   7  | 4 |   7  | Width * 2 |
          //
          Length += (8 + 4 + 7 + 4 + 7 + BlockData->Width * 2);
          DataExist = TRUE;
        }
      }
    }
  }

  //
  // No default value is found. The default string doesn't exist.
  //
  if (!DataExist) {
    Status = EFI_SUCCESS;
    goto Done;
  }

  //
  // Allocate buffer for the entire <DefaultAltCfgResp>
  //
  DefaultAltCfgResp = AllocateZeroPool (Length * sizeof (CHAR16));
  if (DefaultAltCfgResp == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto Done;
  }
  StringPtr = DefaultAltCfgResp;

  //
  // Start with <ConfigHdr>
  //
  StrCpyS (StringPtr, Length, ConfigHdr);
  StringPtr += StrLen (StringPtr);

  for (Link = DefaultIdArray->Entry.ForwardLink; Link != &DefaultIdArray->Entry; Link = Link->ForwardLink) {
    DefaultId = BASE_CR (Link, IFR_DEFAULT_DATA, Entry);
    //
    // Add <AltConfigHdr> of the form "&<ConfigHdr>&ALTCFG=XXXX\0"
    //                                |1| StrLen (ConfigHdr) | 8 | 4 |
    //
    UnicodeSPrint (
      StringPtr,
      (1 + StrLen (ConfigHdr) + 8 + 4 + 1) * sizeof (CHAR16),
      L"&%s&ALTCFG=%04X",
      ConfigHdr,
      DefaultId->DefaultId
      );
    StringPtr += StrLen (StringPtr);

    for (LinkData = VarStorageData->BlockEntry.ForwardLink; LinkData != &VarStorageData->BlockEntry; LinkData = LinkData->ForwardLink) {
      BlockData = BASE_CR (LinkData, IFR_BLOCK_DATA, Entry);
      for (LinkDefault = BlockData->DefaultValueEntry.ForwardLink; LinkDefault != &BlockData->DefaultValueEntry; LinkDefault = LinkDefault->ForwardLink) {
        DefaultValueData = BASE_CR (LinkDefault, IFR_DEFAULT_DATA, Entry);
        if (DefaultValueData->DefaultId == DefaultId->DefaultId) {
          //
          // Add <BlockConfig>
          // <BlockConfig> ::= 'OFFSET='<Number>&'WIDTH='<Number>&'VALUE'=<Number>
          //
          UnicodeSPrint (
            StringPtr,
            (8 + 4 + 7 + 4 + 7 + 1) * sizeof (CHAR16),
            L"&OFFSET=%04X&WIDTH=%04X&VALUE=",
            BlockData->Offset,
            BlockData->Width
            );
          StringPtr += StrLen (StringPtr);

          //
          // Convert Value to a hex string in "%x" format
          // NOTE: This is in the opposite byte that GUID and PATH use
          //
          Width     = BlockData->Width;
          BufferPtr = (UINT8 *) &(DefaultValueData->Value);
//[-start-190830-IB06070047-modify]//
          for (; Width > 0; Width--) {
            UnicodeValueToStringS (
              StringPtr,
              (Length - (StringPtr - DefaultAltCfgResp)) * sizeof (CHAR16),
              PREFIX_ZERO | RADIX_HEX,
              BufferPtr[Width - 1],
              2
              );
            StringPtr += StrnLenS (StringPtr, (Length - (StringPtr - DefaultAltCfgResp)));
          }
//[-end-190830-IB06070047-modify]//
        }
      }
    }
  }
  HiiToLower (DefaultAltCfgResp);

  //
  // 5. Merge string into the input AltCfgResp if the iput *AltCfgResp is not NULL.
  //
  if (*AltCfgResp != NULL && DefaultAltCfgResp != NULL) {
    Status = MergeDefaultString (AltCfgResp, DefaultAltCfgResp);
    FreePool (DefaultAltCfgResp);
  } else if (*AltCfgResp == NULL) {
    *AltCfgResp = DefaultAltCfgResp;
  }

Done:
  if (RequestBlockArray != NULL) {
    //
    // Free Link Array RequestBlockArray
    //
    while (!IsListEmpty (&RequestBlockArray->Entry)) {
      BlockData = BASE_CR (RequestBlockArray->Entry.ForwardLink, IFR_BLOCK_DATA, Entry);
      RemoveEntryList (&BlockData->Entry);
      FreePool (BlockData);
    }

    FreePool (RequestBlockArray);
  }

  if (VarStorageData != NULL) {
    //
    // Free link array VarStorageData
    //
    while (!IsListEmpty (&VarStorageData->BlockEntry)) {
      BlockData = BASE_CR (VarStorageData->BlockEntry.ForwardLink, IFR_BLOCK_DATA, Entry);
      RemoveEntryList (&BlockData->Entry);
      //
      // Free default value link array
      //
      while (!IsListEmpty (&BlockData->DefaultValueEntry)) {
        DefaultValueData = BASE_CR (BlockData->DefaultValueEntry.ForwardLink, IFR_DEFAULT_DATA, Entry);
        RemoveEntryList (&DefaultValueData->Entry);
        FreePool (DefaultValueData);
      }
      FreePool (BlockData);
    }
    if (VarStorageData->Name != NULL) {
      FreePool (VarStorageData->Name);
    }
    FreePool (VarStorageData);
  }

  if (DefaultIdArray != NULL) {
    //
    // Free DefaultId Array
    //
    while (!IsListEmpty (&DefaultIdArray->Entry)) {
      DefaultId = BASE_CR (DefaultIdArray->Entry.ForwardLink, IFR_DEFAULT_DATA, Entry);
      RemoveEntryList (&DefaultId->Entry);
      FreePool (DefaultId);
    }
    FreePool (DefaultIdArray);
  }

  //
  // Free the allocated string
  //
  if (GuidStr != NULL) {
    FreePool (GuidStr);
  }
  if (NameStr != NULL) {
    FreePool (NameStr);
  }
  if (PathStr != NULL) {
    FreePool (PathStr);
  }
  if (ConfigHdr != NULL) {
    FreePool (ConfigHdr);
  }

  //
  // Free Pacakge data
  //
  if (HiiFormPackage != NULL) {
    FreePool (HiiFormPackage);
  }

  if (PointerProgress != NULL) {
    if (*Request == NULL) {
      *PointerProgress = NULL;
    } else if (EFI_ERROR (Status)) {
      *PointerProgress = Progress;
    } else {
      *PointerProgress = *Request + StrLen (*Request);
    }
  }

  if (TmpBuffer != NULL) {
    FreePool (TmpBuffer);
  }

  return Status;
}

/**
 This function gets the full request resp string by
 parsing IFR data in HII form packages.

 @param[in]  This               A pointer to the EFI_HII_CONFIG_ROUTING_PROTOCOL
                                instance.
 @param[in]  EfiVarStoreInfo    The efi varstore info which is save in the EFI
                                varstore data structure.
 @param[in]  Request            Pointer to a null-terminated Unicode string in
                                <ConfigRequest> format.
 @param[out] RequestResp        Pointer to a null-terminated Unicode string in
                                <ConfigResp> format.
 @param[out] AccessProgress     On return, points to a character in the Request
                                string. Points to the string's null terminator if
                                request was successful. Points to the most recent
                                & before the first failing name / value pair (or
                                the beginning of the string if the failure is in
                                the first name / value pair) if the request was
                                not successful.

 @retval EFI_SUCCESS            The Results string is set to the full request string.
                                And AltCfgResp contains all default value string.
 @retval EFI_OUT_OF_RESOURCES   Not enough memory for the return string.
 @retval EFI_INVALID_PARAMETER  Request points to NULL.
**/
EFI_STATUS
GetConfigRespFromEfiVarStore (
  IN  CONST EFI_HII_CONFIG_ROUTING_PROTOCOL  *This,
  IN  EFI_IFR_VARSTORE_EFI                   *EfiVarStoreInfo,
  IN  EFI_STRING                             Request,
  OUT EFI_STRING                             *RequestResp,
  OUT EFI_STRING                             *AccessProgress
  )
{
  EFI_STATUS Status;
  EFI_STRING VarStoreName;
  UINTN      NameSize;
  UINT8      *VarStore;
  UINTN      BufferSize;

  Status       = EFI_SUCCESS;
  BufferSize   = 0;
  VarStore     = NULL;
  VarStoreName = NULL;

  NameSize = AsciiStrSize ((CHAR8 *)EfiVarStoreInfo->Name);
  VarStoreName = AllocateZeroPool (NameSize * sizeof (CHAR16));
  if (VarStoreName == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto Done;
  }
  AsciiStrToUnicodeStrS ((CHAR8 *) EfiVarStoreInfo->Name, VarStoreName, NameSize);

  Status = CommonGetVariableDataAndSize (VarStoreName, &EfiVarStoreInfo->Guid, &BufferSize, (VOID **) &VarStore);
  if (EFI_ERROR (Status)) {
    goto Done;
  }

  Status = HiiBlockToConfig(This, Request, VarStore, BufferSize, RequestResp, AccessProgress);
  if (EFI_ERROR (Status)) {
    goto Done;
  }

Done:
  if (VarStoreName != NULL) {
    FreePool (VarStoreName);
  }

  if (VarStore != NULL) {
    FreePool (VarStore);
  }

  return Status;
}


/**
 This function route the full request resp string for efi varstore.

 @param[in]  This               A pointer to the EFI_HII_CONFIG_ROUTING_PROTOCOL
                                instance.
 @param[in]  EfiVarStoreInfo    The efi varstore info which is save in the EFI
                                varstore data structure.
 @param[in]  RequestResp        Pointer to a null-terminated Unicode string in
                                <ConfigResp> format.
 @param[out] Result             Pointer to a null-terminated Unicode string in
                                <ConfigResp> format.

 @retval EFI_SUCCESS            The Results string is set to the full request string.
                                And AltCfgResp contains all default value string.
 @retval EFI_OUT_OF_RESOURCES   Not enough memory for the return string.
 @retval EFI_INVALID_PARAMETER  Request points to NULL.
**/
EFI_STATUS
RouteConfigRespForEfiVarStore (
  IN  CONST EFI_HII_CONFIG_ROUTING_PROTOCOL  *This,
  IN  EFI_IFR_VARSTORE_EFI                   *EfiVarStoreInfo,
  IN  EFI_STRING                             RequestResp,
  OUT EFI_STRING                             *Result
  )
{
  EFI_STATUS Status;
  EFI_STRING VarStoreName;
  UINTN      NameSize;
  UINT8      *VarStore;
  UINTN      BufferSize;
  UINTN      BlockSize;

  Status       = EFI_SUCCESS;
  BufferSize   = 0;
  VarStore     = NULL;
  VarStoreName = NULL;

  NameSize = AsciiStrSize ((CHAR8 *)EfiVarStoreInfo->Name);
  VarStoreName = AllocateZeroPool (NameSize * sizeof (CHAR16));
  if (VarStoreName == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto Done;
  }
  AsciiStrToUnicodeStrS ((CHAR8 *) EfiVarStoreInfo->Name, VarStoreName, NameSize);

  Status = CommonGetVariableDataAndSize (VarStoreName, &EfiVarStoreInfo->Guid, &BufferSize, (VOID **) &VarStore);
  if (EFI_ERROR (Status)) {
    goto Done;
  }

  BlockSize = BufferSize;
  Status = HiiConfigToBlock(This, RequestResp, VarStore, &BlockSize, Result);
  if (EFI_ERROR (Status)) {
    goto Done;
  }

  Status = CommonSetVariable (VarStoreName, &EfiVarStoreInfo->Guid, EfiVarStoreInfo->Attributes, BufferSize, VarStore);
  if (EFI_ERROR (Status)) {
    goto Done;
  }

Done:
  if (VarStoreName != NULL) {
    FreePool (VarStoreName);
  }

  if (VarStore != NULL) {
    FreePool (VarStore);
  }

  return Status;
}

/**
 This function allows a caller to extract the current configuration
 for one or more named elements from one or more drivers.

 @param[in]  This               A pointer to the EFI_HII_CONFIG_ROUTING_PROTOCOL
                                instance.
 @param[in]  Request            A null-terminated Unicode string in
                                <MultiConfigRequest> format.
 @param[out] Progress           On return, points to a character in the Request
                                string. Points to the string's null terminator if
                                request was successful. Points to the most recent
                                & before the first failing name / value pair (or
                                the beginning of the string if the failure is in
                                the first name / value pair) if the request was
                                not successful.
 @param[out] Results            Null-terminated Unicode string in
                                <MultiConfigAltResp> format which has all values
                                filled in for the names in the Request string.
                                String to be allocated by the called function.

 @retval EFI_SUCCESS            The Results string is filled with the values
                                corresponding to all requested names.
 @retval EFI_OUT_OF_RESOURCES   Not enough memory to store the parts of the
                                results that must be stored awaiting possible
                                future        protocols.
 @retval EFI_NOT_FOUND          Routing data doesn't match any known driver.
                                Progress set to the "G" in "GUID" of the routing
                                header that doesn't match. Note: There is no
                                requirement that all routing data be validated
                                before any configuration extraction.
 @retval EFI_INVALID_PARAMETER  For example, passing in a NULL for the Request
                                parameter would result in this type of error. The
                                Progress parameter is set to NULL.
 @retval EFI_INVALID_PARAMETER  Illegal syntax. Progress set to most recent &
                                before the error or the beginning of the string.
 @retval EFI_INVALID_PARAMETER  Unknown name. Progress points to the & before the
                                name in question.
**/
EFI_STATUS
EFIAPI
HiiConfigRoutingExtractConfig (
  IN  CONST EFI_HII_CONFIG_ROUTING_PROTOCOL  *This,
  IN  CONST EFI_STRING                       Request,
  OUT EFI_STRING                             *Progress,
  OUT EFI_STRING                             *Results
  )
{
  HII_DATABASE_PRIVATE_DATA           *Private;
  EFI_STRING                          StringPtr;
  EFI_STRING                          ConfigRequest;
  UINTN                               Length;
  EFI_DEVICE_PATH_PROTOCOL            *DevicePath;
  EFI_DEVICE_PATH_PROTOCOL            *TempDevicePath;
  EFI_STATUS                          Status;
  LIST_ENTRY                          *Link;
  HII_DATABASE_RECORD                 *Database;
  UINT8                               *DevicePathPkg;
  UINT8                               *CurrentDevicePath;
  EFI_HANDLE                          DriverHandle;
  EFI_HII_HANDLE                      HiiHandle;
  EFI_HII_CONFIG_ACCESS_PROTOCOL      *ConfigAccess;
  EFI_STRING                          AccessProgress;
  EFI_STRING                          AccessResults;
  EFI_STRING                          DefaultResults;
  BOOLEAN                             FirstElement;
  BOOLEAN                             IfrDataParsedFlag;
  BOOLEAN                             IsEfiVarStore;
  EFI_IFR_VARSTORE_EFI                *EfiVarStoreInfo;
  EFI_STRING                          ConfigString;
  UINTN                               ConfigBufferSize;
  UINTN                               UsedBufferSize;

  if (This == NULL || Progress == NULL || Results == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (Request == NULL) {
    *Progress = NULL;
    return EFI_INVALID_PARAMETER;
  }

  Private   = CONFIG_ROUTING_DATABASE_PRIVATE_DATA_FROM_THIS (This);
  StringPtr = Request;
  *Progress = StringPtr;
  DefaultResults = NULL;
  ConfigRequest  = NULL;
  Status         = EFI_SUCCESS;
  AccessResults  = NULL;
  AccessProgress = NULL;
  DevicePath     = NULL;
  IfrDataParsedFlag = FALSE;
  IsEfiVarStore     = FALSE;
  EfiVarStoreInfo   = NULL;

  //
  // The first element of <MultiConfigRequest> should be
  // <GuidHdr>, which is in 'GUID='<Guid> syntax.
  //
  if (StrnCmp (StringPtr, L"GUID=", StrLen (L"GUID=")) != 0) {
    return EFI_INVALID_PARAMETER;
  }

  FirstElement = TRUE;

  //
  // Allocate a fix length of memory to store Results. Reallocate memory for
  // Results if this fix length is insufficient.
  //
  ConfigBufferSize = MAX_CONFIG_STRING_LENGTH;
  UsedBufferSize   = 0;
  ConfigString = (EFI_STRING) AllocateZeroPool (ConfigBufferSize);
  if (ConfigString == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  while (*StringPtr != 0 && StrnCmp (StringPtr, L"GUID=", StrLen (L"GUID=")) == 0) {
    //
    // If parsing error, set Progress to the beginning of the <MultiConfigRequest>
    // or most recent & before the error.
    //
    if (StringPtr == Request) {
      *Progress = StringPtr;
    } else {
      *Progress = StringPtr - 1;
    }

    //
    // Process each <ConfigRequest> of <MultiConfigRequest>
    //
    Length = CalculateConfigStringLen (StringPtr);
    ConfigRequest = AllocateCopyPool ((Length + 1) * sizeof (CHAR16), StringPtr);
    if (ConfigRequest == NULL) {
      Status = EFI_OUT_OF_RESOURCES;
      goto Done;
    }
    *(ConfigRequest + Length) = 0;

    //
    // Get the UEFI device path
    //
    Status = GetDevicePath (ConfigRequest, (UINT8 **) &DevicePath);
    if (EFI_ERROR (Status)) {
      goto Done;
    }

    //
    // Find driver which matches the routing data.
    //
    DriverHandle     = NULL;
    HiiHandle        = NULL;
    Database         = NULL;
    for (Link = Private->DatabaseList.ForwardLink;
         Link != &Private->DatabaseList;
         Link = Link->ForwardLink
        ) {
      Database = CR (Link, HII_DATABASE_RECORD, DatabaseEntry, HII_DATABASE_RECORD_SIGNATURE);
      if ((DevicePathPkg = Database->PackageList->DevicePathPkg) != NULL) {
        CurrentDevicePath = DevicePathPkg + sizeof (EFI_HII_PACKAGE_HEADER);
        if (CompareMem (
              DevicePath,
              CurrentDevicePath,
              GetDevicePathSize ((EFI_DEVICE_PATH_PROTOCOL *) CurrentDevicePath)
              ) == 0) {
          DriverHandle = Database->DriverHandle;
          HiiHandle    = Database->Handle;
          break;
        }
      }
    }

    //
    // Try to find driver handle by device path.
    //
    if (DriverHandle == NULL) {
      TempDevicePath = DevicePath;
      Status = gBS->LocateDevicePath (
                      &gEfiDevicePathProtocolGuid,
                      &TempDevicePath,
                      &DriverHandle
                      );
      if (EFI_ERROR (Status) || (DriverHandle == NULL)) {
        //
        // Routing data does not match any known driver.
        // Set Progress to the 'G' in "GUID" of the routing header.
        //
        *Progress = StringPtr;
        Status = EFI_NOT_FOUND;
        goto Done;
      }
    }

    //
    // Check whether ConfigRequest contains request string OFFSET/WIDTH
    //
    IfrDataParsedFlag = FALSE;
    if ((HiiHandle != NULL) && (StrStr (ConfigRequest, L"&OFFSET=") == NULL)) {
      //
      // Get the full request string from IFR when HiiPackage is registered to HiiHandle
      //
      IfrDataParsedFlag = TRUE;
      Status = GetFullStringFromHiiFormPackages (Database, DevicePath, &ConfigRequest, &DefaultResults, &AccessProgress);
      if (EFI_ERROR (Status)) {
        //
        // AccessProgress indicates the parsing progress on <ConfigRequest>.
        // Map it to the progress on <MultiConfigRequest> then return it.
        //
        *Progress = StrStr (StringPtr, AccessProgress);
        goto Done;
      }
      //
      // Not any request block is found.
      //
      if (StrStr (ConfigRequest, L"&OFFSET=") == NULL) {
        AccessResults = AllocateCopyPool (StrSize (ConfigRequest), ConfigRequest);
        goto NextConfigString;
      }
    }

    //
    // Check whether this ConfigRequest is search from Efi varstore type storage.
    //
    Status = GetVarStoreType(Database, ConfigRequest, &IsEfiVarStore, &EfiVarStoreInfo);
    if (EFI_ERROR (Status)) {
      goto Done;
    }

    if (IsEfiVarStore) {
      //
      // Call the GetVariable function to extract settings.
      //
      Status = GetConfigRespFromEfiVarStore(This, EfiVarStoreInfo, ConfigRequest, &AccessResults, &AccessProgress);
      FreePool (EfiVarStoreInfo);
    } else {
      //
      // Call corresponding ConfigAccess protocol to extract settings
      //
      Status = gBS->HandleProtocol (
                      DriverHandle,
                      &gEfiHiiConfigAccessProtocolGuid,
                      (VOID **) &ConfigAccess
                      );
      ASSERT_EFI_ERROR (Status);

      Status = ConfigAccess->ExtractConfig (
                               ConfigAccess,
                               ConfigRequest,
                               &AccessProgress,
                               &AccessResults
                               );
    }
    if (EFI_ERROR (Status)) {
      //
      // AccessProgress indicates the parsing progress on <ConfigRequest>.
      // Map it to the progress on <MultiConfigRequest> then return it.
      //
      *Progress = StrStr (StringPtr, AccessProgress);
      goto Done;
    }

    //
    // Attach this <ConfigAltResp> to a <MultiConfigAltResp>. There is a '&'
    // which seperates the first <ConfigAltResp> and the following ones.
    //
    ASSERT (*AccessProgress == 0);

    //
    // Update AccessResults by getting default setting from IFR when HiiPackage is registered to HiiHandle
    //
    if (!IfrDataParsedFlag && HiiHandle != NULL) {
      Status = GetFullStringFromHiiFormPackages (Database, DevicePath, &ConfigRequest, &DefaultResults, NULL);
      ASSERT_EFI_ERROR (Status);
    }

    FreePool (DevicePath);
    DevicePath = NULL;

    if (DefaultResults != NULL) {
      Status = MergeDefaultString (&AccessResults, DefaultResults);
      ASSERT_EFI_ERROR (Status);
      FreePool (DefaultResults);
      DefaultResults = NULL;
    }

NextConfigString:
    if (!FirstElement) {
      ConfigString = AppendToMultiString (ConfigString, L"&", &ConfigBufferSize, &UsedBufferSize);
      ASSERT (ConfigString != NULL);
      if (ConfigString == NULL) {
        Status = EFI_OUT_OF_RESOURCES;
        goto Done;
      }
    }

    ConfigString = AppendToMultiString (ConfigString, AccessResults, &ConfigBufferSize, &UsedBufferSize);
    ASSERT (ConfigString != NULL);
    if (ConfigString == NULL) {
      Status = EFI_OUT_OF_RESOURCES;
      goto Done;
    }

    FirstElement = FALSE;

    FreePool (AccessResults);
    AccessResults = NULL;
    FreePool (ConfigRequest);
    ConfigRequest = NULL;

    //
    // Go to next <ConfigRequest> (skip '&').
    //
    StringPtr += Length;
    if (*StringPtr == 0) {
      *Progress = StringPtr;
      break;
    }

    StringPtr++;
  }

Done:
  if (EFI_ERROR (Status)) {
    FreePool (ConfigString);
    ConfigString = NULL;
  }

  if (ConfigRequest != NULL) {
    FreePool (ConfigRequest);
  }

  if (AccessResults != NULL) {
    FreePool (AccessResults);
  }

  if (DefaultResults != NULL) {
    FreePool (DefaultResults);
  }

  if (DevicePath != NULL) {
    FreePool (DevicePath);
  }

  *Results = ConfigString;

  return Status;
}

/**
 This function allows the caller to request the current configuration for the
 entirety of the current HII database and returns the data in a
 null-terminated Unicode string.

 @param[in]  This               A pointer to the EFI_HII_CONFIG_ROUTING_PROTOCOL
                                instance.
 @param[out] Results            Null-terminated Unicode string in
                                <MultiConfigAltResp> format which has all values
                                filled in for the entirety of the current HII
                                database. String to be allocated by the  called
                                function. De-allocation is up to the caller.

 @retval EFI_SUCCESS            The Results string is filled with the values
                                corresponding to all requested names.
 @retval EFI_OUT_OF_RESOURCES   Not enough memory to store the parts of the
                                results that must be stored awaiting possible
                                future        protocols.
 @retval EFI_INVALID_PARAMETER  For example, passing in a NULL for the Results
                                parameter would result in this type of error.
**/
EFI_STATUS
EFIAPI
HiiConfigRoutingExportConfig (
  IN  CONST EFI_HII_CONFIG_ROUTING_PROTOCOL  *This,
  OUT EFI_STRING                             *Results
  )
{
  EFI_STATUS                          Status;
  EFI_HII_CONFIG_ACCESS_PROTOCOL      *ConfigAccess;
  EFI_STRING                          AccessResults;
  EFI_STRING                          Progress;
  EFI_STRING                          StringPtr;
  EFI_STRING                          ConfigRequest;
  UINTN                               Index;
  EFI_HANDLE                          *ConfigAccessHandles;
  UINTN                               NumberConfigAccessHandles;
  BOOLEAN                             FirstElement;
  EFI_DEVICE_PATH_PROTOCOL            *DevicePath;
  EFI_HII_HANDLE                      HiiHandle;
  EFI_STRING                          DefaultResults;
  HII_DATABASE_PRIVATE_DATA           *Private;
  LIST_ENTRY                          *Link;
  HII_DATABASE_RECORD                 *Database;
  UINT8                               *DevicePathPkg;
  UINT8                               *CurrentDevicePath;
  BOOLEAN                             IfrDataParsedFlag;
  EFI_STRING                          ConfigString;
  UINTN                               ConfigBufferSize;
  UINTN                               UsedBufferSize;


  if (This == NULL || Results == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Private = CONFIG_ROUTING_DATABASE_PRIVATE_DATA_FROM_THIS (This);

  //
  // Allocate a fix length of memory to store Results. Reallocate memory for
  // Results if this fix length is insufficient.
  //
  ConfigBufferSize = MAX_CONFIG_STRING_LENGTH;
  UsedBufferSize   = 0;
  *Results         = NULL;
  ConfigString = (EFI_STRING) AllocateZeroPool (ConfigBufferSize);
  if (ConfigString == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }


  NumberConfigAccessHandles = 0;
  Status = gBS->LocateHandleBuffer (
             ByProtocol,
             &gEfiHiiConfigAccessProtocolGuid,
             NULL,
             &NumberConfigAccessHandles,
             &ConfigAccessHandles
             );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  FirstElement = TRUE;

  for (Index = 0; Index < NumberConfigAccessHandles; Index++) {
    Status = gBS->HandleProtocol (
                    ConfigAccessHandles[Index],
                    &gEfiHiiConfigAccessProtocolGuid,
                    (VOID **) &ConfigAccess
                    );
    if (EFI_ERROR (Status)) {
      continue;
    }

    //
    // Get DevicePath and HiiHandle for this ConfigAccess driver handle
    //
    IfrDataParsedFlag = FALSE;
    Progress         = NULL;
    HiiHandle        = NULL;
    DefaultResults   = NULL;
    Database         = NULL;
    ConfigRequest    = NULL;
    DevicePath       = DevicePathFromHandle (ConfigAccessHandles[Index]);
    if (DevicePath != NULL) {
      for (Link = Private->DatabaseList.ForwardLink;
           Link != &Private->DatabaseList;
           Link = Link->ForwardLink
          ) {
        Database = CR (Link, HII_DATABASE_RECORD, DatabaseEntry, HII_DATABASE_RECORD_SIGNATURE);
        if ((DevicePathPkg = Database->PackageList->DevicePathPkg) != NULL) {
          CurrentDevicePath = DevicePathPkg + sizeof (EFI_HII_PACKAGE_HEADER);
          if (CompareMem (
                DevicePath,
                CurrentDevicePath,
                GetDevicePathSize ((EFI_DEVICE_PATH_PROTOCOL *) CurrentDevicePath)
                ) == 0) {
            HiiHandle = Database->Handle;
            break;
          }
        }
      }
    }

    Status = ConfigAccess->ExtractConfig (
                             ConfigAccess,
                             NULL,
                             &Progress,
                             &AccessResults
                             );
    if (EFI_ERROR (Status)) {
      //
      // Update AccessResults by getting default setting from IFR when HiiPackage is registered to HiiHandle
      //
      if (HiiHandle != NULL && DevicePath != NULL) {
        IfrDataParsedFlag = TRUE;
        Status = GetFullStringFromHiiFormPackages (Database, DevicePath, &ConfigRequest, &DefaultResults, NULL);
        //
        // Get the full request string to get the Current setting again.
        //
        if (!EFI_ERROR (Status) && ConfigRequest != NULL) {
          Status = ConfigAccess->ExtractConfig (
                                   ConfigAccess,
                                   ConfigRequest,
                                   &Progress,
                                   &AccessResults
                                   );
          FreePool (ConfigRequest);
        } else {
          Status = EFI_NOT_FOUND;
        }
      }
    }

    if (!EFI_ERROR (Status)) {
      //
      // Update AccessResults by getting default setting from IFR when HiiPackage is registered to HiiHandle
      //
      if (!IfrDataParsedFlag && HiiHandle != NULL && DevicePath != NULL) {
        StringPtr = StrStr (AccessResults, L"&GUID=");
        if (StringPtr != NULL) {
          *StringPtr = 0;
        }
        if (StrStr (AccessResults, L"&OFFSET=") != NULL) {
          Status = GetFullStringFromHiiFormPackages (Database, DevicePath, &AccessResults, &DefaultResults, NULL);
          ASSERT_EFI_ERROR (Status);
        }
        if (StringPtr != NULL) {
          *StringPtr = L'&';
        }
      }
      //
      // Merge the default sting from IFR code into the got setting from driver.
      //
      if (DefaultResults != NULL) {
        Status = MergeDefaultString (&AccessResults, DefaultResults);
        ASSERT_EFI_ERROR (Status);
        FreePool (DefaultResults);
        DefaultResults = NULL;
      }

      //
      // Attach this <ConfigAltResp> to a <MultiConfigAltResp>. There is a '&'
      // which seperates the first <ConfigAltResp> and the following ones.
      //
      if (!FirstElement) {
        ConfigString = AppendToMultiString (ConfigString, L"&", &ConfigBufferSize, &UsedBufferSize);
        ASSERT (ConfigString != NULL);
        if (ConfigString == NULL) {
          return  EFI_OUT_OF_RESOURCES;
        }

      }

      ConfigString = AppendToMultiString (ConfigString, AccessResults, &ConfigBufferSize, &UsedBufferSize);
      ASSERT (ConfigString != NULL);
      if (ConfigString == NULL) {
        return  EFI_OUT_OF_RESOURCES;
      }

      FirstElement = FALSE;

      FreePool (AccessResults);
      AccessResults = NULL;
    }
  }
  FreePool (ConfigAccessHandles);
  *Results = ConfigString;

  return EFI_SUCCESS;
}

/**
 This function processes the results of processing forms and routes it to the
 appropriate handlers or storage.

 @param[in]  This               A pointer to the EFI_HII_CONFIG_ROUTING_PROTOCOL
                                instance.
 @param[in]  Configuration      A null-terminated Unicode string in
                                <MulltiConfigResp> format.
 @param[out] Progress           A pointer to a string filled in with the offset of
                                the most recent & before the first failing name /
                                value pair (or the beginning of the string if the
                                failure is in the first name / value pair) or the
                                terminating NULL if all was successful.

 @retval EFI_SUCCESS            The results have been distributed or are awaiting
                                distribution.
 @retval EFI_OUT_OF_RESOURCES   Not enough memory to store the parts of the
                                results that must be stored awaiting possible
                                future        protocols.
 @retval EFI_INVALID_PARAMETER  Passing in a NULL for the Configuration parameter
                                would result in this type of error.
 @retval EFI_NOT_FOUND          Target for the specified routing data was not
                                found.
**/
EFI_STATUS
EFIAPI
HiiConfigRoutingRouteConfig (
  IN  CONST EFI_HII_CONFIG_ROUTING_PROTOCOL  *This,
  IN  CONST EFI_STRING                       Configuration,
  OUT EFI_STRING                             *Progress
  )
{
  HII_DATABASE_PRIVATE_DATA           *Private;
  EFI_STRING                          StringPtr;
  EFI_STRING                          ConfigResp;
  UINTN                               Length;
  EFI_STATUS                          Status;
  EFI_DEVICE_PATH_PROTOCOL            *DevicePath;
  EFI_DEVICE_PATH_PROTOCOL            *TempDevicePath;
  LIST_ENTRY                          *Link;
  HII_DATABASE_RECORD                 *Database;
  UINT8                               *DevicePathPkg;
  UINT8                               *CurrentDevicePath;
  EFI_HANDLE                          DriverHandle;
  EFI_HII_CONFIG_ACCESS_PROTOCOL      *ConfigAccess;
  EFI_STRING                          AccessProgress;
  EFI_IFR_VARSTORE_EFI                *EfiVarStoreInfo;
  BOOLEAN                             IsEfiVarstore;

  if (This == NULL || Progress == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (Configuration == NULL) {
    *Progress = NULL;
    return EFI_INVALID_PARAMETER;
  }

  Private   = CONFIG_ROUTING_DATABASE_PRIVATE_DATA_FROM_THIS (This);
  StringPtr = Configuration;
  *Progress = StringPtr;
  Database       = NULL;
  AccessProgress = NULL;
  EfiVarStoreInfo= NULL;
  IsEfiVarstore  = FALSE;

  //
  // The first element of <MultiConfigResp> should be
  // <GuidHdr>, which is in 'GUID='<Guid> syntax.
  //
  if (StrnCmp (StringPtr, L"GUID=", StrLen (L"GUID=")) != 0) {
    return EFI_INVALID_PARAMETER;
  }

  while (*StringPtr != 0 && StrnCmp (StringPtr, L"GUID=", StrLen (L"GUID=")) == 0) {
    //
    // If parsing error, set Progress to the beginning of the <MultiConfigResp>
    // or most recent & before the error.
    //
    if (StringPtr == Configuration) {
      *Progress = StringPtr;
    } else {
      *Progress = StringPtr - 1;
    }

    //
    // Process each <ConfigResp> of <MultiConfigResp>
    //
    Length = CalculateConfigStringLen (StringPtr);
    ConfigResp = AllocateCopyPool ((Length + 1) * sizeof (CHAR16), StringPtr);
    if (ConfigResp == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
    //
    // Append '\0' to the end of ConfigRequest
    //
    *(ConfigResp + Length) = 0;

    //
    // Get the UEFI device path
    //
    Status = GetDevicePath (ConfigResp, (UINT8 **) &DevicePath);
    if (EFI_ERROR (Status)) {
      FreePool (ConfigResp);
      return Status;
    }

    //
    // Find driver which matches the routing data.
    //
    DriverHandle     = NULL;
    for (Link = Private->DatabaseList.ForwardLink;
         Link != &Private->DatabaseList;
         Link = Link->ForwardLink
        ) {
      Database = CR (Link, HII_DATABASE_RECORD, DatabaseEntry, HII_DATABASE_RECORD_SIGNATURE);

      if ((DevicePathPkg = Database->PackageList->DevicePathPkg) != NULL) {
        CurrentDevicePath = DevicePathPkg + sizeof (EFI_HII_PACKAGE_HEADER);
        if (CompareMem (
              DevicePath,
              CurrentDevicePath,
              GetDevicePathSize ((EFI_DEVICE_PATH_PROTOCOL *) CurrentDevicePath)
              ) == 0) {
          DriverHandle = Database->DriverHandle;
          break;
        }
      }
    }

    //
    // Try to find driver handle by device path.
    //
    if (DriverHandle == NULL) {
      TempDevicePath = DevicePath;
      Status = gBS->LocateDevicePath (
                      &gEfiDevicePathProtocolGuid,
                      &TempDevicePath,
                      &DriverHandle
                      );
      if (EFI_ERROR (Status) || (DriverHandle == NULL)) {
        //
        // Routing data does not match any known driver.
        // Set Progress to the 'G' in "GUID" of the routing header.
        //
        FreePool (DevicePath);
        *Progress = StringPtr;
        FreePool (ConfigResp);
        return EFI_NOT_FOUND;
      }
    }

    FreePool (DevicePath);

    //
    // Check whether this ConfigRequest is search from Efi varstore type storage.
    //
    Status = GetVarStoreType(Database, ConfigResp, &IsEfiVarstore, &EfiVarStoreInfo);
    if (EFI_ERROR (Status)) {
      return Status;
    }

    if (IsEfiVarstore) {
      //
      // Call the SetVariable function to route settings.
      //
      Status = RouteConfigRespForEfiVarStore(This, EfiVarStoreInfo, ConfigResp, &AccessProgress);
      FreePool (EfiVarStoreInfo);
    } else {
      //
      // Call corresponding ConfigAccess protocol to route settings
      //
      Status = gBS->HandleProtocol (
                      DriverHandle,
                      &gEfiHiiConfigAccessProtocolGuid,
                      (VOID **)  &ConfigAccess
                      );
      ASSERT_EFI_ERROR (Status);

      Status = ConfigAccess->RouteConfig (
                               ConfigAccess,
                               ConfigResp,
                               &AccessProgress
                               );
    }
    if (EFI_ERROR (Status)) {
      //
      // AccessProgress indicates the parsing progress on <ConfigResp>.
      // Map it to the progress on <MultiConfigResp> then return it.
      //
      *Progress = StrStr (StringPtr, AccessProgress);

      FreePool (ConfigResp);
      return Status;
    }

    FreePool (ConfigResp);
    ConfigResp = NULL;

    //
    // Go to next <ConfigResp> (skip '&').
    //
    StringPtr += Length;
    if (*StringPtr == 0) {
      *Progress = StringPtr;
      break;
    }

    StringPtr++;

  }

  return EFI_SUCCESS;
}

/**
 This helper function is to be called by drivers to map configuration data
 stored in byte array ("block") formats such as UEFI Variables into current
 configuration strings.

 @param[in]  This               A pointer to the EFI_HII_CONFIG_ROUTING_PROTOCOL
                                instance.
 @param[in]  ConfigRequest      A null-terminated Unicode string in
                                <ConfigRequest> format.
 @param[in]  Block              Array of bytes defining the block's configuration.
 @param[in]  BlockSize          Length in bytes of Block.
 @param[out] Config             Filled-in configuration string. String allocated
                                by  the function. Returned only if call is
                                successful. It is <ConfigResp> string format.
 @param[out]  Progress          A pointer to a string filled in with the offset of
                                the most recent & before the first failing
                                name/value pair (or the beginning of the string if
                                the failure is in the first name / value pair) or
                                the terminating NULL if all was successful.

 @retval EFI_SUCCESS            The request succeeded. Progress points to the null
                                terminator at the end of the ConfigRequest
                                string.
 @retval EFI_OUT_OF_RESOURCES   Not enough memory to allocate Config.     Progress
                                points to the first character of ConfigRequest.
 @retval EFI_INVALID_PARAMETER  Passing in a NULL for the ConfigRequest or
                                Block parameter would result in this type of
                                error. Progress points to the first character of
                                ConfigRequest.
 @retval EFI_DEVICE_ERROR       Block not large enough. Progress undefined.
 @retval EFI_INVALID_PARAMETER  Encountered non <BlockName> formatted string.
                                Block is left updated and Progress points at
                                the "&" preceding the first non-<BlockName>.
**/
EFI_STATUS
EFIAPI
HiiBlockToConfig (
  IN  CONST EFI_HII_CONFIG_ROUTING_PROTOCOL  *This,
  IN  CONST EFI_STRING                       ConfigRequest,
  IN  CONST UINT8                            *Block,
  IN  CONST UINTN                            BlockSize,
  OUT EFI_STRING                             *Config,
  OUT EFI_STRING                             *Progress
  )
{
  HII_DATABASE_PRIVATE_DATA           *Private;
  EFI_STRING                          StringPtr;
  UINTN                               Length;
  EFI_STATUS                          Status;
  EFI_STRING                          TmpPtr;
  UINT8                               *TmpBuffer;
  UINTN                               TmpBufferSize;
  UINTN                               Offset;
  UINTN                               Width;
  UINT8                               *Value;
  UINTN                               ValueSize;
  EFI_STRING                          ValueStr;
  UINTN                               ValueStrLen;
  EFI_STRING                          ConfigElement;
  UINTN                               ConfigElementLen;
  UINTN                               Index;
  UINT8                               *TemBuffer;
  CHAR16                              *TemString;
  CHAR16                              TemChar;
  EFI_STRING                          ConfigString;
  UINTN                               ConfigBufferSize;
  UINTN                               UsedBufferSize;
  UINTN                               CurrentLen;

  if (This == NULL || Progress == NULL || Config == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (Block == NULL || ConfigRequest == NULL) {
    *Progress = ConfigRequest;
    return EFI_INVALID_PARAMETER;
  }


  Private = CONFIG_ROUTING_DATABASE_PRIVATE_DATA_FROM_THIS (This);
  ASSERT (Private != NULL);

  StringPtr     = ConfigRequest;
  ValueStr      = NULL;
  Value         = NULL;
  ConfigElement = NULL;
  TmpBuffer     = NULL;

  //
  // Allocate a fix length of memory to store Results. Reallocate memory for
  // Results if this fix length is insufficient.
  //
  ConfigBufferSize = MAX_CONFIG_STRING_LENGTH;
  UsedBufferSize   = 0;
  ConfigString = (EFI_STRING) AllocateZeroPool (ConfigBufferSize);
  if (ConfigString == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // Jump <ConfigHdr>
  //
  if (StrnCmp (StringPtr, L"GUID=", STRLEN (L"GUID=")) != 0) {
    *Progress = StringPtr;
    Status = EFI_INVALID_PARAMETER;
    goto Exit;
  }
  while (*StringPtr != 0 && StrnCmp (StringPtr, L"PATH=", STRLEN (L"PATH=")) != 0) {
    StringPtr++;
  }
  if (*StringPtr == 0) {
    *Progress = StringPtr - 1;
    Status = EFI_INVALID_PARAMETER;
    goto Exit;
  }

  while (*StringPtr != L'&' && *StringPtr != 0) {
    StringPtr++;
  }
  if (*StringPtr == 0) {
    *Progress = StringPtr;

    ConfigString = AppendToMultiString(ConfigString, ConfigRequest, &ConfigBufferSize, &UsedBufferSize);
    HiiToLower (ConfigString);
    *Config = ConfigString;

    return EFI_SUCCESS;
  }
  //
  // Skip '&'
  //
  StringPtr++;

  //
  // Copy <ConfigHdr> and an additional '&' to <ConfigResp>
  //
  TemChar = *StringPtr;
  *StringPtr = '\0';
  ConfigString = AppendToMultiString(ConfigString, ConfigRequest, &ConfigBufferSize, &UsedBufferSize);
  *StringPtr = TemChar;

  //
  // Parse each <RequestElement> if exists
  // Only <BlockName> format is supported by this help function.
  // <BlockName> ::= 'OFFSET='<Number>&'WIDTH='<Number>
  //
  TmpBufferSize    = 10;
  TmpBuffer        = AllocatePool (TmpBufferSize);
  ValueSize        = 10;
  Value            = AllocatePool (ValueSize);
  ValueStrLen      = 10;
  ValueStr         = AllocatePool (ValueStrLen * sizeof (CHAR16));
  ConfigElementLen = 50;
  ConfigElement    = AllocatePool (ConfigElementLen * sizeof (CHAR16));
  if (TmpBuffer == NULL || Value == NULL || ValueStr == NULL || ConfigElement == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto Exit;
  }

  while (*StringPtr != 0 && StrnCmp (StringPtr, L"OFFSET=", STRLEN (L"OFFSET=")) == 0) {
    //
    // Back up the header of one <BlockName>
    //
    TmpPtr = StringPtr;

    StringPtr += STRLEN (L"OFFSET=");
    //
    // Get Offset
    //
    Status = GetValueOfNumber (StringPtr, &TmpBuffer, &TmpBufferSize, &Length);
    if (EFI_ERROR (Status)) {
      *Progress = ConfigRequest;
      goto Exit;
    }
    Offset = 0;
    CopyMem (
      &Offset,
      TmpBuffer,
      (((Length + 1) / 2) < sizeof (UINTN)) ? ((Length + 1) / 2) : sizeof (UINTN)
      );

    StringPtr += Length;
    if (StrnCmp (StringPtr, L"&WIDTH=", STRLEN (L"&WIDTH=")) != 0) {
      *Progress = StringPtr - Length - STRLEN (L"OFFSET=") - 1;
      Status = EFI_INVALID_PARAMETER;
      goto Exit;
    }
    StringPtr += STRLEN (L"&WIDTH=");

    //
    // Get Width
    //
    Status = GetValueOfNumber (StringPtr, &TmpBuffer, &TmpBufferSize, &Length);
    if (EFI_ERROR (Status)) {
      *Progress = ConfigRequest;
      goto Exit;
    }
    Width = 0;
    CopyMem (
      &Width,
      TmpBuffer,
      (((Length + 1) / 2) < sizeof (UINTN)) ? ((Length + 1) / 2) : sizeof (UINTN)
      );

    StringPtr += Length;
    if (*StringPtr != 0 && *StringPtr != L'&') {
      *Progress = StringPtr - Length - STRLEN (L"&WIDTH=");
      Status = EFI_INVALID_PARAMETER;
      goto Exit;
    }

    //
    // Calculate Value and convert it to hex string.
    //
    if (Offset + Width > BlockSize) {
      *Progress = StringPtr;
      Status = EFI_DEVICE_ERROR;
      goto Exit;
    }

    if (Width > ValueSize) {
      FreePool (Value);
      ValueSize = Width;
      Value = (UINT8 *) AllocateZeroPool (Width);
      if (Value == NULL) {
        *Progress = ConfigRequest;
        Status = EFI_OUT_OF_RESOURCES;
        goto Exit;
      }
    } else {
      ZeroMem (Value, ValueSize);
    }

    CopyMem (Value, (UINT8 *) Block + Offset, Width);

    Length = Width * 2 + 1;
    if (Length > ValueStrLen) {
      FreePool (ValueStr);
      ValueStrLen = Length;
      ValueStr = (EFI_STRING) AllocateZeroPool (Length  * sizeof (CHAR16));
      if (ValueStr == NULL) {
        *Progress = ConfigRequest;
        Status = EFI_OUT_OF_RESOURCES;
        goto Exit;
      }
    } else {
      ZeroMem (ValueStr, ValueStrLen * sizeof (CHAR16));
    }

    TemString = ValueStr;
    TemBuffer = Value + Width - 1;
    for (Index = 0; Index < Width; Index ++, TemBuffer --) {
      UnicodeValueToStringS (
        TemString,
        Length  * sizeof (CHAR16) - ((UINTN)TemString - (UINTN)ValueStr),
        PREFIX_ZERO | RADIX_HEX,
        *TemBuffer,
        2
        );
      TemString += StrnLenS (TemString, Length - ((UINTN)TemString - (UINTN)ValueStr) / sizeof (CHAR16));
    }


    //
    // Build a ConfigElement
    //
    Length += StringPtr - TmpPtr + 1 + STRLEN (L"VALUE=");
    if (Length > ConfigElementLen) {
      FreePool (ConfigElement);
      ConfigElementLen = Length;
      ConfigElement = (EFI_STRING) AllocateZeroPool (Length * sizeof (CHAR16));
      if (ConfigElement == NULL) {
        Status = EFI_OUT_OF_RESOURCES;
        goto Exit;
      }
    } else {
      ZeroMem (ConfigElement, ConfigElementLen * sizeof (CHAR16));
    }
    CopyMem (ConfigElement, TmpPtr, (StringPtr - TmpPtr + 1) * sizeof (CHAR16));
    if (*StringPtr == 0) {
      *(ConfigElement + (StringPtr - TmpPtr)) = L'&';
    }
    *(ConfigElement + (StringPtr - TmpPtr) + 1) = 0;

    CurrentLen = (StringPtr - TmpPtr + 1);
    CopyMem (&ConfigElement[CurrentLen], L"VALUE=", sizeof (L"VALUE="));
    CurrentLen += ((sizeof (L"VALUE=") - 1) / sizeof (CHAR16));
    CopyMem (&ConfigElement[CurrentLen], ValueStr, StrSize (ValueStr));

    ConfigString = AppendToMultiString (ConfigString, ConfigElement, &ConfigBufferSize, &UsedBufferSize);

    //
    // If '\0', parsing is finished. Otherwise skip '&' to continue
    //
    if (*StringPtr == 0) {
      break;
    }
    ConfigString = AppendToMultiString (ConfigString, L"&", &ConfigBufferSize, &UsedBufferSize);
    StringPtr++;

  }

  if (*StringPtr != 0) {
    *Progress = StringPtr - 1;
    Status = EFI_INVALID_PARAMETER;
    goto Exit;
  }

  if (ConfigString != NULL) {
    HiiToLower (ConfigString);
  }
  FreePool (ValueStr);
  FreePool (Value);
  FreePool (ConfigElement);
  FreePool (TmpBuffer);

  *Config = ConfigString;
  *Progress = StringPtr;
  return EFI_SUCCESS;

Exit:
  if (ConfigString != NULL) {
    FreePool (ConfigString);
    *Config = NULL;
  }
  if (ValueStr != NULL) {
    FreePool (ValueStr);
  }
  if (Value != NULL) {
    FreePool (Value);
  }
  if (ConfigElement != NULL) {
    FreePool (ConfigElement);
  }
  if (TmpBuffer != NULL) {
    FreePool (TmpBuffer);
  }

  return Status;

}

/**
 This helper function is to be called by drivers to map configuration strings
 to configurations stored in byte array ("block") formats such as UEFI Variables.

 @param[in]  This               A pointer to the EFI_HII_CONFIG_ROUTING_PROTOCOL
                                instance.
 @param[in]  ConfigResp         A null-terminated Unicode string in <ConfigResp>
                                format.
 @param[in, out]  Block         A possibly null array of bytes representing the
                                current  block. Only bytes referenced in the
                                ConfigResp string  in the block are modified. If
                                this parameter is null or if the *BlockSize
                                parameter is (on input) shorter than required by
                                the Configuration string, only the BlockSize
                                parameter is updated and an appropriate status
                                (see below)  is returned.
 @param[in, out]  BlockSize     The length of the Block in units of UINT8.  On
                                input, this is the size of the Block. On output,
                                if successful, contains the largest index of the
                                modified byte in the Block, or the required buffer
                                size if the Block is not large enough.
 @param[out]  Progress          On return, points to an element of the ConfigResp
                                string filled in with the offset of the most
                                recent '&' before the first failing name / value
                                pair (or  the beginning of the string if the
                                failure is in the  first name / value pair) or the
                                terminating NULL if all was successful.

 @retval EFI_SUCCESS            The request succeeded. Progress points to the null
                                terminator at the end of the ConfigResp string.
 @retval EFI_OUT_OF_RESOURCES   Not enough memory to allocate Config.     Progress
                                points to the first character of ConfigResp.
 @retval EFI_INVALID_PARAMETER  Passing in a NULL for the ConfigResp or
                                Block parameter would result in this type of
                                error. Progress points to the first character of
                                ConfigResp.
 @retval EFI_INVALID_PARAMETER  Encountered non <BlockName> formatted name /
                                value pair. Block is left updated and
                                Progress points at the '&' preceding the first
                                non-<BlockName>.
 @retval EFI_BUFFER_TOO_SMALL   Block not large enough. Progress undefined.
                                BlockSize is updated with the required buffer size.
 @retval EFI_NOT_FOUND          Target for the specified routing data was not found.
                                Progress points to the "G" in "GUID" of the errant
                                routing data.
**/
EFI_STATUS
EFIAPI
HiiConfigToBlock (
  IN     CONST EFI_HII_CONFIG_ROUTING_PROTOCOL *This,
  IN     CONST EFI_STRING                      ConfigResp,
  IN OUT UINT8                                 *Block,
  IN OUT UINTN                                 *BlockSize,
  OUT    EFI_STRING                            *Progress
  )
{
  HII_DATABASE_PRIVATE_DATA           *Private;
  EFI_STRING                          StringPtr;
  UINTN                               Length;
  EFI_STATUS                          Status;
  UINT8                               *TmpBuffer;
  UINTN                               TmpBufferSize;
  UINTN                               Offset;
  UINTN                               Width;
  UINTN                               BufferSize;
  UINTN                               MaxBlockSize;

  if (This == NULL || BlockSize == NULL || Progress == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  *Progress = ConfigResp;
  if (ConfigResp == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Private = CONFIG_ROUTING_DATABASE_PRIVATE_DATA_FROM_THIS (This);
  ASSERT (Private != NULL);

  StringPtr  = ConfigResp;
  BufferSize = *BlockSize;
  MaxBlockSize = 0;
  TmpBuffer    = NULL;

  //
  // Jump <ConfigHdr>
  //
  if (StrnCmp (StringPtr, L"GUID=", STRLEN (L"GUID=")) != 0) {
    *Progress = StringPtr;
    Status = EFI_INVALID_PARAMETER;
    goto Exit;
  }
  while (*StringPtr != 0 && StrnCmp (StringPtr, L"PATH=", STRLEN (L"PATH=")) != 0) {
    StringPtr++;
  }
  if (*StringPtr == 0) {
    *Progress = StringPtr;
    Status = EFI_INVALID_PARAMETER;
    goto Exit;
  }

  while (*StringPtr != L'&' && *StringPtr != 0) {
    StringPtr++;
  }
  if (*StringPtr == 0) {
    *Progress = StringPtr;
    Status = EFI_INVALID_PARAMETER;
    goto Exit;
  }
  //
  // Skip '&'
  //
  StringPtr++;

  //
  // Parse each <ConfigElement> if exists
  // Only <BlockConfig> format is supported by this help function.
  // <BlockConfig> ::= 'OFFSET='<Number>&'WIDTH='<Number>&'VALUE='<Number>
  //
  TmpBufferSize = 50;
  TmpBuffer     = AllocatePool (TmpBufferSize);
  if (TmpBuffer == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto Exit;
  }
  while (*StringPtr != 0 && StrnCmp (StringPtr, L"OFFSET=", STRLEN (L"OFFSET=")) == 0) {
    StringPtr += STRLEN (L"OFFSET=");
    //
    // Get Offset
    //
    Status = GetValueOfNumber (StringPtr, &TmpBuffer, &TmpBufferSize, &Length);
    if (EFI_ERROR (Status)) {
      *Progress = ConfigResp;
      goto Exit;
    }
    Offset = 0;
    CopyMem (
      &Offset,
      TmpBuffer,
      (((Length + 1) / 2) < sizeof (UINTN)) ? ((Length + 1) / 2) : sizeof (UINTN)
      );

    StringPtr += Length;
    if (StrnCmp (StringPtr, L"&WIDTH=", STRLEN (L"&WIDTH=")) != 0) {
      *Progress = StringPtr - Length - STRLEN (L"OFFSET=") - 1;
      Status = EFI_INVALID_PARAMETER;
      goto Exit;
    }
    StringPtr += STRLEN (L"&WIDTH=");

    //
    // Get Width
    //
    Status = GetValueOfNumber (StringPtr, &TmpBuffer, &TmpBufferSize, &Length);
    if (EFI_ERROR (Status)) {
      *Progress = ConfigResp;
      goto Exit;
    }
    Width = 0;
    CopyMem (
      &Width,
      TmpBuffer,
      (((Length + 1) / 2) < sizeof (UINTN)) ? ((Length + 1) / 2) : sizeof (UINTN)
      );

    StringPtr += Length;
    if (StrnCmp (StringPtr, L"&VALUE=", STRLEN (L"&VALUE=")) != 0) {
      *Progress = StringPtr - Length - STRLEN (L"&WIDTH=");
      Status = EFI_INVALID_PARAMETER;
      goto Exit;
    }
    StringPtr += STRLEN (L"&VALUE=");

    //
    // Get Value
    //
    Status = GetValueOfNumber (StringPtr, &TmpBuffer, &TmpBufferSize, &Length);
    if (EFI_ERROR (Status)) {
      *Progress = ConfigResp;
      goto Exit;
    }

    StringPtr += Length;
    if (*StringPtr != 0 && *StringPtr != L'&') {
      *Progress = StringPtr - Length - 7;
      Status = EFI_INVALID_PARAMETER;
      goto Exit;
    }

    //
    // Update the Block with configuration info
    //
    if ((Block != NULL) && (Offset + Width <= BufferSize)) {
      CopyMem (Block + Offset, TmpBuffer, Width);
    }
    if (Offset + Width > MaxBlockSize) {
      MaxBlockSize = Offset + Width;
    }

    //
    // If '\0', parsing is finished. Otherwise skip '&' to continue
    //
    if (*StringPtr == 0) {
      break;
    }

    StringPtr++;
  }

  //
  // The input string is ConfigAltResp format.
  //
  if ((*StringPtr != 0) && (StrnCmp (StringPtr, L"&GUID=", STRLEN (L"&GUID=")) != 0)) {
    *Progress = StringPtr - 1;
    Status = EFI_INVALID_PARAMETER;
    goto Exit;
  }
  FreePool (TmpBuffer);

  *Progress = StringPtr + StrLen (StringPtr);
  *BlockSize = MaxBlockSize - 1;

  if (MaxBlockSize > BufferSize) {
    *BlockSize = MaxBlockSize;
    if (Block != NULL) {
      return EFI_BUFFER_TOO_SMALL;
    }
  }

  if (Block == NULL) {
    *Progress = ConfigResp;
    return EFI_INVALID_PARAMETER;
  }

  return EFI_SUCCESS;

Exit:

  if (TmpBuffer != NULL) {
    FreePool (TmpBuffer);
  }
  return Status;
}


/**
 This helper function is to be called by drivers to extract portions of
 a larger configuration string.

 @param[in]  This               A pointer to the EFI_HII_CONFIG_ROUTING_PROTOCOL
                                instance.
 @param[in]  Configuration      A null-terminated Unicode string in
                                <MultiConfigAltResp> format.
 @param[in]  Guid               A pointer to the GUID value to search for in the
                                routing portion of the ConfigResp string when
                                retrieving  the requested data. If Guid is NULL,
                                then all GUID  values will be searched for.
 @param[in]  Name               A pointer to the NAME value to search for in the
                                routing portion of the ConfigResp string when
                                retrieving  the requested data. If Name is NULL,
                                then all Name  values will be searched for.
 @param[in]  DevicePath         A pointer to the PATH value to search for in the
                                routing portion of the ConfigResp string when
                                retrieving  the requested data. If DevicePath is
                                NULL, then all  DevicePath values will be searched
                                for.
 @param[in]  AltCfgId           A pointer to the ALTCFG value to search for in the
                                routing portion of the ConfigResp string when
                                retrieving  the requested data.  If this parameter
                                is NULL,  then the current setting will be
                                retrieved.
 @param[out] AltCfgResp         A pointer to a buffer which will be allocated by
                                the  function which contains the retrieved string
                                as requested.   This buffer is only allocated if
                                the call was successful. It is <ConfigResp> format.

 @retval EFI_SUCCESS            The request succeeded. The requested data was
                                extracted  and placed in the newly allocated
                                AltCfgResp buffer.
 @retval EFI_OUT_OF_RESOURCES   Not enough memory to allocate AltCfgResp.
 @retval EFI_INVALID_PARAMETER  Any parameter is invalid.
 @retval EFI_NOT_FOUND          Target for the specified routing data was not
                                found.
**/
EFI_STATUS
EFIAPI
HiiGetAltCfg (
  IN  CONST EFI_HII_CONFIG_ROUTING_PROTOCOL    *This,
  IN  CONST EFI_STRING                         Configuration,
  IN  CONST EFI_GUID                           *Guid,
  IN  CONST EFI_STRING                         Name,
  IN  CONST EFI_DEVICE_PATH_PROTOCOL           *DevicePath,
  IN  CONST UINT16                             *AltCfgId,
  OUT EFI_STRING                               *AltCfgResp
  )
{
  EFI_STATUS                          Status;
  EFI_STRING                          StringPtr;
  EFI_STRING                          HdrStart;
  EFI_STRING                          HdrEnd;
  EFI_STRING                          TmpPtr;
  UINTN                               Length;
  EFI_STRING                          GuidStr;
  EFI_STRING                          NameStr;
  EFI_STRING                          PathStr;
  EFI_STRING                          AltIdStr;
  EFI_STRING                          Result;
  BOOLEAN                             GuidFlag;
  BOOLEAN                             NameFlag;
  BOOLEAN                             PathFlag;

  HdrStart = NULL;
  HdrEnd   = NULL;
  GuidStr  = NULL;
  NameStr  = NULL;
  PathStr  = NULL;
  AltIdStr = NULL;
  Result   = NULL;
  GuidFlag = FALSE;
  NameFlag = FALSE;
  PathFlag = FALSE;

  if (This == NULL || Configuration == NULL || AltCfgResp == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  StringPtr = Configuration;
  if (StrnCmp (StringPtr, L"GUID=", STRLEN (L"GUID=")) != 0) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Generate the sub string for later matching.
  //
  GenerateSubStr (L"GUID=", sizeof (EFI_GUID), (VOID *) Guid, 1, &GuidStr);
  if (GuidStr == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto Exit;
  }
  GenerateSubStr (
    L"PATH=",
    GetDevicePathSize ((EFI_DEVICE_PATH_PROTOCOL *) DevicePath),
    (VOID *) DevicePath,
    1,
    &PathStr
    );
  if (PathStr == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto Exit;
  }
  if (AltCfgId != NULL) {
    GenerateSubStr (L"ALTCFG=", sizeof (UINT16), (VOID *) AltCfgId, 3, &AltIdStr);
    if (AltIdStr == NULL) {
      Status = EFI_OUT_OF_RESOURCES;
      goto Exit;
    }
  }
  if (Name != NULL) {
    GenerateSubStr (L"NAME=", StrLen (Name) * sizeof (CHAR16), (VOID *) Name, 2, &NameStr);
  } else {
    GenerateSubStr (L"NAME=", 0, NULL, 2, &NameStr);
  }
  if (NameStr == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto Exit;
  }

  while (*StringPtr != 0) {
    //
    // Try to match the GUID
    //
    if (!GuidFlag) {
      TmpPtr = StrStr (StringPtr, GuidStr);
      if (TmpPtr == NULL) {
        Status = EFI_NOT_FOUND;
        goto Exit;
      }
      HdrStart = TmpPtr;

      //
      // Jump to <NameHdr>
      //
      if (Guid != NULL) {
        StringPtr = TmpPtr + StrLen (GuidStr);
      } else {
        StringPtr = StrStr (TmpPtr, L"NAME=");
        if (StringPtr == NULL) {
          Status = EFI_NOT_FOUND;
          goto Exit;
        }
      }
      GuidFlag = TRUE;
    }

    //
    // Try to match the NAME
    //
    if (GuidFlag && !NameFlag) {
      if (StrnCmp (StringPtr, NameStr, StrLen (NameStr)) != 0) {
        GuidFlag = FALSE;
      } else {
        //
        // Jump to <PathHdr>
        //
        if (Name != NULL) {
          StringPtr += StrLen (NameStr);
        } else {
          StringPtr = StrStr (StringPtr, L"PATH=");
          if (StringPtr == NULL) {
            Status = EFI_NOT_FOUND;
            goto Exit;
          }
        }
        NameFlag = TRUE;
      }
    }

    //
    // Try to match the DevicePath
    //
    if (GuidFlag && NameFlag && !PathFlag) {
      if (StrnCmp (StringPtr, PathStr, StrLen (PathStr)) != 0) {
        GuidFlag = FALSE;
        NameFlag = FALSE;
      } else {
        //
        // Jump to '&' before <DescHdr> or <ConfigBody>
        //
        if (DevicePath != NULL) {
          StringPtr += StrLen (PathStr);
        } else {
          StringPtr = StrStr (StringPtr, L"&");
          if (StringPtr == NULL) {
            Status = EFI_NOT_FOUND;
            goto Exit;
          }
          StringPtr ++;
        }
        PathFlag = TRUE;
        HdrEnd   = StringPtr;
      }
    }

    //
    // Try to match the AltCfgId
    //
    if (GuidFlag && NameFlag && PathFlag) {
      if (AltCfgId == NULL) {
        //
        // Return Current Setting when AltCfgId is NULL.
        //
        Status = OutputConfigBody (StringPtr, &Result);
        goto Exit;
      }
      //
      // Search the <ConfigAltResp> to get the <AltResp> with AltCfgId.
      //
      if (StrnCmp (StringPtr, AltIdStr, StrLen (AltIdStr)) != 0) {
        GuidFlag = FALSE;
        NameFlag = FALSE;
        PathFlag = FALSE;
      } else {
        //
        // Skip AltIdStr and &
        //
        StringPtr = StringPtr + StrLen (AltIdStr);
        Status    = OutputConfigBody (StringPtr, &Result);
        goto Exit;
      }
    }
  }

  Status = EFI_NOT_FOUND;

Exit:
  *AltCfgResp = NULL;
  if (!EFI_ERROR (Status) && (Result != NULL)) {
    //
    // Copy the <ConfigHdr> and <ConfigBody>
    //
    Length = HdrEnd - HdrStart + StrLen (Result) + 1;
    *AltCfgResp = AllocateZeroPool (Length * sizeof (CHAR16));
    if (*AltCfgResp == NULL) {
      Status = EFI_OUT_OF_RESOURCES;
    } else {
      StrnCpyS (*AltCfgResp, Length, HdrStart, HdrEnd - HdrStart);
      StrCatS (*AltCfgResp, Length, Result);
      Status = EFI_SUCCESS;
    }
  }

  if (GuidStr != NULL) {
    FreePool (GuidStr);
  }
  if (NameStr != NULL) {
    FreePool (NameStr);
  }
  if (PathStr != NULL) {
    FreePool (PathStr);
  }
  if (AltIdStr != NULL) {
    FreePool (AltIdStr);
  }
  if (Result != NULL) {
    FreePool (Result);
  }

  return Status;

}


