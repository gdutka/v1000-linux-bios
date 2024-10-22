/*****************************************************************************
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */

#include "CBSFchFunDXE_RV.h"

#ifdef AMD_CBS_FCH_RV
DXE_CBS_FUNCTION_STRUCT CBS_FUNCTABLE_DECLARE(FchDxeFunTable,_RV)[] =
{
  {
    F17_RV_RAW_ID,
    AMD_RESERVE_ALL,
    CBS_FUNC_DECLARE(FchInitEnvParams,_RV),
    IDS_HOOK_FCH_INIT_ENV
  },

  {
    F17_RV2_RAW_ID,
    AMD_RESERVE_ALL,
    CBS_FUNC_DECLARE(FchInitEnvParams,_RV),
    IDS_HOOK_FCH_INIT_ENV
  },

  {
    F17_PIC_RAW_ID,
    AMD_RESERVE_ALL,
    CBS_FUNC_DECLARE(FchInitEnvParams,_RV),
    IDS_HOOK_FCH_INIT_ENV
  },

  {
    0,        // F17_RV_RAW_ID,
    0,        // AMD_RESERVE_ALL,
    NULL,     // CBS_FUNC_DECLARE(FchFunc_Init_DXE,_RV),
    0         // CBS_CALLOUT_ID
  }
};

VOID
CBS_FUNC_DECLARE(FchInitEnvParams,_RV) (
  IN EFI_BOOT_SERVICES        *BootServices,
  IN OUT  VOID                *DataParams,
  IN AMD_SYSTEM_CONFIGURATION *SystemConfiguration
  )
{
  UINT32                    PcdSata32;
  CBS_IDS_HOOK_PARAMS       *CbsIdsHookParams;
  FCH_DATA_BLOCK            *FchParams;

  CbsIdsHookParams = (CBS_IDS_HOOK_PARAMS *)DataParams;
  FchParams = (FCH_DATA_BLOCK *)(CbsIdsHookParams->Data);

  // I2C, Uart, ESPI
  PcdSata32 = PcdGet32 (FchRTDeviceEnableMap);

  if (SystemConfiguration->CbsCmnFchI2C0Config != 0xf) { // Auto
	if (SystemConfiguration->CbsCmnFchI2C0Config) {
	  PcdSata32 |= BIT5;
	} else {
	  PcdSata32 &= ~ BIT5;
	}
  }
  if (SystemConfiguration->CbsCmnFchI2C1Config != 0xf) { // Auto
	if (SystemConfiguration->CbsCmnFchI2C1Config) {
	  PcdSata32 |= BIT6;
	} else {
	  PcdSata32 &= ~ BIT6;
	}
  }
  if (SystemConfiguration->CbsCmnFchI2C2Config != 0xf) { // Auto
	if (SystemConfiguration->CbsCmnFchI2C2Config) {
	  PcdSata32 |= BIT7;
	} else {
	  PcdSata32 &= ~ BIT7;
	}
  }
  if (SystemConfiguration->CbsCmnFchI2C3Config != 0xf) { // Auto
	if (SystemConfiguration->CbsCmnFchI2C3Config) {
	  PcdSata32 |= BIT8;
	} else {
	  PcdSata32 &= ~ BIT8;
	}
  }
  if (SystemConfiguration->CbsCmnFchI2C4Config != 0xf) { // Auto
	if (SystemConfiguration->CbsCmnFchI2C4Config) {
	  PcdSata32 |= BIT9;
	} else {
	  PcdSata32 &= ~ BIT9;
	}
  }
  if (SystemConfiguration->CbsCmnFchI2C5Config != 0xf) { // Auto
	if (SystemConfiguration->CbsCmnFchI2C5Config) {
	  PcdSata32 |= BIT10;
	} else {
	  PcdSata32 &= ~ BIT10;
	}
  }
  if (SystemConfiguration->CbsCmnFchUart0Config != 0xf) { // Auto
	if (SystemConfiguration->CbsCmnFchUart0Config) {
	  PcdSata32 |= BIT11;
	} else {
	  PcdSata32 &= ~ BIT11;
	}
  }
  if (SystemConfiguration->CbsCmnFchUart1Config != 0xf) { // Auto
	if (SystemConfiguration->CbsCmnFchUart1Config) {
	  PcdSata32 |= BIT12;
	} else {
	  PcdSata32 &= ~ BIT12;
	}
  }
  if (SystemConfiguration->CbsCmnFchUart2Config != 0xf) { // Auto
	if (SystemConfiguration->CbsCmnFchUart2Config) {
	  PcdSata32 |= BIT16;
	} else {
	  PcdSata32 &= ~ BIT16;
	}
  }
  if (SystemConfiguration->CbsCmnFchUart3Config != 0xf) { // Auto
	if (SystemConfiguration->CbsCmnFchUart3Config) {
	  PcdSata32 |= BIT26;
	} else {
	  PcdSata32 &= ~ BIT26;
	}
  }

  if (SystemConfiguration->CbsCmnFchEspiConfig != 0xf) { // Auto
	if (SystemConfiguration->CbsCmnFchEspiConfig) {
	  PcdSata32 |= BIT27;
	} else {
	  PcdSata32 &= ~ BIT27;
	}
  }
  PcdSet32 (FchRTDeviceEnableMap, PcdSata32);
}
#endif

