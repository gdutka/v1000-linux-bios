/** @file
 H2O Setup Vatiable library implement code.

;******************************************************************************
;* Copyright (c) 2018, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

*/

#include <Library/DxeSetupVariableLib.h>
//[-start-181002-IB10920060-add]//
#include <Library/DxeOemSvcFeatureLib.h>
//[-end-181002-IB10920060-add]//
//[-start-191111-IB17350005-modify]//
#if FixedPcdGet32 (PcdH2OCcbVersion) < 0x05414000
  typedef UINT8 H2O_BOARD_ID;
#else
  #include <H2OBoardId.h>
#endif
//[-end-191111-IB17350005-modify]//

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
  H2O_BOARD_ID           SkuId;
  BOOLEAN                Success;

  if (VarName == NULL || VarGuid == NULL || DataSize == NULL || Data == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (*DataSize == 0) {
    return EFI_INVALID_PARAMETER;
  }

  ZeroMem (Data, *DataSize);

  SkuId = (H2O_BOARD_ID) LibPcdGetSku ();

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

//[-start-181001-IB10920060-add]//
  OemSvcSkipLoadDefaultVariable (VarName, VarGuid, Data);
//[-end-181001-IB10920060-add]//

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
  H2O_BOARD_ID           SkuId;

  if (VarName == NULL || VarGuid == NULL || DataSize == NULL || Data == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (*DataSize == 0) {
    return EFI_INVALID_PARAMETER;
  }

  ZeroMem (Data, *DataSize);

  SkuId = (H2O_BOARD_ID) LibPcdGetSku ();

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
