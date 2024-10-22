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

#define HELP_STRING \
        "  --list     : List certificates from CA store.\r\n" \
        "  --file     : Get file from HTTP URL\r\n" \
        "  --upload   : Post file to HTTP URL\r\n" \
        "  --file     : Indicate file path to read/write file.\r\n" \
        "  --chatroom : Chat room in HTTP URL.\r\n"

UINTN
GuidToString (
  IN  EFI_GUID  *Guid,
  IN  CHAR16    *Buffer,
  IN  UINTN     BufferSize
  );
        
#endif

