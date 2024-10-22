/* $NoKeywords:$ */
/**
 * @file
 *
 *
 * AMD Ccx Microcode Patch Library
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Lib
 * @e \$Revision: 312770 $   @e \$Date: 2015-02-11 13:24:42 +0800 (Wed, 11 Feb 2015) $
 *
 */
/*
 ******************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 **/

#ifndef _CCX_MICROCODE_PATCH_LIB_H_
#define _CCX_MICROCODE_PATCH_LIB_H_
#include "cpuRegisters.h"
#pragma pack (push, 1)

#define MSR_PATCH_LEVEL  0x0000008Bul
#define MSR_PATCH_LOADER 0xC0010020ul
#define AMD_CPUID_FMF    0x80000001ul

/// Microcode patch field definitions
typedef struct {
  UINT32   DateCode;                   ///< Date of patch creation
  UINT32   PatchID;                    ///< Patch level
  UINT16   MicrocodePatchDataID;       ///< Internal use only
  UINT8    MicrocodePatchDataLength;   ///< Internal use only
  UINT8    InitializationFlag;         ///< Internal use only
  UINT32   MicrocodePatchDataChecksum; ///< Doubleword sum of data block
  UINT32   Chipset1DeviceID;           ///< Device ID of 1st Chipset device to match
  UINT32   Chipset2DeviceID;           ///< Device ID of 2nd Chipset device to match
  UINT16   ProcessorRevisionID;        ///< Equivalent ID
  UINT8    Chipset1RevisionID;         ///< Revision level of 1st Chipset device to match
  UINT8    Chipset2RevisionID;         ///< Revision level of 2nd Chipset device to match
  UINT8    BiosApiRevision;            ///< BIOS INT 15 API revision required
  UINT8    Reserved1[3];               ///< Reserved
  UINT32   MatchRegister0;             ///< Internal use only
  UINT32   MatchRegister1;             ///< Internal use only
  UINT32   MatchRegister2;             ///< Internal use only
  UINT32   MatchRegister3;             ///< Internal use only
  UINT32   MatchRegister4;             ///< Internal use only
  UINT32   MatchRegister5;             ///< Internal use only
  UINT32   MatchRegister6;             ///< Internal use only
  UINT32   MatchRegister7;             ///< Internal use only
  UINT8    PatchDataBlock[896];        ///< Raw patch data
  UINT8    Reserved2[896];             ///< Reserved
  UINT8    X86CodePresent;             ///< Boolean to determine if executable code exists
  UINT8    X86CodeEntry[191];          ///< Code to execute if X86CodePresent != 0
} MICROCODE_PATCH;

/// Patch Loader Register
typedef struct {
  UINT64 PatchBase:32;                ///< Linear address of patch header address block
  UINT64 SBZ:32;                      ///< Should be zero
} PATCH_LOADER_MSR;

/// Patch Loader
typedef union {
  UINT64           RawData;           ///< Raw Data
  PATCH_LOADER_MSR BitFields;         ///< BitFields
} PATCH_LOADER;



/* -----------------------------------------------------------------------------*/
/**
 *  Update microcode patch in current processor.
 *
 *  Then reads the patch id, and compare it to the expected, in the Microprocessor
 *  patch block.
 *
 *  @param[in,out] UcodePatchAddr        - The selected UcodePatch address, return 0 if not found
 *  @param[in] StdHeader                 - Config handle for library and services.
 *
 *  @retval    TRUE   - Patch Loaded Successfully.
 *  @retval    FALSE  - Patch Did Not Get Loaded.
 *
 */
BOOLEAN
LoadMicrocodePatch (
  IN OUT   UINT64            *UcodePatchAddr,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  );

#pragma pack (pop)
#endif // _CCX_MICROCODE_PATCH_LIB_H_

