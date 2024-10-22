/** @file

  Code for MTRR synchronization.

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

#include "Cpu.h"

MTRR_SETTINGS   *mMtrrTable;

/**
  Initialize memory region for MTRR data.
  
  This function allocates ACPI NVS memory for MTRR data.
  Each time MTRRs are written, this memory region will be updated accordingly.

**/
VOID
InitializeMtrrData (
  VOID
  )
{
  //
  // Allocate memory for fixed MTRRs, variable MTRRs and MTRR_DEF_TYPE
  //
  mMtrrTable = AllocateAcpiNvsMemoryBelow4G (sizeof (MTRR_SETTINGS));

  PcdSet64 (PcdCpuMtrrTableAddress, (UINT64) (UINTN) mMtrrTable);
}

/**
  Synchronzies up the MTRR values with BSP for calling processor.

  This function synchronzies up the MTRR values with BSP for calling processor.

  @param  Buffer         Mtrr table address.

**/
VOID
EFIAPI
LoadMtrrData (
  VOID    *Buffer
  )
{
  UINT32 RegEax;
  // Only set variable MTRR for family 15
  AsmCpuid (0x80000001, &RegEax, NULL, NULL, NULL);
  if (((RegEax >> 20) & 0xFF) == 0x6) {
    MtrrSetAllMtrrs (mMtrrTable);
  }
}

/**
  Allocate EfiACPIMemoryNVS below 4G memory address.

  This function allocates EfiACPIMemoryNVS below 4G memory address.

  @param  Size         Size of memory to allocate.
  
  @return Allocated address for output.

**/
VOID*
AllocateAcpiNvsMemoryBelow4G (
  IN   UINTN   Size
  )
{
  UINTN                 Pages;
  EFI_PHYSICAL_ADDRESS  Address;
  EFI_STATUS            Status;
  VOID*                 Buffer;

  Pages = EFI_SIZE_TO_PAGES (Size);
  Address = 0xffffffff;

  Status  = gBS->AllocatePages (
                   AllocateMaxAddress,
                   EfiACPIMemoryNVS,
                   Pages,
                   &Address
                   );
  ASSERT_EFI_ERROR (Status);

  Buffer = (VOID *) (UINTN) Address;
  ZeroMem (Buffer, Size);

  return Buffer;
}
