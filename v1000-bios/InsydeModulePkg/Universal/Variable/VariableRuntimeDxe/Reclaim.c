/** @file
  Handles non-volatile variable store garbage collection, using FTW
  (Fault Tolerant Write Lite) protocol.

;******************************************************************************
;* Copyright (c) 2012 - 2020, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/


#include "Variable.h"
#include "VariableCache.h"
#include <PostCode.h>

/**
  Gets LBA of block and offset by given address."

  This function gets the Logical Block Address (LBA) of a firmware
  volume block containing the given address, and the offset of the
  address on the block.

  @param[in]  Address        Address which should be contained
                         by returned FVB handle.
  @param[out] Lba            Pointer to LBA for output.
  @param[out] Offset         Pointer to offset for output.

  @retval EFI_SUCCESS    LBA and offset successfully returned.
  @retval EFI_NOT_FOUND  Fail to find FVB handle by address.
  @retval EFI_ABORTED    Fail to find valid LBA and offset.
**/
EFI_STATUS
GetVariableStoreInfo (
  IN  EFI_PHYSICAL_ADDRESS   VariableBase,
  OUT EFI_PHYSICAL_ADDRESS   *FvbBaseAddress,
  OUT EFI_LBA                *Lba,
  OUT UINTN                  *Offset
  )
{
  EFI_STATUS                          Status;
  EFI_HANDLE                          FvbHandle;
  EFI_FIRMWARE_VOLUME_BLOCK_PROTOCOL  *Fvb;
  EFI_FIRMWARE_VOLUME_HEADER          *FwVolHeader;
  EFI_FV_BLOCK_MAP_ENTRY              *FvbMapEntry;
  UINT32                              LbaIndex;


  *Lba    = (EFI_LBA) (-1);
  *Offset = 0;
  //
  // Get the proper FVB
  //
  Status = GetFvbInfoByAddress (VariableBase, &FvbHandle, &Fvb);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  //
  // Get the Base Address of FV
  //
  Status = Fvb->GetPhysicalAddress (Fvb, FvbBaseAddress);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  FwVolHeader = (EFI_FIRMWARE_VOLUME_HEADER *) ((UINTN) *FvbBaseAddress);

  //
  // Get the (LBA, Offset) of Address
  //
  if ((VariableBase >= *FvbBaseAddress) && (VariableBase <= (*FvbBaseAddress + FwVolHeader->FvLength))) {
    if ((FwVolHeader->FvLength) > (FwVolHeader->HeaderLength)) {
      //
      // BUGBUG: Assume one FV has one type of BlockLength
      //
      FvbMapEntry = &FwVolHeader->BlockMap[0];
      for (LbaIndex = 1; LbaIndex <= FvbMapEntry->NumBlocks; LbaIndex += 1) {
        if (VariableBase < (*FvbBaseAddress + FvbMapEntry->Length * LbaIndex)) {
          //
          // Found the (Lba, Offset)
          //
          *Lba    = LbaIndex - 1;
          *Offset = (UINTN) (VariableBase - (*FvbBaseAddress + FvbMapEntry->Length * (LbaIndex - 1)));
          return EFI_SUCCESS;
        }
      }
    }
  }

  return EFI_ABORTED;
}


/**
  Writes a buffer to variable storage space, in the working block.

  This function writes a buffer to variable storage space into a firmware
  volume block device. The destination is specified by the parameter
  VariableBase. Fault Tolerant Write protocol is used for writing.

  @param[in] VariableBase   Base address of the variable to write.
  @param[in] Buffer         Point to the data buffer.

  @retval EFI_SUCCESS       The function completed successfully.
  @retval EFI_NOT_FOUND     Fail to locate Fault Tolerant Write protocol.
  @retval EFI_ABORTED       The function could not complete successfully.
**/
EFI_STATUS
FtwVariableSpace (
  IN EFI_PHYSICAL_ADDRESS   VariableBase,
  IN UINT8                  *Buffer
  )
{
  EFI_STATUS            Status;
  EFI_FTW_LITE_PROTOCOL *FtwLiteProtocol;
  EFI_LBA               VarLba;
  UINTN                 VarOffset;
  UINT8                 *FtwBuffer;
  UINTN                 FtwBufferSize;
  EFI_SMM_FTW_PROTOCOL  *SmmFtwProtocol;


  FtwBufferSize = GetNonVolatileVariableStoreSize ();
  FtwBuffer     = Buffer;
  if (mSmst == NULL) {
    //
    // Prepare for the variable data
    //
    FtwLiteProtocol = mVariableReclaimInfo->FtwLiteProtocol;
    VarOffset = mVariableReclaimInfo->VariableOffset;
    VarLba = mVariableReclaimInfo->VariableLba;
    //
    // FTW write record
    //
    Status = FtwLiteProtocol->Write (
                                FtwLiteProtocol,
                                NULL,
                                VarLba,         // LBA
                                VarOffset,      // Offset
                                &FtwBufferSize, // NumBytes,
                                FtwBuffer
                                );

  } else {
    SmmFtwProtocol = NULL;
    if (SmmFtwProtocol == NULL) {
      Status = mSmst->SmmLocateProtocol (
                        &gEfiSmmFtwProtocolGuid,
                        NULL,
                        (VOID **)&SmmFtwProtocol
                        );
      if (EFI_ERROR (Status)) {
        return Status;
      }
    }
    //
    // FTW write record
    //
    Status = SmmFtwProtocol->Write(
                              SmmFtwProtocol,
                              mSmmVariableGlobal->VariableStoreInfo.FvbBaseAddress,
                              mSmmVariableGlobal->VariableStoreInfo.Lba,
                              mSmmVariableGlobal->VariableStoreInfo.Offset,
                              &FtwBufferSize,
                              FtwBuffer
                              );
  }
  return Status;
}

/**
  Variable store garbage collection and reclaim operation.

  @param[in]  VariableBase        Base address of variable store
  @param[in]  LastVariableOffset  Offset of last variable
  @param[in]  IsVolatile          The variable store is volatile or not,
                                  if it is non-volatile, need FTW
  @param[in]  CurrentVariable     If it is not NULL, it means not to process
                                  current variable for Reclaim.

  @return EFI_SUCCESS             Reclaim variable store successfully.
  @return EFI_OUT_OF_RESOURCES    Allocate pool failed or total variable size is large than variable store size.
  @return Others                  Other error causes reclaim failed.
**/
EFI_STATUS
EFIAPI
Reclaim (
  IN       EFI_PHYSICAL_ADDRESS  VariableBase,
  OUT      UINTN                 *LastVariableOffset,
  IN       BOOLEAN               IsVolatile,
  IN OUT   VARIABLE_HEADER       **CurrentVariable OPTIONAL
  )
{
  VARIABLE_HEADER             *Variable;
  VARIABLE_HEADER             *StartPtr;
  VARIABLE_HEADER             *EndPtr;
  VARIABLE_HEADER             *NextVariable;
  VARIABLE_STORE_HEADER       *VariableStoreHeader;
  UINT8                       *ValidBuffer;
  UINTN                       VariableSize;
  UINT8                       *CurrPtr;
  EFI_STATUS                  Status;
  VARIABLE_HEADER             *WorkingVariable;
  UINTN                       TotalBufferSize;
  UINTN                       CommonUserVariableTotalSize;


  POST_CODE (DXE_VARIABLE_RECLAIM);

  VariableStoreHeader = (VARIABLE_STORE_HEADER *) ((UINTN) VariableBase);
  StartPtr = GetStartPointer (VariableStoreHeader);
  Variable = StartPtr;
  EndPtr   = GetNonVolatileEndPointer (VariableStoreHeader);

  CommonUserVariableTotalSize = 0;
  //
  // To make the reclaim, here we just allocate a memory that equal to the original memory
  //
  if (IsVolatile) {
    TotalBufferSize = GetVariableStoreSize (VariableStoreHeader);
  } else {
    TotalBufferSize = GetNonVolatileVariableStoreSize ();
  }

  WorkingVariable = NULL;
  ValidBuffer = (UINT8 *) mVariableReclaimInfo->BackupBuffer;
  SetMem (ValidBuffer, TotalBufferSize, 0xff);
  CurrPtr = ValidBuffer;
  //
  // Copy variable store header
  //
  CopyMem (CurrPtr, VariableStoreHeader, GetVariableStoreHeaderSize ());
  CurrPtr += GetVariableStoreHeaderSize ();
  for (Variable = GetStartPointer (VariableStoreHeader); IsValidVariableHeader (Variable); Variable = NextVariable) {
    NextVariable = GetNextVariablePtr (Variable);
    //
    // Check the contents of this variable is in variable range.
    //
    if ((UINTN) NextVariable - (UINTN) VariableBase > TotalBufferSize) {
      ASSERT (FALSE);
      break;
    }
    //
    // check variable name size is correct.
    //
    if (Variable->NameSize != StrSize (GET_VARIABLE_NAME_PTR (Variable))) {
      ASSERT (FALSE);
      continue;
    }

    //
    // State VAR_ADDED or VAR_IN_DELETED_TRANSITION are to kept,
    // The CurrentVariable, is also saved, as SetVariable may fail duo to lack of space
    //
    if ((Variable->State == VAR_ADDED || Variable->State == (VAR_ADDED & VAR_IN_DELETED_TRANSITION)) &&
        IsMostValueVariable (Variable, StartPtr, EndPtr)) {
      VariableSize = (UINTN) NextVariable - (UINTN) Variable;
      CopyMem (CurrPtr, (UINT8 *) Variable, VariableSize);
      if (IsUserVariable ((CHAR16 *) (Variable + 1), &Variable->VendorGuid)) {
        CommonUserVariableTotalSize += VariableSize;
      }
      //
      // If CurrentVariable == Variable, mark as VAR_IN_DELETED_TRANSITION
      //
      if (Variable->State == (VAR_ADDED & VAR_IN_DELETED_TRANSITION) && CurrentVariable != NULL) {
        if (Variable != *CurrentVariable){
          ((VARIABLE_HEADER *)CurrPtr)->State = VAR_ADDED;
        } else  {
          WorkingVariable = (VARIABLE_HEADER *) ((UINTN) VariableBase + ((UINTN) CurrPtr - (UINTN) ValidBuffer));
        }
      } else {
        ((VARIABLE_HEADER *)CurrPtr)->State = VAR_ADDED;
      }
      CurrPtr += VariableSize;
    }
  }

  if (IsVolatile) {
    //
    // If volatile variable store, just copy valid buffer
    //
    SetMem ((UINT8 *) (UINTN) VariableBase, GetVariableStoreSize (VariableStoreHeader), 0xff);
    CopyMem ((UINT8 *) (UINTN) VariableBase, ValidBuffer, (UINTN) CurrPtr - (UINTN) ValidBuffer);
    *LastVariableOffset = (UINTN) (CurrPtr - ValidBuffer);
    Status              = EFI_SUCCESS;
  } else {
    //
    // If non-volatile variable store, perform FTW here.
    //
    Status = FtwVariableSpace (VariableBase, ValidBuffer);
    if (!EFI_ERROR (Status)) {
      if (mSmst == NULL) {
        mVariableModuleGlobal->CommonUserVariableTotalSize = CommonUserVariableTotalSize;
      } else {
        mSmmVariableGlobal->ProtectedModeVariableModuleGlobal->CommonUserVariableTotalSize = CommonUserVariableTotalSize;
      }
      *LastVariableOffset = (UINTN)  CurrPtr - (UINTN) ValidBuffer;
    }
  }

  if (EFI_ERROR (Status)) {
    *LastVariableOffset = 0;
	if (mSmst || !VariableAtRuntime ()) {
      REPORT_STATUS_CODE (
        EFI_ERROR_CODE,
        EFI_SOFTWARE_EFI_RUNTIME_SERVICE | H2O_VARIABLE_RECLAIM_FAILED
        );
	}
  }
  if (!EFI_ERROR (Status) && CurrentVariable != NULL) {
    *CurrentVariable = WorkingVariable;
  }
  if (!EFI_ERROR (Status) && !IsVolatile && mVariableModuleGlobal->NonVolatileVariableCache != NULL) {
    SetMem (mVariableModuleGlobal->NonVolatileVariableCache, TotalBufferSize, 0xff);
    CopyMem (
      mVariableModuleGlobal->NonVolatileVariableCache,
      (VOID *) (UINTN) VariableBase,
      (UINTN) (CurrPtr - ValidBuffer)
      );
      SetNonVolatileVariableCacheCrc32 ();
  } else if (!IsVolatile && mVariableModuleGlobal->NonVolatileVariableCache != NULL) {
    //
    // Turn off variable cache, if reclaim NV data failed
    //
    DisableVariableCache ();
  }

  return Status;
}

/**
  Check the variable is whether in the preserved variable table or not.

  @param[In]    Variable        pointer to the variable

  @retval TRUE   the variable is in the preserved variable table
  @retval FALSE  the variable is not in the preserved variable table

**/
STATIC
BOOLEAN
IsVariableInKeepList (
  VARIABLE_HEADER               *Variable
  )
{
  UINTN Index;

  for (Index = 0; mVariableModuleGlobal->PreservedVariableTable[Index].VariableName != NULL; Index++) {
    if ((CompareGuid (&Variable->VendorGuid, &mVariableModuleGlobal->PreservedVariableTable[Index].VendorGuid)) &&
        (StrCmp (GET_VARIABLE_NAME_PTR (Variable), mVariableModuleGlobal->PreservedVariableTable[Index].VariableName) == 0)) {
      return TRUE;
    }
  }

  return FALSE;
}

/**
  Check the variable is whether authenticated or not.

  @param[In]    Variable        pointer to the variable

  @retval TRUE  the variable is authenticated
  @retval FALSE the variable is not authenticated

**/
STATIC
BOOLEAN
IsAuthenticatedVariable (
  VARIABLE_HEADER               *Variable
  )
{

  if (((Variable->Attributes & EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS) == EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS) ||
      ((Variable->Attributes & EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS) == EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS)) {
    return TRUE;
  }

  return FALSE;
}


/**
  This function elimates all the variables except authenticated variables and
  the variables form mVariableModuleGlobal->PreservedVariableTable.

  @param[In]   ValidBuffer        pointer to the valid variable buffer
  @param[In]   TotalBufferSize    the total size of the variable region
  @param[Out]  ValidVariableSize  pointer to the size of the valid variable

**/
STATIC
VOID
ElimateVariableWhenRegionFull (
  IN  UINT8                     *ValidBuffer,
  IN  UINTN                     TotalBufferSize,
  OUT UINTN                     *ValidVariableSize
  )
{
  VARIABLE_HEADER               *Variable;
  VARIABLE_HEADER               *NextVariable;
  UINTN                         VariableSize;
  UINT8                         *ValidVariablePtr;


  if (mVariableModuleGlobal->PreservedVariableTable == NULL) {
    return;
  }

  Variable = (VARIABLE_HEADER *) (ValidBuffer + GetVariableStoreHeaderSize ());

  ValidVariablePtr = (UINT8 *) Variable;
  VariableSize = 0;
  while (IsValidVariableHeader (Variable)) {
    NextVariable = GetNextVariablePtr (Variable);
    VariableSize = (UINTN) NextVariable - (UINTN) Variable;
    if (IsVariableInKeepList (Variable) || IsAuthenticatedVariable (Variable)) {
      if (ValidVariablePtr != (UINT8 *) Variable) {
        CopyMem (ValidVariablePtr, Variable, VariableSize);
      }
      ValidVariablePtr = ValidVariablePtr + VariableSize;
    }
    Variable = NextVariable;
  }

  *ValidVariableSize = (UINTN) (ValidVariablePtr - ValidBuffer);
  SetMem (ValidVariablePtr, TotalBufferSize - *ValidVariableSize, 0xFF);
}


/**
  non-volatile variable store garbage collection and reclaim operation

  @param[in]  Variable                Pointer to new added variable.
  @param[in]  VarSize                 The size of new added variable size.
  @param[in]  VariableBase            Base address of variable store
  @param[out] LastVariableOffset      Offset of last variable

  @retval EFI_SUCCESS             Reclaim non-volatile variable store successfully.
  @retval EFI_INVALID_PARAMETER   Any input parameter is invalid.
  @retval EFI_OUT_OF_RESOURCES    Allocate pool failed or total variable size is large than variable store size.
  @retval EFI_DEVICE_ERROR        Unable to reclaim non-volatile variable caused by flash device error.
--*/
EFI_STATUS
EFIAPI
ReclaimNonVolatileVariable (
  IN   VARIABLE_HEADER        *NewVariable,
  IN   UINTN                  NewVarSize,
  IN   EFI_PHYSICAL_ADDRESS   VariableBase,
  OUT  UINTN                  *LastVariableOffset
  )
{
  VARIABLE_HEADER             *Variable;
  VARIABLE_HEADER             *StartPtr;
  VARIABLE_HEADER             *EndPtr;
  VARIABLE_HEADER             *NextVariable;
  VARIABLE_STORE_HEADER       *VariableStoreHeader;
  UINT8                       *ValidBuffer;
  UINTN                       ValidBufferSize;
  UINTN                       VariableSize;
  UINTN                       TotalBufferSize;
  UINT8                       *CurrPtr;
  EFI_STATUS                  Status;
  UINTN                       CommonUserVariableTotalSize;

  POST_CODE (DXE_VARIABLE_RECLAIM);
  if (NewVariable == NULL || LastVariableOffset == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  CommonUserVariableTotalSize = 0;
  TotalBufferSize = GetNonVolatileVariableStoreSize ();
  if (mVariableModuleGlobal->NonVolatileVariableCache == NULL) {
    VariableStoreHeader = (VARIABLE_STORE_HEADER *) ((UINTN) VariableBase);
  } else {
    CopyMem (
      mVariableModuleGlobal->NonVolatileVariableCache,
      (VOID *) (UINTN) VariableBase,
      TotalBufferSize
      );
    VariableStoreHeader = (VARIABLE_STORE_HEADER *) ((UINTN) mVariableModuleGlobal->NonVolatileVariableCache);
    SetNonVolatileVariableCacheCrc32 ();
  }
  StartPtr = GetStartPointer (VariableStoreHeader);
  Variable = StartPtr;
  EndPtr   = GetNonVolatileEndPointer (VariableStoreHeader);

  ValidBuffer = (UINT8 *) mVariableReclaimInfo->BackupBuffer;
  SetMem (ValidBuffer, TotalBufferSize, 0xff);

  CurrPtr = ValidBuffer;

  //
  // Copy variable store header
  //
  CopyMem (CurrPtr, VariableStoreHeader, GetVariableStoreHeaderSize ());
  CurrPtr += GetVariableStoreHeaderSize ();
  for (Variable = GetStartPointer (VariableStoreHeader); IsValidVariableHeader (Variable); Variable = NextVariable) {
    NextVariable = GetNextVariablePtr (Variable);
    //
    // Check the contents of this variable is in variable range.
    //
    if ((UINTN) NextVariable - (UINTN) VariableStoreHeader > TotalBufferSize) {
      ASSERT (FALSE);
      break;
    }
    //
    // check variable name size is correct.
    //
    if (Variable->NameSize != StrSize (GET_VARIABLE_NAME_PTR (Variable))) {
      ASSERT (FALSE);
      continue;
    }
    //
    // State VAR_ADDED or VAR_IN_DELETED_TRANSITION are to kept,
    // The CurrentVariable, is also saved, as SetVariable may fail due to lack of space
    //
    if ((Variable->State == VAR_ADDED || Variable->State == (VAR_ADDED & VAR_IN_DELETED_TRANSITION)) &&
        !(CompareGuid (&NewVariable->VendorGuid, &Variable->VendorGuid) &&
        !StrCmp (GET_VARIABLE_NAME_PTR (NewVariable), GET_VARIABLE_NAME_PTR (Variable))) &&
        IsMostValueVariable (Variable, StartPtr, EndPtr)) {
      VariableSize = (UINTN) NextVariable - (UINTN) Variable;
      CopyMem (CurrPtr, (UINT8 *) Variable, VariableSize);
      ((VARIABLE_HEADER *) CurrPtr)->State = VAR_ADDED;
      CurrPtr += VariableSize;
      if (IsUserVariable ((CHAR16 *) (Variable + 1), &Variable->VendorGuid)) {
        CommonUserVariableTotalSize += VariableSize;
      }
    }
  }

  ValidBufferSize = (UINTN) (CurrPtr - ValidBuffer);
  if (ValidBufferSize + NewVarSize > TotalBufferSize) {
    if (mSmst || !VariableAtRuntime ()) {
      REPORT_STATUS_CODE (
        EFI_ERROR_CODE,
        EFI_SOFTWARE_EFI_RUNTIME_SERVICE | H2O_VARIABLE_STORE_FULL
        );
    }
    ElimateVariableWhenRegionFull (ValidBuffer, TotalBufferSize, &ValidBufferSize);
    if (ValidBufferSize + NewVarSize > TotalBufferSize) {
      if (mSmst != NULL) {
        EFI_FREE_POOL (ValidBuffer);
      }
      return EFI_OUT_OF_RESOURCES;
    }

    CurrPtr = ValidBuffer + ValidBufferSize;
  }


  CopyMem (CurrPtr, NewVariable, NewVarSize);
  ((VARIABLE_HEADER *) CurrPtr)->State &= VAR_ADDED;
  CurrPtr += NewVarSize;
  //
  // If non-volatile variable store, perform FTW here.
  //
  Status = FtwVariableSpace (VariableBase, ValidBuffer);
  *LastVariableOffset = 0;
  if (!EFI_ERROR (Status)) {
    if (mSmst == NULL) {
      mVariableModuleGlobal->CommonUserVariableTotalSize = CommonUserVariableTotalSize;
    } else {
      mSmmVariableGlobal->ProtectedModeVariableModuleGlobal->CommonUserVariableTotalSize = CommonUserVariableTotalSize;
    }
    *LastVariableOffset = (UINTN) CurrPtr - (UINTN) ValidBuffer;
  } else if (mSmst || !VariableAtRuntime ()) {
    REPORT_STATUS_CODE (
      EFI_ERROR_CODE,
      EFI_SOFTWARE_EFI_RUNTIME_SERVICE | H2O_VARIABLE_RECLAIM_FAILED
      );
  }

  if (!EFI_ERROR (Status) && mVariableModuleGlobal->NonVolatileVariableCache != NULL) {
    SetMem (mVariableModuleGlobal->NonVolatileVariableCache, TotalBufferSize, 0xff);
    CopyMem (
      mVariableModuleGlobal->NonVolatileVariableCache,
      (VOID *) (UINTN) VariableBase,
      (UINTN) CurrPtr - (UINTN) ValidBuffer
      );
      SetNonVolatileVariableCacheCrc32 ();
  } else if (mVariableModuleGlobal->NonVolatileVariableCache != NULL){
    //
    // reclaim failed, turn off variable chache to prevent from data corrupted
    //
    DisableVariableCache ();
  }
  return Status;
}

/**
  Internal function to check the existence of VARIABLE_RECLAIM_THRESHOLD variable.

  @retval TRUE    VARIABLE_RECLAIM_THRESHOLD variable exists.
  @retval FALSE   VARIABLE_RECLAIM_THRESHOLD variable doesn't exist.
**/
BOOLEAN
DoesNeedDoReclaimVariableExist (
  VOID
  )
{
  EFI_STATUS              Status;
  VARIABLE_POINTER_TRACK  Variable;
  UINTN                   VariableCount;


  Status = FindVariableByLifetime (
             NEED_DO_RECLAIM_NAME,
             &gEfiGenericVariableGuid,
             &Variable,
             &VariableCount,
             &mVariableModuleGlobal->VariableBase
             );

  return EFI_ERROR (Status) ? FALSE : TRUE;

}

/**
  Internal function to delete VARIABLE_RECLAIM_THRESHOLD variable.

  @retval EFI_SUCCESS    VARIABLE_RECLAIM_THRESHOLD variable exists.
  @return Other          VARIABLE_RECLAIM_THRESHOLD variable doesn't exist.
**/
STATIC
EFI_STATUS
DeleteNeedDoReclaimVariable (
  VOID
  )

{
  EFI_STATUS              Status;
  VARIABLE_POINTER_TRACK  Variable;
  UINTN                   VariableCount;


  Status = FindVariableByLifetime (
             NEED_DO_RECLAIM_NAME,
             &gEfiGenericVariableGuid,
             &Variable,
             &VariableCount,
             &mVariableModuleGlobal->VariableBase
             );
  if (EFI_ERROR (Status)) {
    return EFI_SUCCESS;
  }

  return UpdateVariable (
           NEED_DO_RECLAIM_NAME,
           &gEfiGenericVariableGuid,
           NULL,
           0,
           0,
           0,
           0,
           &Variable,
           NULL,
           &mVariableModuleGlobal->VariableBase
           );
}


/**
  Make sure the remaining non-volatile variable space is larger than
  VARIABLE_RECLAIM_THRESHOLD before boot to OS
**/
VOID
EFIAPI
ReclaimForOS (
  VOID
  )
{
  EFI_STATUS                      Status;
  STATIC BOOLEAN                  Reclaimed;

  //
  // This function will be called only once at EndOfDxe or ReadyToBoot event.
  //
  if (Reclaimed) {
    return;
  }
  Reclaimed = TRUE;
  mVariableModuleGlobal->NonVolatileLastVariableOffset = GetCurrentNonVolatileOffset ();
  //
  // Check if the free area is blow a threshold
  //
  if ((GetNonVolatileVariableStoreSize () - mVariableModuleGlobal->NonVolatileLastVariableOffset) < VARIABLE_RECLAIM_THRESHOLD ||
      DoesNeedDoReclaimVariableExist ()) {
    Status = Reclaim (
              mVariableModuleGlobal->VariableBase.NonVolatileVariableBase,
              &mVariableModuleGlobal->NonVolatileLastVariableOffset,
              FALSE,
              NULL
              );
    ASSERT(!EFI_ERROR(Status));
    if (!EFI_ERROR (Status) && DoesNeedDoReclaimVariableExist ()) {
      DeleteNeedDoReclaimVariable ();
    }
  }
}

/**
  Internal function to check VARIABLE_RECLAIM_THRESHOLD or set NEED_DO_RECLAIM_NAME variable to make sure system will
  do reclaim in next boot.

  @retval EFI_SUCCESS    Nonvolatile variable free space is smaller than VARIABLE_RECLAIM_THRESHOLD or
                         set NEED_DO_RECLAIM_NAME variable successfully.
  @return Other          Any error occurred while writing NEED_DO_RECLAIM_NAME variable.
**/
EFI_STATUS
SetDoReclaimNextBoot (
  VOID
  )
{
  EFI_STATUS              Status;
  VARIABLE_POINTER_TRACK  Variable;
  UINTN                   VariableCount;
  UINT8                   Data;
  VARIABLE_GLOBAL         *Global;

  //
  // System will do reclaim next boot if free area is below a threshold, so just return EFI_SUCCESS if free is below
  // a threshold.
  //
  if ((GetNonVolatileVariableStoreSize () - GetCurrentNonVolatileOffset ()) < VARIABLE_RECLAIM_THRESHOLD) {
    return EFI_SUCCESS;
  }

  //
  // return EFI_SUCCESS directly if Need do reclaim variable is set
  //
  Global = &mVariableModuleGlobal->VariableBase;
  Status = FindVariableByLifetime (
             NEED_DO_RECLAIM_NAME,
             &gEfiGenericVariableGuid,
             &Variable,
             &VariableCount,
             Global
             );
  if (!EFI_ERROR (Status)) {
    return EFI_SUCCESS;
  }
  //
  // Write NEED_DO_RECLAIM_NAME variable to indicate system need do reclaim next POST.
  //
  Data = 1;
  return UpdateVariable (
           NEED_DO_RECLAIM_NAME,
           &gEfiGenericVariableGuid,
           &Data,
           sizeof (UINT8),
           EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
           0,
           0,
           &Variable,
           NULL,
           Global
           );
}

/**
  Convert all of preserved variable table relative pointers to virtual address.
**/
VOID
PreservedTableAddressChange (
  VOID
  )
{
  UINTN Index;

  if (mVariableModuleGlobal->PreservedVariableTable == NULL) {
    return;
  }

  for (Index = 0; mVariableModuleGlobal->PreservedVariableTable[Index].VariableName != NULL; Index++) {
    gRT->ConvertPointer (0x0, (VOID **) &mVariableModuleGlobal->PreservedVariableTable[Index].VariableName);
  }
  gRT->ConvertPointer (0x0, (VOID **) &mVariableModuleGlobal->PreservedVariableTable);
}