/* $NoKeywords:$ */
/**
 * @file
 *
 * Generate Maximum System Characteristic Tables (MSCT)
 *
 * Contains code that generate Maximum System Characteristic Table (MSCT)
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

#ifndef _AMD_ACPI_MSCT_H_
#define _AMD_ACPI_MSCT_H_

#include "Protocol/AcpiTable.h"

#pragma pack (push, 1)

/// Format for MSCT Header

/// Format for Maximum Proximity Domain Information Structure

AGESA_STATUS
AmdAcpiMsct (
  IN       EFI_ACPI_TABLE_PROTOCOL  *AcpiTableProtocol,
  IN       AMD_CONFIG_PARAMS        *StdHeader
  );
#pragma pack (pop)
#endif
