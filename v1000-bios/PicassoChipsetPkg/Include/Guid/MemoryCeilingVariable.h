/** @file
  Definitions for Memory Ceiling Variable

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _MEMORY_CEILING_VARIABLE_H_
#define _MEMORY_CEILING_VARIABLE_H_

//
// Variable Name
//
#define MEMORY_CEILING_VARIABLE_NAME        L"MemCeil."

#define MEMORY_CEILING_VARIABLE_ATTRIBUTES \
        (EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | \
         EFI_VARIABLE_RUNTIME_ACCESS)

typedef EFI_PHYSICAL_ADDRESS MEMORY_CEILING_VARIABLE_DATA;

//
// Variable Guid
//
#define MEMORY_CEILING_VARIABLE_GUID \
  { 0xad22518b, 0x450a, 0x4857, {0x96, 0xc3, 0xd1, 0xe2, 0x2f, 0x20, 0x61, 0x29} }

extern EFI_GUID gMemoryCeilingVariableGuid;

#endif
