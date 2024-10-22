/** @file
  Header file for PostCodeConfigLib
  
  This library provides functions to configure the debug registers
  and IDT table entry to perform PostCode feature.

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

#ifndef _INTERNAL_POST_CODE_MISC_LIB_H_
#define _INTERNAL_POST_CODE_MISC_LIB_H_

#include <Library/BaseLib.h>
#include <Library/DebugLib.h>

#define SCREEN_DISPLAY_1_BYTE "%02x"
#define SCREEN_DISPLAY_2_BYTE "%04x"
#define SCREEN_DISPLAY_4_BYTE "%08x"
#define SCREEN_CLEAR_1_BYTE   "  "
#define SCREEN_CLEAR_2_BYTE   "    "
#define SCREEN_CLEAR_4_BYTE   "        "
#define COM_PORT_1_BYTE       "<<Post Code>>:%02x\n"
#define COM_PORT_2_BYTE       "<<Post Code>>:%04x\n"
#define COM_PORT_4_BYTE       "<<Post Code>>:%08x\n"

#define MAX_STRING_SIZE       24


typedef struct {
  UINT8  OperandLength;
  UINT8  MachineCodeSize;
  UINT8  MachineCode[3];
} INSTRUCTION_MACHINE_CODE;


#endif 
