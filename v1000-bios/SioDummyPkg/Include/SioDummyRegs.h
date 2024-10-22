/** @file
 This file contains following things about this SIO.

 1. Global configuration registers
 2. Common logical device registers
 3. Logical Device Number

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
#ifndef _SIO_DUMMY_REGS_H_
#define _SIO_DUMMY_REGS_H_

#include <SioCommonDefine.h>

#define SIO_DUMMY_CHIP_VENDOR                   SioInsydeDummy

//
// Logical Device Number
//
#define V_DUMMY_UART_PORT1                      SioLdnMaximum
#define V_DUMMY_UART_PORT2                      SioLdnMaximum
#define V_DUMMY_KBC                             SioLdnMaximum
#endif
