/** @file

  PSP SMM Resume PPI definition.

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

#ifndef _PSP_SMM_RESUME_PPI_H_
#define _PSP_SMM_RESUME_PPI_H_

#define PSP_SMM_RESUME_PPI_GUID \
  { \
    0x724c1d03, 0xeb, 0x474b, {0xa4, 0x6, 0xdf, 0xc8, 0x34, 0x45, 0x63, 0x23 } \
  }

typedef struct _PSP_SMM_RESUME_PPI PSP_SMM_RESUME_PPI;

struct _PSP_SMM_RESUME_PPI {
  EFI_PHYSICAL_ADDRESS        HeapBase;
};

extern EFI_GUID gPspSmmResumePpiGuid;

#endif
