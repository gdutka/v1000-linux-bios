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

#ifndef _AMD_MEM_SERVICES_LIB_H_
#define _AMD_MEM_SERCICES_LIB_H_

EFI_STATUS
AmdGetMemoryAbove4gbService (
  IN      EFI_PEI_SERVICES  **PeiServices,
     OUT  UINT32            *AmountOfMemory
  );

EFI_STATUS
AmdGetMemoryBelow4gbService (
  IN      EFI_PEI_SERVICES  **PeiServices,
     OUT  UINT32            *AmountOfMemory
  );

EFI_STATUS
AmdGetMemoryBelow1TbService (
  IN      EFI_PEI_SERVICES  **PeiServices,
     OUT  UINT32            *AmountOfMemory
  );

EFI_STATUS
AmdGetBottomIoService (
  IN      EFI_PEI_SERVICES  **PeiServices,
     OUT  UINT32            *AmountOfMemory
  );

EFI_STATUS
AmdGetTotalMemorySizeService (
  IN      EFI_PEI_SERVICES  **PeiServices,
     OUT  UINT32            *AmountOfMemory
  );

EFI_STATUS
AmdGetMemoryFrequencyService (
  IN      EFI_PEI_SERVICES  **PeiServices,
     OUT  UINT32            Channel,
     OUT  UINT32            *Frequency
  );

EFI_STATUS
AmdGetUmaInfoService (
  IN      EFI_PEI_SERVICES  **PeiServices,
     OUT  UINT32            *UmaInfo
  );


#endif  // _AMD_MEM_SERCICES_LIB_H_

