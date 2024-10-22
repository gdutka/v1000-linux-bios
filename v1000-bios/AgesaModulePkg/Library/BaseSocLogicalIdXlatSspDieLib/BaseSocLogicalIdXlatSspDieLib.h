/*
 ******************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */
#ifndef _BASE_SOC_LOGICAL_ID_XLAT_SSP_DIE_H_
#define _BASE_SOC_LOGICAL_ID_XLAT_SSP_DIE_H_

/// Translation table entry for revision determination
typedef struct {
  UINT8  RawBaseModel;             ///< CPUID Fn0000_0001_EAX[BaseModel]
  UINT8  RawStepping;              ///< CPUID Fn0000_0001_EAX[Stepping]
  UINT16 LogicalRevision;          ///< Logical revision
  UINT16 LogicalCoreRev;           ///< Logical Core revision
} SOC_LOGICAL_REVISION_XLAT;

/// Translation table entry for family determination
typedef struct {
  UINT16 RawFamily;                ///< CPUID Fn0000_0001_EAX[BaseFamily] + [ExtFamily]
  UINT8  RawExtModel;              ///< CPUID Fn0000_0001_EAX[ExtModel]
  UINT32 LogicalFamily;            ///< Logical family
  UINT32 LogicalCoreFamily;        ///< Logical Core family
  CONST SOC_LOGICAL_REVISION_XLAT *LogicalRevisionTable;  ///< Pointer to CPU_LOGICAL_REVISION_XLAT table
} SOC_LOGICAL_FAMILY_XLAT;

#define SOC_LOGICAL_FAMILY_XLAT_TERMINATOR   {0, 0, AMD_FAMILY_UNKNOWN, NULL}
#define SOC_LOGICAL_REVISION_XLAT_TERMINATOR {0, 0, AMD_REVISION_UNKNOWN}

#endif

