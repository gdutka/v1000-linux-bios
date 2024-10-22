/** @file
  DNS application sample code

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _DNS_TEST_H_
#define _DNS_TEST_H_

#include <Uefi.h>
#include <Library/ShellLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <IndustryStandard/Http11.h>

#include <Protocol/Rest.h>

#define HELP_STRING \
        "  --device    : Select network device.\r\n" \
        "  --get       : Get file from HTTP URL\r\n" \
        "  --upload    : Post file to HTTP URL\r\n" \
        "  --file      : Indicate file path to read/write file.\r\n" \
        "  --chatroom  : Chat room in HTTP URL.\r\n" \
        "  --IPv4      : Rest do HTTP IPv4\r\n" \
        "  --IPv6      : Rest do HTTP IPv6\r\n"

#define CHAT_ROOM_BACKGROUND        0
#define CHAT_ROOM_TEXT              0
#define CHAT_ROOM_INPUT_BACKGROUND  0

#define HTML_HTML        "html"
#define HTML_HEADER      "head"
#define HTML_BODY        "body"
#define HTML_PRE         "pre"

#define HTML_TABLE       "table"
#define HTML_TABLE_TR    "tr"
#define HTML_TABLE_TD    "td"

#define HTML_TAG_START   '<'
#define HTML_TAG_END     '>'
#define HTML_SECTION_END '/'

#define CHAT_ROOM_POST_STRING  "String="
#define CHAT_ROOM_POST_NAME    "Name="

#define HTTP_CACHE_INDEX       "Etag"
#define HTTP_TRY_CACHE         "If-None-Match"

#define HTTP_DEFAULT_BOUNDARY  "------XXXXX"
#define HTTP_END_BOUNDARY      "--"
#define HTTP_CONTENT_TYPE_FILE "multipart/form-data; boundary="
#define HTTP_CONTENT_TYPE_POST "application/x-www-form-urlencoded"

#define HTTP_POST_CONTENT_DISPOSITION  "Content-Disposition: form-data; name=\"file1\"; filename=\""
#define HTTP_POST_CONTENT_TYPE         "Content Type: application/octet-stream"

#define REST_TEST_PAGE_UP      8888
#define REST_TEST_PAGE_DOWN    9999

typedef enum _HTML_STACK{
  HtmlStackNull = 0,
  HtmlTable,
  HtmlTableTr,
  HtmlTableTd
} HTML_STACK;

typedef struct _REST_HTTP_CACHE {
  CHAR8                      *AsciiURL;
  CHAR16                     *URL;
  CHAR8                      *Etag;
  UINT8                      *Content;
  UINTN                      ContentLength;
  EFI_HTTP_HEADER            *Headers;
  UINTN                      HeaderCnt;
} REST_HTTP_CACHE;

typedef struct _HTML_DISPLAY_LINES {
  CHAR8                      **Lines;
  UINTN                      LinesCnt;
  UINTN                      AllocatedCnt;
} HTML_DISPLAY_LINES;

#endif

