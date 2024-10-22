/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD CCX Stall Library
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Lib
 * @e \$Revision: 309090 $   @e \$Date: 2014-12-10 02:28:05 +0800 (Wed, 10 Dec 2014) $
 *
 */
/*
 ******************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */


/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */

#include "Uefi.h"
#include <Library/BaseLib.h>
#include "AGESA.h"
#include "Filecode.h"
#include <Library/IdsLib.h>
#include <Library/CcxStallLib.h>
#include "CcxRegistersZp.h"

#define FILECODE LIBRARY_CCXBISTLIB_CCXSTALLLIB_FILECODE

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */

 /*----------------------------------------------------------------------------------------
  *                          E X P O R T E D    F U N C T I O N S
  *----------------------------------------------------------------------------------------
 */

/*---------------------------------------------------------------------------------------*/
/**
 *  Spins until the number of microseconds specified have
 *  expired regardless of CPU operational frequency.
 *
 *  @param[in]   Microseconds  Wait time in microseconds
 *
 */
VOID
CcxWaitMicroseconds (
  IN       UINT32 Microseconds
  )
{
  UINT32      CpuFid;
  UINT32      CpuDfsId;
  UINT32      TscRateInMhz;
  UINT64      NumberOfTicks;
  UINT64      InitialTsc;
  UINT64      CurrentTsc;
  PSTATE_MSR  PstateMsr;

  TscRateInMhz = 0;
  if (FeaturePcdGet (PcdAmdCcxStallEnable)) {
    InitialTsc = AsmReadMsr64 (TSC);
    // TSC increments at the P0 frequency
    PstateMsr.Value = AsmReadMsr64 (MSR_PSTATE_0);
    CpuFid   = (UINT32) PstateMsr.Field.CpuFid_7_0;
    CpuDfsId = (UINT32) PstateMsr.Field.CpuDfsId;
    if (CpuDfsId == 0) {
      TscRateInMhz = 0;
      ASSERT (FALSE);
    } else if ((CpuDfsId >= 8) && (CpuDfsId <= 0x3C)) {
      TscRateInMhz = (UINTN) ((200 * CpuFid) / CpuDfsId);
    }
    NumberOfTicks = Microseconds * TscRateInMhz;
    do {
      CurrentTsc = AsmReadMsr64 (TSC);
    } while ((CurrentTsc - InitialTsc) < NumberOfTicks);
  }
}

