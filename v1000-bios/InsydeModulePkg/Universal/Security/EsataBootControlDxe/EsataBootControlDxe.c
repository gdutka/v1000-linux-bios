/** @file
  Driver to control ESATA boot options

;******************************************************************************
;* Copyright (c) 2021, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <Protocol/AtaPassThru.h>
#include <Protocol/DevicePath.h>
#include <Protocol/PciIo.h>
#include <Protocol/H2OBdsServices.h>

#include <Guid/EventGroup.h>
#include <Guid/H2OCp.h>

#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/ReportStatusCodeLib.h>
#include <Library/PrintLib.h>
#include <Library/PcdLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DevicePathLib.h>
#include <Library/H2OCpLib.h>
#include <Library/VariableLib.h>

#include <KernelSetupConfig.h>
#include <IndustryStandard/Atapi.h>

#define EFI_AHCI_PORT_START                    0x0100
#define EFI_AHCI_PORT_REG_WIDTH                0x0080
#define EFI_AHCI_BAR_INDEX                     0x05
#define EFI_AHCI_PORT_CMD                      0x0018
#define   EFI_AHCI_PORT_CMD_ST_MASK            0xFFFFFFFE
#define   EFI_AHCI_PORT_CMD_ST                 BIT0
#define   EFI_AHCI_PORT_CMD_SUD                BIT1
#define   EFI_AHCI_PORT_CMD_POD                BIT2
#define   EFI_AHCI_PORT_CMD_COL                BIT3
#define   EFI_AHCI_PORT_CMD_CR                 BIT15
#define   EFI_AHCI_PORT_CMD_FRE                BIT4
#define   EFI_AHCI_PORT_CMD_FR                 BIT14
#define   EFI_AHCI_PORT_CMD_PMA                BIT17
#define   EFI_AHCI_PORT_CMD_HPCP               BIT18
#define   EFI_AHCI_PORT_CMD_MPSP               BIT19
#define   EFI_AHCI_PORT_CMD_CPD                BIT20
#define   EFI_AHCI_PORT_CMD_ESP                BIT21
#define   CONTROLLED_BY_SETUP_OPTION              0
#define   CONTROLLED_BY_PCD                      1

/**

  Read registers from AHCI MMIO
  @param[in]        PciIo               instance of Pciio protocol
  @param[in]        Offset              location of the register

**/
UINT32
EFIAPI
AhciReadReg (
  IN EFI_PCI_IO_PROTOCOL  *PciIo,
  IN  UINT32              Offset
  )
{
  UINT32                  Data;

  ASSERT (PciIo != NULL);

  Data = 0;

  PciIo->Mem.Read (
               PciIo,
               EfiPciIoWidthUint32,
               EFI_AHCI_BAR_INDEX,
               (UINT64) Offset,
               1,
               &Data
               );

  return Data;
}


/**

  Check Esata setup option and Pcd to decide if Esata device is allowed to boot
  @param[in]        Event               Pointer to this event
  @param[in]        Handle              The handle associated with a previously registered checkpoint handler.

**/
VOID
H2OBdsCpBootDeviceSelectCallback (
  IN EFI_EVENT                              Event,
  IN H2O_CP_HANDLE                          Handle
  )
{
  EFI_STATUS                             Status;
  H2O_BDS_CP_BOOT_DEVICE_SELECT_DATA     *BdsBootDeviceSelectData;
  LIST_ENTRY                             *Link;
  LIST_ENTRY                             *ExpandLink;
  H2O_BDS_LOAD_OPTION                    *BootOption;
  LIST_ENTRY                             *BootLists;
  H2O_BDS_LOAD_OPTION                    *ExpandedBootOption;
  EFI_DEVICE_PATH_PROTOCOL              *TmpDevicePath;  
  EFI_HANDLE                            PciHandle;
  SATA_DEVICE_PATH                      *SataDevPath;
  EFI_PCI_IO_PROTOCOL                   *PciIo;  
  UINT32                                Value;
  UINT32                                Offset; 
  KERNEL_CONFIGURATION                  *SystemConfiguration;
  
  BootLists = NULL;
  
  if (PcdGetBool (PcdH2OEsataDriveBootPolicy)) {
    SystemConfiguration = CommonGetVariableData (SETUP_VARIABLE_NAME, &gSystemConfigurationGuid);    
    if ((SystemConfiguration == NULL) || (SystemConfiguration->EsataDriveBootControl)) {
      return;
    }
  } else {
     if (FeaturePcdGet (PcdH2OEsataDriveBootControl)) {
       return;
     }
  } 

  Status = H2OCpLookup (Handle, (VOID **) &BdsBootDeviceSelectData, &gH2OBdsCpBootDeviceSelectGuid);    
  BootLists = *(BdsBootDeviceSelectData->BootList);
  if (BootLists == NULL) {
    return;
  }

  for (Link = GetFirstNode (BootLists); Link != BootLists; Link = GetNextNode (BootLists, Link)) {
    BootOption = BDS_OPTION_FROM_LINK (Link);
    if (!BootOption->Expanded) {
      continue;
    }
    
    for (ExpandLink = GetFirstNode (&BootOption->ExpandedLoadOptions); ExpandLink != &(BootOption->ExpandedLoadOptions); ExpandLink = GetNextNode (&BootOption->ExpandedLoadOptions, ExpandLink)) {
      ExpandedBootOption = BDS_OPTION_FROM_LINK (ExpandLink); 
      TmpDevicePath = ExpandedBootOption->DevicePath;
      Status = gBS->LocateDevicePath(&gEfiPciIoProtocolGuid, &TmpDevicePath, &PciHandle);
      SataDevPath = NULL;
      if (!EFI_ERROR(Status)) {
        Status = gBS->HandleProtocol (
                        PciHandle,
                        &gEfiPciIoProtocolGuid,
                        (VOID **)&PciIo
                        );
        while (!IsDevicePathEnd (TmpDevicePath)) {
          if (TmpDevicePath->Type == MESSAGING_DEVICE_PATH && TmpDevicePath->SubType == MSG_SATA_DP) {
            SataDevPath = (SATA_DEVICE_PATH *) TmpDevicePath;
            break;
          } 
          TmpDevicePath = NextDevicePathNode (TmpDevicePath);
        }
    
        Value = 0;
        if (SataDevPath != NULL) {
          Offset = EFI_AHCI_PORT_START + SataDevPath->HBAPortNumber * EFI_AHCI_PORT_REG_WIDTH + EFI_AHCI_PORT_CMD;
          Value = AhciReadReg(PciIo, Offset);
          //
          // Check if the port that SATA device attached to is ESATA enabled, if YES, disable the boot option
          //
          if (Value & EFI_AHCI_PORT_CMD_ESP) {
            BootOption->Attributes &= !LOAD_OPTION_ACTIVE;
          }
        }
      }
    }
  }

}


/**
  Entry point of Smart Status Code Driver.

  This function is the entry point of this DXE Status Code Driver.
  It initializes registers status code handlers, and registers event for EVT_SIGNAL_VIRTUAL_ADDRESS_CHANGE.

  @param  ImageHandle       The firmware allocated handle for the EFI image.
  @param  SystemTable       A pointer to the EFI System Table.

  @retval EFI_SUCCESS       The entry point is executed successfully.

**/
EFI_STATUS
EFIAPI
EsataBootControlDxeEntryPoint (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS                          Status;

  if (FeaturePcdGet (PcdH2OBdsCpBootDeviceSelectSupported)) {
    H2O_CP_HANDLE                         H2OBdsCpBootDeviceSelectHandle;
    //
    // Register for callback on Storage Security Protocol publication
    //
    H2OBdsCpBootDeviceSelectHandle = NULL;
    Status = H2OCpRegisterHandler (
               &gH2OBdsCpBootDeviceSelectGuid,
               H2OBdsCpBootDeviceSelectCallback,
               H2O_CP_MEDIUM_LOW,
               &H2OBdsCpBootDeviceSelectHandle
               );
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "Checkpoint Register Fail: %g (%r)\n", &gH2OBdsCpBootDeviceSelectGuid, Status));
      return Status;
    }
    DEBUG ((DEBUG_INFO, "Checkpoint Registered: %g (%r)\n", &gH2OBdsCpBootDeviceSelectGuid, Status));
  }    

  return EFI_SUCCESS;
}
