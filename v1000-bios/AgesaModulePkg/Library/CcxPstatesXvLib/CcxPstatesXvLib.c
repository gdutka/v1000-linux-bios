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
#include "CcxRegistersXv.h"
#include "UnbRegisters.h"
#include "Filecode.h"
#include "Library/AmdBaseLib.h"
#include "Library/CcxPstatesLib.h"
#include <Library/PciLib.h>

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
#define FILECODE LIBRARY_CCXPSTATESXVLIB_CCXPSTATESXVLIB_FILECODE

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
  UINT8     CpuFid;
  UINT8     CpuDid;
  UINT8     CpuVid;
  UINT8     IddValue;
  UINT8     IddDiv;
  UINT64    MsrData;
  UINTN     PciAddr;
  CPB_CTRL_REGISTER  CpbCtrl;

  if (Pstate < HwPstate0) {
    // Software Pstate
    PciAddr = CPB_CTRL_PCI_ADDR;
    *((UINT32 *) &CpbCtrl) = PciRead32 (PciAddr);
    (UINT8) CpbCtrl.NumBoostStates;

    MsrData = AsmReadMsr64 ((UINT32) (MSR_PSTATE_0 + Pstate + CpbCtrl.NumBoostStates));
  } else {
    // Hardware Pstate
    MsrData = AsmReadMsr64 ((UINT32) (MSR_PSTATE_0 + Pstate - HwPstate0));
  }

  if (((PSTATE_MSR *) &MsrData)->PsEnable == 0) {
    return FALSE;
  }

  CpuFid   = (UINT8) (((PSTATE_MSR *) &MsrData)->CpuFid_5_0);
  CpuDid   = (UINT8) (((PSTATE_MSR *) &MsrData)->CpuDid);
  CpuVid   = (UINT8) (((PSTATE_MSR *) &MsrData)->CpuVid);
  IddValue = (UINT8) (((PSTATE_MSR *) &MsrData)->IddValue);
  IddDiv   = (UINT8) (((PSTATE_MSR *) &MsrData)->IddDiv);

  // Frequency
  *Frequency = (100 * (CpuFid + 0x10) / (1 << CpuDid));

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
  CPB_CTRL_REGISTER    Cpb;
  CLK_PWR_TIMING_CTRL2_REGISTER Cptc2;
  UINTN                PciAddress;

  // NumberOfBoostPstate = F4x15C[NumBoostStates].
  PciAddress = CPB_CTRL_PCI_ADDR;
  *((UINT32 *) &Cpb) = PciRead32 (PciAddress);
  *NumberOfBoostPstate = Cpb.NumBoostStates;

  // NumberOfHwPstate = F3xDC[HwPstateMaxVal]
  PciAddress = CPTC2_PCI_ADDR;
  *((UINT32 *) &Cptc2) = PciRead32 (PciAddress);
  *NumberOfHwPstate = Cptc2.HwPstateMaxVal;
}

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
  IN       CCX_SWITCH_PSTATE *SwitchPstate
  )
{
  PSTATE_STS_MSR  PstateSts;

  AsmWriteMsr64 (MSR_PSTATE_CTL, SwitchPstate->Pstate);

  if (SwitchPstate->WaitForCompletion) {
    do {
      PstateSts.CurPstate = (UINT8) AsmReadMsr64 (MSR_PSTATE_STS);
    } while (PstateSts.CurPstate != SwitchPstate->Pstate);
  }
}

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

  PstateSts.CurPstate = (UINT8) AsmReadMsr64 (MSR_PSTATE_STS);
  return (UINT8) PstateSts.CurPstate;
}

/*---------------------------------------------------------------------------------------
 *                           L O C A L    F U N C T I O N S
 *---------------------------------------------------------------------------------------
 */


