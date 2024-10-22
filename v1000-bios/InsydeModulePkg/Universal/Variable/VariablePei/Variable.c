/** @file
  Implement ReadOnly Variable Services required by PEIM and install
  PEI ReadOnly Varaiable2 PPI. These services operates the non volatile storage space.

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

/**


Copyright (c) 2006 - 2011, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
Module Name:

**/


#include "Variable.h"
#include <Library/FlashRegionLib.h>
#include <Ppi/MemoryDiscovered.h>

EFI_STATUS
EFIAPI
MemoryDiscoveredNotifyVariableCallback (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  );

//
// Module globals
//
EFI_PEI_READ_ONLY_VARIABLE2_PPI mVariable2Ppi = {
  PeiGetVariable,
  PeiGetNextVariableName
};

EFI_PEI_PPI_DESCRIPTOR     mPpiListVariable2 = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gEfiPeiReadOnlyVariable2PpiGuid,
  &mVariable2Ppi
};

static EFI_PEI_NOTIFY_DESCRIPTOR      mNotifyList[] = {
  {
    (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gEfiPeiMemoryDiscoveredPpiGuid,
    MemoryDiscoveredNotifyVariableCallback
  }
};

STATIC EFI_GUID  mVariableDefaultId = FDM_VARIABLE_DEFAULT_ID_VARIABLE_DEFAULT;

/**
   This function verifies variable default region if permanent memory is ready.

   @param  PeiServices      Indirect reference to the PEI Services Table.
   @param  NotifyDescriptor Address of the notification descriptor data structure.
   @param  Ppi              Address of the PPI that was installed.

   @return EFI_SUCCESS      Verify variable default region successfully.
   @return Others           Some error occurs during verifing variable default region.

**/
EFI_STATUS
EFIAPI
MemoryDiscoveredNotifyVariableCallback (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  )
{
  if (!FeaturePcdGet (PcdH2OFdmChainOfTrustSupported)) {
    return EFI_SUCCESS;
  }
  
  return FdmVerifyById (&gH2OFlashMapRegionVarDefaultGuid, &mVariableDefaultId, 1);
}

/**
  Determine need force variable defaults or not.

  @retval TRUE    System need force variable defaults.
  @retval FALSE   System need not force variable defaults.
**/
STATIC
BOOLEAN
ForceVariableDefaults (
  VOID
  )
{
  H2O_PEI_CP_FORCE_VARIABLE_DEFAULTS_DATA               ForceVariableDefaultsData;
  EFI_FIRMWARE_VOLUME_HEADER                            *FvHeader;
  VARIABLE_STORE_HEADER                                 *VariableStoreHeader;

  if (!FeaturePcdGet (PcdH2OPeiCpForceVariableDefaultsSupported)) {
    return FALSE;
  }

  FvHeader = (EFI_FIRMWARE_VOLUME_HEADER *) (UINTN) FdmGetNAtAddr (&gH2OFlashMapRegionVarGuid, 1);
  VariableStoreHeader = (VARIABLE_STORE_HEADER *) ((UINT8 *) FvHeader + FvHeader->HeaderLength);
  ForceVariableDefaultsData.Size                  = sizeof (H2O_PEI_CP_FORCE_VARIABLE_DEFAULTS_DATA);
  ForceVariableDefaultsData.Status                = H2O_CP_TASK_NORMAL;
  ForceVariableDefaultsData.VariableStoreHdr      = VariableStoreHeader;
  ForceVariableDefaultsData.ForceVariableDefaults = FALSE;
  DEBUG ((DEBUG_INFO, "Checkpoint Trigger: %g\n", &gH2OPeiCpForceVariableDefaultsGuid));
  H2OCpTrigger (&gH2OPeiCpForceVariableDefaultsGuid, &ForceVariableDefaultsData);
  DEBUG ((DEBUG_INFO, "Checkpoint Result: %x\n", ForceVariableDefaultsData.Status));
  if (ForceVariableDefaultsData.Status == H2O_CP_TASK_UPDATE && ForceVariableDefaultsData.ForceVariableDefaults) {
    return TRUE;
  }
  return FALSE;
}
/**
  Provide the functionality of the variable services.
  NOTE: If verification fails, this function will hang.

  @param  FileHandle   Handle of the file being invoked.
                       Type EFI_PEI_FILE_HANDLE is defined in FfsFindNextFile().
  @param  PeiServices  General purpose services available to every PEIM.

  @retval EFI_SUCCESS  If the interface could be successfully installed
  @retval Others       Returned from PeiServicesInstallPpi()
**/
EFI_STATUS
EFIAPI
PeimInitializeVariableServices (
  IN       EFI_PEI_FILE_HANDLE       FileHandle,
  IN CONST EFI_PEI_SERVICES          **PeiServices
  )
{
  BOOLEAN            ForceDefaults;
  EFI_STATUS         Status;

  Status = (**PeiServices).NotifyPpi (PeiServices, &mNotifyList[0]);
  ASSERT_EFI_ERROR (Status);  
  if (ForceVariableDefaults ()) {
    ForceDefaults = TRUE;
    BuildGuidDataHob (&gH2OFlashMapRegionVarGuid, &ForceDefaults, sizeof (BOOLEAN));
  }
  return PeiServicesInstallPpi (&mPpiListVariable2);
}

/**
  This function compares a variable with variable entries in database.

  @param  Variable      Pointer to the variable in our database
  @param  VariableName  Name of the variable to compare to 'Variable'
  @param  VendorGuid    GUID of the variable to compare to 'Variable'
  @param  PtrTrack      Variable Track Pointer structure that contains Variable Information.

  @retval EFI_SUCCESS    Found match variable
  @retval EFI_NOT_FOUND  Variable not found

**/
EFI_STATUS
CompareWithValidVariable (
  IN  VARIABLE_HEADER               *Variable,
  IN  CONST CHAR16                  *VariableName,
  IN  CONST EFI_GUID                *VendorGuid,
  OUT VARIABLE_POINTER_TRACK        *PtrTrack
  )
{
  VOID  *Point;

  if (VariableName[0] == 0) {
    PtrTrack->CurrPtr = Variable;
    return EFI_SUCCESS;
  } else {
    //
    // Don't use CompareGuid function here for performance reasons.
    // Instead we compare the GUID a UINT32 at a time and branch
    // on the first failed comparison.
    //
    if ((((INT32 *) VendorGuid)[0] == ((INT32 *) &Variable->VendorGuid)[0]) &&
        (((INT32 *) VendorGuid)[1] == ((INT32 *) &Variable->VendorGuid)[1]) &&
        (((INT32 *) VendorGuid)[2] == ((INT32 *) &Variable->VendorGuid)[2]) &&
        (((INT32 *) VendorGuid)[3] == ((INT32 *) &Variable->VendorGuid)[3])
        ) {
      ASSERT (NameSizeOfVariable (Variable) != 0);
      Point = (VOID *) GET_VARIABLE_NAME_PTR (Variable);
      if (CompareMem (VariableName, Point, NameSizeOfVariable (Variable)) == 0) {
        PtrTrack->CurrPtr = Variable;
        return EFI_SUCCESS;
      }
    }
  }

  return EFI_NOT_FOUND;
}

/**
  Return the variable store header and the index table based on the Index.

  @param Type       The type of the variable store.
  @param IndexTable Return the index table.

  @return  Pointer to the variable store header.
**/
VARIABLE_STORE_HEADER *
GetVariableStore (
  IN VARIABLE_STORE_TYPE         Type,
  OUT VARIABLE_INDEX_TABLE_EX    **IndexTable  OPTIONAL
  )
{
  EFI_HOB_GUID_TYPE           *GuidHob;
  EFI_FIRMWARE_VOLUME_HEADER  *FvHeader;
  VARIABLE_STORE_HEADER       *VariableStoreHeader;

  if (IndexTable != NULL) {
    *IndexTable       = NULL;
  }
  VariableStoreHeader = NULL;
  switch (Type) {
    case VariableStoreTypeHob:
      GuidHob     = GetFirstGuidHob (&gEfiAuthenticatedVariableGuid);
      if (GuidHob != NULL) {
        VariableStoreHeader = (VARIABLE_STORE_HEADER *) GET_GUID_HOB_DATA (GuidHob);
      }
      break;

    case VariableStoreTypeNv:
      if (GetBootModeHob () != BOOT_IN_RECOVERY_MODE) {
        //
        // The content of NV storage for variable is not reliable in recovery boot mode.
        //
        FvHeader = (EFI_FIRMWARE_VOLUME_HEADER *) (UINTN) FdmGetNAtAddr (&gH2OFlashMapRegionVarGuid, 1);
        VariableStoreHeader = (VARIABLE_STORE_HEADER *) ((UINT8 *) FvHeader + FvHeader->HeaderLength);

        if (IndexTable != NULL) {
          GuidHob = GetFirstGuidHob (&gEfiVariableIndexTableGuid);
          if (GuidHob != NULL) {
            *IndexTable = GET_GUID_HOB_DATA (GuidHob);
          } else {
            //
            // If it's the first time to access variable region in flash, create a guid hob to record
            // VAR_ADDED type variable info.
            // Note that as the resource of PEI phase is limited, only store the limited number of
            // VAR_ADDED type variables to reduce access time.
            //
            *IndexTable = BuildGuidHob (&gEfiVariableIndexTableGuid, sizeof (VARIABLE_INDEX_TABLE_EX));
            if (*IndexTable != NULL) {
              (*IndexTable)->Length      = 0;
              (*IndexTable)->StartPtr    = GetStartPointer (VariableStoreHeader);
              (*IndexTable)->EndPtr      = GetEndPointer   (VariableStoreHeader);
              (*IndexTable)->GoneThrough = 0;
            }
          }
        }
      }
      break;

    default:
      ASSERT (FALSE);
      break;
  }

  return VariableStoreHeader;
}

/**
  This code gets the size of variable store.

  @param  VarStoreHeader  Pointer to the Variable Store Header.

  @return  Total size of variable store


**/
UINT32
GetVariableSize (
  IN VARIABLE_STORE_HEADER       *VariableStoreHeader
  )
{
  ECP_VARIABLE_STORE_HEADER        *EcpVariableStoreHeader;

  if (PcdGetBool (PcdUseEcpVariableStoreHeader)) {
    EcpVariableStoreHeader = (ECP_VARIABLE_STORE_HEADER *) VariableStoreHeader;
    return EcpVariableStoreHeader->Size;
  } else {
    return VariableStoreHeader->Size;
  }
}

/**
  Get the specific config data from MultiConfig region.

  @param  RequireKind         Find Setup Setting for SETUP_FOR_BIOS_POST or SETUP_FOR_LOAD_DEFAULT.
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
  IN CONST  CHAR16                  *VariableName,
  IN CONST  EFI_GUID                *VariableGuid,
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
  DataPtr = NULL;
  ConfigCount = GetConfigCount();

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
          return EFI_SUCCESS;
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
  Find the variable in the specified variable store.

  @param  VariableStoreHeader Pointer to the variable store header.
  @param  IndexTable          Pointer to the index table.
  @param  VariableName        Name of the variable to be found
  @param  VendorGuid          Vendor GUID to be found.
  @param  PtrTrack            Variable Track Pointer structure that contains Variable Information.

  @retval  EFI_SUCCESS            Variable found successfully
  @retval  EFI_NOT_FOUND          Variable not found
  @retval  EFI_INVALID_PARAMETER  Invalid variable name

**/
EFI_STATUS
FindVariableEx (
  IN VARIABLE_STORE_HEADER       *VariableStoreHeader,
  IN VARIABLE_INDEX_TABLE_EX     *IndexTable,
  IN CONST CHAR16                *VariableName,
  IN CONST EFI_GUID              *VendorGuid,
  OUT VARIABLE_POINTER_TRACK     *PtrTrack
  )
{
  VARIABLE_HEADER         *Variable;
  VARIABLE_HEADER         *LastVariable;
  VARIABLE_HEADER         *MaxIndex;
  UINTN                   Index;
  UINTN                   Offset;
  BOOLEAN                 StopRecord;
  VARIABLE_HEADER         *InDeletedVariable;

  if (VariableStoreHeader == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (GetVariableStoreStatus (VariableStoreHeader) != EfiValid) {
    return EFI_UNSUPPORTED;
  }

  if (~GetVariableSize (VariableStoreHeader) == 0) {
    return EFI_NOT_FOUND;
  }

  PtrTrack->StartPtr = GetStartPointer (VariableStoreHeader);
  PtrTrack->EndPtr   = GetEndPointer   (VariableStoreHeader);

  InDeletedVariable = NULL;

  //
  // No Variable Address equals zero, so 0 as initial value is safe.
  //
  MaxIndex   = NULL;

  if (IndexTable != NULL) {
    //
    // traverse the variable index table to look for variable.
    // The IndexTable->Index[Index] records the distance of two neighbouring VAR_ADDED type variables.
    //
    for (Offset = 0, Index = 0; Index < IndexTable->Length; Index++) {
      ASSERT (Index < sizeof (IndexTable->Index) / sizeof (IndexTable->Index[0]));
      Offset   += IndexTable->Index[Index];
      MaxIndex  = (VARIABLE_HEADER *) ((UINT8 *) IndexTable->StartPtr + Offset);
      if (CompareWithValidVariable (MaxIndex, VariableName, VendorGuid, PtrTrack) == EFI_SUCCESS) {
        if (PtrTrack->CurrPtr->State == (VAR_IN_DELETED_TRANSITION & VAR_ADDED)) {
          InDeletedVariable = PtrTrack->CurrPtr;
        } else {
          return EFI_SUCCESS;
        }
      }
    }

    if (IndexTable->GoneThrough != 0) {
      //
      // If the table has all the existing variables indexed, return.
      //
      PtrTrack->CurrPtr = InDeletedVariable;
      return (PtrTrack->CurrPtr == NULL) ? EFI_NOT_FOUND : EFI_SUCCESS;
    }
  }

  if (MaxIndex != NULL) {
    //
    // HOB exists but the variable cannot be found in HOB
    // If not found in HOB, then let's start from the MaxIndex we've found.
    //
    Variable     = GetNextVariablePtr (MaxIndex);
    LastVariable = MaxIndex;
  } else {
    //
    // Start Pointers for the variable.
    // Actual Data Pointer where data can be written.
    //
    Variable     = PtrTrack->StartPtr;
    LastVariable = PtrTrack->StartPtr;
  }

  //
  // Find the variable by walk through variable store
  //
  StopRecord = FALSE;
  while ((GetNextVariablePtr (Variable) <= PtrTrack->EndPtr) && IsValidVariableHeader (Variable)) {
    if (Variable->State == VAR_ADDED || Variable->State == (VAR_IN_DELETED_TRANSITION & VAR_ADDED)) {
      //
      // Record Variable in VariableIndex HOB
      //
      if ((IndexTable != NULL) && !StopRecord) {
        Offset = (UINTN) Variable - (UINTN) LastVariable;
        if (IndexTable->Length >= sizeof (IndexTable->Index) / sizeof (IndexTable->Index[0])) {
          //
          // Stop to record if the record buffer is full.
          //
          StopRecord = TRUE;
        } else {
          IndexTable->Index[IndexTable->Length++] = (UINT32) Offset;
          LastVariable = Variable;
        }
      }

      if (CompareWithValidVariable (Variable, VariableName, VendorGuid, PtrTrack) == EFI_SUCCESS) {
        if (PtrTrack->CurrPtr->State == (VAR_IN_DELETED_TRANSITION & VAR_ADDED)) {
          InDeletedVariable = PtrTrack->CurrPtr;
        } else {
          return EFI_SUCCESS;
        }
      }
    }

    Variable = GetNextVariablePtr (Variable);
  }
  //
  // If gone through the VariableStore, that means we never find in Firmware any more.
  //
  if ((IndexTable != NULL) && !StopRecord) {
    IndexTable->GoneThrough = 1;
  }

  PtrTrack->CurrPtr = InDeletedVariable;

  return (PtrTrack->CurrPtr == NULL) ? EFI_NOT_FOUND : EFI_SUCCESS;
}

/**
  Find the variable in HOB and Non-Volatile variable storages.

  @param  VariableName  Name of the variable to be found
  @param  VendorGuid    Vendor GUID to be found.
  @param  PtrTrack      Variable Track Pointer structure that contains Variable Information.

  @retval  EFI_SUCCESS            Variable found successfully
  @retval  EFI_NOT_FOUND          Variable not found
  @retval  EFI_INVALID_PARAMETER  Invalid variable name
**/
EFI_STATUS
FindVariable (
  IN CONST  CHAR16            *VariableName,
  IN CONST  EFI_GUID          *VendorGuid,
  OUT VARIABLE_POINTER_TRACK  *PtrTrack
  )
{
  EFI_STATUS                  Status;
  VARIABLE_STORE_HEADER       *VariableStoreHeader;
  VARIABLE_INDEX_TABLE_EX     *IndexTable;
  VARIABLE_STORE_TYPE         Type;
  EFI_HOB_GUID_TYPE           *GuidHob;
  BOOLEAN                     ForceDefaults;


  if (VariableName[0] != 0 && VendorGuid == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  ForceDefaults = FALSE;
  GuidHob = GetFirstGuidHob (&gH2OFlashMapRegionVarGuid);
  if (GuidHob != NULL) {
    ForceDefaults = TRUE;
  }

  for (Type = (VARIABLE_STORE_TYPE) 0; Type < VariableStoreTypeMax; Type++) {
    VariableStoreHeader = GetVariableStore (Type, &IndexTable);
    if (Type == VariableStoreTypeNv && ForceDefaults) {
      continue;
    }
    Status = FindVariableEx (
               VariableStoreHeader,
               IndexTable,
               VariableName,
               VendorGuid,
               PtrTrack
               );
    if (!EFI_ERROR (Status)) {
      return Status;
    }
  }

  return EFI_NOT_FOUND;
}

/**
  Internal function to get the start address of variable default header according to
  input SKU ID.

  @param[in]  SkuId   Input SKU ID.

  @return  The address of variable store header or NULL if not found.
**/
STATIC
VOID *
GetVariableDefaultStoreHeaderBySkuId (
  IN  UINT8       SkuId
  )
{
  VARIABLE_STORE_HEADER           *VariableStoreHeader;
  ECP_VARIABLE_STORE_HEADER       *EcpVariableStoreHeader;
  UINTN                           VariableDefaultBase;
  UINTN                           VariableDefaultSize;
  EFI_GUID                        Id = FDM_VARIABLE_DEFAULT_ID_VARIABLE_DEFAULT;


  VariableDefaultBase = (UINTN) FdmGetAddressById (&gH2OFlashMapRegionVarDefaultGuid, &Id, 1);
  VariableDefaultSize = (UINTN) FdmGetSizeById (&gH2OFlashMapRegionVarDefaultGuid, &Id, 1);

  if (PcdGetBool (PcdUseEcpVariableStoreHeader)) {
    for (EcpVariableStoreHeader = (ECP_VARIABLE_STORE_HEADER *) VariableDefaultBase;
         EcpVariableStoreHeader != NULL &&
         EcpVariableStoreHeader->Signature == ECP_VARIABLE_STORE_SIGNATURE &&
         EcpVariableStoreHeader->Format == VARIABLE_STORE_FORMATTED &&
         EcpVariableStoreHeader->State == VARIABLE_STORE_HEALTHY &&
         (EcpVariableStoreHeader->Flags & VARIABLE_STORE_ACTIVE_MASK) == VARIABLE_STORE_ACTIVE &&
         EcpVariableStoreHeader->Size >= sizeof (ECP_VARIABLE_STORE_HEADER) &&
         (UINTN) EcpVariableStoreHeader < VariableDefaultBase + VariableDefaultSize;
         EcpVariableStoreHeader = (ECP_VARIABLE_STORE_HEADER *) ((UINT8 *) EcpVariableStoreHeader + EcpVariableStoreHeader->Size)) {
      if (EcpVariableStoreHeader->DefaultId == 0 && SkuId == EcpVariableStoreHeader->BoardId) {
        return (VOID *) EcpVariableStoreHeader;
      }
    }
  } else {
    for (VariableStoreHeader = (VARIABLE_STORE_HEADER *) VariableDefaultBase;
         VariableStoreHeader != NULL &&
         (CompareGuid (&VariableStoreHeader->Signature, &gEfiAuthenticatedVariableGuid) ||
           CompareGuid (&VariableStoreHeader->Signature, &gEfiVariableGuid)) &&
         VariableStoreHeader->Format == VARIABLE_STORE_FORMATTED &&
         VariableStoreHeader->State == VARIABLE_STORE_HEALTHY &&
         (VariableStoreHeader->Flags & VARIABLE_STORE_ACTIVE_MASK) == VARIABLE_STORE_ACTIVE &&
         VariableStoreHeader->Size >= sizeof (VARIABLE_STORE_HEADER) &&
         (UINTN) VariableStoreHeader < VariableDefaultBase + VariableDefaultSize;
         VariableStoreHeader = (VARIABLE_STORE_HEADER *) ((UINT8 *) VariableStoreHeader + VariableStoreHeader->Size)) {
      if (VariableStoreHeader->DefaultId == 0 && SkuId == VariableStoreHeader->BoardId) {
        return (VOID *) VariableStoreHeader;
      }
    }
  }
  return NULL;
}

/**
  Get default SKU ID variable store header in variable default store.

  @return  The address of variable store header or NULL if not found.
**/
STATIC
VOID *
GetDefaultVariableDefaultStoreHeader (
  VOID
  )
{
  return GetVariableDefaultStoreHeaderBySkuId (0);
}

/**
  According to current SKU ID to get variable store header in variable default store.

  @return  The address  of variable store header or NULL if not found.
**/
STATIC
VOID *
GetCurrentVariableDefaultStoreHeader (
  VOID
  )
{
  //
  // BUG! BUG!
  // Sku value may be changed after PcdSkuInit () is invoked in PlatformStage1Pei entrypoint.
  // Different SKU value may cause the variable data is different.
  //
  //
  return GetVariableDefaultStoreHeaderBySkuId ((UINT8) LibPcdGetSku ());
}

/**
  Internal function to check whether the variable is stored in variable store.

  @param[in]  VariableName  Name of the variable to be found
  @param[in]  VendorGuid    Vendor GUID to be found.

  @retval TRUE              Can find variable in variable store.
  @retval FALSE             Cannot find variable in variable store.
**/
STATIC
BOOLEAN
VariableInVariableStore (
  IN CONST  CHAR16            *VariableName,
  IN CONST  EFI_GUID          *VendorGuid
  )
{
  VARIABLE_POINTER_TRACK  Variable;
  EFI_STATUS              Status;

  Status = FindVariable (VariableName, VendorGuid, &Variable);
  return Status == EFI_SUCCESS;
}

/**
  Get variable from specific variable store region.

  @param[in]  VariableName         Name of the variable to be found
  @param[in]  VendorGuid           Vendor GUID to be found.
  @param[in]  VariableStoreHeader  Pointer to the start address of variable store header.
  @param[out] PtrTrack             Variable Track Pointer structure that contains Variable Information.

  @return  The address of variable store header or NULL if not found.
**/
STATIC
EFI_STATUS
GetVariableFromSpecificDefaultRegion (
  IN CONST  CHAR16                           *VariableName,
  IN CONST  EFI_GUID                         *VendorGuid,
  IN CONST  VOID                             *VariableStoreHeader,
  OUT VARIABLE_POINTER_TRACK                 *PtrTrack
  )
{
  VARIABLE_HEADER                 *Variable;

  PtrTrack->StartPtr = (VARIABLE_HEADER *) ((UINT8 *) VariableStoreHeader + GetVariableStoreHeaderSize ());
  PtrTrack->EndPtr   = (VARIABLE_HEADER *) ((UINT8 *) VariableStoreHeader + GetVariableStoreSize (VariableStoreHeader));
  Variable           = PtrTrack->StartPtr;
  while ((GetNextVariablePtr (Variable) <= PtrTrack->EndPtr) && IsValidVariableHeader (Variable)) {
    if ((CompareGuid (VendorGuid, &Variable->VendorGuid) &&
        !StrCmp (VariableName, GET_VARIABLE_NAME_PTR (Variable))) ||
        VariableName [0] == 0) {
      if (Variable->State == VAR_ADDED && !VariableInVariableStore (GET_VARIABLE_NAME_PTR (Variable), &Variable->VendorGuid)) {
        PtrTrack->CurrPtr = Variable;
        return EFI_SUCCESS;
      }
    }
    Variable = GetNextVariablePtr (Variable);
  }

  return EFI_NOT_FOUND;
}

/**
  Get variable from variable default store.

  @param[in]  VariableName  Name of the variable to be found
  @param[in]  VendorGuid    Vendor GUID to be found.
  @param[out] PtrTrack      Variable Track Pointer structure that contains Variable Information.

  @retval EFI_SUCCESS   Find variable in variable default store successfully.
  @retval EFI_NOT_FOUND Cannot find variable in variable default store.
**/
STATIC
EFI_STATUS
GetVariableFromVariableDefault (
  IN CONST  CHAR16            *VariableName,
  IN CONST  EFI_GUID          *VendorGuid,
  OUT VARIABLE_POINTER_TRACK  *PtrTrack
  )
{
  VOID                           *VariableStoreHeader;
  VOID                           *DefaultVariableStoreHeader;
  EFI_STATUS                      Status;


  PtrTrack->CurrPtr   = NULL;
  VariableStoreHeader = GetCurrentVariableDefaultStoreHeader ();
  if (VariableStoreHeader != NULL) {
    Status = GetVariableFromSpecificDefaultRegion (VariableName, VendorGuid, VariableStoreHeader, PtrTrack);
    if (Status == EFI_SUCCESS) {
      return Status;
    }
  }
  DefaultVariableStoreHeader = GetDefaultVariableDefaultStoreHeader ();
  if (DefaultVariableStoreHeader != NULL && DefaultVariableStoreHeader != VariableStoreHeader) {
    Status = GetVariableFromSpecificDefaultRegion (VariableName, VendorGuid, DefaultVariableStoreHeader, PtrTrack);
    if (Status == EFI_SUCCESS) {
      return Status;
    }
  }
  return EFI_NOT_FOUND;
}

/**
  This code Finds the Next available variable in variable default region.

  @param[in, out] VariableNameSize Size of the variable.
  @param[in, out] VariableName     Pointer to variable name.
  @param[in, out] VendorGuid       Variable Vendor Guid.

  @retval EFI_SUCCESS              Get variable successfully.
  @retval EFI_NOT_FOUND            Not found.
**/
STATIC
EFI_STATUS
GetNextDefaultVariableName (
  IN OUT  UINTN             *VariableNameSize,
  IN OUT  CHAR16            *VariableName,
  IN OUT  EFI_GUID          *VendorGuid
  )
{
  EFI_STATUS              Status;
  VARIABLE_POINTER_TRACK  VariableTrack;
  UINTN                   VarNameSize;
  VARIABLE_HEADER         *Variable;

  Status = GetVariableFromVariableDefault (VariableName, VendorGuid, &VariableTrack);
  if (Status != EFI_SUCCESS) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Only need try to find next variable if first character isn't 0.
  // If first character is 0 and GetVariableFromVariableDefault () return EFI_SUCCESS,
  // It means we already find first valid variable.
  //
  if (VariableName[0] != 0) {

    VariableTrack.CurrPtr = GetNextVariablePtr (VariableTrack.CurrPtr);
    if (VariableTrack.CurrPtr == NULL) {
      return EFI_NOT_FOUND;
    }

    Variable          = VariableTrack.CurrPtr;
    VariableTrack.CurrPtr  = NULL;
    while ((GetNextVariablePtr (Variable) <= VariableTrack.EndPtr) && IsValidVariableHeader (Variable)) {
      if (Variable->State == VAR_ADDED && !VariableInVariableStore (GET_VARIABLE_NAME_PTR (Variable), &Variable->VendorGuid)) {
        VariableTrack.CurrPtr = Variable;
        break;
      }
      Variable = GetNextVariablePtr (Variable);
    }

    if (VariableTrack.CurrPtr == NULL) {
      return EFI_NOT_FOUND;
    }
  }

  VarNameSize = NameSizeOfVariable (VariableTrack.CurrPtr);
  if (VarNameSize <= *VariableNameSize) {
    CopyMem (VariableName, GET_VARIABLE_NAME_PTR (VariableTrack.CurrPtr), VarNameSize);
    CopyMem (VendorGuid, &VariableTrack.CurrPtr->VendorGuid, sizeof (EFI_GUID));
    Status = EFI_SUCCESS;
  } else {
    Status = EFI_BUFFER_TOO_SMALL;
  }
  *VariableNameSize = VarNameSize;
  return Status;

}

/**
  This code Finds the first available variable in variable default region.

  @param[in, out] VariableNameSize Size of the variable.
  @param[in, out] VariableName     Pointer to variable name.
  @param[in, out] VendorGuid       Variable Vendor Guid.

  @retval EFI_SUCCESS              Get variable successfully.
  @retval EFI_NOT_FOUND            Not found.
**/
STATIC
EFI_STATUS
GetFirstDefaultVariableName (
  IN OUT  UINTN             *VariableNameSize,
  IN OUT  CHAR16            *VariableName,
  IN OUT  EFI_GUID          *VendorGuid
  )
{

  CHAR16          SavedChar;
  EFI_STATUS      Status;


  SavedChar = VariableName[0];
  VariableName[0] = 0;
  Status = GetNextDefaultVariableName (VariableNameSize, VariableName, VendorGuid);
  if (EFI_ERROR (Status)) {
    VariableName[0] = SavedChar;
  }
  return Status;
}


/**
  This service retrieves a variable's value using its name and GUID.

  Read the specified variable from the UEFI variable store. If the Data
  buffer is too small to hold the contents of the variable, the error
  EFI_BUFFER_TOO_SMALL is returned and DataSize is set to the required buffer
  size to obtain the data.

  @param  This                  A pointer to this instance of the EFI_PEI_READ_ONLY_VARIABLE2_PPI.
  @param  VariableName          A pointer to a null-terminated string that is the variable's name.
  @param  VariableGuid          A pointer to an EFI_GUID that is the variable's GUID. The combination of
                                VariableGuid and VariableName must be unique.
  @param  Attributes            If non-NULL, on return, points to the variable's attributes.
  @param  DataSize              On entry, points to the size in bytes of the Data buffer.
                                On return, points to the size of the data returned in Data.
  @param  Data                  Points to the buffer which will hold the returned variable value.
                                May be NULL with a zero DataSize in order to determine the size of the buffer needed.

  @retval EFI_SUCCESS           The variable was read successfully.
  @retval EFI_NOT_FOUND         The variable was not found.
  @retval EFI_BUFFER_TOO_SMALL  The DataSize is too small for the resulting data.
                                DataSize is updated with the size required for
                                the specified variable.
  @retval EFI_INVALID_PARAMETER VariableName, VariableGuid, DataSize or Data is NULL.
  @retval EFI_DEVICE_ERROR      The variable could not be retrieved because of a device error.

**/
EFI_STATUS
EFIAPI
PeiGetVariable (
  IN CONST  EFI_PEI_READ_ONLY_VARIABLE2_PPI *This,
  IN CONST  CHAR16                          *VariableName,
  IN CONST  EFI_GUID                        *VariableGuid,
  OUT       UINT32                          *Attributes,
  IN OUT    UINTN                           *DataSize,
  OUT       VOID                            *Data OPTIONAL
  )
{
  VARIABLE_POINTER_TRACK  Variable;
  UINTN                   VarDataSize;
  EFI_STATUS              Status;

  if (VariableName == NULL || VariableGuid == NULL || DataSize == NULL || VariableName[0] == 0) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Find existing variable
  //
  Status = FindVariable (VariableName, VariableGuid, &Variable);

  if (Status == EFI_NOT_FOUND) {
    Status = GetVariableFromVariableDefault (VariableName, VariableGuid, &Variable);
  }

  if (FeaturePcdGet (PcdMultiConfigSupported) && (Status == EFI_NOT_FOUND)) {
    Status = GetConfigData (SETUP_FOR_BIOS_POST, VariableName, VariableGuid, Attributes, DataSize, Data);
    if (Status == EFI_NOT_FOUND) {
      Status = GetConfigData (SETUP_FOR_LOAD_DEFAULT, VariableName, VariableGuid, Attributes, DataSize, Data);
    }
    if (!EFI_ERROR (Status)) {
      return Status;
    }
  }
  if (EFI_ERROR (Status)) {
    return Status;
  }
  //
  // Get data size
  //
  VarDataSize = DataSizeOfVariable (Variable.CurrPtr);
  if (*DataSize >= VarDataSize) {
    if (Data == NULL) {
      return EFI_INVALID_PARAMETER;
    }

    CopyMem (Data, GetVariableDataPtr (Variable.CurrPtr), VarDataSize);

    if (Attributes != NULL) {
      *Attributes = (Variable.CurrPtr->Attributes & ~(EFI_VARIABLE_DEFAULT_READY_ONLY | EFI_VARIABLE_INSYDE_AUTHENTICATED_WRITE_ACCESS));
    }

    *DataSize = VarDataSize;
    return EFI_SUCCESS;
  } else {
    *DataSize = VarDataSize;
    return EFI_BUFFER_TOO_SMALL;
  }
}


/**
  Return the next variable name and GUID.

  This function is called multiple times to retrieve the VariableName
  and VariableGuid of all variables currently available in the system.
  On each call, the previous results are passed into the interface,
  and, on return, the interface returns the data for the next
  interface. When the entire variable list has been returned,
  EFI_NOT_FOUND is returned.

  @param  This              A pointer to this instance of the EFI_PEI_READ_ONLY_VARIABLE2_PPI.

  @param  VariableNameSize  On entry, points to the size of the buffer pointed to by VariableName.
                            On return, the size of the variable name buffer.
  @param  VariableName      On entry, a pointer to a null-terminated string that is the variable's name.
                            On return, points to the next variable's null-terminated name string.
  @param  VariableGuid      On entry, a pointer to an EFI_GUID that is the variable's GUID.
                            On return, a pointer to the next variable's GUID.

  @retval EFI_SUCCESS           The variable was read successfully.
  @retval EFI_NOT_FOUND         The variable could not be found.
  @retval EFI_BUFFER_TOO_SMALL  The VariableNameSize is too small for the resulting
                                data. VariableNameSize is updated with the size
                                required for the specified variable.
  @retval EFI_INVALID_PARAMETER VariableName, VariableGuid or
                                VariableNameSize is NULL.
  @retval EFI_DEVICE_ERROR      The variable could not be retrieved because of a device error.

**/
EFI_STATUS
EFIAPI
PeiGetNextVariableName (
  IN CONST  EFI_PEI_READ_ONLY_VARIABLE2_PPI *This,
  IN OUT UINTN                              *VariableNameSize,
  IN OUT CHAR16                             *VariableName,
  IN OUT EFI_GUID                           *VariableGuid
  )
{
  VARIABLE_STORE_TYPE     Type;
  VARIABLE_POINTER_TRACK  Variable;
  VARIABLE_POINTER_TRACK  VariableInHob;
  VARIABLE_POINTER_TRACK  VariablePtrTrack;
  VARIABLE_INDEX_TABLE_EX *IndexTable;
  UINTN                   VarNameSize;
  EFI_STATUS              Status;
  VARIABLE_STORE_HEADER   *VariableStoreHeader[VariableStoreTypeMax];

  if (VariableName == NULL || VariableGuid == NULL || VariableNameSize == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (StrSize (VariableName) > *VariableNameSize) {
    return EFI_INVALID_PARAMETER;
  }

  Status = FindVariable (VariableName, VariableGuid, &Variable);
  if (Variable.CurrPtr == NULL || Status != EFI_SUCCESS) {
    return GetNextDefaultVariableName (VariableNameSize, VariableName, VariableGuid);
  }

  if (VariableName[0] != 0) {
    //
    // If variable name is not NULL, get next variable
    //
    Variable.CurrPtr = GetNextVariablePtr (Variable.CurrPtr);
  }

  VariableStoreHeader[VariableStoreTypeHob] = GetVariableStore (VariableStoreTypeHob, NULL);
  VariableStoreHeader[VariableStoreTypeNv]  = GetVariableStore (VariableStoreTypeNv, NULL);

  while (TRUE) {
    //
    // Switch from HOB to Non-Volatile.
    //
    while ((Variable.CurrPtr >= Variable.EndPtr) ||
           (Variable.CurrPtr == NULL)            ||
           !IsValidVariableHeader (Variable.CurrPtr)
          ) {
      //
      // Find current storage index
      //
      for (Type = (VARIABLE_STORE_TYPE) 0; Type < VariableStoreTypeMax; Type++) {
        if ((VariableStoreHeader[Type] != NULL) && (Variable.StartPtr == GetStartPointer (VariableStoreHeader[Type]))) {
          break;
        }
      }
      ASSERT (Type < VariableStoreTypeMax);
      //
      // Switch to next storage
      //
      for (Type++; Type < VariableStoreTypeMax; Type++) {
        if (VariableStoreHeader[Type] != NULL) {
          break;
        }
      }
      //
      // Capture the case that
      // 1. current storage is the last one, or
      // 2. no further storage
      //
      if (Type >= VariableStoreTypeMax) {
        //
        // Assume secure boot database default variable is at the end of whole data base,
        // so start to get these variables after all variable.
        //
        return GetFirstDefaultVariableName (VariableNameSize, VariableName, VariableGuid);
      }
      Variable.StartPtr = GetStartPointer (VariableStoreHeader[Type]);
      Variable.EndPtr   = GetEndPointer   (VariableStoreHeader[Type]);
      Variable.CurrPtr  = Variable.StartPtr;
    }

    if (Variable.CurrPtr->State == VAR_ADDED || Variable.CurrPtr->State == (VAR_IN_DELETED_TRANSITION & VAR_ADDED)) {
      if (Variable.CurrPtr->State == (VAR_IN_DELETED_TRANSITION & VAR_ADDED)) {
        //
        // If it is a IN_DELETED_TRANSITION variable,
        // and there is also a same ADDED one at the same time,
        // don't return it.
        //
        for (Type = (VARIABLE_STORE_TYPE) 0; Type < VariableStoreTypeMax; Type++) {
          if ((VariableStoreHeader[Type] != NULL) && (Variable.StartPtr == GetStartPointer (VariableStoreHeader[Type]))) {
            break;
          }
        }
        ASSERT (Type < VariableStoreTypeMax);
        if (Type == VariableStoreTypeMax) {
          //
          // Assume secure boot database default variable is at the end of whole data base,
          // so start to get these variables after all variable.
          //
          return GetFirstDefaultVariableName (VariableNameSize, VariableName, VariableGuid);
        }
        GetVariableStore (Type, &IndexTable);
        Status = FindVariableEx (
                   VariableStoreHeader[Type],
                   IndexTable,
                   GET_VARIABLE_NAME_PTR (Variable.CurrPtr),
                   &Variable.CurrPtr->VendorGuid,
                   &VariablePtrTrack
                   );
        if (!EFI_ERROR (Status) && VariablePtrTrack.CurrPtr->State == VAR_ADDED) {
          Variable.CurrPtr = GetNextVariablePtr (Variable.CurrPtr);
          continue;
        }
      }

      //
      // Don't return NV variable when HOB overrides it
      //
      if ((VariableStoreHeader[VariableStoreTypeHob] != NULL) && (VariableStoreHeader[VariableStoreTypeNv] != NULL) &&
          (Variable.StartPtr == GetStartPointer (VariableStoreHeader[VariableStoreTypeNv]))
         ) {
        Status = FindVariableEx (
                   VariableStoreHeader[VariableStoreTypeHob],
                   NULL,
                   GET_VARIABLE_NAME_PTR (Variable.CurrPtr),
                   &Variable.CurrPtr->VendorGuid,
                   &VariableInHob
                   );
        if (!EFI_ERROR (Status)) {
          Variable.CurrPtr = GetNextVariablePtr (Variable.CurrPtr);
          continue;
        }
      }

      VarNameSize = NameSizeOfVariable (Variable.CurrPtr);
      ASSERT (VarNameSize != 0);

      if (VarNameSize <= *VariableNameSize) {
        CopyMem (VariableName, GET_VARIABLE_NAME_PTR (Variable.CurrPtr), VarNameSize);

        CopyMem (VariableGuid, &Variable.CurrPtr->VendorGuid, sizeof (EFI_GUID));

        Status = EFI_SUCCESS;
      } else {
        Status = EFI_BUFFER_TOO_SMALL;
      }

      *VariableNameSize = VarNameSize;
      //
      // Variable is found
      //
      return Status;
    } else {
      Variable.CurrPtr = GetNextVariablePtr (Variable.CurrPtr);
    }
  }
}
