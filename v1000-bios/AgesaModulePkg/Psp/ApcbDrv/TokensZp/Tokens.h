/* $NoKeywords:$ */
/**
 * @file
 *
 * APCB DXE Driver Header
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  APCB
 * @e \$Revision$   @e \$Date$
 *
 */
/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef _AMD_APCBDRV_TOKENS_TOKENS_H_
#define _AMD_APCBDRV_TOKENS_TOKENS_H_

UINT16
mTranslateConfigTokenId (
  IN       UINT16             CommonId
  );

UINT16
mTranslateCbsExtTokenId (
  IN       UINT16             CommonId
  );

#endif //_AMD_APCBDRV_TOKENS_TOKENS_H_

