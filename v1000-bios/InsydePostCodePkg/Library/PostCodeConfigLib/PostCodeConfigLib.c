/** @file
  The instance of PostCodeConfigLib Library

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

#include <Library/PostCodeConfigLib.h>

/**
 Configure the debug registers to perform the trap feature.

 @param [in] BpNumber   The specified breakpoint number
 @param [in] BpType     The breakpoint condition for the corresponding BpNumber
 @param [in] BpSize     The size of the memory location at the address specified in the BpAddress
 @param [in] BpAddress  The address of a breakpoint
 
**/
VOID
EFIAPI
SetUpDebugRegister (
  IN BREAKPOINT_NUMBER BpNumber,
  IN BREAKPOINT_TYPE   BpType,
  IN BREAKPOINT_SIZE   BpSize,
  IN UINTN             BpAddress
  )
{
  UINTN  Cr4;
  UINTN  Dr7;
  UINT32 RegField;
  UINT32 RegSet;
  
  //
  // Set the DE flag
  //
  Cr4 = AsmReadCr4 ();
  Cr4 |= 0x8;
  AsmWriteCr4 (Cr4);

  //
  // Set the trap address to the register of specified breakpoint (DR0 ~ DR3)
  // Compute the field of DR7 and the data which will be set to DR7 
  //
  switch (BpNumber) {

  case Breakpoint0:
    AsmWriteDr0 (BpAddress);
    RegField = 0xFFF0FFFC;
    RegSet   = (BpSize << 18) | (BpType << 16) | 0x03;
    break;
    
  case Breakpoint1:
    AsmWriteDr1 (BpAddress);
    RegField = 0xFF0FFFF3;
    RegSet   = (BpSize << 22) | (BpType << 20) | 0x0C;
    break;
    
  case Breakpoint2:
    AsmWriteDr2 (BpAddress);
    RegField = 0xF0FFFFCF;
    RegSet   = (BpSize << 26) | (BpType << 24) | 0x30;
    break;
    
  case Breakpoint3:
    AsmWriteDr3 (BpAddress);
    RegField = 0x0FF0FF30;
    RegSet   = (BpSize << 30) | (BpType << 28) | 0xC0;
    break;
    
  default:
    AsmWriteDr0 (BpAddress);
    RegField = 0xFFF0FFFC;
    RegSet   = (BpSize << 18) | (BpType << 16) | 0x03;
    break;
  }

  //
  // Set the type and size of the specified breakpoint
  // And enable the specified breakpoint
  //
  Dr7 = AsmReadDr7 ();
  Dr7 &= RegField;
  Dr7 |= RegSet;
  AsmWriteDr7 (Dr7);

  //
  // Clean the trap status of specified breakpoint
  //
  ClearTrapStatus (BpNumber);
}

/**
 Clean the trap status of the specified breakpoint

 @param [in] BpNumber   The specified breakpoint number

**/
VOID
EFIAPI
ClearTrapStatus (
  IN BREAKPOINT_NUMBER BpNumber
  )
{
  UINTN  Dr6;
  UINT32 RegSet;

  //
  // Compute the specified breakpoint status field
  //
  RegSet = ~(0x00000001 << BpNumber);

  //
  // Clean the specified breakpoint trap status
  //
  Dr6 = AsmReadDr6 ();
  Dr6 &= RegSet;
  AsmWriteDr6 (Dr6);
}

/**
 Clean the debug registers to disable the trap feature
 
 @param [in] BpNumber   The specified breakpoint number

**/
VOID
EFIAPI
ClearDebugRegister (
  IN BREAKPOINT_NUMBER BpNumber
  )
{
  UINTN  Dr7;  
  UINT32 RegSet;

  //
  // Clean the trap address of specified breakpoint
  // Compute the field of DR7
  //
  switch (BpNumber) {

  case Breakpoint0:
    AsmWriteDr0 (0);
    RegSet = 0xFFF0FFFC;
    break;
    
  case Breakpoint1:
    AsmWriteDr1 (0);
    RegSet = 0xFF0FFFF3;
    break;
    
  case Breakpoint2:
    AsmWriteDr2 (0);
    RegSet = 0xF0FFFFCF;
    break;
    
  case Breakpoint3:
    AsmWriteDr3 (0);
    RegSet = 0x0FF0FF30;
    break;
    
  default:
    AsmWriteDr0 (0);
    RegSet = 0xFFF0FFFC;
    break;
  }

  //
  // Clean DR7
  //
  Dr7 = AsmReadDr7 ();
  Dr7 &= RegSet;
  AsmWriteDr7 (Dr7);

  //
  // Clean the specified breakpoint trap status
  //
  ClearTrapStatus (BpNumber);
}

