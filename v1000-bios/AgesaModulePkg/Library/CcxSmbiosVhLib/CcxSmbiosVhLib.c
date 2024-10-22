/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Ccx SMBIOS Valhalla Library
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
 */


/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include <Library/BaseLib.h>
#include <AGESA.h>
#include <cpuRegisters.h>
#include <CcxRegistersVh.h>
#include <Filecode.h>
#include "CcxSmbiosVhCommonLib.h"
#include <Library/AmdBaseLib.h>
#include <Library/CcxSmbiosLib.h>

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
#define FILECODE LIBRARY_CCXSMBIOSVHLIB_CCXSMBIOSVHLIB_FILECODE

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/* -----------------------------------------------------------------------------*/
/**
 *
 *  CcxGetProcessorFamilyForSmbios
 *
 *    Get processor family information for SMBIOS
 *
 *    @param[in]      Socket         Zero-based socket number to check
 *    @param[in]      StdHeader      Standard Head Pointer
 *
 */
UINT16
CcxGetProcessorFamilyForSmbios (
  IN       UINTN               Socket,
  IN       AMD_CONFIG_PARAMS  *StdHeader
  )
{
  return 0x6B;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *  CcxGetProcessorUpgradeForSmbios
 *
 *    Get processor upgrade information for SMBIOS
 *
 *    @param[in]      Socket         Zero-based socket number to check
 *    @param[in]      StdHeader      Standard Head Pointer
 *
 */
UINT8
CcxGetProcessorUpgradeForSmbios (
  IN       UINTN               Socket,
  IN       AMD_CONFIG_PARAMS  *StdHeader
  )
{
  UINT8       PackageType;
  UINT8       ProcUpgrade;
  CPUID_DATA  CpuId;

  AsmCpuid (
      AMD_CPUID_FMF,
      &(CpuId.EAX_Reg),
      &(CpuId.EBX_Reg),
      &(CpuId.ECX_Reg),
      &(CpuId.EDX_Reg)
      );
  PackageType = (UINT8) (CpuId.EBX_Reg >> 28) & 0xF; // bit 31:28

  switch (PackageType) {
  case 4:
    ProcUpgrade = P_UPGRADE_SP3;
    break;
  case 7:
    ProcUpgrade = P_UPGRADE_SP3r2;
    break;
  default:
    ProcUpgrade = P_UPGRADE_UNKNOWN;
    break;
  }
  return ProcUpgrade;
}

VOID
CcxGetCacheInfo (
  IN       UINTN               Socket,
  IN       GET_CACHE_INFO     *CacheInfo,
  IN       AMD_CONFIG_PARAMS  *StdHeader
  )
{
  UINT32                CacheSize;
  UINT32                NumThreads;
  UINT32                NumSharing;
  CPUID_DATA            CpuId;

  AsmCpuidEx (
      0x80000008,
      0,
      &(CpuId.EAX_Reg),
      &(CpuId.EBX_Reg),
      &(CpuId.ECX_Reg),
      &(CpuId.EDX_Reg)
      );
  NumThreads = ((CpuId.ECX_Reg & 0xFF) + 1);

  // L1 Size & Associativity
  AsmCpuidEx (
      0x8000001D,
      0,
      &(CpuId.EAX_Reg),
      &(CpuId.EBX_Reg),
      &(CpuId.ECX_Reg),
      &(CpuId.EDX_Reg)
      );
  NumSharing = (((CpuId.EAX_Reg >> 14) & 0xFFF) + 1);
  AsmCpuid (
      AMD_CPUID_TLB_L1Cache,
      &(CpuId.EAX_Reg),
      &(CpuId.EBX_Reg),
      &(CpuId.ECX_Reg),
      &(CpuId.EDX_Reg)
      );
  CacheSize = (CpuId.ECX_Reg >> 24) * (NumThreads / NumSharing);

  AsmCpuidEx (
      0x8000001D,
      1,
      &(CpuId.EAX_Reg),
      &(CpuId.EBX_Reg),
      &(CpuId.ECX_Reg),
      &(CpuId.EDX_Reg)
      );
  NumSharing = (((CpuId.EAX_Reg >> 14) & 0xFFF) + 1);
  AsmCpuid (
      AMD_CPUID_TLB_L1Cache,
      &(CpuId.EAX_Reg),
      &(CpuId.EBX_Reg),
      &(CpuId.ECX_Reg),
      &(CpuId.EDX_Reg)
      );
  CacheSize += (CpuId.EDX_Reg >> 24) * (NumThreads / NumSharing);
  CacheInfo->CacheEachLevelInfo[CpuL1Cache].CacheSize = AdjustGranularity (&CacheSize);
  CacheInfo->CacheEachLevelInfo[CpuL1Cache].Associativity = DMI_ASSOCIATIVE_8_WAY;

  // L2 Size & Associativity
  AsmCpuidEx (
      0x8000001D,
      2,
      &(CpuId.EAX_Reg),
      &(CpuId.EBX_Reg),
      &(CpuId.ECX_Reg),
      &(CpuId.EDX_Reg)
      );
  NumSharing = (((CpuId.EAX_Reg >> 14) & 0xFFF) + 1);
  AsmCpuid (
      AMD_CPUID_L2L3Cache_L2TLB,
      &(CpuId.EAX_Reg),
      &(CpuId.EBX_Reg),
      &(CpuId.ECX_Reg),
      &(CpuId.EDX_Reg)
      );
  CacheSize = (CpuId.ECX_Reg >> 16) * (NumThreads / NumSharing);
  CacheInfo->CacheEachLevelInfo[CpuL2Cache].CacheSize = AdjustGranularity (&CacheSize);
  CacheInfo->CacheEachLevelInfo[CpuL2Cache].Associativity = DMI_ASSOCIATIVE_8_WAY;

  // L3 Size & Associativity
  AsmCpuid (
      AMD_CPUID_L2L3Cache_L2TLB,
      &(CpuId.EAX_Reg),
      &(CpuId.EBX_Reg),
      &(CpuId.ECX_Reg),
      &(CpuId.EDX_Reg)
      );
  CacheSize = (CpuId.EDX_Reg >> 18) * 512;
  CacheInfo->CacheEachLevelInfo[CpuL3Cache].CacheSize = AdjustGranularity (&CacheSize);
  switch (((CpuId.EDX_Reg >> 12) & 0xF)) {
  case 8:
    CacheInfo->CacheEachLevelInfo[CpuL3Cache].Associativity = DMI_ASSOCIATIVE_16_WAY;
    break;
  case 0xA:
    CacheInfo->CacheEachLevelInfo[CpuL3Cache].Associativity = DMI_ASSOCIATIVE_32_WAY;
    break;
  case 0xC:
    CacheInfo->CacheEachLevelInfo[CpuL3Cache].Associativity = DMI_ASSOCIATIVE_64_WAY;
    break;
  default:
    CacheInfo->CacheEachLevelInfo[CpuL3Cache].Associativity = DMI_ASSOCIATIVE_UNKNOWN;
    break;
  }
}

/*---------------------------------------------------------------------------------------
 *                           L O C A L    F U N C T I O N S
 *---------------------------------------------------------------------------------------
 */


