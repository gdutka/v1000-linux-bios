/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD CCX Stall Library
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Lib
 * @e \$Revision: 312770 $   @e \$Date: 2015-02-11 13:24:42 +0800 (Wed, 11 Feb 2015) $
 *
 */
/*
 ******************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 **/

#ifndef _CCX_STALL_LIB_H_
#define _CCX_STALL_LIB_H_

/*---------------------------------------------------------------------------------------*/
/**
*  Spins until the number of microseconds specified have
*  expired regardless of CPU operational frequency.
*
*  @param[in]   Microseconds  Wait time in microseconds
*
*/
VOID
CcxWaitMicroseconds (
  IN       UINT32 Microseconds
  );

#endif // _CCX_STALL_LIB_H_

