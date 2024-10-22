/** @file
  The instance of PostCodeConfigLib Library

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

#include <Library/PostCodeMiscLib.h>
#include "InternalPostCodeMiscLib.h"

//[-start-240426-IB18160212-modify-]//
INSTRUCTION_MACHINE_CODE mOutInstructionList [] = {
  {0x01, 0x02, {(UINT8) FixedPcdGet16 (PcdH2OPostCodeDebugPortAddress), 0xE6, 0x00}},  // out  0x80,al
  {0x02, 0x03, {(UINT8) FixedPcdGet16 (PcdH2OPostCodeDebugPortAddress), 0xE7, 0x66}},  // out  0x80,ax
  {0x04, 0x02, {(UINT8) FixedPcdGet16 (PcdH2OPostCodeDebugPortAddress), 0xE7, 0x00}},  // out  0x80,eax
  {0x01, 0x01, {0xEE, 0x00, 0x00}},                                                    // out  dx,al
  {0x02, 0x02, {0xEF, 0x66, 0x00}},                                                    // out  dx,ax
  {0x04, 0x01, {0xEF, 0x00, 0x00}}                                                     // out  dx,eax
};

INSTRUCTION_MACHINE_CODE mInInstructionList [] = {
  {0x01, 0x01, {0xEC, 0x00, 0x00}}
};
//[-end-240426-IB18160212-modify-]//

VOID
EFIAPI
GetPostCodeLength (
  IN  UINTN SystemContextIp,
  OUT UINT8 *PostCodeLength
  )
{
  UINT8   *InsData;
  UINT8   ListIndex;
  UINT8   CodeIndex;
  BOOLEAN FindLength;

  if (PostCodeLength == NULL) {
    return;
  }

  InsData = (UINT8*)((VOID*) SystemContextIp);

  //
  // Get the last byte of the previous instruction
  //
  InsData -= 1;

  //
  // Compare the previous instruction with each out instruction of the mOutInstructionList
  // to get the operand size of the previous instruction (Ex: out 0x80,al => the size of al is 1 byte)
  // This size will be the length of the Post Code
  //
  FindLength = FALSE;
  for (ListIndex = 0; ListIndex < sizeof (mOutInstructionList) / sizeof (INSTRUCTION_MACHINE_CODE); ListIndex++) {
    for (CodeIndex = 0; CodeIndex < mOutInstructionList[ListIndex].MachineCodeSize; CodeIndex++) {
      if (*(InsData - CodeIndex) != mOutInstructionList[ListIndex].MachineCode[CodeIndex]) {
        break;
      }

      if (CodeIndex == (mOutInstructionList[ListIndex].MachineCodeSize - 1)) {
        *PostCodeLength = mOutInstructionList[ListIndex].OperandLength;
        FindLength = TRUE;
        break;
      }
    }
    if (FindLength) {
      break;
    }
  }

  //
  // If we can't parse the previous instruction, return the default length
  //
  if (!FindLength) {
    for (ListIndex = 0; ListIndex < sizeof (mInInstructionList) / sizeof (INSTRUCTION_MACHINE_CODE); ListIndex++) {
      for (CodeIndex = 0; CodeIndex < mInInstructionList[ListIndex].MachineCodeSize; CodeIndex++) {
        if (*(InsData - CodeIndex) != mInInstructionList[ListIndex].MachineCode[CodeIndex]) {
          break;
        }

        if (CodeIndex == (mInInstructionList[ListIndex].MachineCodeSize - 1)) {
          *PostCodeLength = 0xFF;
          break;
        }
      }
      if (*PostCodeLength == 0xFF) {
        return;
      }
    }

    ASSERT (FALSE);
    *PostCodeLength = 0x01;
  }
}

UINTN
EFIAPI
GetScreenPostCodeStr (
  IN  UINT8              DisplayLength,
  IN  UINTN              PostCode,
  IN  UINTN              BufferSize,
  OUT CHAR8              *Buffer
  )
{
  UINTN NumofChar;

  switch (DisplayLength) {
  case 2:
    NumofChar = AsciiSPrint (Buffer, BufferSize, SCREEN_DISPLAY_2_BYTE, (UINT16) PostCode);
    break;

  case 4:
    NumofChar = AsciiSPrint (Buffer, BufferSize, SCREEN_DISPLAY_4_BYTE, (UINT32) PostCode);
    break;

  case 1:
  default:
    NumofChar = AsciiSPrint (Buffer, BufferSize, SCREEN_DISPLAY_1_BYTE, (UINT8) PostCode);
  }

  return NumofChar;
}

UINTN
EFIAPI
GetScreenClearStr (
  IN  UINT8              DisplayLength,
  IN  UINTN              BufferSize,
  OUT CHAR8              *Buffer
  )
{
  UINTN NumofChar;

  switch (DisplayLength) {
  case 2:
    NumofChar = AsciiSPrint (Buffer, BufferSize, SCREEN_CLEAR_2_BYTE);
    break;

  case 4:
    NumofChar = AsciiSPrint (Buffer, BufferSize, SCREEN_CLEAR_4_BYTE);
    break;

  case 1:
  default:
    NumofChar = AsciiSPrint (Buffer, BufferSize, SCREEN_CLEAR_1_BYTE);
  }

  return NumofChar;
}

UINTN
EFIAPI
GetComPortPostCodeStr (
  IN  UINT8              DisplayLength,
  IN  UINTN              PostCode,
  IN  UINTN              BufferSize,
  OUT CHAR8              *Buffer
  )
{
  UINTN NumofChar;

  switch (DisplayLength) {
  case 2:
    NumofChar = AsciiSPrint (Buffer, BufferSize, COM_PORT_2_BYTE, (UINT16) PostCode);
    break;

  case 4:
    NumofChar = AsciiSPrint (Buffer, BufferSize, COM_PORT_4_BYTE, (UINT32) PostCode);
    break;

  case 1:
  default:
    NumofChar = AsciiSPrint (Buffer, BufferSize, COM_PORT_1_BYTE, (UINT8) PostCode);
  }

  return NumofChar;
}

UINTN
EFIAPI
GetPostCodeString (
  IN  POST_CODE_STR_TYPE StringType,
  IN  UINTN              PostCode,     OPTIONAL
  IN  UINTN              BufferSize,
  OUT CHAR8              *Buffer
  )
{
  UINT8 DisplayLength;
  UINTN NumofChar;

  if (Buffer == NULL) {
    return 0;
  }

  if (BufferSize < MAX_STRING_SIZE) {
    return 0;
  }

  DisplayLength = FixedPcdGet8 (PcdH2OPostCodeDisplayLength);

  switch (StringType) {
  case ScreenDisplayStr:
    NumofChar = GetScreenPostCodeStr (DisplayLength, PostCode, BufferSize, Buffer);
    break;

  case ScreenClearStr:
    NumofChar = GetScreenClearStr (DisplayLength, BufferSize, Buffer);
    break;

  case ComPortStr:
    NumofChar = GetComPortPostCodeStr (DisplayLength, PostCode, BufferSize, Buffer);
    break;

  default:
    NumofChar = 0;
  }

  if (((PostCode) & 0x000000FF) == 0x91) {
	  NumofChar = 0;
     }
  return NumofChar;
}

