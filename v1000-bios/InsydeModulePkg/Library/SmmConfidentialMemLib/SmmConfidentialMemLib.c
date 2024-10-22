/** @file
   SMM Confidential Memory Library

;******************************************************************************
;* Copyright (c) 2019, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/
/** @file

 @copyright
  INTEL CONFIDENTIAL
  Copyright 2019 Intel Corporation.

  The source code contained or described herein and all documents related to the
  source code ("Material") are owned by Intel Corporation or its suppliers or
  licensors. Title to the Material remains with Intel Corporation or its suppliers
  and licensors. The Material may contain trade secrets and proprietary and
  confidential information of Intel Corporation and its suppliers and licensors,
  and is protected by worldwide copyright and trade secret laws and treaty
  provisions. No part of the Material may be used, copied, reproduced, modified,
  published, uploaded, posted, transmitted, distributed, or disclosed in any way
  without Intel's prior express written permission.

  No license under any patent, copyright, trade secret or other intellectual
  property right is granted to or conferred upon you by disclosure or delivery
  of the Materials, either expressly, by implication, inducement, estoppel or
  otherwise. Any license under such intellectual property rights must be
  express and approved by Intel in writing.

  Unless otherwise agreed by Intel in writing, you may not remove or alter
  this notice or any other notice embedded in Materials by Intel or
  Intel's suppliers or licensors in any way.

  This file contains 'Framework Code' and is licensed as such under the terms
  of your license agreement with Intel or your vendor. This file may not be
  modified, except as allowed by additional terms of your license agreement.

**/

#include <PiSmm.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>

#include <Library/SmmConfidentialMemLib.h>
#include <Protocol/SmmConfidentialMem.h>

EDKII_SMM_CONF_MEM_PROTOCOL* mSmmConfMemProt = NULL;

/**
  SmmAllocateConfidentialPool - Allocates SMM memory for confidential storage

  @param[in]     Size           Requested size in bytes
  @param[in,out] Buffer         Buffer address

  @retval EFI_SUCCESS           Requested buffer was allocated successfully.
  @retval EFI_UNSUPPORTED       Current CPU doesn't support this feature.
  @retval EFI_INVALID_PARAMETER Size is 0 or Buffer is NULL.
  @retval EFI_OUT_OF_RESOURCES  Requested buffer could not be allocated.

**/
EFI_STATUS
EFIAPI
SmmAllocateConfidentialPool (
  IN      UINTN  Size,
  IN OUT  VOID** Buffer
  )
{
  if (gSmst == NULL) {
    return EFI_UNSUPPORTED;
  }
  if (Size == 0 || Buffer == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  if (mSmmConfMemProt != NULL) {
    return mSmmConfMemProt->SmmAllocateConfPool (Size, Buffer);
  }
  return gSmst->SmmAllocatePool (EfiRuntimeServicesData, Size, Buffer);
}

/**
  SmmFreeConfidentialPool -     Frees previously allocated SMM memory

  @param[in,out] Buffer                 Buffer address

  @retval EFI_SUCCESS           Buffer was freed successfully.
  @retval EFI_INVALID_PARAMETER Buffer is NULL.
  @retval EFI_NOT_FOUND         Buffer could not be freed.

**/
EFI_STATUS
EFIAPI
SmmFreeConfidentialPool (
  IN OUT  VOID   **Buffer
  )
{
  EFI_STATUS Status;

  if (gSmst == NULL) {
    return EFI_UNSUPPORTED;
  }

  if (Buffer == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (mSmmConfMemProt != NULL) {
    return mSmmConfMemProt->SmmFreeConfPool (Buffer);
  }

  Status = gSmst->SmmFreePool (*Buffer);
  if (!EFI_ERROR (Status)) {
    *Buffer = NULL;
  }
  return Status;
}

/**
  SmmConfidentialMemConstructor

  @param ImageHandle
  @param SystemTable

  @retval EFI_SUCCESS           Buffer was freed successfully.
  @retval EFI_OUT_OF_RESOURCES  Buffer could not be allocated.

**/
EFI_STATUS
EFIAPI
SmmConfidentialMemConstructor (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS Status;

  if (gSmst == NULL) {
    return EFI_SUCCESS;
  }
  Status = gSmst->SmmLocateProtocol (
                    &gEdkiiSmmConfMemProtocolGuid,
                    NULL,
                    (void**)&mSmmConfMemProt
                    );

  return EFI_SUCCESS;
}
