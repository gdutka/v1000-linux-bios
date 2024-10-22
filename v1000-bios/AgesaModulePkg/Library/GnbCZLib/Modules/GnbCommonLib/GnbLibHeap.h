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

#ifndef _GNBHEAPLIB_H_
#define _GNBHEAPLIB_H_

extern VOID *
GnbAllocateHeapBuffer (
  IN      UINT32              Handle,
  IN      UINTN               Length,
  IN      AMD_CONFIG_PARAMS   *StdHeader
 );

extern VOID *
GnbLocateHeapBuffer (
  IN      UINT32              Handle,
  IN      AMD_CONFIG_PARAMS   *StdHeader
 );

extern VOID *
GnbAllocateHeapBufferAndClear (
  IN      UINT32              Handle,
  IN      UINTN               Length,
  IN      AMD_CONFIG_PARAMS   *StdHeader
  );

#endif

