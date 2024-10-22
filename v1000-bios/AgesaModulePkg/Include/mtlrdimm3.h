/**
 * @file
 *
 * mtlrdimm3.h
 *
 * Definitions and declarations for DDR3 LRDIMM support
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem)
 * @e \$Revision: 309090 $ @e \$Date: 2014-12-10 02:28:05 +0800 (Wed, 10 Dec 2014) $
 *
 **/
/*****************************************************************************
  *
  * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
  *
  *******************************************************************************
  *
 */

#ifndef _MTLRDIMM3_H_
#define _MTLRDIMM3_H_

/*----------------------------------------------------------------------------
 *   Mixed (DEFINITIONS AND MACROS / TYPEDEFS, STRUCTURES, ENUMS)
 *
 *----------------------------------------------------------------------------
 */

/*-----------------------------------------------------------------------------
 *                         DEFINITIONS AND MACROS
 *
 *-----------------------------------------------------------------------------
 */
#define RCW_FN_SELECT 7

#define F0 0
#define F1 1
#define F2 2
#define F3 3
#define F4 4
#define F5 5
#define F6 6
#define F7 7
#define F8 8
#define F9 9
#define F10 10
#define F11 11
#define F12 12
#define F13 13
#define F14 14
#define F15 15

#define RC0 0
#define RC1 1
#define RC2 2
#define RC3 3
#define RC4 4
#define RC5 5
#define RC6 6
#define RC7 7
#define RC8 8
#define RC9 9
#define RC10 10
#define RC11 11
#define RC12 12
#define RC13 13
#define RC14 14
#define RC15 15

#define SPD_NONE             0
#define SPD_67               67
#define SPD_68               68
#define SPD_69               69
#define SPD_70               70
#define SPD_71               71

#define SPD_MDQ_800_1066     72
#define SPD_QXODT_800_1066   73
#define SPD_MR1_MR2_800_1066 77
#define SPD_PERSONALITY_BYTE 102
#define SPD_FREQ_DIFF_OFFSET 6

#define SPECIAL_CASE         0xFF
#define WAIT_6US             0xF6

/*----------------------------------------------------------------------------
 *                         TYPEDEFS, STRUCTURES, ENUMS
 *
 *----------------------------------------------------------------------------
 */

/// LRDIMM SPECIALIZED HOOK ENTRY POINTS
typedef enum {
  AFTER_TSTAB,          ///< Time point after tStab
  AFTER_RCW,            ///< Time point after LrDimm Rcw commands are sent
  BEFORE_BUFFERTRN,     ///< Time point just before Buffer training
  AFTER_BUFFERTRN,      ///< Time point just after Buffer training
  BEFORE_HOST_WL,       ///< Time point before host WL
  AFTER_HOST_WL         ///< Time point after host WL
} LRDIMM_HOOK_ENTRYPOINT;

/*----------------------------------------------------------------------------
 *                           FUNCTIONS PROTOTYPE
 *
 *----------------------------------------------------------------------------
 */

#endif  /* _MTLRDIMM3_H_ */

