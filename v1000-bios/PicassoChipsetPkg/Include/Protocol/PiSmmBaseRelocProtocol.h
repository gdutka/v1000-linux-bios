/** @file
  PiSmmBaseReloc protocol interface definition.

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

#ifndef _PI_SMM_BASE_RELOC_PROTOCOL_H_
#define _PI_SMM_BASE_RELOC_PROTOCOL_H_

///
/// Global ID for the PI_SMM_BASE_RELOC_PROTOCOL.
///
#define PI_SMM_BASE_RELOC_PROTOCOL_GUID \
  { \
    0xda5a04d4, 0x712f, 0x4075, { 0x92, 0xd6, 0x26, 0xf4, 0x9f, 0xe9, 0xb5, 0xd7 } \
  }

///
/// Forward declaration for the PI_SMM_BASE_RELOC_PROTOCOL.
///
typedef struct _PI_SMM_BASE_RELOC_PROTOCOL PI_SMM_BASE_RELOC_PROTOCOL;

typedef
EFI_STATUS
(EFIAPI *PI_SMM_BASE_RELOC)(
  IN PI_SMM_BASE_RELOC_PROTOCOL *This,
  IN UINTN                      *Smmbase,
  IN UINTN                      NumberOfCpu,
  IN UINTN                      BspNumber
  );

struct _PI_SMM_BASE_RELOC_PROTOCOL {
  PI_SMM_BASE_RELOC  PiSmmBaseReloc;
};

extern EFI_GUID gPiSmmBaseRelocProtocolGuid;

#endif
