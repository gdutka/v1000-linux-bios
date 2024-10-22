/** @file
  AllPciIoPrtclsInstlFinished protocol interface definition.

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

#ifndef _ALL_PCI_IO_PRTCLS_INSTL_FINISHED_H
#define _ALL_PCI_IO_PRTCLS_INSTL_FINISHED_H

//
// Global ID for the AllPciIoPrtclsInstlFinished Protocol
//
#define ALL_PCI_IO_PRTCLS_INSTL_FINISHED_PROTOCOL_GUID \
  { \
    0x676D7012, 0x139B, 0x485A, {0x96, 0xF1, 0x98, 0x6F, 0xC4, 0x8A, 0x86, 0x4B} \
  }

//
// Forward reference for pure ANSI compatability
//
typedef struct _ALL_PCI_IO_PRTCLS_INSTL_FINISHED_PROTOCOL    ALL_PCI_IO_PRTCLS_INSTL_FINISHED_PROTOCOL;

//
// Prototypes for the AllPciIoPrtclsInstlFinished Protocol
//
//None

//
// Interface structure for the AllPciIoPrtclsInstlFinished Protocol
//
struct _ALL_PCI_IO_PRTCLS_INSTL_FINISHED_PROTOCOL {
  VOID *Buffer;
};

extern EFI_GUID gAllPciIoPrtclsInstlFinishedProtocolGuid;

#endif
