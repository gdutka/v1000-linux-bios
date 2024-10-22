/** @file
  The H2O_CAPSULE_UPDATE_PROGRESS_PROTOCOL is the interface to update capusle
  update progress.

;******************************************************************************
;* Copyright (c) 2017, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _H2O_CAPSULE_UPDATE_PROGRESS_PROTOCOL_H_
#define _H2O_CAPSULE_UPDATE_PROGRESS_PROTOCOL_H_

#define H2O_CAPSULE_UPDATE_PROGRESS_PROTOCOL_GUID \
  { \
    0x142b9a2a, 0x2e71, 0x486f, 0x80, 0xbd, 0x5b, 0x7b, 0xda, 0xc1, 0xde, 0xec \
  }

//
// Forward reference for pure ANSI compatability
//
typedef struct  _H2O_CAPSULE_UPDATE_PROGRESS_PROTOCOL H2O_CAPSULE_UPDATE_PROGRESS_PROTOCOL;

//
// Protocol API definitions
//

/**
  This function updates the hardware with the new firmware image.

  @param[in]  Completion           A value between 1 and 100 indicating the current completion progress of the
                                   firmware update. Completion progress is reported as from 1 to 100 percent.
                                   A value of 0 is used by the driver to indicate that progress reporting is not
                                   supported.

  @retval EFI_SUCCESS              The device was successfully updated with the new image.
*/
typedef
EFI_STATUS
(EFIAPI *H2O_CAPSULE_UPDATE_PROGRESS) (
 IN UINTN             Completion
) ;

//
// Protocol declaration
//
struct _H2O_CAPSULE_UPDATE_PROGRESS_PROTOCOL {
  H2O_CAPSULE_UPDATE_PROGRESS               CapsuleUpdateProgress;
  EFI_GUID                                  FwClass;
};

extern EFI_GUID gH2OCapsuleUpdateProgressProtocolGuid;

#endif
