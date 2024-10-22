/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD CCX Boot Time Calibration Library
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Lib
 * @e \$Revision$   @e \$Date$
 *
 */
/*
 ******************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 **/

#ifndef _CCX_BTC_LIB_H_
#define _CCX_BTC_LIB_H_

AGESA_STATUS
CcxBtcBefore (
  IN       AMD_CONFIG_PARAMS   *StdHeader
  );

VOID
CcxBtcAfter (
  IN       AMD_CONFIG_PARAMS   *StdHeader
  );

#endif // _CCX_BTC_LIB_H_
