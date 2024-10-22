/** @file

  Delay for at least the request number of microseconds.

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
#ifndef _SCU_VAR_INFO_LIB_H_
#define _SCU_VAR_INFO_LIB_H_

#pragma pack(1)
typedef struct {
  EFI_GUID              Guid;
  UINT32                Attributes;
  UINTN                 DataSize;
  VOID                 (*LoadDefaultCallBack)(UINT8 *);
  CHAR16                *VarName;
} SCU_VAR_INFO;
#pragma pack()

EFI_STATUS 
GetSCUVarInfo (
  IN CONST CHAR16                           *VariableName,
  IN CONST EFI_GUID                         *VendorGuid,
  OUT SCU_VAR_INFO                          *ScuVarInfo
  );
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

EFI_STATUS 
GetSCUVarInfoByIndex (
  IN  UINT16                                Index,
  OUT SCU_VAR_INFO                          *ScuVarInfo
  );
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

UINT16 
GetSCUVarInfoListCount (
  VOID
  );
/*++

Routine Description:

  Get the count of SCU Variable information list.

Arguments:

  None.

Returns:

 UINT16   - the count of SCU Variable information list.

--*/

#endif
