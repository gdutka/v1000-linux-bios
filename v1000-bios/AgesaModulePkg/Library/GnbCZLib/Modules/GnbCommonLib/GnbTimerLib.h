/* $NoKeywords:$ */
/**
 * @file
 *
 * Various Timer Services
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

#ifndef _GNBTIMERLIB_H_
#define _GNBTIMERLIB_H_

VOID
GnbLibStallS3Save (
  IN      UINT32              Microsecond,
  IN      AMD_CONFIG_PARAMS   *StdHeader
 );

VOID
GnbLibStall (
  IN      UINT32              Microsecond,
  IN      AMD_CONFIG_PARAMS   *StdHeader
 );

VOID
GnbLibStallS3Script (
  IN      AMD_CONFIG_PARAMS    *StdHeader,
  IN      UINT16               ContextLength,
  IN      VOID*                Context
  );

UINT32
GnbFmTimeStamp (
  IN       AMD_CONFIG_PARAMS          *StdHeader
  );

#endif

