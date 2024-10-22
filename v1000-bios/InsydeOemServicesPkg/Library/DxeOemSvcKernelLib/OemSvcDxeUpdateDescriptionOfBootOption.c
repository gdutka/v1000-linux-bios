/** @file
  Provide OEM to customize the description string in Boot#### variable.

;******************************************************************************
;* Copyright (c) 2016, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <Library/DxeOemSvcKernelLib.h>

/**
 This function updates the description string in Boot#### variable.

 @param[in] DevicePath                Pointer to the device path of Boot#### variable. The device path describes the location
                                      of the device the handle is for.
 @param[in] FilePath                  Pointer to the file path of Boot#### variable. The file path describes the location
                                      of a file in a file system.
 @param[in] BbsTable                  A pointer to the BBS table of Boot#### variable. This parameter is only valid if
                                      the type of DevicePath is BIOS Boot Specification.
 @param[in, out] Description          On Input pointing to deafult description string.
                                      On Output pointing to customized description string.

 @retval EFI_UNSUPPORTED              Returns unsupported by default.
 @retval EFI_MEDIA_CHANGED            Description has been updated.
**/
EFI_STATUS
OemSvcDxeUpdateDescriptionOfBootOption (
  IN CONST EFI_DEVICE_PATH_PROTOCOL      *DevicePath,
  IN CONST EFI_DEVICE_PATH_PROTOCOL      *FilePath OPTIONAL,
  IN CONST BBS_TABLE                     *BbsTable OPTIONAL,
  IN OUT   CHAR16                        **Description
  )
{
  /*++
    Todo:
      Add project specific code in here.
  --*/

  return EFI_UNSUPPORTED;
}

