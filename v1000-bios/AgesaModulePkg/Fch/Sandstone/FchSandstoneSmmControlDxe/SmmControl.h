/* $NoKeywords:$ */
/**
 * @file
 *
 * FCH DXE Driver
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project   SMM Control DXE Driver
 * @e \$Revision: 309090 $   @e \$Date: 2014-12-09 10:28:05 -0800 (Tue, 09 Dec 2014) $
 *
 */
/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef _AMD_SMM_CONTROL_H_
#define _AMD_SMM_CONTROL_H_

#include <Uefi.h>
#include <Library/HobLib.h>
#include <Library/DebugLib.h>
#include <Library/DxeServicesTableLib.h>
#include <Library/IoLib.h>

#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeLib.h>
#include <Guid/EventGroup.h>

#include "FchPlatform.h"
#include <Library/FchDxeLib.h>

// Consumed Protocols
#include <Protocol/PciRootBridgeIo.h>

// Produced Protocols
#include <Protocol/SmmControl2.h>

//
// Module specific definitions
//

#endif // _AMD_SMM_CONTROL_H_

