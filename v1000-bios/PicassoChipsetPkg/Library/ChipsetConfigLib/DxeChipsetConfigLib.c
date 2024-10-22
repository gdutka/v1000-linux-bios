/** @file
 Library Instance for ChipsetConfigLib Library Class.

;******************************************************************************
;* Copyright (c) 2012 - 2017, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <Uefi.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiLib.h>
#include <Library/VariableLib.h>
#include <KernelSetupConfig.h>
#include <ChipsetSetupConfig.h>

/**
 Set chipset setup configuration data in Variable

 @param[out] ChipsetConfig      Pointer to the structure of CHIPSET_CONFIGURATION,
                                this pointer must be allocated with sizeof(CHIPSET_CONFIGURATION)
                                before being called

 @retval EFI_SUCCESS            The chipset configuration is successfully retrieved
 @retval EFI_INVALID_PARAMETER  NULL pointer for input KernelConfig paramater
 @return others                 Failed to retrieve kernel configuration
**/
EFI_STATUS
EFIAPI
SetChipsetConfigVariable (
  IN CHIPSET_CONFIGURATION        *ChipsetConfig
  )
{
  EFI_STATUS Status;
  UINT8      *Buffer;
  UINTN      BufferSize;

  ASSERT ((ChipsetConfig != NULL));
  if (ChipsetConfig == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Buffer = NULL;
  Status = CommonGetVariableDataAndSize (
             SETUP_VARIABLE_NAME,
             &gSystemConfigurationGuid,
             &BufferSize,
             (VOID **)&Buffer
             );

  if (EFI_ERROR (Status)) {
    return Status;
  }

  ASSERT ((PcdGet32 (PcdSetupConfigSize) == sizeof (CHIPSET_CONFIGURATION)));

  CopyMem ((VOID*)Buffer, (VOID*)ChipsetConfig, (UINTN)sizeof (CHIPSET_CONFIGURATION));

  Status = CommonSetVariable (
             SETUP_VARIABLE_NAME,
             &gSystemConfigurationGuid,
             (UINT32)(EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS),
             (UINTN)BufferSize,
             (VOID*)Buffer
             );
  ASSERT_EFI_ERROR (Status);

  FreePool (Buffer);

  return Status;

}

/**
 Retrieve chipset setup configuration data

 @param[out] ChipsetConfig      Pointer to the structure of CHIPSET_CONFIGURATION,
                                this pointer must be allocated with sizeof(CHIPSET_CONFIGURATION)
                                before being called

 @retval EFI_SUCCESS            The chipset configuration is successfully retrieved
 @retval EFI_INVALID_PARAMETER  NULL pointer for input KernelConfig paramater
 @return others                 Failed to retrieve kernel configuration
**/
EFI_STATUS
EFIAPI
GetChipsetConfiguration (
  OUT CHIPSET_CONFIGURATION     *ChipsetConfig
  )
{
  EFI_STATUS Status;
  UINT8      *Buffer;
  UINTN      BufferSize;

  ASSERT ((ChipsetConfig != NULL));
  if (ChipsetConfig == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Buffer = NULL;
  Status = CommonGetVariableDataAndSize (
             SETUP_VARIABLE_NAME,
             &gSystemConfigurationGuid,
             &BufferSize,
             (VOID **)&Buffer
             );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  ASSERT ((PcdGet32 (PcdSetupConfigSize) == sizeof (CHIPSET_CONFIGURATION)));

  CopyMem ((VOID*)ChipsetConfig, (VOID*)Buffer, (UINTN)sizeof (CHIPSET_CONFIGURATION));
  FreePool ((VOID*)Buffer);

  return EFI_SUCCESS;
}


