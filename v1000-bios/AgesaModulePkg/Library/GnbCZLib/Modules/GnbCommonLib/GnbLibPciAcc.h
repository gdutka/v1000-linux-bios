/* $NoKeywords:$ */
/**
 * @file
 *
 * Service procedure to access PCI config space registers
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 309090 $   @e \$Date: 2014-12-10 02:28:05 +0800 (Wed, 10 Dec 2014) $
 *
 */
/*
*****************************************************************************
*
* Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
*
*******************************************************************************
*
*/
#ifndef _GNBLIBPCIACC_H_
#define _GNBLIBPCIACC_H_

VOID
GnbLibPciWrite (
  IN      UINT32              Address,
  IN      ACCESS_WIDTH        Width,
  IN      VOID                *Value,
  IN      AMD_CONFIG_PARAMS  *StdHeader
  );

VOID
GnbLibPciRead (
  IN       UINT32             Address,
  IN       ACCESS_WIDTH       Width,
     OUT   VOID               *Value,
  IN       AMD_CONFIG_PARAMS  *StdHeader
  );

#endif

