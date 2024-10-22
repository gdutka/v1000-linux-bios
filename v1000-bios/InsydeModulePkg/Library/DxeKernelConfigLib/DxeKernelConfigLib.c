/** @file
 Library Instance for KernelConfigLib Library Class.

;******************************************************************************
;* Copyright (c) 2012 - 2019, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <Uefi.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DebugLib.h>
#include <Library/VariableLib.h>
#include <Library/PcdLib.h>
#include <KernelSetupConfig.h>


//
// For padding rule, the SETUP_DATA_PROVIDER_CONTEXT size should be 0x0C in IA32 mode and
// should be 0x18 in X64 mode.
// typedef struct {
//   EFI_PEI_READ_ONLY_VARIABLE2_PPI    *VariablePpi;
//   UINTN                              SetupDataSize;
//   UINT8                              SetupData[1];
// } SETUP_DATA_PROVIDER_CONTEXT;
//
//
#define IA32_SETUP_DATA_PROVIDER_CONTEXT_SIZE      0x0C
#define X64_SETUP_DATA_PROVIDER_CONTEXT_SIZE       0x18

/**
 Retrieve kernel setup configuration data

 @param[out] KernelConfig       Pointer to the structure of KERNEL_CONFIGURATION,
                                this pointer must be allocated with sizeof(KERNEL_CONFIGURATION)
                                before being called

 @retval EFI_SUCCESS            The kernel configuration is successfully retrieved
 @retval EFI_INVALID_PARAMETER  NULL pointer for input KernelConfig paramater
 @return others                 Failed to retrieve kernel configuration
**/
EFI_STATUS
EFIAPI
GetKernelConfiguration (
  OUT KERNEL_CONFIGURATION      *KernelConfig
  )
{
  EFI_STATUS Status;
  VOID       *Buffer;
  UINTN      BufferSize;

  if (KernelConfig == NULL) {
    ASSERT (KernelConfig != NULL);
    return EFI_INVALID_PARAMETER;
  }

  Buffer = NULL;
  Status = CommonGetVariableDataAndSize (
             SETUP_VARIABLE_NAME,
             &gSystemConfigurationGuid,
             &BufferSize,
             (VOID **) &Buffer
             );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  CopyMem (KernelConfig, Buffer, sizeof(KERNEL_CONFIGURATION));
  FreePool (Buffer);
  return EFI_SUCCESS;
}

/**
 Retrieve the size of Setup variable

 @return                        Size of the setup variable data
**/
UINTN
EFIAPI
GetSetupVariableSize (
  VOID
  )
{
  EFI_STATUS             Status;
  UINTN                  Size;
  UINT8                  SkuId;

  Size = 0;
  Status = CommonGetVariable (
             SETUP_VARIABLE_NAME,
             &gSystemConfigurationGuid,
             &Size,
             NULL
             );
  if (Status == EFI_BUFFER_TOO_SMALL) {
    return Size;
  }

  SkuId = (UINT8) LibPcdGetSku ();
  Size = 0;
  Status = CommonGetDefaultVariable (
             SETUP_VARIABLE_NAME,
             &gSystemConfigurationGuid,
             SkuId,
             NULL,
             &Size,
             NULL
             );
  if (Status == EFI_BUFFER_TOO_SMALL) {
    return Size;
  }

  if (SkuId != 0) {
    Size = 0;
    Status = CommonGetDefaultVariable (
               SETUP_VARIABLE_NAME,
               &gSystemConfigurationGuid,
               0,
               NULL,
               &Size,
               NULL
               );
    if (Status == EFI_BUFFER_TOO_SMALL) {
      return Size;
    }
  }

  ASSERT (FALSE);
  return 0;
}

