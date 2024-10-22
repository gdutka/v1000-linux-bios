/** @file
  Header file for PostCodeConfigLib
  
  This library provides functions to configure the debug registers
  and IDT table entry to perform PostCode feature.

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

#ifndef _POST_CODE_CONFIG_LIB_H_
#define _POST_CODE_CONFIG_LIB_H_

#include <Library/BaseLib.h>

#define INTERRUPT_GATE_ATTRIBUTE 0x8e00

//
// The breakpoint numbler
//
typedef enum {
  Breakpoint0,
  Breakpoint1,
  Breakpoint2,
  Breakpoint3
} BREAKPOINT_NUMBER;

//
// The breakpoint condition
//
typedef enum {
  InstructionExecution,
  DataWrite,
  IoReadOrWrite,
  DataReadOrWrite
} BREAKPOINT_TYPE;

//
// The size of specified breakpoint address
//
typedef enum {
  OneByte,
  TwoByte,
  EightByte,
  fourByte
} BREAKPOINT_SIZE;


#pragma pack(1)

#if defined (MDE_CPU_IA32)
//
// The IA-32 architecture IDT interrupt gate descriptor
//
typedef struct {
  UINT16  OffsetLow;
  UINT16  SegmentSelector;
  UINT16  Attributes;
  UINT16  OffsetHigh;
} INTERRUPT_GATE_DESCRIPTOR;

#endif

#if defined (MDE_CPU_X64)
//
// The x64 architecture IDT interrupt gate descriptor
//
typedef struct {
  UINT16    Offset15To0;
  UINT16    SegmentSelector;
  UINT16    Attributes;
  UINT16    Offset31To16;
  UINT32    Offset63To32;
  UINT32    Reserved;
} INTERRUPT_GATE_DESCRIPTOR;

#endif

#pragma pack()


/**
 Register a new handler to the specified IDT entry.
 
 @param [in] VectorNumber   The vector number of IDT entry
 @param [in] Handler        The new handler

**/
VOID
EFIAPI
RegisterIdtHandler (
  IN UINT8 VectorNumber,
  IN UINTN Handler
  );

/**
 Configure the debug registers to perform the trap feature.

 @param [in] BpNumber    The specified breakpoint number
 @param [in] BpType      The breakpoint condition for the corresponding BpNumber
 @param [in] BpSize      The size of the memory location at the address specified in the BpAddress
 @param [in] BpAddress   The address of a breakpoint
 
**/
VOID
EFIAPI
SetUpDebugRegister (
  IN BREAKPOINT_NUMBER BpNumber,
  IN BREAKPOINT_TYPE   BpType,
  IN BREAKPOINT_SIZE   BpSize,
  IN UINTN             BpAddress
  );

/**
 Clean the trap status of the specified breakpoint

 @param [in] BpNumber   The specified breakpoint number

**/
VOID
EFIAPI
ClearTrapStatus (
  IN BREAKPOINT_NUMBER BpNumber
  );

/**
 Clean the debug registers to disable the trap feature
 
 @param [in] BpNumber   The specified breakpoint number

**/
VOID
EFIAPI
ClearDebugRegister (
  IN BREAKPOINT_NUMBER BpNumber
  );

#endif 
