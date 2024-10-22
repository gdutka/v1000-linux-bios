/** @file
    Implementation functions and structures for var default services.

;******************************************************************************
;* Copyright (c) 2015 - 2018, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "VarDefault.h"

/**
  Internal function to get the start address of varaible default header according to
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

  if (PcdGetBool (PcdUseEcpVariableStoreHeader)) {
    for (EcpVariableStoreHeader = (ECP_VARIABLE_STORE_HEADER *) (UINTN) mVariableModuleGlobal->VariableDefaultCache;
         EcpVariableStoreHeader != NULL &&
         EcpVariableStoreHeader->Signature == ECP_VARIABLE_STORE_SIGNATURE &&
         EcpVariableStoreHeader->Format == VARIABLE_STORE_FORMATTED &&
         EcpVariableStoreHeader->State == VARIABLE_STORE_HEALTHY &&
         (EcpVariableStoreHeader->Flags & VARIABLE_STORE_ACTIVE_MASK) == VARIABLE_STORE_ACTIVE &&
         EcpVariableStoreHeader->Size >= sizeof (ECP_VARIABLE_STORE_HEADER) &&
         (UINTN) EcpVariableStoreHeader < (UINTN) mVariableModuleGlobal->VariableDefaultCache + mVariableModuleGlobal->VariableDefaultSize;
         EcpVariableStoreHeader = (ECP_VARIABLE_STORE_HEADER *) ((UINT8 *) EcpVariableStoreHeader + EcpVariableStoreHeader->Size)) {
      if (EcpVariableStoreHeader->DefaultId == 0 && SkuId == EcpVariableStoreHeader->BoardId) {
        return (VOID *) EcpVariableStoreHeader;
      }
    }
  } else {
    for (VariableStoreHeader = (VARIABLE_STORE_HEADER *) (UINTN) mVariableModuleGlobal->VariableDefaultCache;
         VariableStoreHeader != NULL &&
         (CompareGuid (&VariableStoreHeader->Signature, &gEfiAuthenticatedVariableGuid) ||
          CompareGuid (&VariableStoreHeader->Signature, &gEfiVariableGuid)) &&
         VariableStoreHeader->Format == VARIABLE_STORE_FORMATTED &&
         VariableStoreHeader->State == VARIABLE_STORE_HEALTHY &&
         (VariableStoreHeader->Flags & VARIABLE_STORE_ACTIVE_MASK) == VARIABLE_STORE_ACTIVE &&
         VariableStoreHeader->Size >= sizeof (VARIABLE_STORE_HEADER) &&
         (UINTN) VariableStoreHeader < (UINTN) mVariableModuleGlobal->VariableDefaultCache + mVariableModuleGlobal->VariableDefaultSize;
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

  @return  The address of variable store header or NULL if not found.
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
  UINTN                   VariableCount;

  Status = FindVariable (
             VariableName,
             VendorGuid,
             &Variable,
             &VariableCount,
             &mVariableModuleGlobal->VariableBase
             );
  return Status == EFI_SUCCESS;
}


/**
  Get variable from specific varaible store region.

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

  @param  VariableName  Name of the variable to be found
  @param  VendorGuid    Vendor GUID to be found.
  @param  PtrTrack      Variable Track Pointer structure that contains Variable Information.

  @retval EFI_SUCCESS   Find variable in variable default store successfully.
  @retval EFI_NOT_FOUND Cannot find variable in variable default store.
**/
EFI_STATUS
GetVariableFromVariableDefault (
  IN CONST  CHAR16            *VariableName,
  IN CONST  EFI_GUID          *VendorGuid,
  OUT VARIABLE_POINTER_TRACK  *PtrTrack
  )
{
  VOID                            *VariableStoreHeader;
  VOID                            *DefaultVariableStoreHeader;
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
  // It means we alreasy find first valid variable.
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
  Function to check the input address is whether in variable default region.

  @param[in] Address   Input physical address to check.

  @retval TRUE         The address is in the variable default region.
  @retval FALSE        The address isn't in the variable default region.
**/
BOOLEAN
AddressInVariableDefaultRegion (
  IN    PHYSICAL_ADDRESS   Address
  )
{

  if (Address >= (UINTN) mVariableModuleGlobal->VariableDefaultCache &&
      Address < (UINTN) mVariableModuleGlobal->VariableDefaultCache + mVariableModuleGlobal->VariableDefaultSize) {
    return TRUE;
  }
  return FALSE;
}
