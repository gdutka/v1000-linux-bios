/* $NoKeywords:$ */
/**
 * @file
 *
 * FCH PEIM
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project   FCH PEIM
 * @e \$Revision: 309090 $   @e \$Date: 2014-12-09 10:28:05 -0800 (Tue, 09 Dec 2014) $
 *
 */
/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */

#include <Library/DebugLib.h>

#include "FchStall.h"
#define FILECODE UEFI_PEI_FCHPEI_FCHSTALL_FILECODE

// consumed PPI
#include <Ppi/CpuIo.h>


/*********************************************************************************
 * Name: FchPeiStall
 *
 * Description
 *   Blocking Stall. Stall the system for at least the specified number of
 *   microsecond
 *
 * Input
 *   PeiServices : a pointer to the PEI service table pointer
 *
 * Output
 *   none if the reset is successful
 *
 *********************************************************************************/
EFI_STATUS
FchPeiStall (
  IN  CONST     EFI_PEI_SERVICES   **PeiServices,
  IN  CONST     EFI_PEI_STALL_PPI  *This,
  IN       UINTN              Microseconds
  )
{
  EFI_PEI_CPU_IO_PPI  *CpuIo;
  UINTN               uSec;
  UINT8               Data;

  // Issue the reset
  CpuIo = (*PeiServices)->CpuIo;

  //Simplified implementation
  uSec = Microseconds / 2;
  while (uSec != 0) {
    Data = CpuIo->IoRead8 (
                    PeiServices,
                    CpuIo,
                    (UINT64) 0x80
                    );
    uSec--;
  }
  return (EFI_SUCCESS);
}
