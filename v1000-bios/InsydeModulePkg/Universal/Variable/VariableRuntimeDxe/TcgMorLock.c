/** @file
  TCG MOR (Memory Overwrite Request) Lock Control support.

  This module initializes MemoryOverwriteRequestControlLock variable.
  This module adds Variable Hook and check MemoryOverwriteRequestControlLock.

;******************************************************************************
;* Copyright (c) 2016 - 2019, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <PiDxe.h>
#include <Guid/MemoryOverwriteControl.h>
#include <IndustryStandard/MemoryOverwriteRequestControlLock.h>
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/SmmConfidentialMemLib.h>
#include "Variable.h"

typedef struct {
  CHAR16                                 *VariableName;
  EFI_GUID                               *VendorGuid;
} VARIABLE_TYPE;

VARIABLE_TYPE  mMorVariableType[] = {
  {MEMORY_OVERWRITE_REQUEST_VARIABLE_NAME,      &gEfiMemoryOverwriteControlDataGuid},
  {MEMORY_OVERWRITE_REQUEST_CONTROL_LOCK_NAME,  &gEfiMemoryOverwriteRequestControlLockGuid},
};

#define MOR_LOCK_DATA_UNLOCKED           0x0
#define MOR_LOCK_DATA_LOCKED_WITHOUT_KEY 0x1
#define MOR_LOCK_DATA_LOCKED_WITH_KEY    0x2

#define MOR_LOCK_V1_SIZE      1
#define MOR_LOCK_V2_KEY_SIZE  8

typedef enum {
  MorLockStateUnlocked = 0,
  MorLockStateLocked = 1,
} MOR_LOCK_STATE;

UINT8           *mMorLockKey = NULL;
BOOLEAN         mMorLockKeyEmpty = TRUE;
MOR_LOCK_STATE  mMorLockState = MorLockStateUnlocked;

/**
  Returns if this is MOR related variable.

  @param[in]  VariableName the name of the vendor's variable, it's a Null-Terminated Unicode String
  @param[in]  VendorGuid   Unify identifier for vendor.

  @retval  TRUE            The variable is MOR related.
  @retval  FALSE           The variable is NOT MOR related.
**/
BOOLEAN
IsAnyMorVariable (
  IN CHAR16                                 *VariableName,
  IN EFI_GUID                               *VendorGuid
  )
{
  UINTN   Index;

  for (Index = 0; Index < sizeof(mMorVariableType)/sizeof(mMorVariableType[0]); Index++) {
    if ((StrCmp (VariableName, mMorVariableType[Index].VariableName) == 0) &&
        (CompareGuid (VendorGuid, mMorVariableType[Index].VendorGuid))) {
      return TRUE;
    }
  }
  return FALSE;
}

/**
  Returns if this is MOR lock variable.

  @param[in]  VariableName the name of the vendor's variable, it's a Null-Terminated Unicode String
  @param[in]  VendorGuid   Unify identifier for vendor.

  @retval  TRUE            The variable is MOR lock variable.
  @retval  FALSE           The variable is NOT MOR lock variable.
**/
BOOLEAN
IsMorLockVariable (
  IN CHAR16                                 *VariableName,
  IN EFI_GUID                               *VendorGuid
  )
{
  if ((StrCmp (VariableName, MEMORY_OVERWRITE_REQUEST_CONTROL_LOCK_NAME) == 0) &&
      (CompareGuid (VendorGuid, &gEfiMemoryOverwriteRequestControlLockGuid))) {
    return TRUE;
  }
  return FALSE;
}

/**
  Set MOR lock variable.

  @param[in]  Data         MOR Lock variable data.

  @retval  EFI_SUCCESS            The firmware has successfully stored the variable and its data as
                                  defined by the Attributes.
  @retval  EFI_INVALID_PARAMETER  An invalid combination of attribute bits was supplied, or the
                                  DataSize exceeds the maximum allowed.
  @retval  EFI_INVALID_PARAMETER  VariableName is an empty Unicode string.
  @retval  EFI_OUT_OF_RESOURCES   Not enough storage is available to hold the variable and its data.
  @retval  EFI_DEVICE_ERROR       The variable could not be saved due to a hardware failure.
  @retval  EFI_WRITE_PROTECTED    The variable in question is read-only.
  @retval  EFI_WRITE_PROTECTED    The variable in question cannot be deleted.
  @retval  EFI_SECURITY_VIOLATION The variable could not be written due to EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS
                                  set but the AuthInfo does NOT pass the validation check carried
                                  out by the firmware.
  @retval  EFI_NOT_FOUND          The variable trying to be updated or deleted was not found.
**/
EFI_STATUS
SetMorLockVariable (
  IN UINT8  Data
  )
{
  VARIABLE_POINTER_TRACK  Variable;
  EFI_STATUS              Status;
  UINTN                   VariableCount;

  Status = FindVariableByLifetime (
             MEMORY_OVERWRITE_REQUEST_CONTROL_LOCK_NAME,
             &gEfiMemoryOverwriteRequestControlLockGuid,
             &Variable,
             &VariableCount,
             &mVariableModuleGlobal->VariableBase
             );
  Status = UpdateVariable (
             MEMORY_OVERWRITE_REQUEST_CONTROL_LOCK_NAME,
             &gEfiMemoryOverwriteRequestControlLockGuid,
             &Data,
             sizeof(Data),
             EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
             0,
             0,
             &Variable,
             NULL,
             &mVariableModuleGlobal->VariableBase
             );
  return Status;
}

/**
  Delete MOR lock variable.
**/
STATIC
VOID
DeleteMorLockVariable (
  VOID
  )
{
  VARIABLE_POINTER_TRACK  Variable;
  EFI_STATUS              Status;
  UINTN                   VariableCount;

  Status = FindVariableByLifetime (
             MEMORY_OVERWRITE_REQUEST_CONTROL_LOCK_NAME,
             &gEfiMemoryOverwriteRequestControlLockGuid,
             &Variable,
             &VariableCount,
             &mVariableModuleGlobal->VariableBase
             );
  if (EFI_ERROR (Status)) {
    return;
  }
  Status = UpdateVariable (
             MEMORY_OVERWRITE_REQUEST_CONTROL_LOCK_NAME,
             &gEfiMemoryOverwriteRequestControlLockGuid,
             NULL,
             0,
             EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
             0,
             0,
             &Variable,
             NULL,
             &mVariableModuleGlobal->VariableBase
             );

}

/**
  Delete MOR variable.
**/
STATIC
VOID
DeleteMorVariable (
  VOID
  )
{
  VARIABLE_POINTER_TRACK  Variable;
  EFI_STATUS              Status;
  UINTN                   VariableCount;

  Status = FindVariableByLifetime (
             MEMORY_OVERWRITE_REQUEST_VARIABLE_NAME,
             &gEfiMemoryOverwriteControlDataGuid,
             &Variable,
             &VariableCount,
             &mVariableModuleGlobal->VariableBase
             );
  if (EFI_ERROR (Status)) {
    return;
  }
  Status = UpdateVariable (
             MEMORY_OVERWRITE_REQUEST_VARIABLE_NAME,
             &gEfiMemoryOverwriteControlDataGuid,
             NULL,
             0,
             EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
             0,
             0,
             &Variable,
             NULL,
             &mVariableModuleGlobal->VariableBase
             );

}

/**
  This service is an MorLock checker handler for the SetVariable().

  @param[in]  VariableName the name of the vendor's variable, as a
                       Null-Terminated Unicode String
  @param[in]  VendorGuid   Unify identifier for vendor.
  @param[in]  Attributes   Point to memory location to return the attributes of variable. If the point
                       is NULL, the parameter would be ignored.
  @param[in]  DataSize     The size in bytes of Data-Buffer.
  @param[in]  Data         Point to the content of the variable.

  @retval  EFI_SUCCESS            The MorLock check pass, and Variable driver can store the variable data.
  @retval  EFI_INVALID_PARAMETER  The MorLock data or data size or attributes is not allowed.
  @retval  EFI_ACCESS_DENIED      The MorLock is locked.
  @retval  EFI_WRITE_PROTECTED    The MorLock deletion is not allowed.
  @retval  EFI_ALREADY_STARTED    The MorLock variable is handled inside this function.
                                  Variable driver can just return EFI_SUCCESS.
**/
EFI_STATUS
SetVariableCheckHandlerMorLock (
  IN CHAR16     *VariableName,
  IN EFI_GUID   *VendorGuid,
  IN UINT32     Attributes,
  IN UINTN      DataSize,
  IN VOID       *Data
  )
{
  EFI_STATUS  Status;

  //
  // Basic Check
  //
  if (Attributes == 0 || DataSize == 0 || Data == NULL) {
    return EFI_WRITE_PROTECTED;
  }

  if ((Attributes != (EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS)) ||
      ((DataSize != MOR_LOCK_V1_SIZE) && (DataSize != MOR_LOCK_V2_KEY_SIZE))) {
    return EFI_INVALID_PARAMETER;
  }

  if (mMorLockState == MorLockStateUnlocked) {
    Status = EFI_INVALID_PARAMETER;
    //
    // In Unlocked State
    //
    if (DataSize == MOR_LOCK_V1_SIZE) {
      //
      // V1 - lock permanently
      //
      if (*(UINT8 *)Data == MOR_LOCK_DATA_UNLOCKED) {
        //
        // Unlock
        //
        Status = SetMorLockVariable (MOR_LOCK_DATA_UNLOCKED);
        if (!EFI_ERROR (Status)) {
          //
          // return EFI_ALREADY_STARTED to skip variable set.
          //
          Status = EFI_ALREADY_STARTED;
        }
      } else if (*(UINT8 *)Data == MOR_LOCK_DATA_LOCKED_WITHOUT_KEY) {
        //
        // Lock without key
        //
        Status = SetMorLockVariable (MOR_LOCK_DATA_LOCKED_WITHOUT_KEY);
        if (!EFI_ERROR (Status)) {
          //
          // Lock success
          //
          mMorLockState = MorLockStateLocked;
          //
          // return EFI_ALREADY_STARTED to skip variable set.
          //
          Status = EFI_ALREADY_STARTED;
        }
      }
    } else if (DataSize == MOR_LOCK_V2_KEY_SIZE) {
      //
      // V2 lock and provision the key
      if (mMorLockKey == NULL) {
        return EFI_OUT_OF_RESOURCES;
      }

      //
      // Need set here because the data value on flash is different
      //
      Status = SetMorLockVariable (MOR_LOCK_DATA_LOCKED_WITH_KEY);
      if (!EFI_ERROR(Status)) {
        //
        // Lock success, provision the key
        //
        mMorLockKeyEmpty = FALSE;
        CopyMem (mMorLockKey, Data, MOR_LOCK_V2_KEY_SIZE);
        mMorLockState = MorLockStateLocked;
        //
        // return EFI_ALREADY_STARTED to skip variable set.
        //
        Status = EFI_ALREADY_STARTED;
      }
    }
    return Status;
  } else {
    //
    // In Locked State
    //
    if (mMorLockKeyEmpty || (DataSize != MOR_LOCK_V2_KEY_SIZE)) {
      return EFI_ACCESS_DENIED;
    }
    if (mMorLockKey == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }

    if ((CompareMem (Data, mMorLockKey, MOR_LOCK_V2_KEY_SIZE) == 0)) {
      //
      // Key match - unlock
      //

      //
      // Need set here because the data value on flash is different
      //
      Status = SetMorLockVariable (MOR_LOCK_DATA_UNLOCKED);
      if (EFI_ERROR (Status)) {
        //
        // SetVar fail
        //
        return Status;
      } else {
        //
        // Unlock Success
        //
        mMorLockState = MorLockStateUnlocked;
        mMorLockKeyEmpty = TRUE;
        ZeroMem (mMorLockKey, sizeof(mMorLockKey));
        //
        // return EFI_ALREADY_STARTED to skip variable set.
        //
        return EFI_ALREADY_STARTED;
      }
    } else {
      //
      // Key mismatch - Prevent Dictionary Attack
      //
      mMorLockState = MorLockStateLocked;
      mMorLockKeyEmpty = TRUE;
      ZeroMem (mMorLockKey, sizeof(mMorLockKey));
      return EFI_ACCESS_DENIED;
    }
  }
}


/**
  This service is an MOR/MorLock checker handler for the SetVariable().

  @param  VariableName the name of the vendor's variable, as a
                       Null-Terminated Unicode String
  @param  VendorGuid   Unify identifier for vendor.
  @param  Attributes   Point to memory location to return the attributes of variable. If the point
                       is NULL, the parameter would be ignored.
  @param  DataSize     The size in bytes of Data-Buffer.
  @param  Data         Point to the content of the variable.

  @retval  EFI_SUCCESS            The MOR/MorLock check pass, and Variable driver can store the variable data.
  @retval  EFI_INVALID_PARAMETER  The MOR/MorLock data or data size or attributes is not allowed for MOR variable.
  @retval  EFI_ACCESS_DENIED      The MOR/MorLock is locked.
  @retval  EFI_ALREADY_STARTED    The MorLock variable is handled inside this function.
                                  Variable driver can just return EFI_SUCCESS.
**/
EFI_STATUS
SetVariableCheckHandlerMor (
  IN CHAR16     *VariableName,
  IN EFI_GUID   *VendorGuid,
  IN UINT32     Attributes,
  IN UINTN      DataSize,
  IN VOID       *Data
  )
{
  //
  // do not handle non-MOR variable
  //
  if (!IsAnyMorVariable (VariableName, VendorGuid)) {
    return EFI_SUCCESS;
  }
  //
  // To save copies of the key for locked with key feature in a confidentiality-protected location.
  // we save the key in SMRAM and Mor related variables are only allowed to set in SMM.
  //
  if (mSmst == NULL && IsMorLockVariable (VariableName, VendorGuid)) {
    return EFI_ACCESS_DENIED;
  }
  //
  // MorLock variable
  //
  if (IsMorLockVariable (VariableName, VendorGuid)) {
    return SetVariableCheckHandlerMorLock (
             VariableName,
             VendorGuid,
             Attributes,
             DataSize,
             Data
             );
  }

  //
  // Mor Variable
  //

  //
  // Basic Check
  //
  if ((Attributes != (EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS)) ||
      (DataSize != sizeof(UINT8)) ||
      (Data == NULL)) {
    return EFI_INVALID_PARAMETER;
  }
  if (mMorLockState == MorLockStateLocked && mEnableLocking) {
    //
    // If lock, deny access
    //
    return EFI_ACCESS_DENIED;
  }
  //
  // grant access
  //
  return EFI_SUCCESS;
}

/**
  Callback function to make system is whether support Tcg or Tcg2 protocol.

  @param[in] Event    Event whose notification function is being invoked.
  @param[in] Context  Pointer to the notification function's context.
**/
VOID
EFIAPI
CheckTcgAvailableCallback (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
{
  VOID                    *TcgInterface;
  EFI_STATUS              Status;

  Status = gBS->LocateProtocol (
                     &gEfiTcgProtocolGuid,
                     NULL,                 
                     &TcgInterface
                     );
  if (EFI_ERROR (Status)) {
    Status = gBS->LocateProtocol (
                       &gEfiTcg2ProtocolGuid,
                       NULL,                  
                       &TcgInterface
                       );
  }
  if (!EFI_ERROR (Status)) {
    mVariableModuleGlobal->TcgAvailable = TRUE;
    gBS->CloseEvent (Event);
  }
}

/**
  Initialization for MOR Lock Control.

  @retval EFI_SUCCESS     MorLock initialization success.
  @return Others          Some error occurs.
**/
EFI_STATUS
MorLockInit (
  VOID
  )
{
  VOID                                   *Registration;

  EfiCreateProtocolNotifyEvent(
    &gEfiTcgProtocolGuid,
    TPL_CALLBACK,
    CheckTcgAvailableCallback,
    NULL,
    &Registration
    );

  EfiCreateProtocolNotifyEvent(
    &gEfiTcg2ProtocolGuid,
    TPL_CALLBACK,
    CheckTcgAvailableCallback,
    NULL,
    &Registration
    );

  //
  // Set variable to report capability to OS
  //
  return SetMorLockVariable (0);
}

/**
  Initialization for MOR Lock Control at ready to boot event in SMM.
**/
VOID
MorLockInitAtSmmReadyToBoot (
  VOID
  )
{
  VARIABLE_POINTER_TRACK  Variable;
  UINTN                   VariableCount;
  EFI_STATUS              Status;

  //
  // Check if the MOR variable exists.
  //
  Status = FindVariable (
             MEMORY_OVERWRITE_REQUEST_VARIABLE_NAME,
             &gEfiMemoryOverwriteControlDataGuid,
             &Variable,
             &VariableCount,
             &mVariableModuleGlobal->VariableBase
             );

  if (!EFI_ERROR (Status)) {
    //
    // The MOR variable exists.
    //
    // Some OSes don't follow the TCG's Platform Reset Attack Mitigation spec
    // in that the OS should never create the MOR variable, only read and write
    // it -- these OSes (unintentionally) create MOR if the platform firmware
    // does not produce it. Whether this is the case (from the last OS boot)
    // can be deduced from the absence of the TCG / TCG2 protocols, as edk2's
    // MOR implementation depends on (one of) those protocols.
    // Note: To prevent from using gBS to located protocol in SMM after end of DXE
    //       event, we check the protocol is available in DXE and only check the
    //       status in SMM.
    //
    if (mSmmVariableGlobal->ProtectedModeVariableModuleGlobal->TcgAvailable) {
      return;
    }
  }
  DeleteMorLockVariable ();
  DeleteMorVariable ();
}

/**
  Allocate SMM confidential pool for MOR lock key.
**/
VOID
InitMorLockKey (
  VOID
  )
{
  EFI_STATUS Status;

  if (mSmst == NULL) {
    return;
  }

  Status = SmmAllocateConfidentialPool (MOR_LOCK_V2_KEY_SIZE, (void**)&mMorLockKey);
  if (EFI_ERROR(Status) || mMorLockKey == NULL) {
    return;
  }

  DEBUG((EFI_D_INFO, "%a() - MOR Lock Key Address: 0x%p\n", __FUNCTION__, mMorLockKey));
}