/** @file
  This file offers some oem service for ApcbTokensUpdate
;******************************************************************************
;* Copyright (c) 2022, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/
#include <Uefi.h>
#include <Base.h>

#include <Protocol/AmdApcbProtocol.h>
#include <Protocol/CbsBctSmmProtocol.h>

#include <Library/PcdLib.h>
#include <Library/DebugLib.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>

#include <Guid/AmdCbsConfig.h>
//[-start-221013-IB14860855-modify]//
//#include <Features/AODv2/Include/AodSetup.h>
//[-end-221013-IB14860855-modify]//

#include <H2OIhisi.h>
#include "ApcbTokensUpdate.h"

extern H2O_IHISI_PROTOCOL               *mH2OIhisi;

EFI_STATUS
AodUpdateApcbTokens (
  VOID *AodVariable,
  VOID *vApcbProtocol
  );

/**
  Check if the process is allowed to execute in current security level.

  @retval TRUE           Allowed to execute in current security level.
  @retval FALSE          NOT allowed to execute in current security level.
**/
STATIC
BOOLEAN
CheckSecurityLevelAllowed (
  VOID
  )
{
  EFI_STATUS     Status;
  UINT32         SecurityLevelsSupported;
  UINT32         CurrentSecurityLevel;

  Status = mH2OIhisi->GetAuthStatus (&SecurityLevelsSupported, &CurrentSecurityLevel);
  if (Status != EFI_SUCCESS) {
    return FALSE;
  }

  return ((PcdGet32 (PcdH2oUveSecurityLevelAllowed) & CurrentSecurityLevel) != 0) ? TRUE : FALSE;
}

/**
  Internal function to check Whole ASCII String(include the null terminate) is located in command buffer

  @retval TRUE        Whole String is located in coomand buffer.
  @return FALSE       Any byte of the String in not located in command buffer.
**/
STATIC
BOOLEAN
AsciiStringInCmdBuffer (
  IN  CHAR8      *String
  )
{
  UINTN           Index;

  for (Index = 0; mH2OIhisi->BufferInCmdBuffer ((VOID *) (String + Index), sizeof (CHAR8)); Index++) {
    if (String[Index] == 0) {
      return TRUE;
    }
  }

  return FALSE;
}

/**
  Check for buffer-overlap when two buffers are passed in.

  @param[in] Buffer1       The pointer to the buffer1 to be checked.
  @param[in] Buffer1Size   The size in bytes of the input buffer1
  @param[in] Buffer2       The pointer to the buffer2 to be checked.
  @param[in] Buffer2Size   The size in bytes of the input buffer2

  @retval  TURE        The buffers overlap.
  @retval  FALSE       The buffers doesn't overlap.

**/
BOOLEAN
EFIAPI
BuffersOverlap (
  IN VOID           *Buffer1,
  IN UINT32         Buffer1Size,
  IN VOID           *Buffer2,
  IN UINT32         Buffer2Size
  )
{
  return (Buffer1Size != 0 && Buffer2Size != 0 && ((UINTN)Buffer1 + Buffer1Size) > (UINTN)Buffer2 && (UINTN)Buffer1 < ((UINTN)Buffer2 + Buffer2Size));
}

/**
  Get callback list Size.

  @param[in] pCallbackListBuffer     A pointer to the buffer of callback list.
  @param[OUT] Size                   The size of callbacklist.

  @retval EFI_SUCCESS             
  @retval EFI_INVALID_PARAMETER      pCallbackListBuffer or Size is null.
  @retval EFI_BAD_BUFFER_SIZE        Buffer lies outside the command buffer.

**/
STATIC 
EFI_STATUS 
GetCallBackListSize (
  IN  VATS_CALLBACK_LIST_STRUCTURE  *pCallbackListBuffer,
  OUT UINT32                        *Size
  )
{

  CALLBACK_LIST_CALL_BACK_DATA *CallBackData;
  UINT32 Offset;
  UINT32 Index;
  UINT32 StrLength;

  if ((pCallbackListBuffer == NULL) || (Size == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  if (!mH2OIhisi->BufferInCmdBuffer ((VOID *) pCallbackListBuffer, sizeof (pCallbackListBuffer->ListCount))) {
    DEBUG ((DEBUG_ERROR, "[%a:%d]\n", __FUNCTION__, __LINE__));
    return EFI_BAD_BUFFER_SIZE;
  }

  CallBackData = NULL;
  Offset = 0;
  Index = 0;
  StrLength = 0;

  for (Index = 0; Index < pCallbackListBuffer->ListCount; ++Index) {
    CallBackData = (CALLBACK_LIST_CALL_BACK_DATA *) (Offset + (UINTN)pCallbackListBuffer->CallbackData);
    if (!mH2OIhisi->BufferInCmdBuffer ((VOID *) CallBackData, sizeof(CALLBACK_LIST_CALL_BACK_DATA)) || !AsciiStringInCmdBuffer (CallBackData->CallbackString)) {
      DEBUG ((DEBUG_ERROR, "[%a:%d]\n", __FUNCTION__, __LINE__));
      return EFI_BAD_BUFFER_SIZE;
    }
    StrLength = (UINT32)AsciiStrnLenS ((CONST CHAR8 *) CallBackData->CallbackString, STRING_COUNT_MAX);
    if (StrLength == STRING_COUNT_MAX) {
      DEBUG ((DEBUG_ERROR, "Warning!! The Length of StrData is incorrect in CallBackData.(Callback ID:0x%x , String Length: 0x%x)\n", CallBackData->CallbackID, StrLength));
    }
    Offset += (UINT32)sizeof(CALLBACK_LIST_CALL_BACK_DATA) + StrLength;
  }
  *Size = Offset + sizeof(UINT32);

  return EFI_SUCCESS;
}

/**
  Callback function for the modifying of APCB tokens.

  @retval EFI_SUCCESS
  @retval EFI_INVALID_PARAMETER          Rsi/Rcx is 0, variable name or GUID is mismatched with variable.

**/
EFI_STATUS
EFIAPI
ApcbSyncCallback (
  VOID
)
{
  UINT64                                Rcx;
  UINT64                                Rsi;
  UINT64                                Rdi;
  UINTN                                 Index;
  VOID                                  *SetupNVData;
  VATS_CALLBACK_LIST_STRUCTURE          *CallbackList;
  CALLBACK_LIST_CALL_BACK_DATA          *CallbackData;
  VATS_RESULT_LIST_STRUCTURE            *ResultArray;
  UINTN                                 Offset;
  EFI_STATUS                            Status;
  CHAR16                                Buffer[STRING_COUNT_MAX];
  VATS_VARIABLE_BUFFER_STRUCTURE        *VarBuffer;
  BOOLEAN                               IsLoadDefault;
  UINT32                                Size;
  UINTN                                 StrLength;
  CBS_BCT_SMM_PROTOCOL                  *CbsBctSmmProtocol;
  AMD_APCB_SERVICE_PROTOCOL             *ApcbSmmServiceProtocol;

  if (FeaturePcdGet (PcdH2OIhisiAuthSupported)) {
    if (!CheckSecurityLevelAllowed ()) {
      return IHISI_ACCESS_PROHIBITED;
    }
  }

  Rcx = (UINT64)(UINTN) mH2OIhisi->ReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RCX);
  Rsi = (UINT64)(UINTN) mH2OIhisi->ReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RSI);
  Rdi = (UINT64)(UINTN) mH2OIhisi->ReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RDI);
  
  ZeroMem (Buffer, sizeof (CHAR16) * STRING_COUNT_MAX);

  Index = 0;
  SetupNVData  = NULL;
  CallbackList = NULL;
  CallbackData = NULL;
  ResultArray  = NULL;
  Offset = 0;
  Status = 0;
  VarBuffer = NULL;
  IsLoadDefault = FALSE;
  Size = 0;
  StrLength = 0;

  if ((Rsi == 0) || (Rdi == 0) || (Rcx == 0)) {
    return EFI_INVALID_PARAMETER;
  }

  VarBuffer    = (VATS_VARIABLE_BUFFER_STRUCTURE *) (UINTN) Rsi;
  CallbackList = (VATS_CALLBACK_LIST_STRUCTURE *) (UINTN) Rdi;
  ResultArray  = (VATS_RESULT_LIST_STRUCTURE *) (UINTN) Rcx;

  //
  // Check the DataSize is actually in the Command Buffer.
  //
  if (!mH2OIhisi->BufferInCmdBuffer ((VOID *) &(VarBuffer->DataSize), sizeof (UINT32))) {
    DEBUG ((DEBUG_ERROR, "[%a:%d]\n", __FUNCTION__, __LINE__));
    return IHISI_BUFFER_RANGE_ERROR;
  }

  //
  // Check the VariableNameLength is actually in the Command Buffer.
  //
  if (!mH2OIhisi->BufferInCmdBuffer ((VOID *) &(VarBuffer->VariableNameLength), sizeof (UINT32))) {
    DEBUG ((DEBUG_ERROR, "[%a:%d]\n", __FUNCTION__, __LINE__));
    return IHISI_BUFFER_RANGE_ERROR;
  }

  //
  // Check variable Buffer
  //
  if (!mH2OIhisi->BufferInCmdBuffer ((VOID *) VarBuffer, sizeof (VATS_VARIABLE_BUFFER_STRUCTURE) + VarBuffer->VariableNameLength * sizeof (CHAR16) + VarBuffer->DataSize)) {
    DEBUG ((DEBUG_ERROR, "[%a:%d]\n", __FUNCTION__, __LINE__));
    return IHISI_BUFFER_RANGE_ERROR;
  }

  //
  // Check Callback List
  //
  Status = GetCallBackListSize (CallbackList, &Size);
  if (EFI_ERROR(Status)) {
    if (Status == EFI_BAD_BUFFER_SIZE) {
      Status = IHISI_BUFFER_RANGE_ERROR;
    }
    return Status;
  }
  if (!mH2OIhisi->BufferInCmdBuffer ((VOID *) CallbackList, Size)) {
    DEBUG ((DEBUG_ERROR, "[%a:%d]\n", __FUNCTION__, __LINE__));
    return IHISI_BUFFER_RANGE_ERROR;
  }

  //
  // Check the ListCount is actually in the Command Buffer.
  //
  if (!mH2OIhisi->BufferInCmdBuffer ((VOID *) &(CallbackList->ListCount), sizeof (UINT32))) {
    DEBUG ((DEBUG_ERROR, "[%a:%d]\n", __FUNCTION__, __LINE__));
    return IHISI_BUFFER_RANGE_ERROR;
  }
  //
  // Check Result Array
  //
  if (!mH2OIhisi->BufferInCmdBuffer ((VOID *) ResultArray, RESULT_SIZE(CallbackList->ListCount))) {
    DEBUG ((DEBUG_ERROR, "[%a:%d]\n", __FUNCTION__, __LINE__));
    return IHISI_BUFFER_RANGE_ERROR;
  }

  //
  // Check for buffer-overlap
  //
  if (BuffersOverlap ((VOID *) ResultArray, RESULT_SIZE(CallbackList->ListCount), (VOID *) VarBuffer, sizeof (VATS_VARIABLE_BUFFER_STRUCTURE) + VarBuffer->VariableNameLength * sizeof (CHAR16) + VarBuffer->DataSize)) {
    DEBUG ((DEBUG_ERROR, "[%a:%d]\n", __FUNCTION__, __LINE__));
    return IHISI_BUFFER_RANGE_ERROR;
  }
  if (BuffersOverlap ((VOID *) ResultArray, RESULT_SIZE(CallbackList->ListCount), (VOID *) CallbackList, Size)) {
    DEBUG ((DEBUG_ERROR, "[%a:%d]\n", __FUNCTION__, __LINE__));
    return IHISI_BUFFER_RANGE_ERROR;
  }
  if (BuffersOverlap ((VOID *) VarBuffer, sizeof (VATS_VARIABLE_BUFFER_STRUCTURE) + VarBuffer->VariableNameLength * sizeof (CHAR16) + VarBuffer->DataSize, (VOID *) CallbackList, Size)) {
    DEBUG ((DEBUG_ERROR, "[%a:%d]\n", __FUNCTION__, __LINE__));
    return IHISI_BUFFER_RANGE_ERROR;
  }

  //
  // Check the content of variable Buffer
  // 
  if ((CompareGuid (&VarBuffer->VariableGuid, &gCbsSystemConfigurationGuid) == TRUE) &&
     (StrnCmp (&VarBuffer->VariableName[0], CBS_SYSTEM_CONFIGURATION_NAME, VarBuffer->VariableNameLength) == 0)) {
    
    SetupNVData  = VARIABLE_BUFFER_TO_DATA (VarBuffer);
  
    Status = gSmst->SmmLocateProtocol (
               &gCbsBctSmmProtocolGuid,
               NULL,
               (VOID**)&CbsBctSmmProtocol
               );
    if (EFI_ERROR (Status)) {
      return IHISI_UNSUPPORTED_FUNCTION;
    }
    Status = CbsBctSmmProtocol->CbsBctSetVariable (CbsBctSmmProtocol, SetupNVData);
    return IHISI_END_FUNCTION_CHAIN;
  }

  if ((CompareGuid (&VarBuffer->VariableGuid, &gAodSystemConfigurationGuid) == TRUE) &&
     (StrnCmp (&VarBuffer->VariableName[0], AOD_SETUP_VARIABLE_NAME, VarBuffer->VariableNameLength) == 0)) {
    
    SetupNVData  = VARIABLE_BUFFER_TO_DATA (VarBuffer);
  
    Status = gSmst->SmmLocateProtocol (
               &gAmdApcbSmmServiceProtocolGuid,
               NULL,
               (VOID**)&ApcbSmmServiceProtocol
               );
    if (EFI_ERROR (Status)) {
      return IHISI_UNSUPPORTED_FUNCTION;
    }
//[-start-221013-IB14860855-remove]//
    // This platform not support AOD
//    Status = AodUpdateApcbTokens (SetupNVData, ApcbSmmServiceProtocol);
//[-end-221013-IB14860855-remove]//
    return IHISI_END_FUNCTION_CHAIN;
  }

  return IHISI_SUCCESS;
}

