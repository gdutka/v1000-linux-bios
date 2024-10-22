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
#ifndef _GNBURAV1LIB_H_
#define _GNBURAV1LIB_H_

#include <GnbUra.h>
#include <GnbUraToken.h>

/*----------------------------------------------------------------------------
 *                           FUNCTION PROTOTYPES
 *
 *----------------------------------------------------------------------------
 */

VOID
GnbUraGet (
  IN       DEV_OBJECT          *Device,
  IN       URA_TOKEN           UraToken,
  IN OUT   VOID                *Value
  );

VOID
GnbUraSet (
  IN       DEV_OBJECT          *Device,
  IN       URA_TOKEN           UraToken,
  IN       VOID                *Value
  );

VOID
GnbUraCombinedGet (
  IN       DEV_OBJECT           *Device,
  IN       URA_TOKEN            UraTokenRegister,
  IN OUT   URA_TUPLE            *UraTuple,
  IN       UINT32               CombinedCount
  );

VOID
GnbUraCombinedSet (
  IN       DEV_OBJECT           *Device,
  IN       URA_TOKEN            UraTokenRegister,
  IN OUT   URA_TUPLE            *UraTuple,
  IN       UINT32               CombinedCount
  );

#endif

