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
 * @e \$Revision: 313706 $   @e \$Date: 2015-02-25 21:00:43 -0600 (Wed, 25 Feb 2015) $
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

#include <AMD.h>

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

VOID
GnbLibPciRmw (
  IN       UINT32              Address,
  IN       ACCESS_WIDTH        Width,
  IN       UINT32              Mask,
  IN       UINT32              OrValue,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  );

#endif

