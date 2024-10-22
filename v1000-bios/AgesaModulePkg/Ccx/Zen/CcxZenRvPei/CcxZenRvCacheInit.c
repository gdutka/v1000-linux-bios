/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD CCX Zen - Zeppelin API, and related functions.
 *
 * Contains code that initializes the core complex
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CCX
 * @e \$Revision$   @e \$Date$
 *
 */
/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */
/*++
Module Name:

  CcxZenRvCacheInit.c
  Initialize Cache

Abstract:
--*/

#include <Library/BaseLib.h>
#include <AGESA.h>
#include <Filecode.h>
#include <Library/AmdBaseLib.h>
#include <Library/CcxBaseX86Lib.h>

#define FILECODE CCX_ZEN_CCXZENRVPEI_CCXZENRVCACHEINIT_FILECODE


/* -----------------------------------------------------------------------------*/
/**
 *
 *
 * Program MTRRs to describe given range as given cache type.  Use MTRR pairs
 * starting with the given MTRRphys Base address, and use as many as is
 * required up to (excluding) MSR 020C, which is reserved for OS.
 *
 * "Limit" in the context of this procedure is not the numerically correct
 * limit, but rather the Last address+1, for purposes of coding efficiency
 * and readability.  Size of a region is then Limit-Base.
 *
 * 1. Size of each range must be a power of two
 * 2. Each range must be naturally aligned (Base is same as size)
 *
 * There are two code paths: the ascending path and descending path (analogous
 * to bsf and bsr), where the next limit is a function of the next set bit in
 * a forward or backward sequence of bits (as a function of the Limit).  We
 * start with the ascending path, to ensure that regions are naturally aligned,
 * then we switch to the descending path to maximize MTRR usage efficiency.
 * Base=0 is a special case where we start with the descending path.
 * Correct Mask for region is 2comp(Size-1)-1,
 * which is 2comp(Limit-Base-1)-1 *
 * @param[in]     Base - Base address[47:16] of specified range.
 * @param[in]     *LimitPtr - Limit address[47:16] of specified range.
 * @param[in]     MtrrAddr - address of var MTRR pair to start using.
 * @param[in]     MtrrType - Cache type for the range.
 * @param[in]  StdHeader         Config handle for library and services
 *
 *
 * @retval     AGESA_STATUS
 */

AGESA_STATUS
SetVariableMTRRs (
  IN       UINT32            Base,
  IN OUT   UINT32            *LimitPtr,
  IN       UINT32            MtrrAddr,
  IN       UINT8             MtrrType,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT64 LocalMsrRegister;
  UINT32 CurBase;
  UINT32 CurLimit;
  UINT32 CurSize;
  UINT32 CurAddr;
  UINT32 Value32;

  CurBase = Base;
  CurLimit = *LimitPtr;
  CurAddr = MtrrAddr;

  while ((CurAddr >= 0x200) && (CurAddr < 0x20C) && (CurBase < *LimitPtr)) {
    CurSize = CurLimit = (UINT32) 1 << LowBitSet32 (CurBase);
    CurLimit += CurBase;
    if ((CurBase == 0) || (*LimitPtr < CurLimit)) {
      CurLimit = *LimitPtr - CurBase;
      CurSize = CurLimit = (UINT32) 1 << HighBitSet32 (CurLimit);
      CurLimit += CurBase;
    }

    // prog. MTRR with current region Base
    LocalMsrRegister = (CurBase << 16) | (UINT32) MtrrType;
    AsmWriteMsr64 (CurAddr, LocalMsrRegister);

    // prog. MTRR with current region Mask
    CurAddr++;                              // other half of MSR pair
    Value32 = CurSize - (UINT32) 1;
    Value32 = ~Value32;
    LocalMsrRegister = 0xFFFF00000000ull | (Value32 << 16) | ((UINT32) 1 << 11);
    AsmWriteMsr64 (CurAddr, LocalMsrRegister);

    CurBase = CurLimit;
    CurAddr++;                              // next MSR pair
  }

  if (CurLimit < *LimitPtr) {
    // Announce failure
    *LimitPtr = CurLimit;
    IDS_ERROR_TRAP;
  }

  while ((CurAddr >= 0x200) && (CurAddr <= 0x20F)) {
    // Check if this vMTRR caches SPI ROM, skip it if yes
    if ((CurAddr % 2) == 0) {
      LocalMsrRegister = AsmReadMsr64 (CurAddr);
      if ((LocalMsrRegister & 0xFFFFFFFFFFFFFFF8ull) >= 0xFF000000) {
        CurAddr += 2;
        continue;
      }
    }

    LocalMsrRegister = 0;
    AsmWriteMsr64 (CurAddr, LocalMsrRegister);
    CurAddr++;
  }

  return AGESA_SUCCESS;
}

/*++

Routine Description:

  Zen - Zeppelin Microcode Patch loading

Arguments:
  AMD_CONFIG_PARAMS *

Returns:

--*/
VOID
CcxZenRvCacheInit (
  IN       AMD_CONFIG_PARAMS  *StdHeader
  )
{
  UINT32        Cache32bTOP;
  UINT64        LocalMsrRegister;
  AGESA_STATUS  Status;

  LocalMsrRegister = AsmReadMsr64 (0xC001001A);

  Cache32bTOP = (UINT32) (RShiftU64 (LocalMsrRegister, 16) & 0xFFFFFFFF);

  //
  //======================================================================
  // Set default values for CPU registers
  //======================================================================
  //
  AsmMsrOr64 (0xC0010010, BIT19);

  LocalMsrRegister = 0x1E1E1E1E1E1E1E1Eull;
  AsmWriteMsr64 (0x250, LocalMsrRegister);      // 0 - 512K = WB Mem
  LocalMsrRegister = 0x1E1E1E1E1E1E1E1Eull;
  AsmWriteMsr64 (0x258, LocalMsrRegister);      // 512K - 640K = WB Mem

  //
  //======================================================================
  // Set variable MTRR values
  //======================================================================
  //
  Status = SetVariableMTRRs (0, &Cache32bTOP, 0x200, 6, StdHeader);

  AsmMsrOr64 (0xC0010010, (BIT18 | BIT20));

  LocalMsrRegister = AsmReadMsr64 (0xC0010010);
  if ((LocalMsrRegister & BIT21) != 0) {
    LocalMsrRegister |= BIT22;
  }

  LocalMsrRegister &= ~BIT19;
  AsmWriteMsr64 (0xC0010010, LocalMsrRegister);
}

