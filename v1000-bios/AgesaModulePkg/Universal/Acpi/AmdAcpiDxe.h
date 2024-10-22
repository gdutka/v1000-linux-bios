/* $NoKeywords:$ */
/**
 * @file
 *
 * Generate SMBIOS type 4 7 16 17 19 20
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Universal
 * @e \$Revision$   @e \$Date$
 *
 */
/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */
#ifndef _UNIVERSAL_ACPI_AMD_ACPI_DXE_H_
#pragma pack (push, 1)

#define ACPI_TABLE_MAX_LENGTH           0x100000ul ///< Reserve 1M for ACPI table

// Defines for Common ACPI
// -----------------------------
#define     SCOPE_OPCODE                0x10
#define     NAME_OPCODE                 0x08
#define     METHOD_OPCODE               0x14
#define     PACKAGE_OPCODE              0x12
#define     BUFFER_OPCODE               0x11
#define     BYTE_PREFIX_OPCODE          0x0A
#define     WORD_PREFIX_OPCODE          0x0B
#define     DWORD_PREFIX_OPCODE         0x0C
#define     STRING_PREFIX_OPCODE        0x0D
#define     QWORD_PREFIX_OPCODE         0x0E
#define     RETURN_OPCODE               0xA4
#define     ACPI_BUFFER                 0x080A0B11ul

/// Format for Header
typedef struct {
  UINT8   Sign[4];                      ///< Signature
  UINT32  TableLength;                  ///< Table Length
  UINT8   Revision;                     ///< Revision
  UINT8   Checksum;                     ///< Checksum
  UINT8   OemId[6];                     ///< OEM ID
  UINT8   OemTableId[8];                ///< OEM Tabled ID
  UINT32  OemRev;                       ///< OEM Revision
  UINT8   CreatorId[4];                 ///< Creator ID
  UINT32  CreatorRev;                   ///< Creator Revision
} ACPI_TABLE_HEADER;

VOID
ChecksumAcpiTable (
  IN OUT   ACPI_TABLE_HEADER *Table,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

#pragma pack (pop)
#endif // _UNIVERSAL_ACPI_AMD_ACPI_DXE_H_

