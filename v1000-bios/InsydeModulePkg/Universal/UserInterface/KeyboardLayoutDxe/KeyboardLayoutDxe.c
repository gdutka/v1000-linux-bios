/** @file
 Keyboard layout driver to install keyboard layouts into HII database.

;******************************************************************************
;* Copyright (c) 2018, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "KeyboardLayoutDxe.h"
#include "UsKeyboardLayout.h"
#include "FrenchKeyboardLayout.h"
#include "UkKeyboardLayout.h"

/**
 Install keyboard layouts into HII database and set default keyboard layout.

 @param[in] ImageHandle    The firmware allocated handle for the EFI image.
 @param[in] SystemTable    A pointer to the EFI System Table.

 @retval EFI_SUCCESS       The entry point is executed successfully.
 @retval other             Some error occurs when executing this entry point.
**/
EFI_STATUS
EFIAPI
KeyboardLayoutDxeEntryPoint (
  IN EFI_HANDLE                ImageHandle,
  IN EFI_SYSTEM_TABLE          *SystemTable
  )
{
  EFI_STATUS                   Status;
  EFI_HII_DATABASE_PROTOCOL    *HiiDatabase;

  Status = gBS->LocateProtocol (&gEfiHiiDatabaseProtocolGuid, NULL, (VOID **) &HiiDatabase);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Install Keyboard Layout package to HII database
  //
  if (FeaturePcdGet (PcdH2OKeyboardLayoutUsSupported)) {
    HiiAddPackages (&mH2OKeyboardLayoutUsBin.Guid, NULL, &mH2OKeyboardLayoutUsBin, NULL);
  }
  if (FeaturePcdGet (PcdH2OKeyboardLayoutFrenchSupported)) {
    HiiAddPackages (&mH2OKeyboardLayoutFrenchBin.Guid , NULL, &mH2OKeyboardLayoutFrenchBin, NULL);
  }
  if (FeaturePcdGet (PcdH2OKeyboardLayoutUkSupported)) {
    HiiAddPackages (&mH2OKeyboardLayoutUkBin.Guid, NULL, &mH2OKeyboardLayoutUkBin, NULL);
  }

  Status = HiiDatabase->SetKeyboardLayout (HiiDatabase, (EFI_GUID *) PcdGetPtr(PcdH2ODefautKeyboardLayout));
  ASSERT_EFI_ERROR (Status);
  return Status;
}

