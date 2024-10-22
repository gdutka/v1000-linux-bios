/** @file
  This module forces BIOS load default if CMOS checksum corrupted

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
#include <Library/PeiServicesLib.h>
#include <Library/DebugLib.h>
#include <Library/CmosLib.h>
#include <Library/HobLib.h>

#include <Ppi/ReadOnlyVariable2.h>
#include <Guid/NvVariableFail.h>

typedef struct {
  EFI_PEI_READ_ONLY_VARIABLE2_PPI    *VariablePpi;
} NV_VARIABLE_FAIL_DETECT_CONTEXT;

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
NewPeiNextVariableName (
  IN CONST  EFI_PEI_READ_ONLY_VARIABLE2_PPI *This,
  IN OUT UINTN                              *VariableNameSize,
  IN OUT CHAR16                             *VariableName,
  IN OUT EFI_GUID                           *VendorGuid
  )
{
  EFI_PEI_HOB_POINTERS                      GuidHob;
  NV_VARIABLE_FAIL_DETECT_CONTEXT           *NvVariableFailDetectContext;

  GuidHob.Raw = GetHobList ();
  GuidHob.Raw = GetNextGuidHob (&gNvVariableFailGuid, GuidHob.Raw);
  if (GuidHob.Raw != NULL) {
    return EFI_NOT_FOUND;
  }

  NvVariableFailDetectContext = (NV_VARIABLE_FAIL_DETECT_CONTEXT *)GET_GUID_HOB_DATA (GuidHob.Guid);
  if (NvVariableFailDetectContext == NULL) {
    ASSERT (FALSE);
    return EFI_NOT_FOUND;
  }

  return NvVariableFailDetectContext->VariablePpi->NextVariableName (
                                                       This,
                                                       VariableNameSize,
                                                       VariableName,
                                                       VendorGuid
                                                       );
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
NewPeiGetVariable (
  IN CONST EFI_PEI_READ_ONLY_VARIABLE2_PPI  *This,
  IN CONST CHAR16                           *VariableName,
  IN CONST EFI_GUID                         *VendorGuid,
  OUT UINT32                                *Attributes OPTIONAL,
  IN OUT UINTN                              *DataSize,
  OUT VOID                                  *Data
  )
{
  EFI_PEI_HOB_POINTERS                      GuidHob;
  NV_VARIABLE_FAIL_DETECT_CONTEXT           *NvVariableFailDetectContext;

  GuidHob.Raw = GetHobList ();
  GuidHob.Raw = GetNextGuidHob (&gNvVariableFailGuid, GuidHob.Raw);
  if (GuidHob.Raw != NULL) {
    return EFI_NOT_FOUND;
  }

  NvVariableFailDetectContext = (NV_VARIABLE_FAIL_DETECT_CONTEXT *)GET_GUID_HOB_DATA (GuidHob.Guid);
  if (NvVariableFailDetectContext == NULL) {
    ASSERT (FALSE);
    return EFI_NOT_FOUND;
  }

  return NvVariableFailDetectContext->VariablePpi->GetVariable (
                                                  This,
                                                  VariableName,
                                                  VendorGuid,
                                                  Attributes,
                                                  DataSize,
                                                  Data
                                                  );
}

EFI_PEI_READ_ONLY_VARIABLE2_PPI mNvVariableFailDetectVariablePpi = {
  NewPeiGetVariable,
  NewPeiNextVariableName
};

EFI_PEI_PPI_DESCRIPTOR     mNvVariableFailDetectVariablePpiList = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gEfiPeiReadOnlyVariable2PpiGuid,
  &mNvVariableFailDetectVariablePpi
};

/**
  Hook variable PPI in PEI phase.

  @param FileHandle             Handle of the file being invoked.
  @param PeiServices            Describes the list of possible PEI Services.

 @retval EFI_STATUS

**/
EFI_STATUS
ReadOnlyVariable2HookCallback (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  )
{
  EFI_STATUS                                Status;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI           *VariablePpi;
  EFI_PEI_PPI_DESCRIPTOR                    *VariablePpiDescriptor;
  NV_VARIABLE_FAIL_DETECT_CONTEXT           *NvVariableFailDetectContext;
  EFI_PEI_HOB_POINTERS                      GuidHob;

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
  GuidHob.Raw = GetNextGuidHob (&gNvVariableFailGuid, GuidHob.Raw);
  if (GuidHob.Raw != NULL) {
    return EFI_SUCCESS;
  }

  NvVariableFailDetectContext = BuildGuidHob (
                                 &gNvVariableFailGuid,
                                 sizeof (NV_VARIABLE_FAIL_DETECT_CONTEXT)
                                 );
  if (NvVariableFailDetectContext == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    ASSERT_EFI_ERROR (Status);
    return Status;
  }
  NvVariableFailDetectContext->VariablePpi = VariablePpi;

  Status = PeiServicesReInstallPpi (
             VariablePpiDescriptor,
             &mNvVariableFailDetectVariablePpiList
             );

  return Status;
}

EFI_PEI_NOTIFY_DESCRIPTOR      mVariableNotifyPpiList[] = {
  {
    (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gEfiPeiReadOnlyVariable2PpiGuid,
    ReadOnlyVariable2HookCallback
  }
};

/**
  Initialize CMOS default data and hook variable PPI in PEI phase.

  @param FileHandle             Handle of the file being invoked.
  @param PeiServices            Describes the list of possible PEI Services.

 @retval EFI_STATUS

**/
EFI_STATUS
EFIAPI
NvVariableFailDetectEntryPoint (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  UINT16  RunningChecksum;
  UINT16  Checksum;
  UINT16  TempChecksum;
  UINT8   Address;
  UINT8   Temp;

  //
  // Initialize and Check validate CMOS field 0x10~0x2F.
  // After CMOS clear, the case of CMOS 0x10 ~ 0x2F = 0 results in checksum OK,
  // which causes BIOS setup can't load default.
  // Check whether CMOS 0x10 ~ 0x2F = 0, if so, modify CMOS 0x10 ~ 0x2F = 0xFF
  //
  for (Address = 0x10; Address < 0x30; Address++) {
    Temp = ReadCmos8 (Address);
    if (Temp != 0) break;
  }
  if (Address == 0x30) {
    for (Address = 0x10; Address < 0x30; Address++) {
      Temp = 0xFF;
      WriteCmos8 (Address, Temp);
    }
  }

  RunningChecksum = SumaryCmos();

  TempChecksum = ReadCmos16(CmosCheckSum2E);
  Checksum = (TempChecksum >> 8) + ((TempChecksum & 0xff) << 8);

  if (Checksum != RunningChecksum) {
    PeiServicesNotifyPpi (mVariableNotifyPpiList);
  }
  
  return EFI_SUCCESS;
}
