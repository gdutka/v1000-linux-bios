/* $NoKeywords:$ */
/**
 * @file
 *
 * AmdMemServicesLib.c
 *
 * Sub-engine to extract CAD bus settings from recommended table
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Ps)
 * @e \$Revision: 311790 $ @e \$Date: 2015-01-26 23:03:49 -0600 (Mon, 26 Jan 2015) $
 *
 **/
/*****************************************************************************
*
* Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
*
*******************************************************************************
*
*/
/*
 *----------------------------------------------------------------------------
 *                                MODULES USED
 *
 *----------------------------------------------------------------------------
 */



#include "PiPei.h"
#include "Library/BaseMemoryLib.h"
#include "Library/BaseLib.h"
#include "Ppi/AmdMemPpi.h"
#include "Library/Mem/BaseLib/AmdMemBaseLib.h"
#include "Library/AmdBaseLib.h"
#include "Filecode.h"

#define FILECODE LIBRARY_MEM_SERVICESCZLIB_AMDMEMSERVICESCZLIB_FILECODE

/*----------------------------------------------------------------------------
 *                          DEFINITIONS AND MACROS
 *
 *----------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------
 *                           TYPEDEFS AND STRUCTURES
 *
 *----------------------------------------------------------------------------
 */
/*----------------------------------------------------------------------------
 *                        PROTOTYPES OF LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */
/*----------------------------------------------------------------------------
 *                            EXPORTED FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */
/**
  Read Service for the Memory Above 4GB for CZ

  Parameters:
  PeiServices
    A pointer to the PEI services
  AmountOfMemory
    Return data pointer for the amount of memory

  Status Codes Returned:
  EFI_SUCCESS           - Return value is valid
  EFI_UNSUPPORTED       - This function is not supported by this version of the driver
**/
EFI_STATUS
AmdGetMemoryAbove4gbServiceCz (
  IN       EFI_PEI_SERVICES  **PeiServices,
     OUT   UINT32            *AmountOfMemory
  )
{
  return EFI_SUCCESS;
}
/**
  Read Service for the Memory Below 4GB for CZ

  Parameters:
  PeiServices
    A pointer to the PEI services
  AmountOfMemory
    Return data pointer for the amount of memory

  Status Codes Returned:
  EFI_SUCCESS           - Return value is valid
  EFI_UNSUPPORTED       - This function is not supported by this version of the driver
**/
EFI_STATUS
AmdGetMemoryBelow4gbServiceCz (
  IN       EFI_PEI_SERVICES  **PeiServices,
     OUT   UINT32            *AmountOfMemory
  )
{
  return EFI_SUCCESS;
}
/**
  Read Service for the memory Below 1TB for CZ

  Parameters:
  PeiServices
    A pointer to the PEI services
  AmountOfMemory
    Return data pointer for the amount of memory

  Status Codes Returned:
  EFI_SUCCESS           - Return value is valid
  EFI_UNSUPPORTED       - This function is not supported by this version of the driver
**/
EFI_STATUS
AmdGetMemoryBelow1TbServiceCz (
  IN       EFI_PEI_SERVICES  **PeiServices,
     OUT   UINT32            *AmountOfMemory
  )
{
  return EFI_SUCCESS;
}
/**
  Read Service for the Bottom of MMIO for CZ

  Parameters:
  PeiServices
    A pointer to the PEI services
  AmountOfMemory
    Return data pointer for the amount of memory

  Status Codes Returned:
  EFI_SUCCESS           - Return value is valid
  EFI_UNSUPPORTED       - This function is not supported by this version of the driver
**/
EFI_STATUS
AmdGetBottomIoServiceCz (
  IN       EFI_PEI_SERVICES  **PeiServices,
     OUT   UINT32            *AmountOfMemory
  )
{
  return EFI_SUCCESS;
}
/**
  Read Service for the total amount of memory in the system for CZ

  Parameters:
  PeiServices
    A pointer to the PEI services
  AmountOfMemory
    Return data pointer for the amount of memory

  Status Codes Returned:
  EFI_SUCCESS           - Return value is valid
  EFI_UNSUPPORTED       - This function is not supported by this version of the driver
**/
EFI_STATUS
AmdGetTotalMemorySizeServiceCz (
  IN       EFI_PEI_SERVICES  **PeiServices,
     OUT   UINT32            *AmountOfMemory
  )
{
  return EFI_SUCCESS;
}

