/* $NoKeywords:$ */
/**
 * @file
 *
 * Service procedure to access heap.
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

#ifndef _GNBHEAPLIB_H_
#define _GNBHEAPLIB_H_

VOID *
GnbAllocateHeapBuffer (
  IN      UINT32              Handle,
  IN      UINTN               Length,
  IN      AMD_CONFIG_PARAMS   *StdHeader
 );

VOID *
GnbLocateHeapBuffer (
  IN      UINT32              Handle,
  IN      AMD_CONFIG_PARAMS   *StdHeader
 );

VOID *
GnbAllocateHeapBufferAndClear (
  IN      UINT32              Handle,
  IN      UINTN               Length,
  IN      AMD_CONFIG_PARAMS   *StdHeader
  );

#endif

