/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD CCX x86 Base Library
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Lib
 * @e \$Revision$   @e \$Date$
 *
 */
/*
 ******************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 **/

#ifndef _CCX_BASE_X86_LIB_H_
#define _CCX_BASE_X86_LIB_H_
#include "AGESA.h"
/*---------------------------------------------------------------------------------------*/
/**
 * Returns the MMIO address used for PCI extended configuration space
 *
 * @param[in]     MmioAddress   Mmio Address
 * @param[in]     MmioSize      Mmio Size
 * @param[in]     StdHeader     Standard configuration header (Optional)
 */
BOOLEAN
CcxGetPciMmioAddress (
     OUT   UINT64            *MmioAddress,
     OUT   UINT32            *MmioSize,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

/*---------------------------------------------------------------------------------------*/
/**
 * Returns the package type mask for the processor
 *
 *
 * @param[in]     StdHeader     Standard configuration header (Optional)
 */
UINT32
CcxGetProcessorPackageType (
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

/*---------------------------------------------------------------------------------------*/
/**
 * Returns the core's APIC ID at reset
 *
 *
 * @param[in]     StdHeader     Standard configuration header (Optional)
 */
UINT32
CcxGetInitialApicID (
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

/*---------------------------------------------------------------------------------------*/
/**
 * Returns the value of ThreadsPerCore
 *
 *
 * @retval Value of ThreadsPerCore + 1
 */
UINT8
CcxGetThreadsPerCore (
  );


/*---------------------------------------------------------------------------------------*/
/**
 * Returns the value of PhysAddrSize
 *
 *
 * @retval Value of CPUID_Fn8000_00008_EAX[7:0, PhysAddrSize]
 */
UINT8
CcxGetPhysAddrSize (
  );


/*---------------------------------------------------------------------------------------*/
/**
 * Returns the value of ApicIdCoreIdSize
 *
 *
 * @retval Value of CPUID_Fn8000_00008_ECX[15:12, ApicIdCoreIdSize]
 */
UINT8
CcxGetApicIdCoreIdSize(
  );


/*---------------------------------------------------------------------------------------*/
/**
 * Returns the value of LFuncExt
 *
 *
 * @retval Value of CPUID_Fn8000_00000_EAX[31:0, LFuncExt]
 */
UINT32
CcxGetLFuncExt (
  );

#endif // _CCX_BASE_X86_LIB_H_

