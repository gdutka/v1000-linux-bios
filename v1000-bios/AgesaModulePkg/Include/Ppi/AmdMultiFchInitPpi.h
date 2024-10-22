/* $NoKeywords:$ */
 /**
 * @file
 *
 * Multiple FCH Init PPI.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  UEFI
 * @e \$Revision: 309090 $   @e \$Date: 2014-12-09 10:28:05 -0800 (Tue, 09 Dec 2014) $
 *
 */
/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef _AMD_MULTI_FCH_INIT_PPI_H_
#define _AMD_MULTI_FCH_INIT_PPI_H_

extern EFI_GUID gAmdFchMultiFchInitPpiGuid;

typedef struct _AMD_FCH_MULTI_FCH_INIT_PPI  AMD_FCH_MULTI_FCH_INIT_PPI;

typedef struct _FCH_MULITI_FCH_RESET_DATA_BLOCK {
  UINT64                FchAcpiMmioBase[8];            ///< FCH ACPI MMIO Base
} FCH_MULITI_FCH_RESET_DATA_BLOCK;

/// PPI definition
typedef struct _AMD_FCH_MULTI_FCH_INIT_PPI {
  UINTN                              Revision;         ///< Revision number
  FCH_MULITI_FCH_RESET_DATA_BLOCK    FchMfResetData;  ///< Data Block
} AMD_FCH_MULTI_FCH_INIT_PPI;

//
// current PPI revision
//
#define AMD_MULTI_FCH_INIT_PPI_REV  0x01


#endif // _AMD_MULTI_FCH_INIT_PPI_H_

