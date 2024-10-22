/** @file
  REST test application sample code

;******************************************************************************
;* Copyright (c) 2017, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "RestTest.h"
#include <Protocol/Dns4.h>
#include <Protocol/Dns6.h>

BOOLEAN             mDebugMode = 1;

REST_HTTP_CACHE     *HttpCacheTags;
UINTN               HttpCacheTagsCnt;

HTML_DISPLAY_LINES  *mHtmlDisplayData;

UINTN               MaxRows;
UINTN               MaxColumns;
UINTN               InputTextLength;

SHELL_PARAM_ITEM    mNetworkCheckList[] = {
  {
    L"--device",    // Device select
    TypeValue,
  },
  {
    L"--get",       // Get file from http URL
    TypeValue
  },
  {
    L"--upload",    // Put file to http URL
    TypeValue
  },
  {
    L"--file",      // Dest/Source file name of get and post
    TypeValue
  },
  {
    L"--chatroom",  // chat room demo
    TypeValue
  },
  {
    L"--name",      // name in chat room
    TypeValue
  },
  {
    L"--debug",
    TypeFlag
  },
  {
    L"--IPv4",
    TypeFlag
  },
  {
    L"--IPv6",
    TypeFlag
  },
  {
    NULL,
    TypeMax
  },
};

UINT8 *HttpErrorString [] = {
  "HTTP_STATUS_UNSUPPORTED_STATUS", 
  "HTTP_STATUS_100_CONTINUE",
  "HTTP_STATUS_101_SWITCHING_PROTOCOLS",
  "HTTP_STATUS_200_OK",
  "HTTP_STATUS_201_CREATED",
  "HTTP_STATUS_202_ACCEPTED",
  "HTTP_STATUS_203_NON_AUTHORITATIVE_INFORMATION",
  "HTTP_STATUS_204_NO_CONTENT",
  "HTTP_STATUS_205_RESET_CONTENT",
  "HTTP_STATUS_206_PARTIAL_CONTENT",
  "HTTP_STATUS_300_MULTIPLE_CHIOCES",
  "HTTP_STATUS_301_MOVED_PERMANENTLY",
  "HTTP_STATUS_302_FOUND",
  "HTTP_STATUS_303_SEE_OTHER",
  "HTTP_STATUS_304_NOT_MODIFIED",
  "HTTP_STATUS_305_USE_PROXY",
  "HTTP_STATUS_307_TEMPORARY_REDIRECT",
  "HTTP_STATUS_400_BAD_REQUEST",
  "HTTP_STATUS_401_UNAUTHORIZED",
  "HTTP_STATUS_402_PAYMENT_REQUIRED",
  "HTTP_STATUS_403_FORBIDDEN",
  "HTTP_STATUS_404_NOT_FOUND",
  "HTTP_STATUS_405_METHOD_NOT_ALLOWED",
  "HTTP_STATUS_406_NOT_ACCEPTABLE",
  "HTTP_STATUS_407_PROXY_AUTHENTICATION_REQUIRED",
  "HTTP_STATUS_408_REQUEST_TIME_OUT",
  "HTTP_STATUS_409_CONFLICT",
  "HTTP_STATUS_410_GONE",
  "HTTP_STATUS_411_LENGTH_REQUIRED",
  "HTTP_STATUS_412_PRECONDITION_FAILED",
  "HTTP_STATUS_413_REQUEST_ENTITY_TOO_LARGE",
  "HTTP_STATUS_414_REQUEST_URI_TOO_LARGE",
  "HTTP_STATUS_415_UNSUPPORTED_MEDIA_TYPE",
  "HTTP_STATUS_416_REQUESTED_RANGE_NOT_SATISFIED",
  "HTTP_STATUS_417_EXPECTATION_FAILED",
  "HTTP_STATUS_500_INTERNAL_SERVER_ERROR",
  "HTTP_STATUS_501_NOT_IMPLEMENTED",
  "HTTP_STATUS_502_BAD_GATEWAY",
  "HTTP_STATUS_503_SERVICE_UNAVAILABLE",
  "HTTP_STATUS_504_GATEWAY_TIME_OUT",
  "HTTP_STATUS_505_HTTP_VERSION_NOT_SUPPORTED"
};

UINT16 *
SplitSlash (
  IN UINT16                             *FileName
  )
{
  UINTN                                 FileNameLength;
  UINTN                                 Index;
  UINT16                                *TmpFileName;

  FileNameLength = StrLen (FileName);
  TmpFileName = NULL;

  for (Index = 0; Index < FileNameLength; Index++) {
    if ((FileName[Index] == '\\') && Index + 1 < FileNameLength) {
      TmpFileName = &(FileName[Index+1]);
    }
  }
  if (TmpFileName == NULL) {
    return FileName;
  }
  return TmpFileName;
}

UINTN
EFIAPI
AsciiValueToString (
  OUT CHAR8      *Buffer,
  IN  UINTN      Flags,
  IN  INT64      Value,
  IN  UINTN      Width
  );

EFI_STATUS
RestGetFile (
  EFI_REST_PROTOCOL                    *Rest,
  UINT16                               *URL,
  UINTN                                *HeaderLen,
  EFI_HTTP_HEADER                      **Headers,
  UINTN                                *ContentLength,
  VOID                                 **Content
  );

EFI_STATUS
DisplayHttpHeaders (
  EFI_HTTP_HEADER                      *Headers,
  UINTN                                HeaderCnt
  )
{
  UINTN                                Index;

  AsciiPrint ("\nHTTP HEADERS :\n");
  for (Index = 0; Index < HeaderCnt; Index++) {
    AsciiPrint ("%a : %a\n", Headers[Index].FieldName, Headers[Index].FieldValue);
  }

  AsciiPrint ("HTTP CONTENTS :\n");
  return EFI_SUCCESS;
}

EFI_STATUS
FileFineNameFromURL (
  IN CHAR16                            *String,
  IN UINTN                             StringLength,
  OUT CHAR16                           **FileName
  )
{
  CHAR16                               *TmpString;
  UINTN                                Index;

  TmpString = String;
  for (Index = 0; Index < StringLength; Index++) {
    if (String[Index] == L'/') {
      TmpString = String + Index + 1;
    }
  }
  *FileName = TmpString;
  return EFI_SUCCESS;
}

CHAR8 *
AsciiStrnSplit (
  OUT CHAR8                            *OriginalStr,
  IN  UINTN                            StringLength,
  IN  CHAR8                            *StringEnds
  )
{
  CHAR8                                *TmpCharPtr;
  CHAR8                                *TmpEndCharPtr;
  BOOLEAN                              StringEndFound;

  StringEndFound = FALSE;
  TmpCharPtr = OriginalStr;
  while (!StringEndFound) {
    if (*TmpCharPtr == 0) {
      break;
    }
    if ((UINTN)(TmpCharPtr - OriginalStr) > (UINTN) OriginalStr) {
      break;
    }
    TmpEndCharPtr = StringEnds;
    for (;*TmpEndCharPtr != 0; TmpEndCharPtr++) {
      if (*TmpEndCharPtr == *TmpCharPtr) {
        /// String end
        StringEndFound = TRUE;
        break;
      }
    }
    if (StringEndFound) {
      break;
    }
    TmpCharPtr++;
  }
  return TmpCharPtr;
}

VOID
HtmlParserDestroy (
  IN OUT HTML_DISPLAY_LINES            *HtmlDisplayData
  )
{
  if (HtmlDisplayData != NULL) {
    if (HtmlDisplayData->Lines != 0) {
      FreePool (HtmlDisplayData->Lines);
    }
    FreePool (HtmlDisplayData);
  }
}

EFI_STATUS
HtmlParserInitialize (
  IN OUT HTML_DISPLAY_LINES            **HtmlDisplayData,
  IN     BOOLEAN                       Extend
  )
{
  EFI_STATUS                           Status;
  UINTN                                LinesCntStep;
  HTML_DISPLAY_LINES                   *TmpHtmlData;

  Status = EFI_SUCCESS;
  LinesCntStep = 100;
  if (*HtmlDisplayData == NULL) {
    TmpHtmlData = AllocateZeroPool (sizeof (HTML_DISPLAY_LINES));
    if (TmpHtmlData == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
    *HtmlDisplayData = TmpHtmlData;

  } else {
    TmpHtmlData = *HtmlDisplayData;

  }
  if (TmpHtmlData->AllocatedCnt == 0) {
    TmpHtmlData->Lines = AllocateZeroPool (sizeof (CHAR8 *) * LinesCntStep);
  } else {
    TmpHtmlData->Lines = ReallocatePool (
                           sizeof (CHAR8 *) * (TmpHtmlData->AllocatedCnt),
                           sizeof (CHAR8 *) * (TmpHtmlData->AllocatedCnt + LinesCntStep),
                           TmpHtmlData->Lines
                           );

  }
  if (TmpHtmlData->Lines == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto EXIT;
  }

  TmpHtmlData->AllocatedCnt += LinesCntStep;

EXIT:
  if (EFI_ERROR (Status)) {
    HtmlParserDestroy (TmpHtmlData);
  }
  return Status;
}

EFI_STATUS
HtmlParserAddLine (
  IN OUT HTML_DISPLAY_LINES            *HtmlDisplayData,
  IN     CHAR8                         *Line
  )
{
  HtmlDisplayData->Lines[HtmlDisplayData->LinesCnt] = Line;
  HtmlDisplayData->LinesCnt++;

  if (HtmlDisplayData->LinesCnt == HtmlDisplayData->AllocatedCnt) {
    return HtmlParserInitialize (&mHtmlDisplayData, TRUE);
  }
  return EFI_SUCCESS;
}

VOID
DisplayHtml (
  HTML_DISPLAY_LINES  *HtmlDataCache,
  UINTN               TextWindowRows,
  UINTN               StartLine
  )
{
  UINTN               Index;
  for (Index = StartLine; Index < HtmlDataCache->LinesCnt; Index++) {
    AsciiPrint ("%a\n", HtmlDataCache->Lines[Index]);
    TextWindowRows--;
    if (TextWindowRows == 0) {
      break;
    }
  }
  return;
}

VOID
DisplayHelp (
  INT32               Col,
  INT32               Row,
  UINTN               StartLine,
  UINTN               TextWindowsRows
  )
{
  gST->ConOut->SetCursorPosition (gST->ConOut, Col, Row);
  gST->ConOut->SetAttribute (gST->ConOut, EFI_TEXT_ATTR (EFI_YELLOW, EFI_BLUE));

  AsciiPrint ("[PGUP][PGDOWN] Scroll text window [ESC] Exit chat room [ENTRY] Send/Refreash message%*a", MaxColumns - gST->ConOut->Mode->CursorColumn, "");

  gST->ConOut->SetCursorPosition (gST->ConOut, MaxColumns - 12, Row);
  AsciiPrint ("%3d-%3d:%4d", StartLine, StartLine + TextWindowsRows, mHtmlDisplayData->LinesCnt);
}

/// This function for demo, only parsing one of table column.
CHAR8 *
RestParsingHtmlTable (
  IN HTML_DISPLAY_LINES                *HtmlParser,
  IN CHAR8                             *HtmlContent,
  IN UINTN                             HtmlLength
  )
{
  EFI_STATUS                           Status;
  BOOLEAN                              Parsing;
  CHAR8                                *NextString;
  CHAR8                                *ContentPtr;
  UINTN                                RemainingLength;
  BOOLEAN                              InTag;
  BOOLEAN                              IsFinishTag;
  BOOLEAN                              IsData;
  UINT8                                SectionStack[100];
  UINTN                                StackPtr;
  CHAR8                                *DataString;
  CHAR8                                *TagStartPosition;
  HTML_STACK                           TagType;

  Parsing = TRUE;
  InTag   = TRUE;
  IsData  = FALSE;
  IsFinishTag = FALSE;
  DataString = NULL;
  TagStartPosition = NULL;
  ZeroMem (SectionStack, sizeof (UINT8) * 100);
  StackPtr = 0;
  TagType = HtmlStackNull;
  ContentPtr = HtmlContent;
  RemainingLength = HtmlLength;

  Status = EFI_SUCCESS;

  while (Parsing) {
    if (AsciiStrnCmp ((ContentPtr), HTML_TABLE, AsciiStrLen (HTML_TABLE)) == 0) {
      TagType = HtmlTable;
    } else if (AsciiStrnCmp ((ContentPtr), HTML_TABLE_TR, AsciiStrLen (HTML_TABLE_TR)) == 0) {
      TagType = HtmlTableTr;
    } else if (AsciiStrnCmp ((ContentPtr), HTML_TABLE_TD, AsciiStrLen (HTML_TABLE_TD)) == 0) {
      TagType = HtmlTableTd;
    }
    NextString = AsciiStrnSplit (ContentPtr, HtmlLength, "<>/");
    HtmlLength -= (UINTN)(NextString - ContentPtr);
    ContentPtr = NextString;
    if (*NextString == '<') {
      InTag = TRUE;
      TagStartPosition = ContentPtr;
      ContentPtr++;
    } else if (*NextString == '>') {
      if (IsFinishTag) {
        if (TagType == SectionStack[StackPtr-1]) {
          StackPtr--;
          SectionStack[StackPtr] = 0;
        }
        if (TagType == HtmlTableTd) {
          *TagStartPosition = 0;
          Status = HtmlParserAddLine (HtmlParser, DataString);
        } else if (TagType == HtmlTable) {
          break;
        }
        IsData = FALSE;
        IsFinishTag = FALSE;
      } else {
        SectionStack[StackPtr] = TagType;
        StackPtr++;
        if (TagType == HtmlTableTr) {
          /// New line
        } else if (TagType == HtmlTableTd) {
          IsData = TRUE;
          DataString = ContentPtr + 1;
        }
      }
      InTag = FALSE;
      ContentPtr++;
    }
    if (InTag) {
      if (*NextString == '/') {
        IsFinishTag = TRUE;
        ContentPtr++;
      }
    }
  }
  return ContentPtr+1;
}

EFI_STATUS
ParsingHtml (
  UINT8                                *Content,
  UINTN                                ContentLength
  )
{
  EFI_STATUS                           Status;
  UINTN                                Index;
  BOOLEAN                              InHtml;
  BOOLEAN                              InHeader;
  BOOLEAN                              InBody;
  BOOLEAN                              InPre;
  BOOLEAN                              InTable;
  BOOLEAN                              InTag;
  BOOLEAN                              IsFinishTag;

  CHAR8                                **TempContentLines;
  UINTN                                TempContentCnt;
  CHAR8                                *NextString;

  Status = HtmlParserInitialize (&mHtmlDisplayData, FALSE);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  InHtml   = FALSE;
  InHeader = FALSE;
  InBody   = FALSE;
  InPre    = FALSE;
  InTag    = FALSE;
  IsFinishTag = FALSE;

  TempContentCnt = 0;
  TempContentLines = NULL;
  for (Index = 0; Index < ContentLength; Index++) {
    if (AsciiStrnCmp ((Content + Index), HTML_HTML, AsciiStrLen (HTML_HTML)) == 0) {
      if (InTag) {
        if (IsFinishTag) {
          InHtml = FALSE;
        } else {
          InHtml = TRUE;
        }
      }
      Index += AsciiStrLen (HTML_HTML);
    } else if (AsciiStrnCmp ((Content + Index), HTML_HEADER, AsciiStrLen (HTML_HEADER)) == 0) {
      if (InTag) {
        if (IsFinishTag) {
          InHeader = FALSE;
        } else {
          InHeader = TRUE;
        }
      }
      Index += AsciiStrLen (HTML_HEADER);
    } else if (AsciiStrnCmp ((Content + Index), HTML_BODY, AsciiStrLen (HTML_BODY)) == 0) {
      if (InTag) {
        if (IsFinishTag) {
          InBody = FALSE;
        } else {
          InBody = TRUE;
        }
      }
      Index += AsciiStrLen (HTML_BODY);
    } else if (AsciiStrnCmp ((Content + Index), HTML_PRE, AsciiStrLen (HTML_PRE)) == 0) {
      if (InTag) {
        if (IsFinishTag) {
          InPre = FALSE;
        } else {
          InPre = TRUE;
        }
      }
      Index += AsciiStrLen (HTML_BODY);
    } else if (AsciiStrnCmp ((Content + Index), HTML_TABLE, AsciiStrLen (HTML_TABLE)) == 0) {
      if (InTag) {
        if (IsFinishTag) {
          InTable = FALSE;
        } else {
          /// ptr will be pointer to "table ..... >"
          InTable = TRUE;
          NextString = RestParsingHtmlTable (mHtmlDisplayData, Content + Index, ContentLength - Index);
          Index = NextString - Content;
        }
      }  
    }
    switch (Content[Index]) {
    case HTML_TAG_START:
      InTag = TRUE;
      IsFinishTag = FALSE;
      break;

    case HTML_TAG_END:
      InTag = FALSE;
      IsFinishTag = FALSE;
      break;

    case HTML_SECTION_END:
      IsFinishTag = TRUE;
      break;

    case 0x0a:
    case 0x0d:
      if (InBody && InPre) {
        AsciiPrint ("\n");
      }
      break;

    default:
      if (InBody) {
        if (InPre) {
          NextString = AsciiStrnSplit (Content + Index, ContentLength - Index, "\r\n<");
          Index = NextString - (Content + Index);
        }
      }
    }
  }
  return EFI_SUCCESS;
}

EFI_STATUS
ReadString (
  IN  INT32                       InputTextColumnPosX,
  IN  INT32                       InputTextColumnPosY,
  IN  CHAR16                      *HintString,
  IN  UINTN                       StringBufLen,
  OUT CHAR8                       *StringPtr,
  OUT UINTN                       *StringLen
  )
{
  EFI_STATUS                      Status;
  EFI_INPUT_KEY                   Key;
  UINTN                           Offset;
  BOOLEAN                         IsGettingString;
  UINTN                           HintStringLen;

  HintStringLen = StrLen (HintString) + 3;
  gST->ConOut->SetCursorPosition (gST->ConOut, InputTextColumnPosX, InputTextColumnPosY);
  AsciiPrint ("%s : %*a", HintString, MaxColumns - HintStringLen - 1, "");

  Offset = 0;
  IsGettingString = TRUE;
  Status = EFI_NOT_READY;
  while(IsGettingString) {
    gST->ConOut->SetCursorPosition (gST->ConOut, InputTextColumnPosX + HintStringLen + AsciiStrLen (StringPtr), InputTextColumnPosY);
    Status = gST->ConIn->ReadKeyStroke (gST->ConIn, &Key);
    if (EFI_ERROR(Status)) {
      if (Status == EFI_NOT_READY) {
        continue;
      } else {
        break;
      }
    }
    if (Key.ScanCode == 0x00) {
      if (Key.UnicodeChar == CHAR_CARRIAGE_RETURN) {
        Status = EFI_SUCCESS;
        IsGettingString = FALSE;
        break;
      }
      switch (Key.UnicodeChar) {
      case CHAR_BACKSPACE:
        if (Offset != 0) {
          Offset -= 1;
          StringPtr[Offset] = 0;
        }
        break;

      default:
        if (Offset < (StringBufLen - 1)) {
          StringPtr[Offset] = (CHAR8) Key.UnicodeChar;
          Offset += 1;
          StringPtr[Offset] = 0;
        }
      }
    } else if (Key.ScanCode == SCAN_ESC) {
      IsGettingString = FALSE;
      Status = EFI_NOT_READY;
      continue;
    } else if (Key.ScanCode == SCAN_PAGE_UP) {
      IsGettingString = FALSE;
      Status = REST_TEST_PAGE_UP;
    } else if (Key.ScanCode == SCAN_PAGE_DOWN) {
      IsGettingString = FALSE;
      Status = REST_TEST_PAGE_DOWN;
    }
    gST->ConOut->SetCursorPosition (gST->ConOut, InputTextColumnPosX, InputTextColumnPosY);
    AsciiPrint ("%s : %a", HintString, StringPtr);
    AsciiPrint ("%*a", MaxColumns - HintStringLen - AsciiStrLen (StringPtr) - 1, "");
  }
  if (Offset != 0) {
    *StringLen = Offset;
  }
  return Status;
}



EFI_STATUS
RestEtagToIndex (
  IN  CHAR16                           *URL,
  IN  CHAR8                            *AsciiURL,
  IN OUT CHAR8                         *Etag,
  OUT UINTN                            *Index OPTIONAL
  )
{
  EFI_STATUS                           Status;
  UINTN                                InternalIndex;

  //
  // Compare Etag with cache.
  //
  Status = EFI_NOT_FOUND;
  for (InternalIndex = 0; InternalIndex < HttpCacheTagsCnt; Index++) {
    if (AsciiURL != NULL) {
      if (AsciiStrCmp (HttpCacheTags[InternalIndex].AsciiURL, AsciiURL) != 0) {
        continue;
      }
    } else if (URL != NULL) {
      if (StrCmp (HttpCacheTags[InternalIndex].URL, URL) != 0) {
        continue;
      }
    }
    if (AsciiStrCmp (HttpCacheTags[InternalIndex].Etag, Etag) == 0) {
      /// Found
      Status = EFI_SUCCESS;
      break;
    }
    /// Content update
    Status = EFI_MEDIA_CHANGED;
    break;
  }
  if (Status != EFI_NOT_FOUND) {
    if (Index != NULL) {
      *Index = InternalIndex;
    }
  }
  return Status;
}

EFI_STATUS
RestStoreCacheByEtag (
  IN  CHAR16                           *URL OPTIONAL,
  IN  CHAR8                            *AsciiURL OPTIONAL,
  IN  EFI_HTTP_HEADER                  *Headers,
  IN  UINTN                            HeaderCnt,
  IN  UINT8                            *Content,
  IN  UINTN                            ContentLength
  )
{
  EFI_STATUS                           Status;
  UINTN                                Index;
  CHAR8                                *Etag;

  //
  // Find Etag from HTTP headers.
  //
  Status = EFI_NOT_FOUND;
  Etag = NULL;
  for (Index = 0; Index < HeaderCnt; Index++) {
    if (AsciiStrCmp (Headers[Index].FieldName, HTTP_CACHE_INDEX) != 0) {
      continue;
    }
    Etag = Headers[Index].FieldValue;
  }

  Status = RestEtagToIndex (URL, AsciiURL, Etag, &Index);
  if (Status == EFI_MEDIA_CHANGED) {
    FreePool (HttpCacheTags[Index].Etag);
    FreePool (HttpCacheTags[Index].Content);
    FreePool (HttpCacheTags[Index].Headers);
    HttpCacheTags[Index].Etag          = Etag;
    HttpCacheTags[Index].Content       = Content;
    HttpCacheTags[Index].ContentLength = ContentLength;
    HttpCacheTags[Index].Headers       = Headers;
    HttpCacheTags[Index].HeaderCnt     = HeaderCnt;

  } else if (Status == EFI_NOT_FOUND) {
    HttpCacheTags = ReallocatePool (
                      sizeof (REST_HTTP_CACHE) * HttpCacheTagsCnt,
                      sizeof (REST_HTTP_CACHE) * (HttpCacheTagsCnt + 1),
                      HttpCacheTags
                      );
    if (HttpCacheTags == NULL) {
      Status = EFI_OUT_OF_RESOURCES;
      return Status;
    }
    HttpCacheTags[HttpCacheTagsCnt].AsciiURL      = AsciiURL;
    HttpCacheTags[HttpCacheTagsCnt].URL           = URL;
    HttpCacheTags[HttpCacheTagsCnt].Etag          = Etag;
    HttpCacheTags[HttpCacheTagsCnt].Content       = Content;
    HttpCacheTags[HttpCacheTagsCnt].ContentLength = ContentLength;
    HttpCacheTags[HttpCacheTagsCnt].Headers       = Headers;
    HttpCacheTags[HttpCacheTagsCnt].HeaderCnt     = HeaderCnt;
    HttpCacheTagsCnt++;
    Status = EFI_SUCCESS;

  }
  return Status;
}

/// Get HTTP header to compare Etag
EFI_STATUS
RestCheckCacheByEtag (
  EFI_REST_PROTOCOL                    *Rest,
  UINT16                               *URL,
  CHAR8                                *Etag,
  UINTN                                CacheIndex
  )
{
  EFI_STATUS                           Status;
  EFI_HTTP_MESSAGE                     HttpRequest;
  EFI_HTTP_MESSAGE                     HttpResponse;
  EFI_HTTP_REQUEST_DATA                RequestData;
  EFI_HTTP_RESPONSE_DATA               ResponseData;

  EFI_HTTP_HEADER                      RequestHeader[1];

  //
  // Prepare HTTP request data.
  //
  RequestData.Method = HttpMethodGet;
  RequestData.Url    = URL;

  RequestHeader[0].FieldName  = HTTP_TRY_CACHE;
  RequestHeader[0].FieldValue = Etag;

  HttpRequest.Data.Request = &RequestData;
  HttpRequest.HeaderCount  = 1;
  HttpRequest.Headers      = RequestHeader;
  HttpRequest.BodyLength   = 0;
  HttpRequest.Body         = 0;

  //
  // Prepare HTTP response data.
  //
  ZeroMem (&HttpResponse, sizeof (EFI_HTTP_MESSAGE));
  HttpResponse.Data.Response = &ResponseData;
  Status = Rest->SendReceive (
                   Rest,
                   &HttpRequest,
                   &HttpResponse
                   );
  if (HttpResponse.Data.Response->StatusCode == HTTP_STATUS_200_OK) {
    //
    // Update data to cache.
    //
    Status = RestStoreCacheByEtag (URL, NULL, HttpResponse.Headers, HttpResponse.HeaderCount, HttpResponse.Body, HttpResponse.BodyLength);

  } else if (HttpResponse.Data.Response->StatusCode == HTTP_STATUS_304_NOT_MODIFIED) {
    //
    // Keep using data from cache.
    //
    Status = EFI_SUCCESS;
  }
  return Status;
}

/// Using WebkitFormBoundary
EFI_STATUS
RestPostFile (
  EFI_REST_PROTOCOL                    *Rest,
  UINT16                               *URL,
  CHAR16                               *FileName,
  UINTN                                FileLength,
  UINT8                                *FileContent
  )
{
  EFI_STATUS                           Status;
  EFI_HTTP_MESSAGE                     HttpRequest;
  EFI_HTTP_MESSAGE                     HttpResponse;
  EFI_HTTP_REQUEST_DATA                RequestData;
  EFI_HTTP_RESPONSE_DATA               ResponseData;

  CHAR8                                BodyLength[100];
  CHAR8                                *AsciiFileName;

  EFI_HTTP_HEADER                      RequestHeaders[2];

  UINTN                                PostContentLen;
  UINT8                                *PostContent;
  UINT8                                *HeaderPtr;

  AsciiFileName = AllocateZeroPool (sizeof (CHAR8) * (StrLen (FileName) + 1));
  if (AsciiFileName == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  UnicodeStrToAsciiStr (FileName, AsciiFileName);

  //
  // Prepare HTTP request data.
  //
  RequestData.Method = HttpMethodPost;
  RequestData.Url    = URL;

  //
  // Parsing POST data
  //
  PostContentLen = 0;
  PostContentLen += AsciiStrLen (HTTP_END_BOUNDARY);     // start with --
  PostContentLen += AsciiStrLen (HTTP_DEFAULT_BOUNDARY); // boundary
  PostContentLen += 2; // 0a0d
  PostContentLen += AsciiStrLen (HTTP_POST_CONTENT_DISPOSITION);
  PostContentLen += AsciiStrLen (AsciiFileName);
  PostContentLen += 1; // "
  PostContentLen += 2; // 0a0d
  PostContentLen += AsciiStrLen (HTTP_POST_CONTENT_TYPE);
  PostContentLen += 2; // 0a0d
  PostContentLen += 2; // 0a0d
  /// File Start
  PostContentLen += FileLength;
  /// File End
  PostContentLen += 2; // 0a0d
  PostContentLen += AsciiStrLen (HTTP_END_BOUNDARY);
  PostContentLen += AsciiStrLen (HTTP_DEFAULT_BOUNDARY);
  PostContentLen += AsciiStrLen (HTTP_END_BOUNDARY);
  PostContentLen += 2; // 0a0d

  PostContent = AllocateZeroPool (PostContentLen);
  if (PostContent == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  HeaderPtr = PostContent;

  //
  // Fill POST data
  //
  PostContent = AsciiStrCat (PostContent, HTTP_END_BOUNDARY);   // start with --
  PostContent += AsciiStrLen (HTTP_END_BOUNDARY);
  PostContent = AsciiStrCat (PostContent, HTTP_DEFAULT_BOUNDARY);
  PostContent += AsciiStrLen (HTTP_DEFAULT_BOUNDARY);
  PostContent[0] = 0x0d;
  PostContent[1] = 0x0a;
  PostContent += 2;
  PostContent = AsciiStrCat (PostContent, HTTP_POST_CONTENT_DISPOSITION);
  PostContent += AsciiStrLen (HTTP_POST_CONTENT_DISPOSITION);
  PostContent = AsciiStrCat (PostContent, AsciiFileName);
  PostContent += AsciiStrLen (AsciiFileName);
  PostContent[0] = '\"';
  PostContent[1] = 0x0d;
  PostContent[2] = 0x0a;
  PostContent += 3;
  PostContent = AsciiStrCat (PostContent, HTTP_POST_CONTENT_TYPE);
  PostContent += AsciiStrLen (HTTP_POST_CONTENT_TYPE);
  PostContent[0] = 0x0d;
  PostContent[1] = 0x0a;
  PostContent += 2;
  PostContent[0] = 0x0d;
  PostContent[1] = 0x0a;
  PostContent += 2;
  /// File Start
  CopyMem (PostContent, FileContent, FileLength);
  PostContent += FileLength;
  /// File End
  PostContent[0] = 0x0d;
  PostContent[1] = 0x0a;
  PostContent += 2;
  PostContent = AsciiStrCat (PostContent, HTTP_END_BOUNDARY);
  PostContent += AsciiStrLen (HTTP_END_BOUNDARY);
  PostContent = AsciiStrCat (PostContent, HTTP_DEFAULT_BOUNDARY);
  PostContent += AsciiStrLen (HTTP_DEFAULT_BOUNDARY);
  PostContent = AsciiStrCat (PostContent, HTTP_END_BOUNDARY);
  PostContent += AsciiStrLen (HTTP_END_BOUNDARY);
  PostContent[0] = 0x0d;
  PostContent[1] = 0x0a;
  PostContent += 2;

  PostContent = HeaderPtr;

  AsciiValueToString (BodyLength, 0, (INT64) PostContentLen, 20);
  RequestHeaders[0].FieldName  = HTTP_HEADER_CONTENT_LENGTH;
  RequestHeaders[0].FieldValue = BodyLength;
  RequestHeaders[1].FieldName  = HTTP_HEADER_CONTENT_TYPE;
  RequestHeaders[1].FieldValue = HTTP_CONTENT_TYPE_FILE HTTP_DEFAULT_BOUNDARY;

  HttpRequest.Data.Request = &RequestData;
  HttpRequest.HeaderCount  = 2;
  HttpRequest.Headers      = RequestHeaders;
  HttpRequest.BodyLength   = PostContentLen;
  HttpRequest.Body         = PostContent;

  ZeroMem (&HttpResponse, sizeof (EFI_HTTP_MESSAGE));
  HttpResponse.Data.Response = &ResponseData;
  Status = Rest->SendReceive (
                   Rest,
                   &HttpRequest,
                   &HttpResponse
                   );
  if (HeaderPtr != NULL) {
    FreePool (HeaderPtr);
  }
  if (AsciiFileName != NULL) {
    FreePool (AsciiFileName);
  }
  return Status;
}

EFI_STATUS
RestPostString (
  EFI_REST_PROTOCOL                    *Rest,
  UINT16                               *URL,
  EFI_HTTP_HEADER                      *PostData,
  UINTN                                PostDataCnt
  )
{
  EFI_STATUS                           Status;
  EFI_HTTP_MESSAGE                     HttpRequest;
  EFI_HTTP_MESSAGE                     HttpResponse;
  EFI_HTTP_REQUEST_DATA                RequestData;
  EFI_HTTP_RESPONSE_DATA               ResponseData;

  UINTN                                Index;
  CHAR8                                **PostStrings;
  UINTN                                PostStringLen;
  CHAR8                                *PostString;
  UINTN                                TotalPostStringLen;

  CHAR8                                BodyLength[100];

  EFI_HTTP_HEADER                      RequestHeaders[2];

  //
  // Prepare HTTP request data.
  //
  RequestData.Method = HttpMethodPost;
  RequestData.Url    = URL;

  //
  // Parsing POST data
  //
  PostStringLen      = 0;
  TotalPostStringLen = 0;
  PostString         = NULL;
  PostStrings        = AllocateZeroPool (sizeof (CHAR8 *) * PostDataCnt);
  if (PostStrings == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto EXIT;
  }
  for (Index = 0; Index < PostDataCnt; Index++) {
    PostStringLen = AsciiStrLen (PostData[Index].FieldName);
    PostStringLen += AsciiStrLen (PostData[Index].FieldValue);
    PostString = AllocateZeroPool (PostStringLen + 1);
    if (TotalPostStringLen != 0) {
      // Add '&' length
      TotalPostStringLen ++;
    }
    TotalPostStringLen += PostStringLen;
    if (PostString == NULL) {
      Status = EFI_OUT_OF_RESOURCES;
      goto EXIT;
    }
    PostString = AsciiStrCat (PostString, PostData[Index].FieldName);
    PostString = AsciiStrCat (PostString, PostData[Index].FieldValue);
    PostStrings[Index] = PostString;
  }

  PostString = AllocateZeroPool (TotalPostStringLen + 1);
  if (PostString == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto EXIT;
  }
  for (Index = 0; Index < PostDataCnt; Index++) {
    if (Index != 0) {
      PostString = AsciiStrCat (PostString, "&");
    }
    PostString = AsciiStrCat (PostString, PostStrings[Index]);
  }

  AsciiValueToString (BodyLength, 0, (INT64) TotalPostStringLen, 20);
  RequestHeaders[0].FieldName  = HTTP_HEADER_CONTENT_LENGTH;
  RequestHeaders[0].FieldValue = BodyLength;
  RequestHeaders[1].FieldName  = HTTP_HEADER_CONTENT_TYPE;
  RequestHeaders[1].FieldValue = HTTP_CONTENT_TYPE_POST;

  HttpRequest.Data.Request = &RequestData;
  HttpRequest.HeaderCount  = 2;
  HttpRequest.Headers      = RequestHeaders;
  HttpRequest.BodyLength   = TotalPostStringLen;
  HttpRequest.Body         = PostString;

  ZeroMem (&HttpResponse, sizeof (EFI_HTTP_MESSAGE));
  HttpResponse.Data.Response = &ResponseData;
  Status = Rest->SendReceive (
                   Rest,
                   &HttpRequest,
                   &HttpResponse
                   );
EXIT:
  if (PostString != NULL) {
    FreePool (PostString);
  }
  if (PostStrings != NULL) {
    for (Index = 0; Index < PostDataCnt; Index++) {
      FreePool (PostStrings[Index]);
    }
  }
  return Status;
}

#define INPUT_PAGE_UP_DOWN_STEP        5

EFI_STATUS
RestChatRoom (
  EFI_REST_PROTOCOL                    *Rest,
  UINT16                               *URL,
  CHAR16                               *Name
  )
{
  EFI_STATUS                           Status;
  UINT8                                *Content;
  UINTN                                ContentLength;
  EFI_HTTP_HEADER                      *Headers;
  UINTN                                HeaderCnt;

  CHAR8                                *InputString;
  UINTN                                StringLen;
  UINTN                                TextWindowsRows;

  EFI_HTTP_HEADER                      PostData[2];

  INT32                                OriginalAttributes;
  UINTN                                StartLine;
  BOOLEAN                              IsUpdateHttp;
  CHAR8                                *AsciiName;

  //
  // Clean screen and get max columns and rows.
  //
  gST->ConOut->ClearScreen (gST->ConOut);
  OriginalAttributes = gST->ConOut->Mode->Attribute;
  Status = gST->ConOut->QueryMode (gST->ConOut, gST->ConOut->Mode->Mode, &MaxColumns, &MaxRows);
  if (EFI_ERROR (Status)) {
    MaxColumns = 25;
    MaxRows    = 80;
  }

  AsciiName = AllocateZeroPool (sizeof (CHAR8) * (StrLen (Name) + 1));
  if (AsciiName == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  UnicodeStrToAsciiStr (Name, AsciiName);
  
  PostData[0].FieldName  = CHAT_ROOM_POST_STRING;
  PostData[0].FieldValue = NULL;
  PostData[1].FieldName  = CHAT_ROOM_POST_NAME;
  PostData[1].FieldValue = AsciiName;

  if (mDebugMode) {
    MaxRows -= 1;
  }

  InputTextLength = MaxColumns - StrLen (Name) - 4;
  InputString = AllocateZeroPool (InputTextLength);
  if (InputString == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // HTTP cache initialize
  //
  HeaderCnt     = 0;
  Headers       = NULL;
  ContentLength = 0;
  Content       = NULL;
  StartLine     = 0;
  IsUpdateHttp  = TRUE;
  mHtmlDisplayData = NULL;
  ZeroMem (InputString, InputTextLength);

  while (TRUE) {
    if (IsUpdateHttp) {
      IsUpdateHttp = FALSE;
      Content = NULL;
      ContentLength = 0;
      Status = RestGetFile (Rest, URL, &HeaderCnt, &Headers, &ContentLength, &Content);
      if (EFI_ERROR (Status)) {
        if (Status != EFI_MEDIA_CHANGED) {
          return Status;
        }
      }
    }

    gST->ConOut->ClearScreen (gST->ConOut);
    gST->ConOut->SetCursorPosition (gST->ConOut, 0, 0);
    if (mDebugMode) {
      DisplayHttpHeaders (Headers, HeaderCnt);
    }
    TextWindowsRows = MaxRows - gST->ConOut->Mode->CursorRow - 2;
    if (((mHtmlDisplayData == NULL) && (Status == EFI_SUCCESS)) || (Status == EFI_MEDIA_CHANGED)){
      if (Status == EFI_MEDIA_CHANGED) {
        /// Clean cache
        HtmlParserDestroy (mHtmlDisplayData);
        mHtmlDisplayData = NULL;
      }
      Status = ParsingHtml (Content, ContentLength);
      if (mHtmlDisplayData->LinesCnt > TextWindowsRows) {
        StartLine = mHtmlDisplayData->LinesCnt - TextWindowsRows;
      }
    }
    DisplayHtml (mHtmlDisplayData, TextWindowsRows, StartLine);
    DisplayHelp (0, (INT32) MaxRows - 2, StartLine, TextWindowsRows);
  
    StringLen = 0;
    gST->ConOut->SetAttribute (gST->ConOut, EFI_TEXT_ATTR (EFI_WHITE, EFI_BLUE));
    Status = ReadString (0, (INT32) MaxRows - 1, Name, InputTextLength, InputString, &StringLen);
    gST->ConOut->SetAttribute (gST->ConOut, OriginalAttributes);

    //
    // Scroll
    //
    if (Status == REST_TEST_PAGE_DOWN) {
      if (mHtmlDisplayData->LinesCnt > (StartLine + TextWindowsRows + INPUT_PAGE_UP_DOWN_STEP)) {
        StartLine += INPUT_PAGE_UP_DOWN_STEP;
      } else if (mHtmlDisplayData->LinesCnt < TextWindowsRows) {
        StartLine = 0;
      } else {
        StartLine = mHtmlDisplayData->LinesCnt - TextWindowsRows;
      }
      continue;
    } else if (Status == REST_TEST_PAGE_UP) {
      if (StartLine < INPUT_PAGE_UP_DOWN_STEP) {
        StartLine = 0;
      } else {
        StartLine -= INPUT_PAGE_UP_DOWN_STEP;
      }
      continue;
    }
    if (EFI_ERROR (Status)) {
      break;
    }
    IsUpdateHttp = TRUE;
    if (StringLen == 0) {
      continue;
    }
    PostData[0].FieldValue = InputString;
    Status = RestPostString (Rest, URL, PostData, 2);
    ZeroMem (InputString, InputTextLength);
//    ShellPrintEx (MaxColumns, 0, L"%s", InputString);
  }
  gST->ConOut->ClearScreen (gST->ConOut);
  gST->ConOut->SetCursorPosition (gST->ConOut, 0, 0);
  return Status;
}

EFI_STATUS
RestGetFile (
  EFI_REST_PROTOCOL                    *Rest,
  UINT16                               *URL,
  UINTN                                *HeaderLen,
  EFI_HTTP_HEADER                      **Headers,
  UINTN                                *ContentLength,
  VOID                                 **Content
  )
{
  EFI_STATUS                           Status;
  EFI_HTTP_MESSAGE                     HttpRequest;
  EFI_HTTP_MESSAGE                     HttpResponse;
  EFI_HTTP_RESPONSE_DATA               ResponseData;
  EFI_HTTP_REQUEST_DATA                RequestData;

  UINTN                                Index;
  CHAR8                                *AsciiURL;
  CHAR8                                *Etag;

  AsciiURL = AllocateZeroPool (sizeof (CHAR8) * (StrLen (URL) + 1));
  if (AsciiURL == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  UnicodeStrToAsciiStr (URL, AsciiURL);

  Etag = NULL;
  for (Index = 0; Index < HttpCacheTagsCnt; Index++) {
    if (AsciiStrCmp (AsciiURL, HttpCacheTags[Index].AsciiURL) != 0) {
      continue;
    }
    Etag = HttpCacheTags[Index].Etag;
    break;
  }

  if (Etag != NULL) {
    Status = RestCheckCacheByEtag (Rest, URL, Etag, Index);
    if (!EFI_ERROR (Status) || Status == EFI_MEDIA_CHANGED) {
      *ContentLength = HttpCacheTags[Index].ContentLength;
      *Content       = HttpCacheTags[Index].Content;
      *HeaderLen     = HttpCacheTags[Index].HeaderCnt;
      *Headers       = HttpCacheTags[Index].Headers;
      /// AsciiURL has already saved in cache
      FreePool (AsciiURL);
      return Status;
    }
  }
  //
  // Prepare HTTP request data.
  //
  RequestData.Method = HttpMethodGet;
  RequestData.Url    = URL;
  
  HttpRequest.Data.Request = &RequestData;
  HttpRequest.HeaderCount  = 0;
  HttpRequest.Headers      = NULL;
  HttpRequest.BodyLength   = 0;
  HttpRequest.Body         = 0;
  ZeroMem (&HttpResponse, sizeof (EFI_HTTP_MESSAGE));
  ResponseData.StatusCode = HTTP_STATUS_400_BAD_REQUEST;
  HttpResponse.Data.Response = &ResponseData;
  Status = Rest->SendReceive (
                   Rest,
                   &HttpRequest,
                   &HttpResponse
                   );
  if (!EFI_ERROR (Status)) {
    *ContentLength = HttpResponse.BodyLength;
    *Content       = HttpResponse.Body;
    *HeaderLen     = HttpResponse.HeaderCount;
    *Headers       = HttpResponse.Headers;

    Status = RestStoreCacheByEtag (URL, AsciiURL, *Headers, *HeaderLen, *Content, *ContentLength);
  } else {
    AsciiPrint ("%a\n", HttpErrorString[HttpResponse.Data.Response->StatusCode]);
    for (Index = 0; Index < HttpResponse.HeaderCount; Index++) {
      AsciiPrint ("  %a : %a\n", HttpResponse.Headers[Index].FieldName, HttpResponse.Headers[Index].FieldValue);
    }
  }
  return Status;
}

EFI_STATUS
RestTestSetRestPolicy (
  IN BOOLEAN                           IsIpv6
  )
{
  VOID                                 *IsIPv6Ptr;

  IsIPv6Ptr = PcdGetExPtr (&gInsydeTokenSpaceGuid, PcdH2ORestIPv6DnsPolicy);
  if (IsIPv6Ptr == NULL) {
    return EFI_NOT_FOUND;
  }

  PcdSetExBool (&gInsydeTokenSpaceGuid, PcdH2ORestIPv6DnsPolicy, IsIpv6);
  return EFI_SUCCESS;
}

/**
  The application entry point

  @param[in]  ImageHandle              Common header of ImageHandle
  @param[in]  SystemTable              Common header of SystemTable

  @retval EFI_SUCCESS                  Application process success
  @retval other                        Wrong status.

**/
EFI_STATUS
EFIAPI
RestTestEntry (
  IN EFI_HANDLE                        ImageHandle,
  IN EFI_SYSTEM_TABLE                  *SystemTable
  )
{
  EFI_STATUS                           Status;
  EFI_REST_PROTOCOL                    *Rest;
  LIST_ENTRY                           *ParamPackage;
  CHAR16                               *ValueStr;
  CHAR16                               *Name;
  CHAR16                               *FileName;
  EFI_HANDLE                           *RestHandles;
  UINTN                                RestHandlesCnt;
  UINTN                                DeviceId;
  VOID                                 *TestProtocol;
  BOOLEAN                              IsIPv6;
//  UINTN                                DeviceList[16];

  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiSimpleNetworkProtocolGuid,
                  NULL,
                  &RestHandlesCnt,
                  &RestHandles
                  );
  if (EFI_ERROR (Status)) {
    Print (L"REST protocol not ready\n");
    return Status; 
  }

  Name = NULL;
  FileName = NULL;
  ShellInitialize ();
  ParamPackage = NULL;
  Status = ShellCommandLineParseEx (mNetworkCheckList, &ParamPackage, NULL, TRUE, FALSE);
  if (EFI_ERROR (Status)) {
    Status = EFI_INVALID_PARAMETER;
    goto ERROR;
  }

  HttpCacheTags    = NULL;
  HttpCacheTagsCnt = 0;
  mDebugMode = ShellCommandLineGetFlag (ParamPackage, L"--debug");
  //
  // select device by user
  //
  if (ShellCommandLineGetFlag (ParamPackage, L"--device")) {
    ValueStr = (CHAR16 *) ShellCommandLineGetValue (ParamPackage, L"--device");
    if (ValueStr == NULL) {
      Status = EFI_INVALID_PARAMETER;
      AsciiPrint (HELP_STRING);
      goto ERROR;
    }
    DeviceId = StrDecimalToUintn (ValueStr);
    if (DeviceId >= RestHandlesCnt) {
      AsciiPrint (HELP_STRING);
      Status = EFI_INVALID_PARAMETER;
      goto ERROR;
    }
    Status = gBS->HandleProtocol (
                    *(RestHandles + DeviceId),
                    &gEfiRestProtocolGuid,
                    &Rest
                    );
    if (EFI_ERROR (Status)) {
      AsciiPrint (HELP_STRING);
      goto ERROR;
    }
    
    Status = gBS->HandleProtocol (
                    *(RestHandles + DeviceId),
                    &gEfiDns4ServiceBindingProtocolGuid,
                    &TestProtocol
                    );
    if (EFI_ERROR (Status)) {
      Print (L"DNS IPv4 is not supported\n");
    }
  
    Status = gBS->HandleProtocol (
                    *(RestHandles + DeviceId),
                    &gEfiDns4ServiceBindingProtocolGuid,
                    &TestProtocol
                    );
    if (EFI_ERROR (Status)) {
      Print (L"DNS IPv6 is not supported\n");
    }

  } else {
    Status = EFI_INVALID_PARAMETER;
    AsciiPrint (HELP_STRING);
    goto ERROR;
  }

  if (Rest == NULL) {
    Status = EFI_INVALID_PARAMETER;
    AsciiPrint (HELP_STRING);
    goto ERROR;
  }

  //
  // Setting REST policy, IPv4 or IPv6.
  //
  IsIPv6 = FALSE;
  if (ShellCommandLineGetFlag (ParamPackage, L"--IPv6")) {
    IsIPv6 = TRUE;
  }
  Status = RestTestSetRestPolicy (IsIPv6);
  if (EFI_ERROR (Status)) {
    AsciiPrint ("REST policy PCD is not found.\n");
    IsIPv6 = FALSE;
  }
  AsciiPrint ("REST do HTTP %a\n", (IsIPv6) ? "IPv6" : "IPv4");

  Name = NULL;
  if (ShellCommandLineGetFlag (ParamPackage, L"--name")) {
    Name = (CHAR16 *) ShellCommandLineGetValue (ParamPackage, L"--name");
    if (Name == NULL) {
      Status = EFI_INVALID_PARAMETER;
      goto ERROR;
    }
  } else {
    Name = L"CLIENT12345";
  }

  FileName = NULL;
  if (ShellCommandLineGetFlag (ParamPackage, L"--file")) {
    FileName = (CHAR16 *) ShellCommandLineGetValue (ParamPackage, L"--file");
    if (FileName == NULL) {
      Status = EFI_INVALID_PARAMETER;
      goto ERROR;
    }
  }

  if (ShellCommandLineGetFlag (ParamPackage, L"--chatroom")) {
    ValueStr = (CHAR16 *) ShellCommandLineGetValue (ParamPackage, L"--chatroom");
    if (ValueStr == NULL) {
      Status = EFI_INVALID_PARAMETER;
      goto ERROR;
    }
    Status = RestChatRoom (Rest, ValueStr, Name);

  } else if (ShellCommandLineGetFlag (ParamPackage, L"--get")) {
    UINT8                                *Content;
    UINTN                                ContentLength;
    EFI_HTTP_HEADER                      *Headers;
    UINTN                                HeadersCnt;
    UINTN                                Index;

    SHELL_FILE_HANDLE                    FileHandle;
//    EFI_FILE_INFO                        *FileInfo;

    ValueStr = (CHAR16 *) ShellCommandLineGetValue (ParamPackage, L"--get");
    if (ValueStr == NULL) {
      Status = EFI_INVALID_PARAMETER;
      goto ERROR;
    }

    if (FileName == NULL) {
      /// Try to get last filename from URL.
      Status = FileFineNameFromURL (ValueStr, StrLen (ValueStr), &FileName);
      if (EFI_ERROR (Status)) {
        AsciiPrint ("Cannot identifier file name!!\n");
        goto ERROR;
      }
    }
    
    Status = RestGetFile (Rest, ValueStr, &HeadersCnt, &Headers, &ContentLength, &Content);
    if (EFI_ERROR (Status)) {
      AsciiPrint ("GET file failed %08x\n", Status);
      goto ERROR;
    }

    AsciiPrint ("HeadersCnt : %d\n", HeadersCnt);
    for (Index = 0; Index < HeadersCnt; Index++) {
      AsciiPrint ("  %a : %a\n", Headers[Index].FieldName, Headers[Index].FieldValue);
    }
    AsciiPrint ("ContentLength : %d\n", ContentLength);
    AsciiPrint ("Content : %08x\n", Content);

    FileHandle = NULL;
    Status = ShellOpenFileByName (FileName, &FileHandle, EFI_FILE_MODE_READ + EFI_FILE_MODE_WRITE + EFI_FILE_MODE_CREATE, 0);
    if (EFI_ERROR (Status)) {
      AsciiPrint ("File open failed %08x", Status);
      goto ERROR;
    }

    AsciiPrint ("Save to FILE NAME : %s\n", FileName);
    Status = ShellWriteFile (FileHandle, &ContentLength, (VOID *)(UINTN) Content);
    if (EFI_ERROR (Status)) {
      AsciiPrint ("File open failed %08x", Status);
    }
    ShellCloseFile (&FileHandle);
    AsciiPrint ("FILE write %d bytes\n", ContentLength);

  } else if (ShellCommandLineGetFlag (ParamPackage, L"--upload")) {
    UINT8                                *Content;
    UINTN                                ContentLength;
    SHELL_FILE_HANDLE                    FileHandle;
    EFI_FILE_INFO                        *FileInfo;

    if (FileName == NULL) {
      AsciiPrint ("Must input file name by --file [FileName]");
    }
  
    ValueStr = (CHAR16 *) ShellCommandLineGetValue (ParamPackage, L"--upload");
    if (ValueStr == NULL) {
      Status = EFI_INVALID_PARAMETER;
      goto ERROR;
    }

    FileHandle = NULL;
    Status = ShellOpenFileByName (FileName, &FileHandle, EFI_FILE_MODE_READ, 0);
    if (EFI_ERROR (Status)) {
      AsciiPrint ("File open failed %08x", Status);
    }

    Content = NULL;
    ContentLength = 0;
    FileInfo = ShellGetFileInfo (FileHandle);
    if (FileInfo == NULL) {
      Status = EFI_INVALID_PARAMETER;
      goto ERROR;
    }
    ContentLength = (UINTN) FileInfo->FileSize;
    Content = AllocateZeroPool (ContentLength);
    if (Content == NULL) {
      Status = EFI_OUT_OF_RESOURCES;
      goto ERROR;
    }
    Status = ShellReadFile (FileHandle, &ContentLength, Content);
    if (EFI_ERROR (Status)) {
      AsciiPrint ("File read failed %08x", Status);
    }

    FileName = SplitSlash (FileName);
    
    ShellCloseFile (&FileHandle);
    Status = RestPostFile (Rest, ValueStr, FileName, ContentLength, Content);

  } else {
    AsciiPrint (HELP_STRING);
  }

  if (EFI_ERROR (Status)) {
//    Print (L"REST response error : %r\n", Status);
  }

ERROR:
  if (HttpCacheTags != NULL) {
    UINTN                      Index;
    for (Index = 0; Index < HttpCacheTagsCnt; Index++) {
      if (HttpCacheTags[Index].URL != NULL) {
        FreePool (HttpCacheTags[Index].URL);
      }
      if (HttpCacheTags[Index].Etag != NULL) {
        FreePool (HttpCacheTags[Index].Etag);
      }
      if (HttpCacheTags[Index].Content != NULL) {
        FreePool (HttpCacheTags[Index].Content);
      }
    }
    FreePool (HttpCacheTags);
  }
  if (ParamPackage != NULL) {
    gBS->FreePool (ParamPackage);
  }
  return Status;
}
