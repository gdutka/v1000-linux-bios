/* $NoKeywords:$ */
/**
 * @file
 *
 * mttEdgeDetect.h
 *
 * Technology Common Training Header file
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

#ifndef _MTTEDGEDETECT_H_
#define _MTTEDGEDETECT_H_

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


//#define SCAN_LEFT       0            ///<  Scan Down
//#define SCAN_RIGHT      1            ///<  Scan Up
#define LEFT_EDGE       0            ///<  searching for the left edge
#define RIGHT_EDGE      1            ///<  searching for the right edge

#define SweepStages     4
#define TRN_DELAY_MAX   31           ///<  Max Virtual delay value for DQS Position Training

/*----------------------------------------------------------------------------
 *                         TYPEDEFS, STRUCTURES, ENUMS
 *
 *----------------------------------------------------------------------------
 */

/**
 *  Sweep Table Structure. ROM based table defining parameters for DQS position
 *  training delay sweep.
*/
typedef struct {
  INT8    BeginDelay;          ///<  Starting Delay Value
  INT8    EndDelay;            ///<  Ending Delay Value
  BOOLEAN ScanDir;             ///<  Scan Direction.  0 = down, 1 = up
  INT8    Step;                ///<  Amount to increment delay value
  UINT16  EndResult;           ///<  Result value to stop sweeping (to compare with failure mask)
  BOOLEAN MinMax;              ///<  Flag indicating lower (left edge) or higher(right edge)
} DQS_POS_SWEEP_TABLE;

/**
 * Sweep Information Struct - Used to track data through the DQS Delay Sweep
 *
*/
typedef struct _SWEEP_INFO {
  BOOLEAN Error;                               ///< Indicates an Error has been found
  UINT32  TestAddrRJ16[MAX_CS_PER_CHANNEL];    ///< System address of chipselects RJ 16 bits (Addr[47:16])
  BOOLEAN CsAddrValid[MAX_CS_PER_CHANNEL];     ///< Indicates which chipselects to test
  INT8    BeginDelay;                          ///< Beginning Delay value (Virtual)
  INT8    EndDelay;                            ///< Ending Delay value (Virtual)
  INT8    Step;                                ///< Amount to Inc or Dec Virtual Delay value
  BOOLEAN Edge;                                ///< Left or right edge (0 = LEFT, 1= RIGHT)
  UINT16  EndResult;                           ///< Result value that will stop a Dqs Sweep
  UINT16  InsertionDelayMsk;                   ///< Mask of Byte Lanes that should use ins. dly. comparison
  UINT16  LaneMsk;                             ///< Mask indicating byte lanes to update
  UINT16  ResultFound;                         ///< Mask indicating byte lanes where desired result was found on a sweep
  INT8    *TrnDelays;                          ///< Delay Values for each byte (Virtual).  Points into the delay values
} SWEEP_INFO;                                  ///< stored in the CH_DEF_STRUCT.

/*----------------------------------------------------------------------------
 *                           FUNCTIONS PROTOTYPE
 *
 *----------------------------------------------------------------------------
 */



#endif  /* _MTTEDGEDETECT_H_ */



