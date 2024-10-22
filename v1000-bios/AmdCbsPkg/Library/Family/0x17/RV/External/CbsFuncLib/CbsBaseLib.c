/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */
#include <Base.h>
#include <Uefi.h>
#include <Library/BaseLib.h>
#include "AmdSoc.h"

BOOLEAN
CbsFamilyIdentify (
  )
{
  UINT32     EAX_Reg;
  UINT32     EBX_Reg;
  UINT32     ECX_Reg;
  UINT32     EDX_Reg;
  UINT64     SocFamilyID;

  AsmCpuid (
      0x80000001,
      &EAX_Reg,
      &EBX_Reg,
      &ECX_Reg,
      &EDX_Reg
      );
  SocFamilyID = EAX_Reg & RAW_FAMILY_ID_MASK;
  if ((SocFamilyID == F17_RV_RAW_ID) || (SocFamilyID == F17_RV2_RAW_ID) || (SocFamilyID == F17_PIC_RAW_ID)) {
    return TRUE;
  }
  return FALSE;
}

