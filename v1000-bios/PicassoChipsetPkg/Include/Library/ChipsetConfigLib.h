/** @file
  Chipset Configuration Helper Library

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _CHIPSET_SETUP_CONFIG_LIB_H_
#define _CHIPSET_SETUP_CONFIG_LIB_H_

#include <Uefi.h>
#include <ChipsetSetupConfig.h>

#define CHIPSET_CONFIGURATION_SIZE  sizeof(CHIPSET_CONFIGURATION)

/**
 Retrieve chipset setup configuration data

 @param[out] ChipsetConfig       Pointer to the structure of CHIPSET_CONFIGURATION,
                                this pointer must be allocated with sizeof(CHIPSET_CONFIGURATION)
                                before being called

 @retval EFI_SUCCESS            The chipset configuration is successfully retrieved
 @retval EFI_INVALID_PARAMETER  NULL pointer for input ChipsetConfig paramater
 @return others                 Failed to retrieve kernel configuration
**/
EFI_STATUS
EFIAPI
SetChipsetConfigVariable (
  IN CHIPSET_CONFIGURATION        *ChipsetConfig
  );

/**
 Retrieve chipset setup configuration data

 @param[out] ChipsetConfig       Pointer to the structure of CHIPSET_CONFIGURATION,
                                this pointer must be allocated with sizeof(CHIPSET_CONFIGURATION)
                                before being called

 @retval EFI_SUCCESS            The chipset configuration is successfully retrieved
 @retval EFI_INVALID_PARAMETER  NULL pointer for input ChipsetConfig paramater
 @return others                 Failed to retrieve kernel configuration
**/
EFI_STATUS
EFIAPI
GetChipsetConfiguration (
  OUT CHIPSET_CONFIGURATION     *ChipsetConfig
  );

#endif

