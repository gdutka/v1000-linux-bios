/* $NoKeywords:$ */
/**
 * @file
 *
 * NBIO Config Codec verb table.
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 313706 $   @e \$Date: 2015-02-26 11:00:43 +0800 (Thu, 26 Feb 2015) $
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

#ifndef _NBIOAZALIALIB_H_
#define _NBIOAZALIALIB_H_

AGESA_STATUS
NbioConfigureVerbTable (
  IN      UINT32            HdaBaseAddress,
  IN      VOID              *VerbTable
  );

#endif

