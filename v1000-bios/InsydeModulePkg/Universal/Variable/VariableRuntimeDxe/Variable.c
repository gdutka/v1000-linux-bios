/** @file
  Provide support functions for variable services.

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

#include "Variable.h"
#include "AuthService.h"
#include "InsydeSecureVariable.h"
#include "VariableLock.h"
#include "VarCheck.h"
#include "SensitiveVariable.h"
#include "VariableCache.h"
#include "RestoreFactoryDefault.h"
#include "VarDefault.h"
#include <PostCode.h>


#include <Library/FlashRegionLib.h>

#define VARIABLE_WORKING_REGION_HOB_GUID {0x92888eba, 0xe125, 0x4c41, 0xbc, 0x9d, 0x68, 0x3e, 0x8f, 0x7e, 0x61, 0x18}
STATIC EFI_GUID  mVariableWorkingRegionHobGuid = VARIABLE_WORKING_REGION_HOB_GUID;
extern EFI_GUID        gEfiAlternateFvBlockGuid;


typedef struct {
  EFI_PHYSICAL_ADDRESS              VariableWorkingRegionStart;
  EFI_PHYSICAL_ADDRESS              VariableWorkingRegionLength;
} VARIABLE_WORKING_REGION_INFO;


SMM_VAR_BUFFER                        *mSmmVarBuf;
SMM_VAR_BUFFER                        *mSmmPhyVarBuf;
VARIABLE_RECLAIM_INFO                 *mVariableReclaimInfo;
BOOLEAN                                mDxeSmmReadyToLockBeforeTriggered;
/**
  This code gets the size of non-volatile variable store.

  @return UINTN           The size of non-volatile variable store.

**/
UINTN
GetNonVolatileVariableStoreSize (
  VOID
  )
{
  return mVariableModuleGlobal->NonVolatileVariableCacheSize;
}



/**
 This code gets the pointer to the last variable memory pointer byte

 @param [in]   VarStoreHeader   Pointer to the Variable Store Header.

 @return                     Pointer to last unavailable Variable Header.
**/
VARIABLE_HEADER *
GetNonVolatileEndPointer (
  IN VARIABLE_STORE_HEADER       *VarStoreHeader
  )
{
  return (VARIABLE_HEADER *) ((UINTN) VarStoreHeader + GetNonVolatileVariableStoreSize ());
}

/**
  This code checks if variable header is valid or not and is whether whole data in variable region.

  @param[in] Variable          Pointer to the Variable Header.
  @param[in] VariableStoreEnd  Pointer to the Variable store end.

  @retval TRUE                 Variable header is valid and in variable region.
  @retval FALSE                Variable header is not valid or isn't in variable region.
**/
BOOLEAN
IsValidVariableHeaderInVarRegion (
  IN CONST VARIABLE_HEADER   *Variable,
  IN CONST VARIABLE_HEADER   *VariableStoreEnd
  )
{
  if (Variable == NULL || VariableStoreEnd == NULL) {
    return FALSE;
  }
  if (!IsValidVariableHeader (Variable) || (UINTN) GetNextVariablePtr (Variable) > (UINTN) VariableStoreEnd) {
    return FALSE;
  }
  return TRUE;
}


/**
  Check the saved data and the input variable data is whether the same.

  @param[in]  VariableBase    Base address of variable store
  @param[in]  VariableOffset  The offset of current non-volatile variable
  @param[in]  Variable        Pointer to the Variable Store Header.

  @retval TRUE            This variable is correct.
  @retval FALSE           This variable is incorrect.
**/
BOOLEAN
EFIAPI
IsCorrectVariable (
  IN   EFI_PHYSICAL_ADDRESS  VariableBase,
  IN   UINTN                 VariableOffset,
  IN   VARIABLE_HEADER       *Variable

  )
{
  VARIABLE_HEADER        *CurrentVariable;
  EFI_PHYSICAL_ADDRESS   ReadPtr;
  UINT32                 WholeVariableSize;

  //
  // Initialize local variable
  //
  CurrentVariable = Variable;
  ReadPtr = VariableBase + VariableOffset;
  //
  // Calculate variable size from current variable
  //
  WholeVariableSize = 0;
  WholeVariableSize = sizeof (VARIABLE_HEADER) + CurrentVariable->NameSize + CurrentVariable->DataSize;

  //
  // Check variable is whether correct
  //
  return (CompareMem ((UINT8 *) ((UINTN) ReadPtr), Variable, WholeVariableSize) == 0) ? TRUE : FALSE;
}

/**
  Internal function to get the start address of most value variable.

  The most value variable rule is:
  1. If sysmte has the specific variable with VAR_ADDED state, using the the last variable with VAR_ADDED state.
  2. If system doesn't have specific variable with VAR_ADDED but has the same variable with VAR_ADDED & VAR_IN_DELETED_TRANSITION
     state, using the last variable with VAR_ADDED & VAR_IN_DELETED_TRANSITION state.
  3. Otherwise, return NULL.

  @param[in] Variable  Pointer to start position

  @return   Pointer to the most value variable or return NULL if not found.
**/
STATIC
VARIABLE_HEADER *
GetMostValueVariable (
  IN  CONST VARIABLE_POINTER_TRACK  *Variable
  )
{
  VARIABLE_HEADER      *CurrentHeader;
  VARIABLE_HEADER      *MostValueHeader;

  MostValueHeader = NULL;
  CurrentHeader   = Variable->StartPtr;
  while (IsValidVariableHeaderInVarRegion (CurrentHeader, Variable->EndPtr)) {
    if (CurrentHeader->State == VAR_ADDED || CurrentHeader->State == (VAR_ADDED & VAR_IN_DELETED_TRANSITION)) {
      if (CompareGuid (&Variable->CurrPtr->VendorGuid, &CurrentHeader->VendorGuid) &&
          StrCmp (GET_VARIABLE_NAME_PTR (Variable->CurrPtr), GET_VARIABLE_NAME_PTR (CurrentHeader)) == 0) {
        if (MostValueHeader == NULL) {
          MostValueHeader = CurrentHeader;
        } else {
          if (MostValueHeader->State == VAR_ADDED) {
            if (CurrentHeader->State == VAR_ADDED) {
              MostValueHeader = CurrentHeader;
			  if (mSmst || !VariableAtRuntime ()) {
                REPORT_STATUS_CODE (
                  EFI_ERROR_CODE,
                  EFI_SOFTWARE_EFI_RUNTIME_SERVICE | H2O_VARIABLE_DUPLICATION
                  );
			  }
            }
          } else {
            MostValueHeader = CurrentHeader;
          }
        }
      }
    }
    CurrentHeader = GetNextVariablePtr (CurrentHeader);
  }
  return MostValueHeader;
}


/**
  This function writes data to the FWH at the correct LBA even if the LBAs
  are fragmented.

  @param[in] Global              Pointer to VARIABLE_GLOBAL structure.
  @param[in] Volatile            If the Variable is Volatile or Non-Volatile
  @param[in] SetByIndex          TRUE: Target pointer is given as index.
                                 FALSE: Target pointer is absolute.
  @param[in] DataPtrIndex        Pointer to the Data from the end of VARIABLE_STORE_HEADER
                                 structure.
  @param[in] DataSize            Size of data to be written.
  @param[in] Buffer              Pointer to the buffer from which data is written.

  @retval EFI_SUCCESS            Variable store successfully updated.
  @retval EFI_INVALID_PARAMETER  Parameters not valid.
--*/
EFI_STATUS
UpdateVariableStore (
  IN  VARIABLE_GLOBAL         *Global,
  IN  BOOLEAN                 Volatile,
  IN  BOOLEAN                 SetByIndex,
  IN  UINTN                   DataPtrIndex,
  IN  UINT32                  DataSize,
  IN  UINT8                   *Buffer
  )
{
  EFI_FV_BLOCK_MAP_ENTRY              *PtrBlockMapEntry;
  UINTN                               BlockIndex2;
  UINTN                               LinearOffset;
  UINTN                               CurrWriteSize;
  UINTN                               CurrWritePtr;
  UINT8                               *CurrBuffer;
  EFI_LBA                             LbaNumber;
  UINTN                               Size;
  EFI_FIRMWARE_VOLUME_HEADER          *FwVolHeader;
  VARIABLE_STORE_HEADER               *VolatileBase;
  EFI_PHYSICAL_ADDRESS                FvVolHdr;
  EFI_PHYSICAL_ADDRESS                DataPtr;
  EFI_STATUS                          Status;
  UINTN                               WriteDataSize;
  BOOLEAN                             FvFound;
  EFI_FIRMWARE_VOLUME_BLOCK_PROTOCOL  *Fvb;

  Status      = EFI_SUCCESS;
  DataPtr     = DataPtrIndex;

  if (Volatile) {
    //
    // Update volatile variable in SMM mode or protected mode
    //
    Status = EFI_INVALID_PARAMETER;
    VolatileBase = (VARIABLE_STORE_HEADER *) ((UINTN) Global->VolatileVariableBase);
    if (SetByIndex) {
      DataPtr += Global->VolatileVariableBase;
    }
    if ((DataPtr + DataSize) <= ((UINTN) ((UINT8 *) VolatileBase + GetVariableStoreSize (VolatileBase)))) {
      //
      // If Volatile Variable just do a simple mem copy.
      //
      CopyMem ((UINT8 *) ((UINTN) DataPtr), Buffer, DataSize);
      Status =  EFI_SUCCESS;
    }
  } else if (mSmst == NULL) {
    //
    // update non-volatile variable in protected mode
    //
    Fvb         = mVariableModuleGlobal->FvbInstance;
    Status = Fvb->GetPhysicalAddress(Fvb, &FvVolHdr);
    ASSERT_EFI_ERROR (Status);
    FwVolHeader = (EFI_FIRMWARE_VOLUME_HEADER *) ((UINTN) FvVolHdr);

    //
    // Data Pointer should point to the actual Address where data is to be
    // written
    //
    if (SetByIndex) {
      DataPtr += Global->NonVolatileVariableBase;
    }

    if ((DataPtr + DataSize) > ((EFI_PHYSICAL_ADDRESS) (UINTN) ((UINT8 *) FwVolHeader + FwVolHeader->FvLength))) {
      return EFI_INVALID_PARAMETER;
    }
    //
    // If we are here we are dealing with Non-Volatile Variables
    //
    LinearOffset  = (UINTN) FwVolHeader;
    CurrWritePtr  = (UINTN) DataPtr;
    CurrWriteSize = DataSize;
    CurrBuffer    = Buffer;
    LbaNumber     = 0;

    if (CurrWritePtr < LinearOffset) {
      return EFI_INVALID_PARAMETER;
    }

    FvFound = FALSE;
    for (PtrBlockMapEntry = FwVolHeader->BlockMap; PtrBlockMapEntry->NumBlocks != 0; PtrBlockMapEntry++) {
      for (BlockIndex2 = 0; BlockIndex2 < PtrBlockMapEntry->NumBlocks; BlockIndex2++) {
        //
        // Check to see if the Variable Writes are spanning through multiple
        // blocks.
        //
        if ((CurrWritePtr >= LinearOffset) && (CurrWritePtr < LinearOffset + PtrBlockMapEntry->Length)) {
          FvFound = TRUE;
          if ((CurrWritePtr + CurrWriteSize) <= (LinearOffset + PtrBlockMapEntry->Length)) {
            Status = Fvb->Write (
                            Fvb,
                            LbaNumber,
                            (UINTN) (CurrWritePtr - LinearOffset),
                            &CurrWriteSize,
                            CurrBuffer
                            );
            break;
          } else {
            Size = (UINT32) (LinearOffset + PtrBlockMapEntry->Length - CurrWritePtr);
            Status = Fvb->Write (
                           Fvb,
                           LbaNumber,
                           (UINTN) (CurrWritePtr - LinearOffset),
                           &Size,
                           CurrBuffer
                           );
            if (EFI_ERROR (Status)) {
              break;
            }
            CurrWritePtr  = LinearOffset + PtrBlockMapEntry->Length;
            CurrBuffer    = CurrBuffer + Size;
            CurrWriteSize = CurrWriteSize - Size;
          }
        }

        LinearOffset += PtrBlockMapEntry->Length;
        LbaNumber++;
      }
      if (FvFound) {
        break;
      }
    }
  } else {
    //
    // update non-volatile variable in SMM mode
    //
    if (SetByIndex) {
      DataPtr += Global->NonVolatileVariableBase;
    }
    WriteDataSize = (UINTN) DataSize;
    Status = mSmmVariableGlobal->SmmFwbService->Write (
                                                  mSmmVariableGlobal->SmmFwbService,
                                                  (UINTN ) DataPtr,
                                                  &WriteDataSize,
                                                  Buffer
                                                  );
  }

  if (mVariableModuleGlobal->NonVolatileVariableCache != NULL && !Volatile) {
    if (!EFI_ERROR (Status)) {
      CopyMem (
        (VOID *) ((UINTN) DataPtr - (UINTN) Global->NonVolatileVariableBase + (UINTN) mVariableModuleGlobal->NonVolatileVariableCache),
         Buffer,
         DataSize
         );
      SetNonVolatileVariableCacheCrc32 ();
    } else {
      //
      // If updating NV data failed, turn off variable chache to prevent from data corrupted
      //
      DisableVariableCache ();
    }
  }

  return Status;
}

/**
  This code finds variable in storage blocks (Volatile or Non-Volatile)

  @param[in] VariableName            Name of the variable to be found
  @param[in] VendorGuid              Vendor GUID to be found.
  @param[in] PtrTrack                Variable Track Pointer structure that contains Variable Information.
  @param[in] VariableCount           The number of found variable.
  @param[in] Global                  VARIABLE_GLOBAL pointer.
  @param[in] CovertPointer           Convert pointer from cache to MMIO or not.

  @retval EFI_SUCCESS                Variable successfully found.
  @retval EFI_INVALID_PARAMETER      If VariableName is not an empty string, while
                                     VendorGuid is NULL.
  @retval EFI_NOT_FOUND              Variable not found

**/
EFI_STATUS
FindVariableEx (
  IN  CONST CHAR16                  *VariableName,
  IN  CONST EFI_GUID                *VendorGuid,
  OUT       VARIABLE_POINTER_TRACK  *PtrTrack,
  OUT       UINTN                   *VariableCount,
  IN        VARIABLE_GLOBAL         *Global,
  IN        BOOLEAN                 CovertPointer
  )
{
  VARIABLE_HEADER         *Variable[2];
  VARIABLE_STORE_HEADER   *VariableStoreHeader[2];
  UINTN                   Index;
  UINTN                   IndexMax;
  VARIABLE_HEADER         *InDeletedVariable;
  UINTN                   InDeletedIndex;
  BOOLEAN                 VariableFound;
  EFI_STATUS              Status;
  EFI_HOB_GUID_TYPE       *GuidHob;
  BOOLEAN                 ForceDefaults;


  InDeletedVariable = NULL;
  InDeletedIndex    = (UINTN)-1;
  *VariableCount = 0;

  //
  // 0: Non-Volatile, 1: Volatile
  //
  IndexMax = 2;
  if (mVariableModuleGlobal->NonVolatileVariableCache == NULL) {
    VariableStoreHeader[0]  = (VARIABLE_STORE_HEADER *) ((UINTN) Global->NonVolatileVariableBase);
  } else {
    VariableStoreHeader[0]  = (VARIABLE_STORE_HEADER *) mVariableModuleGlobal->NonVolatileVariableCache;
  }
  VariableStoreHeader[1]  = (VARIABLE_STORE_HEADER *) ((UINTN) Global->VolatileVariableBase);
  //
  // Start Pointers for the variable.
  // Actual Data Pointer where data can be written.
  //
  Variable[0] = GetStartPointer (VariableStoreHeader[0]);
  Variable[1] = GetStartPointer (VariableStoreHeader[1]);


  if (VariableName[0] != 0 && VendorGuid == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  //
  // Check whether variable defaults were forced during PEI
  //
  ForceDefaults = FALSE;
  if (mSmst == NULL && !mVariableModuleGlobal->VariableWriteReady) {
    GuidHob = GetFirstGuidHob (&gH2OFlashMapRegionVarGuid);
    if (GuidHob != NULL) {
      ForceDefaults = TRUE;
    }
  }
  VariableFound = FALSE;
  //
  // Find the variable by walk through non-volatile and volatile variable store
  //
  for (Index = 0; Index < IndexMax; Index++) {
    PtrTrack->StartPtr  = GetStartPointer (VariableStoreHeader[Index]);
    if (Index == 0) {
       //
       // Skip non-volatile variable store if defaults were forced.
       //
      if (ForceDefaults) {
        continue;
      }
      PtrTrack->EndPtr = GetNonVolatileEndPointer (VariableStoreHeader[Index]);
    } else {
      PtrTrack->EndPtr = GetEndPointer (VariableStoreHeader[Index]);
    }

    while (IsValidVariableHeaderInVarRegion (Variable[Index], PtrTrack->EndPtr)) {
      if (Variable[Index]->State == VAR_ADDED) {
        PtrTrack->CurrPtr = Variable[Index];
        if (VariableName[0] == 0 && Variable[Index] == GetMostValueVariable (PtrTrack)) {
          VariableFound = TRUE;
          break;
        } else if (CompareGuid (VendorGuid, &Variable[Index]->VendorGuid) &&
                   !StrCmp (VariableName, GET_VARIABLE_NAME_PTR (Variable[Index]))) {
          VariableFound = TRUE;
          break;
        }
      } else if (Variable[Index]->State == (VAR_ADDED & VAR_IN_DELETED_TRANSITION)) {
        //
        // VAR_IN_DELETED_TRANSITION should also be checked.
        //
        if (VariableName[0] == 0) {
          InDeletedVariable = Variable[Index];
          InDeletedIndex    = Index;
          (*VariableCount)++;
        } else if (CompareGuid (VendorGuid, &Variable[Index]->VendorGuid) &&
                   !StrCmp (VariableName, GET_VARIABLE_NAME_PTR (Variable[Index]))) {
          InDeletedVariable = Variable[Index];
          InDeletedIndex    = Index;
          (*VariableCount)++;
        }
      }
      Variable[Index] = GetNextVariablePtr (Variable[Index]);
    }
    if (VariableFound) {
      break;
    }
  }

  if (VariableFound) {
    PtrTrack->CurrPtr   = Variable[Index];
    PtrTrack->Volatile  = (BOOLEAN) (Index);
    (*VariableCount)++;
  } else if (InDeletedVariable != NULL) {
    PtrTrack->CurrPtr   = InDeletedVariable;
    PtrTrack->Volatile  = (BOOLEAN) InDeletedIndex;
    if (InDeletedIndex == 0) {
      PtrTrack->StartPtr  = GetStartPointer (VariableStoreHeader[0]);
      PtrTrack->EndPtr    = GetNonVolatileEndPointer (VariableStoreHeader[0]);
    }
    VariableFound = TRUE;
  }

  Status = EFI_SUCCESS;
  if (VariableFound) {
    if (CovertPointer) {
      ConvertCacheAddressToPhysicalAddress (Global, PtrTrack);
      //
      // If non-volatile variable data is invalid, we should flush variable cache to synchronize non-volatile variable data.
      //
      if (!IsValidVariableHeader (PtrTrack->CurrPtr)) {
		if (mSmst || !VariableAtRuntime ()) {
          REPORT_STATUS_CODE (
            EFI_ERROR_CODE,
            EFI_SOFTWARE_EFI_RUNTIME_SERVICE | H2O_VARIABLE_CACHE_CORRUPTION
            );
		}
        ASSERT (FALSE);
        Status = FlushVariableCache (
                   mVariableModuleGlobal->NonVolatileVariableCache,
                   (UINT8 *) (UINTN) mVariableModuleGlobal->VariableBase.NonVolatileVariableBase,
                   mVariableModuleGlobal->NonVolatileVariableCacheSize,
                   &mVariableModuleGlobal->NonVolatileLastVariableOffset
                   );
        SetNonVolatileVariableCacheCrc32 ();
        if (!EFI_ERROR (Status)) {
          Status = FindVariableByLifetime (
                     VariableName,
                     VendorGuid,
                     PtrTrack,
                     VariableCount,
                     Global
                     );
        }
      }
    }
  } else {
    PtrTrack->CurrPtr = NULL;
    Status = EFI_NOT_FOUND;
  }

  return Status;
}


/**
  This code finds variable in storage blocks (Volatile or Non-Volatile)

  @param VariableName                Name of the variable to be found
  @param VendorGuid                  Vendor GUID to be found.
  @param PtrTrack                    Variable Track Pointer structure that contains Variable Information.
  @param VariableCount               The number of found variable.
  @param Global                      VARIABLE_GLOBAL pointer.

  @retval EFI_INVALID_PARAMETER      If VariableName is not an empty string, while
                                     VendorGuid is NULL.
  @retval EFI_SUCCESS                Variable successfully found.
  @retval EFI_NOT_FOUND              Variable not found

**/
EFI_STATUS
FindVariable (
  IN  CONST CHAR16                  *VariableName,
  IN  CONST EFI_GUID                *VendorGuid,
  OUT       VARIABLE_POINTER_TRACK  *PtrTrack,
  OUT       UINTN                   *VariableCount,
  IN        VARIABLE_GLOBAL         *Global
  )
{
  return FindVariableEx (VariableName, VendorGuid, PtrTrack, VariableCount, Global, TRUE);
}

/**
  This code finds variable in all of storage blocks (Volatile, Non-Volatile variable store and
  variable default store)

  @param VariableName                Name of the variable to be found
  @param VendorGuid                  Vendor GUID to be found.
  @param PtrTrack                    Variable Track Pointer structure that contains Variable Information.
  @param VariableCount               The number of found variable.
  @param Global                      VARIABLE_GLOBAL pointer.

  @retval EFI_INVALID_PARAMETER      If VariableName is not an empty string, while
                                     VendorGuid is NULL.
  @retval EFI_SUCCESS                Variable successfully found.
  @retval EFI_NOT_FOUND              Variable not found

**/
EFI_STATUS
FindVariableInAllRegions (
  IN  CONST CHAR16                  *VariableName,
  IN  CONST EFI_GUID                *VendorGuid,
  OUT       VARIABLE_POINTER_TRACK  *PtrTrack,
  OUT       UINTN                   *VariableCount,
  IN        VARIABLE_GLOBAL         *Global
  )
{
  EFI_STATUS       Status;

  Status = FindVariable (VariableName, VendorGuid, PtrTrack, VariableCount, Global);
  if (Status != EFI_NOT_FOUND) {
    return Status;
  }
  return GetVariableFromVariableDefault (VariableName, VendorGuid, PtrTrack);
}

/**
  Delete all variables which the state is VAR_ADDED or VAR_ADDED & VAR_IN_DELETED_TRANSITION and
  attribute is non-volatile.

  @param  VariableName      Name of the variable which will be deleted
  @param  VendorGuid        GUID of the variable which will be deleted
  @param  Global            VARIABLE_GLOBAL pointer

  @retval EFI_SUCCESS       Delete variable successfully.
  @return others            Unable to delete variable.

**/
EFI_STATUS
DeleteAllOldVariable (
  IN  CHAR16                  *VariableName,
  IN  EFI_GUID                *VendorGuid,
  IN  VARIABLE_GLOBAL         *Global
  )

{
  VARIABLE_HEADER       *Variable;
  VARIABLE_HEADER       *StartPtr;
  VARIABLE_HEADER       *EndPtr;
  VARIABLE_STORE_HEADER *VariableStoreHeader;
  UINT8                 State;
  EFI_STATUS            Status;

  if (VariableName == NULL || VariableName[0] == 0 || VendorGuid == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  VariableStoreHeader = (VARIABLE_STORE_HEADER *) ((UINTN) Global->NonVolatileVariableBase);
  StartPtr = GetStartPointer (VariableStoreHeader);
  Variable = StartPtr;
  EndPtr   = GetNonVolatileEndPointer (VariableStoreHeader);

  while (IsValidVariableHeaderInVarRegion (Variable, EndPtr)) {
    if ((Variable->State == VAR_ADDED || Variable->State == (VAR_ADDED & VAR_IN_DELETED_TRANSITION)) &&
         !(VariableAtRuntime () && !(Variable->Attributes & EFI_VARIABLE_RUNTIME_ACCESS))) {
      if (CompareGuid (VendorGuid, &Variable->VendorGuid) &&
          StrCmp (VariableName, GET_VARIABLE_NAME_PTR (Variable)) == 0 &&
          !IsMostValueVariable (Variable, StartPtr, EndPtr)) {
        State = Variable->State;
        State &= VAR_DELETED;
        Status = UpdateVariableStore (
                  Global,
                  FALSE,
                  FALSE,
                  (UINTN) &Variable->State,
                  sizeof (UINT8),
                  &State
                  );
        if (EFI_ERROR (Status)) {
          return Status;
        }
      }
    }
    Variable = GetNextVariablePtr (Variable);
  }

  return EFI_SUCCESS;
}

/**
  This code finds variable in storage blocks (Volatile or Non-Volatile)

  This code finds variable in storage blocks of volatile and non-volatile storage areas.
  If VariableName is an empty string, then we just return the first
  qualified variable without comparing VariableName and VendorGuid.
  Otherwise, VariableName and VendorGuid are compared.

  @param[in]  VariableName       Name of the variable to be found
  @param[in]  VendorGuid         Vendor GUID to be found.
  @param[out] PtrTrack           Variable Track Pointer structure that contains Variable Information.
  @param[out] VariableCount      The number of found variable.
  @param[in]  Global             VARIABLE_GLOBAL pointer.
  @param[in] CovertPointer       Convert pointer from cache to MMIO or not.

  @retval EFI_SUCCESS            Variable successfully found.
  @retval EFI_INVALID_PARAMETER  If VariableName is not an empty string, while VendorGuid is NULL.
  @retval EFI_NOT_FOUND          Variable not found
**/
EFI_STATUS
FindVariableByLifetimeEx (
  IN  CONST CHAR16                  *VariableName,
  IN  CONST EFI_GUID                *VendorGuid,
  OUT       VARIABLE_POINTER_TRACK  *PtrTrack,
  OUT       UINTN                   *VariableCount,
  IN        VARIABLE_GLOBAL         *Global,
  IN        BOOLEAN                 ConvertPointer
  )
{
  EFI_STATUS         Status;

  Status = FindVariableEx (
             VariableName,
             VendorGuid,
             PtrTrack,
             VariableCount,
             Global,
             ConvertPointer
             );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if (VariableAtRuntime () && (PtrTrack->CurrPtr->Attributes & EFI_VARIABLE_RUNTIME_ACCESS) != EFI_VARIABLE_RUNTIME_ACCESS) {
    PtrTrack->CurrPtr = NULL;
    return EFI_NOT_FOUND;
  }

  return EFI_SUCCESS;
}

/**
  This code finds variable in storage blocks (Volatile or Non-Volatile)

  This code finds variable in storage blocks of volatile and non-volatile storage areas.
  If VariableName is an empty string, then we just return the first
  qualified variable without comparing VariableName and VendorGuid.
  Otherwise, VariableName and VendorGuid are compared.

  @param[in]  VariableName       Name of the variable to be found
  @param[in]  VendorGuid         Vendor GUID to be found.
  @param[out] PtrTrack           Variable Track Pointer structure that contains Variable Information.
  @param[out] VariableCount      The number of found variable.
  @param[in]  Global             VARIABLE_GLOBAL pointer

  @retval EFI_SUCCESS            Variable successfully found.
  @retval EFI_INVALID_PARAMETER  If VariableName is not an empty string, while VendorGuid is NULL.
  @retval EFI_NOT_FOUND          Variable not found
**/
EFI_STATUS
FindVariableByLifetime (
  IN  CONST CHAR16                  *VariableName,
  IN  CONST EFI_GUID                *VendorGuid,
  OUT       VARIABLE_POINTER_TRACK  *PtrTrack,
  OUT       UINTN                   *VariableCount,
  IN        VARIABLE_GLOBAL         *Global
  )
{
  return FindVariableByLifetimeEx (VariableName, VendorGuid, PtrTrack, VariableCount, Global, TRUE);
}

/**
  This code finds variable in all of storage blocks (Volatile, Non-Volatile variable store and
  variable default store)

  @param[in]  VariableName       Name of the variable to be found
  @param[in]  VendorGuid         Vendor GUID to be found.
  @param[out] PtrTrack           Variable Track Pointer structure that contains Variable Information.
  @param[out] VariableCount      The number of found variable.
  @param[in]  Global             VARIABLE_GLOBAL pointer

  @retval EFI_SUCCESS            Variable successfully found.
  @retval EFI_INVALID_PARAMETER  If VariableName is not an empty string, while VendorGuid is NULL.
  @retval EFI_NOT_FOUND          Variable not found
**/
EFI_STATUS
FindVariableByLifetimeInAllRegions (
  IN  CONST CHAR16                  *VariableName,
  IN  CONST EFI_GUID                *VendorGuid,
  OUT       VARIABLE_POINTER_TRACK  *PtrTrack,
  OUT       UINTN                   *VariableCount,
  IN        VARIABLE_GLOBAL         *Global
  )
{
  EFI_STATUS       Status;

  Status = FindVariableInAllRegions (
             VariableName,
             VendorGuid,
             PtrTrack,
             VariableCount,
             Global
             );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if (VariableAtRuntime () && (PtrTrack->CurrPtr->Attributes & EFI_VARIABLE_RUNTIME_ACCESS) != EFI_VARIABLE_RUNTIME_ACCESS) {
    PtrTrack->CurrPtr = NULL;
    return EFI_NOT_FOUND;
  }

  return EFI_SUCCESS;
}

/**
  Internal function to get current nonvolatile offset from physical hardware device.

  @return UINTN       The offset of the first free nonvolatile variable space.ata into this region.
**/
UINTN
GetCurrentNonVolatileOffset (
  VOID
  )
{
  UINT8                   *CurrPtr;
  VARIABLE_HEADER         *LastVariable;

  CurrPtr   = (UINT8 *) ((UINTN) mVariableModuleGlobal->VariableBase.NonVolatileVariableBase);
  //
  // Update NonVolatileOffset to make sure the the NonVolatileOffset is correct
  //
  LastVariable = (VARIABLE_HEADER *) (CurrPtr + GetVariableStoreHeaderSize ());
  while (IsValidVariableHeader (LastVariable)) {
    LastVariable = GetNextVariablePtr (LastVariable);
  }
  return ((UINTN) LastVariable - (UINTN) CurrPtr);
}

/**
  Update the variable region with Variable information. If EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS is set,
  index of associated public key is needed.

  @param[in] VariableName       Name of variable.
  @param[in] VendorGuid         Guid of variable.
  @param[in] Data               Variable data.
  @param[in] DataSize           Size of data. 0 means delete.
  @param[in] Attributes         Attributes of the variable.
  @param[in] KeyIndex           Index of associated public key.
  @param[in] MonotonicCount     Value of associated monotonic count.
  @param[in] Variable           The variable information that is used to keep track of variable usage.

  @param[in] TimeStamp          Value of associated TimeStamp.
  @param[in] Global             Pointer to VARIABLE_GLOBAL

  @retval EFI_SUCCESS           The update operation is success.
  @retval EFI_OUT_OF_RESOURCES  Variable region is full, cannot write other data into this region.
**/
EFI_STATUS
UpdateVariable (
  IN CONST CHAR16                 *VariableName,
  IN CONST EFI_GUID               *VendorGuid,
  IN CONST VOID                   *Data,
  IN       UINTN                  DataSize,
  IN       UINT32                 Attributes,
  IN       UINT32                 KeyIndex        OPTIONAL,
  IN       UINT64                 MonotonicCount  OPTIONAL,
  IN       VARIABLE_POINTER_TRACK *Variable,
  IN       EFI_TIME               *TimeStamp      OPTIONAL,
  IN       VARIABLE_GLOBAL        *Global
  )
 {
  EFI_STATUS                          Status;
  VARIABLE_HEADER                     *NextVariable;
  UINTN                               NonVolatileVarableStoreSize;
  UINTN                               VarNameOffset;
  UINTN                               VarDataOffset;
  UINTN                               VarNameSize;
  UINTN                               VarSize;
  BOOLEAN                             Volatile;
  UINT8                               State;
  UINTN                               BufSize;
  UINTN                               DataOffset;
  UINT8                               *WriteBuffer;
  UINTN                               TrySetVariableTime;
  BOOLEAN                             SetVariableSuccess;
  UINTN                               VariableCount;
  UINTN                               *VolatileLastVariableOffset;


  WriteBuffer       = NULL;

  // for Secure Boot variables, perform sanity check of incoming data since the structure is known to firmware
  //
  if (IsSecureDatabaseVariable (VariableName, VendorGuid)) {
    Status = CheckSecureBootVarData (VariableName, VendorGuid, Data, DataSize);
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }

  if (Variable->CurrPtr != NULL) {
    if (!AddressInVariableDefaultRegion ((EFI_PHYSICAL_ADDRESS) (UINTN) Variable->CurrPtr)) {
      if (VariableAtRuntime () && !IsReadOnlyVariable (VariableName, VendorGuid)) {
        //
        // If AtRuntime and the variable is Volatile and Runtime Access,
        // the volatile is ReadOnly, and SetVariable should be aborted and
        // return EFI_WRITE_PROTECTED.
        //
        if (Variable->Volatile) {
          Status = EFI_WRITE_PROTECTED;
          goto Done;
        }
        //
        // Only variable that have NV attributes can be updated/deleted in Runtime.
        //
        if ((Variable->CurrPtr->Attributes & EFI_VARIABLE_NON_VOLATILE) == 0) {
          Status = EFI_INVALID_PARAMETER;
          goto Done;
        }
      }
    }
    //
    // Setting a data variable with no access, or zero DataSize attributes
    // specified causes it to be deleted.
    //
    if ((((Attributes & EFI_VARIABLE_APPEND_WRITE) == 0) && (DataSize == 0)) || ((Attributes & (EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_BOOTSERVICE_ACCESS)) == 0)) {
      if (AddressInVariableDefaultRegion ((EFI_PHYSICAL_ADDRESS) (UINTN) Variable->CurrPtr)) {
        Status = EFI_WRITE_PROTECTED;
        goto Done;
      }

      State = Variable->CurrPtr->State;
      State &= VAR_DELETED;

      Status = UpdateVariableStore (
                 Global,
                 Variable->Volatile,
                 FALSE,
                 (UINTN) &Variable->CurrPtr->State,
                 sizeof (UINT8),
                 &State
                 );
      if (Status == EFI_SUCCESS && !Variable->Volatile && IsUserVariable ((CHAR16 *) (Variable->CurrPtr + 1), &Variable->CurrPtr->VendorGuid)) {
        VarSize = ((UINTN) GetNextVariablePtr (Variable->CurrPtr) - (UINTN) Variable->CurrPtr);
        if (mSmst == NULL) {
          mVariableModuleGlobal->CommonUserVariableTotalSize -= VarSize;
        } else {
          mSmmVariableGlobal->ProtectedModeVariableModuleGlobal->CommonUserVariableTotalSize -= VarSize;
        }
      }
      goto Done;
    }
    //
    // If the variable is marked valid, and the same data has been passed in,
    // then return to the caller immediately.
    //
    if (!AddressInVariableDefaultRegion ((EFI_PHYSICAL_ADDRESS) (UINTN) Variable->CurrPtr) &&
        DataSizeOfVariable (Variable->CurrPtr) == DataSize &&
        (CompareMem (Data, GetVariableDataPtr (Variable->CurrPtr), DataSize) == 0) &&
        ((Attributes & EFI_VARIABLE_APPEND_WRITE) == 0) && (TimeStamp == NULL || CompareTimeStamp (TimeStamp, &Variable->CurrPtr->TimeStamp))) {
      Status = EFI_SUCCESS;
      goto Done;
    }
    //
    // EFI_VARIABLE_APPEND_WRITE attribute only effects for existing variable
    // FIXUP - need to check for duplicate data before appending data, also need to append to correct
    // list, assuming this is a cert, or a PubKey!
    //
    if ((Attributes & EFI_VARIABLE_APPEND_WRITE) != 0) {

      BufSize = Variable->CurrPtr->DataSize + DataSize;

      SetMem (mStorageArea, APPEND_BUFF_SIZE, 0xff);
      //
      // Cache the previous variable data into StorageArea.
      //
      DataOffset = sizeof (VARIABLE_HEADER) + Variable->CurrPtr->NameSize + GET_PAD_SIZE (Variable->CurrPtr->NameSize);
      CopyMem (mStorageArea, (UINT8*)((UINTN)Variable->CurrPtr + DataOffset), Variable->CurrPtr->DataSize);

      //
      // Append the new data to the end of previous data.
      // for special secure boot variables, need to check for duplicate data before append
      //
      if ((DataSize > 0) && ((IsKekVariable (VariableName, VendorGuid)) ||
        (IsImageSecureDatabaseVariable (VariableName, VendorGuid)))) {
        BufSize = Variable->CurrPtr->DataSize;
        Status = AppendSignatureList (DataSize, Data, MAX_VARIABLE_SIZE, &BufSize, mStorageArea);
        if (EFI_ERROR (Status)) {
          goto Done;
        }
        //
        // If appended signature is already existence, just return SUCCESS;
        //
        if ((BufSize == Variable->CurrPtr->DataSize) &&
            (TimeStamp == NULL || CompareTimeStamp (TimeStamp, &Variable->CurrPtr->TimeStamp))) {
          Status = EFI_SUCCESS;
          goto Done;
        }
      } else {
        //
        // If appended data size is 0, just return EFI_SUCCESS
        //
        if ((DataSize == 0) &&
            (TimeStamp == NULL || CompareTimeStamp (TimeStamp, &Variable->CurrPtr->TimeStamp))) {
          Status = EFI_SUCCESS;
          goto Done;
        }

      if (BufSize > APPEND_BUFF_SIZE) {
        //
        // If variable size (previous + current) is bigger than 4K that was reserved in runtime,
        // return EFI_OUT_OF_RESOURCES.
        //
        return EFI_OUT_OF_RESOURCES;
      }
        CopyMem ((UINT8*)((UINTN)mStorageArea + Variable->CurrPtr->DataSize), Data, DataSize);
      }
      //
      // Override Data and DataSize which are used for combined data area including previous and new data.
      //
      Data     = mStorageArea;
      DataSize = BufSize;
    }

    if (!AddressInVariableDefaultRegion ((EFI_PHYSICAL_ADDRESS) (UINTN) Variable->CurrPtr) &&
        ((Variable->CurrPtr->State == VAR_ADDED) ||
        (Variable->CurrPtr->State == (VAR_ADDED & VAR_IN_DELETED_TRANSITION)))) {

      //
      // Mark the old variable as in delete transition.
      //
      State = Variable->CurrPtr->State;
      State &= VAR_IN_DELETED_TRANSITION;

      Status = UpdateVariableStore (
                 Global,
                 Variable->Volatile,
                 FALSE,
                 (UINTN) &Variable->CurrPtr->State,
                 sizeof (UINT8),
                 &State
                 );
      if (EFI_ERROR (Status)) {
        goto Done;
      }
    }
  //
  // Not found existing variable. Create a new variable.
  //
  } else {
    //
    // Make sure we are trying to create a new variable.
    // Setting a data variable with no access, or zero DataSize attributes means to delete it.
    //
    if (DataSize == 0 || (Attributes & (EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_BOOTSERVICE_ACCESS)) == 0) {
      Status = EFI_NOT_FOUND;
      goto Done;
    }

    //
    // Only variable have NV|RT attribute can be created in Runtime.
    //
    if (VariableAtRuntime () &&
        (((Attributes & EFI_VARIABLE_RUNTIME_ACCESS) == 0) || ((Attributes & EFI_VARIABLE_NON_VOLATILE) == 0))) {
      Status = EFI_INVALID_PARAMETER;
      goto Done;
    }
  }

  //
  // Tricky part: Use scratch data area at the end of volatile variable store
  // as a temporary storage.
  //
  if (mSmst == NULL) {
    NextVariable = GetEndPointer ((VARIABLE_STORE_HEADER *) ((UINTN) Global->VolatileVariableBase));
  } else {
    WriteBuffer = VariableAllocateZeroBuffer (SCRATCH_SIZE, FALSE);
    if (WriteBuffer == NULL) {
      Status = EFI_OUT_OF_RESOURCES;
      goto Done;
    }

    NextVariable = (VARIABLE_HEADER *) WriteBuffer;
  }

  SetMem (NextVariable, SCRATCH_SIZE, 0xff);

  NextVariable->StartId     = VARIABLE_DATA;
  NextVariable->Attributes  = Attributes;
  //
  // NextVariable->State = VAR_ADDED;
  //
  NextVariable->Reserved        = 0;
  NextVariable->PubKeyIndex     = KeyIndex;
  NextVariable->MonotonicCount  = MonotonicCount;
  SetMem (&NextVariable->TimeStamp, sizeof (EFI_TIME), 0);
  //
  // if creating a new variable or is admin password, use the passed in timestamp, if available
  //
  if (TimeStamp != NULL && (Variable->CurrPtr == NULL || IsAdminPasswordVariable (VariableName, VendorGuid))) {
    CopyMem (&NextVariable->TimeStamp, TimeStamp, sizeof (EFI_TIME));
  }
  //
  // for UEFI 2.3.1
  //
  if (((Attributes & EFI_VARIABLE_APPEND_WRITE) == 0) &&
  		((Attributes & EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS) ==
		EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS)) {
    ASSERT (TimeStamp != NULL);
    if (TimeStamp != NULL) {
      CopyMem (&NextVariable->TimeStamp, TimeStamp, sizeof (EFI_TIME));
    }
  } else if (((Attributes & EFI_VARIABLE_APPEND_WRITE) != 0) &&
             ((Attributes & EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS) != 0) &&
             (Variable->CurrPtr != NULL))  {
      //
      // In the case when the EFI_VARIABLE_APPEND_WRITE attribute is set, only
      // when the new TimeStamp value is later than the current timestamp associated
      // with the variable, we need associate the new timestamp with the updated value.
      //
    CopyMem (&NextVariable->TimeStamp, &Variable->CurrPtr->TimeStamp, sizeof (EFI_TIME));
    if (TimeStamp != NULL && !CompareTimeStamp (TimeStamp, &Variable->CurrPtr->TimeStamp)) {
      CopyMem (&NextVariable->TimeStamp, TimeStamp, sizeof (EFI_TIME));
    }
  }

  //
  // The EFI_VARIABLE_APPEND_WRITE attribute will never be set in the returned
  // Attributes bitmask parameter of a GetVariable() call.
  //
  NextVariable->Attributes  = Attributes & (~EFI_VARIABLE_APPEND_WRITE);

  VarNameOffset                 = sizeof (VARIABLE_HEADER);
  VarNameSize                   = StrSize (VariableName);
  CopyMem (
    (UINT8 *) ((UINTN) NextVariable + VarNameOffset),
    VariableName,
    VarNameSize
    );
  VarDataOffset = VarNameOffset + VarNameSize + GET_PAD_SIZE (VarNameSize);
  CopyMem (
    (UINT8 *) ((UINTN) NextVariable + VarDataOffset),
    Data,
    DataSize
    );
  CopyMem (&NextVariable->VendorGuid, VendorGuid, sizeof (EFI_GUID));
  //
  // There will be pad bytes after Data, the NextVariable->NameSize and
  // NextVariable->DataSize should not include pad size so that variable
  // service can get actual size in GetVariable.
  //
  NextVariable->NameSize  = (UINT32)VarNameSize;
  NextVariable->DataSize  = (UINT32)DataSize;

  //
  // The actual size of the variable that stores in storage should
  // include pad size.
  //
  VarSize = VarDataOffset + DataSize + GET_PAD_SIZE (DataSize);
  if ((Attributes & EFI_VARIABLE_NON_VOLATILE) != 0) {
    //
    // Sync mVariableModuleGlobal->NonVolatileLastVariableOffset first. This is caused by we
    // may invoke this function between two different modes (protected mode and SMM mode).
    //
    mVariableModuleGlobal->NonVolatileLastVariableOffset = GetCurrentNonVolatileOffset ();

    //
    // If variable cache is enabled, we should check variable data and variable variable is consistent before we write
    // to non-volatile storage.
    //
    if (mVariableModuleGlobal->NonVolatileVariableCache != NULL && !CheckNonVolatileVariableCacheCrc32 ()) {
      ASSERT (FALSE);
	  if (mSmst || !VariableAtRuntime ()) {
        REPORT_STATUS_CODE (
          EFI_ERROR_CODE,
          EFI_SOFTWARE_EFI_RUNTIME_SERVICE | H2O_VARIABLE_CACHE_CORRUPTION
          );
	  }
      Status = FlushVariableCache (
                 mVariableModuleGlobal->NonVolatileVariableCache,
                 (UINT8 *) (UINTN) mVariableModuleGlobal->VariableBase.NonVolatileVariableBase,
                 mVariableModuleGlobal->NonVolatileVariableCacheSize,
                 &mVariableModuleGlobal->NonVolatileLastVariableOffset
                 );
      SetNonVolatileVariableCacheCrc32 ();
      //
      // Try to find exist variable after flush variable cache to mark the exist variable
      //
      VariableCount = 0;
      Status = FindVariableByLifetime (VariableName, VendorGuid, Variable, &VariableCount, Global);
      if (Variable->CurrPtr != NULL && Variable->CurrPtr->State == VAR_ADDED) {
        State = (VAR_ADDED & VAR_IN_DELETED_TRANSITION);
        Status = UpdateVariableStore (
                   Global,
                   Variable->Volatile,
                   FALSE,
                   (UINTN) &Variable->CurrPtr->State,
                   sizeof (UINT8),
                   &State
                   );
        if (EFI_ERROR (Status)) {
          goto Done;
        }
      }
    }
    //
    // Create a nonvolatile variable.
    //
    Volatile = FALSE;
    NonVolatileVarableStoreSize = GetNonVolatileVariableStoreSize ();
    if ((UINT32) (VarSize + mVariableModuleGlobal->NonVolatileLastVariableOffset) >
          NonVolatileVarableStoreSize
          ) {
      if (DoesNeedDoReclaimVariableExist ()) {
        Status = EFI_OUT_OF_RESOURCES;
        goto Done;
      }
      if ((VariableAtRuntime () && !PcdGetBool(PcdRuntimeReclaimSupported))) {
        SetDoReclaimNextBoot ();
        Status = EFI_OUT_OF_RESOURCES;
        goto Done;
      }

      //
      // Perform garbage collection & reclaim operation.
      //
      Status = ReclaimNonVolatileVariable (
                 NextVariable,
                 VarSize,
                 Global->NonVolatileVariableBase,
                 &mVariableModuleGlobal->NonVolatileLastVariableOffset
                 );
      if (EFI_ERROR (Status)) {
        SetDoReclaimNextBoot ();
      }
      goto Done;
    }
    //
    // Start try to set non-volatile variable and use do-while to
    // make sure at least set variable once.
    //
    TrySetVariableTime = 0;
    SetVariableSuccess = FALSE;

    //
    // Four steps
    // 1. Write variable header
    // 2. Set variable state to header valid
    // 3. Write variable data
    // 4. Set variable state to valid
    //
    //
    // Step 1:

    do {
      NextVariable->State = 0xFF;
      Status = UpdateVariableStore (
                 Global,
                 FALSE,
                 TRUE,
                 mVariableModuleGlobal->NonVolatileLastVariableOffset,
                 sizeof (VARIABLE_HEADER),
                 (UINT8 *) NextVariable
                 );

      if (EFI_ERROR (Status)) {
        goto Done;
      }

      //
      // Step 2:
      //
      NextVariable->State = VAR_HEADER_VALID_ONLY;
      Status = UpdateVariableStore (
                 Global,
                 FALSE,
                 TRUE,
                 mVariableModuleGlobal->NonVolatileLastVariableOffset + OFFSET_OF (VARIABLE_HEADER, State),
                 sizeof (UINT8),
                 &NextVariable->State
                 );

      if (EFI_ERROR (Status)) {
        goto Done;
      }
      //
      // Step 3:
      //
      Status = UpdateVariableStore (
                 Global,
                 FALSE,
                 TRUE,
                 mVariableModuleGlobal->NonVolatileLastVariableOffset + sizeof (VARIABLE_HEADER),
                 (UINT32) VarSize - sizeof (VARIABLE_HEADER),
                 (UINT8 *) NextVariable + sizeof (VARIABLE_HEADER)
                 );

      if (EFI_ERROR (Status)) {
        goto Done;
      }
      TrySetVariableTime++;
      //
      //if update SetVariableSuccess to TRUE
      //
      if (IsCorrectVariable (Global->NonVolatileVariableBase, mVariableModuleGlobal->NonVolatileLastVariableOffset, NextVariable)) {
        SetVariableSuccess = TRUE;
      } else {
        if (VariableAtRuntime () && !PcdGetBool(PcdRuntimeReclaimSupported)) {
          Status = EFI_OUT_OF_RESOURCES;
          goto Done;
        }
        NextVariable->State &= VAR_DELETED;
        Status = UpdateVariableStore (
                   Global,
                   FALSE,
                   TRUE,
                   mVariableModuleGlobal->NonVolatileLastVariableOffset + OFFSET_OF (VARIABLE_HEADER, State),
                   sizeof (UINT8),
                   &NextVariable->State
                   );
        //
        // reclaim non-volatile variable : skip this setting variable
        //
        Status = Reclaim (
                   Global->NonVolatileVariableBase,
                   &mVariableModuleGlobal->NonVolatileLastVariableOffset,
                   FALSE,
                   AddressInVariableDefaultRegion ((EFI_PHYSICAL_ADDRESS) (UINTN) Variable->CurrPtr) ? NULL : &Variable->CurrPtr
                   );
        if (EFI_ERROR (Status)) {
          goto Done;
        }
      }
    } while (!SetVariableSuccess  && (TrySetVariableTime < MAX_TRY_SET_VARIABLE_TIMES));

    if (!SetVariableSuccess) {
	  if (mSmst || !VariableAtRuntime ()) {
        REPORT_STATUS_CODE (
          EFI_ERROR_CODE,
          EFI_SOFTWARE_EFI_RUNTIME_SERVICE | H2O_VARIABLE_WRITE_NON_VOLATILE_FAILED
          );
	  }
      Status = EFI_DEVICE_ERROR;
      goto Done;
    }
    //
    // Step 4:write the VAR_ADDED, until make sure data is correct
    // Set variable state to valid
    //
    NextVariable->State = VAR_ADDED;
    Status = UpdateVariableStore (
               Global,
               FALSE,
               TRUE,
               mVariableModuleGlobal->NonVolatileLastVariableOffset + OFFSET_OF (VARIABLE_HEADER, State),
               sizeof (UINT8),
               &NextVariable->State
               );

    if (EFI_ERROR (Status)) {
      goto Done;
    }

    mVariableModuleGlobal->NonVolatileLastVariableOffset += (VarSize);
    if (IsUserVariable ((CHAR16 *) (NextVariable + 1), &NextVariable->VendorGuid)) {
      if (mSmst == NULL) {
        mVariableModuleGlobal->CommonUserVariableTotalSize += VarSize;
      } else {
        mSmmVariableGlobal->ProtectedModeVariableModuleGlobal->CommonUserVariableTotalSize += VarSize;
      }
    }
  } else {
    //
    // Create a volatile variable.
    //
    Volatile = TRUE;
    VolatileLastVariableOffset = NULL;
    if (mSmst == NULL) {
      VolatileLastVariableOffset = &mVariableModuleGlobal->VolatileLastVariableOffset;
    } else {
      VolatileLastVariableOffset = &mSmmVariableGlobal->ProtectedModeVariableModuleGlobal->VolatileLastVariableOffset;
    }

    if ((UINT32) (VarSize + *VolatileLastVariableOffset) >
        GetVariableStoreSize ((VARIABLE_STORE_HEADER *) (UINTN) (Global->VolatileVariableBase))) {
      //
      // Perform garbage collection & reclaim operation.
      //
      Status = Reclaim (
                 Global->VolatileVariableBase,
                 VolatileLastVariableOffset,
                 TRUE,
                 &Variable->CurrPtr
                 );
      if (EFI_ERROR (Status)) {
        goto Done;
      }
      //
      // If still no enough space, return out of resources.
      //
      if ((UINT32) (VarSize + *VolatileLastVariableOffset) >
            GetVariableStoreSize ((VARIABLE_STORE_HEADER *) (UINTN) (Global->VolatileVariableBase))
            ) {
        Status = EFI_OUT_OF_RESOURCES;
        goto Done;
      }
    }

    NextVariable->State = VAR_ADDED;
    Status = UpdateVariableStore (
               Global,
               TRUE,
               TRUE,
               *VolatileLastVariableOffset,
               (UINT32) VarSize,
               (UINT8 *) NextVariable
               );

    if (EFI_ERROR (Status)) {
      goto Done;
    }

    *VolatileLastVariableOffset += (VarSize);
  }

  //
  // Mark the old variable as deleted.
  //
  if (!EFI_ERROR (Status) && Variable->CurrPtr != NULL &&
      !AddressInVariableDefaultRegion ((EFI_PHYSICAL_ADDRESS) (UINTN) Variable->CurrPtr)) {
    State = Variable->CurrPtr->State;
    State &= VAR_DELETED;

    Status = UpdateVariableStore (
             Global,
             Variable->Volatile,
             FALSE,
             (UINTN) &Variable->CurrPtr->State,
             sizeof (UINT8),
             &State
             );
    if (Status == EFI_SUCCESS && !Variable->Volatile && IsUserVariable ((CHAR16 *) (Variable->CurrPtr + 1), &Variable->CurrPtr->VendorGuid)) {
      VarSize = ((UINTN) GetNextVariablePtr (Variable->CurrPtr) - (UINTN) Variable->CurrPtr);
      if (mSmst == NULL) {
        mVariableModuleGlobal->CommonUserVariableTotalSize -= VarSize;
      } else {
        mSmmVariableGlobal->ProtectedModeVariableModuleGlobal->CommonUserVariableTotalSize -= VarSize;
      }
    }
  }

Done:
  if (mSmst != NULL && WriteBuffer != NULL) {
    EFI_FREE_POOL (WriteBuffer);
  }
  return Status;
 }

/**
  Routine to confirm the existence of factory default.

  @retval TRUE      The factory default data exists
  @retval FALSE     The factory default data doesn't exist.

**/
STATIC
BOOLEAN
DoesFactoryDefaultExist (
  VOID
  )
{
  EFI_STATUS       Status;
  UINTN            DataSize;

  DataSize = 0;
  Status = GetSecureDatabaseDefaultVariables (EFI_PK_DEFAULT_VARIABLE_NAME, &gEfiGlobalVariableGuid, NULL, &DataSize, NULL);
  return Status == EFI_BUFFER_TOO_SMALL;
}

/**
  This code sets authenticated variable in storage blocks (Volatile or Non-Volatile).

  @param VariableName                     Name of Variable to be found.
  @param VendorGuid                       Variable vendor GUID.
  @param Attributes                       Attribute value of the variable found
  @param DataSize                         Size of Data found. If size is less than the
                                          data, this value contains the required size.
  @param Data                             Data pointer.
  @param Global                           Pointer to VARIABLE_GLOBAL structure

  @retval EFI_INVALID_PARAMETER           Invalid parameter.
  @retval EFI_SUCCESS                     Set successfully.
  @retval EFI_OUT_OF_RESOURCES            Resource not enough to set variable.
  @retval EFI_NOT_FOUND                   Not found.
  @retval EFI_WRITE_PROTECTED             Variable is read-only.

**/
EFI_STATUS
EFIAPI
SetAuthVariable (
  IN CHAR16                  *VariableName,
  IN EFI_GUID                *VendorGuid,
  IN UINT32                  Attributes,
  IN UINTN                   DataSize,
  IN VOID                    *Data,
  IN VARIABLE_GLOBAL         *Global
  )
{
  VARIABLE_POINTER_TRACK              Variable;
  EFI_STATUS                          Status;
  UINTN                               PayloadSize;
  UINTN                               VariableCount;

  if (!FeaturePcdGet (PcdH2OSecureBootSupported) && IsSecureDatabaseVariable (VariableName, VendorGuid)) {
    return EFI_WRITE_PROTECTED;
  }
  //
  // Check input parameters.
  //
  if (VariableName == NULL || VariableName[0] == 0 || VendorGuid == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (DataSize != 0 && Data == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // UEFI 2.3.1 - check for incompatible attributes
  //
  if ((Attributes & EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS) &&
    (Attributes &  EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS)) {
    return EFI_INVALID_PARAMETER;
  }

  //
  //  Make sure if runtime bit is set, boot service bit is set also.
  //
  if ((Attributes & (EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_BOOTSERVICE_ACCESS)) == EFI_VARIABLE_RUNTIME_ACCESS) {
    return EFI_INVALID_PARAMETER;
  }

  if (IsInsydeSecureVariable (Attributes, DataSize, Data)) {
    if (!IsValidInsydeSecureVariable (VariableName, VendorGuid, Attributes, DataSize, Data)) {
      return EFI_SECURITY_VIOLATION;
    }
    PayloadSize = DataSize;
  } else if ((Attributes & EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS) == EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS) {
    if (DataSize < AUTHINFO_SIZE) {
      //
      // Try to write Authenticated Variable without AuthInfo.
      //
      return EFI_SECURITY_VIOLATION;
    }
    PayloadSize = DataSize - AUTHINFO_SIZE;

  } else if ((Attributes & EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS) == EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS) {
    if (DataSize < AUTHINFO2_SIZE (Data)) {
      //
      // Try to write Authenticated Variable without AuthInfo.
      //
      return EFI_SECURITY_VIOLATION;
    }
    PayloadSize = DataSize - AUTHINFO2_SIZE (Data);
  } else {
    PayloadSize = DataSize;
  }

  //
  //  The size of the VariableName, including the Unicode Null in bytes plus
  //  the DataSize is limited to maximum size of PcdGet32 (PcdMaxHardwareErrorVariableSize)
  //  bytes for HwErrRec, and PcdGet32 (PcdMaxVariableSize) bytes for the others.
  //
  if ((Attributes & EFI_VARIABLE_HARDWARE_ERROR_RECORD) == EFI_VARIABLE_HARDWARE_ERROR_RECORD) {
    if ((PayloadSize > MAX_HARDWARE_ERROR_VARIABLE_SIZE) ||
        (sizeof (VARIABLE_HEADER) + StrSize (VariableName) + PayloadSize > MAX_HARDWARE_ERROR_VARIABLE_SIZE)) {
      return EFI_INVALID_PARAMETER;
    }
    //
    // According to UEFI spec, HARDWARE_ERROR_RECORD variable name convention should be L"HwErrRecXXXX".
    //
    if (StrnCmp (VariableName, L"HwErrRec", StrLen (L"HwErrRec")) != 0) {
      return EFI_INVALID_PARAMETER;
    }
  } else {
    //
    //  The size of the VariableName, including the Unicode Null in bytes plus
    //  the DataSize is limited to maximum size of PcdGet32 (PcdMaxVariableSize) bytes.
    //
    if ((PayloadSize > MAX_VARIABLE_SIZE) ||
        (sizeof (VARIABLE_HEADER) + StrSize (VariableName) + PayloadSize > MAX_VARIABLE_SIZE)) {
      return EFI_INVALID_PARAMETER;
    }
  }

  //
  // Check whether the input variable is already existed.
  //
  VariableCount = 0;
  Status = FindVariableByLifetimeInAllRegions (VariableName, VendorGuid, &Variable, &VariableCount, Global);

  //
  // Process different authenticated variables
  //
  if (IsPkVariable (VariableName, VendorGuid)){
    Status = ProcessVarWithPk (VariableName, VendorGuid, Data, DataSize, &Variable, Attributes, TRUE, Global);
    if (!EFI_ERROR (Status) && DoesFactoryDefaultExist ()) {
      //
      // Set "CustomSecurity" variable to 1 indicates secure boot database has been modified by user and machine isn't in factory.
      //
      Status = UpdateCustomSecurityStatus (1);
    }

  } else if (IsKekVariable (VariableName, VendorGuid)) {
    Status = ProcessVarWithPk (VariableName, VendorGuid, Data, DataSize, &Variable, Attributes, FALSE, Global);
  } else if (IsImageSecureDatabaseVariable (VariableName, VendorGuid)) {
    if (IsDbtVariable (VariableName, VendorGuid) && (PcdGet64 (PcdOsIndicationsSupported) & EFI_OS_INDICATIONS_TIMESTAMP_REVOCATION) == 0) {
      return EFI_WRITE_PROTECTED;
    }
    if (IsDbrVariable (VariableName, VendorGuid) && (PcdGet64 (PcdOsIndicationsSupported) & EFI_OS_INDICATIONS_START_OS_RECOVERY) == 0) {
      return EFI_WRITE_PROTECTED;
    }
    Status = ProcessVarWithPk (VariableName, VendorGuid, Data, DataSize, &Variable, Attributes, FALSE, Global);
    if (EFI_ERROR (Status)) {
      Status = ProcessVarWithKek (VariableName, VendorGuid, Data, DataSize, &Variable, Attributes, Global);
    }
  } else if (IsOsRecoveryOrderVariable (VariableName, VendorGuid) || IsOsRecoveryVariable (VariableName)) {
    if ((PcdGet64 (PcdOsIndicationsSupported) & EFI_OS_INDICATIONS_START_OS_RECOVERY) == 0) {
      return EFI_WRITE_PROTECTED;
    }
    Status = ProcessVarWithPk (VariableName, VendorGuid, Data, DataSize, &Variable, Attributes, FALSE, Global);
    if (EFI_ERROR (Status)) {
      Status = ProcessVarWithKek (VariableName, VendorGuid, Data, DataSize, &Variable, Attributes, Global);
    }
    if (EFI_ERROR (Status)) {
      Status = ProcessVarWithDbr (VariableName, VendorGuid, Data, DataSize, &Variable, Attributes, Global);
    }
  } else if (IsInsydeSecureVariable (Attributes, DataSize, Data)) {
    Status = ProcessInsydeSecureVariable (VariableName, VendorGuid, Data, DataSize, &Variable, Attributes, Global);
  } else {
    Status = ProcessVariable (VariableName, VendorGuid, Data, DataSize, &Variable, Attributes, Global);
  }

  if (Status == EFI_SECURITY_VIOLATION && (mSmst || !VariableAtRuntime ())) {
      REPORT_STATUS_CODE (
        EFI_ERROR_CODE,
        EFI_SOFTWARE_EFI_RUNTIME_SERVICE | H2O_AUTH_VARIABLE_VERIFY_FAILED
        );
  }
  return Status;
}

/**
  Internal function for set variable through SMI.

  @param VariableName              Name of Variable to be found
  @param VendorGuid                Variable vendor GUID
  @param Attributes                Attribute value of the variable found
  @param DataSize                  Size of Data found. If size is less than the
                                   data, this value contains the required size.
  @param Data                      Data pointer

  @retval EFI_INVALID_PARAMETER    Invalid parameter
  @retval EFI_SUCCESS              Set successfully
  @retval EFI_OUT_OF_RESOURCES     Resource not enough to set variable
  @retval EFI_NOT_FOUND            Not found
  @retval EFI_SECURITY_VIOLATION   The variable could not be written due to EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS
                                   set but the AuthInfo does NOT pass the validation check carried out by the firmware.

**/
EFI_STATUS
EFIAPI
SetVariableThroughSmi (
  IN CHAR16                  *VariableName,
  IN EFI_GUID                *VendorGuid,
  IN UINT32                  Attributes,
  IN UINTN                   DataSize,
  IN VOID                    *Data
  )
{
  UINT8                       *WorkingBuf;
  UINTN                       SmmBufSize;

  SmmBufSize = sizeof (SMM_VAR_BUFFER) + MAX_VARIABLE_NAME_SIZE + MAX_VARIABLE_SIZE;
  SetMem (mSmmVarBuf, SmmBufSize, 0x0);
  mSmmVarBuf->Signature = SMM_VARIABLE_SIGNATURE;
  CopyMem (&mSmmVarBuf->VarGuid, VendorGuid, sizeof (EFI_GUID));
  mSmmVarBuf->Attributes = Attributes;
  mSmmVarBuf->VariableNameSize = StrSize (VariableName);
  mSmmVarBuf->DataSize = DataSize;
  mSmmVarBuf->VarChecksum = InternalCalculateSum16 ((UINT8 *)Data, DataSize);
  WorkingBuf = (UINT8 *) (mSmmVarBuf + 1);

  StrCpyS ((CHAR16 *) WorkingBuf, (MAX_VARIABLE_NAME_SIZE + MAX_VARIABLE_SIZE) / sizeof(CHAR16), VariableName);
  WorkingBuf += mSmmVarBuf->VariableNameSize;
  CopyMem (WorkingBuf, Data, DataSize);
  mSmmVarBuf->Status = EFI_UNSUPPORTED;
  SmmSecureBootCall ((UINT8 *) mSmmPhyVarBuf, SmmBufSize, SMM_SET_VARIABLE_SMI_FUN_NUM, SW_SMI_PORT);

  return mSmmVarBuf->Status;
}

/**
  Function uses to check BootOrder variable hook mechanism is whether enabled.

  @retval TRUE     BootOrder variable hook mechanism is enabled.
  @return FALSE    BootOrder variable hook mechanism is disabled.
**/
STATIC
BOOLEAN
IsBootOrderHookEnabled (
  VOID
  )
{
  return mSmst ? mSmmVariableGlobal->ProtectedModeVariableModuleGlobal->BootOrderVariableHook : mVariableModuleGlobal->BootOrderVariableHook;
}

/**
  According to variable name and GUID to Determine the variable is BoorOrder or not.

  @param[in] VariableName  Name of Variable to be found.
  @param[in] VendorGuid    Variable vendor GUID.

  @retval TRUE     This is BoorOrder variable.
  @return FALSE    This isn't BoorOrder variable.
**/
STATIC
BOOLEAN
IsBootOrderVariable (
  IN     CHAR16                             *VariableName,
  IN     EFI_GUID                           *VendorGuid
  )
{
  if (VariableName == NULL || VendorGuid == NULL) {
    return FALSE;
  }

  if (StrCmp (VariableName, L"BootOrder") == 0 && CompareGuid (VendorGuid, &gEfiGlobalVariableGuid)) {
    return TRUE;
  }
  return FALSE;
}

/**
  Get the specific config data from MultiConfig region.

  @param  RequireKind           Find Setup Setting for SETUP_FOR_BIOS_POST or SETUP_FOR_LOAD_DEFAULT.
  @param  VariableName          A pointer to a null-terminated string that is the variable's name.
  @param  VariableGuid          A pointer to an EFI_GUID that is the variable's GUID. The combination of
                                VariableGuid and VariableName must be unique.
  @param  Attributes            If non-NULL, on return, points to the variable's attributes.
  @param  DataSize              On entry, points to the size in bytes of the Data buffer.
                                On return, points to the size of the data returned in Data.
  @param  Data                  Points to the buffer which will hold the returned variable value.

  @retval  EFI_SUCCESS               Config data found successfully
  @retval  EFI_NOT_FOUND          Config data not found
  @retval  EFI_INVALID_PARAMETER  Data is NULL
  @retval EFI_BUFFER_TOO_SMALL  The DataSize is too small for the resulting data.
                                                  DataSize is updated with the size required for
                                                  the specified variable.

**/
EFI_STATUS
GetConfigData (
  IN CONST  UINT8                   RequireKind,
  IN        CHAR16                  *VariableName,
  IN        EFI_GUID                *VariableGuid,
  OUT       UINT32                  *Attributes,
  IN OUT    UINTN                   *DataSize,
  OUT       VOID                    *Data
  )
{
  EFI_STATUS            Status;
  UINT16                ConfigCount;
  UINT16                Index;
  VOID                  *NamePtr;
  VOID                  *DataPtr;
  UINTN                 VarDataSize;

  Status = EFI_NOT_FOUND;
  Index = 0;
  ConfigCount = GetConfigCount();
  DataPtr = NULL;

  for ( ; Index < ConfigCount; Index ++) {
    //
    // Get Active (Attribute: ACTIVE & BIOS_POST) Full Setup Setting from Multi Config Region
    //
    Status = GetFullSetupSetting (
                  RequireKind,
                  &Index,
                  *DataSize,
                  &DataPtr,
                  NULL,
                  NULL
                  );
    if (!EFI_ERROR (Status) && DataPtr != NULL) {
      NamePtr = (VOID *) GET_VARIABLE_NAME_PTR (DataPtr);
      if (CompareMem (VariableName, NamePtr, NameSizeOfVariable (DataPtr)) == 0) {
        //
        // Get data size
        //
        VarDataSize = DataSizeOfVariable (DataPtr);
        if (*DataSize >= VarDataSize) {
          if (Data == NULL) {
            return EFI_INVALID_PARAMETER;
          }

          CopyMem (Data, GetVariableDataPtr (DataPtr), VarDataSize);

          if (Attributes != NULL) {
            *Attributes = ((VARIABLE_HEADER *)DataPtr)->Attributes;
          }

          *DataSize = VarDataSize;

          Status = VariableServicesSetVariable (
                       VariableName,
                       VariableGuid,
                       ((VARIABLE_HEADER *)DataPtr)->Attributes,
                       *DataSize,
                       Data
                       );
          return Status;
        } else {
          *DataSize = VarDataSize;
          return EFI_BUFFER_TOO_SMALL;
        }
      }
    }
  }
  return EFI_NOT_FOUND;
}


/**
  This code finds variable in storage blocks (Volatile or Non-Volatile).

  @param[in]      VariableName   Name of Variable to be found.
  @param[in]      VendorGuid     Variable vendor GUID.
  @param[out]     Attributes     Attribute value of the variable found.
  @param[in, out] DataSize       Size of Data found. If size is less than the
                                 data, this value contains the required size.
  @param[out]     Data           Data pointer.

  @retval  EFI_INVALID_PARAMETE  Invalid parameter.
  @retval  EFI_SUCCESS           Find the specified variable.
  @retval  EFI_NOT_FOUND         Not found.
  @retval  EFI_BUFFER_TO_SMALL   DataSize is too small for the result.
**/
EFI_STATUS
EFIAPI
VariableServicesGetVariable (
  IN      CHAR16            *VariableName,
  IN      EFI_GUID          * VendorGuid,
  OUT     UINT32            *Attributes OPTIONAL,
  IN OUT  UINTN             *DataSize,
  OUT     VOID              *Data OPTIONAL
  )
{
  VARIABLE_POINTER_TRACK  Variable;
  UINTN                   VarDataSize;
  EFI_STATUS              Status;
  UINTN                   VariableCount;
  VARIABLE_GLOBAL         *Global;
  BOOLEAN                 InteruptEnabled;

  if (IsBootOrderHookEnabled () && IsBootOrderVariable (VariableName, VendorGuid)) {
   return VariableServicesGetVariable (
            L"PhysicalBootOrder",
            &gEfiGenericVariableGuid,
            Attributes,
            DataSize,
            Data
            );
  }


  if (VariableName == NULL || VendorGuid == NULL || DataSize == NULL || VariableName[0] == 0) {
    return EFI_INVALID_PARAMETER;
  }

  InteruptEnabled = GetInterruptState ();
  if (VariableAtRuntime () && InteruptEnabled) {
    DisableInterrupts ();
  }
  if (VariableAtRuntime () && FeaturePcdGet (PcdH2OVariableFlushCacheAtRuntime)) {
    WriteBackInvalidateDataCacheRange (
      mVariableModuleGlobal->NonVolatileVariableCache,
      mVariableModuleGlobal->NonVolatileVariableCacheSize + mVariableModuleGlobal->FactoryDefaultSize + mVariableModuleGlobal->VariableDefaultSize
      );
  }
  AcquireLockOnlyAtBootTime (&mVariableModuleGlobal->VariableBase.VariableServicesLock);

  //
  // Check Secure Database Default variable first.
  //
  if (FeaturePcdGet (PcdH2OSecureBootSupported) && IsSecureDatabaseDefaultVariable (VariableName, VendorGuid)) {
     Status = GetSecureDatabaseDefaultVariables (VariableName, VendorGuid, Attributes, DataSize, Data);
     goto Done;
  }
  //
  // Find existing variable
  //
  Global = &mVariableModuleGlobal->VariableBase;
  VariableCount = 0;
  Status = FindVariableByLifetimeInAllRegions (VariableName, VendorGuid, &Variable, &VariableCount, Global);
  if (!(VariableAtRuntime ()) && FeaturePcdGet (PcdMultiConfigSupported) && (Status == EFI_NOT_FOUND)) {
    Status = GetConfigData (SETUP_FOR_BIOS_POST, VariableName, VendorGuid, Attributes, DataSize, Data);
    if (Status == EFI_NOT_FOUND) {
      Status = GetConfigData (SETUP_FOR_LOAD_DEFAULT, VariableName, VendorGuid, Attributes, DataSize, Data);
    }
    if (!EFI_ERROR (Status)) {
      goto Done;
    }
  }
  if (Variable.CurrPtr == NULL || EFI_ERROR (Status)) {
    goto Done;
  }

  if (IsAdminPasswordVariable (VariableName, VendorGuid)) {
    //
    // Set data size to 1 and contents of data to 1 to indicate the existence of admin password.
    //
    VarDataSize = sizeof (UINT8);
  } else {
    VarDataSize = Variable.CurrPtr->DataSize;
  }
  if (*DataSize >= VarDataSize) {
    if (Data == NULL) {
      Status = EFI_INVALID_PARAMETER;
      goto Done;
    }
    if (IsAdminPasswordVariable (VariableName, VendorGuid)) {
      //
      // Set data size to 1 and contents of data to 1 to indicate the existence of admin password.
      //
      SetMem (Data, sizeof (UINT8), 0x01);
    } else {
      CopyMem (Data, GetVariableDataPtr (Variable.CurrPtr), VarDataSize);
    }

    if (Attributes != NULL) {
      //
      // According to AuditMode and DeployedMode should return EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS.
      // But they stored in non-volatile storage to make sure them can be still available after reset system.
      //
      if (IsReadOnlyVariable (VariableName, VendorGuid) ||
          (FeaturePcdGet (PcdH2OCustomizedSecureBootSupported) &&
          (IsAuditModeVariable (VariableName, VendorGuid) || IsDeployedModeVariable (VariableName, VendorGuid)))) {
        //
        // According EFI spec 3.2 Globally defined variables, convert all of read-only variables to
        // fit specification definition.
        //
        *Attributes = EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS;
      } else {
        *Attributes = (Variable.CurrPtr->Attributes & ~(EFI_VARIABLE_DEFAULT_READY_ONLY | EFI_VARIABLE_INSYDE_AUTHENTICATED_WRITE_ACCESS));
      }

    }
    *DataSize = VarDataSize;
    Status = EFI_SUCCESS;
  } else {
    *DataSize = VarDataSize;
    Status = EFI_BUFFER_TOO_SMALL;
  }

Done:
  ReleaseLockOnlyAtBootTime (&mVariableModuleGlobal->VariableBase.VariableServicesLock);
  if (VariableAtRuntime () && InteruptEnabled) {
    EnableInterrupts ();
  }
  return Status;

}

/**
  This code Finds the Next available secure boot database default variable.

  @param[in, out] VariableNameSize Size of the variable.
  @param[in, out] VariableName     Pointer to variable name.
  @param[in, out] VendorGuid       Variable Vendor Guid.

  @retval EFI_SUCCESS              Invalid parameter.
  @retval EFI_BUFFER_TOO_SMALL     Find the specified variable.
  @retval EFI_NOT_FOUND            Not found.
**/
STATIC
EFI_STATUS
GetNextSecureBootDefaultVariableName (
  IN OUT  UINTN             *VariableNameSize,
  IN OUT  CHAR16            *VariableName,
  IN OUT  EFI_GUID          *VendorGuid
  )
{
  EFI_STATUS        Status;
  UINTN             Index;
  UINTN             CurrentIndex;
  UINTN             DefaultVariableCount;
  CHAR16            *DefaultVariableName[] = {
                       EFI_PK_DEFAULT_VARIABLE_NAME,
                       EFI_KEK_DEFAULT_VARIABLE_NAME,
                       EFI_DB_DEFAULT_VARIABLE_NAME,
                       EFI_DBX_DEFAULT_VARIABLE_NAME,
                       EFI_DBT_DEFAULT_VARIABLE_NAME,
                       EFI_DBR_DEFAULT_VARIABLE_NAME
                       };

  Index = 0;
  DefaultVariableCount = sizeof (DefaultVariableName) / sizeof (CHAR16 *);
  if (VariableName[0] != 0) {
    for (Index = 0; Index < DefaultVariableCount; Index++) {
      if (StrCmp (DefaultVariableName[Index], VariableName) == 0) {
        Index++;
        break;
      }
    }
  }

  for (CurrentIndex = Index; CurrentIndex < DefaultVariableCount; CurrentIndex++) {
    Status = FindSecureDatabaseDefaultVariables (DefaultVariableName[CurrentIndex], &gEfiGlobalVariableGuid, NULL, NULL);
    if (!EFI_ERROR (Status)) {
      if (StrSize (DefaultVariableName[CurrentIndex]) > *VariableNameSize) {
        *VariableNameSize = StrSize (DefaultVariableName[CurrentIndex]);
        return EFI_BUFFER_TOO_SMALL;
      } else {
        *VariableNameSize = StrSize (DefaultVariableName[CurrentIndex]);
        CopyMem (VariableName, DefaultVariableName[CurrentIndex], *VariableNameSize);
        CopyMem (VendorGuid, &gEfiGlobalVariableGuid, sizeof (EFI_GUID));
        return EFI_SUCCESS;
      }
    }
  }
  return EFI_NOT_FOUND;
}

/**
  This code Finds the first available secure boot database default variable.

  @param[in, out] VariableNameSize Size of the variable.
  @param[in, out] VariableName     Pointer to variable name.
  @param[in, out] VendorGuid       Variable Vendor Guid.

  @retval EFI_SUCCESS              Invalid parameter.
  @retval EFI_BUFFER_TOO_SMALL     Find the specified variable.
  @retval EFI_NOT_FOUND            Not found.
**/
STATIC
EFI_STATUS
GetFirstSecureBootDefaultVariableName (
  IN OUT  UINTN             *VariableNameSize,
  IN OUT  CHAR16            *VariableName,
  IN OUT  EFI_GUID          *VendorGuid
  )
{
  CHAR16          SavedChar;
  EFI_STATUS      Status;


  SavedChar = VariableName[0];
  VariableName[0] = 0;
  Status = GetNextSecureBootDefaultVariableName (VariableNameSize, VariableName, VendorGuid);
  if (EFI_ERROR (Status)) {
    VariableName[0] = SavedChar;
  }
  return Status;
}

/**
  Check the variable which pointed by CurrPtr is whether a most value variable.

  @param[in, out] CurrPtr      Pointer to the header of current variable
  @param[in, out] StartPtr     Pointer to the start address of variable header.
  @param[in, out] EndPtr       Pointer to the end address of variable header.

  @retval TRUE       The input variable is most value variable.
  @retval FALSE      The input variable isn't most value variable.
**/
BOOLEAN
IsMostValueVariable (
  IN  VARIABLE_HEADER        *CurrPtr,
  IN  VARIABLE_HEADER        *StartPtr,
  IN  VARIABLE_HEADER        *EndPtr
  )
{
  VARIABLE_POINTER_TRACK  Variable;

  if (CurrPtr == NULL || StartPtr == NULL || EndPtr == NULL) {
    return FALSE;
  }
  Variable.CurrPtr  = CurrPtr;
  Variable.StartPtr = StartPtr;
  Variable.EndPtr   = EndPtr;
  return (BOOLEAN) (GetMostValueVariable (&Variable) == CurrPtr);
}

/**
  Get the variable count of input variable in variable cache. If the variable
  cache is NULL, the variable count is 0.

  @param[in] VariableName        Name of Variable to be found.
  @param[in] VendorGuid          Variable vendor GUID.

  @return   The variable count of input variable.
**/
STATIC
UINTN
GetVariableCountFromVariableCache (
  IN CONST CHAR16            *VariableName,
  IN CONST EFI_GUID          *VendorGuid
  )
{
  UINTN                       VariableCount;
  VARIABLE_HEADER             *Variable;
  VARIABLE_HEADER             *EndPtr;

  if (mVariableModuleGlobal->NonVolatileVariableCache == 0) {
    return 0;
  }
  VariableCount = 0;
  Variable      = GetStartPointer ((VARIABLE_STORE_HEADER *) mVariableModuleGlobal->NonVolatileVariableCache);
  EndPtr        = GetNonVolatileEndPointer ((VARIABLE_STORE_HEADER *) mVariableModuleGlobal->NonVolatileVariableCache);
  while (IsValidVariableHeaderInVarRegion (Variable, EndPtr)) {
    if (Variable->State == VAR_ADDED || Variable->State == (VAR_ADDED & VAR_IN_DELETED_TRANSITION)) {
      if (CompareGuid (VendorGuid, &Variable->VendorGuid) &&
          StrCmp (VariableName, GET_VARIABLE_NAME_PTR (Variable)) == 0) {
        VariableCount++;
      }
    }
    Variable = GetNextVariablePtr (Variable);
  }
  return VariableCount;
}

/**
  This code Finds the Next available variable.

  @param[in, out] VariableNameSize  Size of the variable name buffer.
  @param[in, out] VariableName      On input, supplies the last VariableName that was returned by GetNextVariableName().
                                    On output, returns the Null-terminated string of the current variable.
  @param[in, out] VendorGuid        On input, supplies the last VendorGuid that was returned by GetNextVariableName().
                                    On output, returns the VendorGuid of the current variable.

  @retval EFI_SUCCESS               Find the specified variable.
  @retval EFI_INVALID_PARAMETER     Invalid parameter.
  @retval EFI_NOT_FOUND             Not found.
  @retval EFI_BUFFER_TO_SMALL       DataSize is too small for the result.
**/
EFI_STATUS
EFIAPI
VariableServicesGetNextVariableName (
  IN OUT  UINTN             *VariableNameSize,
  IN OUT  CHAR16            *VariableName,
  IN OUT  EFI_GUID          *VendorGuid
  )
{
  VARIABLE_POINTER_TRACK  Variable;
  UINTN                   VarNameSize;
  EFI_STATUS              Status;
  UINTN                   VariableCount;
  VARIABLE_GLOBAL         *Global;
  BOOLEAN                 InteruptEnabled;
  UINTN                   MaxLen;


  if (VariableNameSize == NULL || VariableName == NULL || VendorGuid == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  MaxLen = *VariableNameSize / sizeof (CHAR16);
  if ((MaxLen == 0) || (StrnLenS (VariableName, MaxLen) == MaxLen)) {
    return EFI_INVALID_PARAMETER;
  }

  InteruptEnabled = GetInterruptState ();
  if (VariableAtRuntime () && InteruptEnabled) {
    DisableInterrupts ();
  }
  if (VariableAtRuntime () && FeaturePcdGet (PcdH2OVariableFlushCacheAtRuntime)) {
    WriteBackInvalidateDataCacheRange (
      mVariableModuleGlobal->NonVolatileVariableCache,
      mVariableModuleGlobal->NonVolatileVariableCacheSize + mVariableModuleGlobal->FactoryDefaultSize + mVariableModuleGlobal->VariableDefaultSize
      );
  }
  AcquireLockOnlyAtBootTime (&mVariableModuleGlobal->VariableBase.VariableServicesLock);

  if (FeaturePcdGet (PcdH2OSecureBootSupported) && IsSecureDatabaseDefaultVariable (VariableName, VendorGuid)) {
    Status = GetNextSecureBootDefaultVariableName (VariableNameSize, VariableName, VendorGuid);
    goto Done;
  }

  Global = &mVariableModuleGlobal->VariableBase;
  VariableCount = 0;
  Status = FindVariableByLifetimeEx (VariableName, VendorGuid, &Variable, &VariableCount, Global, FALSE);

  if (Variable.CurrPtr == NULL || EFI_ERROR (Status)) {
    Status = GetNextDefaultVariableName (VariableNameSize, VariableName, VendorGuid);
    //
    // According UEFI specification 2.7. The input values of VariableName and VendorGuid are not a name
    // and GUID of an existing variable should return EFI_INVALID_PARAMETER.
    //
    if (Status == EFI_SUCCESS || Status == EFI_BUFFER_TOO_SMALL || Status == EFI_INVALID_PARAMETER) {
      goto Done;
    }
    if (!FeaturePcdGet (PcdH2OSecureBootSupported)) {
      Status = EFI_NOT_FOUND;
      goto Done;
    }
    Status = GetFirstSecureBootDefaultVariableName (VariableNameSize, VariableName, VendorGuid);
    goto Done;
  }
  //
  // Get variable count from variable cache first and only try to update variable pointer if system
  // has multiple same variables. It can prevent from always getting variable from MMIO.
  //
  if ((Variable.CurrPtr->Attributes & EFI_VARIABLE_NON_VOLATILE) == EFI_VARIABLE_NON_VOLATILE &&
       GetVariableCountFromVariableCache (GET_VARIABLE_NAME_PTR (Variable.CurrPtr), &Variable.CurrPtr->VendorGuid) > 1) {
    Variable.CurrPtr = GetMostValueVariable (&Variable);
  }
  if (VariableName[0] != 0) {
    //
    // If variable name is not NULL, get next variable
    //
    Variable.CurrPtr = GetNextVariablePtr (Variable.CurrPtr);
  }

  while (TRUE) {
    //
    // If both volatile and non-volatile variable store are parsed,
    // return not found
    //
    if (Variable.CurrPtr >= Variable.EndPtr || Variable.CurrPtr == NULL) {
      Variable.Volatile = (BOOLEAN) (Variable.Volatile ^ ((BOOLEAN) 0x1));
      if (Variable.Volatile) {
        Variable.StartPtr = GetStartPointer ((VARIABLE_STORE_HEADER *) (UINTN) (Global->VolatileVariableBase));
        Variable.EndPtr   = GetEndPointer ((VARIABLE_STORE_HEADER *) (UINTN) (Global->VolatileVariableBase));
      } else {
        //
        // Skip finding security default variables if PcdH2OSecureBootSupported PCD is FALSE.
        //
        Status = GetFirstDefaultVariableName (VariableNameSize, VariableName, VendorGuid);
        if (Status == EFI_SUCCESS || Status == EFI_BUFFER_TOO_SMALL) {
          goto Done;
        }
        if (!FeaturePcdGet (PcdH2OSecureBootSupported)) {
          Status = EFI_NOT_FOUND;
          goto Done;
        }
        Status = GetFirstSecureBootDefaultVariableName (VariableNameSize, VariableName, VendorGuid);
        goto Done;
      }

      Variable.CurrPtr = Variable.StartPtr;
      if (!IsValidVariableHeaderInVarRegion (Variable.CurrPtr, Variable.EndPtr)) {

        continue;
      }
    }
    //
    // Variable is found
    // Get variable count from variable cache first and only try to  variable pointer if system
    // has multiple same variables. It can prevent from always getting variable from MMIO.
    //
    if (IsValidVariableHeaderInVarRegion (Variable.CurrPtr, Variable.EndPtr) &&
        (Variable.CurrPtr->State == VAR_ADDED || Variable.CurrPtr->State == (VAR_ADDED & VAR_IN_DELETED_TRANSITION)) &&
        (GetVariableCountFromVariableCache (GET_VARIABLE_NAME_PTR (Variable.CurrPtr), &Variable.CurrPtr->VendorGuid) == 1 ||
         IsMostValueVariable (Variable.CurrPtr, Variable.StartPtr, Variable.EndPtr))) {
      ASSERT (Variable.CurrPtr->NameSize == StrSize (GET_VARIABLE_NAME_PTR (Variable.CurrPtr)));
      if (!(VariableAtRuntime () && !(Variable.CurrPtr->Attributes & EFI_VARIABLE_RUNTIME_ACCESS)) &&
           Variable.CurrPtr->NameSize == StrSize (GET_VARIABLE_NAME_PTR (Variable.CurrPtr))) {
        VarNameSize = Variable.CurrPtr->NameSize;
        if (VarNameSize <= *VariableNameSize) {
          CopyMem (
            VariableName,
            GET_VARIABLE_NAME_PTR (Variable.CurrPtr),
            VarNameSize
            );
          CopyMem (
            VendorGuid,
            &Variable.CurrPtr->VendorGuid,
            sizeof (EFI_GUID)
            );
          Status = EFI_SUCCESS;
        } else {
          Status = EFI_BUFFER_TOO_SMALL;
        }

        *VariableNameSize = VarNameSize;
        goto Done;
      }
    }

    Variable.CurrPtr = GetNextVariablePtr (Variable.CurrPtr);
  }
  Status = EFI_NOT_FOUND;

Done:
  ReleaseLockOnlyAtBootTime (&mVariableModuleGlobal->VariableBase.VariableServicesLock);
  if (VariableAtRuntime () && InteruptEnabled) {
    EnableInterrupts ();
  }
  return Status;
}


/**
  No access attribute (0) is used to delete variable. This function is used update no access
  attribute to correct attribute if variable exists and is authenticated variable and the
  variable data format is correct.

  @param VariableName                     Name of Variable to be found.
  @param VendorGuid                       Variable vendor GUID.
  @param DataSize                         Size of Data found. If size is less than the
                                          data, this value contains the required size.
  @param Data                             Data pointer.
  @param Attributes                       Pointer for attributes.
                                          [in]: must be 0 to indicate this is no access attributes.
                                          [out]: the attributes of pre-exist variable.

  @retval EFI_SUCCESS                     Update attribute successful.
  @retval EFI_INVALID_PARAMETER           Any input parameter is invalid.
  @retval EFI_NOT_FOUND                   Cannot find pre-exist variable.

**/
EFI_STATUS
UpdateNoAccessAttribute (
  IN     CHAR16          *VariableName,
  IN     EFI_GUID        *VendorGuid,
  IN     UINTN           DataSize,
  IN     VOID            *Data,
  IN OUT UINT32          *Attributes
  )
{
  UINTN                      VariableCount;
  VARIABLE_POINTER_TRACK     Variable;
  EFI_STATUS                 Status;
  BOOLEAN                    IsAuthenVariable;

  if (VariableName == NULL || VariableName[0] == 0 || VendorGuid == NULL ||
      Attributes == NULL || *Attributes != 0) {
    return EFI_INVALID_PARAMETER;
  }

  VariableCount = 0;
  Status = FindVariableByLifetimeInAllRegions (VariableName, VendorGuid, &Variable, &VariableCount, &mVariableModuleGlobal->VariableBase);
  if (!EFI_ERROR (Status)) {
    IsAuthenVariable = FALSE;
    if ((Variable.CurrPtr->Attributes & EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS) == EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS) {
      IsAuthenVariable = TRUE;
      if (DataSize != AUTHINFO_SIZE) {
        return EFI_INVALID_PARAMETER;
      }
    } else if ((Variable.CurrPtr->Attributes & EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS) == EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS) {
      IsAuthenVariable = TRUE;
      //
      // Check datasize to prevent from reading data from no access address. If we don't do this check, any access
      // this no access address in runtime will cause blue screen. (ex: WHCK8324 SecureBoot manual test 05-ClearTestConfiguration)
      //
      if (DataSize == 0 || Data == NULL || DataSize != AUTHINFO2_SIZE (Data)) {
        return EFI_INVALID_PARAMETER;
      }
    } else if ((Variable.CurrPtr->Attributes & EFI_VARIABLE_DEFAULT_READY_ONLY) == EFI_VARIABLE_DEFAULT_READY_ONLY) {
      return EFI_WRITE_PROTECTED;
    }

    if (IsAuthenVariable) {
      *Attributes = Variable.CurrPtr->Attributes;
    }
  }

  return Status;
}

/**
  Internal function to check system is whether executing runtime code in SMM.

  @retval TRUE      System executes runtime code in SMM.
  @retval FALSE     System isn't in SMM or run SMM code in SMM.

**/
STATIC
BOOLEAN
DoesRunRuntimeCodeInSmm (
  VOID
  )
{
  EFI_SMM_BASE2_PROTOCOL     *SmmBase;
  BOOLEAN                    InSmm;
  EFI_STATUS                 Status;

  if (mSmst != NULL) {
    return FALSE;
  }
  if (VariableAtRuntime ()) {
    return FALSE;
  }
  Status = gBS->LocateProtocol (
                  &gEfiSmmBase2ProtocolGuid,
                  NULL,
                  (VOID **) &SmmBase
                  );
  if (EFI_ERROR (Status)) {
    return FALSE;
  }
  InSmm = FALSE;
  SmmBase->InSmm (SmmBase, &InSmm);

  return InSmm;
}

/**
  Internal function to check is whether PK variable exist.

  @retval TRUE    PK variable exist.
  @retval FALSE   PK variable doesn't exist.
**/
BOOLEAN
DoesPkExist (
  VOID
  )
{
  VARIABLE_POINTER_TRACK  Variable;
  EFI_STATUS              Status;
  UINTN                   VariableCount;

  Status = FindVariableInAllRegions (
             EFI_PLATFORM_KEY_NAME,
             &gEfiGlobalVariableGuid,
             &Variable,
             &VariableCount,
             &mVariableModuleGlobal->VariableBase
             );
  return Status == EFI_SUCCESS;
}

/**
  Update related variables if any of secure modes variable is changed.

  @param[in] VariableName        A Null-terminated string that is the name of the vendor's variable.
  @param[in] VendorGuid          A unique identifier for the vendor.
**/
VOID
UpdateStateForModeVariablesChanged (
  IN CHAR16                  *VariableName,
  IN EFI_GUID                *VendorGuid
  )
{
  VARIABLE_POINTER_TRACK  Variable;
  EFI_STATUS              Status;
  UINTN                   VariableCount;
  UINT8                   *AuditMode;
  UINT8                   *DeployedMode;
  UINT8                   *SetupMode;

  if ((!FeaturePcdGet (PcdH2OSecureBootSupported)) ||
      (!IsAuditModeVariable (VariableName, VendorGuid) && !IsDeployedModeVariable (VariableName, VendorGuid))) {
    return;
  }
  //
  // Needn't do anything if user deletes AuditMode or DeployedMode variable.
  // (Not allow to delete AuditMode or DeployedMode even if the value of these variables is 0).
  //
  Status = FindVariableByLifetime (VariableName, VendorGuid, &Variable, &VariableCount, &mVariableModuleGlobal->VariableBase);
  if (Status != EFI_SUCCESS) {
    return;
  }

  if (IsAuditModeVariable (VariableName, VendorGuid)) {
    //
    // Needn't do anything if user creates new AuditMode variable with value 0.
    // It means the secure boot modes isn't changed.
    //
    AuditMode = GetVariableDataPtr (Variable.CurrPtr);
    if (*AuditMode == 0) {
      return;
    }
    //
    // Change related variable if system wants to operate in audit mode.
    //
    Status = FindVariableByLifetime (
               EFI_SETUP_MODE_NAME,
               &gEfiGlobalVariableGuid,
               &Variable,
               &VariableCount,
               &mVariableModuleGlobal->VariableBase
               );
    if (Status != EFI_SUCCESS) {
      return;
    }
    SetupMode = GetVariableDataPtr (Variable.CurrPtr);
    //
    // 1. Update AuditMode and DeployedMode to read-only if system wants to operate in audit mode.
    //
    UpdateAuditModeProperty (VAR_CHECK_VARIABLE_PROPERTY_READ_ONLY);
    UpdateDeployedModeProperty (VAR_CHECK_VARIABLE_PROPERTY_READ_ONLY);
    if (*SetupMode == 0) {
      //
      // 2.If system operates in user mode previously, need delete PK variable, change SetupMode
      // to 1 and SecureBoot to 0.
      //
      Status = FindVariableByLifetime (
                 EFI_PLATFORM_KEY_NAME,
                 &gEfiGlobalVariableGuid,
                 &Variable,
                 &VariableCount,
                 &mVariableModuleGlobal->VariableBase
                 );
      ASSERT (Status == EFI_SUCCESS);
      Status = UpdateVariable (
                 EFI_PLATFORM_KEY_NAME,
                 &gEfiGlobalVariableGuid,
                 NULL,
                 0,
                 0,
                 0,
                 0,
                 &Variable,
                 NULL,
                 &mVariableModuleGlobal->VariableBase
                 );
      ASSERT (Status == EFI_SUCCESS);
      UpdatePlatformMode (SETUP_MODE, &mVariableModuleGlobal->VariableBase);
      UpdatePlatformBootMode (SECURE_BOOT_MODE_DISABLE, &mVariableModuleGlobal->VariableBase);
      //
      // Update status to indicate security signature database is modified.
      //
      UpdateCustomSecurityStatus (1);
    }
  } else if (IsDeployedModeVariable (VariableName, VendorGuid)) {
    //
    // Needn't do anything if user creates new Deployed variable with value 0.
    // It means the secure boot modes isn't changed.
    //
    DeployedMode = GetVariableDataPtr (Variable.CurrPtr);
    if (*DeployedMode == 0) {
      return;
    }
    //
    // Update AuditMode and DeployedMode to read-only if system wants to operate in deployed mode.
    //
    UpdateAuditModeProperty (VAR_CHECK_VARIABLE_PROPERTY_READ_ONLY);
    UpdateDeployedModeProperty (VAR_CHECK_VARIABLE_PROPERTY_READ_ONLY);
  }
}

/**
  This code sets variable in storage blocks (Volatile or Non-Volatile).

  @param[in] VariableName        Name of Variable to be found.
  @param[in] VendorGuid          Variable vendor GUID.
  @param[in] Attributes          Attribute value of the variable found
  @param[in] DataSize            Size of Data to be set.
  @param[in] Data                Data pointer.

  @retval EFI_INVALID_PARAMETER  Invalid parameter.
  @retval EFI_SUCCESS            Set successfully.
  @retval EFI_OUT_OF_RESOURCES   Resource not enough to set variable.
  @retval EFI_NOT_FOUND          Not found.
  @retval EFI_WRITE_PROTECTED    Variable is read-only.
**/
EFI_STATUS
EFIAPI
VariableServicesSetVariable (
  IN CHAR16                  *VariableName,
  IN EFI_GUID                *VendorGuid,
  IN UINT32                  Attributes,
  IN UINTN                   DataSize,
  IN VOID                    *Data
  )
{
  VARIABLE_POINTER_TRACK  Variable;
  EFI_STATUS              Status;
  UINTN                   VariableCount;
  VARIABLE_GLOBAL         *Global;
  UINTN                   PayloadSize;


  if (mSmst == NULL && !mVariableModuleGlobal->VariableWriteReady && (Attributes & EFI_VARIABLE_NON_VOLATILE) != 0) {
    return EFI_NOT_READY;
  }
  if (IsBootOrderHookEnabled () && IsBootOrderVariable (VariableName, VendorGuid)) {
   return VariableServicesSetVariable (
            L"PhysicalBootOrder",
            &gEfiGenericVariableGuid,
            Attributes,
            DataSize,
            Data
            );
  }


  if (VariableName == NULL || VariableName[0] == 0 || VendorGuid == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Check for reserved bit in variable attribute.
  // EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS is deprecated but we still allow
  // the delete operation of common authenticated variable at user physical presence.
  // So leave EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS attribute check to AuthVariableLib
  //
  if ((Attributes & (~(EFI_VARIABLE_ATTRIBUTES_MASK | EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS))) != 0) {
    return EFI_INVALID_PARAMETER;
  }

  //
  //  Make sure if runtime bit is set, boot service bit is set also
  //
  if ((Attributes & (EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_BOOTSERVICE_ACCESS)) == EFI_VARIABLE_RUNTIME_ACCESS) {
    if ((Attributes & EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS) != 0) {
      return EFI_UNSUPPORTED;
    } else {
      return EFI_INVALID_PARAMETER;
    }
  }
  //
  // EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS and EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS attribute
  // cannot be set both.
  //
  if (((Attributes & EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS) == EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS)
     && ((Attributes & EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS) == EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS)) {
    return EFI_UNSUPPORTED;
  }

  PayloadSize = DataSize;
  //
  //  The size of the VariableName, including the Unicode Null in bytes plus
  //  the DataSize is limited to maximum size of MAX_HARDWARE_ERROR_VARIABLE_SIZE
  //  bytes for HwErrRec, and MAX_VARIABLE_SIZE bytes for the others.
  //
  if ((Attributes & EFI_VARIABLE_HARDWARE_ERROR_RECORD) == EFI_VARIABLE_HARDWARE_ERROR_RECORD) {
    if ((DataSize > MAX_HARDWARE_ERROR_VARIABLE_SIZE) ||
        (sizeof (VARIABLE_HEADER) + StrSize (VariableName) + DataSize > MAX_HARDWARE_ERROR_VARIABLE_SIZE)) {
      return EFI_INVALID_PARAMETER;
    }
  } else {
    if ((Attributes & EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS) == EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS) {
      if (!IsInsydeSecureVariable (Attributes, DataSize, Data)) {
        return EFI_UNSUPPORTED;
      } else {
        if (DataSize < sizeof (WIN_CERTIFICATE_UEFI_GUID) - sizeof (UINT8)) {
          return EFI_SECURITY_VIOLATION;
        }
        PayloadSize = DataSize - (sizeof (WIN_CERTIFICATE_UEFI_GUID) - sizeof (UINT8));
      }
    } else if ((Attributes & EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS) == EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS) {
      //
      // Sanity check for EFI_VARIABLE_AUTHENTICATION_2 descriptor.
      //
      if (DataSize < OFFSET_OF_AUTHINFO2_CERT_DATA ||
        ((EFI_VARIABLE_AUTHENTICATION_2 *) Data)->AuthInfo.Hdr.dwLength > DataSize - (OFFSET_OF (EFI_VARIABLE_AUTHENTICATION_2, AuthInfo)) ||
        ((EFI_VARIABLE_AUTHENTICATION_2 *) Data)->AuthInfo.Hdr.dwLength < OFFSET_OF (WIN_CERTIFICATE_UEFI_GUID, CertData)) {
        return EFI_SECURITY_VIOLATION;
      }
      PayloadSize = DataSize - AUTHINFO2_SIZE (Data);
    }
    if ((UINTN)(~0) - PayloadSize < StrSize(VariableName)){
      //
      // Prevent whole variable size overflow
      //
      return EFI_INVALID_PARAMETER;
    }
    //
    //  The size of the VariableName, including the Unicode Null in bytes plus
    //  the DataSize is limited to maximum size of MAX_VARIABLE_SIZE bytes.
    //
    if (sizeof (VARIABLE_HEADER) + StrSize (VariableName) + PayloadSize > MAX_VARIABLE_SIZE) {
      return EFI_INVALID_PARAMETER;
    }

    if (GetMaxUserVariableSpace () != 0 && (Attributes & EFI_VARIABLE_NON_VOLATILE) == EFI_VARIABLE_NON_VOLATILE &&
        IsUserVariable (VariableName, VendorGuid)) {
      if (sizeof (VARIABLE_HEADER) + StrSize (VariableName) + PayloadSize + GetUserVariableTotalSize () > GetMaxUserVariableSpace ()) {
        RecordVarErrorFlag (VAR_ERROR_FLAG_SYSTEM_ERROR, VariableName, VendorGuid, Attributes, PayloadSize);
        return EFI_OUT_OF_RESOURCES;
      }
    }

  }
  Global = &mVariableModuleGlobal->VariableBase;


  if (IsVariableLocked (VariableName, VendorGuid)) {
    return EFI_WRITE_PROTECTED;
  }

  Status = InternalVarCheckSetVariableCheck (VariableName, VendorGuid, Attributes, PayloadSize, (VOID *) ((UINTN) Data + DataSize - PayloadSize));
  if (EFI_ERROR (Status)) {
    return Status;
  }

  AcquireLockOnlyAtBootTime (&mVariableModuleGlobal->VariableBase.VariableServicesLock);
  if (VariableAtRuntime () && FeaturePcdGet (PcdH2OVariableFlushCacheAtRuntime)) {
    WriteBackInvalidateDataCacheRange (
      mVariableModuleGlobal->NonVolatileVariableCache,
      mVariableModuleGlobal->NonVolatileVariableCacheSize + mVariableModuleGlobal->FactoryDefaultSize + mVariableModuleGlobal->VariableDefaultSize
      );
  }

  if (Global->ReentrantState != 0) {
    ReleaseLockOnlyAtBootTime (&mVariableModuleGlobal->VariableBase.VariableServicesLock);
    return EFI_ACCESS_DENIED;
  }

  Global->ReentrantState++;
  if (FeaturePcdGet (PcdH2OCustomizedSecureBootSupported) &&
      (IsAuditModeVariable (VariableName, VendorGuid) || IsDeployedModeVariable (VariableName, VendorGuid))) {
    //
    // For AuditMode and DeployedMode variables the value should be 0 or 1
    //
    if ((Attributes == 0 || DataSize == 0) || (*((UINT8 *) Data) != 0 && *((UINT8 *) Data) != 1)) {
      Status = EFI_INVALID_PARAMETER;
      goto Done;
    }
    Attributes = VARIABLE_ATTRIBUTE_NV_BS_RT;
  }

  //
  // Some CPU spends much more time to access normal memory region if system is in SMM. Therefore, we check
  // normal nonvolatile variable data in protected mode and don't call SetupVariable in SMM if variable data
  // and attributes are all the same.
  //
  if ((mDxeSmmReadyToLockBeforeTriggered || mReadyToBootEventSignaled) && mSmst == NULL && mVariableModuleGlobal->SmmCodeReady &&
      (Attributes == (EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS) ||
       Attributes == (EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS))) {
    Status = FindVariable (
               VariableName,
               VendorGuid,
               &Variable,
               &VariableCount,
               Global
               );
    if (!EFI_ERROR (Status) && Variable.CurrPtr->Attributes == Attributes && DataSizeOfVariable (Variable.CurrPtr) == DataSize &&
        (CompareMem (Data, GetVariableDataPtr (Variable.CurrPtr), DataSize) == 0)) {
      Status = EFI_SUCCESS;
      goto Done;
    }
  }
  //
  // Some CPU spends much more time to access normal memory region if system is in SMM. Therefore, we always set
  // volatile variable in protected mode to enhance performance.
  //
  if ((mDxeSmmReadyToLockBeforeTriggered || mReadyToBootEventSignaled || IsInsydeSecureVariable (Attributes, DataSize, Data) ||
       IsAdminPasswordVariable (VariableName, VendorGuid) || IsAnyMorVariable (VariableName, VendorGuid)) &&
      mSmst == NULL && mVariableModuleGlobal->SmmCodeReady && ((Attributes & EFI_VARIABLE_NON_VOLATILE) == EFI_VARIABLE_NON_VOLATILE || Attributes == 0)) {
    if (Attributes != 0 && (IsAuditModeVariable (VariableName, VendorGuid) || IsDeployedModeVariable (VariableName, VendorGuid))) {
      Attributes = VARIABLE_ATTRIBUTE_BS_RT;
    }
    if (!DoesRunRuntimeCodeInSmm ()) {
      Status = SetVariableThroughSmi (
                 VariableName,
                 VendorGuid,
                 Attributes,
                 DataSize,
                 Data
                 );
    } else {
      Status = mVariableModuleGlobal->SmmSetVariable (
                                        VariableName,
                                        VendorGuid,
                                        Attributes,
                                        DataSize,
                                        Data
                                        );

    }
    goto Done;
  }

  if (mSmst != NULL || mVariableModuleGlobal->VariableWriteReady) {
    SyncAuthData (Global);
  }
  //
  // Special Handling for MOR Lock variable.
  //
  Status = SetVariableCheckHandlerMor (VariableName, VendorGuid, Attributes, DataSize, Data);
  if (Status == EFI_ALREADY_STARTED) {
    //
    // EFI_ALREADY_STARTED means the SetVariable() action is handled inside of SetVariableCheckHandlerMor().
    // Variable driver can just return SUCCESS.
    //
    Status = EFI_SUCCESS;
    goto Done;
  }
  if (EFI_ERROR (Status)) {
    goto Done;
  }

  if (VariableAtRuntime () || Attributes != 0) {
    VariableCount = 0;
    Status = FindVariableInAllRegions (
               VariableName,
               VendorGuid,
               &Variable,
               &VariableCount,
               Global
               );
    if (!EFI_ERROR (Status)) {
      //
      // Add policy to make sure we cannot update any variable which attribute doesn't have
      // EFI_VARIABLE_RUNTIME_ACCESS at runtime.
      //
      if (VariableAtRuntime () && (Variable.CurrPtr->Attributes & EFI_VARIABLE_RUNTIME_ACCESS) == 0) {
        Status = EFI_WRITE_PROTECTED;
        goto Done;
      }
      //
      // To prevent updating variable which with RO attribute in variable default region,
      // return EFI_WRITE_PROTECTED directly in this case.
      //
      if ((Variable.CurrPtr->Attributes & EFI_VARIABLE_DEFAULT_READY_ONLY) == EFI_VARIABLE_DEFAULT_READY_ONLY) {
        Status = EFI_WRITE_PROTECTED;
        goto Done;
      }

      //
      // If a preexisting variable is rewritten with different attributes, SetVariable() shall not
      // modify the variable and shall return EFI_INVALID_PARAMETER. Two exceptions to this rule:
      // 1. No access attributes specified
      // 2. The only attribute differing is EFI_VARIABLE_APPEND_WRITE
      // 3. Previous variable with EFI_VARIABLE_DEFAULT_READY_ONLY attribute (this attribtue must be different)
      //
      if (Attributes != 0 && (Attributes & (~EFI_VARIABLE_APPEND_WRITE)) != Variable.CurrPtr->Attributes) {
        Status = EFI_INVALID_PARAMETER;
        goto Done;
      }
    }
  }

  if (Attributes == 0) {
    Status = UpdateNoAccessAttribute (
               VariableName,
               VendorGuid,
               DataSize,
               Data,
               &Attributes
               );
    if (EFI_ERROR (Status)) {
      goto Done;
    }
    if (mSmst == NULL && !mVariableModuleGlobal->VariableWriteReady && (Attributes & EFI_VARIABLE_NON_VOLATILE) != 0) {
      return EFI_NOT_READY;
    }
  }

  VariableCount = 0;
  Status = FindVariableByLifetimeInAllRegions (VariableName, VendorGuid, &Variable, &VariableCount, Global);
  if (Status == EFI_INVALID_PARAMETER) {
    goto Done;
  }

  //
  // To prevent calling DeleteAllOldVariable () spend too much time, Getting variable count from variable cache
  // to detremine need call DeleteAllOldVariable () or not.
  //
  if (Variable.CurrPtr != NULL && (Variable.CurrPtr->Attributes & EFI_VARIABLE_NON_VOLATILE) &&
      GetVariableCountFromVariableCache (VariableName, VendorGuid) > 1) {
    DeleteAllOldVariable (VariableName, VendorGuid, Global);
    if (Variable.CurrPtr != GetMostValueVariable (&Variable)) {
      FindVariableByLifetimeInAllRegions (VariableName, VendorGuid, &Variable, &VariableCount, Global);
    }
  }

  if (VariableAtRuntime () && Attributes && (!(Attributes & EFI_VARIABLE_RUNTIME_ACCESS) || !(Attributes & EFI_VARIABLE_NON_VOLATILE))) {
    //
    // Runtime but Attribute is not Runtime or is volatile
    //
    Status = EFI_INVALID_PARAMETER;
  } else if ((Attributes & EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS) || (Attributes & EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS) ||
             IsSecureDatabaseVariable (VariableName, VendorGuid)) {
    Status = SetAuthVariable (
               VariableName,
               VendorGuid,
               Attributes,
               DataSize,
               Data,
               Global
               );
  } else {
    //
    // Only can update variable directly when variable doesn't exist or exist variable doesn't authenticated variable
    //
    Status = EFI_WRITE_PROTECTED;
    if ((Variable.CurrPtr == NULL) || (Variable.CurrPtr != NULL && (Variable.CurrPtr->Attributes & EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS) == 0 &&
        (Variable.CurrPtr->Attributes & EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS) == 0)) {
      Status = UpdateVariable (
                 VariableName,
                 VendorGuid,
                 Data,
                 DataSize,
                 Attributes,
                 0,
                 0,
                 &Variable,
                 0,
                 Global
                 );
      if (FeaturePcdGet (PcdH2OCustomizedSecureBootSupported) && Status == EFI_SUCCESS) {
        UpdateStateForModeVariablesChanged (VariableName, VendorGuid);
      }
    }
  }
Done:
  Global->ReentrantState--;
  ReleaseLockOnlyAtBootTime (&mVariableModuleGlobal->VariableBase.VariableServicesLock);
  return Status;
}


/**
  This code returns information about the EFI variables.

  @param Attributes                     Attributes bitmask to specify the type of variables
                                        on which to return information.
  @param MaximumVariableStorageSize     Pointer to the maximum size of the storage space available
                                        for the EFI variables associated with the attributes specified.
  @param RemainingVariableStorageSize   Pointer to the remaining size of the storage space available
                                        for EFI variables associated with the attributes specified.
  @param MaximumVariableSize            Pointer to the maximum size of an individual EFI variables


  @retval EFI_INVALID_PARAMETER         An invalid combination of attribute bits was supplied.
  @retval EFI_SUCCESS                   Query successfully.
  @retval EFI_UNSUPPORTED               The attribute is not supported on this platform.

**/
EFI_STATUS
EFIAPI
VariableServicesQueryVariableInfo (
  IN  UINT32                 Attributes,
  OUT UINT64                 *MaximumVariableStorageSize,
  OUT UINT64                 *RemainingVariableStorageSize,
  OUT UINT64                 *MaximumVariableSize
  )
{
  VARIABLE_HEADER        *Variable;
  VARIABLE_HEADER        *NextVariable;
  VARIABLE_HEADER        *VariableEndPointer;
  UINT64                 VariableSize;
  VARIABLE_STORE_HEADER  *VariableStoreHeader;
  VARIABLE_GLOBAL        *Global;
  UINTN                  VariableStoreSize;
  BOOLEAN                InteruptEnabled;

  if(MaximumVariableStorageSize == NULL || RemainingVariableStorageSize == NULL || MaximumVariableSize == NULL || Attributes == 0) {
    return EFI_INVALID_PARAMETER;
  }

  if ((Attributes & EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS) != 0) {
    //
    //  Deprecated attribute, make this check as highest priority.
    //
    return EFI_UNSUPPORTED;
  }

  if((Attributes & (EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_HARDWARE_ERROR_RECORD)) == 0) {
    //
    // Make sure the Attributes combination is supported by the platform.
    //
    return EFI_UNSUPPORTED;
  } else if ((Attributes & (EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_BOOTSERVICE_ACCESS)) == EFI_VARIABLE_RUNTIME_ACCESS) {
    //
    // Make sure if runtime bit is set, boot service bit is set also.
    //
    return EFI_INVALID_PARAMETER;
  } else if (VariableAtRuntime () && !(Attributes & EFI_VARIABLE_RUNTIME_ACCESS)) {
    //
    // Make sure RT Attribute is set if we are in Runtime phase.
    //
    return EFI_INVALID_PARAMETER;
  }
  //
  // In current code doesn't support EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS.
  //
  else if ((Attributes & EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS) != 0) {
    return EFI_INVALID_PARAMETER;
  }

  InteruptEnabled = GetInterruptState ();
  if (VariableAtRuntime () && InteruptEnabled) {
    DisableInterrupts ();
  }
  if (VariableAtRuntime () && FeaturePcdGet (PcdH2OVariableFlushCacheAtRuntime)) {
    WriteBackInvalidateDataCacheRange (
      mVariableModuleGlobal->NonVolatileVariableCache,
      mVariableModuleGlobal->NonVolatileVariableCacheSize + mVariableModuleGlobal->FactoryDefaultSize + mVariableModuleGlobal->VariableDefaultSize
      );
  }
  AcquireLockOnlyAtBootTime (&mVariableModuleGlobal->VariableBase.VariableServicesLock);

  Global = &mVariableModuleGlobal->VariableBase;
  if((Attributes & EFI_VARIABLE_NON_VOLATILE) == 0) {
    //
    // Query is Volatile related.
    //
    VariableStoreHeader = (VARIABLE_STORE_HEADER *) ((UINTN) Global->VolatileVariableBase);
    VariableEndPointer = GetEndPointer (VariableStoreHeader);
    VariableStoreSize = GetVariableStoreSize (VariableStoreHeader);
  } else {
    //
    // Query is Non-Volatile related.
    //
    VariableStoreHeader = (VARIABLE_STORE_HEADER *) ((UINTN) Global->NonVolatileVariableBase);
    VariableEndPointer = GetNonVolatileEndPointer (VariableStoreHeader);
    VariableStoreSize = GetNonVolatileVariableStoreSize ();
  }

  //
  // Now let's fill *MaximumVariableStorageSize *RemainingVariableStorageSize
  // with the storage size (excluding the storage header size).
  //
  *MaximumVariableStorageSize   = VariableStoreSize - GetVariableStoreHeaderSize ();
  *RemainingVariableStorageSize = VariableStoreSize - GetVariableStoreHeaderSize ();

  //
  // Let *MaximumVariableSize be MAX_VARIABLE_SIZE with the exception of the variable header size.
  //
  *MaximumVariableSize = MAX_VARIABLE_SIZE - sizeof (VARIABLE_HEADER);
  //
  // Harware error record variable needs larger size.
  //
  if ((Attributes & EFI_VARIABLE_HARDWARE_ERROR_RECORD) == EFI_VARIABLE_HARDWARE_ERROR_RECORD) {
    *MaximumVariableSize = MAX_HARDWARE_ERROR_VARIABLE_SIZE - sizeof (VARIABLE_HEADER);
  }

  //
  // Point to the starting address of the variables.
  //
  Variable = GetStartPointer (VariableStoreHeader);

  //
  // Now walk through the related variable store.
  //
  while (IsValidVariableHeaderInVarRegion (Variable, VariableEndPointer)) {
    NextVariable = GetNextVariablePtr (Variable);
    VariableSize = (UINT64) (UINTN) NextVariable - (UINT64) (UINTN) Variable;

    if (VariableAtRuntime ()) {
      //
      // we don't take the state of the variables in mind
      // when calculating RemainingVariableStorageSize,
      // since the space occupied by variables not marked with
      // VAR_ADDED is not allowed to be reclaimed in Runtime.
      //
      *RemainingVariableStorageSize -= VariableSize;
    } else {
      //
      // Only care about Variables with State VAR_ADDED,because
      // the space not marked as VAR_ADDED is reclaimable now.
      //
      if ((Variable->State == VAR_ADDED) ||
          (Variable->State == (VAR_ADDED & VAR_IN_DELETED_TRANSITION))) {
        *RemainingVariableStorageSize -= VariableSize;
      }
    }

    //
    // Go to the next one
    //
    Variable = NextVariable;
  }

  if (*RemainingVariableStorageSize < sizeof (VARIABLE_HEADER)) {
    *MaximumVariableSize = 0;
  } else if ((*RemainingVariableStorageSize - sizeof (VARIABLE_HEADER)) < *MaximumVariableSize) {
    *MaximumVariableSize = *RemainingVariableStorageSize - sizeof (VARIABLE_HEADER);
  }
  ReleaseLockOnlyAtBootTime (&mVariableModuleGlobal->VariableBase.VariableServicesLock);
  if (VariableAtRuntime () && InteruptEnabled) {
    EnableInterrupts ();
  }
  return EFI_SUCCESS;
}

/**
  This function uses to restore default variable store header to non-volatile sotrage

  @param VariableStoreHeader     pointer the variable store header in non-volatile sotrage.

  @retval EFI_SUCCESS            The restore process is success.
  @retval EFI_INVALID_PARAMETER  VariableStoreHeader parameter is NULL.
  @return EFI_VOLUME_CORRUPTED   Firmware Volume is corrupted.

--*/
EFI_STATUS
RestoreDefaultVariableStoreHeader (
  IN  VARIABLE_STORE_HEADER                 *VariableStoreHeader
  )
{
  VARIABLE_STORE_HEADER                  WorkingStoreHeader;
  ECP_VARIABLE_STORE_HEADER              EcpVarStoreHeader;
  EFI_STATUS                             Status;
  UINT32                                 DataSize;
  UINT8                                  *Buffer;

  if (VariableStoreHeader == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (PcdGetBool (PcdUseEcpVariableStoreHeader)) {
    SetMem (&EcpVarStoreHeader, sizeof (EcpVarStoreHeader), 0xFF);
    EcpVarStoreHeader.Signature = ECP_VARIABLE_STORE_SIGNATURE;
    EcpVarStoreHeader.Format    = VARIABLE_STORE_FORMATTED;
    EcpVarStoreHeader.State     = VARIABLE_STORE_HEALTHY;
    EcpVarStoreHeader.DefaultId = 0;
    EcpVarStoreHeader.BoardId   = 0;
    EcpVarStoreHeader.Flags     = 0;
    EcpVarStoreHeader.Reserved  = 0;
    DataSize = sizeof (EcpVarStoreHeader);
    Buffer = (UINT8 *)&EcpVarStoreHeader;
  } else {
    SetMem (&WorkingStoreHeader, sizeof (WorkingStoreHeader), 0xFF);
    CopyMem (
      &WorkingStoreHeader.Signature,
      &gEfiAuthenticatedVariableGuid,
      sizeof (EFI_GUID)
      );
    WorkingStoreHeader.Format    = VARIABLE_STORE_FORMATTED;
    WorkingStoreHeader.State     = VARIABLE_STORE_HEALTHY;
    WorkingStoreHeader.DefaultId = 0;
    WorkingStoreHeader.BoardId   = 0;
    WorkingStoreHeader.Flags     = 0;
    WorkingStoreHeader.Reserved  = 0;
    DataSize = sizeof (WorkingStoreHeader);
    Buffer = (UINT8 *)&WorkingStoreHeader;
  }
  Status = UpdateVariableStore (
           &mVariableModuleGlobal->VariableBase,
           FALSE,
           FALSE,
           (UINTN) VariableStoreHeader,
           DataSize,
           Buffer
           );
  if (EFI_ERROR (Status) || CompareMem (Buffer, VariableStoreHeader, DataSize) != 0) {
    return EFI_VOLUME_CORRUPTED;
  }
  return EFI_SUCCESS;
}

/**
  Internal function to clear variable storage firmware volume

  @param [in] FvHeader             Pointer to firmware volume header
  @param [in] VariableStoreLength  Variable storage length in bytes.
**/
STATIC
VOID
ResetFirmwareVolume (
  IN  EFI_FIRMWARE_VOLUME_HEADER            *FvHeader,
  IN  UINTN                                 VariableStoreLength
  )
{
  EFI_STATUS                             Status;
  EFI_FIRMWARE_VOLUME_BLOCK_PROTOCOL     *FvbProtocol;

  Status = GetFvbInfoByAddress ((EFI_PHYSICAL_ADDRESS) (UINTN) FvHeader, NULL, &FvbProtocol);
  if (EFI_ERROR (Status)) {
    return;
  }
  //
  // Variable Store header is not reliable, clean up whole variable region.
  // EFI_FIRMWARE_VOLUME_HEADER is reliable since it passed by FVB driver.
  // Temp to use VariableStoreLength/BlockLength as num of LBA.
  //
  Status = FvbProtocol->EraseBlocks (FvbProtocol, 0, VariableStoreLength / FvHeader->BlockMap[0].Length, EFI_LBA_LIST_TERMINATOR);
  //
  // force system reboot to recovery!!!
  //
  gST->RuntimeServices->ResetSystem (EfiResetCold, EFI_SUCCESS, 0, NULL);

}

/**
  Determine need write back to variable defaults or not.

  @retval TRUE    System need write back to variable defaults.
  @retval FALSE   System need not write back to variable defaults.
**/
STATIC
BOOLEAN
WriteVariableDefaults (
  VOID
  )
{
  EFI_HOB_GUID_TYPE                               *GuidHob;
  H2O_DXE_CP_WRITE_VARIABLE_DEFAULTS_DATA         WriteVariableDefaultsData;

  GuidHob = GetFirstGuidHob (&gH2OFlashMapRegionVarGuid);
  if (GuidHob == NULL) {
    return FALSE;
  }

  if (FeaturePcdGet (PcdH2ODxeCpWriteVariableDefaultsSupported)) {
    WriteVariableDefaultsData.Size    = sizeof (H2O_DXE_CP_WRITE_VARIABLE_DEFAULTS_DATA);
    WriteVariableDefaultsData.Status  = H2O_CP_TASK_NORMAL;
    DEBUG ((DEBUG_INFO, "Checkpoint Trigger: %g\n", &gH2ODxeCpWriteVariableDefaultsGuid));
    H2OCpTrigger (&gH2ODxeCpWriteVariableDefaultsGuid, &WriteVariableDefaultsData);
    DEBUG ((DEBUG_INFO, "Checkpoint Result: %x\n", WriteVariableDefaultsData.Status));
    if (WriteVariableDefaultsData.Status == H2O_CP_TASK_SKIP) {
      return FALSE;
    }
  }
  return TRUE;
}
/**
  Internal function to restore whole variable storage region to defaults.

  @param [in] FvHeader             Pointer to firmware volume header
  @param [in] VariableStoreLength  Variable storage length in bytes.
**/
STATIC
VOID
WriteBackToVariableDefaults (
  IN  EFI_FIRMWARE_VOLUME_HEADER            *FvHeader,
  IN  UINTN                                 VariableStoreLength
  )
{
  EFI_STATUS                             Status;
  UINT8                                  *FvData;
  UINTN                                  HeaderLength;
  EFI_FIRMWARE_VOLUME_BLOCK_PROTOCOL     *FvbProtocol;
  VARIABLE_STORE_HEADER                  *VariableStoreHeader;

  HeaderLength = FvHeader->HeaderLength;
  FvData = AllocatePool (HeaderLength);
  if (FvData == NULL) {
    return;
  }
  CopyMem (FvData, FvHeader, HeaderLength);
  Status = GetFvbInfoByAddress ((EFI_PHYSICAL_ADDRESS) (UINTN) FvHeader, NULL, &FvbProtocol);
  if (EFI_ERROR (Status)) {
    return;
  }
  FvbProtocol->EraseBlocks (FvbProtocol, 0, (VariableStoreLength + HeaderLength) / FvHeader->BlockMap[0].Length, EFI_LBA_LIST_TERMINATOR);
  FvbProtocol->Write (FvbProtocol, 0, 0, &HeaderLength, FvData);
  VariableStoreHeader = (VARIABLE_STORE_HEADER *)((UINT8 *) FvHeader + FvHeader->HeaderLength);
  RestoreDefaultVariableStoreHeader (VariableStoreHeader);
  UpdateVariableStore (
    &mVariableModuleGlobal->VariableBase,
    FALSE,
    FALSE,
    PcdGetBool (PcdUseEcpVariableStoreHeader) ? (UINTN) &((ECP_VARIABLE_STORE_HEADER *) VariableStoreHeader)->Size : (UINTN) &VariableStoreHeader->Size,
    sizeof (UINT32),
    (UINT8 *) &VariableStoreLength
    );
  FreePool (FvData);
}

/**
  This function uses to initialize volatile variable store header

  @param  VolatileVariableStore   pointer to volatile variable store which want to initialize

  @retval EFI_SUCCESS             Initialize volatile variable store successful.
  @retval EFI_INVALID_PARAMETER   VolatileVariableStore pointer to NULL.

**/
STATIC
EFI_STATUS
InitializeVolatileVariableStoreHeader (
  IN OUT VARIABLE_STORE_HEADER                 *VolatileVariableStore
  )
{
  ECP_VARIABLE_STORE_HEADER     *EcpVarStoreHeader;

  if (VolatileVariableStore == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (PcdGetBool (PcdUseEcpVariableStoreHeader)) {
    EcpVarStoreHeader = (ECP_VARIABLE_STORE_HEADER *) VolatileVariableStore;
    EcpVarStoreHeader->Signature                  = ECP_VARIABLE_STORE_SIGNATURE;
    EcpVarStoreHeader->Size                       = VARIABLE_STORE_SIZE;
    EcpVarStoreHeader->Format                     = VARIABLE_STORE_FORMATTED;
    EcpVarStoreHeader->State                      = VARIABLE_STORE_HEALTHY;
    EcpVarStoreHeader->DefaultId                  = 0;
    EcpVarStoreHeader->BoardId                    = 0;
    EcpVarStoreHeader->Flags                      = 0;
    EcpVarStoreHeader->Reserved                   = 0;
  } else {
    CopyGuid (&VolatileVariableStore->Signature, &gEfiAuthenticatedVariableGuid);
    VolatileVariableStore->Size                   = VARIABLE_STORE_SIZE;
    VolatileVariableStore->Format                 = VARIABLE_STORE_FORMATTED;
    VolatileVariableStore->State                  = VARIABLE_STORE_HEALTHY;
    VolatileVariableStore->DefaultId              = 0;
    VolatileVariableStore->BoardId                = 0;
    VolatileVariableStore->Flags                  = 0;
    VolatileVariableStore->Reserved               = 0;
  }
  mVariableModuleGlobal->VariableBase.VolatileVariableBase = (EFI_PHYSICAL_ADDRESS) (UINTN) VolatileVariableStore;
  mVariableModuleGlobal->VolatileLastVariableOffset = GetVariableStoreHeaderSize ();

  return EFI_SUCCESS;
}

/**
  Build the variable working region record for use of other drivers.

  @param[in] Address
  @param[in] Length

  @retval EFI_SUCCESS           Build variable working region record successful.
  @retval EFI_OUT_OF_RESOURCES  Allocate memory failed.
  @retval Other                 Install configuration table failed.
**/
EFI_STATUS
BuildVariableWorkingRegionRecord (
  IN EFI_PHYSICAL_ADDRESS              Address,
  IN EFI_PHYSICAL_ADDRESS              Length
  )
{
  VARIABLE_WORKING_REGION_INFO        *VariableWorkingRegionInfo;

  VariableWorkingRegionInfo = VariableAllocateZeroBuffer (sizeof(VARIABLE_WORKING_REGION_INFO), TRUE);
  if (VariableWorkingRegionInfo == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  VariableWorkingRegionInfo->VariableWorkingRegionStart  = Address;
  VariableWorkingRegionInfo->VariableWorkingRegionLength = Length;
  return gBS->InstallConfigurationTable (&mVariableWorkingRegionHobGuid, VariableWorkingRegionInfo);
}

/**
 Check the integrity of firmware volume header.

 @param[in] FwVolHeader  A pointer to a firmware volume header.

 @retval TRUE     The firmware volume is consistent.
 @retval FALSE    The firmware volume has corrupted. So it is not an FV.
**/
STATIC
BOOLEAN
ValidateFvHeader (
  IN EFI_FIRMWARE_VOLUME_HEADER            *FwVolHeader
  )
{
  //
  // Verify the header revision, header signature, length
  // Length of FvBlock cannot be 2**64-1
  // HeaderLength cannot be an odd number
  //
  if ((FwVolHeader->Revision != EFI_FVH_REVISION) ||
      (FwVolHeader->Signature != EFI_FVH_SIGNATURE) ||
      (FwVolHeader->FvLength == ((UINTN) -1)) ||
      ((FwVolHeader->HeaderLength & 0x01) != 0)) {
    return FALSE;
  }

  //
  // Verify the header checksum
  //
  if (CalculateCheckSum16 ((UINT16 *) FwVolHeader, FwVolHeader->HeaderLength) != 0) {
    return FALSE;
  }

  return TRUE;
}


/**
  This function uses to flush current variable default data to variable default cache

  @param  CacheBaseAddress         pointer to variable default cache base address.
  @param  VarDefaultBaseAddress   pointer to variable default base address.
  @param  CacheSize               Variable default cache size.

  @return EFI_SUCCESS             Flush variable default data to variable default cache successful.
  @return EFI_INVALID_PARAMETER   CacheBaseAddress or VarDefaultBaseAddress is NULL.
  @return EFI_BUFFER_TOO_SMALL    CacheSize is too small.

**/
STATIC
EFI_STATUS
FlushVariableDefaultCache (
  IN  UINT8    *CacheBaseAddress,
  IN  UINT8    *VarDefaultBaseAddress,
  IN  UINTN    CacheSize
  )
{
  UINTN                                 LastOffset;
  VARIABLE_STORE_HEADER                 *VarStoreHeader;

  if (CacheBaseAddress == NULL || VarDefaultBaseAddress == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  for (VarStoreHeader = (VARIABLE_STORE_HEADER *) VarDefaultBaseAddress;
       GetVariableStoreStatus (VarStoreHeader) == EfiValid;
       VarStoreHeader = (VARIABLE_STORE_HEADER *) ((UINTN) VarStoreHeader +  GetVariableStoreSize (VarStoreHeader)));

  LastOffset = (UINTN) VarStoreHeader - (UINTN) VarDefaultBaseAddress;
  if (CacheSize < LastOffset) {
    return  EFI_BUFFER_TOO_SMALL;
  }

  SetMem (CacheBaseAddress, CacheSize, 0xff);
  CopyMem (CacheBaseAddress, VarDefaultBaseAddress, LastOffset);

  return EFI_SUCCESS;
}

/**
  This function uses to flush current factory default data to factory default cache

  @param  CacheBaseAddress            pointer to factory default cache base address.
  @param  FactoryDefaultBaseAddress  pointer to factory default base address.
  @param  CacheSize                  Variable cache size.

  @return EFI_SUCCESS             Flush factory default data to factory default cache successful.
  @return EFI_INVALID_PARAMETER   CacheBaseAddress or FactoryDefaultBaseAddress is NULL.
  @return EFI_BUFFER_TOO_SMALL    CacheSize is too small.

**/
STATIC
EFI_STATUS
FlushFactoryDefaultCache (
  IN  UINT8    *CacheBaseAddress,
  IN  UINT8    *FactoryDefaultBaseAddress,
  IN  UINTN    CacheSize
  )
{
  VARIABLE_HEADER                       *NextVariable;
  UINTN                                 HeaderSize;
  UINTN                                 LastOffset;

  if (CacheBaseAddress == NULL || FactoryDefaultBaseAddress == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  HeaderSize = sizeof (UINT64) + sizeof (EFI_FIRMWARE_VOLUME_HEADER) + sizeof (EFI_FV_BLOCK_MAP_ENTRY) + GetVariableStoreHeaderSize ();
  NextVariable  = (VARIABLE_HEADER *) (FactoryDefaultBaseAddress + HeaderSize);
  while (IsValidVariableHeader (NextVariable)) {
    NextVariable = GetNextVariablePtr (NextVariable);
  }

  LastOffset = (UINTN) NextVariable - (UINTN) FactoryDefaultBaseAddress;
  if (CacheSize < LastOffset) {
    return  EFI_BUFFER_TOO_SMALL;
  }

  SetMem (CacheBaseAddress, CacheSize, 0xff);
  CopyMem (CacheBaseAddress, FactoryDefaultBaseAddress, LastOffset);
  return EFI_SUCCESS;
}

/**
 Internal function to initialize variable information base address.

 @retval EFI_SUCCESS           Init variable information related base address successfully.
 @retval EFI_NOT_READY         firmware volume to store variable data is corrupted.
 @retval EFI_BUFFER_TOO_SMALL  Unable flush variable data to variable cache because
                               all of variable data size is larger than variable cache size.
 @retval EFI_OUT_OF_RESOURCES  There are not enough memory to allocate.
 @return Other                 Other error occurred in this function.
**/
EFI_STATUS
InitVariableBaseAddress (
  VOID
  )
{
  EFI_FIRMWARE_VOLUME_HEADER            *FvHeader;
  VARIABLE_STORE_HEADER                 *VariableStoreHeader;
  UINT64                                VariableStoreLength;
  EFI_STATUS                            Status;
  VARIABLE_STORE_HEADER                 *VolatileVariableStore;
  UINTN                                 AllocateSize;

  //
  // memory address has been initialized so return directly.
  //
  if (mVariableModuleGlobal->VariableBase.NonVolatileVariableBase != 0) {
    return EFI_SUCCESS;
  }
  //
  // firmware volume header is invalid return directly. expected this function will be invoked
  // after restoring correct firmware volume header.
  //
  FvHeader = (EFI_FIRMWARE_VOLUME_HEADER *) (UINTN) FdmGetNAtAddr (&gH2OFlashMapRegionVarGuid, 1);
  if (!ValidateFvHeader (FvHeader)) {
    return EFI_NOT_READY;
  }

  VariableStoreHeader = (VARIABLE_STORE_HEADER *) ((UINT8 *) FvHeader + FvHeader->HeaderLength);

  mVariableModuleGlobal->VariableBase.NonVolatileVariableBase = (EFI_PHYSICAL_ADDRESS) (UINTN) VariableStoreHeader;
  VariableStoreLength =  FdmGetNAtSize (&gH2OFlashMapRegionVarGuid, 1) - FvHeader->HeaderLength;

  //
  // Since memory for non-volatile variable cache and volatile variable may be modified after start image, so combine these two
  // memory and save the start address and length in configuration table for other driver use.
  //
  AllocateSize = VARIABLE_STORE_SIZE + SCRATCH_SIZE + (UINTN) VariableStoreLength + mVariableModuleGlobal->FactoryDefaultSize + mVariableModuleGlobal->VariableDefaultSize;
  VolatileVariableStore = VariableAllocateZeroBuffer (AllocateSize, TRUE);
  if (VolatileVariableStore == NULL) {
    POST_CODE (DXE_VARIABLE_INIT_FAIL);
    Status = EFI_OUT_OF_RESOURCES;
    goto Done;
  }
  Status = BuildVariableWorkingRegionRecord (
           (EFI_PHYSICAL_ADDRESS) (UINTN) VolatileVariableStore,
           (EFI_PHYSICAL_ADDRESS) AllocateSize
           );
  if (EFI_ERROR (Status)) {
    goto Done;
  }
  SetMem (VolatileVariableStore, VARIABLE_STORE_SIZE + SCRATCH_SIZE, 0xff);
  Status = InitializeVolatileVariableStoreHeader (VolatileVariableStore);
  ASSERT_EFI_ERROR (Status);


  //
  // Allocate EfiRuntimeServicesData memory type for variable cache, and then we can use this memory as variable cache in
  // POST time and runtime. system will pass this variable cache address to SMM mode driver. Finally, system can access this
  // variable cache in SMM mode and protected mode. But we must disable cache mechanism if LegacyBoot event is signaled, due to
  // EfiRuntimeServicesData memory is used by legacy OS.
  //
  mVariableModuleGlobal->NonVolatileVariableCache = (UINT8 *) VolatileVariableStore + VARIABLE_STORE_SIZE + SCRATCH_SIZE;
  mVariableModuleGlobal->NonVolatileVariableCacheSize = (UINTN) VariableStoreLength;

  Status = FlushVariableCache (
             mVariableModuleGlobal->NonVolatileVariableCache,
             (UINT8 *) (UINTN) mVariableModuleGlobal->VariableBase.NonVolatileVariableBase,
             mVariableModuleGlobal->NonVolatileVariableCacheSize,
             &mVariableModuleGlobal->NonVolatileLastVariableOffset
             );
  SetNonVolatileVariableCacheCrc32 ();
  mVariableModuleGlobal->FactoryDefaultCache = mVariableModuleGlobal->NonVolatileVariableCache + mVariableModuleGlobal->NonVolatileVariableCacheSize;
  Status = FlushFactoryDefaultCache (
             mVariableModuleGlobal->FactoryDefaultCache,
             (UINT8 *) (UINTN) mVariableModuleGlobal->FactoryDefaultBase,
             mVariableModuleGlobal->FactoryDefaultSize
             );
  mVariableModuleGlobal->VariableDefaultCache = mVariableModuleGlobal->FactoryDefaultCache + mVariableModuleGlobal->FactoryDefaultSize;
  Status = FlushVariableDefaultCache (
             mVariableModuleGlobal->VariableDefaultCache,
             (UINT8 *) (UINTN) mVariableModuleGlobal->VariableDefaultBase,
             mVariableModuleGlobal->VariableDefaultSize
             );
Done:
  if (Status != EFI_SUCCESS && Status != EFI_BUFFER_TOO_SMALL) {
    mVariableModuleGlobal->VariableBase.NonVolatileVariableBase = 0;
  }
  return Status;
}

/**
  Initializes read-only variable services.

  @retval EFI_SUCCESS           Function successfully executed.
  @retval EFI_OUT_OF_RESOURCES  Fail to allocate enough memory resource.
  @return Others                Ohter error occurred in this function.
**/
EFI_STATUS
VariableReadyOnlyInitialize (
  VOID
  )
{
  EFI_GUID                 VariableDefaultId  = FDM_VARIABLE_DEFAULT_ID_VARIABLE_DEFAULT;

  if (GetBootModeHob () == BOOT_IN_RECOVERY_MODE) {
    return EFI_UNSUPPORTED;
  }

  mVariableModuleGlobal = VariableAllocateZeroBuffer (sizeof (ESAL_VARIABLE_GLOBAL), TRUE);
  if (mVariableModuleGlobal == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  EfiInitializeLock (&mVariableModuleGlobal->VariableBase.VariableServicesLock, TPL_NOTIFY);

  mVariableModuleGlobal->FactoryDefaultBase = FdmGetAddressById (&gH2OFlashMapRegionVarDefaultGuid, &gH2OFlashMapRegionFactoryCopyGuid, 1);
  mVariableModuleGlobal->FactoryDefaultSize = (UINTN) FdmGetSizeById (&gH2OFlashMapRegionVarDefaultGuid, &gH2OFlashMapRegionFactoryCopyGuid, 1);

  mVariableModuleGlobal->VariableDefaultBase = FdmGetAddressById (&gH2OFlashMapRegionVarDefaultGuid, &VariableDefaultId, 1);
  mVariableModuleGlobal->VariableDefaultSize = (UINTN) FdmGetSizeById (&gH2OFlashMapRegionVarDefaultGuid, &VariableDefaultId, 1);

  InitVariableBaseAddress ();
  return EFI_SUCCESS;
}


/**
  Function to provide runtime type memory to store variable data which can be used to
  communicate with variable code in SMM.

  @param[out]    Buffer             Pointer to the return runtime type memory.
  @param[out]    BufferSize         The size of the return buffer.

  @retval  EFI_SUCCESS              Get runtime type memory successfully.
  @retval  EFI_NOT_FOUND            Cannot find runtime type memory.
  @retval  EFI_INVALID_PARAMETER    Buffer is NULL or BufferSize is NULL.
**/
EFI_STATUS
GetRuntimeVariableBuffer (
  OUT    VOID        **Buffer,
  OUT     UINTN      *BufferSize
  )
{
  if (Buffer == NULL || BufferSize == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  if (mSmmVarBuf == NULL) {
    return EFI_NOT_FOUND;
  }

  *Buffer     = mSmmVarBuf;
  *BufferSize = sizeof (SMM_VAR_BUFFER) + MAX_VARIABLE_NAME_SIZE + MAX_VARIABLE_SIZE;
  return EFI_SUCCESS;
}

/**
 gH2OBdsCpDxeSmmReadyToLockBeforeGuid CP handler to notify the gH2OBdsCpDxeSmmReadyToLockBeforeGuid
 CP is triggered.

 @param[in] Event           A pointer to the Event that triggered the callback.
 @param[in] Handle          Checkpoint handle.

**/
VOID
EFIAPI
VariableDxeSmmReadyToLockCpHandler (
  IN EFI_EVENT                              Event,
  IN H2O_CP_HANDLE                          Handle
  )
{
  H2OCpUnregisterHandler (Handle);
  mDxeSmmReadyToLockBeforeTriggered = TRUE;
  ReclaimForOS ();
}

/**
  Initializes variable store area for non-volatile and volatile variable.

  @retval EFI_SUCCESS           Function successfully executed.
  @retval EFI_OUT_OF_RESOURCES  Fail to allocate enough memory resource.

**/
EFI_STATUS
VariableCommonInitialize (
  VOID
  )
{
  EFI_STATUS                            Status;
  EFI_STATUS                            InfoStatus;
  VARIABLE_STORE_HEADER                 *VariableStoreHeader;
  UINT64                                VariableStoreLength;
  EFI_HANDLE                            NewHandle;
  EFI_EVENT                             Event;
  EFI_FIRMWARE_VOLUME_HEADER            *FvHeader;
  EFI_FIRMWARE_VOLUME_BLOCK_PROTOCOL    *FvbProtocol;
  EFI_PHYSICAL_ADDRESS                  FvBaseAddress;
  UINT32                                MaxUserNvVariableSpaceSize;
  H2O_CP_HANDLE                         CpHandle;

  mSmmVarBuf = VariableAllocateZeroBuffer (sizeof (SMM_VAR_BUFFER) + MAX_VARIABLE_NAME_SIZE + MAX_VARIABLE_SIZE, TRUE);
  if (mSmmVarBuf == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto Shutdown;
  }
  mSmmPhyVarBuf = mSmmVarBuf;
  mVariableModuleGlobal->SmmVarBuf      = mSmmVarBuf;
  mVariableModuleGlobal->SmmVarBufSize  = sizeof (SMM_VAR_BUFFER) + MAX_VARIABLE_NAME_SIZE + MAX_VARIABLE_SIZE;
  //
  // Reserved MAX_VARIABLE_SIZE runtime buffer for "Append" operation in virtual mode.
  //
  mStorageArea  = VariableAllocateZeroBuffer (APPEND_BUFF_SIZE, TRUE);
  if (mStorageArea == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto Shutdown;
  }
  //
  // Reserved MAX_VARIABLE_SIZE runtime buffer for certificated database list (normal time based authenticated variable)
  // operation in virtual mode.
  //
  mCertDbList  = VariableAllocateZeroBuffer (MAX_VARIABLE_SIZE, TRUE);
  if (mCertDbList == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto Shutdown;
  }

  //
  // Allocate memory for volatile variable store
  //
  POST_CODE (DXE_VARIABLE_INIT);

  mVariableModuleGlobal->VarCheckVariableList = &mVarCheckVariableList;

  FvHeader = (EFI_FIRMWARE_VOLUME_HEADER *) (UINTN) FdmGetNAtAddr (&gH2OFlashMapRegionVarGuid, 1);
  VariableStoreHeader = (VARIABLE_STORE_HEADER *) ((UINT8 *) FvHeader + FvHeader->HeaderLength);
  VariableStoreLength =  FdmGetNAtSize (&gH2OFlashMapRegionVarGuid, 1) - FvHeader->HeaderLength;
  Status = GetFvbInfoByAddress ((EFI_PHYSICAL_ADDRESS) (UINTN) FvHeader, NULL, &FvbProtocol);
  if (EFI_ERROR (Status)) {
    goto Shutdown;
  }
  mVariableModuleGlobal->FvbInstance = FvbProtocol;

  //
  // To prevent from variable store header is corrupted (may be caused by other tool),
  // restore to default variable store header to make sure variable service can work
  // properly.
  //
  if (GetVariableStoreStatus (VariableStoreHeader) != EfiValid) {
    Status = RestoreDefaultVariableStoreHeader (VariableStoreHeader);
    if (EFI_ERROR (Status)) {
      ResetFirmwareVolume (FvHeader, (UINTN) (VariableStoreLength + FvHeader->HeaderLength));
      goto Shutdown;
    }
  }

  if (~GetVariableStoreSize (VariableStoreHeader) == 0) {
    Status = UpdateVariableStore (
              &mVariableModuleGlobal->VariableBase,
              FALSE,
              FALSE,
              PcdGetBool (PcdUseEcpVariableStoreHeader) ? (UINTN) &((ECP_VARIABLE_STORE_HEADER *) VariableStoreHeader)->Size : (UINTN) &VariableStoreHeader->Size,
              sizeof (UINT32),
              (UINT8 *) &VariableStoreLength
              );

    if (EFI_ERROR (Status)) {
      goto Shutdown;
    }
  }

  if (WriteVariableDefaults ()) {
    REPORT_STATUS_CODE (
      EFI_PROGRESS_CODE,
      EFI_SOFTWARE_EFI_RUNTIME_SERVICE | EFI_SW_EC_CFG_CLR_REQUEST
      );
    WriteBackToVariableDefaults (FvHeader, (UINTN)VariableStoreLength);
  }

  InitializeInsydeVariableLockedState ();

  MaxUserNvVariableSpaceSize = PcdGet32 (PcdMaxUserNvVariableSpaceSize);
  ASSERT (MaxUserNvVariableSpaceSize < (VariableStoreLength - sizeof (VARIABLE_STORE_HEADER)));
  mVariableModuleGlobal->CommonMaxUserVariableSpace = MaxUserNvVariableSpaceSize;

  //
  // Allocate reclaim related memory before doing reclaim
  //
  mVariableReclaimInfo = VariableAllocateZeroBuffer (sizeof (VARIABLE_RECLAIM_INFO), TRUE);
  if (mVariableReclaimInfo == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto Shutdown;
  }
  mVariableModuleGlobal->ReclaimInfo = mVariableReclaimInfo;
  Status = gBS->LocateProtocol (
                  &gEfiFaultTolerantWriteLiteProtocolGuid,
                  NULL,
                  (VOID **)&mVariableReclaimInfo->FtwLiteProtocol
                  );
  if (EFI_ERROR (Status)) {
    goto Shutdown;
  }

  Status = InitVariableBaseAddress ();
  if (Status != EFI_SUCCESS && Status != EFI_BUFFER_TOO_SMALL) {
    goto Shutdown;
  }

  mVariableReclaimInfo->BackupBuffer = VariableAllocateZeroBuffer (GetNonVolatileVariableStoreSize () + GetVariableStoreHeaderSize (), TRUE);
  if (mVariableReclaimInfo->BackupBuffer == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto Shutdown;
  }

  InfoStatus = GetVariableStoreInfo (
                 mVariableModuleGlobal->VariableBase.NonVolatileVariableBase,
                 &FvBaseAddress,
                 &mVariableReclaimInfo->VariableLba,
                 &mVariableReclaimInfo->VariableOffset
                 );
  if (InfoStatus != EFI_SUCCESS) {
    goto Shutdown;
  }
  if (Status == EFI_BUFFER_TOO_SMALL) {
    ASSERT (FALSE);

    //
    // If variable data size is large than cache size, there are something wrong in variable data.
    // Try to use reclaim mechanism to fix this error.
    //
    Status = Reclaim (
               mVariableModuleGlobal->VariableBase.NonVolatileVariableBase,
               &mVariableModuleGlobal->NonVolatileLastVariableOffset,
               FALSE,
               NULL
               );
    if (EFI_ERROR (Status)) {
      goto Shutdown;
    }
    Status = FlushVariableCache (
               mVariableModuleGlobal->NonVolatileVariableCache,
               (UINT8 *) (UINTN) mVariableModuleGlobal->VariableBase.NonVolatileVariableBase,
               mVariableModuleGlobal->NonVolatileVariableCacheSize,
               &mVariableModuleGlobal->NonVolatileLastVariableOffset
               );
    SetNonVolatileVariableCacheCrc32 ();
  }

  if (EFI_ERROR (Status)) {
    goto Shutdown;
  }

  //
  // Reclaim if the free area is blow a threshold to release some non-volatile space
  //
  if (GetNonVolatileVariableStoreSize () - mVariableModuleGlobal->NonVolatileLastVariableOffset < VARIABLE_RECLAIM_THRESHOLD) {
    Status = Reclaim (
               mVariableModuleGlobal->VariableBase.NonVolatileVariableBase,
               &mVariableModuleGlobal->NonVolatileLastVariableOffset,
               FALSE,
               NULL
               );
    if (EFI_ERROR (Status)) {
      goto Shutdown;
    }
  }
  mVariableModuleGlobal->GlobalVariableList = (VOID *) mGlobalVariableList;
  mVariableModuleGlobal->NonVolatileVariableProtocol.GetRuntimeVariableBuffer = GetRuntimeVariableBuffer;
  //
  // Install gEfiNonVolatileVariableProtocolGuid to let platform know support
  // non-volatile variable and also passes some information to SMM variable driver.
  //
  NewHandle = NULL;
  Status = gBS->InstallProtocolInterface (
                  &NewHandle,
                  &gEfiNonVolatileVariableProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  mVariableModuleGlobal
                  );
  if (EFI_ERROR (Status)) {
    goto Shutdown;
  }
  RegisterEventToDisableSecureBoot ();
  Status = EfiCreateEventReadyToBootEx (
             TPL_NOTIFY,
             ReadyToBootCallback,
             NULL,
             &Event
             );
  if (EFI_ERROR (Status)) {
    goto Shutdown;
  }
  Status = EfiCreateEventLegacyBootEx (
             TPL_NOTIFY,
             LegacyBootCallback,
             NULL,
             &Event
             );
  if (EFI_ERROR (Status)) {
    goto Shutdown;
  }

  InitializeRestoreVariableDefaultService ();
  MorLockInit ();
  //
  // Register the event handling function to set the End Of DXE flag.
  //
  Status = gBS->CreateEventEx (
                  EVT_NOTIFY_SIGNAL,
                  TPL_NOTIFY,
                  OnEndOfDxe,
                  NULL,
                  &gEfiEndOfDxeEventGroupGuid,
                  &Event
                  );
  ASSERT_EFI_ERROR (Status);
  mVariableModuleGlobal->VariableWriteReady = TRUE;
  if (FeaturePcdGet (PcdH2OBdsCpDxeSmmReadyToLockBeforeSupported)) {
    //
    // Register checkpoint of gH2OBdsCpDxeSmmReadyToLockBeforeGuid to lockdown flash part protection
    //
    Status = H2OCpRegisterHandler (
               &gH2OBdsCpDxeSmmReadyToLockBeforeGuid,
               VariableDxeSmmReadyToLockCpHandler,
               H2O_CP_MEDIUM,
               &CpHandle
               );
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "Checkpoint Register Fail: %g (%r)\n", &gH2OBdsCpDxeSmmReadyToLockBeforeGuid, Status));
      return Status;
    }
    DEBUG ((DEBUG_INFO, "Checkpoint Registered: %g (%r)\n", &gH2OBdsCpDxeSmmReadyToLockBeforeGuid, Status));
  }
  return Status;

Shutdown:

  if (mVariableModuleGlobal != NULL) {
    if (mVariableModuleGlobal->VariableBase.VolatileVariableBase != 0) {
      gBS->FreePool ((VOID *) (UINTN) mVariableModuleGlobal->VariableBase.VolatileVariableBase);
    }
    EFI_FREE_POOL (mVariableModuleGlobal);
  }

  if (mSmmVarBuf != NULL) {
    EFI_FREE_POOL (mSmmVarBuf);
  }
  if (mStorageArea != NULL) {
    EFI_FREE_POOL (mStorageArea);
  }

  if (mVariableReclaimInfo != NULL) {
    if (mVariableReclaimInfo->BackupBuffer != NULL) {
      EFI_FREE_POOL (mVariableReclaimInfo->BackupBuffer);
    }
    EFI_FREE_POOL (mVariableReclaimInfo);
  }

  return Status;
}


/**
  Callback function for service administer secure boot

  @param[in]     DispatchHandle  The unique handle assigned to this handler by SmiHandlerRegister().
  @param[in]     Context         Points to an optional handler context which was specified when the
                                 handler was registered.
  @param[in,out] CommBuffer      A pointer to a collection of data in memory that will
                                 be conveyed from a non-SMM environment into an SMM environment.
  @param[in,out] CommBufferSize  The size of the CommBuffer.

  @retval EFI_SUCCESS                         The interrupt was handled and quiesced. No other handlers
                                              should still be called.
  @retval EFI_WARN_INTERRUPT_SOURCE_QUIESCED  The interrupt has been quiesced but other handlers should
                                              still be called.
  @retval EFI_WARN_INTERRUPT_SOURCE_PENDING   The interrupt is still pending and other handlers should still
                                              be called.
  @retval EFI_INTERRUPT_PENDING               The interrupt could not be quiesced.

**/
EFI_STATUS
EFIAPI
SecureBootCallback (
  IN EFI_HANDLE     DispatchHandle,
  IN CONST VOID     *Context         OPTIONAL,
  IN OUT VOID       *CommBuffer      OPTIONAL,
  IN OUT UINTN      *CommBufferSize  OPTIONAL
  )
{
  UINTN                          Cmd;
  UINTN                          Index;
  EFI_STATUS                     Status;
  UINT32                         RegisterEax;
  UINT32                         BufferSize;
  UINT8                          *VariableBuffer;
  UINT8                          *OrgVariableBuffer;

  //
  // Since the mSmst->CurrentlyExecutingCpu may be incorrect, so we
  // need update CurrentlyExecutingCpu in our private data and then
  // we can get register directly
  //
  Status = UpdateCurrentExecutingCpu ();
  if (EFI_ERROR (Status)) {
    return Status;
  }

  GetDwordRegister (EFI_SMM_SAVE_STATE_REGISTER_RAX ,&RegisterEax);
  Status = EFI_UNSUPPORTED;
  Cmd = (UINTN)((RegisterEax >> 8) & 0xFF);
  for (Index = 0; mSecureBootFunctionsTable[Index].SmiSubFunction != NULL ; Index++) {
    if (Cmd == mSecureBootFunctionsTable[Index].FunNum) {
      if (mReadyToBootEventSignaled  && !mSecureBootFunctionsTable[Index].SupportedAfterReadyToBoot) {
        break;
      }
      GetDwordRegister (EFI_SMM_SAVE_STATE_REGISTER_RBX ,&BufferSize);
      VariableBuffer = NULL;
      GetDwordRegister (EFI_SMM_SAVE_STATE_REGISTER_RSI ,(UINT32 *) &VariableBuffer);
      if ((BufferSize != 0 && (UINTN)VariableBuffer != (UINTN)mVariableModuleGlobal->SmmVarBuf) ||
          BufferSize > mVariableModuleGlobal->SmmVarBufSize ||
          (Cmd == SET_SENSITIVE_VARIABLE_FUN_NUM &&
          BufferSize + sizeof (SENSITIVE_VARIABLE_AUTHENTICATION) > mVariableModuleGlobal->SmmVarBufSize)) {
        Status = EFI_ACCESS_DENIED;
        break;
      }
      OrgVariableBuffer = VariableBuffer;
      if (BufferSize != 0) {
        if (Cmd == SET_SENSITIVE_VARIABLE_FUN_NUM) {
          BufferSize += sizeof (SENSITIVE_VARIABLE_AUTHENTICATION);
        }

        VariableBuffer =  VariableAllocateZeroBuffer (BufferSize, TRUE);
        if (VariableBuffer == NULL) {
          Status = EFI_OUT_OF_RESOURCES;
          break;
        }
        CopyMem (VariableBuffer, OrgVariableBuffer, BufferSize);
        SetDwordRegister (EFI_SMM_SAVE_STATE_REGISTER_RSI, (UINT32)(UINTN)VariableBuffer);
      }
      Status = mSecureBootFunctionsTable[Index].SmiSubFunction ();
      if (BufferSize != 0) {
        CopyMem (OrgVariableBuffer, VariableBuffer, BufferSize);
        SetDwordRegister (EFI_SMM_SAVE_STATE_REGISTER_RAX, (UINT32)(UINTN)OrgVariableBuffer);
        EFI_FREE_POOL (VariableBuffer);
      }
      break;
    }
  }

  //
  // Convert Status to al register
  //
  Status &= 0xff;
  RegisterEax = (UINT32)((RegisterEax & 0xffffff00) | Status);
  SetDwordRegister (EFI_SMM_SAVE_STATE_REGISTER_RAX, RegisterEax);
  return EFI_SUCCESS;
}


/**
  Copy protected mode information from EfiRuntimeServicesData to SMM Ram.
  We only need invoke this function when Legacy Boot event is signaled and system is in SMM mode.

  EfiRuntimeServicesData memory is used by legacy OS, so we must copy memory to SMM Ram
  to prevent from data corrupted by OS.

  @retval EFI_SUCCESS           Restore information to SMM Ram successful.
  @retval EFI_UNSUPPORTED       System isn't in SMM mode.
  @retval EFI_OUT_OF_RESOURCES  Allocate memory failed.
--*/
STATIC
EFI_STATUS
RestoreProtectedModeInfoToSmmRam (
  VOID
  )
{
  ESAL_VARIABLE_GLOBAL            *VariableGlobal;

  if (mSmst == NULL) {
    return EFI_UNSUPPORTED;
  }

  VariableGlobal = VariableAllocateZeroBuffer (sizeof (ESAL_VARIABLE_GLOBAL), TRUE);
  if (VariableGlobal == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  CopyMem (VariableGlobal, mSmmVariableGlobal->ProtectedModeVariableModuleGlobal, sizeof (ESAL_VARIABLE_GLOBAL));
  mSmmVariableGlobal->ProtectedModeVariableModuleGlobal = VariableGlobal;
  return EFI_SUCCESS;
}

/**
  This function uses to do specific action when legacy boot event is signaled.

  @retval EFI_SUCCESS      All of action for legacy boot event in SMM mode is successful.
  @retval Other            Any error occurred.
--*/
EFI_STATUS
SmmLegacyBootEvent (
  VOID
  )
{

  UINT32         BufferSize;
  UINT8          *VariableBuffer;
  UINT32         Signature;
  STATIC BOOLEAN Initialized = FALSE;

  if (Initialized) {
    return EFI_ALREADY_STARTED;
  }
  GetDwordRegister (EFI_SMM_SAVE_STATE_REGISTER_RBX ,&BufferSize);
  VariableBuffer = NULL;
  GetDwordRegister (EFI_SMM_SAVE_STATE_REGISTER_RSI ,(UINT32 *) &VariableBuffer);
  Signature = SMM_LEGACY_BOOT_SIGNATURE;

  if (BufferSize != sizeof (UINT32) ||
      CompareMem (VariableBuffer, &Signature, sizeof (UINT32))!= 0) {
    return EFI_UNSUPPORTED;
  }
  Initialized = TRUE;
  return RestoreProtectedModeInfoToSmmRam ();
}

/**
  This function uses to invoke SMM mode SetVariable ()

  @retval EFI_SUCCESS     Calling SMM mode SetVariable () successful.
  @return Other           Any error occurred while setting variable.
**/
EFI_STATUS
SmmInternalSetVariable (
  VOID
  )
{
  UINT32             BufferSize;
  SMM_VAR_BUFFER    *SmmVarBuffer;
  CHAR16            *VariableName;
  UINT8             *VariableBuffer;


  GetDwordRegister (EFI_SMM_SAVE_STATE_REGISTER_RBX ,&BufferSize);
  SmmVarBuffer = NULL;
  GetDwordRegister (EFI_SMM_SAVE_STATE_REGISTER_RSI ,(UINT32 *) &SmmVarBuffer);

  if (BufferSize != sizeof (SMM_VAR_BUFFER) + MAX_VARIABLE_NAME_SIZE + MAX_VARIABLE_SIZE ||
      BufferSize < SmmVarBuffer->VariableNameSize +  SmmVarBuffer->DataSize ||
      SmmVarBuffer->Signature != SMM_VARIABLE_SIGNATURE) {
    return EFI_UNSUPPORTED;
  }
  VariableName = (CHAR16 *) (SmmVarBuffer + 1);
  VariableBuffer = ((UINT8 *) (SmmVarBuffer + 1)) + SmmVarBuffer->VariableNameSize;
  if (InternalCalculateSum16 (VariableBuffer, SmmVarBuffer->DataSize) != SmmVarBuffer->VarChecksum) {
    return EFI_UNSUPPORTED;
  }
  SmmVarBuffer->Status = VariableServicesSetVariable (
                           VariableName,
                           &SmmVarBuffer->VarGuid,
                           SmmVarBuffer->Attributes,
                           SmmVarBuffer->DataSize,
                           VariableBuffer
                           );
  return EFI_SUCCESS;
}

/**
  Check this input handle is whether a NV store FVB handle.

  @param[in] Handle    Input EFI_HANDLE instance

  @retval TRUE         This is NV storage FVB handle.
  @retval FALSE        This isn't NV storage FVB handle.
**/
STATIC
BOOLEAN
IsNvStorageHandle (
  EFI_HANDLE      Handle
  )
{
  EFI_STATUS   Status;

  Status = gBS->OpenProtocol (
                  Handle,
                  &gEfiFirmwareVolumeBlockProtocolGuid,
                  NULL,
                  NULL,
                  NULL,
                  EFI_OPEN_PROTOCOL_TEST_PROTOCOL
                  );
  if (EFI_ERROR (Status)) {
    return FALSE;
  }
  Status = gBS->OpenProtocol (
                  Handle,
                  &gEfiAlternateFvBlockGuid,
                  NULL,
                  NULL,
                  NULL,
                  EFI_OPEN_PROTOCOL_TEST_PROTOCOL
                  );

  return (BOOLEAN) (Status == EFI_SUCCESS);
}

/**
  Function returns an array of handles that support the FVB protocol
  in a buffer allocated from pool.

  @param[out] NumberHandles      The number of handles returned in Buffer.
  @param[out] Buffer             A pointer to the buffer to return the requested
                                 array of  handles that support FVB protocol.

  @retval EFI_SUCCESS            The array of handles was returned in Buffer, and the number of
                                 handles in Buffer was returned in NumberHandles.
  @retval EFI_NOT_FOUND          No FVB handle was found.
  @retval EFI_OUT_OF_RESOURCES   There is not enough pool memory to store the matching results.
  @retval EFI_INVALID_PARAMETER  NumberHandles is NULL or Buffer is NULL.

**/
STATIC
EFI_STATUS
GetFvbCountAndBuffer (
  OUT UINTN                               *NumberHandles,
  OUT EFI_HANDLE                          **Buffer
  )
{
  //
  // Locate all handles of Fvb protocol
  //
  return gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiFirmwareVolumeBlockProtocolGuid,
                  NULL,
                  NumberHandles,
                  Buffer
                  );
}

/**
  Retrieve the FVB protocol interface by HANDLE.

  @param[in]  FvBlockHandle      The handle of FVB protocol that provides services for
                                 reading, writing, and erasing the target block.
  @param[out] FvBlock            The interface of FVB protocol

  @retval EFI_SUCCESS            The interface information for the specified protocol was returned.
  @retval EFI_UNSUPPORTED        The device does not support the FVB protocol.
  @retval EFI_INVALID_PARAMETER  FvBlockHandle is not a valid EFI_HANDLE or FvBlock is NULL.

**/
STATIC
EFI_STATUS
GetFvbByHandle (
  IN  EFI_HANDLE                          FvBlockHandle,
  OUT EFI_FIRMWARE_VOLUME_BLOCK_PROTOCOL  **FvBlock
  )
{
  //
  // To get the FVB protocol interface on the handle
  //
  return gBS->HandleProtocol (
                FvBlockHandle,
                &gEfiFirmwareVolumeBlockProtocolGuid,
                (VOID **) FvBlock
                );
}

/**
  Get the proper fvb handle and/or fvb protocol by the given Flash address.

  @param[in]  Address      The Flash address.
  @param[out] FvbHandle    In output, if it is not NULL, it points to the proper FVB handle.
  @param[out] FvbProtocol  In output, if it is not NULL, it points to the proper FVB protocol.

  @retval EFI_SUCCESS      Get fvb handle and/or fvb protocol successfully.
  @retval EFI_NOT_FOUND    Cannot find fvb handle and/or fvb protocol.
**/
EFI_STATUS
GetFvbInfoByAddress (
  IN  EFI_PHYSICAL_ADDRESS                Address,
  OUT EFI_HANDLE                          *FvbHandle OPTIONAL,
  OUT EFI_FIRMWARE_VOLUME_BLOCK_PROTOCOL  **FvbProtocol OPTIONAL
  )
{
  EFI_STATUS                              Status;
  EFI_HANDLE                              *HandleBuffer;
  UINTN                                   HandleCount;
  UINTN                                   Index;
  EFI_PHYSICAL_ADDRESS                    FvbBaseAddress;
  EFI_FIRMWARE_VOLUME_BLOCK_PROTOCOL      *Fvb;
  EFI_FVB_ATTRIBUTES_2                    Attributes;
  UINTN                                   BlockSize;
  UINTN                                   NumberOfBlocks;


  //
  // Get all FVB handles.
  //
  Status = GetFvbCountAndBuffer (&HandleCount, &HandleBuffer);
  if (EFI_ERROR (Status)) {
    return EFI_NOT_FOUND;
  }

  //
  // Get the FVB to access variable store.
  //
  Fvb = NULL;
  for (Index = 0; Index < HandleCount; Index += 1, Status = EFI_NOT_FOUND, Fvb = NULL) {
    Status = GetFvbByHandle (HandleBuffer[Index], &Fvb);
    if (EFI_ERROR (Status)) {
      Status = EFI_NOT_FOUND;
      break;
    }

    //
    // Ensure this FVB protocol supported Write operation.
    //
    Status = Fvb->GetAttributes (Fvb, &Attributes);
    if (EFI_ERROR (Status) || ((Attributes & EFI_FVB2_WRITE_STATUS) == 0)) {
      continue;
    }

    //
    // Compare the address and select the right one.
    //
    Status = Fvb->GetPhysicalAddress (Fvb, &FvbBaseAddress);
    if (EFI_ERROR (Status)) {
      continue;
    }
    //
    // Assume one FVB has one type of BlockSize.
    //
    Status = Fvb->GetBlockSize (Fvb, 0, &BlockSize, &NumberOfBlocks);
    if (EFI_ERROR (Status)) {
      continue;
    }

    if ((Address >= FvbBaseAddress) && (Address < (FvbBaseAddress + BlockSize * NumberOfBlocks)) && IsNvStorageHandle (HandleBuffer[Index])) {
      if (FvbHandle != NULL) {
        *FvbHandle  = HandleBuffer[Index];
      }
      if (FvbProtocol != NULL) {
        *FvbProtocol = Fvb;
      }
      Status = EFI_SUCCESS;
      break;
    }
  }
  FreePool (HandleBuffer);

  if (Fvb == NULL) {
    Status = EFI_NOT_FOUND;
  }

  return Status;
}
