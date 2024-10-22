/** @file
    The header file for variable default service.

;******************************************************************************
;* Copyright (c) 2015, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _INTERNAL_VARIABLE_DEFAULT_H_
#define _INTERNAL_VARIABLE_DEFAULT_H_
#include "Variable.h"
#include <Library/FlashRegionLib.h>

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
  );


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
  );

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
  );

/**
  Function to check the input address is whether in variable default region.

  @param[in] Address   Input physical address to check.

  @retval TRUE         The address is in the variable default region.
  @retval FALSE        The address isn't in the variable default region.
**/
BOOLEAN
AddressInVariableDefaultRegion (
  IN    PHYSICAL_ADDRESS   Address
  );

#endif
