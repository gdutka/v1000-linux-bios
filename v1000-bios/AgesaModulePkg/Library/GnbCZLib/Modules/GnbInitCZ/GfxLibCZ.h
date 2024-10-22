/* $NoKeywords:$ */
/**
 * @file
 *
 * Various GFX service procedures
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 309090 $   @e \$Date: 2014-12-10 02:28:05 +0800 (Wed, 10 Dec 2014) $
 *
 */
/*
*****************************************************************************
*
* Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
*
*******************************************************************************
*
*/
#ifndef _GFXLIBCZ_H_
#define _GFXLIBCZ_H_

// GPU information for CZ
#define GPU__GC__NUM_SE 1
#define GPU__GC__NUM_SH_PER_SE 1
#define GPU__GC__NUM_CU_PER_SH 10

/// Flags field of the CRAT HSA Processing Unit Affinity Structure
typedef struct {
  UINT32 Enabled:1;                  ///< Enabled
  UINT32 HotPluggable:1;             ///< Hot Pluggable
  UINT32 CpuPresent:1;               ///< Cpu Present
  UINT32 GpuPresent:1;               ///< Gpu Present
  UINT32 IommuPresent:1;             ///< IOMMU Present
  UINT32 :27;                        ///< Reserved
} CRAT_HSA_PROCESSING_UNIT_FLAG;

/// CRAT HSA Processing Unit Affinity Structure
typedef struct {
  UINT8  Type;                       ///< 0 - CRAT HSA Processing Unit Structure
  UINT8  Length;                     ///< 40
  UINT16 Reserved;                   ///< Reserved
  CRAT_HSA_PROCESSING_UNIT_FLAG Flags; ///< Flags - HSA Processing Unit Affinity Structure
  UINT32 ProximityDomain;            ///< Integer that represents the proximity domain to which the node belongs to
  UINT32 ProcessorIdLow;             ///< Low value  of the logical processor included in this HSA proximity domain
  UINT16 NumCPUCores;                ///< Indicates count of CCompute execution units present in this APU node.
  UINT16 NumSIMDCores;               ///< Indicates maximum count of HCompute SIMD cores present in this node.
  UINT16 MaxWavesSIMD;               ///< This identifies the max. number of launched waves per SIMD.
  UINT16 IoCount;                    ///< Number of discoverable IO Interfaces connecting this node to other components.
  UINT16 HSACapability;              ///< Must be 0
  UINT16 LDSSizeInKB;                ///< Size of LDS memory per SIMD Wavefront
  UINT8  WaveFrontSize;              ///< 64, may be 32 for some FSA based architectures
  UINT8  NumBanks;                   ///< Number of Banks or "Shader Engines", typically 1 or 2
  UINT16 uEngineIdentifier;          ///< Identifier (Rev) of the GPU uEngine or firmware
  UINT8  NumArrays;                  ///< Number of SIMD Arrays per Engine
  UINT8  NumCUPerArray;              ///< Number of Compute Units (CU) per SIMD Array
  UINT8  NumSIMDPerCU;               ///< Number of SIMD representing a Compute Unit
  UINT8  MaxSlotsScratchCU;          ///< Max. Number of temp. memory ("scratch") wave slots
                                     ///< available to access, may be 0 if HW has no restrictions
  UINT8  Reserved1[4];               ///< Reserved
} CRAT_HSA_PROCESSING_UNIT;

VOID
GfxDisableControllerCZ (
  IN       AMD_CONFIG_PARAMS      *StdHeader
  );

UINT32
GfxLibGetSystemPllCofCZ (
  IN       AMD_CONFIG_PARAMS      *StdHeader
  );

UINT32
GfxCalculateClockCZ (
  IN       UINT8                       Did,
  IN       AMD_CONFIG_PARAMS          *StdHeader
  );

BOOLEAN
GfxIsVbiosPostedCZ (
  IN      GFX_PLATFORM_CONFIG   *Gfx
  );

AGESA_STATUS
GfxMakeHSAProcUnitEntryCZ (
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
);

#endif

