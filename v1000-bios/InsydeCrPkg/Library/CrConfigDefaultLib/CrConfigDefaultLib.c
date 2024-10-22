/** @file
  This Library will install CrConfigDefaultLib for reference.

;******************************************************************************
;* Copyright (c) 2015, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <Guid/CrConfigHii.h>

#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <Library/VariableLib.h>
#include <Library/HiiLib.h>
#include <Library/HiiExLib.h>

/**

  Extract default CrConfig data.

  @param CrConfigDefault     A pointer to the CrConfig data buffer

  @retval EFI_SUCCESS        Extract CrConfig data successfully

**/
EFI_STATUS
ExtractCrConfigDefault (
  UINT8 *CrConfigDefault
  )
{
  UINTN                  ConfigSize;
  UINT8                  SkuId;
  EFI_STATUS             Status;

  ConfigSize = sizeof (CR_CONFIGURATION);
  SkuId      = (UINT8) LibPcdGetSku ();
  
  Status = CommonGetDefaultVariable (
             CONSOLE_REDIRECTION_VARSTORE_NAME,
             &gH2OCrConfigurationGuid,
             SkuId,
             NULL,
             &ConfigSize,
             CrConfigDefault
             );
  if (Status != EFI_SUCCESS) {
    Status = CommonGetDefaultVariable (
               CONSOLE_REDIRECTION_VARSTORE_NAME,
               &gH2OCrConfigurationGuid,
               0,
               NULL,
               &ConfigSize,
               CrConfigDefault
               );
  }
  ASSERT (Status == EFI_SUCCESS);
  
  return Status;
}

/**
  Initial CrConfig variable data

  @retval EFI_SUCCESS        Initial CrConfig variable data successfully

**/
EFI_STATUS
CrConfigVarInit (
  void
  )
{
  EFI_STATUS             Status;
  CR_CONFIGURATION       *CrConfig;
  CR_CONFIGURATION       *CrConfigDefault;

  Status = EFI_SUCCESS;
  CrConfigDefault = CommonGetVariableData (CONSOLE_REDIRECTION_VARSTORE_DEFAULT_NAME, &gH2OCrConfigurationGuid);
  if (CrConfigDefault == NULL) {
    CrConfigDefault = (CR_CONFIGURATION*)AllocatePool (sizeof (CR_CONFIGURATION));
    ExtractCrConfigDefault ((UINT8*)CrConfigDefault);

    Status = CommonSetVariable (
               CONSOLE_REDIRECTION_VARSTORE_DEFAULT_NAME,
               &gH2OCrConfigurationGuid,
               EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
               sizeof (CR_CONFIGURATION),
               CrConfigDefault
               );

  }

  CrConfig = CommonGetVariableData (CONSOLE_REDIRECTION_VARSTORE_NAME, &gH2OCrConfigurationGuid);
  if (CrConfig == NULL) {

    Status = CommonSetVariable (
               CONSOLE_REDIRECTION_VARSTORE_NAME,
               &gH2OCrConfigurationGuid,
               EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
               sizeof (CR_CONFIGURATION),
               CrConfigDefault
               );
  }

  if (CrConfigDefault != NULL) {
    FreePool (CrConfigDefault);
  }

  if (CrConfig != NULL) {
    FreePool (CrConfig);
  }

  return Status;
}

/**
  Function initialize the Setup browser data to default according to input variable data (VariableName/VendorGuid).

  @param[in]      VarName    String part of EFI variable name.
  @param[in]      VarGuid    GUID part of EFI variable name.
  @param[in]      DataSize   Returns the size of the EFI variable that was read.
  @param[in, out] Data       Dynamically allocated memory that contains a copy of the EFI variable.
                             Caller is responsible freeing the buffer.

  @retval EFI_SUCCESS                Get EFI variable Successful.
  @retval EFI_INVALID_PARAMETER      VariableName is NULL or VendorGuid is NULL or VariableDataSize is NULL or VariableData
                                     is NULL.
  @retval EFI_NOT_FOUND              Cannot find specific EFI variable.
  @retval EFI_BUFFER_TOO_SMALL       Input variable data size is too small.  VariableDataSize has been updated with the size
                                     needed to complete the request.
**/
EFI_STATUS
LoadVarDefault (
  IN CHAR16              *VarName,
  IN EFI_GUID            *VarGuid,
  IN UINTN               *DataSize,
  IN OUT VOID            *Data
  )
{
  EFI_STATUS             Status;
  UINT8                  SkuId;
  BOOLEAN                Success;

  if (VarName == NULL || VarGuid == NULL || DataSize == NULL || Data == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (*DataSize == 0) {
    return EFI_INVALID_PARAMETER;
  }

  ZeroMem (Data, *DataSize);

  SkuId = (UINT8) LibPcdGetSku ();

  Status = CommonGetDefaultVariable (
             VarName,
             VarGuid,
             SkuId,
             NULL,
             DataSize,
             Data
             );
  if (Status == EFI_BUFFER_TOO_SMALL) {
    return Status;
  } else if (Status != EFI_SUCCESS) {
    SkuId = 0;
    Status = CommonGetDefaultVariable (
               VarName,
               VarGuid,
               SkuId,
               NULL,
               DataSize,
               Data
               );
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }

  Success = HiiSetBrowserData (VarGuid, VarName, *DataSize, Data, NULL);
  if (!Success) {
    return EFI_NOT_FOUND;
  }

  return EFI_SUCCESS;
}

/**
  Function initialize the EFI variable to default according to input variable data (VariableName/VendorGuid).

  @param[in]      VarName    String part of EFI variable name.
  @param[in]      VarGuid    GUID part of EFI variable name.
  @param[in]      DataSize   Returns the size of the EFI variable that was read.
  @param[in, out] Data       Dynamically allocated memory that contains a copy of the EFI variable.
                             Caller is responsible freeing the buffer.

  @retval EFI_SUCCESS                Get EFI variable Successful.
  @retval EFI_INVALID_PARAMETER      VariableName is NULL or VendorGuid is NULL or VariableDataSize is NULL or VariableData
                                     is NULL.
  @retval EFI_NOT_FOUND              Cannot find specific EFI variable.
  @retval EFI_BUFFER_TOO_SMALL       Input variable data size is too small.  VariableDataSize has been updated with the size
                                     needed to complete the request.
**/
EFI_STATUS
InitVarDefault (
  IN CHAR16              *VarName,
  IN EFI_GUID            *VarGuid,
  IN UINTN               *DataSize,
  IN OUT VOID            *Data
  )
{
  EFI_STATUS             Status;
  UINT8                  SkuId;

  if (VarName == NULL || VarGuid == NULL || DataSize == NULL || Data == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (*DataSize == 0) {
    return EFI_INVALID_PARAMETER;
  }

  ZeroMem (Data, *DataSize);

  SkuId = (UINT8) LibPcdGetSku ();

  Status = CommonGetDefaultVariable (
             VarName,
             VarGuid,
             SkuId,
             NULL,
             DataSize,
             Data
             );
  if (Status == EFI_BUFFER_TOO_SMALL) {
    return Status;
  } else if (Status != EFI_SUCCESS) {
    SkuId = 0;
    Status = CommonGetDefaultVariable (
               VarName,
               VarGuid,
               SkuId,
               NULL,
               DataSize,
               Data
               );
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }

  Status = CommonSetVariable (
              VarName,
              VarGuid,
              EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS |EFI_VARIABLE_NON_VOLATILE,
              *DataSize,
              Data
              );

  return Status;
}

