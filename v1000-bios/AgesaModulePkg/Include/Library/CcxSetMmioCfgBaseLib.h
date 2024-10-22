/* $NoKeywords:$ */
/**
 * @file
 *
 *
 * AMD Ccx set MMIO configuration space base address LIB
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

#ifndef _CCX_SET_MMIO_CFG_BASE_LIB_H_
#define _CCX_SET_MMIO_CFG_BASE_LIB_H_
#pragma pack (push, 1)

/* -----------------------------------------------------------------------------*/
/**
 *
 *  CcxSetMmioCfgBaseLib
 *
 *  Description:
 *    This routine sets MSR_MMIO_Cfg_Base register
 *
 */
VOID
CcxSetMmioCfgBaseLib (
  );

#pragma pack (pop)
#endif // _CCX_SET_MMIO_CFG_BASE_LIB_H_
