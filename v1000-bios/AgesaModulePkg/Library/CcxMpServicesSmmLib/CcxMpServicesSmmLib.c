/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD CCX MP Services DXE Library
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Lib
 * @e \$Revision: 312770 $   @e \$Date: 2015-02-11 13:24:42 +0800 (Wed, 11 Feb 2015) $
 *
 */
/*
 ******************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 **/

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include <PiDxe.h>
#include <Filecode.h>
#include <Library/AmdBaseLib.h>
#include <Library/CcxMpServicesLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/CcxRolesLib.h>
#include <Protocol/AmdCoreTopologyProtocol.h>
#include <Protocol/AmdCoreTopologyV2Protocol.h>
#include <Protocol/FabricTopologyServices2.h>


UINTN                      NumberOfThreadsPerCore  = 0;
UINTN                      NumberOfCoresPerComplex = 0;
UINTN                      NumberOfComplexesPerCcd = 0;
UINTN                      NumberOfCcdsPerDie      = 0;
UINTN                      NumberOfDiesPerSocket   = 0;
UINTN                      NumberOfSockets         = 0;


/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
#define FILECODE  LIBRARY_CCXMPSERVICESSMMLIB_CCXMPSERVICESSMMLIB_FILECODE


/*---------------------------------------------------------------------------------------*/
/**
 * Run function on selected APs
 *
 *
 * @param[in]  CoreSelector       core selector
 * @param[in]  RunThisFunction    function
 * @param[in]  Argument           argument
 * @param[in]  BlockingMode       BLOCKING_MODE     - execute in blocking mode. BSP waits until this AP finishes
 *                                NON_BLOCKING_MODE - execute in non-blocking mode. BPS won't wait for AP
 *                                WARNING: LIB instance for SMM won't support NonBlockingMode
 *
 * @retval     EFI_STATUS
 *
 */
EFI_STATUS
CcxRunFunctionOnAps (
  IN       CCX_MP_SERVICES_CORE_SELECTOR  CoreSelector,
  IN       EFI_AP_PROCEDURE               RunThisFunction,
  IN       VOID                          *Argument,
  IN       CCX_MP_SERVICES_BLOCKING_MODE  BlockingMode
  )
{
  UINTN       StartThisAp;
  UINTN       StartThisApFrom;
  UINTN       StartThisApTo;
  UINTN       StartThisApSteps;
  EFI_STATUS  Status;
  EFI_STATUS  CalledStatus;

  if (!CcxIsBsp (NULL)) {
    ASSERT (FALSE);
    return EFI_UNSUPPORTED;
  }

  if ((NumberOfSockets == 0) || (NumberOfDiesPerSocket == 0) || (NumberOfComplexesPerDie == 0) || (NumberOfCoresPerComplex == 0) || (NumberOfThreadsPerCore == 0)) {
    return EFI_INVALID_PARAMETER;
  }

  // Get StartThisApFrom, StartThisApTo, StartThisApSteps
  StartThisApTo = gSmst->NumberOfCpus;
  ASSERT (StartThisApTo == (NumberOfThreadsPerCore * NumberOfCoresPerComplex * NumberOfComplexesPerDie * NumberOfDiesPerSocket * NumberOfSockets));

  switch (CoreSelector) {
  case ALL_THREADS:
    StartThisApFrom = 1;
    StartThisApSteps = 1;
    break;
  case ALL_CORE_PRIMARY:
    StartThisApFrom = 0 + NumberOfThreadsPerCore;
    StartThisApSteps = StartThisApFrom;
    break;
  case ALL_COMPLEX_PRIMARY:
    StartThisApFrom = 0 + (NumberOfThreadsPerCore * NumberOfCoresPerComplex);
    StartThisApSteps = StartThisApFrom;
    break;
  case ALL_CCD_PRIMARY:
    StartThisApFrom = 0 + (NumberOfThreadsPerCore * NumberOfCoresPerComplex * NumberOfComplexesPerCcd);
    StartThisApSteps = StartThisApFrom;
    break;
  case ALL_DIE_PRIMARY:
    StartThisApFrom = 0 + (NumberOfThreadsPerCore * NumberOfCoresPerComplex * NumberOfComplexesPerCcd * NumberOfCcdsPerDie);
    StartThisApSteps = StartThisApFrom;
    break;
  case ALL_SOCKET_PRIMARY:
    StartThisApFrom = 0 + (NumberOfThreadsPerCore * NumberOfCoresPerComplex * NumberOfComplexesPerCcd * NumberOfCcdsPerDie * NumberOfDiesPerSocket);
    StartThisApSteps = StartThisApFrom;
    break;
  default:
    ASSERT (FALSE);
    return EFI_INVALID_PARAMETER;
  }

  // Run function on APs
  Status = EFI_SUCCESS;
  for (StartThisAp = StartThisApFrom; StartThisAp < StartThisApTo; StartThisAp += StartThisApSteps) {
    CalledStatus = gSmst->SmmStartupThisAp (RunThisFunction, StartThisAp, Argument);
    Status = (CalledStatus > Status) ? CalledStatus : Status;
  }

  return Status;
}


/*---------------------------------------------------------------------------------------*/
/**
 * CcxMpServicesSmmLibConstructor
 *
 * initial the global variable
 *
 */
EFI_STATUS
EFIAPI
CcxMpServicesSmmLibConstructor (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  UINTN       TotalNumberOfDie;
  EFI_STATUS  Status;
  EFI_STATUS  CalledStatus;
  AMD_CORE_TOPOLOGY_SERVICES_PROTOCOL    *CoreTopology;
  AMD_CORE_TOPOLOGY_SERVICES_V2_PROTOCOL *CoreTopology2;
  AMD_FABRIC_TOPOLOGY_SERVICES2_PROTOCOL *FabricTopology;

  Status = EFI_SUCCESS;
  // Get system topology information
  if ((NumberOfSockets == 0) || (NumberOfDiesPerSocket == 0) || (NumberOfComplexesPerDie == 0) || (NumberOfCoresPerComplex == 0) || (NumberOfThreadsPerCore == 0)) {
    CalledStatus = gBS->LocateProtocol (&gAmdFabricTopologyServices2ProtocolGuid, NULL, &FabricTopology);
    Status = (CalledStatus > Status) ? CalledStatus : Status;
    CalledStatus = gBS->LocateProtocol (&gAmdCoreTopologyServicesV2ProtocolGuid, NULL, &CoreTopology2);
    if (CalledStatus == EFI_SUCCESS) {
      CoreTopology2->GetCoreTopologyOnDie (CoreTopology2, 0, 0, &NumberOfCcdsPerDie, &NumberOfComplexesPerCcd, &NumberOfCoresPerComplex, &NumberOfThreadsPerCore);
    } else {
      CalledStatus = gBS->LocateProtocol (&gAmdCoreTopologyServicesProtocolGuid, NULL, &CoreTopology);
      NumberOfComplexesPerDie = 1;
      CoreTopology->GetCoreTopologyOnDie (CoreTopology, 0, 0, &NumberOfComplexesPerCcd, &NumberOfCoresPerComplex, &NumberOfThreadsPerCore);
    }

    Status = (CalledStatus > Status) ? CalledStatus : Status;

    if (Status == EFI_SUCCESS) {
      FabricTopology->GetSystemInfo (FabricTopology, &NumberOfSockets, &TotalNumberOfDie, NULL, NULL, NULL);
      NumberOfDiesPerSocket = TotalNumberOfDie / NumberOfSockets;
    }
  }
  return EFI_SUCCESS;
}


