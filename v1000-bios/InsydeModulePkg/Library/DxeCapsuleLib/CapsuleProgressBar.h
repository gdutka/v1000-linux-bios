/** @file
  Header file for Capsule Progress Bar

;******************************************************************************
;* Copyright (c) 2017, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _CAPSULE_PROGRESS_BAR_H_
#define _CAPSULE_PROGRESS_BAR_H_

#include <Uefi.h>
#include <PiDxe.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/BaseLib.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/PrintLib.h>
#include <Library/DxeServicesLib.h>
#include <Protocol/FirmwareVolume2.h>
#include <Protocol/GraphicsOutput.h>
#include <Protocol/TgaDecoder.h>

//
// Progress image type definitions
//
#define CAPSULE_PROGRESS_BOARD                     1
#define CAPSULE_PROGRESS_BAR                       2

//
// Setting position as below:
//   Position X = Horizontal Resolution / HORIZONTAL_SEGMENTATION * HORIZONTAL_POSITION_MULTIPLE
//   Position Y = Vertical Resolution   / VERTICAL_SEGMENTATION   * VERTICAL_POSITION_MULTIPLE
//
// Setting width of Progress Bar as below:
//   Width of Progress Bar = (HorizontalResolution / HORIZONTAL_SEGMENTATION) * HORIZONTAL_MULTIPLE
//
#define HORIZONTAL_SEGMENTATION                    3
#define HORIZONTAL_POSITION_MULTIPLE               1
#define HORIZONTAL_MULTIPLE                        1
#define VERTICAL_SEGMENTATION                      6
#define VERTICAL_POSITION_MULTIPLE                 4

//
// Setting keep image edge pixel for avoid edge distortion as below:
//   Keep Horizontal Pixel = IMAGE_SCALING_KEEP_EDGE_HORIZONTAL_PIXEL
//   Keep Vertical Pixel   = IMAGE_SCALING_KEEP_EDGE_VERTICAL_PIXEL
//
#define IMAGE_SCALING_KEEP_EDGE_HORIZONTAL_PIXEL   15
#define IMAGE_SCALING_KEEP_EDGE_VERTICAL_PIXEL     15

//
// Setting compute accuracy for after the decimal point as below:
//   Compute accuracy = PROCESS_ACCURACY
//
#define PROCESS_ACCURACY                           100

/**
  Function to display progress bar to reflect current completion progress of the firmware
  update.

  @param  Completion    A value between 1 and 100 indicating the current completion progress of the firmware update

  @retval EFI_SUCESS    Input capsule is a correct FMP capsule.
**/
EFI_STATUS
CapsuleProgressBar (
  IN   UINTN                             Completion
  );

#endif
