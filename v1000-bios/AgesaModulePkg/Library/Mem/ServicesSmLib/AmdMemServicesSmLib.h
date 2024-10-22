/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Memory option API.
 *
 * Contains structures and values used to control the Memory option code.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  OPTION
 * @e \$Revision: 311790 $   @e \$Date: 2015-01-26 23:03:49 -0600 (Mon, 26 Jan 2015) $
 *
 */
/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef _AMD_MEM_SERVICES_SM_LIB_H_
#define _AMD_MEM_SERCICES_SM_LIB_H_

EFI_STATUS
AmdGetMemoryAbove4gbServiceSm (
  IN      EFI_PEI_SERVICES  **PeiServices,
     OUT  UINT32       *AmountOfMemory
  );

EFI_STATUS
AmdGetMemoryBelow4gbServiceSm (
  IN      EFI_PEI_SERVICES  **PeiServices,
     OUT  UINT32       *AmountOfMemory
  );

EFI_STATUS
AmdGetMemoryBelow1TbServiceSm (
  IN      EFI_PEI_SERVICES  **PeiServices,
     OUT  UINT32       *AmountOfMemory
  );

EFI_STATUS
AmdGetBottomIoServiceSm (
  IN      EFI_PEI_SERVICES  **PeiServices,
     OUT  UINT32       *AmountOfMemory
  );

EFI_STATUS
AmdGetTotalMemorySizeServiceSm (
  IN      EFI_PEI_SERVICES  **PeiServices,
     OUT  UINT32       *AmountOfMemory
  );

#endif  // _AMD_MEM_SERCICES_SM_LIB_H_

