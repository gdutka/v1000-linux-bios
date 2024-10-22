/* $NoKeywords:$ */
/**
 * @file
 *
 * FCH DXE Driver
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project   Legacy Interrupt DXE Driver
 * @e \$Revision: 309090 $   @e \$Date: 2014-12-09 10:28:05 -0800 (Tue, 09 Dec 2014) $
 *
 */
/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef _AMD_LEGACY_INTERRUPT_H_
#define _AMD_LEGACY_INTERRUPT_H_

#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>

#include "FchPlatform.h"
#include <Library/FchDxeLib.h>

// Consumed Protocols
#include <Protocol/PciRootBridgeIo.h>

// Produced Protocols
#include <Protocol/LegacyInterrupt.h>

//
// Module specific definitions
//
#define     MAX_NUMBER_PIRQS     8

#endif // _AMD_LEGACY_INTERRUPT_H_

