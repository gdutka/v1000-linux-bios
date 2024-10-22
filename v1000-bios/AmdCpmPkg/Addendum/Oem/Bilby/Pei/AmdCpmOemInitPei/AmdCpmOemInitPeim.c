/*****************************************************************************
 *
 * Copyright (C) 2016-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 ******************************************************************************
 */

#include <AmdCpmPei.h>
#include <Library/IoLib.h>

#include "AmdCpmOemInitPeim.h"
//#include <Ppi/AmdFchInitPpi.h>
#include <Ppi/I2cMaster.h>
#include <Ppi/M24Lc128Ppi.h>
#include <Ppi/NbioPcieComplexPpi.h>
#include <Fch.h>

extern  AMD_CPM_PLATFORM_ID_TABLE             gCpmPlatformIdTable;
extern  AMD_CPM_PLATFORM_ID_CONVERT_TABLE     gCpmPlatformIdConvertTable;
extern  AMD_CPM_PRE_INIT_TABLE                gCpmPreInitTable;
extern  AMD_CPM_GPIO_INIT_TABLE               gCpmGpioInitTable;
extern  AMD_CPM_GEVENT_INIT_TABLE             gCpmGeventInitTable;
extern  AMD_CPM_GPIO_DEVICE_CONFIG_TABLE      gCpmGpioDeviceConfigTable;
extern  AMD_CPM_GPIO_DEVICE_DETECTION_TABLE   gCpmGpioDeviceDetectionTable;
extern  AMD_CPM_GPIO_DEVICE_RESET_TABLE       gCpmGpioDeviceResetTable;
extern  AMD_CPM_GPIO_DEVICE_POWER_TABLE       gCpmGpioDevicePowerTable;
extern  AMD_CPM_PCIE_CLOCK_TABLE              gCpmPcieClockTable;
extern  AMD_CPM_PCIE_CLOCK_TABLE              gCpmPcieClockTable_RV2;
extern  AMD_CPM_DXIO_TOPOLOGY_TABLE           gCpmDxioTopologyTable;
extern  AMD_CPM_DXIO_TOPOLOGY_TABLE           gCpmDxioTopologyTable_RV2;
extern  AMD_CPM_DEVICE_PATH_TABLE             gCpmDevicePathTable;
extern  AMD_CPM_DISPLAY_FEATURE_TABLE         gCpmDisplayFeatureTable;
extern  AMD_CPM_SAVE_CONTEXT_TABLE            gCpmSaveContextTable;
// Bilby doesn't support ZPODD and UCSI (SATA EXPRESS Card)
//extern  AMD_CPM_ZERO_POWER_ODD_TABLE          gCpmZeroPowerOddTable;
//extern  AMD_CPM_UCSI_TABLE                    gCpmUcsiTable;
//
EFI_STATUS
EFIAPI
AmdCpmOemInitPeimNotifyCallback (
  IN       EFI_PEI_SERVICES             **PeiServices,
  IN       EFI_PEI_NOTIFY_DESCRIPTOR    *NotifyDescriptor,
  IN       VOID                         *Ppi
  );

EFI_STATUS
EFIAPI
AmdCpmOemInitPpiTableNotifyCallback (
  IN       EFI_PEI_SERVICES             **PeiServices,
  IN       EFI_PEI_NOTIFY_DESCRIPTOR    *NotifyDescriptor,
  IN       VOID                         *Ppi
  );

EFI_STATUS
EFIAPI
AmdNbioPcieComplexPpiNotifyCallback(
  IN       EFI_PEI_SERVICES             **PeiServices,
  IN       EFI_PEI_NOTIFY_DESCRIPTOR    *NotifyDescriptor,
  IN       VOID                         *Ppi
);


STATIC EFI_PEI_NOTIFY_DESCRIPTOR  mCpmOemInitPeimNotify = {
  (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gEfiPeiSmbus2PpiGuid,
  AmdCpmOemInitPeimNotifyCallback
};

STATIC EFI_PEI_NOTIFY_DESCRIPTOR mCpmOemGpioInitPeimNotify = {
  (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gAmdCpmGpioInitFinishedPpiGuid,
  AmdCpmOemInitPpiTableNotifyCallback
};
STATIC EFI_PEI_NOTIFY_DESCRIPTOR mAmdNbioPcieComplexPpiNotify = {
  (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gAmdNbioPcieComplexPpiGuid,
  AmdNbioPcieComplexPpiNotifyCallback
};


EFI_STATUS
EFIAPI
AmdCpmTableOverride (
  IN       EFI_PEI_SERVICES       **PeiServices
  );

EFI_STATUS
EFIAPI
AmdCpmOverrideTableNotifyCallback (
  IN       EFI_PEI_SERVICES             **PeiServices,
  IN       EFI_PEI_NOTIFY_DESCRIPTOR    *NotifyDescriptor,
  IN       VOID                         *Ppi
  );

STATIC EFI_PEI_NOTIFY_DESCRIPTOR mCpmOemTableOverrideNotify = {
  (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gAmdCpmTablePpiGuid,
  AmdCpmOverrideTableNotifyCallback
};

//
// CPM Main Table
//
AMD_CPM_MAIN_TABLE gCpmMainTable = {
  {CPM_SIGNATURE_MAIN_TABLE, sizeof (gCpmMainTable) / sizeof (UINT8), 0, 0, 0, 0x0000000F},
  "Bilby",   // PlatformName
  0xFF,         // BiosType
  1,            // CurrentPlatformId
  0,            // PcieMemIoBaseAddr
  0,            // AcpiMemIoBaseAddr
  {NULL},         // Reserved for Internal Used
  {NULL},         // Reserved for Internal Used
  {NULL},         // Reserved for Internal Used
  {NULL},         // Reserved for Internal Used
  {NULL},         // Reserved for Internal Used
  {0x2200},       // DisplayFeature
                //  - IsBrightnessByDriver
                //  - SpecialPostIgpu
  0,            // ZeroPowerOddEn
  0,            // AcpiThermalFanEn
  0,            // ExtClkGen
  0,            // UnusedGppClkOffEn
  0,            // AdaptiveS4En
  0,            // WirelessButtonEn
  {0},            // Ec
  0,            // TdpLimitChangeEn
  0,            // SmiCheckToolEn
  0,            // LpcUartEn
  0,            // ProchotEn
  0,            // PtBrMemIoBaseAddr
  0,            // KbcSupport
  0,            // EcSupport
  0,            // UcsiEn
  0,            // ModernStandbyEn
  0,            // SensorFusionEn
  1,            // AcpPowerGatingEn
  0,            // SmbusIoBaseAddr
  0,            // SwSmiPort
  0,            // AcpClockGatingEn
  0,            // BxbBusNumDetEn
  0             // CpmCpuRevisionId
};

VOID *gCpmTableList[] = {
  &gCpmMainTable,

  &gCpmPlatformIdTable,
  &gCpmPlatformIdConvertTable,
  &gCpmPreInitTable,
  &gCpmSaveContextTable,
  &gCpmGpioInitTable,
  &gCpmGeventInitTable,
  &gCpmGpioDeviceConfigTable,
  &gCpmGpioDevicePowerTable,
  &gCpmGpioDeviceDetectionTable,
  &gCpmGpioDeviceResetTable,
  &gCpmPcieClockTable,
  &gCpmPcieClockTable_RV2,
  &gCpmDxioTopologyTable,
  &gCpmDxioTopologyTable_RV2,
  &gCpmDisplayFeatureTable,
  &gCpmDevicePathTable,
  //&gCpmAdaptiveS4Table,
  //&gCpmZeroPowerOddTable,
  //&gCpmUcsiTable,

  NULL
};

/*----------------------------------------------------------------------------------------*/
/**
 * Entry point of the AMD CPM OEM Init PEIM driver
 *
 * This function defines CPM OEM definition tables and installs AmdCpmOemTablePpi.
 * It also defines callback function to update these definition table on run time.
 *
 * @param[in]     FileHandle     Pointer to the firmware file system header
 * @param[in]     PeiServices    Pointer to Pei Services
 *
 * @retval        EFI_SUCCESS    Module initialized successfully
 * @retval        EFI_ERROR      Initialization failed (see error for more details)
 */
EFI_STATUS
EFIAPI
AmdCpmOemInitPeimEntryPoint (
  IN       CPM_PEI_FILE_HANDLE      FileHandle,
  IN       CPM_PEI_SERVICES         **PeiServices
  )
{
  EFI_STATUS                      Status;
#ifndef AGESA_V9
  EFI_PEI_NOTIFY_DESCRIPTOR       NotifyDescriptorV9;
  VOID                            *PpiV9 = NULL;
#endif

  DEBUG((DEBUG_INFO, "OEM-PEI-AmdCpmOemInitPeimEntryPoint-Start\n"));
  PcdSetBool (PcdVddOffVidCtrl, 1);
  PcdSet32 (PcdVddOffVid, 400);
#ifdef AGESA_V9
  DEBUG((DEBUG_INFO, "OEM-PEI-AmdCpmOemInitPeimEntryPoint-NotifyPpi-gEfiPeiSmbus2PpiGuid\n"));
  DEBUG((DEBUG_INFO, "OEM-PEI-AmdCpmOemInitPeimEntryPoint-For-AmdCpmOemInitPeimNotifyCallback\n"));
  Status = (**PeiServices).NotifyPpi (PeiServices, &mCpmOemInitPeimNotify);
#else
  DEBUG((DEBUG_INFO, "OEM-PEI-AmdCpmOemInitPeimEntryPoint-AmdCpmOemInitPeimNotifyCallback\n"));
  Status = AmdCpmOemInitPeimNotifyCallback ((EFI_PEI_SERVICES**)PeiServices,
                               &NotifyDescriptorV9,
                               PpiV9);
#endif
  if (EFI_ERROR (Status)) {
    DEBUG((DEBUG_INFO, "OEM-PEI-AmdCpmOemInitPeimEntryPoint-End-1-Status=%r\n", Status));
    return Status;
  }

#ifdef AGESA_V9
  Status = (**PeiServices).NotifyPpi (PeiServices, &mCpmOemGpioInitPeimNotify);
  if (EFI_ERROR (Status)) {
    DEBUG((DEBUG_INFO, "OEM-PEI-AmdCpmOemInitPeimEntryPoint, Notification of mCpmOemGpioInitPeimNotify failed, Status = %r\n", Status));
    return Status;
  }
  Status = (**PeiServices).NotifyPpi(PeiServices, &mAmdNbioPcieComplexPpiNotify);
  if (EFI_ERROR(Status)) {
    DEBUG((DEBUG_INFO, "OEM-PEI-AmdCpmOemInitPeimEntryPoint, Notification of mCpmOemGpioInitPeimNotify failed, Status = %r\n", Status));
    return Status;
  }
#endif

  DEBUG((DEBUG_INFO, "OEM-PEI-AmdCpmOemInitPeimEntryPoint-End-2-Status=%r\n", Status));
  return Status;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Callback Function of AMD CPM OEM Init PEIM driver
 *
 * @param[in]     PeiServices       The PEI core services table.
 * @param[in]     NotifyDescriptor  The descriptor for the notification event.
 * @param[in]     Ppi               Pointer to the PPI in question
 *
 * @retval        EFI_SUCCESS    Module initialized successfully
 * @retval        EFI_ERROR      Initialization failed (see error for more details)
 */
EFI_STATUS
EFIAPI
AmdCpmOemInitPeimNotifyCallback (
  IN  EFI_PEI_SERVICES                **PeiServices,
  IN  EFI_PEI_NOTIFY_DESCRIPTOR       *NotifyDescriptor,
  IN  VOID                            *Ppi
  )
{
  EFI_STATUS              Status;
  AMD_CPM_OEM_TABLE_PPI   *AmdCpmOemTablePpi;
  EFI_PEI_PPI_DESCRIPTOR  *PpiListCpmOemTable;

  DEBUG((DEBUG_INFO, "OEM-PEI-AmdCpmOemInitPeimNotifyCallback-Start\n"));
#ifdef AGESA_V9
  DEBUG((DEBUG_INFO, "OEM-PEI-AmdCpmOemInitPeimNotifyCallback-From-"));
  DEBUG((DEBUG_INFO, "AmdCpmOemInitPeimEntryPoint-NotifyPpi-gEfiPeiSmbus2PpiGuid\n"));
#endif
  RECORD_TIME_PEI (BeginAmdCpmOemInitPeimDriver);

  if (PcdGetBool (PcdLegacyFree) && PcdGetBool (PcdSerialIoDecode)) {
    UINT32  Address;
    UINT32  Data;
    Address = (UINT32)PcdGet64 (PcdPciExpressBaseAddress) + (0x0 << 20) + (0x14 << 15) + (0x3 << 12) + 0x44;
    Data = MmioRead32 (Address);
    switch (PcdGet64 (PcdSerialRegisterBase)) {
    case 0x3F8:
      Data |= BIT6;
      MmioWrite32 (Address, Data);
      break;
    case 0x2F8:
      Data |= BIT7;
      MmioWrite32 (Address, Data);
      break;
    case 0x2E8:
      Data |= BIT11;
      MmioWrite32 (Address, Data);
      break;
    case 0x3E8:
      Data |= BIT13;
      MmioWrite32 (Address, Data);
      break;
    }
  }

  DEBUG((DEBUG_INFO, "OEM-PEI-AmdCpmOemInitPeimNotifyCallback-LocatePpi-gAmdCpmOemTablePpiGuid\n"));
  Status = (*PeiServices)->LocatePpi (
                               (CPM_PEI_SERVICES**)PeiServices,
                               &gAmdCpmOemTablePpiGuid,
                               0,
                               NULL,
                               (VOID**)&AmdCpmOemTablePpi
                               );
  if (EFI_ERROR (Status)) {
    DEBUG((DEBUG_INFO, "OEM-PEI-AmdCpmOemInitPeimNotifyCallback-LocatePpi-gAmdCpmOemTablePpiGuid-Fail\n"));
    DEBUG((DEBUG_INFO, "OEM-PEI-AmdCpmOemInitPeimNotifyCallback-AllocatePool-AmdCpmOemTablePpi\n"));
    Status = (*PeiServices)->AllocatePool (
                             (CPM_PEI_SERVICES**)PeiServices,
                             sizeof (AMD_CPM_OEM_TABLE_PPI),
                             (VOID**)&AmdCpmOemTablePpi
                             );
    if (EFI_ERROR (Status)) {
      DEBUG((DEBUG_INFO, "OEM-PEI-AmdCpmOemInitPeimNotifyCallback-End-1-Status=%r\n", Status));
      return Status;
    }

    AmdCpmOemTablePpi->Revision = AMD_CPM_OEM_REVISION;
    AmdCpmOemTablePpi->PlatformId = AMD_CPM_PLATFORM_ID_DEFAULT;
    AmdCpmOemTablePpi->TableList = &gCpmTableList[0];

    DEBUG((DEBUG_INFO, "OEM-PEI-AmdCpmOemInitPeimNotifyCallback-AllocatePool-PpiListCpmOemTable\n"));
    Status = (*PeiServices)->AllocatePool (
                               (CPM_PEI_SERVICES**)PeiServices,
                               sizeof (EFI_PEI_PPI_DESCRIPTOR),
                               (VOID**)&PpiListCpmOemTable
                               );
    if (EFI_ERROR (Status)) {
      DEBUG((DEBUG_INFO, "OEM-PEI-AmdCpmOemInitPeimNotifyCallback-End-2-Status=%r\n", Status));
      return Status;
    }

    PpiListCpmOemTable->Flags = (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST);
    PpiListCpmOemTable->Guid = &gAmdCpmOemTablePpiGuid;
    PpiListCpmOemTable->Ppi = AmdCpmOemTablePpi;
    DEBUG((DEBUG_INFO, "OEM-PEI-AmdCpmOemInitPeimNotifyCallback-InstallPpi-gAmdCpmOemTablePpiGuid\n"));
    Status = (*PeiServices)->InstallPpi (
                                (CPM_PEI_SERVICES**)PeiServices,
                                PpiListCpmOemTable
                                );
    if (EFI_ERROR (Status)) {
      DEBUG((DEBUG_INFO, "OEM-PEI-AmdCpmOemInitPeimNotifyCallback-End-3-Status=%r\n", Status));
      return Status;
    }

    DEBUG((DEBUG_INFO, "OEM-PEI-AmdCpmOemInitPeimNotifyCallback-NotifyPpi-gAmdCpmTablePpiGuid\n"));
    DEBUG((DEBUG_INFO, "OEM-PEI-AmdCpmOemInitPeimNotifyCallback-For-AmdCpmOverrideTableNotifyCallback\n"));
    Status = (**PeiServices).NotifyPpi ((CPM_PEI_SERVICES**)PeiServices, &mCpmOemTableOverrideNotify);
  }

  RECORD_TIME_PEI (EndAmdCpmOemInitPeimDriver);

  DEBUG((DEBUG_INFO, "OEM-PEI-AmdCpmOemInitPeimNotifyCallback-End-4-Status=%r\n", Status));
  return Status;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Callback Function to Override CPM OEM Definition Tables
 *
 * @param[in]     PeiServices       The PEI core services table.
 * @param[in]     NotifyDescriptor  The descriptor for the notification event.
 * @param[in]     Ppi               Pointer to the PPI in question
 *
 * @retval        EFI_SUCCESS    Module initialized successfully
 * @retval        EFI_ERROR      Initialization failed (see error for more details)
 */
EFI_STATUS
EFIAPI
AmdCpmOverrideTableNotifyCallback (
  IN       EFI_PEI_SERVICES             **PeiServices,
  IN       EFI_PEI_NOTIFY_DESCRIPTOR    *NotifyDescriptor,
  IN       VOID                         *Ppi
  )
{
  EFI_STATUS                            Status;
  AMD_CPM_OVERRIDE_TABLE_PPI            *AmdCpmOverrideTablePpi;
  EFI_PEI_PPI_DESCRIPTOR                *PpiListCpmOverrideTablePtr;

  DEBUG((DEBUG_INFO, "OEM-PEI-AmdCpmOverrideTableNotifyCallback-Start\n"));
  DEBUG((DEBUG_INFO, "OEM-PEI-AmdCpmOverrideTableNotifyCallback-From-"));
  DEBUG((DEBUG_INFO, "AmdCpmOemInitPeimNotifyCallback-NotifyPpi-gAmdCpmTablePpiGuid\n"));
  RECORD_TIME_PEI (BeginAmdCpmOemTableOverride);

  DEBUG((DEBUG_INFO, "OEM-PEI-AmdCpmOverrideTableNotifyCallback-AmdCpmTableOverride\n"));
  Status = AmdCpmTableOverride (PeiServices);

  DEBUG((DEBUG_INFO, "OEM-PEI-AmdCpmOverrideTableNotifyCallback-AllocatePool-AmdCpmOverrideTablePpi\n"));
  Status = (*PeiServices)->AllocatePool (
                              (CPM_PEI_SERVICES**)PeiServices,
                              sizeof (AMD_CPM_OVERRIDE_TABLE_PPI),
                              (VOID**)&AmdCpmOverrideTablePpi
                              );
  if (EFI_ERROR (Status)) {
    DEBUG((DEBUG_INFO, "OEM-PEI-AmdCpmOverrideTableNotifyCallback-End-1-Status=%r\n", Status));
    return Status;
  }

  DEBUG((DEBUG_INFO, "OEM-PEI-AmdCpmOverrideTableNotifyCallback-AllocatePool-PpiListCpmOverrideTablePtr\n"));
  Status = (*PeiServices)->AllocatePool (
                              (CPM_PEI_SERVICES**)PeiServices,
                              sizeof (EFI_PEI_PPI_DESCRIPTOR),
                              (VOID**)&PpiListCpmOverrideTablePtr
                              );
  if (EFI_ERROR (Status)) {
    DEBUG((DEBUG_INFO, "OEM-PEI-AmdCpmOverrideTableNotifyCallback-End-2-Status=%r\n", Status));
    return Status;
  }

  PpiListCpmOverrideTablePtr->Flags   = (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST);
  PpiListCpmOverrideTablePtr->Guid    = &gAmdCpmOverrideTablePpiGuid;
  PpiListCpmOverrideTablePtr->Ppi     = AmdCpmOverrideTablePpi;

  DEBUG((DEBUG_INFO, "OEM-PEI-AmdCpmOverrideTableNotifyCallback-InstallPpi-gAmdCpmOverrideTablePpiGuid\n"));
  Status = (*PeiServices)->InstallPpi (
                              (CPM_PEI_SERVICES**)PeiServices,
                              PpiListCpmOverrideTablePtr
                              );
  if (EFI_ERROR (Status)) {
    DEBUG((DEBUG_INFO, "OEM-PEI-AmdCpmOverrideTableNotifyCallback-End-3-Status=%r\n", Status));
    return Status;
  }

  RECORD_TIME_PEI (EndAmdCpmOemTableOverride);
  DEBUG((DEBUG_INFO, "OEM-PEI-AmdCpmOverrideTableNotifyCallback-End-4-Status=%r\n", Status));
  return Status;
}


/**
 *
 * This function Programs MAC addresses to Ancillary Data Structure.
 *
 * @param[in]     FileHandle     Pointer to the firmware file system header
 * @param[in]     PeiServices    Pointer to Pei Services
 *
 * @retval        EFI_SUCCESS    Module initialized successfully
 * @retval        EFI_ERROR      Initialization failed (see error for more details)
 */

//Arista Modify
EFI_STATUS
EFIAPI
AmdXgbeMacAddr (
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
//  EFI_PEI_M24LC128_PPI            *M24Lc128Ppi;
  UINT8                            MacBuffer[12];
  UINT32                           MacAddrVal;
  UINT64                           Value;
  UINT8                           *MacPointer;
  DXIO_PORT_DESCRIPTOR            *EngineDescriptor;
  DXIO_COMPLEX_DESCRIPTOR         *PcieTopologyData;
  PEI_AMD_NBIO_PCIE_COMPLEX_PPI   *NbioPcieComplexPpi;
  EFI_STATUS  Status = EFI_SUCCESS;
  UINT32                           MacAddrValLow = 0;
  UINT32                           MacAddrValHigh = 0;

  UINT8 Index;
//  UINT8    IOMUXx13Value,IOMUXx14Value;

//  DEBUG((EFI_D_ERROR, "Inside AmdXgbeMacAddr\n"));
  DEBUG((EFI_D_INFO, "Inside AmdXgbeMacAddr\n"));

  // store the IOMUX values; later these values will be restored
//  IOMUXx13Value = MmioRead8(FCH_IOMUXx13_SCL1_I2C3_SCL_EGPIO19); 
//  IOMUXx14Value = MmioRead8(FCH_IOMUXx14_SDA1_I2C3_SDA_EGPIO20); 

  // Enable I2C3 CLK and Data
//  MmioWrite8(FCH_IOMUXx13_SCL1_I2C3_SCL_EGPIO19,1);   //Programming IOMUX to enable I2C3_SCL
//  MmioWrite8(FCH_IOMUXx14_SDA1_I2C3_SDA_EGPIO20,1);   //Programming IOMUX to enable I2C3_SDA

//  Status = (*PeiServices)->LocatePpi ((CPM_PEI_SERVICES**)PeiServices, &gM24Lc128PpiGuid, 0, NULL, (VOID**)&M24Lc128Ppi);
//  if (EFI_ERROR (Status)) {
//    DEBUG((EFI_D_ERROR, "Could not locate gM24Lc128PpiGuid\n"));
//    goto Exit;
//  }

//  Status = M24Lc128Ppi->Read (PeiServices, 3, 0x50, 0x4d, 12, MacBuffer);

  for (Index = 0; Index < 6; Index++) {
    MacBuffer[Index] = MmioRead8(0xFF000000 + Index); 
  }
  MacPointer = MacBuffer;
  MacAddrValLow = *((UINT32*) MacPointer);
  MacPointer += 4;
  MacAddrValHigh = *((UINT32*) MacPointer);
  MacAddrValHigh &= 0xFFFF;
  if ((MacAddrValHigh == 0 && MacAddrValLow == 0) || 
	(MacAddrValHigh == 0xffff && MacAddrValLow == 0xffffffff)) {
		MacAddrValLow = 0x9297c4d8;
		MacAddrValHigh = 0x8000b402;
  }
  for (Index = 6; Index < 11; Index++) {
    MacBuffer[Index] = MmioRead8(0xFF000000 + (Index - 6)); 
  }
  MacBuffer[11] = MacBuffer[5] + 1; //Increment by 1 for the second Ethernet controller MAC address

  MacPointer = MacBuffer;

  //for (Index = 0; Index < 12; Index++)
  //  DEBUG((EFI_D_INFO, "MacBuffer[%d] = %x\n", Index, MacBuffer[Index]));
  //
  //  Update the DXIO table MAC address
  //
  Status = (*PeiServices)->LocatePpi ((CPM_PEI_SERVICES**)PeiServices, &gAmdNbioPcieComplexPpiGuid, 0, NULL, (VOID**)&NbioPcieComplexPpi);
  if (EFI_ERROR (Status)) {
     DEBUG((EFI_D_ERROR, "Could not locate gAmdNbioPcieComplexPpiGuid\n"));
     goto Exit;
  }
  NbioPcieComplexPpi->PcieGetComplex(NbioPcieComplexPpi, &PcieTopologyData);

  EngineDescriptor = PcieTopologyData->PciePortList;
  while (EngineDescriptor != NULL) {
    if (EngineDescriptor->EngineData.EngineType == DxioEthernetEngine) {
        // Initialize this descriptor for Port 0
        MacAddrVal = *((UINT32*) MacPointer);
        EngineDescriptor->EtherNet.MacAddressLo = MacAddrValLow;  //Read the low value of the mac address here...;
        MacPointer += 4;
        MacAddrVal = *((UINT32*) MacPointer);
        MacAddrValHigh &= 0xFFFF;
        MacAddrValHigh |= 0x80000000;  // Set Valid
        EngineDescriptor->EtherNet.MacAddressHi = MacAddrValHigh;  //Read the high value of the mac address here...;
        MacPointer += 2;
    }
    EngineDescriptor = DxioInputParserGetNextDescriptor (EngineDescriptor);
  }
 //
 // Update Mac Address PCDs
 //
  MacPointer = MacBuffer;
  //Value = *((UINT32*) (MacPointer + 4));
  Value = MacAddrValHigh;
  //DEBUG((EFI_D_ERROR, "DEBUG1 = %x\n", Value));
  Value &= 0xFFFF;
  Value = (Value << 32);
  //DEBUG((EFI_D_ERROR, "DEBUG2 = %x\n", Value));
  //Value |=  *((UINT32*) MacPointer);
  Value |= MacAddrValLow;
  //DEBUG((EFI_D_ERROR, "DEBUG3 = %x\n", Value));

  PcdSet64(PcdXgbePort0MAC, Value);
  //DEBUG((EFI_D_ERROR, "PcdXgbePort0MAC = %x\n", Value));
  MacPointer +=6;

  Value = *((UINT32*) (MacPointer + 4));
  Value = MacAddrValHigh;
  Value &= 0xFFFF;
  Value =  (Value << 32);
  //Value |=  *((UINT32*) MacPointer);
  Value |= MacAddrValLow;
  PcdSet64(PcdXgbePort1MAC,Value);
  //DEBUG((EFI_D_ERROR, "PcdXgbePort0MAC-1 = %x\n", Value));

Exit:
  // Restore the IOMUX values
//  MmioWrite8(FCH_IOMUXx13_SCL1_I2C3_SCL_EGPIO19,IOMUXx13Value);   
//  MmioWrite8(FCH_IOMUXx14_SDA1_I2C3_SDA_EGPIO20,IOMUXx14Value); 

  return Status;
}



/**
 * TODO - Remove this code once all boards have MAC address programmed
 * This function Programs MAC addresses to Ancillary Data Structure.
 *
 * @param[in]     FileHandle     Pointer to the firmware file system header
 * @param[in]     PeiServices    Pointer to Pei Services
 *
 * @retval        EFI_SUCCESS    Module initialized successfully
 * @retval        EFI_ERROR      Initialization failed (see error for more details)
 */
/*
EFI_STATUS
EFIAPI
ProgramMacAddress (
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_PEI_M24LC128_PPI            *M24Lc128Ppi;
  UINT8 i;
  UINT8                            MacReadBuffer[12];
  UINT8                            MacBuffer[12] = {0x0,0x0,0x1A,0x1C,0x6E,0xC1,0x00,0x00,0x1A,0x1C,0x6E,0xC2};
  EFI_STATUS              Status = EFI_SUCCESS;

  DEBUG((EFI_D_ERROR, "Inside ProgramMacAddress\n"));
  ZeroMem(&MacReadBuffer, 12);

  Status = (*PeiServices)->LocatePpi (PeiServices, &gM24Lc128PpiGuid, 0, NULL, &M24Lc128Ppi);
  if (EFI_ERROR (Status)) {
     DEBUG((EFI_D_ERROR, " Locate gM24Lc128PpiGuid Failed %x,\n", Status));
  }
  Status = M24Lc128Ppi->Read(PeiServices, 3, 0x50, 0x4d, 12, MacReadBuffer);
  if (EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, " Read Failed %x,\n", Status));
  }
  DEBUG((EFI_D_ERROR, " Dumping I2C bus 3 Before Write: slave 0x50: address: 0x4D \n"));
  for (i=0; i<12;i++) {
    DEBUG((EFI_D_ERROR, "Byte [%x] : %x \n", i, MacReadBuffer[i]));
 }

  DEBUG((EFI_D_ERROR, "Writing\n"));
  for (i = 0; i<12; i++) {
    DEBUG((EFI_D_ERROR, "Byte [%x] : %x \n", i, MacBuffer[i]));
  }

  Status = M24Lc128Ppi->Write (PeiServices, 3, 0x50, 0x4d, 12, MacBuffer);
  if (EFI_ERROR (Status)) {
     DEBUG((EFI_D_ERROR, " Write Failed %x,\n", Status));
  }
  ZeroMem(&MacReadBuffer, 12);

   DEBUG((EFI_D_ERROR, "After Write Dumping I2C bus 3 : slave 0x50: address: 0x4D \n"));
  Status = M24Lc128Ppi->Read (PeiServices, 3, 0x50, 0x4d, 12, MacReadBuffer);
  for (i=0; i<12;i++) {
    DEBUG((EFI_D_ERROR, "Byte [%x] : %x \n", i, MacReadBuffer[i] ));
  }
  return Status;
}
*/

/**
 *
 * This function resets the external MDIO PHY.
 *
 * @param[in]     PeiServices    Pointer to Pei Services
 *
 * @retval        EFI_SUCCESS    PHY Reset complete
 * @retval        EFI_ERROR      PHY Reset failed.
 */
EFI_STATUS
EFIAPI
XgbeMdioPhyReset (
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  UINT16 Value = 0x0;
  EFI_STATUS Status = EFI_SUCCESS;
  AMD_CPM_TABLE_PPI* AmdCpmTablePpi = NULL;

  Status = (*PeiServices)->LocatePpi (
                           PeiServices,
                           &gAmdCpmTablePpiGuid,
                           0,
                           NULL,
                           (VOID**)&AmdCpmTablePpi
                           );
  if (EFI_ERROR (Status)) {
    DEBUG((DEBUG_INFO, "OEM-PEI-AmdCpmOemInitPpiTableNotifyCallback, XgbeMdioPhyReset, Status = %r\n", Status));
    return Status;
  }
  //
  // GPIO 27 is used to reset the external PHY. By default, this signal is driven High.  To start the PHY in a known
  // state, reset the PHY by driving this Low, and pull it back to High state.
  //
  Value = GPIO_CONTROL_REG(GPIO_FUNCTION_0, GPIO_OUTPUT_LOW, GPIO_PU_PD_DIS, GPIO_STICKY_DIS);
  DEBUG((DEBUG_INFO, "Value(Low) = %x\n", Value));
  AmdCpmTablePpi->CommonFunction.SetGpio(AmdCpmTablePpi, PHY_RESET_GPIO, Value);

  // wait for 10msec. This delay is recommended by Marvell.
  AmdCpmTablePpi->CommonFunction.Stall(AmdCpmTablePpi, PHY_RESET_DELAY);

  //
  // Pull reset to High state
  //
  Value = GPIO_CONTROL_REG(GPIO_FUNCTION_0, GPIO_OUTPUT_HIGH, GPIO_PU_PD_DIS, GPIO_STICKY_DIS);
  DEBUG((DEBUG_INFO, "Value(High) = %x\n", Value));
  AmdCpmTablePpi->CommonFunction.SetGpio(AmdCpmTablePpi, PHY_RESET_GPIO, Value);

  return Status;
}

/**
 *
 * This function configures the RFMUX to enable DP Ports 2 and 3.
 *
 * @param[in]     PeiServices    Pointer to Pei Services
 *
 * @retval        EFI_SUCCESS    Successfully Enabled DP Ports.
 * @retval        EFI_ERROR      Failed to enable DP Ports.
 */
EFI_STATUS
EFIAPI
ConfigureRfMuxforDPPorts (
  IN CONST EFI_PEI_SERVICES** PeiServices
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  USB_PD_PORT_CONTROL DpPort;
  AMD_CPM_TABLE_PPI* AmdCpmTablePpi = NULL;

  Status = (*PeiServices)->LocatePpi (
                           PeiServices,
                           &gAmdCpmTablePpiGuid,
                           0,
                           NULL,
                           (VOID**)&AmdCpmTablePpi
                           );
  if (EFI_ERROR (Status)) {
    DEBUG((DEBUG_INFO, "OEM-PEI-AmdCpmOemInitPpiTableNotifyCallback, ConfigureRfMuxforDPPorts, Status = %r\n", Status));
    return Status;
  }

  //
  // For DP ports 2 and 3 to work as "DP", RFMUX register needs to be programmed to select the
  // lanes to work as DP, since these are muxed with USB Type C. The RFMUX select bits for DP
  // are programmed according to the table defined below.
  //
  // ts0      ts1     us0     us1     tx1     tx2     rx1     rx2     Description
  //  0       0       0       0       safe    safe    safe    safe    Safe State1
  //  0       0       0       1       safe    usbtx   safe    usbrx   USB3.1 Mode (flip)
  //  0       0       1       0       usbtx   safe    usbrx   safe    USB3.1 Mode
  //  0       0       1       1       usbtx   usbtx   usbrx   usbrx   ATE mode
  //  0       1       0       0       safe    dp1     safe    dp0     Displayport x2 Mode
  //  0       1       1       0       usbtx   dp1     usbrx   dp0     Multi-Function Mode(Alt mode pin assignments D & F)
  //  1       0       0       0       dp2     safe    dp3     safe    Displayport x2 Mode
  //  1       0       0       1       dp2     usbtx   dp3     usbrx   Multi-Function Mode (flip)(Alt mode pin assignments D & F)
  //  1       1       0       0       dp2     dp1     dp3     dp0     Displayport x4 Mode2(Alt mode pin assignments C & E)

  // USB_PD_PORT0_CONTROL0 => Controls DP Port3
  // USB_PD_PORT1_CONTROL0 => Controls DP Port2
  DpPort.RegVal = AmdCpmTablePpi->CommonFunction.MmioRead32((UINTN)USB_PD_PORT0_CONTROL);
  DEBUG((DEBUG_INFO, "Default DP Port3 RFMUX RegVal = %x\n", DpPort.RegVal));

  DpPort.Bits.Usb_Pd_RfMux_Override = 0x1;
  DpPort.Bits.Ts0 = 0x1;
  DpPort.Bits.Ts1 = 0x1;
  DpPort.Bits.Us0 = 0x0;
  DpPort.Bits.Us1 = 0x0;

  DEBUG((DEBUG_INFO, "DP Port3 RFMUX RegVal = %x\n", DpPort.RegVal));

  AmdCpmTablePpi->CommonFunction.MmioWrite32((UINTN)USB_PD_PORT0_CONTROL, DpPort.RegVal);

  DpPort.RegVal = AmdCpmTablePpi->CommonFunction.MmioRead32((UINTN)USB_PD_PORT1_CONTROL);

  DEBUG((DEBUG_INFO, "Default DP Port2 RFMUX RegVal = %x\n", DpPort.RegVal));

  DpPort.Bits.Usb_Pd_RfMux_Override = 0x1;
  DpPort.Bits.Ts0 = 0x1;
  DpPort.Bits.Ts1 = 0x1;
  DpPort.Bits.Us0 = 0x0;
  DpPort.Bits.Us1 = 0x0;

  DEBUG((DEBUG_INFO, "DP Port2 RFMUX RegVal = %x\n", DpPort.RegVal));

  AmdCpmTablePpi->CommonFunction.MmioWrite32((UINTN)USB_PD_PORT1_CONTROL, DpPort.RegVal);
  return Status;
}

/**
 *
 * This function Changes the default functions of GPIOs 19 and 20 from I2C to SMBUS.
 *
 * @param[in]     PeiServices    Pointer to Pei Services
 *
 * @retval        EFI_SUCCESS    Successfully Enabled DP Ports.
 * @retval        EFI_ERROR      Failed to enable DP Ports.
 */
EFI_STATUS
EFIAPI
ProgramI2CGpioToSmbus (
  IN CONST EFI_PEI_SERVICES** PeiServices
  )
{

  UINT16 Value = 0x0;
  EFI_STATUS Status = EFI_SUCCESS;
  AMD_CPM_TABLE_PPI* AmdCpmTablePpi = NULL;

  Status = (*PeiServices)->LocatePpi (
                           PeiServices,
                           &gAmdCpmTablePpiGuid,
                           0,
                           NULL,
                           (VOID**)&AmdCpmTablePpi
                           );
  if (EFI_ERROR (Status)) {
    DEBUG((DEBUG_INFO, "OEM-PEI-AmdCpmOemInitPpiTableNotifyCallback, ProgramI2CGpioToSmbus, Status = %r\n", Status));
    return Status;
  }

  Value = GPIO_CONTROL_REG(GPIO_FUNCTION_0, GPIO_NA, GPIO_PU_EN, GPIO_STICKY_DIS);
  DEBUG((DEBUG_INFO, "GPIO 19 Value = %x\n", Value));
  AmdCpmTablePpi->CommonFunction.SetGpio(AmdCpmTablePpi, I2C_SMBUS_GPIO_19, Value);

  Value = GPIO_CONTROL_REG(GPIO_FUNCTION_0, GPIO_NA, GPIO_PU_EN, GPIO_STICKY_DIS);
  DEBUG((DEBUG_INFO, "GPIO 20 Value = %x\n", Value));
  AmdCpmTablePpi->CommonFunction.SetGpio(AmdCpmTablePpi, I2C_SMBUS_GPIO_20, Value);

  return Status;
}

/**
 *
 * Callback  function to execute when gAmdCpmGpioInitFinishedPpiGuid is published.
 * This PPI is published when the default GPIO programming and its associated functionality
 * as defined in the OemTable is completed.
 *
 * @param[in]     PeiServices    Pointer to Pei Services
 *
 * @retval        EFI_SUCCESS    Successfully Enabled DP Ports.
 * @retval        EFI_ERROR      Failed to enable DP Ports.
 */
EFI_STATUS
EFIAPI
AmdNbioPcieComplexPpiNotifyCallback(
  IN  EFI_PEI_SERVICES                **PeiServices,
  IN  EFI_PEI_NOTIFY_DESCRIPTOR       *NotifyDescriptor,
  IN  VOID                            *Ppi
)
{
  EFI_STATUS Status = EFI_SUCCESS;

  DEBUG((DEBUG_INFO, "In AmdNbioPcieComplexPpiNotifyCallback\n"));

  //ProgramMacAddress(PeiServices);
  Status = AmdXgbeMacAddr((CPM_PEI_SERVICES**)PeiServices);
  return Status;
}

EFI_STATUS
EFIAPI
AmdCpmOemInitPpiTableNotifyCallback (
  IN  EFI_PEI_SERVICES                **PeiServices,
  IN  EFI_PEI_NOTIFY_DESCRIPTOR       *NotifyDescriptor,
  IN  VOID                            *Ppi
  )
{
  EFI_STATUS Status = EFI_SUCCESS;

  DEBUG((DEBUG_INFO, "In AmdCpmOemInitPpiTableNotifyCallback\n"));
  RECORD_TIME_PEI (BeginAmdCpmOemInitPeimDriver);

#if 0
  //
  // The FCH code enables these GPIOs to function as I2C rather than SMBUS. If we want to switch to SMBUS
  // functionality, we need to disable the I2C 3 in the FchRTDeviceEnableMap which disables the visibilty
  // from OS. Currently keeping the functionality of these GPIO pins to SMBUS as of now.
  //
  //
  // Change GPIO19 and GPIO20 to default functionality (Function 0) after we are done with reading the EEPROM.
  //
  Status = ProgramI2CGpioToSmbus(PeiServices);
  if (EFI_ERROR(Status)) {
    DEBUG((DEBUG_INFO, "AmdCpmOemInitPpiTableNotifyCallback, ProgramI2CGpioToSmbus failed, Status = %r\n", Status));
    return Status;
  }
#endif

  Status = XgbeMdioPhyReset((CPM_PEI_SERVICES**)PeiServices);
  if (EFI_ERROR(Status)) {
    DEBUG((DEBUG_INFO, "AmdCpmOemInitPpiTableNotifyCallback, XgbeMdioPhyReset failed, Status = %r\n", Status));
    return Status;
  }

  Status = ConfigureRfMuxforDPPorts((CPM_PEI_SERVICES**)PeiServices);
  if (EFI_ERROR(Status)) {
    DEBUG((DEBUG_INFO, "AmdCpmOemInitPpiTableNotifyCallback, ConfigureRfMuxforDPPorts failed, Status = %r\n", Status));
    return Status;
  }

  RECORD_TIME_PEI (EndAmdCpmOemInitPeimDriver);

  DEBUG((DEBUG_INFO, "AmdCpmOemInitPpiTableNotifyCallback, Status = %r\n", Status));
  return Status;
}


