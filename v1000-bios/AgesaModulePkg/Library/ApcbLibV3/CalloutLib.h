/* $NoKeywords:$ */
/**
 * @file
 *
 * APCB.h
 *
 * AGESA PSP Customization Block
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem)
 * @e \$Revision: 192403 $ @e \$Date: 2012-12-12 15:46:25 -0600 (Wed, 12 Dec 2012) $
 *
 **/
/*****************************************************************************
*
* Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
*
*******************************************************************************
*
*/

#ifndef _CALLOUT_LIB_H_
#define _CALLOUT_LIB_H_

#include "Porting.h"
#include "CoreApcbInterface.h"
#include <Library/IdsLib.h>

#define customPrint(...) IDS_HDT_CONSOLE_PSP_TRACE(__VA_ARGS__)

VOID
customGetTypeCoreDb (
  IN        CORE_APCB_TYPE_ENTRY      **apcbTypeOrigin
  );

VOID
customGetTokenCoreDb (
  IN        CORE_APCB_TOKEN_ENTRY     **apcbTokenOrigin
  );

VOID
customSetTypeCoreDb (
  IN        CORE_APCB_TYPE_ENTRY      *apcbTypeOrigin
  );

VOID
customSetTokenCoreDb (
  IN        CORE_APCB_TOKEN_ENTRY     *apcbTokenOrigin
  );

VOID *
customAlloc (
    IN          UINT32      sizeBuf
    );

VOID
customFree (
    IN          VOID        *buffer
    );


#endif
