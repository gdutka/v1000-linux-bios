/* $NoKeywords:$ */
/**
 * @file
 *
 * mndct.c
 *
 * Common Northbridge DCT support
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/NB)
 * @e \$Revision: 311790 $ @e \$Date: 2015-01-27 13:03:49 +0800 (Tue, 27 Jan 2015) $
 *
 **/
/*****************************************************************************
*
* Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
*
*******************************************************************************
*
*/


/*
 *----------------------------------------------------------------------------
 *                                MODULES USED
 *
 *----------------------------------------------------------------------------
 */



#include "AGESA.h"
#include "Library/AmdBaseLib.h"
#include "Library/IdsLib.h"
#include "Ids.h"
#include "mport.h"
#include "mm.h"
#include "mn.h"
#include "mt.h"
#include "mu.h"
#include "mftds.h"
#include "merrhdl.h"
#include "OptionMemory.h"
#include "PlatformMemoryConfiguration.h"
#include "cpuRegisters.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE LIBRARY_MEM_NULL_NBLIBNULL_MNDCT_FILECODE
/*----------------------------------------------------------------------------
 *                          DEFINITIONS AND MACROS
 *
 *----------------------------------------------------------------------------
 */
#define UNUSED_CLK 4

/*----------------------------------------------------------------------------
 *                           TYPEDEFS AND STRUCTURES
 *
 *----------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------
 *                        PROTOTYPES OF LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */

VOID
STATIC
MemNAfterStitchMemNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

/*----------------------------------------------------------------------------
 *                            EXPORTED FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *      This function combines all the memory into a contiguous map.
 *      Requires that Mask values for each bank be programmed first and that
 *      the chip-select population indicator is correctly set.
 *
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 *     @return          TRUE - An Error value lower than AGESA_FATAL may have occurred
 *     @return          FALSE - An Error value greater than or equal to AGESA_FATAL may have occurred
 */

BOOLEAN
MemNStitchMemoryNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  return TRUE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function gets platform specific config/timing values from the interface layer and
 *   programs them into DCT.
 *
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 *     @return          TRUE - An Error value lower than AGESA_FATAL may have occurred
 *     @return          FALSE - An Error value greater than or equal to AGESA_FATAL may have occurred
 */

BOOLEAN
MemNPlatformSpecUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  return TRUE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function disables unused MemClk to save power
 *
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 *     @return          TRUE - An Error value lower than AGESA_FATAL may have occurred
 *     @return          FALSE - An Error value greater than or equal to AGESA_FATAL may have occurred
 */

VOID
MemNDisableMemClkUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function disables the DCT and mem clock for UNB
 *
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */

VOID
MemNDisableDCTUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{

}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *  This function initializes the DRAM devices on all DCTs at the same time
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */

VOID
MemNStartupDCTUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{

}

/* -----------------------------------------------------------------------------*/
/**
 *
 *      This function ramp up frequency to target frequency
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 *     @return          TRUE -  No fatal error occurs.
 *     @return          FALSE - Fatal error occurs.
 */

BOOLEAN
MemNRampUpFrequencyUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  return TRUE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *      This function uses calculated values from DCT.Timings structure to
 *      program its registers for UNB
 *
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */

VOID
MemNProgramCycTimingsUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{

}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function gets platform specific settings for the current channel
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 *     @return          TRUE - All platform types defined have initialized successfully
 *     @return          FALSE - At least one of the platform types gave not been initialized successfully
 */

BOOLEAN
MemNGetPlatformCfgNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  return TRUE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function sends the ZQCL command
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */

VOID
MemNSendZQCmdNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{

}


/*----------------------------------------------------------------------------
 *                              LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function is used to create the DRAM map
 *
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 */

VOID
STATIC
MemNAfterStitchMemNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{

}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function swaps bits for OnDimmMirror support for Unb
 *
 *     Dimm Mirroring Requires that, during MRS command cycles, the following
 *     bits are swapped by software
 *
 *             A3 -> A4         A7 -> A8
 *             A4 -> A3         BA0 -> BA1
 *             A5 -> A6         BA1 -> BA0
 *             A6 -> A5
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */

VOID
MemNSwapBitsUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{

}

/* -----------------------------------------------------------------------------*/
/**
 *
 *   Programs Address/command timings, driver strengths, and tri-state fields.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */
VOID
MemNProgramPlatformSpecNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{

}

/* -----------------------------------------------------------------------------*/
/**
 *
 *   Programs Address/command timings, driver strengths, and tri-state fields.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */
VOID
MemNTristateUnusedChipSelectsNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{

}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function enables swapping interleaved region feature.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in]   Base   - Swap interleaved region base [47:27]
 *     @param[in]   Limit   - Swap interleaved region limit [47:27]
 *
 */
VOID
MemNEnableSwapIntlvRgnNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT32 Base,
  IN       UINT32 Limit
  )
{

}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function converts MemClk frequency in MHz to MemClkFreq value
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in]       Speed   - MemClk frequency in MHz
 *
 *     @return      MemClkFreq value
 */
UINT8
MemNGetMemClkFreqIdUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT16 Speed
  )
{
  return 0;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function converts MemClkFreq Id value to MemClk frequency in MHz
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in]       FreqId   - FreqId from Register
 *
 *     @return          MemClk frequency in MHz
 */
UINT16
MemNGetMemClkFreqUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 FreqId
  )
{
  return 0;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *  This function change MemClk frequency to the value that is specified by DCTPtr->Timings.Speed
 *  for UNB.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */

VOID
MemNChangeFrequencyUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{

}

/* -----------------------------------------------------------------------------*/
/**
 *
 *   This function changes NB frequency foras below:
 *     NBP0-DDR800 -> NBP0-DDR1066 -> ... -> NBP0-DDRTarget -> NBP1-DDRTarget -> NBP2-DDRTarget -> NBP3-DDRTarget -> NBP0-DDRTarget
 *
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */

BOOLEAN
MemNChangeNbFrequencyUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  return TRUE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function gets "Dram Term" value from data structure for Unb
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in]       ChipSel  - Targeted chipsel
 *
 *     @return       Dram Term value
 */
UINT8
MemNGetDramTermTblDrvNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 ChipSel
  )
{
  return 0;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function gets "Dynamic Dram Term" value from data structure
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in]       ChipSel  - Targeted chipsel
 *
 *     @return       Dynamic Dram Term value
 */
UINT8
MemNGetDynDramTermTblDrvNb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 ChipSel
  )
{
  return 0;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function returns MR0[WR] value
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 *     @return       MR0[WR] value
 */
UINT32
MemNGetMR0WRTblDrvNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  return 0;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *   This function returns MR2[CWL] value for UNB
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 *     @return       MR0[CWL] value
 */
UINT32
MemNGetMR2CWLUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  return 0;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *   This function sets Txp and Txpdll
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 *     @return       none
 */
VOID
MemNSetTxpNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *   This function is a wrapper to handle or switch NB Pstate for UNB
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in]  *NBPstate - NB Pstate
 *
 *     @return          TRUE - Succeed
 *     @return          FALSE - Fail
 */

VOID
MemNWaitForNbPstateChangeCompletion (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 NBPstate
  )
{

}

/* -----------------------------------------------------------------------------*/
/**
 *
 *   This function waits for NB P-state transition to be completed
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in]  *NBPstate - NB Pstate
 *
 *     @return          TRUE - Succeed
 *     @return          FALSE - Fail
 */

BOOLEAN
MemNChangeNbFrequencyWrapUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT32 NBPstate
  )
{
  return TRUE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function sends an MRS command for Unb
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */

VOID
MemNSendMrsCmdUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{

}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function returns MR0[CL] value with table driven support
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 *     @return       MR0[CL] value
 */
UINT32
MemNGetMR0CLTblDrvNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  return 0;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *     This function programs dram power management timing related registers
 *
 *     @param[in,out]  *NBPtr     - Pointer to the MEM_NB_BLOCK
 *
 *     @return    none
 * ----------------------------------------------------------------------------
 */
VOID
MemNDramPowerMngTimingNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *       The function resets Rcv Fifo
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *     @param[in]       Dummy - Dummy parameter
 *
 */

VOID
MemTResetRcvFifoUnb (
  IN OUT   struct _MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 Dummy
  )
{

}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function gets the memory width
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 *     @return      Memory width
 */

UINT32
MemNGetMemoryWidthUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  return 0;
}

/*-----------------------------------------------------------------------------
 *
 *
 *     This function displays AMP voltage values if applicable
 *
 *     @param[in,out]  *NBPtr     - Pointer to the MEM_NB_BLOCK
 *     @param[in,out]  OptParam   - Optional parameter
 *
 *     @return    FALSE
 * ----------------------------------------------------------------------------
 */
BOOLEAN
MemNAmpVoltageDispUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *OptParam
  )
{
  return FALSE;
}


