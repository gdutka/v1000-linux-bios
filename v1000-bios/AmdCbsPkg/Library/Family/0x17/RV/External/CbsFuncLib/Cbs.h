/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef _CBS_H_
#define _CBS_H_

#include  <IdsHookId.h>

#define CBS_FUNC_DECLARE(Name, Family)	     CBS##Name##Family
#define CBS_FUNCTABLE_DECLARE(Name, Family)	 Cbs##Name##Family

#define CPUID_STEPPING_MASK      0x0000000Ful
#define CPUID_BASE_MODEL_MASK    0x000000F0ul
#define CPUID_BASE_FAMILY_MASK   0x00000F00ul
#define CPUID_EXT_MODEL_MASK     0x000F0000ul
#define CPUID_EXT_FAMILY_MASK    0x0FF00000ul

#define RAW_FAMILY_ID_MASK (UINT32) (CPUID_EXT_FAMILY_MASK | CPUID_EXT_MODEL_MASK | CPUID_BASE_FAMILY_MASK)

#define F15_BR_RAW_ID   0x00660F00ul
#define F17_ZP_RAW_ID   0x00800F00ul
#define F17_RV_RAW_ID   0x00810F00ul
#define F17_RV2_RAW_ID  0x00820F00ul
#define F17_PIC_RAW_ID  0x00810F80ul
#define AMD_RESERVE_ALL 0x00000000ul

#endif

