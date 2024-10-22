/* $NoKeywords:$ */
/**
 * @file
 *
 *  Supporting services to collect discrete GFX card info
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


#ifndef _GFXCARDINFOLIB_H_
#define _GFXCARDINFOLIB_H_

VOID
GfxGetDiscreteCardInfo (
  OUT     GFX_CARD_CARD_INFO  *GfxCardInfo,
  IN      AMD_CONFIG_PARAMS   *StdHeader
  );

#endif

