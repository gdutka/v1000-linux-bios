/** @file
  Source file for LegacyBiosPlatformHookDxe driver.

;*******************************************************************************
;* Copyright (c) 2013 - 2018, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;*******************************************************************************
*/

#include <Uefi.h>
#include <IndustryStandard/Pci.h>

#include <Guid/GlobalVariable.h>

#include <Library/BaseLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DxeServicesLib.h>
#include <Library/DevicePathLib.h>
#include <Library/ChipsetConfigLib.h>

#include <Protocol/LegacyBios.h>
#include <Protocol/LegacyBiosPlatform.h>
#include <Protocol/PciIo.h>
#include <Protocol/LegacyInterrupt.h>
#include <Protocol/OemServices.h>
#include <Protocol/DevicePath.h>

// #include <OemServices/OemIrqRouting.h>
// #include <OemServices/OemOptionRomTable.h>
// #include <Oemservices/Kernel.h>

// #include <Chipset.h>
// #include <H2OVIPIrqPriority.h>
// #include <OemOptionRomType.h>
// #include <FchRegs/FchRegs.h>
// #include <AmdSetupConfig.h>
#include <ChipsetSetupConfig.h>

#include <Library/DxeChipsetSvcLib.h>
#include <IrqRoutingInformation.h>

#define MaxPIRQIndex                    8
#define PCI_UNUSED                      0x00
#define PCI_USED                        0xFF
#define LEGACY_USED                     0xFE
#define ROM_FOUND                       0x01
#define VALID_LEGACY_ROM                0x02

typedef enum {
  COMMON_DMI_SWITCH_TABLE,       //0
  COMMON_BOOTMANGER_KEY_TABLE,   //1
  COMMON_POST_KEY_TABLE,         //2
  COMMON_PCI_SKIP_TABLE_INDEX,   //3
  COMMON_LEGACY_BIOS_OEM_SLP,    //4
  COMMON_GET_MAX_CHECK_COUNT,    //5
  COMMON_ERROR_PASSWORD_CALL_BACK, //6
  COMMON_PASSWORD_ENTRY_CALL_BACK, //7
  COMMON_UPDATE_SCU_SYSTEM_INFO,   //8
  COMMON_SCU_CALLBACK_FUNCTION,    //9
  COMMON_CHECK_VGA_ORDER,          //10
  COMMON_DISPLAY_LOGO,             //11
  COMMON_CALCULATE_WRITE_CMOS_CHECKSUM, //12
  COMMON_CSM16_REFER_SWITCH,            //13
  COMMON_UPDATE_SLP20_PUBKEY_AND_SLP_MARKER, //14
  COMMON_UPDATE_MSDM_DATA,                   //15
  COMMON_FORM_LENGTH_UDPATE,                 //16
  COMMON_GET_MULTI_SIO_RESOURCE_TABLE,       //17
  COMMON_LOAD_DEFAULT_SETUP_MENU,            //18
  COMMON_LOGO_RESOLUTION,                    //19
  COMMON_IRQ_ROUTING_TABLE,                  //20
  COMMON_OPTION_ROM_TABLE,                   //21
  COMMON_UNSKIP_PCI_DEVICE,                  //22
  COMMON_GET_HOTPLUG_BRIDGE_INFO,            //23
  COMMON_UNLOAD_PCI_OPTION_ROM,              //24
  COMMON_BOOT_DISPLAY_DEVICE_REPLACE,        //25
  COMMON_PREPARE_INSTALL_MP_TABLE,           //26
  COMMON_GET_SIO_RESOURCE_TABLE,             //27
  COMMON_INSTALL_PCI_ROM_SWITCH_TEXT_MODE,   //28
  COMMON_DISPLAY_SPI_NOT_SUPPORT,
  COMMON_UPDATE_BBS_TABLE,
  COMMON_GET_SSID_SVID_INFO_CALLBACK,
  COMMON_PNP_DXE_UPDATE_STRING_TABLE,
  COMMON_PNP_DXE_GPNV_HANDLE_TABLE,
  COMMON_VARIABLE_RESERVED_TABLE,
  RUNTIME_MAX_NUM_COMMON_SUPPORT
} OEM_SERVICES_RUNTIME_COMMON_SUPPORT;


typedef struct {
  UINTN       BridgeBus;
  UINTN       BridgeDevice;
  UINTN       BridgeFunction;
  UINT8       InitialBus;
} EFI_LEGACY_MODIFY_PIR_TABLE;

typedef struct {
  UINT8 PirqNum;
  UINT8 CsReg;
} CHIPSET_DEV_IRQ_ROUTING_ENTRY;

#ifdef  RAID_OPTION_ROM

#define PMM_SIGNATURE  0x4D4D5024
#define MISC_SIGNATURE 0x4353494D
BOOLEAN                           gFindPMM = FALSE;

//
// stack of PMM function argment
//
typedef struct {
  UINT16  Function;
  UINT16  LengthL;
  UINT16  LengthH;
  UINT16  HandleL;
  UINT16  HandleH;
  UINT16  Flags;
} PMM_FUN_ARG;

#endif //#ifdef  RAID_OPTION_ROM

//
// The IRQ listed will be choosed to set PIRQA~PIRQH if they are not used by legacy
//
EFI_LEGACY_IRQ_PRIORITY_TABLE_ENTRY IrqPriorityTable[] = {
    {03, PCI_UNUSED},  \
  {04, PCI_UNUSED},  \
  {05, PCI_UNUSED},  \
  {07, PCI_UNUSED},  \
  {10, PCI_UNUSED},  \
  {11, PCI_UNUSED},  \
  {00, PCI_UNUSED},  \
};

//
// The IRQ listed will be selected to set PIRQA~PIRQH if they are not used by legacy
//
EFI_LEGACY_IRQ_PRIORITY_TABLE_ENTRY SetPIrqPriority[MaxPIRQIndex] = {
  {00, PCI_UNUSED},  /* PIRQ A */  \
  {00, PCI_UNUSED},  /* PIRQ B */  \
  {00, PCI_UNUSED},  /* PIRQ C */  \
  {00, PCI_UNUSED},  /* PIRQ D */  \
  {00, PCI_UNUSED},  /* PIRQ E */  \
  {00, PCI_UNUSED},  /* PIRQ F */  \
  {00, PCI_UNUSED},  /* PIRQ G */  \
  {00, PCI_UNUSED},  /* PIRQ H */  \
};

// FCH internal PCI devices IRQ logic mapping
CHIPSET_DEV_IRQ_ROUTING_ENTRY      mCsDevRoutingMap[] = {
  {0, 0x13},        // HD audio-> INTA#
  {2, 0x30},        // Dev18 (USB) IntA#-> INTC#
  {1, 0x31},        // Dev18 (USB) IntB# -> INTB#
  {2, 0x32},        // Dev19 (USB) IntA# -> INTC#
  {1, 0x33},        // Dev19 (USB) IntB#-> INTB#
  {2, 0x34},        // Dev22 (USB) IntA#-> INTC#
  {1, 0x35},        // Dev22 (USB) IntB#-> INTB#
  {2, 0x36},        // Dev20 (USB) IntC#-> INTC#
  {1, 0x40},        // IDE pci interrupt-> INTB#
  {3, 0x41},        // SATA pciPCI interrupt-> INTD#
  {0, 0x50},        // GPPInt0-> INTA#
  {1, 0x51},        // GPPInt1-> INTB#
  {2, 0x52},        // GPPInt2-> INTC#
  {3, 0x53},        // GPPInt3-> INTD#
  {0xff, 0xff}      // EOT
};

#define MAX_IRQ_PRIORITY_ENTRIES (sizeof (IrqPriorityTable) / sizeof (EFI_LEGACY_IRQ_PRIORITY_TABLE_ENTRY))

#define AHCI_ENABLE                       0x8000
#define AHCI_PORT0                        0x0100
#define AHCI_PORT2                        0x0200
#define AHCI_PORT4                        0x0400
#define AHCI_PORT6                        0x0800

#pragma pack(1)
typedef struct {
  EFI_HANDLE  Handle;
  UINT16      Vid;
  UINT16      Did;
  UINT16      SvId;
  UINT16      SysId;
} DEVICE_STRUCTURE;
#pragma pack()

// EFI_LEGACY_BIOS_PLATFORM_GET_PLATFORM_HANDLE mLegacyBiosPlatformGetPlatformHandle;
// EFI_LEGACY_BIOS_PLATFORM_HOOKS               mLegacyBiosPlatformHooks;
//CHIPSET_CONFIGURATION                        *mSystemConfiguration = NULL;
EFI_LEGACY_BIOS_PROTOCOL                     *mLegacyBios;

BOOLEAN                                      mGetVideoOriginalModeOK = FALSE;;
UINT16                                       mVideoOriginalMode = 0;
EFI_HANDLE                                   mDiskHandles[0x20];
EFI_HANDLE                                   mVideoHandles[8];

// static
// EFI_STATUS
// EFIAPI
// OemGetPlatformHandle (
//   IN  EFI_LEGACY_BIOS_PLATFORM_PROTOCOL  *This,
//   IN  EFI_GET_PLATFORM_HANDLE_MODE       Mode,
//   IN  UINT16                             Type,
//   OUT EFI_HANDLE                         **HandleBuffer,
//   OUT UINTN                              *HandleCount,
//   OUT VOID                               **AdditionalData OPTIONAL
//   );

// static
// EFI_STATUS
// EFIAPI
// OemPlatformHooks (
//   IN EFI_LEGACY_BIOS_PLATFORM_PROTOCOL   *This,
//   IN EFI_GET_PLATFORM_HOOK_MODE          Mode,
//   IN UINT16                              Type,
//   IN  EFI_HANDLE                         DeviceHandle,
//   IN  OUT UINTN                          *ShadowAddress,
//   IN  EFI_COMPATIBILITY16_TABLE          *Compatibility16Table,
//   IN  VOID                               **AdditionalData OPTIONAL
//   );

// static
// EFI_STATUS
// ConditionalSwitchToTextMode (
//   EFI_HANDLE    DeviceHandle
//   );

static
VOID
InstallSOLOptionROM (
  VOID
  );

static
VOID
AddIrqUsedTime (
  IN UINT8      IrqNumber
  );

static
UINT8
GetSuitableIrq (
  IN UINT8      SmallLimitation,
  IN UINT8      BigLimitation
  );

static
EFI_STATUS
EFIAPI
TranslatePirqHook (
  IN     EFI_LEGACY_BIOS_PLATFORM_PROTOCOL        *This,
  IN     UINTN                                    PciBus,
  IN     UINTN                                    PciDevice,
  IN     UINTN                                    PciFunction,
  IN OUT UINT8                                    *Pirq,
     OUT UINT8                                    *PciIrq
  );

/**
  Install callback when EFI_LEGACY_BIOS_PLATFORM_PROTOCOL installed.  

  @param[in] Standard EFI Image entry - EFI_IMAGE_ENTRY_POINT
             
  @retval    EFI_SUCEESS  Driver Dispatch success.

**/
EFI_STATUS
EFIAPI
LegacyBiosPlatformHookDxeEntry (
  IN EFI_HANDLE                       ImageHandle,
  IN EFI_SYSTEM_TABLE                 *SystemTable
  )
{
  EFI_STATUS                          Status;
  EFI_LEGACY_BIOS_PLATFORM_PROTOCOL   *LegacyBiosPlatform;

  Status = gBS->LocateProtocol (&gEfiLegacyBiosPlatformProtocolGuid, NULL, (VOID**)&LegacyBiosPlatform);
  if (EFI_ERROR (Status)) {
    return Status;
  }
//   mLegacyBiosPlatformGetPlatformHandle = LegacyBiosPlatform->GetPlatformHandle;
//   mLegacyBiosPlatformHooks = LegacyBiosPlatform->PlatformHooks;
//   LegacyBiosPlatform->GetPlatformHandle = OemGetPlatformHandle;
//   LegacyBiosPlatform->PlatformHooks = OemPlatformHooks;
  LegacyBiosPlatform->TranslatePirq = TranslatePirqHook;

//  Status = GetChipsetConfiguration (mSystemConfiguration);
//  if (EFI_ERROR (Status)) {
//    return Status;
//  }

  Status = gBS->LocateProtocol (
                  &gEfiLegacyBiosProtocolGuid,
                  NULL,
                  (VOID**)&mLegacyBios
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  return Status;

}

// EFI_STATUS
// FindAllDeviceTypes (
//   IN  UINT8                    BaseCodeValue,
//   IN  UINT8                    SubClassCodeValue,
//   IN OUT DEVICE_STRUCTURE      *DeviceTable,
//   IN OUT UINT16                *DeviceIndex,
//   IN BOOLEAN                   DeviceFlags
//   )
// {
//   UINTN                        HandleCount;
//   EFI_HANDLE                   *HandleBuffer;
//   UINTN                        Index;
//   UINTN                        StartIndex;
//   PCI_TYPE00                   PciConfigHeader;
//   EFI_PCI_IO_PROTOCOL          *PciIo;
//   EFI_LEGACY_BIOS_PROTOCOL     *LegacyBios;
//   UINTN                        Flags;
//   EFI_STATUS                   Status;
// 
//   StartIndex = *DeviceIndex;
//   Status = gBS->LocateProtocol (
//                   &gEfiLegacyBiosProtocolGuid,
//                   NULL,
//                   &LegacyBios
//                   );
//   ASSERT_EFI_ERROR (Status);
// 
//   gBS->LocateHandleBuffer (
//          ByProtocol,
//          &gEfiPciIoProtocolGuid,
//          NULL,
//          &HandleCount,
//          &HandleBuffer
//          );
//   for ( Index = 0; Index < HandleCount; Index ++) {
//     gBS->HandleProtocol (
//            HandleBuffer[Index],
//            &gEfiPciIoProtocolGuid,
//            &PciIo
//            );
//     PciIo->Pci.Read (
//                  PciIo,
//                  EfiPciIoWidthUint32,
//                  0,
//                  sizeof (PciConfigHeader) / sizeof (UINT32),
//                  &PciConfigHeader
//                  );
//     if ((PciConfigHeader.Hdr.ClassCode[2] == BaseCodeValue) && (PciConfigHeader.Hdr.ClassCode[1] == SubClassCodeValue)) {
//       DeviceTable->Handle = HandleBuffer[Index];
//       LegacyBios->CheckPciRom (
//                     LegacyBios,
//                     HandleBuffer[Index],
//                     NULL,
//                     NULL,
//                     &Flags
//                     );
// //
// // Verify that results of OPROM check match request.
// // The two valid requests are:
// //   DeviceFlags = 0 require a valid legacy ROM
// //   DeviceFlags = 1 require either no ROM or a valid legacy ROM
// //
//       if (((DeviceFlags != 0) && (Flags != ROM_FOUND)) ||
//           ((DeviceFlags == 0) && (Flags == (ROM_FOUND | VALID_LEGACY_ROM)))
//           ) {
//         DeviceTable->Vid   = PciConfigHeader.Hdr.VendorId;
//         DeviceTable->Did   = PciConfigHeader.Hdr.DeviceId;
//         DeviceTable->SvId  = PciConfigHeader.Device.SubsystemVendorID;
//         DeviceTable->SysId = PciConfigHeader.Device.SubsystemID;
//         ++ *DeviceIndex;
//         DeviceTable ++;
//       }
//     }
//   }
// //
// // Free any allocated buffers
// //
//   gBS->FreePool (HandleBuffer);
// 
//   if (*DeviceIndex != StartIndex) {
//     return EFI_SUCCESS;
//   } else {
//     return  EFI_NOT_FOUND;
//   }
// }

/**
  A hook for LegacyBiosPlatform->GetPlatformHandle().

  @param[in]  The same as EFI_LEGACY_BIOS_PLATFORM_GET_PLATFORM_HANDLE

  @retval     The same as EFI_LEGACY_BIOS_PLATFORM_GET_PLATFORM_HANDLE

**/
// static
// EFI_STATUS
// EFIAPI
// OemGetPlatformHandle (
//   IN  EFI_LEGACY_BIOS_PLATFORM_PROTOCOL  *This,
//   IN  EFI_GET_PLATFORM_HANDLE_MODE       Mode,
//   IN  UINT16                             Type,
//   OUT EFI_HANDLE                         **HandleBuffer,
//   OUT UINTN                              *HandleCount,
//   OUT VOID                               **AdditionalData OPTIONAL
//   )
// {
//   EFI_STATUS                         Status;
//   HDD_INFO                           *HddInfo;
//   UINT32                             HddInfoIndex;
//   UINT32                             Index;
//   UINT8                              SubClassCodeValue[] = {0x01, 0x06, 0x04};
//   DEVICE_STRUCTURE                   RegionalDevice[0x40];
//   UINT32                             RegionalIndex;
//   static BOOLEAN                     Link;
//   DEVICE_STRUCTURE                   BufferDevice;
//   UINT16                             IdeVendorId;
//   EFI_HANDLE                         PataHandle;
//   EFI_HANDLE                         SataHandle;
//   EFI_HANDLE                         BufferHandle;
//   UINT32                             PataIndex;
//   UINT32                             SataIndex;
//   EFI_SETUP_UTILITY_PROTOCOL         *SetupUtility;
//   SYSTEM_CONFIGURATION               *SystemConfiguration;
//   EFI_PCI_IO_PROTOCOL                *PciIo;
//   PCI_TYPE00                         PciTypeDevice;
//   UINTN                              Seg;
//   UINTN                              Bus;
//   UINTN                              Dev;
//   UINTN                              Fun;
//   DEVICE_STRUCTURE                   TempDevice;
//   VOID                               *VarConout;
//   UINTN                              DevicePathSize;
//   EFI_DEVICE_PATH_PROTOCOL           *DevicePath;
//   ACPI_ADR_DEVICE_PATH               AcpiDeviceNode;
//   BOOLEAN                            Found;
// 
//   RegionalIndex        = 0;
//   HddInfo              = NULL;
//   Link                 = FALSE;
//   PataHandle           = NULL;
//   SataHandle           = NULL;
//   BufferHandle         = NULL;
//   PataIndex            = 0;
//   SataIndex            = 0;
//   SystemConfiguration  = NULL;
//   HddInfoIndex         = 0x00;
//   VarConout            = NULL;
//   DevicePathSize       = 0;
//   Found                = FALSE;
// 
//   Status = gBS->LocateProtocol (
//                   &gEfiSetupUtilityProtocolGuid,
//                   NULL,
//                   &SetupUtility
//                   );
//   if (EFI_ERROR(Status)) {
//     return  Status;
//   }
//   SystemConfiguration = (SYSTEM_CONFIGURATION *) SetupUtility->SetupNvData;
// 
//   switch (Mode) {
// 
//   case EfiGetPlatformVgaHandle:
//     FindAllDeviceTypes (
//                   (UINT8) 0x00,
//                   (UINT8) 0x01,
//                   (DEVICE_STRUCTURE *) (&RegionalDevice[0]),
//                   (UINT16 *) (&RegionalIndex),
//                   0
//                   );
//     FindAllDeviceTypes (
//                   (UINT8) 0x03,
//                   (UINT8) 0x00,
//                   (DEVICE_STRUCTURE *) (&RegionalDevice[RegionalIndex]),
//                   (UINT16 *) (&RegionalIndex),
//                   0
//                   );
//     if (RegionalIndex == 0) {
//       return EFI_NOT_FOUND;
//     }
// 
//     Status = gRT->GetVariable (
//                     L"ConOut",
//                     &gEfiGlobalVariableGuid,
//                     NULL,
//                     &DevicePathSize,
//                     VarConout
//                     );
// 
//     if (Status == EFI_BUFFER_TOO_SMALL) {
//       VarConout = AllocateZeroPool (DevicePathSize);
//       if (VarConout == NULL) {
//         return EFI_NOT_FOUND;
//       }
//       //
//       // Read variable into the allocated buffer.
//       //
//       Status = gRT->GetVariable (
//                       L"ConOut",
//                       &gEfiGlobalVariableGuid,
//                       NULL,
//                       &DevicePathSize,
//                       VarConout
//                       );
//       if (EFI_ERROR (Status)) {
//         DevicePathSize = 0;
//         return Status;
//       }
//     }
// 
//     for ( Index = 0; (Index < RegionalIndex) && !Found; Index ++) {
//       Status = gBS->HandleProtocol (
//                       RegionalDevice[Index].Handle,
//                       &gEfiDevicePathProtocolGuid,
//                       &DevicePath
//                       );
//       ZeroMem (&AcpiDeviceNode, sizeof (ACPI_ADR_DEVICE_PATH));
//       AcpiDeviceNode.Header.Type = ACPI_DEVICE_PATH;
//       AcpiDeviceNode.Header.SubType = ACPI_ADR_DP;
//       AcpiDeviceNode.ADR = ACPI_DISPLAY_ADR (1, 0, 0, 1, 0, ACPI_ADR_DISPLAY_TYPE_VGA, 0, 0);
//       SetDevicePathNodeLength (&AcpiDeviceNode.Header, sizeof (ACPI_ADR_DEVICE_PATH));
// 
//       DevicePath = AppendDevicePathNode (DevicePath, (EFI_DEVICE_PATH_PROTOCOL *)&AcpiDeviceNode);
// 
//       if (CompareMem (
//             (EFI_DEVICE_PATH_PROTOCOL *)VarConout,
//             DevicePath,
//             GetDevicePathSize (DevicePath)
//             ) == 0) {
// 
//         TempDevice = RegionalDevice[0];
//         RegionalDevice[0] = RegionalDevice[Index];
//         RegionalDevice[Index] = TempDevice;
// 
//         Found = TRUE;
//       }
//     }
// 
//     for (Index = 0; Index < RegionalIndex; Index++) {
//       mVideoHandles[Index] = RegionalDevice[Index].Handle;
//     }
//     *HandleBuffer = &mVideoHandles[0];
//     *HandleCount = RegionalIndex;
//     return  EFI_SUCCESS;
// 
//   case EfiGetPlatformIdeHandle:
//     //
//     // Locate all found block io devices
//     //
//     if (AdditionalData != NULL) {
//       HddInfo = (HDD_INFO *) *AdditionalData;
//     }
//     for (Index = 0; Index < (sizeof (SubClassCodeValue) / sizeof (UINT8)); Index++) {
//       FindAllDeviceTypes (
//                   (UINT8) 0x01,
//                   (UINT8) SubClassCodeValue[Index],
//                   (DEVICE_STRUCTURE *) (&RegionalDevice[RegionalIndex]),
//                   (UINT16 *) (&RegionalIndex),
//                   1
//                   );
//     }
// 
//     //
//     // Here place any setup ordering
//     // Make sure all IDE controllers are connected. This is absolutely necessary
//     // in NO_CONFIG_CHANGE boot path to ensure IDE controller is correctly
//     // initialized and all IDE drives are enumerated
//     //
//     if (RegionalIndex == 0) {
//       return EFI_NOT_FOUND;
//     }
//     //
//     // Make sure all IDE controllers are connected. This is absolutely necessary
//     // in NO_CONFIG_CHANGE boot path to ensure IDE controller is correctly
//     // initialized and all IDE drives are enumerated
//     //
//     if (!Link) {
//       for ( Index = 0; Index < RegionalIndex; Index ++) {
//         gBS->ConnectController (
//                             RegionalDevice[Index].Handle,
//                             NULL,
//                             NULL,
//                             FALSE
//                             );
//       }
//     }
// 
//     IdeVendorId = AMD_FCH_VID;
//     for (Index = 0; Index < RegionalIndex; Index ++) {
//       if (RegionalDevice[Index].Vid == IdeVendorId) {
//         switch (RegionalDevice[Index].Did){
//         case FCH_IDE_DID:
//           PataHandle = RegionalDevice[Index].Handle;
//           PataIndex = Index;
//           break;
//         case FCH_SATA_DID :
//           SataHandle = RegionalDevice[Index].Handle;
//           SataIndex = Index;
//           break;
//         }
//       }
//     }
// 
//     //
//     // Move PataHandle to the head of RegionalDevice[]
//     //
//     if ((PataHandle != NULL) && (PataIndex != 0)) {
//       BufferDevice = RegionalDevice[PataIndex];
//       for (Index = PataIndex; Index > 0; Index-- ) {
//         RegionalDevice[Index] = RegionalDevice[Index-1];
//         if ((Index-1) == SataIndex) {
//           SataIndex = SataIndex + 1;
//         }
//       }
//       RegionalDevice[0] = BufferDevice;
//       PataIndex = 0;
//     }
// 
// {
//   UINTN                             VarSize;
//   AMD_SYSTEM_CONFIGURATION          *AmdSystemConfiguration;  
//   EFI_GUID                          AmdSetupConfigVarGuid = AMD_SYSTEM_CONFIGURATION_GUID;
// 
//   VarSize = sizeof (AMD_SYSTEM_CONFIGURATION);
//   Status = gBS->AllocatePool (
//                   EfiBootServicesData,
//                   VarSize,
//                   &AmdSystemConfiguration
//                   );
//   Status = gRT->GetVariable (
//                   L"AmdSetup",
//                   &AmdSetupConfigVarGuid,
//                   NULL,
//                   &VarSize,
//                   AmdSystemConfiguration
//                   );
// 
//   if (AmdSystemConfiguration->CbsCmnFchSataClass == 3) {
//     SystemConfiguration->AtaIdeMode = 1;
//   }
// }
// 
//     //
//     // Sata Legacy Mode
//     //
//     if (!Link) {
//       if (SystemConfiguration->AtaIdeMode == 1) {
//         if (RegionalDevice[0].Handle != BufferHandle) {
//           if (SataHandle == BufferHandle) {
//             BufferDevice = RegionalDevice[0];
//             RegionalDevice[0] = RegionalDevice[SataIndex];
//             RegionalDevice[SataIndex] = BufferDevice;
//           }
//           else {
//             BufferDevice = RegionalDevice[0];
//             RegionalDevice[0] = RegionalDevice[PataIndex];
//             RegionalDevice[PataIndex] = BufferDevice;
//           }
//         }
//       }
//     }
//     for ( Index = 0; Index < RegionalIndex; Index ++) {
//       mDiskHandles[Index] = RegionalDevice[Index].Handle;
//     }
// 
//     *HandleBuffer = &mDiskHandles[0];
//     *HandleCount  = RegionalIndex;
// 
//     //
//     // Make sure the legacy IDE addresses are forwarded to this controller
//     //
//     Link = TRUE;
// 
//     for (Index = 0; ( Index < RegionalIndex) && (AdditionalData != NULL); Index ++) {
//       if (RegionalDevice[Index].Vid == AMD_FCH_VID) {
//         Status = gBS->HandleProtocol (
//                         RegionalDevice[Index].Handle,
//                         &gEfiPciIoProtocolGuid,
//                         (VOID **) &PciIo
//                         );
//         PciIo->Pci.Read (
//                      PciIo,
//                      EfiPciIoWidthUint32,
//                      0,
//                      sizeof (PciTypeDevice) / sizeof (UINT32),
//                      &PciTypeDevice
//                      );
//         if (!EFI_ERROR (Status)) {
//           if (PciTypeDevice.Hdr.VendorId == 0xFFFF) {
//             continue;
//           }
// /* wait for PcdSeamlessAhciSupported ready
//           if (FeaturePcdGet (PcdSeamlessAhciSupported)) {
//             if ((RegionalDevice[Index].Did != FCH_IDE_DID) &&
//               (RegionalDevice[Index].Did != FCH_SATA_DID) &&
//               (RegionalDevice[Index].Did != FCH_SATA_AHCI_DID)) {
//                 continue;
//             }
//           } else {
//             if ((RegionalDevice[Index].Did != FCH_IDE_DID) &&
//               (RegionalDevice[Index].Did != FCH_SATA_DID) &&
//               ((RegionalDevice[Index].Did != FCH_SATA_AHCI_DID) &&
//               (RegionalDevice[Index].Did != FCH_SATA_AMDAHCI_DID) || (!mSystemConfiguration->AHCIMODE))) {
//                 continue;
//             }           
//           }
// */
//           if ((RegionalDevice[Index].Did != FCH_IDE_DID) &&
// #ifndef SEAMLESS_AHCI_SUPPORT
//               (RegionalDevice[Index].Did != FCH_SATA_DID) &&
//               ((RegionalDevice[Index].Did != FCH_SATA_AHCI_DID) &&
//               (RegionalDevice[Index].Did != FCH_SATA_AMDAHCI_DID) || (!mSystemConfiguration->AHCIMODE))) {
//             continue;
//           }
// #else
//               (RegionalDevice[Index].Did != FCH_SATA_DID) &&
//               (RegionalDevice[Index].Did != FCH_SATA_AHCI_DID)) {
//             continue;
//           }
// #endif
// 
//           PciIo->GetLocation (
//                    PciIo,
//                    &Seg,
//                    &Bus,
//                    &Dev,
//                    &Fun
//                    );
// 
//           HddInfo[HddInfoIndex].Status |= HDD_PRIMARY;
//           if ((PciTypeDevice.Hdr.ClassCode[1] == PCI_CLASS_MASS_STORAGE_SATADPA) ||
//               (PciTypeDevice.Hdr.ClassCode[1] == PCI_CLASS_MASS_STORAGE_RAID)) {
//             HddInfo[HddInfoIndex].Status |= HDD_PRIMARY | AHCI_ENABLE | (AHCI_PORT0 << HddInfoIndex);
//           }
//           HddInfo[HddInfoIndex + 1].Status |= HDD_SECONDARY;
// 
//           if ((PciTypeDevice.Hdr.ClassCode[1] == PCI_CLASS_MASS_STORAGE_SATADPA) ||
//               (PciTypeDevice.Hdr.ClassCode[1] == PCI_CLASS_MASS_STORAGE_RAID)) {
//             HddInfo[HddInfoIndex + 1].Status |= HDD_SECONDARY | AHCI_ENABLE | (AHCI_PORT2 << HddInfoIndex);
//           }
//           HddInfo[HddInfoIndex].Bus           = (UINT32)Bus;
//           HddInfo[HddInfoIndex + 1].Bus       = (UINT32)Bus;
//           HddInfo[HddInfoIndex].Device        = (UINT32)Dev;
//           HddInfo[HddInfoIndex + 1].Device    = (UINT32)Dev;
//           HddInfo[HddInfoIndex].Function      = (UINT32)Fun;
//           HddInfo[HddInfoIndex + 1].Function  = (UINT32)Fun;
// 
//           HddInfo[HddInfoIndex].BusMasterAddress    = (UINT16) (PciTypeDevice.Device.Bar[4] & 0xfffc);
//           HddInfo[HddInfoIndex+1].BusMasterAddress  = (UINT16)(HddInfo[HddInfoIndex].BusMasterAddress + 8);
// 
//           //
//           // Primary controller data
//           //
//           if ((PciTypeDevice.Hdr.ClassCode[0] & 0x01) != 0) {
//             HddInfo[HddInfoIndex].CommandBaseAddress  = (UINT16) (PciTypeDevice.Device.Bar[0] & 0xfffc);
// 
//             HddInfo[HddInfoIndex].ControlBaseAddress  = (UINT16) ((PciTypeDevice.Device.Bar[1] & 0xfffc) + 2);
// 
//             HddInfo[HddInfoIndex].HddIrq              = PciTypeDevice.Device.InterruptLine;
// 
//           } else {
// 
//             HddInfo[HddInfoIndex].HddIrq                = 14;
//             HddInfo[HddInfoIndex].CommandBaseAddress    = 0x1f0;
//             HddInfo[HddInfoIndex].ControlBaseAddress    = 0x3f6;
//             HddInfo[HddInfoIndex + 1].BusMasterAddress  = 0;
//           }
//           //
//           // Secondary controller data
//           //
//           if ((PciTypeDevice.Hdr.ClassCode[0] & 0x04) != 0) {
//             HddInfo[HddInfoIndex + 1].CommandBaseAddress  = (UINT16) (PciTypeDevice.Device.Bar[2] & 0xfffc);
// 
//             HddInfo[HddInfoIndex + 1].ControlBaseAddress  = (UINT16) ((PciTypeDevice.Device.Bar[3] & 0xfffc) + 2);
// 
//             HddInfo[HddInfoIndex + 1].HddIrq              = PciTypeDevice.Device.InterruptLine;
//           } else {
//             HddInfo[HddInfoIndex + 1].HddIrq              = 15;
//             HddInfo[HddInfoIndex + 1].CommandBaseAddress  = 0x170;
//             HddInfo[HddInfoIndex + 1].ControlBaseAddress  = 0x376;
//             HddInfo[HddInfoIndex + 1].BusMasterAddress    = 0;
//           }
//           HddInfoIndex += 2;
//         }
//       }
//     }
//     Status = EFI_SUCCESS;
//     break;
// 
//   default:
//     Status = mLegacyBiosPlatformGetPlatformHandle (
//                This,
//                Mode,
//                Type,
//                HandleBuffer,
//                HandleCount,
//                AdditionalData
//                );
//     break;
//   }
// 
//   return Status;
// }

/**
  A hook for LegacyBiosPlatform->PlatformHooks().

  @param[in]  The same as EFI_LEGACY_BIOS_PLATFORM_HOOKS

  @retval     The same as EFI_LEGACY_BIOS_PLATFORM_HOOKS

**/
// static
// EFI_STATUS
// EFIAPI
// OemPlatformHooks (
//   IN EFI_LEGACY_BIOS_PLATFORM_PROTOCOL   *This,
//   IN EFI_GET_PLATFORM_HOOK_MODE          Mode,
//   IN UINT16                              Type,
//   IN  EFI_HANDLE                         DeviceHandle,
//   IN  OUT UINTN                          *ShadowAddress,
//   IN  EFI_COMPATIBILITY16_TABLE          *Compatibility16Table,
//   IN  VOID                               **AdditionalData OPTIONAL
//   )
// {
//   EFI_STATUS                    Status;
//   EFI_IA32_REGISTER_SET         RegisterSet;
// 
//   switch (Mode) {
// 
//   case EfiPlatformHookPrepareToScanRom:
//     //
//     // The DeviceHandle will be NULL if the OptionROM to dispatch is not a PCI Option ROM
//     //
//     ConditionalSwitchToTextMode (DeviceHandle);
//     break;
// 
//   case EfiPlatformHookShadowServiceRoms:
//     InstallSOLOptionROM ();
//     break;
// 
//   case EfiPlatformHookAfterRomInit:
//     if (mGetVideoOriginalModeOK) {
//       ZeroMem (&RegisterSet, sizeof (EFI_IA32_REGISTER_SET));
//       RegisterSet.X.AX = 0x4F02;
//       RegisterSet.X.BX = mVideoOriginalMode;
//       mLegacyBios->Int86 (mLegacyBios, 0x10, &RegisterSet);
//       mGetVideoOriginalModeOK = FALSE;
//     }
//     break;
//   }
// 
//   Status = mLegacyBiosPlatformHooks (
//              This,
//              Mode,
//              Type,
//              DeviceHandle,
//              ShadowAddress,
//              Compatibility16Table,
//              AdditionalData
//              );
//   return Status;
// }

/**
  This routine check the PCI device to see if we need to switch to text mode.

  @param[in]  DeviceHandle  -   The PCI device handle when dispacth option rom

  @retval     None

**/
// static
// EFI_STATUS
// ConditionalSwitchToTextMode (
//   EFI_HANDLE    DeviceHandle
//   )
// {
//   EFI_STATUS                    Status;
//   EFI_IA32_REGISTER_SET         RegisterSet;
// #ifdef  RAID_OPTION_ROM
//   OEM_SERVICES_PROTOCOL         *OemServices;
//   SERVICE_ROM_TABLE             *ServiceRomTable;
//   UINTN                         FilenameIndex;
// #endif
// 
//   UINTN                         VarSize;
//   AMD_SYSTEM_CONFIGURATION      *AmdSystemConfiguration;  
// EFI_GUID                        AmdSetupConfigVarGuid = AMD_SYSTEM_CONFIGURATION_GUID;
//   	
//   VarSize = sizeof (AMD_SYSTEM_CONFIGURATION);
//   Status = gBS->AllocatePool (
//                   EfiBootServicesData,
//                   VarSize,
//                   &AmdSystemConfiguration
//                   );
//   Status = gRT->GetVariable (
//                   L"AmdSetup",
//                   &AmdSetupConfigVarGuid,
//                   NULL,
//                   &VarSize,
//                   AmdSystemConfiguration
//                   );
// 
// #ifdef  RAID_OPTION_ROM
// {
//   UINTN                             MemoryAddress;
//   VOID                              *RaidDLLAddr = NULL;
//   UINTN                             RaidDLLSize  = 0;
//   UINT32                            *SearchAddr;
//   UINT8                             *PTR;
//   UINT16                            Offset = 0, Segment = 0;
//   PMM_FUN_ARG                       Arg;
//   EFI_IA32_REGISTER_SET             Regs;
//   EFI_HANDLE                        *HandleBuffer;
//   UINTN                             NoHandles;
//   UINT16                            VendorId = 0;
//   UINT16                            DeviceId  = 0;
//   UINTN                             Index;
//   EFI_PCI_IO_PROTOCOL               *PciIo;
//   UINTN                             RaidOptionRomType;
// 
// //  if (!gFindPMM && (mSystemConfiguration->SataCnfigure == 1)) {
//   if (!gFindPMM && (AmdSystemConfiguration->CbsCmnFchSataClass == 1)) {
//     //
//     // find raid controller pciio
//     //
//     Status = gBS->LocateHandleBuffer (
//                     ByProtocol,
//                     &gEfiPciIoProtocolGuid,
//                     NULL,
//                     &NoHandles,
//                     &HandleBuffer
//                     );
// 
//     if (!EFI_ERROR (Status)) {
//       for (Index = 0; Index < NoHandles; Index++) {
//         Status = gBS->HandleProtocol (
//                         HandleBuffer[Index],
//                         &gEfiPciIoProtocolGuid,
//                         &PciIo
//                         );
//         if (EFI_ERROR (Status)) {
//           continue;
//         }
//         PciIo->Pci.Read (PciIo, EfiPciIoWidthUint16, PCI_VENDOR_ID_OFFSET, 1, &VendorId);
//         PciIo->Pci.Read (PciIo, EfiPciIoWidthUint16, PCI_DEVICE_ID_OFFSET, 1, &DeviceId);
// 
//         if (VendorId == FCH_SATA_VID && ((DeviceId == FCH_SATA_RAID_DID)||(DeviceId == FCH_SATA_RAID5_DID)||(DeviceId == FCH_SATA_RAID5A_DID))) { 
//           break;
//         }
//       }// end for
//     } // end if
//     if ((DeviceId == FCH_SATA_RAID5_DID)||(DeviceId == FCH_SATA_RAID5A_DID)) {
//       RaidOptionRomType = AMD_RAID_5_OPTION_ROM;
//     } else {
//       RaidOptionRomType = AMD_RAID_OPTION_ROM;
//     }
//     //
//     //  Locate OemService Protocol
//     //
//     Status = gBS->LocateProtocol (
//                     &gOemServicesProtocolGuid,
//                     NULL,
//                     &OemServices
//                     );
//   
//     if (EFI_ERROR (Status)) {
//       return EFI_NOT_FOUND;
//     }
// 
//     //
//     // Get Service Rom table from OptionRom Table of OemServices
//     //
//     Status = OemServices->Funcs[COMMON_OPTION_ROM_TABLE] (
//                             OemServices,
//                             COMMON_OPTION_ROM_TABLE_ARG_COUNT,
//                             SERVICE_ROM,
//                             &ServiceRomTable
//                             );
//     if (EFI_ERROR (Status)) {
//       return EFI_NOT_FOUND;
//     }   
// 
//     for (FilenameIndex = 0; ServiceRomTable[FilenameIndex].Valid; FilenameIndex++) {
//       if (ServiceRomTable[FilenameIndex].OpRomType == RaidOptionRomType) { 
//         Status = GetSectionFromAnyFv (
//                    &ServiceRomTable[FilenameIndex].FileName, 
//                    EFI_SECTION_RAW,
//                    0,
//                    &RaidDLLAddr,
//                    &RaidDLLSize
//                    );
//         ASSERT_EFI_ERROR (Status);
//         if (EFI_ERROR (Status)) {
//           continue;
//         } else {
//           break;
//         }
//       }
//     }
//     if (Status == EFI_SUCCESS ) {
//       //
//       //  Search $PMM for pmm service
//       //
//       for (MemoryAddress = 0xE0000; MemoryAddress < 0x0100000; ++MemoryAddress){
//          SearchAddr = (UINT32 *)MemoryAddress;
//          if ( *SearchAddr == PMM_SIGNATURE ) {
//            PTR = (UINT8 *) SearchAddr;
//            PTR +=7;
//            Offset = *(UINT16 *)PTR;
//            PTR +=2;
//            Segment = *(UINT16 *)PTR;
//            break;
//          }       
//       }
//       //
//       // try to using PMM find a 64K space in 0 ~512K 
//       //
//       ZeroMem (&Regs, sizeof (EFI_IA32_REGISTER_SET));
//       Arg.Function = 0x0;
//       Arg.LengthL = (UINT16)(RaidDLLSize >> 4);
//       Arg.LengthH = 0x0;    
//       Arg.HandleL = 0x5670;
//       Arg.HandleH = 0x1234;    
//       Arg.Flags = 0x1;      
//       Status = mLegacyBios->FarCall86 (
//                               mLegacyBios,
//                               Segment,
//                               Offset,
//                               &Regs,
//                               &Arg,
//                               sizeof (Arg)
//                               );   
//       MemoryAddress = (Regs.X.DX << 16 | Regs.X.AX);
//       if (MemoryAddress != 0) {
//         gBS->CopyMem ((UINT32 *)MemoryAddress, RaidDLLAddr, RaidDLLSize);
//         gFindPMM = TRUE;
//       } 
//       
//       ASSERT (MemoryAddress != 0);
//       gBS->FreePool (RaidDLLAddr);
//     }
//     gBS->FreePool (ServiceRomTable);
//   }
// }
// #endif
//   ZeroMem (&RegisterSet, sizeof (EFI_IA32_REGISTER_SET));
//   RegisterSet.X.AX = 0x4F03;
//   mLegacyBios->Int86 (mLegacyBios, 0x10, &RegisterSet);
//   if (RegisterSet.X.AX == 0x4F) {
//     mVideoOriginalMode = RegisterSet.X.BX;
//     mGetVideoOriginalModeOK = TRUE;
//   }
//   //
//   // Set the 80x25 Text VGA Mode before dispatch option ROM
//   //
//   RegisterSet.H.AH = 0x00;
//   RegisterSet.H.AL = 0x03;
//   Status    = mLegacyBios->Int86 (mLegacyBios, 0x10, &RegisterSet);
// 
//   return EFI_SUCCESS;
// }

/**
  This routine install SOL option ROM if needed

  @param[in]  None

  @retval     None

**/
// static
// VOID
// InstallSOLOptionROM (
//   VOID
//   )
// {
// }

/**
  Add the used time of designate IRQ

  @param[in]  IrqNumber - The IRQ which been assigned.

  @retval     None

**/
VOID
AddIrqUsedTime (
  IN UINT8        IrqNumber
  )
{
  UINTN   Index;

  Index = 0;

  for (Index = 0; Index < MAX_IRQ_PRIORITY_ENTRIES; Index++) {
    if (IrqPriorityTable[Index].Irq == IrqNumber) {
      IrqPriorityTable[Index].Used++;
      break;
    }
  }

  return ;
}

/**
  Get the suitable IRQ by request.

  @param[in]  SmallLimitation - The IRQ which must bigger than SmallLimitation.
              BigLimitation   - The IRQ which must smaller than BigLimitation.

  @retval     The suitable IRQ

**/
UINT8
GetSuitableIrq (
  IN UINT8        SmallLimitation,
  IN UINT8        BigLimitation
  )
{
  UINTN   Index;
  UINT8   SuitableIrq;
  UINT8   UsedTime;

  Index = 0;
  SuitableIrq = 0;
  UsedTime = 0xff;

  if (SmallLimitation < 1) {
    SmallLimitation = 1;
  }
  if (BigLimitation >= 0x10 ||
      BigLimitation == 0) {
    BigLimitation = 0x10;
  }

  for (Index = 0; Index < MAX_IRQ_PRIORITY_ENTRIES; Index++) {
    if (IrqPriorityTable[Index].Irq > SmallLimitation &&
        IrqPriorityTable[Index].Irq < BigLimitation &&
        IrqPriorityTable[Index].Irq != 0 &&
        IrqPriorityTable[Index].Used != LEGACY_USED &&
        IrqPriorityTable[Index].Used != PCI_USED &&
        IrqPriorityTable[Index].Used < UsedTime
      ) {
      SuitableIrq = IrqPriorityTable[Index].Irq;
      UsedTime = IrqPriorityTable[Index].Used;
    }
  }

  return SuitableIrq;
}

EFI_STATUS
SvcInstallIrqRoutingTableThunk (
  IN IRQ_ROUTING_TABLE **IrqRoutingTable
  )
{
  EFI_STATUS                            Status;
  LEGACY_MODIFY_PIR_TABLE               *VirtualBusTablePtr;
  UINT8                                 VirtualBusTableEntryNumber;
  UINT8                                 *PirqPriorityTablePtr;
  UINT8                                 PirqPriorityTableEntryNumber;
  EFI_LEGACY_IRQ_PRIORITY_TABLE_ENTRY   *IrqPoolTablePtr;
  UINT8                                 IrqPoolTableNumber;

  VirtualBusTablePtr             = NULL;
  VirtualBusTableEntryNumber     = 0;
  IrqPoolTablePtr                = NULL;
  IrqPoolTableNumber             = 0;
  PirqPriorityTablePtr           = NULL;
  PirqPriorityTableEntryNumber   = 0;

  //
  // make a call to DxeCsSvc internally
  //
  Status = DxeCsSvcIrqRoutingInformation( &VirtualBusTablePtr,    &VirtualBusTableEntryNumber, 
                                          &IrqPoolTablePtr,       &IrqPoolTableNumber, 
                                          &PirqPriorityTablePtr,  &PirqPriorityTableEntryNumber,
                                          IrqRoutingTable
                                         );   

  return Status;
}

/**
  A hook for LegacyBiosPlatform->TranslatePirq().

  @param  This                  The protocol instance pointer.
  @param  PciBus                The PCI bus number for this device.
  @param  PciDevice             The PCI device number for this device.
  @param  PciFunction           The PCI function number for this device.
  @param  Pirq                  Input is PIRQ reported by device, and output is true PIRQ.
  @param  PciIrq                The IRQ already assigned to the PIRQ, or the IRQ to be
                                assigned to the PIRQ.

  @retval EFI_SUCCESS           The PIRQ was translated.

**/
static
EFI_STATUS
EFIAPI
TranslatePirqHook (
  IN  EFI_LEGACY_BIOS_PLATFORM_PROTOCOL           *This,
  IN UINTN                                        PciBus,
  IN UINTN                                        PciDevice,
  IN UINTN                                        PciFunction,
  IN  OUT UINT8                                   *Pirq,
  OUT UINT8                                       *PciIrq
  )
{
  EFI_LEGACY_IRQ_ROUTING_ENTRY        *IrqRoutingEntryTablePtr;
  IRQ_ROUTING_TABLE                   *IrqTablePtr;
  EFI_LEGACY_INTERRUPT_PROTOCOL       *LegacyInterrupt;
  EFI_STATUS                          Status;
  UINTN                               Index;
  UINT8                               LocalPirq;
  UINT8                               PirqData;
  UINTN                               MaxIrqEntriesCount;

  Status = gBS->LocateProtocol (
                            &gEfiLegacyInterruptProtocolGuid,
                            NULL,
                            (VOID**)&LegacyInterrupt
                            );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  IrqTablePtr = NULL;
  Status = SvcInstallIrqRoutingTableThunk(&IrqTablePtr);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  MaxIrqEntriesCount = IrqTablePtr->MaxRoutingTableCount;
  IrqRoutingEntryTablePtr = IrqTablePtr->IrqRoutingTablePtr;

  LocalPirq = (UINT8) (*Pirq);

  for ( Index = 0; Index < MaxIrqEntriesCount; Index ++) {
    if ((IrqRoutingEntryTablePtr[Index].Bus == PciBus) &&
        (IrqRoutingEntryTablePtr[Index].Device == PciDevice)
        ) {

      LocalPirq = (UINT8) (IrqRoutingEntryTablePtr[Index].PirqEntry[LocalPirq].Pirq & 0x0f);
      //
      // Read PIRQ data from Chipset
      // (Note for SB800, 0x1f is the default value)
      //
      LegacyInterrupt->ReadPirq (
                         LegacyInterrupt,
                         LocalPirq,
                         &PirqData
                         );
      if (PirqData == 0x1f) {
        //
        // There is no IRQ assigned, get the suitable IRQ from SetPirqPriority table or IrqPriorityTable
        //
        PirqData = SetPIrqPriority[LocalPirq].Irq;
        if (PirqData != 0) {
          //
          // There is a IRQ already been assigned for this PIRQ before, so use it directly.
          //
          LegacyInterrupt->WritePirq (
                             LegacyInterrupt,
                             LocalPirq,
                             PirqData
                             );
        } else {
          //
          // There is no IRQ assigned, get the suitable IRQ from IrqPriorityTable
          //
          PirqData = GetSuitableIrq (0x0, 0x10);
          if (PirqData != 0) {
            //
            // Get the suitable IRQ from IrqPriority table, set it to PIRQ register in chipset.
            //
            LegacyInterrupt->WritePirq (
                               LegacyInterrupt,
                               LocalPirq,
                               PirqData
                               );
            SetPIrqPriority[LocalPirq].Irq = PirqData;
            AddIrqUsedTime (PirqData);
          } else {
            return EFI_NOT_FOUND;
          }
        }
      }

      //
      // There is already a IRQ been assigned in chipset, so use it directly.
      //
      *PciIrq = PirqData;
      *Pirq = LocalPirq;

      Index = 0;
      while (mCsDevRoutingMap[Index].PirqNum != 0xff) {
        if (mCsDevRoutingMap[Index].PirqNum == LocalPirq) {
          // Set IRQ number to PIC
          LegacyInterrupt->WritePirq (
                             LegacyInterrupt,
                             mCsDevRoutingMap[Index].CsReg,
                             PirqData
                             );
          // Set IRQ number to APIC
          LegacyInterrupt->WritePirq (
                             LegacyInterrupt,
                             mCsDevRoutingMap[Index].CsReg | BIT7,
                             LocalPirq | BIT4
                             );
        }
        Index++;
      }
      return EFI_SUCCESS;
    }
  }

  return  EFI_NOT_FOUND;
}
