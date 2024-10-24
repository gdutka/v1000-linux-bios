/**
 * @file
 *
 * mtlrdimm3.c
 *
 * Technology initialization and control workd support for DDR3 LRDIMMS
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Tech/DDR3)
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
#include "mm.h"
#include "mn.h"
#include "mu.h"
#include "mt.h"
#include "mt3.h"
#include "mtspd3.h"
#include "mtrci3.h"
#include "mtsdi3.h"
#include "mtlrdimm3.h"
#include "merrhdl.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)
#define FILECODE LIBRARY_MEM_NULL_TECHDDR3LIBNULL_MTLRDIMM3_FILECODE
/*----------------------------------------------------------------------------
 *                          DEFINITIONS AND MACROS
 *
 *----------------------------------------------------------------------------
 */

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
MemTSendMBCtlWord3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 Fn,
  IN       UINT8 Rcw,
  IN       UINT8 Value
  );

VOID
STATIC
MemTSendExtMBCtlWord3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT16 Addr,
  IN       UINT16 Data,
  IN       UINT8 Len
  );

UINT8
STATIC
MemTGetSpecialMBCtlWord3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 Dimm,
  IN       UINT8 Fn,
  IN       UINT8 Rc
  );

BOOLEAN
STATIC
MemTLrDimmControlRegInit3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN OUT   VOID *OptParam
  );

BOOLEAN
STATIC
MemTLrDimmFreqChgCtrlWrd3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN OUT   VOID *OptParam
  );

BOOLEAN
STATIC
MemTWLPrepareLrdimm3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN OUT   VOID *Wl
  );

BOOLEAN
STATIC
MemTSendAllMRCmdsLR3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN OUT   VOID *CsPtr
  );

VOID
STATIC
MemTEMRS1Lr3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 ChipSel,
  IN       UINT8 PhyRank
  );

VOID
STATIC
MemTEMRS2Lr3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 ChipSel
  );


BOOLEAN
STATIC
MemTLrdimmRankMultiplication (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN OUT   VOID *DimmID
  );

BOOLEAN
STATIC
MemTLrdimmBuf2DramTrain3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN OUT   VOID *OptParam
  );

BOOLEAN
STATIC
MemTLrdimmSyncTrainedDlys (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN OUT   VOID *OptParam
  );

BOOLEAN
STATIC
MemTLrdimmPresence (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN OUT   VOID *DimmID
  );

UINT32
STATIC
MemTLrDimmGetBufferID (
  IN OUT  MEM_TECH_BLOCK *TechPtr,
  IN      UINT8 Dimm
  );

VOID
STATIC
MemTLrdimmInitHook (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       LRDIMM_HOOK_ENTRYPOINT Entrypoint,
  IN       UINT8 Dimm,
  IN OUT   VOID  *OptParam
  );

/*----------------------------------------------------------------------------
 *                            EXPORTED FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */

/* -----------------------------------------------------------------------------*/
/**
 *
 *   This function initializes LRDIMM functions.
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *
 */

BOOLEAN
MemTLrdimmConstructor3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  )
{
  return TRUE;
}

/*----------------------------------------------------------------------------
 *                              LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */

/* -----------------------------------------------------------------------------*/
/**
 *
 *   This function sends a Control word command to an LRDIMM Memory Buffer
 *
 *     @param[in,out] *TechPtr - Pointer to the MEM_TECH_BLOCK
 *     @param[in]     Fn       - control word function
 *     @param[in]     Rcw      - control word number
 *     @param[in]     Value    - value to send
 *
 */

VOID
STATIC
MemTSendMBCtlWord3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 Fn,
  IN       UINT8 Rcw,
  IN       UINT8 Value
  )
{

}

/* -----------------------------------------------------------------------------*/
/**
 *
 *   This function sends a an Extended Control word command to an LRDIMM Memory Buffer
 *
 *     @param[in,out] *TechPtr - Pointer to the MEM_TECH_BLOCK
 *     @param[in]     Addr     - Extended Control Word Address
 *                                 Addr[15:12] Extended Control Workd Function Select
 *                                 Addr[11:0] Extended Control Word CSR Address
 *     @param[in]     Data     - value to send
 *     @param[in]     Len      - Length of data.  1 or 2 bytes
 *
 */

VOID
STATIC
MemTSendExtMBCtlWord3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT16 Addr,
  IN       UINT16 Data,
  IN       UINT8 Len
  )
{

}

/* -----------------------------------------------------------------------------*/
/**
 *
 *   This function gets the value of special RCW
 *
 *     @param[in,out] *TechPtr - Pointer to the MEM_TECH_BLOCK
 *     @param[in]     Dimm     - Physical LR DIMM number
 *     @param[in]     Fn       - control word function
 *     @param[in]     Rc       - control word number
 *
 *     @return      Special RCW value
 *
 */

UINT8
STATIC
MemTGetSpecialMBCtlWord3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 Dimm,
  IN       UINT8 Fn,
  IN       UINT8 Rc
  )
{
  return 0;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *   This function sends LRDIMM Control Words to all LRDIMMS
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *     @param[in,out]   OptParam   - Optional parameter
 *
 *     @return    TRUE
 */

BOOLEAN
STATIC
MemTLrDimmControlRegInit3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN OUT   VOID *OptParam
  )
{
  return TRUE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *   This function sends LRDIMM Control Words to all LRDIMMS
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *     @param[in,out]   OptParam   - Optional parameter
 *
 *     @return    FALSE  - The current channel does not have LRDIMM populated
 *                TRUE   - The current channel has LRDIMM populated
 */
BOOLEAN
STATIC
MemTLrDimmFreqChgCtrlWrd3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN OUT   VOID *OptParam
  )
{
  return FALSE;
}

/*-----------------------------------------------------------------------------
 *
 *
 *     This function prepares LRDIMMs for WL training.
 *
 *     @param[in,out]  *TechPtr     - Pointer to the MEM_TECH_BLOCK
 *     @param[in,out]  *Wl - Indicates if WL mode should be enabled
 *
 *     @return    TRUE - LRDIMMs present
 * ----------------------------------------------------------------------------
 */
BOOLEAN
STATIC
MemTWLPrepareLrdimm3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN OUT   VOID *Wl
  )
{
  return TRUE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *   This send all MR commands to all physical ranks of an LRDIMM
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *     @param[in]       *CsPtr     - Target Chip Select
 *
 *     @return    FALSE  - The current channel does not have LRDIMM populated
 *                TRUE   - The current channel has LRDIMM populated
 */
BOOLEAN
STATIC
MemTSendAllMRCmdsLR3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN OUT   VOID *CsPtr
  )
{
  return FALSE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *   This function calculates the EMRS1 value for an LRDIMM
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *     @param[in]        ChipSel   - Chip select number
 *     @param[in]        PhyRank   - Physical rank number
 */

VOID
STATIC
MemTEMRS1Lr3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 ChipSel,
  IN       UINT8 PhyRank
  )
{

}

/* -----------------------------------------------------------------------------*/
/**
 *
 *   This function calculates the EMRS2 value for an LRDIMM
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *     @param[in]        ChipSel   - Chip select number
 */

VOID
STATIC
MemTEMRS2Lr3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 ChipSel
  )
{

}

/*-----------------------------------------------------------------------------
 *
 *
 *     This function to determine the Rank Multiplication to use for an LRDIMM
 *
 *     @param[in,out]  *TechPtr     - Pointer to the MEM_TECH_BLOCK
 *     @param[in,out]  *DimmID      - Dimm ID
 *
 *     @return    TRUE - LRDIMM Support is installed and LRDIMMs are present
 * ----------------------------------------------------------------------------
 */
BOOLEAN
STATIC
MemTLrdimmRankMultiplication (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN OUT   VOID *DimmID
  )
{
  return TRUE;
}

/* -----------------------------------------------------------------------------
 *
 *   This function performs buffer to DRAM training for LRDIMMs
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *     @param[in,out]   OptParam   - Optional parameter
 *
 *     @return    TRUE
 */

BOOLEAN
STATIC
MemTLrdimmBuf2DramTrain3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN OUT   VOID *OptParam
  )
{
  return TRUE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *   This function copies trained delays of the first rank of a QR LRDIMM to the third rank
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *     @param[in,out]   OptParam   - Optional parameter
 *
 *     @return    TRUE
 */

BOOLEAN
STATIC
MemTLrdimmSyncTrainedDlys (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN OUT   VOID *OptParam
  )
{
  return TRUE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *   This function performs LRDIMM specific tasks during Dimm Presence detection
 *
 *     @param[in,out]  *TechPtr     - Pointer to the MEM_TECH_BLOCK
 *     @param[in,out]  *DimmID      - Dimm ID
 *
 *     @return    TRUE
 *
 */

BOOLEAN
STATIC
MemTLrdimmPresence (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN OUT   VOID *DimmID
  )
{
  return TRUE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *  This function returns LRDIMM Buffer ID Info from the SPD
 *
 *
 *     @param[in,out]   *TechPtr   - Pointer to the Technology Block
 *     @param[in]       Dimm      - Dimm number
 *
 *     @return          Buffer ID Information
 *
 */

UINT32
STATIC
MemTLrDimmGetBufferID (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 Dimm
  )
{
  return 0;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function implements special case Initialization hooks for LRDIMMs
 *
 *     @param[in]     TechPtr    - Tech Block Pointer
 *     @param[in]     Entrypoint - Entrypoint to indicate when this hook is called
 *     @param[in]     Dimm       - Dimm being configured when hook is called
 *     @param[in]     OptParam   - Not Used
 */

VOID
STATIC
MemTLrdimmInitHook (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       LRDIMM_HOOK_ENTRYPOINT Entrypoint,
  IN       UINT8 Dimm,
  IN OUT   VOID  *OptParam
  )
{

}
