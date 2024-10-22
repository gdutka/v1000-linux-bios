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

#define FILECODE LIBRARY_MEM_SERVICESLIB_AMDMEMSERVICESLIB_FILECODE

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
  Read Service for the Memory Above 4GB

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
AmdGetMemoryAbove4gbService (
  IN       EFI_PEI_SERVICES  **PeiServices,
     OUT   UINT32            *AmountOfMemory
  )
{
  PEI_MEMORY_FAMILY_SERVICE_PPI *AmdFamilyServicesPtr;
  EFI_STATUS Status;
  Status = (*PeiServices)->LocatePpi (PeiServices,
                                      &gAmdMemoryFamilyServicesPpiGuid,
                                      0,
                                      NULL,
                                      &AmdFamilyServicesPtr);
  if (EFI_SUCCESS != Status) {
    return Status;
  }
  Status = AmdFamilyServicesPtr->AmdGetMemoryAbove4gbFamilyService (PeiServices, AmountOfMemory);
  return EFI_SUCCESS;
}
/**
  Read Service for the Memory Below 4GB

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
AmdGetMemoryBelow4gbService (
  IN       EFI_PEI_SERVICES  **PeiServices,
     OUT   UINT32            *AmountOfMemory
  )
{
  PEI_MEMORY_FAMILY_SERVICE_PPI *AmdFamilyServicesPtr;
  EFI_STATUS Status;
  Status = (*PeiServices)->LocatePpi (PeiServices,
                                      &gAmdMemoryFamilyServicesPpiGuid,
                                      0,
                                      NULL,
                                      &AmdFamilyServicesPtr);
  if (EFI_SUCCESS != Status) {
    return Status;
  }
  Status = AmdFamilyServicesPtr->AmdGetMemoryBelow4gbFamilyService (PeiServices, AmountOfMemory);
  return EFI_SUCCESS;
}
/**
  Read Service for the memory Below 1TB

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
AmdGetMemoryBelow1TbService (
  IN       EFI_PEI_SERVICES  **PeiServices,
     OUT   UINT32            *AmountOfMemory
  )
{
  PEI_MEMORY_FAMILY_SERVICE_PPI *AmdFamilyServicesPtr;
  EFI_STATUS Status;
  Status = (*PeiServices)->LocatePpi (PeiServices,
                                      &gAmdMemoryFamilyServicesPpiGuid,
                                      0,
                                      NULL,
                                      &AmdFamilyServicesPtr);
  if (EFI_SUCCESS != Status) {
    return Status;
  }
  Status = AmdFamilyServicesPtr->AmdGetMemoryBelow1TbFamilyService (PeiServices, AmountOfMemory);
  return EFI_SUCCESS;
}
/**
  Read Service for the Bottom of MMIO

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
AmdGetBottomIoService (
  IN       EFI_PEI_SERVICES  **PeiServices,
     OUT   UINT32            *AmountOfMemory
  )
{
  PEI_MEMORY_FAMILY_SERVICE_PPI *AmdFamilyServicesPtr;
  EFI_STATUS Status;
  Status = (*PeiServices)->LocatePpi (PeiServices,
                                      &gAmdMemoryFamilyServicesPpiGuid,
                                      0,
                                      NULL,
                                      &AmdFamilyServicesPtr);
  if (EFI_SUCCESS != Status) {
    return Status;
  }
  Status = AmdFamilyServicesPtr->AmdGetBottomIo1TbFamilyService (PeiServices, AmountOfMemory);
  return EFI_SUCCESS;
}
/**
  Read Service for the total amount of memory in the system

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
AmdGetTotalMemorySizeService (
  IN       EFI_PEI_SERVICES  **PeiServices,
     OUT   UINT32            *AmountOfMemory
  )
{
  PEI_MEMORY_FAMILY_SERVICE_PPI *AmdFamilyServicesPtr;
  EFI_STATUS Status;
  Status = (*PeiServices)->LocatePpi (PeiServices,
                                      &gAmdMemoryFamilyServicesPpiGuid,
                                      0,
                                      NULL,
                                      &AmdFamilyServicesPtr);
  if (EFI_SUCCESS != Status) {
    return Status;
  }
  Status = AmdFamilyServicesPtr->AmdGetTotalMemorySize4gbFamilyService (PeiServices, AmountOfMemory);
  return EFI_SUCCESS;
}
/**
  Read Service for the Memory Frequency of a channel

  Parameters:
  PeiServices
    A pointer to the PEI services
    Channel
       A pointer to the channel to read
  Frequency
      Return data pointer for a channel frequency

  Status Codes Returned:
  EFI_SUCCESS           - Return value is valid
  EFI_UNSUPPORTED       - This function is not supported by this version of the driver
**/
EFI_STATUS
AmdGetMemoryFrequencyService (
  IN       EFI_PEI_SERVICES  **PeiServices,
     OUT   UINT32            Channel,
     OUT   UINT32            *Frequency
  )
{
  return EFI_SUCCESS;
}
/**
  Read Service for the Memory Frequency of a channel

  Parameters:
  PeiServices
    A pointer to the PEI services
   UmaInfo
      Return data pointer for the UmaInfo

  Status Codes Returned:
  EFI_SUCCESS           - Return value is valid
  EFI_UNSUPPORTED       - This function is not supported by this version of the driver
**/
EFI_STATUS
AmdGetUmaInfoService (
  IN       EFI_PEI_SERVICES  **PeiServices,
     OUT   UINT32            *UmaInfo
  )
{
  return EFI_SUCCESS;
}

