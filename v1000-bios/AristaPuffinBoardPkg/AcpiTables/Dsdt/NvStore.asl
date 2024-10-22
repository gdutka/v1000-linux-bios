/** @file

;*******************************************************************************
;* Copyright (c) 2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;*******************************************************************************
*/

#ifndef NV_STORE_ASI
#define NV_STORE_ASI

OperationRegion(NVST, SystemMemory, 0x55AA55AA, 0x55AA55AA)
Field(NVST,AnyAcc,Lock,Preserve)
{
  SMIF,	8,	// SMI Function Call (ASL to SMI via I/O Trap)
  PRM0,	8,	// SMIF - Parameter 0
  PRM1,	8,	// SMIF - Parameter 1
  BRTL,	8,	// Brightness Level
  TLST,	8,	// Display Toggle List Selection
  IGDS,	8,	// IGD State (Primary Display = 1)
  LCDA,	16,	// LCD Address (Default = 0x400)
  CSTE,	16,	// Current Display State
  NSTE,	16,	// Next Display State
  CADL,	16,	// Current Attached Device List
  PADL,	16,	// Previous Attached Device List
  LIDS,	8,	// Lid State (Lid Open = 1)
  PWRS,	8,	// Power State (AC Mode = 1)
  BVAL,	32,	// 32-bit Battery Value returned in _BST
  ADDL, 16, // Available Display Device List
  BCMD, 8,  // SMI command
  SBFN, 8,  // Sub-function for SMI command
  DID,  32, // Function ID for ATIF
  INFO, 2048, // Information buffer for ATIF
  TOML, 8,  // Low byte of Top of memory register
  TOMH, 8,  // High byte of Top of memory register
  CEBP, 8,  // EnabledCpuBitMap; A bit map to indicate which CPU has support PowerNow
  C0LS, 8,  // Cpu0LowestState; The Lowest P-state number of CPU0
  C1LS, 8,  // Cpu1LowestState; The Lowest P-state number of CPU1
  C0HS, 8,  // Cpu0HighestState; The Highest P-state number of CPU0
  C1HS, 8,  // Cpu1HighestState; The Highest P-state number of CPU1
  ROMS, 32,
  MUXF, 8,
  PDDN, 8,  // Primary display device number
  CNSB, 8,  // Connect Standby support
  RDHW, 8,  // Reduced Hardware Support
}

#endif
