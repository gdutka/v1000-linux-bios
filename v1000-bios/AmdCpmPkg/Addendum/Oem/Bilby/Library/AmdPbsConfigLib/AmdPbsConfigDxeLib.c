/*****************************************************************************
 *
 * Copyright (C) 2015-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 ******************************************************************************
 */

#include <Uefi.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiLib.h>
#include <Dxe/AmdPbsSetupDxe/AmdPbsConfig.h>


/**
 Set AMD PBS setup configuration data in Variable

 @param[out] AmdPbsConfiguration Pointer to the structure of AMD_PBS_SETUP_OPTION,
                                this pointer must be allocated with sizeof(AMD_PBS_SETUP_OPTION)
                                before being called

 @retval EFI_SUCCESS            The AMD PBS configuration is successfully retrieved
 @retval EFI_INVALID_PARAMETER  NULL pointer for input KernelConfig paramater
 @return others                 Failed to retrieve kernel configuration
**/
EFI_STATUS
EFIAPI
SetAmdPbsConfiguration (
  IN AMD_PBS_SETUP_OPTION        *AmdPbsConfiguration
  )
{
  EFI_STATUS Status;
  UINT8      *Buffer;
  UINTN      BufferSize;

  if (AmdPbsConfiguration == NULL) {
    ASSERT_EFI_ERROR (AmdPbsConfiguration != NULL);
    return EFI_INVALID_PARAMETER;
  }

  Buffer       = NULL;
  BufferSize   = 0;
  Status = gRT->GetVariable (
                  AMD_PBS_SETUP_VARIABLE_NAME,
                  &gAmdPbsSystemConfigurationGuid,
                  NULL,
                  &BufferSize,
                  Buffer
                  );
  if (Status == EFI_BUFFER_TOO_SMALL && (BufferSize == sizeof (AMD_PBS_SETUP_OPTION))) {
    Buffer = AllocatePool (BufferSize);
    if (Buffer == NULL) {
      ASSERT_EFI_ERROR (Buffer != NULL);
      return EFI_OUT_OF_RESOURCES;
    }
    Status = gRT->GetVariable (
                    AMD_PBS_SETUP_VARIABLE_NAME,
                    &gAmdPbsSystemConfigurationGuid,
                    NULL,
                    &BufferSize,
                    Buffer
                    );
    if (Status == EFI_SUCCESS) {
      CopyMem (Buffer, AmdPbsConfiguration, sizeof (AMD_PBS_SETUP_OPTION));
      Status = gRT->SetVariable (
                    AMD_PBS_SETUP_VARIABLE_NAME,
                    &gAmdPbsSystemConfigurationGuid,
                    EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                    BufferSize,
                    Buffer
                    );
    }
    ASSERT_EFI_ERROR (Status);
    FreePool (Buffer);
    return Status;
  }

  //
  // No Setup variable, power on at the first time after updating firmware
  // retrieve default Setup data from SetupUtility protocol
  //
  Buffer = AllocatePool (sizeof (AMD_PBS_SETUP_OPTION));
  if (Buffer == NULL) {
    ASSERT_EFI_ERROR (Buffer != NULL);
    return EFI_OUT_OF_RESOURCES;
  }
  CopyMem (Buffer, AmdPbsConfiguration, sizeof (AMD_PBS_SETUP_OPTION));
  BufferSize = sizeof (AMD_PBS_SETUP_OPTION);
  Status = gRT->SetVariable (
                AMD_PBS_SETUP_VARIABLE_NAME,
                &gAmdPbsSystemConfigurationGuid,
                EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                BufferSize,
                Buffer
                );

    return Status;
}

/**
 Retrieve AMD PBS setup configuration data

 @param[out] AmdPbsConfiguration Pointer to the structure of AMD_PBS_SETUP_OPTION,
                                this pointer must be allocated with sizeof(AMD_PBS_SETUP_OPTION)
                                before being called

 @retval EFI_SUCCESS            The AMD PBS configuration is successfully retrieved
 @retval EFI_INVALID_PARAMETER  NULL pointer for input KernelConfig paramater
 @return others                 Failed to retrieve kernel configuration
**/
EFI_STATUS
EFIAPI
GetAmdPbsConfiguration (
  OUT AMD_PBS_SETUP_OPTION     *AmdPbsConfiguration
  )
{
  EFI_STATUS Status;
  UINT8      *Buffer;
  UINTN      BufferSize;

  if (AmdPbsConfiguration == NULL) {
    ASSERT_EFI_ERROR (AmdPbsConfiguration != NULL);
    return EFI_INVALID_PARAMETER;
  }

  // The first time set the BufferSize to 0 to get the actual BufferSize should be used, then allocate the Buffer and get the variable again
  Buffer       = NULL;
  BufferSize   = 0;
  Status = gRT->GetVariable (
                  AMD_PBS_SETUP_VARIABLE_NAME,
                  &gAmdPbsSystemConfigurationGuid,
                  NULL,
                  &BufferSize,
                  Buffer
                  );
  if (Status == EFI_BUFFER_TOO_SMALL && (BufferSize == sizeof (AMD_PBS_SETUP_OPTION))) {
    Buffer = AllocatePool (BufferSize);
    if (Buffer == NULL) {
      ASSERT_EFI_ERROR (Buffer != NULL);
      return EFI_OUT_OF_RESOURCES;
    }
    Status = gRT->GetVariable (
                    AMD_PBS_SETUP_VARIABLE_NAME,
                    &gAmdPbsSystemConfigurationGuid,
                    NULL,
                    &BufferSize,
                    Buffer
                    );
    if (Status == EFI_SUCCESS) {
      CopyMem (AmdPbsConfiguration, Buffer, sizeof (AMD_PBS_SETUP_OPTION));
    }
    ASSERT_EFI_ERROR (Status);
    FreePool (Buffer);
    return Status;
  }

  return Status;
}
