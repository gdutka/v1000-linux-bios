/** @file
  Initialize DMA controller for ISA device

;******************************************************************************
;* Copyright (c) 2019, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <Uefi.h>

#include <Library/IoLib.h>

#include "8237aRegs.h"

/**
  Initialize DMA controller for ISA device

  This function initialize a 2-level DMA system consist of 2 Intel 8237A
  (-like) DMA controllers.

**/
VOID
InitDmaController (
  VOID
  )
{
  UINT8   Data8;
  UINTN   Channel;

  //
  // Clear command register and internal flip-flop
  //
  IoWrite8 (R_8237A_DMA1_STS_CMD, 0);
  IoWrite8 (R_8237A_DMA1_CLEAR_BYTE_POINTER, 0);
  IoWrite8 (R_8237A_DMA2_STS_CMD, 0);
  IoWrite8 (R_8237A_DMA2_CLEAR_BYTE_POINTER, 0);

  //
  // Setup CH.0 (a.k.a. CH.4) on DMA2 for cascading
  //
  Data8 =
    V_8237A_CHANNEL_0 |
    (V_8237A_VERIFY_TRANSFER << N_8237A_OPERATION) |
    (V_8237A_AUTO_INIT_DISABLE << N_8237A_AUTO_INIT) |
    (V_8237A_ADDRESS_INCREMENT << N_8237A_ADDRESS_DIRECTION) |
    (V_8237A_CASCADE_MODE << N_8237A_MODE_SELECT);

  IoWrite8 (R_8237A_DMA2_MODE, Data8);

  //
  // Clear Request Register and unset Mask Register of CH.0 (a.k.a. CH.4) on DMA2
  //
  IoWrite8 (R_8237A_DMA2_REQ, 0);
  IoWrite8 (R_8237A_DMA2_SINGLE_MASK, 0);


  for (Channel = 0; Channel <= 3; Channel++) {
    //
    // Setup Mode Register for each channel (except for CH.4)
    //
    Data8 =
      (Channel & B_8237A_CHANNEL_SELECT) |
      (V_8237A_AUTO_INIT_DISABLE << N_8237A_AUTO_INIT) |
      (V_8237A_ADDRESS_INCREMENT << N_8237A_ADDRESS_DIRECTION) |
      (V_8237A_SINGLE_MODE << N_8237A_MODE_SELECT);

    IoWrite8 (R_8237A_DMA1_MODE, Data8);
    if (Channel != 0) {
      IoWrite8 (R_8237A_DMA2_MODE, Data8);
    }

    //
    // Clear Request Register and unset Mask Register for each channel (except for CH.4)
    //
    IoWrite8 (R_8237A_DMA1_REQ, 0);
    IoWrite8 (R_8237A_DMA1_SINGLE_MASK, 0);
    if (Channel != 0) {
      IoWrite8 (R_8237A_DMA1_REQ, 0);
      IoWrite8 (R_8237A_DMA2_SINGLE_MASK, 0);
    }
  }
}
