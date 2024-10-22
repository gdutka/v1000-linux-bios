/* $NoKeywords:$ */
/**
 * @file
 *
 * Generate System Locality Distance Information Table (SLIT).
 *
 * Contains implementation of the interfaces: Generate SLIT in AmdAcpiSlit.h. *
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
#ifndef _AMD_ACPI_SLIT_H_
#define _AMD_ACPI_SLIT_H_
#include "Protocol/AcpiTable.h"
#pragma pack (push, 1)

/*----------------------------------------------------------------------------------------
 *                    T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */
/// Format for SLIT Header
typedef struct {
  UINT8   Sign[4];                   ///< SLIT, Signature for the System Locality Distance Information Table
  UINT32  Length;                    ///< Length, in bytes, of the entire SLIT
  UINT8   Revision;                  ///< 1
  UINT8   Checksum;                  ///< Entire table must sum to zero.
  UINT8   OemId[6];                  ///< OEM ID
  UINT8   OemTableId[8];             ///< OEM Tabled ID
  UINT32  OemRev;                    ///< OEM Revision
  UINT8   CreatorId[4];              ///< Creator ID
  UINT32  CreatorRev;                ///< Creator Revision
  UINT64  NumberOfSystemLocalities;  ///< Indicates the number of System Localities in the system
} SLIT_HEADER;

/*----------------------------------------------------------------------------------------
 *                 D E F I N I T I O N S     A N D     M A C R O S
 *----------------------------------------------------------------------------------------
 */

AGESA_STATUS
AmdAcpiSlit (
  IN       EFI_ACPI_TABLE_PROTOCOL  *AcpiTableProtocol,
  IN       AMD_CONFIG_PARAMS        *StdHeader
  );

#pragma pack (pop)

#endif // _AMD_ACPI_SLIT_H_
