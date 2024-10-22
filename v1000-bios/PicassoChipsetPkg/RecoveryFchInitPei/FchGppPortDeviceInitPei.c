/** @file

;******************************************************************************
;* Copyright (c) 2013 - 2018, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/
#include "RecoveryFchInitPei.h"
#include "Xhci.h"
#include "Sata.h"
#include "Nvme.h"
#include "Vga.h"

#define PCIE_BRIDGE_RESOUCE_LENGTH      SIZE_1MB
#define MAX_FUNCTION_NUMBER             0x07
#define TEMP_BUS_NUMBER                 0x02

#define PCI_TO_PCI_BRIDGE_CLASSCODE     0x06040000
#define MAX_CHILD_DEVICE_NUMBER         0x00
#define MAX_CHILD_FUNCTION_NUMBER       0x04

#define MAX_P2P_CONTROLLERS             0x10

STATIC UINTN mUsbHcPciAdrBuffer[MAX_XHCI_CONTROLLERS]  = {0};
STATIC UINTN mSataPciAdrBuffer[MAX_SATA_CONTROLLERS]   = {0};
STATIC UINTN mNvmePciAdrBuffer[MAX_NVME_CONTROLLERS]   = {0};
STATIC UINTN mGppPciAdrBuffer[MAX_P2P_CONTROLLERS]     = {0};
STATIC UINT8 mCurrentGppCounter                        = 0;

STATIC UINTN mTempGppPciAdrBuffer[MAX_P2P_CONTROLLERS] = {0};
STATIC UINT8 mTempGppIndex                             = 0;

STATIC UINT8 mGppPortDeviceNumber[] = {
  0x01,
  0x08,
  0xff
};

EFI_STATUS
GetAhciMemBase (
  IN EFI_PEI_SERVICES               **PeiServices,
  IN PEI_AHCI_CONTROLLER_PPI        *This,
  IN UINT8                          AhciControllerId,
  IN OUT EFI_PHYSICAL_ADDRESS       *AhciMemBaseAddr
  );

EFI_STATUS
GetNvmeMemBase (
  IN     EFI_PEI_SERVICES           **PeiServices,
  IN     PEI_NVME_CONTROLLER_PPI    *This,
  IN     UINT8                      NvmeControllerId,
  IN OUT EFI_PHYSICAL_ADDRESS       *NvmeMemBaseAddr
  );

EFI_STATUS
EFIAPI
EndOfAhciPeiPpiNotifyCallback (
  IN       EFI_PEI_SERVICES          **PeiServices,
  IN       EFI_PEI_NOTIFY_DESCRIPTOR *NotifyDescriptor,
  IN VOID                            *Ppi
  );

EFI_STATUS
EFIAPI
EndOfNvmePeiPpiNotifyCallback (
  IN       EFI_PEI_SERVICES          **PeiServices,
  IN       EFI_PEI_NOTIFY_DESCRIPTOR *NotifyDescriptor,
  IN VOID                            *Ppi
  );

PEI_AHCI_CONTROLLER_PPI mAhciControllerPpi = {GetAhciMemBase};
PEI_NVME_CONTROLLER_PPI mNvmeControllerPpi = {GetNvmeMemBase};

EFI_PEI_PPI_DESCRIPTOR mAhciPpiList = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gPeiAhciControllerPpiGuid,
  NULL
};

EFI_PEI_PPI_DESCRIPTOR mNvmePpiList = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gPeiNvmeControllerPpiGuid,
  NULL
};

EFI_STATUS
InitP2pBridge (
  IN CONST EFI_PEI_SERVICES      **PeiServices,
  IN OUT UINT8                   *Bus,
  IN UINTN                       CurrentGppPciAdr,
  IN OUT UINT32                  *MmioResource
  );

EFI_STATUS
RemoveGppResource (
  IN  EFI_PEI_SERVICES                **PeiServices,
  IN  EFI_PEI_NOTIFY_DESCRIPTOR       *NotifyDesc,
  IN  VOID                            *InvokePpi
  );

STATIC EFI_PEI_NOTIFY_DESCRIPTOR mRemoveGppResourceNotify = {
  (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gEfiEndOfPeiSignalPpiGuid,
  RemoveGppResource
};


/*---------------------------------------------------------------------------------------*/
/**
 * Check RV1 or RV2
 *
 * @retval    TRUE    This is RV1
 *            FALSE   This is RV2
 *
 */
BOOLEAN
CheckRvOpnB4 (
 
  )
{
  UINT32        TempData32;

  TempData32 =  (PcdGet32 (PcdGetRVFamilyOPN) >> 30) & 0x3;
  
  if (TempData32 == 1) {
    return FALSE;
  } else if (TempData32 == 3) {
    return FALSE;
  }
  return TRUE;  
}

EFI_STATUS
RemoveGppResource (
  IN  EFI_PEI_SERVICES                **PeiServices,
  IN  EFI_PEI_NOTIFY_DESCRIPTOR       *NotifyDesc,
  IN  VOID                            *InvokePpi
  )
{

  UINT8 Index;

  Index = 0;

  DEBUG ((EFI_D_INFO, "[%a]Entry\n", __FUNCTION__));

  for (Index = 0 ; Index < MAX_P2P_CONTROLLERS ; Index++) {
    if (mGppPciAdrBuffer[Index] == 0) {
      continue;
    }

    DEBUG ((EFI_D_INFO, "  [%d]Gpp Found = 0x%x\n", Index, (UINT32)mGppPciAdrBuffer[Index]));

    //
    // Clear address range for root bridge
    //
    PciWrite8 ((mGppPciAdrBuffer[Index] | PCI_COMMAND_OFFSET), 0);
    PciWrite16 ((mGppPciAdrBuffer[Index] | 0x20), 0);
    PciWrite16 ((mGppPciAdrBuffer[Index] | 0x22), 0);
  }

  DEBUG ((EFI_D_INFO, "[%a]Exit\n", __FUNCTION__));

  return EFI_SUCCESS;

}

STATIC
EFI_STATUS
EFIAPI
GetXhciUsbController (
  IN  EFI_PEI_SERVICES        **PeiServices,
  IN  PEI_USB_CONTROLLER_PPI  *This,
  IN  UINT8                   UsbControllerId,
  OUT UINTN                   *ControllerType,
  OUT UINTN                   *BaseAddress
  )
{
  PEI_USBHC_DEVICE         *PeiUsbHcDev;

  DEBUG ((EFI_D_INFO, "    GetXhciUsbController Entry - %x\n", UsbControllerId));

  PeiUsbHcDev = PEI_USBHC_DEVICE_FROM_THIS (This);

  if (UsbControllerId >= PeiUsbHcDev->TotalUsbControllers) {
    return EFI_INVALID_PARAMETER;
  }

  *ControllerType = PEI_XHCI_CONTROLLER;
  *BaseAddress = PeiUsbHcDev->MemBase[UsbControllerId];

  DEBUG ((EFI_D_INFO, "      MMIO = 0x%x\n", (UINT32)*BaseAddress));
  DEBUG ((EFI_D_INFO, "    GetXhciUsbController Exit - %x\n", UsbControllerId));
  return EFI_SUCCESS;
}

STATIC
EFI_STATUS
EFIAPI
EndOfXhciPeiPpiNotifyCallback (
  IN EFI_PEI_SERVICES                     **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR            *NotifyDescriptor,
  IN VOID                                 *Ppi
  )
{
  EFI_STATUS               Status;
  UINT8                    Index;
  UINT8                    *BaseAddress;
  UINT32                   Command;
  UINT32                   UsbSts;
  UINTN                    Delay;
  PEI_USBHC_DEVICE         *PeiUsbHcDev;

  DEBUG ((EFI_D_INFO, "    EndOfXhciPeiPpiNotifyCallback Entry.\n"));

  PeiUsbHcDev = PEI_USBHC_DEVICE_FROM_NOTIFY_DESC (NotifyDescriptor);

  //
  // Reset the HC
  //
  for (Index = 0; Index < PeiUsbHcDev->TotalUsbControllers; Index++) {
    BaseAddress = (UINT8*) PeiUsbHcDev->MemBase[Index];
    BaseAddress = BaseAddress + (*(UINT32*)(UINTN)BaseAddress & 0xff);
    //
    // Halt HC first
    //
    Command = *(UINT32*)(UINTN)BaseAddress;
    Command &= ~0x01;
    *(UINT32*)(UINTN)BaseAddress = Command;

    //
    // Set timeout to 500 millisecond
    //
    Delay = (500 * 1000) / 30;
    do {
      //
      // Check the HCHalted of USBSTS register is set by HC
      //
      CopyMem (&UsbSts, (BaseAddress + 4), sizeof (UINT32));
      if (UsbSts & 0x01) {
        break;
      }
      Status = PeiUsbHcDev->StallPpi->Stall (
                                        (CONST EFI_PEI_SERVICES**)PeiServices,
                                        PeiUsbHcDev->StallPpi,
                                        30
                                        );
    } while (Delay--);

    //
    // HCReset
    //
    Command = *(UINT32*)(UINTN)BaseAddress;
    Command |= 0x02;
    *(UINT32*)(UINTN)BaseAddress = Command;
  }

  DEBUG ((EFI_D_INFO, "    EndOfXhciPeiPpiNotifyCallback Exit.\n"));

  return EFI_SUCCESS;
}

STATIC
EFI_STATUS
EnableXhciUsbController (
  IN CONST EFI_PEI_SERVICES       **PeiServices,
  IN PEI_USBHC_DEVICE             *PeiUsbHcDev,
  IN UINT8                        UsbControllerId
  )
{
  UINTN                     BaseAddress;

  DEBUG ((EFI_D_INFO, "                EnableXhciUsbController Entry - %x\n", UsbControllerId));

  if (UsbControllerId >= PeiUsbHcDev->TotalUsbControllers) {
    return EFI_INVALID_PARAMETER;
  }

  BaseAddress =  PeiUsbHcDev->MemBase[UsbControllerId];

  DEBUG ((EFI_D_INFO, "                  BaseAddress = 0x%x\n", (UINT32)BaseAddress));

  DEBUG_CODE (
    DEBUG ((EFI_D_INFO, "                  Pci Addr = 0x%x\n", (UINT32)PeiUsbHcDev->PciCfgAddress[UsbControllerId]));
    DEBUG ((EFI_D_INFO, "                    Vid/Did = 0x%x\n", (UINT32)PciRead32 (PeiUsbHcDev->PciCfgAddress[UsbControllerId])));
    //
    // Check required size.
    //
    PciWrite32 (PeiUsbHcDev->PciCfgAddress[UsbControllerId] | PCI_BASE_ADDRESSREG_OFFSET, (UINT32)0xFFFFFFFF);
    DEBUG ((
      EFI_D_INFO,
      "                  Requied Size = 0x%x\n",
      (UINT32)PciRead32 (PeiUsbHcDev->PciCfgAddress[UsbControllerId] | PCI_BASE_ADDRESSREG_OFFSET)
      ));
  );

  //
  // Assign base address register
  //
  PciWrite32 (
    PeiUsbHcDev->PciCfgAddress[UsbControllerId] | PCI_BASE_ADDRESSREG_OFFSET,
    (UINT32)BaseAddress
    );

  //
  // Enable UsbHc
  //
  PciOr16 (
    PeiUsbHcDev->PciCfgAddress[UsbControllerId] | PCI_COMMAND_OFFSET,
    (EFI_PCI_COMMAND_BUS_MASTER | EFI_PCI_COMMAND_MEMORY_SPACE)
    );

  DEBUG ((EFI_D_INFO, "                EnableXhciUsbController Exit - %x\n", UsbControllerId));

  return EFI_SUCCESS;
}

EFI_STATUS
InitXhciController (
  IN CONST EFI_PEI_SERVICES      **PeiServices,
  IN UINTN                       *XhciPciDevAddress,
  IN OUT UINT32                  *MmioResource,
  IN UINT8                       XHCIControllersNumber  
  )
{
  EFI_STATUS              Status;
  EFI_PEI_STALL_PPI       *StallPpi;
  EFI_PHYSICAL_ADDRESS    AllocateAddress;
  PEI_USBHC_DEVICE        *PeiUsbHcDev;
  UINT8                   Index;

  DEBUG ((EFI_D_INFO, "              InitXhciController Entry, MMIO = 0x%x\n", (*MmioResource)));

  Status = (**PeiServices).LocatePpi (
                             PeiServices,
                             &gEfiPeiStallPpiGuid,
                             0,
                             NULL,
                             (VOID **)&StallPpi
                             );
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "          gEfiPeiStallPpiGuid LOST!!\n"));
    return EFI_UNSUPPORTED;
  }

  //
  // For Xhci init, it must be 32K byte alignment.
  //
  if (((*MmioResource) & (UINT32)(SIZE_1MB - 1)) != 0) {
    (*MmioResource) += (UINT32)SIZE_1MB;
    (*MmioResource) &= (UINT32)0xFFF00000;
    DEBUG ((EFI_D_INFO, "              For Xhci Init, Adjust MMIO = 0x%x\n", (*MmioResource)));
  }

  Status = (**PeiServices).AllocatePages (
                             PeiServices,
                             EfiBootServicesData,
                             sizeof (PEI_XHCI_DEVICE) / EFI_PAGE_SIZE + 1,
                             &AllocateAddress
                             );
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "          AllocatePages FAIL!! (%r)\n", Status));
    return Status;
  }

  PeiUsbHcDev = (PEI_USBHC_DEVICE *)((UINTN)AllocateAddress);
  ZeroMem (PeiUsbHcDev, sizeof (PEI_USBHC_DEVICE));

  PeiUsbHcDev->Signature                         = PEI_USBHC_SIGNATURE;
  PeiUsbHcDev->UsbControllerPpi.GetUsbController = GetXhciUsbController;
  PeiUsbHcDev->StallPpi                          = StallPpi;
  PeiUsbHcDev->TotalUsbControllers               = XHCIControllersNumber;
  PeiUsbHcDev->PciCfgAddress                     = mUsbHcPciAdrBuffer;

  //
  // Assign resources and enable Usb controllers
  //
  for (Index = 0; Index < PeiUsbHcDev->TotalUsbControllers; Index++) {
    PeiUsbHcDev->MemBase[Index] = (*MmioResource);
    Status = EnableXhciUsbController (PeiServices, PeiUsbHcDev, Index);
    (*MmioResource) += SIZE_1MB;
  }

  //
  // Install Usb Controller Ppi
  //
  PeiUsbHcDev->PpiList.Flags = (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST);
  PeiUsbHcDev->PpiList.Guid  = &gPeiUsbControllerPpiGuid;
  PeiUsbHcDev->PpiList.Ppi   = &PeiUsbHcDev->UsbControllerPpi;

  Status = (**PeiServices).InstallPpi (
                             PeiServices,
                             &PeiUsbHcDev->PpiList
                             );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Install notification in order to reset the UsbHc if needed
  //
  PeiUsbHcDev->NotifyList.Flags  = (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST);
  PeiUsbHcDev->NotifyList.Guid   = &gEfiEndOfPeiSignalPpiGuid;
  PeiUsbHcDev->NotifyList.Notify = !PcdGetBool(PcdUseFastCrisisRecovery) ? NULL : EndOfXhciPeiPpiNotifyCallback;

  if (PeiUsbHcDev->NotifyList.Notify != NULL) {
    Status = (**PeiServices).NotifyPpi (
                               PeiServices,
                               &PeiUsbHcDev->NotifyList
                               );
    if (EFI_ERROR(Status)) {
      return Status;
    }
  }

  DEBUG ((EFI_D_INFO, "              InitXhciController Exit, MMIO = 0x%x\n", (*MmioResource)));

  return EFI_SUCCESS;
}

EFI_STATUS
InitSataController (
  IN CONST EFI_PEI_SERVICES      **PeiServices,
  IN UINTN                       SataPciDevAddress,
  IN OUT UINT32                  *MmioResource
  )
{
  EFI_STATUS              Status;
  PEI_AHCI_DEVICE         *PeiAhciDev;
  EFI_PHYSICAL_ADDRESS    AllocateAddress;

  Status                = EFI_SUCCESS;

  DEBUG ((EFI_D_INFO, "              InitSataController Entry, MMIO = 0x%x\n", (*MmioResource)));

  //
  // For Ahci init, it must be 128K byte alignment.
  //
  if (((*MmioResource) & (UINT32)(SIZE_128KB - 1)) != 0) {
    (*MmioResource) += (UINT32)SIZE_128KB;
    (*MmioResource) &= (UINT32)0xFFFE0000;
    DEBUG ((EFI_D_INFO, "                For Ahci Init, Adjust MMIO = 0x%x\n", (*MmioResource)));
  }

  DEBUG_CODE (
    DEBUG ((EFI_D_INFO, "                Pci Addr = 0x%x\n", (UINT32)SataPciDevAddress));
    DEBUG ((EFI_D_INFO, "                  Vid/Did = 0x%x\n", (UINT32)PciRead32 (SataPciDevAddress)));
    //
    // Check required size.
    //
    PciWrite32 ((UINTN)SataPciDevAddress | SATA_AHCI_BAR, (UINT32)0xFFFFFFFF);
    DEBUG ((
      EFI_D_INFO,
      "                Requied Size = 0x%x\n",
      (UINT32)PciRead32 ((UINTN)SataPciDevAddress | SATA_AHCI_BAR)
      ));
  );

  //
  // Assign base address register to AHCI and enable it.
  //
  DEBUG ((EFI_D_INFO, "                MmioResource = 0x%x\n", (UINT32)(*MmioResource)));
  PciWrite32 ((UINTN)SataPciDevAddress | SATA_AHCI_BAR, (UINT32)*MmioResource);
  PciOr16 (
    (UINTN)SataPciDevAddress | PCI_COMMAND_OFFSET,
    (UINT16)(EFI_PCI_COMMAND_BUS_MASTER + EFI_PCI_COMMAND_MEMORY_SPACE)
    );

  //
  // Provide interface related to AHCI controller.
  //
  Status = (**PeiServices).AllocatePages (
                             PeiServices,
                             EfiBootServicesData,
                             sizeof (PEI_AHCI_DEVICE) / EFI_PAGE_SIZE + 1,
                             &AllocateAddress
                             );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  PeiAhciDev = (PEI_AHCI_DEVICE *)((UINTN)AllocateAddress);
  ZeroMem (PeiAhciDev, sizeof (PEI_AHCI_DEVICE));

  PeiAhciDev->Signature            = PEI_AHCI_SIGNATURE;
  PeiAhciDev->AhciControllerPpi    = mAhciControllerPpi;
  PeiAhciDev->PpiList              = mAhciPpiList;
  PeiAhciDev->PpiList.Ppi          = &PeiAhciDev->AhciControllerPpi;
  PeiAhciDev->TotalAhciControllers = 1;
  PeiAhciDev->MemBase[0]           = (EFI_PHYSICAL_ADDRESS)(*MmioResource);
  PeiAhciDev->PciCfgAddress        = mSataPciAdrBuffer;

  //
  // Note : Only support 1 controller init.
  //
  PeiAhciDev->PciCfgAddress[0]     = SataPciDevAddress;

  //
  // Install AHCI Controller PPI
  //
  Status = (**PeiServices).InstallPpi (
                             PeiServices,
                             &PeiAhciDev->PpiList
                             );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Install notification in order to reset the AHCI
  //
  if (PcdGetBool (PcdUseFastCrisisRecovery)) {
    PeiAhciDev->NotifyList.Flags  = (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST);
    PeiAhciDev->NotifyList.Guid   = &gEfiEndOfPeiSignalPpiGuid;
    PeiAhciDev->NotifyList.Notify = EndOfAhciPeiPpiNotifyCallback;

    Status = (**PeiServices).NotifyPpi (
                               PeiServices,
                               &PeiAhciDev->NotifyList
                               );
  }

  *MmioResource += SIZE_128KB;

  DEBUG ((EFI_D_INFO, "              InitSataController Exit, MMIO = 0x%x\n", (*MmioResource)));

  return Status;

}

EFI_STATUS
GetAhciMemBase (
  IN     EFI_PEI_SERVICES           **PeiServices,
  IN     PEI_AHCI_CONTROLLER_PPI    *This,
  IN     UINT8                      AhciControllerId,
  OUT    EFI_PHYSICAL_ADDRESS       *AhciMemBaseAddr
  )
{
  PEI_AHCI_DEVICE                   *PeiAhciDev;

  DEBUG ((EFI_D_INFO, "GetAhciMemBase Entry\n"));
  DEBUG ((EFI_D_INFO, "  AhciControllerId = %x\n", AhciControllerId));

  PeiAhciDev = PEI_AHCI_DEVICE_FROM_THIS (This);

  if (AhciControllerId >= PeiAhciDev->TotalAhciControllers) {
    return EFI_INVALID_PARAMETER;
  }

  *AhciMemBaseAddr = PeiAhciDev->MemBase[AhciControllerId];

  DEBUG ((EFI_D_INFO, "  AhciMemBaseAddr = 0x%x\n", (UINT32)(*AhciMemBaseAddr)));


  DEBUG ((EFI_D_INFO, "GetAhciMemBase Exit\n"));

  return EFI_SUCCESS;

}

EFI_STATUS
EFIAPI
EndOfAhciPeiPpiNotifyCallback (
  IN       EFI_PEI_SERVICES               **PeiServices,
  IN       EFI_PEI_NOTIFY_DESCRIPTOR      *NotifyDescriptor,
  IN VOID                                 *Ppi
  )
{
  UINT8                    Index;
  UINT8                    *BaseAddress;
  UINT32                   Register;
  PEI_AHCI_DEVICE          *PeiAhciDev;

  DEBUG ((EFI_D_INFO, "EndOfAhciPeiPpiNotifyCallback Entry\n"));

  PeiAhciDev = PEI_AHCI_DEVICE_FROM_NOTIFY_DESC (NotifyDescriptor);

  //
  // Reset the HC
  //
  for (Index = 0; Index < PeiAhciDev->TotalAhciControllers; Index++) {
    DEBUG ((EFI_D_INFO, "  MemBase = 0x%x\n", (UINT32)(UINTN)(PeiAhciDev->MemBase[Index])));

    //
    // Reset the HC
    //
    BaseAddress = (UINT8*)(UINTN) PeiAhciDev->MemBase[Index];
    BaseAddress = BaseAddress + (*(UINT32*)(UINTN)BaseAddress & 0xff);

    //
    // Halt HC first
    //
    Register = *(UINT32*)(UINTN)BaseAddress;
    Register &= ~0x01;
    *(UINT32*)(UINTN)BaseAddress = Register;

    //
    // HCReset
    //
    Register = *(UINT32*)(UINTN)BaseAddress;
    Register |= 0x02;
    *(UINT32*)(UINTN)BaseAddress = Register;

    //
    // Disable AHCI
    //
    PciAnd8 ((UINTN)PeiAhciDev->PciCfgAddress[Index] | PCI_COMMAND_OFFSET, (UINT8)(0xF9));

  }

  DEBUG ((EFI_D_INFO, "EndOfAhciPeiPpiNotifyCallback Exit\n"));

  return EFI_SUCCESS;

}

EFI_STATUS
InitVgaController (
  IN CONST EFI_PEI_SERVICES      **PeiServices,
  IN UINTN                       VgaPciDevAddress,
  IN OUT UINT32                  *MmioResource
  )
{
  EFI_STATUS                     Status;
  FABRIC_RESOURCE_FOR_EACH_RB    MmioForEachDie;
  UINT32                         Data32;
  UINT16                         Data16;
  UINT64                         CurrentPrefetchableBase = 0;
  UINT64                         AllocatedPrefetchableBase = 0;
  UINT64                         AllocatedPrefetchableSize = 0;
  UINT64                         AllocatedIoBase = RES_IO_BASE;      
  
  DEBUG ((EFI_D_INFO, "              [%a] Entry, MMIO = 0x%x\n", __FUNCTION__, (*MmioResource)));
  //
  // For VGA init, it must be 128K byte alignment.
  //
  if (((*MmioResource) & (UINT32)(SIZE_512KB - 1)) != 0) {
    (*MmioResource) += (UINT32)SIZE_512KB;
    (*MmioResource) &= (UINT32)0xFFF80000;
    DEBUG ((EFI_D_INFO, "                For VGA Init, Adjust MMIO = 0x%x\n", (*MmioResource)));
  }

  DEBUG_CODE (
    DEBUG ((EFI_D_INFO, "                Pci Addr = 0x%x\n", (UINT32)VgaPciDevAddress));
    DEBUG ((EFI_D_INFO, "                  Vid/Did = 0x%x\n", (UINT32)PciRead32 (VgaPciDevAddress)));
    //
    // Check required size.
    //
    PciWrite32 ((UINTN)VgaPciDevAddress | VGA_NON_PREFECTABLE_BAR5, (UINT32)0xFFFFFFFF);
    DEBUG ((
      EFI_D_INFO,
      "                Requied Size = 0x%x\n",
      (UINT32)PciRead32 ((UINTN)VgaPciDevAddress | VGA_NON_PREFECTABLE_BAR5)
      ));
  );  

  //
  // Assign base address register to VGA and enable it.
  //
  DEBUG ((EFI_D_INFO, "                MmioResource = 0x%x\n", (UINT32)(*MmioResource)));
  PciWrite32 ((UINTN)VgaPciDevAddress | VGA_NON_PREFECTABLE_BAR5, (UINT32)*MmioResource);
  *MmioResource += SIZE_512KB;

  Status = FabricGetAvailableResource (&MmioForEachDie);
  if (EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, " VGA init:Get Fabric Resource failed.\n")); 
    return EFI_OUT_OF_RESOURCES; 
  }
  
  if (MmioForEachDie.PrefetchableMmioSizeBelow4G[0][0].Size >= (SIZE_256MB + SIZE_2MB)) {
    AllocatedPrefetchableBase = MmioForEachDie.PrefetchableMmioSizeBelow4G[0][0].Base;
    AllocatedPrefetchableSize = MmioForEachDie.PrefetchableMmioSizeBelow4G[0][0].Size;  
  }

  if (MmioForEachDie.PrimaryRbSecondPrefetchableMmioSizeBelow4G.Size >= (SIZE_256MB + SIZE_2MB) && AllocatedPrefetchableBase == 0) {
    AllocatedPrefetchableBase = MmioForEachDie.PrimaryRbSecondPrefetchableMmioSizeBelow4G.Base;
    AllocatedPrefetchableSize = MmioForEachDie.PrimaryRbSecondPrefetchableMmioSizeBelow4G.Size; 
  }

  if (MmioForEachDie.NonPrefetchableMmioSizeBelow4G[0][0].Size >= (SIZE_256MB + SIZE_2MB) && AllocatedPrefetchableBase == 0) {
    AllocatedPrefetchableBase = MmioForEachDie.NonPrefetchableMmioSizeBelow4G[0][0].Base;
    AllocatedPrefetchableSize = MmioForEachDie.NonPrefetchableMmioSizeBelow4G[0][0].Size;
  }
  
  if (MmioForEachDie.PrimaryRbSecondNonPrefetchableMmioSizeBelow4G.Size >= (SIZE_256MB + SIZE_2MB) && AllocatedPrefetchableBase == 0) {
    AllocatedPrefetchableBase = MmioForEachDie.PrimaryRbSecondNonPrefetchableMmioSizeBelow4G.Base;
    AllocatedPrefetchableSize = MmioForEachDie.PrimaryRbSecondNonPrefetchableMmioSizeBelow4G.Size;
  }
  DEBUG((EFI_D_ERROR, "AllocatedPrefetchableBase: 0x%016LX, AllocatedPrefetchableSize: 0x%08X.\n", AllocatedPrefetchableBase, AllocatedPrefetchableSize));  
  if (AllocatedPrefetchableBase == 0) {
    DEBUG((EFI_D_ERROR, " VGA init:Out of resource\n")); 
    return EFI_OUT_OF_RESOURCES; 
  }
  
  CurrentPrefetchableBase = AllocatedPrefetchableBase; 
  DEBUG ((EFI_D_INFO, "                BAR0 MMIO = 0x%x\n", CurrentPrefetchableBase));   
  PciWrite32 ((UINTN)VgaPciDevAddress | VGA_PREFECTABLE_BAR0, (UINT32)CurrentPrefetchableBase);        
  PciWrite32 ((UINTN)VgaPciDevAddress | VGA_PREFECTABLE_BAR1, 0); 
  CurrentPrefetchableBase += SIZE_256MB; 

  DEBUG ((EFI_D_INFO, "                BAR2 MMIO = 0x%x\n", CurrentPrefetchableBase));  
  PciWrite32 ((UINTN)VgaPciDevAddress | VGA_PREFECTABLE_BAR2, (UINT32)CurrentPrefetchableBase);        
  PciWrite32 ((UINTN)VgaPciDevAddress | VGA_PREFECTABLE_BAR3, 0); 
  CurrentPrefetchableBase += SIZE_2MB;
  
  // Assign Prefetchable MMIO resource to bridge.
  if (AllocatedPrefetchableBase != 0) {  
    Data32 = ((AllocatedPrefetchableBase & 0xFFF00000) >> 16) + \
             ((AllocatedPrefetchableBase + (SIZE_256MB + SIZE_2MB) - 1) & 0xFFF00000);
    DEBUG((EFI_D_ERROR, "PREF_BASE_LIMIT: 0x%x.\n", Data32));             
    PciWrite32(PCI_LIB_ADDRESS (0, 8, 1, 0x24), Data32);
    
    Data32 = (AllocatedPrefetchableBase >> 32) & 0xFFFFFFFF;
    DEBUG((EFI_D_ERROR, "PREF_BASE_UPPER: 0x%x.\n", Data32));
    PciWrite32(PCI_LIB_ADDRESS (0, 8, 1, 0x28), Data32);

    Data32 = ((AllocatedPrefetchableBase + (SIZE_256MB + SIZE_2MB) - 1) >> 32) & 0xFFFFFFFF;
    DEBUG((EFI_D_ERROR, "PREF_LIMIT_UPPER: 0x%x.\n", Data32));
    PciWrite32(PCI_LIB_ADDRESS (0, 8, 1, 0x2C), Data32);                           
  }  
  
  PciWrite32 ((UINTN)VgaPciDevAddress | VGA_IO_BAR4, (UINT32)AllocatedIoBase); 
  
  // Assign IO resource to bridge.
  if (AllocatedIoBase != 0) {
    Data16 =  ((AllocatedIoBase & 0xF000) >> 8) + BIT0 + \
              ((AllocatedIoBase + VGA_IO_SIZE - 1) & 0xF000) + BIT8;
    DEBUG((EFI_D_ERROR, "IO_BASE_LIMIT: 0x%x.\n", Data16));               
    PciWrite16(PCI_LIB_ADDRESS (0, 8, 1, 0x1C), Data16);
    
    Data32 = ((AllocatedIoBase & 0xFFFF0000) >> 16) + \
             ((AllocatedIoBase + VGA_IO_SIZE - 1) & 0xFFFF0000);
    DEBUG((EFI_D_ERROR, "IO_BASE_LIMIT_HI: 0x%x.\n", Data32));
    PciWrite32(PCI_LIB_ADDRESS (0, 8, 1, 0x30), Data32);    
  }  
  PciOr8 (PCI_LIB_ADDRESS (0, 8, 1, 0x3E), BIT2 + BIT3); //ISA_EN and VGA_EN           
        
  PciOr16 (
    (UINTN)VgaPciDevAddress | PCI_COMMAND_OFFSET,
    (UINT16)(EFI_PCI_COMMAND_BUS_MASTER + EFI_PCI_COMMAND_MEMORY_SPACE + EFI_PCI_COMMAND_IO_SPACE)
    );
  DEBUG ((EFI_D_INFO, "              [%a] Exit, MMIO = 0x%x\n", __FUNCTION__, (*MmioResource)));    
  return EFI_SUCCESS;            
}

EFI_STATUS
InitNvmeController (
  IN CONST EFI_PEI_SERVICES      **PeiServices,
  IN UINTN                       NvmePciDevAddress,
  IN OUT UINT32                  *MmioResource
  )
{
  EFI_STATUS              Status;
  EFI_PEI_STALL_PPI       *StallPpi;
  PEI_NVME_DEVICE         *PeiNvmeDev;
  EFI_PHYSICAL_ADDRESS    AllocateAddress;

  Status = EFI_SUCCESS;

  DEBUG ((EFI_D_INFO, "              [%a] Entry, MMIO = 0x%x\n", __FUNCTION__, (*MmioResource)));

  Status = (**PeiServices).LocatePpi (
                             PeiServices,
                             &gEfiPeiStallPpiGuid,
                             0,
                             NULL,
                             (VOID**)&StallPpi
                             );
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "                gEfiPeiStallPpiGuid LOST!!\n"));
    return EFI_UNSUPPORTED;
  }

  //
  // For Nvme init, it must be at least 16K byte alignment. (Provides 32K)
  //
  if (((*MmioResource) & (UINT32)(SIZE_32KB - 1)) != 0) {
    (*MmioResource) += (UINT32)SIZE_32KB;
    (*MmioResource) &= (UINT32)0xFFFF8000;
    DEBUG ((EFI_D_INFO, "                For Nvme Init, Adjust MMIO = 0x%x\n", (*MmioResource)));
  }

  DEBUG_CODE (
    DEBUG ((EFI_D_INFO, "                Pci Addr = 0x%x\n", (UINT32)NvmePciDevAddress));
    DEBUG ((EFI_D_INFO, "                  Vid/Did = 0x%x\n", (UINT32)PciRead32 (NvmePciDevAddress)));
    //
    // Check required size.
    //
    PciWrite32 ((UINTN)NvmePciDevAddress | PCI_BASE_ADDRESSREG_OFFSET, (UINT32)0xFFFFFFFF);
    DEBUG ((
      EFI_D_INFO,
      "                Requied Size = 0x%x\n",
      (UINT32)PciRead32 ((UINTN)NvmePciDevAddress | PCI_BASE_ADDRESSREG_OFFSET)
      ));
  );

  //
  // Assign base address register to NVME and enable it.
  //
  DEBUG ((EFI_D_INFO, "                MmioResource = 0x%x\n", (UINT32)(*MmioResource)));
  PciWrite32 ((UINTN)NvmePciDevAddress | PCI_BASE_ADDRESSREG_OFFSET, (UINT32)*MmioResource);
  PciOr16 (
    (UINTN)NvmePciDevAddress | PCI_COMMAND_OFFSET,
    (UINT16)(EFI_PCI_COMMAND_BUS_MASTER + EFI_PCI_COMMAND_MEMORY_SPACE)
    );

  //
  // Provide interface related to Nvme controller.
  //
  Status = (**PeiServices).AllocatePages (
                             PeiServices,
                             EfiBootServicesData,
                             sizeof (PEI_NVME_DEVICE) / EFI_PAGE_SIZE + 1,
                             &AllocateAddress
                             );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  PeiNvmeDev = (PEI_NVME_DEVICE *)((UINTN)AllocateAddress);
  ZeroMem (PeiNvmeDev, sizeof (PEI_NVME_DEVICE));

  PeiNvmeDev->Signature            = PEI_NVME_SIGNATURE;
  PeiNvmeDev->NvmeControllerPpi    = mNvmeControllerPpi;
  PeiNvmeDev->StallPpi             = StallPpi;
  PeiNvmeDev->PpiList              = mNvmePpiList;
  PeiNvmeDev->PpiList.Ppi          = &PeiNvmeDev->NvmeControllerPpi;
  PeiNvmeDev->TotalNvmeControllers = 1;
  PeiNvmeDev->MemBase              = (UINTN)(*MmioResource);
  PeiNvmeDev->PciCfgAddress        = mNvmePciAdrBuffer;

  //
  // Note : Only support 1 controller init.
  //
  PeiNvmeDev->PciCfgAddress[0]     = NvmePciDevAddress;

  //
  // Install AHCI Controller PPI
  //
  Status = (**PeiServices).InstallPpi (
                             PeiServices,
                             &PeiNvmeDev->PpiList
                             );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Install notification in order to reset the NVME.
  //
  PeiNvmeDev->NotifyList.Flags  = (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST);
  PeiNvmeDev->NotifyList.Guid   = &gEfiEndOfPeiSignalPpiGuid;
  PeiNvmeDev->NotifyList.Notify = !PcdGetBool(PcdUseFastCrisisRecovery) ? NULL : EndOfNvmePeiPpiNotifyCallback;

  if (PeiNvmeDev->NotifyList.Notify != NULL) {
    Status = (**PeiServices).NotifyPpi (
                               PeiServices,
                               &PeiNvmeDev->NotifyList
                               );
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }

  *MmioResource += NVME_RECOVERY_TEMP_MEM_BASE_SIZE;

  DEBUG ((EFI_D_INFO, "              [%a] Exit, MMIO = 0x%x\n", __FUNCTION__, (*MmioResource)));

  return Status;

}

EFI_STATUS
GetNvmeMemBase (
  IN     EFI_PEI_SERVICES           **PeiServices,
  IN     PEI_NVME_CONTROLLER_PPI    *This,
  IN     UINT8                      NvmeControllerId,
  IN OUT EFI_PHYSICAL_ADDRESS       *NvmeMemBaseAddr
  )
{
  PEI_NVME_DEVICE                   *PeiNvmeDev;

  DEBUG ((EFI_D_INFO, "[%a] Entry\n", __FUNCTION__));
  DEBUG ((EFI_D_INFO, "  NvmeControllerId = %x\n", NvmeControllerId));

  PeiNvmeDev = PEI_NVME_DEVICE_FROM_THIS (This);

  if (NvmeControllerId >= PeiNvmeDev->TotalNvmeControllers) {
    return EFI_INVALID_PARAMETER;
  }

  *NvmeMemBaseAddr = PeiNvmeDev->MemBase;

  DEBUG ((EFI_D_INFO, "  NvmeMemBaseAddr = 0x%x\n", (UINT32)(*NvmeMemBaseAddr)));


  DEBUG ((EFI_D_INFO, "[%a] Exit\n", __FUNCTION__));

  return EFI_SUCCESS;

}

EFI_STATUS
EFIAPI
EndOfNvmePeiPpiNotifyCallback (
  IN       EFI_PEI_SERVICES               **PeiServices,
  IN       EFI_PEI_NOTIFY_DESCRIPTOR      *NotifyDescriptor,
  IN VOID                                 *Ppi
  )
{
  UINT8                    *BaseAddress;
  UINT32                   Register;
  PEI_NVME_DEVICE          *PeiNvmeDev;

  DEBUG ((EFI_D_INFO, "[%a] Entry\n", __FUNCTION__));

  PeiNvmeDev = PEI_NVME_DEVICE_FROM_NOTIFY_DESC (NotifyDescriptor);

  //
  // Disable the NVME
  //
  DEBUG ((EFI_D_INFO, "  MemBase = 0x%x\n", (UINT32)(UINTN)(PeiNvmeDev->MemBase)));

  BaseAddress = (UINT8*)(UINTN)PeiNvmeDev->MemBase + 0x14;
  Register    = *(UINT32*)(UINTN)BaseAddress;
  Register    &= ~0x01;
  *(UINT32*)(UINTN)BaseAddress = Register;

  PeiNvmeDev->StallPpi->Stall (
                          (CONST EFI_PEI_SERVICES**)PeiServices,
                          PeiNvmeDev->StallPpi,
                          500 * 1000
                          );

  PciAnd8 ((UINTN)PeiNvmeDev->PciCfgAddress[0] | PCI_COMMAND_OFFSET, (UINT8)(0xF9));

  DEBUG ((EFI_D_INFO, "[%a] Exit\n", __FUNCTION__));

  return EFI_SUCCESS;

}
VOID
UpdateAboveP2pBridge (
  IN BOOLEAN      IncreaseResource
  )
{

  UINT16 Range;
  UINT8  Index;
  UINTN  GppPciAdr;
  UINT8  SubBus;

  Range     = 0;
  Index     = 0;
  GppPciAdr = 0;
  SubBus    = 0;

  DEBUG ((EFI_D_INFO, "          [%a]Entry\n", __FUNCTION__));

  for (Index = 0 ; Index < mTempGppIndex ; Index++) {
    GppPciAdr = mTempGppPciAdrBuffer[Index];
    if (GppPciAdr == 0) {
      continue;
    }

    DEBUG ((
      EFI_D_INFO,
      "            Org Bus 0x%02x, 0x%02x, 0x%02x, Range = 0x%x ~ 0x%x\n",
      PciRead8 ((UINTN)(GppPciAdr | PCI_BRIDGE_PRIMARY_BUS_REGISTER_OFFSET)),
      PciRead8 ((UINTN)(GppPciAdr | PCI_BRIDGE_SECONDARY_BUS_REGISTER_OFFSET)),
      PciRead8 ((UINTN)(GppPciAdr | PCI_BRIDGE_SUBORDINATE_BUS_REGISTER_OFFSET)),
      PciRead16 ((UINTN)(GppPciAdr | 0x20)),
      PciRead16 ((UINTN)(GppPciAdr | 0x22))
      ));

    Range  = PciRead16 ((UINTN)(GppPciAdr | 0x22));
    SubBus = PciRead8  ((UINTN)(GppPciAdr | PCI_BRIDGE_SUBORDINATE_BUS_REGISTER_OFFSET));
    if (IncreaseResource) {
      Range += (UINT16)(PCIE_BRIDGE_RESOUCE_LENGTH >> 16);
      SubBus++;
    } else {
      Range -= (UINT16)(PCIE_BRIDGE_RESOUCE_LENGTH >> 16);
      SubBus--;
    }

    PciWrite16 ((UINTN)(GppPciAdr | 0x22), Range);
    PciWrite8  ((UINTN)(GppPciAdr | PCI_BRIDGE_SUBORDINATE_BUS_REGISTER_OFFSET), SubBus);

    DEBUG ((
      EFI_D_INFO,
      "            Mod Bus 0x%02x, 0x%02x, 0x%02x, Range = 0x%x ~ 0x%x\n",
      PciRead8 ((UINTN)(GppPciAdr | PCI_BRIDGE_PRIMARY_BUS_REGISTER_OFFSET)),
      PciRead8 ((UINTN)(GppPciAdr | PCI_BRIDGE_SECONDARY_BUS_REGISTER_OFFSET)),
      PciRead8 ((UINTN)(GppPciAdr | PCI_BRIDGE_SUBORDINATE_BUS_REGISTER_OFFSET)),
      PciRead16 ((UINTN)(GppPciAdr | 0x20)),
      PciRead16 ((UINTN)(GppPciAdr | 0x22))
      ));
  }


  DEBUG ((EFI_D_INFO, "          [%a]Exit\n", __FUNCTION__));

  return;

}

EFI_STATUS
SearchChildDevice (
  IN CONST EFI_PEI_SERVICES      **PeiServices,
  IN OUT UINT8                   *Bus,
  IN OUT UINT32                  *MmioResource
  )
{
  EFI_STATUS    Status;
  EFI_STATUS    InitP2pStatus;
  BOOLEAN       AvailableDevFound;
  UINT8         ChildFunc;
  UINT8         ChildDev;
  UINT32        Value32;
  UINT8         LastBus;
  UINT8         OrgBus;
  UINT32        OrgMmioRes;
  UINT8         XHCIControllersNumber;
  UINT8         MaxXHCIControllerNumbers;

  Status                = EFI_NOT_FOUND;
  InitP2pStatus         = EFI_NOT_FOUND;
  AvailableDevFound     = FALSE;
  ChildFunc             = 0;
  ChildDev              = 0;
  Value32               = 0;
  LastBus               = 0;
  OrgBus                = (*Bus);
  OrgMmioRes            = 0;
  XHCIControllersNumber = 0;

  DEBUG ((EFI_D_INFO, "        [%a]Entry, MMIO = 0x%x, Bus = 0x%x\n", __FUNCTION__, (*MmioResource), (*Bus)));
  
  if (CheckRvOpnB4()) {
    MaxXHCIControllerNumbers = MAX_XHCI_CONTROLLERS;
    DEBUG ((EFI_D_INFO, "            ** RV1: Max XHCI controller number: 0x%x \n", MaxXHCIControllerNumbers));
  } else {
    MaxXHCIControllerNumbers = MAX_XHCI_CONTROLLERS_RV2; 
    DEBUG ((EFI_D_INFO, "            ** RV2: Max XHCI controller number: 0x%x \n", MaxXHCIControllerNumbers));
  }

  //
  // Search XHCI, AHCI and NVME controller.
  //
  for (ChildDev = 0 ; ChildDev <= MAX_CHILD_DEVICE_NUMBER ; ChildDev++) {
    for (ChildFunc = 0 ; ChildFunc <= MAX_CHILD_FUNCTION_NUMBER ; ChildFunc++) {
      Value32 = PciRead32 ((UINTN)PCI_LIB_ADDRESS (OrgBus, ChildDev, ChildFunc, CLASSCODE_REGISTER));
      Value32 &= 0xFFFFFF00;

      switch (Value32) {
      
      case VGA_CLASSCODE:
           DEBUG ((EFI_D_INFO, "            ** Found VGA Device (B%xD%xF%x) ** \n", OrgBus, ChildDev, ChildFunc));     
           Status = InitVgaController (PeiServices, (UINTN)PCI_LIB_ADDRESS (OrgBus, ChildDev, ChildFunc, 0), MmioResource);    
           if (!EFI_ERROR (Status)) {
             AvailableDevFound = TRUE;
           }                  
           break;

      case XHCI_CLASSCODE:
           DEBUG ((EFI_D_INFO, "            ** Found Xhci Device (B%xD%xF%x) ** \n", OrgBus, ChildDev, ChildFunc));
           mUsbHcPciAdrBuffer[XHCIControllersNumber] = (UINTN)PCI_LIB_ADDRESS (OrgBus, ChildDev, ChildFunc, 0);
           XHCIControllersNumber++;
           if (XHCIControllersNumber == MaxXHCIControllerNumbers) {
             Status = InitXhciController (PeiServices, mUsbHcPciAdrBuffer, MmioResource, XHCIControllersNumber);
             if (!EFI_ERROR (Status)) {
               AvailableDevFound = TRUE;
             }
           }
           break;

      //
      // TODO: NOT Ready.
      //
  //    case IDE_CLASSCODE:
      case AHCI_CLASSCODE:
           DEBUG ((EFI_D_INFO, "            ** Found Ahci Device (B%xD%xF%x) ** \n", OrgBus, ChildDev, ChildFunc));
           Status = InitSataController (PeiServices, (UINTN)PCI_LIB_ADDRESS (OrgBus, ChildDev, ChildFunc, 0), MmioResource);
           if (!EFI_ERROR (Status)) {
             AvailableDevFound = TRUE;
           }
           break;

      case NVME_CLASSCODE:
           DEBUG ((EFI_D_INFO, "            ** Found Nvme Device (B%xD%xF%x) ** \n", OrgBus, ChildDev, ChildFunc));
           Status = InitNvmeController (PeiServices, (UINTN)PCI_LIB_ADDRESS (OrgBus, ChildDev, ChildFunc, 0), MmioResource);
           if (!EFI_ERROR (Status)) {
             AvailableDevFound = TRUE;
           }
           break;

      case PCI_TO_PCI_BRIDGE_CLASSCODE:
           DEBUG ((EFI_D_INFO, "            ** Found P2p Bridge (B%xD%xF%x) ** \n", OrgBus, ChildDev, ChildFunc));
           LastBus    = (*Bus) + 1;
           OrgMmioRes = (*MmioResource);

           //
           // For sub gpp init, it must be 1MB alignment.
           //
           if (((*MmioResource) & (UINT32)(SIZE_1MB - 1)) != 0) {
               (*MmioResource) += (UINT32)SIZE_1MB;
               (*MmioResource) &= (UINT32)0xFFF00000;
             DEBUG ((EFI_D_INFO, "            For Sub-Gpp Init, Adjust MMIO = 0x%x\n", (*MmioResource)));
           }
           InitP2pStatus = InitP2pBridge (
                             PeiServices,
                             &LastBus,
                             (UINTN)PCI_LIB_ADDRESS (OrgBus, ChildDev, ChildFunc, 0),
                             MmioResource
                             );
           if (!EFI_ERROR (InitP2pStatus)) {
             AvailableDevFound = TRUE;
             (*Bus) = LastBus;
           } else {
             (*MmioResource) = OrgMmioRes;
           }
           break;

      default:
           break;
      }
    }
  }

  if (AvailableDevFound) {
    //
    // Due to available device found with this layer,
    // so return EFI_SUCCES to keep resource of the above bridge.
    //
    Status = EFI_SUCCESS;
  }

  DEBUG ((EFI_D_INFO, "        [%a]Exit, MMIO = 0x%x, Bus = 0x%x, (%r)\n", __FUNCTION__, (*MmioResource), (*Bus), Status));

  return Status;

}

EFI_STATUS
InitP2pBridge (
  IN CONST EFI_PEI_SERVICES      **PeiServices,
  IN OUT UINT8                   *Bus,
  IN UINTN                       CurrentGppPciAdr,
  IN OUT UINT32                  *MmioResource
  )
{
  EFI_STATUS  Status;
  UINT8       OrgPBus;
  UINT8       OrgBus;
  UINT32      OrgMmioRes;

  Status     = EFI_NOT_FOUND;
  OrgPBus    = 0;
  OrgBus     = 0;
  OrgMmioRes = (*MmioResource);

  DEBUG ((
    EFI_D_INFO,
    "    [%a]Entry, Bus = 0x%x, MMIO = 0x%x\n",
    __FUNCTION__,
    (*Bus),
    (*MmioResource)
    ));

  //
  // For gpp init, it must be 1MB alignment.
  //
  if (((*MmioResource) & (UINT32)(SIZE_1MB - 1)) != 0) {
      (*MmioResource) += (UINT32)SIZE_1MB;
      (*MmioResource) &= (UINT32)0xFFF00000;
    DEBUG ((EFI_D_INFO, "      For Gpp Init, Adjust MMIO = 0x%x\n", (*MmioResource)));
  }

  //
  // Find P2P bridge and init it.
  //  1. Update the above P2p bridge.
  //  2. Assign bus number.
  //  3. Assign MMIO (Prepare 1MB).
  //  4. Enable P2P bridge.
  //
  UpdateAboveP2pBridge (TRUE);

  OrgPBus = PciRead8 ((UINTN)(CurrentGppPciAdr | PCI_BRIDGE_PRIMARY_BUS_REGISTER_OFFSET));
  OrgBus  = PciRead8 ((UINTN)(CurrentGppPciAdr | PCI_BRIDGE_SECONDARY_BUS_REGISTER_OFFSET));

  PciWrite8 ((UINTN)(CurrentGppPciAdr | PCI_BRIDGE_PRIMARY_BUS_REGISTER_OFFSET), (UINT8)(CurrentGppPciAdr >> 20));
  PciWrite8 ((UINTN)(CurrentGppPciAdr | PCI_BRIDGE_SECONDARY_BUS_REGISTER_OFFSET),   (*Bus));
  PciWrite8 ((UINTN)(CurrentGppPciAdr | PCI_BRIDGE_SUBORDINATE_BUS_REGISTER_OFFSET), (*Bus));
  PciWrite16 ((UINTN)(CurrentGppPciAdr | 0x20), ((*MmioResource) >> 16));  

  //
  // Search child device.
  //
  mTempGppPciAdrBuffer[mTempGppIndex++] = (UINTN)CurrentGppPciAdr;
  Status = SearchChildDevice (
             PeiServices,
             (UINT8*)Bus,
             MmioResource
             );
  mTempGppPciAdrBuffer[--mTempGppIndex] = (UINTN)0;

  PciWrite16 ((UINTN)(CurrentGppPciAdr | 0x22), ((*MmioResource) >> 16));
  //
  // PCIE bridge 0:8:1 has two XHCI controllers.
  //
  if (CurrentGppPciAdr == PCI_LIB_ADDRESS (0x00, 0x08, 0x01, 0x00)) {
    PciWrite8 (
      (UINTN)(CurrentGppPciAdr | PCI_COMMAND_OFFSET),
      (UINT8)(EFI_PCI_COMMAND_MEMORY_SPACE | EFI_PCI_COMMAND_BUS_MASTER|EFI_PCI_COMMAND_IO_SPACE)
      );  
  } else {
    PciWrite8 (
      (UINTN)(CurrentGppPciAdr | PCI_COMMAND_OFFSET),
      (UINT8)(EFI_PCI_COMMAND_MEMORY_SPACE | EFI_PCI_COMMAND_BUS_MASTER)
      );  
    }

  if (EFI_ERROR (Status)) {
    //
    // Could NOT find out device under this P2P bridge, So,
    //  1. Disable P2P bridge.
    //  2. Revert MMIO resouce.
    //  3. Restore bus number.
    //  4. Update the above P2p bridge.
    //
    DEBUG ((EFI_D_ERROR, "      == SearchChildDevice Error!!, revert resource. (%r) ==\n", Status));
    PciWrite8  ((UINTN)(CurrentGppPciAdr | PCI_COMMAND_OFFSET), 0);

    PciWrite16 ((UINTN)(CurrentGppPciAdr | 0x20), 0);
    PciWrite16 ((UINTN)(CurrentGppPciAdr | 0x22), 0);
    (*MmioResource) = OrgMmioRes;

    PciWrite8 ((UINTN)(CurrentGppPciAdr | PCI_BRIDGE_SUBORDINATE_BUS_REGISTER_OFFSET), OrgBus);
    PciWrite8 ((UINTN)(CurrentGppPciAdr | PCI_BRIDGE_SECONDARY_BUS_REGISTER_OFFSET),   OrgBus);
    PciWrite8 ((UINTN)(CurrentGppPciAdr | PCI_BRIDGE_PRIMARY_BUS_REGISTER_OFFSET),    OrgPBus);

    UpdateAboveP2pBridge (FALSE);
  } else {
    //
    // Keep resouce.
    //
    DEBUG ((
      EFI_D_INFO,
      "      Log GPP - [%d]0x%x (B%xD%xF%x)\n",
      mCurrentGppCounter,
      (UINT32)CurrentGppPciAdr,
      (UINT8)((CurrentGppPciAdr >> 20) & 0xFF),
      (UINT8)((CurrentGppPciAdr >> 15) & 0x1F),
      (UINT8)((CurrentGppPciAdr >> 12) & 0x07)
      ));
    mGppPciAdrBuffer[mCurrentGppCounter++] = (UINTN)(CurrentGppPciAdr);
    (*Bus) = PciRead8 ((UINTN)(CurrentGppPciAdr | PCI_BRIDGE_SUBORDINATE_BUS_REGISTER_OFFSET)) + 1;
    (*MmioResource) = (UINT32)((PciRead16 ((UINTN)(CurrentGppPciAdr | 0x22)) + 0x10) << 16);
  }

  DEBUG ((EFI_D_INFO, "    [%a]Exit, Bus = 0x%x, MMIO = 0x%x, (%r)\n", __FUNCTION__, (*Bus), (*MmioResource), Status));

  return Status;

}


EFI_STATUS
RecoveryInitGppPortDevice (
  IN CONST EFI_PEI_SERVICES      **PeiServices,
  IN OUT UINT32                  *MmioResource
  )
{
  EFI_STATUS                Status;
  UINT8                     *BridgeDev;
  UINT8                     Fun;
  UINT32                    Register;
  UINT8                     Bus;

  DEBUG ((EFI_D_INFO, "  [%a]Entry, MMIO = 0x%x\n", __FUNCTION__, (*MmioResource)));

  BridgeDev          = mGppPortDeviceNumber;
  Bus                = TEMP_BUS_NUMBER;
  mCurrentGppCounter = 0;

  for (; *BridgeDev != 0xFF; BridgeDev++) {
    for (Fun = 0; Fun <= MAX_FUNCTION_NUMBER; Fun++) {
      //
      // Find all PCI to PCI bridge.
      //
      Register = PciRead32 ((UINTN)PCI_LIB_ADDRESS (0x00, *BridgeDev, Fun, CLASSCODE_REGISTER));
      Register &= 0xffffff00;
      if (Register != PCI_TO_PCI_BRIDGE_CLASSCODE) {
        continue;
      }

      DEBUG ((EFI_D_INFO, "    Found P2P Bridge, B0D%xF%x\n", (UINT8)*BridgeDev, Fun));

      mTempGppIndex = 0;

      Status = InitP2pBridge (
                 PeiServices,
                 (UINT8*)&Bus,
                 (UINTN)(PCI_LIB_ADDRESS (0x00, *BridgeDev, Fun, 0)),
                 (UINT32*)MmioResource
                 );
    }
  }

  if (PcdGetBool (PcdUseFastCrisisRecovery)) {
    Status = (**PeiServices).NotifyPpi (PeiServices, &mRemoveGppResourceNotify);
  }

  DEBUG ((EFI_D_INFO, "  [%a]Exit, MMIO = 0x%x\n", __FUNCTION__, (*MmioResource)));

  return EFI_SUCCESS;

}
