/** @file
  This library class provides services to non-SMM drivers to access
  IHISI services.

;******************************************************************************
;* Copyright (c) 2023, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/
#ifndef _EFI_INTERAL_H2O_IHISI_LIB_H_
#define _EFI_INTERAL_H2O_IHISI_LIB_H_

#include <Uefi.h>
#include <SmiTable.h>
#include <H2OIhisi.h>

#include <Library/BaseLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DebugLib.h>
#include <Library/H2OIhisiLib.h>

#define H2O_IHISI_LIB_BUFFER_SIZE         0x20000
#define IHISI_EBX_SIGNATURE               SIGNATURE_32 ('O', '2', 'H', '$')

#pragma pack(1)
typedef struct _IHISI_LIB_SMI_DATA {
  UINT32  Eax;
  UINT32  Ebx;
  UINT32  Ecx;
  UINT32  Edx;
  UINT32  Esi;
  UINT32  Edi;
} IHISI_LIB_SMI_DATA;
#pragma pack()
#endif

