/* $NoKeywords:$ */
/**
 * @file
 *
 * AmdCoreLaunchServicePpi Implementation
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  AmdCoreLaunchServicePpi
 * @e \$Revision: 312065 $   @e \$Date: 2015-01-30 04:23:05 -0600 (Fri, 30 Jan 2015) $
 *
 */
/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */
#include <AMD.h>
#include <Ppi/AmdCoreTopologyServicesPpi.h>
#include <Ppi/NbioPcieServicesPpi.h>
#include <Library/PeiServicesLib.h>
#include <Library/PeiServicesTablePointerLib.h>
#include <Library/IdsLib.h>
#include <Library/NbioHandleLib.h>
#include <Library/GnbRegisterAccZpLib.h>
#include <Library/PcieConfigLib.h>
#include <Library/NbioSmuV10Lib.h>
#include <GnbRegistersRV.h>
#include <Filecode.h>

#define FILECODE        NBIO_SMU_AMDNBIOSMUV10PEI_AMDCORELAUNCHSERVICEPPI_FILECODE

/*----------------------------------------------------------------------------------------
 *                         E X T E R N   D E F I N I T I O N S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/**
 * This service retrieves information about the layout of the cores on the given die.
 *
 * @param[in]  This                           A pointer to the
 *                                            AMD_CORE_TOPOLOGY_SERVICES_PPI instance.
 * @param[in]  Socket                         Zero-based socket number to check.
 * @param[in]  Die                            The target die's identifier within Socket.
 * @param[out] NumberOfComplexes              Pointer to the number of enabled complexes on
 *                                            the given socket / die.
 * @param[out] NumberOfCores                  Pointer to the number of enabled cores per
 *                                            complex on the given socket / die.
 * @param[out] NumberOfThreads                Pointer to the number of enabled threads per
 *                                            core on the given socket / die.
 *
 * @retval EFI_SUCCESS                        The core topology information was successfully retrieved.
 * @retval EFI_INVALID_PARAMETER              Socket is non-existent.
 * @retval EFI_INVALID_PARAMETER              Die is non-existent.
 * @retval EFI_INVALID_PARAMETER              All output parameter pointers are NULL.
 *
 **/
EFI_STATUS
SmuV10GetCoreTopologyOnDie (
  IN       AMD_CORE_TOPOLOGY_SERVICES_PPI   *This,
  IN       UINTN                            Socket,
  IN       UINTN                            Die,
     OUT   UINTN                            *NumberOfComplexes,
     OUT   UINTN                            *NumberOfCores,
     OUT   UINTN                            *NumberOfThreads
  )
{
  EFI_PEI_SERVICES                **PeiServices;
  PEI_AMD_NBIO_PCIE_SERVICES_PPI  *PcieServicesPpi;
  PCIe_PLATFORM_CONFIG            *Pcie;
  EFI_STATUS                      Status;
  GNB_HANDLE                      *GnbHandle;
  AMD_CONFIG_PARAMS               *StdHeader;
  GNB_PCIE_INFORMATION_DATA_HOB   *PciePlatformConfigHob;
  PWR_01C_STRUCT                  ThreadConfiguration;

  IDS_HDT_CONSOLE (MAIN_FLOW, "SmuV10GetCoreTopologyOnDie Entry\n");
  GnbHandle = NULL;
  StdHeader = NULL;  ///@todo - do we need this?
  // Need topology structure
  PeiServices = (EFI_PEI_SERVICES **) GetPeiServicesTablePointer ();
  Status = (*PeiServices)->LocatePpi (
                             (CONST EFI_PEI_SERVICES**)PeiServices,
                             &gAmdNbioPcieServicesPpiGuid,
                             0,
                             NULL,
                             (VOID **)&PcieServicesPpi
                             );
  ASSERT (Status == EFI_SUCCESS);
  if (Status == EFI_SUCCESS) {
    PcieServicesPpi->PcieGetTopology (PcieServicesPpi, &PciePlatformConfigHob);
    Pcie = &(PciePlatformConfigHob->PciePlatformConfigHob);

    GnbHandle = NbioGetHandle (Pcie);
    while (GnbHandle != NULL) {
      //Find GnbHandle for this Socket/Die
      if ((Socket == GnbHandle->SocketId) && (Die == GnbHandle->DieNumber))  {
        break;
      }
      GnbHandle = GnbGetNextHandle (GnbHandle);
    }
    ASSERT (GnbHandle != NULL);
    if (GnbHandle != NULL) {
      Status = GnbRegisterReadZP (GnbHandle,
                                  THREAD_CONFIGURATION_TYPE,
                                  THREAD_CONFIGURATION_ADDRESS,
                                  &ThreadConfiguration,
                                  0,
                                  StdHeader);
      *NumberOfComplexes = ThreadConfiguration.Field.ComplexCount + 1;
      *NumberOfCores = ThreadConfiguration.Field.CoreCount + 1;
      *NumberOfThreads = (ThreadConfiguration.Field.SMTMode == 0) ? 2 : 1;
    } else {
      Status = EFI_INVALID_PARAMETER;
    }
  }

  IDS_HDT_CONSOLE (MAIN_FLOW, "SmuV10GetCoreTopologyOnDie Exit\n");
  return Status;
}

/**
 * This service will start a core to fetch its first instructions from the reset
 * vector.  This service may only be called from the BSP.
 *
 * @param[in]  This                           A pointer to the
 *                                            AMD_CORE_TOPOLOGY_SERVICES_PPI instance.
 * @param[in]  Socket                         Zero-based socket number of the target thread.
 * @param[in]  Die                            Zero-based die number within Socket of the target thread.
 * @param[in]  LogicalComplex                 Zero-based logical complex number of the target thread.
 * @param[in]  LogicalThread                  Zero-based logical thread number of the target thread.
 *
 * @retval EFI_SUCCESS                        The thread was successfully launched.
 * @retval EFI_DEVICE_ERROR                   The thread has already been launched.
 * @retval EFI_INVALID_PARAMETER              Socket is non-existent.
 * @retval EFI_INVALID_PARAMETER              Die is non-existent.
 * @retval EFI_INVALID_PARAMETER              LogicalComplex is non-existent.
 * @retval EFI_INVALID_PARAMETER              LogicalThread is non-existent.
 *
 **/
EFI_STATUS
SmuV10LaunchThread (
  IN       AMD_CORE_TOPOLOGY_SERVICES_PPI   *This,
  IN       UINTN                            Socket,
  IN       UINTN                            Die,
  IN       UINTN                            LogicalComplex,
  IN       UINTN                            LogicalThread
  )
{
  EFI_PEI_SERVICES                **PeiServices;
  PEI_AMD_NBIO_PCIE_SERVICES_PPI  *PcieServicesPpi;
  PCIe_PLATFORM_CONFIG            *Pcie;
  EFI_STATUS                      Status;
  GNB_HANDLE                      *GnbHandle;
  AMD_CONFIG_PARAMS               *StdHeader;
  GNB_PCIE_INFORMATION_DATA_HOB   *PciePlatformConfigHob;
  PWR_018_STRUCT                  ThreadEnable;
  PWR_01C_STRUCT                  ThreadConfiguration;
  UINTN                           NumberOfThreads;
  UINTN                           NumberOfCores;
  UINTN                           NumberOfLogicalThreads;
  UINT32                          SmuArg[6];

  IDS_HDT_CONSOLE (MAIN_FLOW, "SmuV10GetCoreTopologyOnDie Entry\n");
  GnbHandle = NULL;
  StdHeader = NULL;  ///@todo - do we need this?
  // Need topology structure
  PeiServices = (EFI_PEI_SERVICES **) GetPeiServicesTablePointer ();
  Status = (*PeiServices)->LocatePpi (
                             (CONST EFI_PEI_SERVICES**)PeiServices,
                             &gAmdNbioPcieServicesPpiGuid,
                             0,
                             NULL,
                             (VOID **)&PcieServicesPpi
                             );
  ASSERT (Status == EFI_SUCCESS);
  if (Status == EFI_SUCCESS) {
    PcieServicesPpi->PcieGetTopology (PcieServicesPpi, &PciePlatformConfigHob);
    Pcie = &(PciePlatformConfigHob->PciePlatformConfigHob);

    GnbHandle = NbioGetHandle (Pcie);
    while (GnbHandle != NULL) {
      //Find GnbHandle for this Socket/Die
      if ((Socket == GnbHandle->SocketId) && (Die == GnbHandle->DieNumber)) {
        break;
      }
      GnbHandle = GnbGetNextHandle (GnbHandle);
    }
    ASSERT (GnbHandle != NULL);
    if (GnbHandle != NULL) {
      Status = GnbRegisterReadZP (GnbHandle,
                                  THREAD_ENABLE_TYPE,
                                  THREAD_ENABLE_ADDRESS,
                                  &ThreadEnable,
                                  0,
                                  StdHeader);
      Status = GnbRegisterReadZP (GnbHandle,
                                  THREAD_CONFIGURATION_TYPE,
                                  THREAD_CONFIGURATION_ADDRESS,
                                  &ThreadConfiguration,
                                  0,
                                  StdHeader);
      NumberOfThreads = (ThreadConfiguration.Field.SMTMode == 0) ? 2 : 1;
      NumberOfCores = ThreadConfiguration.Field.CoreCount + 1;
      NumberOfLogicalThreads = NumberOfCores * NumberOfThreads;
      if ((LogicalComplex <= ThreadConfiguration.Field.ComplexCount) && (LogicalThread < NumberOfLogicalThreads)) {
        if ((ThreadEnable.Field.ThreadEn & (1 << (LogicalThread + (8 * LogicalComplex)))) == 0) {
          ThreadEnable.Field.ThreadEn |= 1 << (LogicalThread + (8 * LogicalComplex));
          Status = GnbRegisterWriteZP (GnbHandle,
                                       THREAD_ENABLE_TYPE,
                                       THREAD_ENABLE_ADDRESS,
                                       &ThreadEnable,
                                       0,
                                       StdHeader);
          NbioSmuServiceCommonInitArgumentsV10 (SmuArg);
          SmuArg[0] = 0xAA55AA55;
          Status = NbioSmuServiceRequestV10 (NbioGetHostPciAddress (GnbHandle), SMC_MSG_TestMessage, SmuArg, GNB_REG_ACC_FLAG_S3SAVE);
        } else {
          Status = EFI_DEVICE_ERROR;
        }
      } else {
        Status = EFI_INVALID_PARAMETER;
      }
    } else {
      Status = EFI_INVALID_PARAMETER;
    }
  }
  IDS_HDT_CONSOLE (MAIN_FLOW, "SmuV10GetCoreTopologyOnDie Exit\n");
  return Status;
}

/*----------------------------------------------------------------------------------------
 *                          P P I   D E S C R I P T O R
 *----------------------------------------------------------------------------------------
 */
AMD_CORE_TOPOLOGY_SERVICES_PPI mAmdCoreTopologyServicesPpi = {
  SmuV10GetCoreTopologyOnDie,
  SmuV10LaunchThread
};


