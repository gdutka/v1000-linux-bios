/* $NoKeywords:$ */
/**
 * @file
 *
 * FCH DXE Driver
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project   FCH DXE Driver
 * @e \$Revision: 309090 $   @e \$Date: 2014-12-09 10:28:05 -0800 (Tue, 09 Dec 2014) $
 *
 */
/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef _Cf9_RESET_H_
#define _Cf9_RESET_H_

#include <Uefi.h>

#include <Library/IoLib.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/Reset.h>

#include "FchPlatform.h"

//
// Reset control register values
//
#define FULLRESET       0x0E
#define HARDRESET       0x06
#define SOFTRESET       0x04
#define HARDSTARTSTATE  0x02
#define SOFTSTARTSTATE  0x0

#define SUS_S3          0x0C00U     // S3
#define SUS_S5          0x1400U     // S4
#define SLP_TYPE        0x1C00U     // MASK
#define SLP_EN          0x2000U     // BIT13


VOID
EFIAPI
Cf9ResetSystem (
  IN EFI_RESET_TYPE   ResetType,
  IN EFI_STATUS       ResetStatus,
  IN UINTN            DataSize,
  IN CHAR16           *ResetData OPTIONAL
  );
/*++

Routine Description:


Arguments:

  ResetType   -
  ResetStatus -
  DataSize    -
  ResetData   -

Returns:

--*/

#endif

