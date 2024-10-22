/** @file
 CrConfig Default library 

;******************************************************************************
;* Copyright (c) 2012 - 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _CRCONFIG_DEFAULT_LIB_H_
#define _CRCONFIG_DEFAULT_LIB_H_

#include <Uefi.h>

EFI_STATUS
CrConfigVarInit (
  VOID
  );

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
  );

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
  );

#endif

