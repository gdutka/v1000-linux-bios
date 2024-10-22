/* $NoKeywords:$ */
/**
 * @file
 *
 * Fabric Watchdog Timer library
 *
 * Contains interface to the family specific fabric watchdog timer library
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

#ifndef _FABRIC_WDT_LIB_H_
#define _FABRIC_WDT_LIB_H_
#pragma pack (push, 1)

/* -----------------------------------------------------------------------------*/
/**
 *
 *  FabricGetCcmWdtInfo
 *
 *  @param[in, out] DfCcmTimeout        DF CCM WDT time in ns
 *
 *  @retval TRUE  WDT is enabled
 *  @retval FALSE WDT is disabled
 *
 */
BOOLEAN
FabricGetCcmWdtInfo (
  IN OUT   UINT64 *DfCcmTimeout
 );

#pragma pack (pop)
#endif // _FABRIC_WDT_LIB_H_
