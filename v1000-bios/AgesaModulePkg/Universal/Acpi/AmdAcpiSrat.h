/* $NoKeywords:$ */
/**
 * @file
 *
 * Generate System Resource Affinity Table (SRAT)
 *
 * Contains implementation of the interfaces: Generate SRAT in AmdAcpiSrat.h
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
#ifndef _AMD_ACPI_SRAT_H_
#define _AMD_ACPI_SRAT_H_
#include "Protocol/AcpiTable.h"
#include "Protocol/AmdAcpiSratServicesProtocol.h"
#pragma pack (push, 1)

/*----------------------------------------------------------------------------------------
 *                    T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                 D E F I N I T I O N S     A N D     M A C R O S
 *----------------------------------------------------------------------------------------
 */
/**
 * Make SRAT structure
 *
 *    @param[in]      SratHeaderStructPtr  SRAT header pointer
 *    @param[in, out] TableEnd             The end of SRAT
 *
 *    @retval         EFI_SUCCESS          The SRAT structure was successfully created.
 *                    EFI_ABORTED          SRAT should not be created.
 */
typedef EFI_STATUS F_MAKE_SRAT_STRUCT (
  IN       SRAT_HEADER       *SratHeaderStructPtr,
  IN OUT   UINT8            **TableEnd
  );
/// Reference to a Method.
typedef F_MAKE_SRAT_STRUCT *PF_MAKE_SRAT_STRUCT;

/**
 * A struct that contains function pointe
 */
typedef struct _S_MAKE_SRAT_STRUCT {
  PF_MAKE_SRAT_STRUCT MakeSratStruct; ///< Function Pointer, which points to the function which makes SRAT structure.
} S_MAKE_SRAT_STRUCT;

AGESA_STATUS
AmdAcpiSrat (
  IN       EFI_ACPI_TABLE_PROTOCOL  *AcpiTableProtocol,
  IN       AMD_CONFIG_PARAMS        *StdHeader
  );

#pragma pack (pop)

#endif // _AMD_ACPI_SRAT_H_

