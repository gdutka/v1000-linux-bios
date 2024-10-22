/*****************************************************************************
 *
 * Copyright (C) 2012-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 ******************************************************************************
 */

#ifndef _AMD_CPM_PLATFORM_H_
#define _AMD_CPM_PLATFORM_H_

// Platform BIOS should redefine the following CPU SUPPORT defines in AmdCpmPkg\Addendum\Oem\*Board*\AmdCpmPlatform.h.
// Then, copy AmdCpmPkg\Addendum\Oem\*Board*\AmdCpmPlatform.h to AmdCpmPkg\AmdCpmPlatform.h before building BIOS.
  #define AMD_CPM_CPU_FAMILY_0x14_ON_SUPPORT
  #define AMD_CPM_CPU_FAMILY_0x15_TN_SUPPORT
  #define AMD_CPM_CPU_FAMILY_0x15_KV_SUPPORT
  #define AMD_CPM_CPU_FAMILY_0x15_CZ_SUPPORT
  #define AMD_CPM_CPU_FAMILY_0x16_KB_SUPPORT
  #define AMD_CPM_CPU_FAMILY_0x16_ML_SUPPORT
  #define AMD_CPM_CPU_FAMILY_0x16_NL_SUPPORT
  #define AMD_CPM_CPU_FAMILY_0x21_AM_SUPPORT
  #define AMD_CPM_CPU_FAMILY_0x15_ST_SUPPORT
  #define AMD_CPM_CPU_FAMILY_0x15_BR_SUPPORT
  #define AMD_CPM_CPU_FAMILY_0x17_ZP_SUPPORT
  #define AMD_CPM_CPU_FAMILY_0x17_RV_SUPPORT
  #define AMD_CPM_CPU_FAMILY_0x17_RV2_SUPPORT
  #define AMD_CPM_CPU_FAMILY_0x17_SSP_SUPPORT
  #define AMD_CPM_CPU_FAMILY_0x17_MTS_SUPPORT

#endif
