/* $NoKeywords:$ */
/**
 * @file
 *
 * CCX base Services library prototype definition
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Library
 * @e \$Revision: 313706 $   @e \$Date: 2015-02-25 21:00:43 -0600 (Wed, 25 Feb 2015) $
 */
/*****************************************************************************
 *
 * Copyright (C) 2016-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 **/
#ifndef _CCX_PSTATES_LIB_H_
#define _CCX_PSTATES_LIB_H_
#pragma pack (push, 1)

/*---------------------------------------------------------------------------------------
 *               T Y P E D E F S,   S T R U C T U R E S,    E N U M S
 *---------------------------------------------------------------------------------------
 */
/// Pstate enum
typedef enum {
  SwPstate0 = 0,       ///< Software P0
  SwPstate1,           ///< Software P1
  SwPstate2,           ///< Software P2
  SwPstate3,           ///< Software P3
  SwPstate4,           ///< Software P4
  SwPstate5,           ///< Software P5
  SwPstate6,           ///< Software P6
  SwPstate7,           ///< Software P7
  HwPstate0 = 0x80,    ///< Hardware P0
  HwPstate1,           ///< Hardware P1
  HwPstate2,           ///< Hardware P2
  HwPstate3,           ///< Hardware P3
  HwPstate4,           ///< Hardware P4
  HwPstate5,           ///< Hardware P5
  HwPstate6,           ///< Hardware P6
  HwPstate7            ///< Hardware P7
} CCX_PSTATE;

/// Switch Core Pstate
typedef struct {
  UINTN    Pstate;             ///< Target Pstate
  BOOLEAN  WaitForCompletion;  ///< Wait for Pstate transition to complete
} CCX_SWITCH_PSTATE;

#define SW_PSTATE_MAX SwPstate0
#define HW_PSTATE_MAX HwPstate0
#define HW_PSTATE_MIN HwPstate7

/*---------------------------------------------------------------------------------------
 *                        F U N C T I O N    P R O T O T Y P E
 *---------------------------------------------------------------------------------------
 */
BOOLEAN
CcxGetPstateInfo (
  IN       UINTN               Socket,
  IN       CCX_PSTATE          Pstate,
     OUT   UINTN              *Frequency,
     OUT   UINTN              *VoltageInuV,
     OUT   UINTN              *PowerInmW,
  IN       AMD_CONFIG_PARAMS  *StdHeader
  );

VOID
CcxGetPstateNumber (
  IN       UINTN               Socket,
     OUT   UINTN              *NumberOfBoostPstate,
     OUT   UINTN              *NumberOfHwPstate,
  IN       AMD_CONFIG_PARAMS  *StdHeader
  );

VOID
CcxSwitchPstate (
  IN       VOID              *Buffer
  );

UINT8
CcxGetCurrentPstate (
  );

#pragma pack (pop)
#endif // _CCX_PSTATES_LIB_H_
