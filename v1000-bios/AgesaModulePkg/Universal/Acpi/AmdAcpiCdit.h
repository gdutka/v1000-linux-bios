/* $NoKeywords:$ */
/**
 * @file
 *
 * Generate Component Locality Distance Information Table (CDIT)
 *
 * Contains implementation of the interfaces: Generate CDIT in AmdAcpiCdit.h. *
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
#ifndef _AMD_ACPI_CDIT_H_
#define _AMD_ACPI_CDIT_H_
#include "Protocol/AcpiTable.h"
#pragma pack (push, 1)

/*----------------------------------------------------------------------------------------
 *                    T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */
/// Format for CDIT Header
typedef struct {
  UINT8   Sign[4];                   ///< CDIT, Signature for the Component Locality Distance Information Table
  UINT32  Length;                    ///< Length, in bytes, of the entire CDIT
  UINT8   Revision;                  ///< 0
  UINT8   Checksum;                  ///< Entire table must sum to zero.
  UINT8   OemId[6];                  ///< OEM ID
  UINT8   OemTableId[8];             ///< OEM Tabled ID
  UINT32  OemRev;                    ///< OEM Revision
  UINT8   CreatorId[4];              ///< Creator ID
  UINT32  CreatorRev;                ///< Creator Revision
  UINT32  NumDomains;                ///< Number of Fusion proximity domains
} CDIT_HEADER;

/*----------------------------------------------------------------------------------------
 *                 D E F I N I T I O N S     A N D     M A C R O S
 *----------------------------------------------------------------------------------------
 */

AGESA_STATUS
AmdAcpiCdit (
  IN       EFI_ACPI_TABLE_PROTOCOL  *AcpiTableProtocol,
  IN       AMD_CONFIG_PARAMS        *StdHeader
  );

#pragma pack (pop)

#endif // _AMD_ACPI_CDIT_H_
