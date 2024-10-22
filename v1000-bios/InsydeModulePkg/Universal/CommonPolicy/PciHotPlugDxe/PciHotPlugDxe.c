/** @file
  Statements that include other files

;******************************************************************************
;* Copyright (c) 2012 - 2020, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "PciHotPlugDxe.h"

//
// Instantiation of Driver private data.
//
PCIE_HOT_PLUG_DEVICE_PATH DefaultPcieHotPlugDevicePath [1] = {
  {
    ACPI(0x0),
    PCI(0x20, 0x8),
    END
  }
};

EFI_HPC_LOCATION          DefaultHpcLocation [1] = {
  {
    (EFI_DEVICE_PATH_PROTOCOL *) &DefaultPcieHotPlugDevicePath[0],
    (EFI_DEVICE_PATH_PROTOCOL *) &DefaultPcieHotPlugDevicePath[0]
  }
};

UINTN                       NumberOfHotplugBridge = 0;
EFI_HPC_LOCATION            *HpcLocationTbl;
PCIE_HOT_PLUG_DEVICE_PATH   *PcieHotPlugDevicePathTbl;
HOT_PLUG_BRIDGE_INFO        *mHotPlugBridgeInfoTable;


/**
  Entry point for the driver.

  This routine reads the PlatformType GPI on FWH and produces a protocol
  to be consumed by the chipset driver to effect those settings.


  @param[in] ImageHandle    The firmware allocated handle for the EFI image.
  @param[in] SystemTable    A pointer to the EFI System Table.

  @retval EFI_SUCCESS       The entry point is executed successfully.

**/
EFI_STATUS
EFIAPI
PciHotPlugEntryPoint (
  IN EFI_HANDLE                   ImageHandle,
  IN EFI_SYSTEM_TABLE             *SystemTable
  )
{
  EFI_STATUS            Status;
  PCI_HOT_PLUG_INSTANCE *PciHotPlug;

  //
  // OemServices
  //
  NumberOfHotplugBridge = 0;
  mHotPlugBridgeInfoTable = NULL;

  DEBUG ((DEBUG_INFO, "OemKernelServices Call: OemSvcGetHotplugBridgeInfo \n"));
  Status = OemSvcGetHotplugBridgeInfo (
             &mHotPlugBridgeInfoTable
             );
  DEBUG ((DEBUG_INFO, "OemKernelServices OemSvcGetHotplugBridgeInfo Status: %r\n", Status));

  if (!EFI_ERROR (Status)) {
    return Status;
  }
  if (FeaturePcdGet (PcdH2ODxeCpPciHpcGetBridgeInfoSupported)) {
    H2O_DXE_CP_PCI_HPC_GET_BRIDGE_INFO_DATA      PciHpcGetBridgeInfoData;

    PciHpcGetBridgeInfoData.Size                     = sizeof (H2O_DXE_CP_PCI_HPC_GET_BRIDGE_INFO_DATA);
    PciHpcGetBridgeInfoData.Status                   = H2O_CP_TASK_NORMAL;
    PciHpcGetBridgeInfoData.HotPlugBridgeInfoTable   = mHotPlugBridgeInfoTable;

    DEBUG ((DEBUG_INFO, "Checkpoint Trigger: %g\n", &gH2ODxeCpPciHpcGetBridgeInfoGuid));
    H2OCpTrigger (&gH2ODxeCpPciHpcGetBridgeInfoGuid, &PciHpcGetBridgeInfoData);
    DEBUG ((DEBUG_INFO, "Checkpoint Result: %x\n", PciHpcGetBridgeInfoData.Status));

    if (PciHpcGetBridgeInfoData.Status == H2O_CP_TASK_UPDATE) {
      mHotPlugBridgeInfoTable = PciHpcGetBridgeInfoData.HotPlugBridgeInfoTable;
    }
  }

  ProcessHotplugBridgeInfoTable (mHotPlugBridgeInfoTable);

  if (NumberOfHotplugBridge == 0) {
    return EFI_NOT_FOUND;
  }

  PciHotPlug = AllocatePool (sizeof (PCI_HOT_PLUG_INSTANCE));
  if (PciHotPlug == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // Initialize driver private data.
  //
  ZeroMem (PciHotPlug, sizeof (PCI_HOT_PLUG_INSTANCE));

  PciHotPlug->Signature = EFI_PCIHOTPLUG_DRIVER_PRIVATE_SIGNATURE;
  PciHotPlug->HotPlugInitProtocol.GetRootHpcList      = GetRootHpcList;
  PciHotPlug->HotPlugInitProtocol.InitializeRootHpc   = InitializeRootHpc;
  PciHotPlug->HotPlugInitProtocol.GetResourcePadding  = GetResourcePadding;

  Status = gBS->InstallProtocolInterface (
                  &PciHotPlug->Handle,
                  &gEfiPciHotPlugInitProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &PciHotPlug->HotPlugInitProtocol
                  );
  ASSERT_EFI_ERROR (Status);

  return EFI_SUCCESS;
}

VOID
ProcessHotplugBridgeInfoTable (
  IN HOT_PLUG_BRIDGE_INFO  *HotPlugBridgeInfoTable
)
{
  UINTN       Index;
  UINTN       Index2;

  Index = 0;
  if (HotPlugBridgeInfoTable == NULL) {
    return;
  }
  //
  // Calculate how many hot plug bridge we have.
  //
  while (HotPlugBridgeInfoTable[Index].NodeUID!= END_OF_TABLE) {
    if (HotPlugBridgeInfoTable[Index].NodeUID != IGNORE_DEVICE) {
      NumberOfHotplugBridge++;
    }
    Index++;
  }
  if (NumberOfHotplugBridge == 0) {
    //
    // There is no Hot plug bridge
    //
    return;
  }
  //
  // Prepare PcieHotPlugDevicePathTbl
  //
  PcieHotPlugDevicePathTbl = AllocatePool ((sizeof (PCIE_HOT_PLUG_DEVICE_PATH) * NumberOfHotplugBridge));
  if (PcieHotPlugDevicePathTbl == NULL) {
    //
    // Fail to allocate memory for table, treat as no hot plug bridge.
    //
    NumberOfHotplugBridge = 0;
    return;
  }
  //
  // Initial Table default
  //
  ZeroMem (PcieHotPlugDevicePathTbl, (sizeof (PCIE_HOT_PLUG_DEVICE_PATH) * NumberOfHotplugBridge));
  for (Index = 0; Index < NumberOfHotplugBridge; Index++) {
    CopyMem ((VOID*)&PcieHotPlugDevicePathTbl[Index].PciRootBridgeNode, (VOID*)&DefaultPcieHotPlugDevicePath[0].PciRootBridgeNode, sizeof (PCIE_HOT_PLUG_DEVICE_PATH));
  }
  //
  // Prepare HotPlugBridgeInfoTable
  //
  HpcLocationTbl = AllocatePool ((sizeof (EFI_HPC_LOCATION) * NumberOfHotplugBridge));
  if (HpcLocationTbl == NULL) {
    //
    // Fail to allocate memory for table, treat as no hot plug bridge.
    //
    NumberOfHotplugBridge = 0;
    return;
  }
  //
  // Initial Table default
  //
  ZeroMem (HpcLocationTbl, (sizeof (EFI_HPC_LOCATION) * NumberOfHotplugBridge));
  for (Index = 0; Index < NumberOfHotplugBridge; Index++) {
    CopyMem ((VOID*)&HpcLocationTbl[Index].HpcDevicePath, (VOID*)&DefaultHpcLocation[0].HpcDevicePath, sizeof (EFI_HPC_LOCATION));
  }
  //
  // Initial table content by HotPlugBridgeInfoTable
  //
  Index = 0;
  Index2 = 0;
  while (HotPlugBridgeInfoTable[Index2].NodeUID != END_OF_TABLE) {
    if (HotPlugBridgeInfoTable[Index2].NodeUID != IGNORE_DEVICE) {
      PcieHotPlugDevicePathTbl[Index].PciRootBridgeNode.UID = HotPlugBridgeInfoTable[Index2].NodeUID;
      PcieHotPlugDevicePathTbl[Index].PciRootPortNode.Device = HotPlugBridgeInfoTable[Index2].DevNum;
      PcieHotPlugDevicePathTbl[Index].PciRootPortNode.Function = HotPlugBridgeInfoTable[Index2].FuncNum;

      HpcLocationTbl[Index].HpcDevicePath = (EFI_DEVICE_PATH_PROTOCOL *)&PcieHotPlugDevicePathTbl[Index];
      HpcLocationTbl[Index].HpbDevicePath = (EFI_DEVICE_PATH_PROTOCOL *)&PcieHotPlugDevicePathTbl[Index];
      Index++;
    }
    Index2++;
  }

}

/**
  This procedure returns a list of Root Hot Plug controllers that require
  initialization during boot process


  @param[in] This            The pointer to the instance of the EFI_PCI_HOT_PLUG_INIT protocol.
  @param[in] HpcCount        The number of Root HPCs returned.
  @param[in] HpcList         The list of Root HPCs. HpcCount defines the number of elements in this list.

  @retval EFI_SUCCESS

**/
EFI_STATUS
EFIAPI
GetRootHpcList (
  IN EFI_PCI_HOT_PLUG_INIT_PROTOCOL    *This,
  OUT UINTN                            *HpcCount,
  OUT EFI_HPC_LOCATION                 **HpcList
  )
{

  *HpcCount = (UINTN) NumberOfHotplugBridge;
  *HpcList  = HpcLocationTbl;

  return EFI_SUCCESS;
}

/**
  This procedure Initializes one Root Hot Plug Controller
  This process may casue initialization of its subordinate buses

  @param[in] This            The pointer to the instance of the EFI_PCI_HOT_PLUG_INIT protocol.
  @param[in] HpcDevicePath   The Device Path to the HPC that is being initialized.
  @param[in] HpcPciAddress   The address of the Hot Plug Controller function on the PCI bus.
  @param[in] Event           The event that should be signaled when the Hot Plug Controller initialization is complete. Set to NULL if the caller wants to wait until the entire initialization process is complete. The event must be of the type EFI_EVT_SIGNAL.
  @param[in] HpcState        The state of the Hot Plug Controller hardware. The type EFI_Hpc_STATE is defined in section 3.1.

  @retval EFI_SUCCESS

**/
EFI_STATUS
EFIAPI
InitializeRootHpc (
  IN  EFI_PCI_HOT_PLUG_INIT_PROTOCOL      *This,
  IN  EFI_DEVICE_PATH_PROTOCOL            *HpcDevicePath,
  IN  UINT64                              HpcPciAddress,
  IN  EFI_EVENT                           Event, OPTIONAL
  OUT EFI_HPC_STATE                       *HpcState
  )
{
  if (Event) {
    gBS->SignalEvent (Event);
  }

  *HpcState = EFI_HPC_STATE_INITIALIZED;

  return EFI_SUCCESS;
}

/**
  Get start bus number according input unique ID.

  @param[in]  Uid              Unique ID that is required by ACPI.
  @param[out] BusStartNumber   The start bus number that this PCI root bridge resides.


  @retval EFI_SUCCESS             Find start bus number successfully.
  @retval EFI_INVALID_PARAMETER   BusStartNumber is NULL.
  @retval EFI_NOT_FOUND           Cannot find start bus number.
**/
STATIC
EFI_STATUS
GetBusStartNumberFromUid (
  IN    UINT32      Uid,
  OUT   UINT32      *BusStartNumber
  )
{
  EFI_STATUS                                           Status;
  UINTN                                                NoHandles;
  EFI_HANDLE                                           *Handles;
  UINTN                                                Index;
  ACPI_HID_DEVICE_PATH                                 *DevicePath;
  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL                      *PciRootBridgeIo;
  EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL     *PciHostBridge;
  EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR                    *BusNumberRanges;

  if (BusStartNumber == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiPciRootBridgeIoProtocolGuid,
                  NULL,
                  &NoHandles,
                  &Handles
                  );
  if (EFI_ERROR (Status)) {
    return EFI_NOT_FOUND;
  }

  for (Index = 0; Index < NoHandles; Index++) {
    Status = gBS->HandleProtocol (
                    Handles[Index],
                    &gEfiDevicePathProtocolGuid,
                    (VOID **)&DevicePath
                    );
    if (EFI_ERROR (Status) || DevicePathType (DevicePath) !=  ACPI_DEVICE_PATH ||
        DevicePathSubType (DevicePath) != ACPI_DP || DevicePath->UID != Uid) {
      continue;
    }
    Status = gBS->HandleProtocol (
                    Handles[Index],
                    &gEfiPciRootBridgeIoProtocolGuid,
                    (VOID **)&PciRootBridgeIo
                    );
    if (EFI_ERROR (Status)) {
      break;
    }
    Status = gBS->HandleProtocol (
                    PciRootBridgeIo->ParentHandle,
                    &gEfiPciHostBridgeResourceAllocationProtocolGuid,
                    (VOID **)&PciHostBridge
                    );
    if (EFI_ERROR (Status)) {
      break;
    }
    Status = PciHostBridge->StartBusEnumeration (
                              PciHostBridge,
                              Handles[Index],
                              &BusNumberRanges
                              );
    if (EFI_ERROR (Status) || BusNumberRanges == NULL) {
      Status = EFI_NOT_FOUND;
      break;
    }
    if (BusNumberRanges->Desc == ACPI_ADDRESS_SPACE_DESCRIPTOR && BusNumberRanges->ResType == ACPI_ADDRESS_SPACE_TYPE_BUS) {
      *BusStartNumber = (UINT32)BusNumberRanges->AddrRangeMin;
    } else {
      Status = EFI_NOT_FOUND;
    }
    FreePool (BusNumberRanges);
    break;
  }
  FreePool (Handles);
  return EFI_ERROR (Status) ? EFI_NOT_FOUND : EFI_SUCCESS;
}


/**
  Returns the resource padding required by the PCI bus that is controlled by the specified Hot Plug Controller.

  @param[in] This               The pointer to the instance of the EFI_PCI_HOT_PLUG_INIT protocol. initialized.
  @param[in] HpcDevicePath      The Device Path to the Hot Plug Controller.
  @param[in] HpcPciAddress      The address of the Hot Plug Controller function on the PCI bus.
  @param[in] HpcState           The state of the Hot Plug Controller hardware. The type EFI_HPC_STATE is defined in section 3.1.
  @param[in] Padding            This is the amount of resource padding required by the PCI bus under the control of the specified Hpc. Since the caller does not know the size of this buffer, this buffer is allocated by the callee and freed by the caller.
  @param[in] Attribute          Describes how padding is accounted for.

  @retval EFI_SUCCESS
**/
EFI_STATUS
EFIAPI
GetResourcePadding (
  IN  EFI_PCI_HOT_PLUG_INIT_PROTOCOL  *This,
  IN  EFI_DEVICE_PATH_PROTOCOL        *HpcDevicePath,
  IN  UINT64                          HpcPciAddress,
  OUT EFI_HPC_STATE                   *HpcState,
  OUT VOID                            **Padding,
  OUT EFI_HPC_PADDING_ATTRIBUTES      *Attributes
  )
{
  EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR *PaddingResource;
  EFI_STATUS                        Status;
  UINT64                            PciAddress;
  UINTN                             Index;
  BOOLEAN                           ResourcePaddingFound;
  BOOLEAN                           Above4GbMmioSupported;
  UINT8                             ReservedBusCount;
  UINT16                            ReservedIoRange;
  UINT64                            ReservedNonPrefetchableMmio;
  UINT64                            AlignmentOfReservedNonPrefetchableMmio;
  UINT64                            ReservedPrefetchableMmio;
  UINT64                            AlignmentOfReservedPrefetchableMmio;
  UINT32                            BusStartNumber;

  Index                                  = 0;
  ResourcePaddingFound                   = FALSE;
  ReservedBusCount                       = 0;
  ReservedIoRange                        = 0;
  ReservedNonPrefetchableMmio            = 0;
  AlignmentOfReservedNonPrefetchableMmio = 0;
  ReservedPrefetchableMmio               = 0;
  AlignmentOfReservedPrefetchableMmio    = 0;

  while (mHotPlugBridgeInfoTable[Index].NodeUID != END_OF_TABLE) {
    if (mHotPlugBridgeInfoTable[Index].NodeUID != IGNORE_DEVICE) {
      Status = GetBusStartNumberFromUid (mHotPlugBridgeInfoTable[Index].NodeUID, &BusStartNumber);
      if (EFI_ERROR (Status)) {
        return EFI_NOT_READY;
      }
      PciAddress = EFI_PCI_ADDRESS (BusStartNumber, mHotPlugBridgeInfoTable[Index].DevNum, mHotPlugBridgeInfoTable[Index].FuncNum, 0);
      if (HpcPciAddress == PciAddress) {
        ResourcePaddingFound                   = TRUE;
        ReservedBusCount                       = mHotPlugBridgeInfoTable[Index].ReservedBusCount;
        ReservedIoRange                        = mHotPlugBridgeInfoTable[Index].ReservedIoRange;
        ReservedNonPrefetchableMmio            = mHotPlugBridgeInfoTable[Index].ReservedNonPrefetchableMmio;
        AlignmentOfReservedNonPrefetchableMmio = mHotPlugBridgeInfoTable[Index].AlignemntOfReservedNonPrefetchableMmio;
        ReservedPrefetchableMmio               = mHotPlugBridgeInfoTable[Index].ReservedPrefetchableMmio;
        AlignmentOfReservedPrefetchableMmio    = mHotPlugBridgeInfoTable[Index].AlignemntOfReservedPrefetchableMmio;
        break;
      }
    }
    Index++;
  }

  Above4GbMmioSupported = FALSE;
  if (FeaturePcdGet (PcdH2ODxeCpPciHpcGetResourcePaddingSupported)) {
    H2O_DXE_CP_PCI_HPC_GET_RESOURCE_PADDING_DATA      PciHpcGetResourcePaddingData;

    PciHpcGetResourcePaddingData.Size                                   = sizeof (H2O_DXE_CP_PCI_HPC_GET_RESOURCE_PADDING_DATA);
    PciHpcGetResourcePaddingData.Status                                 = H2O_CP_TASK_NORMAL;
    PciHpcGetResourcePaddingData.Above4GbMmioSupported                  = Above4GbMmioSupported;
    PciHpcGetResourcePaddingData.ResourcePaddingFound                   = ResourcePaddingFound;
    PciHpcGetResourcePaddingData.PciAddress                             = HpcPciAddress;
    PciHpcGetResourcePaddingData.PciDevicePath                          = HpcDevicePath;
    PciHpcGetResourcePaddingData.ReservedBusCount                       = ReservedBusCount;
    PciHpcGetResourcePaddingData.ReservedIoRange                        = ReservedIoRange;
    PciHpcGetResourcePaddingData.ReservedNonPrefetchableMmio            = ReservedNonPrefetchableMmio;
    PciHpcGetResourcePaddingData.AlignmentOfReservedNonPrefetchableMmio = AlignmentOfReservedNonPrefetchableMmio;
    PciHpcGetResourcePaddingData.ReservedPrefetchableMmio               = ReservedPrefetchableMmio;
    PciHpcGetResourcePaddingData.AlignmentOfReservedPrefetchableMmio    = AlignmentOfReservedPrefetchableMmio;

    DEBUG ((DEBUG_INFO, "Checkpoint Trigger: %g\n", &gH2ODxeCpPciHpcGetResourcePaddingGuid));
    H2OCpTrigger (&gH2ODxeCpPciHpcGetResourcePaddingGuid, &PciHpcGetResourcePaddingData);
    DEBUG ((DEBUG_INFO, "Checkpoint Result: %x\n", PciHpcGetResourcePaddingData.Status));

    if (PciHpcGetResourcePaddingData.Status == H2O_CP_TASK_UPDATE) {
      Above4GbMmioSupported                  = PciHpcGetResourcePaddingData.Above4GbMmioSupported;
      ResourcePaddingFound                   = PciHpcGetResourcePaddingData.ResourcePaddingFound;
      ReservedBusCount                       = PciHpcGetResourcePaddingData.ReservedBusCount;
      ReservedIoRange                        = PciHpcGetResourcePaddingData.ReservedIoRange;
      ReservedNonPrefetchableMmio            = PciHpcGetResourcePaddingData.ReservedNonPrefetchableMmio;
      AlignmentOfReservedNonPrefetchableMmio = PciHpcGetResourcePaddingData.AlignmentOfReservedNonPrefetchableMmio;
      ReservedPrefetchableMmio               = PciHpcGetResourcePaddingData.ReservedPrefetchableMmio;
      AlignmentOfReservedPrefetchableMmio    = PciHpcGetResourcePaddingData.AlignmentOfReservedPrefetchableMmio;
    }
  }

  if (!ResourcePaddingFound) {
    return EFI_UNSUPPORTED;
  }

  PaddingResource = AllocatePool (4 * sizeof (EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR) + sizeof (EFI_ACPI_END_TAG_DESCRIPTOR));
  if (PaddingResource == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  *Padding = (VOID *) PaddingResource;

  //
  // Padding for bus
  //
  ZeroMem (PaddingResource, 4 * sizeof (EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR) + sizeof (EFI_ACPI_END_TAG_DESCRIPTOR));
  *Attributes                   = EfiPaddingPciBus;

  PaddingResource->Desc         = 0x8A;
  PaddingResource->Len          = 0x2B;
  PaddingResource->ResType      = ACPI_ADDRESS_SPACE_TYPE_BUS;
  PaddingResource->GenFlag      = 0x0;
  PaddingResource->SpecificFlag = 0;
  PaddingResource->AddrRangeMin = 0;
  PaddingResource->AddrRangeMax = 0;
  PaddingResource->AddrLen      = ReservedBusCount;
  //
  // Pad 0 Bus
  //
  // Padding for non-prefetchable memory
  //
  PaddingResource++;
  PaddingResource->Desc                 = 0x8A;
  PaddingResource->Len                  = 0x2B;
  PaddingResource->ResType              = ACPI_ADDRESS_SPACE_TYPE_MEM;
  PaddingResource->GenFlag              = 0x0;
  PaddingResource->AddrSpaceGranularity = Above4GbMmioSupported ? 64 : 32;
  PaddingResource->SpecificFlag         = 0;
  //
  // Pad non-prefetchable
  //
  PaddingResource->AddrRangeMin = 0;
  PaddingResource->AddrLen      = ReservedNonPrefetchableMmio;
  //
  // Force minimum 4KByte alignment for Virtualization technology for Directed I/O
  //
  if (PaddingResource->AddrLen < SIZE_4K) {
    PaddingResource->AddrRangeMax = SIZE_4K - 1;
  } else {
    PaddingResource->AddrRangeMax = AlignmentOfReservedNonPrefetchableMmio < SIZE_4K ? SIZE_4K - 1 : AlignmentOfReservedNonPrefetchableMmio - 1;
  }
  //
  // Padding for prefetchable memory
  //
  PaddingResource++;
  PaddingResource->Desc                 = 0x8A;
  PaddingResource->Len                  = 0x2B;
  PaddingResource->ResType              = ACPI_ADDRESS_SPACE_TYPE_MEM;
  PaddingResource->GenFlag              = 0x0;
  //PaddingResource->AddrSpaceGranularity = Above4GbMmioSupported ? 64 : 32;//Arista Modify
  PaddingResource->AddrSpaceGranularity = 64;  //Arista Modify
  PaddingResource->SpecificFlag         = 06;
  //
  // Pad prefetchable
  //
  PaddingResource->AddrRangeMin = 0;
  PaddingResource->AddrLen      = ReservedPrefetchableMmio;
  //
  // Force minimum 4KByte alignment for Virtualization technology for Directed I/O
  //
  if (PaddingResource->AddrLen < SIZE_4K) {
    PaddingResource->AddrRangeMax = SIZE_4K - 1;
  } else {
    PaddingResource->AddrRangeMax = AlignmentOfReservedPrefetchableMmio < SIZE_4K ? SIZE_4K - 1 : AlignmentOfReservedPrefetchableMmio - 1;
  }
  //
  // Padding for I/O
  //
  PaddingResource++;
  PaddingResource->Desc         = 0x8A;
  PaddingResource->Len          = 0x2B;
  PaddingResource->ResType      = ACPI_ADDRESS_SPACE_TYPE_IO;
  PaddingResource->GenFlag      = 0x0;
  PaddingResource->SpecificFlag = 0;
  PaddingResource->AddrRangeMin = 0;
  PaddingResource->AddrLen      = ReservedIoRange;
  //
  // Pad 4K of IO
  //
  PaddingResource->AddrRangeMax = ReservedIoRange / 0x1000;
  //
  // Alignment
  //
  // Terminate the entries.
  //
  PaddingResource++;
  ((EFI_ACPI_END_TAG_DESCRIPTOR *) PaddingResource)->Desc     = ACPI_END_TAG_DESCRIPTOR;
  ((EFI_ACPI_END_TAG_DESCRIPTOR *) PaddingResource)->Checksum = 0x0;

  *HpcState = EFI_HPC_STATE_INITIALIZED | EFI_HPC_STATE_ENABLED;

  return EFI_SUCCESS;
}
