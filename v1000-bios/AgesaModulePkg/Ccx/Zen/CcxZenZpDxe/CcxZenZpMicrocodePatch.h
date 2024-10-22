/* $NoKeywords:$ */
/**
 * @file
 *
 * CCX Zen - Zeppelin Microcode definitions
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CCX
 * @e \$Revision$   @e \$Date$
 *
 */
/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */
#ifndef _CCX_ZEN_ZP_MICROCODE_PATCH_H_
#define _CCX_ZEN_ZP_MICROCODE_PATCH_H_

VOID
CcxZenZpMicrocodeInit (
  IN OUT   UINT64             *UcodePatchAddr,
  IN       AMD_CONFIG_PARAMS  *StdHeader
  );

#endif

