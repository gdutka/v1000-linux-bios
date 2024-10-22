/** @file
 DXE Chipset Services Library.

 This file contains only one function that is DxeCsSvcSkipGetPciRom().
 The function DxeCsSvcSkipGetPciRom() use chipset services to return 
 a PCI ROM image for the device is represented or not.

***************************************************************************
* Copyright (c) 2015 - 2017, Insyde Software Corp. All Rights Reserved.
*
* You may not reproduce, distribute, publish, display, perform, modify, adapt,
* transmit, broadcast, present, recite, release, license or otherwise exploit
* any part of this publication in any form, by any means, without the prior
* written permission of Insyde Software Corporation.
*
******************************************************************************
*/

#include <Library/UefiBootServicesTableLib.h>
#include <Library/PciLib.h>
#include <IndustryStandard/Pci.h> 
#include <Library/UefiRuntimeServicesTableLib.h>

#include <Library/KernelConfigLib.h>
#include <Library/ChipsetConfigLib.h>
#include <Fch.h>
#include <SSFch.h>

/**
 Skip to load PCI ROM for specify device.

 @param[in]         Segment             The PCI segment number for PCI device.
 @param[in]         Bus                 The PCI bus number for PCI device.
 @param[in]         Device              The PCI device number for PCI device.
 @param[in]         Function            The PCI function number for PCI device.
 @param[in]         VendorId            The vendor ID for PCI device.
 @param[in]         DeviceId            The device ID for PCI device.
 @param[in, out]    SkipGetPciRom       If SkipGetPciRom == TRUE means that there is no ROM in this device.

 @retval            EFI_SUCCESS         The operation completed successfully.
 @retval            EFI_PROTOCOL_ERROR  Can't find setup utility protocol.
*/
EFI_STATUS
SkipGetPciRom (
  IN  UINTN        Segment,
  IN  UINTN        Bus,
  IN  UINTN        Device,
  IN  UINTN        Function,
  IN  UINT16       VendorId,
  IN  UINT16       DeviceId,
  OUT BOOLEAN     *SkipGetPciRom
  )
{
  EFI_STATUS                  Status;
  UINT16                      DeviceClass;
  KERNEL_CONFIGURATION        KernelSetup;
  CHIPSET_CONFIGURATION       ChipsetSetup;
  UINT8                       BusB = 0x0;

  //
  // Get NVRAM data
  //
  Status = GetKernelConfiguration (&KernelSetup);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = GetChipsetConfiguration (&ChipsetSetup);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  DeviceClass = PciRead16 (PCI_LIB_ADDRESS (Bus, Device, Function, 0x0A));

  //
  // Internal PCIe GPP Bridge 0 to Bus B. (PPR.1 Table 13)
  //
  BusB = PciRead8 (PCI_LIB_ADDRESS (0, 8, 2, 0x19));

  //
  // Do not run IDE RAID if Raid is disabled
  //
  
  if ((Bus == BusB) && (Device == 0) && (Function == 0)) {
    if ((DeviceClass != ((PCI_CLASS_MASS_STORAGE << 8) | PCI_CLASS_MASS_STORAGE_SATADPA)) &&
        (DeviceClass != ((PCI_CLASS_MASS_STORAGE << 8) | PCI_CLASS_MASS_STORAGE_RAID))
       ) {
      *SkipGetPciRom = TRUE;
    }
  } else {
    //
    // Run PXE ROM only if Boot network is enabled
    //
    if ((KernelSetup.PxeBootToLan == 0) &&
         (DeviceClass == ((PCI_CLASS_NETWORK << 8) | PCI_CLASS_NETWORK_ETHERNET))
       ) {
      *SkipGetPciRom = TRUE;
    }
  }

  if ((VendorId == FCH_SATA_VID) && (DeviceId == SANDSTONE_FCH_SATA_AHCI_DID)) {
    if ((KernelSetup.SataCnfigure == 2) && // SATA AS AHCI MODE
        (ChipsetSetup.AHCIMODE == 1)       // AHCI IN uEFI Native Driver MODE
       ) {
      *SkipGetPciRom = TRUE;
    }
  }

  //
  // Run PXE ROM under onboard USB controller only when Boot network is enabled and legacy mode
  //
  if ((DeviceClass == ((PCI_CLASS_SERIAL << 8) | PCI_CLASS_SERIAL_USB)) &&
      ((KernelSetup.PxeBootToLan == 0) || (KernelSetup.NetworkProtocol != 3))
    ) {
    *SkipGetPciRom = TRUE;
  }

  return EFI_SUCCESS;
}
