/** @file
  IHISI Variable Services Implementation

;******************************************************************************
;* Copyright (c) 2023, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/
#include "IhisiVats.h"

//
// Factory default Support Function
//
STATIC
IHISI_REGISTER_TABLE
VATS_REGISTER_TABLE[] = {
  //
  // AH=00h
  //
  { VATSRead, "S00Kn_VatsRead00000", KernelVatsRead}, \

  //
  // AH=01h
  //
  { VATSWrite, "S01Kn_VatsWrite0000", KernelVatsWrite}, \

  //
  // AH=05h
  //
  { VATSNext,  "S05Kn_VatsGetNext00", KernelVatsNext}
};


/**
  Get IHISI status.translated from EFI status

  @param[in] Status  EFI_STATUS

  @return UINT32     IHISI status
**/
UINT32
GetVatsIhisiStatus (
  IN EFI_STATUS                             Status
  )
{
  switch (Status) {
    case EFI_SUCCESS:
      return (UINT32) IHISI_SUCCESS;
      break;

    case EFI_BUFFER_TOO_SMALL:
      return (UINT32) IHISI_OB_LEN_TOO_SMALL;
      break;

    case EFI_NOT_FOUND:
      return (UINT32) IHISI_VATS_VARIABLE_NOT_FOUND;
      break;

    case EFI_INVALID_PARAMETER:
      return (UINT32) IHISI_VATS_WRONG_OB_FORMAT;
      break;

    default:
      return (UINT32) IHISI_VATS_VARIABLE_ACCESS_ERROR;
      break;
  }
}

/**
  IHISI checksum calulation.

  @param[in] Ptr       A pointer to to a buffer.
  @param[in] Size      The buffer size.
  @param[in] Checksum  An element in IHISI output table.

  @retval TURE         Ckecksum error.
  @return FALSE        Ckecksum correct.
**/
BOOLEAN
IhisiChecksumError (
  IN VOID                                *Ptr,
  IN UINTN                               Size,
  IN UINT16                              Checksum
  )
{
  UINTN      Index;
  UINT8      *TempPtr;
  UINT32     Sum;

  TempPtr = Ptr;
  Sum = 0;

  for (Index = 0 ; Index < Size ; Index++) {
    Sum = Sum + TempPtr[Index];
  }

  Sum = (UINT32) ((Checksum + Sum) & 0x0000ffff);

  return (BOOLEAN) Sum;
}




/**
  AH=00h, Read the specific variable into the specified buffer.

  @retval EFI_SUCCESS        AL 00h = Function succeeded.
  @return Others             AL Returned error code.
**/
EFI_STATUS
KernelVatsRead (
  VOID
  )
{
  IHISI_VATS_INPUT_BUFFER                *InBuffer;
  IHISI_VATS_OUTPUT_BUFFER               *OutBuffer;
  UINT8                                  *VarData;
  CHAR16                                 *VarName;
  EFI_STATUS                             Status;
  UINTN                                  VarLength;
  InBuffer  = (IHISI_VATS_INPUT_BUFFER *)(UINTN) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RSI);
  OutBuffer = (IHISI_VATS_OUTPUT_BUFFER *)(UINTN) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RDI);
  VarName   = (CHAR16 *) ((UINTN) InBuffer + (sizeof (IHISI_VATS_INPUT_BUFFER) / sizeof (UINT8)));
  VarData   = (UINT8 *) ((UINTN) OutBuffer + (sizeof (IHISI_VATS_OUTPUT_BUFFER) / sizeof (UINT8)));
  VarLength = (UINTN) OutBuffer->VarLength;
  Status = EFI_UNSUPPORTED;
  if (mSmmVariable != NULL) {
    Status = mSmmVariable->SmmGetVariable (
                             VarName,
                             &InBuffer->VarGuid,
                             &OutBuffer->Attribute,
                             &VarLength,
                             VarData
                             );
  }

  if (Status == EFI_SUCCESS || Status == EFI_BUFFER_TOO_SMALL) {
    //
    // Status is EFI_BUFFER_TOO_SMALL also need return required data size to application
    //
    OutBuffer->VarLength = (UINT32) VarLength;
  }

  return Status;
}

/**
  Check the variable lock is enabled or disabled.

  @retval TRUE        Variable lock is enabled..
  @return FALSE       Variable lock is disabled.
**/
BOOLEAN
IsVariableLockEnabled (
  VOID
  )
{
  UINTN                             DataSize;
  UINT8                             SecureBootEnabled;
  EFI_STATUS                        Status;

  if (FeaturePcdGet (PcdH2OIhisiVatsWriteLockEnabled)) {
    return TRUE;
  }
  DataSize = sizeof (SecureBootEnabled);
  Status = mSmmVariable->SmmGetVariable (
                           EFI_SECURE_BOOT_MODE_NAME,
                           &gEfiGlobalVariableGuid,
                           NULL,
                           &DataSize,
                           &SecureBootEnabled
                           );
  //
  // Variable lock is disabled if secure boot is disabled and PcdH2OVariableLockInVatsWriteEnabled is FALSE.
  //
  if (EFI_ERROR (Status) || SecureBootEnabled == 0) {
    return FALSE;
  }
  return TRUE;
}


/**
  AH=01h, Write the specified buffer to the specific variable.

  @retval EFI_SUCCESS        AL	00h = Function succeeded.
  @return Others             AL	Returned error code.
**/
EFI_STATUS
KernelVatsWrite (
  VOID
  )
{
  IHISI_VATS_OUTPUT_BUFFER               *OutBuffer;
  IHISI_VATS_INPUT_BUFFER                *InBuffer;
  UINT8                                  *VarData;
  CHAR16                                 *VarName;
  EFI_STATUS                             Status;
  H2O_SMM_VARIABLE_LOCK_POLICY_PROTOCOL  *SmmVariableLockPolicy;
  BOOLEAN                                VariableLockEnabled;
  //
  // DS:ESI  Pointer to the beginning of the VATS input buffer.
  // DS:EDI  Pointer to the beginning of the VATS output buffer.
  //
  InBuffer = (IHISI_VATS_INPUT_BUFFER *)(UINTN) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RSI);
  OutBuffer = (IHISI_VATS_OUTPUT_BUFFER *)(UINTN) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RDI);
  VarName = (CHAR16 *) ((UINTN) InBuffer + (sizeof (IHISI_VATS_INPUT_BUFFER) / sizeof (UINT8)));
  VarData = (UINT8 *) ((UINTN) OutBuffer + (sizeof (IHISI_VATS_OUTPUT_BUFFER) / sizeof (UINT8)));
  if (IhisiChecksumError (VarData, OutBuffer->VarLength, OutBuffer->VarChecksum)) {
    return  IHISI_VATS_OB_CHECKSUM_FAILED;
  }

  if (OutBuffer->Attribute == 0) {
    OutBuffer->Attribute = EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS;
  }

  Status = IHISI_VATS_VARIABLE_ACCESS_ERROR;
  if (mSmmVariable != NULL) {
    Status = gSmst->SmmLocateProtocol (
                      &gH2OSmmVariableLockPolicyProtocolGuid,
                      NULL,
                      (VOID **) &SmmVariableLockPolicy
                      );
    if (EFI_ERROR (Status)) {
      SmmVariableLockPolicy = NULL;
    }
    VariableLockEnabled = IsVariableLockEnabled ();
    if (SmmVariableLockPolicy != NULL && VariableLockEnabled) {
      SmmVariableLockPolicy->EnableVariableLock (TRUE);
    }
    Status = mSmmVariable->SmmSetVariable (
                             VarName,
                             &InBuffer->VarGuid,
                             OutBuffer->Attribute,
                             (UINTN) OutBuffer->VarLength,
                             VarData
                             );
    if (SmmVariableLockPolicy != NULL && VariableLockEnabled) {
      SmmVariableLockPolicy->EnableVariableLock (FALSE);
    }
    if (EFI_ERROR (Status)) {
      Status = IHISI_VATS_VARIABLE_ACCESS_ERROR;
    }
  }

  return Status;
}

/**
  AH=05h, Get next specified buffer to the specific variable.

  @retval EFI_SUCCESS        AL	00h = Function succeeded.
  @return Others             AL	Returned error code.
**/
EFI_STATUS
KernelVatsNext (
  VOID
  )
{
  IHISI_VATS_INPUT_BUFFER                *InBuffer;
  CHAR16                                 *VarName;
  UINTN                                  NameSize;
  EFI_STATUS                             Status;

  //
  // DS:ESI  Pointer to the beginning of the VATS input buffer.
  // DS:ECX  Next Variable Name buffer size
  //
  InBuffer  = (IHISI_VATS_INPUT_BUFFER *)(UINTN) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RSI);
  NameSize  = (UINT32) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RCX);
  VarName   = (CHAR16 *) ((UINTN) InBuffer + (sizeof (IHISI_VATS_INPUT_BUFFER) / sizeof (UINT8)));
  Status = EFI_UNSUPPORTED;
  if (mSmmVariable != NULL) {
    Status = mSmmVariable->SmmGetNextVariableName(
                             &NameSize,
                             VarName,
                             &InBuffer->VarGuid
                             );

    if (Status == EFI_SUCCESS || Status == EFI_BUFFER_TOO_SMALL) {
      IhisiProtWriteCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RCX, (UINT32)NameSize);
    }
  }

  return Status;
}

EFI_STATUS
InstallVatsServices (
  VOID
  )
{
  EFI_STATUS            Status;
  IHISI_REGISTER_TABLE   *SubFuncTable;
  UINT16                  TableCount;

  SubFuncTable = VATS_REGISTER_TABLE;
  TableCount = sizeof(VATS_REGISTER_TABLE)/sizeof(VATS_REGISTER_TABLE[0]);
  Status = RegisterIhisiSubFunction (SubFuncTable, TableCount);
  if (EFI_ERROR(Status)) {
    ASSERT_EFI_ERROR (Status);
  }

  return EFI_SUCCESS;
}

