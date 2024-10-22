/** @file

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


#ifndef _AMD_MEM_CONTEXT_SAVE_RESTORE_PPI_H_
#define _AMD_MEM_CONTEXT_SAVE_RESTORE_PPI_H_

#define AMD_MEM_CONTEXT_SAVE_RESTORE_PPI_GUID \
  { \
    0xfbce6942, 0x598a, 0x417d, {0xa6, 0x25, 0x43, 0x5a, 0x9d, 0x6b, 0x45, 0x26 } \
  }

typedef struct _AMD_MEM_CONTEXT_SAVE_RESTORE_PPI AMD_MEM_CONTEXT_SAVE_RESTORE_PPI;


//
// GET_MEM_CONTEXT_POLICY_PPI
//
typedef
BOOLEAN
(EFIAPI *GET_MEM_CONTEXT_POLICY_PPI) (
  IN  EFI_PEI_SERVICES   **PeiServices
  );

//
// AMD_MEM_CONTEXT_SAVE_RESTORE_PPI
//
struct _AMD_MEM_CONTEXT_SAVE_RESTORE_PPI {
  GET_MEM_CONTEXT_POLICY_PPI  GetMemContextPolicy;
};

extern EFI_GUID gAmdMemContextSaveRestorePpiGuid;

#endif
