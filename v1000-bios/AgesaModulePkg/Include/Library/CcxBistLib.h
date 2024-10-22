/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD CCX BIST Library
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

#ifndef _CCX_BIST_LIB_H_
#define _CCX_BIST_LIB_H_

#define MSR_BIST 0xC0010060ul


 /*---------------------------------------------------------------------------------------*/
 /**
  *
  * Reads the BIST register and if the lower 32 bits is non-zero, returns AGESA_ALERT.
  *
  * @param[in]      StdHeader              Header for library and services
  *
  * @retval         AGESA_ALERT            A BIST error has occurred
  * @retval         AGESA_SUCCESS          No BIST errors have occured
  */
AGESA_STATUS
CheckBistStatus (
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

#endif // _CCX_BIST_LIB_H_

