/* $NoKeywords:$ */
/**
 * @file
 *
 * mmerrhdl.h
 *
 * main error handling
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

#ifndef _MMERRHDL_H_
#define _MMERRHDL_H_

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
#define EXCLUDE_ALL_DCT 0xFF
#define EXCLUDE_ALL_CHIPSEL  0xFF

/// default times of training
#define DEFAULT_TRAINING_TIMES 1

/// number of us to wait in parallel training
#define PARALLEL_TRAINING_TIMEOUT 60000000

/// number of us to wait in PCI space access
#define PCI_ACCESS_TIMEOUT 10000000
/// number of us to wait in special PCI space access which takes much longer than others
#define SPECIAL_PCI_ACCESS_TIMEOUT 20000000

/// Beginning of retrain handling, must be ended with the ending of the handling
#define ERROR_HANDLE_RETRAIN_BEGIN(counter, limit) while (counter < limit)

/// Ending of retrain handling
#define ERROR_HANDLE_RETRAIN_END(condition, counter) \
if (condition) { \
  counter ++; \
} else { \
  break; \
}

/*----------------------------------------------------------------------------
 *                         TYPEDEFS, STRUCTURES, ENUMS
 *
 *----------------------------------------------------------------------------
 */
/*----------------------------------------------------------------------------
 *                           FUNCTIONS PROTOTYPE
 *
 *----------------------------------------------------------------------------
 */

BOOLEAN
MemErrHandle (
  IN       DIE_STRUCT *MCTPtr,
  IN       UINT8  DCT,
  IN       UINT16  ChipSelMask,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

#endif  /* _MMERRHDL_H_ */

