/** @file
  Header file for SmmResourceCheckDxe driver.

;******************************************************************************
;* Copyright (c) 2018 - 2021, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _SMM_RESOURCE_CHECK_DXE_H_
#define _SMM_RESOURCE_CHECK_DXE_H_

#include <PiSmm.h>


#include <Protocol/SmmBase2.h>
#include <Protocol/SmmCommunication.h>
#include <Protocol/SmmAccess2.h>
#include <Protocol/H2ODialog.h>
//
// The Library classes this module consumes
//
#include <Library/UefiLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/PcdLib.h>


#pragma pack (1)
typedef struct {
  UINTN                        Function;
  EFI_STATUS                   ReturnStatus;
} SMM_RESOURCE_COMMUNICATE_HEADER;

typedef struct {
  UINTN       TotalSmmMemorySize;
  UINTN       RemainingSmmMemorySize;
} SMM_MEMORY_INFO_DATA;

typedef struct {
  SMM_RESOURCE_COMMUNICATE_HEADER       ResourceHeader;
  SMM_MEMORY_INFO_DATA                  MemoryInfo;
} SMM_MEMORY_INFO;
#pragma pack ()

#define GET_SMM_MEMORY_INFO     0x01
#define SMM_RESOURCE_COMM_BUFF_SIZE (OFFSET_OF (EFI_SMM_COMMUNICATE_HEADER, Data) + sizeof(SMM_MEMORY_INFO))
#define SMM_COMMUNICATE_HEADER_SIZE   (OFFSET_OF (EFI_SMM_COMMUNICATE_HEADER, Data))

#define MAX_ALLOCATE_COUNT     0x20

#endif
