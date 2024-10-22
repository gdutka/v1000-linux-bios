/** @file
  Source Code Analysis

;******************************************************************************
;* Copyright (c) 2018 - 2019, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef SOURCE_CODE_ANALYSIS_H
#define SOURCE_CODE_ANALYSIS_H

#include <Uefi.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>

#if defined (__GCC__)
#error "GCC doesn't support SCA build now."
#endif

#define SAL_PATH_STR(X) X
#define SAL_PATH(filename) <SAL_PATH_STR(SAL_INC_PATH)\\filename>

#pragma include_alias(<sal.h>, SAL_PATH(sal.h))
#pragma include_alias(<concurrencysal.h>, SAL_PATH(concurrencysal.h))

#include <sal.h>

#define MUST_INSPECT_RETURN _Must_inspect_result_
#define NEW_DEPRECATED_INTERFACE __declspec(deprecated)

#pragma warning( push )
#pragma warning (disable: 28301)

//
// BaseLib.h
//
NEW_DEPRECATED_INTERFACE
CHAR16 *
EFIAPI
StrCpy (
  OUT     CHAR16                    *Destination,
  IN      CONST CHAR16              *Source
  );

NEW_DEPRECATED_INTERFACE
CHAR16 *
EFIAPI
StrnCpy (
  OUT     CHAR16                    *Destination,
  IN      CONST CHAR16              *Source,
  IN      UINTN                     Length
  );

NEW_DEPRECATED_INTERFACE
CHAR16 *
EFIAPI
StrCat (
  IN OUT  CHAR16                    *Destination,
  IN      CONST CHAR16              *Source
  );

NEW_DEPRECATED_INTERFACE
CHAR16 *
EFIAPI
StrnCat (
  IN OUT  CHAR16                    *Destination,
  IN      CONST CHAR16              *Source,
  IN      UINTN                     Length
  );

NEW_DEPRECATED_INTERFACE
CHAR8 *
EFIAPI
UnicodeStrToAsciiStr (
  IN      CONST CHAR16              *Source,
  OUT     CHAR8                     *Destination
  );

NEW_DEPRECATED_INTERFACE
CHAR8 *
EFIAPI
AsciiStrCpy (
  OUT     CHAR8                     *Destination,
  IN      CONST CHAR8               *Source
  );

NEW_DEPRECATED_INTERFACE
CHAR8 *
EFIAPI
AsciiStrnCpy (
  OUT     CHAR8                     *Destination,
  IN      CONST CHAR8               *Source,
  IN      UINTN                     Length
  );

NEW_DEPRECATED_INTERFACE
CHAR8 *
EFIAPI
AsciiStrCat (
  IN OUT CHAR8    *Destination,
  IN CONST CHAR8  *Source
  );

NEW_DEPRECATED_INTERFACE
CHAR8 *
EFIAPI
AsciiStrnCat (
  IN OUT  CHAR8                     *Destination,
  IN      CONST CHAR8               *Source,
  IN      UINTN                     Length
  );

NEW_DEPRECATED_INTERFACE
CHAR16 *
EFIAPI
AsciiStrToUnicodeStr (
  IN      CONST CHAR8               *Source,
  OUT     CHAR16                    *Destination
  );

//
// DebugLib.h
//
#ifdef ASSERT_EFI_ERROR
#undef ASSERT_EFI_ERROR
#endif

#define ASSERT_EFI_ERROR(StatusParameter)                                              \
  do {                                                                                 \
    if (EFI_ERROR (StatusParameter)) {                                                 \
    }                                                                                  \
  } while (FALSE)


//
// PcdLib.h
//
MUST_INSPECT_RETURN
NEW_DEPRECATED_INTERFACE
UINT8
EFIAPI
LibPcdSetEx8 (
  IN CONST GUID        *Guid,
  IN UINTN             TokenNumber,
  IN UINT8             Value
  );

MUST_INSPECT_RETURN
NEW_DEPRECATED_INTERFACE
UINT16
EFIAPI
LibPcdSetEx16 (
  IN CONST GUID        *Guid,
  IN UINTN             TokenNumber,
  IN UINT16            Value
  );

MUST_INSPECT_RETURN
NEW_DEPRECATED_INTERFACE
UINT32
EFIAPI
LibPcdSetEx32 (
  IN CONST GUID        *Guid,
  IN UINTN             TokenNumber,
  IN UINT32            Value
  );

MUST_INSPECT_RETURN
NEW_DEPRECATED_INTERFACE
UINT64
EFIAPI
LibPcdSetEx64 (
  IN CONST GUID        *Guid,
  IN UINTN             TokenNumber,
  IN UINT64            Value
  );

MUST_INSPECT_RETURN
NEW_DEPRECATED_INTERFACE
VOID *
EFIAPI
LibPcdSetExPtr (
  IN      CONST GUID        *Guid,
  IN      UINTN             TokenNumber,
  IN OUT  UINTN             *SizeOfBuffer,
  IN      VOID              *Buffer
  );

MUST_INSPECT_RETURN
NEW_DEPRECATED_INTERFACE
BOOLEAN
EFIAPI
LibPcdSetExBool (
  IN CONST GUID        *Guid,
  IN UINTN             TokenNumber,
  IN BOOLEAN           Value
  );

MUST_INSPECT_RETURN
RETURN_STATUS
EFIAPI
LibPcdSet8S  (
  IN UINTN          TokenNumber,
  IN UINT8          Value
  );

MUST_INSPECT_RETURN
RETURN_STATUS
EFIAPI
LibPcdSet16S (
  IN UINTN          TokenNumber,
  IN UINT16         Value
  );

MUST_INSPECT_RETURN
RETURN_STATUS
EFIAPI
LibPcdSet32S (
  IN UINTN          TokenNumber,
  IN UINT32         Value
  );

MUST_INSPECT_RETURN
RETURN_STATUS
EFIAPI
LibPcdSet64S (
  IN UINTN          TokenNumber,
  IN UINT64         Value
  );

MUST_INSPECT_RETURN
RETURN_STATUS
EFIAPI
LibPcdSetPtrS (
  IN       UINTN    TokenNumber,
  IN OUT   UINTN    *SizeOfBuffer,
  IN CONST VOID     *Buffer
  );

MUST_INSPECT_RETURN
RETURN_STATUS
EFIAPI
LibPcdSetBoolS (
  IN UINTN          TokenNumber,
  IN BOOLEAN        Value
  );

MUST_INSPECT_RETURN
RETURN_STATUS
EFIAPI
LibPcdSetEx8S (
  IN CONST GUID     *Guid,
  IN UINTN          TokenNumber,
  IN UINT8          Value
  );

MUST_INSPECT_RETURN
RETURN_STATUS
EFIAPI
LibPcdSetEx16S (
  IN CONST GUID     *Guid,
  IN UINTN          TokenNumber,
  IN UINT16         Value
  );

MUST_INSPECT_RETURN
RETURN_STATUS
EFIAPI
LibPcdSetEx32S (
  IN CONST GUID     *Guid,
  IN UINTN          TokenNumber,
  IN UINT32         Value
  );

MUST_INSPECT_RETURN
RETURN_STATUS
EFIAPI
LibPcdSetEx64S (
  IN CONST GUID     *Guid,
  IN UINTN          TokenNumber,
  IN UINT64         Value
  );

MUST_INSPECT_RETURN
RETURN_STATUS
EFIAPI
LibPcdSetExPtrS (
  IN CONST GUID     *Guid,
  IN       UINTN    TokenNumber,
  IN OUT   UINTN    *SizeOfBuffer,
  IN       VOID     *Buffer
  );

MUST_INSPECT_RETURN
RETURN_STATUS
EFIAPI
LibPcdSetExBoolS (
  IN CONST GUID     *Guid,
  IN UINTN          TokenNumber,
  IN BOOLEAN        Value
  );

#pragma deprecated (PcdSet8, PcdSet16, PcdSet32, PcdSet64, PcdSetPtr, PcdSetBool)
#pragma deprecated (PcdSetEx8, PcdSetEx16, PcdSetEx32, PcdSetEx64, PcdSetExPtr, PcdSetExBool)

//
// PrintLib.h
//
NEW_DEPRECATED_INTERFACE
UINTN
EFIAPI
UnicodeValueToString (
  IN OUT CHAR16  *Buffer,
  IN UINTN       Flags,
  IN INT64       Value,
  IN UINTN       Width
  );

NEW_DEPRECATED_INTERFACE
UINTN
EFIAPI
AsciiValueToString (
  OUT CHAR8      *Buffer,
  IN  UINTN      Flags,
  IN  INT64      Value,
  IN  UINTN      Width
  );

//
// UefiLib.h
//
NEW_DEPRECATED_INTERFACE
VOID *
EFIAPI
GetVariable (
  IN CONST CHAR16    *Name,
  IN CONST EFI_GUID  *Guid
  );

NEW_DEPRECATED_INTERFACE
VOID *
EFIAPI
GetEfiGlobalVariable (
  IN CONST CHAR16  *Name
  );

#include "SourceCodeAnalysis/ScaBaseMemoryLib.h"
#include "SourceCodeAnalysis/ScaMemoryAllocationLib.h"

#pragma warning( pop )

#endif

