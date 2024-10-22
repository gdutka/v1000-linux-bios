/* $NoKeywords:$ */
/**
 * @file
 *
 * FCH Base lib
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: FCH
 * @e \$Revision: 309090 $   @e \$Date: 2014-12-09 10:28:05 -0800 (Tue, 09 Dec 2014) $
 *
 */
/*
*****************************************************************************
*
* Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
*
*******************************************************************************
*/
#ifndef _FCH_BASE_LIB_H_
#define _FCH_BASE_LIB_H_

#include <Library/IoLib.h>
#include <Library/PciLib.h>
#include <Library/BaseLib.h>
#include "AGESA.h"

/// CPUID related registers
#define AMD_CPUID_FMF            0x80000001ul  // Family Model Features information

#define CPUID_STEPPING_MASK      0x0000000Ful
#define CPUID_BASE_MODEL_MASK    0x000000F0ul
#define CPUID_BASE_FAMILY_MASK   0x00000F00ul
#define CPUID_EXT_MODEL_MASK     0x000F0000ul
#define CPUID_EXT_FAMILY_MASK    0x0FF00000ul

#define AMD_ACPI_S0              0x00
#define AMD_ACPI_S3              0x03
#define AMD_ACPI_S4              0x04
#define AMD_ACPI_S5              0x05
#define AMD_ACPI_UNDEFINED       0xFF

///
/// PCI_REG8_MASK - Byte Register R/W structure
///
typedef struct _PCI_REG8_MASK {
  UINT8                 RegIndex;                       /// RegIndex - Reserved
  UINT8                 AndMask;                        /// AndMask - Reserved
  UINT8                 OrMask;                         /// OrMask - Reserved
} PCI_REG8_MASK;

///
/// ACPI_REG8_MASK - ACPI MMIO register R/W structure
///
typedef struct _ACPI_REG8_MASK {
  UINT8        MmioBase;               /// MmioBase: Index of Fch block (For instance GPIO_BASE:0x01 SMI_BASE:0x02)
  UINT8        MmioReg;                /// MmioReg      : Register index
  UINT8        DataAndMask;            /// DataANDMask  : AND Register Data
  UINT8        DataOrMask;             /// DataOrMask   : Or Register Data
} ACPI_REG8_MASK;

///
/// SCI_MAP_CONTROL - FCH SCI MAP CONTROL
///
/*
typedef struct {
  IN         UINT8        InputPin;              ///< Input Pin, valid range 0-63
  IN         UINT8        GpeMap;                ///< Gpe Map, valid range 0-31
} SCI_MAP_CONTROL;
*/

VOID
FchReadPmio (
  IN       UINT8               Address,
  IN       ACCESS_WIDTH        AccessWidth,
  IN       VOID                *Value
);

VOID
FchWritePmio (
  IN       UINT8               Address,
  IN       ACCESS_WIDTH        AccessWidth,
  IN       VOID                *Value
);

VOID
FchRwPmio (
  IN       UINT8               Address,
  IN       ACCESS_WIDTH        AccessWidth,
  IN       UINT32              AndMask,
  IN       UINT32              OrMask
);

VOID
FchGetAcpiPmEvtBlk (
     OUT   UINT16              *AcpiPmBase
);

UINT8
FchReadSleepType (
  VOID
);

VOID
FchWriteSleepType (
  IN       UINT16  SleepType
  );

UINT32
FchGetSocId (
  VOID
  );

UINT32
FchGetRawId (
  VOID
  );

UINT32
FchGetFamilyId (
  VOID
  );

UINT32
FchGetModelId (
  VOID
  );

BOOLEAN
FchCheckZPB2 (
  );

BOOLEAN
FchCheckSsp (
  );

BOOLEAN
FchCheckMts (
  );

BOOLEAN
FchCheckMtsA0 (
  );

VOID
FchStall (
  IN       UINT32              uSec,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  );

VOID
FchOutPort80 (
  IN       UINT32              pcode,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  );

VOID
FchOutPort1080 (
  IN       UINT32              pcode,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  );

VOID*
FchGetRomSigPtr (
  IN      UINTN               *RomSigPtr,
  IN      AMD_CONFIG_PARAMS   *StdHeader
  );

VOID
FchRecordFchConfigPtr (
  IN       UINT32 FchConfigPtr,
  IN       AMD_CONFIG_PARAMS     *StdHeader
  );

UINT32
FchReadAlink (
  IN       UINT32              Index,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  );

VOID
FchWriteAlink (
  IN       UINT32              Index,
  IN       UINT32              Data,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  );

VOID
FchRwAlink (
  IN       UINT32              Index,
  IN       UINT32              AndMask,
  IN       UINT32              OrMask,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  );

VOID
FchReadPmio2 (
  IN       UINT8               Address,
  IN       UINT8               OpFlag,
  IN       VOID                *Value,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  );

VOID
FchWritePmio2 (
  IN       UINT8               Address,
  IN       UINT8               OpFlag,
  IN       VOID                *Value,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  );

VOID
FchRwPmio2 (
  IN       UINT8 Address,
  IN       UINT8  OpFlag,
  IN       UINT32 AndMask,
  IN       UINT32 OrMask,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  );

VOID
FchReadBiosram (
  IN       UINT8               Address,
  IN       UINT8               OpFlag,
  IN       VOID                *Value,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  );

VOID
FchWriteBiosram (
  IN       UINT8               Address,
  IN       UINT8               OpFlag,
  IN       VOID                *Value,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  );

VOID
FchRecordSmiStatus (
  IN       AMD_CONFIG_PARAMS   *StdHeader
  );

VOID
FchClearAllSmiStatus (
  IN       AMD_CONFIG_PARAMS   *StdHeader
  );

VOID
FchSleepTrapControl (
  IN        BOOLEAN          SleepTrap
  );

VOID
FchSmiGenerateControl (
  IN        BOOLEAN          SmiControl
  );

VOID
FchDisablePowerButtonSmi (
  IN  AMD_CONFIG_PARAMS     *StdHeader
  );

VOID
FchSmnRead (
  IN       UINT32              IohcBus,
  IN       UINT32              SmnAddress,
  IN       UINT32              *Value,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  );

VOID
FchSmnWrite (
  IN       UINT32              IohcBus,
  IN       UINT32              SmnAddress,
  IN       UINT32              *Value,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  );

VOID
FchSmnRW (
  IN       UINT32              IohcBus,
  IN       UINT32              SmnAddress,
  IN       UINT32              AndMask,
  IN       UINT32              OrMask,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  );

VOID
FchSmnRead8 (
  IN       UINT32              IohcBus,
  IN       UINT32              SmnAddress,
  IN       UINT8               *Value8,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  );

VOID
FchSmnWrite8 (
  IN       UINT32              IohcBus,
  IN       UINT32              SmnAddress,
  IN       UINT8               *Value8,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  );

VOID
FchSmnRW8 (
  IN       UINT32              IohcBus,
  IN       UINT32              SmnAddress,
  IN       UINT8               AndMask,
  IN       UINT8               OrMask,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  );

VOID
FchSetPciByteTable (
  IN       PCI_REG8_MASK       *pPciByteTable,
  IN       UINT16              dwTableSize,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  );

VOID
FchSetFchAcpiMmioTbl (
  IN       ACPI_REG8_MASK      *pAcpiTbl,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  );

VOID
FchSetFchSciMapTbl (
  IN       SCI_MAP_CONTROL  *pSciMapTbl,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  );

VOID
FchGetChipStrap (
  IN       VOID                *Value,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  );

VOID
FchGetChipStrap (
  IN       VOID                *Value,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  );

VOID
FchGetEfuseStatus (
  IN       VOID                *Value,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  );

VOID
FchAcLossControl (
  IN       UINT8 AcLossControlValue,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  );

#endif

