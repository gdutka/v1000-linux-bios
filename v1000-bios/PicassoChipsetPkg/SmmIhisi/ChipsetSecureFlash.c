/** @file
  SecureFlashLib Library Instance implementation

;******************************************************************************
;* Copyright (c) 2014 - 2016, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/
#include "ChipsetSecureFlash.h"
#include <Library/KernelConfigLib.h>
#include <Library/PcdLib.h>
#include <Library/ProcessPendingCapsuleLib.h>

BOOLEAN  mInPOST = TRUE;

/**
  Chipset secure flash.(AH=48h)

  @retval EFI_SUCCESS            Function succeeded.
  @return Others                 Error occurred in this function.
**/
EFI_STATUS
SecureFlashPermit (
  VOID
  )
{
  EFI_STATUS                                  Status;
  UINT32                                      TotalImageSize;
  FBTS_EXTEND_FLAG_STRUCTURE                  *ExtendFlagPtr;
  KERNEL_CONFIGURATION                        SystemConfiguration;  
  BOOLEAN                                     EspSupportCapability;

  if (!PcdGetBool (PcdSecureFlashSupported)) {
    return IHISI_UNSUPPORTED_FUNCTION;
  }

  //
  // mInPOST is a flag used for disabling IHISI write function at Runtime.
  // There is a SMI triggerd by SecureFlashDxe to modify this flag.
  //
  if ((mInPOST) && (mH2OIhisi->ReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RCX) == SECURE_FLASH_SIGNATURE)) {
    mInPOST = FALSE;
    return EFI_SUCCESS;
  }
  
  EspSupportCapability = FALSE;
  Status = GetKernelConfiguration (&SystemConfiguration);
  if (!EFI_ERROR (Status) && (SystemConfiguration.BootType != LEGACY_BOOT_TYPE)) {
    EspSupportCapability = TRUE;
  }

  //
  // The image size cannot be zero.
  //
  TotalImageSize = mH2OIhisi->ReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RCX);

  //
  // EDI offset8 Bit0: 0 = Update secure capsule via IHISI. (Default)
  //                   1 = Update secure capsule via ESP (EFI System Partition)
  //    
  ExtendFlagPtr = (FBTS_EXTEND_FLAG_STRUCTURE *)(UINTN)mH2OIhisi->ReadCpuReg32 (EFI_SMM_SAVE_STATE_REGISTER_RDI);
  if (TotalImageSize == 0) {
    if (mH2OIhisi->BufferInCmdBuffer ((VOID *) ExtendFlagPtr, sizeof (FBTS_EXTEND_FLAG_STRUCTURE)) &&
	   (ExtendFlagPtr->Signature == EXTEND_FLAG_INPUT_SIGNATURE) &&
        (ExtendFlagPtr->Flag.Bits.SecureCapsuleOrigin == FROME_ESP) &&
        (EspSupportCapability == TRUE)) {
        Status = SetEspFlashFlag();
        DEBUG ((EFI_D_INFO, "SetEspFlashFlag Status=%x\n",Status));    
        return EFI_SUCCESS;
    }
    return IHISI_ACCESS_PROHIBITED;
  }  
  return EFI_SUCCESS;
}

STATIC
IHISI_REGISTER_TABLE
CHIPSET_SECURE_FLASH_REGISTER_TABLE[] = {
  //
  // AH=48h
  //
  { FBTSOEMCapsuleSecureFlash, "S48Cs_CpSecureFlash", SecureFlashPermit}
};

EFI_STATUS
ChipsetSecureFlashInit (
  VOID
  )
{
  EFI_STATUS              Status;
  IHISI_REGISTER_TABLE    *SubFuncTable;
  UINT16                  TableCount;

  SubFuncTable = CHIPSET_SECURE_FLASH_REGISTER_TABLE;
  TableCount = sizeof(CHIPSET_SECURE_FLASH_REGISTER_TABLE)/sizeof(CHIPSET_SECURE_FLASH_REGISTER_TABLE[0]);
  Status = RegisterIhisiSubFunction (SubFuncTable, TableCount);
  if (EFI_ERROR(Status)) {
    ASSERT_EFI_ERROR (Status);
  }

  return EFI_SUCCESS;
}

