/** @file
 DXE Chipset Services Library.

 This file contains only one function that is DxeCsSvcSkipGetPciRom().
 The function DxeCsSvcSkipGetPciRom() use chipset services to return
 a PCI ROM image for the device is represented or not.

***************************************************************************
* Copyright (c) 2013 - 2018, Insyde Software Corp. All Rights Reserved.
*
* You may not reproduce, distribute, publish, display, perform, modify, adapt,
* transmit, broadcast, present, recite, release, license or otherwise exploit
* any part of this publication in any form, by any means, without the prior
* written permission of Insyde Software Corporation.
*
******************************************************************************
*/

#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/AcpiPlatformLib.h>
#include <Library/BaseLib.h>
#include <Library/PciLib.h>
#include <Library/IoLib.h>
#include <Library/DxeOemSvcChipsetLib.h>
#include <Library/PciExpressLib.h>
#include <Dxe/AmdPbsSetupDxe/AmdPbsConfig.h>
#include <Library/AmdPbsConfigLib/AmdPbsConfigLib.h>
#include <Guid/EventGroup.h>

// #include <Library/BaseMemoryLib.h>
#include <Protocol/AllPciIoPrtclsInstlFinished.h>
#include <Protocol/GlobalNvsArea.h>
#include <Protocol/AcpiSupport.h>
#include <Protocol/MpService.h>
#include <Protocol/PciEnumerationComplete.h>
#include <Protocol/AmdCoreTopologyProtocol.h>

#include <IndustryStandard/MemoryMappedConfigurationSpaceAccessTable.h>
#include <IndustryStandard/WatchdogResourceTable.h>
#include <IndustryStandard/WindowsSmmSecurityMitigationTable.h>
#include <AcpiTables/Wsmt/Wsmt.h>

//#include <NvStore.h>
// #include <SetupConfig.h>
#include <KernelSetupConfig.h>
#include <ChipsetSetupConfig.h>
#include <Fch.h>
#include <Library/AmdPspBaseLibV2.h>

#include <Library/VariableLib.h>

#define LVL_C2_EN                         0x12
#define LVL_C3_EN                         0x53
#define LVL_C2_DIS                        101
#define LVL_C3_DIS                        1001
#define P_LVL2_LAT                        0x64
#define P_LVL3_LAT                        0x03e9

UINT8
GetEndBusNumber (
  VOID
  );

EFI_STATUS
RemoveAcpitable (
  UINT32       Signature
);

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

/**
 Update MADT table content
 Depends on MPService return data to setup MADT core enable flag and APIC ID.

 @param[in, out]    Table               The table to update
 @param[in]         SetupNVRam          SETUP Variable pointer

 @retval            EFI_SUCCESS         Update table success
 @retval            EFI_UNSUPPORTED     APIC Mode disabled
*/
STATIC
EFI_STATUS
UpdateMadt (
  IN OUT EFI_ACPI_DESCRIPTION_HEADER  *Table,
  IN UINT8                            *SetupNVRam
  )
{
  EFI_STATUS                                      Status;
  EFI_ACPI_1_0_PROCESSOR_LOCAL_APIC_STRUCTURE     *ApicPtr = NULL;
  EFI_ACPI_1_0_IO_APIC_STRUCTURE                  *IoApicPtr = NULL;
  UINT8                                           *CurrPtr = NULL;
  UINT8                                           CurrProcessor = 0;
  EFI_PROCESSOR_INFORMATION                       ProcessorInfo;
  EFI_MP_SERVICES_PROTOCOL                        *MpService;
  UINTN                                           NumCPUs = 1;
  UINTN                                           NumEnabledCPUs;
  KERNEL_CONFIGURATION                            *SetupNVRamKernel;
  UINT32                                          RegEax;

  SetupNVRamKernel  = (KERNEL_CONFIGURATION *)SetupNVRam;
  if (SetupNVRamKernel->IoApicMode == 0) {
    Status = EFI_UNSUPPORTED;
  }
  //
  // Find the MP Protocol. This is an MP platform, so MP protocol must be
  // there.
  //
  Status = gBS->LocateProtocol (
                  &gEfiMpServiceProtocolGuid,
                  NULL,
                  (VOID**)&MpService
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  //
  // Determine the number of processors
  //
  MpService->GetNumberOfProcessors (
               MpService,
               &NumCPUs,
               &NumEnabledCPUs
               );

  CurrPtr = (UINT8*) &((EFI_ACPI_DESCRIPTION_HEADER*) (Table))[1];
  CurrPtr = CurrPtr + 8; // Size of Local APIC Address & Flag
  while (CurrPtr < ((UINT8*) (Table) + ((Table)->Length))) {
    ApicPtr = (EFI_ACPI_1_0_PROCESSOR_LOCAL_APIC_STRUCTURE*) CurrPtr;
    //
    // Check table entry type
    //
    if (ApicPtr->Type == EFI_ACPI_1_0_PROCESSOR_LOCAL_APIC) {
      ApicPtr->Flags  = 0;
      ApicPtr->ApicId = 0;

      //
      // Look up this processor information
      //
      Status = MpService->GetProcessorInfo (
                            MpService,
                            CurrProcessor,
                            &ProcessorInfo
                            );
      if (!EFI_ERROR (Status)) {
        if (ProcessorInfo.StatusFlag & PROCESSOR_ENABLED_BIT) {
          ApicPtr->Flags = EFI_ACPI_1_0_LOCAL_APIC_ENABLED;
        }
        ApicPtr->ApicId = (UINT8) (ProcessorInfo.ProcessorId);
      }

      //
      // Increment the procesor count
      //
      CurrProcessor++;
    }

    // Update IO APIC ID for family 17 only
    // Make sure IO APIC ID in MADT is aligned with AGESA IVRS table
    AsmCpuid (0x80000001, &RegEax, NULL, NULL, NULL);
    if (((RegEax >> 20) & 0xFF) != 0x6) {
      IoApicPtr = (EFI_ACPI_1_0_IO_APIC_STRUCTURE *) CurrPtr;
      if (IoApicPtr->Type == EFI_ACPI_1_0_IO_APIC) {
        if (IoApicPtr->IoApicId == 4) {
          IoApicPtr->IoApicId = PcdGet8 (PcdCfgFchIoapicId);
        } else if (IoApicPtr->IoApicId == 5) {
          IoApicPtr->IoApicId = PcdGet8 (PcdCfgGnbIoapicId);
        }
      }
    }
    //
    // Go to the next structure in the APIC table
    //
    CurrPtr += (ApicPtr->Length);
  }

  return EFI_SUCCESS;
}

/**
 Update FACP Table according to platform specific.

 @param[in, out]    Table               The table to update
 @param[in]         SetupNVRam          SETUP Variable pointer

 @retval            EFI_SUCCESS         Update table success
*/
STATIC
EFI_STATUS
UpdateFacp (
  IN OUT EFI_ACPI_DESCRIPTION_HEADER  *Table,
  IN UINT8                            *SetupNVRam
  )
{
  EFI_ACPI_1_0_FIXED_ACPI_DESCRIPTION_TABLE   *FadtPointer1;
  EFI_ACPI_3_0_FIXED_ACPI_DESCRIPTION_TABLE   *FadtPointer3;
  EFI_STATUS                                  Status;
  AMD_PBS_SETUP_OPTION                        AmdPbsConfiguration;

  Status = GetAmdPbsConfiguration (&AmdPbsConfiguration);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  FadtPointer1 = (EFI_ACPI_1_0_FIXED_ACPI_DESCRIPTION_TABLE*) Table;
  FadtPointer3 = (EFI_ACPI_3_0_FIXED_ACPI_DESCRIPTION_TABLE*) Table;

  // C2 Latency value in FACP.
  FadtPointer1->PLvl2Lat = (UINT16) P_LVL2_LAT;
  // C3 Latency value in FACP.
  FadtPointer1->PLvl3Lat = (UINT16) P_LVL3_LAT;

  FadtPointer1->Flags |= EFI_ACPI_1_0_P_LVL2_UP;
  if (FeaturePcdGet (PcdLegacyFreeSupported)) {
    FadtPointer1->Flags |= EFI_ACPI_2_0_RESET_REG_SUP;
  }

 // The following HW_REDUCED_ACPI and LOW_POWER_S0_IDLE_CAPABLE flags are supposed to be
 // set up by FchD3ColdTbl in AGESA, but it is currently broken.  Let's do them here until
 // that bug is resolved.
 // debug {
#ifdef CONNECTED_STANDBY_SUPPORT
  if (FeaturePcdGet (PcdModernStandbySupport)) {
    if (AmdPbsConfiguration.ModernStandbyEn == 1) {
      FadtPointer1->Flags|=BIT21;           // Set LOW_POWER_S0_IDLE_CAPABLE flag
    } else {
      FadtPointer1->Flags&=~BIT21;          // Reset LOW_POWER_S0_IDLE_CAPABLE flag
    }
  }
#endif

#ifdef REDUCED_HARDWARE_SUPPORT
  if (FeaturePcdGet (PcdReduceHardwareSupport)) {
    if (AmdPbsConfiguration.ReduceHardwareEn == 1) {
      FadtPointer1->Flags|=BIT20;           // Set HW_REDUCED_ACPI flag
      FadtPointer1->Flags|=BIT4;            // Set PWR_BUTTON flag to 1 to use Control method power button
    } else {
      FadtPointer1->Flags&=~BIT20;          // Reset HW_REDUCED_ACPI flag
      FadtPointer1->Flags&=~BIT4;           // Set PWR_BUTTON flag to 0 to use Fixed hardware power button
    }
  }
#endif
  return EFI_SUCCESS;
}

VOID
EFIAPI
UpdatePspBar3 (
  IN EFI_EVENT                        Event,
  IN VOID                             *Context
  )
{
  EFI_STATUS                          Status;
  EFI_ACPI_SUPPORT_PROTOCOL           *AcpiSupport;
  INTN                                Index;
  UINTN                               Handle;
  EFI_ACPI_TABLE_VERSION              Version;
  EFI_ACPI_DESCRIPTION_HEADER         *Table;
  UINT8                               *Operation;
  UINT8                               *DsdtPointer;
  UINT32                              *RegionAddress;
  UINT32                              *Signature;
  BOOLEAN                             IsFound;

  DEBUG ((EFI_D_ERROR, "    UpdatePspBar3\n"));
  IsFound = FALSE;

  //
  // Found DSDT table
  //
  Status = gBS->LocateProtocol (
                  &gEfiAcpiSupportProtocolGuid,
                  NULL,
                  (VOID**)&AcpiSupport
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "      ERR: Failed to locate gEfiAcpiSupportProtocolGuid\n"));
    return;
  }

  Index  = 0;
  Handle = 0;
  do {
    Table  = NULL;
    Status = AcpiSupport->GetAcpiTable (
                            AcpiSupport,
                            Index,
                            (VOID**)&Table,
                            &Version,
                            &Handle
                            );
    if (EFI_ERROR (Status)) {
      break;
    }

    //
    // Check Signture and update DSDT table
    //
    if (Table->Signature == SIGNATURE_32('D', 'S', 'D', 'T')) {
      for (DsdtPointer = (UINT8 *)((UINTN)Table + sizeof (EFI_ACPI_COMMON_HEADER)); DsdtPointer <= (UINT8 *)((UINTN)Table + (Table->Length)); DsdtPointer ++) {
        Signature = (UINT32*) DsdtPointer;

        switch (*Signature) {

        case (SIGNATURE_32 ('B', 'A', 'R', '3')):
          Operation = (DsdtPointer - 1);
          if ((*Operation == AML_NAME_OP)) {
            RegionAddress = (UINT32 *)(DsdtPointer + 5);

            if (GetPspMmioBase (RegionAddress)) {
              IsFound = TRUE;
              DEBUG ((EFI_D_ERROR, "    PSP BAR3 ADDR = 0x%x\n", *RegionAddress));
            } else {
              ASSERT (FALSE);
            }

          }
          break;

        default:
          break;
        }
      }

      //
      // Great, we have found and update the table.
      // Publish it now.
      //
      if (IsFound) {
        Status = AcpiSupport->SetAcpiTable (
                              AcpiSupport,
                              Table,
                              TRUE,
                              Version,
                              &Handle
                              );
        if (!EFI_ERROR (Status)) {
          Status = AcpiSupport->PublishTables (
                                AcpiSupport,
                                EFI_ACPI_TABLE_VERSION_1_0B | EFI_ACPI_TABLE_VERSION_2_0 | EFI_ACPI_TABLE_VERSION_3_0 | EFI_ACPI_TABLE_VERSION_4_0
                                );
        }
        gBS->FreePool (Table);

        return;
      }
    }

    //
    // Not found, get next
    //
    gBS->FreePool (Table);
    Index++;
  } while (TRUE);

  return;
}

/**
 Update Dsdt Table according to platform specific.

 @param [in, out]   Table               The table to update
 @param [in]        SetupNVRam          SETUP Variable pointer

 @retval            EFI_SUCCESS         Update Table Success
*/
STATIC
EFI_STATUS
UpdateDsdt (
  IN OUT EFI_ACPI_DESCRIPTION_HEADER  *Table,
  IN UINT8                            *SetupNVRam
  )
{
  EFI_STATUS                          Status;
  UINT8                               *DsdtPointer;
  UINT32                              *Signature;
  EFI_GLOBAL_NVS_AREA_PROTOCOL        *GlobalNvsAreaProtocol;
  EFI_GLOBAL_NVS_AREA                 *mGlobalNvsArea;
  EFI_OEM_GLOBAL_NVS_AREA             *mOemGlobalNvsArea;
  KERNEL_CONFIGURATION                *SetupNVRamKernel;
  CHIPSET_CONFIGURATION               *SetupNVRamChipset;
//  NV_STORE_STRUCT                     *NvStore = NULL;
  UINT64                              TopMem;
  UINT8                              *RegionAddress;
  UINT8                               *Operation;
  EFI_EVENT                           Event;
  VOID                                *Registration;
  AMD_PBS_SETUP_OPTION                AmdPbsConfiguration;
  UINTN                                 NumberOfComplexes;
  UINTN                                 NumberOfCores;
  UINTN                                 NumberOfThreads;

  NumberOfComplexes = 0;
  NumberOfCores = 0;
  NumberOfThreads = 0;

  Status = gBS->LocateProtocol (&gEfiGlobalNvsAreaProtocolGuid, NULL, (VOID**)&GlobalNvsAreaProtocol);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = GetAmdPbsConfiguration (&AmdPbsConfiguration);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  mGlobalNvsArea    = GlobalNvsAreaProtocol->Area;
  mOemGlobalNvsArea = GlobalNvsAreaProtocol->OemArea;

  SetupNVRamKernel  = (KERNEL_CONFIGURATION *)SetupNVRam;
  SetupNVRamChipset = (CHIPSET_CONFIGURATION *)SetupNVRam;

  //
  // Initial Global NVS Data - Start
  //
    if (PcdGetBool (PcdAmdAcpiS3Support) == FALSE) {
      mGlobalNvsArea->DisableAcpiS3 = 0;
    } else {
      mGlobalNvsArea->DisableAcpiS3 = 1;
    }
  mGlobalNvsArea->amdThermalSensorSelect = SetupNVRamChipset->amdThermalSensorSelect;
  mGlobalNvsArea->amdTNBHystersis = SetupNVRamChipset->amdCpuHysteresisInfo;
  mGlobalNvsArea->amdTCPU0 = SetupNVRamChipset->amdCpuAC1;
  mGlobalNvsArea->amdTCPU1 = SetupNVRamChipset->amdCpuAC0;
  mGlobalNvsArea->amdPwmCPU0 = SetupNVRamChipset->amdCpuAL1;
  mGlobalNvsArea->amdPwmCPU1 = SetupNVRamChipset->amdCpuAL0;
  mGlobalNvsArea->amdLcdPanelType = SetupNVRamChipset->amdLcdPanelType;
//[-start-190529-IB06070029-modify]//
  mGlobalNvsArea->WakeOnPME = SetupNVRamKernel->WakeOnPME;
//[-end-190529-IB06070029-modify]//
  mGlobalNvsArea->amdExternalUSBController = SetupNVRamChipset->amdExternalUSBController;

  mGlobalNvsArea->AcpiThermalZone = SetupNVRamChipset->ThermalFanControl;
  mGlobalNvsArea->PcieBaseAddress = (UINT32)PcdGet64 (PcdPciExpressBaseAddress);
  mGlobalNvsArea->PcieBaseLimit = (UINT32)(PcdGet64 (PcdPciExpressBaseAddress) + ((GetEndBusNumber () + 1) * 0x100000) - 1);
  mGlobalNvsArea->MITTWITTControl = AmdPbsConfiguration.MITTWITTControl;

  if (FeaturePcdGet (PcdNbIoApicSupport)) {
    mGlobalNvsArea->NbIoApic = TRUE;
  } else {
    mGlobalNvsArea->NbIoApic = FALSE;
  }
  mGlobalNvsArea->BoardId = PcdGet16 (OriginalPlatformId);
  mGlobalNvsArea->KbcSupport = AmdPbsConfiguration.KbcSupport;
  mGlobalNvsArea->EcSupport = 1;

  TopMem = AsmReadMsr64(0x0C0010058);
  mOemGlobalNvsArea->PcieBar = (UINT8)(TopMem >> 24);

//[-start-190618-IB06070035-add]//
  #ifdef EFI_DEBUG
    mOemGlobalNvsArea->InternalBuild = (UINT8)1;
  #endif
//[-end-190618-IB06070035-add]//

  if (CheckRvOpnB4()){
    mOemGlobalNvsArea->RV2Identifier = 0;
  } else {
    mOemGlobalNvsArea->RV2Identifier = 1;
  }
  mGlobalNvsArea->BatteryValue = DEFAULT_BATTERY_VALUE;
  mGlobalNvsArea->IgdStatus = PRIMARY_DISPLAY;
  if (!FeaturePcdGet (PcdLegacyFreeTiSupport)) {
    mGlobalNvsArea->BrightnessLevel = DEFAULT_BRIGHTNESS_LEVEL;
  }
  mGlobalNvsArea->CurrentDispState = (PIPE_A_CRT | (PIPE_B_CRT));
  mGlobalNvsArea->ConnectStandbySupport = 0;
  mGlobalNvsArea->AcpiReduceHwSupport = 0;

  for (DsdtPointer = (UINT8 *)((UINTN)Table + sizeof (EFI_ACPI_DESCRIPTION_HEADER)); DsdtPointer <= (UINT8 *)((UINTN)Table + (UINTN)(Table->Length)); DsdtPointer++) {
    Signature = (UINT32*) DsdtPointer;

    switch (*Signature) {

//    case (SIGNATURE_32 ('N', 'V', 'S', 'T')): // NVST operation region
//      if (IsAmlOpRegionObject (DsdtPointer)) {
//        SetOpRegion (DsdtPointer, NvStore, sizeof (NV_STORE_STRUCT));
//      }
//      break;

    case (SIGNATURE_32 ('_', 'O', 'S', 'C')): // _OSC operation region
      if (SetupNVRamChipset->OscSwitch == 0) {
        *((UINT32*)DsdtPointer) = SIGNATURE_32 ('X', 'O', 'S', 'C');
      }
      break;

    case (SIGNATURE_32 ('P', 'E', 'B', 'A')): // PEBA operation region
      if (IsAmlOpRegionObject (DsdtPointer)) {
      }
      break;

    case (SIGNATURE_32 ('X', 'P', 'E', 'X')): // XPEX operation region
      if (IsAmlOpRegionObject (DsdtPointer)) {
      }
      break;

    case (SIGNATURE_32 ('S', 'T', 'S', 'I')):
      if (IsAmlOpRegionObject (DsdtPointer)) {
      }
      break;

    case (SIGNATURE_32 ('G', 'N', 'V', 'S')):
      if (IsAmlOpRegionObject (DsdtPointer)) {
        SetOpRegion (DsdtPointer, mGlobalNvsArea, sizeof (EFI_GLOBAL_NVS_AREA));
      }
      break;

    case (SIGNATURE_32 ('O', 'G', 'N', 'S')):
      if (IsAmlOpRegionObject (DsdtPointer)) {
        SetOpRegion (DsdtPointer, mOemGlobalNvsArea, sizeof (EFI_OEM_GLOBAL_NVS_AREA));
      }
      break;

    case (SIGNATURE_32 ('F', 'U', '3', 'E')): //xHCI Enable
      break;

//    case (SIGNATURE_32 ('A', 'F', '7', 'E')):
//      if (SetupNVRamChipset->BrightnessSwitch == 1) { //Brightness control by driver Enable
//        Operation = (DsdtPointer - 1);
//        if ((*Operation) == 0x08) {                       //Default: Name(AF7E,0x80000000)
//        RegionAddress = (UINT32 *)(DsdtPointer + 5);
//        *RegionAddress = (UINT32)0x80000001;
//        }
//      }
//      break;

    case (SIGNATURE_32 ('B', 'A', 'R', '3')):
      Status = gBS->CreateEvent (
                       EVT_NOTIFY_SIGNAL,
                       TPL_NOTIFY,
                       UpdatePspBar3,
                       NULL,
                       &Event
                      );
      if (!EFI_ERROR (Status)) {
        Status = gBS->RegisterProtocolNotify (
                      &gEfiPciEnumerationCompleteProtocolGuid,
                      Event,
                      &Registration
                      );
      }
      break;

    case (SIGNATURE_32 ('C', '0', '0', 'E')):
     if (NumberOfCores >= 1) {
        Operation = (DsdtPointer - 1);
        if ((*Operation == AML_NAME_OP)) {
          RegionAddress = (UINT8 *)(DsdtPointer + 4);
          *RegionAddress = 1;
        }
     }
     break;
    case (SIGNATURE_32 ('C', '0', '1', 'E')):
     if (NumberOfCores >= 2) {
        Operation = (DsdtPointer - 1);
        if ((*Operation == AML_NAME_OP)) {
          RegionAddress = (UINT8 *)(DsdtPointer + 4);
          *RegionAddress = 1;
        }
     }
     break;
    case (SIGNATURE_32 ('C', '0', '2', 'E')):
     if (NumberOfCores >= 3) {
        Operation = (DsdtPointer - 1);
        if ((*Operation == AML_NAME_OP)) {
          RegionAddress = (UINT8 *)(DsdtPointer + 4);
          *RegionAddress = 1;
        }
     }
     break;
    case (SIGNATURE_32 ('C', '0', '3', 'E')):
     if (NumberOfCores >= 4) {
        Operation = (DsdtPointer - 1);
        if ((*Operation == AML_NAME_OP)) {
          RegionAddress = (UINT8 *)(DsdtPointer + 4);
          *RegionAddress = 1;
        }
     }
     break;
    case (SIGNATURE_32 ('C', '0', '4', 'E')):
     if (NumberOfCores >= 5) {
        Operation = (DsdtPointer - 1);
        if ((*Operation == AML_NAME_OP)) {
          RegionAddress = (UINT8 *)(DsdtPointer + 4);
          *RegionAddress = 1;
        }
     }
     break;
    case (SIGNATURE_32 ('C', '0', '5', 'E')):
     if (NumberOfCores >= 6) {
        Operation = (DsdtPointer - 1);
        if ((*Operation == AML_NAME_OP)) {
          RegionAddress = (UINT8 *)(DsdtPointer + 4);
          *RegionAddress = 1;
        }
     }
     break;
    case (SIGNATURE_32 ('C', '0', '6', 'E')):
     if (NumberOfCores >= 7) {
        Operation = (DsdtPointer - 1);
        if ((*Operation == AML_NAME_OP)) {
          RegionAddress = (UINT8 *)(DsdtPointer + 4);
          *RegionAddress = 1;
        }
     }
     break;
    case (SIGNATURE_32 ('C', '0', '7', 'E')):
     if (NumberOfCores >= 8) {
        Operation = (DsdtPointer - 1);
        if ((*Operation == AML_NAME_OP)) {
          RegionAddress = (UINT8 *)(DsdtPointer + 4);
          *RegionAddress = 1;
        }
     }
     break;
    //
    //For Moedern standby and Reduced hardware end
    //
    default:
      break;
    }
  }
  Status = OemSvcUpdateGlobalNvs (mGlobalNvsArea, mOemGlobalNvsArea);

  return EFI_SUCCESS;
}

/**
 Update Ssdt Table according to platform specific.

 @param[in, out]    Table               The table to update
 @param[in]         SetupNVRam          SETUP Variable pointer

 @retval            EFI_SUCCESS         Update Table Success
*/
STATIC
EFI_STATUS
UpdateSsdt (
  IN OUT EFI_ACPI_DESCRIPTION_HEADER  *Table,
  IN UINT8                            *SetupNVRam
  )
{
  return EFI_SUCCESS;
}

/**
 Update HPET Table according to platform specific.

 @param[in, out]    Table               The table to update
 @param[in]         SetupNVRam          SETUP Variable pointer

 @retval            EFI_SUCCESS         Update Table Success
*/
STATIC
EFI_STATUS
UpdateHpet (
  IN OUT EFI_ACPI_DESCRIPTION_HEADER  *Table,
  IN UINT8                            *SetupNVRam
  )
{
  KERNEL_CONFIGURATION                *SetupNVRamKernel;

  SetupNVRamKernel = (KERNEL_CONFIGURATION *)SetupNVRam;

  //
  // If HPET is disabled in setup, don't publish the table.
  //
  if (!SetupNVRamKernel->HPETSupport) {
    return EFI_UNSUPPORTED;
  }

  return EFI_SUCCESS;
}

UINT8
GetEndBusNumber (
  VOID
  )
{
  UINT8                               Value8;
  UINT64                              MsrData;
  UINT8                               EndBusNumber;

  MsrData = AsmReadMsr64 (0xC0010058);
  Value8 = (UINT8)((MsrData >> 2) & 0x0F);
  EndBusNumber = (UINT8)((1 << Value8) - 1);

  return EndBusNumber;
}

/**
 Update MCFG Table according to platform specific.

 @param[in, out]    Table               The table to update
 @param[in]         SetupNVRam          SETUP Variable pointer

 @retval            EFI_SUCCESS         Update Table Success
*/
STATIC
EFI_STATUS
UpdateMcfg (
  IN OUT EFI_ACPI_DESCRIPTION_HEADER  *Table,
  IN UINT8                            *SetupNVRam
  )
{
  EFI_ACPI_MEMORY_MAPPED_ENHANCED_CONFIGURATION_SPACE_BASE_ADDRESS_ALLOCATION_STRUCTURE *McfgTbl;
  UINT32                              TableSize;
  UINT32                              Index;
  UINTN                               TableAddress;

  TableSize = ((Table)->Length - sizeof (EFI_ACPI_DESCRIPTION_HEADER) - sizeof (UINT64));
  TableSize /= sizeof (EFI_ACPI_MEMORY_MAPPED_ENHANCED_CONFIGURATION_SPACE_BASE_ADDRESS_ALLOCATION_STRUCTURE);
  TableAddress = (UINTN)(Table);
  TableAddress += sizeof (EFI_ACPI_DESCRIPTION_HEADER);
  TableAddress += sizeof (UINT64);
  McfgTbl = (EFI_ACPI_MEMORY_MAPPED_ENHANCED_CONFIGURATION_SPACE_BASE_ADDRESS_ALLOCATION_STRUCTURE*)(UINTN)TableAddress;
  for (Index = 0; Index < TableSize; Index++) {
    McfgTbl->BaseAddress = PcdGet64 (PcdPciExpressBaseAddress);
    McfgTbl->EndBusNumber = GetEndBusNumber ();
    McfgTbl += sizeof (EFI_ACPI_MEMORY_MAPPED_ENHANCED_CONFIGURATION_SPACE_BASE_ADDRESS_ALLOCATION_STRUCTURE);
  }

  return EFI_SUCCESS;
}

EFI_STATUS
RemoveAcpitable (
  UINT32       Signature
  )
{
  EFI_STATUS                              Status;
  EFI_ACPI_SUPPORT_PROTOCOL               *AcpiSupport;
  INTN                                    Index;
  UINTN                                   Handle;
  EFI_ACPI_TABLE_VERSION                  Version;
  EFI_ACPI_DESCRIPTION_HEADER             *Table;

  //
  // Found  table
  //
  Status = gBS->LocateProtocol (
                  &gEfiAcpiSupportProtocolGuid,
                  NULL,
                  (VOID**)&AcpiSupport
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Index  = 0;
  Handle = 0;
  do {
    Table = NULL;
    Status = AcpiSupport->GetAcpiTable (
                            AcpiSupport,
                            Index,
                            (VOID**)&Table,
                            &Version,
                            &Handle
                            );
    if (EFI_ERROR (Status)) {
      break;
    }

    //
    // Check Signture and update table
    //
    if (Table->Signature == Signature) {
      //
      // Delete the table
      //
      gBS->FreePool (Table);
      Table = NULL;
      Status = AcpiSupport->SetAcpiTable (
                              AcpiSupport,
                              Table,
                              TRUE,
                              Version,
                              &Handle
                              );
      return Status;
    }
    //
    // Not found, get next
    //
    gBS->FreePool (Table);
    Index++;
  } while (TRUE);

  return Status;
}

/**
  Remove WDRT TABLE

  @param[in]        Event               Event whose notification function is being invoked.
  @param[in]        Context             Pointer to the notification function's context.

*/
VOID
EFIAPI
RemoveWdrtEventNotify (
  IN EFI_EVENT    Event,
  IN VOID         *Context
  )
{
  EFI_STATUS                              Status;

  Status = RemoveAcpitable(EFI_ACPI_3_0_WATCHDOG_RESOURCE_TABLE_SIGNATURE);
  gBS->CloseEvent (Event);
}

/**
  Remove WDAT TABLE

  @param[in]        Event               Event whose notification function is being invoked.
  @param[in]        Context             Pointer to the notification function's context.

*/
VOID
EFIAPI
RemoveWdatEventNotify (
  IN EFI_EVENT    Event,
  IN VOID         *Context
  )
{
  EFI_STATUS                              Status;

  Status = RemoveAcpitable(EFI_ACPI_3_0_WATCHDOG_ACTION_TABLE_SIGNATURE);
  gBS->CloseEvent (Event);
}

STATIC
VOID
GetWatchDogBase (
  OUT UINT32                          *WatchDogBase
  )
{
  *WatchDogBase = PcdGet32 (PcdWatchdogTimerBase);
}

/**
 Update Wdrt Table according to platform specific.

 @param[in, out]    Table               The table to update
 @param[in]         SetupNVRam          SETUP Variable pointer

 @retval            EFI_SUCCESS         Update Table Success
*/
STATIC
EFI_STATUS
UpdateWdrt (
  IN OUT EFI_ACPI_DESCRIPTION_HEADER  *Table,
  IN UINT8                            *SetupNVRam
  )
{
  EFI_STATUS                              Status;
  EFI_ACPI_WATCHDOG_RESOURCE_1_0_TABLE    *WdrtPointer;
  UINT32                                  ControlRegisterAddress;
  EFI_EVENT                               RemoveWdrtEvent;

  Status = EFI_SUCCESS;
  if (MmioRead32 (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REG00) & BIT7) {
    GetWatchDogBase (&ControlRegisterAddress);

    WdrtPointer = (EFI_ACPI_WATCHDOG_RESOURCE_1_0_TABLE*) Table;
    WdrtPointer->ControlRegisterAddress.Address = ControlRegisterAddress;
    WdrtPointer->CountRegisterAddress.Address   = ControlRegisterAddress + 4;
  } else {
    Status = gBS->CreateEventEx (
                  EVT_NOTIFY_SIGNAL,
                  TPL_CALLBACK,
                  RemoveWdrtEventNotify,
                  NULL,
                  &gEfiEventReadyToBootGuid,
                  &RemoveWdrtEvent
                  );
  }

  return Status;
}

/**
 Update Wdat Table according to platform specific.

 @param[in, out]    Table               The table to update
 @param[in]         SetupNVRam          SETUP Variable pointer

 @retval            EFI_SUCCESS         Update Table Success
*/
STATIC
EFI_STATUS
UpdateWdat (
  IN OUT EFI_ACPI_DESCRIPTION_HEADER  *Table,
  IN UINT8                            *SetupNVRam
  )
{
  EFI_STATUS                              Status;
  EFI_EVENT                               RemoveWdatEvent;

  Status = EFI_SUCCESS;
  if (!(MmioRead32 (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REG00) & BIT7)) {
  	 Status = gBS->CreateEventEx (
                  EVT_NOTIFY_SIGNAL,
                  TPL_CALLBACK,
                  RemoveWdatEventNotify,
                  NULL,
                  &gEfiEventReadyToBootGuid,
                  &RemoveWdatEvent
                  );
  }

  return Status;
}

/**
 Update ACPI table content according to platform specific

 @param[in, out]    TableHeader             Pointer of the table to update
 @param[in, out]    CommonCodeReturnStatus  Return Status from Common Code
*/
VOID
UpdateAcpiTable (
  IN OUT EFI_ACPI_DESCRIPTION_HEADER      *TableHeader,
  IN OUT EFI_STATUS                       *CommonCodeReturnStatus
)
{
  UINT8                                   *SetupNVRam;
  EFI_ACPI_WSMT_TABLE                     *pWSMT;

  SetupNVRam = NULL;

  SetupNVRam = CommonGetVariableData (SETUP_VARIABLE_NAME, &gSystemConfigurationGuid);
  if (SetupNVRam == NULL) {
    return;
  }

  switch ((TableHeader)->Signature) {

  case EFI_ACPI_3_0_MULTIPLE_APIC_DESCRIPTION_TABLE_SIGNATURE:
    *CommonCodeReturnStatus = UpdateMadt (TableHeader, SetupNVRam);
    break;

  case EFI_ACPI_3_0_FIXED_ACPI_DESCRIPTION_TABLE_SIGNATURE:
    *CommonCodeReturnStatus= UpdateFacp (TableHeader, SetupNVRam);
    break;

  case EFI_ACPI_3_0_DIFFERENTIATED_SYSTEM_DESCRIPTION_TABLE_SIGNATURE:
    *CommonCodeReturnStatus = UpdateDsdt (TableHeader, SetupNVRam);
    break;

  case EFI_ACPI_3_0_SECONDARY_SYSTEM_DESCRIPTION_TABLE_SIGNATURE:
    *CommonCodeReturnStatus = UpdateSsdt (TableHeader, SetupNVRam);
    break;

  case EFI_ACPI_3_0_HIGH_PRECISION_EVENT_TIMER_TABLE_SIGNATURE:
    *CommonCodeReturnStatus = UpdateHpet (TableHeader, SetupNVRam);
    break;

  case EFI_ACPI_3_0_PCI_EXPRESS_MEMORY_MAPPED_CONFIGURATION_SPACE_BASE_ADDRESS_DESCRIPTION_TABLE_SIGNATURE:
    *CommonCodeReturnStatus = UpdateMcfg (TableHeader, SetupNVRam);
    break;

  case EFI_ACPI_3_0_WATCHDOG_RESOURCE_TABLE_SIGNATURE:
    *CommonCodeReturnStatus = UpdateWdrt (TableHeader, SetupNVRam);
    break;

  case EFI_ACPI_3_0_WATCHDOG_ACTION_TABLE_SIGNATURE:
    *CommonCodeReturnStatus = UpdateWdat (TableHeader, SetupNVRam);
    break;

  case EFI_ACPI_WINDOWS_SMM_SECURITY_MITIGATION_TABLE_SIGNATURE:
    pWSMT = (EFI_ACPI_WSMT_TABLE *)TableHeader;
    pWSMT->ProtectionFlags = (UINT32)(PcdGet32(PcdWsmtProtectionFlags));
    break;
  }
}
