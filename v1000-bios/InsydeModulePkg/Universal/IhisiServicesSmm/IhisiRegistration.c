/** @file
  Runtime DXE driver implementation for the OemSvc Registration

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
#include "IhisiRegistration.h"
#include "IhisiServicesSmm.h"
#include "IhisiFbts.h"
#include "IhisiVats.h"
#include "SecureFlash.h"
#include "IhisiCheckBuffer.h"
#include "IhisiOemExtraDataCommunication.h"
#include <SmiTable.h>

#include <Library/UefiBootServicesTableLib.h>
#include <Library/SeamlessRecoveryLib.h>
#include <Library/LockBoxLib.h>

#include <Protocol/SmmCpu.h>
#include <Protocol/SmmAccess2.h>
#include <Protocol/SmmSwDispatch2.h>
#include <Protocol/SmmExitBootServices.h>

UINTN                                   mSmramRangeCount;
EFI_SMM_VARIABLE_PROTOCOL              *mSmmVariable;
EFI_SMRAM_DESCRIPTOR                   *mSmramRanges;
IHISI_CONTEXT                          *mIhisiContext = NULL;
EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL      *mSmmFwBlockService;
EFI_HANDLE                             mIhisiHandle = NULL;
BOOLEAN                                mCapsuleUpdate = FALSE;
BOOLEAN                                mRuntimeDisableIhisi = FALSE;
EFI_PHYSICAL_ADDRESS                   mIhisiCmdBuffer;
UINTN                                  mIhisiCmdBufferSize;
EFI_PHYSICAL_ADDRESS                   mIhisiImageBuffer;
UINTN                                  mIhisiImageBufferSize;
extern AP_COMMUNICATION_DATA_TABLE     mApCommDataBuffer;
STATIC CONST  BOOLEAN                  mPcdIhisiRegisterTableValid = (BOOLEAN)(FixedPcdGetPtrSize (PcdIhisiRegisterTable) % sizeof (PCD_IHISI_REGISTER_TABLE) == 0);
STATIC IHISI_SMI                       mForbiddenCmdInAuthNone [] = {
                                         DATSWrite,
                                         DATSErase,
                                         FBTSWrite,
                                         FBTSComplete,
                                         FBTSSkipMcCheckAndBinaryTrans,
                                         FETSWrite,
                                         FMTSWrite,
                                         OEMSFOEMExCommunication,
                                         OEMSFOEMExDataWrite,
                                         FBTSCommonWrite,
                                         FBTSWriteToSPIRom
                                        };
BOOLEAN                                mPurifyVariable = FALSE;

/**
  Allocate IHISI memory buffer to communicate information with tools.

  @retval   EFI_SUCCESS     IHISI memory buffer is allocated.
  @retval   Others          Allocate IHISI memory buffer failed.
**/
EFI_STATUS
AllocateIhisiBuffer (
  VOID
  )
{
  UINTN           CmdBufferSize;
  UINTN           ImageBufferSize;
  EFI_STATUS      Status;

  if (FeaturePcdGet(PcdH2OIhisiCmdBufferSupported)) {
    CmdBufferSize = PcdGet32 (PcdH2OIhisiCmdBufferSize);
    if (CmdBufferSize != 0) {
      Status = gBS->AllocatePool (
                      EfiReservedMemoryType,
                      CmdBufferSize,
                      (VOID **) (UINTN) (&mIhisiCmdBuffer)
                      );

      if (EFI_ERROR (Status)) {
        return Status;
      }
      mIhisiCmdBufferSize = CmdBufferSize;
    }
  }

  if (FeaturePcdGet(PcdH2OIhisiImageBufferSupported)) {
    ImageBufferSize = PcdGet32 (PcdH2OIhisiImageBufferSize);
    if (ImageBufferSize == 0) {
      //
      // TODO: Need add code to auto detect image buffer size because PcdH2OIhisiImageBufferSize
      //       is 0 indicate the image buffer should be auto detected at runtime.
      //
    }
    Status = gBS->AllocatePool (
                    EfiReservedMemoryType,
                    ImageBufferSize,
                    (VOID **) (UINTN)  (&mIhisiImageBuffer)
                    );

    if (EFI_ERROR (Status)) {
      return Status;
    }
    mIhisiImageBufferSize = ImageBufferSize;
  }

  return EFI_SUCCESS;
}
/**
  The notification of gEfiSmmFwBlockServiceProtocolGuid protocol is installed

  @param[in] Protocol      Points to the protocol's unique identifier.
  @param[in] Interface     Points to the interface instance.
  @param[in] Handle        The handle on which the interface was installed.

  @retval EFI_SUCCESS      Locate gEfiSmmVariableProtocolGuid protocol successful.
  @retval EFI_NOT_FOUND    Cannot find gEfiSmmVariableProtocolGuid instance.
**/
STATIC
EFI_STATUS
SmmFwBlockNotify (
  IN     CONST EFI_GUID                *Protocol,
  IN     VOID                          *Interface,
  IN     EFI_HANDLE                     Handle
  )
{
  EFI_STATUS            Status;

  Status = gSmst->SmmLocateProtocol (
                    &gEfiSmmFwBlockServiceProtocolGuid,
                    NULL,
                    (VOID **) &mSmmFwBlockService
                    );

  if (EFI_ERROR(Status)) {
    ASSERT_EFI_ERROR (Status);
  }
  return Status;
}

/**
  The notification of gEfiSmmVariableProtocolGuid protocol is installed

  @param[in] Protocol      Points to the protocol's unique identifier.
  @param[in] Interface     Points to the interface instance.
  @param[in] Handle        The handle on which the interface was installed.

  @retval EFI_SUCCESS      Locate gEfiSmmVariableProtocolGuid protocol successful.
  @retval EFI_NOT_FOUND    Cannot find gEfiSmmVariableProtocolGuid instance.
**/
STATIC
EFI_STATUS
SmmVariableNotify (
  IN     CONST EFI_GUID                *Protocol,
  IN     VOID                          *Interface,
  IN     EFI_HANDLE                     Handle
  )
{
  return gSmst->SmmLocateProtocol (
                  &gEfiSmmVariableProtocolGuid,
                  NULL,
                  (VOID **) &mSmmVariable
                  );
}

UINT32
GetOtherIhisiStatus (
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

  case EFI_INVALID_PARAMETER:
    return (UINT32) IHISI_INVALID_PARAMETER;
    break;

  case EFI_UNSUPPORTED:
  case EFI_NOT_FOUND:
    return (UINT32) IHISI_UNSUPPORTED_FUNCTION;
	break;

  case EFI_OUT_OF_RESOURCES:
    return (UINT32) IHISI_OUT_OF_RESOURCES;
	break;

  default:
    return (UINT32) IHISI_ACCESS_PROHIBITED;
    break;
  }
}

/**
  Get IHISI status.translated from EFI status

  @param[in] Status  EFI_STATUS

  @return UINT32     IHISI status
**/
UINT32
EfiStatusToIhisiStatus (
  UINT32        FuncType,
  EFI_STATUS    Status
  )
{
  UINT32        IhisiStatus;

  IhisiStatus = (UINT32)Status;

  switch (FuncType) {
    case VATSRead:
    case VATSWrite:
    case VATSNext:
      IhisiStatus = GetVatsIhisiStatus(Status);
      break;

    case FBTSRead:
      if (Status == EFI_ACCESS_DENIED) {
        IhisiStatus = IHISI_FBTS_ME_LOCK_READ_FAILED;
        break;
      }

    case FBTSWrite:
      if (Status == EFI_WRITE_PROTECTED) {
        IhisiStatus = IHISI_FBTS_SKIP_THIS_WRITE_BLOCK;
        break;
      }
    case FBTSGetSupportVersion:
    case FBTSGetPlatformInfo:
    case FBTSGetPlatformRomMap:
    case FBTSGetFlashPartInfo:
    case FBTSComplete:
    case FBTSGetRomFileAndPlatformTable:
    case FBTSSecureFlashBIOS:
    case FBTSOemCustomization1:
    case FBTSOemCustomization2:
    case FBTSSkipMcCheckAndBinaryTrans:
    case FBTSGetATpInformation:
    case FBTSPassPlatforminiSettings:
    case FBTSGetWholeBiosRomMap:
    case FBTSApHookPoint:
    case FBTSOEMCapsuleSecureFlash:
    case FBTSPassImageFromTool:
    case FBTSGetRuntimeBuffer:
    case FBTSPassImagetoBios:
    case FBTSWriteToSPIRom:
    case FBTSCommonCommunication:
    case FBTSCommonWrite:
    case FBTSCommonRead:
      IhisiStatus = GetFbtsIhisiStatus(Status);
      break;

    case FETSWrite:
    case FETSGetEcPartInfo:
    case FETSRead:
      IhisiStatus = GetFbtsIhisiStatus(Status);
      break;

    default :
      IhisiStatus = GetOtherIhisiStatus(Status);
      break;
  }
  return IhisiStatus;
}

/**
  Returned error code in AL.

  @param[in] IhisiStatus  Returned error code in AL.
**/
VOID
IhisiErrorCodeHandler (
  IN  UINT32            IhisiStatus
  )
{
  UINT32            Eax;

  Eax = IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RAX);
  Eax = (UINT32) ((Eax & 0xffffff00) | IhisiStatus);
  IhisiProtWriteCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RAX, Eax);
  return;
}

VOID
UpdateApRequestFlag (
  UINT8                         ApRequest
  )
{
  UINT32                        Ecx;

  Ecx = (IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RCX) & 0xffffff00) | ApRequest;
  IhisiProtWriteCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RCX, Ecx);
}

BOOLEAN
IsSecureFlashFlagEnabled (
  VOID
  )
{
  UINT8                         SecureFlashFlag;
  UINTN                         Size;
  EFI_STATUS                    Status;

  SecureFlashFlag = FALSE;
  Size = sizeof (SecureFlashFlag);
  Status = RestoreLockBox (&gSecureFlashInfoGuid, &SecureFlashFlag, &Size);
  if (EFI_ERROR (Status)) {
    return FALSE;
  }
  return (BOOLEAN) (SecureFlashFlag == FLASH_COMPLETE_IN_RECOVERY);
}

/**
  Update the purify variable flag according to gSecureFlashInfoGuid lock box data.
**/
STATIC
VOID
UpdatePurifyVariableFlag (
  VOID
  )
{
  UINT8                         SecureFlashFlag;
  UINTN                         Size;
  EFI_STATUS                    Status;

  mPurifyVariable = TRUE;
  SecureFlashFlag = 0;
  Size = sizeof (SecureFlashFlag);
  Status = RestoreLockBox (&gSecureFlashInfoGuid, &SecureFlashFlag, &Size);
  if (!EFI_ERROR (Status) && (BOOLEAN) (SecureFlashFlag == RESET_IN_RECOVERY)) {
    mPurifyVariable = FALSE;
  }
}

VOID
RecoveryUpdateCheck (
  VOID
  )
{
  UINT8                         RequestAction;

  if (IsSecureFlashFlagEnabled ()) {
    RequestAction = (UINT8) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RCX);
    UpdateLockBox (&gSecureFlashInfoGuid, 0, &RequestAction, sizeof (RequestAction));
    UpdateApRequestFlag (0x00);
  }
}

VOID
CapsuleUpdateCheck (
  VOID
  )
{

  if (mCapsuleUpdate) {
    //
    // Force override CL register as 0x00 to avoid system do reset behavior during Windows Capsule Update
    // It will cause ESRT of LastAttemptStatus field not be updated completely.
    //
    UpdateApRequestFlag (0x00);
  }
}

VOID
SpecialCaseFunHook(
  UINT32       Cmd
  )
{
  if (!IsSecureFlashFlagEnabled () && IsFirmwareFailureRecovery ()) {
    //
    // Check the PcdSeamlessRecoverySignature GUID have been existed on FtwSpareBase during CapsuleUpdate
    //
    mCapsuleUpdate = TRUE;
  }

  switch (Cmd) {

  case FBTSComplete:
    UpdatePurifyVariableFlag ();
    if (PcdGetBool (PcdSecureFlashSupported)) {
      //
      // If the firmware update is initiated by capsule update, IHISI FBTS Complete should not
      // reset or shutdown the system, otherwise the ESRT status won't get updated
      //
      CapsuleUpdateCheck ();

      //
      // In Crisis recovery process, IHISI FBTS complete should not reset or shutdown the system
      //
      RecoveryUpdateCheck ();
    }

  break;

    default:
      break;
  }
  return;
}

IHISI_COMMAND_ENTRY *
IhisiFindCommandEntry (
  UINT32         CmdNumber
  )
{
  IHISI_COMMAND_ENTRY          *Node;
  LIST_ENTRY                   *Link;

  Link = GetFirstNode (&mIhisiContext->CommandList);
  while (!IsNull (&mIhisiContext->CommandList, Link)) {
    Node = IHISI_COMMAND_ENTRY_FROM_LINK (Link);
    if (Node->CmdNumber == CmdNumber) {
      return Node;
    }
    Link = GetNextNode (&mIhisiContext->CommandList, Link);
  }
  return NULL;
}

/**
  Internal function to chekc if platform supports input IHISI command.

  @param[in] CmdNumber        Command code of the IHISI function

  @retval    TRUE             Platform supports this IHISI command
  @retval    FALSE            Platform doens't support this IHISI command
**/
STATIC
BOOLEAN
IhsiCommandSupported (
  IN UINT32                              CmdNumber
  )
{
  if (!FeaturePcdGet (PcdH2OIhisiVatsSupported)) {
    if (CmdNumber == VATSRead || CmdNumber == VATSWrite || CmdNumber == VATSNext) {
      return FALSE;
    }
  }

  if (!FeaturePcdGet (PcdH2OIhisiFbtsSupported)) {
    if (CmdNumber >= FBTSGetSupportVersion &&  CmdNumber <= FBTSApHookPoint) {
      return FALSE;
    }
  }
  if (!FeaturePcdGet (PcdH2OIhisiFbts2Supported)) {
   if (CmdNumber >= FBTSOEMCapsuleSecureFlash && CmdNumber <= FBTSWriteToSPIRom) {
      return FALSE;
    }
  }

  if (!FeaturePcdGet (PcdH2OIhisiFetsSupported)) {
    if (CmdNumber == FETSWrite || CmdNumber == FETSGetEcPartInfo || CmdNumber == FETSRead) {
      return FALSE;
    }
  }
  if (!FeaturePcdGet (PcdH2OIhisiFmtsSupported)) {
    if (CmdNumber == FMTSGetMEPartInfo || CmdNumber == FMTSWrite) {
      return FALSE;
    }
  }

  if (!FeaturePcdGet (PcdH2OIhisiDatsSupported)) {
    if (CmdNumber == DATSWrite || CmdNumber == DATSErase || CmdNumber == DATSQuery) {
      return FALSE;
    }
  }
  if (!FeaturePcdGet (PcdH2OIhisiAuthSupported)) {
    if (CmdNumber == IhisiAuthStatus || CmdNumber == IhisiAuthLock || CmdNumber == IhisiAuthUnlock || CmdNumber == IhisiAuth) {
      return FALSE;
    }
  }
  return TRUE;
}

EFI_STATUS
EFIAPI
IhisiProtRegisterCommand (
  IN UINT32                              CmdNumber,
  IN IHISI_FUNCTION                      IhisiFunction,
  IN UINT8                               Priority
  )
{
  LIST_ENTRY                 *Link;
  IHISI_COMMAND_ENTRY        *CmdNode;
  IHISI_FUNCTION_ENTRY       *FunctionNode;
  IHISI_FUNCTION_ENTRY       *FunctionEntry;

  if (!IhsiCommandSupported (CmdNumber)) {
    return EFI_UNSUPPORTED;
  }

  CmdNode = IhisiFindCommandEntry (CmdNumber);
  if (CmdNode == NULL) {
    CmdNode = AllocatePool (sizeof (IHISI_COMMAND_ENTRY));
    if (CmdNode == NULL) {
      ASSERT (CmdNode != NULL);
      return EFI_OUT_OF_RESOURCES;
    }
    CmdNode->Signature = IHISI_SIGNATURE;
    CmdNode->CmdNumber = CmdNumber;
    InitializeListHead (&CmdNode->FunctionChain);
    InsertTailList (&mIhisiContext->CommandList, &CmdNode->Link);
  }

  FunctionEntry = AllocatePool (sizeof(IHISI_FUNCTION_ENTRY));
  if (FunctionEntry == NULL) {
    ASSERT (FunctionEntry != NULL);
    return EFI_OUT_OF_RESOURCES;
  }
  FunctionEntry->Signature = IHISI_SIGNATURE;
  FunctionEntry->Function = IhisiFunction;
  FunctionEntry->Priority = Priority;

  Link = GetFirstNode (&CmdNode->FunctionChain);
  while (!IsNull (&CmdNode->FunctionChain, Link)) {
    FunctionNode = IHISI_FUNCTION_ENTRY_FROM_LINK (Link);
    if (FunctionNode->Priority == Priority) {
      FreePool (FunctionEntry);
      return EFI_ALREADY_STARTED;
    }
    if (FunctionNode->Priority < Priority) {
      InsertTailList (Link, &FunctionEntry->Link);
      return EFI_SUCCESS;
    }
    Link = GetNextNode (&CmdNode->FunctionChain, Link);
  }
  InsertTailList (&CmdNode->FunctionChain, &FunctionEntry->Link);

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
IhisiProtRemoveFunctions (
  UINT32         CmdNumber,
  UINT8          FromPriority,
  UINT8          ToPriority
  )
{
  LIST_ENTRY                   *Link;
  LIST_ENTRY                   *NextLink;
  IHISI_COMMAND_ENTRY          *CmdNode;
  IHISI_FUNCTION_ENTRY         *FunctionNode;

  CmdNode = IhisiFindCommandEntry (CmdNumber);
  if (CmdNode == NULL) {
    return EFI_NOT_FOUND;
  }
  Link = GetFirstNode (&CmdNode->FunctionChain);
  while (!IsNull (&CmdNode->FunctionChain, Link)) {
    NextLink = GetNextNode (&CmdNode->FunctionChain, Link);
    FunctionNode = IHISI_FUNCTION_ENTRY_FROM_LINK (Link);
    if ( (FunctionNode->Priority >= FromPriority) &&
         (FunctionNode->Priority <= ToPriority)) {
      RemoveEntryList (&FunctionNode->Link);
      FreePool (FunctionNode);
    }
    Link = NextLink;
  }
  if (IsListEmpty (&CmdNode->FunctionChain)) {
    RemoveEntryList (&CmdNode->Link);
    FreePool (CmdNode);
  }
  return EFI_SUCCESS;
}


EFI_STATUS
EFIAPI
IhisiProtExecuteCommand (
  UINT32         CmdNumber
  )
{
  return IhisiProtExecuteCommandByPriority (CmdNumber, 0x00, 0xFF);
}


UINT32
EFIAPI
IhisiProtReadCpuReg32 (
  IN  EFI_SMM_SAVE_STATE_REGISTER       RegisterNum
  )
{
  EFI_STATUS      Status;
  UINT32          Value;

  Status = mIhisiContext->SmmCpu->ReadSaveState (
                                    mIhisiContext->SmmCpu,
                                    sizeof (UINT32),
                                    RegisterNum,
                                    mIhisiContext->IhisiCpuIndex,
                                    &Value
                                    );
  ASSERT_EFI_ERROR (Status);
  return Value;
}

EFI_STATUS
EFIAPI
IhisiProtWriteCpuReg32 (
  IN  EFI_SMM_SAVE_STATE_REGISTER       RegisterNum,
  IN  UINT32                            Value
  )
{
  EFI_STATUS      Status;

  Status = mIhisiContext->SmmCpu->WriteSaveState (
                                    mIhisiContext->SmmCpu,
                                    sizeof (UINT32),
                                    RegisterNum,
                                    mIhisiContext->IhisiCpuIndex,
                                    &Value
                                    );
  ASSERT_EFI_ERROR (Status);
  return Status;
}

/**
  Check the input memory buffer is whether overlap the SMRAM ranges.

  @param[in] Buffer       The pointer to the buffer to be checked.
  @param[in] BufferSize   The size in bytes of the input buffer

  @retval  TURE        The buffer overlaps SMRAM ranges.
  @retval  FALSE       The buffer doesn't overlap SMRAM ranges.
**/
BOOLEAN
EFIAPI
IhisiProtBufferOverlapSmram (
  IN VOID              *Buffer,
  IN UINTN              BufferSize
  )
{
  UINTN                 Index;
  EFI_PHYSICAL_ADDRESS  BufferEnd;
  EFI_PHYSICAL_ADDRESS  BufferStart;

  if (Buffer == NULL || BufferSize == 0) {
    return FALSE;
  }

  BufferStart = (EFI_PHYSICAL_ADDRESS) (UINTN) Buffer;
  BufferEnd =   (EFI_PHYSICAL_ADDRESS) ((UINTN) Buffer + BufferSize - 1);

  //
  // Integer overflow check
  //
  if (BufferEnd < BufferStart) {
    return TRUE;
  }

  for (Index = 0; Index < mSmramRangeCount; Index ++) {
    //
    // The condition for two ranges doesn't overlap is:
    // Buffer End is smaller than the range start or Buffer start is larger than the range end.
    // so the overlap condition is above condition isn't satisfied.
    //
    if (!(BufferEnd < mSmramRanges[Index].CpuStart ||
        BufferStart >= (mSmramRanges[Index].CpuStart + mSmramRanges[Index].PhysicalSize))) {
      return TRUE;
    }
  }

  return FALSE;
}


/**
  Check the input memory buffer whether lies within the SMRAM .

  @param[in] Buffer       Pointer to the first byte in a buffer.
  @param[in] BufferSize   Unsigned integer that specifies the size of the Buffer, in bytes.

  @retval  TURE        Buffer lies within the SMRAM.
  @retval  FALSE       Buffer lies outside the SMRAM.
**/
STATIC
BOOLEAN
IhisiBufferInSmramBuffer (
  IN VOID              *Buffer,
  IN UINTN              BufferSize
  )
{
  UINTN                 Index;
  EFI_PHYSICAL_ADDRESS  BufferEnd;
  EFI_PHYSICAL_ADDRESS  BufferStart;

  if (Buffer == NULL || BufferSize == 0) {
    return FALSE;
  }
  BufferStart = (EFI_PHYSICAL_ADDRESS) (UINTN) Buffer;
  BufferEnd =   (EFI_PHYSICAL_ADDRESS) ((UINTN) Buffer + BufferSize - 1);

  //
  // Integer overflow check
  //
  if (BufferEnd < BufferStart) {
    return FALSE;
  }

  for (Index = 0; Index < mSmramRangeCount; Index ++) {
    if (BufferStart >= mSmramRanges[Index].CpuStart &&
        BufferEnd < (mSmramRanges[Index].CpuStart + mSmramRanges[Index].PhysicalSize)) {
      return TRUE;
    }
  }

  return FALSE;
}


/**
  Check the input memory buffer whether lies within the command buffer..

  @param[in] Buffer       Pointer to the first byte in a buffer.
  @param[in] BufferSize   Unsigned integer that specifies the size of the Buffer, in bytes.

  @retval  TURE        Buffer lies within the command buffer.
  @retval  FALSE       Buffer lies outside the command buffer.
**/
BOOLEAN
EFIAPI
IhisiProtBufferInCmdBuffer (
  IN VOID              *Buffer,
  IN UINTN              BufferSize
  )
{
  EFI_PHYSICAL_ADDRESS  BufferEnd;
  EFI_PHYSICAL_ADDRESS  BufferStart;


  if (mSharedBufferInSmRam) {
    return IhisiBufferInSmramBuffer (Buffer, BufferSize);
  }


  if (!FeaturePcdGet(PcdH2OIhisiCmdBufferSupported)) {
    return IhisiProtBufferOverlapSmram (Buffer, BufferSize) ? FALSE : TRUE;
  }

  if (Buffer == NULL || BufferSize == 0) {
    return FALSE;
  }
  BufferStart = (EFI_PHYSICAL_ADDRESS) (UINTN) Buffer;
  BufferEnd =   (EFI_PHYSICAL_ADDRESS) ((UINTN) Buffer + BufferSize - 1);

  //
  // Integer overflow check
  //
  if (BufferEnd < BufferStart) {
    return FALSE;
  }

  if (BufferStart >= mIhisiCmdBuffer &&
      BufferEnd < mIhisiCmdBuffer + mIhisiCmdBufferSize) {
    return TRUE;
  }


  return FALSE;
}


/**
  This function returns the current possible security levels and the current security level (if any).

  @param[out]  SecurityLevelsSupported    Pointer to a bitmask that indicates the security levels supported by
                                          IHISI on this platform.
  @param[out]  CurrentSecurityLevel       Pointer to a bitmask that indicates which security level was used when the
                                          IHISI session was opened.

  @retval EFI_SUCCESS                     This function completed successfully.
  @return EFI_INVALID_PARAMETER           SecurityLevelsSupported or CurrentSecurityLevel are NULL.
**/
EFI_STATUS
EFIAPI
IhisiProtGetAuthStatus (
  OUT UINT32                    *SecurityLevelsSupported,
  OUT UINT32                    *CurrentSecurityLevel
  )
{
  if (SecurityLevelsSupported == NULL || CurrentSecurityLevel == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  //
  // TODO: Need add code to determine the current security level in different time. also need
  //       report what security level is supported in different situation.
  //
  *SecurityLevelsSupported = GetSupportedIhisiSecurityLevels ();
  *CurrentSecurityLevel    = GetCurrentIhisiSecurityLevel ();
  return EFI_SUCCESS;
}

/**
  This function gets the command buffer and size in bytes of the command buffer.

  @param[out]  Buffer               The address to return a pointer to the command buffer.
  @param[out]  BufferSize           The size in bytes of the return command buffer.

  @retval EFI_SUCCESS               This function completed successfully.
  @return EFI_INVALID_PARAMETER     Buffer or BufferSize is NULL.
**/
EFI_STATUS
EFIAPI
IhisiProtGetCmdBuffer (
  OUT VOID                      **Buffer,
  OUT UINTN                     *BufferSize
  )
{
  if (Buffer == NULL || BufferSize == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  *Buffer =     (VOID *) (UINTN) mIhisiCmdBuffer;
  *BufferSize = mIhisiCmdBufferSize;
  return EFI_SUCCESS;
}

/**
  This function gets the image buffer and size in bytes of the image buffer.

  @param[out]  Buffer               The address to return a pointer to the image buffer.
  @param[out]  BufferSize           The size in bytes of the return image buffer.

  @retval EFI_SUCCESS               This function completed successfully.
  @return EFI_INVALID_PARAMETER     Buffer or BufferSize is NULL.
**/
EFI_STATUS
EFIAPI
IhisiProtGetImageBuffer (
  OUT VOID                      **Buffer,
  OUT UINTN                     *BufferSize
  )
{
  if (Buffer == NULL || BufferSize == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  *Buffer =     (VOID *) (UINTN) mIhisiImageBuffer;
  *BufferSize = mIhisiImageBufferSize;
  return EFI_SUCCESS;
}

/**
  This function returns whether the bytes from Buffer through Buffer + BufferSize - 1
  lie entirely within the image buffer (TRUE) or whether one or more bytes lie outside the
  image buffer (FALSE).
  If PcdH2OIhisiImageBufferSupported is set to FALSE, then this function will always
  return TRUE.

  @param[in]  Buffer            Pointer to the first byte in a buffer.
  @param[in]  BufferSize        Unsigned integer that specifies the size of the Buffer, in bytes.

  @retval TRUE                  Whole buffer lies within the image buffer.
  @return FALSE                 Any of byte in buffer lies outside the image buffer.
**/
BOOLEAN
EFIAPI
IhisiProtBufferInImageBuffer (
  IN VOID              *Buffer,
  IN UINTN              BufferSize
  )
{
  EFI_PHYSICAL_ADDRESS  BufferEnd;
  EFI_PHYSICAL_ADDRESS  BufferStart;

  if (!FeaturePcdGet(PcdH2OIhisiImageBufferSupported)) {
    return IhisiProtBufferOverlapSmram (Buffer, BufferSize) ? FALSE : TRUE;
  }

  if (Buffer == NULL || BufferSize == 0) {
    return FALSE;
  }
  BufferStart = (EFI_PHYSICAL_ADDRESS) (UINTN) Buffer;
  BufferEnd =   (EFI_PHYSICAL_ADDRESS) ((UINTN) Buffer + BufferSize - 1);

  //
  // Integer overflow check
  //
  if (BufferEnd < BufferStart) {
    return FALSE;
  }

  if (BufferStart >= mIhisiImageBuffer &&
      BufferEnd < mIhisiImageBuffer + mIhisiImageBufferSize) {
    return TRUE;
  }

  return FALSE;
}

/**
  Handle all registered handlers for the specified IHISI command with priorities greater than
  or equal to FromPriority and less than or equal to ToPriority, or until one of the handlers
  returns IHISI_END_FUNCTION_CHAIN.

  Functions ranges from FromPriority to ToPriority (inclusive) in the IHISI command function
  chain will be executed.

  @param[in]  CmdCode            Pointer to the first byte in a buffer.
  @param[in]  FromPriority       Vlaue of From Priority
  @param[in]  ToPriority         Vlaue of To Priority

  @retval TRUE                  Whole buffer lies within the image buffer.
  @return FALSE                 Any of byte in buffer lies outside the image buffer.
**/
EFI_STATUS
EFIAPI
IhisiProtExecuteCommandByPriority (
  IN UINT32         CmdCode,
  IN UINT8          FromPriority,
  IN UINT8          ToPriority
  )
{
  EFI_STATUS                    Status;
  LIST_ENTRY                   *Link;
  IHISI_COMMAND_ENTRY          *CmdNode;
  IHISI_FUNCTION_ENTRY         *FunctionNode;

  CmdNode = IhisiFindCommandEntry (CmdCode);
  if (CmdNode == NULL) {
    return EFI_NOT_FOUND;
  }
  ;
  for (Link = GetFirstNode (&CmdNode->FunctionChain);
       !IsNull (&CmdNode->FunctionChain, Link);
       Link = GetNextNode (&CmdNode->FunctionChain, Link)) {
    FunctionNode = IHISI_FUNCTION_ENTRY_FROM_LINK (Link);
    if (FunctionNode->Priority > ToPriority || FunctionNode->Priority < FromPriority) {
      continue;
    }
    Status = FunctionNode->Function();
    if (Status == IHISI_END_FUNCTION_CHAIN) {
      return EFI_SUCCESS;
    } else if (Status != EFI_SUCCESS) {
      return Status;
    }

    if (CmdCode == OEMSFOEMExCommunication &&
        IhisiProtBufferInCmdBuffer ((VOID *) (UINTN) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RCX), sizeof(AP_COMMUNICATION_DATA_TABLE))) {
      CopyMem(
        (AP_COMMUNICATION_DATA_TABLE*) (UINTN) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RCX),
        &mApCommDataBuffer,
        sizeof (AP_COMMUNICATION_DATA_TABLE)
        );
    }
  }
  return EFI_SUCCESS;
}

/**
  Helper function to check if this is NONE authorization security level function action.

  @param[in] Cmd         IHISI command number.

  @retval TRUE           This is NONE authorization security level function action.
  @retval FALSE          This is not NONE authorization security level function action.
**/
STATIC
BOOLEAN
IsAuthNoneLevelFunctionAction (
  IHISI_SMI                    Cmd
  )
{
  FBTS_FLASH_COMPLETE_STATUS   *FlashCompleteStatus;
  UINT8                         ApRequest;

  if (Cmd != FBTSComplete) {
    return FALSE;
  }

  FlashCompleteStatus = (FBTS_FLASH_COMPLETE_STATUS *)(UINTN) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RSI);
  if ((FlashCompleteStatus->Signature != FLASH_COMPLETE_STATUS_SIGNATURE) ||
      (FlashCompleteStatus->StructureSize != sizeof (FBTS_FLASH_COMPLETE_STATUS))) {
    return FALSE;
  }
  ApRequest           = (UINT8) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RCX);
  if (FlashCompleteStatus->CompleteStatus != ApTerminated || ApRequest != FlashCompleteDoNothing) {
    return FALSE;
  }
  return TRUE;
}



/**
  Helper function to check if this is OEMSFOEMExCommunication normal security level sub-function.

  @param[in] Cmd         IHISI command number.

  @retval TRUE           This is normal security level sub function.
  @retval FALSE          This is not NONE normal security level sub function.
**/
STATIC
BOOLEAN
IsOEMSFOEMExCommunicationNormalSubFunction (
  IHISI_SMI                    Cmd
  )
{
  AP_COMMUNICATION_DATA_TABLE       *ApCommDataBuffer;
  UINT8                              DataType;

  if (Cmd != OEMSFOEMExCommunication) {
    return FALSE;
  }
  ApCommDataBuffer   = (AP_COMMUNICATION_DATA_TABLE*) (UINTN) IhisiProtReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RCX);
  DataType           = ApCommDataBuffer->DataType;

  if (DataType == Oa30ReadWrite     || DataType == Oa30Erase         || DataType == LogoUpdate        ||
      DataType == WriteOa2Marker    || DataType == WriteOa2PublicKey || DataType == EraseOa2Marker    ||
      DataType == EraseOa2PublicKey) {
    return FALSE;
  }
  return TRUE;

}

/**
  Internal function to check the input IHISI command is allowed to execute in current security level.

  @param[in] Cmd         IHISI command number.

  @retval TRUE           This IHISI command is allowed to execute in current security level.
  @retval FALSE          This IHISI command is NOT allowed to execute in current security level.
**/
STATIC
BOOLEAN
CheckCommandBySecurityLevel (
  IHISI_SMI                    Cmd
  )
{
  UINTN          CmdCount;
  UINTN          Index;
  UINT32         SecurityLevel;

  SecurityLevel = GetCurrentIhisiSecurityLevel ();
  if (((UINTN)SecurityLevel & ~((UINTN)IHISI_AUTH_NONE)) != 0) {
    return TRUE;
  }
  CmdCount = sizeof (mForbiddenCmdInAuthNone) / sizeof (IHISI_SMI);
  for (Index = 0; Index < CmdCount; Index++) {
    if (mForbiddenCmdInAuthNone[Index] == Cmd) {
      if (IsOEMSFOEMExCommunicationNormalSubFunction (Cmd) || IsOEMSFOEMExDataWriteNormalSubFunction (Cmd)) {
        return TRUE;
      }
      if (SecurityLevel == IHISI_AUTH_NONE && IsAuthNoneLevelFunctionAction (Cmd)) {
        return TRUE;
      }
      return FALSE;
    }
  }
  return TRUE;
}
/**
  IHISI Services entry function

  @param[in]     DispatchHandle               The unique handle assigned to this handler by SmiHandlerRegister().
  @param[in]     Context                      Points to an optional handler context which was specified when the
                                              handler was registered.
  @param[in,out] CommBuffer                   A pointer to a collection of data in memory that will
                                              be conveyed from a non-SMM environment into an SMM environment.
  @param[in,out] CommBufferSize               The size of the CommBuffer.

  @retval EFI_SUCCESS                         The interrupt was handled and quiesced. No other handlers
                                              should still be called.
  @retval EFI_WARN_INTERRUPT_SOURCE_QUIESCED  The interrupt has been quiesced but other handlers should
                                              still be called.
  @retval EFI_WARN_INTERRUPT_SOURCE_PENDING   The interrupt is still pending and other handlers should still
                                              be called.
  @retval EFI_INTERRUPT_PENDING               The interrupt could not be quiesced.

**/
EFI_STATUS
IhisiServicesCallback (
  IN EFI_HANDLE                 DispatchHandle,
  IN CONST VOID                *Context         OPTIONAL,
  IN OUT VOID                  *CommBuffer      OPTIONAL,
  IN OUT UINTN                 *CommBufferSize  OPTIONAL
  )
{
  EFI_STATUS                    Status;
  UINT32                        Cmd;
  UINTN                         Index;
  UINT32                        IhisiStatus;
  UINT32                        RegisterValue;
  IHISI_COMMAND_ENTRY          *CommandEntry;
  UINT32                       SessionToken;
  UINT32                       RandomNum;

  if (mSmmFwBlockService == NULL) {
    Status = gSmst->SmmLocateProtocol (
                      &gEfiSmmFwBlockServiceProtocolGuid,
                      NULL,
                      (VOID **) &mSmmFwBlockService
                      );
    if (EFI_ERROR (Status)) {
      IhisiErrorCodeHandler ((UINT32)IHISI_ACCESS_PROHIBITED);
      return EFI_NOT_FOUND;
    }
  }

  if (mSmmVariable == NULL) {
    Status = gSmst->SmmLocateProtocol (
                      &gEfiSmmVariableProtocolGuid,
                      NULL,
                      (VOID **) &mSmmVariable
                      );

    if (EFI_ERROR (Status)) {
      IhisiErrorCodeHandler ((UINT32)IHISI_ACCESS_PROHIBITED);
      return EFI_NOT_FOUND;
    }
  }
  //
  // Find out which CPU triggered the S/W SMI
  //
  RegisterValue = 0;
  for (Index = 0; Index < gSmst->NumberOfCpus; Index++) {
    Status = mIhisiContext->SmmCpu->ReadSaveState (
                                      mIhisiContext->SmmCpu,
                                      sizeof (UINT32),
                                      EFI_SMM_SAVE_STATE_REGISTER_RBX,
                                      Index,
                                      &RegisterValue
                                      );
    if ((Status == EFI_SUCCESS) && (RegisterValue == IHISI_EBX_SIGNATURE || (mIhisiLock && RegisterValue == mIhisiSessionToken))) {
      break;
    }
  }

  if (Index == gSmst->NumberOfCpus) {
    IhisiErrorCodeHandler ((UINT32)IHISI_ACCESS_PROHIBITED);
    Status = EFI_NOT_FOUND;
    goto Done;
  }
  SessionToken = RegisterValue;
  if (mRuntimeDisableIhisi == TRUE) {
    IhisiErrorCodeHandler ((UINT32)IHISI_ACCESS_PROHIBITED);
    return EFI_NOT_FOUND;
  }

  mIhisiContext->IhisiCpuIndex = Index;

  Status = mIhisiContext->SmmCpu->ReadSaveState (
                                    mIhisiContext->SmmCpu,
                                    sizeof (UINT32),
                                    EFI_SMM_SAVE_STATE_REGISTER_RAX,
                                    mIhisiContext->IhisiCpuIndex,
                                    &RegisterValue
                                    );
  ASSERT_EFI_ERROR (Status);

  Cmd = (UINTN)((RegisterValue >> 8) & 0xFF);
  Status = CheckSharedBuffer (Cmd);
  if (EFI_ERROR (Status)) {
    IhisiErrorCodeHandler ((UINT32)IHISI_ACCESS_PROHIBITED);
    Status = EFI_NOT_FOUND;
    goto Done;
  }
  Status = StoreSharedBufferToSmram (Cmd);
  if (EFI_ERROR (Status)) {
    IhisiErrorCodeHandler ((UINT32)IHISI_ACCESS_PROHIBITED);
    Status = EFI_NOT_FOUND;
    goto Done;
  }



  if (FeaturePcdGet (PcdH2OIhisiAuthSupported)) {
    if (Cmd != IhisiAuthStatus && Cmd != IhisiAuthLock && Cmd != IhisiAuthUnlock) {
      if (!mIhisiLock) {
        IhisiErrorCodeHandler (IHISI_INTERFACE_CLOSED);
        Status = EFI_SUCCESS;
        goto Done;
      } else if (SessionToken != mIhisiSessionToken) {
        IhisiErrorCodeHandler (IHISI_ACCESS_PROHIBITED);
        Status = EFI_SUCCESS;
        goto Done;
      }
    } else if (Cmd == IhisiAuthStatus && mIhisiLock && SessionToken != mIhisiSessionToken) {
      IhisiErrorCodeHandler (IHISI_ACCESS_PROHIBITED);
      Status = EFI_SUCCESS;
      goto Done;
    }

    if (!CheckCommandBySecurityLevel ((IHISI_SMI)Cmd)) {
      IhisiErrorCodeHandler (IHISI_ACCESS_PROHIBITED);
      Status = EFI_SUCCESS;
      goto Done;
    }
  }

  RandomNum = 0;
  GetRandomNumber32 (&RandomNum);
  CommandEntry = IhisiFindCommandEntry (Cmd);
  if (CommandEntry == NULL) {
    if (FeaturePcdGet (PcdH2OIhisiAuthSupported)) {
      mIhisiSessionToken = RandomNum;
      IhisiProtWriteCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RBX, RandomNum);
    }
    IhisiErrorCodeHandler ((UINT32)IHISI_UNSUPPORTED_FUNCTION);
    Status = EFI_NOT_FOUND;
    goto Done;
  }

  SpecialCaseFunHook(Cmd);
  Status = IhisiProtExecuteCommand (Cmd);
  if ((Status & IHISI_STATUS_BIT) == 00) {
    IhisiStatus = EfiStatusToIhisiStatus (Cmd, Status);
  } else {
    IhisiStatus = (UINT32)Status;
  }

  IhisiErrorCodeHandler (IhisiStatus);
  if (IhisiStatus == IHISI_BUFFER_RANGE_ERROR) {
    mIhisiLock = FALSE;
    mIhisiSessionToken = IHISI_H2O_SIGNATURE;
  //
  // needn't update session token in below cases:
  // 1. IhisiAuthLock (0x81). This is because the session token is already controlled by this function.
  // 2. IhisiAuthUnlock (0x82). After doing unlock, will unlock the system and set session token to default ($H2O).
  // 3. IhisiAuthStatus (0x83). This is because the session token is already controlled by this function.
  //
  } else if (Cmd != IhisiAuthLock && Cmd != IhisiAuthUnlock && Cmd != IhisiAuthStatus) {
    mIhisiSessionToken = RandomNum;
    IhisiProtWriteCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RBX, RandomNum);
  }

Done:
  RestoreSharedBufferFromSmram ();
  return EFI_SUCCESS;
}

/**
  IHISI Protocol installation routine

  @retval EFI_SUCCESS:          IHISI Protocol is successfully installed
  @retval Others                Failed to install IHISI Protocol
**/
EFI_STATUS
InstallIhisiProtocol(
  VOID
  )
{
  EFI_STATUS                            Status;
  EFI_HANDLE                            Handle;
  EFI_SMM_SW_REGISTER_CONTEXT           SwContext;
  EFI_SMM_SW_DISPATCH2_PROTOCOL        *SwDispatch;

  //
  // Software SMI for IHISI services callback function
  //
  Status = gSmst->SmmLocateProtocol (
                    &gEfiSmmSwDispatch2ProtocolGuid,
                    NULL,
                    (VOID **) &SwDispatch
                    );
  if (EFI_ERROR (Status)) {
     ASSERT_EFI_ERROR (Status);
    return Status;
  }

  SwContext.SwSmiInputValue = IHISI_SW_SMI;
  Status = SwDispatch->Register (
                         SwDispatch,
                         IhisiServicesCallback,
                         &SwContext,
                         &mIhisiHandle
                         );
  if (EFI_ERROR (Status)) {
     ASSERT_EFI_ERROR (Status);
     return Status;
  }

  Handle = NULL;
  Status = gSmst->SmmInstallProtocolInterface (
                    &Handle,
                    &gH2OIhisiProtocolGuid,
                    EFI_NATIVE_INTERFACE,
                    &mIhisiContext->Ihisi
                    );
  if (EFI_ERROR (Status)) {
    FreePool (mIhisiContext);
    ASSERT_EFI_ERROR (Status);
    return Status;
  }
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
InitIhisi (
VOID
  )
{
  EFI_STATUS                  Status;
  UINTN                       Size;
  EFI_SMM_ACCESS2_PROTOCOL   *SmmAccess;
  VOID                       *FwBlockRegistration;
  VOID                       *VariableRegistration;

  //
  // Locate SMM FwBlock Protocol
  //
  mSmmFwBlockService = NULL;
  Status = gSmst->SmmLocateProtocol (
                    &gEfiSmmFwBlockServiceProtocolGuid,
                    NULL,
                    (VOID **) &mSmmFwBlockService
                    );

  if (EFI_ERROR (Status)) {
    Status = gSmst->SmmRegisterProtocolNotify (
                      &gEfiSmmFwBlockServiceProtocolGuid,
                      SmmFwBlockNotify,
                      &FwBlockRegistration
                      );
    ASSERT_EFI_ERROR (Status);
  }

  //
  // Locate SMM Variable Protocol
  //
  mSmmVariable = NULL;
  Status = gSmst->SmmLocateProtocol (
                    &gEfiSmmVariableProtocolGuid,
                    NULL,
                    (VOID **) &mSmmVariable
                    );

  if (EFI_ERROR (Status)) {
    Status = gSmst->SmmRegisterProtocolNotify (
                      &gEfiSmmVariableProtocolGuid,
                      SmmVariableNotify,
                      &VariableRegistration
                      );
    ASSERT_EFI_ERROR (Status);
  }

  //
  // Locate SMM Access2 Protocol
  //
  Status = gBS->LocateProtocol (
                  &gEfiSmmAccess2ProtocolGuid,
                  NULL,
                  (VOID **)&SmmAccess
                  );
  if (EFI_ERROR (Status)) {
    ASSERT_EFI_ERROR (Status);
    return Status;
  }

  //
  // Get SMRAM range information
  //
  Size = 0;
  Status = SmmAccess->GetCapabilities (SmmAccess, &Size, NULL);
  ASSERT (Status == EFI_BUFFER_TOO_SMALL);
  mSmramRanges = NULL;
  mSmramRanges = AllocatePool (Size);
  ASSERT (mSmramRanges != NULL);
  if (mSmramRanges == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  Status = SmmAccess->GetCapabilities (SmmAccess, &Size, mSmramRanges);
  ASSERT_EFI_ERROR (Status);

  mSmramRangeCount = Size / sizeof (EFI_SMRAM_DESCRIPTOR);

  //
  //Initialize mIhisiContext
  //
  mIhisiContext = AllocateZeroPool (sizeof (IHISI_CONTEXT));
  if (mIhisiContext == NULL) {
    ASSERT (mIhisiContext != NULL);
    return EFI_OUT_OF_RESOURCES;
  }
  InitializeListHead (&mIhisiContext->CommandList);
  mIhisiContext->Signature                         = IHISI_SIGNATURE;
  mIhisiContext->Ihisi.Size                        = sizeof (H2O_IHISI_PROTOCOL);
  mIhisiContext->Ihisi.RegisterCommand             = IhisiProtRegisterCommand;
  mIhisiContext->Ihisi.RemoveFunctions             = IhisiProtRemoveFunctions;
  mIhisiContext->Ihisi.ExecuteCommand              = IhisiProtExecuteCommand;
  mIhisiContext->Ihisi.ReadCpuReg32                = IhisiProtReadCpuReg32;
  mIhisiContext->Ihisi.WriteCpuReg32               = IhisiProtWriteCpuReg32;
  mIhisiContext->Ihisi.BufferOverlapSmram          = IhisiProtBufferOverlapSmram;
  mIhisiContext->Ihisi.BufferInCmdBuffer           = IhisiProtBufferInCmdBuffer;
  mIhisiContext->Ihisi.GetAuthStatus               = IhisiProtGetAuthStatus;
  mIhisiContext->Ihisi.GetCmdBuffer                = IhisiProtGetCmdBuffer;
  mIhisiContext->Ihisi.GetImageBuffer              = IhisiProtGetImageBuffer;
  mIhisiContext->Ihisi.BufferInImageBuffer         = IhisiProtBufferInImageBuffer;
  mIhisiContext->Ihisi.ExecuteCommandByPriority    = IhisiProtExecuteCommandByPriority;


  Status = gSmst->SmmLocateProtocol (
                    &gEfiSmmCpuProtocolGuid,
                    NULL,
                    (VOID **) &mIhisiContext->SmmCpu
                    );
  if (EFI_ERROR (Status)) {
     FreePool (mIhisiContext);
     ASSERT_EFI_ERROR (Status);
    return Status;
  }
  return EFI_SUCCESS;
}

/**
  Unregister the SMI which used by IHISI interface.

  @retval EFI_SUCCESS:          Locate EFI_SMM_SW_DISPATCH2_PROTOCOL success
  @retval Others                EFI_SMM_SW_DISPATCH2_PROTOCOL not installed
**/

STATIC
EFI_STATUS
EFIAPI
RemoveIhisiServicesCallback (
  IN CONST EFI_GUID                       *Protocol,
  IN VOID                                 *Interface,
  IN EFI_HANDLE                           Handle
  )
{
  if (mIhisiHandle != NULL) {
    mRuntimeDisableIhisi = TRUE;
  }

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
IhisiRuntimeProtect (
  VOID
  )
{
  VOID                *Registration;

  mRuntimeDisableIhisi = FALSE;
  gSmst->SmmRegisterProtocolNotify (
           &gEdkiiSmmExitBootServicesProtocolGuid,
           RemoveIhisiServicesCallback,
           &Registration
           );

  return EFI_SUCCESS;
}

/**
  Helper function to get IHISI function priority.

  @param[in]  Function            Pointer to ihisi register table.
  @param[out] Priority            Pointer to function priority

  @retval EFI_SUCCESS             Get function priority successfully.
  @retval EFI_INVALID_PARAMETER   Function or Priority is NULL.
  @retval EFI_NOT_FOUND           Cannot find function priority in PCD table.

**/
STATIC
EFI_STATUS
GetSubFunctionPriorityFromPcd (
  IN    IHISI_REGISTER_TABLE     *Function,
  OUT   UINT8                     *Priority
  )
{
  UINT8                         *PcdTable;
  UINTN                         PcdOffest;
  UINTN                         PcdSize;
  UINTN                         SignatureSize;

  if (Function == NULL || Priority == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  PcdTable   = (UINT8 *)PcdGetPtr (PcdIhisiRegisterTable);
  PcdSize    = FixedPcdGetPtrSize (PcdIhisiRegisterTable);

  SignatureSize = AsciiStrSize (Function->FuncSignature);
  PcdOffest = 0;
  while (PcdOffest + sizeof (UINT8) + SignatureSize + sizeof (UINT8) <= PcdSize) {
    if (PcdTable[PcdOffest] == Function->CmdNumber &&
        CompareMem (PcdTable + PcdOffest + sizeof (UINT8), Function->FuncSignature, SignatureSize) ==0) {
      *Priority = PcdTable[PcdOffest + sizeof (UINT8) + SignatureSize];
      return EFI_SUCCESS;
    }
    //
    // Get next function in IHISI registered table.
    // NOTE: The FuncSignature size in PcdIhisiRegisterTable isn't always 20 bytes so we
    //       need add extra check to get next function in PcdIhisiRegisterTable.
    //
    PcdOffest ++;
    while (PcdOffest < PcdSize && PcdTable[PcdOffest] != 0) {
      PcdOffest++;
    }
    //
    // Skip end of char in function signature (1 byte) and function priority (1 byte)
    //
    PcdOffest += 2;
  }
  return EFI_NOT_FOUND;
}

/**
  Register IHISI sub function if SubFuncTable CmdNumber/AsciiFuncGuid define in PcdIhisiRegisterTable list.

  @param[out] SubFuncTable        Pointer to ihisi register table.
  @param[out] TableCount          SubFuncTable count

  @retval EFI_SUCCESS        Function succeeded.
  @return Other              Error occurred in this function.
**/
EFI_STATUS
RegisterIhisiSubFunction (
  IHISI_REGISTER_TABLE          *SubFuncTable,
  UINT16                        TableCount
  )
{
  EFI_STATUS                    Status;
  EFI_STATUS                    PriorityStatus;
  UINT8                         PcdPriority;
  UINTN                         Index;

  if (!mPcdIhisiRegisterTableValid) {
    DEBUG ((EFI_D_ERROR, "PcdIhisiRegisterTable of description not follow PCD_IHISI_REGISTER_TABLE definition, \
                          it may cause some of IHISI function register fail \n"));
  }
  Status = EFI_SUCCESS;
  for (Index = 0; Index < TableCount; Index ++) {
    PcdPriority        = 0x80;
    PriorityStatus = GetSubFunctionPriorityFromPcd (&SubFuncTable[Index], &PcdPriority);
    if (EFI_ERROR (PriorityStatus)) {
      continue;
    }
    if (IhsiCommandSupported (SubFuncTable[Index].CmdNumber)) {
      Status = IhisiProtRegisterCommand (SubFuncTable[Index].CmdNumber, SubFuncTable[Index].IhisiFunction, PcdPriority);
      if (EFI_ERROR(Status)) {
        DEBUG ((EFI_D_ERROR, "IHISI command :0x%X, priority : 0x%X, that already has a registered function\n", \
                              SubFuncTable[Index].CmdNumber, PcdPriority));
        ASSERT (FALSE);
      }
    }
  }
  return Status;
}

