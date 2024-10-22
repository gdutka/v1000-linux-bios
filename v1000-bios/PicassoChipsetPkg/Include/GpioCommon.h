/** @file

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

#ifndef _GPIO_COMMON_H_
#define _GPIO_COMMON_H_

//
// GPIO Function
//
typedef enum {
  GpioFunction0     = 0x0,
  GpioFunction1     = 0x1,
  GpioFunction2     = 0x2,
  GpioFunction3     = 0x3
} GPIO_FUNCTION;

//
// GPIO Enable
//
typedef enum {
  GpioDisable     = 0x0,
  GpioEnable      = 0x1
} GPIO_SWITCH;

//
// GPIO Socket
//
typedef enum {
  GpioSocket0     = 0x0,
  GpioSocket1     = 0x1
} GPIO_SOCKET;

//
// GPIO Die
//
typedef enum {
  GpioDie0     = 0x0,
  GpioDie1     = 0x1,
  GpioDie2     = 0x2,
  GpioDie3     = 0x3,
} GPIO_DIE;

//
// GPIO Output
//
typedef enum {
  GpioOutputLow  = 0x0,
  GpioOutputHigh = 0x1,
  GpioInput      = 0x2,
  GpioNA         = 0x3
} GPIO_OUTPUT;

//
// GPIO Pullup
//
typedef enum {
  GpioPullDownEnable = 0x0,
  GpioPullUpEnable   = 0x1,
  GpioPullDisable    = 0x2
} GPIO_PULLUP;

typedef struct {
  BOOLEAN            Enable;                     ///< GPIO Switch
  AMD_CPM_GPIO_ITEM  Gpio;
} GPIO_CONFIG;

//[-start-181222-IB09860036-add]//
#define GPIO_SETTING(function, output, pullup) \
        (function <= 3 ? 0x800 : 0) + (function << 8) + \
        (output < 3 ? (output < 2 ? output : 0x02) : 0x08) + ((pullup == 0 ? 0x40 : 0) + (pullup == 1 ? 0x20 : 0) + 0x80)
//[-end-181222-IB09860036-add]//

#endif
