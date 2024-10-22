/** @file

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

#include <PiPei.h>
#include <Library/IoLib.h>

EFI_STATUS
PeiEntry (
  IN EFI_FFS_FILE_HEADER    *FfsHeader,
  IN CONST EFI_PEI_SERVICES **PeiServices
  )
{
  IoWrite8 (0xCD6, 00);
  IoWrite8 (0xCD7, 0xF7); // bit0 enable legacy IO

  IoWrite32(0xCF8, 0x8000A004);
  IoWrite32(0xCFC, 0x02200403);

  //
  // UARTControlReg: UART0_1p843MCLK_EN and UART0_legacy_PW_FCR_Dis27 set to enable
  //
  IoWrite32(0xCF8, 0x8000A0FC);
  IoWrite32(0xCFC, 0x11000000);

  {
    UINT32 *IoMux = (UINT32*)(UINTN) 0xFED80D84;
    *IoMux = 0x0000000;
    IoMux++;
    *IoMux = 0x0000000;
  }

  return EFI_SUCCESS;
}

