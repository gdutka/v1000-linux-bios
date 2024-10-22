/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD CCX MP Services Library
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

#ifndef _CCX_MP_SERVICES_LIB_H_
#define _CCX_MP_SERVICES_LIB_H_
#include "Library/IdsLib.h"
#include <Protocol/MpService.h>
#pragma pack (push, 1)

/// Core Selector
typedef enum {
  ALL_THREADS,                    ///< Run function on all threads
  ALL_CORE_PRIMARY,               ///< Run function on all core primary (thread 0 of every core)
  ALL_COMPLEX_PRIMARY,            ///< Run function on all complex primary (logical core 0 of every complex)
  ALL_CCD_PRIMARY,                ///< Run function on all CCD primary (logical core 0 of every CCD)
  ALL_DIE_PRIMARY,                ///< Run function on all DIE primary (logical core 0 of every DIE)
  ALL_SOCKET_PRIMARY,             ///< Run function on all socket primary (logical core 0 of every socket)
} CCX_MP_SERVICES_CORE_SELECTOR;

/// Blocking Mode
typedef enum {
  BLOCKING_MODE,                  ///< execute in blocking mode. BSP waits until this AP finishes
  NON_BLOCKING_MODE,              ///< execute in non-blocking mode. BPS won't wait for AP
} CCX_MP_SERVICES_BLOCKING_MODE;

/*---------------------------------------------------------------------------------------*/
/**
 * Run function on selected APs
 *
 *
 * @param[in]  CoreSelector       core selector
 * @param[in]  RunThisFunction    function
 * @param[in]  Argument           argument
 * @param[in]  BlockingMode       BLOCKING_MODE     - execute in blocking mode. BSP waits until this AP finishes
 *                                NON_BLOCKING_MODE - execute in non-blocking mode. BPS won't wait for AP
 *                                WARNING: LIB instance for SMM won't support NonBlockingMode
 *
 */
EFI_STATUS
CcxRunFunctionOnAps (
  IN       CCX_MP_SERVICES_CORE_SELECTOR  CoreSelector,
  IN       EFI_AP_PROCEDURE               RunThisFunction,
  IN       VOID                          *Argument,
  IN       CCX_MP_SERVICES_BLOCKING_MODE  BlockingMode
  );

#pragma pack (pop)
#endif // _CCX_MP_SERVICES_LIB_H_

