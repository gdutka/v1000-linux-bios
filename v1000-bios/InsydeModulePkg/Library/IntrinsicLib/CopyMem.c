/** @file
  Intrinsic Memory Routines Wrapper Implementation for OpenSSL-based
  Cryptographic Library.

Copyright (c) 2010, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Base.h>
#include <Library/BaseMemoryLib.h>

/* Copies bytes between buffers */
void * memcpy (void *DestinationBuffer, const void *SourceBuffer, UINTN Length)
{
  volatile UINT8                    *Destination8;
  CONST UINT8                       *Source8;

  if (Length == 0) {
    return DestinationBuffer;
  }

  if (DestinationBuffer == SourceBuffer) {
    return DestinationBuffer;
  }

  if (SourceBuffer > DestinationBuffer) {
    Destination8 = (UINT8*)DestinationBuffer;
    Source8 = (CONST UINT8*)SourceBuffer;
    while (Length-- != 0) {
      *(Destination8++) = *(Source8++);
    }
  } else if (SourceBuffer < DestinationBuffer) {
    Destination8 = (UINT8*)DestinationBuffer + Length;
    Source8 = (CONST UINT8*)SourceBuffer + Length;
    while (Length-- != 0) {
      *(--Destination8) = *(--Source8);
    }
  }

  return DestinationBuffer;
}

