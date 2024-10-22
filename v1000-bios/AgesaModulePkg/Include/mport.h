/* $NoKeywords:$ */
/**
 * @file
 *
 * mport.h
 *
 * API's to support different OS
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
 *===========================================================================
 *  AMD Revision History
 *  Initial Version
 *
 */
#ifndef _MPORT_H_
#define _MPORT_H_

///< 64 bit data structure
///< lo - Lower 32 bits
///< hi - Upper 32 bits
typedef struct {
  UINT32 lo; ///< Lower 32 bits
  UINT32 hi; ///< Upper 32 bits
} TWO_UINT32;

typedef union msrvalue {
  UINT64      Value64;

  TWO_UINT32  Values32;
} S_UINT64;
/*
 *   SBDFO - Segment Bus Device Function Offset
 *   31:28   Segment (4-bits)
 *   27:20   Bus     (8-bits)
 *   19:15   Device  (5-bits)
 *   14:12   Function(3-bits)
 *   11:00   Offset  (12-bits)
 */
typedef UINT32 SBDFO;

#define GET_SIZE_OF(x)   (sizeof (x) / sizeof (x[0]))

#endif /* _MPORT_H_ */

