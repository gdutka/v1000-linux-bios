/** @file
  This module provides default Setup variable data if Setupvariable is not found
  and also provides a Setup variable cache mechanism in PEI phase

;******************************************************************************
;* Copyright (c) 2013 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/
#include <PiPei.h>
#include <Library/PcdLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/HobLib.h>
#include <Library/DebugLib.h>
#include <Library/KernelConfigLib.h>
#include <Library/VariableLib.h>
#include <Library/MultiConfigBaseLib.h>
#include <Library/SCUVarInfoLib.h>

#include <Ppi/ReadOnlyVariable2.h>
#include <Guid/PeiMCVarHookHob.h>
#include <Guid/SetupDefaultHob.h>
#include <Guid/PeiDefaultSetupProviderHob.h>

#include "MultiConfigVarHookPei.h"

STATIC CHAR16 mSetupVarName[]    = L"Setup";

STATIC 
EFI_PEI_READ_ONLY_VARIABLE2_PPI mMCVarHookFuncPpi = {
  MCPeiGetVariable,
  MCPeiNextVariableName
};

STATIC 
EFI_PEI_PPI_DESCRIPTOR     mMCVarHookFuncPpiList = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gEfiPeiReadOnlyVariable2PpiGuid,
  &mMCVarHookFuncPpi
};
 
STATIC 
EFI_PEI_NOTIFY_DESCRIPTOR      mMCVariableNotifyPpiList[] = {
  {
    (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gEfiPeiReadOnlyVariable2PpiGuid,
    MCReadOnlyVar2HookCallback
  }
};
/**
  Provide the get next variable functionality of the variable services.

  @param  This              A pointer to this instance of the EFI_PEI_READ_ONLY_VARIABLE2_PPI.

  @param  VariableNameSize  On entry, points to the size of the buffer pointed to by VariableName.
  @param  VariableName      On entry, a pointer to a null-terminated string that is the variable's name.
                            On return, points to the next variable's null-terminated name string.

  @param  VariableGuid      On entry, a pointer to an EFI_GUID that is the variable's GUID.
                            On return, a pointer to the next variable's GUID.

  @retval EFI_SUCCESS       The interface could be successfully installed
  @retval EFI_NOT_FOUND     The variable could not be discovered

**/
EFI_STATUS
EFIAPI
MCPeiNextVariableName (
  IN CONST  EFI_PEI_READ_ONLY_VARIABLE2_PPI *This,
  IN OUT UINTN                              *VariableNameSize,
  IN OUT CHAR16                             *VariableName,
  IN OUT EFI_GUID                           *VendorGuid
  )
{
  EFI_STATUS                                Status;
  EFI_PEI_HOB_POINTERS                      GuidHob;
  MC_VAR_HOOK_CONTEXT                       *MCVarHookContext;

  GuidHob.Raw = GetHobList ();
  GuidHob.Raw = GetNextGuidHob (&gPeiMCVarHookGuid, GuidHob.Raw);
  if (GuidHob.Raw == NULL) {
    return EFI_NOT_FOUND;
  }

  MCVarHookContext = (MC_VAR_HOOK_CONTEXT *)GET_GUID_HOB_DATA (GuidHob.Guid);

  Status = MCVarHookContext->VariablePpi->NextVariableName (
                                            This,
                                            VariableNameSize,
                                            VariableName,
                                            VendorGuid
                                            );

  return Status;
}

/**
  Provide the revised read variable functionality of the variable services.

  @param  This                  A pointer to this instance of the EFI_PEI_READ_ONLY_VARIABLE2_PPI.
  @param  VariableName          A pointer to a null-terminated string that is the variable's name.
  @param  VariableGuid          A pointer to an EFI_GUID that is the variable's GUID. The combination of
                                VariableGuid and VariableName must be unique.
  @param  Attributes            If non-NULL, on return, points to the variable's attributes.
  @param  DataSize              On entry, points to the size in bytes of the Data buffer.
                                On return, points to the size of the data returned in Data.
  @param  Data                  Points to the buffer which will hold the returned variable value.

  @retval EFI_SUCCESS           The interface could be successfully installed
  @retval EFI_NOT_FOUND         The variable could not be discovered
  @retval EFI_BUFFER_TOO_SMALL  The caller buffer is not large enough

**/
EFI_STATUS
EFIAPI
MCPeiGetVariable (
  IN CONST EFI_PEI_READ_ONLY_VARIABLE2_PPI  *This,
  IN CONST CHAR16                           *VariableName,
  IN CONST EFI_GUID                         *VendorGuid,
  OUT UINT32                                *Attributes OPTIONAL,
  IN OUT UINTN                              *DataSize,
  OUT VOID                                  *Data
  )
{
  EFI_STATUS                                Status;
  EFI_PEI_HOB_POINTERS                      GuidHob;
  MC_VAR_HOOK_CONTEXT                       *MCVarHookContext;
  SCU_VAR_INFO                              ScuVarInfo;

  if (VariableName == NULL || VendorGuid == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  GuidHob.Raw = GetHobList ();
  GuidHob.Raw = GetNextGuidHob (&gPeiMCVarHookGuid, GuidHob.Raw);
  if (GuidHob.Raw == NULL) {
    ASSERT (FALSE);
    return EFI_NOT_FOUND;
  }

  MCVarHookContext = (MC_VAR_HOOK_CONTEXT *)GET_GUID_HOB_DATA (GuidHob.Guid);
  if (MCVarHookContext == NULL) {
    ASSERT (FALSE);
    return EFI_NOT_FOUND;
  }

  Status = MCVarHookContext->VariablePpi->GetVariable (
                                            This,
                                            VariableName,
                                            VendorGuid,
                                            Attributes,
                                            DataSize,
                                            Data
                                            );
  if ((Status != EFI_NOT_FOUND) && (Status != EFI_SUCCESS)) {
    return Status;
  }

  //
  // Get SCU Var Information structure
  //
  if (GetSCUVarInfo (VariableName, VendorGuid, &ScuVarInfo) != EFI_SUCCESS) {
    //
    // The variable doesn't need to be change from Multi-Config
    //
    return Status;
  }

  if (*DataSize < ScuVarInfo.DataSize) {
    *DataSize = ScuVarInfo.DataSize;
    return EFI_BUFFER_TOO_SMALL;
  } else if (Data == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (Attributes != NULL) {
    *Attributes = ScuVarInfo.Attributes;
  }

  if (Status == EFI_NOT_FOUND) {
    SetMem (Data, ScuVarInfo.DataSize, 0);
    ScuVarInfo.LoadDefaultCallBack (Data);
  }
  
  if (EFI_NOT_FOUND == SetSCUDataFromMC (ScuVarInfo.VarName, &ScuVarInfo.Guid, SETUP_FOR_BIOS_POST, Data, ScuVarInfo.DataSize)) {
    SetSCUDataFromMC (ScuVarInfo.VarName, &ScuVarInfo.Guid, SETUP_FOR_LOAD_DEFAULT, Data, ScuVarInfo.DataSize);
  }
  
  return EFI_SUCCESS;
}

/**
 PeiReadOnlyVariable2 PPI callback routine, filter SCU variable

 @param[in]         PeiServices         General purpose services available to every PEIM.
 @param[in]         NotifyDescriptor    A pointer to notification structure this PEIM registered on install.
 @param[in]         Ppi                 A pointer to S3RestoreAcpiCallback PPI

 @retval            EFI_SUCCESS         Procedure complete.
*/
EFI_STATUS
MCReadOnlyVar2HookCallback (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  )
{
  EFI_STATUS                                Status;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI           *VariablePpi;
  EFI_PEI_PPI_DESCRIPTOR                    *VariablePpiDescriptor;
  MC_VAR_HOOK_CONTEXT                       *MCVarHookContext;
  UINTN                                     VariableSize;
  EFI_PEI_HOB_POINTERS                      GuidHob;
  VOID                                      *SetupDefaultHobData;

  GuidHob.Raw = GetHobList ();
  GuidHob.Raw = GetNextGuidHob (&gPeiDefaultSetupProviderGuid, GuidHob.Raw);
  if (GuidHob.Raw == NULL) {
    //
    // The hook module is hook for SetupDataProviderPei
    //
    return EFI_SUCCESS;
  }

  Status = PeiServicesLocatePpi (
             &gEfiPeiReadOnlyVariable2PpiGuid,
             0,
             &VariablePpiDescriptor,
             (VOID **)&VariablePpi
             );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  GuidHob.Raw = GetHobList ();
  GuidHob.Raw = GetNextGuidHob (&gPeiMCVarHookGuid, GuidHob.Raw);
  if (GuidHob.Raw != NULL) {
    return EFI_SUCCESS;
  }

  MCVarHookContext = BuildGuidHob (
                       &gPeiMCVarHookGuid,
                       sizeof (MC_VAR_HOOK_CONTEXT)
                       );
  if (MCVarHookContext == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    ASSERT_EFI_ERROR (Status);
    return Status;
  }
  MCVarHookContext->VariablePpi = VariablePpi;

  //
  // Change gSetupDefaultHobGuid data
  //
  GuidHob.Raw = GetHobList ();
  GuidHob.Raw = GetNextGuidHob (&gSetupDefaultHobGuid, GuidHob.Raw);
  SetupDefaultHobData = GET_GUID_HOB_DATA (GuidHob.Guid);
  if (SetupDefaultHobData != NULL) {
    VariableSize = GetSetupVariableSize ();

    if (EFI_NOT_FOUND == SetSCUDataFromMC (mSetupVarName, &gSystemConfigurationGuid, SETUP_FOR_BIOS_POST, SetupDefaultHobData, VariableSize)) {
      SetSCUDataFromMC (mSetupVarName, &gSystemConfigurationGuid, SETUP_FOR_LOAD_DEFAULT, SetupDefaultHobData, VariableSize);
    }

  }

  Status = PeiServicesReInstallPpi (
             VariablePpiDescriptor,
             &mMCVarHookFuncPpiList
             );
  return Status;
}

/**
  SCU setup default data and hook variable PPI in PEI phase.

  @param FileHandle             Handle of the file being invoked.
  @param PeiServices            Describes the list of possible PEI Services.

 @retval EFI_STATUS

**/
EFI_STATUS
EFIAPI
MultiConfigVarHookPeiEntryPoint (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  PeiServicesNotifyPpi (mMCVariableNotifyPpiList);
  return EFI_SUCCESS;
}
