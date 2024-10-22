/*****************************************************************************
 * Copyright (C) 2016-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 ******************************************************************************
 */
#include <Uefi.h>
#include <Library/IoLib.h>

/*--------------------------------------------------------------------------------------*/
/**
 *  Hook to store the vector that all APs should jump to out of reset to a non-volatile,
 *  shared location.
 *
 *  @param[in]     ApInitAddress     Address of the code that AP should jump to
 *  @param[in,out] ContentToRestore  The current value in the non-volatile storage
 *
 **/
/*--------------------------------------------------------------------------------------*/
VOID
SaveApInitVector (
  IN       UINT32  ApInitAddress,
  IN OUT   UINT32  *ContentToRestore
  )
{
  UINT32                  Value32;
  UINTN                   Index;

  *ContentToRestore = 0;
  //
  // !!! This is one of the suggested solutions where BSP can launch AP without
  // going through intitializing of Pei Core Services. This function saves the
  // ApInit pointer. In this process, the current content of CMOS gets saved
  // and returned to caller which will be restored once APInit is done by
  // calling RestoreContentVector
  //
  for (Index = 0xA0; Index <= 0xA3; Index++) {
    IoWrite8 (0x72, (UINT8)Index);
    Value32 = (UINT32)(IoRead8 (0x73));
    *ContentToRestore |= (Value32 << ((Index - 0xA0)*8));
    Value32 = (UINT32) (ApInitAddress & 0xff);
    IoWrite8 (0x73, (UINT8)Value32);
    ApInitAddress >>= 8;
  }
}

/*--------------------------------------------------------------------------------------*/
/**
 *  Hook to restore the initial content of the non-volatile storage location.
 *
 *  @param[in]     ContentToRestore  The value to restore
 *
 **/
/*--------------------------------------------------------------------------------------*/
VOID
RestoreContentVector (
  IN       UINT32  ContentToRestore
  )
{
  UINT32                 Value32;
  UINTN                  Index;

  Value32 = 0;
  for (Index = 0xA0; Index <= 0xA3; Index++) {
    IoWrite8 (0x72, (UINT8)Index);
    Value32 = (UINT8) ((ContentToRestore >> (Index - 0xA0)*8));
    IoWrite8 (0x73, (UINT8)Value32);
  }
}

