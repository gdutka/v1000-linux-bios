/** @file
  This module provides default Setup variable data if Setupvariable is not found
  and also provides a Setup variable cache mechanism in PEI phase

;******************************************************************************
;* Copyright (c) 2013 - 2015, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <PiPei.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <Library/KernelConfigLib.h>
#include <Library/VariableLib.h>


/*
  Update legacy boot type order from PCD setting to kernel configuration

  @param[out]  KernelConfig     A pointer to kernel configuration
*/
VOID
UpdateDefaultLegacyBootTypeOrder (
  OUT KERNEL_CONFIGURATION        *KernelConfig
  )
{
  UINT8                           *DefaultLegacyBootTypeOrder;
  UINTN                           Index;

  if (KernelConfig == NULL) {
    return;
  }

  ZeroMem (KernelConfig->BootTypeOrder, MAX_BOOT_ORDER_NUMBER);

  DefaultLegacyBootTypeOrder = (UINT8 *) PcdGetPtr (PcdLegacyBootTypeOrder);
  if (DefaultLegacyBootTypeOrder == NULL) {
    return;
  }

  Index = 0;
  while (DefaultLegacyBootTypeOrder[Index] != 0 && Index < MAX_BOOT_ORDER_NUMBER) {
    KernelConfig->BootTypeOrder[Index] = DefaultLegacyBootTypeOrder[Index];
    Index++;
  }
}

/**
  Extract default Setup variable data from VFR forms

  @param[in,out]  SetupData     A pointer to the Setup variable data buffer
  @param[in]  SetupDataSize     Data size in bytes of the Setup variable
*/
VOID
ExtractSetupDefault (
  UINT8 *SetupData
  )
{
  UINTN                  SetupDataSize;
  UINT8                  SkuId;
  EFI_STATUS             Status;

  SetupDataSize = GetSetupVariableSize();
  SkuId = (UINT8) LibPcdGetSku ();
  Status = CommonGetDefaultVariable (
             L"Setup",
             &gSystemConfigurationGuid,
             SkuId,
             NULL,
             &SetupDataSize,
             SetupData
             );
  if (Status != EFI_SUCCESS) {
    Status = CommonGetDefaultVariable (
               L"Setup",
               &gSystemConfigurationGuid,
               0,
               NULL,
               &SetupDataSize,
               SetupData
               );
  }
  ASSERT (Status == EFI_SUCCESS);
  UpdateDefaultLegacyBootTypeOrder ((KERNEL_CONFIGURATION *) SetupData);
}