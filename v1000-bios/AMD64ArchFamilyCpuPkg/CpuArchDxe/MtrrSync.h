/** @file

  Header file for MTRR synchronization.

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _EFI_MTRR_SYNC_H_
#define _EFI_MTRR_SYNC_H_

#include "Cpu.h"

extern MTRR_SETTINGS   *mMtrrTable;

/**
  Initialize memory region for MTRR data.
  
  This function allocates ACPI NVS memory for MTRR data, and fills the region
  with current MTRR data. Each time MTRRs are written, this memory region
  will be updated accordingly.

**/
VOID
InitializeMtrrData (
  VOID
  );

/**
  Synchronzies up the MTRR values with BSP for calling processor.

  This function synchronzies up the MTRR values with BSP for calling processor.

  @param  Buffer         Mtrr table address.

**/
VOID
EFIAPI
LoadMtrrData (
  VOID    *Buffer
  );

/**
  Allocate EfiACPIMemoryNVS below 4G memory address.

  This function allocates EfiACPIMemoryNVS below 4G memory address.

  @param  Size         Size of memory to allocate.
  
  @return Allocated address for output.

**/
VOID*
AllocateAcpiNvsMemoryBelow4G (
  IN   UINTN   Size
  );
#endif
