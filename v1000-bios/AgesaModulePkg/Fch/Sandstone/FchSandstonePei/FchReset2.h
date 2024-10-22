/* $NoKeywords:$ */
/**
 * @file
 *
 * FCH PEIM
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project   FCH PEIM
 * @e \$Revision: 309090 $   @e \$Date: 2014-12-09 10:28:05 -0800 (Tue, 09 Dec 2014) $
 *
 */
/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef _FCH_RESET2_H_
#define _FCH_RESET2_H_

#include <Uefi.h>
#include <Library/IoLib.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>

#define FCH_RESET_REGCD6          0xCD6
#define FCH_RESET_REGCD7          0xCD7
#define FCH_RESET_REGCF9          0xCF9

#define FCH_RESET_PMIOA_REG60     0x60
#define FCH_RESET_PMIOA_REG62     0x62
#define FCH_RESET_PMIOA_REG68     0x68
// Reset control register values
//
#define HARDRESET       0x06
#define SOFTRESET       0x04
#define HARDSTARTSTATE  0x02
#define SOFTSTARTSTATE  0x0

#define SUS_S3          0x0C00U     // S3
#define SUS_S5          0x1400U     // S5
#define SLP_TYPE        0x1C00U     // MASK
#define SLP_EN          0x2000U     // BIT13

#define FCH_DEADLOOP()    { volatile UINTN __i; __i = 1; while (__i); }

//
// Functions Prototypes
//
VOID
EFIAPI
FchPeiReset2 (
  IN EFI_RESET_TYPE     ResetType,
  IN EFI_STATUS         ResetStatus,
  IN UINTN              DataSize,
  IN VOID               *ResetData OPTIONAL
  );
#endif // _FCH_RESET2_H_

