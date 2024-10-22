/* $NoKeywords:$ */
/**
 * @file
 *
 * PSP TPM ACPI related functions
 *
 * Contains PSP TPM interface
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  PSP
 * @e \$Revision$   @e \$Date$
 */
/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 **/
#ifndef _FTPM_ACPI_H_
#define _FTPM_ACPI_H_

#include <Protocol/AcpiSystemDescriptionTable.h>

#define TPM2_ACPI_OEM_ID        {'A', 'M', 'D', ' ', ' ', ' '}
#define TPM2_ACPI_TABLE_OEM_ID  {'A', 'G', 'E', 'S', 'A', ' ',' ',' '}
#define TPM2_ACPI_OEM_REVISION  SIGNATURE_32 ('P', 'S', 'P', ' ')

#pragma  pack (push, 1)
/// Define TPM_2_ACPI_TABLE
typedef struct {
  EFI_ACPI_SDT_HEADER             Header;         ///< Header
  UINT32                          Flags;          ///< Flags
  EFI_PHYSICAL_ADDRESS            ControlArea;    ///< ControlArea
  UINT32                          StartMethod;    ///< StartMethod
} TPM2_ACPI_TABLE;

#pragma  pack (pop)

#endif //_FTPM_ACPI_H_
