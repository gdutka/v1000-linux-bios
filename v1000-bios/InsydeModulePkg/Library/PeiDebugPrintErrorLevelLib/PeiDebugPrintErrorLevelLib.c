/** @file
  Debug Print Error Level library instance that retrieves the current error
  level from PcdDebugPrintErrorLevel.  This generic library instance does not
  support the setting of the global debug print error level mask for the platform.

  Copyright (c) 2011, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Base.h>
#include <PiPei.h>
#include <Library/DebugPrintErrorLevelLib.h>
#include <Library/PcdLib.h>

#include <Library/PeiServicesTablePointerLib.h>
#include <Library/VariableLib.h>
#include <Library/HobLib.h>
#include <Guid/DebugMask.h>

STATIC EFI_GUID       mInternalDebugPrintErrorLevelGuid = {0x778a82c7, 0x4d64, 0x43ed, 0x8e, 0x40, 0x42, 0xc2, 0xc7, 0x37, 0xbc, 0x5b};

/**
  Returns the debug print error level mask for the current module.

  @return  Debug print error level mask for the current module.

**/
UINT32
EFIAPI
GetDebugPrintErrorLevel (
  VOID
  )
{
  EFI_STATUS                      Status;
  UINTN                           Size;
  UINT64                          GlobalErrorLevel;
  UINT32                          DebugPrintErrorLevel;
  EFI_HOB_GUID_TYPE               *GuidHob;
  UINT32                          *HobInfo;

  //
  // Try to get print debug level from HOB to reduce POST time by reducing unnecessary
  // get variable.
  //
  GuidHob = GetFirstGuidHob (&mInternalDebugPrintErrorLevelGuid);
  if (GuidHob != NULL) {
    return *((UINT32 *) GET_GUID_HOB_DATA (GuidHob));
  }

  //
  // Retrieve the current debug print error level mask from PcdDebugPrintErrorLevel.
  //
  DebugPrintErrorLevel = PcdGet32 (PcdDebugPrintErrorLevel);

  Size = sizeof (GlobalErrorLevel);
  Status = CommonGetVariable (
             DEBUG_MASK_VARIABLE_NAME,
             &gEfiGenericVariableGuid,
             &Size,
             &GlobalErrorLevel
             );
  if (!EFI_ERROR (Status)) {
    DebugPrintErrorLevel = (UINT32)GlobalErrorLevel;
  }
  //
  // Store the print debug level to HOB and then it can be found from HOB next time.
  //
  HobInfo  = BuildGuidHob (&mInternalDebugPrintErrorLevelGuid, sizeof (UINT32));
  if (HobInfo != NULL) {
    *HobInfo = DebugPrintErrorLevel;
  }
  return DebugPrintErrorLevel;
}

/**
  Sets the global debug print error level mask fpr the entire platform.

  @param   ErrorLevel     Global debug print error level.

  @retval  TRUE           The debug print error level mask was sucessfully set.
  @retval  FALSE          The debug print error level mask could not be set.

**/
BOOLEAN
EFIAPI
SetDebugPrintErrorLevel (
  UINT32  ErrorLevel
  )
{
  //
  // This library uinstance does not support setting the global debug print error
  // level mask.
  //
  return FALSE;
}

