/* $NoKeywords:$ */
/**
 * @file
 *
 * Implement Custom Core Pstates
 *
 * Contains code that Custom Core Pstates
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Library
 * @e \$Revision$   @e \$Date$
 *
 */
/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */

/*****************************************************************************
 *
 * This software package can be used to enable the Overclocking of certain
 * AMD processors and its use is subject to the terms and conditions of the
 * AMD Overclocking Waiver. Enabling overclocking through use of the low-level
 * routines included in this package and operating an AMD processor outside of
 * the applicable AMD product specifications will void any AMD warranty and can
 * result in damage to the processor or the system into which the processor has
 * been integrated. The user of this software assumes, and AMD disclaims, all
 * risk, liability, costs and damages relating to or arising from the overclocking
 * of AMD processors.
 *
 ******************************************************************************
 */

#ifndef _CCX_ZEN_ZP_IDS_CUSTOM_PSTATES_H_
#define _CCX_ZEN_ZP_IDS_CUSTOM_PSTATES_H_

#pragma pack (push, 1)

IDS_HOOK_STATUS
CcxZenZpIdsDxeCustomPstates (
  IN       HOOK_ID HookId,
  IN       VOID *Handle,
  IN       VOID *Data
  );

#pragma pack (pop)
#endif // _CCX_ZEN_ZP_IDS_CUSTOM_PSTATES_H_

