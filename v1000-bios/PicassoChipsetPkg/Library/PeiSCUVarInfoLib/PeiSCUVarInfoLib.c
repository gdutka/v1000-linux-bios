/** @file

  Delay for at least the request number of microseconds.  Timer used is DMA refresh timer, which
  has 15us granularity. You can call with any number of microseconds, but this implementation 
  cannot support 1us granularity.

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/
#include <Library/BaseMemoryLib.h>
#include <Library/KernelConfigLib.h>
#include <Library/BaseLib.h>
#include <Library/SetupUtilityLib.h>
#include <Library/BaseSetupDefaultLib.h>
#include <Include/SetupConfig.h>
#include <Library/SCUVarInfoLib.h>


STATIC CONST SCU_VAR_INFO        mSCUVarInfo[] = {
                                   {
                                     SYSTEM_CONFIGURATION_GUID,
                                     EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                                     sizeof (SYSTEM_CONFIGURATION),
                                     &ExtractSetupDefault,
                                     L"Setup"
                                   },
                                 };

EFI_STATUS 
GetSCUVarInfo (
  IN CONST CHAR16                           *VariableName,
  IN CONST EFI_GUID                         *VendorGuid,
  OUT SCU_VAR_INFO                          *ScuVarInfo
  ) 
/*++

Routine Description:

  Get a SCU Variable information from mSCUVarInfo by Variable name and Guid.

Arguments:

  VariableName - Variable name
  VendorGuid   - Vendor Guid
  ScuVarInfo   - SCU Variable information. (The space need caller to prepare)

Returns:

 EFI_SUCCESS   - Find the SCU Variable information successfully.
 EFI_INVALID_PARAMETER - VariableName, VendorGuid or ScuVarInfo is NULL.
 EFI_NOT_FOUND - Can't find the specified SCU Variable information.

--*/
{
  UINT16 Index;

  if ((VariableName == NULL) || (VendorGuid == NULL) || (ScuVarInfo == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  for (Index = 0; Index < (sizeof (mSCUVarInfo) / sizeof (SCU_VAR_INFO)); Index++) {
    if (CompareGuid ((EFI_GUID *)VendorGuid, &mSCUVarInfo[Index].Guid) &&
      (StrCmp (VariableName, mSCUVarInfo[Index].VarName) == 0)) {
      CopyMem(ScuVarInfo, &mSCUVarInfo[Index], sizeof (SCU_VAR_INFO));
      return EFI_SUCCESS;
    }
  }

  return EFI_NOT_FOUND;
}

EFI_STATUS 
GetSCUVarInfoByIndex (
  IN  UINT16                                Index,
  OUT SCU_VAR_INFO                          *ScuVarInfo
  ) 
/*++

Routine Description:

  Get a SCU Variable information from mSCUVarInfo by Index.

Arguments:

  Index        - The Index of mSCUVarInfo.
  ScuVarInfo   - SCU Variable information. (The space need caller to prepare)

Returns:

 EFI_SUCCESS   - Get the SCU Variable information successfully.
 EFI_INVALID_PARAMETER - Index or ScuVarInfo is not correct.

--*/
{
  if ((Index >= GetSCUVarInfoListCount ()) || ((ScuVarInfo == NULL))) {
    return EFI_INVALID_PARAMETER;
  }

  CopyMem(ScuVarInfo, &mSCUVarInfo[Index], sizeof (SCU_VAR_INFO));

  return EFI_SUCCESS;
}

UINT16 
GetSCUVarInfoListCount (
  VOID
  ) 
/*++

Routine Description:

  Get the count of SCU Variable information list.

Arguments:

  None.

Returns:

 UINT16   - the count of SCU Variable information list.

--*/
{
  return (UINT16)(sizeof (mSCUVarInfo) / sizeof (SCU_VAR_INFO));
}
