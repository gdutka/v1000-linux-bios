/* $NoKeywords:$ */
/**
 * @file
 *
 * FCH common SMM
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
#include "FchPlatform.h"
#define FILECODE PROC_FCH_COMMON_FCHCOMMONSMM_FILECODE
VOID
FchSmmAcpiOn (
  IN      FCH_DATA_BLOCK   *FchDataPtr
  );

/*----------------------------------------------------------------------------------------*/
/**
 * FchSmmAcpiOn - Config Fch during ACPI_ON
 *
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
FchSmmAcpiOn (
  IN  FCH_DATA_BLOCK     *FchDataPtr
  )
{
  //
  // Commented the following code since we need to leave the IRQ1/12 filtering enabled always as per latest
  // recommendation. This is required to fix the keyboard stuck issue when playing games under Windows
  //

  //
  // Disable Power Button SMI
  //
  RwMem (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REGAC, AccessWidth8, ~(UINT32) (BIT6), 0);
}


