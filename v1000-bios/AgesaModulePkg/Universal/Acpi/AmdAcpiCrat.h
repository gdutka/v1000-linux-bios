/* $NoKeywords:$ */
/**
 * @file
 *
 * Generate Component Resource Affinity Table (CRAT).
 *
 * Contains implementation of the interfaces: Generate CRAT in AmdAcpiCrat.h. *
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
#ifndef _AMD_ACPI_CRAT_H_
#define _AMD_ACPI_CRAT_H_
#include "Protocol/AcpiTable.h"
#include "Protocol/AmdAcpiCratServicesProtocol.h"
#pragma pack (push, 1)

/*----------------------------------------------------------------------------------------
 *                    T Y P E D E F S     A N D     S T R U C T U R E S
 *                            Component Resource Affinity Table (r0.73)
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                 D E F I N I T I O N S     A N D     M A C R O S
 *----------------------------------------------------------------------------------------
 */

/**
 * Make CRAT entry
 *
 *    @param[in]      CratHeaderStructPtr  CRAT header pointer
 *    @param[in, out] TableEnd             The end of CRAT
 *    @param[in, out] StdHeader            Standard Head Pointer
 *
 */
typedef VOID F_MAKE_CRAT_ENTRY (
  IN       CRAT_HEADER       *CratHeaderStructPtr,
  IN OUT   UINT8            **TableEnd,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  );
/// Reference to a Method.
typedef F_MAKE_CRAT_ENTRY *PF_MAKE_CRAT_ENTRY;

/**
 * A struct that contains function pointe
 */
typedef struct _S_MAKE_CRAT_ENTRY {
  PF_MAKE_CRAT_ENTRY MakeCratEntry; ///< Function Pointer, which points to the function which makes CRAT entry.
} S_MAKE_CRAT_ENTRY;


AGESA_STATUS
AmdAcpiCrat (
  IN       EFI_ACPI_TABLE_PROTOCOL  *AcpiTableProtocol,
  IN       AMD_CONFIG_PARAMS        *StdHeader
  );

#pragma pack (pop)

#endif // _AMD_ACPI_CRAT_H_

