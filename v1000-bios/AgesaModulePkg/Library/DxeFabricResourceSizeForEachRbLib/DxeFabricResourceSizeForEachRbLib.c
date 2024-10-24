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
#include "PiDxe.h"
#include "Filecode.h"
#include "AGESA.h"
#include <Library/AmdBaseLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Protocol/FabricResourceManagerServicesProtocol.h>

#define FILECODE LIBRARY_DXEFABRICRESOURCESIZEFOREACHRBLIB_DXEFABRICRESOURCESIZEFOREACHRBLIB_FILECODE

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

  VariableSize = sizeof (FABRIC_RESOURCE_FOR_EACH_RB);
  Status = gRT->GetVariable (
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
 * @retval       EFI_SUCCESS        Save user's request to NV variable successfully
 */
EFI_STATUS
FabricSetResourceSizeForEachRb (
  IN       FABRIC_RESOURCE_FOR_EACH_RB *CurrentUsed,
  IN       FABRIC_RESOURCE_FOR_EACH_RB *UserRequest
  )
{
  UINT8  i;
  UINT8  j;
  UINTN  VariableSize;
  EFI_STATUS Status;
  FABRIC_RESOURCE_FOR_EACH_RB CurrentRequest;

  // Calculate total size for each RootBridge
  for (i = 0; i < MAX_SOCKETS_SUPPORTED; i++) {
    for (j = 0; j < MAX_RBS_PER_SOCKET; j++) {
      CurrentRequest.NonPrefetchableMmioSizeAbove4G[i][j].Size = CurrentUsed->NonPrefetchableMmioSizeAbove4G[i][j].Size + UserRequest->NonPrefetchableMmioSizeAbove4G[i][j].Size;
      CurrentRequest.NonPrefetchableMmioSizeBelow4G[i][j].Size = CurrentUsed->NonPrefetchableMmioSizeBelow4G[i][j].Size + UserRequest->NonPrefetchableMmioSizeBelow4G[i][j].Size;
      CurrentRequest.PrefetchableMmioSizeAbove4G[i][j].Size = CurrentUsed->PrefetchableMmioSizeAbove4G[i][j].Size + UserRequest->PrefetchableMmioSizeAbove4G[i][j].Size;
      CurrentRequest.PrefetchableMmioSizeBelow4G[i][j].Size = CurrentUsed->PrefetchableMmioSizeBelow4G[i][j].Size + UserRequest->PrefetchableMmioSizeBelow4G[i][j].Size;

      CurrentRequest.NonPrefetchableMmioSizeAbove4G[i][j].Alignment = UserRequest->NonPrefetchableMmioSizeAbove4G[i][j].Alignment;
      CurrentRequest.NonPrefetchableMmioSizeBelow4G[i][j].Alignment = UserRequest->NonPrefetchableMmioSizeBelow4G[i][j].Alignment;
      CurrentRequest.PrefetchableMmioSizeAbove4G[i][j].Alignment = UserRequest->PrefetchableMmioSizeAbove4G[i][j].Alignment;
      CurrentRequest.PrefetchableMmioSizeBelow4G[i][j].Alignment = UserRequest->PrefetchableMmioSizeBelow4G[i][j].Alignment;

      CurrentRequest.IO[i][j].Size = CurrentUsed->IO[i][j].Size + UserRequest->IO[i][j].Size;

      CurrentRequest.PciBusNumber[i][j] = UserRequest->PciBusNumber[i][j];
    }
  }

  // Primary RootBridge's 2nd MMIO
  CurrentRequest.PrimaryRbSecondNonPrefetchableMmioSizeBelow4G.Alignment = UserRequest->PrimaryRbSecondNonPrefetchableMmioSizeBelow4G.Alignment;
  CurrentRequest.PrimaryRbSecondNonPrefetchableMmioSizeBelow4G.Size = CurrentUsed->PrimaryRbSecondNonPrefetchableMmioSizeBelow4G.Size + UserRequest->PrimaryRbSecondNonPrefetchableMmioSizeBelow4G.Size;
  CurrentRequest.PrimaryRbSecondPrefetchableMmioSizeBelow4G.Alignment = UserRequest->PrimaryRbSecondPrefetchableMmioSizeBelow4G.Alignment;
  CurrentRequest.PrimaryRbSecondPrefetchableMmioSizeBelow4G.Size = CurrentUsed->PrimaryRbSecondPrefetchableMmioSizeBelow4G.Size + UserRequest->PrimaryRbSecondPrefetchableMmioSizeBelow4G.Size;

  // Save user's request to NV
  VariableSize = sizeof (FABRIC_RESOURCE_FOR_EACH_RB);

  Status = gRT->SetVariable (
                  L"ResourceSizeForEachRb",
                  &gAmdResourceSizeForEachRbGuid,
                  EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_BOOTSERVICE_ACCESS,
                  VariableSize,
                  (VOID *) &CurrentRequest
                  );
  IDS_HDT_CONSOLE (CPU_TRACE, "  Set DF Resource size per RootBridge to NV variable, Status: %x\n", Status);

  return Status;
}

/*---------------------------------------------------------------------------------------*/
/**
 * FabricClearResourceSizeForEachRb
 *
 * Clear user's request about MMIO/IO size for each RootBridge to NV variable
 *
 *
 * @retval       EFI_SUCCESS        Clear NV variable
 */
EFI_STATUS
FabricClearResourceSizeForEachRb (
  )
{
  UINT8  Ignore;

  gRT->SetVariable (
                  L"ResourceSizeForEachRb",
                  &gAmdResourceSizeForEachRbGuid,
                  EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_BOOTSERVICE_ACCESS,
                  0,
                  (VOID *) &Ignore
                  );
  IDS_HDT_CONSOLE (CPU_TRACE, "  Clear DF Resource size per RootBridge to NV variable\n");

  return EFI_SUCCESS;
}

/*---------------------------------------------------------------------------------------*/
/**
 * FabricGetResourceDistribution
 *
 * Get the information about which RootBridge's MMIO should be put above Pcie Cfg
 *
 * @param[in, out]    ResourceDistribution       If it's above Pcie Cfg
 *
 */
EFI_STATUS
FabricGetResourceDistribution (
  IN       BOOLEAN   *ResourceDistribution
  )
{
  UINTN                             VariableSize;
  EFI_STATUS                        Status;

  VariableSize = sizeof (BOOLEAN) * MAX_SOCKETS_SUPPORTED * MAX_RBS_PER_SOCKET;
  Status = gRT->GetVariable (
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
 */
EFI_STATUS
FabricSetResourceDistribution (
  IN       BOOLEAN   *ResourceDistribution
  )
{
  UINTN                             VariableSize;
  EFI_STATUS                        Status;

  VariableSize = sizeof (BOOLEAN) * MAX_SOCKETS_SUPPORTED * MAX_RBS_PER_SOCKET;
  Status = gRT->SetVariable (
                  L"ResourceDistribution",
                  &gAmdResourceDistributionGuid,
                  EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_BOOTSERVICE_ACCESS,
                  VariableSize,
                  (VOID *) ResourceDistribution
                  );

  return Status;
}


