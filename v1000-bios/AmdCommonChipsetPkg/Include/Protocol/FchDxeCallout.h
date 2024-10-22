/** @file
  Azalia policy structure and GUID declaration.

;*******************************************************************************
;* Copyright (c) 2012 - 2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;*******************************************************************************
*/

#ifndef _FCH_DXE_CALLOUT_H_
#define _FCH_DXE_CALLOUT_H_

#define FCH_DXE_CALLOUT_PROTOCOL_GUID \
  { \
	0x972EF504, 0x281b, 0x470d, 0xa6, 0x54, 0xcc, 0xad, 0xeb, 0x12, 0x6c, 0xca \
  }

typedef struct _EFI_FCH_DXE_CALLOUT_PROTOCOL   EFI_FCH_DXE_CALLOUT_PROTOCOL;

typedef
VOID
(EFIAPI *EFI_FCH_CALLOUT_ENTRY)(
  IN EFI_FCH_DXE_CALLOUT_PROTOCOL  *This
  );

struct _EFI_FCH_DXE_CALLOUT_PROTOCOL {
  EFI_FCH_CALLOUT_ENTRY Entry;
};

extern EFI_GUID gFchDxeCalloutProtocolGuid;

#endif