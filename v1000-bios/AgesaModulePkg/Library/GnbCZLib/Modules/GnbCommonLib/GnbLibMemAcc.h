/* $NoKeywords:$ */
/**
 * @file
 *
 * Service procedure to access MMIO registers.
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
#ifndef _MEMACCLIB_H_
#define _MEMACCLIB_H_

VOID
GnbLibMemWrite (
  IN      UINT64        Address,
  IN      ACCESS_WIDTH  Width,
  IN      VOID          *Value,
  IN      VOID          *StdHeader
  );

VOID
GnbLibMemRead (
  IN      UINT64        Address,
  IN      ACCESS_WIDTH  Width,
  OUT     VOID          *Value,
  IN      VOID          *StdHeader
  );

VOID
GnbLibMemRMW (
  IN      UINT64        Address,
  IN      ACCESS_WIDTH  Width,
  IN      UINT32        Mask,
  IN      UINT32        Value,
  IN      VOID          *Config
  );

#endif

