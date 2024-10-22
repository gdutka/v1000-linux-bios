/*****************************************************************************
 *
 * Copyright (C) 2012-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 ******************************************************************************
 */

#include <AmdCpmPei.h>


EFI_STATUS
EFIAPI
AmdCpmDisplayFeaturePeim (
  IN       EFI_PEI_SERVICES             **PeiServices,
  IN       EFI_PEI_NOTIFY_DESCRIPTOR    *NotifyDescriptor,
  IN       VOID                         *Ppi
  );

STATIC EFI_PEI_NOTIFY_DESCRIPTOR mCpmDisplayFeaturePeimNotify = {
  (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gAmdCpmGpioInitFinishedPpiGuid,
  AmdCpmDisplayFeaturePeim
};

/*----------------------------------------------------------------------------------------*/
/**
 *
 * The function to set HotPlug attribute in PCIe Topology Table if Express Card
 * supports after AMD CPM GPIO INIT FINISHED PPI is installed.
 *
 * @param[in]     PeiServices       Pointer to Pei Services
 * @param[in]     NotifyDescriptor  The descriptor for the notification event
 * @param[in]     Ppi               Pointer to the PPI in question.
 *
 * @retval        EFI_SUCCESS       Module initialized successfully
 * @retval        EFI_ERROR         Initialization failed (see error for more details)
 *
 */

EFI_STATUS
EFIAPI
AmdCpmDisplayFeaturePeim (
  IN       EFI_PEI_SERVICES             **PeiServices,
  IN       EFI_PEI_NOTIFY_DESCRIPTOR    *NotifyDescriptor,
  IN       VOID                         *Ppi
  )
{
  EFI_STATUS                        Status;
  AMD_CPM_TABLE_PPI                 *CpmTablePpiPtr;
  AMD_CPM_MAIN_TABLE                *MainTablePtr;
  AMD_CPM_PCIE_TOPOLOGY_TABLE       *PcieTopologyTablePtr;
  AMD_CPM_DXIO_TOPOLOGY_TABLE       *DxioTopologyTablePtr;
  AMD_CPM_DEVICE_PATH_TABLE         *DevicePathTablePtr;
  AMD_CPM_SAVE_CONTEXT_TABLE        *SaveContextTablePtr;
  UINT8                             i;
  UINT8                             j;
  UINT32                            PowerXpressFixedMode;
  UINT32                            PowerXpressDynamicMode;
  UINT32                            D3ColdSupport;
  UINT32                            HybridGraphicsSupport;
  UINT32                            CurrentPowerXpress;
  UINT32                            CurrentHybridGraphics;
  UINT32                            CurrentD3Cold;

  RECORD_TIME_PEI (BeginAmdCpmDisplayFeaturePeimDriver);

  Status = (*PeiServices)->LocatePpi (
                               (CPM_PEI_SERVICES**)PeiServices,
                               &gAmdCpmTablePpiGuid,
                               0,
                               NULL,
                               (VOID**)&CpmTablePpiPtr
                               );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  CpmTablePpiPtr->CommonFunction.PostCode (CpmTpDisplayFeaturePeimDriverBegin);

  // Override PCIE Port LinkHotplug
  MainTablePtr            = CpmTablePpiPtr->MainTablePtr;
  PowerXpressFixedMode    = MainTablePtr->DisplayFeature.Config.PowerXpressFixedMode;
  PowerXpressDynamicMode  = MainTablePtr->DisplayFeature.Config.PowerXpressDynamicMode;
  D3ColdSupport           = MainTablePtr->DisplayFeature.Config.D3ColdSupport;
  HybridGraphicsSupport   = MainTablePtr->DisplayFeature.Config.HybridGraphicsSupport;
  PcieTopologyTablePtr    = CpmTablePpiPtr->CommonFunction.GetTablePtr2 (CpmTablePpiPtr, CPM_SIGNATURE_PCIE_TOPOLOGY);
  DxioTopologyTablePtr    = CpmTablePpiPtr->CommonFunction.GetTablePtr2 (CpmTablePpiPtr, CPM_SIGNATURE_DXIO_TOPOLOGY);
  DevicePathTablePtr      = CpmTablePpiPtr->CommonFunction.GetTablePtr (CpmTablePpiPtr, CPM_SIGNATURE_DEVICE_PATH);
  SaveContextTablePtr     = CpmTablePpiPtr->CommonFunction.GetTablePtr (CpmTablePpiPtr, CPM_SIGNATURE_SAVE_CONTEXT);
  CurrentPowerXpress      = 0;
  CurrentHybridGraphics   = 0;
  CurrentD3Cold           = 0;

  if (MainTablePtr && PcieTopologyTablePtr && DevicePathTablePtr) {
    if (PowerXpressFixedMode || PowerXpressDynamicMode || HybridGraphicsSupport || D3ColdSupport) {
      for (i = 0; i < AMD_DISPLAY_DEVICE_PATH_SIZE; i++) {
        if (! DevicePathTablePtr->Path[i].FeatureMask.Mask.Valid) {
          break;
        }
        if (DevicePathTablePtr->Path[i].IsDgpu) {
          CurrentPowerXpress    = DevicePathTablePtr->Path[i].FeatureMask.Mask.PowerXpress;
          CurrentHybridGraphics = DevicePathTablePtr->Path[i].FeatureMask.Mask.HybridGraphics;
          CurrentD3Cold         = DevicePathTablePtr->Path[i].FeatureMask.Mask.D3Cold;
          if ((CurrentPowerXpress && (PowerXpressFixedMode || PowerXpressDynamicMode)) ||
              (CurrentHybridGraphics && HybridGraphicsSupport) ||
              (CurrentD3Cold && D3ColdSupport)) {
            for (j = 0; j < AMD_PCIE_PORT_DESCRIPTOR_SIZE; j++) {
              if (PcieTopologyTablePtr->Port[j].EngineData.EngineType == PciePortEngine && PcieTopologyTablePtr->Port[j].Port.PortPresent) {
                if ((PcieTopologyTablePtr->Port[j].Port.DeviceNumber == DevicePathTablePtr->Path[i].Bridge.Device) &&
                    (PcieTopologyTablePtr->Port[j].Port.FunctionNumber == DevicePathTablePtr->Path[i].Bridge.Function)) {
                  if (MainTablePtr->DisplayFeature.Config.DgpuHotPlugMode == 1) {
                    PcieTopologyTablePtr->Port[j].Port.LinkHotplug = HotplugEnhanced;
                  } else {
                    PcieTopologyTablePtr->Port[j].Port.LinkHotplug = HotplugBasic;
                  }
                  if (SaveContextTablePtr) {
                    if ((CpmTablePpiPtr->CommonFunction.GetBootMode () == CPM_BOOT_MODE_S3) || (CpmTablePpiPtr->CommonFunction.GetBootMode () == CPM_BOOT_MODE_S4)) {
                      if (CpmTablePpiPtr->CommonFunction.GetSaveContext (CpmTablePpiPtr, 0x80 + OFFSET_OF (AMD_CPM_PREDEFINED_SAVE_CONTEXT, dGpuStateOnResume)) & BIT0) {
                        PcieTopologyTablePtr->Port[j].Port.PortPresent = PortDisabled;
                      }
                    }
                  }
                  break;
                }
              }
              if (PcieTopologyTablePtr->Port[j].Flags == DESCRIPTOR_TERMINATE_LIST) {
                break;
              }
            }
          }
        }
      }
    }
  }

  if (MainTablePtr && DxioTopologyTablePtr && DevicePathTablePtr) {
    if (PowerXpressFixedMode || PowerXpressDynamicMode || HybridGraphicsSupport || D3ColdSupport) {
      for (i = 0; i < AMD_DISPLAY_DEVICE_PATH_SIZE; i++) {
        if (! DevicePathTablePtr->Path[i].FeatureMask.Mask.Valid) {
          break;
        }
        if (DevicePathTablePtr->Path[i].IsDgpu) {
          CurrentPowerXpress    = DevicePathTablePtr->Path[i].FeatureMask.Mask.PowerXpress;
          CurrentHybridGraphics = DevicePathTablePtr->Path[i].FeatureMask.Mask.HybridGraphics;
          CurrentD3Cold         = DevicePathTablePtr->Path[i].FeatureMask.Mask.D3Cold;
          if ((CurrentPowerXpress && (PowerXpressFixedMode || PowerXpressDynamicMode)) ||
              (CurrentHybridGraphics && HybridGraphicsSupport) ||
              (CurrentD3Cold && D3ColdSupport)) {
            for (j = 0; j < AMD_PCIE_PORT_DESCRIPTOR_SIZE; j++) {
              if (DxioTopologyTablePtr->Port[j].EngineData.EngineType == DxioPcieEngine && DxioTopologyTablePtr->Port[j].Port.PortPresent) {
                if ((DxioTopologyTablePtr->Port[j].Port.DeviceNumber == DevicePathTablePtr->Path[i].Bridge.Device) &&
                    (DxioTopologyTablePtr->Port[j].Port.FunctionNumber == DevicePathTablePtr->Path[i].Bridge.Function)) {
                  DxioTopologyTablePtr->Port[j].EngineData.HotPluggable = HotPluggableDisabled; //HotPluggableEnabled;
                  if (MainTablePtr->DisplayFeature.Config.DgpuHotPlugMode == 1) {
                    DxioTopologyTablePtr->Port[j].Port.LinkHotplug = DxioHotplugEnhanced;
                  } else {
                    DxioTopologyTablePtr->Port[j].Port.LinkHotplug = DxioHotplugBasic;
                  }
                  if (SaveContextTablePtr) {
                    if ((CpmTablePpiPtr->CommonFunction.GetBootMode () == CPM_BOOT_MODE_S3) || (CpmTablePpiPtr->CommonFunction.GetBootMode () == CPM_BOOT_MODE_S4)) {
                      if (CpmTablePpiPtr->CommonFunction.GetSaveContext (CpmTablePpiPtr, 0x80 + OFFSET_OF (AMD_CPM_PREDEFINED_SAVE_CONTEXT, dGpuStateOnResume)) & BIT0) {
                        DxioTopologyTablePtr->Port[j].Port.PortPresent = DxioPortDisabled;
                      }
                    }
                  }
                  break;
                }
              }
              if (DxioTopologyTablePtr->Port[j].Flags == DESCRIPTOR_TERMINATE_LIST) {
                break;
              }
            }
          }
        }
      }
    }
  }

  CpmTablePpiPtr->CommonFunction.PostCode (CpmTpDisplayFeaturePeimDriverEnd);

  RECORD_TIME_PEI (EndAmdCpmDisplayFeaturePeimDriver);

  return EFI_SUCCESS;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Entry point of the AMD CPM Display Feature PEIM driver
 *
 * This function registers the functions to set HotPlug attribute in PCIe Topology Table
 * if Express Card supports.
 *
 * @param[in]     FileHandle     Pointer to the firmware file system header
 * @param[in]     PeiServices    Pointer to Pei Services
 *
 * @retval        EFI_SUCCESS    Module initialized successfully
 * @retval        EFI_ERROR      Initialization failed (see error for more details)
 */

EFI_STATUS
EFIAPI
AmdCpmDisplayFeaturePeimEntryPoint (
  IN        CPM_PEI_FILE_HANDLE         FileHandle,
  IN        CPM_PEI_SERVICES            **PeiServices
  )
{
  EFI_STATUS                      Status;

  Status = (**PeiServices).NotifyPpi (PeiServices, &mCpmDisplayFeaturePeimNotify);

  return EFI_SUCCESS;
}
