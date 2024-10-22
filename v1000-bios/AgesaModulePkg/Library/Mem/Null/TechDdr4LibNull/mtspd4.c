/* $NoKeywords:$ */
/**
 * @file
 *
 * mtspd4.c
 *
 * Technology SPD supporting functions for DDR4
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Tech/DDR4)
 * @e \$Revision: 328552 $ @e \$Date: 2015-10-09 19:05:57 +0800 (Fri, 09 Oct 2015) $
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
#include "mt4.h"
#include "mu.h"
#include "mtspd4.h"
#include "mftds.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE LIBRARY_MEM_NULL_TECHDDR4LIBNULL_MTSPD4_FILECODE

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

typedef struct _DDR4_DRAM_ADDRESS_MAP {
  UINT8  DimmAddressMap;
  UINT8  BGBits;
  UINT8  BABits;
  UINT8  RowBits;
  UINT8  ColBits;
} DDR4_DRAM_ADDRESS_MAP;

typedef struct _DQMAP_NIBBLE {
  UINT8 Nibble;
  UINT8 SpdOffset;
} DQMAP_NIBBLE;

/*----------------------------------------------------------------------------
 *                        PROTOTYPES OF LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */
BOOLEAN
STATIC
MemTCRCCheck4 (
  IN OUT   UINT8 *SPDPtr
  );

UINT8
STATIC
MemTSPDGetTCL4 (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  );

UINT8
STATIC
MemTSPDGetTrcpage4 (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  );

BOOLEAN
STATIC
MemTCheckBankAddr4 (
  OUT   UINT8 *DimmAddrMap,
  IN    UINT8 BGBits,
  IN    UINT8 BABits,
  IN    UINT8 RowBits,
  IN    UINT8 ColBits
  );

INT32
MemTGetMTB (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN OUT   UINT8 *SpdBuffer
  );

INT32
MemTGetFTB (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN OUT   UINT8 *SpdBuffer
  );

/*----------------------------------------------------------------------------
 *                            EXPORTED FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */

/* -----------------------------------------------------------------------------*/
/**
 *
 *   This function determines if DIMMs are present. It checks checksum and interrogates the SPDs
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *
 *     @return  TRUE  - indicates that a FATAL error has not occurred
 *     @return  FALSE - indicates that a FATAL error has occurred
 */

BOOLEAN
MemTDIMMPresence4 (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  )
{
  return TRUE;
}


/* -----------------------------------------------------------------------------*/
/**
 *
 *   This function finds the maximum frequency that each channel is capable to run at.
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *
 *     @return  TRUE  - indicates that a FATAL error has not occurred
 *     @return  FALSE - indicates that a FATAL error has occurred
 */

BOOLEAN
MemTSPDGetTargetSpeed4 (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  )
{
  return TRUE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *   This function check the symmetry of DIMM pairs (DIMM on Channel A matching with
 *   DIMM on Channel B), the overall DIMM population, and determine the width mode:
 *   64-bit, 64-bit muxed, 128-bit.
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *
 *     @return  TRUE  - indicates that a FATAL error has not occurred
 *     @return  FALSE - indicates that a FATAL error has occurred
 */

BOOLEAN
MemTSPDCalcWidth4 (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  )
{
  return TRUE;
}


/* -----------------------------------------------------------------------------*/
/**
 *
 *       Initialize DCT Timing registers as per DIMM SPD.
 *       For primary timing (T, CL) use best case T value.
 *       For secondary timing params., use most aggressive settings
 *           of slowest DIMM.
 *
 *   Note:
 *   There are three components to determining "maximum frequency": SPD component,
 *   Bus load component, and "Preset" max frequency component.
 *   The SPD component is a function of the min cycle time specified by each DIMM,
 *   and the interaction of cycle times from all DIMMs in conjunction with CAS
 *   latency.  The SPD component only applies when user timing mode is 'Auto'.
 *
 *   The Bus load component is a limiting factor determined by electrical
 *   characteristics on the bus as a result of varying number of device loads.  The
 *   Bus load component is specific to each platform but may also be a function of
 *   other factors.  The bus load component only applies when user timing mode is
 * ' Auto'.
 *
 *   The Preset component is subdivided into three items and is the minimum of
 *   the set: Silicon revision, user limit setting when user timing mode is 'Auto' and
 *   memclock mode is 'Limit', OEM build specification of the maximum frequency.
 *   The Preset component only applies when user timing mode is 'Auto'.
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *
 *     @return  TRUE  - indicates that a FATAL error has not occurred
 *     @return  FALSE - indicates that a FATAL error has occurred
 */

BOOLEAN
MemTAutoCycTiming4 (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  )
{
  return TRUE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *   This function sets the bank addressing, program Mask values and build a chip-select population map.
 *   This routine programs PCI 0:24N:2x80 config register.
 *   This routine programs PCI 0:24N:2x60,64,68,6C config registers (CS Mask 0-3)
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *
 *     @return  TRUE  - indicates that a FATAL error has not occurred
 *     @return  FALSE - indicates that a FATAL error has occurred
 */

BOOLEAN
MemTSPDSetBanks4 (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  )
{
  return TRUE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *      This function returns the low bit that will be swapped to enable CS
 *      interleaving
 *
 *     @param[in,out] *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *     @param[in]    BankAddrMap - AddrMap Bank encoding from F2x80
 *     @param[in]        *LowBit - pointer to low bit
 *     @param[in]        *HiBit  - pointer hight bit
 *
 */

VOID
MemTGetCSIntLvAddr4 (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 BankAddrMap,
  OUT   UINT8 *LowBit,
  OUT   UINT8 *HiBit
  )
{
  return;
}

/*----------------------------------------------------------------------------
 *                              LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */

/* -----------------------------------------------------------------------------*/
/**
 *
 *       This function determines if the checksum is correct
 *
 *     @param[in]   *SPDPtr - Pointer to SPD data
 *
 *     @return    TRUE  - CRC check passes
 *     @return    FALSE - CRC check fails
 */

BOOLEAN
STATIC
MemTCRCCheck4 (
  IN OUT   UINT8 *SPDPtr
  )
{
  return TRUE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *   This function returns the CAS latency of the current frequency (DCTPtr->Timings.Speed).
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *
 *     @return  CAS Latency
 */

UINT8
STATIC
MemTSPDGetTCL4 (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  )
{
  return 0;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *   This function returns Trcpage.
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *
 *     @return  Trcpage
 */

UINT8
STATIC
MemTSPDGetTrcpage4 (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  )
{
  return 0;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function returns the encoded value of the bank address.
 *
 *     @param[out]  *DimmAddrMap - Dimm Address Map value
 *     @param[in]   BGBits  - Number of Bank Group bits
 *     @param[in]   BABits  - Number of Bank Address bits
 *     @param[in]   RowBits - Number of Row Address bits
 *     @param[in]   ColBits - Number of Column Address bits
 *
 *     @return      TRUE   - encoded value is found.
 *                  FALSE  - encoded value is not found.
 */

BOOLEAN
STATIC
MemTCheckBankAddr4 (
  OUT   UINT8 *DimmAddrMap,
  IN       UINT8 BGBits,
  IN       UINT8 BABits,
  IN       UINT8 RowBits,
  IN       UINT8 ColBits
  )
{
  return FALSE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *       This function returns a pointer to the SPD Buffer of a specific dimm on
 *    the current channel.
 *
 *     @param[in,out] *TechPtr - Pointer to the MEM_TECH_BLOCK
 *     @param[in,out] **SpdBuffer - Pointer to a pointer to a UINT8 Buffer
 *     @param[in] Dimm - Dimm number
 *
 *
 *     @return BOOLEAN - Value of DimmPresent
 *                       TRUE = Dimm is present, pointer is valid
 *                       FALSE = Dimm is not present, pointer has not been modified.
 */

BOOLEAN
MemTGetDimmSpdBuffer4 (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN OUT   UINT8 **SpdBuffer,
  IN       UINT8 Dimm
  )
{
  return TRUE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *       This function returns the MTB
 *
 *     @param[in,out] *TechPtr - Pointer to the MEM_TECH_BLOCK
 *     @param[in,out] *SpdBuffer - Pointer to a pointer to a UINT8 Buffer
 *
 *
 *     @return INT32 - MTB value in ps
 */
INT32
MemTGetMTB (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN OUT   UINT8 *SpdBuffer
  )
{
  return 0;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *       This function returns the FTB
 *
 *     @param[in,out] *TechPtr - Pointer to the MEM_TECH_BLOCK
 *     @param[in,out] *SpdBuffer - Pointer to a pointer to a UINT8 Buffer
 *
 *
 *     @return INT32 - FTB value in ps
 */
INT32
MemTGetFTB (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN OUT   UINT8 *SpdBuffer
  )
{
  return 0;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function returns DDR4 SPD Offset for a given Bus Nibble Range
 *
 *     @param[in] Nibble - Enum value indicating nibble range
 *
 *     @return UINT8 - DDR4 SPD Offset containing nibble mapping info for
 *                     the supplied range.
 */
UINT8
MemTGetDQMappingByte (
  IN DQ_MAP_NIBBLES Nibble
  )
{
  return 0;
}









