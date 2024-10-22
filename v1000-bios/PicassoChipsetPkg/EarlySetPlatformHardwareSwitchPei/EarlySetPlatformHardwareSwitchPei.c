/** @file

;******************************************************************************
;* Copyright (c) 2017 - 2018, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <PiPei.h>

#include <Library/PeiServicesLib.h>
#include <Library/DebugLib.h>
#include <Library/HobLib.h>
#include <Library/ChipsetConfigLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PeiOemSvcKernelLib.h>
#include <Ppi/ReadOnlyVariable2.h>
#include <Guid/PlatformHardwareSwitch.h>
#include <AmdCpmPei.h>

EFI_STATUS
EFIAPI
PciePlatformCustomizePei (
  IN EFI_PEI_SERVICES                **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR       *NotifyDescriptor,
  IN VOID                            *Ppi
  );

EFI_STATUS
EFIAPI
UsbPlatformCustomizePei (
  IN EFI_PEI_SERVICES                **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR       *NotifyDescriptor,
  IN VOID                            *Ppi
  );

EFI_STATUS
EFIAPI
SetPlatformHardwareSwitchNotifyCallback (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  );


EFI_PEI_NOTIFY_DESCRIPTOR      mPlatformCustomizeNotifyPpiList[] = {
  {
    (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gAmdCpmOverrideTablePpiGuid,
    PciePlatformCustomizePei
  }
};

EFI_PEI_NOTIFY_DESCRIPTOR      mUsb3PcdNotifyPpiList[] = {
  {
    (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gAmdCbsHookPpiGuid,
    UsbPlatformCustomizePei
  }
};

STATIC EFI_PEI_NOTIFY_DESCRIPTOR      mNotifyPpiList[] = {
  {
    (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gEfiPeiReadOnlyVariable2PpiGuid,
    SetPlatformHardwareSwitchNotifyCallback
  }
};
/**
 * Set PCI-E PlatformHardwareSwitch
 *
 *
 *
 * @param[in]  PeiServices       Pointer to EFI_PEI_SERVICES pointer
 * @param[in]  NotifyDescriptor  NotifyDescriptor pointer
 * @param[in]  Ppi               Ppi pointer
 * @retval     EFI_STATUS
 */
EFI_STATUS
EFIAPI
PciePlatformCustomizePei (
  IN EFI_PEI_SERVICES                **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR       *NotifyDescriptor,
  IN VOID                            *Ppi
  )
{
  EFI_STATUS                         Status;
  AMD_CPM_TABLE_PPI                  *AmdCpmTablePpi;
  AMD_CPM_DXIO_TOPOLOGY_TABLE        *DxioTopologyTablePtr;
  DXIO_PORT_DESCRIPTOR               *DxioPort;
  VOID                               *PlatformHardwareSwitchBuffer;
  PLATFORM_HARDWARE_SWITCH           *PlatformHardwareSwitch;

  Status = (*PeiServices)->LocatePpi (
                           (CONST EFI_PEI_SERVICES**)PeiServices,
                           &gAmdCpmTablePpiGuid,
                           0,
                           NULL,
                           (VOID**)&AmdCpmTablePpi
                           );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  DxioTopologyTablePtr = AmdCpmTablePpi->CommonFunction.GetTablePtr (AmdCpmTablePpi, CPM_SIGNATURE_DXIO_TOPOLOGY);

  // Device control
  DxioPort = &(DxioTopologyTablePtr->Port[0]);

  PlatformHardwareSwitchBuffer = GetFirstGuidHob (&gPlatformHardwareSwitchGuid);

  //
  // APU GPP PCIe
  //
  if (PlatformHardwareSwitchBuffer != NULL) {
    PlatformHardwareSwitch = (PLATFORM_HARDWARE_SWITCH *) GET_GUID_HOB_DATA (PlatformHardwareSwitchBuffer);

    // GPP 0
    if (!PlatformHardwareSwitch->PciePortSwitch.Config.Port00En) {
      DxioPort[0].Port.PortPresent = DxioPortDisabled;
    }

    // GPP 1
    if (!PlatformHardwareSwitch->PciePortSwitch.Config.Port01En) {
      DxioPort[2].Port.PortPresent = DxioPortDisabled;
    }

    // GPP 2
    if (!PlatformHardwareSwitch->PciePortSwitch.Config.Port02En) {
      DxioPort[3].Port.PortPresent = DxioPortDisabled;
    }

    // GPP 3
    if (!PlatformHardwareSwitch->PciePortSwitch.Config.Port03En) {
      DxioPort[4].Port.PortPresent = DxioPortDisabled;
    }

    // GPP 4
    if (!PlatformHardwareSwitch->PciePortSwitch.Config.Port04En) {
      DxioPort[5].Port.PortPresent = DxioPortDisabled;
    }

    // GPP 5
    if (!PlatformHardwareSwitch->PciePortSwitch.Config.Port05En) {
      DxioPort[6].Port.PortPresent = DxioPortDisabled;
    }

    // GPP 6
    if (!PlatformHardwareSwitch->PciePortSwitch.Config.Port06En) {
      DxioPort[1].Port.PortPresent = DxioPortDisabled;
    }
  }
  return EFI_SUCCESS;
}
/**
 * Set XHCI PlatformHardwareSwitch to PCD
 *
 *
 *
 * @param[in]  PeiServices       Pointer to EFI_PEI_SERVICES pointer
 * @param[in]  NotifyDescriptor  NotifyDescriptor pointer
 * @param[in]  Ppi               Ppi pointer
 * @retval     EFI_STATUS
 */

EFI_STATUS
EFIAPI
UsbPlatformCustomizePei (
  IN EFI_PEI_SERVICES                **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR       *NotifyDescriptor,
  IN VOID                            *Ppi
  ) 
{
  VOID                               *PlatformHardwareSwitchBuffer;
  PLATFORM_HARDWARE_SWITCH           *PlatformHardwareSwitch;
  UINT32                             UsbPortDisable;

  PlatformHardwareSwitchBuffer = GetFirstGuidHob (&gPlatformHardwareSwitchGuid);
  if (PlatformHardwareSwitchBuffer != NULL) {
    PlatformHardwareSwitch = (PLATFORM_HARDWARE_SWITCH *) GET_GUID_HOB_DATA (PlatformHardwareSwitchBuffer);

    UsbPortDisable = ~(UINT32)(PlatformHardwareSwitch->UsbPortSwitch.CONFIG & 0xFFFFFFFF);
    
    //USB3 Port0-4
    PcdSet32 (PcdXhciUsb3PortDisable, UsbPortDisable & 0x1f);
    //USB2 Port0-5
    PcdSet32 (PcdXhciUsb2PortDisable, UsbPortDisable & 0x3f);
  }

  return EFI_SUCCESS;
}

/**
  To feedback its proprietary settings of the hardware switches.
  if you wants it to be not auto detected while it is set to 'PORT_ENABLE'. 
  That means, you has 3 choices to a port of PCI-e Ports - 'PORT_DISABLE', 'PCIE_PORT_AUTO', 
  and 'PORT_ENABLE'. For Usb and Sata ports, only two options are available -'PORT_DISABLE' and 'PORT_ENABLE'.

  @param[out]  PlatformHardwareSwitch    A Pointer to PLATFORM_HARDWARE_SWITCH.

  @retval      EFI_SUCCESS                Set hardware switch success.
  @retval      EFI_NOT_FOUND              Locate ReadOnlyVariablePpi failed.
**/
EFI_STATUS
SetPlatformHardwareSwitch (
  OUT PLATFORM_HARDWARE_SWITCH          **PlatformHardwareSwitchDptr
  )
{
  EFI_STATUS                            GetSetupVariableStatus;
  PLATFORM_HARDWARE_SWITCH              *PlatformHardwareSwitch;
  CHIPSET_CONFIGURATION                 ChipsetConfiguration;

  PlatformHardwareSwitch = *PlatformHardwareSwitchDptr;

  GetSetupVariableStatus = GetChipsetConfiguration (&ChipsetConfiguration);

  //
  // Default Enable all ports
  //
  PlatformHardwareSwitch->SataPortSwitch.CONFIG = 0xFFFFFFFFFFFFFFFF;
  PlatformHardwareSwitch->UsbPortSwitch.CONFIG = 0xFFFFFFFFFFFFFFFF;

  //
  // For Carrizo platform, there are 5 PCIE port in APU.
  // The PciePortSwitch map shows as below
  // Port00En -> APU GPP Port 0 (Bus 0 Device 2 Function 1)
  // Port01En -> APU GPP Port 1 (Bus 0 Device 2 Function 2)
  // Port02En -> APU GPP Port 2 (Bus 0 Device 2 Function 3)
  // Port03En -> APU GPP Port 3 (Bus 0 Device 2 Function 4)
  // Port04En -> APU GPP Port 4 (Bus 0 Device 2 Function 5)
  //

  //
  // Default set PCIE port configure to Auto mode
  //
  PlatformHardwareSwitch->PciePortSwitch.CONFIG = 0xAAAAAAAAAAAAAAAA;

  if (!EFI_ERROR (GetSetupVariableStatus)) {
    PlatformHardwareSwitch->PciePortSwitch.Config.Port00En = ChipsetConfiguration.APUGpp0;
    PlatformHardwareSwitch->PciePortSwitch.Config.Port01En = ChipsetConfiguration.APUGpp1;
    PlatformHardwareSwitch->PciePortSwitch.Config.Port02En = ChipsetConfiguration.APUGpp2;
    PlatformHardwareSwitch->PciePortSwitch.Config.Port03En = ChipsetConfiguration.APUGpp3;
    PlatformHardwareSwitch->PciePortSwitch.Config.Port04En = ChipsetConfiguration.APUGpp4;
    PlatformHardwareSwitch->PciePortSwitch.Config.Port05En = ChipsetConfiguration.APUGpp5;
    PlatformHardwareSwitch->PciePortSwitch.Config.Port06En = ChipsetConfiguration.APUGpp6;

    PlatformHardwareSwitch->SataPortSwitch.Config.Port00En = ChipsetConfiguration.SataPort0Power;
    PlatformHardwareSwitch->SataPortSwitch.Config.Port01En = ChipsetConfiguration.SataPort1Power;

    PlatformHardwareSwitch->UsbPortSwitch.Config.Port00En  = ChipsetConfiguration.UsbPort0;
    PlatformHardwareSwitch->UsbPortSwitch.Config.Port01En  = ChipsetConfiguration.UsbPort1;
    PlatformHardwareSwitch->UsbPortSwitch.Config.Port02En  = ChipsetConfiguration.UsbPort2;
    PlatformHardwareSwitch->UsbPortSwitch.Config.Port03En  = ChipsetConfiguration.UsbPort3;
    PlatformHardwareSwitch->UsbPortSwitch.Config.Port04En  = ChipsetConfiguration.UsbPort4;
    PlatformHardwareSwitch->UsbPortSwitch.Config.Port05En  = ChipsetConfiguration.UsbPort5;
  }

  /*
  Sample code to set Platform Hardware Switch by platform design
  All ports is enabled by default. All PCIE ports is configured to Auto
  Below sample code disable Sata PORT, USB PORT and PCIE PORT.

  PlatformHardwareSwitch->SataPortSwitch.Config.Port01En = PORT_DISABLE;
  PlatformHardwareSwitch->SataPortSwitch.Config.Port01En = PORT_DISABLE;

  PlatformHardwareSwitch->UsbPortSwitch.Config.Port00En  = PORT_DISABLE;

  PlatformHardwareSwitch->Usb3PortSwitch.Config.Port00En = PORT_DISABLE;
  PlatformHardwareSwitch->Usb3PortSwitch.Config.Port01En = PORT_DISABLE;

  //
  // GNB has 5 GPP. They are port0 to port4
  //
  PlatformHardwareSwitch->PciePortSwitch.Config.Port00En = PORT_DISABLE;
  PlatformHardwareSwitch->PciePortSwitch.Config.Port01En = PORT_DISABLE;
  PlatformHardwareSwitch->PciePortSwitch.Config.Port02En = PORT_DISABLE;
  PlatformHardwareSwitch->PciePortSwitch.Config.Port03En = PORT_DISABLE;
  PlatformHardwareSwitch->PciePortSwitch.Config.Port04En = PORT_DISABLE;
  
  */

  return EFI_SUCCESS;
}

/**
  Use the PEI_READ_ONLY_VARIABLE_ACCESS_PPI to notify and set platform hardware switch value.

  @param  **PeiServices         General purpose services available to every PEIM.
  @param  *NotifyDescriptor     Pointer to EFI_PEI_NOTIFY_DESCRIPTOR.
  @param  *Ppi                  EFI_PEI_FV_FILE_LOADER_PPI
  
  @retval EFI_SUCCESS           if the interface could be successfully installed.
**/
EFI_STATUS
EFIAPI
SetPlatformHardwareSwitchNotifyCallback (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  )
{
  EFI_STATUS                     Status;
  PLATFORM_HARDWARE_SWITCH       PlatformHardwareSwitch;
  PLATFORM_HARDWARE_SWITCH       EmptyPlatformHardwareSwitch;
  PLATFORM_HARDWARE_SWITCH       *PlatformHardwareSwitchDptr;

  Status = EFI_SUCCESS;

  ZeroMem (&PlatformHardwareSwitch, sizeof (PLATFORM_HARDWARE_SWITCH));
  ZeroMem (&EmptyPlatformHardwareSwitch, sizeof (PLATFORM_HARDWARE_SWITCH));

  PlatformHardwareSwitchDptr = &PlatformHardwareSwitch;

  SetPlatformHardwareSwitch (&PlatformHardwareSwitchDptr);

  if (CompareMem (
  &EmptyPlatformHardwareSwitch, 
  &PlatformHardwareSwitch, 
  sizeof (PLATFORM_HARDWARE_SWITCH)) == 0) {
    return Status;
  }

  //
  // Set PlatformHardwareSwitch data to HOB.
  //
  BuildGuidDataHob (
    &gPlatformHardwareSwitchGuid, 
    PlatformHardwareSwitchDptr, 
    sizeof (PLATFORM_HARDWARE_SWITCH)
    );

  return Status;
}

/**
  Notify some PPI for special purpose.

  @param  *FfsHeader            Point to EFI_FFS_FILE_HEADER.
  @param  **PeiServices         Point to EFI_PEI_SERVICES.

  @retval EFI_SUCCESS           Store Notify success.
  @retval Others                Create Notify failed.
**/
EFI_STATUS
EFIAPI
EarlySetPlatformHardwareSwitchPeiEntryPoint (
  IN EFI_PEI_FILE_HANDLE              FileHandle,
  IN CONST EFI_PEI_SERVICES           **PeiServices
  )
{
  EFI_STATUS                    Status;

  Status = (**PeiServices).NotifyPpi (PeiServices, &mNotifyPpiList[0]);
  if (EFI_ERROR(Status)) {
    return Status;
  }
  
  Status = PeiServicesNotifyPpi (mUsb3PcdNotifyPpiList);
  if (EFI_ERROR(Status)) {
    return Status;
  }
  
  Status = PeiServicesNotifyPpi (mPlatformCustomizeNotifyPpiList);
  if (EFI_ERROR(Status)) {
    return Status;
  }

  return EFI_SUCCESS;
}

