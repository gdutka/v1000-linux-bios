/* $NoKeywords:$ */
/**
 * @file
 *
 * Service procedure to access various CPU registers.
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
#ifndef _CPUACCLIB_H_
#define _CPUACCLIB_H_

VOID
GnbLibCpuPciIndirectWrite (
  IN      UINT32        Address,
  IN      UINT32        IndirectAddress,
  IN      UINT32        *Value,
  IN      VOID          *Config
  );

VOID
GnbLibCpuPciIndirectRead (
  IN       UINT32        Address,
  IN       UINT32        IndirectAddress,
     OUT   UINT32        *Value,
  IN       VOID          *Config
  );


#endif

