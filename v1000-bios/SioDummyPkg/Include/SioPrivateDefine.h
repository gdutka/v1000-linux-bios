/** @file
 This file contains the following information about this SIO.

 1. redefined global configuration registers
 2. redefined common logical device registers

 This is not intended to be used by any code outside of this SIO package.

;******************************************************************************
;* Copyright 2017 Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corp.
;*
;******************************************************************************
*/

#ifndef _SIO_PRIVATE_DEFINE_H_
#define _SIO_PRIVATE_DEFINE_H_

#include <SioDummyRegs.h>

#define SIO_CHIP_VENDOR                   SIO_DUMMY_CHIP_VENDOR

//
// Redefine the global configuration registers
//
#define  V_UART_PORT1                     V_DUMMY_UART_PORT1
#define  V_UART_PORT2                     V_DUMMY_UART_PORT2
#define  V_KBC                            V_DUMMY_KBC
#endif
