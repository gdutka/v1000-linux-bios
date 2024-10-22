/* $NoKeywords:$ */
/**
 * @file
 *
 * AmdNbioPcieRVDxe Driver
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  AmdNbioPcieRVDxe
 * @e \$Revision: 312065 $   @e \$Date: 2015-01-30 04:23:05 -0600 (Fri, 30 Jan 2015) $
 *
 */
/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */
#include <PiDxe.h>
#include <Filecode.h>
#include <Library/AmdBaseLib.h>
#include <Library/IdsLib.h>
#include <Library/GnbLib.h>
#include <Library/GnbPciLib.h>
#include <Library/GnbPciAccLib.h>
#include <Library/NbioHandleLib.h>
#include <Library/PcieConfigLib.h>
#include <SocLogicalId.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Guid/GnbPcieInfoHob.h>
#include <Library/UefiLib.h>
#include <Protocol/AcpiTable.h>
#include <Library/NbioIommuIvrsLib.h>
#include <Protocol/AmdNbioPcieServicesProtocol.h>
#include <Protocol/SocLogicalIdProtocol.h>
#include <IdsHookId.h>
#include <Library/AmdIdsHookLib.h>
#include <Library/NbioRegisterAccLib.h>
#include <CcxRegistersRv.h>
#include <GnbRegistersRV.h>

#define FILECODE        NBIO_IOMMU_AMDNBIOIOMMURVDXE_AMDNBIOIOMMURVDXE_FILECODE

VOID
IommuMsixWorkaround (
  IN       GNB_HANDLE  *GnbHandle
  );


/*----------------------------------------------------------------------------------------*/
/**
 * Enable IOMMU base address. (MMIO space )
 *
 *
 * @param[in]     GnbHandle       GNB handle
 * @param[in]     StdHeader       Standard Configuration Header
 * @retval        AGESA_SUCCESS
 * @retval        AGESA_ERROR
 */
AGESA_STATUS
STATIC
EnableIommuMmio (
  IN       GNB_HANDLE           *GnbHandle,
  IN       AMD_CONFIG_PARAMS    *StdHeader
  )
{
  AGESA_STATUS            Status;
  UINT16                  CapabilityOffset;
  UINT64                  BaseAddress;
  UINT32                  Value;
  PCI_ADDR                GnbIommuPciAddress;

  Status = AGESA_SUCCESS;
  IDS_HDT_CONSOLE (GNB_TRACE, "GnbEnableIommuMmio Enter\n");
  GnbIommuPciAddress = NbioGetHostPciAddress (GnbHandle);
  GnbIommuPciAddress.Address.Function = 0x2;
  if (GnbLibPciIsDevicePresent (GnbIommuPciAddress.AddressValue, StdHeader)) {
    CapabilityOffset = GnbLibFindPciCapability (GnbIommuPciAddress.AddressValue, IOMMU_CAP_ID, StdHeader);

    GnbLibPciRead (GnbIommuPciAddress.AddressValue | (CapabilityOffset + 0x8), AccessWidth32, &Value, StdHeader);
    BaseAddress = (UINT64) Value << 32;
    GnbLibPciRead (GnbIommuPciAddress.AddressValue | (CapabilityOffset + 0x4), AccessWidth32, &Value, StdHeader);
    BaseAddress |= Value;

    if ((BaseAddress & 0xfffffffffffffffe) != 0x0) {
      IDS_HDT_CONSOLE (GNB_TRACE, "  Enable IOMMU MMIO at address %x for Socket %d Silicon %d\n", BaseAddress, GnbHandle->SocketId, GnbHandle->DieNumber);
      GnbLibPciRMW (GnbIommuPciAddress.AddressValue | (CapabilityOffset + 0x8), AccessS3SaveWidth32, 0xFFFFFFFF, 0x0, StdHeader);
      GnbLibPciRMW (GnbIommuPciAddress.AddressValue | (CapabilityOffset + 0x4), AccessS3SaveWidth32, 0xFFFFFFFE, 0x1, StdHeader);
    } else {
//      ASSERT (FALSE);
      Status = AGESA_ERROR;
      IDS_HDT_CONSOLE (GNB_TRACE, "No base address assigned - IOMMU disabled\n");
    }
  }

  IDS_HDT_CONSOLE (GNB_TRACE, "GnbEnableIommuMmio Exit\n");
  return Status;
}

/**
 *---------------------------------------------------------------------------------------
 *  InstallIvrsAcpiTable
 *
 *  Description:
 *     notification event handler for install Ivrs Acpi Table
 *  Parameters:
 *    @param[in]     Event      Event whose notification function is being invoked.
 *    @param[in]     *Context   Pointer to the notification function's context.
 *
 *---------------------------------------------------------------------------------------
 **/
VOID
EFIAPI
InstallIvrsAcpiTable (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  )
{
  EFI_ACPI_TABLE_PROTOCOL       *AcpiTable;
  EFI_STATUS                    Status;
  AMD_SOC_LOGICAL_ID_PROTOCOL   *SocLogicalId;
  SOC_LOGICAL_ID                LogicalId;
  UINT64                        EfrAndMask;
  UINT64                        EfrOrMask;

  IDS_HDT_CONSOLE (MAIN_FLOW, "InstallIvrsAcpiTable Start\n");

  Status = gBS->LocateProtocol (&gEfiAcpiTableProtocolGuid, NULL, (VOID **)&AcpiTable);
  if (EFI_ERROR (Status)) {
    return;
  }
  //
  // Create ACPI IVRS SSDT table
  //

//  Status = GnbIommuScratchMemoryRangeInterface ();


  EfrAndMask = 0xFFFFFFFFFFFFFFFF;
  EfrOrMask = 0;

  Status = gBS->LocateProtocol (&gAmdSocLogicalIdProtocolGuid, NULL, (VOID **) &SocLogicalId);
  ASSERT (!EFI_ERROR (Status));
  SocLogicalId->GetLogicalIdOnCurrentCore (SocLogicalId, &LogicalId);
  if ((LogicalId.Revision & AMD_REV_F17_RV_A0) != 0) {
    EfrOrMask |= (1ull << 62);
  }

  GnbIommuIvrsTable (EfrAndMask, EfrOrMask);
  //
  // Close event, so it will not be invoked again.
  //
  gBS->CloseEvent (Event);
  IDS_HDT_CONSOLE (MAIN_FLOW, "InstallIvrsAcpiTable End\n");
}

EFI_STATUS
EFIAPI
AmdNbioIOMMURVDxeEntry (
  IN       EFI_HANDLE         ImageHandle,
  IN       EFI_SYSTEM_TABLE   *SystemTable
  )
{
  DXE_AMD_NBIO_PCIE_SERVICES_PROTOCOL *PcieServicesProtocol;
  PCIe_PLATFORM_CONFIG                *Pcie;
  EFI_STATUS                          Status;
  EFI_STATUS                          AgesaStatus;
  GNB_HANDLE                          *GnbHandle;
  AMD_CONFIG_PARAMS                   *StdHeader;
  GNB_PCIE_INFORMATION_DATA_HOB       *PciePlatformConfigHobData;
  VOID                                *Registration;
  BOOLEAN                             IsIommuSupport;
  AMD_SOC_LOGICAL_ID_PROTOCOL         *SocLogicalId;
  SOC_LOGICAL_ID                      LogicalId;
  UINT32                               PackageType;

  AGESA_TESTPOINT (TpNbioIommuDxeEntry, NULL);
  IDS_HDT_CONSOLE (MAIN_FLOW, "AmdNbioIOMMURVDxeEntry\n");

  AgesaStatus = EFI_SUCCESS;
  GnbHandle = NULL;
  StdHeader = NULL;  ///@todo - do we need this?
  IsIommuSupport = FALSE;

  PackageType = LibAmdGetPackageType ((AMD_CONFIG_PARAMS *) NULL);
  Status = gBS->LocateProtocol (&gAmdSocLogicalIdProtocolGuid, NULL, (VOID **) &SocLogicalId);
  ASSERT (!EFI_ERROR (Status));
  if (EFI_ERROR (Status)) {
    SocLogicalId = (AMD_SOC_LOGICAL_ID_PROTOCOL *)NULL;
  }

  // Need topology structure
  Status = gBS->LocateProtocol (
                  &gAmdNbioPcieServicesProtocolGuid,
                  NULL,
                  (VOID**)&PcieServicesProtocol
                  );
  AGESA_STATUS_UPDATE (Status, AgesaStatus);
//  ASSERT (Status == EFI_SUCCESS);
  if (Status == EFI_SUCCESS) {
    PcieServicesProtocol->PcieGetTopology (PcieServicesProtocol, (UINT32 **) &PciePlatformConfigHobData);
    Pcie = &(PciePlatformConfigHobData->PciePlatformConfigHob);
    GnbHandle = NbioGetHandle (Pcie);
    while (GnbHandle != NULL) {
      // Enable IOMMU BUS_MASTER bit for PPR requirement
      NbioRegisterRMW (GnbHandle, TYPE_SMN, PCIE_IOMMU_COMMAND_Address, (UINT32) ~(MEM_ACCESS_EN_Mask), (1 << BUS_MASTER_EN_OFFSET), GNB_REG_ACC_FLAG_S3SAVE);
      if ((1 << RV_SOCKET_AM4) == PackageType) {
        if (SocLogicalId != (AMD_SOC_LOGICAL_ID_PROTOCOL *)NULL) {
          SocLogicalId->GetLogicalIdOnCurrentCore(SocLogicalId, &LogicalId);
          if ((LogicalId.Revision & AMD_REV_F17_RV_A0) != 0) {
            IommuMsixWorkaround (GnbHandle);
          }
        }
      }
      Status = EnableIommuMmio (GnbHandle, StdHeader);
      if (Status == EFI_SUCCESS) {
        IsIommuSupport = TRUE;
      }
      GnbHandle = GnbGetNextHandle (GnbHandle);
    }
    IDS_HOOK (IDS_HOOK_NBIO_AFTER_CCX, NULL, (void *)NbioGetHandle (Pcie));
  }
  if (PcdGetBool (PcdCfgIommuSupport) && PcdGetBool (PcdIvrsControl) && IsIommuSupport) {
    EfiCreateProtocolNotifyEvent (&gEfiAcpiTableProtocolGuid, TPL_CALLBACK, InstallIvrsAcpiTable, NULL, &Registration);
  }

  IDS_HDT_CONSOLE (MAIN_FLOW, "AmdNbioIOMMURVDxeExit\n");
  AGESA_TESTPOINT (TpNbioIommuDxeExit, NULL);
  return AgesaStatus;
}

