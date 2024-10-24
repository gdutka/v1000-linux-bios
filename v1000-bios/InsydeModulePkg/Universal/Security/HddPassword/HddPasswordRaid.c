/** @file
  RAID supporting in HDD Password Protocol

;******************************************************************************
;* Copyright (c) 2012 - 2018, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "HddPassword.h"

extern DRIVER_INSTALL_INFO              *mDriverInstallInfo;
//
// Port register
//
#define AHCI_PORT_BASE_X(Abar, port)    (Abar + HBA_PORTS_START + HBA_PORTS_REG_WIDTH * port)
#define AHCI_PORT_FIS_X(Abar, port)     (AHCI_PORT_BASE_X(Abar, port) + 0x08)
#define AHCI_PORT_CMD_X(Abar, port)     (AHCI_PORT_BASE_X(Abar, port) + 0x18)

EFI_LEGACY_BIOS_INSTALL_ROM              mOriginalInstallPciRom;
/**
  This function either writes to or read from AHCI BAR register.

  @param[in]        SaveRestoreFlag     True: write data to AHCI BAR registers.
                                        False: read data from AHCI BAR registers to global registers.
  @param[in]        AtaControllerInfo   Private controller data

  @retval           EFI_SUCCESS         if read or write is successful.
**/
EFI_STATUS
SaveRestoreAbar (
  IN  BOOLEAN                           SaveRestoreFlag,
  IN  ATA_CONTROLLER_INFO               *AtaControllerInfo
  )
{
  UINT8                                 Index;
  UINTN                                 Abar;
  UINTN                                 Seg;
  UINTN                                 Bus;
  UINTN                                 Dev;
  UINTN                                 Func;
  UINTN                                 NumOfPorts;
  UINTN                                 NumOfBackupPorts;
  EFI_PCI_IO_PROTOCOL                   *PciIo;
  PORT_NUMBER_MAP                       *PortMappingTable;
  PORT_NUMBER_MAP                       EndEntry;
  UINT32                                *AhciAbarIoSave;

  Abar  = 0;
  Seg   = 0;
  Bus   = 0;
  Dev   = 0;
  Func  = 0;

  NumOfBackupPorts   = 0;
  NumOfPorts         = 0;
  PortMappingTable   = NULL;
  AhciAbarIoSave     = NULL;
  PciIo = AtaControllerInfo->RaidSetupInfo.PciIo;
  
  PciIo->GetLocation (
          PciIo,
          &Seg,
          &Bus,
          &Dev,
          &Func
          );

  if (SaveRestoreFlag) {
    //
    //  ready to Restore "Port Registers"
    //
    AhciAbarIoSave   = AtaControllerInfo->RaidSetupInfo.AhciAbarIoSave;
    NumOfBackupPorts = AtaControllerInfo->RaidSetupInfo.NumOfBackupPorts;
  } else {
    //
    //  ready to Save "Port Registers"
    //

    PortMappingTable = NULL;

    ZeroMem (&EndEntry, sizeof (PORT_NUMBER_MAP));

    PortMappingTable = (PORT_NUMBER_MAP *)PcdGetPtr (PcdPortNumberMapTable);

    NumOfPorts = 0;
    while (CompareMem (&EndEntry, &PortMappingTable[NumOfPorts], sizeof (PORT_NUMBER_MAP)) != 0) {
      NumOfPorts++;
    }

    if (NumOfPorts == 0) {
      return FALSE;
    }

    for (Index = 0; Index < NumOfPorts; Index++) {
      if (PortMappingTable[Index].Bus      == Bus &&
          PortMappingTable[Index].Device   == Dev) {
        NumOfBackupPorts++;
      }
    }


    AhciAbarIoSave = AllocateBuffer (NumOfBackupPorts * sizeof (UINT32) * 3);
    AtaControllerInfo->RaidSetupInfo.AhciAbarIoSave   = AhciAbarIoSave;
    AtaControllerInfo->RaidSetupInfo.NumOfBackupPorts = NumOfBackupPorts;
  }

  if (AhciAbarIoSave == NULL) {
    //
    // check the save buffer
    //

    return EFI_OUT_OF_RESOURCES;
  }

  PciIo->Pci.Read (
               PciIo,
               EfiPciIoWidthUint32,
               PCI_AHCI_BAR,
               sizeof(UINT32),
               &Abar
               );

  for (Index = 0; Index < NumOfBackupPorts; Index++) {
    if (SaveRestoreFlag) {
      *((volatile UINT32*) AHCI_PORT_BASE_X (Abar, Index)) = AhciAbarIoSave[Index * 3];
      *((volatile UINT32*) AHCI_PORT_FIS_X (Abar, Index))  = AhciAbarIoSave[Index * 3 + 1];
      *((volatile UINT32*) AHCI_PORT_CMD_X (Abar, Index))  = AhciAbarIoSave[Index * 3 + 2];
    } else {
      AhciAbarIoSave[Index * 3]     = *((volatile UINT32*) AHCI_PORT_BASE_X (Abar, Index));
      AhciAbarIoSave[Index * 3 + 1] = *((volatile UINT32*) AHCI_PORT_FIS_X (Abar, Index));
      AhciAbarIoSave[Index * 3 + 2] = *((volatile UINT32*) AHCI_PORT_CMD_X (Abar, Index));
    }

  }

  if (SaveRestoreFlag) {
    FreeBuffer (AhciAbarIoSave);
    AtaControllerInfo->RaidSetupInfo.AhciAbarIoSave = NULL;
  }

  return  EFI_SUCCESS;
}

/**
  Check RAID OpRom.

  @param[in]        Pci                 Point to Type00 Pci configuration header
  @param[in]        Bus                 PCI bus number
  @param[in]        Device              PCI device number
  @param[in]        Function            PCI function number
  @param[in]        Index               Number of device

  @retval           TRUE                Is RAID OpRom.
  @retval           FALSE               Not RAID OpRom. (If no support this,please return "False")
**/
BOOLEAN
IsRaidOpRom (
  IN VOID                               *Pci,
  IN UINTN                              Bus,
  IN UINTN                              Device,
  IN UINTN                              Function,
  IN UINTN                              Index
  )
{
  if (IsOnBoardPciDevice((UINT32)Bus, (UINT32)Device, (UINT32)Function)) {
    if (IS_PCI_RAID ((PCI_TYPE00*)Pci) && Index != 0) {
      return TRUE;
    }
  }

  return FALSE;
}

/**
  Hook Legacy service "InstallPciRom" to delay install RAID OpROM

  @param[in]        This                Protocol instance pointer.
  @param[in]        PciHandle           The PCI PC-AT OPROM from this devices ROM BAR will be loaded.
                                        This value is NULL if RomImage is non-NULL. This is the normal
                                        case.
  @param[in]        RomImage            A PCI PC-AT ROM image. This argument is non-NULL if there is
                                        no hardware associated with the ROM and thus no PciHandle,
                                        otherwise is must be NULL.
                                        Example is PXE base code.
  @param[in]        Flags               Return Status if ROM was found and if was Legacy OPROM.
  @param[in]        DiskStart           Disk number of first device hooked by the ROM. If DiskStart
                                        is the same as DiskEnd no disked were hooked.
  @param[in]        DiskEnd             Disk number of the last device hooked by the ROM.
  @param[in]        RomShadowAddress    Shadow address of PC-AT ROM
  @param[in]        RomShadowSize       Size of RomShadowAddress in bytes

  @retval                               By InstallPciRom
**/
EFI_STATUS
EFIAPI
HookInstallPciRom (
  IN  EFI_LEGACY_BIOS_PROTOCOL          *This,
  IN  EFI_HANDLE                        PciHandle,
  IN  VOID                              **RomImage,
  OUT UINTN                             *Flags,
  OUT UINT8                             *DiskStart, OPTIONAL
  OUT UINT8                             *DiskEnd, OPTIONAL
  OUT VOID                              **RomShadowAddress, OPTIONAL
  OUT UINT32                            *ShadowedRomSize OPTIONAL
  )
{
  EFI_STATUS                            Status;
  EFI_PCI_IO_PROTOCOL                   *PciIo;
  PCI_TYPE00                            Pci;
  UINTN                                 Device;
  UINTN                                 Seg;
  UINTN                                 Bus;
  UINTN                                 Function;
  ATA_CONTROLLER_INFO                   *AtaControllerInfo;
  DRIVER_INSTALL_INFO                   *DriverInstallInfo;
  LIST_ENTRY                            *AtaControllerInfoListHead;
  LIST_ENTRY                            *Link;

  DriverInstallInfo = mDriverInstallInfo;


  Status = gBS->HandleProtocol (
                  PciHandle,
                  &gEfiPciIoProtocolGuid,
                  (VOID **)&PciIo
                  );

  if (EFI_ERROR (Status)) {
    goto EXIT;
  }
  
  Status = PciIo->GetLocation (
                    PciIo,
                    &Seg,
                    &Bus,
                    &Device,
                    &Function
                    );
  if (EFI_ERROR (Status)) {
    goto EXIT;
  }
  
  Status = PciIo->Pci.Read (
                        PciIo,
                        EfiPciIoWidthUint32,
                        0,
                        sizeof (Pci) / sizeof (UINT32),
                        &Pci
                        );
  if (EFI_ERROR(Status) || !IS_PCI_RAID (&Pci) || 
        !IsRaidOpRom (&Pci ,Bus, Device, Function, 1)) {
    goto EXIT;
  }

  //
  // Surf through different ATA controllers 
  //
  AtaControllerInfoListHead = &DriverInstallInfo->AtaControllerInfoListHead;
  for (Link = GetFirstNode (AtaControllerInfoListHead);
       !IsNull (AtaControllerInfoListHead, Link);
       Link = GetNextNode (AtaControllerInfoListHead, Link)) {
    AtaControllerInfo = (ATA_CONTROLLER_INFO *)Link;
    if (AtaControllerInfo->AtaMode != PCI_SUB_CLASS_RAID) {
      continue;
    }

    //
    // Find the correct one and save Abar
    //
    if (AtaControllerInfo->PciSeg == Seg &&
        AtaControllerInfo->PciBus == Bus &&
        AtaControllerInfo->PciDevice == Device &&
        AtaControllerInfo->PciFunction == Function) {
        AtaControllerInfo->RaidSetupInfo.RaidController = PciHandle;
        AtaControllerInfo->RaidSetupInfo.PciIo          = PciIo;
        SaveRestoreAbar (FALSE, AtaControllerInfo);
    }
  }
        
EXIT:
  return mOriginalInstallPciRom (
           This,
           PciHandle,
           RomImage,
           Flags,
           DiskStart,
           DiskEnd,
           RomShadowAddress,
           ShadowedRomSize
           );
}

/**
  To set hook InstallPciRom services by event.

  @param[in]        Event               Pointer to this event
  @param[in]        Context             Event hanlder private data

  @retval           EFI_SUCCESS

**/
EFI_STATUS
SetHookInstallPciRom (
  IN EFI_EVENT                          Event,
  IN VOID                               *Context
  )
{
  EFI_STATUS                            Status;
  EFI_LEGACY_BIOS_PROTOCOL              *LegacyBios;


  Status = gBS->LocateProtocol (
                  &gEfiLegacyBiosProtocolGuid,
                  NULL,
                  (VOID **)&LegacyBios
                  );

  if (!EFI_ERROR (Status)) {
    mOriginalInstallPciRom = LegacyBios->InstallPciRom;
    LegacyBios->InstallPciRom= HookInstallPciRom;
    mDriverInstallInfo->LegacyBios = LegacyBios;
  }

  return EFI_SUCCESS;
}

/**
  Process RAID support.

  @param


**/
VOID
ProcessLegacyRaidSupport (
  VOID
  )
{
  ATA_CONTROLLER_INFO                   *AtaControllerInfo;
  DRIVER_INSTALL_INFO                   *DriverInstallInfo;
  LIST_ENTRY                            *AtaControllerInfoListHead;
  LIST_ENTRY                            *Link;
  EFI_PCI_IO_PROTOCOL                   *PciIo;



  DriverInstallInfo = mDriverInstallInfo;

  //
  // Surf through different ATA controllers 
  //
  AtaControllerInfoListHead = &DriverInstallInfo->AtaControllerInfoListHead;
  for (Link = GetFirstNode (AtaControllerInfoListHead);
       !IsNull (AtaControllerInfoListHead, Link);
       Link = GetNextNode (AtaControllerInfoListHead, Link)) {
    AtaControllerInfo = (ATA_CONTROLLER_INFO *)Link;
    if (AtaControllerInfo->AtaMode != PCI_SUB_CLASS_RAID) {
      continue;
    }

    if (AtaControllerInfo->RaidSetupInfo.PciIo == NULL) {
      continue;
    }

    //
    // Find the correct one and save Abar
    //
    PciIo = AtaControllerInfo->RaidSetupInfo.PciIo;
    SaveRestoreAbar (TRUE, AtaControllerInfo);
  }
}

/**
  Init. RAID support.

  @param

  @retval           EFI_SUCCESS
**/
EFI_STATUS
InitLegacyRaidSupport (
  VOID
  )
{
  EFI_STATUS                            Status;
  EFI_LEGACY_BIOS_PROTOCOL              *LegacyBios;
  VOID                                  *RegistrationSetHook;
  EFI_EVENT                             SetHookLegacyProtocolEvent = NULL;


  Status = gBS->LocateProtocol (
                  &gEfiLegacyBiosProtocolGuid,
                  NULL,
                  (VOID **)&LegacyBios
                  );

  if (!EFI_ERROR (Status)) {

    //
    //  Hook InstallPciRom
    //
    mOriginalInstallPciRom = LegacyBios->InstallPciRom;
    LegacyBios->InstallPciRom= HookInstallPciRom;
    mDriverInstallInfo->LegacyBios = LegacyBios;
  } else {
    //
    //  Create event used for Hook InstallPciRom callback
    //  if HddPassword run before LegacyBios
    //
    Status = gBS->CreateEvent (
                    EVT_NOTIFY_SIGNAL,
                    TPL_CALLBACK,
                    (EFI_EVENT_NOTIFY)SetHookInstallPciRom,
                    NULL,
                    &SetHookLegacyProtocolEvent
                    );
    if (EFI_ERROR (Status)) {
      return Status;
    }
    //
    // Register for callback on SetHookLegacyProtocolEvent publication
    //
    Status = gBS->RegisterProtocolNotify (
                    &gEfiLegacyBiosProtocolGuid,
                    SetHookLegacyProtocolEvent,
                    &RegistrationSetHook
                    );
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }

  return EFI_SUCCESS;
}

