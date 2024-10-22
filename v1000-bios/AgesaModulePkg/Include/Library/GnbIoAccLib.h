/* $NoKeywords:$ */
/**
 * @file
 *
 * Service procedure to access I/O registers.
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
#ifndef _IOACCLIB_H_
#define _IOACCLIB_H_

#include <AMD.h>

VOID
GnbLibIoWrite (
  IN      UINT16        Address,
  IN      ACCESS_WIDTH  Width,
  IN      VOID          *Value,
  IN      VOID          *StdHeader
  );

VOID
GnbLibIoRead (
  IN      UINT16        Address,
  IN      ACCESS_WIDTH  Width,
  OUT     VOID          *Value,
  IN      VOID          *StdHeader
  );


#endif

