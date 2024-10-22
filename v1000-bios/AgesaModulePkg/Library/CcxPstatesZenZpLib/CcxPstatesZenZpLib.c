/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Ccx Pstates Library
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
#include <Library/BaseLib.h>
#include "AGESA.h"
#include "cpuRegisters.h"
#include "CcxRegistersZp.h"
#include "Filecode.h"
#include "Library/AmdBaseLib.h"
#include "Library/CcxPstatesLib.h"

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
#define FILECODE LIBRARY_CCXPSTATESZENZPLIB_CCXPSTATESZENZPLIB_FILECODE

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/* -----------------------------------------------------------------------------*/
/**
 *
 *  CcxGetPstateInfo
 *
 *    Get Pstate information
 *
 *    @param[in]      Socket         Zero-based socket number to check
 *    @param[in]      Pstate         Which P-state to analyze
 *    @param[in]      Frequency      The P-State's frequency in MegaHertz
 *    @param[in]      VoltageInuV    The P-State's voltage in uV
 *    @param[in]      PowerInmW      The Power in milliWatts of that P-State
 *    @param[in]      StdHeader      Standard Head Pointer
 *
 *    @retval         TRUE           Get Pstate information successful.
 *    @retval         FALSE          Pstate is disabled
 *
 */
BOOLEAN
CcxGetPstateInfo (
  IN       UINTN               Socket,
  IN       CCX_PSTATE          Pstate,
     OUT   UINTN              *Frequency,
     OUT   UINTN              *VoltageInuV,
     OUT   UINTN              *PowerInmW,
  IN       AMD_CONFIG_PARAMS  *StdHeader
  )
{
  UINT32    CpuFid;
  UINT32    CpuDfsId;
  UINT32    CpuVid;
  UINT32    IddValue;
  UINT32    IddDiv;
  UINT32    PstateNumber;
  PSTATE_MSR  PstateMsr;

  PstateNumber = Pstate < HwPstate0 ? (UINT32) Pstate : (UINT32) Pstate - HwPstate0;
  PstateMsr.Value = AsmReadMsr64 (MSR_PSTATE_0 + PstateNumber);

  if (PstateMsr.Field.PstateEn == 0) {
    return FALSE;
  }

  IDS_HDT_CONSOLE (CPU_TRACE, "  F17ZpGetPstateFrequency - P%d\n", PstateNumber);

  CpuFid   = (UINT32) PstateMsr.Field.CpuFid_7_0;
  CpuDfsId = (UINT32) PstateMsr.Field.CpuDfsId;
  CpuVid   = (UINT32) PstateMsr.Field.CpuVid;
  IddValue = (UINT32) PstateMsr.Field.IddValue;
  IddDiv   = (UINT32) PstateMsr.Field.IddDiv;

  ASSERT ((CpuDfsId <= 0x1A) || ((CpuDfsId & BIT0) == 0));

  if (CpuDfsId == 0) {
    *Frequency = 0;
  } else if ((CpuDfsId >= 8) && (CpuDfsId <= 0x3C)) {
    *Frequency = (UINTN) ((200 * CpuFid) / CpuDfsId);
  } else {
    IDS_HDT_CONSOLE (CPU_TRACE, "    Undefined CpuDfsId %d used. Force it to div 1.\n", CpuDfsId);
    ASSERT (FALSE);
    *Frequency = (UINTN) (25 * CpuFid);
  }
  IDS_HDT_CONSOLE (CPU_TRACE, "    FrequencyInMHz=%d, CpuFid=%d, CpuDfsId=%d\n", *Frequency, CpuFid, CpuDfsId);

  // Voltage
  if ((CpuVid >= 0xF8) && (CpuVid <= 0xFF)) {
    *VoltageInuV = 0;
  } else {
    *VoltageInuV = 1550000L - (6250 * CpuVid);
  }

  // PowerInmW
  *PowerInmW = (*VoltageInuV) / 10 * IddValue;

  switch (IddDiv) {
  case 0:
    *PowerInmW = *PowerInmW / 100L;
    break;
  case 1:
    *PowerInmW = *PowerInmW / 1000L;
    break;
  case 2:
    *PowerInmW = *PowerInmW / 10000L;
    break;
  default:
    // IddDiv is set to an undefined value.  This is due to either a misfused CPU, or
    // an invalid P-state MSR write.
    ASSERT (FALSE);
    *PowerInmW = 0;
    break;
  }
  return TRUE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *  CcxGetPstateNumber
 *
 *    Get number of Pstate
 *
 *    @param[in]      Socket               Zero-based socket number to check
 *    @param[in]      NumberOfBoostPstate  Number of boost Pstate
 *    @param[in]      NumberOfHwPstate     Number of hardware Pstate
 *    @param[in]      StdHeader            Standard Head Pointer
 *
 */
VOID
CcxGetPstateNumber (
  IN       UINTN               Socket,
     OUT   UINTN              *NumberOfBoostPstate,
     OUT   UINTN              *NumberOfHwPstate,
  IN       AMD_CONFIG_PARAMS  *StdHeader
  )
{
  PSTATE_CURLIM_MSR  CurLim;

  *NumberOfBoostPstate = 0;

  CurLim.Value = AsmReadMsr64 (MSR_PSTATE_CURRENT_LIMIT);
  *NumberOfHwPstate = (UINTN) CurLim.Field.PstateMaxVal;
}

//[-start-240426-IB18160212-modify-]//
/* -----------------------------------------------------------------------------*/
/**
 *
 *  CcxSwitchPstate
 *
 *    Switch to a certain Pstate
 *
 *    @param[in]      SwitchPstate            Target Pstate & wait for completion
 *
 */
VOID
CcxSwitchPstate (
  IN       VOID              *Buffer
  )
{
  PSTATE_MSR           PstateMsr;
  HW_PSTATE_STATUS_MSR HwPstateSts;
  CCX_SWITCH_PSTATE    *SwitchPstate;

  SwitchPstate = (CCX_SWITCH_PSTATE*)Buffer;

  PstateMsr.Value = AsmReadMsr64 (MSR_PSTATE_0 + (UINT32) (SwitchPstate->Pstate));
  if (PstateMsr.Field.PstateEn == 0) {
    ASSERT (FALSE);
    return;
  }

  AsmWriteMsr64 (MSR_PSTATE_CTL, SwitchPstate->Pstate);

  if (SwitchPstate->WaitForCompletion) {
    if (SwitchPstate->Pstate != 0) {
      // Switch to non-P0, check FID & DID
      do {
        HwPstateSts.Value = AsmReadMsr64 (MSR_HW_PSTATE_STATUS);
      } while ((HwPstateSts.Field.CurCpuFid != PstateMsr.Field.CpuFid_7_0) ||
               (HwPstateSts.Field.CurCpuDfsId != PstateMsr.Field.CpuDfsId));
    } else {
      // Switch to P0, check current frequency >= P0
      do {
        HwPstateSts.Value = AsmReadMsr64 (MSR_HW_PSTATE_STATUS);
      } while ((DivU64x32 (MultU64x64 (200, (UINT32) HwPstateSts.Field.CurCpuFid), (UINT32) HwPstateSts.Field.CurCpuDfsId)) <
               (DivU64x32 (MultU64x64 (200, (UINT32) PstateMsr.Field.CpuFid_7_0), (UINT32) PstateMsr.Field.CpuDfsId)));
    }
  }
}
//[-end-240426-IB18160212-modify-]//

/* -----------------------------------------------------------------------------*/
/**
 *
 *  CcxGetCurrentPstate
 *
 *    Get current Pstate
 *
 *    @retval  UINT8    current Pstate
 *
 */
UINT8
CcxGetCurrentPstate (
  )
{
  PSTATE_STS_MSR PstateSts;

  PstateSts.Value = AsmReadMsr64 (MSR_PSTATE_STS);
  return (UINT8) PstateSts.Field.CurPstate;
}

/*---------------------------------------------------------------------------------------
 *                           L O C A L    F U N C T I O N S
 *---------------------------------------------------------------------------------------
 */


