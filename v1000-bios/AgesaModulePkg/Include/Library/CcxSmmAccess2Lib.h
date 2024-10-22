/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Library
 *
 * Contains implementation for the SMM Access2 Protocol
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Lib
 * @e \$Revision:  $   @e \$Date: $
 *
 */
/*
 ******************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef _CCX_SMM_ACCESS2_LIB_H_
#define _CCX_SMM_ACCESS2_LIB_H_

#include <Protocol/SmmAccess2.h>

#define  SMMADDR_ADDRESS      0xC0010112ul
#define  SMMMASK_ADDRESS      0xC0010113ul
#define      SMMLOCK          0x0000000000000001ull
#define  FCH_MMIO_ADDRESS     0xFED80000ull
#define  SMI_REGISTER_SPACE   0x00000200ull
#define  SMI_TRIG0_OFFSET     0x00000098ull
#define      SMIENB           0x80000000ul

///  Struct for Tseg configuration
typedef struct {
  IN      UINT64 TsegBase;  ///< TsegBase
  IN      UINT64 TsegMask;  ///< TsegMask
} CCX_SMM_TSEG_CONFIG;


EFI_STATUS
EFIAPI
CcxSmmOpen2 (
  IN       EFI_SMM_ACCESS2_PROTOCOL *This
  );

EFI_STATUS
EFIAPI
CcxSmmClose2 (
  IN       EFI_SMM_ACCESS2_PROTOCOL *This
  );

EFI_STATUS
EFIAPI
CcxSmmLock2 (
  IN       EFI_SMM_ACCESS2_PROTOCOL *This
  );

EFI_STATUS
EFIAPI
CcxSmmGetCapabilities2 (
  IN CONST EFI_SMM_ACCESS2_PROTOCOL *This,
  IN OUT   UINTN                    *SmramMapSize,
  IN OUT   EFI_SMRAM_DESCRIPTOR     *SmramMap
  );

EFI_STATUS
EFIAPI
CcxSmmAccess2ProtocolInstall (
);

#endif
