/** @file

  PiSmmCommunication private data structure.

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

#ifndef _SMM_COMMUNICATION_PRIVATE_H_
#define _SMM_COMMUNICATION_PRIVATE_H_

#pragma pack(push, 1)

#define SMM_COMMUNICATION_SIGNATURE SIGNATURE_32 ('S','M','M','C')

typedef struct {
  UINT32                   Signature;
  UINT32                   SwSmiNumber;
  EFI_PHYSICAL_ADDRESS     BufferPtrAddress;
} EFI_SMM_COMMUNICATION_CONTEXT;

#pragma pack(pop)

#endif
