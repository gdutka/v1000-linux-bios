/* $NoKeywords:$ */
/**
 * @file
 *
 * Get MMIO size for each Die
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Library
 * @e \$Revision$   @e \$Date$
 *
 */
/*
 ******************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */


/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include "Uefi.h"
#include "PiPei.h"
#include "Filecode.h"
#include "AGESA.h"
#include "Library/AmdBaseLib.h"
#include <Library/PeiServicesTablePointerLib.h>
#include <Protocol/FabricResourceManagerServicesProtocol.h>
#include <Ppi/ReadOnlyVariable2.h>

#define FILECODE LIBRARY_PEIFABRICRESOURCESIZEFOREACHRBLIB_PEIFABRICRESOURCESIZEFOREACHRBLIB_FILECODE

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*---------------------------------------------------------------------------------------*/
/**
 * FabricGetResourceSizeForEachRb
 *
 * Try to get user's request about MMIO/IO size for each RootBridge from NV variable
 *
 * @param[in, out]    ResourceSizeForEachRb     User's request about fabric resource size for each RootBridge
 *
 * @retval            EFI_SUCCESS               Get user's request from NV variable successfully
 */
EFI_STATUS
FabricGetResourceSizeForEachRb (
  IN       FABRIC_RESOURCE_FOR_EACH_RB *ResourceSizeForEachRb
  )
{
  UINTN                             VariableSize;
  EFI_STATUS                        Status;
  CONST EFI_PEI_SERVICES            **PeiServices;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI  *ReadVariable2;

  PeiServices = GetPeiServicesTablePointer();
  Status = (**PeiServices).LocatePpi (PeiServices, &gEfiPeiReadOnlyVariable2PpiGuid, 0, NULL, (VOID**)&ReadVariable2);
  ASSERT (Status == EFI_SUCCESS);

  VariableSize = sizeof (FABRIC_RESOURCE_FOR_EACH_RB);
  Status = ReadVariable2->GetVariable (
                      ReadVariable2,
                      L"ResourceSizeForEachRb",
                      &gAmdResourceSizeForEachRbGuid,
                      NULL,
                      &VariableSize,
                      (VOID *) ResourceSizeForEachRb
                      );
  return Status;
}

/*---------------------------------------------------------------------------------------*/
/**
 * FabricSetResourceSizeForEachRb
 *
 * Save user's request about MMIO/IO size for each RootBridge to NV variable
 *
 * @param[in]    CurrentUsed        Current used Mmio size for each RootBridge
 * @param[in]    UserRequest        User's request about Mmio size for each RootBridge
 *
 * @retval       EFI_UNSUPPORTED        Don't support this function during PEI
 */
EFI_STATUS
FabricSetResourceSizeForEachRb (
  IN       FABRIC_RESOURCE_FOR_EACH_RB *CurrentUsed,
  IN       FABRIC_RESOURCE_FOR_EACH_RB *UserRequest
  )
{
  return EFI_UNSUPPORTED;
}

/*---------------------------------------------------------------------------------------*/
/**
 * FabricClearResourceSizeForEachRb
 *
 * Clear user's request about MMIO/IO size for each RootBridge to NV variable
 *
 *
 * @retval       EFI_UNSUPPORTED        Don't support this function during PEI
 */
EFI_STATUS
FabricClearResourceSizeForEachRb (
  )
{
  return EFI_UNSUPPORTED;
}

/*---------------------------------------------------------------------------------------*/
/**
 * FabricGetResourceDistribution
 *
 * Get the information about which RootBridge's MMIO should be put above Pcie Cfg
 *
 * @param[in, out]    ResourceDistribution       If it's above Pcie Cfg
 *
 * @retval            EFI_UNSUPPORTED            Don't support this function during PEI
 */
EFI_STATUS
FabricGetResourceDistribution (
  IN       BOOLEAN   *ResourceDistribution
  )
{
  UINTN                             VariableSize;
  EFI_STATUS                        Status;
  CONST EFI_PEI_SERVICES            **PeiServices;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI  *ReadVariable2;

  PeiServices = GetPeiServicesTablePointer();
  Status = (**PeiServices).LocatePpi (PeiServices, &gEfiPeiReadOnlyVariable2PpiGuid, 0, NULL, (VOID**)&ReadVariable2);
  ASSERT (Status == EFI_SUCCESS); 

  VariableSize = sizeof (BOOLEAN) * MAX_SOCKETS_SUPPORTED * MAX_RBS_PER_SOCKET;
  Status = ReadVariable2->GetVariable (
                  ReadVariable2,
                  L"ResourceDistribution",
                  &gAmdResourceDistributionGuid,
                  NULL,
                  &VariableSize,
                  (VOID *) ResourceDistribution
                  );

  return Status;
}

/*---------------------------------------------------------------------------------------*/
/**
 * FabricSetResourceDistribution
 *
 * Set the information about which RootBridge's MMIO should be put above Pcie Cfg
 *
 * @param[in, out]    ResourceDistribution       If it's above Pcie Cfg
 *
 * @retval            EFI_UNSUPPORTED            Don't support this function during PEI
 */
EFI_STATUS
FabricSetResourceDistribution (
  IN       BOOLEAN   *ResourceDistribution
  )
{
  return EFI_UNSUPPORTED;
}


