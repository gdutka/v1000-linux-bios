
/** @file
  CommonPciPlatformDxe driver will produces the PciPlatform Protocol for Dxe driver.
  If Platform layer already install this protocol, This driver will hook platform protocol.

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

#include <Uefi.h>
#include <IndustryStandard/Pci.h>
#include <Protocol/Bds.h>
#include <Protocol/PciIo.h>
#include <Protocol/PciPolicy.h>
#include <Protocol/PciPlatform.h>
#include <Protocol/PciRootBridgeIo.h>
#include <Library/DebugLib.h>
#include <Library/CommonPciLib.h>
#include <Library/PciExpressLib.h>
#include <Library/DxeServicesLib.h>
#include <Library/S3BootScriptLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DxeOemSvcKernelLib.h>
#include <Library/DxeChipsetSvcLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/VariableLib.h>
#include <Guid/ZeroGuid.h>
#include <Guid/H2OCp.h>
#include <Guid/AutoGenPciOptionRomList.h>
#include <Guid/H2OSetup.h>

#include <Library/FlashRegionLib.h>

#include <Library/PcdLib.h>
#include <Library/H2OCpLib.h>
#include <MultiBoard.h>
#include <KernelSetupConfig.h>
#define PCCARD_CARDBUS_LATENCY_TIMER_OFFSET 0x1B

EFI_STATUS
EFIAPI
CommonPlatformNotify (
  IN EFI_PCI_PLATFORM_PROTOCOL                        *This,
  IN EFI_HANDLE                                       HostBridge,
  IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PHASE    Phase,
  IN EFI_PCI_CHIPSET_EXECUTION_PHASE                  ChipsetPhase
  );

EFI_STATUS
EFIAPI
CommonPlatformPrepController (
  IN EFI_PCI_PLATFORM_PROTOCOL                      *This,
  IN EFI_HANDLE                                     HostBridge,
  IN EFI_HANDLE                                     RootBridge,
  IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_PCI_ADDRESS    PciAddress,
  IN EFI_PCI_CONTROLLER_RESOURCE_ALLOCATION_PHASE   Phase,
  IN EFI_PCI_CHIPSET_EXECUTION_PHASE                ChipsetPhase
  );

EFI_STATUS
EFIAPI
CommonGetPlatformPolicy (
  IN  CONST EFI_PCI_PLATFORM_PROTOCOL               *This,
  OUT       EFI_PCI_PLATFORM_POLICY                 *PciPolicy
  );

EFI_STATUS
EFIAPI
CommonGetPciRom (
  IN CONST EFI_PCI_PLATFORM_PROTOCOL                 *This,
  IN       EFI_HANDLE                                PciHandle,
  OUT      VOID                                      **RomImage,
  OUT      UINTN                                     *RomSize
  );

EFI_STATUS
ProgramLatencyTime (
  IN     UINT8                                       Bus,
  IN     UINT8                                       Device,
  IN     UINT8                                       Func,
  IN     PCI_POLICY                                  *PciPolicy
);

EFI_STATUS
ProgramPciePayLoad(
  IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_PCI_ADDRESS PciAddress,
  IN     UINT8                                       CapIdOffset,
  IN     PCI_POLICY                                  *PciPolicy
);

EFI_STATUS
GetSmallestMaxPayLoad(
  IN     UINT8                                       Bus,
  IN OUT UINT8                                       *SmallestPayLoadValue
);

EFI_STATUS
SetSmallestPayLoadValue (
  IN     UINT8                                       Bus,
  IN     UINT8                                       SmallestPayLoadValue
);

VOID
EFIAPI
CommonPciPlatformHookBdsCallback (
  IN EFI_EVENT                                       Event,
  IN VOID                                            *Context
);

VOID
EFIAPI
CommonPciPlatformBds (
  IN EFI_BDS_ARCH_PROTOCOL                           *Bds
);

VOID
CheckOpromId (
  IN  OUT VOID                                    **RomImage,
  IN      UINTN                                   *RomSize,
  IN      UINT16                                  VendorId,
  IN      UINT16                                  DeviceId
);

EFI_STATUS
ProcessPciOptionRom (
  IN     UINTN                                 Segment,
  IN     UINT8                                 Bus,
  IN     UINT8                                 Device,
  IN     UINT8                                 Function,
  IN     UINT16                                VendorId,
  IN     UINT16                                DeviceId,
  IN OUT VOID                                  **RomImage,
  IN OUT UINTN                                 *RomSize
);

EFI_STATUS
ProcessPciOptionRomBdf (
  IN     UINTN                                 Segment,
  IN     UINT8                                 Bus,
  IN     UINT8                                 Device,
  IN     UINT8                                 Function,
  IN     UINT16                                VendorId,
  IN     UINT16                                DeviceId,
  IN OUT VOID                                  **RomImage,
  IN OUT UINTN                                 *RomSize
);

EFI_STATUS
CheckBoardId (
  IN  BOOLEAN                               IsPciOptionRomBdf,
  IN  UINT8                                 *RegionId
);

EFI_STATUS
GetFromFv (
  IN     UINT16                             VendorId,
  IN     UINT16                             DeviceId,
  IN OUT VOID                               **RomImage,
  IN OUT UINTN                              *RomSize
);

EFI_PCI_PLATFORM_PROTOCOL mCommonPciPlatform = {
  CommonPlatformNotify,
  CommonPlatformPrepController,
  CommonGetPlatformPolicy,
  CommonGetPciRom
};

EFI_PCI_PLATFORM_PROTOCOL mPciPlatformHook = {
  NULL,
  NULL,
  NULL,
  NULL
};

EFI_HANDLE                mCommonPciPlatformHandle = NULL;
EFI_BDS_ENTRY             mOriginalBdsEntry = NULL;

PCI_OPTION_ROM_TABLE      *mPciOpRomTable;
UINTN                     mPciOpRomTableCnt;
UINTN                     mMaxOpRomTableCnt;

BOOLEAN                   mUsbOpRomTableInitialized = FALSE;
UINTN                     mUsbOpRomCount            = 0;
USB_OPROM_TABLE           *mUsbOpRomTable           = NULL;

/**
  Initialize USB option ROM table into module variable.

  @retval EFI_SUCCESS                  Initialize USB option ROM table successfully.
  @retval EFI_NOT_FOUND                Failed to get USB option ROM table or skip by oem service return status.
**/
STATIC
EFI_STATUS
InitializeUsbOpRomTable (
  VOID
  )
{
  EFI_STATUS                           Status;
  USB_OPROM_TABLE                      *UsbOpRomTable;
  UINTN                                UsbOpRomCount;

  mUsbOpRomTableInitialized = TRUE;

  UsbOpRomTable = NULL;
  DEBUG ((DEBUG_INFO, "OemKernelServices Call: OemSvcInstallOptionRomTable \n"));
  Status = OemSvcInstallOptionRomTable (USB_OPROM, (VOID **) &UsbOpRomTable);
  DEBUG ((DEBUG_INFO, "OemKernelServices OemSvcInstallOptionRomTable Status: %r\n", Status));
  if (Status != EFI_MEDIA_CHANGED || UsbOpRomTable == NULL) {
    return EFI_NOT_FOUND;
  }

  UsbOpRomCount = 0;
  while (!CompareGuid(&UsbOpRomTable[UsbOpRomCount].FileName, &gZeroGuid)) {
    UsbOpRomCount++;
  }
  if (UsbOpRomCount == 0) {
    return EFI_NOT_FOUND;
  }

  mUsbOpRomTable = UsbOpRomTable;
  mUsbOpRomCount = UsbOpRomCount;

  return EFI_SUCCESS;
}

/**
  Check if input file name belongs to USB option ROM file.

  @param[in] FileName                  Pointer to file name

  @retval TRUE                         Input file name belong to USB option ROM file.
  @retval EFI_NOT_FOUND                Input file name does not belong to USB option ROM file.
**/
STATIC
BOOLEAN
IsUsbOptionRom (
  IN EFI_GUID                          *FileName
  )
{
  UINTN                                Index;

  if (!mUsbOpRomTableInitialized) {
    InitializeUsbOpRomTable ();
  }

  if (mUsbOpRomTable == NULL) {
    return FALSE;
  }

  for (Index = 0; Index < mUsbOpRomCount; Index++) {
    if (CompareGuid(&mUsbOpRomTable[Index].FileName, FileName)) {
      return TRUE;
    }
  }

  return FALSE;
}

/**
  Internal function to initialize H2O_DXE_CP_PCI_CHECK_SYSTEM_OPTION_ROM_DATA data and trigger
  gH2ODxeCpPciCheckSysOptionRomGuid checkpoint.

  @param[in] PciOpRom                             Pointer to PCI option ROM.

  @retval The checkpoint status value.
**/
STATIC
UINT32
TriggerDxeCpPciCheckSysOptionRom (
  IN PCI_OPTION_ROM_TABLE                         *PciOpRom
  )
{
  EFI_STATUS                                      Status;
  H2O_DXE_CP_PCI_CHECK_SYSTEM_OPTION_ROM_DATA     CpData;

  CpData.Size        = sizeof (H2O_DXE_CP_PCI_CHECK_SYSTEM_OPTION_ROM_DATA);
  CpData.Status      = H2O_CP_TASK_NORMAL;
  CpData.VendorId    = PciOpRom->VendorId;
  CpData.DeviceId    = PciOpRom->DeviceId;
  CopyGuid (&CpData.FileName, &PciOpRom->FileName);

  DEBUG ((DEBUG_INFO, "Checkpoint Trigger: %g\n", &gH2ODxeCpPciCheckSysOptionRomGuid));
  Status = H2OCpTrigger (&gH2ODxeCpPciCheckSysOptionRomGuid, &CpData);
  DEBUG ((DEBUG_INFO, "Checkpoint Result: %x\n", CpData.Status));

  return CpData.Status;
}

EFI_STATUS
CommonPciInstallOptionRomTable (
  IN    PCI_OPTION_ROM_TABLE           *PciOpRom
  )
{
  UINTN                                TableIndex;

  //
  // If find VID and DID is exist, return success.
  // Let first install can select option rom from OemSvc.
  // The VID and DID is only installed one set.
  //
  for (TableIndex = 0; TableIndex < mPciOpRomTableCnt; TableIndex++) {
    if ((mPciOpRomTable[TableIndex].VendorId == PciOpRom->VendorId) &&
        (mPciOpRomTable[TableIndex].DeviceId == PciOpRom->DeviceId)) {
      return EFI_SUCCESS;
    }
  }

  if (FeaturePcdGet (PcdH2ODxeCpPciCheckSysOptionRomSupported)) {
    if (TriggerDxeCpPciCheckSysOptionRom (PciOpRom) == H2O_CP_TASK_SKIP) {
      return EFI_ABORTED;
    }
  }

  //
  // Check Database size
  //
  if (mPciOpRomTableCnt == mMaxOpRomTableCnt) {
    mPciOpRomTable = ReallocatePool (
                       sizeof (PCI_OPTION_ROM_TABLE) * mMaxOpRomTableCnt,
                       sizeof (PCI_OPTION_ROM_TABLE) * (mMaxOpRomTableCnt + 0x200),
                       mPciOpRomTable
                       );
    if (mPciOpRomTable == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
    mMaxOpRomTableCnt += 0x200;
  }

  //
  // Put Item
  //
  CopyGuid (&(mPciOpRomTable[mPciOpRomTableCnt].FileName), &(PciOpRom->FileName));
  mPciOpRomTable[mPciOpRomTableCnt].VendorId = PciOpRom->VendorId;
  mPciOpRomTable[mPciOpRomTableCnt].DeviceId = PciOpRom->DeviceId;
  mPciOpRomTableCnt++;
  return EFI_SUCCESS;
}

/**
  Install Option rom to table from OemSvc

  @retval EFI_SUCCESS                  Success
  @retval other                        Get OemService table failed

**/
EFI_STATUS
CommonPciInstallOptionRomFromOemService (
  )
{
  EFI_STATUS                           Status;
  PCI_OPTION_ROM_TABLE                 *PciOptionRomTable;
  UINTN                                TableIndex;

  PciOptionRomTable = NULL;
  DEBUG ((DEBUG_INFO, "OemKernelServices Call: OemSvcInstallOptionRomTable \n"));
  Status = OemSvcInstallOptionRomTable (
             PCI_OPROM,
             (VOID **)&PciOptionRomTable
             );
  DEBUG ((DEBUG_INFO, "OemKernelServices OemSvcInstallOptionRomTable Status: %r\n", Status));
  if (Status != EFI_MEDIA_CHANGED) {
    return Status;
  }
  if (PciOptionRomTable == NULL) {
    return EFI_NOT_READY;
  }
  for (TableIndex = 0; PciOptionRomTable[TableIndex].VendorId != 0xFFFF; TableIndex++) {
    CommonPciInstallOptionRomTable (PciOptionRomTable + TableIndex);
  }
  return EFI_SUCCESS;
}

/**
  Install Option rom to table from AutoGenList

  @retval EFI_SUCCESS                  Success
  @retval other                        Section not found
  @retval other                        Memory allocation failed
  @retval other                        Content invalid

**/
EFI_STATUS
CommonPciInstallOptionRomFromAutoGenList (
  )
{
  EFI_STATUS                                Status;
  UINTN                                     SectionSize;
  UINTN                                     Offset;
  PCI_OPTION_ROM_TABLE                      PciOptionRomTable;
  AUTO_GEN_PCI_OPTION_ROM_TABLE             *AutoGenOpRomList;
  AUTO_GEN_PCI_OPTION_ROM_TABLE             *Ptr;
  UINTN                                     Index;

  Status = GetSectionFromAnyFv (
             &gAutoGenPciOptionRomListGuid,
             EFI_SECTION_RAW,
             0,
             &AutoGenOpRomList,
             &SectionSize
             );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Check data vaildation
  //
  Offset = 0;
  Ptr = AutoGenOpRomList;
  while (Offset < SectionSize) {
    Offset += sizeof (AUTO_GEN_PCI_OPTION_ROM_TABLE) + sizeof (UINT16) * (Ptr->DeviceIdCount - 1);
    if ((Ptr->DeviceIdCount == 0) || (Offset > SectionSize)) {
      Status = EFI_INVALID_PARAMETER;
      goto DONE;
    }
    Ptr = (AUTO_GEN_PCI_OPTION_ROM_TABLE *)(((UINTN)AutoGenOpRomList) + Offset);
  }

  //
  // Put Item
  //
  Offset = 0;
  Ptr = AutoGenOpRomList;
  while (Offset < SectionSize) {
    Offset += sizeof (AUTO_GEN_PCI_OPTION_ROM_TABLE) + sizeof (UINT16) * (Ptr->DeviceIdCount - 1);
    if (!IsUsbOptionRom (&Ptr->FileName)) {
      CopyGuid (&(PciOptionRomTable.FileName), &(Ptr->FileName));
      PciOptionRomTable.VendorId = Ptr->VendorId;
      for (Index = 0; Index < Ptr->DeviceIdCount; Index++) {
        PciOptionRomTable.DeviceId = Ptr->DeviceId[Index];
        CommonPciInstallOptionRomTable (&PciOptionRomTable);
      }
    }
    Ptr = (AUTO_GEN_PCI_OPTION_ROM_TABLE *)(((UINTN)AutoGenOpRomList) + Offset);
  }

DONE:
  if (AutoGenOpRomList != NULL) {
    FreePool (AutoGenOpRomList);
  }
  return Status;
}

/**

 @param [in]   ImageHandle
 @param [in]   SystemTable

 @retval EFI_STATUS

**/
EFI_STATUS
EFIAPI
CommonPciPlatformDriverEntryPoint (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS                                Status;
  EFI_EVENT                                 PciPlatformBdsInstalledEvent;
  VOID                                      *Registration;
  EFI_BDS_ARCH_PROTOCOL                     *Bds;

  Status = gBS->LocateProtocol (
                  &gEfiBdsArchProtocolGuid,
                  NULL,
                  (VOID **)&Bds
                  );
  if (EFI_ERROR (Status)) {
    //
    // gEfiBdsArchProtocolGuid is not installed, register a protocolnotify event.
    // Once gEfiBdsArchProtocolGuid is installed, callback to CommonPciPlatformBdsCallback to Hook Bds->Entry
    //
    Status = gBS->CreateEvent (
                    EVT_NOTIFY_SIGNAL,
                    TPL_CALLBACK,
                    CommonPciPlatformHookBdsCallback,
                    NULL,
                    &PciPlatformBdsInstalledEvent
                    );
    if (!EFI_ERROR (Status)) {
      Status = gBS->RegisterProtocolNotify (
                      &gEfiBdsArchProtocolGuid,
                      PciPlatformBdsInstalledEvent,
                      &Registration
                      );
    }
    ASSERT_EFI_ERROR (Status);

  } else {
    //
    // gEfiBdsArchProtocolGuid is installed, Hook Bds->Entry to CommonPciPlatformBds
    //
    mOriginalBdsEntry = Bds->Entry;
    Bds->Entry = (EFI_BDS_ENTRY) CommonPciPlatformBds;
  }

  //
  // Initial PCI Option Rom database.
  // Integrate AutoGen Oprom List with OpRomTable from OemService.
  //
  mPciOpRomTable    = NULL;
  mPciOpRomTableCnt = 0;
  mMaxOpRomTableCnt = 0;
  CommonPciInstallOptionRomFromOemService ();
  CommonPciInstallOptionRomFromAutoGenList ();

  return Status;
}

/**
  This is the callback function after gEfiBdsArchProtocolGuid is installed.
  Hook Bds->Entry to CommonPciPlatformBds.

  @param    EFI_EVENT   Event,
  @param    VOID        *Context

  @retval   None.

**/
VOID
EFIAPI
CommonPciPlatformHookBdsCallback (
  IN EFI_EVENT   Event,
  IN VOID        *Context
  )
{
  EFI_BDS_ARCH_PROTOCOL     *Bds;
  EFI_STATUS                Status;
  Status = gBS->LocateProtocol (
                  &gEfiBdsArchProtocolGuid,
                  NULL,
                  (VOID **)&Bds
                  );
  //
  // gEfiBdsArchProtocolGuid should be installed at this moment.
  //
  ASSERT_EFI_ERROR (Status);

  mOriginalBdsEntry = Bds->Entry;
  Bds->Entry = (EFI_BDS_ENTRY) CommonPciPlatformBds;
}

/**
  This is the Hook function of Bds->Entry. After dxemain called gBds->Entry, code should come here.
  The purpose of this function is to hook PciPlatform.
  if gEfiPciPlatformProtocolGuid is installed by PciPlatform driver, Hook the original protocols.
  else, install gEfiPciPlatformProtocolGuid protocol.

  @param      Bds          - Bds architechtural protocol passed from Dxemain

  @retval   None.

**/
VOID
EFIAPI
CommonPciPlatformBds (
  IN EFI_BDS_ARCH_PROTOCOL  *Bds
)
{
  EFI_STATUS                                Status;
  EFI_PCI_PLATFORM_PROTOCOL                 *PciPlatform;

  Status = gBS->LocateProtocol (
                  &gEfiPciPlatformProtocolGuid,
                  NULL,
                  (VOID **)&PciPlatform
                  );
  if (!EFI_ERROR(Status)) {
    mPciPlatformHook.PlatformNotify         = PciPlatform->PlatformNotify;
    mPciPlatformHook.PlatformPrepController = PciPlatform->PlatformPrepController;
    mPciPlatformHook.GetPlatformPolicy      = PciPlatform->GetPlatformPolicy;
    mPciPlatformHook.GetPciRom              = PciPlatform->GetPciRom;
    //
    // Hook original PciPlatform protocols
    //
    PciPlatform->PlatformNotify         = CommonPlatformNotify;
    PciPlatform->PlatformPrepController = CommonPlatformPrepController;
    PciPlatform->GetPlatformPolicy      = CommonGetPlatformPolicy;
    PciPlatform->GetPciRom              = CommonGetPciRom;
  } else {
    //
    // gEfiPciPlatformProtocol is not installed. Install it on a new handle
    //
    Status = gBS->InstallProtocolInterface (
                    &mCommonPciPlatformHandle,
                    &gEfiPciPlatformProtocolGuid,
                    EFI_NATIVE_INTERFACE,
                    &mCommonPciPlatform
                    );
  }
  //
  // call to the original bdsentry
  //
  Bds->Entry = mOriginalBdsEntry;
  Bds->Entry (Bds);
}

EFI_STATUS
EFIAPI
CommonPlatformNotify (
  IN  EFI_PCI_PLATFORM_PROTOCOL                      *This,
  IN  EFI_HANDLE                                     HostBridge,
  IN  EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PHASE  Phase,
  IN  EFI_PCI_CHIPSET_EXECUTION_PHASE                ExecPhase
  )
{
  if (mPciPlatformHook.PlatformNotify != NULL) {
        mPciPlatformHook.PlatformNotify (
                                This,
                                HostBridge,
                                Phase,
                                ExecPhase
                                );
  }

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
CommonPlatformPrepController (
  IN  EFI_PCI_PLATFORM_PROTOCOL                      *This,
  IN  EFI_HANDLE                                     HostBridge,
  IN  EFI_HANDLE                                     RootBridge,
  IN  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_PCI_ADDRESS    PciAddress,
  IN  EFI_PCI_CONTROLLER_RESOURCE_ALLOCATION_PHASE   Phase,
  IN  EFI_PCI_CHIPSET_EXECUTION_PHASE                ChipsetPhase
  )
{
  EFI_STATUS  Status = EFI_SUCCESS;
  PCI_POLICY  *PciPolicy;
  UINT8       CapIdOffset;

  if ((Phase == EfiPciBeforeResourceCollection) && (ChipsetPhase == ChipsetEntry)) {

    Status = gBS->LocateProtocol (
                            &gPciPolicyProtocolGuid,
                            NULL,
                            (VOID **)&PciPolicy
                            );

    if (!EFI_ERROR(Status)) {
      Status = PciFindCapId (PciAddress.Bus, PciAddress.Device, PciAddress.Function, EFI_PCI_CAPABILITY_ID_PCIEXP, &CapIdOffset);

      if (!EFI_ERROR (Status)) {
        //
        // Only PCIE need to be prgorammed Max_Payload_Size register.
        //
        if (PciAddress.Bus == 0) {
          //
          // Program this root port and all device behind this root port with the same smallest Max_Payload_Size register.
          //
          ProgramPciePayLoad (PciAddress, CapIdOffset, PciPolicy);
        }
      }

      if (EFI_ERROR (Status)) {
        //
        // Only PCI need to be prgorammed Latency Time register.
        //
        ProgramLatencyTime (PciAddress.Bus, PciAddress.Device, PciAddress.Function, PciPolicy);
      }
    }

  }

  if (mPciPlatformHook.PlatformPrepController != NULL) {
          mPciPlatformHook.PlatformPrepController (
                                This,
                                HostBridge,
                                RootBridge,
                                PciAddress,
                                Phase,
                                ChipsetPhase
                                );
  }

  return EFI_SUCCESS;
}

/**
  GC_TODO: Add function description

  @param     This        add argument description
  @param     PciPolicy   add argument description.

  @retval    EFI_INVALID_PARAMETER  It mean there is no any platform policy.
  @retval    EFI_SUCCESS            Get the platform policy successfully.

**/
EFI_STATUS
EFIAPI
CommonGetPlatformPolicy (
  IN CONST EFI_PCI_PLATFORM_PROTOCOL               *This,
  OUT EFI_PCI_PLATFORM_POLICY                      *PciPolicy
  )
{
  if (PciPolicy == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Always reserved VGA IO range.
  //
  *PciPolicy |= EFI_RESERVE_VGA_IO_ALIAS;

  if (mPciPlatformHook.GetPlatformPolicy != NULL) {
    mPciPlatformHook.GetPlatformPolicy (
                       This,
                       PciPolicy
                       );
  }

  return EFI_SUCCESS;
}


/**
  Check if the input PCI Oprom is allowed to be load and dispatched.
  if it is a storage OPROM and StorageOpromAccessRight is FALSE, return FALSE
  else return TRUE.
  
  @retval   TRUE      Allowed
  @retval   FALSE     Denied

**/
STATIC
BOOLEAN
IsStorageOpromAllowed (
  IN EFI_PCI_IO_PROTOCOL   *PciIo
  )
{
  KERNEL_CONFIGURATION      *SystemConfiguration;
  PCI_TYPE00                PciData;  
  EFI_STATUS                Status;
  
  SystemConfiguration = CommonGetVariableData (SETUP_VARIABLE_NAME, &gSystemConfigurationGuid);
  if (SystemConfiguration != NULL && SystemConfiguration->StorageOpromAccessRight == FALSE) {
    Status = PciIo->Pci.Read (
                      PciIo,
                      EfiPciIoWidthUint8,
                      0,
                      sizeof (PciData),
                      &PciData
                      );
    //
    // Currently, we will only check IDE/AHCI/NVME/EMMC/UFS controllers
    //
    if(!EFI_ERROR(Status) &&
       (PciData.Hdr.ClassCode[2] == PCI_CLASS_MASS_STORAGE) ||
        (PciData.Hdr.ClassCode[2] == 0x08) && (PciData.Hdr.ClassCode[1] == 0x05)) {    
      return FALSE;         
    } else {
       return TRUE;
    }
  }
  return TRUE;
}


/**
  Return a PCI ROM image for the onboard device represented by PciHandle

  @param    This       Protocol instance pointer.
  @param    PciHandle  PCI device to return the ROM image for.
  @param    RomImage   PCI Rom Image for onboard device
  @param    RomSize    Size of RomImage in bytes

  @retval   EFI_NOT_FOUND  No RomImage.
  @retval   EFI_SUCCESS    RomImage is valid.

**/
EFI_STATUS
EFIAPI
CommonGetPciRom (
  IN  CONST EFI_PCI_PLATFORM_PROTOCOL             *This,
  IN  EFI_HANDLE                                  PciHandle,
  OUT VOID                                        **RomImage,
  OUT UINTN                                       *RomSize
  )
{
  EFI_STATUS            Status;
  EFI_PCI_IO_PROTOCOL   *PciIo;
  UINTN                 Segment;
  UINTN                 Bus;
  UINTN                 Device;
  UINTN                 Function;
  UINT16                VendorId;
  UINT16                DeviceId;
  BOOLEAN               SkipGetPciRom;
  EFI_STATUS            PciPlatformHookStatus = EFI_SUCCESS;
  UINT64                Result;
  UINT8                 PciOptionRomCount;
  UINT8                 PciOptionRomBdfCount;



  PciOptionRomCount            = 0;
  PciOptionRomBdfCount         = 0;
  SkipGetPciRom                = FALSE;

  *RomImage = NULL;
  *RomSize = 0;

  Status = gBS->HandleProtocol (
                  PciHandle,
                  &gEfiPciIoProtocolGuid,
                  (VOID **) &PciIo
                  );
  if (EFI_ERROR (Status)) {
    return EFI_NOT_FOUND;
  }

  PciIo->GetLocation (PciIo, &Segment, &Bus, &Device, &Function);

  PciIo->Pci.Read (PciIo, EfiPciIoWidthUint16, 0, 1, &VendorId);

  PciIo->Pci.Read (PciIo, EfiPciIoWidthUint16, 2, 1, &DeviceId);

  PciIo->Attributes(
         PciIo,
         EfiPciIoAttributeOperationGet,
         0,
         &Result
         );


  SkipGetPciRom = PcdGetBool(PcdH2OMultiBoardPciOptionRomSupported) ? PcdGetBool(PcdH2OPciOptionRomPreferPlugIn) : FALSE;
  DxeCsSvcSkipGetPciRom (Segment, Bus, Device, Function, VendorId, DeviceId, &SkipGetPciRom);
  if (SkipGetPciRom) {
    return EFI_NOT_FOUND;
  }
  //
  // OemServices
  //
  DEBUG ((DEBUG_INFO, "OemKernelServices Call: OemSvcSkipLoadPciOptionRom \n"));
  Status = OemSvcSkipLoadPciOptionRom (
                          Segment,
                          Bus,
                          Device,
                          Function,
                          VendorId,
                          DeviceId,
                          &SkipGetPciRom
                          );
  DEBUG ((DEBUG_INFO, "OemKernelServices OemSvcSkipLoadPciOptionRom Status: %r\n", Status));

  //
  // Check if  1. It include a Storage Oprom and 
  //           2. For this PCI Oprom, OemSvc follow Kernel default policy
  //           3. If current policy allow it to be loaded and dispatched
  //
  if (!IsStorageOpromAllowed(PciIo) && Status == EFI_UNSUPPORTED) {
    PciIo->RomSize = 0;
    PciIo->RomImage = NULL;
  }
  
  if ((Status == EFI_SUCCESS) || (Status == EFI_MEDIA_CHANGED)) {
    Status = EFI_NOT_FOUND;

      //
      // Per UEFI spec 2.6
      // If EFI_PCI_IO_ATTRIBUTE_EMBEDDED_ROM bit is set, then the PCI option ROM described by the RomImage and RomSize
      // fields is not from ROM BAR of the PCI controller. If this bit is clear, then the
      // RomImage and RomSize fields were initialized based on the PCI option ROM
      // found through the ROM BAR of the PCI controller. Below condition is to skip ROM that found from ROMBAR.
      //
    if (SkipGetPciRom && (!(Result & EFI_PCI_IO_ATTRIBUTE_EMBEDDED_ROM))) {
      PciIo->RomSize = 0;
      PciIo->RomImage = NULL;
    }
  } else {

    if (!PcdGetBool(PcdH2OMultiBoardPciOptionRomSupported)) {
      //
      // Get PCI Option ROM from FV.
      //
      Status = GetFromFv (VendorId, DeviceId, RomImage, RomSize);
    } else {
      //
      // MultiBoard: Get PCI Option ROM from regions of FD.
      //
      Status = EFI_NOT_FOUND;

      //
      // Get PCI Option ROM count from FDM.
      //
      FdmGetNCount (&gH2OFlashMapRegionOptionRomGuid, &PciOptionRomCount);
      FdmGetNCount (&gH2OFlashMapRegionOptionRomBdfGuid, &PciOptionRomBdfCount);
      if (PciOptionRomCount == 0 && PciOptionRomBdfCount == 0) {
        Status = EFI_NOT_FOUND;
      } else {
        //
        // PciOptionRomBdf region.
        //
        Status = ProcessPciOptionRomBdf (
                          Segment,
                          (UINT8)Bus,
                          (UINT8)Device,
                          (UINT8)Function,
                          VendorId,
                          DeviceId,
                          RomImage,
                          RomSize
                          );
        if (EFI_ERROR (Status)) {
          //
          // PciOptionRom region.
          //
          Status = ProcessPciOptionRom (
                            Segment,
                            (UINT8)Bus,
                            (UINT8)Device,
                            (UINT8)Function,
                            VendorId,
                            DeviceId,
                            RomImage,
                            RomSize
                            );
        }
      }

      if (EFI_ERROR (Status)) {
        //
        // If PCI OpRom can not be found in FD regions, then get from FV.
        //
        Status = GetFromFv (VendorId, DeviceId, RomImage, RomSize);
      }

    }
  }

  if (mPciPlatformHook.GetPciRom != NULL) {
      PciPlatformHookStatus = mPciPlatformHook.GetPciRom (
                                  This,
                                  PciHandle,
                                  RomImage,
                                  RomSize
                                  );
  }

  if (EFI_ERROR (PciPlatformHookStatus)) {
    return PciPlatformHookStatus;
  }
  return Status;
}

/**
  Check Vendor ID and Device ID in Option rom, if not equal, make them the same.

  @param    RomImage - Oprom Image Offset
  @param    VendorId - Pci Vendor Id
  @param    DeviceId - Pci Device Id

  @retval

**/
VOID
CheckOpromId (
  IN  OUT VOID                                    **RomImage,
  IN      UINTN                                   *RomSize,
  IN      UINT16                                  VendorId,
  IN      UINT16                                  DeviceId
)
{
  EFI_PCI_ROM_HEADER  RomHeader;
  PCI_DATA_STRUCTURE  *Pcir;
  BOOLEAN             Done;

  if (*RomSize < sizeof (EFI_PCI_ROM_HEADER)) {
    return ;
  }

  RomHeader.Raw = *RomImage;
  Done          = FALSE;
  while (!Done) {
    if (RomHeader.Generic->Signature == PCI_EXPANSION_ROM_HEADER_SIGNATURE) {

      if (*RomSize < RomHeader.Generic->PcirOffset + sizeof (PCI_DATA_STRUCTURE)) {
        return ;
      }

      Pcir = (PCI_DATA_STRUCTURE *) (RomHeader.Raw + RomHeader.Generic->PcirOffset);

      if (Pcir->CodeType == PCI_CODE_TYPE_PCAT_IMAGE) {
        //
        // We have found a PC-AT ROM
        //
        Pcir->VendorId = VendorId;
        Pcir->DeviceId = DeviceId;
        return ;

      }

      if ((Pcir->Indicator & 0x80) == 0x80) {
        Done = TRUE;
      } else {
        RomHeader.Raw += 512 * Pcir->ImageLength;
      }
    } else {
      Done = TRUE;
    }
  }
}


EFI_STATUS
ProcessPciOptionRom (
  IN     UINTN                                 Segment,
  IN     UINT8                                 Bus,
  IN     UINT8                                 Device,
  IN     UINT8                                 Function,
  IN     UINT16                                VendorId,
  IN     UINT16                                DeviceId,
  IN OUT VOID                                  **RomImage,
  IN OUT UINTN                                 *RomSize
)
{
  EFI_STATUS                                   Status;
  EFI_STATUS                                   OemSvcStatus;
  H2O_FLASH_MAP_OPTION_ROM_ID                  *PciRomRegionId;
  UINT8                                        OptionRomIndex;
  UINT8                                        PciOptionRomCount;
  UINT8                                        RegionId[FDM_ENTRY_REGION_ID_SIZE];
  UINT64                                       RegionOffset;
  UINT64                                       RegionSize;
  UINT32                                       Attr;
  BOOLEAN                                      IsPciOptionRomBdf;
  BOOLEAN                                      IsVendorId;
  BOOLEAN                                      IsDeviceId;
  BOOLEAN                                      SkipGetPciRom;


  Status               = EFI_NOT_FOUND;
  OemSvcStatus         = EFI_UNSUPPORTED;
  IsPciOptionRomBdf    = FALSE;

  if ((RomImage == NULL) || (RomSize == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  FdmGetNCount (&gH2OFlashMapRegionOptionRomGuid, &PciOptionRomCount);
  //
  // PciOptionRom region.
  // Check VendorId, DeviceId and BoardId for get PCI Option ROM.
  //
  for (OptionRomIndex = 1; OptionRomIndex <= PciOptionRomCount; OptionRomIndex++) {
    Status = FdmGetNAt (&gH2OFlashMapRegionOptionRomGuid, OptionRomIndex, RegionId, &RegionOffset, &RegionSize, &Attr);
    if (EFI_ERROR(Status)) {
      continue;
    }

    //
    //  typedef struct _H2O_FLASH_MAP_OPTION_ROM_ID {
    //    UINT16 VendorId;
    //    UINT16 DeviceId;
    //    UINT8 Flags;
    //    UINT8 Reserved[5];
    //    UINT8 Boards[6];
    //  } H2O_FLASH_MAP_OPTION_ROM_ID;
    //
    PciRomRegionId = (H2O_FLASH_MAP_OPTION_ROM_ID *)(RegionId);
    if ((PciRomRegionId->VendorId == 0xffff) && (PciRomRegionId->DeviceId == 0xffff)) {
      continue;
    }

    IsVendorId = ((PciRomRegionId->VendorId == 0xffff) || (PciRomRegionId->VendorId == VendorId)) ? TRUE : FALSE;
    IsDeviceId = ((PciRomRegionId->DeviceId == 0xffff) || (PciRomRegionId->DeviceId == DeviceId)) ? TRUE : FALSE;

    if (IsVendorId && IsDeviceId) {
      Status = CheckBoardId (IsPciOptionRomBdf, RegionId);
      if (EFI_ERROR (Status)) {
        continue;
      }

      //
      // BoardId matched, then get image from FDM.
      //
      *RomImage = (VOID *)((UINTN)(FdmGetBaseAddr() + RegionOffset));
      *RomSize = (UINTN)RegionSize;

      SkipGetPciRom = PcdGetBool(PcdH2OPciOptionRomPreferPlugIn);
      //
      // OemServices
      //
      DEBUG ((DEBUG_INFO, "OemKernelServices Call: OemSvcSkipLoadPciOptionRom2 \n"));
      OemSvcStatus = OemSvcSkipLoadPciOptionRom2 (
                                     Segment,
                                     (UINT8)Bus,
                                     (UINT8)Device,
                                     (UINT8)Function,
                                     VendorId,
                                     DeviceId,
                                     (VOID *)(*RomImage),
                                     (UINT32)(*RomSize),
                                     &SkipGetPciRom
                                     );
      DEBUG ((DEBUG_INFO, "OemKernelServices OemSvcSkipLoadPciOptionRom2 Status: %r\n", OemSvcStatus));
      if (SkipGetPciRom) {
        break;
      }

      CheckOpromId (RomImage, RomSize, VendorId, DeviceId);
      break;
    }
  }

  return Status;
}

EFI_STATUS
ProcessPciOptionRomBdf (
  IN     UINTN                                 Segment,
  IN     UINT8                                 Bus,
  IN     UINT8                                 Device,
  IN     UINT8                                 Function,
  IN     UINT16                                VendorId,
  IN     UINT16                                DeviceId,
  IN OUT VOID                                  **RomImage,
  IN OUT UINTN                                 *RomSize
)
{
  EFI_STATUS                                   Status;
  EFI_STATUS                                   OemSvcStatus;
  H2O_FLASH_MAP_OPTION_ROM_BDF_ID              *PciRomBdfRegionId;
  UINT8                                        OptionRomIndex;
  UINT8                                        PciOptionRomBdfCount;
  UINT8                                        RegionId[FDM_ENTRY_REGION_ID_SIZE];
  UINT64                                       RegionOffset;
  UINT64                                       RegionSize;
  UINT32                                       Attr;
  BOOLEAN                                      IsPciOptionRomBdf;
  BOOLEAN                                      IsBus;
  BOOLEAN                                      IsDevice;
  BOOLEAN                                      IsFunction;
  BOOLEAN                                      SkipGetPciRom;


  Status                  = EFI_NOT_FOUND;
  OemSvcStatus            = EFI_UNSUPPORTED;
  IsPciOptionRomBdf       = TRUE;

  if ((RomImage == NULL) || (RomSize == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  FdmGetNCount (&gH2OFlashMapRegionOptionRomBdfGuid, &PciOptionRomBdfCount);
  //
  // PciOptionRomBdf region.
  // Check Bus, Device, Function and BoardId for get PCI Option ROM.
  //
  for (OptionRomIndex = 1; OptionRomIndex <= PciOptionRomBdfCount; OptionRomIndex++) {
    Status = FdmGetNAt (&gH2OFlashMapRegionOptionRomBdfGuid, OptionRomIndex, RegionId, &RegionOffset, &RegionSize, &Attr);
    if (EFI_ERROR(Status)) {
      continue;
    }

    //
    //  typedef struct _H2O_FLASH_MAP_OPTION_ROM_BDF_ID {
    //    UINT8 Segment;
    //    UINT8 Bus;
    //    UINT8 Device;
    //    UINT8 Function;
    //    UINT8 Flags;
    //    UINT8 Reserved[5];
    //    UINT8 Boards[6];
    //  } H2O_FLASH_MAP_OPTION_ROM_BDF_ID;
    //
    PciRomBdfRegionId = (H2O_FLASH_MAP_OPTION_ROM_BDF_ID *)(RegionId);
    if ((PciRomBdfRegionId->Bus == 0xff) && (PciRomBdfRegionId->Device == 0xff) && (PciRomBdfRegionId->Function == 0xff)) {
      continue;
    }

    IsBus = ((PciRomBdfRegionId->Bus == 0xff) || (PciRomBdfRegionId->Bus == Bus)) ? TRUE : FALSE;
    IsDevice = ((PciRomBdfRegionId->Device == 0xff) || (PciRomBdfRegionId->Device == Device)) ? TRUE : FALSE;
    IsFunction = ((PciRomBdfRegionId->Function == 0xff) || (PciRomBdfRegionId->Function == Function)) ? TRUE : FALSE;

    if (IsBus && IsDevice && IsFunction) {
      Status = CheckBoardId (IsPciOptionRomBdf, RegionId);
      if (EFI_ERROR (Status)) {
        continue;
      }

      //
      // BoardId matched, then get image from FDM.
      //
      *RomImage = (VOID *)((UINTN)(FdmGetBaseAddr() + RegionOffset));
      *RomSize = (UINTN)RegionSize;

      SkipGetPciRom = PcdGetBool(PcdH2OPciOptionRomPreferPlugIn);
      //
      // OemServices
      //
      DEBUG ((DEBUG_INFO, "OemKernelServices Call: OemSvcSkipLoadPciOptionRom2 \n"));
      OemSvcStatus = OemSvcSkipLoadPciOptionRom2 (
                                     Segment,
                                     (UINT8)Bus,
                                     (UINT8)Device,
                                     (UINT8)Function,
                                     VendorId,
                                     DeviceId,
                                     (VOID *)(*RomImage),
                                     (UINT32)(*RomSize),
                                     &SkipGetPciRom
                                     );
      DEBUG ((DEBUG_INFO, "OemKernelServices OemSvcSkipLoadPciOptionRom2 Status: %r\n", OemSvcStatus));
      if (SkipGetPciRom) {
        break;
      }

      CheckOpromId (RomImage, RomSize, VendorId, DeviceId);
      break;
    }
  }

  return Status;
}


/**
  Check Board Id.

  @param[in]  IsPciOptionBdf        Determine PciOptionRom or PciOptionRomBdf for process.
  @param[in]  RegionId              PCI Option ROM region Id.
  @param[in]  Instance              Get the instance from region of the FDM.

  @retval     EFI_SUCCESS           BoardId matched.
  @retval     EFI_UNSUPPORTED       No BoardId matched.
  @retval     EFI_INVALID_PARAMETER Invalid parameter.
**/
EFI_STATUS
CheckBoardId (
  IN  BOOLEAN                               IsPciOptionRomBdf,
  IN  UINT8                                 *RegionId
)
{
  UINT8                                     BoardId;
  H2O_FLASH_MAP_OPTION_ROM_BDF_ID           *PciRomBdfRegionId;
  H2O_FLASH_MAP_OPTION_ROM_ID               *PciRomRegionId;
  UINT8                                     *OptionRomBoardId;
  UINT8                                     BoardIndex;


  PciRomBdfRegionId = NULL;
  PciRomRegionId    = NULL;
  BoardId           = PcdGet8(PcdH2OBoardId);

  if (RegionId == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (IsPciOptionRomBdf) {
    PciRomBdfRegionId = (H2O_FLASH_MAP_OPTION_ROM_BDF_ID *)RegionId;
    OptionRomBoardId = PciRomBdfRegionId->Boards;
  } else {
    PciRomRegionId = (H2O_FLASH_MAP_OPTION_ROM_ID *)RegionId;
    OptionRomBoardId = PciRomRegionId->Boards;
  }

  for (BoardIndex = 0 ; BoardIndex < SUPPORT_BOARD_COUNT; BoardIndex++) {
    if (*OptionRomBoardId == H2O_FLASH_MAP_REGION_BOARDS_END) {
      break;
    }

    if ((BoardId == *(OptionRomBoardId + BoardIndex)) ||
        (BoardId == DEFAULT_BOARD) ||
        (*(OptionRomBoardId + BoardIndex) == DEFAULT_BOARD)) {

      return EFI_SUCCESS;
    }
  }

  return EFI_UNSUPPORTED;
}

EFI_STATUS
GetFromFv (
  IN   UINT16                             VendorId,
  IN   UINT16                             DeviceId,
  OUT  VOID                               **RomImage,
  OUT  UINTN                              *RomSize
)
{
  EFI_STATUS            Status;
  UINTN                 TableIndex;
  VOID                  *TempRomImage;
  UINTN                 TempRomSize;

  if ((RomImage == NULL) || (RomSize == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Get PCI Option ROM from FV.
  //
  Status = EFI_NOT_FOUND;
  for (TableIndex = 0; TableIndex < mPciOpRomTableCnt; TableIndex++) {
    //
    // See if the PCI device specified by PciHandle matches at device in PciOptionRomTable
    //
    if (VendorId != mPciOpRomTable[TableIndex].VendorId ||
      DeviceId != mPciOpRomTable[TableIndex].DeviceId) {
      continue;
    }

    Status = GetSectionFromAnyFv (
              &mPciOpRomTable[TableIndex].FileName,
              EFI_SECTION_RAW,
              0,
              &TempRomImage,
              &TempRomSize);

    if (EFI_ERROR (Status)) {
      continue;
    }

    *RomImage = TempRomImage;
    *RomSize  = TempRomSize;
    CheckOpromId (RomImage, RomSize, VendorId, DeviceId);
    break;
  }

  return Status;
}

/**
  Program Pci latency time register as SCU value.

  @param    BUS           - Pointer to the  EFI_PCI_PLATFORM_PROTOCOL  instance.
  @param    Device        - The associated PCI host bridge handle.
  @param    Func          - The associated PCI root bridge handle.

  @retval   EFI_UNSUPPORTED
  @retval   EFI_SUCCESS
**/
EFI_STATUS
ProgramLatencyTime (
  IN  UINT8       Bus,
  IN  UINT8       Device,
  IN  UINT8       Func,
  IN  PCI_POLICY  *PciPolicy
)
{
  UINT8       LatTime;
  UINT64      BootScriptPciAddress;
  UINT8       HeaderType;

  if(PciPolicy->LatencyTimePolicy.Enable == FALSE) {
    return EFI_UNSUPPORTED;
  }

  //
  // Program SCU value into Latency Time register.
  //
  LatTime = PciPolicy->LatencyTimePolicy.LatencyTime;

  PciExpressAndThenOr8 (PCI_EXPRESS_LIB_ADDRESS (Bus, Device, Func, PCI_LATENCY_TIMER_OFFSET), 0x00, LatTime);

  BootScriptPciAddress = S3_BOOT_SCRIPT_LIB_PCI_ADDRESS (Bus, Device, Func, PCI_LATENCY_TIMER_OFFSET);
  S3BootScriptSavePciCfgWrite (
      S3BootScriptWidthUint8,
      BootScriptPciAddress,
      1,
      &LatTime);

  //
  // Program Sec. Latency Time register of the pci Bridge.
  //
  HeaderType = PciExpressAnd8 (PCI_EXPRESS_LIB_ADDRESS (Bus, Device, Func, PCI_HEADER_TYPE_OFFSET), HEADER_LAYOUT_CODE);

  if (HeaderType == HEADER_TYPE_PCI_TO_PCI_BRIDGE) {

    PciExpressAndThenOr8 (PCI_EXPRESS_LIB_ADDRESS (Bus, Device, Func, PCCARD_CARDBUS_LATENCY_TIMER_OFFSET), 0x00, LatTime);

    BootScriptPciAddress = S3_BOOT_SCRIPT_LIB_PCI_ADDRESS (Bus, Device, Func, PCCARD_CARDBUS_LATENCY_TIMER_OFFSET);
    S3BootScriptSavePciCfgWrite (
        S3BootScriptWidthUint8,
        BootScriptPciAddress,
        1,
        &LatTime);

  }

  return EFI_SUCCESS;
}

/**
  Program PCIE Root port bridge and all endpoints with the same smallest Max_PayLoad value.

  @param    PciAddress     The address of the PCI device on the PCI bus.
  @param    CapIdOffset    This CapId indicate this device is a PCIE device.

  @retval   EFI_SUCCESS

**/
EFI_STATUS
ProgramPciePayLoad(
  IN     EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_PCI_ADDRESS    PciAddress,
  IN     UINT8                                          CapIdOffset,
  IN     PCI_POLICY                                     *PciPolicy
)
{
  UINT8       SmallestMaxPayLoad;
  UINT8       SecondBus;
  UINT8       HeaderType;

  if(PciPolicy->ProgramPciePayLoadPolicy.Enable == FALSE) {
    return EFI_UNSUPPORTED;
  }

  HeaderType = (PciExpressRead8 (PCI_EXPRESS_LIB_ADDRESS (PciAddress.Bus, PciAddress.Device, PciAddress.Function, PCI_HEADER_TYPE_OFFSET)) & (HEADER_LAYOUT_CODE));

  if (HeaderType == HEADER_TYPE_PCI_TO_PCI_BRIDGE) {

    SecondBus = PciExpressRead8 (PCI_EXPRESS_LIB_ADDRESS (PciAddress.Bus, PciAddress.Device, PciAddress.Function, PCI_BRIDGE_SECONDARY_BUS_REGISTER_OFFSET));

    SmallestMaxPayLoad = (PciExpressRead8 (PCI_EXPRESS_LIB_ADDRESS (PciAddress.Bus, PciAddress.Device, PciAddress.Function, CapIdOffset + 0x04)) & (BIT2 | BIT1 | BIT0)) << 5;

    //
    //  Scan all device behind this root port to get the Smallest Max_Pay_Load.
    //
    GetSmallestMaxPayLoad (SecondBus, &SmallestMaxPayLoad);

    //
    //  Program all device behind this root port with the same Smallest Max_Pay_Load.
    //
    SetSmallestPayLoadValue (SecondBus, SmallestMaxPayLoad);

    //
    //  Program this root port with smallest Max_Pay_Load.
    //
    PciExpressAndThenOr8 (PCI_EXPRESS_LIB_ADDRESS (PciAddress.Bus, PciAddress.Device, PciAddress.Function, CapIdOffset + 0x08), 0x1F, SmallestMaxPayLoad) ;

  }
  return EFI_SUCCESS;
}

/**
  Scan all endpoints behind this root port bridge and find the smallest Max_PayLoad Value.

  @param    Bus                    Bus number which will be recursived to scan all the bus behine this bridge.
  @param    SmallestPayLoadValue   This value point to the smallest payload value.

  @retval   EFI_SUCCESS

**/
EFI_STATUS
GetSmallestMaxPayLoad(
  IN     UINT8   Bus,
  IN OUT UINT8   *SmallestPayLoadValue
)
{
  EFI_STATUS      Status;
  UINT8           SecondBus;
  UINT8           Device;
  UINT8           Func;
  UINT8           HeaderType;
  UINT8           EndPointCapIdOffset;
  UINT8           TempPayLoad;

  for (Device = 0; Device <= PCI_MAX_DEVICE; Device++){
    for (Func = 0; Func <= PCI_MAX_FUNC; Func++){

      //
      // Check whether a pci device is present or not.
      //
      if ( PciExpressRead16 (PCI_EXPRESS_LIB_ADDRESS (Bus, Device, Func, 0)) == 0xffff) {
        if (Func == 0) {
          Func = PCI_MAX_FUNC;
        }
        continue;
      }

      //
      // Check whether a PCIE or not.
      //
      EndPointCapIdOffset = 0;
      Status = PciFindCapId (Bus, Device, Func, EFI_PCI_CAPABILITY_ID_PCIEXP, &EndPointCapIdOffset);

      if (EFI_ERROR (Status)){
        continue;
      }

      TempPayLoad = (PciExpressRead8 (PCI_EXPRESS_LIB_ADDRESS (Bus, Device, Func, EndPointCapIdOffset + 0x04)) & (BIT2 | BIT1 | BIT0)) << 5;

      if (*SmallestPayLoadValue > TempPayLoad ) {
        *SmallestPayLoadValue = TempPayLoad;
      }

      HeaderType = (PciExpressRead8 (PCI_EXPRESS_LIB_ADDRESS (Bus, Device, Func, PCI_HEADER_TYPE_OFFSET)) & HEADER_LAYOUT_CODE);

      if (HeaderType == HEADER_TYPE_PCI_TO_PCI_BRIDGE) {

        SecondBus = PciExpressRead8 (PCI_EXPRESS_LIB_ADDRESS (Bus, Device, Func, PCI_BRIDGE_SECONDARY_BUS_REGISTER_OFFSET));

        GetSmallestMaxPayLoad (SecondBus, SmallestPayLoadValue);

      }
    }
  }
  return EFI_SUCCESS;
}

/**
  Program all endpoints behind this root port bridge with the same smallest Max_PayLoad Value.

  @param    Bus                    Bus number which will be recursived to scan all the bus behine this bridge.
  @param    SmallestPayLoadValue   This value indicate smallest payload value.

  @retval   EFI_SUCCESS
**/
EFI_STATUS
SetSmallestPayLoadValue (
  IN     UINT8   Bus,
  IN     UINT8   SmallestPayLoadValue
)
{
  EFI_STATUS      Status;
  UINT8           Device;
  UINT8           Func;
  UINT8           HeaderType;
  UINT8           SecondBus;
  UINT8           EndPointCapIdOffset;

  //
  // Program minimal payload value into all PCIE devices below this bridge.
  //
  for (Device = 0; Device <= PCI_MAX_DEVICE; Device++){
    for (Func = 0; Func <= PCI_MAX_FUNC; Func++){
      //
      // Check whether a pci device is present or not.
      //
      if ( PciExpressRead16 (PCI_EXPRESS_LIB_ADDRESS (Bus, Device, Func, 0)) == 0xffff) {
        if (Func == 0) {
          Func = PCI_MAX_FUNC;
        }
        continue;
      }

      //
      // Check whether a PCIE or not.
      //
      EndPointCapIdOffset = 0;
      Status = PciFindCapId (Bus, Device, Func, EFI_PCI_CAPABILITY_ID_PCIEXP, &EndPointCapIdOffset);

      if (EFI_ERROR (Status)){
        continue;
      }

      PciExpressAndThenOr8 (PCI_EXPRESS_LIB_ADDRESS (Bus, Device, Func, EndPointCapIdOffset + 0x08), 0x1F, SmallestPayLoadValue);

      HeaderType = (PciExpressRead8 (PCI_EXPRESS_LIB_ADDRESS (Bus, Device, Func, PCI_HEADER_TYPE_OFFSET)) & HEADER_LAYOUT_CODE);

      if (HeaderType == HEADER_TYPE_PCI_TO_PCI_BRIDGE) {

        SecondBus = PciExpressRead8 (PCI_EXPRESS_LIB_ADDRESS (Bus, Device, Func, PCI_BRIDGE_SECONDARY_BUS_REGISTER_OFFSET));

        SetSmallestPayLoadValue (SecondBus, SmallestPayLoadValue);

      }

    }
  }

  return EFI_SUCCESS;
}

